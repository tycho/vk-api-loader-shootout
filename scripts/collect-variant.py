#!/usr/bin/env python3
"""Run the test binaries and collect per-variant benchmark + size data.

Writes reports/<host>/<variant>.json where <variant> is `patched` or
`unpatched` (referring to the Vulkan loader used at measurement time). The
Makefile is responsible for getting the correct loader linked into bin/
before invoking this script - see the `link-vulkan` / `unlink-vulkan` targets.

For each loader passed via --loader, this script:
  1. Runs `bin/test-<loader>[.exe] --json` and parses the benchmark payload.
  2. Records the binary and .o sizes as reported by os.stat.
  3. Runs `size` on the binary and parses the text/data/bss columns,
     tolerating both GNU size (ELF, COFF) and Apple size (Mach-O) formats.

Stdlib only. Run standalone with --help for details.
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path


EXE_SUFFIX = ".exe" if os.name == "nt" or sys.platform.startswith("win") else ""


def _find_binary(bin_dir: Path, loader: str) -> Path | None:
    """Locate bin/test-<loader> with or without a .exe suffix."""
    for suffix in (EXE_SUFFIX, "", ".exe"):
        candidate = bin_dir / f"test-{loader}{suffix}"
        if candidate.is_file():
            return candidate
    return None


def run_benchmark(binary: Path) -> dict:
    """Run a test binary in --json mode and return the parsed payload.

    The binary prints a single JSON object to stdout when invoked with
    --json. If anything goes wrong we raise with context so the caller can
    decide whether to abort the whole variant or mark this loader as missing.

    We resolve to an absolute path before launching because some Windows
    Python builds (notably msys2/clang64 Python 3.14) construct WindowsPath
    objects with forward-slash separators, which CreateProcess refuses to
    launch for relative paths. abspath() forces native separators and an
    absolute drive-qualified path, which CreateProcess always accepts.
    """
    proc = subprocess.run(
        [os.path.abspath(str(binary)), "--json"],
        capture_output=True,
        text=True,
        timeout=600,
    )
    if proc.returncode != 0:
        raise RuntimeError(
            f"{binary} exited {proc.returncode}\nstderr:\n{proc.stderr}"
        )
    try:
        return json.loads(proc.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(
            f"{binary} --json emitted non-JSON output:\n{proc.stdout}\n"
            f"decode error: {exc}"
        ) from exc


def parse_size_output(text: str) -> dict[str, int]:
    """Parse `size` output (GNU ELF/COFF or Apple Mach-O variants).

    GNU form looks like:
        text    data     bss     dec     hex filename
       72438   39600       0  112038   1b5a6 bin/test-glad-dav1dde.exe

    Apple form looks like:
        __TEXT  __DATA  __OBJC  others      dec         hex
        114688  16384   0       4295016448  4295147520  10002c000  bin/test-foo

    Returns a {text, data, bss} dict. Mach-O has no notion of BSS as a
    separate segment (zero-init goes into __DATA), so we report it as 0.
    The Mach-O `others` column is almost entirely the 4 GB base address and
    is not useful in reports, so we discard it.
    """
    lines = [ln for ln in text.strip().splitlines() if ln.strip()]
    if len(lines) < 2:
        return {"text": 0, "data": 0, "bss": 0}

    header = lines[0].split()
    data_row = lines[1].split()

    if header and header[0].startswith("__"):
        # Apple size. Columns: __TEXT __DATA __OBJC others dec hex filename
        try:
            return {
                "text": int(data_row[0]),
                "data": int(data_row[1]),
                "bss": 0,
            }
        except (ValueError, IndexError):
            return {"text": 0, "data": 0, "bss": 0}

    # GNU size. Columns: text data bss dec hex filename
    try:
        return {
            "text": int(data_row[0]),
            "data": int(data_row[1]),
            "bss": int(data_row[2]),
        }
    except (ValueError, IndexError):
        return {"text": 0, "data": 0, "bss": 0}


def run_size(binary: Path) -> dict[str, int]:
    try:
        proc = subprocess.run(
            ["size", str(binary)],
            capture_output=True,
            text=True,
            timeout=30,
        )
    except FileNotFoundError:
        print("warning: `size` not found on PATH; section sizes will be zero",
              file=sys.stderr)
        return {"text": 0, "data": 0, "bss": 0}
    if proc.returncode != 0:
        print(f"warning: `size {binary}` exited {proc.returncode}", file=sys.stderr)
        return {"text": 0, "data": 0, "bss": 0}
    return parse_size_output(proc.stdout)


def collect_loader(loader: str, bin_dir: Path, obj_dir: Path) -> dict | None:
    """Gather all per-loader data. Returns None if the binary is missing."""
    binary = _find_binary(bin_dir, loader)
    if binary is None:
        print(f"warning: bin/test-{loader} not found; skipping", file=sys.stderr)
        return None

    payload = run_benchmark(binary)
    benchmarks = payload.get("benchmarks", {})

    obj_path = obj_dir / f"loader-{loader}.o"
    loader_object_size = obj_path.stat().st_size if obj_path.is_file() else 0

    return {
        "binary": binary.name,
        "binary_size": binary.stat().st_size,
        "loader_object_size": loader_object_size,
        "sections": run_size(binary),
        "benchmarks": benchmarks,
    }


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--variant", required=True, choices=("patched", "unpatched"),
                   help="Which Vulkan loader build this run targets.")
    p.add_argument("--bin-dir", required=True, type=Path,
                   help="Directory holding the test-<loader>[.exe] binaries.")
    p.add_argument("--obj-dir", required=True, type=Path,
                   help="Directory holding the loader-<loader>.o objects.")
    p.add_argument("--loader", action="append", required=True, default=[],
                   help="Loader name without the test- prefix. Repeatable. "
                        "Example: --loader volk --loader glad-tycho")
    p.add_argument("--output", required=True, type=Path,
                   help="Output JSON path (typically reports/<host>/<variant>.json).")
    args = p.parse_args()

    loaders: dict[str, dict] = {}
    for loader in args.loader:
        print(f"  collecting {loader}...", file=sys.stderr)
        record = collect_loader(loader, args.bin_dir, args.obj_dir)
        if record is not None:
            loaders[loader] = record

    if not loaders:
        print("error: no loader data collected", file=sys.stderr)
        return 1

    out = {
        "variant": args.variant,
        "loaders": loaders,
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(out, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {args.output}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
