
## leaderboard-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/leaderboard/
**Parent:** leaderboard-20260506-JJJJJ (D=6580..6589)
**Subsystem:** util

Depth-2 deferred callees from leaderboard session JJJJJ:

| D | Address | Description |
|---|---------|-------------|
| D-6580 | 0x0040e560 | FUN_0040e560 — intermediate caller between TimeTrial tick and Replay::LapFinish; confirm call chain |
| D-6581 | 0x0040fc00 | Race main tick (620b, ~30 callees); calls TimeTrial::Tick; broad |
| D-6582 | 0x00430b00 | FUN_00430b00 — HUD time display updater (S-2220); resolve split/best display path |
| D-6583 | 0x0042f790 | FUN_0042f790 — ghost-mode flag getter (S-2225) |
| D-6584 | 0x0040d040 | FUN_0040d040 — car finish validator (S-2226) |

---

## audio_music_d2-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/audio_music_d3/
**Parent:** audio_music_d2 (D-6520..D-6521, bucket audio_music_d2-cont1)
**Subsystem:** audio

Depth-3 callees from audio_music_d2. All items deferred:

| D | Address | Description |
|---|---------|-------------|
| D-6520 | 0x005a7520 | Audio state dispatcher: mode 0→FUN_005a7560, 1→FUN_005a7460, 2→FUN_005a75b0 (conditional on node+0xc bit-0) |
| D-6521 | 0x005a6d60 | Actual audio parameter setter (4 args; param_4 by address); resolve U-2209 U-2210 |

---

## track_loader_d3-cont1  [queued 2026-05-06]

**Bucket:** re/analysis/track_loader_d4/  
**Parent:** track_loader_d3 (D-5740..D-5755, bucket track_loader_d3-cont1)  
**Subsystem:** render/track  

Depth-4 callees from track_loader_d3. All 16 stubs deferred:

| D | Address | Description |
|---|---------|-------------|
| D-5740 | 0x004b3c60 | BSP/RpWorld stream reader |
| D-5741 | 0x00558df0 | UVAnim chunk loader |
| D-5742 | 0x004b3cc0 | Spline stream reader |
| D-5743 | 0x004b3de0 | Animation stream reader |
| D-5744 | 0x00479030 | Course post-load callback table |
| D-5745 | 0x00474fb0 | DFF clump node iterator |
| D-5746 | 0x00474f30 | Sky dome per-node callback |
| D-5747 | 0x0047f4c0 | Physics world constructor |
| D-5748 | 0x0047d080 | Activate physics body slot |
| D-5749 | 0x0047d100 | Secondary enable physics body |
| D-5750 | 0x00487280 | Broadphase body registration |
| D-5751 | 0x0047be80 | Triangle mesh init |
| D-5752 | 0x0047bcc0 | Collect portal/neighbor list |
| D-5753 | 0x004b53b0 | Bounding sphere builder |
| D-5754 | 0x004c3d90 | Sector bounding geometry builder |
| D-5755 | 0x00546380 | Audio waypoint set constructor |

