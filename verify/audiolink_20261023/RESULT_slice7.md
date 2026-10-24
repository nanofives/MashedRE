# RESULT: audiolink slice 7, AudioQueuePop / AudioVecLength lose their exe addresses

PREREG: `PREREG_slice7.md` (+ amendment 1). Branch `work/audiolink`. Build: exe `d50a46de`, dev .asi `c327216d`.
Evidence: `D:/MashedWT/audiolink_data/path1/S7_*`, `T_runtime_table.txt`, `arms_slice7.txt`, `disasm_slice7.txt`.

**Verdict: T, P1, P2, P3b and G hold.** P3, the pre-existing live-capture entry, is a harness failure and is recorded
below. The two files in both rsp since before this lane no longer reach a MASHED.exe address in the exe arm.

## AudioQueuePop.cpp

- 0x005ad2e0 (C2) is ported from the bytes as `AudioQueueConsume`. It is exported and not installed.
  - Count +0xc non-zero: decrement, return 0.
  - Count zero: unlink. prev [+0x18] <- next [+0x14], then both are re-read and next+4 <- prev. The node is freed
    unless byte +0x10 bit 0 is set (0x004522d0 in the .asi, CRT free in the exe). Return 1.
- 0x005b0f40 calls it by name in both arms.
- **P1** `audio_queue_consume` 10 / 10 GREEN. This needed the new handler `queue_node_consume`, whose 10 vectors
  include prev == next and prev = q+0x14, where the re-read matters.
- **P3b** `audio_queue_pop_fixture` (obj+0x28 = node) 10 / 10 GREEN, 10 distinct outputs.
- **P3** `audio_queue_pop` (race, live capture): 6 / 6 RED with `orig=None reimpl=None`. The capture filled no
  argument (all 0), and both sides fault identically at 0x28. The entry never produced evidence; the row is C2.
  Shadow A/B is not usable for this function: it runs the function twice on a live, mutated queue.

## AudioVecLength.cpp

- The exe arm reads private copies of 0x005d757c (0.0f) and the 256 x s16 table 0x00633b48. The index is the upper
  word of a mantissa of at most 24 bits, so 0..0xff, and the table is file-backed. The .asi arm is unchanged.
- Built exports: exe `fcomp [0x1f2488]` / `movsx ecx,[ecx*2+0x1f2490]`, .asi `[0x5d757c]` / `[0x633b48]`.
- **T:** the original's runtime bytes at the menu equal the file for both (512 + 4 bytes), so nothing rewrites the
  table.
- **P2** `audio_vec_length` 12 / 12 GREEN.

## G (exe `d50a46de`)

H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged. Lint 0 NEW.

## Trackers (`apply_trackers_s7.py`)

- exe_file + file -> Audio/AudioQueuePop.cpp for 0x005ad2e0.
- 0x005b0f40 file-column drift fixed.
- Notes on all three rows.
- Levels kept (C2).

## Not covered

The free path of 0x005ad2e0 (bit 0 clear would free harness memory through the RW allocator). The exe arms at runtime.
