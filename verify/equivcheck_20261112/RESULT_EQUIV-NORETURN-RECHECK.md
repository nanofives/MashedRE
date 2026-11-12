# RESULT EQUIV-NORETURN-RECHECK (2026-11-12): no-return paths reported with callee and reason, 1,106 rows re-run, 0x00499400 still NOT-PROVEN

L2 worker `cmwddc6as5n8ksc1b6euo4lfl` for lead `cmwddbai85n02sc1bwkkpmtb7`. Branch `orch/EQUIV-NORETURN-RECHECK` from
`orch/integration` `0e277caaf`. TOOL row: tool, tests, one PROVISIONAL.tsv cell and this RESULT. No tracker edit, no
C-level change, nothing under `mashedmod/`, no build, no game run, no gates.

## Verdict

**GREEN.**

- **(a) Done.** Every path that ends at a no-return callee or a trap is listed in a new `halts` field. Each entry gives
  the side, the call site, the callee and the reason.
- **(b) Done.** The 1,106 rows of `verify/equivcheck_20261111/measure_v4.tsv` were re-run in default mode (`--relax-icall`
  off). My changes move **0 verdicts**. All 33 `ADMITTED.tsv` rows keep their verdicts, so there is no L-126
  withdrawal.
- **(c) Not proven.** `0x00499400` is still NOT-PROVEN, now one call later and on EDI only. The cause is missing CRT
  callee summaries, not our source. It stays `provisional` in `PROVISIONAL.tsv`.
- **(d) Done.** The `or r,-1` false live-in is fixed (`and r,0` too). I also fixed a second false live-in source: frame
  alignment.

## 1. (a) No-return reporting (`re/tools/match/equiv.py`)

- **What the summary records.** `summary()` now gives `nr_why` for a no-return callee: each path end of that callee
  (`call <callee> @<pc> (<its nr_why>)`, `jmp ...`, or `int3/hlt/ud2 @<pc>`), with nested reasons inline.
  It keeps the first 4 ends and caps the text at 400 characters.
- **What the proof records.** Each path the pair walk ends at a halt adds one entry to `halts`:
  - for a no-return call: `<side> <site> call <callee>: no-return, <nr_why>`;
  - for a trap: `<side> <site> <mnemonic>: trap`.
- **Where it shows.** The `no-return` rule tag stays as it was. The text output appends
  `unchecked after no-return: ...`, and `equiv_measure.py` writes `halts` as a new last TSV column.
- **Demo (`demo_halts.txt`).** I forced the pre-PROV-DIVERGED-18 summary of acos `0x004a3384` (no-return):
  - `0x00408b00` reports `orig 0x00408bcf call 0x4a3384: no-return, ...`;
  - the old source of `0x00448220` reports `orig 0x00448475 call 0x408b00: no-return, call 0x004a3384 @0x00408bcf (...)`.
  - That is the defect PROV-DIVERGED-18 found by replay. It is now on the verdict line.
- **Same family, new fallback (`r12`).** Before, a summary whose only path ends were tail jumps to a callee with no
  summary reached no `ret`, so it came out no-return. It now gives an unknown pop count.
  - Example: `0x004a460d` tail-jumps at `0x004a465e` to `0x004a466a`, which has no Ghidra size (`r2`).
  - Rows whose NOT-PROVEN kind moved to it: `0x004a7796` (callee `0x4a460d`) and `0x005b2190` (callee `0x5ab190`).
  - Neither row's verdict changes.

## 2. (b) The 1,106 rows, default mode

Each run uses the same rows and sources (`equiv_measure.py --rows-from verify/equivcheck_20261111/measure_v4.tsv`,
new option), the sources at this branch, and a frozen copy of the tool (SHA-256 below):

| Run | Tool | EQUIV | IF-CALLERS-IGNORE | NOT-PROVEN |
|---|---|---|---|---|
| v4 (`equivcheck_20261111`) | `be32d7f80` | 133 | 61 | 912 |
| prefix | `0e635d0c6` (EQUIV-CALLEE-POPS, before the acos fix) | 202 | 38 | 866 |
| acos | `3e5ad2b98` (PROV-DIVERGED-18, today's `orch/integration`) | 182 | 38 | 886 |
| new | this row, (a) + `r12` + `or/and` idiom | 182 | 38 | 886 |
| final | this row, plus frame alignment and A7 | 182 | 38 | 886 |

Every changed row is listed in `compare_all.txt`.

- **v4 to prefix: 69 changes.** None is from this row. They are EQUIV-CALLEE-POPS's A6 pop counts plus source
  edits since v4:
  - 46 NOT-PROVEN rows became EQUIV;
  - 23 IF-CALLERS-IGNORE rows became EQUIV.
- **prefix to acos: 20 changes**, all EQUIV to NOT-PROVEN `callee-pop-unknown`. This is the PROV-DIVERGED-18 fix,
  which is already on `orch/integration`. It is listed here because no full re-run was done after it.
  - **Through acos `0x004a3384`:** `0x00408b00`, `0x00415e20`, `0x00455150`, plus `0x00448220`, whose new kind is
    `call-memory` (its swap, now in view).
  - **Through `0x00517250` / `0x005172b0` (16 rows):** `0x0051cfc0`, `0x00523170`, `0x005238c0`, `0x00523cd0`,
    `0x00523e40`, `0x005241f0`, `0x005245b0`, `0x00524f70`, `0x005253c0`, `0x00525870`, `0x00526310`, `0x00526630`,
    `0x00526910`, `0x00526bf0`, `0x00527560`, `0x00528810`.
    - Both callees end with `call 0x5c1ea0` then `pop esi` and run into the next table entry (`0x00517270` /
      `0x005172e6`..`0x005172f0`).
    - `0x5c1ea0` reads `fs:[0]` and the buffer at `[esp+4]`. Its own summary is unknown (`r9`).
    - The old tool called both callees no-return only because of the fall-through.
    - Not admitted. None of the 20 rows is in `ADMITTED.tsv`.
  - The 3 acos rows other than `0x00408b00` / `0x00448220` (`0x00415e20`, `0x00455150`) were never admitted either.
- **acos to new: 0 verdict changes**, 3 NOT-PROVEN kind changes:
  - `0x00445aa0`: `call-regargs` to `x87-region` (the `or/and` idiom);
  - `0x004a7796` and `0x005b2190`: to `callee-pop-unknown` (`r12`).
- **new to final: 0 verdict changes**, 7 NOT-PROVEN kind changes (the proof now gets further in each):
  - `0x00422ba0` and `0x004427c0`: `branch-condition` to `call-memory`;
  - `0x00426810`, `0x0045d7a0` and `0x00448220`: to `x87-region`;
  - `0x004b55a0` (`call-target`) and `0x00551840` (`branch-condition`): to `call-regargs`;
  - plus `0x00499400`, still `call-regargs` but one call later (section 3).
- **No proven row in the final run has a `halts` entry.** Every EQUIV / IF verdict compares every path to a return.
- **ADMITTED.tsv, all 33 rows, final tool.** 32 EQUIV and `0x00555360` EQUIV-IF-CALLERS-IGNORE(EAX), as admitted.
  No withdrawal.
- **No dynamic cross-check was run.** The final tool proves no new row, so there is no new proof to cross-check.
- **Tests.** `py -3.12 -m pytest re/tools/match/test_equiv.py`: 3 passed. They are `test_cases`, `test_references`
  and the new `test_summary_precision`, which covers r11, frame alignment, the constant idioms and A7.

## 3. (c) `0x00499400`

- **The pickaxe.** `git log -S 0x00499400 -- re/match/ADMITTED.tsv re/match/PROVISIONAL.tsv` gives one commit only:
  `34647278b` (PROVISIONAL-BODIES), which added the `PROVISIONAL.tsv` row. The row was never in `ADMITTED.tsv`.
  RESULT_PROV-DIVERGED-18 section 5 was wrong to say it was.
- **The first difference has two causes:** `call 0x498a00` at `0x00499469` and its EBX / EDI register arguments. Both
  are tool imprecision.
  1. **False live-in EDI (and ESI) in `0x00498a00`.**
     - The prologue is `push ebp; mov ebp,esp; and esp,0xfffffff8; sub esp,0x88; ...; push esi; push edi`.
     - `and esp` set the summary's ESP to unknown, so the two saves counted as reads.
     - Fix: under a frame pointer, `and esp, imm` moves ESP to a virtual base. A `ret` with ESP not at entry is still
       `r7`. Documented in `equiv_rules.md` section 4.
     - Result: `summary(0x00498a00)` live-in is {EBX, ESI}, not {EBX, ESI, EDI}.
  2. **The EBX term.** EBX is the `Mode*` built from `DAT_00773408`.
     - Ours loads `DAT_00773408` after the first store of `char line[64] = ""`. The original loads it after more of
       the fill (`0x00499470`..).
     - `line` escapes to `0x00498a00`, so the frame is ordinary memory (A1'). The tool had no rule saying that a
       frame address cannot equal an image address, so the two loads were different terms.
     - New assumption **A7** (`A7-stack-image`): `esp0 + k` never overlaps a constant whose whole access lies inside
       the mapped image. Reported in `rules` when used. Documented in `equiv_rules.md` section 4.
- **What is left.**
  - The new first difference is `NOT-PROVEN call-regargs: region ours +0x69 / orig 0x00499469 call 0x496400`, on EDI
    only: ours 0, original `esp0-0x4`. EBX, ESI and EBP are equal.
  - EDI counts as a register argument of `0x496400` only because its CRT callees have no summary:
    - `0x004a42c5` -> `0x004a504f` (`r10`);
    - `0x004a4254` (`r7`).
    - With no summary, every register counts as read.
  - This cannot be closed soundly in `summary()` without modelling the CRT print / lock / SEH helpers. That is out of
    this row's scope. It would be a library-facts rule, and since it widens what admits, it would need a decision row.
- **Status.**
  - `PROVISIONAL.tsv` row `0x00499400`: the `equiv_first_difference` cell now holds this difference, and the evidence
    cell points here.
  - The status stays `provisional`. It is not admitted.

## 4. (d) `or r,-1` false live-ins

- `const_idiom()`: `or r, -1` and `and r, 0` (for any operand size) write a constant result with constant flags.
  They now count as writes, not reads.
- It applies in `summary()` and in the caller scan (`caller_reads`).
- The cited case: `0x0040e187` / `0x0040e18f` gave EDI / EBP live-ins for `0x00441c80`. `0x00441c80` now checks EQUIV
  (BYTE-IDENTICAL). It is not one of the 1,106 rows.
- Over the 1,106 rows the effect is 1 NOT-PROVEN kind change (`0x00445aa0`), with 0 verdict changes.

## 5. Not covered here

- **Possible follow-up row.** `ret_pop()` and the summary's pop set ignore a path that tail-jumps to an unknown
  callee while other paths reach a `ret`. The pop count then comes from the `ret`s alone.
  - This is the pop-count analogue of `r12`. I did not change it here: it is not a no-return case.
  - A follow-up row could make such summaries unknown and re-run the 1,106 rows.
- **The 16 `0x00517250` / `0x005172b0` rows** prove only if `0x5c1ea0` is shown to be no-return. That needs the same
  kind of library fact as above.

## Evidence (this folder)

- `RESULT_EQUIV-NORETURN-RECHECK.md` (this file).
- `measure_final.tsv`: the final tool over the 1,106 rows. It adds the `halts` column.
- `compare_all.txt`: `py -3.12 verify/equivcheck_20261112/compare.py v4=... prefix=... acos=... new=... final=...
  --admitted re/match/ADMITTED.tsv`.
- `compare.py`: the verdict diff.
- `demo_halts.txt`: the (a) demo.
- Tool changes: `re/tools/match/equiv.py`, `equiv_measure.py` (`--rows-from`, the `halts` column), `equiv_rules.md`,
  `test_equiv.py`.

Raw files in `D:/MashedWT/EQUIV-NORETURN-RECHECK_data/`, with SHA-256:

- `measure_prefix.tsv` e51279904c5cafb2e8d80afc7090c35d24d834c066ace25fa107d517078bfcdf
- `measure_acos.tsv` 7302bbeb6fee9aabb1596efea0b60348ab8bfb609414d522237198adeffcdac2
- `measure_new.tsv` acc2795dd35a7028c463a36b7200daef384e2076884961b4e6cfd6e01c09f49e
- `measure_final.tsv` efb139f43ae294f5e9a83c4d7c6543bddf4dbfa7f820f6025b8cb749069eeb0e (same bytes as the committed copy)
- Frozen tools:
  - `equiv_prefix.py` 1ff87b92df0c01016493c64ed5dcf531425783385e11ee8fc6773c0faadf79a9 (`0e635d0c6`)
  - `equiv_acos.py` 6480f318320849d9eef2e34dd137794f6c80f43490f5d03457550258245b885e (`3e5ad2b98`)
  - `equiv_new.py` 47856e44988f10079c7bcf227d03d0c03a8d8590e877db2bc3d47a5326b36875
  - `equiv_final.py` ab53dd6e3d01d6b455f8c65c2833c2b470471de85e2cae693c1fbd90835a1885 (= the committed `equiv.py`)
- `run_*.log`: one `[N/1096]` line per source file.
