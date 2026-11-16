# PREREG PORT-OT-RACE (run 52, 2026-11-16)

Row PORT-OT-RACE (D4, top-down). Worker on claude3, lead cmwjaf65a3d7fp0153fg1nefc. Branch `orch/PORT-OT-RACE` from
`orch/integration` `810042e0e`. Committed alone, before any arm.

## Change under test

Knob `MASHED_OT_RACELOAD=1`, default OFF, exe only, `MASHED_ORIG_TOP` path only.

- Case 2 of the matched game-mode machine 0x004929d0 calls the matched 0x0040d440 (`Match/PortTop_0040d440.cpp`), which
  calls 0x0040d270. Today that call is the counting stand-in `PortTop_StandIn05_0040d270` (no load).
- With the knob on, 0x0040d440 calls the matched body of 0x0040d270 (`re/match/top07/0040d270.c`, match.py 100%, C4 by
  match) compiled as `Orig_0040d270` (`Match/PortTop_0040d270.cpp`). Knob off: the stand-in, unchanged.
- 0x0040d270 calls 0x004950b0 twice (entry, and the elapsed value before the `course LoadTime` print). On the replay arm
  those are wrapped clock reads (Replay_QPC, return address inside the matched 0x004950b0 body).
- Callees bind to existing exe bodies by name. The small matched leaves with no exe body get ORIG_TOP-only TUs
  (0x0040d020, 0x00462510, 0x00462500, 0x0042f790, 0x0042f7a0, 0x00426070). The large subtrees (0x00426e10 under
  0x0040d020, 0x004264d0, 0x00426340, 0x00407800, 0x004074a0, 0x0040d110, 0x00411d90, 0x0040d040, 0x00458c70) are named
  counting stand-ins that log their first call and their argument. They do not run.
- Guard: when the pointer at 0x005f2770 is 0 (no file-backed .data image), the knob falls back to the stand-in and logs it,
  because the body dereferences `**(PTR_PTR_005f2770 + track * 4)`.

## Predictions (stated before the arms)

1. Knob OFF: the exe arm log shows the old stand-in line `rva 0x0040d270: stand-in (PORT-TOP-05) course load` and the race
   anchor at warp + 0, as in RESULT_RO-EXE-ARM-LOADALIGN.md.
2. Knob ON (`RO_ROLOAD`, stack `ROLOAD` = `@ROIMG` + `MASHED_OT_RACELOAD=1`): the log shows `Orig_0040d270` entered and
   left, the `course LoadTime is %u` line, and between the warp and the race anchor at least 3 wrapped clock reads (load
   length > 1).
3. `[REPLAY] self-test PASS` and `[TRACE] self-test PASS` with the knob on.
4. The first divergent record against ref1 stays record 0 at `0x00604604` (boot-write class), because the knob runs only
   at the warp (tick 300).

## Measurement

- Both builds (VS2022 and `MASHED_DDKOBJ=1`), `ddkobj.py check` (every new TU IDENTICAL or its refusal reason recorded).
- Knob boot (ORIG_TOP, no replay) to the menu, knob on, no crash.
- Arms: `RO_ROIMG-pr1` (knob off control) and `RO_ROLOAD-pr1` (knob on), object build, `A4_RO_CKPT=0,1,17,18,299,301,600`.
- Score each with `detorig_firstdiv.py` against `D:/MashedWT/RO-REF-NAME_data/ref1`, the same masks and ignore set as
  RO-EXE-ARM-LOADALIGN.
- Load length = wrapped clock reads from the warp read to the anchor read (meta / log), against ref1's 108.

No G-CTRL, no B, no 12x4: top-down row, those hashes are computed with ORIG_TOP off.
