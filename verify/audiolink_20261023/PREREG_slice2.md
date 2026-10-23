# PREREG: audiolink slice 2, Audio/AudioMemory.cpp linked into the exe

Written before any arm. Branch `work/audiolink` on `1143bd25b`. Build: exe `b2f7fba6`, dev .asi `a71dbd61`
(slice 1: exe `71587212`, .asi `dc146391`). Lint 8 findings, all allowlisted, 0 NEW.
Bytes: `disasm_slice2.txt` (`disasm_slice1.py 005aea00 005aea10 005aea40 005aec00 005aee20 005aec30 004522d0`).

## Change

- AudioMemory.cpp (0x005aea10 AudioAlignedAlloc, 0x005aea40 AudioAlignedFree, 0x005aec00 AudioByteReverse,
  0x005aee20 AudioBitScanForward, 0x005aec30 AudioByteSwapBuffer) joins `exe_sources.rsp`.
- Its only addresses were the two raw-allocator trampolines 0x005aea00 (`jmp [RwGlobals+0x108]`) and 0x004522d0
  (`jmp [RwGlobals+0x10c]`). They move behind `AudioRawAlloc` / `AudioRawFree`: exe arm = CRT malloc / free, tag
  dropped (the declared deviation of Render/RwStreamCore.cpp's HostAlloc), .asi arm = the same trampolines as before.
- Step 1: all five bodies match the bytes, no body change. Recorded from the bytes: the free tail-jumps after
  rewriting its argument slot (0x005aea47 / 0x005aea4b); all 7 direct call sites pop that slot at once and do not
  read EAX. 0x005aee20 returns AL (0x005aee3a), which settles U-0352's open question.

## Arms (sandbox disabled; explicit PIDs only)

- **P path1.** `run_diff.py` for audio_aligned_alloc, audio_aligned_free, audio_byte_reverse, audio_bit_scan_forward,
  audio_byte_swap_buffer with .asi `a71dbd61`.
- **G gates.** Exe `b2f7fba6`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`, root D:/MashedWT/savelink_root.

## Predictions

- **P.** 5 / 5 GREEN, 0 mismatches (5 / 4 / 7 / 10 / 8 cases).
- **G.** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

No control arm: no body changed, so there is no old behaviour to separate from.

## Not covered

The exe arm's malloc / free (nothing in the exe calls it, and the original has no CRT-heap counterpart to compare).

## Decision rule

P and G hold: commit, hooks.csv exe_file for the five rows (levels kept), U-0352 note, via re-classify. P fails:
report, no commit. G fails: stop for Mariano.
