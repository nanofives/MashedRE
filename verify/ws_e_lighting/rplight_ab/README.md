# WS-E vehicle lighting — MASHED_RPLIGHT A/B evidence (2026-07-02)

Branch `r6/ws-e-vehicle-lighting`. Standalone `mashed_re.exe` race demo on Arctic
(`MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DEMO_DRIVE=1 MASHED_DRIVE_HOLD=1`,
worktree run with `MASHED_ROOT` at the main repo), toggle `MASHED_RPLIGHT`
unset (ON, relit path) vs `=0` (legacy load-time model-space bake).

## Car-body region channel means (crop 250,300–420,440, chase cam)

| capture | heading | legacy (off) RGB | relit (on) RGB | delta (on−off) |
|---|---|---|---|---|
| 01_grid     | spawn        | 34.7, 37.6, 38.9 | 34.6, 37.4, 38.7 | −0.2, −0.2, −0.2 |
| 01_turned_a | mid-donut    | 43.2, 45.2, 46.9 | 55.4, 51.9, 53.9 | **+12.2, +6.7, +7.0** |
| 01_turned_b | re-grid      | 37.1, 41.5, 42.6 | 36.9, 41.3, 42.3 | −0.3, −0.2, −0.3 |

Signature: at the spawn heading the two models agree; with the car rotated
toward the sun the relit panel brightens (+12 R) while the legacy bake stays
frozen to the body; after the round re-grids the car, agreement returns.
Menus are byte-stable across the toggle (mean abs diff 0.01).

## Files

- `sheet_turned.png` — 2×2: rows legacy/relit, cols two headings.
- `diff_01_grid.png`, `diff_01_action.png` — imgdiff heatmaps off↔on
  (diffs confined to car/track rows; sky cells 0.0).
- `crop_grid_rplight_{off,on}.png` — spawn-heading car crops.

This is toggle-level behavioral evidence only. The merge acceptance gate
(parity vs ORIGINAL captures) is tracked in
`re/prior_art/notes/rw_lighting_research_2026-07.md` §10.
