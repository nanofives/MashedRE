# PREREG RACE-PIZ-DEVICE (run 56, race-path row, top-down kind)

Committed alone, before any arm of this row. Branch `orch/RACE-PIZ-DEVICE`, stacked on `orch/RACE-VFS-COURSELUA` 028c09d3c
(prep until L0 reports that row merged; arms, ddkobj check and selftest count only after the rebase onto the merged head).

## Change (knob path only, no new knob, no default change)

- Sources (match work): `re/match/s18/{004b6630,004b6f90,004b6f60,004b6ba0,004b6ed0}.c` match.py `--bytes` BYTE-IDENTICAL,
  relocheck OK (promotion file `re/promotions/RACE-PIZ-DEVICE.tsv`, applied by the train). `re/match/s18/004b6c00.c` (83.1%)
  and `004b6cd0.c` (88.7%) NOT-PROVEN in equiv.py, listed in `re/match/PROVISIONAL.tsv` with 0x004b6640 (60.5%, NOT-PROVEN
  call-regargs). hooks.csv C1 rows for the 7 methods (`re/tools/c1_add.py`).
- Profile data (RULE 1, no porttop_bind.py edit): raceload roots + the 7 methods, 0x004b6640, the mount chain
  0x004b6570 / 0x004b6940, the unmount 0x004b67a0 with 0x004b6770 / 0x004b65e0, the read 0x004b67e0, 0x005507a0; rules
  prov_bind 0x004b6640 / 0x004b6c00 / 0x004b6cd0 and raceload-only CRT aliases for 0x004a34b0 / 0x004b6b10 / 0x004b302f.
  Generated wrappers, rsp, allowlist (+2), the bulk profile's 0x004b6590 wrapper (stop -> reuse of 0x004b67a0, moved.tsv).
- Hand file `Match/PortTop_00402750_Pieces.cpp`, only when `RwCore_FrameRoute()` (MASHED_RWC_CORE=1 + MASHED_RWC_FRAME=1,
  both default OFF): `PortTop_PizFs_Init_004b6540` also runs PortTop_Bound_004b6640(flag); `PortTop_PizFs_Mount_00495280`
  also runs PortTop_Bound_004b6570(exe path) after the exe reader loaded the archive; `PortTop_PizFs_Unmount_004952f0`
  first runs PortTop_Bound_004b67a0. Off that route nothing changes (the exe piz reader stays the only one).

## Arms (object build + staged image, private root D:\MashedWT\RACE-PIZ-DEVICE_root)

`RO_ROLOADRWC-piz1`, `RO_ROLOADRWCC-piz1` (`verify/savelink_20261023/run_arms.py`), then one arm per further stop.

## Predictions, written before the arms

1. The boot log carries `[PIZ_FS] 0x004b6640 piz: file system (provisional body) installed` with `[0x007dc75c]` one
   above the RACE-VFS-COURSELUA count (4 -> 5), and every track-load mount line carries
   `0x004b6570(...) original mount chain returned 1` with `[0x008ad9a8]` equal to the exe reader's entry count for that piz.
2. The COURSE.LUA open no longer reports `Unable to find COURSE.LUA in piz`, and the AV at exe+0x1E824
   (RwCore_005509b0+0x4) is gone. The next stop is [UNCERTAIN]: no census of the functions the original executes after
   the COURSE.LUA open has been crossed with this tree yet.
3. Load length (`meta.json` `race.n`) stays 0 unless the arm runs through the remaining race_load walls (282 at
   RACE-VFS-COURSELUA). Prediction: not above 3 in this row.
4. Default path unchanged (knob off): the knob-boot VS2022 exe reaches `WinMain 0x00492370 returned 0 after 600 frames`
   as at kb_vs1 (RACE-VFS-COURSELUA).

Done-means (decision B): load length above 3 on the object build under MASHED_ORIG_TOP, or the CUT wall count: the
original's race_load functions first executed after the COURSE.LUA open, from a call-order census on the original (the
RO-AUDIOINIT-DEPTH method, verify/audioinit_depth_20261127/), crossed with bound / C4 unbound / source not C4 / no source /
no row. If only the stop moves, the RESULT's first line says "not met, stop moved from X to Y".
