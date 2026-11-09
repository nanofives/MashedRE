# PREREG PORT-SAVE-SPAN-MIRROR (2026-11-09)

Step: PORT-SAVE-SPAN-MIRROR, parent cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-SAVE-SPAN-MIRROR` from `orch/integration`
fa6fe7612. Committed alone, before any knob arm. Arm 1 (the watch) ran before this PREREG, because the brief makes it
the input that decides what the knob does.

## 1. Arm 1 result (done): the writer of 0x007f0a40 on the original

`savespan/watch_0a40.py` (hardware write watchpoints on 0x007f0a40 / 0x007f0a44 / 0x007f0a7c, armed before resume,
re-armed every 50 ms). One boot, then the exe walk's three confirms to the colour select. 6 hits, all on the main
thread:

| hits | pc (after the store) | when | values after |
|---|---|---|---|
| 1-3 | 0x004925be / 0x004925d5 / 0x004925f6 (inside 0x004924f0) | boot, before the title | 0 / 1 / 2 |
| 4-6 | 0x00404e91 (`rep movsd` in 0x00404e80 Save::DeserializeFromBuffer) | between the title and the main menu (the press-button load) | 0 / 1 / 2 |

0x00404e80, read from MASHED.exe.unpatched:

1. 0x00404e82..0x00404e91: `rep movsd`, 0x148 dwords from 0x00827d98 (= save buffer 0x00803358 + 0x24a40, file offset
   150080) to 0x007f0a40. This is the span 0x007f0a40..0x007f0f5f.
2. 0x00404e93..0x00404eb2: for i = 0..11, `*(u32*)(0x007f105c + i*0x4c) = (u8)*(0x007f0f54 + i)`. The bytes come from the
   span just copied (span + 0x514).
3. 0x00404eb4..0x00404ed2: `0x008a95ac = *(u32*)0x00828254` (save buffer + 0x24efc). Then, if `*(u32*)0x008a94a8` is not
   zero, `rep movsd` 0x928f dwords from 0x0080335c (save buffer + 4) to that pointer.

## 2. What the exe does today

`exe_main.cpp` R2-2 (`Nav_GameStateLoadSave`, `Frontend/MenuNavSM.cpp:1517`) reads `original/gamesave.bin` at boot,
after 0x004924f0 or its stand-in, the same order as the original. With the shipped save (magic 0xDEADBEEF) it runs
step 1 at the real address (`CopySpanAndRederive`, MenuNavSM.cpp:1535). The comment at `exe_main.cpp:189-190` ("our port
does not mirror the span back") is stale, and PORT-TOP-04D's and PORT-TOP-04D-WALK04's mechanism text relied on it.
**Steps 2 and 3 are not run at the real addresses.** `Save/GameSaveBuffer.cpp` binds 0x007f105c to a stand-in base
(`kStrideBase`), and the boot load does not call it.

For the shipped save, the step-2 bytes are `0 1 1 1 1 1 1 1 0 0 0 0`. So the original holds player 0's 0x007f105c field =
0 and players 1..7 = 1 at the colour select. The exe's matched 0x004924f0 path leaves `DAT_007f1058[i].f4 = 1` for i < 8
(0x004924f0's own write). The hand path leaves 0 (zero fill). This is the one 0x004924f0 write outside the span that
the save does not overwrite in the exe and does overwrite on the original.

## 3. Change (default OFF)

Knob **`MASHED_SAVE_SPAN_MIRROR`** (`1` = on, read once). When on and the boot save load succeeded, run steps 2 and 3
of 0x00404e80 at the real addresses, right after `Nav_GameStateLoadSave`, from the loaded save bytes:

- step 2: the 12 stores to 0x007f105c + i*0x4c.
- step 3: the 0x008a95ac store. The 0x008a94a8 block copy runs only when that pointer is non-zero, as in the original.
  The log records whether it ran.

One log line with the 12 values written, the 0x008a95ac value and the 0x008a94a8 pointer. Step 1 already runs and is
unchanged. Knob OFF runs no new code apart from the getenv test. `exe_main.cpp:189-190`'s stale comment is not edited
in this step, because a comment edit shifts `__LINE__` and so the default exe bytes. Its correction goes in the RESULT.

## 4. Arms (after this PREREG)

2. **Default gates, knob OFF**, worktree build `MASHED_NO_DEPLOY=1`: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3
   (the `merge_train.json` refs win). Pass bar: unchanged.
3. **Knob-ON gates** (H, B x3, stack `SSM` in run_arms.py): hashes reported, no pass bar. A move is a flip decision.
4. **walk_04** (nav demo, `MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, settings baseline restored as in 04D), four runs:
   hand knob OFF, hand knob ON, `MASHED_ORIG_TOP=1 MASHED_TOP04D_DATAZERO=1` (0x004924f0 matched) knob OFF and knob ON.
   Compared against the original's walk_04 `d40b38fdd6a30c50` (PORT-TOP-04D-WALK04) by layout: player 1's icon column and
   tint. Pixel identity is not expected, because the 55 px bar offset (row PORT-COLOURSEL-BAR-Y, left alone) and the
   background video differ. Prediction (H2): matched + knob ON shows player 1 in column 1, tinted red, as the
   original does. Hand + knob ON is reported. Step 3 of the brief (0x004924f0 matched under MASHED_ORIG_TOP) is the
   matched + knob ON run. WinMain's default path is not changed.

## 5. Rules

Every launch through `mashed_lock.py run` inside test-slot. No build during a merge-train PRIORITY-HOLD or SOLO. No
default flip. Raw data in `D:/MashedWT/PORT-SAVE-SPAN-MIRROR_data` with SHA-256 in the RESULT.
