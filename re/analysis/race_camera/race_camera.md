# The shared race camera + elimination rule — RE blueprint (2026-06-10)

Decompiled in Ghidra (Mashed_pool0, read-only) from the SHA-256-anchored
binary. Disassembly evidence: `FUN_00446520.asm`, `FUN_00410d10.asm`,
`FUN_00442a60.asm` (this directory, capstone over the anchored bytes).
Entry anchors located via SciLor MFL.CT byte signatures re-matched to our
build (`re/tools/trainer_anchor_check.py`): camera-height block at
0x00447578, elimination compare at 0x00410ee3.

## Cast

| RVA | Name | Role |
|---|---|---|
| 0x00446520 | `FUN_00446520(int* cam, int force_reset)` | camera director (7411 B); race branch = shared race camera |
| 0x00448220 | `FUN_00448220` | per-frame camera dispatcher; calls 0x00446520 with `&DAT_00897fe0` |
| 0x00897fe0 | `DAT_00897fe0` | THE camera struct (global) |
| 0x00442a60 | `FUN_00442a60` | per-player planar-distance array (feeds elimination + HUD) |
| 0x0040e180 | `FUN_0040e180(&a,&b)` | most-separated pair finder (432 B, XZ distance via Vec3Magnitude) |
| 0x00410d10 | `FUN_00410d10` | round-end check incl. THE ELIMINATION RULE |
| 0x00441820 | `FUN_00441820(node, out_dir, out_h)` | per-node camera direction sampler |
| 0x00441760 | `Camera::Apply` | cam struct → RW frame (yaw→pitch→roll + translate) |
| 0x0040eee0 | `FUN_0040eee0(car, delta)` | race-scoring callback (3356 B) — points system entry |

## Camera struct fields (byte offsets from 0x00897fe0)

- `+0x1c` ([7]) mode flag — race branch requires ==0; getter `FUN_00443080` (0x00897ffc)
- `+0x34/0x38/0x3c` elevation° / azimuth° / roll° (written at 0x00448120..0x00448180 region; consumed by `Camera::Apply` 0x00441760)
- `+0x40..0x48` position; `+0x4c..0x54` look-target ([0x13..0x15])
- `+0x58` ([0x16]) view-window scalar: 0x3f19999a (0.6) race / 0x3f4ccccd (0.8) mode-5
- `+0x964..0x96c` ([0x259..0x25b]) smoothed position
- `+0x970..0x978` ([0x25c..0x25e]) position spring velocity
- `+0x97c..0x984` ([0x25f..0x261]) smoothed look-target
- `+0x988..0x990` ([0x262..0x264]) target spring velocity
- `+0x994/0x998` ([0x265/0x266]) tracked pair (player indices)
- `+0x99c` ([0x267]) pair-switch blend factor
- `+0x9a0` ([0x268]) **final required-zoom — the elimination metric `DAT_00898980`**, read by stub `FUN_00442df0`
- `+0x9a4` ([0x269]) mode-5 distance state

## Constants (.rdata of anchored binary; f32 unless noted)

| VA | value | role |
|---|---|---|
| 0x005cc320 | 1.0 | "static add" + blend complement |
| 0x005cc32c | 0.5 | averaging |
| 0x005cc33c | -1.0 | acos clamp lo |
| 0x005cc358 | 5.0 | height divider + snap threshold |
| 0x005cc55c | 10.0 | **zoom cap = elimination threshold** |
| 0x005cc568 | 100.0 | progress wrap span |
| 0x005cc574 | 2.0 | midpoint divisor |
| 0x005cc730 | 80.0 | wrap window hi |
| 0x005ccd6c | 20.0 | wrap window lo |
| 0x005cc9a0 | 0.05 | spring acceleration |
| 0x005cc9bc | 0.8 | distance→zoom scale AND spring damping |
| 0x005cc9c0 | 0.2 | sway amplitude bias |
| 0x005cc9fc | 1000.0 | death-timer→zoom divisor |
| 0x005ccabc | 1.1 | path-zoom multiplier |
| 0x005ccac4 | 360.0 | yaw wrap |
| 0x005ccad0 | 90.0 | pitch bias |
| 0x005ccae0 | f64 57.295799255371094 | rad→deg (float-precision 180/π as double) |
| 0x005cd09c | 180.0 | angle bias |
| 0x005cd030 | f64 5.0 | offset-law divisor |
| 0x005ce1c0 | f64 -3.95 | camera offset base |
| 0x005ce1c8 | f64 0.25 | camera offset per-zoom |
| 0x005cd120 | 50.0 | pitch-from-zoom numerator (normal tracks) |
| 0x005ce1b8 | 67.5 | pitch-from-zoom numerator (track-type 0x1a) |
| 0x005cd074 | 1.25 | zoom multiplier (track-type 0x1a) |
| 0x005ce1b0 | -2.5 | height-mix term |
| 0x005ce1ac | 2.25 | height-mix term (type 0x1a) |
| 0x005cd088 | 2.5 | height-mix numerator |
| 0x005ce1b4 | 89.9 | pitch+elev cap |
| 0x005ce040/0x005ce1a8/0x005ce044/0x005cd9e8/0x005ce1a4/0x005ce1a0/0x005ce19c/0x005ce198/0x005ce190 | 420/635/400/155/175/83/139.5/33.9/16.5 | the 9 sway oscillator rate divisors |
| 0x005ce194 | 200.0 | sway divisor for osc 8/9 outer |
| 0x005ccd18 | 0.00015 | velocity-lead scale |
| 0x005cd8fc | (FUN_0046cb30 alt path scale; not dumped) | velocity getter alt scale |

Runtime globals: `DAT_007f0fd0` game mode; `DAT_007f0fd4` "it" player (mode 1);
`DAT_007f0f38` overhead-cam toggle (pitch=90, zoom=10); `DAT_007f100c`
per-tick blend increment; `DAT_007f1030` timer (sway phase = u32 quotient
/60000 at 0x00447c33..0x00447c3f) [UNCERTAIN: unit — resolve via live read];
`DAT_007f0fc8` target jitter amplitude [UNCERTAIN: value — live read].

## Race-branch algorithm (0x00446520, mechanical)

1. 4× `Player::WriteFieldZero` (0x0041ef60) + `FUN_0041f120(i,0)`; phase-2/
   mode-5/10/9 extra `FUN_0041ef60(0,0)`. `cam[9] = DAT_00803344`;
   `CameraPath::InitForTrack` (0x00442600, once-guarded).
2. `FUN_00442a60()` — zero `DAT_008989b0[4]`; pick most-separated pair;
   wrap-adjust progress (>80 vs <20 → trailer −100); reference = leader
   (unless dead); for each active+alive player store
   `0.8 × XZ-dist(player, reference)` into `DAT_008989b0[i]`.
3. Pair = `FUN_0040e180`; mode 7 overrides pair to (leader, closest chaser
   within 0.03 progress); modes 4/8/9 pair=(0,0).
4. Lead position: `posA + velA×0.00015` (vel = `FUN_0046cb30`); rel =
   leadA − posB; minus velA-lead again → pair separation vec (Y forced 0
   at 0x00446bxx region); `zoom_pair = |sep| × 0.8` (`local_d8`).
   Special: pair degenerate (A==B) + zoom<1.0 + A dead → zoom_pair = 8.0.
5. Midpoint: `mid = leadA − sep/2.0`.
6. Pair-switch hysteresis: if pair ≠ cam[0x265/0x266] (and !force_reset):
   blend factor cam[0x267] += 2×`DAT_007f100c`; while <1.0 compute OLD pair
   midpoint/zoom and lerp `mid = mid_old×(1−w) + mid_new×w` with
   `w = max(2×cam[0x267] − 1, 0)`; else commit new pair.
7. Path ribbon (per pair member): `prog = FUN_00408a50(player)` (float at
   `DAT_008a96e8 + p×0x30c`); `node = round(prog)` (0x004a2c48, banker's);
   `frac = prog − node`; `next = node+1` wrapped vs `CameraPath::GetCount`
   (0x00426bb0 → `DAT_0066d6d8`); dir/height from `FUN_00441820`:
   `sample = next×frac + cur×(1−frac)` (extrapolates for frac<0 — literal).
8. `FUN_00441820(node, out_dir, out_h)`: poll cmd stream (0x00409790,
   opcodes 0xC..0xF fill table `DAT_0063a5f0` stride 0xC =
   (elev°, azim°, height)); if entry.elev != −1: dir = rotX(90−elev) then
   rotY(azim+180) applied to (0,−1,0); else dir = gate-node dir
   (`FUN_00426cc0` → `DAT_00663658 + n×0x4c`) rotated −25° about
   (corner0−corner3) (corners via `FrontendArraySlotGet` 0x00426d00);
   height = entry.height if != −1 else 0. [UNCERTAIN: whether real tracks
   populate the override table — resolve live]
9. `view_dir = normalize(dirA_sample + dirB_sample)`;
   `path_zoom = (hA + hB) × 0.5`.
10. Last-survivor: if exactly 1 active+alive and dead-flag set:
    `path_zoom += dead_timer_ms/1000` capped at 10.
11. `path_zoom ×= 1.1`; modes 8/4 → 4.0. `zoom = max(path_zoom, zoom_pair)`
    cap 10; overhead toggle → 10. **`elim_zoom = max(path_zoom_pre_max,
    zoom_pair_unclamped...)` — precisely: `local_b8 = path_zoom(×1.1);
    if (local_b8 < local_30) local_b8 = local_30` where local_30 =
    zoom_pair (or 8.0 special); cap 10 → `cam[0x268]` = elimination metric.**
12. Pitch of view dir: `vert = clamp(dot(view_dir, (0,1,0)@0x6146fc), −1, 1)`;
    `ang = acos(vert) × 57.2958`; `elev_dir = ang − 90` (0x004473e7..ff).
13. Camera offset: `scale = −3.95 − zoom×0.25/5.0` (doubles); offset_dir =
    view_dir × scale → normalize → `axis = cross(offset_dir, Yup)` (the
    0x00447457 block via components); `pitch_extra = 50×zoom/10` (type-0x1a:
    67.5×zoom/10, zoom×=1.25) − 5.0; cap `pitch_extra + elev_dir ≤ 89.9`;
    overhead → 90. Rotate offset_dir about axis by pitch_extra
    (`RwMatrix_SetRotAxisAngle` + transform).
14. Distance: `dist = (zoom/0.8)/5.0 + 1.0` (THE SciLor block, 0x00447578);
    `cam_pos = mid + offset_dir × dist` (overhead: pos = mid, Y += 15.0
    [0x005cc9b0]).
15. Height mix: `h = 2.5×zoom/10 + (10−zoom)×(−2.5)/10` (type-0x1a: −2.25×
    zoom/10 extra); if !overhead: both mid and cam_pos += view_dir(normalized
    pre-offset) × h.
16. Position snap/spring (0x00447fxx): if |new_pos − cam[0x259..]| > 5.0 or
    force_reset → snap (zero both spring vels, commit pair, blend=0);
    else spring: `vel += (new − smoothed) × 0.05; vel ×= 0.8;
    pos = new − ((10−zoom)/10) × (new − (smoothed + vel))` — same spring for
    look-target with its own state.
17. Sway (skipped when overhead): `amp = (zoom + 0.2)/5.0`;
    `t = DAT_007f1030 / 60000` (u32 quotient); 9 oscillators:
    target.x += sin(t/420)/10×amp; target.z += sin(t/635)/10×amp;
    pos.x += sin(t/400)/5×amp; pos.y += sin(t/155)/5×amp;
    pos.z += sin(t/175)/5×amp; pos.x += sin(t/83)/10×amp;
    pos.z += sin(t/139.5)/10×amp; pos.x += sin(t/33.9)/200×amp;
    pos.z += sin(t/16.5)/200×amp.
    (sin = CRT fsin, radians; divisor table above; /10 = ÷0x5cc55c,
    /5 = ÷0x5cc358, /200 = ÷0x5ce194.)
18. Jitter: target += rand(−DAT_007f0fc8, +DAT_007f0fc8) per component
    (`FUN_00472650` = PRNG float range).
19. Write-out: cam[0x10..0x12] = pos; cam[0x259..] = pos; cam[0x25f..] =
    target; cam[0x16] = 0.6; az/elev from (target − pos): if dz==0 → az =
    90/270 by dx sign else `az = 180 − (−atan(dx/dz)×57.2958)` (+180 if
    dz>0); elev likewise from dy over planar length (0x00448020..);
    `cam[0xd] = 360 − elev_raw`... (literal: cam[0xe]=azimuth computed
    first, cam[0xd] = 0x5ccac4(360) − value, cam[0xf] = 0); overhead →
    cam[0xe] = 0. `Camera::Apply` (0x00441760): RW frame identity,
    rotate Y-axis by +0x38, then current-right by +0x34, then at-row by
    +0x3c (combine op 2 = postconcat), translate to +0x40. cam[1..3] = pos,
    cam[4..6] = target; if cam[0] != 0 → `Camera::InitWithMatrix`.

## The elimination rule (0x00410d10, mechanical)

Called from race tick `FUN_00411170` after replay record. Skip if
`FUN_00443080()==1` (cam mode flag). Mode-specific early end-conditions for
modes 4,5,7,8,9,10 (timers vs 3.0 [0x5cc31c], dead-flags via
`FUN_0046cbb0`). Then the core (all modes incl. 0/1):

1. `if (FUN_00442df0() == 10.0)` — fcomp equality vs 0x005cc55c at
   0x00410ee3: the camera's required-zoom `cam[0x268]` SATURATED.
2. Re-pick most-separated pair (0x0040e180); wrap-adjust progress
   (`RaceScoreFloatGetBySlot` 0x00408ad0 = `DAT_008a96ec + p×0x30c`);
   mode-1: the "it" player (DAT_007f0fd4) gets progress 100 (never last).
3. Straggler = lesser progress of the pair; if BOTH already dead → return 0.
4. `FUN_004922e0(straggler, 3, 10, 0x80)` (hit sound/particle);
   `FUN_00422fd0(straggler)` (the eliminator — already C3-ported);
   `FUN_0040eee0(straggler, 1)` — scoring callback; its return 1 → round
   ends immediately.
5. Tail: count active (slot ptr `PTR_PTR_005f2770+0x34..0x44`) and alive
   (`FUN_0046c7b0(i)==1`); `DAT_00803320 = alive_count`; solo-race: end when
   0 alive; else end when exactly 1 alive (normal round end) or 0 alive
   (draw: every car's progress set to max via FUN_00408a50/FUN_00408a70,
   return 1).

So: **a car is eliminated the moment the camera cannot zoom out far enough
to keep the pack on screen (required zoom > 10.0), and the victim is the
pair member with less race progress.** Round ends at ≤1 alive.

## Data sources for the standalone port

- Path nodes = the AI gate ribbon (`DAT_00663658` stride 0x4c, count
  `DAT_0066d6d8`): +0x0 vec3 node dir, corners at +0x18+j×0xc (j=0..3 via
  0x00426d00). Standalone equivalent: gates from AI.BSP (already parsed).
- Per-node (elev, azim, height) override: table `DAT_0063a5f0` stride 0xC,
  fed by cmd-stream opcodes 0xC..0xF (0x00409790); producer not yet located
  (not COURSE.LUA/LAPDATA.LUA/KTCSCRIPT.LUA text) [UNCERTAIN — live dump
  will show whether populated and from where; GRAPH.BSP is unexplored].
- Progress per player: 0..100-ish float (wrap math 80/20/100), per-gate
  fractional (node = round(prog) indexes gates) — equals
  `gate_index + frac` of our standalone race state.

## Live probe results (2026-06-10, camera_probe.py on Training track)

- `FUN_00446520` fires 541×/3s (~180/s) — per-frame, multiple subticks.
- **Elimination empirically confirmed**: `cam[0x268]` (`DAT_00898980`)
  reached exactly 10.0 and race-phase `DAT_0063ba8c` flipped 6 → 7.
- Override table populated: 30 entries (elev=15.0, azim per node, h=−1).
- Node array = gate ribbon (dir = unit race direction, c0/c3 = lateral
  corner positions, sequential along track) — count 30 = Training's gates.
- `DAT_007f1030` advanced ~3.0M/s (≈3MHz tick); /60000 quotient ≈ 50/s
  sway phase. `DAT_007f0fc8` (jitter) = 0.0 in this mode.
  `DAT_007f100c` = 0.016666 (1/60 per-tick blend step).
- Ground truth: `log/camera_trace.csv` (286 rows), `log/camera_probe_static.json`.

## Camera-angle data source — SOLVED: Common/LED.piz

`LE<id>.LED` where id = `Course_Id(N)` from the track's COURSE.LUA
(Arctic=0, Training=30, City=26 — matching the type-0x1a==26 City special
case in 0x00446520). Format: 12-byte header + 384 × stride-0xC f32 triplets
(elev°, azim°, height; −1 = unset) — byte-identical to the live
`DAT_0063a5f0` table (verified LE0.LED hex vs live dump values 22.5/45/
67.5/90/112.5/135/157.5/180 progression). The cmd-stream opcodes 0xC..0xF
(0x00409790) are the loader transport for this file.

## Adapter mapping (port contract)

Original getter → standalone state: `FUN_0046d4a0(+0x30/34/38)` → car pos;
`FUN_0046cb30` → car velocity (×0.00015 lead); `FUN_00408a50/0x00408ad0` →
gate-progress float; `FUN_0046c7b0` → alive; `FUN_0046cbb0` → (dead_flag,
ms_since_death); `IsCarSlotActive/GetLiveCarCount` already C3/C4 ports.
