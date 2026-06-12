#!/usr/bin/env python3
"""audit_seeded_argtypes.py — derive which diff_template.js arg_type handlers
seed non-zero state (sentinels / registry inputs) into the observed region
before calling the target.

Feeds the SEEDED_ARG_TYPES set in run_diff.py (non-degeneracy assertion):
for a SEEDED handler an all-zero post-observation still discriminates (a
no-op would have left the sentinel), so the degeneracy flag must not fire.
For an OBSERVE-ONLY handler, all-trivial observations on both sides prove
nothing — the run is INCONCLUSIVE-DEGENERATE, not GREEN.

Heuristic source audit; re-run after any diff_template.js handler change and
reconcile the printed sets against run_diff.py.
"""

import re
from pathlib import Path

TEMPLATE = Path(__file__).resolve().parent / "diff_template.js"

# Pre-call writes of something non-zero: a sentinel constant, a registry
# input, or a saved/derived value. Zero-initialization (writeU32(0)) does
# NOT count — a zero-filled out-buffer plus an all-zero result is no signal.
NONZERO_SEED = re.compile(
    r"write(?:U32|U16|U8|S32|Float|Double|ByteArray|Pointer)\(\s*"
    r"(?!0\s*\)|0x0+\s*\)|ptr\(\s*0\s*\))"
    r"[^)]*?(?:input|sentinel|0x[0-9A-Fa-f]*[1-9A-Fa-f]|\bsv\b|srcVal|test|seed|pre[A-Z0-9_]"
    r"|\bv\b|\bvals?\b|\bv\[)",
)
SENTINEL_LITERALS = ("0xDEADBEEF", "0xA5A5A5A5", "0xCD", "sentinel", "Sentinel")

# Known heuristic misses (manually reconciled 2026-06-12):
#   - restore-after-call writes (e.g. large_buffer_save_restore's
#     writeByteArray(lbSnapshot)) are NOT seeds — that handler observes live
#     memory and is correctly OBSERVE-ONLY.
#   - guard/baseline restores (writeU32(saved...)) likewise are not seeds; a
#     handler is SEEDED only if the non-zero write lands in the observed
#     region BEFORE the target call. When in doubt, leave it OBSERVE-ONLY —
#     the worst case is an explicit degenerate_ok per hook, never a silent
#     false GREEN.


def handler_blocks(src: str):
    hits = [(m.start(), m.group(1))
            for m in re.finditer(r"CONFIG\.arg_type\s*===\s*'(\w+)'", src)]
    blocks = {}
    for i, (pos, name) in enumerate(hits):
        end = hits[i + 1][0] if i + 1 < len(hits) else len(src)
        blocks[name] = blocks.get(name, "") + src[pos:end]
    return blocks


def classify(blocks):
    seeded, observe = [], []
    for name, blk in sorted(blocks.items()):
        if NONZERO_SEED.search(blk) or any(s in blk for s in SENTINEL_LITERALS):
            seeded.append(name)
        else:
            observe.append(name)
    return seeded, observe


def main():
    src = TEMPLATE.read_text(encoding="utf-8")
    blocks = handler_blocks(src)
    seeded, observe = classify(blocks)
    print(f"{len(blocks)} arg_types found in {TEMPLATE.name}")
    print(f"\nSEEDED ({len(seeded)}) — all-zero post-observation still discriminates:")
    for n in seeded:
        print(f"  '{n}',")
    print(f"\nOBSERVE-ONLY ({len(observe)}) — all-trivial observations = no evidence:")
    for n in observe:
        print(f"  {n}")


if __name__ == "__main__":
    main()
