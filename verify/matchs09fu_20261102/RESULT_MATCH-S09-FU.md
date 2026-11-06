# RESULT MATCH-S09-FU, 2026-11-02 (gates and C4 2026-11-06)

**Verdict: GREEN.** Three of the seven S09 NOT-SAFE rows are swapped in (0x00458920, 0x00451ce0, 0x00451690), merged
onto `orch/integration` 45821300e, rebuilt, byte match and relocheck re-confirmed, gates equal to the refs, and each
body ran in a knob-free default run. All three re-classified to **C4 (route match)** through re-classify. The other
four S09 NOT-SAFE rows stay unswapped (table below).

Branch `orch/MATCH-S09-FU`: cut from `orch/MATCH-S09` @ 632669f29, source commit 86a3e7017, RESULT 98501779a (BLOCKED
2026-11-02, display off line), merge of `orch/integration` 45821300e = 8daa9e709 (MATCH-S09 is in integration since
merge train 20261106_015649).

## Merge of orch/integration (2026-11-06)

One conflict, `re/tools/naked_specs/pickups_m3.json` `funcs`: this branch had removed `PK_458920`, integration had
removed `PK_46d4a0` (D4-DC-INC-0046d4a0-REPLAYORDER, cd148e9ec; 0x0046d4a0 is now the call bind `PtrCompute881ec8`).
Resolved to both removals (`PK_458570` alone). `gen_naked_asm.py` on the resolved spec reproduces the auto-merged
`Gameplay/PickupCollect_naked.inc` byte for byte. `PickupTick.cpp`, `exe_sources.rsp`, `x87_tus.txt` auto-merged.
Post-merge grep: the exe readers of 0x005ce548 are still only the 00458920 wrapper (which seeds it) and the naked tick's
own `g_pk_c_5ce548`.

| check (on 8daa9e709) | result |
|---|---|
| build `set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` | Build OK, .asi not deployed |
| rva-lint / asm-lint | 1 finding CROSS-TARGET, allowlisted=1 NEW=0 / OK, 1206 files |
| `match.py --bytes` 00458920.c / 00451ce0.c / 00451690.c | 100% 83/83, 38/38, 17/17 ins, BYTE-IDENTICAL 276 / 151 / 49 B (`/O2 /GS /Ob1 tc=ddk`) |
| `relocheck.py` | OK checked=10 / 4 / 2, unchecked=0 |
| src sha256 | 00458920.c `d78f31ac94e72459e44659e273c5b651de96e6b970d76c67a3a1b578f8846e67`, 00451ce0.c `55c4f16ec5a2ccc27015329432eca95a779ded217c90c1b7de55953aeffcf979`, 00451690.c `4705c10b7d72aab11cd79ec02899cff935b51633e1618f98d9b514e28923bb14` |

exe sha256 `38be833be078657aa3bad0c04ce8b310697cfb4231fe275d7fc868660e0a3bcd`, asi
`73d6a20efffd571863cad16d887ec69212c0ca9368efc78410b9fa433be42382`. Integration comparison exe (git archive of
45821300e, built the same way in `D:/MashedWT/MATCH-S09-FU_data/intbuild`)
`debb8205c6d78623263ee4f47863c7b6d097724857df100fe7a47fac22f980ca`.

## Gates (2026-11-06, exe 38be833b, root D:/MashedWT/MATCH-S09-FU_root, test slot outside, mashed_lock inside)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, every arm exit 0, self-exited.

| arm | file | sha8 | ref (merge_train.json) |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

`vsum.py`: B_DEF_K0/K1/K26 3/3 car1-3 A PASS. `ai_speed_env.py --check`: v2 (e) PASS, v3 (e) PASS.
Not run here: D2 x3 and the 13-track set (`mashedmod/src` is in `full_set_paths`, so the merge train runs `-Full` on
enqueue), same as MATCH-S09.

## Execution in default runs (the C4 "body runs" leg)

Observation runs on the same exe, knob-free except a trace file, run through
`D:/MashedWT/MATCH-S09-FU_data/tools/run_arms_fu.py`, which execs run_arms.py unchanged plus one stack
`PKT = MASHED_PKCB_TRACE=<out>/<arm>.pkcb.txt`.

| rva | run | evidence |
|---|---|---|
| 0x00458920 | H_PKT (the G-CTRL recipe + PKCB trace) | step / gates / ptrace = 8284be46 / ba407a16 / 31fe043e (the trace changes nothing); 2 `PK` rows = 2 Pickups_Callback -> PK_458920 calls (ticks 919, 1025) |
| 0x00451ce0 | W_DEF (Warzone, rule 8, 3600 frames) and WL_DEF (18000) | pontoon trace: state 0 on every logged tick of W_DEF; StateMachine state 0 calls Sample (`Track/WarzonePontoon.cpp:452`), states 1..4 also call it |
| 0x00451690 | WL_DEF | pontoon trace transitions: 0->1 x2, 1->2 x2, 2->5 x2, 5->0 x2. The only port writer of state 5 is the matched LoopCb (`re/match/s09/00451690.c:36`, `*(param_2 + 0x48) = 5`). WarzonePontoon.cpp writes 0/1/2/3 only (lines 372, 461, 481, 714). W_DEF (3600 frames) never left state 0 |

G-M3-REPLAY (`pkcb_replay.ps1`, under the lock): live 6/6 exact (collect=6) and synthetic 3888/3888 exact (bump=60
collect=60 no-op=3522 vanish=246, the vanish arm reads 0x005ce548), entry_bad=0 calls_bad=0, at PC_24 and PC_53.
Same as `verify/d3_mode7_m3_20261010/m3_replay_*`.

### Warzone A/B (this exe vs the integration exe, same recipe and seed)

| arm | file | this exe 38be833b | integration debb8205 |
|---|---|---|---|
| W_DEF | step.csv / pontoon.txt / seek.txt | eeb92153 / a329ca22 / fa8618df | eeb92153 / a329ca22 / fa8618df |
| WL_DEF | step.csv / pontoon.txt / seek.txt | 5e7ec010 / 9e5d825f / 73979f14 | 5e7ec010 / 9e5d825f / 73979f14 |

The swapped pontoon bodies give the same Warzone run as the bodies they replaced, the two state-5 passes included.

## Re-classify (applied 2026-11-06, re-classify skill, match route)

| rva | name | old -> new | exe callers (reach WinMain) |
|---|---|---|---|
| 0x00458920 | FUN_00458920 | C2 -> C4 | Pickups_Callback <- RG_CbDispatch <- ... <- RenderFrame <- WinMain |
| 0x00451ce0 | FUN_00451ce0 | C2 -> C4 | StateMachine <- Pontoon_Tick <- TrackRenderer::CopterTick <- ... <- WinMain |
| 0x00451690 | WarzonePontoonAnimLoopCb | C1 -> C4 | LoopCb (registered in InitFrom) <- Pontoon_Init <- TrackRenderer::Load <- WinMain |

`file` / `exe_file` moved to the Match wrappers, status `impl`, the `c4=match` evidence string in `notes`, three
CHANGELOG lines. No stubs in the bodies, no unfiled `[UNCERTAIN]` markers, no new U-/S- rows.

## Swapped (exe only, as the bodies they replace)

| rva | was | now | callee binds | exe caller (no replay, L-89) |
|---|---|---|---|---|
| 0x00458920 | naked `PK_458920` (Gameplay/PickupCollect_naked.inc) | `Match/MatchS09_00458920.cpp` as `PK_458920` | Pred405890, SubsystemARecordKey, PK_SubMode, PredNot45c330, PK_Burst, PK_Grant, PK_Sfx (the naked copy's own binds); held 0x00458570 through an EAX/ESI forwarder to PK_458570_w | Pickups_Callback <- RG_CbDispatch <- RG_484de0 <- RG_485070 <- Race::Registry_FrameTick <- TrackRenderer::UpdateCar <- RenderFrame <- WinMain |
| 0x00451ce0 | WarzonePontoon.cpp Sample | `Match/MatchS09_00451ce0.cpp` (`WarzonePontoonSample`) | 0x00550bc0 = Save/VfsStream.cpp VfsStreamGetType (its one port body, `return *(p + 8)`) | StateMachine <- Pontoon_Tick <- TrackRenderer::CopterTick <- UpdateCar <- RenderFrame <- WinMain |
| 0x00451690 | WarzonePontoon.cpp LoopCb | `Match/MatchS09_00451690.cpp` (`WarzonePontoonLoopCb`) | 0x0052df70 / 0x00533ec0 = forwarders to the TU's SubAnimTime / UpdateMatrices transcriptions | LoopCb (pointer registered in InitFrom, called by AddAnimTime / SubAnimTime) <- InitFrom <- Pontoon_Init <- TrackRenderer::Load <- WinMain |

Chains: `D:/MashedWT/MATCH-S09-FU_data/tools/exe_reach_fu.py --exclude Replay` (S09's `exe_reach.py` plus function-pointer
edges and a name filter that drops the env-gated replays `PickupCb_MaybeReplay` / `RunReplay`), output
`exe_reach_noreplay.txt` below. Without the filter the shortest chains of 0x00458920 and 0x00451ce0 run through those
replays, which L-89 does not count.

What each swap needed:
- **0x00458920.** `PK_458920` left `re/tools/naked_specs/pickups_m3.json` and the .inc was regenerated with
  `gen_naked_asm.py` (before the edit the generator reproduced the committed .inc byte for byte). PickupTick.cpp now
  declares `PK_458920` itself. The source reads 1.0f at `.rdata` 0x005ce548 (`fsub` at 0x00458979). The exe arena
  holds 0 there, so the wrapper seeds the original's 0x3f800000 at static init. No other exe code reads 0x005ce548
  (grep: the naked tick uses its own `g_pk_c_5ce548`). Chosen over a rebind because the `MGLOBAL` line cannot be
  rebound without editing the matched source.
- **0x00451ce0.** S09 called this NOT-SAFE because the port's +0x50 is its own `Hier`. The source calls 0x00550bc0 on
  obj +0x50 and uses the result + 0x40 as node 1's matrix. 0x00550bc0 is `mov eax,[eax+8]` (port body
  VfsStreamGetType, C4), so the port `Hier` now holds `&mats[0]` at +0x08 (`matArray`, set in InitFrom), and +0x40
  is `mats[1]`, the matrix the old Sample read. Obj offsets +0x38/+0x50/+0x84/+0x94/+0xa8 match the source's struct
  (Obj static_assert).
- **0x00451690.** The source reads the interpolator at hier +0x20. The port `Hier` now has `interp` at +0x20
  (static_assert); it already was the interpolator the old LoopCb was handed (`s.hier.interp = &s.interp`). Return
  value: the source returns param_1 (the interpolator, non-zero), the old body returned 1; the only reader tests == 0.
- All three wrappers are in `mashedmod/x87_tus.txt`.

## Still NOT-SAFE (not swapped)

| rva | reason |
|---|---|
| 0x00451820 Trigger | reads car count / matrices through 0x0040e340 / 0x0046d4a0 where the port uses PontoonHost callbacks; 0x0046be10 inlined in the source has no port body |
| 0x00451ad0 Coupling | printf, 0x0046bf50, the four wheel blocks and 0x00480720 have no exe bodies (U-9738) |
| 0x00451d80 StateMachine | drives the RW anim objects through 0x0052e0e0 etc.; state 5 uses host sphere_visible (U-9740) |
| 0x00452170 ResetCb | 0x0047d180 x2 (RWP body update, none in the exe) and 0x004518f0 have no exe bodies; also no exe caller: the port stores the pointer at obj +0xc4 and nothing calls it, so a swap would be callerless (L-68) |

## Static checks (on 86a3e7017, before the merge)

| check | result |
|---|---|
| `match.py` 00458920.c / 00451ce0.c / 00451690.c | 100% 83/83, 38/38, 17/17 ins (`/O2 /GS /Ob1 tc=ddk`) |
| `relocheck.py` | OK checked=10 / 4 / 2, unchecked=0 |
| build `set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` | Build OK, .asi not deployed |
| `lint_rva_bodies.py` | 2 findings, allowlisted=2, NEW=0; `rva_body_scan`: exe 0x00458920 / 0x00451ce0 / 0x00451690 one body each (the Match wrappers), 0x00550bc0 one body (VfsStream.cpp) |

exe sha256 `b933f964c2574a4bfccd9cf994748f1fa7fed6ebcf6cd510aae7c196947f4e77`, asi
`226df1601de1991ae8bea62052cab0a7176fd742641e995ac6240c0202e00238`.

## 2026-11-02 attempt (history)

First attempt 09:43 (`gates.ps1`, the merge train's default + `-Full` set): every arm `exit=3` within 3 s,
`CreateDevice` 0x8876086A on all eight device variants. Probe 11:08 (H_DEF alone, under the lock): same. Then polled
the display every 5 min for 30 min (11:13 .. 11:44, seven reads, all Availability 8 and no root with a new successful device). Other sessions' roots show the same failure
in the same window (MATCH-PORTSTATE-RECORDS 09:43, MATCH-T13-SEED-WRITER 09:45 / 10:47, MATCH-S10 11:05,
D4-FLIP-MATCHEND-ST3PRETICK 11:07).

## Raw files (not committed, `D:/MashedWT/MATCH-S09-FU_data`)

| file | sha256 |
|---|---|
| build2.log (Build OK) | 2d4ca8e2088ef1306822447173b570fd45f38df67dc17e2d8623be83f87cb8c3 |
| exe_reach_noreplay.txt | 39d871f2e5aec1df8d9dce386e09b4aba2e7d7cb49f436533ffed3369285dd2e |
| tools/exe_reach_fu.py | bcdeabcd1feabd9c011e7efb0ebdaa5cc5014b000e44721001cc87197b027f22 |
| gates.ps1 | f74a6feffeb2ee1fb52bc8a21b8c979ecddefbdcae5a9e9bfcfd1cf7f7fce5e1 |
| pkcb_replay.ps1 | ff645a9edea3b8a2b115396e9cb84ef26a739080fd0bc5b0814de2943cdf9c62 |
| gates.log (the 09:43 attempt) | c443a2dcb2ef4350909c14e62a170d8eaede30f22ac42172893886086e7cf49c |
| build3.log (post-merge Build OK) | 6990c998fc4df1066f29cc0d924795ff3a4f94a77e1aa44a081d4631fbc9f038 |
| intbuild/build.log (integration 45821300e) | d4c2c406918e54bc77513aa0041a8d6de32250c5f0d704d03f1d18b7458eb04b |
| intbuild/mashedmod/build/mashed_re.exe | debb8205c6d78623263ee4f47863c7b6d097724857df100fe7a47fac22f980ca |
| match_postmerge.txt | f7e6117499cc71a94db076de85b62b29897f482cda26ed7ffb48d9581b8d9bf0 |
| relocheck_postmerge.txt | dc0bc331c5ef301f7d46d4e1e59da34aab089988c9a26dec36d77d9c68aab8b3 |
| gates2.ps1 | 5e1d1c681b0d9bcd3246025fd9482c67abc537c9e81af5cdf3b4fa01084e03bd |
| gates2.log | 6813f5e2bbd5e809c1e66988314c407c1a674ac0cfdcad4347dbb7d7828021e3 |
| gates3.ps1 (WL_DEF pair) | 8c75e014548c601db562a3c5cfef179d36fba2807a7835db007a06e2f7fc2bb4 |
| gates3.log | ca85028ab897c8a5611a6d3ec040eb3188e062df1717c7e32c6d8f8986e263a6 |
| tools/run_arms_fu.py | 8bbe67a5c2c97f40a25c10e0a8da4ec221a0fb67b19ebd956c300c175ce8d945 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv (K1, K26 identical) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/H_PKT.pkcb.txt | 54f4c5585fcaf77368ff0ed86349602d1f8cdba4289d5df15641d75b126a6976 |
| arms/W_PKT.pkcb.txt | 8429a95af5c23a8ccfa80a85852163fa6d8d2fe7c9e0b83cf0d374360791fa50 |
| arms/W_DEF.step.csv (= arms_int) | eeb921531da3e3086f8c63eb57c2821c163cf4710a67e83d953813ccf84eb955 |
| arms/W_DEF.pontoon.txt (= arms_int) | a329ca226e5b6908123987ef3a8518258385d5202dd79552217dadc593373d31 |
| arms/WL_DEF.step.csv (= arms_int) | 5e7ec010ad9c135dd08a19ad87303cd508c2dab03099d8b8c912b5764da923ef |
| arms/WL_DEF.pontoon.txt (= arms_int) | 9e5d825fad8e48ccd98a1ab8034ffc3b375b1ac41b72e58a3995e2e1ca17075b |
| pkcb/m3_replay_live_pc24.txt | 9275868cc55ed1826945986f64a98196c3613a50f6dfc62976e1d57657e3e722 |
| pkcb/m3_replay_live_pc53.txt | 436aef3ebfe11fd7a3783846983ebb09548519f28f0c8cf1d418d8753658dd2a |
| pkcb/m3_replay_synth_pc24.txt | 6749b83d9a0d7058bc504faea0d98b0a9d78afabfd1d28199c5219e799e98403 |
| pkcb/m3_replay_synth_pc53.txt | 67e4870a451a9e545a571858e5abc092c3dbe2a462d5003af2a35f15621fa42e |
