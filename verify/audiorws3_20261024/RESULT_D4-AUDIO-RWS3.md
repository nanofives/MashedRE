# RESULT: D4-AUDIO-RWS3 slice 3, the last 13 AudioRws.cpp installs, AudioRws.cpp linked into the exe

PREREG: `PREREG_rws3.md` (commit `8b9ff8c93`, alone). Branch `orch/D4-AUDIO-RWS3`. Build: exe `f614d0f8`, dev .asi
`027b0ce5` (worktree, not deployed). Evidence in this folder: `path1/P_*.txt` (final run per hook),
`path1_try1/` (the six harness errors of the first pass), `path1_runs.txt` (runner progress lines), `arms_rws3.txt`,
`disasm_rws3.txt`, `table_633674.txt`.

**Verdict: P, G and L hold.** Audio/AudioRws.cpp is now in both rsp files. Its 13 remaining installs have one body
each in both targets, and they are exe dead code:

| RVA | Function | path1 cases | mismatches |
|---|---|---|---|
| 0x005acd10 | AudioFmtTableSearch | 10 | 0 |
| 0x005acd60 | AudioFmtGlobalScan | 10 | 0 |
| 0x005ae080 | AudioSubStructAFree | 7 | 0 |
| 0x005ae050 | AudioSubStructBFree | 6 | 0 |
| 0x005ae030 | AudioSubStructCleanup | 6 | 0 |
| 0x005a9e10 | AudioSubStructTwoCallInit | 10 | 0 |
| 0x005ae010 | AudioSubStructLinkDevice | 10 | 0 |
| 0x005adfe0 | AudioSubStructLinkBuffer | 10 | 0 |
| 0x005ac7b0 | AudioSubStructDualInit | 10 | 0 |
| 0x005abcb0 | AudioWaveNodeFree | 7 | 0 |
| 0x005ac740 | AudioSubStructBufCleanup | 5 | 0 |
| 0x005ac900 | AudioContextLookup | 6 | 0 |
| 0x005ad540 | AudioSubStructLinkBoth | 10 | 0 |

- **P:** path1 13 / 13 GREEN with the worktree's dev .asi. On the first pass six runs ended with a harness error
  and no results: five with `freeze: first thread never stopped inside the safe-point modules`
  (`re/frida/js/standins.js:575`), and audio_sub_struct_two_call_init with `attach failed: timeout`. None reported a
  mismatch. All six re-ran GREEN under one lock hold (`path1_try1/` keeps the failed outputs). audio_context_lookup
  is a crash-equal entry (root DAT_009146fc NULL at the menu), so it covers the NULL-root path only.
- **G:** exe `f614d0f8`, private root `D:/MashedWT/D4-AUDIO-RWS3_root`. H_DEF bacfc747 / 8dd1c004 / 6f9932b6,
  B_DEF_K0 / K1 / K26 f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.
- **L:** `lint_rva_bodies.py` 8 findings, all allowlisted, 0 NEW.
- Exe arm: CRT free for RwFree 0x004522d0. Private zero storage for the pool headers 0x007dda28 and 0x007dd634
  (BSS, past .data's raw end at 0x00637000). A private copy of the 0x00633674 table: 9 pointers in table order onto
  the nine 16-byte .rdata keys. A byte-port of LAB_005ac930 (U-1730) as the AudioContextLookup predicate, which is
  static evidence only. It has no row and no install, and the .asi arm still passes the original 0x005ac930.
- Callee declarations are in the new `Audio/AudioRwsCallees.h`. The rva-body scan's second pass (bind by name,
  `scripts/rva_body_scan.py` `_function_defs`) does not skip `;`-terminated declarations, so an in-file `extern` of
  AudioPoolFree with a `{` within six lines was read as a second body of 0x005ae920 (2 NEW DUP-IN-TARGET). That is a
  scanner false positive, recorded here for `re/HARNESS_BACKLOG.md`. The header avoids it without changing the lint.
- Body changes against the bytes: none in logic. The callees are called by name now. No EAX change: 0x005abcb0 and
  0x005ac740 leave path-dependent EAX, so they stay void.
- Trackers (re-classify, `apply_trackers_rws3.py`): exe_file -> Audio/AudioRws.cpp for the 13. 0x005ad540's file
  column moved from its analysis note to the .cpp. C3 kept. One CHANGELOG line. No U- or S- rows created. U-1730
  stays open: its resolution path is creating the function in Ghidra, and that goes through ghidra-sweep.
- Process note: the per-hook lock in `run_path1.py` let other lanes take the lock between hooks.
  `RWS3_NOLOCK=1` under one outer `mashed_lock.py run` holds it for the whole batch.

## What is left of AudioRws.cpp

Nothing is asi-only now. The file holds 13 installs, and all of them are linked into the exe. No exe caller reaches
them yet. They become live when the audio open chain (0x004627f0 -> 0x005a5f00 -> init table 0x00632ba8) is ported.

Open follow-ups:
- 0x005ad540's hooks.csv status is `mapped` while its body is installed. A re-classify status fix belongs to a
  separate step.
- The predicate 0x005ac930 has no path1 entry. A registry fixture with a non-NULL tree (node ring at +0x20, key
  pointer at link-0x40) would measure it and the non-NULL path of 0x005ac900.
