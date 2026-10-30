# RESULT D4-WSB-PORT-BROAD-1: GREEN, 17/17 shadow CLEAN, 14 rows C2->C3, 3 rows held at C2 on the caller gate

PREREG `PREREG_D4-WSB-PORT-BROAD-1.md` committed alone at `cc069cc98`, amendment 1 committed at `a39cf1c7d` (with its
code fix), both before the arms they govern. Branch `orch/D4-WSB-PORT-BROAD-1` off `orch/integration` @ `f6033d936`.

## Per-row outcome

| row | before | after | shadow A/B (census scenario, hook alone) | heap pages o/n per sample |
|---|---|---|---|---|
| `0x00558c50` | C2 | C2 (caller gate) | CLEAN n=1, `uninstalled=56` | 5/5 |
| `0x00558ca0` | C2 | **C3** | CLEAN n=2, `uninstalled=8b` | 0/0 x2 |
| `0x00558cb0` | C2 | C2 (caller gate) | CLEAN n=1, `uninstalled=83` | 0/0 |
| `0x00559040` | C2 | **C3** | CLEAN n=24, `uninstalled=83` (amendment 1 build; pass 1 DIVERGENT, superseded) | 0/0 x24 |
| `0x00559560` | C2 | **C3** | CLEAN n=1, `uninstalled=56` | 0/0 |
| `0x005595d0` | C2 | **C3** | CLEAN n=1, `uninstalled=53` | 13/13 |
| `0x00559b50` | C2 | **C3** | CLEAN n=1, `uninstalled=83` | 17/17 |
| `0x00559bc0` | C2 | **C3** | CLEAN n=24, `uninstalled=8b` | 1/1 x24 |
| `0x00559be0` | C2 | **C3** | CLEAN n=24, `uninstalled=8b` | 2/2 x24 |
| `0x00559c40` | C2 | **C3** | CLEAN n=24, `uninstalled=83` | 8/8 x1, 10/10 x21, 11/11 x2 |
| `0x00559ee0` | C2 | **C3** | CLEAN n=24, `uninstalled=51` | 5/5 x2, 7/7 x4, 8/8 x18 |
| `0x0055ad20` | C2 | **C3** | CLEAN n=1, `uninstalled=d9` | 1/1 |
| `0x0055ad30` | C2 | **C3** | CLEAN n=24, `uninstalled=53` | 9/9 x24 |
| `0x0055ade0` | C2 | **C3** | CLEAN n=24, `uninstalled=56` | 4/4 x24 |
| `0x0055ae70` | C2 | **C3** | CLEAN n=24, `uninstalled=83` | 6/6 x13, 8/8 x6, 9/9 x5 |
| `0x0055af40` | C2 | **C3** | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x0055b030` | C2 | C2 (caller gate) | CLEAN n=24, `uninstalled=56` | 8/8 x23, 9/9 x1 |

Every CLEAN row: ndiff 0 on every sample, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`. Verdicts from
`re/tools/shadow_ab_report.py` (`shadow_summary.tsv`). Heap pages counted from the row's own sample lines only (nested
lines from called port wrappers excluded). The race_load rows sample 1-2 calls per boot because the census scenario
loads the race once.

**Caller gate (pre-registered, PREREG "C3 gate pre-check").** `0x00558c50` (only xref `0x0055164c` in `FUN_00551610`),
`0x00558cb0` (only xref `0x00551982` in `FUN_005518f0`) and `0x0055b030` (only xref `0x005a59e5`, inside no Ghidra
function) have no caller at C2+ and no identified caller, so they stay C2 with a dated note. Their ports and CLEAN
shadow results stand. The next step for them is a caller row (C1->C2 of `0x00551610` / `0x005518f0`, a Ghidra function
at `0x005a59e5`), not more work on the rows.

**No C4.** No row has a race diff with the hook installed (`re/CONFIDENCE.md` write-set leg, L-38/L-40/L-54). Rows with
heap pages compared > 0 are recorded above for a later C4 leg. `0x00559040`, `0x00559560`, `0x00558cb0`, `0x00558ca0`
compared 0 heap pages on every sample.

## Amendment 1: `0x00559040`

Pass 1 (`.asi 6965a567`): DIVERGENT 12/24, return equal, 0 pages differ, 1-2 bytes at `stk+064` / `stk+068` of the caller's
stack window (an out-parameter's low bytes). Cause: five three-term float sums transcribed in Ghidra's order, not the
x87 order (`0x005590c7`, `0x005591c9`, `0x00559275`, `0x00559292`, `0x005592bc`; table in the PREREG amendment). Fix
`a39cf1c7d` parenthesizes them to the x87 order, nothing else. Re-run on `.asi a5b857aa`: CLEAN 24/24. The pass-1 summary
row is kept in `shadow_summary_pass1.tsv` and is not used.

## What was built

- 17 Lane-2 TUs `mashedmod/src/mashed_re/Lane2/L2_<rva>.cpp`: generator output `38a4c4d09` (after `prep_broad1.py`),
  hand fixes `5b9eb00b2`, amendment 1 `a39cf1c7d`. Registered `L2_FUN_<rva>`, `.asi` only, opt-in
  (`Core/HookSystem.cpp:244-253`): a default `.asi` run and `mashed_re.exe` do not install them. No exe source changed.
- Hand fixes (each cited in its TU, full list in the PREREG): `.rdata` float constants read as float (`0x00559040`,
  `0x005595d0`); raw float bits instead of numeric float->uint conversions (`0x005595d0` x2, `0x00559b50`); EAX return of
  the void-typed `0x00559b50` (caller reads it at `0x0055dcc0`) and of the `0x00563df0` / `0x00564130` thunks; the three
  locals of `0x00559b50` as one 12-byte block; one `sqrtf` reused and an ordered compare in `0x00559040`; `kPhaseAny` for
  the 14 menu / race_load rows.
- `prep_broad1.py` (5 TUs): object vtable / RwGlobals slot calls as cdecl `L2_vcall` (every site caller-cleans in the
  disassembly), and the `0x0055ae70` CONCAT22 high half dropped (masked by the callee at `0x00565208`).

Final build (`MASHED_NO_DEPLOY=1`, at `a39cf1c7d`): `mashed_re.exe` `3d27a4a2de15541e8e5d54a1ca2e1eff183f8a86e04305f2dfadba9cf61acc79`,
`mashed_re_dev.asi` `a5b857aa629df5f6b39d8b473a0826070cf34d85d3bd91e09ba2eef44e96a52f` (copied to the private root only,
`original/mashed_re_dev.asi` not touched). rva-lint 0 NEW. The first build's exe was `d8c498ec...` with the same exe
sources (the exe link is not bit-reproducible, as D4-WSB-PORT-SOLV found); the gates below carry the exe check.

## Default gates on this build

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + machine lock,
`A4_ROOT=D:/MashedWT/D4-WSB-PORT-BROAD-1_root`, exe `3d27a4a2`. All six equal `re/orchestrator/merge_train.json`:

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `5943c5f1` | `5943c5f1` |
| H_DEF | gates.csv | `306186d4` | `306186d4` |
| H_DEF | ptrace.log | `52052890` | `52052890` |
| B_DEF_K0 | step.csv | `3a03670c` | `3a03670c` |
| B_DEF_K1 | step.csv | `9fcdbd3a` | `9fcdbd3a` |
| B_DEF_K26 | step.csv | `53778664` | `53778664` |

## Run log

- Boot check first (no hooks, track 0, mode 10, 4 cars, hold 15): race running, game PID 17040, 0 MASHED Event-1000 events,
  endpoint `OK Philips FTV (NVIDIA High Definition Audio)` before and after.
- Endpoint present before and after every one of the 18 shadow windows (17 rows + the amendment re-run). 0 crash events in
  any window, no fault at `0x005baf48`, no U-9677 signature, no re-run needed.
- The machine lock was shared with D4-WSB-PORT-BVH (solo windows), the merge train and D3-SUBSTATE-FLIP-D2. Row 3 and
  row 17 queued for their windows. No run was stopped.

## Tracker transaction (re-classify)

- `hooks.csv`: 14 rows C2 -> **C3 impl**, `file` = the port, `scenario` `shadow_tracked:track0-mode10-cars4`, `frida_diff`
  `shadow_ab:verify/d4wsbportbroad1_20261029/shadow_summary.tsv`, dated note. 3 rows stay C2 (`mapped` -> `impl`, `file` =
  the port) with a dated note naming the failed caller half.
- C3 gate per promoted row: port under `mashedmod/src/mashed_re/`, clean build, `RH_ScopedInstall`, runtime-toggleable
  (`MASHED_HOOK_ONLY` / `MASHED_HOOK_LANE2`), a caller at C2, callees at C2+ or none (the vtable and RwGlobals slot calls
  are runtime-data dispatch, CONFIDENCE.md indirect-dispatch clause).
- `UNCERTAINTIES.md`: U-9340 (`0x0055ad30`), U-9343 (`0x00559c40`), U-9520 (`0x00558cb0`), U-9522 (`0x00559ee0`),
  U-9523 (`0x0055ade0`) resolved through `re/tools/uncertainty_resolve.py`. Each lists generator items "target not
  determined statically" and "esp depth not determined / taken from one path" around an indirect call; their Blocks cell
  reads "the C3 hand-read must close or carry these". The hand-read for this step closes them: every listed indirect call
  is caller-cleans (`add esp, 4*n` right after it: `0x00558cf6`/`0x00558cfc`, `0x00559d0f`, `0x00559de0`, `0x00559e76`,
  `0x00559f45`, `0x0055ad77`, `0x0055ae1a`), which fixes the esp depth at every listed address, and the targets are object
  vtable / RwGlobals slots read at run time (indirect-dispatch clause), confirmed by the CLEAN shadow A/B. No new U or S rows.
- `re/analysis/CHANGELOG.md`: one line per row below `<!-- ENTRIES -->`.

## Files

Committed: `PREREG_D4-WSB-PORT-BROAD-1.md`, `RESULT_D4-WSB-PORT-BROAD-1.md`, `shadow_summary.tsv`, `shadow_summary_pass1.tsv`,
`prep_broad1.py`, `disasm_lin.py` / `scan_eax_use.py` (copies from `verify/d4wsbportbvh_20261029`), `scan_eax_use.txt`,
`run_shadow8.py` (copy of `verify/d4wsbportbvh_20261029/run_shadow7.py`, data path and label changed), `drive_pb1.py` (copy
of `verify/d4wsbportbvh_20261029/drive_pb.py`, runner name, data path and label changed).

Raw (not committed), `D:/MashedWT/D4-WSB-PORT-BROAD-1_data/`:

| file | SHA-256 |
|---|---|
| `decomp.json` | `eeacb0e0bf005b16d41f69d49bf1c519a02bd1a5050ae6cdf82b233f3f318b7b` |
| `decomp_prep.json` | `43e4a5d7760b8d2d775898829240f09102029c418e593fb79bc8bb6b0ebadbc1` |
| `dis_00559040.txt` | `d627c1d5ea0e5edd148eabf48c6466a015f698513305ec781a1198303c4049d1` |
| `bootcheck.txt` | `6c116fe60b827ef0fae3a73d9bf2bb9c7ab5e88ed62f5b0b89214245c1f1483f` |
| `00558c50/shadow_ab.log` | `55591cb9fd447dcbfdd9988f90573486c31cd2e0e4f60844780c7a81700d8047` |
| `00558ca0/shadow_ab.log` | `200545458a002453ebc0f8c807245db15fe4e980a4ddf9a677ea93f050bd7557` |
| `00558cb0/shadow_ab.log` | `7398a09f9ebc73e529126ce55a8aeef4344e8219cb154d202f028b38f105d94b` |
| `00559040/shadow_ab.log` (amendment re-run) | `936fbe221faa6bd36aad223267dc0e06acb3f14962f7b9c30bb8b68e2cf44a66` |
| `00559040_pass1_superseded/shadow_ab.log` | `39c5749752c1affafaa8670080ecacffbf9e3645bc38c7affe626b08e91e16f3` |
| `00559560/shadow_ab.log` | `99f6aca24799e1b785e64e0ded02a0073a3f9f0c7b57cdedaced2c5c4a009b4c` |
| `005595d0/shadow_ab.log` | `482b89229e04ecacb5ae668e32d5eb6765d0a0cc02cec7b1cfeac307803df729` |
| `00559b50/shadow_ab.log` | `0727abc0a148e56b3fc68752e87fc9e91be736b1c8dab9d9a4a82a92b341017c` |
| `00559bc0/shadow_ab.log` | `4d1d8faa29df223f30d0226f7aa7be744f892a40eb5819e866317af0ac08dc03` |
| `00559be0/shadow_ab.log` | `399b39a31ba56bc28e562f3bf686cb7e98c13ff2c28fa6a94269d09e7d553317` |
| `00559c40/shadow_ab.log` | `b66b2050b8b7df7602be984cb2bcadf7365320dd7269136a44cacd1a0dd0008c` |
| `00559ee0/shadow_ab.log` | `8fdc503118e7b32f29f39dcc15db52b001f6042d4c66cfa10bbb746e653a54bc` |
| `0055ad20/shadow_ab.log` | `8d696624ddad8fa38632e6527c05cb6b2937930c273a93644be4f574a9301754` |
| `0055ad30/shadow_ab.log` | `d26fd491de566f3f096e1afe7f56d9a126e479ce8514764c8abf8d48192050c1` |
| `0055ade0/shadow_ab.log` | `2a888a0b38c1a13e5717667acccfc24c3c662916d3d910c2d6cede38ab76f28b` |
| `0055ae70/shadow_ab.log` | `f7ec483a56514c5381db05be0d1a074b177cb069a5d1458d3b41bbf78fd6ff6c` |
| `0055af40/shadow_ab.log` | `c302e013bd4f67fa59bd0be1db6c8abb02901c7cbb4f8e694872c88a0627d8c9` |
| `0055b030/shadow_ab.log` | `4fac313c25288960cc485e9fdcdd2210f43c704ecea60fde83b563921fa44380` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `306186d44ce7193b93b53a12e1658b871e90bb276480c2add7d8c9f7c08e6a2e` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |

Game PIDs spawned (only these killed, by PID, by the runner): boot check 17040 (self-exited); shadow 8428, 376, 5292,
13376 (pass 1 of `0x00559040`), 4080, 5492, 2200, 5472, 22708, 16668, 22300, 23364, 6180, 7004, 22400, 21752, 3544,
20392 (amendment re-run); gates 20780, 19380, 2036, 15644 (self-exited). None alive at the end. No watcher left running.
