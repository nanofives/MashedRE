"""
Resolve an RVA to (md_path, plate_text) for the ghidra-sweep.

Usage:
    py -3.12 scripts/sweep_plate_lookup.py 0x00492370
    py -3.12 scripts/sweep_plate_lookup.py 0x00492370 0x00492270 ...

For each RVA, prints one line:
    RVA <tab> MD_PATH <tab> PLATE_TEXT
where PLATE_TEXT is the first bullet of "## Mechanical description" (or
"## Why C2" fallback), prefixed "[C1 YYYY-MM-DD] ", trimmed to <=120 chars
at word boundary with "..." sentinel.

Missing files print:
    RVA <tab> MISSING <tab> -
"""

import sys
import re
from pathlib import Path
from datetime import date

REPO = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
ANALYSIS = REPO / "re" / "analysis"
TODAY = date.today().isoformat()

BULLET_RE = re.compile(r"^[\s]*[-*]\s+(.+?)(?:\n[\s]*[-*]|\n##|\n\n|\Z)", re.MULTILINE | re.DOTALL)


def find_md(rva_hex):
    """Search re/analysis/**/ for files matching <rva>.md or 0x<rva>.md."""
    rva_lc = rva_hex.lower().lstrip("0x").zfill(8)
    candidates = [
        f"{rva_lc}.md",
        f"0x{rva_lc}.md",
        f"{rva_lc[1:]}.md" if rva_lc.startswith("0") else None,  # 0047xxxx -> 47xxxx
    ]
    candidates = [c for c in candidates if c]
    for cand in candidates:
        matches = list(ANALYSIS.rglob(cand))
        if matches:
            return matches[0]
    return None


def first_bullet_after(text, headings):
    """Find first bullet after any of the given headings (in order). Returns the bullet text."""
    for h in headings:
        idx = text.find(h)
        if idx < 0:
            continue
        section = text[idx + len(h):]
        # Stop at next ## heading
        next_h = section.find("\n## ")
        if next_h > 0:
            section = section[:next_h]
        # First bullet
        m = re.search(r"^[\s]*[-*]\s+(.+?)(?=\n[\s]*[-*]|\n\n|\Z)", section, re.MULTILINE | re.DOTALL)
        if m:
            return m.group(1).strip()
    return None


def make_plate(bullet, today=TODAY):
    """Prefix with [C1 date] and truncate to <=120 chars at word boundary."""
    # Collapse newlines/whitespace
    flat = re.sub(r"\s+", " ", bullet).strip()
    prefix = f"[C1 {today}] "
    budget = 120 - len(prefix)
    if len(flat) <= budget:
        return prefix + flat
    # Truncate at word boundary
    cut = flat[:budget - 1].rsplit(" ", 1)[0]
    return prefix + cut + "..."


def lookup(rva_str):
    rva_hex = rva_str.lower().replace("0x", "").lstrip("0").rjust(8, "0")
    md = find_md(rva_hex)
    if md is None:
        return rva_str, "MISSING", "-"
    text = md.read_text(encoding="utf-8")
    bullet = first_bullet_after(text, ["## Mechanical description", "## Why C2", "## Description"])
    if not bullet:
        return rva_str, str(md.relative_to(REPO)), "[C1 plate] (no mechanical description found)"
    return rva_str, str(md.relative_to(REPO)), make_plate(bullet)


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 1
    for arg in sys.argv[1:]:
        rva, path, plate = lookup(arg)
        print(f"{rva}\t{path}\t{plate}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
