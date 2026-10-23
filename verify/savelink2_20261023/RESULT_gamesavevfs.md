# RESULT: savelink2 (b) slice 2, GameSaveVFS as shared bodies

PREREG: `PREREG_gamesavevfs.md` (b3ddbea05). Build: exe `5f8af4bb`, dev .asi `5c5f8aa9`. Evidence:
`D:/MashedWT/savelink2_data/leaf_ab/V1.*`, `V2.*`, `D:/MashedWT/savelink2_data/arms_vfs.txt`.

**Verdict: P1 to P3 hold.** VfsFileExists (0x00550b00) and AutosaveTrigger (0x004099a0) each have one body in
Save/GameSaveVFS.cpp, now in both rsp files. In the exe both are dead code.

## Fixes against the previous copy (static, from the disassembly)

| Function | Original | Previous copy |
|---|---|---|
| VfsFileExists | prefix "X:" (terminator at buf[i+1], 0x00550b4b) | prefix "X" (terminator at buf[i]) |
| VfsFileExists | after a missed prefix or an empty list, the scan resumes (0x00550b74) | stopped at the first colon |
| VfsFileExists | NULL default object: error callback (6), return 0 (0x00550b79..0x00550b8e) | returned 0, no callback |
| AutosaveTrigger | `printf` 0x004a2cbd on 0x005ccb34 "starting autosave\n" | `wprintf` on 0x005cc7dc ("...EarthBuWheel") |

The previous copy's no-colon path and the three autosave stores were already right. Those are the only paths a race
exercises, which is why its C4 evidence did not see the rest.

## Arms

| Arm | Result |
|---|---|
| V1 sweep, VfsFileExists | **154 cases, noo 0, ndiff 0.** 74 dispatched to an fs object, 40 called the error callback. Comparisons cover the return, the exists calls (object, name) and the callback calls (count, code). |
| V1 sweep, AutosaveTrigger | **4 cases, noo 0, ndiff 0.** guard 0 left the sentinels a1a1a1a1/b2b2b2b2/c3c3c3c3. guard 1 / 0x80000000 / 0xffffffff wrote 4/1/0. |
| V1 / V2 live, VfsFileExists | 256 to 511 calls per run (the SUM line prints every 256), **all colon-free**, noo 0, ndiff 0. |
| V2 live, AutosaveTrigger | no call (it runs after a championship track). |
| G (exe 5f8af4bb) | H_DEF-3 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged. |
| L lint | 0 NEW (8 allowlisted). |

The sweep swaps the four VFS words (0x007dc75c / 54 / 6c / 68) for synthetic fs objects and restores them afterwards. The
RwGlobals scan and compare functions (+0xf4 / +0xf0) are the game's own.

Coverage limits:
- The colon paths and the error-callback branch are covered by the sweep only, because the game made no colon lookup.
- AutosaveTrigger's printf output is not compared.

## Trackers

Notes only, no file move and no level change. hooks.csv rows 0x00550b00 and 0x004099a0 (C4 kept), plus CHANGELOG
entries. The same flag applies as for slice 1: the colon paths never ran in a canonical scenario. Also recorded:
Mariano 2026-10-23, 0x004cbe80 C4 kept.
