# RESULT MATCH-RACEPATH (L1 section, race-path C4 by match for the ORIG_TOP route, L-111), 2026-11-09

**Verdict: GREEN. 26 race-path rows C2/C3 -> C4 by route match on 11 enqueued slice branches. 0x004111c0 is C4.**

Lead branch `orch/MATCH-RACEPATH` (this file and the inventory only). Every slice used `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178) with each file's own FLAGS, `relocheck.py` OK, promotion through
`re/tools/c4_reclass.py batch --apply`. No mashedmod/ edit and no game run in any slice. The only changed source that
mashedmod includes is `re/match/t01g/004111c0.c` (port TU `Match/MatchT01G_004111c0.cpp`). Its change is DDK-side
only (`#ifndef MATCH_PORT`), so the merge train's build and gates cover it.

## Inventory (lead, `lead/`)

`lead/inv.py` builds `lead/inv.tsv` from `orch/PORT-ORIGTOP-SIZE:verify/portorigtop_20261109/race_point.tsv`: every
race_load / race_run row below C4 (game C0..C3 plus non-C4 RW-Physics, RW core, CRT, Lua, qhull). 185 rows: 153 with
a source under re/match (the gate refused all 153 on the base, `D:/MashedWT/MATCH-RACEPATH_data/gate_all.log`, SHA-256
9ee32276f5c50b0a683612227119b3c24169932b2c2717fe8a4f824e8c0d26c8), 32 with none. Only 8 RW-Physics rows on the path
were below C4, not 25. The "13.7 KB" for 0x004111c0 in hooks.csv was stale: its whole symbol was already
BYTE-IDENTICAL, and the gate refused it on extent only.

## Slices

| slice | rows | closed | yield | C4 rows | branch @ sha |
|---|---|---|---|---|---|
| FRAME | 3 | 1 | exempt | 0x004111c0 (extent override, 6 members = 1498 B) | orch/MATCH-RACEPATH-FRAME @ cbb6eaf73 |
| PILOT (near-miss 1..36 B, never pooled) | 20 | 6 | 30% | 0x00477920 0x00476320 0x00475210 0x00415e20 0x00417640 0x005b10e0 | orch/MATCH-RACEPATH-PILOT @ 837246574 |
| BIG | 2 | 1 | exempt | 0x00446520 (7,412 B) | orch/MATCH-RACEPATH-BIG @ db505a5ef |
| X87A (near-miss, already permuted) | 34 | 8 | 24% | 0x00412cf0 0x00464a50 0x004726f0 0x0048f780 0x00422570 0x00442a60 0x004c1210 0x005a86a0 | orch/MATCH-RACEPATH-X87A @ 0b7cb8ef4 |
| X87B (near-miss 40..155 B) | 12 worked | 1 | 8%, stopped | 0x0041f120 | orch/MATCH-RACEPATH-X87B @ 14c1faada |
| SHIFT (high insn%, many bad B) | 6 worked | 0 | 0%, stopped | none (3 sources improved) | orch/MATCH-RACEPATH-SHIFT @ 92293401b |
| BIG2 | 2 | 0 | exempt | none (0x0046f6c0 44.3 -> 50.7%, 0x00469df0 30.1 -> 63.1%) | orch/MATCH-RACEPATH-BIG2 @ 44f5b5b1a |
| FRESH (no hand source, <= 600 B) | 7 worked | 4 | 57% | 0x004844a0 0x00484580 0x0040d8f0 0x00410860 | orch/MATCH-RACEPATH-FRESH @ 636b61e23 |
| FRESH2 (base FRESH) | 6 worked | 3 | 50% | 0x0048aff0 0x0048b230 0x0048b440 | orch/MATCH-RACEPATH-FRESH2 @ e40d20acf |
| FRESH3 (no hand source, 700..1014 B) | 6 worked | 1 | 17%, stopped | 0x00418560 | orch/MATCH-RACEPATH-FRESH3 @ e06f10e42 |
| FRESH4 (no hand source, 1.1..1.8 KB) | 3 worked | 1 | 33% | 0x004177b0 | orch/MATCH-RACEPATH-FRESH4 @ d616310a5 |

Each slice's RESULT is `verify/matchracepath_20261109/<slice>/RESULT_MATCH-RACEPATH-<SLICE>.md` on its branch.

Yield by type: no hand source up to 600 B 7/13 (54%). Near-misses up to about 30 B 14/54 (26%). Near-misses of
40..155 B 1/12 (8%). Large bodies with a shifted tail 0/6. No hand source of 700 B and more 2/9 (22%).

## Levers found (each one closed rows that earlier slices had marked unreachable)

1. Double intermediate cast back to float, `(float)(a + (double)b)`, or a named double temporary
   `{ double t = a - b; x = (float)t; }` (PILOT, X87A). A double literal that turns into `fmul qword` is a false gain.
2. A comma rvalue `(0, p->f)` makes cl load the struct field into ST0 before the frame operand, and
   `p->a = (0, p->a) + v` does the same for `+=` (BIG: 16 statements in 0x00446520).
3. Fix the first length change first. One statement 6 B too long shifted 2,509 of 0x00446520's 2,725 bad bytes (BIG).
4. Floats that g2s declares as `_DAT_005c....` externs are `__real@` literals. Write the literal (FRESH).
5. A same-TU `static` callee takes its argument in EAX. Switch cases each keep their own call pair. Data shape matters
   against `/GS` cookies. cl unrolls a 4-step constant loop (FRESH).

## What remains on the race path (159 rows below C4)

- Near-closes worth one more pass: 0x00488e70 7 B, 0x0040aef0 12 B, 0x00411ae0 27 B, 0x0048ab70 34 B,
  0x00414c30 50 B (racepath sources), and the 2..10 B partials in PILOT and X87A `results.tsv`.
- No hand source yet: 0x0040ca00 (907 B), 0x004128f0 (1014 B), 0x0040be50 (1849 B), 0x00487280 (2678 B),
  0x004883b0 (2745 B), 0x0040eee0 (3356 B). Library residue (CRT acos/atan/0x004a3700/0x004a371d, Lua 0x004b70d0,
  qhull 0x0057ca30) belongs to the libid route, not match.
- Big, by match %: 0x0040e590 84.6%, 0x00443dc0 57.5%, 0x0046f6c0 50.7%, 0x00469df0 63.1% (frame size first).
- Parked under the 30% rule: near-misses of 40 B and more (`lead/poolB.tsv`, 21 rows never worked) and the 9
  unworked SHIFT rows.
- Open question: 0x0055ae50 is 27 B BYTE-IDENTICAL with a `void` return, but the unmatched caller source
  0x0047def0 compares its return with 6 and 7. The PILOT refused it.

## Tool notes

- `re/tools/match/match_permute.py` mulswap ignores operator precedence. FRAME rejected a variant at 87.3% that
  changed behaviour (`frame/variants/0040e590_permute_rejected.c`).
- `re/tools/decomp_pc.py` run from a worktree resolves `mashed_pool/` under the worktree and fails. Run the main
  checkout's copy (FRESH3, FRESH4).
