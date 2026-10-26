# PREREG D4-WSH-EXEFILE run 11, amendment 1: strict C4 check on the shared bodies

Committed alone, before any arm of run 11 has produced output (the G arms are queued on the machine
lock). Adds arms; changes no prediction of `PREREG_D4-WSH-EXEFILE_run11.md`.

## Why

I1 (`canonical_install_observe.py`) shows the JMP is live and the game survives, not that the body
ran. The strict check (2026-10-16, `0x0040b9a0` / `0x0040ba60` demoted for this) is: exercised +
installed + diff. The three rows' `.asi` text changed in `de6a16a34`, so their C4 must stand on the
shared body.

## Added arms

| Arm | Command | Prediction |
|---|---|---|
| I2 | `canonical_c4_verify.py 0x00432b30,0x0040b890,0x0040b8e0` (boot to menu, OFF counts calls on the original RVA, ON installs) | `0x00432b30`: calls > 0 (one per nav op, boot pushes the title and main menu), JMP live, survived. `0x0040b890` / `0x0040b8e0`: 0 calls (callers are race HUD paths) |
| I3 | `canonical_c4_racediff.py --rvas 0x0040b890,0x0040b8e0` (Quick race, OFF vs ON) | only if I2 gives 0 calls for them: C4-CLEAN, non-empty sets, JMP `0xe9` |

`canonical_c4_verify.py` and `canonical_c4_racediff.py` get the `MASHED_ROOT` extension (default unchanged).

## Decision rules (replace the P1/T1/T2/I1 rule for the C-level)

- `0x00432b30`: P1 GREEN and I2 exercised + installed + survived -> C4 holds on the shared body.
- `0x0040b890` / `0x0040b8e0`: T1/T2 GREEN and (I2 exercised, or I3 C4-CLEAN) -> C4 holds.
  T1/T2 GREEN but neither I2 nor I3 shows the body running -> C4 -> C3 through `re-classify`
  (the 2026-07-24 install-observe was on the old text and never showed execution).
- Any diff RED -> demote per the rubric.
- In every case `exe_file := file` (one body), once G equals the refs.
