# PREREG: audiolink AudioRws slice 1, seven pure leaves split into Audio/AudioRwsLeaves.cpp

Written before any arm. Branch `work/audiolink` on `62cee1fd2` (+ slice 7 `afc9e7f89`). Build: exe `ed93b4fd`, dev
.asi `d4f80839`. Lint 0 NEW. Bytes: `disasm_rws1.txt`. The comparison was done by the worker and checked on the four
EAX claims.

## Change

- These move from Audio/AudioRws.cpp (asi-only) into the new Audio/AudioRwsLeaves.cpp, in both rsp files:

  | RVA | Function |
  |---|---|
  | 0x005aca80 | AudioFmtSizeCalc |
  | 0x005ac980 | AudioFmtDescCopy |
  | 0x005ae0b0 | AudioSubStructZeroInit |
  | 0x005aeca0 | AudioFieldEndianPack |
  | 0x005ae0c0 | AudioWaveFmtCopy (calls EndianPack by name) |
  | 0x005be190 | AudioRwsSubZeroInit |
  | 0x005be140 | AudioSubStructThreeWrite |

- Bodies: all seven match the bytes. Four were `void` where the bytes leave a pointer in EAX: 0x005ac980 (param_2,
  0x005ac988), 0x005ae0b0, 0x005be190 and 0x005be140 (param_1). They now return it. No caller is checked. The
  registry declares these `void` with per-side buffers, so path1 cannot see the return. That is static evidence only.
- The 0x005aca80 block also holds 0x005acaa0, the unported desc pack / unpack, at 0x005acaa0 with no padding. Its
  decoding from the bytes is recorded in RESULT_rws1.md for U-3032 / U-1731, not ported.

## Arms

- **P path1.** `run_diff.py` audio_fmt_size_calc, audio_fmt_desc_copy, audio_sub_struct_zero_init,
  audio_field_endian_pack, audio_wave_fmt_copy, audio_rws_sub_zero_init, audio_sub_struct_three_write (10 each).
- **G.** Exe `ed93b4fd`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`.

## Predictions

- **P.** 7 / 7 GREEN.
- **G.** Unchanged (bacfc747 / 8dd1c004 / 6f9932b6, f9deb9b9 / d3601fcc / 9da9a26a).

## Decision rule

Both hold: commit, hooks.csv file + exe_file -> Audio/AudioRwsLeaves.cpp for the seven, levels kept. P fails: report.
G fails: stop.
