# RESULT: audiolink slice 2, Audio/AudioMemory.cpp linked into the exe

PREREG: `PREREG_slice2.md`. Branch `work/audiolink`. Build: exe `b2f7fba6`, dev .asi `a71dbd61`. Evidence:
`D:/MashedWT/audiolink_data/path1/S2_*.txt`, `D:/MashedWT/audiolink_data/arms_slice2.txt`, `disasm_slice2.txt`.

**Verdict: P and G hold.** AudioAlignedAlloc 0x005aea10, AudioAlignedFree 0x005aea40, AudioByteReverse 0x005aec00,
AudioBitScanForward 0x005aee20 and AudioByteSwapBuffer 0x005aec30 are in both rsp files with one body each. In the
exe they are dead code with no address: the raw allocator is CRT malloc / free (tag dropped, the declared deviation of
Render/RwStreamCore.cpp's HostAlloc). In the .asi it is still the original's trampolines 0x005aea00
(`jmp [RwGlobals+0x108]`) and 0x004522d0 (`jmp [RwGlobals+0x10c]`). Lint 0 NEW.

## Step 1: bytes vs port

All five match, no body change. Recorded from the bytes:
- 0x005aea10 returns 0 on a NULL raw block (`test ecx,ecx / jbe` 0x005aea27 -> 0x005aea38).
- 0x005aea40 rewrites its argument slot and tail-jumps (0x005aea47 / 0x005aea4b). The 7 direct call sites
  (0x005a7dc4, 0x005a7e39, 0x005abdd8, 0x005abe1d, 0x005ac1e7, 0x005b153d, 0x005b1563) pop the slot with `add esp,4`
  and none reads EAX. Indirect callers are not covered.
- 0x005aee20 returns AL at the `ret` 0x005aee3a, the question U-0352 left open.
- 0x005aec30 width 4 builds b0<<24 | b1<<16 | b2<<8 | b3 through the saved dword (0x005aec4c..0x005aec71) and stores
  it back to its argument slot on the way (0x005aec53). The result is a plain bswap.
- 0x005aea60 (not ported) with a NULL argument writes 0x004ccef0 / 0x004a460d / 0x005aead0 / 0x005aeaf0 into RwGlobals
  +0x108 / +0x10c / +0x110 / +0x114.

## P: path1 (.asi `a71dbd61`)

5 / 5 GREEN, 0 mismatches: aligned_alloc 5, aligned_free 4, byte_reverse 7, bit_scan_forward 10, byte_swap_buffer 8.

## G: gates (exe `b2f7fba6`, root D:/MashedWT/savelink_root)

H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged. (The first run stopped on a missing
output directory before any comparison. It was re-run.)

## Trackers (`apply_trackers_s2.py`)

exe_file set for the five rows, notes on 0x005aea40 and 0x005aee20, levels kept.

## Not covered

The exe arm's malloc / free: nothing in the exe calls it, and the original has no CRT-heap counterpart to compare with.

## Next in the survey order

RwsStream (0x005ab380 arity and 0x005abf80 CALL / JMP first), then the 0x005adf30 split + RwsFmt.
