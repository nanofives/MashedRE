# RESULT — the stale-citation sweep FAILS its own known-answer check. The fix is a schema change.

**RAN 2026-10-05.** New tool `re/tools/stale_uncertain_refs.py`. **No C-level moved, no band moved,
no code under `mashedmod/` changed.** Read-only sweep.

---

## 1. The known-answer check, and it failed

The detection rule was written into the tool's docstring **before** the first run. The obvious KA:
run it against the **pre-fix `gamesave_parse.py`** (`git e3e60fec~1`) — the exact file whose stale
header misled three legs.

**Result: 0 hits.** The tool does not catch the case it was written for.

**Cause, and it is on the row side.** The citation side worked — the old header says *"Who writes it
is **unresolved** (**UNCERTAIN** U-3559)"*, which matches every open-framing word. The row side did
not: **U-3559's row states its answer in prose** —

> `| U-3559 | structural | 2026-05-22 | Tail [...] **IS written by** Save::SerializeToBuffer ...`

— with no `RESOLVED` / `CLOSED` / `~~struck~~` marker anywhere, so it is never classified resolved.
**U-3558 is the same shape**: `"Field position ANSWERED"` inside a row typed plain `structural`.

I predicted this failure mode in the docstring (*"some rows state their answer in prose … Those are
MISSED"*) and ran the sweep anyway. Without the KA I would have reported its 60 code hits / 15 tool
hits as a worklist for the gamesave class. **They are not that class.**

## 2. What the sweep does find, stated at its real strength

A **weaker** class: rows that are *explicitly marker-resolved* AND cited as open. 3125 rows, 1369
marker-resolved; **60 citations** across `mashedmod/src`, `re/tools`, `re/frida`, `scripts`, of
which **15** are in tools and harnesses.

Spot-checking the tool hits, most are benign — a row resolved on one axis while the code cites a
still-open sub-question (`U-0170` "is 0xff000000 a colour, mask or arbitrary u32", `U-6621` audio
callback contract, `U-1552` `DAT_00898ab0 = 0x40` semantic). Three hits in `scripts/reclassify_batch_*.py`
are mint-time provenance strings, not claims.

**One candidate in the same subsystem, and it is weaker than the gamesave case.**
`re/frida/probe_save_globals.py:3` describes U-3558 as *"identity of the 12 bytes stride-gathered
from **0x007F105C** (stride 0x4C)"*, while the row says the gathered byte is *"at offset +0x24 of a
19-dword (0x4C) record based at **0x007F1038** — **NOT** dword0 of a record at 0x007F105C"*.

**But `0x7F1038 + 0x24 = 0x7F105C`**, so both name the same address and today's decompilation of
`FUN_00404ee0` shows Ghidra's `&DAT_007f105c` too. The header is **superseded in framing, not
factually wrong** — unlike `gamesave_parse.py`, which carried a transposed digit in a live constant.
Recorded, **not** rewritten: changing it would be churn, and the row already holds the better
description.

## 3. The real fix is a schema change, and it is the owner's call

**`UNCERTAINTIES.md` has no machine-readable status field.** 1369 of 3125 rows happen to carry a
marker word; the rest cannot be classified by any text rule that does not also mis-fire, because the
file's row format is heterogeneous (U-3559 has ~5 cells, newer rows have 7) and resolutions are
written in free prose.

A `status:` column (`open` / `narrowed` / `resolved` / `retracted`) would make this sweep reliable
in one pass. That is a tracker-schema change across 3125 rows — **architecture-level, not a
mechanical edit**, and `CLAUDE.md`'s "stop and ask" list covers it. Not done.

**Interim mitigation that costs nothing:** when resolving a row, put a marker word in the **Type**
cell, as the 2026-10-05 rows do (`~~structural~~ **RESOLVED …**`). U-3559 and U-3558 predate that
habit.

## 4. The tool is committed WITH its failure recorded

It is kept rather than deleted, with a boxed warning at the top of its docstring, because the next
person to notice this problem would otherwise rebuild the same regex and hit the same wall. **A
clean run from it must not be read as "no stale citations".**

## 5. What is NOT concluded

- **The sweep did not find a second gamesave-class defect.** It also could not have, given §1 — so
  that is not evidence one does not exist.
- No file was edited as a result of this sweep. `probe_save_globals.py` is left alone deliberately.
- No C-level, no band, no tracker row changed.
