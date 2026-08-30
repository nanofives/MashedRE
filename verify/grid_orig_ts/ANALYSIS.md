# Race starting-grid parity — original vs standalone (child B, 2026-08-30)

## Verdict

The starting grids **genuinely differ**. The roll-away timing confound is ruled
out: the original's four cars are **byte-identical every frame from t=0.121s
through t=6.910s** (frozen on the grid during the pre-race countdown, spread
locked at 3.439, zero drift). The staggered diagonal IS the real starting grid,
not a few moments into the roll-away.

- Evidence: `orig_grid_timeseries.txt` / `.json` (probe
  `re/frida/orig_grid_timeseries.py`, Quick Battle -> TRAINING, gameplay car
  array 0x008815a0/0xd04 active matrix, layout from FUN_0046d4a0).

| car | original (frozen grid) | standalone f0 (2x2 square) |
|---|---|---|
| 0 (player) | ( 0.50, 0.43, -2.00) | (-0.87, 0.51, -0.80) |
| 1 (ai0) | (-1.10, 0.47, -0.90) | ( 0.93, 0.51, -0.80) |
| 2 (ai1) | ( 1.14, 0.50,  0.20) | (-0.86, 0.57,  1.00) |
| 3 (ai2) | (-0.47, 0.54,  1.30) | ( 0.94, 0.57,  1.00) |

Original = a single-file **staggered zig-zag** (z steps +1.10 per rank across 4
ranks, x alternates sides). Standalone = a symmetric **2x2 box** (2 ranks of 2,
sides +/-0.9). The standalone's grid is explicitly invented (TrackRenderer.cpp:80
"instead of the original's AI tile grid"); the original spawn routine was never
ported.

## Original grid construction (RVA-cited)

`FUN_00408b00` @ **0x00408b00** — "race grid start-position calculator"
(hooks.csv; note `re/analysis/game_state_d5_cont2/0x00408b00.md`). Decompiled
this session: `FUN_00408b00_decomp.txt`. Called once per car slot; returns
`param_4` = XYZ, `param_5` = yaw.

Inputs:
- `A` = `FUN_00426cb0()` — anchor/camera base position.
- `F` = normalize(node0 - node3), node_k = `FUN_00426d00(desc, k)` — a track axis.
- `L` = `FUN_00426cc0(desc)` — the (unit-magnitude here) lateral track vector.

.rdata constants (read from MASHED.exe, `dump_grid_consts.py`):
- `_DAT_005cc9bc` = 0.8 (forward spacing)
- `_DAT_005ccac0` = 0.4 (perpendicular)
- `_DAT_005ccabc / ab8 / ab4` = 1.1 / 2.2 / 3.3 (lateral A/B/C)
- `_DAT_005cc318` = 0.6 (2-car), `_DAT_005cc9c8` = 0.9 (3-car)

For a 4-car race (param_3==4; both the `param_2<4` and the rotating `param_2>=4`
paths yield the same four positions), slot s in {0,1,2,3}:

```
pos0 = A + F*0.48                       ( 0.8*(1-0.4) = 0.48 )
pos1 = A - F*1.12 - L*1.1               ( 0.8*(1+0.4) = 1.12 )
pos2 = A + F*1.12 - L*2.2
pos3 = A - F*0.48 - L*3.3
```

Verification against the measured original grid (solving for A,F,L): gives
`F ~= (1,0,0)`, `L ~= (0,0,-1)`, `A ~= (0.02, 0.43, -2.0)`, and the formula
reproduces all four measured positions to <=0.02 in x/z. Exact decode, not a fit.

## Mapping into the standalone

Standalone StartRound frame: `g0`=gate0 center, `dir`=normalize(g1-g0) (forward),
`lat`={-dir.z, dir.x} (left perp). Measured standalone f0 + yaw=-1.576 imply
`dir ~= (0,0,-1)`, `lat ~= (1,0,0)`, and `g0 ~= (0.03, ~0.51, -2.0)` — i.e.
**gate0 coincides with the original anchor A**, `lat` matches `F`, `dir` matches
`L`. So the original formula ports directly:

```
pos_s = g0 + lat*latMul[s] - dir*dirMul[s]
latMul = [ +0.48, -1.12, +1.12, -0.48 ]     (from F*fwdOff)
dirMul = [  0.00,   1.10,  2.20,   3.30 ]    (from L*latOff)
slot order = spawn order: player=0, ai0=1, ai1=2, ai2=3
```

Plugging g0=(0.03,·,-2.0), lat=(1,0,0), dir=(0,0,-1) gives
(0.51,-2.0)/(-1.09,-0.9)/(1.15,0.2)/(-0.45,1.3) — matching the original to <=0.02.

## Fix applied + result

`TrackRenderer::StartRound` (TrackRenderer.cpp) grid rewritten from the invented
2x2 box to the ported formula above (kLatMul/kDirMul). Rebuilt, re-captured with
MASHED_DBG_CARPOS=1 (verify/grid_re_fixed/carpos.txt).

| slot | standalone NEW f0 | original measured | dx | dz |
|---|---|---|---|---|
| 0 player | ( 0.5009, 0.4748, -2.0025) | ( 0.5009, 0.4345, -1.9989) | 0.000 | 0.004 |
| 1 ai0 | (-1.0933, 0.5095, -0.8941) | (-1.1012, 0.4688, -0.9019) | 0.008 | 0.008 |
| 2 ai1 | ( 1.1524, 0.5435,  0.1942) | ( 1.1367, 0.5033,  0.2024) | 0.016 | 0.008 |
| 3 ai2 | (-0.4418, 0.5781,  1.3025) | (-0.4654, 0.5376,  1.2995) | 0.024 | 0.003 |

x/z match to <=0.024. y is ~0.03-0.04 higher because the standalone samples its
own terrain (GroundHeight) rather than the original's node height — a per-car
height offset, not a formation difference. Roll-away preserved: cars parked at f0,
scattered into racing by f396 (player z=-7.08, AIs spread with varied yaws), so
only the initial formation changed. StartRound spread went 2x2 -> the original's
staggered zig-zag.
