# PREREG: audiolink slice 7, the two precedents AudioQueuePop / AudioVecLength lose their exe addresses

Written before any arm. Branch `work/audiolink` on `195acc434`. Build: exe `d50a46de`, dev .asi `c327216d`
(slice 6: exe `36811066`, .asi `98bacf4b`). Lint 8 findings, all allowlisted, 0 NEW.
Bytes: `disasm_slice7.txt` (0x005ad2e0, 0x005b0f40, 0x005aed20), `bytes_step.txt`.

## Change

- **AudioQueuePop.cpp** (in both rsp since before this lane). 0x005b0f40 called 0x005ad2e0 by address in both arms.
  0x005ad2e0 is now ported as `AudioQueueConsume` in the same file and called by name in both arms. It is exported
  for path1 and not installed (C2).
  - Bytes: count +0xc non-zero -> decrement, return 0. Else unlink (prev [+0x18] <- next [+0x14], then both re-read
    and next+4 <- prev), free unless byte +0x10 bit 0 is set, return 1.
  - The free goes through a host thunk: the .asi calls 0x004522d0, the exe calls CRT free (the AudioMemory
    deviation).
  - 0x005b0f40 itself matched the bytes.
- **AudioVecLength.cpp** (naked, in both rsp). The exe arm reads private copies of the zero threshold 0x005d757c
  (0x00000000) and the 256-entry s16 table 0x00633b48 (file-backed .data, dumped). The .asi arm is unchanged
  (`#ifdef` on the two operands). The transcription matched the bytes.
  - Disassembly of the built exports: exe `fcomp [0x1f2488]` / `movsx ecx,[ecx*2+0x1f2490]`, .asi
    `[0x5d757c]` / `[0x633b48]`.
- Harness: new handler `queue_node_consume` and registry entry `audio_queue_consume` (10 vectors, including
  prev == next and prev = q+0x14, where the re-read matters). Bit 0 is forced on, so the free path is never taken
  (it would free harness memory through the RW allocator).

## Arms (sandbox disabled; explicit PIDs only)

- **P1** `run_diff.py audio_queue_consume` (10).
- **P2** `run_diff.py audio_vec_length` (12).
- **P3** `run_diff.py audio_queue_pop` (6, race scenario).
- **T** Runtime-table check. Spawn the original, wait 20 s (menu), read 512 bytes at 0x00633b48 and 4 at 0x005d757c
  through Frida, kill the PID. Compare with the file dump.
- **G** Exe `d50a46de`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`.

## Predictions

- **P1-P3.** GREEN, 0 mismatches.
- **T.** Runtime bytes equal the file (no runtime writer). If not equal, the exe arm's table is wrong: report, no
  commit of the AudioVecLength change.
- **G.** Unchanged: bacfc747 / 8dd1c004 / 6f9932b6, f9deb9b9 / d3601fcc / 9da9a26a.

## Not covered

The free path of 0x005ad2e0 (bit 0 clear) and the exe arms at runtime (nothing in the exe calls them).

## Decision rule

All hold: commit. hooks.csv exe_file for 0x005ad2e0, notes on 0x005b0f40 / 0x005aed20, levels kept (all C2), via
re-classify. Any fails: report, no commit of that part. G fails: stop for Mariano.
