# PREREG: D4-AUDIO-RWS3 slice 3, the last 13 AudioRws.cpp installs, AudioRws.cpp linked into the exe

Written before any arm. Branch `orch/D4-AUDIO-RWS3` on `1889fb02d` (orch/integration). Precedents:
`verify/audiolink_20261023/PREREG_rws1.md` (slice 1), `verify/audiorws_20261024/PREREG_rws2.md` (slice 2).
Bytes: `disasm_rws3.txt` in this folder (`verify/audiolink_20261023/disasm_slice1.py`, MASHED.exe.unpatched; no
byte differs from the patched exe in these ranges). Table: `table_633674.txt` (`dump_table_633674.py`).
Build (worktree, not deployed): exe `f614d0f8`, dev .asi `027b0ce5`, log `D:/MashedWT/D4-AUDIO-RWS3_build1.log`.
Lint (build step): 8 findings, all allowlisted, 0 NEW.

## Change

- Slice 2 left 13 active installs in Audio/AudioRws.cpp. All 13 stay in that file, and the file is added to
  `mashedmod/exe_sources.rsp` (it was already in `asi_sources.rsp`). One body per RVA in both targets:

  | RVA | Function | Registry name |
  |---|---|---|
  | 0x005acd10 | AudioFmtTableSearch | audio_fmt_table_search |
  | 0x005acd60 | AudioFmtGlobalScan | audio_fmt_global_scan |
  | 0x005ae080 | AudioSubStructAFree | audio_sub_struct_a_free |
  | 0x005ae050 | AudioSubStructBFree | audio_sub_struct_b_free |
  | 0x005ae030 | AudioSubStructCleanup | audio_sub_struct_cleanup |
  | 0x005a9e10 | AudioSubStructTwoCallInit | audio_sub_struct_two_call_init |
  | 0x005ae010 | AudioSubStructLinkDevice | audio_sub_struct_link_device |
  | 0x005adfe0 | AudioSubStructLinkBuffer | audio_sub_struct_link_buffer |
  | 0x005ac7b0 | AudioSubStructDualInit | audio_sub_struct_dual_init |
  | 0x005abcb0 | AudioWaveNodeFree | audio_wave_node_free |
  | 0x005ac740 | AudioSubStructBufCleanup | audio_sub_struct_buf_cleanup |
  | 0x005ac900 | AudioContextLookup | audio_context_lookup |
  | 0x005ad540 | AudioSubStructLinkBoth | audio_sub_struct_link_both |

- Calls go by name in both targets: AudioFmtEntryMatch (Audio/RwsFmt.cpp), AudioFmtKeyCompare
  (Audio/AudioFmtKey.cpp), AudioPoolFree (Audio/AudioPool.cpp), AudioTreeWalkPredicateSearch (Audio/RwsStream.cpp),
  and the sub-struct family inside the file. All of these already link into both targets. In the .asi each callee's
  original RVA is hooked to the same body. Their declarations sit in the new `Audio/AudioRwsCallees.h`, because the
  rva-body scan's second pass read an in-file `extern` of AudioPoolFree as a second body of 0x005ae920.
- Exe arm (`MASHED_STANDALONE`):
  - RwFree 0x004522d0 = CRT `free` (Audio/AudioMemory.cpp's declared deviation, as in slice 2).
  - Pool headers 0x007dda28 (0x005ae092) and 0x007dd634 (0x005abcd1) become private zero storage, 9 dwords each.
    Both lie past .data's raw end (raw size 0x4d000 from 0x005ea000, so raw data ends at 0x00637000), so both are
    BSS, zero until the audio open, the same pre-open state slice 2 used for its pools.
  - The 0x00633674 table (.data, 9 pointers, read at 0x005acd68 / 0x005acd88) becomes a private table of 9 pointers
    in the same order onto private copies of the nine 16-byte .rdata keys it points at (0x005e6414, 0x005e6424,
    0x005e6434, 0x005e6444, 0x005e6454, 0x005e64a4, 0x005e6464, 0x005e6474, 0x005e64b4; bytes in
    `table_633674.txt`).
  - LAB_005ac930 (U-1730, the AudioContextLookup predicate, no hooks.csv row) is ported from the bytes as a
    file-static `ContextKeyMatch_005ac930`, exe arm only. cdecl (root, node, ctx): ring at node+0x20 (0x005ac937 /
    0x005ac93a), per link AudioFmtKeyCompare(*(link-0x40), ctx[0]) with ctx[0] re-read each pass (0x005ac946 /
    0x005ac949 / 0x005ac951), on 0 writes ctx[1] = link-0x40 (0x005ac96a) and returns 1, exhausted returns 0.
    No arm can measure it (see audio_context_lookup below). Static evidence only. It gets no RVA, no hooks.csv row
    and no install.
  - Nothing in the exe calls any of the 13 yet, so they are exe dead code and the default gates cannot move.
- .asi arm: the original's addresses for the two pools, the table, RwFree, and LAB_005ac930 (passed by address).
- Bodies against `disasm_rws3.txt`: all 13 logic paths match. No EAX change: the void ports (0x005abcb0, 0x005ac740)
  leave path-dependent EAX in the bytes (0x005abcd0 returns param_1 on the bit-2 path, the AudioPoolFree result on
  the pool paths; 0x005ac740 leaves the buffer pointer or the free result), so they stay void as slice 2 kept
  0x005ae780. The pointer-returning ports already return what the bytes leave in EAX.

## Arms

- **P path1.** `run_diff.py` on the 13 registry names above, with the worktree's dev .asi (loaded by `run_diff.py`
  from `mashedmod/build`, never deployed), under `mashed_lock.py run`. `run_diff.py` does not honour `MASHED_ROOT`
  (slice 2 process note), so it spawns the shared `original/MASHED.exe` with the hook bypassed, as slices 1 and 2.
- **G.** The worktree exe `f614d0f8`, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`,
  private root `D:/MashedWT/D4-AUDIO-RWS3_root`, under `mashed_lock.py run`.
- **L.** `scripts/lint_rva_bodies.py` (already run in the build, recorded above).

Boots start only after `roadmap.ps1 get HB-LOCKPRIO` shows stage done.

## Predictions

- **P.** 13 / 13 GREEN. audio_context_lookup is a crash-equal entry (`crash_equal_ok`, root DAT_009146fc NULL at the
  menu), so it measures the NULL-root path only, not the predicate.
- **G.** Unchanged: G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a.
- **L.** 0 NEW.

## Decision rule

All three hold: commit, hooks.csv exe_file -> Audio/AudioRws.cpp for the 13 (0x005ad540's file column also
-> Audio/AudioRws.cpp, it pointed at its analysis note), levels kept (re-classify transaction). P fails: report the
failing names, no tracker move for them. G fails: stop and report.
