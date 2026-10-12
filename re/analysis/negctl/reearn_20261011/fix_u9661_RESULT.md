# U-9661 fixture: 0x005ade90 list drain (keys `audio_list_drain`, `audio_list_drain2`)

**Verdict: GREEN.** The installed body reads GREEN 11/11 on a fixture that runs the loop body and the pool free on
both arms. The negctl kit is healthy. No port defect.

## Which body is installed at 0x005ade90

| Copy | File | Install | Registry key |
|---|---|---|---|
| `AudioListDrain` | `mashedmod/src/mashed_re/Audio/AudioRws.cpp:893` | **live** `RH_ScopedInstall(AudioListDrain, 0x005ade90)` at `AudioRws.cpp:913` | `audio_list_drain` |
| `AudioListDrain2` | `mashedmod/src/mashed_re/Audio/AudioDSound.cpp:370` | commented out at `AudioDSound.cpp:396` ("first-wins; superseded") | `audio_list_drain2` |

The U-9661 row and the brief have this the wrong way round. The AudioRws.cpp copy is the live one. The AudioDSound.cpp
copy is the dead one. hooks.csv row 405 (the only row at 0x005ade90) names the **dead** copy (`AudioListDrain2`,
`AudioDSound.cpp`) and cites `log/diff_audio_list_drain2.csv`. Both TUs are in `mashedmod/asi_sources.rsp` (lines 39
and 43), so both exports exist in the .asi. Memory `duplicate-rva-implementations-drift` applies.

The two bodies differ in one way. `AudioListDrain2` calls the pool free by address (`0x005ae920`, original code).
`AudioListDrain` calls the port's `AudioPoolFree` (AudioRws.cpp:743, installed at 0x005ae920, row 413 C3). So the
installed path is port drain plus port pool free.

## Original bytes (MASHED.exe.unpatched)

```
0x005ade91 mov esi,[esp+8]            ; sentinel
0x005ade95 mov eax,[esi+4]            ; current = sentinel[+4]
0x005ade98 cmp eax,esi / je 0x005adec0
0x005ade9c ecx=[eax]; edx=[eax+4]; 0x005adea7 [ecx+4]=edx     ; prev[+4] = node[+4]
0x005adeaa ecx=[eax+4]; edx=[eax]; 0x005adeaf [ecx]=edx       ; next[+0] = node[+0]
0x005adea2 push 0x9146c0 / 0x005adeb1 call 0x005ae920          ; pool free(0x009146c0, node)
0x005adeb6 eax=[esi+4]; cmp; jne 0x005ade9c
```

Both ports match these bytes. Pool free 0x005ae920 (read for the fixture design): walks blocks from h[+0x10]
(0x005ae928); item range `[blk+nbytes+8, +cap*elem]` with an inclusive end (0x005ae93d..0x005ae950); clears bit
`0x80>>(idx&7)` of `blk[8+(idx>>3)]` (0x005ae97c..0x005ae98b); unlinks the block (0x005ae98f); if the list is still
non-empty (0x005ae9a0), `flags&2` (0x005ae9a4) and every bitmap byte is 0 (0x005ae9b2..0x005ae9c1), calls
0x004522d0(blk) (0x005ae9c4), else relinks the block at the head (0x005ae9d1 / 0x005ae9e2). 0x009146c0 is in .data
BSS (offset past SizeOfRawData).

## What the old fixtures got wrong

- `audio_list_drain` (the installed body): `path1_tests` all `0`. Every vector was an empty sentinel, so the
  `je 0x005adec0` at 0x005ade9a was always taken. The loop body and the pool free never ran. Its GREEN 10/10 was
  degenerate.
- `audio_list_drain2` (the dead body): built nodes with the game's insert 0x005addd0, which takes nodes from the pool
  at 0x009146c0. That pool is not set up at inject time, and the insert AVs at 0x0 on both arms
  (`ins_O/ins_R: access violation accessing 0x0`, 8 of 10 vectors in
  `evidence/fix_regress_audio_list_drain2.csv`) before the target runs. The 2 GREEN rows were the `0` vectors, also
  empty lists.

## Fixture design (`re/frida/negctl_fixtures/z_fix_u9661.py`)

`scalars_to_scattered_globals`, scratch 0x800 filled 0xEE, `freeze_main`. No insert call. The list is built directly
by seeds:

- Pool header **seeded at 0x009146c0** (elem, cap, nbytes, align, block-list head/tail, flags, 0x1c bytes).
  The harness saves and restores seeds around each arm.
- Pool blocks B1 (scratch+0x200) and B3 (scratch+0x400), with bitmaps. Their block-list sentinel is the real
  0x009146d0.
- List sentinel at scratch+0x180. The nodes are pool items (`blk + 8 + nbytes + elem*k`), so each pool free does
  real work.
- Fake RwGlobals at scratch+0 (`0x007d3ff8` -> scratch, +0x10c = 0x004099e0, which records its arg at 0x008a95a0).
  This is the same recorder as `z_fix_audio.py`, so a RwFree from the pool free can be observed.
- Observed: scratch+0x100..0x700, 0x009146c0 (0x1c bytes), 0x008a95a0 (fill 0xA5).

11 vectors: empty list; 1 node; 3 nodes in slot order; the same 3 in another order; 5 nodes with other bitmap bits
left set; flags 2 where B1 empties with B3 still listed (RwFree(B1)); flags 2 where B1 keeps a bit and B3 empties
(RwFree(B3)); flags 2 with a lone block emptying (list empty, relink, no free); nbytes 2 / cap 12 (byte 1); a node
outside every block (unlink only, pool free no-op); a node one past the inclusive end plus an unaligned node (idx 1).
All 11 fingerprints are distinct. The same vectors apply to both keys (`path2_tests` = vector 2).

## Evidence

| Key | run_diff | self-baseline | negctl |
|---|---|---|---|
| `audio_list_drain` (installed) | GREEN 11/11, `evidence/fix_diff_audio_list_drain.csv` | GREEN 11/11, `evidence/fix_diff_audio_list_drain_selfbaseline.csv` | `evidence/fix_negctl_u9661.tsv`: real GREEN, baseline GREEN, null RED 10/11, field_first RED 10/11, field_last RED 10/11, field_none CTRL-GREEN, ret SKIP (void) |
| `audio_list_drain2` (dead) | GREEN 11/11, `evidence/fix_diff_audio_list_drain2.csv` | GREEN 11/11, `evidence/fix_diff_audio_list_drain2_selfbaseline.csv` | `evidence/fix_negctl_u9661_drain2.tsv`: same pattern |

The mutants read 10/11, not 11/11, because vector 0 is the empty list. A port that does nothing matches the original
there. Raw negctl runs: `runs/fix_u9661/`. Built with `MASHED_NO_DEPLOY=1`. No harness change (diff_template.js and
run_diff.py untouched). No path2, no tracker edits.

## For the parent

1. **Repoint hooks.csv row 405** to the installed copy: name `AudioListDrain`, file `Audio/AudioRws.cpp`,
   frida_diff `re/analysis/negctl/reearn_20261011/evidence/fix_diff_audio_list_drain.csv`. Also drop the notes
   "harness-limited (empty-drain only; pool uninit at inject-time)", which no longer hold. Level stays C3.
2. **Dead copy:** `AudioListDrain2` (AudioDSound.cpp:352-397) has no callers in `mashedmod/src`. Its only references
   are its own export, the `audio_list_drain2` registry key, hooks.csv row 405, and two generated CSVs
   (`re/console/match/xbuild_match_v2.csv:1660`, `re/parity/matchdiff_sweep_c3.csv:903`). I did not delete it,
   because removing it would orphan the `audio_list_drain2` registry key, and `hooks_registry.py` is outside my
   brief. Recommendation: delete the AudioDSound.cpp copy and the `audio_list_drain2` registry entry in one change,
   after step 1.
3. `re/parity/matchdiff_sweep_c3.csv:904` marks the installed copy as a static matchdiff FAIL (extra data refs
   0x009146c8/0x009146d0/0x009146d8). Those addresses are pool-header fields, which fits the port's `AudioPoolFree`
   being compiled into the drain. [UNCERTAIN] I did not disassemble the .asi to confirm the inlining. Behaviourally
   the installed path is GREEN on vectors that run the pool free's bitmap, relink and RwFree branches.

## Callers / callees

- Callees: 0x005ae920 AudioPoolFree (row 413, C3). 0x004522d0 FUN_004522d0 (row 109, C2), reached through the pool
  free only.
- Direct callers (E8 to 0x005ade90 in .text, 9 sites). The caller is the nearest preceding hooks.csv row, and
  [UNCERTAIN] function bounds were not checked: 0x005a7a13 -> FUN_005a79a0 (C2); 0x005aa1be ->
  AudioTreeWalkPredicateSearch 0x005aa0c0 (C3); 0x005afb3e, 0x005afbcd -> FUN_005afa00 (C2); 0x005afea6, 0x005afee8,
  0x005aff02 -> FUN_005afcf0 (C2); 0x005b0619, 0x005b06dc -> FUN_005b04b0 (C2).
- Sibling 0x005addd0 AudioListInsertHead (row 783, C3) was not exercised. This fixture builds lists by seed.

## New U-rows

None. U-9661 can close once row 405 is repointed. Item 2 is cleanup, not a knowledge gap.

## Summary

The installed body at 0x005ade90 is `AudioListDrain` in AudioRws.cpp. The commented-out copy is `AudioListDrain2`
in AudioDSound.cpp, which is the one hooks.csv row 405 cites. The old `audio_list_drain2` vectors crashed in the
game's insert 0x005addd0 because the pool at 0x009146c0 is not set up at inject time. The old `audio_list_drain`
vectors were all empty lists that never entered the loop. The new fixture seeds the pool header at 0x009146c0 and
builds the list and pool blocks in scratch. Nodes are pool items, so all 10 non-empty vectors run the unlink, the
bitmap clear, the block relink and the flags-2 RwFree branch on both arms. Both keys read run_diff GREEN 11/11 and
self-baseline GREEN 11/11. negctl: null, field_first and field_last RED 10/11, field_none CTRL-GREEN. Verdict GREEN,
no port defect. Repoint row 405 to `AudioListDrain`/AudioRws.cpp and `evidence/fix_diff_audio_list_drain.csv`, then
delete the dead copy together with its registry key.
