#!/usr/bin/env python3
"""Render report fragments and amalgamated per-platform documents.

Reads reports/<host>/{metadata,patched,unpatched}.json and produces:

    reports/<host>/at-a-glance.md      # summary table
    reports/<host>/task/<task>.md      # one detail table per task
    reports/<host>/sizes.md            # binary-size table
    reports/<host>/hardware.md         # collapsible host details block
    reports/<host>/charts/<task>.svg   # log-scale bar charts (function-loading tasks only)
    reports/<host>.md                  # self-contained per-platform document

The amalgamated <host>.md embeds the fragments via plain concatenation (GitHub
Markdown does not support include directives, so this is a build-time splice,
not a render-time one). README.md stays hand-authored and links out to these
per-platform files.

All output is deterministic - given the same input JSON, byte-identical output.
Stdlib only; hand-rolled SVG. Run standalone with --help for details.
"""
from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path
from typing import Iterable


# ---------------------------------------------------------------------------
# Canonical ordering and display names. Anything the renderer knows how to
# show goes in these tables; unknown loaders/tasks fall back to their raw id.

LOADER_ORDER = [
    "glad-dav1dde",
    "glad-tycho",
    "gloam-discover",
    "gloam-enabled",
    "volk",
]

LOADER_DISPLAY = {
    "glad-dav1dde":   "GLAD (dav1dde)",
    "glad-tycho":     "GLAD (tycho)",
    "gloam-discover": "Gloam (discover)",
    "gloam-enabled":  "Gloam (enabled-list)",
    "volk":           "Volk",
}

LOADER_LINK = {
    "glad-dav1dde":   "https://github.com/Dav1dde/glad",
    "glad-tycho":     "https://github.com/tycho/glad",
    "gloam-discover": "https://github.com/tycho/gloam",
    "gloam-enabled":  "https://github.com/tycho/gloam",
    "volk":           "https://github.com/zeux/volk",
}

TASK_ORDER = [
    "load_instance",
    "load_device",
    "init_load_all",
    "full_ctx_persistent",
    "full_ctx_transient",
]

TASK_DISPLAY = {
    "load_instance":       "Load instance functions",
    "load_device":         "Load device functions",
    "init_load_all":       "Init + load all functions",
    "full_ctx_persistent": "Full VK context (libvulkan persistent)",
    "full_ctx_transient":  "Full VK context (libvulkan transient)",
}

# Which tasks get SVG bar charts. The full-context rows are dominated by
# driver overhead (vkCreateInstance, vkCreateDevice), so the per-loader shape
# is usually flat and a chart adds no signal - skip them.
CHARTED_TASKS = ["load_instance", "load_device", "init_load_all"]

# Display name for the host subdir. Falls back to the raw id for unknown
# hosts. The raw id is lowercase ASCII for filesystem safety; this map exists
# purely so the rendered docs say "Linux" / "macOS" / "MinGW" instead of
# "linux" / "macos" / "mingw" in their titles.
HOST_DISPLAY = {
    "linux": "Linux",
    "mingw": "MinGW",
    "macos": "macOS",
}

# Version keys we expect in metadata.json -> versions. Rows appear in this
# order; any unknown keys are appended at the end, alphabetically.
VERSION_ORDER = [
    ("glad_dav1dde",   "GLAD (dav1dde)"),
    ("glad_tycho",     "GLAD (tycho)"),
    ("gloam",          "gloam"),
    ("volk",           "Volk"),
    ("xxhash",         "xxHash"),
    ("vulkan_headers", "Vulkan-Headers"),
]


# ---------------------------------------------------------------------------
# Formatting helpers

def fmt_us(value: float) -> str:
    """Format a microsecond value for a table cell.

    Uses up to 2 decimal places, matching the existing README style.
    """
    return f"{value:.2f}\u00b5s"


def fmt_speedup(ratio: float) -> str:
    """Format a speedup ratio like '1.9x' or '26.1x'."""
    if not math.isfinite(ratio):
        return "-"
    if ratio >= 100:
        return f"{ratio:.0f}\u00d7"
    if ratio >= 10:
        return f"{ratio:.1f}\u00d7"
    return f"{ratio:.1f}\u00d7"


def display_loader(loader_id: str) -> str:
    return LOADER_DISPLAY.get(loader_id, loader_id)


def display_task(task_id: str) -> str:
    return TASK_DISPLAY.get(task_id, task_id)


def display_host(host_id: str) -> str:
    return HOST_DISPLAY.get(host_id, host_id)


def ordered_loaders(loaders: Iterable[str]) -> list[str]:
    """Return loader ids in canonical order, unknown ones appended."""
    present = set(loaders)
    result = [lid for lid in LOADER_ORDER if lid in present]
    result.extend(sorted(present - set(LOADER_ORDER)))
    return result


def ordered_tasks(tasks: Iterable[str]) -> list[str]:
    present = set(tasks)
    result = [tid for tid in TASK_ORDER if tid in present]
    result.extend(sorted(present - set(TASK_ORDER)))
    return result


# ---------------------------------------------------------------------------
# JSON loading

class HostReport:
    """Bundles the three JSON files for one host into a convenient shape."""

    def __init__(self, metadata: dict, patched: dict, unpatched: dict):
        self.metadata = metadata
        self.patched = patched
        self.unpatched = unpatched

    @classmethod
    def load(cls, host_dir: Path) -> "HostReport":
        meta = json.loads((host_dir / "metadata.json").read_text(encoding="utf-8"))
        patched = json.loads((host_dir / "patched.json").read_text(encoding="utf-8"))
        unpatched = json.loads((host_dir / "unpatched.json").read_text(encoding="utf-8"))
        return cls(meta, patched, unpatched)

    @property
    def host_id(self) -> str:
        return self.metadata.get("host_id", "unknown")

    def loaders(self) -> list[str]:
        """Union of loaders present in either variant, in canonical order."""
        ids = set(self.patched.get("loaders", {}).keys()) | set(
            self.unpatched.get("loaders", {}).keys()
        )
        return ordered_loaders(ids)

    def tasks(self) -> list[str]:
        """Union of task ids seen across all loaders in both variants."""
        ids: set[str] = set()
        for variant in (self.patched, self.unpatched):
            for loader_data in variant.get("loaders", {}).values():
                ids.update(loader_data.get("benchmarks", {}).keys())
        return ordered_tasks(ids)

    def get_avg(self, variant: str, loader: str, task: str) -> float | None:
        src = self.patched if variant == "patched" else self.unpatched
        try:
            return src["loaders"][loader]["benchmarks"][task]["avg_us"]
        except KeyError:
            return None


# ---------------------------------------------------------------------------
# Markdown table rendering

def _table(rows: list[list[str]], align: list[str]) -> str:
    """Render a Markdown table with per-column alignment specs.

    align is a list of 'l'/'r'/'c' characters, one per column. rows[0] is the
    header; subsequent rows are the data.
    """
    assert rows, "empty table"
    assert all(len(r) == len(align) for r in rows), "ragged rows"

    # Compute column widths for readable source (GitHub ignores this, but it
    # makes diffs sane and the raw .md files pleasant to look at).
    widths = [max(len(r[c]) for r in rows) for c in range(len(align))]

    def fmt_row(r: list[str]) -> str:
        cells = []
        for c, cell in enumerate(r):
            if align[c] == "r":
                cells.append(cell.rjust(widths[c]))
            elif align[c] == "c":
                cells.append(cell.center(widths[c]))
            else:
                cells.append(cell.ljust(widths[c]))
        return "| " + " | ".join(cells) + " |"

    def sep_row() -> str:
        parts = []
        for c, a in enumerate(align):
            w = widths[c]
            if a == "r":
                parts.append("-" * (w + 1) + ":")
            elif a == "c":
                parts.append(":" + "-" * w + ":")
            else:
                parts.append("-" * (w + 2))
        return "|" + "|".join(parts) + "|"

    out = [fmt_row(rows[0]), sep_row()]
    out.extend(fmt_row(r) for r in rows[1:])
    return "\n".join(out) + "\n"


# ---------------------------------------------------------------------------
# Fragment: at-a-glance

def render_at_a_glance(report: HostReport) -> str:
    """One row per task. Winner = lowest patched avg (ties: canonical order)."""
    loaders = report.loaders()
    lines = [
        f"## At a glance\n",
        f"Lower is better. **Winner** = best patched average on this host "
        f"for each task.\n",
    ]

    header = ["Task", "Winner", "Patched avg", "Unpatched avg", "Patch speedup"]
    align = ["l", "l", "r", "r", "r"]
    rows = [header]

    for task in report.tasks():
        # Find winner = loader with lowest patched avg
        best_loader = None
        best_patched = math.inf
        for lid in loaders:
            avg = report.get_avg("patched", lid, task)
            if avg is not None and avg < best_patched:
                best_patched = avg
                best_loader = lid

        if best_loader is None:
            continue

        unpatched = report.get_avg("unpatched", best_loader, task)
        if unpatched is not None and best_patched > 0:
            speedup = fmt_speedup(unpatched / best_patched)
        else:
            speedup = "-"

        rows.append([
            display_task(task),
            f"**{display_loader(best_loader)}**",
            fmt_us(best_patched),
            fmt_us(unpatched) if unpatched is not None else "-",
            speedup,
        ])

    lines.append(_table(rows, align))
    return "\n".join(lines) + "\n"


# ---------------------------------------------------------------------------
# Fragment: per-task detail

def render_task_detail(report: HostReport, task: str) -> str:
    """One table: rows = loaders (sorted by patched avg asc), bold the winner."""
    loaders = report.loaders()

    # Build (loader, unpatched, patched) tuples, drop loaders missing both
    entries = []
    for lid in loaders:
        p = report.get_avg("patched", lid, task)
        u = report.get_avg("unpatched", lid, task)
        if p is None and u is None:
            continue
        entries.append((lid, u, p))

    if not entries:
        return f"### {display_task(task)}\n\n_No data for this task._\n"

    # Sort by patched avg ascending; loaders without a patched number sink to
    # the bottom in canonical order.
    def sort_key(entry):
        _, _, p = entry
        return (math.inf if p is None else p)

    entries.sort(key=sort_key)
    fastest_patched = entries[0][2]

    header = ["Loader", "Unpatched", "Patched", "Patch speedup", "vs. fastest"]
    align = ["l", "r", "r", "r", "r"]
    rows = [header]

    for i, (lid, u, p) in enumerate(entries):
        name = display_loader(lid)
        if i == 0:
            name = f"**{name}**"

        u_str = fmt_us(u) if u is not None else "-"
        p_str = fmt_us(p) if p is not None else "-"

        if u is not None and p is not None and p > 0:
            patch_speedup = fmt_speedup(u / p)
        else:
            patch_speedup = "-"

        if p is not None and fastest_patched is not None and fastest_patched > 0:
            vs_fastest = fmt_speedup(p / fastest_patched)
        else:
            vs_fastest = "-"

        rows.append([name, u_str, p_str, patch_speedup, vs_fastest])

    out = [f"### {display_task(task)}\n", _table(rows, align)]
    return "\n".join(out) + "\n"


# ---------------------------------------------------------------------------
# Fragment: binary sizes

def render_sizes(report: HostReport) -> str:
    """Binary/section sizes, taken from the patched variant (they match)."""
    loaders = report.loaders()
    variant = report.patched.get("loaders", {})

    header = ["Loader", "Loader .o", "Binary", "text", "data", "bss"]
    align = ["l", "r", "r", "r", "r", "r"]

    entries = []
    for lid in loaders:
        data = variant.get(lid)
        if not data:
            continue
        entries.append((
            lid,
            data.get("loader_object_size", 0),
            data.get("binary_size", 0),
            data.get("sections", {}).get("text", 0),
            data.get("sections", {}).get("data", 0),
            data.get("sections", {}).get("bss", 0),
        ))

    if not entries:
        return "## Binary sizes\n\n_No size data available._\n"

    entries.sort(key=lambda e: e[2])  # by binary size

    rows = [header]
    for lid, lo, binsz, t, d, b in entries:
        rows.append([
            display_loader(lid),
            f"{lo:,}",
            f"{binsz:,}",
            f"{t:,}",
            f"{d:,}",
            f"{b:,}",
        ])

    out = [
        "## Binary sizes\n",
        "All sizes in bytes. Sorted by stripped binary size. "
        "Section values come from `size`; Mach-O binaries report BSS as zero "
        "because the Mach-O segment model folds zero-init into `__DATA`.\n",
        _table(rows, align),
    ]
    return "\n".join(out) + "\n"


# ---------------------------------------------------------------------------
# Fragment: hardware / host details

def render_hardware(report: HostReport) -> str:
    """Collapsible <details> block with host/toolchain/versions/vulkaninfo."""
    meta = report.metadata
    host = meta.get("host", {})
    tc = meta.get("toolchain", {})
    versions = meta.get("versions", {})

    lines = [
        "<details>",
        "<summary>Test host details</summary>",
        "",
        "### Host",
        "",
    ]

    host_rows = [
        ["OS",           f"{host.get('os', '?')} {host.get('kernel_version', '')}".strip()],
        ["Architecture", host.get("arch", "?")],
        ["CPU",          host.get("cpu", "?")],
    ]
    lines.append(_table([["Field", "Value"]] + host_rows, ["l", "l"]))

    lines.extend([
        "",
        "### Toolchain",
        "",
    ])
    tc_rows = [
        ["CC",       f"{tc.get('cc', '?')} ({tc.get('cc_version', '?')})"],
        ["CXX",      f"{tc.get('cxx', '?')} ({tc.get('cxx_version', '?')})"],
        ["OPTFLAGS", f"`{tc.get('optflags', '')}`"],
        ["CFLAGS",   f"`{tc.get('cflags', '')}`"],
        ["CXXFLAGS", f"`{tc.get('cxxflags', '')}`"],
    ]
    lines.append(_table([["Field", "Value"]] + tc_rows, ["l", "l"]))

    lines.extend([
        "",
        "### Project versions",
        "",
    ])

    ver_rows: list[list[str]] = []
    seen: set[str] = set()
    for key, display in VERSION_ORDER:
        if key in versions:
            ver_rows.append([display, f"`{versions[key]}`"])
            seen.add(key)
    for key in sorted(versions.keys()):
        if key not in seen:
            ver_rows.append([key, f"`{versions[key]}`"])

    lines.append(_table([["Project", "Version"]] + ver_rows, ["l", "l"]))

    lines.extend([
        "",
        "### vulkaninfo",
        "",
        "```",
        meta.get("vulkaninfo_summary", "(not captured)").rstrip(),
        "```",
        "",
        "</details>",
        "",
    ])
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# SVG bar chart renderer

# Colors picked to work on both light and dark GitHub themes without needing
# a <picture> dual-SVG trick. Slate-500 text reads on both white and dark
# backgrounds; emerald and slate-400 bars stay distinguishable from each
# other regardless of theme.
SVG_COLOR_TEXT       = "#64748b"
SVG_COLOR_GRID       = "#cbd5e1"
SVG_COLOR_UNPATCHED  = "#94a3b8"
SVG_COLOR_PATCHED    = "#10b981"

SVG_FONT_FAMILY = "system-ui, -apple-system, Segoe UI, Roboto, sans-serif"


def _svg_escape(text: str) -> str:
    return (text.replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;")
                .replace('"', "&quot;"))


def render_chart(report: HostReport, task: str) -> str | None:
    """Horizontal log-scale bar chart: two bars per loader, unpatched + patched.

    Returns the SVG as a string, or None if there's nothing to chart (no
    data for this task in either variant).
    """
    loaders = report.loaders()
    entries: list[tuple[str, float | None, float | None]] = []
    for lid in loaders:
        u = report.get_avg("unpatched", lid, task)
        p = report.get_avg("patched", lid, task)
        if u is None and p is None:
            continue
        entries.append((lid, u, p))

    if not entries:
        return None

    # Sort by patched avg ascending (fastest on top), missing-patched last.
    entries.sort(key=lambda e: math.inf if e[2] is None else e[2])

    all_values = [v for _, u, p in entries for v in (u, p) if v is not None and v > 0]
    if not all_values:
        return None

    lo_pow = math.floor(math.log10(min(all_values)))
    hi_pow = math.ceil(math.log10(max(all_values)))
    if lo_pow == hi_pow:
        hi_pow += 1

    left   = 200       # left edge of bar area (space for loader labels)
    right  = 760       # right edge of bar area
    top    = 70        # top of first bar row
    row_h  = 46        # vertical space per loader (two bars + padding)
    bottom = top + row_h * len(entries) + 40
    width  = 800
    bar_w  = right - left

    def x_for(val: float) -> float:
        if val <= 0:
            return left
        return left + bar_w * (math.log10(val) - lo_pow) / (hi_pow - lo_pow)

    def fmt_axis_tick(power: int) -> str:
        val = 10 ** power
        if power < 0:
            return f"0.{'0' * (-power - 1)}1\u00b5s"
        return f"{int(val):,}\u00b5s"

    svg: list[str] = []
    svg.append(
        f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {bottom}" '
        f'font-family="{SVG_FONT_FAMILY}" font-size="13">'
    )

    # Title
    title = f"{display_task(task)} \u2014 {display_host(report.host_id)}"
    svg.append(
        f'<text x="{width // 2}" y="26" text-anchor="middle" '
        f'font-size="15" font-weight="600" fill="{SVG_COLOR_TEXT}">'
        f'{_svg_escape(title)}</text>'
    )

    # Legend
    lg_y = 46
    svg.append(
        f'<rect x="{left}" y="{lg_y - 10}" width="14" height="12" '
        f'fill="{SVG_COLOR_UNPATCHED}"/>'
        f'<text x="{left + 20}" y="{lg_y}" fill="{SVG_COLOR_TEXT}">unpatched</text>'
        f'<rect x="{left + 110}" y="{lg_y - 10}" width="14" height="12" '
        f'fill="{SVG_COLOR_PATCHED}"/>'
        f'<text x="{left + 130}" y="{lg_y}" fill="{SVG_COLOR_TEXT}">patched</text>'
    )

    # Gridlines + axis labels
    axis_y = top + row_h * len(entries) + 6
    for power in range(lo_pow, hi_pow + 1):
        val = 10 ** power
        x = x_for(val)
        svg.append(
            f'<line x1="{x:.1f}" y1="{top - 6}" x2="{x:.1f}" y2="{axis_y}" '
            f'stroke="{SVG_COLOR_GRID}" stroke-width="1"/>'
        )
        svg.append(
            f'<text x="{x:.1f}" y="{axis_y + 16}" text-anchor="middle" '
            f'fill="{SVG_COLOR_TEXT}" font-size="11">{fmt_axis_tick(power)}</text>'
        )

    # Rows
    for i, (lid, u, p) in enumerate(entries):
        y = top + row_h * i
        label_y = y + 22
        svg.append(
            f'<text x="{left - 10}" y="{label_y}" text-anchor="end" '
            f'fill="{SVG_COLOR_TEXT}">'
            f'{_svg_escape(display_loader(lid))}</text>'
        )

        # Unpatched bar (top)
        if u is not None and u > 0:
            xu = x_for(u)
            svg.append(
                f'<rect x="{left}" y="{y + 6}" width="{max(1.0, xu - left):.1f}" '
                f'height="12" fill="{SVG_COLOR_UNPATCHED}"/>'
            )
            svg.append(
                f'<text x="{xu + 5:.1f}" y="{y + 16}" fill="{SVG_COLOR_TEXT}" '
                f'font-size="11">{u:,.1f}\u00b5s</text>'
            )

        # Patched bar (bottom)
        if p is not None and p > 0:
            xp = x_for(p)
            svg.append(
                f'<rect x="{left}" y="{y + 22}" width="{max(1.0, xp - left):.1f}" '
                f'height="12" fill="{SVG_COLOR_PATCHED}"/>'
            )
            svg.append(
                f'<text x="{xp + 5:.1f}" y="{y + 32}" fill="{SVG_COLOR_TEXT}" '
                f'font-size="11">{p:,.1f}\u00b5s</text>'
            )

    svg.append("</svg>")
    return "\n".join(svg) + "\n"


# ---------------------------------------------------------------------------
# Amalgamated per-platform document

AMALGAMATED_HEADER = """# {host_display} report

_This file is generated by `make render` from `reports/{host_id}/*.json`.
Do not edit by hand - edits will be overwritten on the next render._

> **How to read this.** These numbers measure function-pointer loading overhead
> and Vulkan context setup across different API loaders, comparing the stock
> Vulkan loader against the patched one (see the README for what the patch
> does). **Lower is better.**
>
> **Do not compare across hosts.** Hardware, drivers, OS, and compiler flags
> all differ between the Linux, MinGW, and macOS reports. The meaningful
> comparisons are *within* a single report: loader vs. loader and patched vs.
> unpatched on the same hardware.

"""


def render_host(report: HostReport, out_dir: Path) -> None:
    """Produce all fragments + charts + the amalgamated <host>.md file."""
    host_id = report.host_id
    host_dir = out_dir / host_id
    host_dir.mkdir(parents=True, exist_ok=True)

    # Fragments
    at_a_glance = render_at_a_glance(report)
    sizes = render_sizes(report)
    hardware = render_hardware(report)
    task_fragments: dict[str, str] = {}
    for task in report.tasks():
        task_fragments[task] = render_task_detail(report, task)

    (host_dir / "at-a-glance.md").write_text(at_a_glance, encoding="utf-8")
    (host_dir / "sizes.md").write_text(sizes, encoding="utf-8")
    (host_dir / "hardware.md").write_text(hardware, encoding="utf-8")
    task_sub = host_dir / "task"
    task_sub.mkdir(exist_ok=True)
    for task, md in task_fragments.items():
        (task_sub / f"{task}.md").write_text(md, encoding="utf-8")

    # Charts
    charts_dir = host_dir / "charts"
    charts_dir.mkdir(exist_ok=True)
    chart_paths: dict[str, str] = {}
    for task in CHARTED_TASKS:
        if task not in report.tasks():
            continue
        svg = render_chart(report, task)
        if svg is None:
            continue
        path = charts_dir / f"{task}.svg"
        path.write_text(svg, encoding="utf-8")
        # Path relative to the amalgamated <host>.md, which lives one dir up
        chart_paths[task] = f"{host_id}/charts/{task}.svg"

    # Amalgamated per-platform document
    parts: list[str] = [AMALGAMATED_HEADER.format(
        host_id=host_id,
        host_display=display_host(host_id),
    )]
    parts.append(at_a_glance)
    parts.append("## Task detail\n")
    for task in report.tasks():
        parts.append(task_fragments[task])
        if task in chart_paths:
            parts.append(
                f"![{display_task(task)} ({host_id})]"
                f"({chart_paths[task]})\n"
            )
    parts.append(sizes)
    parts.append(hardware)

    (out_dir / f"{host_id}.md").write_text("\n".join(parts), encoding="utf-8")
    print(f"rendered {host_id} ({len(report.loaders())} loaders, "
          f"{len(report.tasks())} tasks, {len(chart_paths)} charts)",
          file=sys.stderr)


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--reports-dir", type=Path, default=Path("reports"),
                   help="Root reports directory (default: reports).")
    p.add_argument("--host", action="append", default=[],
                   help="Render only these hosts (repeatable). "
                        "Default: render every subdirectory of --reports-dir "
                        "that has the three expected JSON files.")
    args = p.parse_args()

    reports_dir: Path = args.reports_dir
    if not reports_dir.is_dir():
        print(f"error: {reports_dir} is not a directory", file=sys.stderr)
        return 1

    if args.host:
        host_dirs = [reports_dir / h for h in args.host]
    else:
        host_dirs = sorted(
            d for d in reports_dir.iterdir()
            if d.is_dir()
            and (d / "metadata.json").is_file()
            and (d / "patched.json").is_file()
            and (d / "unpatched.json").is_file()
        )

    if not host_dirs:
        print("warning: no host reports found to render", file=sys.stderr)
        return 0

    for host_dir in host_dirs:
        if not host_dir.is_dir():
            print(f"error: {host_dir} not found", file=sys.stderr)
            return 1
        report = HostReport.load(host_dir)
        render_host(report, reports_dir)

    return 0


if __name__ == "__main__":
    sys.exit(main())
