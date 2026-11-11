# RESULT RO-AUDIO-ISOLATION: do the audio blocks and the audio-clock call count reach race state

Date 2026-11-11. Worker session cmwbx2zbt2ivgsc1bk1mvi2zq (claude2), parent cmwbssxl22bg4sc1ba3ghf7dh.
Branch `orch/RO-WARP-NORMALIZE` (shared with RO-WARP-NORMALIZE and RO-STACKROW-READERS). Review: **TEXT-ONLY**.

## Verdict

**Audio does not reach race state in this scenario. It does reach three game-side audio-glue globals, and the
oracle should mask them.**

1. **Audio worker suspended, both arms.** The original ran with its one `__beginthread` worker suspended at the warp
   (`ao_a`, `ao_b`). That thread's start address is `0x005c2dd9`, the CRT `_beginthread` entry. Its spawner
   `FUN_005aef30` is the only caller of `__beginthread`, so this is the audio worker.
   - The two arms match: per tick IDENTICAL-FOR-N **2,724 / 2,724**, checkpoints normalised 15/15, race state 15/15.
   - Against runs with audio running (`n3_b`, `n3_c`): race state **15/15**.
   - Per tick, the first divergence is tick 388, in blocks `0x00603400` and `0x00690400` only. At the checkpoints the
     only dwords outside the existing masks are `0x006036c0`, `0x006036cc` and `0x006904c8`.
   - With the worker suspended at the race anchor instead (`ao_k`): the same three dwords, race state 15/15.
2. **Where audio enters game data (static and write watch).**
   - `FUN_004657b0`, the per-frame sound pump, reads the voice status `FUN_005a89a0(voice)`.
     hooks.csv: `returns *(param_1+0xD6C)`, an audio-engine struct field that the worker advances.
   - From that status it stores the duck target `DAT_006036cc`: `0x3f266666` (0.65) when a channel reports 6,
     else `0x3f800000` (stores `0x0046590c` / `0x00465918`, watch pcs `0x00465916` / `0x00465922`).
   - It stores the channel status copy `DAT_006904c4[ch]` (`0x004658e1`, watch pc `0x004658e7`).
   - `FUN_0045df70` steps `DAT_006036c0` toward the target (`0x0045df8d` / `0x0045dfb0`).
   - Every reader of these three is audio glue that feeds volume back into the audio engine:
     `0x0045dfc0` / `0x0045dfd3` (`FUN_0045dfc0`); `0x0045dd80` (`FUN_0045dd60`: voice volume `+0x40`, then
     `FUN_005a6dc0`); `0x004658a8` (`FUN_004657b0` itself).
3. **The audio blocks.** "`0x007dc000..0x007de000`" in REPLAY-ORACLE-DESIGN is too wide.
   - `0x007dc000..0x007dc944` is RenderWare, RW-Physics and qhull state, by the code that references it
     (`audio_readers.tsv`). It includes the RW RNG plugin offset `0x007dc578` and `0x007dc57c`, read by game code at
     `0x00441070` and on. It must stay compared.
   - The audio-engine globals are `0x007dc948..0x007de000`. Every instruction that references them by displacement is
     in the audio engine (`0x005a59bc..0x005ba1b3`), except 8 stores from game code into `0x007dcaa8` / `0x007dcaac`
     (`0x0045d4a1..0x0045d4b9`, `0x00462aa1..0x00462ab9`). No game-code instruction reads them by displacement.
4. **The audio-clock call count.**
   - `FUN_005ab040` has 6 callers, all hooks.csv `audio`: `FUN_005a6ea0`, `FUN_005aaff0`, `FUN_005ab010`,
     `FUN_005c6f50`, `FUN_005c7570`, `FUN_005c7ee0`.
   - Its main-thread value goes to `DAT_007dca50` / `DAT_007dca54` (`FUN_005a6ea0`, `0x005a6f16` / `0x005a6f10`) and
     `DAT_007dcdf0` (`0x005aaff5`). Their readers are all audio engine: `0x005a66e8`, `0x005a6eb0`, `0x005a6f08`,
     `0x005a6ffb`, `0x005a702c`, `0x005a7097`, `0x005a70b1`, `0x005ab016`.
   - Under the tick clock every main-thread clock read inside one tick returns the same value (`qstep = step = 0`), so
     the number of calls changes only the harness counter N, not a value the game reads.
   - Measured: REPLAY-ORACLE-DESIGN pair `t2_a` / `t2_b` had a different call count per tick from tick 8 and race
     state 15/15. In this branch no batch-3 or batch-4 pair without the negative control had an N misalignment.

**What the exe side must do:**
- Mask `0x007dc948..0x007de000` (MASK:audio), `0x007dc764` (MASK:rw-fs) and `0x006036c0`, `0x006036cc`,
  `0x006904c4..0x006904dc` (MASK:audio-glue). All are rows of `re/frida/warp_normalize.tsv`.
- Run the original reference with `--audio-off warp`. Then even the audio-glue globals are a function of the tick
  (`ao_a` / `ao_b` identical).
- On the exe, the matching move is: the replay knob keeps the audio voice status fixed, either no audio device or
  status constant. `FUN_004657b0`'s port then takes the same branches on both sides.

[UNCERTAIN] One branch of `FUN_004657b0` writes data that is not audio-glue only. When a channel goes from status 6
to 3 (`0x004658a8`), it:
- clears `*(DAT_0088f680[ch] + 0xc)`
- stores `DAT_008aa2e0[ch]` (`0x004658c2`)
- calls `Table88f09cClear` (`FUN_0045c850`, `0x0088f09c[kind] = 0`)

The slot tables `0x0088f09c..0x0088f0b0` are read at `0x0045c822` (`FUN_0045c820`, a free-slot search over 4 entries,
caller `FUN_0045cd30`). `DAT_008aa2e0` is read at `0x0045d1fc`. None of these differed between audio-on and
audio-off runs here. What `FUN_0045cd30` does with the slot was not traced. A scenario where a voice finishes at a
different tick on the two sides could expose it, so the exe-side rule above (fixed voice status) also covers it.

## Runs

From batch 4 / 5 (`batch4.sh`, `batch5.sh`), all with the batch-3 configuration of RESULT_RO-WARP-NORMALIZE.md (tick
clock, timer pin, boot-counter pins, `--normalize`). 15/15 checkpoints each, 0 hangs, 0 crashes. Suspending the worker
did not stall the main thread. Load lengths were 126 / 119 / 123 / 118 main-thread clock calls (batch 3 with audio
running: 118 to 120). The tick clock makes the load length irrelevant to the race.

| run | audio worker | suspended thread (id, start) |
|---|---|---|
| `ao_a` | suspended at the warp | 10128, `0x005c2dd9` |
| `ao_b` | suspended at the warp | 5092, `0x005c2dd9` |
| `ao_k` | suspended at the race anchor | 10080, `0x005c2dd9` |
| `ao_w` | suspended at the warp, write watch `0x006904c8`, `0x006036cc` | |

Every other thread listed starts outside MASHED.exe's image. Their modules were not resolved (`meta/ao_*.json`,
`warp.pins.audioOff.other`). The main thread (start `0x004a4bb7`) was not touched.

| pair | per tick (ignore os/audio masks) | checkpoints (`--norm`) |
|---|---|---|
| `ao_a` / `ao_b` | IDENTICAL-FOR-N 2,724 / 2,724 | normalised 15/15, race state 15/15 |
| `n3_b` / `ao_a` | 388 (NEW `0x00603400`, `0x00690400`; raw `0x006036cc` `3f266666` -> `3f800000` at tick 388, `0x006036c0` at 389) | race state 15/15. Normalised 15/15 with MASK:audio-glue, 2/15 without |
| `n3_c` / `ao_a` | 388 (same blocks) | |
| `n3_b` / `ao_b` | 388 (same blocks) | |
| `n3_b` / `ao_k` | | race state 15/15. Non-masked: `0x006036c0`, `0x006036cc`, `0x006904c8` |

Write watch in `ao_w`, from the race anchor (`reports/watch/ao_w.json`):
- `0x006036cc`: 789 stores each at `0x0046590c` and `0x00465918`
- `0x006904c8`: 422 stores at `0x004658e1`

In the audio-running run `n3_w2`, `0x006904c8` takes 1,998 stores at `0x004658e1`. So the status copy changes about
4.7 times as often with the worker running.

## Harness

- `re/frida/js/detorig.js` `audioOff()`. It walks `Process.enumerateThreads()`, queries
  `NtQueryInformationThread(ThreadQuerySetWin32StartAddress)`, and calls `SuspendThread` on every non-main thread
  whose start address lies in MASHED.exe. It reports each thread.
- `re/frida/detorig.py --audio-off warp|anchor`.
- `verify/replayoracle_20261111/audio_readers.py`, `audio_readers.tsv`: the per-dword census of
  `0x007dc000..0x007de000`.

Raw SHA-256: `raw_sha256_ro_orig_side.txt`. Gates: not run (no build input changed).
