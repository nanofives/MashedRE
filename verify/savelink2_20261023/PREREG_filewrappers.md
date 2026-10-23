# PREREG: savelink2 (b) option C, the file wrappers split into Save/FileWrappers.cpp (both rsp files)

Written before any arm. Build: exe `d21f12c1`, dev .asi `734e2ca9` (work/savelink2, uncommitted on 1f1b9b830). Lint 0 NEW.

## Change

- FileReadWrapper 0x004b3b70 and FileWriteWrapper 0x004b3bb0 move from Save/SettingsAndIO.cpp into the new
  Save/FileWrappers.cpp, which is in both rsp files. SettingsAndIO.cpp keeps the three video-dialog helpers and stays
  asi-only.
- The bodies are unchanged in substance from the previous copies. Those already matched the bytes, including the write
  wrapper closing the write RESULT (0x004b3bd6..0x004b3bdb). Read / write now call the shared RwStreamCoreRead /
  RwStreamCoreWrite by name.
- Exe: RW stream open / close have no body in the tree, so the exe open reports failure and both wrappers return 0.
  This is dead code. Save/GameSave.cpp's CRT stand-ins stay the exe save path.
- Fact recorded from the bytes: RwStreamClose 0x004cc160 has no NULL guard (0x004cc164..0x004cc169). So in the original,
  a write that opens and then fails crashes in the close.
- `nav_champ_probe.py --plan exportsweep --export NAME --hooks RVAS` generalizes slice 3's settingssweep.

## Arms (private root D:/MashedWT/savelink_root, .asi 734e2ca9 deployed there, sandbox disabled)

- **F1 sweep.** `MASHED_ORIG_DIR=D:/MashedWT/savelink_root/original py -3.12 re/frida/nav_champ_probe.py --plan exportsweep
  --export FileWrappersSweep --hooks 0x004b3b70,0x004b3bb0,0x004cbd30,0x004cbe80`.
  No boot-patch guard question: none of the four RVAs overlaps a kBootPatched range.
- **F2 live.** `MASHED_ROOT=D:/MashedWT/savelink_root MASHED_LEAF_SELFTEST=1 py -3.12 re/frida/scenario_launch.py --hooks
  0x004b3b70,0x004b3bb0,0x004cbd30,0x004cbe80 --cars 4 --hold 60`.
- **G gates.** Exe `d21f12c1`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`.

## Predictions

- **P1 (F1).** `SWEEP fn=filewrappers cases=12 noo=0 ndiff=0 clean=1`.
  - Reads (7): sizes 0 / 1 / 63 / 64 return their size. 65 and 200 return 64, a short read. The missing file returns 0.
  - Writes (5): sizes 0 / 1 / 64 / 200 return non-zero with that many bytes in the file. The read-only target returns 0
    and leaves its 8 bytes.
  - `err_emitted` >= 2 (the two short reads). The exact value is reported, not predicted, because whether a failed VFS
    open writes the error slot is not established.
- **P2 (F1, F2).** Every live FileReadWrapper call has noo 0 and ndiff 0. F2 has at least one live read from the race-load
  .dff readers. F1's count is reported, not predicted.
- **P3 (G).** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6 and B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

## Decision rule

- All hold: commit. hooks.csv file column -> Save/FileWrappers.cpp for 0x004b3b70 / 0x004b3bb0, levels kept, via the
  re-classify transaction.
- P1 or P2 fails: report the first failing case. No commit.
- P3 fails: stop for Mariano.
