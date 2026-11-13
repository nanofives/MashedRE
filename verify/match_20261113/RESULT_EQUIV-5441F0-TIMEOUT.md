# RESULT EQUIV-5441F0-TIMEOUT (2026-11-13)

Verdict: **GREEN**. Tool row: equiv.py change, equiv self-test, cross-check re-runs. No build, no gates, no game run.
Opus 5.5 medium, claude2.

- Branch `orch/EQUIV-5441F0-TIMEOUT` from `orch/integration` e47e10795.
- **Tool commit: `fea5b5483`** (`re/tools/match/equiv.py` blob `51bb0c5cc3b3da20bdd221549e1350e78e600f61`; before:
  `b720231cb25f9ebc18e1cb2adebf2f1695733f1b`, the blob on orch/integration e47e10795). Any admission that relies on
  this change merges after fea5b5483.
- **0x005441f0** (`re/match/rwtb/005441f0.c`, 99.1%): `0x005441f0	NOT-PROVEN	unsupported: rep movsd with a variable count`,
  in 3 s wall (1035 steps, 897 terms). Before: no verdict after 24 and 50 min (MATCH-NM-5441F0-4C4A50), and
  `timeout` (240 s) in the HEAD-tool pass below. Nothing admitted: NOT-PROVEN. The refusal is a named rule (the
  model does not cover `rep movs` with a symbolic count), not the new budget.

## 1. Where the time went

Sampler `verify/match_20261113/sample_equiv.py` (a thread reads the main thread's stack every 20 s; output
`sampler_005441f0_before.txt`). All 4 samples (20 to 80 s) are in the same place:

```
_check_once -> Explorer.run (equiv.py line 2628, region of the original side) -> run_region line 1995:
    reg.texts.append("call " + show(tgt))
then 51..64 nested frames of show / T.__repr__ / <genexpr>
```

`show()` caps depth at 6, but a term argument that is not a term (a tuple, e.g. a phi's location key or a call's
register list) goes through `repr(t)`, and `repr` of a tuple renders each term inside it through `T.__repr__`,
which calls `show(self)` at **depth 0** again. On a hash-consed DAG (shared subterms) the work is exponential in the
tuple nesting. The proof itself was small: 897 terms and 1035 instructions for the whole function. A `--trace` run
(faulthandler dumps every 45 s) shows the same stack under `run_region` line 1995, and the trace stops after 14 pair
visits at `ours +0xf5: jle 0x2a0` while the call text of the next region is being rendered.

## 2. The change (fea5b5483)

- `show()` carries a **node budget** per outermost call, `--show-budget` (default 100000). Past it a term renders as
  `<#id>`, its hash-cons id. Terms rendered under the budget give the same text as before (checked: every verdict
  line and every detail text of the 1113-row pass below is unchanged except the 4 timeout rows).
  Why this cannot turn a NOT-PROVEN into EQUIV wrongly: rendered text is compared in one place, `x87_rule` (barrier
  and instruction texts of a region with x87 code). Two texts equal under the budget have the same rendered prefix
  and the same ids where truncated, and equal ids mean the same term, so the HEAD tool would render them equal too.
  The budgeted comparison is at least as strict as the unbounded one. Call events are compared by term identity
  before that, as before.
- The explorer stops with **`NOT-PROVEN budget`** past `--max-steps` (default 250000 instructions executed
  symbolically per proof run) or `--max-terms` (default 1000000 distinct terms per function). The detail names the
  side and instruction, the region pair last entered, pair visits and term count, e.g. from the self-test:
  `step budget 3 (--max-steps) reached at orig +0x6, region pair +0x0 / +0x0, 1 pair visits, 20 terms`.
  Defaults: the 41 admitted rows use at most 3792 steps (0x00424270) and 2670 terms (`--stats`, 2026-11-13), so
  250000 steps is ~66x headroom. At ~1 ms per step (0x004d6200: 3418 steps in 3 s wall including the compile) the cap ends a run in about 4 min.
- `--stats` prints steps and terms per function to stderr. `Explorer.MAX_VISITS` (4000 pair visits) is unchanged.
- `equiv_rules.md` section 1 documents `NOT-PROVEN budget` and the show budget, section 6 the new tests.

## 3. Checks

### 3.1 Self-test: 66/66

`test-slot py -3.12 re/tools/match/test_equiv.py` on fea5b5483 defaults: **66/66 passed**
(`selftest_EQUIV-5441F0-TIMEOUT.txt`). 63 existing items unchanged, 3 new:
`0x005441f0` (frozen copy `re/tools/match/equiv_tests/refs/005441f0_held.c`, must be NOT-PROVEN: got `unsupported`),
`show_budget` (64 levels of a doubled tuple render under the budget, `(eax_in+0x4)` renders as before),
`step_budget` (`reg_call` with `STEP_BUDGET=3` gives `NOT-PROVEN budget` with its place).

### 3.2 Every ADMITTED.tsv entry, HEAD tool against capped tool: 41/41 identical

`verify/match_20261113/admitted_ab.py` runs both tools in default mode (no `--relax-icall`, no `--x87-strict`) on each
row's source and RVA and compares the **whole verdict line** (verdict, classes, pop solutions, halts, callers).
Rows: the 40 of `re/match/ADMITTED.tsv` at e47e10795, plus `0x004c4a50` from `orch/MATCH-NM-5441F0-4C4A50` @ d7910949b
(its source `re/match/s19/004c4a50.c` is identical between that branch and this one, `git diff` empty).
Old tool = `git show b720231cb` (frozen copy, removed after the run). Output `admitted_ab.tsv`, log `admitted_ab.txt`.
Result: **41/41 identical, and every new verdict equals the recorded ADMITTED.tsv verdict. No cap flips any row.**

| rva | recorded (ADMITTED.tsv) | old: HEAD equiv.py b720231cb | new: fea5b5483 | identical |
|---|---|---|---|---|
| 0x00407be0 | EQUIV | EQUIV [ENC,LAYOUT,VALUE-FLOW] | EQUIV [ENC,LAYOUT,VALUE-FLOW] | yes |
| 0x0040b290 | EQUIV | EQUIV [VALUE-FLOW] | EQUIV [VALUE-FLOW] | yes |
| 0x00417180 | EQUIV | EQUIV [ORDER,FRAME,VALUE-FLOW] | EQUIV [ORDER,FRAME,VALUE-FLOW] | yes |
| 0x00417450 | EQUIV | EQUIV [ENC,LAYOUT,VALUE-FLOW] | EQUIV [ENC,LAYOUT,VALUE-FLOW] | yes |
| 0x0041f770 | EQUIV | EQUIV [ENC,LAYOUT] | EQUIV [ENC,LAYOUT] | yes |
| 0x00429310 | EQUIV | EQUIV [VALUE-FLOW] | EQUIV [VALUE-FLOW] | yes |
| 0x00429bd0 | EQUIV | EQUIV [ORDER] | EQUIV [ORDER] | yes |
| 0x0042aa00 | EQUIV | EQUIV [ENC,ORDER,LAYOUT,FRAME,VALUE-FLOW] | EQUIV [ENC,ORDER,LAYOUT,FRAME,VALUE-FLOW] | yes |
| 0x004332a0 | EQUIV | EQUIV [LAYOUT,VALUE-FLOW] | EQUIV [LAYOUT,VALUE-FLOW] | yes |
| 0x0045c480 | EQUIV | EQUIV [VALUE-FLOW] | EQUIV [VALUE-FLOW] | yes |
| 0x00475a00 | EQUIV | EQUIV [SIB] | EQUIV [SIB] | yes |
| 0x0047cea0 | EQUIV | EQUIV [SIB,VALUE-FLOW] | EQUIV [SIB,VALUE-FLOW] | yes |
| 0x00484de0 | EQUIV | EQUIV [ORDER] | EQUIV [ORDER] | yes |
| 0x00485b30 | EQUIV | EQUIV [ENC,LAYOUT,FRAME,VALUE-FLOW] | EQUIV [ENC,LAYOUT,FRAME,VALUE-FLOW] | yes |
| 0x0048a4e0 | EQUIV | EQUIV [VALUE-FLOW] | EQUIV [VALUE-FLOW] | yes |
| 0x004938e0 | EQUIV | EQUIV [ENC,VALUE-FLOW] | EQUIV [ENC,VALUE-FLOW] | yes |
| 0x00498ea0 | EQUIV | EQUIV [SIB,ORDER] | EQUIV [SIB,ORDER] | yes |
| 0x004b61c0 | EQUIV | EQUIV [ENC] | EQUIV [ENC] | yes |
| 0x004c18c0 | EQUIV | EQUIV [BYTE-IDENTICAL] | EQUIV [BYTE-IDENTICAL] | yes |
| 0x004d4f00 | EQUIV | EQUIV [BYTE-IDENTICAL] | EQUIV [BYTE-IDENTICAL] | yes |
| 0x00555360 | EQUIV | EQUIV [BYTE-IDENTICAL] | EQUIV [BYTE-IDENTICAL] | yes |
| 0x005a6b70 | EQUIV | EQUIV [ORDER,LAYOUT,VALUE-FLOW] | EQUIV [ORDER,LAYOUT,VALUE-FLOW] | yes |
| 0x005a8890 | EQUIV | EQUIV [REG,LAYOUT,VALUE-FLOW] | EQUIV [REG,LAYOUT,VALUE-FLOW] | yes |
| 0x005aa7a0 | EQUIV | EQUIV [LAYOUT] | EQUIV [LAYOUT] | yes |
| 0x005aaac0 | EQUIV | EQUIV [BYTE-IDENTICAL] | EQUIV [BYTE-IDENTICAL] | yes |
| 0x005aab00 | EQUIV | EQUIV [VALUE-FLOW] | EQUIV [VALUE-FLOW] | yes |
| 0x005ab410 | EQUIV | EQUIV [LAYOUT,VALUE-FLOW] | EQUIV [LAYOUT,VALUE-FLOW] | yes |
| 0x005ace70 | EQUIV | EQUIV [BYTE-IDENTICAL] | EQUIV [BYTE-IDENTICAL] | yes |
| 0x005ad320 | EQUIV | EQUIV [FRAME,VALUE-FLOW] | EQUIV [FRAME,VALUE-FLOW] | yes |
| 0x005ae3a0 | EQUIV | EQUIV [VALUE-FLOW] | EQUIV [VALUE-FLOW] | yes |
| 0x005ae920 | EQUIV | EQUIV [SIB] | EQUIV [SIB] | yes |
| 0x005bf8a0 | EQUIV | EQUIV [ORDER] | EQUIV [ORDER] | yes |
| 0x005be9f0 | EQUIV | EQUIV [BYTE-IDENTICAL] | EQUIV [BYTE-IDENTICAL] | yes |
| 0x00424270 | EQUIV | EQUIV [VALUE-FLOW] | EQUIV [VALUE-FLOW] | yes |
| 0x0052e5c0 | EQUIV | EQUIV [SIB] | EQUIV [SIB] | yes |
| 0x00533900 | EQUIV | EQUIV [SIB] | EQUIV [SIB] | yes |
| 0x00544100 | EQUIV | EQUIV [ORDER] | EQUIV [ORDER] | yes |
| 0x005516c0 | EQUIV | EQUIV [REG,LAYOUT,FRAME,VALUE-FLOW] | EQUIV [REG,LAYOUT,FRAME,VALUE-FLOW] | yes |
| 0x00493640 | EQUIV | EQUIV [ENC,LAYOUT] | EQUIV [ENC,LAYOUT] | yes |
| 0x00536e90 | EQUIV | EQUIV [ORDER,FRAME] | EQUIV [ORDER,FRAME] | yes |
| 0x004c4a50 | EQUIV | EQUIV [LAYOUT,VALUE-FLOW] | EQUIV [LAYOUT,VALUE-FLOW] | yes |

### 3.3 Cross-check set, 1113 rows: no existing verdict changed

`equiv_measure.py --rows-from verify/match_20261113/xcheck_rows.tsv --jobs 4` with `--tool` = a frozen copy of each
tool (240 s per source file). Rows: the 1106 of `verify/equivcheck_20261112/measure_final.tsv`, plus the ADMITTED rows
not in it, plus 0x005441f0 (1113 rows, 1103 files).

| tool | EQUIV | EQUIV-IF-CALLERS-IGNORE | NOT-PROVEN |
|---|---|---|---|
| HEAD b720231cb (`xcheck_measure_head.tsv`, sha256 ae1918a8ff1a6701b0a7864377893403fc6bd88fe579847a780cf27a249a0dc5) | 226 | 35 | 852 |
| fea5b5483 (`xcheck_measure_new.tsv`, sha256 a98f324db4bfca9898e6ee3d4f42ab9bb8f11320dddda6a6d209d3797f964188) | 229 | 35 | 849 |

Row diff: verdict or kind changed on **4 rows, all `NOT-PROVEN timeout` (240 s) under the HEAD tool**. Classes or rules
changed on 0 rows, detail text on 0 rows. No row with a verdict under the HEAD tool changed. No row hit `budget`.

| rva | source | HEAD | fea5b5483 | steps / terms / wall (`changed4_stats.txt`) |
|---|---|---|---|---|
| 0x00420050 | re/match/s05/00420050.c | NOT-PROVEN timeout | EQUIV [ORDER] | 850 / 1310 / 6 s |
| 0x004d5bc0 | re/match/s21/004d5bc0.c | NOT-PROVEN timeout | EQUIV [BYTE-IDENTICAL] | 2792 / 8389 / 3 s |
| 0x004d6200 | re/match/s21/004d6200.c | NOT-PROVEN timeout | EQUIV [BYTE-IDENTICAL] | 3418 / 12319 / 3 s |
| 0x005441f0 | re/match/rwtb/005441f0.c | NOT-PROVEN timeout | NOT-PROVEN unsupported (rep movsd, variable count) | 1035 / 897 / 3 s |

Note for the lead: the three new EQUIV rows are new verdicts, not admissions. I did not touch ADMITTED.tsv. Before
admitting any of them, a row needs the usual cross-check (`equiv_dyn.py`, default and `--seeded`). 0x00420050 is
EQUIV `[ORDER]` and was `NOT-PROVEN x87-region` in measure_final (older tool), so check that its ORDER is not x87
instruction order (never admitted, L-126). 0x004d5bc0 and 0x004d6200 are byte-identical now and may already be C4 by
match through their own rows.

## 4. Files

- Tool: `re/tools/match/equiv.py`, `re/tools/match/equiv_rules.md`, `re/tools/match/test_equiv.py`,
  `re/tools/match/equiv_tests/refs/005441f0_held.c` (commit fea5b5483).
- Evidence (this directory): `admitted_ab.py`, `admitted_ab.tsv`, `admitted_ab.txt`, `xcheck_rows.tsv`,
  `xcheck_measure_head.tsv`, `xcheck_measure_new.tsv`, `changed4_stats.txt`, `selftest_EQUIV-5441F0-TIMEOUT.txt`,
  `sample_equiv.py`, `sampler_005441f0_before.txt`.
- Run hygiene: I stopped only processes I started (by PID, filtered by my worktree path / scratchpad file names).
