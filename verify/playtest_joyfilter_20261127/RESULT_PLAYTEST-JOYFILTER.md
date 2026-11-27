# RESULT PLAYTEST-JOYFILTER (playtest item 3, 2026-11-27)

Verdict: DONE. On the default path the standalone counts only joystick-type game controllers. This is a deliberate
departure from the original that Mariano decided on 2026-11-27. It is not a parity fix. On MARIANO-PC the three
Keychron devices are skipped, so the keyboard is the only row again. G-CTRL and B are unchanged.

Branch `fix/playtest-joystick-filter` off `race/first-frame-parity` 654c65e9c. Fix commit 6eb0dab84.

## Change

- `mashedmod/src/mashed_re/Input/JoyFilter.h:24` `JoyFilter_TypeIsSteering(dwDevType)`: keep when
  `GET_DIDEVICE_TYPE` (low byte) is 0x14..0x17.
- `mashedmod/src/mashed_re/exe_main.cpp:1142` `JoyFilterOn()` holds the original's RVAs and the departure note.
  `:1158` `CountGameCtrlCb` logs one line per device and returns before CreateDevice when the device is skipped.
  `:1218` adds the count line `game controllers enumerated N, counted K, joystick filter on|off`.
- `MASHED_ALL_CONTROLLERS=1` restores the original's count-everything rule. `MASHED_ORIG_TOP=1` never filters.
  Nothing changed in `re/match`, the top-down bindings or the .asi.

Original, unchanged: enumerator 0x00496040, `EnumDevices(4, 0x00495ee0, &DAT_00772fac, 1)`. Callback 0x00495ee0
counts a device once CreateDevice (0x00495f15) succeeds (increment 0x00495fcb). Slot fill 0x00498510. Colour select
rows 0x004332a0.

## Keep set (Windows SDK 10.0.26100.0 `um/dinput.h`)

| constant | value | line | verdict |
|---|---|---|---|
| DI8DEVTYPE_JOYSTICK | 0x14 | 408 | kept |
| DI8DEVTYPE_GAMEPAD | 0x15 | 409 | kept |
| DI8DEVTYPE_DRIVING | 0x16 | 410 | kept |
| DI8DEVTYPE_FLIGHT | 0x17 | 411 | kept |
| DI8DEVTYPE_1STPERSON | 0x18 | 412 | skipped (Keychron) |
| DEVICECTRL / SCREENPOINTER / REMOTE / SUPPLEMENTAL | 0x19..0x1C | 413-416 | skipped |
| GET_DIDEVICE_TYPE = LOBYTE | | 522 | |

Keychron 0x00010318 = DIDEVTYPE_HID 0x00010000 (:419) | subtype 0x03 DI8DEVTYPE1STPERSON_SIXDOF (:494) | type 0x18.

## Evidence

- Predicate table, built against the SDK constants (`joyprobe.cpp`), in `probe_LIVING-PC.txt`: JOYSTICK, GAMEPAD,
  DRIVING and FLIGHT plus HID 0x00010114 / 0x00010215 are kept, and 0x00010318 is skipped. LIVING-PC has 0 live
  devices.
- MARIANO-PC live probe (same predicate, `EnumDevices(DI8DEVCLASS_GAMECTRL, ..., DIEDFL_ATTACHEDONLY)`), run by a
  child there on 2026-11-27 at 03:44 -03:00, in `probe_MARIANO-PC.txt`. It found 3 devices, not 2:
  `Keychron Link` x2 and `Keychron Q6 HE`, all 0x00010318, all skipped.
- LIVING-PC nav demo (`run_checks.py nav`, `MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`): walk_04 colour select is
  `ba34030d1b7e2fe4` both with the filter on and with `MASHED_ALL_CONTROLLERS=1`. That equals the pre-fix
  reference in `verify/porttop_20261108/capflake_runs.py:16`. The keyboard is the only row
  (`colourselect_filter_on.png`, `colourselect_all_controllers.png`). The log lines in `nav_*.mashed_re.log` read
  `game controllers enumerated 0, counted 0` and `input slots: game controllers 0, types 2 0 0 0, active 1`.
- Gates (`run_checks.py gates` = `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, hashed as merge_train.ps1):
  G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3. Both match `re/orchestrator/merge_train.json`. Log:
  main checkout `log/playtest-joyfilter/gates.log`.
- Build: `MASHED_NO_DEPLOY=1 mashedmod\build.bat` rc 0, rva-lint `NEW=0` (`log/playtest-joyfilter/build.log`).

## Open

- The standalone's own `mashed_re.log` has not been seen on MARIANO-PC. Mariano confirms it after the LAN release
  from the `game controller "..." ... skipped` lines.
- Joypad steering is still not ported (axes). That is out of scope here.
