# RESULT PORT-RENDER-01 (2026-11-08)

Lane A swap (verify/portplan_20261108/PLAN.md section 4) of the 9 render-lane C4-by-match rows whose exe body is a
CALLED hand port. Branch `orch/PORT-RENDER-01` off `orch/integration` 80d933fa7.

**Verdict: GREEN, 1 of 9 swapped, 8 NOT-SAFE.** Default gates unchanged and the Arctic drawlist matches the
reference row for row.

## Swapped (swaps.tsv, relocheck.tsv)

| rva | name | matched source | wrapper | port before -> after |
|---|---|---|---|---|
| 0x004769f0 | ParticleEmitter_SetColour | re/match/s12/004769f0.c | Match/MatchS12_004769f0.cpp | called -> called |

- `match.py re/match/s12/004769f0.c --bytes`: 100.0% (10/10 ins), 30 B whole symbol BYTE-IDENTICAL. `relocheck.py`:
  OK checked=3 unchecked=0.
- The body has no callees. It reads one global and writes one: DAT_00692554 binds to 0x00692554 in both targets (the
  exe's image-pad owns 0x006924d8..0x006925a8). DAT_00613290 (the null-pointer default) binds per target, the
  MatchS06_004282a0.cpp shape (the TU supplies MATCH_PREAMBLE and an MGLOBAL over `MATCH_CELL_<name>`). The .asi
  reads [0x00613290]. The exe reads `g_rfxDefault00613290` = 0xffffffff, the anchored binary's file-backed value
  (the exe's image-pad holds 0 at 0x00613290). The replaced hand port read the same two values
  (`param_1 ? param_1[0] : *k00613290`). Render/RfxEmitter.cpp now names that exe default `g_rfxDefault00613290`
  (extern "C"); its other reader, 0x00476b30, still reads it through `k00613290`.
- Compiled body checked in both binaries (capstone on the export): `test ecx,ecx`, the default through a cell, store
  through a cell. The exe map places `_g_rfxDefault00613290` in RfxEmitter.obj.
- x87_tus.txt += MatchS12_004769f0. Both rsp files list the wrapper after MatchS12_004769a0.cpp.
- rva-lint: OK, no NEW duplicate bodies (1 known, allowlisted). 004769f0 was not an allowlist key, so the allowlist is
  unchanged.
- backfill_exe_file.py moved exe_file to Match/MatchS12_004769f0.cpp. backfill_port.py: already current, port stays
  `called`. The hooks.csv `file` column was moved to the wrapper through re-classify (file-column leg, C4 unchanged,
  same as the S12 swaps 0x004769a0 / 0x00476a10).

## NOT-SAFE (left as they are)

| rva | exe body | reason |
|---|---|---|
| 0x00448940 ArcticTrackNodeSlot0 | Render/ArcticTrackNodeSlot0.cpp | In the exe the export is the S-5713 stub (it returns at once). The matched body calls 22 callees by MASHED address (0x0049a0e0, 0x00496d00, 0x00484000, 0x00448770/0880/0820, 0x004b3f90, 0x004e5fc0, 0x004e45b0, 0x004c15c0, 0x004c1340, 0x004c1b10, 0x004671a0 and others). None of them has an exe body (hooks.csv exe_file empty) and none is one of the 6 RVA thunks, so they cannot be bound by name. |
| 0x00448dc0 | D3d9Render/TrackRenderer.cpp `UpdateCamMen` | The exe body is the generic camera-man loop for 12 helpers. It writes port D3DMATRIX into librw prop instances (`PlaceMatrix`). The matched body writes RW frames at `[DAT_00896460[i] + 4]`, an array the exe never fills. Its callees 0x004c15c0 / 0x004c1520 / 0x004c1340 have no exe body. The hand port draws through librw in a way the matched RW calls cannot reproduce. |
| 0x004756e0 | D3d9Render/TrackRenderer.cpp `ClumpAnimTick` | Callees 0x004752f0 (C2), 0x004b47e0 and 0x004c1480 have no exe body. The hand port runs on port-owned C++ state (MATCH-S12 RESULT line 116). |
| 0x00476df0 | Render/RfxEmitter.cpp `Rfx::Flush` | Callees 0x00535700 (C2) and 0x00535910 have no exe body. The hand port runs on port Emitter / PTank std::vector storage (MATCH-S12 RESULT line 114). |
| 0x00498a00 | Boot/VideoCfgFirstRun.cpp `ModeLabel` | The matched file re/match/s16/00498a00.c is one TU of 10 functions. 0x00498a00 is `static` and takes its Mode* in EBX. Compiling it compiles the whole dialog file, with callees that have no exe body (hooks.csv exe_file empty): 0x00499740 SetControlTextFromResource_i3, 0x004a45fb _malloc, 0x004a2b60 sprintf, 0x004c2e70 RwEngineSetSubSystem, 0x004c2ed0 RwEngineGetModeInfo. (0x00496400 / 0x00498950 / 0x004989b0 do have exe bodies in Save/SettingsConfig.cpp.) The hand port takes a port `VideoMode` from a std::vector (port-side rebind). |
| 0x00498e40 | Boot/VideoCfgFirstRun.cpp `DefaultModeFind` | Same file as above. The matched body reads the tables DAT_007731f8 / DAT_00773408 / DAT_0077340c, which have no writer in the exe (0x00498c00 does not run there). The hand port reads the port std::vector<VideoMode>. |
| 0x004b4b60 | Powerup/PowerupContact.cpp `SweepQuery` | The matched body tails into 0x004b4a80, which has no exe body. The hand port is a stand-in with a test injector (`s_sInject`, the harness that returns 0 when no injector is set), not a body that does the same work. |
| 0x004c3c60 Vec2Normalize | Math/RwV2d.cpp | The hand port carries the RwLutGuard std::sqrt fallback (no engine LUT means `std::sqrt` / `1/mag`). That belongs to MATCH-S19-FU-MATHSAFE, so it is left there. The matched source is also 97.3%, not byte-identical (one x87 operand order of out->y, MATCH-S19 RESULT line 66). |

Port column for all 9 rows: `called` before and after (8 untouched, 1 swapped).

## Gates (worktree exe sha8 d5049754, .asi 3d613049, test-slot + mashed_lock, private root D:/MashedWT/PORT-RENDER-01_root)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`:

- G-CTRL `8284be46/67931834/31fe043e` (ref 8284be46/67931834/31fe043e): unchanged
- B `1d5bf4f9/1d5bf4f9/1d5bf4f9` (ref 1d5bf4f9 x3): unchanged

No flip candidate.

## Drawlist (Arctic, matched-pose replay)

`verify/rsky_20261016/run13.py run --tracks 3`, then `score --cite verify/rres_20261016/cites_rres.json --tracks 3`.
Arctic is the track of 0x00448940 and has particle emitters.

| row | orig | matched | m.tri% | rows | cited | red | verdict |
|---|---|---|---|---|---|---|---|
| 1051 | 180 | 158 | 93.9% | 32 | 32 | 0 | CITED |
| 1091 | 177 | 152 | 92.7% | 36 | 36 | 0 | CITED |
| 1126 | 131 | 108 | 92.8% | 36 | 36 | 0 | CITED |

INSTRUMENT GATES: PASS. Every row equals the reference verify/run13p3_20261029/tracks_score_sub_p3.txt.

## Raw evidence (not committed)

```
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  D:/MashedWT/PORT-RENDER-01_data/arms/H_DEF.step.csv
6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c  D:/MashedWT/PORT-RENDER-01_data/arms/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  D:/MashedWT/PORT-RENDER-01_data/arms/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  D:/MashedWT/PORT-RENDER-01_data/arms/B_DEF_K0.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  D:/MashedWT/PORT-RENDER-01_data/arms/B_DEF_K1.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  D:/MashedWT/PORT-RENDER-01_data/arms/B_DEF_K26.step.csv
f7e4678a3b0faa9295966dc0b511d3235a89ff753bd459eb952892c9169e3119  D:/MashedWT/PORT-RENDER-01_data/run13_score.log
```
Run13 capture directory: D:/MashedWT/PORT-RENDER-01_data/run13/03_Arctic.

## Next

The 8 NOT-SAFE rows need port bodies for their callees first: the RwFrame family 0x004c15c0 / 0x004c1520 / 0x004c1340
for 0x00448dc0, 0x004752f0 / 0x004b47e0 / 0x004c1480 for 0x004756e0, 0x00535700 / 0x00535910 for 0x00476df0,
0x004b4a80 for 0x004b4b60. 0x00498a00 / 0x00498e40 need the whole video-dialog file ported with an exe writer of the
mode tables. 0x004c3c60 belongs to MATCH-S19-FU-MATHSAFE.
