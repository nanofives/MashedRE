# RESULT MATCH-S09-FU, 2026-11-02

**Verdict: BLOCKED (gates not run).** Three of the seven S09 NOT-SAFE rows are now swapped in (built, byte match and
relocheck re-confirmed, one body per RVA). The gates could not run: since 09:37 at the latest every standalone boot on
this machine, mine and other sessions', fails `CreateDevice` with 0x8876086A (D3DERR_NOTAVAILABLE,
`re/diag/KNOWN_ISSUES.md` D3D9-NOTAVAIL) and `Win32_DesktopMonitor.Availability` reads 8 (off line). The last
successful device in any root was `MATCH-S09-REMERGE_root` at 08:17. No re-classify was applied (no gates).

Branch `orch/MATCH-S09-FU`, cut from `orch/MATCH-S09` @ 632669f29 (S09 is in review, not merged into
`orch/integration`; re-merge onto integration once it has). Source commit 86a3e7017.

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

## Static checks (on 86a3e7017)

| check | result |
|---|---|
| `match.py` 00458920.c / 00451ce0.c / 00451690.c | 100% 83/83, 38/38, 17/17 ins (`/O2 /GS /Ob1 tc=ddk`) |
| `relocheck.py` | OK checked=10 / 4 / 2, unchecked=0 |
| build `set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` | Build OK, .asi not deployed |
| `lint_rva_bodies.py` | 2 findings, allowlisted=2, NEW=0; `rva_body_scan`: exe 0x00458920 / 0x00451ce0 / 0x00451690 one body each (the Match wrappers), 0x00550bc0 one body (VfsStream.cpp) |

exe sha256 `b933f964c2574a4bfccd9cf994748f1fa7fed6ebcf6cd510aae7c196947f4e77`, asi
`226df1601de1991ae8bea62052cab0a7176fd742641e995ac6240c0202e00238`.

## Gates: not run

First attempt 09:43 (`gates.ps1`, the merge train's default + `-Full` set): every arm `exit=3` within 3 s,
`CreateDevice` 0x8876086A on all eight device variants. Probe 11:08 (H_DEF alone, under the lock): same. Then polled
the display every 5 min for 30 min (11:13 .. 11:44, seven reads, all Availability 8 and no root with a new successful device). Other sessions' roots show the same failure
in the same window (MATCH-PORTSTATE-RECORDS 09:43, MATCH-T13-SEED-WRITER 09:45 / 10:47, MATCH-S10 11:05,
D4-FLIP-MATCHEND-ST3PRETICK 11:07).

## To resume (display on)

1. `py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py status`, then
   `py -3.12 C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py pwsh -NoProfile -File D:\MashedWT\MATCH-S09-FU_data\gates.ps1`
   (G-CTRL + B, vsum, ai_speed_env, D2 x3 solo with short reruns, 13 tracks under a solo token with
   `MASHED_PONTOON_TRACE`; it aborts after the default arms when H_DEF writes nothing). Compare with
   `merge_train.json` refs and `verify/run13p3_20261029/tracks_score_sub_p3.txt`.
2. `pwsh -NoProfile -File D:\MashedWT\MATCH-S09-FU_data\pkcb_replay.ps1` under the lock: G-M3-REPLAY live (6 rows) and
   synthetic (3888 rows, 246 of them the vanish arm that reads 0x005ce548) at PC_24 and PC_53, expected all exact as in
   `verify/d3_mode7_m3_20261010/m3_replay_*`.
3. The pontoon trace from the Warzone track shows whether 0x00451690 ran (`state ->5`; only LoopCb writes 5);
   0x00451ce0 runs every Pontoon_Tick in states 0..4.
4. Gates equal to refs: re-classify the three rows to C4 (route match) through `re-classify`.

## Raw files (not committed, `D:/MashedWT/MATCH-S09-FU_data`)

| file | sha256 |
|---|---|
| build2.log (Build OK) | 2d4ca8e2088ef1306822447173b570fd45f38df67dc17e2d8623be83f87cb8c3 |
| exe_reach_noreplay.txt | 39d871f2e5aec1df8d9dce386e09b4aba2e7d7cb49f436533ffed3369285dd2e |
| tools/exe_reach_fu.py | bcdeabcd1feabd9c011e7efb0ebdaa5cc5014b000e44721001cc87197b027f22 |
| gates.ps1 | f74a6feffeb2ee1fb52bc8a21b8c979ecddefbdcae5a9e9bfcfd1cf7f7fce5e1 |
| pkcb_replay.ps1 | ff645a9edea3b8a2b115396e9cb84ef26a739080fd0bc5b0814de2943cdf9c62 |
| gates.log (the 09:43 attempt) | c443a2dcb2ef4350909c14e62a170d8eaede30f22ac42172893886086e7cf49c |
