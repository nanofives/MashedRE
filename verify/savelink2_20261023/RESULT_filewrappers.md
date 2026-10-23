# RESULT: savelink2 (b) option C, the file wrappers in Save/FileWrappers.cpp

PREREG: `PREREG_filewrappers.md`. Build: exe `d21f12c1`, dev .asi `734e2ca9`. Evidence:
`D:/MashedWT/savelink2_data/leaf_ab/F1.*`, `F2.*`, `D:/MashedWT/savelink2_data/arms_fw.txt`.

**Verdict: P1 to P3 hold.** FileReadWrapper 0x004b3b70 and FileWriteWrapper 0x004b3bb0 have one body each in
Save/FileWrappers.cpp, now in both rsp files. In the exe they are dead code. SettingsAndIO.cpp keeps the three
video-dialog helpers and stays asi-only, next to SettingsDialog.cpp.

## F1 sweep (`--plan exportsweep --export FileWrappersSweep`)

`SWEEP fn=filewrappers cases=12 noo=0 ndiff=0 err_emitted=4 clean=1`, ret 0.

| Case | Original = shared |
|---|---|
| read 0 / 1 / 63 / 64 of a 64-byte file | returns 0 / 1 / 63 / 64, error slot clear |
| read 65 / 200 (short) | returns 64, error 5 |
| read a missing file | returns 0, error 0x80000002 (failed open) |
| write 0 / 1 / 64 / 200 | non-zero, the file holds 0 / 1 / 64 / 200 bytes |
| write to a read-only file | returns 0, the file keeps its 8 bytes, error 0x80000002 |

Live reads before the sweep: 185, noo 0, ndiff 0. No writes. No temp files were left behind.

## F2 live (60 s race, 4 cars)

The live reads covered `Tri.dff`, `IconCube.dff`, `lazer.dff` (the three .dff readers) and `gamesave.bin`. No call had
noo or ndiff (any differing call is always logged). The total count is not logged in a run without a sweep. The root's
gamesave.bin stayed bd18788182b2343e.

## G gates (exe d21f12c1)

H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged. Lint 0 NEW.

## Not covered

A write that opens and then fails. RwStreamClose 0x004cc160 has no NULL guard (0x004cc164..0x004cc169), and the write
wrapper hands it the NULL result, so the original crashes there. This is recorded in the source and in hooks.csv.

## What is left of the Save TU closure

- SettingsAndIO.cpp (3 dialog helpers) and SettingsDialog.cpp (3 functions) stay asi-only. Dialog 0x65 is unreachable in
  the patched reference (skip_selector), and the exe has no dialog resources.
- Option B, replacing GameSave.cpp's exe stand-ins with these wrappers, needs exe bodies for RW stream open / close
  (0x004cc230 / 0x004cc160) and the VFS open 0x005507b0, plus a decision on the failed-write crash.
