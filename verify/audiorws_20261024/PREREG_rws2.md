# PREREG: D4-AUDIO-RWS slice 2, pool and list family into Audio/AudioPool.cpp on private zero storage

Written before any arm. Branch `orch/D4-AUDIO-RWS` on `30913130b` (orch/integration). Precedent:
`verify/audiolink_20261023/PREREG_rws1.md` (slice 1). Bytes: `disasm_rws2.txt` in this folder
(`verify/audiolink_20261023/disasm_slice1.py`, MASHED.exe.unpatched; no byte differs from the patched exe in these
ranges).

## Change

- These move from Audio/AudioRws.cpp (asi-only) into the new Audio/AudioPool.cpp, in both rsp files:

  | RVA | Function | Registry name |
  |---|---|---|
  | 0x005ae650 | AudioPoolConstruct | audio_pool_construct |
  | 0x005ae800 | AudioPoolBlockAlloc | audio_pool_block_alloc |
  | 0x005ae780 | AudioPoolDestroy | audio_pool_destroy |
  | 0x005ae920 | AudioPoolFree | audio_pool_free |
  | 0x005addd0 | AudioListInsertHead | audio_list_insert_head |
  | 0x005ade10 | AudioListRemoveByValue | audio_list_remove_by_value |
  | 0x005ade90 | AudioListDrain | audio_list_drain |

- Inside the family, calls go by name (Construct -> BlockAlloc / Destroy, Destroy -> Free, the list three ->
  BlockAlloc / Free), as slice 1's AudioWaveFmtCopy calls EndianPack. In the .asi the original's RVAs of those
  callees are hooked to the same bodies.
- Exe arm (`MASHED_STANDALONE`):
  - Raw alloc / free (0x005aea00 / 0x004522d0) = CRT malloc / free, tag dropped. This is the declared deviation of
    Audio/AudioMemory.cpp's AudioRawAlloc / AudioRawFree.
  - The three globals become private zero storage: 0x007ddab0 (header pool pointer), 0x007dda80 (list head pointer
    and +4), 0x009146c0 (node pool header, 9 dwords as 0x005ae650 writes them). `POOL_CONSTRUCTOR.md`
    (audiolink) shows all three are BSS until the audio open (0x004627f0 -> 0x005a5f00 -> table 0x00632ba8), so
    zero is the original's pre-open state. Nothing in the exe calls the family, so it is exe dead code until the
    open chain is ported.
- .asi arm: unchanged addresses (the trampolines and the three globals at their RVAs).
- Bodies: 0x005ae650 / 0x005ae800 / 0x005ae920 keep the 2026-10-12 re-port from the bytes. 0x005ae780 and the
  list three were compared against `disasm_rws2.txt` in this session and the logic matches. Two byte details
  are taken up:
  - 0x005addd0 re-reads `*param_1` after the node stores (0x005addf9). The port read it once. It now re-reads.
  - EAX: 0x005addd0 leaves the node (or 0 from the alloc) in EAX (0x005adde6 / 0x005ade00). 0x005ade90 leaves
    `param_1[1]`, which equals `param_1` at exit (0x005adeb6..0x005adec1). Both ports become pointer-returning.
    0x005ae780's EAX differs by path (free result, or `param_1+0x10` on the owned path at 0x005ae7b2), so it
    stays void. No caller is checked. Static evidence only, since the registry entries do not read the return.

## Arms

- **P path1.** `run_diff.py` audio_pool_construct, audio_pool_block_alloc, audio_pool_destroy, audio_pool_free,
  audio_list_insert_head, audio_list_remove_by_value, audio_list_drain, with the worktree's dev .asi (never deployed).
- **G.** The worktree exe, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, private root
  `D:/MashedWT/D4-AUDIO-RWS_root`, under `mashed_lock.py run`.
- **L.** `scripts/lint_rva_bodies.py`.

## Predictions

- **P.** 7 / 7 GREEN.
- **G.** Unchanged: G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a.
- **L.** 0 NEW.

## Decision rule

All three hold: commit, hooks.csv file + exe_file -> Audio/AudioPool.cpp for the seven, levels kept (re-classify
transaction). P fails: report the failing names, no tracker move. G fails: stop and report.
