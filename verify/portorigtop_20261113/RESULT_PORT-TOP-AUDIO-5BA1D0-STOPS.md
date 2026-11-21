# RESULT PORT-TOP-AUDIO-5BA1D0-STOPS (+ RO-AUDIOINIT-45D460-REACH): audio init arms from 0x005BA1D0 into 0x0045d460

## Verdict: HANDOFF (5BA1D0 stops GREEN; RO reach stops at 0x005C91F0, a BIND-lane RVA)
- **PORT-TOP-AUDIO-5BA1D0-STOPS: GREEN.** With `MASHED_OT_AUDIOTHREAD_STANDIN=1 MASHED_ORIG_TOP_RWMOD=1` both builds pass
  0x005ba1d0 (census 71) and its closure, `0x004627f0` returns (`[OT_AUDIOINIT] 0x004627f0 returned: DAT_00632ba0=0x00000001
  ... DAT_006904a0=28 DAT_0068f4c8=1 DAT_007dcd20=0x007DCCF8`) and 0x0045d460 starts (`0x0045d460 PERMDICT.RWS loader:
  provisional exe body`).
- **RO-AUDIOINIT-45D460-REACH: BLOCKED (ownership), not finished.** Inside 0x0045d460 the arms walk the stops 0x005AB380 ->
  0x005ACAA0 -> 0x005B3A00 -> 0x005BB910 -> **0x005C91F0** (return `_PortTop_Bound_005bb910+0x4f` in both maps). 0x005c91f0 is
  hooks.csv C4 (match) and could bind at once, but it is listed in `log/orch/run53_bindlists/bulk_cp1.tsv` (BIND lane,
  RACE-BIND-C4-BULK checkpoint 1). One owner per RVA: not bound here, kept a named stop (`STOP_FORCE`). 0x0045d460 does not
  return yet, so DAT_00604604 / DAT_0068fc84 after it and the oracle with `MASHED_OT_AUDIOINIT` were not measured.
  **Done-means not met: 0x00604604 is not compared with ref1 at record 0.**
- **Self-test result: PASS** in every arm of every checkpoint, both builds: portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367
  failed, code-trap 0 of 6 failed, pages 459/459.
- Knob off (control, final tree): both builds stop at 0x005AEF30, the base stop (section 5).
- Gates: not run (top-down row). No menu walk (no menu code changed). Replay oracle: not run (the RO arm reaches the same stop
  before tick 1; stack file `verify/savelink_20261023/stacks/ROAUDINIT.txt` is committed for the next row).
- Object build (final): IDENTICAL 1798/1844, **DIFFERENT 31** = the base's 28 (RESULT_PORT-TOP-AUDIO-5AEF30-KNOB merged-tree
  list) + 0x005a6b70 and 0x005ab410 (ADMITTED.tsv, plain EQUIV, newly linked by 0x0045d460's closure) + 0x005acd10 (hooks.csv C4
  `c4=frida-replay`, source below 100% by definition, newly linked). No unexplained DIFFERENT unit. not-in-map 15, units linked
  1791/2051, refusals 11 (unchanged classes).
- equiv.py = tool commit 1ed7ceb1a, no diff on this branch.

## 1. Census 71..92 (PORT-TOP-AUDIO-5BA1D0-STOPS)

| rva | census | source | before -> now | how | status |
|---|---|---|---|---|---|
| 0x005ba1d0 | 71 | re/match/s27/005ba1d0.c | 33.8 -> 40.1% | from the listing: the 0x005bbc10 call block (0x005ba313..0x005ba374) had garbage stack args and a call through address 0 (`iRam00000000`); caps block was 14 dwords but 0x005ba3c5 clears 0x18; COM calls `__stdcall` (no add esp after 0x005ba308 / 0x005ba3e5 / ...); DirectSoundCreate8 named as `FUN_005c1920` so the import-thunk alias binds | provisional |
| 0x005bb000 | 75 | s27/005bb000.c | 42.7 -> 50.7% | rewritten from the listing: first lock region cleared from ptr+4 in the Ghidra form (0x005bb216 is a plain `rep stosd`), +0xec read before its store (0x005bb127 / 0x005bb12d), COM calls `__stdcall`; 0x005aef30 maps to the knob | provisional |
| 0x005bbc10 | 76 | s27/005bbc10.c | 44.4% | checked against the listing (format retry loop) | provisional |
| 0x005bbd50 | 77 | s27/005bbd50.c | 45.7 -> 51.4% | rewritten: the Ghidra form returned the descriptor address; the original returns the created buffer or 0 (0x005bbd9d..0x005bbdaa) | provisional |
| 0x005ae250 | 74 | s26/005ae250.c | 64.9% | checked | provisional |
| 0x005c7d30 | 79 | s28/005c7d30.c (new) | 50.3% | new source from the listing; the branch uses the value loaded at 0x005c7d5a (not a reload); code addresses 0x005aaaa0 / 0x005c7ee0 named as functions; C1 row via `c1_add.py` | provisional |
| 0x005bc470 | 81 | s27/005bc470.c | 29.7% | checked (format block at local 0, descriptor at local 0x14) | provisional |
| 0x005bf670 | 86 | s28/005bf670.c | 40.0 -> **100.0%** | declared `__stdcall`: BYTE-IDENTICAL 17 B, relocheck OK; 0x005bf610 (C4) leaves `STOP_FORCE` and binds | promotion TSV |
| 0x005bbfc0 | 87 | s27/005bbfc0.c | 94.1% | existing | provisional |
| 0x005baf00 | 88 | s27/005baf00.c | 82.6 -> 91.3% | the self-store of [node+0x1c] (fld / fstp 0x005baf16 / 0x005baf1f) kept as a volatile read | provisional |
| 0x005a91b0 | 92 | s25/005a91b0.c (new) | 79.3% | new source from the listing (stored at +0x28 by 0x005a8f30, 0x005a9160); C1 row | provisional |

census 72 / 73 / 78 / 80 / 82..85 are C4 and bind by the closure. Arms after this part: 0x004627f0 returns (section 4).

## 2. Inside 0x0045d460 (RO-AUDIOINIT-45D460-REACH)

- **PERMDICT.RWS open.** The first arm with 0x0045d460 bound faulted (AV, both builds): `RwCore_004cbd30+0xa` (RwStreamRead) read
  a NULL stream (Frida catcher on the object build, `catch1_ddk.json`). The bound 0x0045d460 called the matched RW core
  RwStreamOpen `RwCore_004cc230`, which opens through RtFSManager file systems the exe does not register, and got 0 (the
  original does not test the handle either). The generator now has a per-caller alias (`CALLER_ALIAS`): 0x0045d460's 0x004cc230 /
  0x004cc160 go to the existing PORT-TOP-04F RW_FS binding (`Match/PortTop_RwStream04F.cpp`, already used by the 0x00402750
  loaders and `MatchS06_004274e0.cpp`). Not a librw binding (L-124). Log: `[RW_FS] 0x004cc230(2, 1,
  "toastaudio\pc\audio\pcdics\PERMDICT.RWS"): 2242988 bytes from disk -> memory stream`; the matched RwStreamFindChunk /
  RwStreamRead read it (case 3).
- Bodies bound for the chunk loop:

| rva | source | score | how | status |
|---|---|---|---|---|
| 0x005a7b60 | s25/005a7b60.c | 60.9% | existing row, replay IDENTICAL-FOR-2724 (PROV-FIX-9) | provisional |
| 0x005a71f0 | s25/005a71f0.c | 47.9% | checked against the listing | provisional |
| 0x005a7b40 | s25/005a7b40.c (new) | **100.0%** | BYTE-IDENTICAL 16 B, relocheck OK | promotion TSV |
| 0x005a73a0 | s25/005a73a0.c (new) | **100.0%** | BYTE-IDENTICAL 15 B, relocheck OK; C1 row (callback 0x005a71f0 passes, 0x005a7293) | promotion TSV |
| 0x005ab380 | s25/005ab380.c | 75.0% | checked | provisional |
| 0x005aeca0 | s26/005aeca0.c | 78.4% | checked | provisional |
| 0x005abfa0 / 0x005ad0b0 / 0x005aba20 | existing rows | 67.1 / 44.3 / 77.6% | replay IDENTICAL-FOR-2724 (PROV-FIX-9) | provisional |
| 0x005ac210 | s25/005ac210.c | 57.1 -> 72.4% | rewritten from the listing: allocation base `[desc+0x34] + 0x5c` (0x005ac442; the Ghidra form used param_3), callbacks 0x005ac770 / 0x005aaaa0 named as functions, typed RwGlobals +0xf4 / +0xcc slots | provisional |
| 0x005abd30 / 0x005aec30 | existing | 52.3 / 73.2% | checked | provisional |
| 0x005acaa0 | s26/005acaa0.c | 60.4% | checked (decode branch) | provisional |
| 0x005b3a00 | s27/005b3a00.c | 84.7% | checked | provisional |
| 0x005b3b80 | s27/005b3b80.c | 43.4 -> 42.6% | list entries as `local_48[i]` (the Ghidra form read `local_38[i - 4]`, a stack alias), param_4[4] / [5] read before the second handler call (0x005b3d7b / 0x005b3daf) | provisional |
| 0x005b3760 | s27/005b3760.c | 57.1 -> 49.8% | cache entry +0x1c written after the 0x005ac980 call into it (0x005b3958 / 0x005b3965); the Ghidra form stored to `&iVar1 + DAT_007ddb4c` before the call | provisional |
| 0x005bb910 / 0x005bba30 / 0x005bba60 / 0x005bbac0 | s27/ (new) | 64.5 / 63.6 / 64.9 / 64.9% | the four code pointers 0x005bb850 stores at +0xc..+0x18 (0x005bb88d..0x005bb8a2); new sources from the listings; `pc_funcs.csv` rows for 0x005bb910 / 0x005bba30 (no Ghidra function; bounds from the listings); C1 rows for those two | provisional |

All provisional rows: `re/match/PROVISIONAL.tsv` (24 rows written by this row, equiv verdict and first difference in each;
every verdict NOT-PROVEN: register-argument leftovers, `rep movsd` with a variable count, x87 form, call-memory). None is
admitted, none is C4. Their behaviour claims rest on reading the listings: one [UNCERTAIN] line in
`re/orchestrator/UNCERTAIN_REGISTER.md` (test: PROV-INSITU replay on ref1). Second register line: the factor at 0x005d757c that
0x005a91b0 reads (file image 0x00000000, runtime writer not checked).

## 3. Ownership
- Newly bound by this row: 70 rvas (`newly_bound.txt`). None is in `race_owned.txt` or `bulk_cp1.tsv`.
- Not in any lane's list (reached through this closure, bound here): 0x005bb910, 0x005bba30, 0x005bba60, 0x005bbac0, 0x005c7ee0,
  0x005c9220, 0x005c9740.
- **Needed and not bound: 0x005c91f0** (bulk_cp1.tsv, BIND lane). It is the current stop.

## 4. Arms (knob on + RWMOD), both builds

| checkpoint | commit | stop (VS2022 / object) | return address |
|---|---|---|---|
| base (previous row) | e65c52d47 | 0x005BA1D0 | `_PortTop_Bound_005aa560+0x18a` |
| 1 | 45402f5fa | AV in `RwCore_004cbd30+0xa` (NULL stream) after `0x004627f0 returned` | Frida catcher |
| 2 | 0f76b7e9e | 0x005AB380 | `_PortTop_Bound_005a7b60+0x36` |
| 3 | bb9990520 | 0x005ACAA0 | raw 0x002495DE / 0x002479EE (that build's maps not kept) |
| 4 | (committed in 7c8ea0044) | 0x005B3A00 | raw 0x00249D75 / 0x00051808 (that build's maps not kept) |
| 5 | 7c8ea0044 | 0x005BB910 | `_PortTop_Bound_005aba20+0x278` (code pointer) |
| 6 | 7ce172fa3 | **0x005C91F0** | `_PortTop_Bound_005bb910+0x4f` (both maps) |

Arms command: `ARMS_STEP=PORT-TOP-AUDIO-5BA1D0-STOPS ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIO-5BA1D0-STOPS_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1
MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1 MASHED_OT_AUDIOTHREAD_STANDIN=1 MASHED_ORIG_TOP_RWMOD=1" test-slot py -3.12
<ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIO-5BA1D0-STOPS -- py -3.12
verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`. Private root: robocopy of
`D:/MashedWT/PORT-TOP-AUDIO-5AEF30-KNOB_root`, old `mashed_re.log` removed. Builds: `build_both.sh` waited for every other
`mashedmod\build.bat` and for any PRIORITY-HOLD / SOLO before each build (status lines in `lockstatus.txt`).

## 5. Knob-off control (final tree)
Final tree, ARMS_EXTRA without `MASHED_OT_AUDIOTHREAD_STANDIN` / `MASHED_ORIG_TOP_RWMOD`: both builds stop at **0x005AEF30**
(return addresses 0x00244CDC VS2022 / 0x0004E88D object), the base's knob-off stop (RESULT_PORT-TOP-AUDIO-5AEF30-KNOB section 3).
Self-tests PASS in both. No default-path TU changed: every new TU is `RVA-LINT: ORIGTOP-ONLY`; rva-lint NEW 0 at every build.

## 6. Next audio step (run 54; the HANDOFF of this row)
State: L0 released 0x005c91f0 as REUSE: RACE-BIND-C4-BULK checkpoint 1 bound it as `Match/PortTop_005c91f0.cpp`
(re/match/s28/005c91f0.c, ddkobj IDENTICAL) on orch/RACE-BIND-C4-BULK @ a6acdece6. L0's instruction: do not merge that branch here;
wait until drain 3 puts CUNCLEAN + that checkpoint on orch/integration. Then, on claude3 (git merge prompts on claude2):
1. `git -c merge.autoStash=false merge orch/integration` into orch/PORT-TOP-AUDIO-5BA1D0-STOPS. Regenerated files (wrappers,
   `PortTop_AudioStops.cpp`, `PortTop_BulkRefs.cpp`, BINDINGS.tsv, CODEPTR, plan.json, exeidx.json) are regenerated with
   porttop_bind.py / `audiotick_bind_gen.py`, never hand-merged; append tails (PROVISIONAL.tsv, allowlist, rsp, newfiles.txt) keep
   both sides.
2. Remove the `STOP_FORCE |= {'005c91f0'}` line in `audiotick_bind_gen.py`; 0x005bb910 then names `PortTop_Bound_005c91f0` (reuse
   of the bulk body, no second copy). Generator --write, porttop_bind.py refs step, rsp / allowlist sync, bindings_update,
   codeptr_inventory.
3. Both builds, `ddkobj.py check` (classify any new DIFFERENT unit), knob-on arms (self-test result in the RESULT).
4. Remaining planned stops inside 0x0045d460 (plan13): 0x005abe30 (20.7%), 0x005ac540 (50.0%, 5-instruction leaf, checked),
   0x005bc050 (32.4%), 0x005bc0f0 (23.3%); 0x005bc050 / 0x005bc0f0 / 0x005abe30 carry Ghidra artifacts (2 / 2 / 1 matches of
   `code **` / stack aliasing): read their listings before binding. Commit per stop.
5. When 0x0045d460 returns: DAT_00604604 / DAT_0068fc84 from the `PortTop_004669b0.cpp` log line (test 0x0068fc84 = 139 from
   RESULT_PORT-TOP-AUDIO-BODY-45D460), then the RO arm on the object build:
   `A4_OUT=D:/MashedWT/<ROW>_data A4_ROOT=D:/MashedWT/<ROW>_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe
   test-slot bash -c 'py -3.12 <orch>/re/orchestrator/mashed_lock.py run --label <ROW> -- py -3.12
   verify/savelink_20261023/run_arms.py RO_ROAUDINIT-pr1'` and score with `re/frida/detorig_firstdiv.py
   D:/MashedWT/RO-REF-NAME_data/ref1 <arm dir> --masks re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv --ignore
   0x00614400-0x00614800,0x00771800-0x00771c00 --exe-map mashedmod/build/ddkobj/mashed_re_ddk.map`: report 0x00604604 at record 0
   (ref1 = 4).
- Promotions: `re/promotions/PORT-TOP-AUDIO-5BA1D0-STOPS.tsv` (0x005bf670, 0x005a7b40, 0x005a73a0), dry run `batch: 3 pass, 0
  refused`. No `--apply`.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIO-5BA1D0-STOPS_data/`, sha256 in `sha256.txt` there)
All 119 files listed with full sha256 in `sha256.txt`. First 16 hex of the cited ones:
- final arms: `cp6on/vs_init_audio.log` 435db05048711ade, `cp6on/ddk_init_audio.log` e1d925e2e739247d, `arms6on.out` ba165fd0b0b7489d;
  knob off `cp6off/vs_init_audio.log` 28f392df0b048ca6, `cp6off/ddk_init_audio.log` fd0c2e7dabfc6e6e, `arms6off.out` c2e76f8d59b854c8
- earlier checkpoints: `cp1on/` .. `cp5on/`, `arms1on.out` .. `arms5on.out`
- AV catch (checkpoint 1): `catch1_ddk.json` 1d77e212ae6bb6bf
- object check: `ddkcheck6.log` d6afda6d1cd3b500, `bytecheck6.tsv` 121acef6db62f249 (and `ddkcheck1..5`, `bytecheck1..5`)
- promotions: `match_005bf670.txt` 45f836ea13ba9879, `reloc_005bf670.txt` 9f539d3736f0f10e, `match_005a7b40.txt` 9676a610d0c942ee,
  `reloc_005a7b40.txt` 70be175d530d3fb9, `match_005a73a0.txt` 690ba4ce5116ba80, `reloc_005a73a0.txt` 4f70cea963d901c9, `c4_dry2.txt`
  932eb2c26d5d95e6
- equiv: `equiv_<rva>.txt` for every provisional row (e.g. `equiv_005ba1d0.txt` 590c1b3c7df13856, `equiv_005ac210.txt` 80202456d70f025a)
- previous sources: `005ba1d0_before.c` ef9841c15376c240, `005bb000_before.c` d407d543327da835, `005bbd50_before.c` f6dec2a0349463e9,
  `005baf00_before.c` 9121f0dd465b39a0, `005bf670_before.c` a4da87185bbc9ab5, `005ac210_before.c` 40b2898cd735c341,
  `005b3b80_before.c` dd57464aa00814df, `005b3760_before.c` 2cf08cf0f13fc03a
- `newly_bound.txt` 0a07f8e370b42f34, `lockstatus.txt` 99187a878fa70d24 (lock status printed before every build and arm run; no build
  ran under a PRIORITY-HOLD or SOLO), plans `plan*_dry.txt` / `plan*_write.txt`
