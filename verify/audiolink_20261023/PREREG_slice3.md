# PREREG: audiolink slice 3, Audio/RwsStream.cpp linked into the exe

Written before any arm. Branch `work/audiolink` on `4c7c16508`. Build: exe `e41769c4`, dev .asi `b7fa12f1`
(slice 2: exe `b2f7fba6`, .asi `a71dbd61`). Lint 8 findings, all allowlisted, 0 NEW.
Bytes: `disasm_slice3.txt` (`disasm_slice1.py 005ab380 005ab410 005abf80 005aa0c0`).

## Change

- RwsStream.cpp (0x005ab380 AudioRwsChunkHeaderRead, 0x005ab410 AudioRwsChunkTypeSeek, 0x005abf80
  AudioWaveVtableSlot1cDispatch, 0x005aa0c0 AudioTreeWalkPredicateSearch) joins `exe_sources.rsp`.
- Stream read / skip go to the shared `Render::RwStreamCoreRead` / `RwStreamCoreSkip` by name in BOTH arms (was the
  original 0x004cbd30 / 0x004cc050 by address). The Save/FileWrappers.cpp precedent. The header reader is called by name.
  The tree root DAT_009146fc is the only per-target input: .asi = the global, exe = a private NULL pointer.
- Step 1 (bytes vs port):
  - 0x005ab380 takes TWO arguments (stream [esp+0x14], out [esp+0x18] after the prologue; [esp+0xc] is never read). The
    third dword its callers push is ignored. That settles the survey's open arity question. Body identical.
  - 0x005ab410, 0x005aa0c0: identical.
  - **0x005abf80**: `jmp eax` is a tail call (0x005abf8f). The slot gets param_1 as [esp+4], its EAX is the return
    value, and the NULL path returns 0 (0x005abf8e). The old port was `void` with `fn()`. It matched only because MSVC
    compiled it to `test / je / jmp eax` (old .asi export, `disasm_export.py`). Now `uint32 fn(param_1)`, compiled to
    `mov [esp+4],ecx / jmp eax`.

## Arms (sandbox disabled; explicit PIDs only)

- **P path1.** `run_diff.py` for audio_rws_chunk_header_read, audio_rws_chunk_type_seek,
  audio_wave_vtable_slot_1c_dispatch, audio_tree_walk_predicate_search (the 2026-10-11 negctl re-earn fixtures: scratch
  type-3 memory stream, scratch slot targets, 5-node scratch tree) with .asi `b7fa12f1`.
- **G gates.** Exe `e41769c4`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`, root D:/MashedWT/savelink_root.

## Predictions

- **P.** 4 / 4 GREEN, 0 mismatches, every vector non-degenerate (the fixtures fold the return, out record and stream pos).
  In the .asi the stream calls now run the shared RwStreamCore bodies instead of the original's, so this also compares
  that chain against the original.
- **G.** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

No control arm: the 0x005abf80 old body compiles to the same instructions as the bytes (`disasm_export.py`), so no
vector can separate it. Its change is in the source semantics, not the behaviour.

## Not covered

The exe arm with a non-NULL tree root (nothing sets the private root). Short reads through file streams in the exe
(RwStreamCore's exe file thunks report failure).

## Decision rule

P and G hold: commit, hooks.csv exe_file for the four rows, notes (arity, tail call), levels kept, via re-classify.
P fails: report, no commit. G fails: stop for Mariano.
