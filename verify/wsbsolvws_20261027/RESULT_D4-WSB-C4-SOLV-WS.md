# RESULT D4-WSB-C4-SOLV-WS: SOLV write-set C4 (8 rows)

Verdict: **GREEN.** 5 rows go **C3 -> C4** through re-classify (`0x00568dd0`, `0x00568fd0`, `0x00564c80`,
`0x00561390`, `0x00563e70`). 3 rows stay C3 under the PREREG rule (`0x00568990`, `0x0056b7a0`: heap leg compared 0
pages, the sampled calls write nothing; `0x0056ba30`: racediff HOLD, diagnosed as a label artifact, section 3).
Default gates unchanged on this branch's exe (section 5). No shared header changed: `Core/ShadowTrack.h` and
`Core/ShadowAB.h` are untouched.

Branch `orch/D4-WSB-C4-SOLV-WS` off `orch/integration` @ `6368c086a`. Harness change `eb72bc0b6` (Run -> RunTracked for
the 5 SOLV wrappers, `#include "../Core/ShadowTrack.h"` added to `RwpSolverCore18.cpp` / `RwpSolverCore21.cpp`).
PREREG `423156174` (committed alone), addendum 1 `658ec1451`, addendum 2 `f40534a6a`, addendum 3 (`0x00563e70` census
window), each committed alone before the arms it names.

## Per-row table (the deliverable)

Leg H = heap-tracked write-set shadow: `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, census scenario
(`--track 0 --mode 10 --cars 4`), hook installed alone, `PATCHBYTE ... installed=e9 A/B-IS-REAL` on every row that
sampled. Every counted sample `stack=0 noise=0 ovf=0 ret=0`, no NOISY / UNTRACKED / OTHER-EXC line for any row.
Leg R = racediff tool v7, hook installed alone on ON (`0xE9` at the RVA), `--hooks __none__` on OFF.

| row | leg H n / ndiff | pages o/n per sample | heap pages o/n per sample | **heap pages compared (total)** | leg H read-out | leg R | level |
|---|---|---|---|---|---|---|---|
| `0x00568dd0` | 24 / 0 | 1..1 / 1..1 | 1..1 / 1..1 | **24** | HEAP-CLEAN | QuickBattle C4-CLEAN 6/6 | **C3 -> C4** |
| `0x00568fd0` | 24 / 0, then N: 3 / 0 | 0 (24 samples), N: 1..2 | 0, N: 1..2 / 1..2 | **0, N: 4** | CLEAN-NO-PAGES, N: HEAP-CLEAN | QuickBattle C4-CLEAN 6/6 | **C3 -> C4** |
| `0x00564c80` | 24 / 0 | 6..8 / 6..8 | 5..7 / 5..7 | **129** (+24 image) | HEAP-CLEAN | QuickBattle C4-CLEAN 6/6, 4 sigs | **C3 -> C4** |
| `0x00561390` | 24 / 0 | 12..12 / 12..12 | 11..11 / 11..11 | **264** (+24 image) | HEAP-CLEAN | QuickBattle C4-CLEAN 6/6 | **C3 -> C4** |
| `0x00563e70` | 24 / 0 | 2..3 / 2..3 | 2..3 / 2..3 | **60** | HEAP-CLEAN | census (A4 1..3000) C4-CLEAN 6/6 | **C3 -> C4** |
| `0x0056ba30` | 24 / 0 | 1..1 / 1..1 | 1..1 / 1..1 | **24** | HEAP-CLEAN | QuickBattle **HOLD BEHAVIOR-DIFF** (x2) | stays C3 |
| `0x00568990` | 24 / 0, then N: 0 | 0 | 0 | **0** | NO-WRITES-SAMPLED | QuickBattle C4-CLEAN 6/6 | stays C3 |
| `0x0056b7a0` | 24 / 0, then N: 0 | 0 | 0 | **0** | NO-WRITES-SAMPLED | QuickBattle C4-CLEAN 6/6 | stays C3 |

"N" = arm N (addendum 1): leg H plus `MASHED_SHADOW_NEED_PAGES=1` and a 150 s hold. A call where neither run wrote a
tracked page and stack and return agree is discarded and logged as `[..] EMPTY rv=`.
Per-sample counts from `heap_pages.py` (`heap_pages.tsv`, `heap_pages_needpages.tsv`).

## 1. Leg H: heap-tracked write-set

8 boots, 24 samples each, every boot ended by the driver's SAMPLES-DONE guard (no hang, no NO_LOG, no crash).

- `0x00564c80`: **CLEAN with heap pages restored.** BVH-FU's image-only leg was DIVERGENT 24/24 with port - orig = +1
  on `0x007dc8c8` and `0x007dc8cc` (image page `0x007dc000`). With heap tracking on, 24/24 CLEAN, 1 image page and
  5..7 heap pages compared per sample. The recipe differs from BVH-FU's leg V only by heap tracking (and this build,
  whose only source change is the 5 SOLV wrappers), and the +1 / +1 is gone. That fits the restore gap BVH-FU named
  (heap writes of the original's run not restored before the port ran), and it resolves BVH-FU's `[UNCERTAIN]` on this
  row. The sample lines do not name the compared image page, so I do not claim it was `0x007dc000`.
- `0x00568990`: every sampled call returns 0. The body returns 0 before any store when `param_3 == 0 || param_5 == 0`
  (`RwpSolverCore21.cpp:205-207`). Arm N: 0 counted samples, 1000..1249 EMPTY discards over the 150 s hold, every
  logged one `rv=00000000`. **NO-WRITES-SAMPLED**: in the census scenario this pair pump gets no pairs.
- `0x0056b7a0`: every sampled call returns 0. Arm N: 0 counted samples, 1500..1749 EMPTY discards, every logged one
  `rv=00000000`. **NO-WRITES-SAMPLED.**
- `0x00568fd0`: its return is always `param_1` (`0f58d5e0` in leg H), so `rv` does not separate empty calls. Arm N:
  3 counted samples, all OK, 2 / 1 / 1 heap pages, 1000..1249 EMPTY discards. HEAP-CLEAN under addendum 1.

## 2. Leg R: racediff

Quick Battle window (A4 calls 300..1200) for 7 rows. 6 C4-CLEAN, 6/6 calls each side. `0x0056ba30` HOLD (section 3).
`0x00563e70` census: the default window caught 0 calls on both sides twice (both boots `armed: asi=True`, frame 1208).
BVH-FU's census leg for that row used A4 calls 1..3000, and I had not carried the window over. Addendum 3 recorded the
slip, then the re-run with `C4_WINDOW="1 3000"` gave C4-CLEAN 6/6.
The `c4_racediff_samples.json` of `0x00561390`, `0x00564c80` and `0x00563e70` (w3000) hash identical to BVH-FU's
(`4d461d2e...`, `5174ddd9...`, `1c9dc5f0...`): the normalized capture repeats on this build.

## 3. `0x0056ba30`: racediff HOLD is a label artifact (arm D, addendum 2)

v7 OFF `[c56b7a0,A0+0,h0,1,h1] [c568990,h2,h3,0,h4,258] eax=0`, ON `[c56b7a0,A0+0,h0,1,A0+118] [c568990,h1,h2,0,h3,258]
eax=0`. Only the 4th argument of the call to `0x0056b7a0` differs. Both sides compute it from the same expression:
original `0x0056ba68..0x0056ba78` (`[[[esi+0x70]]]+0xc018`, `push eax`), port asi `0x10099d16..0x10099d1d`, and the
other three pushes match too.

Arm D (tool v8 = v7 + raw values), 6 calls each side, raw entry args and raw 4th argument:

| side | `param_1` | 4th arg | 4th - `param_1` | label |
|---|---|---|---|---|
| OFF | `0x0fa376c8` | `0x0fa37538` | -0x190 | `h1` |
| ON | `0x0f1a7420` | `0x0f1a7498` | +0x78 | `A0+78` |
| ON, first v7 run | | | +0x118 | `A0+118` |

The 4th argument is a heap object of its own. Its offset from `param_1` changes from boot to boot (-0x190, +0x78,
+0x118). The v7 `nrm` labels a value `A<k>+off` only for `0 <= v - arg_k < 0x4000`, else as a heap ordinal. So the
label changes with placement. Every other argument agrees by label, and their raw values keep the same low 16 bits
across the two boots (`h0` `..6e50`, `h2` `..b238`, `h3` `..aa40`, `h4` `..8000`). Read-out: **LAYOUT**.
Per addendum 2, the row still **stays C3** in this step, because leg R did not give C4-CLEAN. Tool finding for the next
step: v7 labels a pointer arg-relative by placement. Fix the label (for example arg-relative only when the value is
derived from the arg, or a ±window plus OFF/ON agreement), then re-run this row's racediff. Its leg H is already
HEAP-CLEAN (24 heap pages).

## 4. Rows that stay C3, and why

| row | reason | what would unblock it |
|---|---|---|
| `0x00568990` | heap leg compared 0 pages, every sampled call returns 0 before any store | a scenario where it receives pairs (`param_3 > 0`) |
| `0x0056b7a0` | heap leg compared 0 pages, every sampled call returns 0 and writes nothing | a scenario where the call writes |
| `0x0056ba30` | racediff HOLD, diagnosed LAYOUT (section 3) | a racediff label fix, then one re-run |

No new `[UNCERTAIN]` marker is filed: the open items are scenario coverage and a tool label, both stated above.

## 5. Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-WSB-C4-SOLV-WS_root`,
`A4_OUT=D:/MashedWT/D4-WSB-C4-SOLV-WS_data/gates`, test slot + `mashed_lock.py run --label D4-WSB-C4-SOLV-WS`, exe
`f0cbcec5`. The first attempt failed after H_DEF ran (pid 4800, self-exited): the runner could not copy its output,
because `A4_OUT` did not exist. The folder was created and all four arms re-ran.

All four `exit=0 self_exited=True` (H_DEF pid 12796, 258 s; B_DEF_K0 18004; K1 6284; K26 5472).

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## Tracker state (re-classify, `apply_reclass.py`)

| row | before | after | C4 evidence |
|---|---|---|---|
| `0x00568dd0` | C3 impl | **C4 verified** | leg H 24 heap pages + racediff QuickBattle C4-CLEAN |
| `0x00568fd0` | C3 impl | **C4 verified** | leg H (arm N) 4 heap pages + racediff QuickBattle C4-CLEAN |
| `0x00564c80` | C3 impl | **C4 verified** | leg H 129 heap pages + racediff QuickBattle C4-CLEAN |
| `0x00561390` | C3 impl | **C4 verified** | leg H 264 heap pages + racediff QuickBattle C4-CLEAN |
| `0x00563e70` | C3 impl | **C4 verified** | leg H 60 heap pages + racediff census C4-CLEAN |
| `0x00568990`, `0x0056b7a0`, `0x0056ba30` | C3 impl | C3 impl (note added) | section 4 |

C4 gates checked per row: no `STUB` in any of the 5 bodies (S-3729 / S-3745 list `0x00561390` / `0x00564c80` as
passthrough callees of `0x0047e9c0` / `0x00559c40` / `0x0055ae70`, not stubs in these bodies). Every `[UNCERTAIN]`
in their notes is filed already (U-9092, U-9093, U-9097, U-9114, U-9118, name / intent labels only). No new marker.
`scenario` column on the 5 rows: `c4_racediff_bvh:<QuickBattle-f300-1200 | census-a1-3000>+shadow_tracked_heap:track0-mode10-cars4`,
`frida_diff` = `verify/wsbsolvws_20261027/racediff_summary.tsv`. CHANGELOG: 8 lines below `<!-- ENTRIES -->`.

## Files

Committed (`verify/wsbsolvws_20261027/`): `PREREG_D4-WSB-C4-SOLV-WS.md`, `PREREG_ADDENDUM_1.md`, `PREREG_ADDENDUM_2.md`,
`PREREG_ADDENDUM_3.md`, `run_shadow4.py` (copy of `run_shadow3.py`, data path, label, `HOLD` env), `run_bvh4.py` (copy
of `run_bvh3.py`, data path, label, `C4_TOOL` / `C4_TAG` env), `c4_racediff_v7.py` (unchanged copy),
`c4_racediff_v8_raw.py` (diagnostic), `heap_pages.py`, `heap_pages.tsv`, `heap_pages_needpages.tsv`,
`shadow_summary.tsv`, `racediff_summary.tsv`, `apply_reclass.py`, this file. Source: 3 wrapper files (`eb72bc0b6`).
Trackers: `hooks.csv` (8 rows), `re/analysis/CHANGELOG.md` (8 lines).

Raw (not committed), `D:/MashedWT/D4-WSB-C4-SOLV-WS_data/`:

| file | SHA-256 |
|---|---|
| `00568dd0_heap/shadow_ab.log` | `5fff6097d7c1f1bf079b899614e4479c994cd16debf4010fe4cfca0b0e7832c0` |
| `00568fd0_heap/shadow_ab.log` | `4cbfeb9b84b5af5a14705e838ae731d1d108935fc5e8dbd8e335ef4a26a82aec` |
| `00568fd0_needpages/shadow_ab.log` | `b9dfb1835a77ba558f42ffa0dc543776dc330b74161a641a1b33f68709b195ad` |
| `00564c80_heap/shadow_ab.log` | `ce38c096735f13df0230b6d8b87aac66554c475e41dbc16e63cf035ba888ef52` |
| `00561390_heap/shadow_ab.log` | `c561bc127f0e37a780fc29ca981db8a6fdc6f7fcb45c2bf3a22b4c160cbfa00d` |
| `00563e70_heap/shadow_ab.log` | `0c616736f815524d2cd94b2ea25c3de2d045cab83b1a3ca1aae2878987d1312c` |
| `0056ba30_heap/shadow_ab.log` | `d91af588e0ee9f13cbe7ec2155a02f0aed9a24552f28cf30a3a09bceaca35180` |
| `00568990_heap/shadow_ab.log` | `9b9c1c672f624a5019c0569d8fa88e9329325e6e29cc5dc676c583dde117e928` |
| `00568990_needpages/shadow_ab.log` | `a98aff5b4f7ec8931a0932a939d0b279628c581479b760a7db5f8d7db54bfeac` |
| `0056b7a0_heap/shadow_ab.log` | `7c0254d0bd19050c1b9f19223d53ae1b1bfc1ad5952c62933d5a491a00593d09` |
| `0056b7a0_needpages/shadow_ab.log` | `4f57cd5742427de4f25f3d46bf49da4461a29fddbb22dccb4939140179ca2e03` |
| `00568dd0/c4_racediff_samples.json` | `e3a9f8d5a5ebb0bc3584e18f4c14b6ca9b769d8435651a3824cb2430df45111c` |
| `00568fd0/c4_racediff_samples.json` | `43d2d3c76f48dbe65867e196a54cd490445f9b390ada3f87738fab13e2cddbbf` |
| `00564c80/c4_racediff_samples.json` | `5174ddd9634aa7bbf6642c203ee7a99f100312bdaa4cc2debaddf8ff8ccfedb7` |
| `00561390/c4_racediff_samples.json` | `4d461d2e6a3dc6336dbfbded8091299f216a17276355645c4803ab95df9d3086` |
| `00563e70_census/c4_racediff_samples.json` (0 calls) | `3968b8b8d5257bbb2c9c5e8ea42e559746d0513df7123f0e1d3c214ebd50c9a8` |
| `00563e70_census_w3000/c4_racediff_samples.json` | `1c9dc5f0dda05218acda98998ae13973fe35bf1321a0760aac6b6e4f3cf3cd94` |
| `00568990/c4_racediff_samples.json` | `7f2239b1fc55490dd487b12c3cbb088f82aea3016f212e21ada19a3f4eacc5d1` |
| `0056b7a0/c4_racediff_samples.json` | `9cb59450f064bba7731575cf50315f1c8e978e6e4ff032d0c2821ce46647adfe` |
| `0056ba30/c4_racediff_samples.json` | `3e5632b1ee338b9828d2696847476efc0e24ad618ae8e2423dd1de3f11008bb2` |
| `0056ba30_raw/c4_racediff_samples.json` (arm D) | `c2314801683155bd1184d1a9833180d7983c979ab065fe73f186e4ff912b4f15` |
| `legH_batch1.log` / `legR_armN_batch2_encodingcrash.log` / `legR_armN_batch3.log` | `4a3829bb...9274` / `d2e4102b...08f6` / `56aa0b93...333e` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/RUNLOG.txt` | `07cec486c33fe9584e191904da888e6fecbcb822392f837c9a1a8eefb94e180c` |

The first racediff chain (`legR_armN_batch2_encodingcrash.log`) died before any game started: the driver hit a cp1252
`UnicodeEncodeError` printing the lock's wait line through `tee`. I killed that chain's own lock waiters (PIDs 20100,
18668, 15848, test-slot tree 16872), all labelled `D4-WSB-C4-SOLV-WS`, none holding the lock or a game, and re-ran it
with `PYTHONIOENCODING=utf-8`.

Build (`MASHED_NO_DEPLOY=1`): `mashed_re.exe` `f0cbcec50a020919c270da0699abf8ea737f4d533a03b20d6b5e006f188ac750`,
`mashed_re_dev.asi` `5c12899c1f32f8a49cbba8bc28faea69c15bd56d33f4d4b07877b877b4fb931e` (private root only, hash matched).
`original/mashed_re_dev.asi` not touched. No source change after this build.
Game PIDs, all spawned by this step: leg H 3504, 17876, 19988, 18144, 3996, 8984, 19984, 6740 (SAMPLES-DONE guard);
arm N 9312, 18624, 9392; racediff ON/OFF 1736/16484, 6364/3856, 6112/732, 5772/20276, 3556/14208, 19392/5224,
2992/4228, 1480/16448, 19996/19564, 6084/4924; gates 4800 (first attempt, self-exited), 12796, 18004, 6284, 5472 (self-exited). No process of this step is running.

## Next

- `0x0056ba30`: fix the racediff arg-relative label (section 3), re-run its racediff. Leg H is already HEAP-CLEAN.
- `0x00568990`, `0x0056b7a0`: heap leg in a scenario where they write (car contacts producing pairs), same as
  `0x00564040` from D4-WSB-SHADOW-PRIVATE.
