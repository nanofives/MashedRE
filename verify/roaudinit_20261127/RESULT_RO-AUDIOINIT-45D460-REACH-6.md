# RESULT RO-AUDIOINIT-45D460-REACH-6 (run 57, G2): met. 0x00604604 at record 0 (tick 1) is IDENTICAL to ref1: 0x00000004 (signed 4) on both sides

Branch `orch/RO-AUDIOINIT-45D460-REACH-6`, rebased from REACH-5's `dadf8ee25` onto orch/integration `c880ab12e`. Worktree
`.worktrees/RO-AUDIOINIT-45D460-REACH-5` (carried). Worker on claude3, Opus 5.5 medium (moved from claude2 at the start).
Row kind: race-path / top-down (audio closure). It ran both builds, the ddkobj check (in the object build), the binder
selftest, the knob-on boots and the RO arm on the object build (L-125). No G-CTRL, no B, no 12 captures.

## Verdict: GREEN (G2, decision B, measured number)

- Compare: `re/frida/detorig_firstdiv.py D:/MashedWT/RO-REF-NAME_data/ref1 <run>/RO_ROAUDINIT-pr1` (ref1 = the replay
  oracle reference, `verify/ro_20261116/RESULT_RO-REF-NAME.md`), and the dword itself read from the tick-1 image dumps
  of both runs (`dword_at.py`):
  - ref1 `ckpt201.data.bin` (tick 1, sha256 fef65df0..., equal to its meta.json sha256): `[0x00604604] = 0x00000004`.
  - run `ckpt00.data.bin` (tick 1): `[0x00604604] = 0x00000004`.
  - detorig_firstdiv re-hashed both tick-1 dumps against their tick records: OK on both sides. The block holding
    0x00604604 (0x00604400) is not in the 94 blocks that differ at record 0.
- REACH-5's stop is gone: the RO arm runs 600 ticks and exits 0 in 7 s (REACH-5: 0xC0000005 after 2 s, 0 ticks).
  Of REACH-5's 5 unbound functions before the write, 0x005c78c0 is bound. The 4 CRT internals under the reused malloc
  body stay unbound and are not reached by a bound body (section 7). The write happens with them unbound.
- Not the whole race: the run diverges from ref1 at record 0 in 94 other blocks (76 raw words, non-PTR, unmasked). The
  NEW-block verdict is IDENTICAL-FOR-N 17 of 600, first NEW block 0x00898c00 at record 17. Those are outside this row.

## 1. Rebase and 0x004ccef0 (one row, one body)

- `git rebase orch/integration` (43 commits). Conflicts in `re/analysis/CHANGELOG.md` (both sides' lines kept) and
  `re/match/PROVISIONAL.tsv` (head's 4 rows and the branch's rows kept, one row dropped, below). Commits: `df893b5cf`
  (REACH-5 bind), `1fd6e3761` (REACH-5 RESULT).
- **Kept for 0x004ccef0: the head's row (line 87, `RwCoreHand_004ccef0`, RwCore/RwCore_Hand.cpp) and its hand body.**
  Dropped: REACH-5's second row (FUN_004ccef0, re/match/s20/004ccef0.c) and its exe TU `Match/PortTop_004ccef0.cpp`.
  Why:
  - 0x004cce20 (generated RW core TU `RwCore/RwCore_004cce20.cpp:10`) already installs `RwCoreHand_004ccef0` at
    RwGlobals+0x108. Keeping the re/match body would have meant editing that generated TU.
  - Both bodies compute the same thing: `RwCoreHand_004ccef0` is `std::malloc(size)` (RwCore_Hand.cpp:36), and the
    re/match body calls `MashedCrt_malloc_004a45fb`, which is `std::malloc(n)` (Match/PortTop_CrtHeap.cpp:22).
  - The s20 source scores 0.0% (tail jmp under every flag set REACH-5 tried), so the exe loses no matched code.
  - How: `state/audio/rules.tsv` `prov_bind 004ccef0` replaced by `alias 004ccef0 RwCoreHand_004ccef0 RwCore/RwCore_Hand.cpp`.
    The binder now plans 0x004ccef0 as reuse, and `Match/PortTop_005aea60.cpp` defines `FUN_004ccef0 RwCoreHand_004ccef0`.
    re/match/s20/004ccef0.c stays in the tree as an RE artifact (hooks.csv C1), not compiled.
- Hand edits the tool cannot do, with the reason: the binder's `sync` and `bindings` only add, so the stale
  `"Match\PortTop_004ccef0.cpp"` line was removed from `mashedmod/exe_sources.rsp` and the BINDINGS.tsv row 0x004ccef0
  (it named the deleted TU) was rewritten to `near-match RwCoreHand_004ccef0` (hand body, L-126 provisional). The
  stale `DUP-IN-TARGET:004ccef0` allowlist line was removed by the tool (`porttop_bind.py prune --profile audio --apply`).
- `git show HEAD:re/match/PROVISIONAL.tsv | cut -f1 | sort | uniq -d` prints nothing. PROVISIONAL.tsv rows 992.
  REACH-5's 4 new keys (0x005a65d0, 0x005b8910, 0x005b8c40, 0x005c6f50) are in it.

## 2. 0x005c78c0 bound

- `re/console/pc_funcs.csv`: `0x005c78c0,0x005c792c,109,FUN_005c78c0` (extent from the REACH-5 listing of
  MASHED.exe.unpatched, last byte the `ret` at 0x005c792c).
- Source `re/match/s28/005c78c0.c`, from the listing 0x005c78c0..0x005c792c: with DAT_007de1b0 non-zero (0x005c78c6), calls
  0x005a6dc0(DAT_007de1b0[i], 6, 1, 0) for i below DAT_007de1b4, unsigned (0x005c78d5..0x005c78f4), then stores
  param_3[1] / param_3[0] at 0x007de1b0 / 0x007de1b4 (0x005c7902 / 0x005c790a), or 0 into both (0x005c7918 / 0x005c7922),
  returns param_1.
  - match.py --bytes (DDK, /O2 /Ob1 /G5): **97.1%**, 34/34 ins, 7 differing bytes. The one difference is where
    `xor esi, esi` sits (ours before the first test, the original after the 0x007de1b4 load). Six source shapes were
    tried, the best is committed.
  - relocheck: BAD (checked 9, unchecked 0), the same targets at shifted offsets (the moved `xor`).
  - equiv.py (tool commit 1ed7ceb1a, the merged one): **EQUIV**, classes ORDER, rule A7-stack-image, mode frame.
  - equiv_dyn.py, 32 runs: SAME 32/0/0 uniform, SAME 32/0/0 seeded.
  - So it is **L-126 admitted** (plain EQUIV, no decision row): `re/match/ADMITTED.tsv` row (source commit 9d06ec7b7).
    Not C4, no promotion row.
- hooks.csv: `c1_add.py` (none)->C1, subsystem audio, and its CHANGELOG line.
- Binder: `state/audio/roots.txt` + `005c78c0` (no source names it, it is reached only through the slots). `write`
  (bind 459 reuse 87 stop 73; head of this branch before: bind 459 reuse 85 stop 74), `sync` (rsp +1), `bindings`
  (added 1, kind matched / L-126 admitted), `externs --profile audio` (0 unguarded), `samesrc` (4, all pre-existing),
  `refs --profile raceload` (0 files changed).

## 3. Code-pointer slots 0x006350cc / 0x00636024

- Neither slot has a Ghidra data reference (`dataref_raw.tsv`), so `codeptr_image.py` could not list them. Tool change
  in this row (`re/tools/image/codeptr_image.py`): a new input `re/tools/image/CODEPTR_EXTRA.tsv` (slot, target,
  evidence). The tool refuses a row unless the slot is 4-aligned, the file dword of MASHED.exe.unpatched at the slot
  equals the target and the target is a pc_funcs.csv entry. Both rows pass: the dword at 0x006350cc and at 0x00636024
  is 0x005c78c0.
- The first regeneration failed the VS2022 link (LNK2019 `_PortTop_Bound_005be160`, `_PortTop_Bound_005bf670`). The head's
  CODEPTR_IMAGE.tsv was older than BINDINGS.tsv, and those two targets are `__stdcall` in their sources
  (re/match/s28/005be160.c:33, re/match/s28/005bf670.c:29) while `CP_IMG_EXTERN` declares `extern "C" void sym(void)`
  (Compat/PortTopCodePtr.cpp:37). Second tool change: a target whose re/match source defines it `__stdcall` stays `trap`.
- Result against the head's PortTopCodePtrImage.inc: 107 -> **113** bound slots. 2 are this row's (0x006350cc, 0x00636024).
  **4 are not this row's RVAs**: 0x00635184 -> 0x005c80a0, 0x00635188 -> 0x005b7440, 0x0063518c -> 0x005b6b80,
  0x00635190 -> 0x005bce20. They are trap on the head and bound now only because the regeneration reads today's
  BINDINGS.tsv (their bodies already link). Reported to the lead. The image slots line of the RO arm: 113 written, 0
  refused.
- The 0x006350cc / 0x00636024 records have more code pointers that no list holds (0x006350b4 -> 0x005bb800, 0x006350e4 ->
  0x005bb750, 0x0063600c -> 0x004d7ff0, 0x0063603c -> 0x005c8050). Not listed, not owned by this row.

## 4. Object build (ddkobj check)

- **IDENTICAL 2458/2567, DIFFERENT 81**, not-in-map 28 (`build6b_ddk.log`).
- Against run 56 drain 3 (c8ea587cf, IDENTICAL 2455/2562, DIFFERENT 79), `cmp_head6.py`: new DIFFERENT **0x005aeb90** and
  **0x005c78c0**, gone none.
  - 0x005aeb90: replay-C4 body (c4=frida-replay, re/match/s26/005aeb90.c, PROV-INSITU-REF1 PROMOTIONS.tsv line 34), as
    REACH-5 classified it.
  - 0x005c78c0: the L-126 admitted body above (97.1%).
- IDENTICAL +3 are REACH-5's 0x005a66c0, 0x005a7420, 0x005c6bb0. Listed unit -> 100% neighbour that fell back to its
  VS2022 body: none new.
- One `[mashed-lock] WAITING (ddkobj check)` for a merge-train PRIORITY-HOLD, cleared after 42 s.

## 5. Checks run

| check | result |
|---|---|
| `mashed_lock.py status` before each build and arm | free (the ddkobj check waited 42 s by itself) |
| test-slot `--status` before the builds | slots free, C: 59 GB free |
| VS2022 build `build6a` | rc 1, LNK2019 (section 3), fixed |
| VS2022 build `build6b` | rc 0, 164 s, rva-lint 664 findings all allowlisted, **NEW=0** |
| object build `build6b` (MASHED_DDKOBJ=1) | rc 0, 510 s, ddkobj as section 4 |
| binder selftest (RULE 7) | **PASS**, every profile (audio plan IDENTICAL bind 459 reuse 87 stop 73, raceload refs IDENTICAL, BINDINGS.tsv IDENTICAL) |
| prov_drop.py (dry run) | PROVISIONAL.tsv 992 rows, DROP 0 |
| c4_reclass.py batch --tsv re/promotions/RO-AUDIOINIT-45D460-REACH-5.tsv (dry run) | 1 pass (0x005a66c0 match), 0 refused |
| RO arm RO_ROAUDINIT-pr1, object build | exit 0, 600 ticks, self-tests: portstate-image PASS 0/8, OT_CODEPTR PASS 0/373, [REPLAY] init 66 checks 0 failed, OT_AUDIOINIT PASS 0/12, OT_TIMER PASS 0/5990, [TRACE] PASS 0/33294, [REPLAY] PASS 0/666 |
| knob-on boots vs_init_audio / ddk_init_audio | rc 0 each, both end at `[OT_TIMER] STOP: ... called original 0x005C70D0` (the audio-tick path, no replay; not G2's path) |
| detorig_firstdiv ref1 vs run | first divergent record 0, 0x00604604 equal (section Verdict) |
| Frida temp folders | 0 before, 0 after (this row ran no Frida harness), deleted 0 |

## 6. RULE 9 counts (sources the exe includes for this closure)

- match (byte-identical + relocheck OK): 3 (0x005a66c0, 0x005a7420, 0x005c6bb0, REACH-5's `rule9_match_reloc.txt`).
- replay-C4: 1 (0x005aeb90).
- admitted (L-126, not C4): 1 (0x005c78c0, 97.1%, relocheck BAD by layout).
- provisional (not C4): 4 re/match sources (0x005b8910, 0x005b8c40, 0x005c6f50, 0x005a65d0), plus the hand body
  `RwCoreHand_004ccef0` (no re/match source in the exe for 0x004ccef0 now).

## 7. Not done (unfinished scope, listed)

- The 4 CRT internals under the reused malloc body (0x004a4554, 0x004a45cf, 0x004a59bf, 0x004a5984): not bound. No bound
  body calls them (0x004a45fb is reused as `MashedCrt_malloc_004a45fb`, a std::malloc wrapper).
- REACH-5's `--watch-write` leg on +0xc: not run.
- The code trap (B17 pad above 0x00401000, stops show as raw AVs): not touched.
- `mashedwt_cleanup.py prep` still does not stage `original\mashed_image.bin` (REACH-5's root, which has it, was reused).
- The knob-on audio-tick stop at 0x005C70D0 (no exe body): seen, not worked.
- `.worktrees\RO-AUDIOINIT-45D460-REACH-5` is not removed: its branch is not merged.

## 8. Files

Committed: this RESULT, `re/match/s28/005c78c0.c`, `re/tools/image/CODEPTR_EXTRA.tsv`, the codeptr_image.py change,
`cmp_head6.py`, `cmp_head6.txt`, `arms_driver6.py`, `dword_at.py`, and the tool-generated outputs.

Raw (`D:/MashedWT/RO-AUDIOINIT-45D460-REACH-6_data/`, full sha256 in `sha256.txt` there), first 16 hex:

| file | sha256 |
|---|---|
| build6a_vs.log / build6b_vs.log / build6b_ddk.log | cbc5f8c2afa0024f / 6ef706c25c06799d / 6e971be53597ba9e |
| BYTECHECK6.tsv | 1e8fb386385b4e6a |
| bind_selftest1.txt | c5a050039dcca5d4 |
| arms_knob.out / roarm.out | ecb666455ae4ddfd / 32ca252249ce81be |
| vs_init_audio.log / ddk_init_audio.log | d3aa7b101691353c / 460b0dad18042a02 |
| fd_ref1_pr1.json / fd_ref1_pr1.md | 4377a0d5fb38d954 / 21c4ea33c752e45b |
| ro/RO_ROAUDINIT-pr1/meta.json / ticks.bin / ckpt00.data.bin | 3525ad278f564c88 / 9b466500bf2b1297 / df852c44c8d3deb0 |
| ro/RO_ROAUDINIT-pr1.log | ac64f46f24efb688 |
| ref1 ckpt201.data.bin (D:/MashedWT/RO-REF-NAME_data/ref1) | fef65df0d21a4f82 |


## Rebase (run 57, L0)

**Stopped at step 2(b): BLOCKED, waits for BIND-SYNC-REMOVE-STALE.** Two of REACH-6's hand edits can only be made
by hand. No builds, ddkobj check or arms ran on the rebased tree. G2 (section Verdict) was measured on base c880ab12e
and has not been re-measured on 287c10369.

Worker on claude3, Opus 5.5 medium (moved from claude2 at pickup).

### 1. Rebase

- I waited while `mashed_lock.py status` showed merge-train PRIORITY-HOLD (`merge-train:20261127_162413`). It cleared
  at 16:41:54 and the head was then **orch/integration 287c10369** (bddefb0f6 + HB-LOCK-BARE-LAUNCH-SURVEY).
- `git -c merge.autoStash=false rebase orch/integration`, 7 commits. Only `verify/porttop_20261108/BINDINGS.tsv`
  conflicted: in 3c2d49f4d (REACH-5 bind) and 345038cad (REACH-6 bind 0x005c78c0). I took the head's version both times
  (RULE 3, no hand merge). The second rebase, from bddefb0f6 onto 287c10369, had no conflicts.
- Regenerated with the tools on the rebased tree:
  - `porttop_bind.py bindings --profile audio`: updated 6, added 6.
  - `codeptr_image.py --write`: CODEPTR_IMAGE.tsv and PortTopCodePtrImage.inc, 117 CP_IMG_FIX, 52 extern syms.
  - `sync --profile audio` would change nothing (rsp + 0, allowlist + 0).
- **Owner of 0x005c6b40 and 0x005c75b0: profile `bulk`** (rows tagged RACE-BIND-C4-BULK, level 1). Why:
  - Neither row existed at REACH-6's base c880ab12e.
  - The bulk rows reached orch/integration first (2382008d2, from 6bfaea322 BIND-BINDINGS-BULK-DRIFT). REACH-6's
    `PORT-TOP-AUDIOTICK-STOPS` rows were appended later, by its own unmerged `bindings --profile audio` in 37aca6283.
  - `cmd_bindings` updates only `missing` rows. A matched row another profile wrote keeps its text
    (porttop_bind.py:1375, "only a missing row is updated; a row another row wrote keeps its text"). So the profile
    whose row came first owns it. That is the same rule BULK-DRIFT applies to stops through `stop_sym_foreign`
    (porttop_bind.py:1372-1374). `stop_sym_foreign` itself only covers `missing` rows, and these two are `matched`.
  - Both profiles name the same exe symbols (`PortTop_Bound_005c6b40`, `AudioVoiceField8cGet`). Only the note tag and
    the level differ (bulk 1, audio 3). The link is the same either way.
- Image slots after regeneration: 117 bound. REACH-6's own number was 113.
  - The 4 new ones are RW core targets that the head's BINDINGS.tsv now names: 0x00618230 -> RwCore_004d7480,
    0x00618240 -> RwCore_004dba60, 0x00623e0c -> RwCore_00551cf0, 0x00623e10 -> RwCore_00551fd0. They are not this
    row's RVAs. No build has checked them.
  - **Ownership:** the 4 slots 0x00635184 / 0x00635188 / 0x0063518c / 0x00635190 are owned by row
    RO-AUDIOINIT-45D460-REACH-7 (filed). This row owns only 0x006350cc and 0x00636024.

### 2. Checks on the rebased tree

(a) Dry counts.

| profile | sync --dry | bindings --dry |
|---|---|---|
| audio | rsp + 0, allowlist + 0 | updated 0 added 0 (after regeneration, with the line from (b) restored) |
| bulk | rsp + 0, allowlist + 0 | updated 0 added 0 |
| gates | rsp + 0 | updated 2 added 9 (same as the head) |
| raceframe | rsp + 0 | updated 4 added 14 (same as the head) |
| raceload | rsp + 0 | updated 14 added 282 (same as the head) |
| renderframe | rsp + 0 | updated 0 added 0 (same as the head) |

(b) REACH-6's two hand edits. **The tool produces neither of them.**

1. **The removal of `"Match\PortTop_004ccef0.cpp"` from `mashedmod/exe_sources.rsp`.**
   - On the rebased tree the line is absent and the file `Match/PortTop_004ccef0.cpp` does not exist.
   - `sync --profile audio --dry` prints `rsp + 0`, so the tool does not add the line back.
   - But `sync` only adds lines, so the removal itself can only be made by hand.
2. **The BINDINGS.tsv row 0x004ccef0.**
   - The tool writes:
     `0x004ccef0	FUN_004ccef0	matched	RwCoreHand_004ccef0	PORT-TOP-AUDIOTICK-STOPS: existing exe body RwCore/RwCore_Hand.cpp	3	`
   - That row says kind `matched` for a hand body that is L-126 provisional (PROVISIONAL.tsv `RwCoreHand_004ccef0`).
     The reason is that the `reuse` branch of `cmd_bindings` always writes `matched`.
   - REACH-6's line, which can only be written by hand:
     `0x004ccef0	FUN_004ccef0	near-match	RwCoreHand_004ccef0	RO-AUDIOINIT-45D460-REACH-6: existing exe body RwCore/RwCore_Hand.cpp (hand body, L-126 provisional, re/match/PROVISIONAL.tsv RwCoreHand_004ccef0; NOT matched, knob path only; one body for the rva)	3	`
   - I kept the hand line in the committed BINDINGS.tsv so the row does not claim `matched`. With it in place the tool
     accepts the tree: `bindings --dry` 0/0, and the selftest says "BINDINGS.tsv from the audio plan: IDENTICAL".

Per the L0 task: the row waits for BIND-SYNC-REMOVE-STALE.

(c) `git show HEAD:re/match/PROVISIONAL.tsv | cut -f1 | sort | uniq -d` prints nothing. PROVISIONAL.tsv has 994
non-empty lines.

(d) equiv verdict for 0x005c78c0.
- I re-ran `py -3.12 re/tools/match/equiv.py re/match/s28/005c78c0.c` on the rebased tree.
- `verify/roaudinit_20261127/equiv_005c78c0_rebase.txt:1` = `0x005c78c0	EQUIV	[ORDER]`.
- The last commit touching `re/tools/match/equiv.py` in the tree is 1ed7ceb1a.
- `git merge-base --is-ancestor 1ed7ceb1a orch/integration` returns 0 (yes).
- ADMITTED.tsv:63 records the same verdict.

### 3. Other checks

| check | result |
|---|---|
| binder selftest on the rebased tree (`verify/roaudinit_20261127/bind_selftest_rebase57.txt`) | **PASS**, every profile. audio plan IDENTICAL (bind 459 reuse 87 stop 73). BINDINGS from the audio plan and the bulk plan IDENTICAL (0/0). |
| VS2022 build, object build + ddkobj check, RO_ROAUDINIT-pr1 arm, 0x00604604 record-0 compare | **not run**: the task says to stop at a hand-only line (2b) |
| Frida temp folders | 0 before, 0 after (no harness run), deleted 0 |

### 4. Left undone

- Step 3 in full: VS2022 build, object build and ddkobj classification against the head's drain, the RO arm, and the
  G2 compare. All of it is to be run once BIND-SYNC-REMOVE-STALE gives the tool a way to remove the stale rsp line and
  to write the 0x004ccef0 row as `near-match`.
- The 4 newly bound RW core image slots (above) need the VS2022 link to confirm the symbols resolve.
