# RESULT HB-CAPFLAKE-WALK04-REF (2026-11-10): capflake_runs.py walk_04 reference = 4d53d93f87f0332d

L2 worker `cmwa9vsl31tf5le1ceow80ck6` for lead `cmw7xu1960f37p41557tefpnd`, on `orch/PORT-OT-HUD` (first commit).

## Change

`verify/porttop_20261108/capflake_runs.py`, list `E` (the 04E capture list), entry 4 (`walk_04_colourselect.bmp`):
`2871487f1fcef51c` -> `4d53d93f87f0332d`. The other 11 entries and the `X` list are unchanged.

## Why

- `2871487f1fcef51c` is the colour-select capture with the BAR_Y knob OFF, `4d53d93f87f0332d` with it ON
  (verify/portstate_20261109/RESULT_PORT-COLOURSEL-BAR-Y.md:48-49).
- BAR_Y is default ON since L-110 (a) (mashedmod/src/mashed_re/Frontend/MenuNavSM.cpp:1148,
  `MASHED_NO_COLOURSEL_BAR_Y=1` opts out). The 04E list predates that merge.
- PORT-OT-FRONTEND's four runs (OFF / ON / OFF / ON, pin default) all gave `4d53d93f87f0332d` for walk_04 and the 04E
  value for the other 11, so the driver printed `class=? eqE=11`
  (verify/portorigtop_20261109/RESULT_PORT-OT-FRONTEND.md, Acceptance, "12 captures" row).

## Check

With the new list, PORT-OT-HUD's four capture runs on this branch print `class=E eqE=12` (see
verify/portorigtop_20261109/RESULT_PORT-OT-HUD.md, Acceptance).
