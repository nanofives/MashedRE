# PREREG PORT-OT-RACE-BIND-HANDLER (run 54, 2026-11-21)

Committed alone, before any arm. Row kind: race-path binding, top-down. Branch `orch/PORT-OT-RACE-BIND-HANDLER` from
`orch/integration` `65ef9f74a`. No new knob, no default change: every bind runs only under `MASHED_ORIG_TOP=1` +
`MASHED_OT_RACELOAD=1` (both default OFF).

## Change (profile data + generated outputs only, KICKOFF_run54 rule 1)

- `re/tools/bind/state/raceload/c_hold.txt`: drop 0x00440bc0, 0x00458dd0, 0x0047b860, 0x0047b8d0, so the C emit binds
  their 100% sources as C units (`Match/PortTop_<rva>.c`).
- `re/tools/bind/state/raceload/roots.txt`: + those four + 0x0040cea0 (`c4=frida-replay`, source
  `re/match/s02/0040cea0.c`, the file the PROV-INSITU-REF1 replay used; unchanged since `c9134b0f7`, 2026-10-31).
- `porttop_bind.py write --profile raceload --roots-only`, `sync`. This also refreshes the stale raceload plan rows
  the selftest reports (0x00478660 / 0x0047a020 provisional -> replay, roots-only stop texts, 0x005a7b60 reuse).
- New arm stack `ROLOADRWCC` = `@ROLOADRWC` + `MASHED_OT_RL_STOPS=count`.

## Arms (object build, root `D:\MashedWT\PORT-OT-RACE-BIND-HANDLER_root` with `original\mashed_image.bin` staged)

`RO_ROLOADRWC-pr1` (stops exit) and `RO_ROLOADRWCC-pr1` (stops count).

## Predictions

1. Both builds link. `ddkobj.py check`: the five new units are IDENTICAL or kept on VS2022 with a stated reason;
   no newly DIFFERENT unit against `log\orch\run54_headcheck\` that is not classified.
2. The exit arm no longer stops at 0x00458dd0 (RACE-RWCORE-RACELOAD-ENGINE's first stop); it stops at a later rva or
   reaches the race.
3. The COURSE.LUA chain under 0x0047b9b0 reaches 0x004b7330 / 0x004b7480 (`third-party-library[lua-4.0]`, C2, no
   source): a library stop. If it is the first stop, the row stops there and reports it (no Lua body in this row).
4. 0x004790e0 stays a named stop (register-arg, no exe body takes EAX; the binder cannot express it from data).
5. Self-tests (`[portstate-image]`, `[OT_CODEPTR]`, `[REPLAY]`, `[OT_TIMER]`, trace) PASS.
6. Load length: measured only if `race.reached` is true (ref1 = 108); otherwise the stop list and final stop class.
