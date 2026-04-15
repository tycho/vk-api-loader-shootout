#!/usr/bin/env python3
"""Collect host/toolchain metadata into reports/<host>/metadata.json.

Invoked by the Makefile with all the values it already has on hand (versions,
compiler names, flags) passed as explicit flags, so we don't duplicate the
`git describe` / `$(shell ...)` logic already living in the Makefile. This
script's job is to serialize those values into a stable JSON shape, plus
capture whatever `vulkaninfo --summary` has to say about the local GPUs.

Stdlib only. Run standalone with --help for flag details.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from pathlib import Path


def read_vulkaninfo(path: str) -> str:
    """Run `vulkaninfo --summary` and return the `Devices:` section.

    Prefers the explicit path (typically bin/vulkaninfo[.exe] which we built
    ourselves with a known loader on the DLL search path). Falls back to PATH
    lookup if the explicit path is empty or missing. Returns an empty string
    on any failure - the caller decides what to do with missing data.
    """
    # Resolve explicit paths to absolute form before launching. On some
    # Windows Python builds (notably msys2/clang64) relative paths with
    # forward slashes get rejected by CreateProcess even when they exist;
    # abspath forces native separators and a drive-qualified path.
    if path:
        cmd = [os.path.abspath(path), "--summary"]
    else:
        cmd = ["vulkaninfo", "--summary"]
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
    except (FileNotFoundError, subprocess.TimeoutExpired) as exc:
        print(f"warning: vulkaninfo failed ({exc}); continuing without it",
              file=sys.stderr)
        return ""
    if proc.returncode != 0:
        print(f"warning: vulkaninfo exited {proc.returncode}; continuing",
              file=sys.stderr)
        return ""
    # Trim to just the Devices: section (the summary mode prints a bit above
    # that we don't care about - loader version lines, instance extensions,
    # etc). If we don't see a Devices: marker, fall back to the full output.
    match = re.search(r"^Devices:\s*$", proc.stdout, re.M)
    text = proc.stdout[match.start():] if match else proc.stdout
    return text.rstrip()


def parse_kv_list(items: list[str]) -> dict[str, str]:
    out: dict[str, str] = {}
    for item in items:
        if "=" not in item:
            print(f"warning: ignoring malformed key=value: {item!r}", file=sys.stderr)
            continue
        key, value = item.split("=", 1)
        out[key.strip()] = value.strip()
    return out


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--host-id", required=True,
                   help="Short host identifier (e.g. linux, mingw, macos).")
    p.add_argument("--output", required=True, type=Path,
                   help="Output JSON path (typically reports/<host>/metadata.json).")

    p.add_argument("--os", required=True, help="uname -s value.")
    p.add_argument("--arch", required=True, help="uname -m value.")
    p.add_argument("--kernel-version", default="", help="uname -r value.")
    p.add_argument("--cpu-brand", default="", help="Human-readable CPU name.")

    p.add_argument("--cc", default="")
    p.add_argument("--cc-version", default="")
    p.add_argument("--cxx", default="")
    p.add_argument("--cxx-version", default="")
    p.add_argument("--optflags", default="")
    p.add_argument("--cflags", default="")
    p.add_argument("--cxxflags", default="")

    p.add_argument("--version", action="append", default=[], metavar="NAME=VALUE",
                   help="Third-party project version (repeatable). "
                        "Example: --version volk=1.4.341.0-21-g78463da")

    p.add_argument("--vulkaninfo", default="",
                   help="Path to vulkaninfo binary (optional; falls back to $PATH).")

    args = p.parse_args()

    metadata = {
        "host_id": args.host_id,
        "host": {
            "os": args.os,
            "arch": args.arch,
            "kernel_version": args.kernel_version,
            "cpu": args.cpu_brand,
        },
        "toolchain": {
            "cc": args.cc,
            "cc_version": args.cc_version,
            "cxx": args.cxx,
            "cxx_version": args.cxx_version,
            "optflags": args.optflags,
            "cflags": args.cflags,
            "cxxflags": args.cxxflags,
        },
        "versions": parse_kv_list(args.version),
        "vulkaninfo_summary": read_vulkaninfo(args.vulkaninfo),
    }

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {args.output}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
