# RESULT PORT-TOP-07, 2026-11-08

**Verdict: GREEN on the RE leg; the port leg is deferred.** Course::Finish **0x0040d270** (441 B) is byte-identical
under `re/tools/match/match.py`, and relocheck reports OK. It is promoted C2 -> C4 by route match (L-93) through
`re/tools/c4_reclass.py batch`. The exe has no body of 0x0040d270 on the default path, and PORT-TOP-02's
`MASHED_ORIG_TOP` scaffold is not on `orch/integration` yet (ledger stage `working`, the knob string is absent from
`mashedmod/`). So no swap was made and `mashedmod/` is unchanged. Gates were not run because nothing in `mashedmod/`
changed.

Base: `orch/integration` @ `ea666edab`.

## Match (RE leg)

| rva | size | file | result | relocheck | C++ |
|---|---|---|---|---|---|
| 0x0040d270 | 441 B | `re/match/top07/0040d270.c` | **100.0%** (regs 100.0, ops 100.0) 135/135 ins, whole symbol **BYTE-IDENTICAL** (`--bytes`, relocations masked) | **OK** checked=48 unchecked=0 | ok (VS2022 `cl /TP /O2 /DMATCH_PORT=1`, compile only) |

- Compiler XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), flags `/O2 /GS /Ob1`, C, toolchain ddk.
- Source sha256/16 `81b6362d6811f771`.
- Not inline asm: plain C, no `__asm`.
- Names: only `FUN_`/`DAT_` names, Ghidra-style locals and one mechanical helper name (`Cond_0040d270`). No semantic
  names were added.

Facts the decomp missed, read from the listing (`re/tools/match/odis.py 0x0040d270`):
- 0x00426340, 0x004264d0 and 0x00411d90 each take one pushed argument, `**(param_1 + param_2 * 4)` (sites
  0x0040d305/0x0040d307, 0x0040d2cc/0x0040d2ce, 0x0040d3df/0x0040d3e1). Ghidra drops it at the first 0x004264d0 site,
  at both 0x00426340 sites and at 0x00411d90.
- 0x00426070 takes no argument here (no push before 0x0040d3c8).
- `DAT_00803324` is stored the float 0x44c30000 (1560.0f) at 0x0040d3d2.
- The elapsed time `FUN_004950b0() - start` is unsigned. At 0x0040d406 the compiler adds the float at 0x005cc94c
  when the value is negative, then multiplies by the float at 0x005ccd68 and prints it with format 0x005ccd48
  (`"course LoadTime is %u (%.3f)\n"`).

What closed it. The first source, which follows Ghidra's structure, compiles at **55.6%** with 130 of 135
instructions. The compiler places the B path (0x0040d345, the 0x0042fe30 / 0x0040d040 branch) straight after the test
and the A path last. It then merges A's duplicated `0x0040d110 + 0x004ccde0` pair. The original lays A out first
(0x0040d2ae..0x0040d343, two `jmp 0x0040d3c3`), and B falls into the common tail at 0x0040d3c3. None of these spellings
moved that layout: either if/else order, `!( && )` vs `||` chains, nested ifs with `goto`, A first with `goto done`, a
one-case `switch`, the duplicated pair written in both arms, the whole tail duplicated in both branches, a
block-local rank variable, `iVar2` reused for the rank, or a `return`-expression inline helper. Each one stays at
55.6% or drops to 44-55%. A permuter pass (`match_permute.py`, 3 rounds, 297 compiles) reached 57.0%.
**100%** came from writing the three-part test (0x0040d289..0x0040d2a8) as a `static __inline` function. Its body is
`if (a && car == FUN_00426c00() && FUN_0042f790() == 0) return 1; return 0;` and the call site is
`if (!Cond_0040d270(...)) {A} else {B}`. Under /Ob1 that function is expanded. The same helper called as
`if (Cond) {B} else {A}` stays at 55.6%.

## C4 claim

`verify/porttop07_20261108/c4_batch.tsv` (1 row: `0x0040d270 match re/match/top07/0040d270.c`).
- Dry run: `[1/1] 0x0040d270 match PASS byte-identical 441 B, relocheck OK (48/0)`.
- `--apply --result verify/porttop07_20261108/RESULT_PORT-TOP-07.md`: hooks.csv row 0x0040d270 C2 -> C4 (`c4=match`),
  plus one CHANGELOG line.

## Port leg (deferred)

- The default path does not run 0x0040d270. The exe has no body for it. `mashedmod/src/mashed_re/Track/CourseLoadDispatch.cpp:37`
  is a raw-address `call_0040d270` used by the .asi hook of 0x0040d440 only. `Race/GameModeMachine.cpp:66` binds the
  matched 0x004929d0's callee 0x0040d440 to `GameModeMachine_Case2Unreached`, which logs "no exe body for 0x0040d270".
  The exe's course load is TrackRenderer port code.
- The task's condition for a knob build is that PORT-TOP-02's `MASHED_ORIG_TOP` scaffold is on `orch/integration`. It
  is not: the ledger row is `working` on `orch/PORT-TOP-02`, and `grep MASHED_ORIG_TOP mashedmod/` returns nothing on
  this base. So the swap is left for a later row.
- Callee body map for that row: `verify/porttop07_20261108/callees_0040d270.tsv` (23 callees, hooks.csv `port`
  column on this base). 5 have an exe body: `body` for 0x00426ba0, 0x00426c00, 0x00426020 and 0x004987b0, `called` for
  0x0042fe30, and 0x00462510 / 0x00462500 / 0x004950b0 / 0x0042f790 / 0x0042f7a0 / 0x00426070 / 0x0040d020 have
  matched or port sources that are not compiled into the exe (`port=none`). 3 callees are still C2: 0x00407800,
  0x0040d110 and 0x00411d90 (Replay::CreateOrLoad). These need a MATCH row, or a stand-in under the knob, before the
  swap can link without raw-address forwarders.
- When the scaffold lands, the swap is: compile `re/match/top07/0040d270.c` as C++ (MATCH_PORT) behind
  `MASHED_ORIG_TOP`, bind callees by name (PLAN.md section 4), add each binding to BINDINGS.tsv, and point
  `GameModeMachine`'s 0x0040d440 binding at the matched 0x0040d440 (`re/match/s02/0040d440.c`) under the knob.

## Gates

Not run, because `mashedmod/` is unchanged on this branch (match source, RESULT, trackers only).

## Files

- `re/match/top07/0040d270.c` (new)
- `verify/porttop07_20261108/RESULT_PORT-TOP-07.md`, `c4_batch.tsv`, `callees_0040d270.tsv`, `decomp_0040d270.txt`
- `hooks.csv`, `re/analysis/CHANGELOG.md` (through c4_reclass.py `--apply`)
