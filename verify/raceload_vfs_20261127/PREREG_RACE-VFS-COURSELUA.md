# PREREG RACE-VFS-COURSELUA (run 56, race-path row, top-down kind)

Committed before any arm of this row.

## Change

- Profile data (RULE 1): `re/tools/bind/state/raceload/roots.txt` + 0x00495780 HardwareInstallFileSystem, 0x004955d0,
  0x00550790 FsManager7dc76cSet (all C4 c4=match). Generated: `Match/PortTop_{00495780,004955d0,00550790}.cpp`,
  plan.json, newfiles.txt, exe_sources.rsp (+3), dual_copy_allowlist.txt (+1), PortTop_BulkRefs.cpp (+1 keep).
  0x00551190 / 0x00550430 reuse the exe bodies PortTop_Bound_00551190 / _00550430 (PORT-TOP-AUDIO-HWINIT-4627F0).
- Hand file `Match/PortTop_00493710.cpp`: the thunk for 0x004951e0 runs PortTop_Bound_00495780 when
  `RwCore_FrameRoute()` (MASHED_RWC_CORE=1 + MASHED_RWC_FRAME=1, both default OFF), the same predicate under which
  RtFSManagerOpen already runs RwCore_00550350. Off that route it stays the missing stub. No new knob, no default change.

## Arms (object build + staged image, private root D:\MashedWT\RACE-VFS-COURSELUA_root)

`RO_ROLOADRWC-vfs1`, `RO_ROLOADRWCC-vfs1` (`verify/savelink_20261023/run_arms.py`).

## Predictions, written before the arms

1. The boot log carries `0x00495780 HardwareInstallFileSystem (matched, RW core FS) returned 1` with
   `[0x007dc75c]` >= 2 (the original's disk devices: one for the current directory, one per fixed/removable drive letter).
2. The COURSE.LUA open (0x0047b910 -> RwCore_005507b0) still returns 0: the original's default device at that open is
   `piz:` (0x0416ee60), installed by 0x004b6640 (the piz FS, 60.5% source, not proven) whose 7 method bodies
   (0x004b6630, 0x004b6c00, 0x004b6f60, 0x004b6cd0, 0x004b6f90, 0x004b6ed0, 0x004b6ba0) have no hooks.csv row and no
   source, and the exe's piz reader (`PortTop_PizFs_Mount_00495280`, hand) writes none of the original's piz globals.
   So the stop is expected NOT to move past `PortTop_Bound_0047b8d0`. [UNCERTAIN] whether the disk device default
   finds a COURSE.LUA on disk in the private root (none is expected: the file lives in the track piz).
3. Load length: `meta.json` `race.reached` false, `race.n` 0 (not above 3).

Done-means (decision B): load length above 3, or the count of walls from the original's executed-function list for the
load crossed with bound / unbound state (`verify/raceload_vfs_20261127/load_walls.py`).
