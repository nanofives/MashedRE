# RESULT DDKOBJ-UNLISTED-10 (run 47, 2026-11-14)

Verdict: **GREEN** (1 admitted, 9 provisional, 1 decision candidate already ruled by L0 and open as L-161; both builds OK, ddkobj check DIFFERENT 19 -> 12, IDENTICAL unchanged). Base `orch/integration` @ 198109d50. equiv.py = the merged one on `orch/integration`
(last commit `fea5b5483`, run from `.worktrees/orch-integration/re/tools/match/equiv.py`; this branch's copy is the
same file). Source: `verify/ddkobj_20261114/RESULT_DDKOBJ-DIFFERENT-REASONS.md` (branch orch/DDKOBJ-DIFFERENT-REASONS).

Freeze check (L-148): none of the 10 sources nor the three PortTop wrappers (`PortTop_004025f0.cpp`,
`PortTop_0045a6b0.cpp`, `PortTop_00498860.cpp`) contains `kCdRows`, `kTrackTable`, `kSaveBufBase` or `Tables().body`
(grep, 0 hits). No unit skipped. Not touched: DDKOBJ-GFFU-DUP-4929D0, DDKOBJ-PROV-MIXED-UNIT, L-140, L-161,
0x00493900 / 0x00493710.

## 1. Verdicts (`py -3.12 equiv.py --only <rva> <src>`, full lines in `unlisted10/equiv_merged_fea5b5483.txt`)

| rva | source (flags) | match.py | equiv.py verdict (first difference) | decision | file |
|---|---|---|---|---|---|
| 0x00401690 | s01/00401690.c (/O2 /GS /Ob1) | 60.2% | NOT-PROVEN branch-condition, ours +0xa / orig 0x0040169a | PROVISIONAL | PROVISIONAL.tsv, BINDINGS.tsv |
| 0x0045a3a0 | s09/0045a3a0.c (/O2 /GS /Ob1) | 81.0% | EQUIV-IF-CALLERS-IGNORE(EAX) [ENC,FRAME,VALUE-FLOW], callers 3: 0x0045a741 ignores, 0x0045a7d9 ignores, 0x0045ac2a unknown | PROVISIONAL + decision candidate (L0 ruling run 47; L-161 open) | PROVISIONAL.tsv, BINDINGS.tsv |
| 0x00498810 | s16/00498810.c (/O2 /GS /Ob1) | 86.7% | NOT-PROVEN call-regargs, ours +0x21 / orig 0x00498831 call 0x4d52d0 | PROVISIONAL | PROVISIONAL.tsv, BINDINGS.tsv |
| 0x00552890 | rwl04/00552890.c (/O2 /Ob1 /G5) | 91.2% | NOT-PROVEN call-memory, ours +0xc / orig 0x0055289c call 0x4c3dc0 | PROVISIONAL | PROVISIONAL.tsv |
| 0x005530f0 | rwl04/005530f0.c (/O2 /Ob1 /G5) | 67.8% | NOT-PROVEN call-args, ours +0x0 / orig 0x005530f0 call 0x4c3b30 arg 0 | PROVISIONAL | PROVISIONAL.tsv |
| 0x005533d0 | rwl04/005533d0.c (/O2 /Ob1 /G5) | 55.0% | NOT-PROVEN x87-region, x87 instruction 3: fld m4[esp+0x20] vs fld m4[esp+0x1c] | PROVISIONAL (x87 order, never admitted) | PROVISIONAL.tsv |
| 0x005538a0 | rwl04/005538a0.c (/O2 /Ob1 /G5) | 19.7% | NOT-PROVEN branch-condition, ours +0x0 / orig 0x005538a0 | PROVISIONAL | PROVISIONAL.tsv |
| 0x00554200 | s24/00554200.c (/O2 /Ob1 /G5, tc rw9044) | 99.3% | **EQUIV [ORDER]**, rules A6-paired-pop, A6-pinned-pop, A7-stack-image, jmp-threading | **ADMITTED** | ADMITTED.tsv |
| 0x00554940 | s24/00554940.c (/O2 /Ob1 /G5) | 12.5% | NOT-PROVEN x87-region, ours +0x3a / orig 0x0055497f: fld m4[ebx+0x8] vs fld m4[ebp+0x8] | PROVISIONAL (x87 order) | PROVISIONAL.tsv |
| 0x005572c0 | s24/005572c0.c (/O2 /Ob1 /G5) | 17.9% | NOT-PROVEN call-regargs, ours +0x40 / orig 0x00557309 call 0x553590 | PROVISIONAL | PROVISIONAL.tsv |

Tally: ADMITTED 1 (plain EQUIV), PROVISIONAL 9 (8 NOT-PROVEN + 1 conditional with an unknown caller). No proof used
`--relax-icall`. No conditional verdict and no x87-order verdict went into ADMITTED.tsv. Match scores from
`match_scores_DIFFERENT_REASONS.txt` (DDKOBJ-DIFFERENT-REASONS). Admitted / provisional is never C4 (L-126, L-128).

## 2. The admission: 0x00554200

- Verdict: plain EQUIV, classes ORDER. `unlisted10/equiv_00554200.json`: `bytes_bad` 6, `mode` strict,
  `local_callees` [`_FUN_00553e80`].
- ORDER is not x87: the original body at 0x00554200 holds no x87 instruction (capstone over 400 bytes, 144 instructions,
  0 `f*` mnemonics), and no x87 rule (x87-barrier, x87-motion) fired. The source header (s24/00554200.c, MATCH-RT2D-RESID)
  names the residual: the store order of the hdr.extra copy and the register renaming of c2 9044.
- Caller scan (not required for plain EQUIV, recorded): rel32 calls at 0x00554311 and 0x0055461e, no absolute dword
  reference (`unlisted10/callers_00554200.txt`).
- Local callee `_FUN_00553e80`: equiv.py EQUIV [BYTE-IDENTICAL] on the same file; hooks.csv 0x00553e80 C4; relocheck OK.
- Cross-check (`equiv_dyn.py`): default `SAME same=32 inc=0 diff=0`; `--seeded` `INCONCLUSIVE same=0 inc=32 diff=0`
  (`unlisted10/dyn_default_00554200.txt`, `dyn_seeded_00554200.txt`). No run differs. The seeded runs are all
  inconclusive; precedent 0x00536e90 was admitted with inc=45 diff=0.
- Row (L0 note, run 47): `orch/AUDIO-STACK-STEP1` already carries an ADMITTED.tsv row for 0x00554200. The ADMITTED
  line on this branch is that line, byte for byte (`diff` of the two lines: identical), so whichever branch merges
  second meets the same line. Column by column, my own runs against theirs:

  | col | AUDIO-STACK-STEP1 | this row's runs | equal inputs |
  |---|---|---|---|
  | source, tool_commit, source_commit | s24/00554200.c, fea5b5483, b7414f7ae | same | yes |
  | flags | `/O2 /Ob1 /G5 (TOOLCHAIN rw9044)` | `/O2 /Ob1 /G5`, `// TOOLCHAIN rw9044` | yes (spelling taken from theirs) |
  | verdict, classes, rules | EQUIV, ORDER, A6-paired-pop,A6-pinned-pop,A7-stack-image,jmp-threading | same | yes |
  | caller_scan | `n/a (plain EQUIV)` | same, plus callers 0x00554311 / 0x0055461e (kept in this RESULT only) | yes |
  | local_callees | `_FUN_00553e80 (match.py --bytes BYTE-IDENTICAL, relocheck OK, BYTECHECK IDENTICAL)` | equiv EQUIV [BYTE-IDENTICAL], relocheck OK | yes |
  | xcheck_default / seeded | `SAME same=32 inc=0 diff=0` / `INCONCLUSIVE same=0 inc=32 diff=0` | same tallies | yes |
  | port_binding, evidence | RwCore/RwCore_00553e80.cpp ..., `verify/audiostack_20261114/RESULT_AUDIO-STACK-STEP1_PART2.md` | the same unit | kept theirs so the line is identical; this RESULT is the second witness |

  Ownership of the row: DDKOBJ-UNLISTED-10 (L0). No other of this row's 9 provisional RVAs appears in
  AUDIO-STACK-STEP1's ADMITTED.tsv, PROVISIONAL.tsv or BINDINGS.tsv diff against 198109d50.

## 3. Decision candidate: 0x0045a3a0 (listed PROVISIONAL meanwhile, L0 ruling)

- Question: may 0x0045a3a0 (EQUIV-IF-CALLERS-IGNORE(EAX)) be admitted, given caller 0x0045ac2a is unknown?
- Evidence (`unlisted10/callers_0045a3a0.txt`, `callers_0045ac20.txt`, on `original/MASHED.exe.unpatched`):
  - 0x0045a741: next is `mov ecx,[0x68d150]`, pushes, `call 0x4892c0` (EAX overwritten by the call) -> ignores.
  - 0x0045a7d9: `mov eax,[esi+0x20]` right after -> ignores.
  - 0x0045ac2a: `add esp,4; cmp esi,0x68bb60; jne 0x45ac26; pop esi; ret`. On the last iteration EAX from 0x0045a3a0
    reaches the `ret` of the function at 0x0045ac20. 0x0045ac20 has no rel32 caller; its only reference is the
    `.data` dword at 0x005f9abc (an indirect call through a table). Whether any indirect caller reads EAX is
    [UNCERTAIN]: the readers of 0x005f9abc and their call sites were not traced.
- Options: (a) trace the indirect callers of the 0x005f9abc slot and admit if every call site ignores EAX (this is L-161,
  open with Mariano); (b) keep it provisional; (c) re-shape the source until equiv gives plain EQUIV.
- Recommendation: (a), resolved under L-161. It stays provisional until then.

## 4. BINDINGS.tsv kinds (`verify/porttop_20261108/BINDINGS.tsv`)

| rva | before | after | why |
|---|---|---|---|
| 0x00401690 | `port` | `near-match` | exe body is the static of re/match/s01/00401690.c, not a hand port; now PROVISIONAL. Precedent: 0x00455400 PROVISIONAL body = `near-match`. exe_symbol adds `___ddkreg_00401690` (object build) |
| 0x0045a3a0 | `near-match` | `near-match` (kept) | consistent: L-115/L-117 near-match, now also listed PROVISIONAL; note adds the verdict |
| 0x00498810 | (no row) | new row `near-match`, caller 0x00498860, level 3 | PROVISIONAL static of Match/PortTop_00498860.cpp |
| 0x00498860 | note "ddkobj refuses the unit, VS2022 body kept" | note corrected | stale: the 9f48df42f BYTECHECK lists `_Orig_00498860` IDENTICAL and `___ddkreg_00498810` DIFFERENT, vs_copy_in_map False, so the unit is linked from DDK objects; 0x00498860 is not in REFUSED_UNITS.tsv |

## 5. Object build effect and checks

`ddkobj.py` main_build (line 751) sets `skip:provisional` when every RVA of a unit is in PROVISIONAL.tsv. Seven
single-RVA units now meet that (from `mashedmod/build/ddkobj/ddkobj_build.json`): `Match/PortTop_0045a6b0.cpp`
[0x0045a3a0] (0x0045a6b0 is its own unit, still IDENTICAL), `RwCore_00552890.cpp`, `RwCore_005530f0.cpp`,
`RwCore_005533d0.cpp`, `RwCore_005538a0.cpp`, `RwCore_00554940.cpp`, `RwCore_005572c0.cpp`. The object build now links
their VS2022 bodies, not the DDK compiles (L-126 (6a)). The units of 0x00401690 (with matched 0x004025f0) and
0x00498810 (with 0x00498860) still build and link the DDK compile of the provisional function: they are mixed units of
the DDKOBJ-PROV-MIXED-UNIT kind (cause B of DDKOBJ-DIFFERENT-REASONS), left as they are, that row owns the rule.
0x00554200 (admitted, with 0x00553e80) still builds: an admitted body runs as its DDK compile.

| build / check | command (from this worktree) | result |
|---|---|---|
| VS2022 | `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` | `=== Build OK ===` (mashed_re.exe, mashed_re_dev.asi, not deployed) |
| object build | `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"` | `=== Build OK ===`; `[ddkobj summary] IDENTICAL 1241/1268 DIFFERENT 12 ... not-in-map 15 units linked 1215/1501 refusals 9 (externalized-static-register-convention=5, wrapper-redefines-globals=4)` |
| `py -3.12 re/tools/ddkobj/ddkobj.py check` | | `{'IDENTICAL': 1241, 'DIFFERENT': 12, 'not-in-map': 15}` |

Against the merged head (run 46 drain 4: IDENTICAL 1241/1275, DIFFERENT 19): IDENTICAL unchanged at 1241, the 7
skipped units leave the check (1275 -> 1268), DIFFERENT 19 -> 12. The 12 left: 0x00401690, 0x00498810 (provisional,
mixed units), 0x00554200 (admitted), 0x004332a0, 0x00475a00 (admitted), 0x0043c5b0, 0x00498510, 0x004c0c20, 0x004c0f00
(provisional-mixed, DDKOBJ-PROV-MIXED-UNIT), 0x004929d0 (DDKOBJ-GFFU-DUP-4929D0), 0x00493710, 0x00493900 (RETARGET
rows). Every one is in ADMITTED.tsv or PROVISIONAL.tsv or owned by an open row. No 100% source DIFFERENT.
Non-IDENTICAL rows committed as `unlisted10/bytecheck_nonidentical.tsv`, check log `unlisted10/ddkobj_check_unlisted10.txt`.
Raw (not committed): `mashedmod/build/ddkobj/BYTECHECK.tsv` sha256
`769782199188215e6adfa850fcc80b17a7efa0f11a22e981412ef622dc5d28a7`, `ddkobj_build.json` sha256
`a2428a2cd020052a5ca8aff3af1ddb2558399e85b58f2870d117951e883fadce`. No game launched, no gates (text/static row with an
object-build effect, no knob boot needed: the default build is unchanged, only the knob-path object build moves).

## 6. Files

Committed: `re/match/ADMITTED.tsv` (+1, the AUDIO-STACK-STEP1 line verbatim), `re/match/PROVISIONAL.tsv` (+9), `verify/porttop_20261108/BINDINGS.tsv`
(2 notes, 1 kind, 1 new row), this RESULT, `verify/ddkobj_20261114/unlisted10/` (equiv lines, equiv json, equiv_dyn
tallies, caller scans, `callers.py`, `append_rows.py`). No hooks.csv edit, no c4_reclass. No game launched.
