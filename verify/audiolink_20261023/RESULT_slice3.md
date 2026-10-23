# RESULT: audiolink slice 3, Audio/RwsStream.cpp linked into the exe

PREREG: `PREREG_slice3.md`. Branch `work/audiolink`. Build: exe `e41769c4`, dev .asi `b7fa12f1`. Evidence:
`D:/MashedWT/audiolink_data/path1/S3_*.txt`, `D:/MashedWT/audiolink_data/arms_slice3.txt`, `disasm_slice3.txt`,
`disasm_export.py`.

**Verdict: P and G hold.** AudioRwsChunkHeaderRead 0x005ab380, AudioRwsChunkTypeSeek 0x005ab410,
AudioWaveVtableSlot1cDispatch 0x005abf80 and AudioTreeWalkPredicateSearch 0x005aa0c0 are in both rsp files with one
body each. Stream read / skip call the shared `Render::RwStreamCoreRead` / `RwStreamCoreSkip` by name in both arms.
The header reader is called by name. The only per-target input is the tree root: the global DAT_009146fc in the .asi,
a private NULL pointer in the exe (dead code). Lint 0 NEW.

## Step 1: bytes vs port

- 0x005ab380: identical, **two arguments**. Stream is at [esp+0x14] (0x005ab388) and out at [esp+0x18] (0x005ab3a4)
  after `sub esp,0xc / push esi`, and [esp+0xc] is never read. The third dword its callers push is ignored: 0 at
  0x005ab41d / 0x005ab44d, flag bit 4 at 0x005abfc0 / 0x005ac07b. This settles the survey's arity question. So
  MixedC3Sweep's 3-argument `Orig_` wrapper passes an unused argument; its 3-dword header buffer is still the overflow
  recorded in the survey.
- 0x005ab410: identical (unsigned gate 0x35000 <= v <= 0x37002 at 0x005ab469 / 0x005ab471).
- 0x005aa0c0: identical. It writes the effective root into its own argument slot (0x005aa0e1) and reads it back for the
  predicate and the out param.
- **0x005abf80**: `jmp eax` at 0x005abf8f is a tail call. The slot gets param_1 as [esp+4] and its EAX is the return
  value. The NULL path returns 0 (0x005abf8e). The caller at 0x005ac1c3 loops on `cmp eax,1`. The old port was
  `void` calling `fn()` with no argument. Its compiled export was `test / je / jmp eax`, the same as the bytes, so it
  matched only through codegen. The port is now `uint32 fn(param_1)`, compiled to `mov [esp+4],ecx / jmp eax`
  (`disasm_export.py` on both .asi builds).

## P: path1 (.asi `b7fa12f1`, the 2026-10-11 negctl re-earn fixtures)

4 / 4 GREEN, 0 mismatches. Distinct original outputs:

| Hook | Cases | Distinct |
|---|---|---|
| header_read | 7 | 7 |
| type_seek | 10 | 9 |
| vtable_slot_1c | 4 | 4 |
| tree_walk | 12 | 10 |

In the .asi the stream calls now run the shared RwStreamCore bodies, so this also compares that chain with the
original's.

## G: gates (exe `e41769c4`, root D:/MashedWT/savelink_root)

H_DEF-5 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.

## Trackers (`apply_trackers_s3.py`)

exe_file set for the four rows, notes on 0x005ab380 (arity) and 0x005abf80 (tail call), levels kept.

## Not covered

The exe arm with a non-NULL tree root. File-stream reads in the exe (RwStreamCore's exe file thunks report failure).

## Next in the survey order

Split AudioFmtKeyCompare 0x005adf30 out of AudioRws.cpp into a shared TU, then RwsFmt (0x005ac5f0, 0x005ac9e0).
