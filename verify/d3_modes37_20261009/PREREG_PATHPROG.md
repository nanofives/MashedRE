# PREREG: faithful per-car path progress (`FUN_00408610`, D-11072) — scope + leg P1 gates

Date 2026-10-09. USER DECISION (this session): the faithful route, not the bridge, for
`0x008a96e8` (read by `FUN_00414a70` via `FUN_00408a50`). Committed before any code.
Evidence for everything below: `FINDINGS.md`, `decomp_*.txt` in this folder, worker survey
`worker_pathdata.txt`.

## 1. What feeds `0x008a96e8` in the original

- Per frame, per car: `FUN_0040da50` -> `FUN_00408610(rec, world=FUN_00426080(v))` (call `0x0040db2c`).
- `FUN_00408610` (1066 B):
  1. sphere `{car pos, r = 0.6 (0x3f19999a), type 3}` vs the AI gate world `DAT_00656ed8` through
     `FUN_00538c80` (RW collision, case 3 -> `FUN_00539ec0`); callback `FUN_00407a90` writes the hit
     triangle's node id (ushort at `RpMaterial+4`, material colour) to `0x008a9640+v*0x30c`
     (last hit wins) and the triangle data to `+0x8a96dc..e4`.
  2. route walkers `FUN_00407b00/b70/be0/cd0/d70/db0` over the route table `0x0066ce58`.
  3. `FUN_00407e20` (1895 B) x1 or x2 projects the car on the node gate planes (node records
     `0x00663658 + i*0x4c`: dir `+0`, `FUN_00426cb0` base `+0xc`, corners `FUN_00426d00(i,k)` at
     `0x00663670 + k*0xc + i*0x4c`) into scratch `0x008a95c0` / `0x008a95e8` (10 dwords each).
  4. selects one, stores `0x008a96e8` (node progress), `0x008a96ec` (race_pct), `0x008a9640`, and
     the split-sector flags `0x008a9914[6]` against the list `0x0066d6e4`; `FUN_00408590` (C4).
- Static data, built at track load by `FUN_00426e10`:
  - `FUN_00478660(course 0x00657498, AI world)`: count `0x0066d6d8 = world+0x14` (30 on Training),
    node records from the AI BSP polygons keyed by material colour `{i&0xff, i>>8, 0, 0xff}`.
  - `LAPDATA.LUA` handlers (`Lap_Line`, `Lap_Line_End`, `Safe_Start_Lines`, `Split_Sector`,
    `Lap_Variations`; registered via `FUN_00440bc0`) fill `0x0066ce58` (Training `[0,29,-1]`) and
    `0x0066d6e4`.
- The port already parses the AI BSP (`TrackRenderer.cpp:2166-2255`, Training `LAP.BSP`, 30 gates) but
  builds its own gate records and never writes any of the addresses above.

## 2. Legs (each its own pre-registration and gates before code)

| leg | content | acceptance |
|---|---|---|
| **P1** static data | port `FUN_00478660` + callees over the port-parsed AI BSP; port the LAPDATA handlers driven by a flat call-list parser; write `0x00663658..`, `0x0066d6d8`, `0x0066ce58`, `0x0066d6e4` in the exe | byte compare vs the original's live course block (G-P1 below) |
| **P2** per-frame | port `FUN_00408610`, `FUN_00407e20`, the walkers, `FUN_00407a90`, the sphere-vs-AI-BSP query, the per-car block init; default-OFF `MASHED_PATHPROG` | run_diff path1 on the asi where the original's tables exist; exe liveness: `0x008a96e8` in [0,30), wraps at the line, grid values 30.00/29.73/29.45/29.18 at frame 0 within 0.01 |
| **P3** consumer | `FUN_00414a70` + `FUN_00414300` + the `+0xbc` speed write (`0x004710c3..c8`), default-OFF | regime-A window counts per car vs the original (§3), then (b) per regime, collateral |

## 3. Original targets carried forward (regime A, identical on 6 runs)

car 1: `14a70==1` 16, `==2` 0. car 2: 35 / 10. car 3: 12 / 26. (`FINDINGS.md` §1.)

## 4. Leg P1 gates (registered now)

- **G-P1-CAPTURE.** Original, Training, race running: dump `0x00657498..0x0066d710` (the course block,
  0x16278 bytes) once after `FUN_00426e10` returns. Also Arctic (94 gates) as the second track.
- **G-P1-COUNT.** Port `0x0066d6d8` == original (30 Training, 94 Arctic).
- **G-P1-NODES.** Port node records `0x00663658 .. +count*0x4c` byte-identical to the original on both
  tracks. Any float difference is a RED, reported with the first differing record and field. No ulp
  tolerance (same inputs, same operations; x87 rule applies).
- **G-P1-ROUTE.** `0x0066ce58` (up to its `-2` terminator) and `0x0066d6e4[6]` byte-identical.
- **G-P1-OFF.** With the leg's knob off, the default G-CTRL trio `ae67b13a / c8317ce2 / 31db9384`
  reproduces.
- Gaps found during P1 that are not in the bytes above (other course-block fields) are listed, not ported.

## 5. Predictions

- P1 is pure load-time data, so G-P1-NODES either matches exactly or exposes a transcription error;
  no behaviour change is predicted, and G-P1-OFF must hold by construction.
- Effort: P1 one session, P2 one to two, P3 one.
