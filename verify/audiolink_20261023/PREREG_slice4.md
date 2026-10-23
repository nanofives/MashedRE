# PREREG: audiolink slice 4, AudioFmtKeyCompare split into Audio/AudioFmtKey.cpp, RwsFmt linked into the exe

Written before any arm. Branch `work/audiolink` on `038c46cb1`. Build: exe `a4f9ed2f`, dev .asi `31a08f99`
(slice 3: exe `e41769c4`, .asi `b7fa12f1`). Lint 8 findings, all allowlisted, 0 NEW.
Bytes: `disasm_slice4.txt` (`disasm_slice1.py 005adf30 005ac5f0 005ac9e0`).

## Change

- 0x005adf30 AudioFmtKeyCompare moves unchanged from Audio/AudioRws.cpp (asi-only) to the new Audio/AudioFmtKey.cpp +
  AudioFmtKey.h, in both rsp files. AudioRws.cpp stays asi-only. Its own by-address call at :207 is left as it is.
- RwsFmt.cpp (0x005ac5f0 AudioFmtDescEqual, 0x005ac9e0 AudioFmtEntryMatch) joins `exe_sources.rsp`. Its only address,
  the key compare 0x005adf30, is now called by name in both arms.
- Step 1: all three bodies match the bytes, no body change. Argument order to the key compare: (p1[1], p2[1]) at
  0x005ac600 / 0x005ac601 and (cand[1], entry[0]) at 0x005aca11 / 0x005aca12, as before. The key compare is unsigned
  `repe cmpsb` with an `sbb / sbb -1` result.

## Arms (sandbox disabled; explicit PIDs only)

- **P path1.** `run_diff.py` audio_fmt_key_compare (15), audio_fmt_desc_equal (10), audio_fmt_entry_match (10) with
  .asi `31a08f99`.
- **G gates.** Exe `a4f9ed2f`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`, root D:/MashedWT/savelink_root.

## Predictions

- **P.** 3 / 3 GREEN, 0 mismatches.
- **G.** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

No control arm: no body changed.

## Decision rule

P and G hold: commit. hooks.csv: file + exe_file -> Audio/AudioFmtKey.cpp for 0x005adf30, exe_file for the two RwsFmt
rows, levels kept, via re-classify. P fails: report, no commit. G fails: stop for Mariano.
