# RESULT D4-WSB-PORT-SOLV: GREEN on 14 of 18 rows (C2->C3), 4 rows not ported (handoff)

PREREG `PREREG_D4-WSB-PORT-SOLV.md` committed alone at `ef8ffacc0`, amendment 1 committed alone at `30c97506f`,
both before the arms they govern. Branch `orch/D4-WSB-PORT-SOLV` off `orch/integration` @ `34235b24c`.

## Per-row outcome

| row | before | after | port | shadow A/B (census scenario, hook alone) | heap pages o/n per sample |
|---|---|---|---|---|---|
| `0x005663a0` | C2 | **C3** | `Lane2/L2_005663a0.cpp` | CLEAN n=24, `installed=e9 uninstalled=8b` | 0/0 x14, 1/1 x10 |
| `0x005672c0` | C2 | **C3** | `Lane2/L2_005672c0.cpp` | CLEAN n=2, `uninstalled=53` | 0/0 |
| `0x00567350` | C2 | **C3** | `Lane2/L2_00567350.cpp` | CLEAN n=1, `uninstalled=83` | 1/1 |
| `0x00567630` | C2 | **C3** | `Lane2/L2_00567630.cpp` | CLEAN n=2, `uninstalled=8b` | 0/0 |
| `0x005676f0` | C2 | **C3** | `Lane2/L2_005676f0.cpp` | CLEAN n=1, `uninstalled=83` | 1/1 |
| `0x00567c40` | C2 | **C3** | `Lane2/L2_00567c40.cpp` | CLEAN n=1, `uninstalled=e8` | 0/0 |
| `0x005687a0` | C2 | **C3** | `Lane2/L2_005687a0.cpp` | CLEAN n=2, `uninstalled=8b` | 0/0 |
| `0x005687d0` | C2 | **C3** | `Lane2/L2_005687d0.cpp` | CLEAN n=2, `uninstalled=8b` | 0/0 |
| `0x00568860` | C2 | **C3** | `Lane2/L2_00568860.cpp` | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x00568c40` | C2 | **C3** | `Lane2/L2_00568c40.cpp` | CLEAN n=2, `uninstalled=8b` | 0/0 |
| `0x00568c80` | C2 | **C3** | `Lane2/L2_00568c80.cpp` | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x00568cf0` | C2 | **C3** | `Lane2/L2_00568cf0.cpp` | CLEAN n=1, `uninstalled=8b` | 0/0 |
| `0x00568d20` | C2 | **C3** | `Lane2/L2_00568d20.cpp` | CLEAN n=1, `uninstalled=53` | 8/8 |
| `0x0056a450` | C2 | **C3** | `Collision/RwpVtableKV1.cpp` (existing port, wrapper added) | CLEAN n=24, `uninstalled=55` | 0/0 x24 |
| `0x0056a7a0` | C2 | C2 | not ported | no arm | |
| `0x0056aae0` | C2 | C2 | not ported | no arm | |
| `0x0056ac40` | C2 | C2 | not ported | no arm | |
| `0x0056adb0` | C2 | C2 | not ported | no arm | |

Every CLEAN row: ndiff 0 on every sample, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`, 0 NOISY / UNTRACKED /
OTHER-EXC lines. Verdicts from `re/tools/shadow_ab_report.py` (`shadow_summary.tsv`). The race_load rows sample 1-2
calls per boot because the census scenario loads the race once. No C4 is claimed: 9 rows compared 0 heap pages and
none has the race diff (`re/CONFIDENCE.md` write-set leg, L-38/L-40/L-54).

## What was built

- 13 Lane-2 TUs, `re/tools/decomp2port.py --emit-dir Lane2 --apply` from `decomp_pc.py --port` (pool0). No hand edit
  of a body. Registered `L2_FUN_<rva>`, `.asi` only, opt-in (`Core/HookSystem.cpp:244-253`): a default `.asi` run
  and `mashed_re.exe` do not install them. Raw-RVA thunks to the original: `0x00574a20`, `0x00574ac0` (from
  `0x00567c40`), `0x00564100` (from `0x00568d20`).
- `0x0056a450`: `ShadowAB::RunTracked` wrapper under `#ifndef MASHED_STANDALONE`. `cl /EP /DMASHED_STANDALONE` of
  `RwpVtableKV1.cpp` before/after: 73,456 non-blank lines, identical.
- Amendment 1 (`f866c7dcc`): the generated wrappers used `kPhaseRace`, which skips any call while
  `*(u8*)0x00771968 < 3` (`Core/ShadowAB.h:159`). The 12 race_load rows logged nothing (`0x005672c0` NO_LOG on the
  first pass). They now use `kPhaseAny`. Bodies unchanged.

Final build (`MASHED_NO_DEPLOY=1`): `mashed_re.exe` `c150342c7e213537dc555fc7c59dc562716de5f84b21a6bce07ad6bedaa21ad2`,
`mashed_re_dev.asi` `b99d6d3accd91ddae918caf57282b248bdb10d3b6cc474ee76fdd3ff9ff4dc8f` (copied to the private root
only, `original/mashed_re_dev.asi` not touched). Two builds with no exe-source change gave different exe hashes
(`d00f585c...` then `c150342c...`), so the exe link is not bit-reproducible here; the gates below carry the exe check.

## Default gates on this build

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + machine lock,
`A4_ROOT=D:/MashedWT/D4-WSB-PORT-SOLV_root`. All six equal `re/orchestrator/merge_train.json`:

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `5943c5f1` | `5943c5f1` |
| H_DEF | gates.csv | `364350ae` | `364350ae` |
| H_DEF | ptrace.log | `52052890` | `52052890` |
| B_DEF_K0 | step.csv | `3a03670c` | `3a03670c` |
| B_DEF_K1 | step.csv | `9fcdbd3a` | `9fcdbd3a` |
| B_DEF_K26 | step.csv | `53778664` | `53778664` |

## Run log

- Boot check first (no hooks, track 0, mode 10, 4 cars, hold 15): race running, 0 MASHED Event-1000 events,
  endpoint `OK Philips FTV (NVIDIA High Definition Audio)` before and after.
- Endpoint present before and after every one of the 16 shadow windows. 0 crash events in any window, no fault at
  `0x005baf48`, no U-9677 signature.
- First pass: row 1 CLEAN, row 2 NO_LOG (phase gate), row 3 stopped mid-boot to apply amendment 1 (its MASHED PID
  17964 and my runner PIDs killed by PID; the data folder is kept as `00567350_stopped`, not used).
- Second pass: `0x00567c40` boot failed with `error: could not attach` (Frida, PID 11308 exited on its own, nothing
  ran); re-run once, CLEAN. The failed transcript is kept as `00567c40_noattach`.

## Tracker transaction (re-classify)

- `hooks.csv`: the 14 rows C2 mapped -> **C3 impl**, `file` = the port, `scenario` `shadow_tracked:track0-mode10-cars4`
  (or `shadow:` for `ShadowAB::Run` wrappers), `frida_diff` `shadow_ab:verify/d4wsbportsolv_20261029/shadow_summary.tsv`,
  dated note. `exe_file` unchanged (empty for the Lane-2 rows: no exe copy exists).
- C3 gate: port under `mashedmod/src/mashed_re/`, clean build, `RH_ScopedInstall`, runtime-toggleable, callers at C2+
  (`0x0055f800`, `0x0055f520`, `0x0055f670`, `0x00563940`; `0x0056a450` via the K13 slot of `0x00560260`, C4), callees C2
  (`0x00574a20`, `0x00574ac0`, `0x00564100`) or none.
- U-9584 / U-9585 / U-9586 / U-9588 (generator esp-depth listing) stay open, carried on the row notes: their Blocks
  cell reads "nothing at C2; the C3 hand-read must close or carry these". No new U or S rows.
- `re/analysis/CHANGELOG.md`: 14 lines below `<!-- ENTRIES -->`.

## Handoff: the 4 unported rows (`0x0056a7a0`, `0x0056aae0`, `0x0056ac40`, `0x0056adb0`)

These are the PGS iteration chain (`0x0056adb0` -> `0x0056aae0` -> `0x0056a7a0`, `0x0056adb0` -> `0x0056ac40`, all census
`race_run`). The decomp mis-models their frames, so no transcription is safe:

- `decomp2port.py` refuses `0x0056aae0` (CALLEE_ARITY: `0x0056a7a0` proto 4, site 3), `0x0056ac40` (float10 return),
  `0x0056adb0` (CALLEE_REG_ARG: `0x0056aae0` proto 12, site 14). It accepts `0x0056a7a0`, but that body reads
  `in_stack_00000020..0000009c` past its 4 declared params.
- `0x0056aae0` REP MOVSDs 0x22 words from `&stack0x0000003c` before its call to `0x0056a7a0`, and `0x0056adb0` copies
  0x22 words from `&stack0x0000000c` before its call to `0x0056aae0` and stores the return addresses `0x56af69` /
  `0x56b16d` into its "locals" before both `0x0056ac40` calls (outgoing frames built in place).
- [UNCERTAIN U-9589 / U-9590] `0x0056a7a0`'s deepest read is `in_stack_0000009c` (word 39). 3 pushed args + 34 copied words +
  `fStack_18` + `iStack_14` in `0x0056aae0` is also 39 words, which would bind `in_stack_00000098` to `fStack_18` and
  `in_stack_0000009c` to `iStack_14`. That is a count from the decomp text only. The push sequence at the call
  sites (and `ret`/`add esp` cleanup) must be read from disassembly before any port.
- Three plates carry U-9589..U-9592 for the same esp-depth gap.

Proposed next step: a disassembly-led port of the four (by-value frame structs as in `RwpVtableKV1.cpp` / K13's
`Kv11Frame`), float10 return of `0x0056ac40` handled as in `RwpSolverCore23.cpp`'s `RwpVolFn24`, then the same
shadow arm (`0x0056ac40` expects `DIVERGENT_FLOAT10`-class limits on a float10 return, see `shadow_ab_report.py`).

## Files

Committed: `PREREG_D4-WSB-PORT-SOLV.md`, `RESULT_D4-WSB-PORT-SOLV.md`, `shadow_summary.tsv`, `run_shadow6.py`
(copy of `verify/wsbstackonly_20261028/run_shadow5.py`, data path and label changed), `drive_ps.py` (copy of
`verify/wsbstackonly_20261028/drive_so.py`, runner name, data path and label changed).

Raw (not committed), `D:/MashedWT/D4-WSB-PORT-SOLV_data/`:

| file | SHA-256 |
|---|---|
| `005663a0/shadow_ab.log` | `dfdc9925c1e22e68d59f204e82ff59a18e163dfb4a3c1b0abe8700b00b7cd6c3` |
| `005672c0/shadow_ab.log` | `af8fc5f2efd08a05c755b5359a5ba77fd653570135df3d7507f992fa65a82acf` |
| `00567350/shadow_ab.log` | `ce3611b78657d11c9cce9378f055bf10a8eb5878f3e254d284056a626ed18d6e` |
| `00567630/shadow_ab.log` | `267f60ec4dfa77324baec3efa1c7a98aa819765973d50a8078799b2c3ea8e747` |
| `005676f0/shadow_ab.log` | `bc47538dcfff0f3dba292d60b4853482a1ce7364dd657f57e0506c9a2d47e122` |
| `00567c40/shadow_ab.log` | `e495ae6de21b2e0309c294fcf06a2b95ba70ac1709e2f6ac1ddfb38c741d5e43` |
| `005687a0/shadow_ab.log` | `1f5b3e45114f70be856a5280c27559fa0fc66ef4c34a0c02552b963ae954f698` |
| `005687d0/shadow_ab.log` | `155460c1f6cee52b990bb8401cc6191a91573208e56dfd3eea0e2cf677cbfcdd` |
| `00568860/shadow_ab.log` | `acfbe6c9211df98865dde28749b52ec01ff68f56a0be70105e33ad55dbb8da2c` |
| `00568c40/shadow_ab.log` | `0f544cf9cabbbb640961cccb556189b690d039caa59de0301011024d46c66932` |
| `00568c80/shadow_ab.log` | `32678f06b1f5c1e06df3ee3bdcabff9f911be253345446c596433d5eed8597c0` |
| `00568cf0/shadow_ab.log` | `e6be0181e7f05f8e93a2735dcc8086f1520a4edf4946d1337f1fdcd3a030bb3c` |
| `00568d20/shadow_ab.log` | `c58ff9fcc77b6346143c439c7cfcd8250affcc319eef1a55a07d8c5b244badbc` |
| `0056a450/shadow_ab.log` | `792a8b566d492cffdbf5992c18b9ac762eb65deb3ab5a0b1331bd55bf6e1c949` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `bootcheck.txt` | `0657b33a7e106f14223d0d5e82bf4187020e8cc203a155a0a994ed474b5c14cc` |

Game PIDs spawned (only these killed, by PID): boot check 15532; shadow 13268, 11340, 11044, 21320, 15512, 5108,
21204, 9424, 14316, 20300, 19940, 19204, 18536, 6612; stopped 17964; no-attach 11308 (self-exited); gates 4060, 9504,
4908, 14712 (self-exited). No watcher left running.
