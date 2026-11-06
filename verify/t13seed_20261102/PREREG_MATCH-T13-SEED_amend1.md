# PREREG MATCH-T13-SEED, amendment 1 (2026-11-02)

Committed alone, after the four O arms of the PREREG and before any arm below. Nothing in the PREREG's scoring
changes. This adds arms because the O arms read `DAT_0089a360` = 0 on a path the PREREG did not anticipate.

## What the O arms and the image showed (facts this amendment rests on)

- O_SANDS / O_CITY / O_NEU / O_TRAIN (mode 3, rule 10): `DAT_0089a360` = `0x00000000` at every `FUN_004046a0`
  entry (2 setup calls + 1..2 segment-end calls each), timers `418c0000` / `41a00000` / `41980000` / `41f00000`.
- The image has one store to `0x0089a360`: `fstp [0x0089a360]` at `0x004140f4` in `FUN_00414060` (called from
  `Race::Tick` at `0x0040ffc7`, before the seed calls at `0x00410071` / `0x00410214`). It stores
  `v = (float)DAT_0089a384[FUN_00413fa0()] + DAT_0089a37c * 0.5` (`0x00414066`..`0x0041407b`), replaced by
  `FUN_00431d80() * 2.5` (`DAT_0067ea7c`) when `DAT_0067e9fc == 6` (`0x00414084`) and by `FUN_0042fe80() * 2.5`
  (`DAT_0067ea90`) when `DAT_0067e9fc == 10` (`0x004140a5`), clamped to [0, 10] (`0x004140c3`..`0x004140e9`).
- `FUN_00413fa0` = `DAT_0067f17c * 3 + (mode 4 ? 1 : mode 5 ? 2 : 0)`. `DAT_0089a384[0..38]` is copied from
  `0x005f2a70` by `FUN_00414120` (which also zeroes `DAT_0089a37c`). `FUN_00414120`'s callers are `0x0043d3d0`
  (`FUN_0043d2a0`) and `0x0043fead` (`FUN_0043dfd0`, championship race launch). The launcher's warp bypasses both, so
  the O arms read an uninitialised table. Every `0x005f2a70` entry is >= 1.
- The three rule-10 events in the cup table (`kCupEvent`, `0x005f65c8`, event 0x4) are row 2 / mode 4, row 5 /
  mode 5 and row 12 / mode 4, which are the launcher's tracks 2 (course 0x27), 5 (0xb) and 12 (0x1a). Their
  `0x005f2a70` entries (index row*3 + modeOff) are 4, 6 and 7.

## Added arms (original, same oracle record, private root, lock-wrapped)

| arm | args | env | predicted `DAT_0089a360` | predicted timer / bonus (float32 bits) |
|---|---|---|---|---|
| O10_SANDS | `--track 5 --mode 10 --rule 10` | - | 2.5 (`DAT_0067ea90` = 1 as booted) | 16.75 `41860000` / 4.525 `4090cccd` |
| O10_CITY | `--track 12 --mode 10 --rule 10` | - | 2.5 | 19.25 `419a0000` / 6.175 `40c5999a` |
| O10_NEU | `--track 2 --mode 10 --rule 10` | - | 2.5 | 18.25 `41920000` / 3.975 `407e6666` |
| OEV_SANDS | `--track 5 --mode 5 --rule 10` | `T13SEED_INIT_DIFF_TABLE=1` | 6.0 | 15.7 `417b3333` / 4.35 `408b3333` |
| OEV_NEU | `--track 2 --mode 4 --rule 10` | `T13SEED_INIT_DIFF_TABLE=1` | 4.0 | 17.8 `418e6666` / 3.9 `40799999` |
| OEV_CITY | `--track 12 --mode 4 --rule 10` | `T13SEED_INIT_DIFF_TABLE=1` | 7.0 | 17.9 `418f3333` / 5.95 `40be6667` |

`T13SEED_INIT_DIFF_TABLE=1` makes `scenario_launch.py` call `FUN_00414120` once at the menu before the launch poke
(CONTRIVED: it stands in for the championship launch's call at `0x0043fead`). Predictions use the image's constants
(`0x005cc320` 1.0, `0x005cc56c` 0.1f, `0x005cc32c` 0.5, `0x005cc740..754`) in double, stored to float32. These apply to
the **first** call of each arm. A later segment-end call may differ if `DAT_0089a37c` moved (`FUN_00417750`,
`0x0041777e`). It is reported per call.

Scoring for these arms: the seed follows the live `DAT_0089a360` (H1-live) when every call's timer equals the
formula on that call's own entry `DAT_0089a360`. A call that does not is RED for H1-live.

## Port arms

Unchanged from the PREREG (P_OFF / P_ON on sands / City / Neustein, plus TRAIN). The exe sets `DAT_0089a360` = 2.5
constant (`TrackRenderer.cpp`, AI race reset) and has no port of `FUN_00414060`, so P_ON can match O10 only. That
is reported, not scored as a pass for the natural events.
