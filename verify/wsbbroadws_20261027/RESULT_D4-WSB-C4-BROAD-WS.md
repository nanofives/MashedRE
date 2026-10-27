# RESULT D4-WSB-C4-BROAD-WS: write-set C4 for 5 BROAD rows (round 2)

Verdict: **GREEN.** **3 rows go C3 -> C4** (`0x0055a1f0`, `0x0055ab30`, `0x0055c0f0`), each with a heap-tracked write-set
leg that compared more than 0 heap pages and came back CLEAN, plus a canonical-scenario racediff C4-CLEAN with the hook
installed alone. **2 rows stay C3** (`0x0055bae0`, `0x0055c000`): their heap leg compared 0 pages, because every write
they make lands in the caller's stack window. Default gates unchanged on this round's exe.

| row | heap leg: heap pages compared (total over 24, per sample) | racediff (tool v10) | level |
|---|---|---|---|
| `0x0055a1f0` | **104** (4..12), HEAP-CLEAN 24/24 | Quick Battle **C4-CLEAN** 6/6, 1 sig | **C3 -> C4** |
| `0x0055ab30` | **150** (6..8), HEAP-CLEAN 24/24 | Quick Battle **C4-CLEAN** 6/6, 4 sigs | **C3 -> C4** |
| `0x0055c0f0` | **24** (1), HEAP-CLEAN 24/24 | census `C4_HOLD=400` **C4-CLEAN** 6/6, 1 sig | **C3 -> C4** |
| `0x0055bae0` | **0**, CLEAN 24/24, NEED_PAGES arm: NO-WRITES-SAMPLED | census `C4_HOLD=400` C4-CLEAN 6/6, 1 sig | stays C3 |
| `0x0055c000` | **0**, CLEAN 24/24, NEED_PAGES arm: NO-WRITES-SAMPLED | census `C4_HOLD=400` not observed (2 attempts) | stays C3 |

Round 1 (`69c14a09d`, RED) is in git history. Its raw data stays in `D:/MashedWT/D4-WSB-C4-BROAD-WS_data/` (top level).
Round 2: branch `orch/D4-WSB-C4-BROAD-WS`, `orch/integration` @ `6368c086a` (SHADOW-PRIVATE) merged in at `430e7feee`
before any arm. Tools `28737b7e1`. **PREREG `PREREG_R2_D4-WSB-C4-BROAD-WS.md` at `5548a2577`, committed alone before any
round-2 arm**, applied as written, no addendum. Arm summaries `ff0821d2b`. Then `orch/integration` @ `8294a5633`
(BROAD-A, SOLV-WS, HB-ARCTIC-OPENSEA merged) was merged in at `f1ebf8581`, before the tracker transaction (see "Build").

Harness: unchanged since round 1 (`879e1ad65`: RunTracked wrappers for `0x0055a1f0`, `0x0055ab30`, `0x0055bae0`, new
RunTracked wrapper for `0x0055c000`). **No change to `Core/ShadowTrack.h` or `Core/ShadowAB.h` on this branch.** The
0056xxxx solver rows (SOLV-WS) were not touched.

## Build

Arms and gates ran on `mashed_re.exe` `ab557985946893a7069dc8d114880a4ed49e7b2d730cceaf338d97af47c19399`,
`mashed_re_dev.asi` `589a353bc3d8f054ea794e85a1bbaef9b9745701721efbfeb7466f558bd956fc` (copied to the private root
only, hash checked after copy), `.map` `2c4d3cb7883f6b424f7c60e7bf1915d84f84a11c1277d3c9dccf526cf9b82024`, built at
`430e7feee` with `MASHED_NO_DEPLOY=1`. `original/mashed_re_dev.asi` not touched. Private root
`D:/MashedWT/D4-WSB-C4-BROAD-WS_root` (its `MASHED.exe` = `original/MASHED.exe`, `b9977dab...fe87`).

The second integration merge (`f1ebf8581`) brings source from other steps only: `RwpSolverCore11/18/21.cpp` (SOLV-WS),
`TrackRenderer.*`, `RwRaceSubmit.*`, `RwSceneBuild.*` (HB-ARCTIC-OPENSEA), each gated by its own merge train. It touches
none of the three files that hold these rows (`RwpSolverBroadphase3.cpp`, `RwpSolverGlue5.cpp`, `RwpIntegrator.cpp`).
The merged tree's exe was **not** rebuilt or re-gated here. The merge train's gate run covers it.

## Leg 1: default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT` = the private root, under the test
slot and `mashed_lock.py run --label D4-WSB-C4-BROAD-WS`. All four `exit=0 self_exited=True` (game PIDs 9612, 17984,
3024, 6888).

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## Leg 2: heap-tracked write set (`run_shadow3.py`)

`MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, census (`--track 0 --mode 10 --cars 4 --hold 60
--hooks 0x<rva>`), one hook per boot, 24 samples. Summary `shadow_heap_summary.tsv`. Every row: `PATCHBYTE installed=e9
... A/B-IS-REAL`, ndiff 0, every sample `stack=0 noise=0 ovf=0 ret=0`, no NOISY / UNTRACKED / OTHER-EXC line.

| row | n | ndiff | pages o (min..max) | heap pages o / n per sample | heap pages compared (total) | rv | read-out |
|---|---|---|---|---|---|---|---|
| `0x0055a1f0` | 24 | 0 | 4..14 | 4..12 / 4..12 | **104** | `0x0f512d50` x24 | **HEAP-CLEAN** |
| `0x0055ab30` | 24 | 0 | 7..9 | 6..8 / 6..8 | **150** | `0x10012d50` x24 | **HEAP-CLEAN** |
| `0x0055c0f0` | 24 | 0 | 1..1 | 1..1 / 1..1 | **24** | void | **HEAP-CLEAN** |
| `0x0055bae0` | 24 | 0 | 0..0 | 0 / 0 | 0 | `0x001afc2c` x24 | CLEAN-NO-HEAP |
| `0x0055c000` | 24 | 0 | 0..0 | 0 / 0 | 0 | `0x001af6fc..0x001af810` | CLEAN-NO-HEAP |

`0x0055a1f0` and `0x0055ab30` also compared 1-2 `MASHED.exe` `.data` pages per sample (pages minus heap), CLEAN.
**Round 1's `0x0055a1f0` divergence at `0x00624048` (sample 0 only, image-only mode) did not recur** in these 24 STW samples,
including sample 0. Per the PREREG the row passes this leg (heap pages compared > 0). [UNCERTAIN] what wrote
`0x00624048` in round 1's first call. Round 1 ran without STW, so another thread is possible, and this round cannot
recover it. The PREREG does not make it block (no U-n filed: the divergence did not reproduce, so there is no hole to
resolve on this row).

**NEED_PAGES arms (`MASHED_SHADOW_NEED_PAGES=1`, `SHADOW_TAG=need`), the two CLEAN-NO-HEAP rows.**

| row | counted samples | discards | every logged rv | read-out |
|---|---|---|---|---|
| `0x0055c000` | 0 | 256 logged (256..499 total) | caller-stack addresses `0x001af78c..0x001af7f8` | NO-WRITES-SAMPLED |
| `0x0055bae0` | 0 | 256 logged (256..499 total) | `0x001afc2c` (caller stack) x256 | NO-WRITES-SAMPLED |

The `0x0055bae0` NEED arm's first attempt failed to attach (`error: could not attach`, PID 18332, no game ran). It was re-run
once per the PREREG (PID 18864), and that run is the one in the table.

Every return of both functions is an address in the main thread's stack (`0x0055c000` returns a pointer, its last
parameter is `void* out`, `RwpIntegrator.cpp:399`; `0x0055bae0`'s return equals the stack pointer it passes to
`0x00563000`, `S+c4` in its racediff signature). ShadowTrack does not page-track the current thread's stack. It compares the 4 KB
above the wrapper's return address (`stack=0`, CLEAN in all 48 leg-2 samples of the two rows). So the whole write set of
these two rows in this scenario is in that window, and no scenario will give them heap pages unless a caller passes a
heap buffer. Under the rule ("more than 0 pages compared") they stay C3. Whether the stack-window compare can serve as
the write-set leg for a row whose output is on the caller's stack is a rule question for the lead (see "Next").

## Leg 3: canonical-scenario racediff (`c4_racediff_v10.py`, `run_bvh3.py`, `leg3_r2.sh`)

Tool copied unchanged from `orch/D4-WSB-C4-BROAD-A` @ `020d70c14` (v7 + opt-in v8/v9/v10 options). `C4_DEREF=1` on every arm
(field-source tokens, BROAD-A's X1 lesson: `A<k>+<d>` heap distances differ between two boots of the original). Summary
`racediff_r2_summary.tsv`. `installed=True` (ON byte `0xE9`) on every ON boot. No arm needed the control/repeat step.

| row | scenario | `C4_ARGW` | verdict | calls off/on | sigs | PIDs off/on |
|---|---|---|---|---|---|---|
| `0x0055a1f0` | Quick Battle, A4 300-1200 | `564040:1:ffff` | **C4-CLEAN** | 6/6 | 1/1 | 18716 / 19116 |
| `0x0055ab30` | Quick Battle, A4 300-1200 | `565260:1:ffff;564c80:1:ffff` | **C4-CLEAN** | 6/6 | 4/4 | 18856 / 20408 |
| `0x0055bae0` | census, `C4_HOLD=400`, A4 300-30000 | none | **C4-CLEAN** | 6/6 | 1/1 | 19888 / 9344 |
| `0x0055c000` | census, `C4_HOLD=400`, A4 300-30000 | none | not observed, 2 attempts | 0/0 | 0/0 | 19912, 18736 / 19740, 7184 |
| `0x0055c0f0` | census, `C4_HOLD=400`, A4 300-30000 | none | **C4-CLEAN** | 6/6 | 1/1 | 6736 / 16668 |

Signatures (identical on both sides):

- `0x0055a1f0` x6: `[c564040,A0@54,18,h0,A0@70,0] [c564040,A0@54,19,h0,A0@70,0] [c564040,A0@54,1a,h0,A0@70,0]
  [c564040,A0@54,1b,h0,A0@70,0] eax=A0+0`. Round 1's diff (arg 1 of the first call, a pointer-range dword on OFF and
  `0x18` on ON) is gone under the declared 16-bit mask: both sides read `18`.
- `0x0055ab30`: `[c565260,A0@0,<k>] [c564c80,A0@0,<k>,S+1c] eax=A1@24` for `<k>` = `18` x2, `19` x2, `1a` x1, `1b` x1.
  Round 1's diff (arg 1 = `Sloc` on OFF) is gone under the two masks.
- `0x0055bae0` x6: `[c563000,A0+0,S+c4] eax=S+c4`.
- `0x0055c0f0` x6: `eax=-`. The body makes no call-out and returns void, so this racediff shows only that the hooked body
  ran 6 times in the window with no call-out on either side. Its output (the transformed point set, 1 heap page per
  sample) is compared by leg 2.

**`0x00565260` `param_2` width (the open item from round 1), `argwidth_00565260.txt`.** On the anchored original the slot is
read once, `mov edi,[esp+0x20]` at `0x0056526b`, and rewritten at `0x005652a1` before any other read of the slot. `edi`
is then read only as `mov ebx,edi` / `and ebx,0xffff` (`0x00565275`/`0x00565277`) and by the 16-bit `cmp bp,di` /
`cmp bx,di` at `0x005652ed`, `0x00565322`, `0x005653c9`, `0x005653fe`, until it is overwritten on both paths (`lea edi`
at `0x0056532a` and `0x00565406`). The callee between them, `0x00563f60`, pushes `edi` (`0x00563f6c`) before it writes `di`
(`0x00563f7f`). So only bits 0-15 of the incoming `param_2` are read. This is the proof the PREREG names for the mask.

`0x0055c000` not observed: the 60 s census shadow leg sampled it 24 times (sample lines `idx=1068`), and the racediff saw 0
calls on both sides in A4 calls 300-30000 in 2 attempts. [UNCERTAIN] in which part of the race the sampled calls run.
The racediff gate starts at A4 call 300. Missing: the A4 call number of the shadow samples. The row stays C3 on leg 2
anyway.

## Tracker transaction (re-classify)

`apply_reclass.py` (run once from the worktree root after `f1ebf8581`): `hooks.csv` 5 rows (csv round-trip of each line,
every other line byte-identical), 5 CHANGELOG lines under `<!-- ENTRIES -->`. No `UNCERTAINTIES.md` / `STUBS.md` change.

| row | before | after | scenario column | frida_diff |
|---|---|---|---|---|
| `0x0055a1f0` | C3 impl | **C4 verified** | `shadow_tracked_heap:track0-mode10-cars4+c4_racediff:quickbattle-a4-300-1200-argw` | `racediff_r2_summary.tsv` |
| `0x0055ab30` | C3 impl | **C4 verified** | same as above | `racediff_r2_summary.tsv` |
| `0x0055c0f0` | C3 impl | **C4 verified** | `shadow_tracked_heap:track0-mode10-cars4+c4_racediff:census-track0-mode10-cars4-hold400-a4-300-30000` | `racediff_r2_summary.tsv` |
| `0x0055bae0` | C3 impl | C3 impl (dated note) | unchanged | unchanged |
| `0x0055c000` | C3 ported | C3 ported (dated note) | unchanged | unchanged |

C4 gate checks: no `STUB` comment in `RwpSolverBroadphase3.cpp` or `RwpSolverGlue5.cpp`. S-3735 names `0x0055ab30` as a
passthrough **callee** of `0x0047d240`, not a stub in its body. The notes' markers are already filed: `0x0055a1f0`
U-9098 / U-9099, `0x0055ab30` U-9100, `0x0055c0f0` U-9116. No unfiled `[UNCERTAIN]`. `scripts/lint_rva_bodies.py`: OK,
0 new.

## Next

- **Rule question for the lead** (`0x0055bae0`, `0x0055c000`): their write set is the caller's stack only (every return is
  a stack address), so the "more than 0 heap pages" rule cannot be met in any scenario where the caller's buffer is a
  local. Options: (a) keep both C3 (the current state), (b) accept the ShadowTrack 4 KB stack-window compare (CLEAN, 48
  samples) as the write-set leg when every return points into that window, (c) a harness leg that page-tracks the
  caller's stack frame. Recommendation (b) for `0x0055bae0`, whose census racediff is already C4-CLEAN. `0x0055c000` also
  needs a racediff window that contains its calls.
- `0x0055c000`: find the A4 call range of its calls (log the A4 counter in its shadow samples), then a racediff with that
  window.

## Files

Committed (`verify/wsbbroadws_20261027/`): `PREREG_R2_D4-WSB-C4-BROAD-WS.md`, `argwidth_00565260.txt`, `c4_racediff_v10.py`
(copy), `run_bvh3.py` and `run_shadow3.py` (copies, label / paths / summary changed), `leg3_r2.sh`,
`shadow_heap_summary.tsv`, `racediff_r2_summary.tsv`, `apply_reclass.py`, this file. Round-1 files unchanged. Trackers:
`hooks.csv` (5 rows), `re/analysis/CHANGELOG.md` (5 lines).

Raw (not committed), `D:/MashedWT/D4-WSB-C4-BROAD-WS_data/r2/`:

| file | SHA-256 |
|---|---|
| `0055a1f0/shadow_ab.log` | `09d01bce4bc3c5d07f9d849279c27e6e5abc6ffc8ef25150cbdc7b3d6ffbad32` |
| `0055ab30/shadow_ab.log` | `d2f73a203f286d4edc8aefc43c6e78776335a1106cf5e82e4209e4f14bb05c52` |
| `0055c0f0/shadow_ab.log` | `dbe56677dce0242f624afb65591500bc2f07c5eac005f610a45903581e0d8e37` |
| `0055bae0/shadow_ab.log` | `3e54dfd1700813228b8be79f9851b19bbf7427f7d8ce5482ee701ab4318b1cb5` |
| `0055c000/shadow_ab.log` | `1852557377720808883c140f7041ffad39cd669bf4c414b114fbb0891bd62fc5` |
| `0055c000_need/shadow_ab.log` | `6ac4367d016d4026f25ce965acd767d64d763ec214ca36bfde601f865ec24eaa` |
| `0055bae0_need/shadow_ab.log` (re-run) | `e89235f1f21ccece7c8c6d02d79c9ca29b48afb753d34b584a7f2d7a77b023bd` |
| `0055a1f0/c4_racediff_samples.json` | `feede1fc3a3b1c3870fc24040e36d0b19774fc018cc0a26c65514612fa71f4ba` |
| `0055ab30/c4_racediff_samples.json` | `8f1b9fcde86ad52a846c99813bf6bfd1980ab3c625f58729137263079429472a` |
| `0055bae0_census/c4_racediff_samples.json` | `53412cbbe8a5a3a211a5429933f7ab00996b6c3262e89d9f08df59fd74c82a36` |
| `0055c000_census/c4_racediff_samples.json` (empty) | `3968b8b8d5257bbb2c9c5e8ea42e559746d0513df7123f0e1d3c214ebd50c9a8` |
| `0055c0f0_census/c4_racediff_samples.json` | `2b3641c5412fd675c0cdd2054b067b956273803ae237f11d29d77525f4f76c84` |
| `shadow_leg2.log` / `shadow_need.log` / `shadow_need_rerun.log` | `90e3d57a...9d94` / `58143556...270` / `7493e4f6...760` |
| `racediff_leg3.log` | `0803e99147bf270f989f9732e5639d16c818182d3b3b1ff737bc1e92b00cb2b9` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/gates_run.log` | `5e0de31189130ef67828692c82ec84d30c8c93d2acf4785f878107209fc087b2` |
| `../build_r2.log` | `71f78619f28be14d5f0a764c4b053784ebdef0d9daf109bb7da5c6c1b32326e7` |

Game PIDs, all spawned by this round: shadow 8124, 20260, 16160, 15296, 7972, 13396, 18864 (18332 never attached);
racediff 18716, 19116, 18856, 20408, 19888, 9344, 19912, 18736, 19740, 7184, 6736, 16668; gates 9612, 17984, 3024,
6888. Every game was ended by its driver by PID or self-exited. No MASHED process is running. No watcher of this step is left.
