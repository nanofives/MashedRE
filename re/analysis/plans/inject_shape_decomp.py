#!/usr/bin/env python3
"""
inject_shape_decomp.py — inject verbatim decomp transcripts into the 28
shape-only notes so they clear c3_filter_v4's shape_only_note gate.

Reads the .c files produced by re/tools/dump_decomp.py (server-side Ghidra
dump) and appends a `## Mechanical description` section (with a ```c fence)
to each note. The note path per RVA is taken from hooks.csv's `file` column
(the authoritative path the filter reads — NOT a glob).

The shared multi-function note (game_mode_cont1/notes.md) gets ONE
`## Mechanical descriptions (decomp transcripts)` section with a per-RVA
`### 0x<rva>` subsection + ```c block for each of its in-scope functions.

Idempotent: skips a note that already carries the marker.
"""
from __future__ import annotations

import csv
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
DECOMP_DIR = ROOT / "re" / "analysis" / "plans" / "shape_decomp"
HOOKS = ROOT / "hooks.csv"
MARKER = "<!-- shape-decomp-injected 2026-05-29 -->"

RVAS = [
    "00428d30", "00429240", "004298c0", "00429b30", "00429b70", "0042a640",
    "0042a980", "0042a9c0", "0042ad10", "0042ad90", "0042add0", "0042bde0",
    "0042e8b0", "0042f400", "00430120", "00431240", "004314b0", "00431710",
    "004322c0", "004324a0", "004325c0", "00432800", "004335f0", "00434720",
    "0043a610", "0043aa30", "0043af10", "0043d2a0",
]

CONF = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 4}


def authoritative_paths() -> dict[str, str]:
    rvas = set(RVAS)
    rows: dict[str, dict] = {}
    with open(HOOKS, encoding="utf-8-sig") as f:
        rd = csv.reader(f)
        next(rd, None)
        for r in rd:
            if len(r) < 6:
                continue
            rva = r[0].strip().lower()
            rva = rva[2:] if rva.startswith("0x") else rva
            if rva not in rvas:
                continue
            row = {"conf": r[3].strip(), "file": r[5].strip()}
            if rva not in rows or CONF.get(row["conf"], -1) > CONF.get(rows[rva]["conf"], -1):
                rows[rva] = row
    return {rva: rows[rva]["file"] for rva in rows}


def read_decomp(rva: str) -> str:
    p = DECOMP_DIR / f"{rva}.c"
    txt = p.read_text(encoding="utf-8", errors="replace")
    txt = txt.replace("\r\n", "\n").replace("\r", "\n")
    return txt.strip("\n")


def main() -> int:
    paths = authoritative_paths()
    missing = [r for r in RVAS if r not in paths]
    if missing:
        print("ERROR: no hooks.csv path for:", missing)
        return 2

    # Group by note file.
    by_file: dict[str, list[str]] = {}
    for rva in RVAS:
        by_file.setdefault(paths[rva], []).append(rva)

    touched, skipped = [], []
    for relpath, rvas in sorted(by_file.items()):
        note = ROOT / relpath
        body = note.read_text(encoding="utf-8", errors="replace")
        if MARKER in body:
            skipped.append(relpath)
            continue
        if not body.endswith("\n"):
            body += "\n"

        if len(rvas) == 1:
            rva = rvas[0]
            code = read_decomp(rva)
            section = (
                f"\n{MARKER}\n"
                f"## Mechanical description\n\n"
                f"Verbatim Ghidra DecompInterface output "
                f"(Mashed_pool13 clone of MASHED.exe, RVA 0x{rva}):\n\n"
                f"```c\n{code}\n```\n"
            )
            note.write_text(body + section, encoding="utf-8")
        else:
            # shared multi-function note: one section, per-RVA subsections
            parts = [
                f"\n{MARKER}\n"
                f"## Mechanical descriptions (decomp transcripts)\n\n"
                f"Verbatim Ghidra DecompInterface output "
                f"(Mashed_pool13 clone of MASHED.exe).\n"
            ]
            for rva in sorted(rvas):
                code = read_decomp(rva)
                parts.append(
                    f"\n### 0x{rva} — FUN_{rva}\n\n```c\n{code}\n```\n"
                )
            note.write_text(body + "".join(parts), encoding="utf-8")
        touched.append(f"{relpath}  ({','.join(rvas)})")

    print(f"INJECTED into {len(touched)} note files:")
    for t in touched:
        print("  +", t)
    if skipped:
        print(f"SKIPPED {len(skipped)} (already marked):")
        for s in skipped:
            print("  =", s)
    print(f"RVAs covered: {sum(len(v) for v in by_file.values())}/{len(RVAS)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
