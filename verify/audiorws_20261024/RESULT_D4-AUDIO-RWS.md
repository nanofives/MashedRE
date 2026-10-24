# RESULT: D4-AUDIO-RWS slice 2, pool and list family in Audio/AudioPool.cpp

PREREG: `PREREG_rws2.md` (commit `f2be14ce4`, alone). Branch `orch/D4-AUDIO-RWS`. Build: exe `78b71d4a`, dev .asi
`37b190e2` (worktree, not deployed). Evidence: `D:/MashedWT/D4-AUDIO-RWS_data/path1/P_*.txt`, `arms_rws2.txt`,
`disasm_rws2.txt`, build log `D:/MashedWT/D4-AUDIO-RWS_build1.log`.

**Verdict: P, G and L hold.** These seven moved from the asi-only Audio/AudioRws.cpp into Audio/AudioPool.cpp, in
both rsp files, one body each, exe dead code:

| RVA | Function | path1 cases | mismatches |
|---|---|---|---|
| 0x005ae650 | AudioPoolConstruct | 9 | 0 |
| 0x005ae800 | AudioPoolBlockAlloc | 15 | 0 |
| 0x005ae780 | AudioPoolDestroy | 7 | 0 |
| 0x005ae920 | AudioPoolFree | 13 | 0 |
| 0x005addd0 | AudioListInsertHead | 4 | 0 |
| 0x005ade10 | AudioListRemoveByValue | 10 | 0 |
| 0x005ade90 | AudioListDrain | 11 | 0 |

- **P:** path1 7 / 7 GREEN with the worktree's dev .asi (loaded by `run_diff.py` from `mashedmod/build`).
- **G:** exe `78b71d4a`, private root `D:/MashedWT/D4-AUDIO-RWS_root`. H_DEF bacfc747 / 8dd1c004 / 6f9932b6,
  B_DEF_K0 / K1 / K26 f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.
- **L:** `lint_rva_bodies.py` 8 findings, all allowlisted, 0 NEW.
- Exe arm: CRT malloc / free for 0x005aea00 / 0x004522d0 (AudioMemory.cpp's declared deviation). Private zero
  storage for 0x007ddab0, 0x007dda80 (+4) and 0x009146c0 (9 dwords). This is the pre-open BSS state per
  `verify/audiolink_20261023/POOL_CONSTRUCTOR.md`.
- Body changes against the bytes (static evidence, the harness does not read returns):
  - 0x005addd0 re-reads `*param_1` (0x005addf9) and returns the node in EAX. It was void.
  - 0x005ade90 returns `param_1[1]` (0x005adeb6). It was void.
  - 0x005ae780 stays void, because its EAX differs by path.
- Trackers (re-classify, `apply_trackers_rws2.py`): file + exe_file moved to Audio/AudioPool.cpp for the seven, C3
  kept, one CHANGELOG line. No U- or S- rows created.
- Process note: `run_diff.py` does not honour `MASHED_ROOT`. path1 spawned the shared `original/MASHED.exe` (hook
  bypassed, .asi loaded via Frida, nothing deployed), the same way as slice 1, under `mashed_lock.py run`.

## What is left of AudioRws.cpp (asi-only, 13 installs)

- AudioFmtTableSearch 0x005acd10
- AudioFmtGlobalScan 0x005acd60 (table 0x00633674 keys dumpable)
- AudioSubStructAFree 0x005ae080 (pool 0x007dda28)
- AudioSubStructBFree 0x005ae050
- AudioSubStructCleanup 0x005ae030
- AudioSubStructTwoCallInit 0x005a9e10
- AudioSubStructLinkDevice 0x005ae010
- AudioSubStructLinkBuffer 0x005adfe0
- AudioSubStructDualInit 0x005ac7b0
- AudioWaveNodeFree 0x005abcb0 (pool 0x007dd634)
- AudioSubStructBufCleanup 0x005ac740
- AudioContextLookup 0x005ac900 (needs 0x005ac930, no hooks.csv row)
- AudioSubStructLinkBoth 0x005ad540

The 20 to 13 count settles the step note: the file held 20 active installs before this slice, so NEXT_SESSION's
"19" was one short.

Next slice candidates:
- The sub-struct group: 0x005ae080 / 0x005ae050 / 0x005ae030 / 0x005ae010 / 0x005adfe0 / 0x005ac7b0 / 0x005ad540 /
  0x005a9e10 / 0x005ac740 / 0x005abcb0. These can now call AudioPoolFree by name, with private storage for
  0x007dda28 / 0x007dd634.
- AudioFmtTableSearch + AudioFmtGlobalScan, which need 0x005ac9e0 and the 0x00633674 table.
