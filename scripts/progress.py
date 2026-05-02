r"""Mashed RE progress dashboard.

Reads hooks.csv and prints a per-subsystem confidence histogram + percent toward
S-DoD. Also surfaces open STUBS / UNCERTAINTIES counts.

Usage:
    py -3.12 scripts\progress.py
    py -3.12 scripts\progress.py --subsystem audio
    py -3.12 scripts\progress.py --json
"""
import argparse
import csv
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
HOOKS = ROOT / "hooks.csv"
STUBS = ROOT / "STUBS.md"
UNCERT = ROOT / "UNCERTAINTIES.md"
DEFER = ROOT / "DEFERRED.md"

LEVELS = ["C0", "C1", "C2", "C3", "C4"]
SUBSYSTEMS = [
    "boot", "audio", "ai", "vehicle", "track", "render",
    "hud", "frontend", "input", "save", "net", "util", "unknown",
]


def load_hooks():
    if not HOOKS.exists():
        return []
    rows = []
    with HOOKS.open(newline="", encoding="utf-8") as f:
        for row in csv.DictReader(line for line in f if not line.startswith("#")):
            rows.append(row)
    return rows


def count_md_table_rows(path: Path, header_anchor: str) -> int:
    """Count table data rows under a section starting at `## <header_anchor>`."""
    if not path.exists():
        return 0
    text = path.read_text(encoding="utf-8")
    # find the section
    m = re.search(r"^## " + re.escape(header_anchor) + r"\s*$", text, re.MULTILINE)
    if not m:
        return 0
    rest = text[m.end():]
    # stop at next H2
    nxt = re.search(r"^## ", rest, re.MULTILINE)
    section = rest[: nxt.start()] if nxt else rest
    # rows look like: | data | data | ... |
    rows = [
        ln for ln in section.splitlines()
        if ln.startswith("|") and not re.match(r"^\|\s*-{3,}", ln) and not re.match(r"^\|\s*(ID|RVA|Type)\b", ln)
    ]
    # remove blanks (just `|     |     |`)
    return sum(1 for r in rows if any(cell.strip() for cell in r.split("|")[1:-1]))


def histogram(rows, by="subsystem"):
    h = defaultdict(Counter)
    for r in rows:
        key = r.get(by, "") or "unknown"
        c = r.get("confidence", "C0") or "C0"
        h[key][c] += 1
    return h


def percent_done(counts: Counter) -> float:
    """Heuristic 'percent toward S-DoD': weighted average where C0=0, C4=100."""
    total = sum(counts.values())
    if total == 0:
        return 0.0
    weights = {"C0": 0, "C1": 25, "C2": 50, "C3": 80, "C4": 100}
    return sum(weights[lvl] * counts.get(lvl, 0) for lvl in LEVELS) / total


def render_table(h, only=None):
    cols = LEVELS + ["total", "%S-DoD"]
    rows_out = []
    keys = sorted(h.keys()) if only is None else [only]
    for sub in keys:
        c = h.get(sub, Counter())
        total = sum(c.values())
        rows_out.append([sub] + [str(c.get(lvl, 0)) for lvl in LEVELS] + [str(total), f"{percent_done(c):5.1f}"])
    if not rows_out:
        return "(no data — hooks.csv has no rows)\n"
    widths = [max(len(r[i]) for r in [["subsystem"] + cols] + rows_out) for i in range(len(cols) + 1)]

    def fmt(cells):
        return "  ".join(s.ljust(w) for s, w in zip(cells, widths))

    out = [fmt(["subsystem"] + cols), fmt(["-" * w for w in widths])]
    for r in rows_out:
        out.append(fmt(r))
    return "\n".join(out) + "\n"


def main(argv=None):
    p = argparse.ArgumentParser()
    p.add_argument("--subsystem", help="filter to one subsystem")
    p.add_argument("--json", action="store_true", help="machine-readable output")
    args = p.parse_args(argv)

    rows = load_hooks()
    h = histogram(rows, by="subsystem")
    open_stubs = count_md_table_rows(STUBS, "Active stubs")
    open_uncert = count_md_table_rows(UNCERT, "Active uncertainties")
    open_defer = count_md_table_rows(DEFER, "Active")

    if args.json:
        out = {
            "total_functions_tracked": len(rows),
            "by_subsystem": {k: dict(v) for k, v in h.items()},
            "percent_dod": {k: round(percent_done(v), 1) for k, v in h.items()},
            "open_stubs": open_stubs,
            "open_uncertainties": open_uncert,
            "open_deferred": open_defer,
        }
        print(json.dumps(out, indent=2, sort_keys=True))
        return 0

    print(f"Mashed RE — progress dashboard")
    print(f"  hooks.csv rows: {len(rows)}")
    print(f"  open stubs:        {open_stubs}")
    print(f"  open uncertainties:{open_uncert}")
    print(f"  open deferred:     {open_defer}")
    print()
    print(render_table(h, only=args.subsystem))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
