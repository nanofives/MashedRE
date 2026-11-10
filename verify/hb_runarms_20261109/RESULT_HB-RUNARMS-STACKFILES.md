# RESULT HB-RUNARMS-STACKFILES (2026-11-09)

Verdict: **GREEN**. Each new `run_arms.py` stack now lives in its own file, so two rows that each add a stack no
longer edit the same lines. The resolved `STACK` dict is byte-identical before and after the move (671 stacks).

## Problem
`verify/savelink_20261023/run_arms.py` (the merge train's gate runner, `re/orchestrator/merge_train.json` `runner`)
held every knob stack as `STACK['X'] = ...` lines. Each row that adds a knob appended at the same tail, so any two such
rows conflicted. On 2026-11-09 the train hand-merged that tail 4 times (PC24, AISPLINE-DUALCOPY, SAVE-SPAN-MIRROR,
BAR-Y-FLIP).

## Change
1. **Loader** `load_stack_files(STACK, STACK_DIR)` in `run_arms.py`, called once after the last in-file definition
   (`STACK['BATCH']`, kept in-file because it reads `A4_BATCH_KNOBS`) and before the arm loop.
   `STACK_DIR = <dir of run_arms.py>/stacks`. Format of `stacks/<TAG>.txt`:
   - `#` comment lines and blank lines are ignored
   - one knob per line, written exactly as in a STACK list: `MASHED_X`, `MASHED_X=v`, `-MASHED_X` (removes a base_env var)
   - `@<TAG>` = the knobs of stack `<TAG>` in place (an in-file stack or another file, resolved recursively)
   - errors, each `exit 1` with a message naming the file: a tag already defined in-file or by another file
     (case-insensitive, never an override), an include cycle, an unknown `@` include, a file name outside `[A-Za-z0-9_]+`
     (a `-` would end the tag in an arm name, see `tag_of`)
   - files load after every in-file line: they get no `<tag>T` trace twin (that loop runs mid-file, as it already did
     for every tail stack), and in-file lines cannot read them.
2. **Moved**: the whole recent tail, old lines 496-537, into 39 files under `verify/savelink_20261023/stacks/`:
   PSC, PSR, PSF, NOPSS, NOPSSO, NPSR (port-state), PSG, PSGO, DEFA, PSGA, PSGF (GAMEFLOW), R7P, PSGOR7P, TK, TKD, NAKOFF,
   PSGOTK (HB-GATE-FLAKE-GAMEFLOW), X24, X24H, X24LCW, DEFLCW, X24NCC, X24MM, X24PK, X24NCL, X24NL1, NCC0, MM0, NCL0, NL10,
   NCC0H, X24NCCH (PORT-X87-PC24), DDK, AL, ALDDK (PORT-AISPLINE-DDKORDER), CSI, CSB, CSIB (PORT-COLOURSEL-ICON / BAR-Y),
   SSM (PORT-SAVE-SPAN-MIRROR). Each file carries its block's original comment. The `_PSG` / `_X24` helpers became
   `@PSG` / `@X24` includes. `X24H`, `X24NCCH` and `NCC0H` include the in-file `DEFH`.
3. **Plan mode** `A4_PLAN=1`: `run()` prints the arm's exe, limit, wall and every `MASHED_*` var, then returns before it
   removes, launches or writes anything.
4. **Docs**: the `run_arms.py` docstring (how to add a stack, `A4_PLAN`), `re/orchestrator/WORKER_RULES.md`
   ("What a change may do": a knob's stack goes in its own file, never a new `STACK[...]` line), and
   `re/orchestrator/MERGE_TRAIN.md` section 2. No .ps1/.sh needed an edit: none of them mention `STACK`
   (`merge_train.ps1:198-201` only runs `$wt/<runner> <arms>`).

## Evidence
| Check | Result | File |
|---|---|---|
| Resolved STACK, before vs after (sorted-key JSON via `dump_stack.py`, runs the script with no arm) | **byte-identical**, 671 stacks, both sha256 `1afe5b3a245bed2aa65c410085f9b513893f07bef39a84ffc90cc719e7f03bb3` | `stack_before.json`, `stack_after.json` |
| Plan run `A4_PLAN=1 run_arms.py H_DEF H_X24H B_CSIB_K0 D2_PSGOTK_1 H_NOPSS` (no launch) | DEF = base env only. X24H = `X87_PC24` + `DRIVE_HOLD_MS=42000`. CSIB = both COLOURSEL knobs. PSGOTK = `PORTSTATE_GAMEFLOW` + `DET_TEST_KEY`. NOPSS = `NO_PORTSTATE_SEED` + `VIDEOCFG_WRITE=0` | `plan_output.txt` (sha256 `6e8da7cc...98bf`) |
| Error paths on a scratch copy: in-file dup (FULLGO), case-only dup (fullgo), cycle, unknown include, bad name `A-B` | all 5 `exit=1` with the message. Control file `ZNEW` (`@X24H` + one knob) resolves to `[X87_PC24, DRIVE_HOLD_MS=42000, NEW=1]` | `negative_tests.txt` |
| Merge test (throwaway repo): branch A adds `stacks/RA.txt`, branch B adds `stacks/RB.txt`, merge B into A | clean, exit 0 | (scratch, not kept) |

Two files that differ only in case (`x24.txt` beside `X24.txt`) cannot exist on NTFS: the second write replaces the
first, so that clash is not detectable at load. A file clashing by case with an in-file tag is detected.

## Live copies
Only `verify/savelink_20261023/run_arms.py` changed. It is the one the train calls (`merge_train.json` `runner`,
run in place from the lane worktree) and the one `re/tools/port_batch_bisect.py:100` calls, and it took 21 of the 22
`verify/*/run_arms.py` commits since 2026-11-01. The other dated copies (`verify/d4flip_matchend_st3pretick_20261102/`
with 1 commit, and the older `u97xx` / `*flip` dirs) are not referenced by the merge train config, `re/orchestrator`,
`re/tools`, `scripts` or any skill, so they stay as they are.

## Gates
Not run. `run_arms.py`'s arm recipes, `base_env`, `knobs()` and every resolved stack are unchanged (byte-identical
dump), and no exe changed. The only new runtime path is the `A4_PLAN` early return, which is off unless set.

## For the next row that adds a stack
Write `verify/savelink_20261023/stacks/<TAG>.txt`, check it with
`A4_PLAN=1 py.exe -3.12 verify/savelink_20261023/run_arms.py H_<TAG>`, and commit the file. Rows already in flight that
append `STACK[...]` lines after `STACK['SSM']` will conflict with this change once, because that block is gone: resolve by
putting their stack in a file (or keeping the line in-file just above `STACK['BATCH']`).
