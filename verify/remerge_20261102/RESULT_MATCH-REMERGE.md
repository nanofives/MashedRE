# RESULT MATCH-REMERGE (2026-11-02)

Verdict: GREEN. All four branches are re-merged onto `orch/integration` 419a9abe9. Each builds with
`MASHED_NO_DEPLOY=1`, passes `lint_rva_bodies.py` (0 NEW) and `archive_lint.py --ref HEAD`, and gives the
`merge_train.json` default gate refs (G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3). The knob-ON gates
that each PORTSTATE RESULT claimed hold too.

History is not rewritten. Each branch gets new commits on top of its old head: a merge of integration (S08,
COURSE), or a merge of the re-merged parent (RECORDS on COURSE, GAMEFLOW on RECORDS). The stack order stays
COURSE -> RECORDS -> GAMEFLOW.

## Branches

| Row | Old head | Gated sha | Commits added | exe sha8 | asi sha8 |
|---|---|---|---|---|---|
| MATCH-S08 | 56a88ebf4 | 3e48a2d09 | 7a4105d1f merge integration, 3e48a2d09 link fix | d029d1da | a3ec9c40 |
| MATCH-PORTSTATE-COURSE | 622335487 | 213952116 | 213952116 merge integration | 8722d9b9 | 4cfd9491 |
| MATCH-PORTSTATE-RECORDS | d5a57c978 | 152c18869 | 152c18869 merge COURSE 213952116 | fcf46454 | cc27cc4c |
| MATCH-PORTSTATE-GAMEFLOW | 0ff69edec | d86dac98c | d86dac98c merge RECORDS 152c18869 | 53420a7f | 6c4f734d |

This RESULT and `raw_sha256.txt` are added afterwards as text-only commits (COURSE, then merged forward into
RECORDS and GAMEFLOW, and the same two files on S08). They change no build input. The final shas are in the
lead's REPORT.

## Conflict resolutions

1. **S08, `mashedmod/src/mashed_re/Util/UtilMid.cpp`** (two hunks in one anonymous namespace). Integration
   (MATCH-S06) moved the body of 0x00429aa0 to `Match/MatchS06_00429aa0.cpp`, which left `X87FtolLoad` /
   `X87FloatCopy` with no caller. S08 swapped out the body that used `X87TimerElapsedFtol` /
   `X87TimerScaledFtol` (the 0x0043c137 / 0x0043c15f sequences). Each side removed its own users and kept the
   other side's helpers. After the merge, nothing in the tree calls any of the four (grep over `mashedmod/src`).
   Resolution: delete the whole helper block (header comment through `} // namespace`). This keeps both
   sides' intent.
2. **S08, link error after the merge (semantic conflict, no text conflict).** `Match/MatchS08_00432ad0.cpp`
   declared `extern "C" void __fastcall MenuIm2DQuad(int)`. MATCH-S06 replaced that body with the
   byte-matched `re/match/s06/0042aae0.c`: cdecl `void(void)`, exported as `_MenuIm2DQuad`, and the old
   `@MenuIm2DQuad@4` alias was dropped. The exe link failed (`LNK2019 @MenuIm2DQuad@4`). Fix (3e48a2d09): the
   forwarder `MatchS08_00432ad0_Call0042aae0(int p0)` keeps the one-int signature that the matched caller
   source declares (`re/match/s08/00432ad0.c:30`, `FUN_0042aae0(0)` at line 50, cdecl push 0 / add esp,4). It
   now calls `MenuIm2DQuad()` as cdecl `void(void)`. The S06 note says the matched callee reads no argument,
   and a cdecl caller pops its own pushed 0, so the call is unchanged. No matched source changed.
3. **COURSE, `verify/savelink_20261023/run_arms.py`.** Both sides only added lines. Integration added
   `ST5L2` / `ST5L2D` / `RD1-3` / `ST5L2RD1-3`, and COURSE added `PSC`. Both are kept.
4. **RECORDS and GAMEFLOW:** both forward merges were clean.

## Gates

Runner: `verify/savelink_20261023/run_arms.py`, with `A4_EXE` = each worktree's
`mashedmod/build/mashed_re.exe`, `A4_ROOT` = `MASHED_ROOT` = `D:\MashedWT\MATCH-REMERGE_root` (a robocopy of
`original\`), and `A4_OUT` = `D:\MashedWT\MATCH-REMERGE_data\<set>`. Each set ran under
`mashed_lock.py run --label MATCH-REMERGE-<set>`. Every arm ran once: no reruns and no crashes.

| Set | Arms | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | Ref |
|---|---|---|---|---|
| S08 | H_DEF B_DEF_K0/K1/K26 | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |
| COURSE default | H_DEF B_DEF_* | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |
| COURSE knob ON | H_PSC B_PSC_* | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal (COURSE P4) |
| RECORDS default | H_DEF B_DEF_* | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |
| RECORDS knob ON | H_PSR B_PSR_* | 8284be46 / **67931834** / 31fe043e | 1d5bf4f9 x3 | RECORDS P4' (gates.csv 67931834 = old g2 value, by construction) |
| GAMEFLOW default | H_DEF B_DEF_* | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |
| GAMEFLOW knob ON | H_PSG B_PSG_* | 8284be46 / **67931834** / 31fe043e | 1d5bf4f9 x3 | GAMEFLOW P3 (same values) |

`verify/portstaterecords_20261101/gates_mask.py H_DEF.gates.csv H_P*.gates.csv` gives this on RECORDS
(H_PSR) and on GAMEFLOW (H_PSG): P3 off 0 nonzero abs rows, P3 on abs != rec on 0 of 54,084 rows, P4 masked
differing cells 0, rc 0.

Not re-run here: COURSE P2/P3 (FNV witness logs), RECORDS P2 witness, GAMEFLOW P2/P4/P5 (witness lines,
H_PSGO, the Arctic pair). This pass re-checks gate equality and the gates.csv mask only. No full set
(D2 / 13 tracks) ran. The lead's note asks for "SOURCE -Full", which the merge train runs.

## Raw data

The raw arm files stay in `D:/MashedWT/MATCH-REMERGE_data/{S08,COURSE,COURSE_ON,RECORDS,GAMEFLOW}/`. SHA-256
for each of the 42 cited step/gates/ptrace files: `raw_sha256.txt` (this folder). Build logs:
`D:/MashedWT/remerge_{s08,course,records,gameflow}_build.log`.
