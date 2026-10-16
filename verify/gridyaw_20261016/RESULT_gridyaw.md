# RESULT gridyaw: the original's grid frame (FUN_00426cc0) for the initial spawn (2026-10-16)

Branch `work/gridyaw` from `work/d2lateral` `5e3fa3a8`. That branch was rebased onto `race/first-frame-parity`
`f9cdbb94` this session, and its spawn-yaw row was renumbered U-9710 -> U-9713, because rfx2 took U-9710 / U-9711 and
d2bsp claims U-9712. PREREG `PREREG_gridyaw.md` + amendments 1-2, each committed before its runs. Exes: base
`30ff67b3` (`5e3fa3a8`), knob build `9896ddac`. Model claude-opus-5-5, account3. One original run (`orig_grid1`, own
PID, `original\` read-only). Port runs on private roots, `MASHED_NO_DEPLOY=1`, PIDs in `RUNLOG.txt`.

## 0. Verdict

1. **M0 (original capture) decides M0-A.** At the initial spawn the original calls `FUN_00408b00(node 0, slot 0,
   mode 1)` from `0x0040ee56`. It returns pos (0.50093675, 1.175, -1.99903715), the player's frame-1 x/z exactly, and
   **yaw 180.0**.
2. The node-0 record direction (`FUN_00426cc0` = `0x00663658 + 0`) is L = (0.00192259776, 0, -1.00007451). The bytes
   turn it into exactly 180.0:
   - `FUN_004c3c60` 2-D normalizes with the RW LUT, and the inverse sqrt of 1.0001527 is **exactly 1.0**;
   - `FUN_004726b0` clamps the dot -1.0000745 to **-1.0**;
   - CRT `acos` (`FUN_004a3384`) times 57.29578 (`0x005cc98c`) gives 180.0;
   - `RwMatrixRotate` about (0, 1, 0) (`FUN_004704c0`, `DAT_006146fc`) gives at = (-8.742278e-08, 0, -1), the
     measured spawn fwd to the bit.
3. **`RoundGrid`'s yaw (`atan2` of the raw record direction, -1.568874) is not the original's law.** It is 0.11 deg
   off on Training, and that explains d2posmodel's "original grid" yaw.
4. **Ported behind `MASHED_GRID_DIR` (default OFF).** The knob sets FUN_00408b00's whole frame (A, F, L from the record,
   and the yaw law with LUT normalize, clamp, acos and the Y rotation) at the node the port grids at: snap(0) by
   default, or the round-grid node with `MASHED_ROUND_GRID`. New helper `Math::LutNormalize2D` (`0x004c3c60`).
5. **The knob stays OFF** (section 5 rule): D2 0/3, D3 (b) regime A 0/3, collateral +3. No-op, knob-took, G-GRID,
   G-SETTLE (with fix B), determinism and H1 pass. **Default build unchanged:** G-CTRL `716cb2fd / 2a2afda1 /
   ba90ed88` on `BASE_H_DEF`, `H_DEF`, `H_DEF-2`.
6. **Driving-median does not enter its bound in any arm.** Closest is fix B + grid + refresh (`D2_BGL_1`) at 1896.97,
   7.7 under the lower bound 1904.70, with slip 1500-2000 0.1980 OUT.

## 1. Gates

| gate | arm(s) | measured | verdict |
|---|---|---|---|
| no-op | `BASE_H_DEF` (`30ff67b3`), `H_DEF`, `H_DEF-2` | `716cb2fd / 2a2afda1 / ba90ed88` x3 | PASS |
| knob-took | `G.griddir.txt` (R6 + StartRound) | node 0, L bits 3afbffad / bf800271, law 180, port yaw -1.57079637 | PASS (A1.4) |
| G-GRID (report) | `P_G` vs `verify/grid_orig_ts` | (0.5009,-1.9990) (-1.1012,-0.9020) (1.1367,0.2024) (-0.4654,1.2994) vs original (0.5009,-1.9989) (-1.1012,-0.9019) (1.1367,0.2024) (-0.4654,1.2995): <= 1e-4; default `P_DEF` up to 0.024 | PASS |
| spawn pose | `D_G` | (0.500936747, 0.474926978, -1.99903715): x/z = original frame 1 bit-exact; physy0 +1.8e-7 | PASS |
| G-SETTLE (amendment 2) | `D_BG` (fix B + grid) | x bit-identical, dz -1.158e-7, dy +4.73e-7 | PASS (control `D_B` FAIL dz +3.6e-7, dy -1.33e-5) |
| determinism | `H_G`, `H_G-2` | `194b30f3 / 5689da21 / 6cb72240` x2 | PASS |
| D2 three metrics | `D2_G_1..3` | 0.2008 OUT / 0.2627 OUT / 1759.10 OUT (n 457) x3 | **FAIL** |
| recovery H1 | `D2_G_1..3` | 399/400 x3 | PASS |
| D3 (b) | `B_G_K{0,1,26}` vs `BASE_B_DEF_K*` | A **0/3** (base 1/3), B 2/3, C 0/3 | **FAIL** |
| collateral | `H_G` (floor `H_G-2`) vs `H_DEF` (floor `H_DEF-2`) | 42 vs 39 of 94 | **FAIL** (+3) |

## 2. Report arms

| arm | knobs | slip 1500-2000 | slip 2000-2600 | driving-median | H1 |
|---|---|---|---|---|---|
| `D2_GL_1..3` | grid + d2lateral refresh | 0.1872 OUT | 0.2566 OUT | 2007.19 / 2011.39 / 2011.39 OUT | 398/400 |
| `D2_BG_1` | fix B + grid | 0.2039 OUT | 0.2605 OUT | 1794.13 OUT | 399/400 |
| `D2_BGL_1` | fix B + grid + refresh | 0.1980 OUT | 0.2529 in | **1896.97 OUT** | 398/400 |
| `D2_DEF_1` | none | 0.1927 in | 0.2489 in | 1860.35 OUT | 398/400 |

- `D3 (b)` under `GL`: A 0/3, B 2/3, C 0/3.
- `G` alone reproduces d2lateral's diagnostic `D2_Y_1` (0.2007 / 0.2627 / 1759.10) to 4 digits (prediction 4.3, first
  half).
- `GL` does **not** reproduce `D2_YL_*` (prediction 4.3, second half, refuted), although its per-step speed matches
  `YLFX`'s through +300 steps past the impact (`D2_GLFX_1`):
  - within 0.06 % for 60 steps before the impact and 0.002 % at it;
  - <= 0.53 % through the second contact (+19..+22), 1.4 % at +24, 3 % at +30, back to 0.17 % at +50;
  - 9 % at +100 and 8 % at +300.

  The two faithful knobs together carry the run through the first two wall contacts, and the D2 metrics are set after
  that. `GL` and `YL` differ in the spawn x/z only (z -1.99904 vs -2.00247). [UNCERTAIN] where after +300 their
  metrics part; not bisected.
- `G` alone (`D2_GFX_1`) still reproduces the +7..+11 lateral hold on the 0.1 floor, but its post-impact speed is +5 %
  (no refresh).

## 3. What the knob does (code)

- `Race/RoundGrid.cpp`:
  - `GridDir_Enabled` (`MASHED_GRID_DIR`).
  - `GridDir_LawDeg`: `FUN_00408b00`'s law.
  - `GridDir_PortYaw`: `RwMatrixRotate`(Y, deg) at row with r = deg * 0x3c8efa35 in float (PC_24), yaw =
    atan2(at.z, at.x).
  - `GridDir_Anchor`: node snap(0) via `FUN_00409290`, then `RoundGrid_AnchorAt`.
  - `GridDir_YawAt`, and the trace `MASHED_GRID_DIR_TRACE`.
- `Math/RwLutTables.cpp`: `LutNormalize2D` (`0x004c3c60`: `p[1]^2 + p[0]^2` as a float, both LUTs).
- `D3d9Render/TrackRenderer.cpp`: R6 spawn yaw, and `StartRound`'s anchor / lat / dir / yaw for every slot. Knob OFF =
  the previous code (no-op measured).

## 4. User decisions needed

1. **`MASHED_GRID_DIR`** is faithful at every point measured (grid, spawn pose, settle under fix B) and fails D2, D3
   (b) A and collateral. Section 5 holds it OFF. A default ON would trade a 0.11-0.3 deg spawn stand-in for a measured
   regression of the D3 (b) A car and 3 collateral fields.
2. **The D2 lane** now runs on two faithful knobs that reproduce the original through the first two contacts and fail
   later. The next divergence is after +24 (the second-contact neighbourhood), then the trajectory.

## 5. Follow-ups

- Bisect `GL`'s late divergence: the per-step table from +24 on, and the fixup replay at the second contact.
- Why `G` costs the regime-A D3 (b) car. The grid moves the AI cars by up to 0.024 and their yaw by 0.3 deg.
- `RoundGrid` under `MASHED_ROUND_GRID` without this knob keeps the raw `atan2` yaw (0.11 deg off). With both knobs
  on, the law applies.

## 6. Provenance

- `RUNLOG.txt`.
- `orig_grid1.msd.grid.tsv` and `.provenance.json` (committed). The node dumps `orig_grid1.nodes.bin` / `.node.bin`
  are not committed (binary). Their decode is in amendment 1.
- Scorers: `score_arms.py` (D2 + H1), `g_settle.py`, `d3b_score.py`, `collateral_run.py`, `d2lat.py`.
- Not committed (over 100 KB or `*.log`): step / gates / ptrace, motion_diag, a6a / sink / wc / fixup logs, carpos.
