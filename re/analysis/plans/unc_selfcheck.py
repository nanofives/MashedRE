#!/usr/bin/env python3
"""
unc_selfcheck.py — per-note self-check for the inline-[UNCERTAIN] hygiene wave.

c3_filter_v4.py hardcodes ROOT to the MAIN checkout, so re-running it from a
worktree reads main's notes, not your edits. This helper takes explicit paths
so a worker can verify a single edited note against its own worktree's
UNCERTAINTIES.md BEFORE merge. It reuses c3_filter_v4's exact inline_uncertain
logic (imported, not re-implemented) so it can't drift from the real gate.

Usage:
    py -3.12 re/analysis/plans/unc_selfcheck.py \
        --note   .worktrees/unc-hyg-sN/re/analysis/<dir>/<note>.md \
        --uncert .worktrees/unc-hyg-sN/UNCERTAINTIES.md \
        --rva    0041d410

Exit 0 + "PASS" => the inline_uncertain gate no longer fires for this note.
Exit 1 + "FAIL: <reason>" => still rejected; reason is the filter's own string.
A PASS here means ONLY that the UNCERTAIN gate is clear — the candidate may
still be rejected by other gates (shape_only, callee_gate, ...); those are out
of scope for this wave.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

# Reuse the real gate logic verbatim — never re-implement it here.
from c3_filter_v4 import inline_uncertain_blocks  # noqa: E402

# Faithful copy of c3_filter_v4.parse_uncertainties_blocks but for an arbitrary
# path (the module's version reads the hardcoded main-checkout UNCERT global).
_TABLE_ROW_RE = re.compile(r"^\|\s*(U-\d{3,5})\s*\|(.*)\|\s*$")


def parse_uncert(path: Path) -> dict[str, list[tuple[str, str]]]:
    out: dict[str, list[tuple[str, str]]] = {}
    if not path.exists():
        return out
    for raw in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = _TABLE_ROW_RE.match(raw)
        if not m:
            continue
        uid = m.group(1)
        cells = [c.strip() for c in m.group(2).split("|")]
        if len(cells) >= 6:
            where, blocks = cells[1].lower(), cells[-1]
        elif len(cells) == 5:
            where, blocks = cells[0].lower(), "none"
        else:
            where, blocks = "", "none"
        out.setdefault(uid, []).append((where, blocks))
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--note", required=True)
    ap.add_argument("--uncert", required=True)
    ap.add_argument("--rva", required=True)
    a = ap.parse_args()

    body = Path(a.note).read_text(encoding="utf-8", errors="replace")
    blocks_map = parse_uncert(Path(a.uncert))
    rva = a.rva.lower().lstrip("0x").zfill(8)

    reason = inline_uncertain_blocks(body, blocks_map, rva)
    if reason:
        print(f"FAIL ({a.rva}): {reason}")
        return 1
    print(f"PASS ({a.rva}): inline_uncertain gate clear")
    return 0


if __name__ == "__main__":
    sys.exit(main())
