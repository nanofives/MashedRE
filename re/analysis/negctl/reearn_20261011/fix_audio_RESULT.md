# Re-earn cluster AUDIO: result (2026-10-12)

Branch `reearn/audio`, fix commit `9b979690`. All six rows were re-ported from the bytes of
`original/MASHED.exe.unpatched` (capstone, whole function), built with `MASHED_NO_DEPLOY=1`.
Harness: `re/frida/negctl_fixtures/z_fix_audio.py` (loads last, overrides b13/b14). No tracker
edits, no re-classify, no path2 (`run_verify_hook.py` is the parent's step after merge).

Evidence directory: `re/analysis/negctl/reearn_20261011/evidence/`.
negctl run: `runs/fix_audio/negctl_20261012_043305.tsv` (copy: `evidence/fix_negctl_audio.tsv`), handler
`scalars_to_scattered_globals`, classes null, ret, field_first, field_last, field_none, 1 slot.

Callers/callees below come from a `call rel32` scan of the unpatched exe. Each call site is attributed to the
nearest lower `hooks.csv` start, so a site inside a function that has no row is charged to its neighbour.

| row | verdict | run_diff | self-baseline | witness |
|---|---|---|---|---|
| 0x005ac900 audio_context_lookup | FIXED | GREEN 0/6 | GREEN 0/6 | process death 3/3 boots, then RED 5/6 |
| 0x005ae650 audio_pool_construct | FIXED | GREEN 0/9 | GREEN 0/9 | RED 7/9 |
| 0x005ae800 audio_pool_block_alloc | FIXED | GREEN 0/15 | GREEN 0/15 | RED 3/15 |
| 0x005ae920 audio_pool_free | FIXED | GREEN 0/13 | GREEN 0/13 | RED 1/13 |
| 0x005baf90 audio_dsound_release | FIXED | GREEN 0/16 | GREEN 0/16 | RED 8/16 |
| 0x005bc400 audio_dsound_qi_chain | FIXED | GREEN 0/8 | GREEN 0/8 | RED 8/8 |

## 0x005ac900 AudioContextLookup: FIXED

- hook key `audio_context_lookup`, arg_type `scalars_to_scattered_globals` (fixture b13, unchanged)
- Defect: the original builds `{key, 0}` as ONE adjacent pair (`sub esp,8` 0x005ac900, `mov [esp+0x14],eax`
  0x005ac917, `mov [esp+0x18],0` 0x005ac91b), passes `&ctx[0]` to 0x005aa0c0 (0x005ac923) and returns ctx[1]
  (0x005ac928). Predicate 0x005ac930 reads ctx[0] (0x005ac946) and writes ctx[1] = entry-0x40 (0x005ac96a). The
  old port held two unrelated locals: the compiler returned the constant 0 and the predicate's store hit the
  caller frame. Port now uses `uint32_t ctx[2]`.
- Evidence: `fix_diff_audio_context_lookup.csv` GREEN 0/6 (6 distinct fingerprints),
  `fix_diff_audio_context_lookup_selfbaseline.csv` GREEN 0/6. negctl: real GREEN, baseline GREEN, null RED 5/6,
  ret RED 6/6, field_first/last NA (return-only port, no byte write), field_none CTRL-GREEN.
- Witness (separate locals restored alone): the process died on 3 of 3 boots (`frida.TransportError: the
  connection is closed`, `fix_witness_separate_locals_audio_context_lookup.txt`). A 4th run on the same witness
  build completed RED 5/6, reimpl AV at 0xeeeeeeee (`fix_witness_separate_locals_audio_context_lookup.csv`).
- Callee: 0x005aa0c0 AudioTreeWalkPredicateSearch C3. Caller: 0x005ac210 FUN_005ac210 C2 (2 sites).
- Plates corrected: `audio_rws_loader_d3/005ac900.md`, `promote_c2_audio_rws/005ac900.md` (both described two
  separate locals, which is the claim the port copied).
- [UNCERTAIN] U-1730 (identity of the predicate at 0x005ac930) still applies to naming only. Its body is now read.

## 0x005ae650 AudioPoolConstruct: FIXED

- hook key `audio_pool_construct`, arg_type `scalars_to_scattered_globals` (z_fix_audio overrides b14)
- Defects (3):
  1. Header source with param_5 NULL: DAT_007ddab0 != 0 -> `FUN_005ae800(DAT_007ddab0, 0x30800)`
     (0x005ae669..0x005ae678), else `FUN_005aea00(0x24, param_6)` (0x005ae67f..0x005ae686). The old port always
     used FUN_005aea00.
  2. param_4 prealloc loop (0x005ae6db..0x005ae73b): block = FUN_005aea00(hdr[0]*count + nbytes + align + 8,
     param_6) (0x005ae6dd..0x005ae6f4), NULL -> FUN_005ae780(hdr), return 0 (0x005ae764..0x005ae76d), else
     blk[1] = blk[0] = 0, head insert into hdr[4], zero nbytes of bitmap (0x005ae702..0x005ae730). The counter is
     tested for zero only (`jbe` after `test` 0x005ae6db, `dec`/`jne` 0x005ae736). The old port skipped it.
  3. Global list: DAT_007dda80 is a pointer head. hdr[8] = 0x007dda80 (0x005ae746), hdr[7] = DAT_007dda80
     (0x005ae74e), DAT_007dda80[1] = &hdr[7] (global re-read 0x005ae750, store 0x005ae758),
     DAT_007dda80 = &hdr[7] (0x005ae75b). The old port treated 0x007dda80 as a node and tail-inserted through
     0x007dda84.
- Vectors (9): the 3 b14 vectors plus header via RwMalloc, header via FUN_005ae800 on a scratch H2 pool, header
  alloc failure, prealloc 1, prealloc 2, prealloc failure into FUN_005ae780.
- Evidence: run_diff GREEN 0/9 (9 distinct), self-baseline GREEN 0/9. negctl: real/baseline GREEN, null RED 8/9,
  ret RED 9/9, field_first RED 8/9, field_last RED 8/9, field_none CTRL-GREEN.
- Witness (node-style tail insert restored alone): RED 7/9 (`fix_witness_node_tail_insert_audio_pool_construct.csv`).
- Callees: 0x005ae800 AudioPoolBlockAlloc (this cluster), 0x005aea00 FUN_005aea00 C2, 0x005ae780 AudioPoolDestroy C3.
  Callers (16 attributed functions): FUN_005a6710, FUN_005a75b0, FUN_005a9280, FUN_005a9e60, FUN_005aa3d0,
  FUN_005aba20, FUN_005ace70, FUN_005ada40, FUN_005add60, FUN_005adf60, FUN_005beae0, FUN_005c0140 (C2), and
  FUN_005ab980, AudioByte54Bit3Get 0x005ac540, FUN_005ae170, AudioArenaBlockIsFree 0x005ae590 (C3).
- Plate corrected: `audio_rws_loader_d3/005ae650.md` (described the global list as circular around 0x007dda80 and
  had no prealloc or FUN_005ae800 path).
- [UNCERTAIN] U-1736 (role of DAT_007ddab0) remains a naming question. The control flow around it is now ported.

## 0x005ae800 AudioPoolBlockAlloc: FIXED

- hook key `audio_pool_block_alloc`, arg_type `scalars_to_scattered_globals` (z_fix_audio overrides b14)
- Defects (3):
  1. Countdown: seeded per block from h[1] (0x005ae828, kept in the param slot [esp+0x18] 0x005ae82f). A 0xff
     byte subtracts 8 (0x005ae90d), each set bit subtracts 1 (0x005ae85c), countdown == 0 ends the current byte
     (0x005ae848, `test`/`jbe` = zero test, so the unsigned count wraps after a 0xff byte taken below 8). The old
     port scanned all 8 bits of every byte.
  2. New block HEAD insert: new[1] = sentinel (0x005ae8e5), new[0] = old head (0x005ae8e8), old_head[1] = new
     (0x005ae8f0), sentinel[0] = new (0x005ae8f3). The old port tail-inserted via sentinel[1].
  3. A computed slot address of 0 keeps scanning (0x005ae88c). Not reachable with scratch blocks, ported for
     fidelity.
- Vectors (15): the 5 b14 vectors plus multi-byte countdown exhaustion, slot 12, the wrap case (slot 8), slot 16,
  MSB-first slot 0, second-block slot, cap exhausted in block 1, elem 0x14 / align 0x10, empty list, RwMalloc 0.
- Evidence: run_diff GREEN 0/15 (15 distinct), self-baseline GREEN 0/15. negctl: real/baseline GREEN, null RED
  14/15, ret RED 15/15, field_first RED 14/15, field_last RED 14/15, field_none CTRL-GREEN.
- Witness (countdown ignored, alone): RED 3/15, vectors 2, 5, 11 (`fix_witness_no_countdown_audio_pool_block_alloc.csv`).
- Callee: 0x005aea00 FUN_005aea00 C2. Callers (14 attributed): AudioListInsertHead 0x005addd0 (C3),
  AudioPoolConstruct 0x005ae650 (this cluster), FUN_005a6c60, FUN_005a7920, FUN_005a9ab0, FUN_005aba20,
  FUN_005ac7e0, FUN_005ad570, FUN_005ad6f0, FUN_005ad770, FUN_005ae250, FUN_005be8c0, FUN_005c0c20, FUN_005c93a0 (C2).
- Plate corrected: `audio_rws_loader_d3/005ae800.md` (algorithm had no countdown and an unspecified insert).

## 0x005ae920 AudioPoolFree: FIXED

- hook key `audio_pool_free`, arg_type `scalars_to_scattered_globals` (z_fix_audio overrides b14), void
- Defects (2):
  1. Range end is INCLUSIVE: `item < base` -> next (jb 0x005ae943), `item <= base + cap*elem` -> found
     (jbe 0x005ae950). The old port used `<`.
  2. After the unlink, compaction is considered only when the list is still non-empty (`cmp ecx,edi` 0x005ae9a0)
     and flags bit 1 is set (0x005ae9a4). Otherwise relink at head (0x005ae9e2). The old port freed the block
     when it was the list's only member.
  The emptiness test sums bitmap byte values (0x005ae9b2..0x005ae9bd). The old popcount was equivalent for the
  zero test, ported as the sum.
- Vectors (13): the 5 b14 vectors plus one past the end, one before base, unaligned item, two-block RwFree,
  two-block relink, no-flag relink, flags 3 freeing B1, 2-byte bitmap.
- Evidence: run_diff GREEN 0/13 (11 distinct: the two no-op vectors share a fingerprint by construction),
  self-baseline GREEN 0/13. negctl: real/baseline GREEN, null RED 11/13, ret SKIP (void), field_first RED 11/13,
  field_last RED 11/13, field_none CTRL-GREEN.
- Witness (exclusive end, alone): RED 1/13, vector 2 (the b14 end-slot vector)
  (`fix_witness_exclusive_end_audio_pool_free.csv`).
- Callee: 0x004522d0 FUN_004522d0 C2. Callers (18 attributed): C3 = AudioPoolFreeGuardA/B/C, AudioWaveNodeFree,
  AudioListRemoveByValue, AudioListDrain2, AudioSubStructAFree, AudioPoolDestroy. C2 = FUN_005a7ea0,
  FUN_005a9be0, FUN_005a9f60, FUN_005aac20, FUN_005ac880, FUN_005ad5f0, FUN_005be990, FUN_005c0c90,
  FUN_005c93a0, FUN_005c9490.
- Plate corrected: `audio_rws_loader_d2/005ae920.md` (no inclusive end, no non-empty test).

## 0x005baf90 AudioDSoundRelease: FIXED

- hook key `audio_dsound_release`, arg_type `scalars_to_scattered_globals` (new fixture, was int_scalar
  crash-equal)
- Defects (2):
  1. Slot 8 (`call [ecx+0x20]` 0x005baf9f) takes TWO stack args: `push &[esp+4]` (the caller's arg slot, still
     holding param_1, 0x005baf94/0x005baf98) and the object `[param_1+0x7c]` (0x005baf99/0x005baf9c), callee
     cleans. The old port passed one.
  2. Return: low byte of the out dword (0x005bafa2/0x005bafa6), `dec`, unsigned `cmp 6`/`ja` (0x005bafab..
     0x005bafaf), table at 0x005bafdc: 1->3 (0x005bafbe), 2->2 (0x005bafb8), 3->1, 4->1 (0x005bafd6), 5->4
     (0x005bafc4), 6->5 (0x005bafca), 7->6 (0x005bafd0), other -> 1. The old port returned 1.
- Fixture: scratch object whose slot 8 is an exe leaf. 0x005c08a0 writes `*out = this[+0x10]`, returns 0.
  0x005c07cf writes 0. 0x0049fa30 writes nothing, so the out dword keeps the param address (the low byte comes
  from the shared scratch base).
- Evidence: run_diff GREEN 0/16 (16 distinct), self-baseline GREEN 0/16. negctl: real/baseline GREEN, null RED
  16/16, ret RED 16/16, field_first/last NA (return-only), field_none CTRL-GREEN.
- Witness (constant return 1, call kept 2-arg so the stack stays balanced): RED 8/16
  (`fix_witness_const_ret1_audio_dsound_release.csv`). The 1-arg call alone was not used as the witness: under a
  callee-cleans 2-arg method it unbalances the stack.
- Callees: none direct (COM slot). Callers: FUN_005ba1d0 C2 (0x005ba66e), and site 0x005bb7e6 attributed to
  AudioPcmSaturatedAdd 0x005bb5b0 C3 by nearest row. That attribution needs a containing-function check before
  it counts for the gate.
- Plates corrected: `audio_dsound_d4/0x005baf90.md` (title and body said "always returns 1"),
  `promote_c2_audio_dsound/0x005baf90.md`.
- [UNCERTAIN] U-0361 (identity of slot 8) remains open. The port does not depend on it.

## 0x005bc400 AudioDSoundQIChain: FIXED

- hook key `audio_dsound_qi_chain`, arg_type `scalars_to_scattered_globals` (new fixture, was int_pair
  crash-equal)
- Defect: param_1 IS the object: `mov eax,[esp+4]` (0x005bc400), `mov ecx,[eax]` (0x005bc420), `call [ecx]`
  (0x005bc422). The old port dereferenced once more. Rest re-read and matching: out[0] = out[1] = 0
  (0x005bc413/0x005bc41a), sign tests `jl` (0x005bc426, 0x005bc437) so S_FALSE counts as success, Release via
  `[[out[0]]+8]` with out[0] re-read (0x005bc441..0x005bc446).
- Fixture: QI slots are exe leaves. 0x005beda0 writes `*ppv = this[+0x128]` and returns 0, 0x005bf6fb writes 0
  and returns E_NOINTERFACE, 0x0049cf50 returns E_NOTIMPL without writing, 0x0049c7f0 returns 1 without writing.
  Release = 0x005044b0 (`inc [this+4]`, observable).
- Evidence: run_diff GREEN 0/8 (8 distinct), self-baseline GREEN 0/8. negctl: real/baseline GREEN, null RED 8/8,
  ret RED 8/8, field_first RED 8/8, field_last RED 8/8, field_none CTRL-GREEN.
- Witness (extra deref, alone): RED 8/8, reimpl-only AV / illegal instruction, original clean
  (`fix_witness_extra_deref_audio_dsound_qi_chain.csv`).
- Callees: none direct (COM slots). Caller: FUN_005ba1d0 C2 (0x005ba5a4).
- Plates corrected: `audio_dsound_d4/0x005bc400.md` (said "HRESULT != 0"), `promote_c2_audio_dsound/0x005bc400.md`.
- [UNCERTAIN] U-0360 (IID at 0x005d09dc) remains open. The port passes the address through.

## Regression sweep (other C3/C4 hooks with bodies in AudioRws.cpp / AudioDSound.cpp)

`evidence/fix_regress_audio.txt`: 29 of 30 GREEN on the fixed build. `audio_list_drain2` (0x005ade90, the
AudioListDrain2 copy in AudioDSound.cpp whose `RH_ScopedInstall` is commented out as superseded) reads RED 8/10
on 2 boots. All 8 mismatches are identical `access violation accessing 0x0` on BOTH arms in the `ins_` prep stage,
before the target runs, with 0 reimpl-only differences (`evidence/fix_regress_audio_list_drain2.csv`). Its body
is outside every hunk of 9b979690, and the live twin `audio_list_drain` at the same RVA is GREEN 10/10.
Verdict: pre-existing fixture/live-state issue in the drain2 entry, not a regression from this change. The
parent should look at it: the hooks.csv row for 0x005ade90 cites `log/diff_audio_list_drain2.csv` as its C3
evidence.

## Notes for the parent

- Build hygiene trap hit and recovered: restoring the witness backups with `shutil.copy2` kept the old mtimes,
  so the next incremental build did not recompile and one run_diff pass ran the witness .asi. Those CSVs were
  discarded, the sources touched, rebuilt (`(source newer)` confirmed), and every `fix_diff_*.csv` in evidence
  is from that rebuilt fixed .asi (GREEN x6).
- path2 (`run_verify_hook.py`) not run, per brief.
