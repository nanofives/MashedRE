# HANDOFF PORT-OT-RACE-BIND-HANDLER (reboot pause, LIVING-PC 2026-11-21 ~11:45)

Branch `orch/PORT-OT-RACE-BIND-HANDLER` (worktree `.worktrees/PORT-OT-RACE-BIND-HANDLER`), base `65ef9f74a`.
PREREG committed `1888ba5ca`. Checkpoint 1 commit = the commit that adds this file.

## Done
- Profile data: `re/tools/bind/state/raceload/c_hold.txt` -4 (0x00440bc0, 0x00458dd0, 0x0047b860, 0x0047b8d0 now C units),
  `roots.txt` +5 (those four + 0x0040cea0 replay-C4, source `re/match/s02/0040cea0.c` = the replay-verified file,
  unchanged since c9134b0f7), `rules.tsv` +28 `extra_pre` lines (C-unit preamble with MGLOBAL emptied + globals as
  `#define` at original address, thunk_ names -> RwCore jump targets, disassembly cited per line).
- `porttop_bind.py write --profile raceload --roots-only` + `sync` (rsp +5, allowlist +0). Plan: bind 178 (match 173,
  replay 3, provisional 2), reuse 113, stop 118. Stale selftest rows refreshed by the write.
- VS2022 build: first try LNK2019 x7 (C-unit externs), after the rules: `=== Build OK ===`, rva-lint NEW=0.
  Log `D:/MashedWT/PORT-OT-RACE-BIND-HANDLER_data/build_vs2.log`.
- Private root `D:\MashedWT\PORT-OT-RACE-BIND-HANDLER_root` (robocopy of original\), `original\mashed_image.bin` staged
  (sha256 81CC7775...). New stack `verify/savelink_20261023/stacks/ROLOADRWCC.txt` (count mode on the RW core).

## In flight at the pause
- Object build (`MASHED_DDKOBJ=1`) finished `=== Build OK ===` before the reboot (`D:/MashedWT/PORT-OT-RACE-BIND-HANDLER_data/build_ddk1.log`);
  only `ddkobj.py check` is left of step 1 (re-run the object build only if the build dir was cleaned).

## Next (in order)
1. `py -3.12 re/orchestrator/mashed_lock.py status` (wait on PRIORITY-HOLD / SOLO), object build, then
   `py -3.12 re/tools/ddkobj/ddkobj.py check`; compare with `log\orch\run54_headcheck\` (BYTECHECK.tsv), classify newly
   DIFFERENT units, list listed-unit -> 100% neighbour fallbacks; match.py --bytes + relocheck on the 5 sources.
2. `porttop_bind.py selftest` (expect raceload plan no longer stale).
3. Arms on the object build: `RO_ROLOADRWC-pr1` (exit) and `RO_ROLOADRWCC-pr1` (count), A4_ROOT = the root above,
   test-slot outside, mashed_lock inside, background timeout 7200000. Prediction: next stop 0x004b7330 (Lua library,
   called first by 0x0047b860) = library stop -> STOP and report on the RESULT's first line.
4. 0x004790e0 stays a stop (register-arg; needs a source port-branch edit of `re/match/s12/00479330.c`, not data).
5. RESULT `verify/racebind_20261121/RESULT_PORT-OT-RACE-BIND-HANDLER.md`, ledger `set review`, REPORT.
