# RESULT D4-WSB-C230-RECHECK: 0x0055c230 C4 recheck (L-40 item 2)

Verdict: **GREEN (rule applied): `0x0055c230` C4 -> C3** through `re-classify`. The racediff leg failed. No window was
found that every original boot reproduces: 2 distinct signature sets over 4 original-only boots at 48 samples, and
again at 200 samples. The heap-tracked write-set leg **passed** (S2 HEAP-CLEAN 24/24, 24 heap pages), so L-38 is met for
this row. Default gates unchanged.

| leg | arm | result |
|---|---|---|
| 1 default gates | first exe `eacbe088`, rebuilt exe `53a97b01` | both match `merge_train.json` |
| 2 heap write set | S1 | DIVERGENT, sample 0 only, 1 port-only dword (harness artifact, addendum 1) |
| 2 heap write set | S2 (warm-up) | **HEAP-CLEAN** 24/24, ndiff 0, 24 heap pages compared |
| 3 racediff window | W1 `C4_SAMPLES=48`, C1 + C2 | 4 original boots, **2 distinct sets**: not 4/4 |
| 3 racediff window | W2 `C4_SAMPLES=200`, C3 + C4 | 4 original boots, **2 distinct sets**: not 4/4 |
| 3 racediff T | hook ON vs original | not run (PREREG: only in a 4/4 window) |

Branch `orch/D4-WSB-C230-RECHECK` off `orch/integration` @ `37383a4bb`. PREREG `PREREG_D4-WSB-C230-RECHECK.md`
(`e9a1f707e`, committed alone before any arm), addendum 1 `PREREG_ADDENDUM_1.md` (`42bafddcd`, committed alone before
S2). Tools and wrapper change `529cf4871`, warm-up `265369c53`.

## Build and method

One source file changed, harness only (`RwpSolverGlue5.cpp`, `0x0055c230` shadow wrapper):
`ShadowAB::Run` -> `ShadowAB::RunTracked(..., SHADOW_STACK_WINDOW(), ...)` (`529cf4871`), and `(void)NarrowDotOrder();`
before it (`265369c53`, addendum 1). Final build at `265369c53` with `MASHED_NO_DEPLOY=1`: `mashed_re.exe`
`53a97b0162ae2bcdd1671d85633509fc13a74fea568bdc8e0d94e8f7ec7a14ce`, `mashed_re_dev.asi`
`23ed96647f13910df91d1aeb96563967ea6c37d158c913c45555c00bc15d277a`, `.map`
`088b717203cc6dc07a9602b061a80079e5fe7623a91f942b15e3b42fcf746a98`. The `.asi` was copied only to the private root
`D:/MashedWT/D4-WSB-C230-RECHECK_root` (robocopy of `original/`, `MASHED.exe` `b9977dab...`, `.unpatched` = anchor
`BDCAE093...`). `original/` not touched. Every boot ran under the test slot and `mashed_lock.py run --label D4-WSB-C230-RECHECK`.
S1 ran on the first `.asi` (`b944210f...`, RunTracked without the warm-up). W1, W2, S2 and the second gate run ran on the
final `.asi`. The control boots install no hook, so the wrapper change does not reach them.

## Leg 1: default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT` = the private root. All arms
`exit=0 self_exited=True`.

| exe | H_DEF step / gates / ptrace | B_DEF_K0 / K1 / K26 | game PIDs |
|---|---|---|---|
| `eacbe088` (RunTracked) | `5943c5f1` / `364350ae` / `52052890` | `3a03670c` / `e85ea6fb` / `53778664` | 4372, 18052, 1608, 9264 |
| `53a97b01` (final) | `5943c5f1` / `364350ae` / `52052890` | `3a03670c` / `e85ea6fb` / `53778664` | 8336, 20092, 4280, 18892 |
| ref `merge_train.json` | `5943c5f1` / `364350ae` / `52052890` | `3a03670c` / `e85ea6fb` / `53778664` | |

## Leg 2: heap-tracked write set (`run_shadow_c230.py`)

`MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, census `--track 0 --mode 10 --cars 4 --hold 60 --hooks
0x0055c230`, 24 samples. Summary `shadow_heap_summary.tsv`. Both arms: `PATCHBYTE installed=e9 uninstalled=83
A/B-IS-REAL`, no NOISY / UNTRACKED / OTHER-EXC line.

**S1** (game PID 2380): DIVERGENT, n 24, ndiff 1. Only sample `[0]`, the first call of the port body in the boot:
`pages=o:1,n:2,diff:1 stack=0 noise=0 ovf=0 ret=0 heap=o:1,n:2 @00b8f000+ae4 ndw=1 v=ae4:80000009/80000009/8000000a`.
The original's page compared equal. The port wrote one more page, with one dword `0x80000009` -> `0x8000000a`.
Samples `[1]`..`[23]` ndiff 0. Addendum 1 traces it to the port's first-call static init in `NarrowDotOrder()`
(`RwpSolverGlue5.cpp:179`): `__Init_thread_footer` increments the per-thread `__Init_thread_epoch`, the second dword
of the `.asi`'s 8-byte TLS template (`.map` `__tls_start` `0002:00018094`, epoch `0002:00018098`), whose live copy sits
in a loader-allocated heap block. [UNCERTAIN] `0x00b8fae4` = main-thread `.asi` TLS block + 4 was not read directly
(missing: `TEB.ThreadLocalStoragePointer[_tls_index]` in that boot). S2 is the indirect test the addendum names.

**S2** (game PID 19676), the wrapper warm-up in place: **HEAP-CLEAN**. n 24, ndiff 0, every sample `ndiff=0 OK` with
`stack=0 noise=0 ovf=0 ret=0`, `heap=o:1,n:1` x24, so **24 heap pages compared**. Sample `[0]` is clean, so the S1
extra page came from the static init the warm-up moved out of the tracked window. The S1 result stands as recorded.

Both logs also carry `fn=FUN_0055c0f0 PATCHBYTE installed=8b uninstalled=8b SUSPECT-no-restore` and `FUN_0055c0f0`
sample lines. The port impl calls the `0x0055c0f0` wrapper (`RwpSolverGlue5.cpp:198`), which is armed by
`MASHED_SHADOW_AB=1` but not installed in these boots. This is the same in S1 and S2. Those lines are not `0x0055c230`
samples and the summary does not count them.

## Leg 3: racediff window (`c4_racediff_v11.py`, `run_c230.py`, `boot_sets.py`)

v11 = BROAD-A v10 (copied unchanged) plus `C4_SAMPLES` (`make_tool6.py`, one asserted replacement, default 6). Every
arm used BROAD-A Z2's settings: census, `C4_HOLD=400`, `C4_WINDOW="300 30000"`,
`C4_TRANSP_SYMS=__init_thread_header,__init_thread_footer,_getenv`, no `C4_DEREF`, no `C4_ARGW`, `C4_CONTROL=1` (both
boots run the original). Summary `racediff_summary.tsv`. No arm needed a re-run.

Every signature has the form `[<t>] [c55c0f0,A4+0,A1+0] eax=A4+0`. `<t>` is the target of `call dword ptr [edx+0x18]`
at `0x0055c2a8` (`edx = [param_1+0x5c]`, `0x0055c2a3`). The part after `<t>` is identical in all 8 boots and all
992 samples.

| arm | boot (PID) | samples | `<t>` counts | set |
|---|---|---|---|---|
| W1 C1 | 14560 | 48 | `c55cdd0` 24, `c57b420` 24 | {cdd0, b420} |
| W1 C1 | 13472 | 48 | `c55c680` 24, `c55cdd0` 17, `c57b420` 7 | {c680, cdd0, b420} |
| W1 C2 | 9972 | 48 | `c55c680` 24, `c55cdd0` 21, `c57b420` 3 | {c680, cdd0, b420} |
| W1 C2 | 5988 | 48 | `c55cdd0` 26, `c57b420` 22 | {cdd0, b420} |
| W2 C3 | 4100 | 200 | `c55cdd0` 100, `c57b420` 100 | {cdd0, b420} |
| W2 C3 | 18108 | 200 | `c55cdd0` 70, `c563180` 39, `c57b420` 91 | {cdd0, 3180, b420} |
| W2 C4 | 12112 | 200 | `c55cdd0` 100, `c57b420` 100 | {cdd0, b420} |
| W2 C4 | 13224 | 200 | `c55cdd0` 100, `c57b420` 100 | {cdd0, b420} |

W1: 2 distinct sets over 4 boots. W2: 2 distinct sets over 4 boots. Neither window reproduces 4/4, and the PREREG
allows no third window, so **leg 3 fails**. The budget filled before A4 call 30000 in every boot (last A4 frame
9462..27771), so the window is call-bounded. Which call sample opens the window changes from boot to boot. With 6
samples (BROAD-A), 3 of 4 original boots gave {cdd0, b420}. Here, at 48 and 200 samples, `c55c680` and `c563180` each
appear in some boots and not in others.

## Tracker transaction (re-classify skill, followed step by step)

- C3 gate re-checked: port in `Collision/RwpSolverGlue5.cpp`, `RH_ScopedInstall(FUN_0055c230, 0x0055c230)`,
  runtime-toggleable, build clean, caller `0x005757d0` C3, callee `0x0055c0f0` C4.
- `hooks.csv` row `0055c230`: `C4,verified` -> `C3,impl`. Scenario
  `shadow_tracked_heap:track0-mode10-cars4+c4_racediff:census-...`. `frida_diff` -> this step's `racediff_summary.tsv`.
  The note segment names the failed leg (racediff, no 4/4 window, both windows' set counts), the passed leg (S2
  HEAP-CLEAN, 24 heap pages, S1 artifact) and the re-promotion path.
- `[UNCERTAIN]` sweep of `re/analysis/bucket_00557fb0/0x0055c230.md`: the only marker is already filed (U-9105,
  class `data`). No new row.
- Stub sweep: none in the body. No `STUBS.md` change.
- `re/analysis/CHANGELOG.md`: one `C4->C3` line inserted directly below `<!-- ENTRIES -->`.
- No `UNCERTAINTIES.md` / `DEFERRED.md` change.

Open on this row: re-promotion needs either a racediff scenario or window whose `[edx+0x18]` target set 4/4 original
boots reproduce, or a rule decision to compare only the tokens after the indirect call. The S1 TLS-address claim stays
[UNCERTAIN] as stated above. It blocks nothing because S2 is clean.

## Harness notes (for `re/HARNESS_BACKLOG.md`, not acted on here)

- A RunTracked wrapper whose port body holds a function-local static gives a one-sample diff on the first call (the
  `.asi` TLS init epoch, counted as heap). Warm the static in the wrapper before `RunTracked` (done here for
  `0x0055c230`). Other rows have the same pattern: every impl that reads a `NarrowFixKnobs.h` knob through a
  function-local static.
- On the original, the racediff's sample set for an indirect call depends on which objects the sampled calls receive.
  For rows like this one, set equality over the first N calls does not reproduce across boots at N = 6, 48 or 200.

## Files

Committed (`verify/wsbc230_20261027/`): `PREREG_D4-WSB-C230-RECHECK.md`, `PREREG_ADDENDUM_1.md`, `make_tool6.py`,
`c4_racediff_v10.py` (copy), `c4_racediff_v11.py`, `run_c230.py`, `run_shadow_c230.py`, `boot_sets.py`,
`racediff_summary.tsv`, `shadow_heap_summary.tsv`, this file. Trackers: `hooks.csv`, `re/analysis/CHANGELOG.md`.

Raw (not committed), `D:/MashedWT/D4-WSB-C230-RECHECK_data/`:

| file | SHA-256 |
|---|---|
| `shadow/0055c230/shadow_ab.log` (S1) | `5be1e86bcd0c3572a87276a58d88356c250e5c41dc2728212533abfc68917336` |
| `shadow/0055c230_s2/shadow_ab.log` (S2) | `189623c795f954903f056c70c87c5e9c1c8e5efd467f3d4a88ceddc03a8dcff3` |
| `0055c230_census_W1C1/c4_racediff_samples.json` | `5acefc576e61925b0ad4df33b0aa080e385c30866a3ebd832b60fed7c1e3de95` |
| `0055c230_census_W1C2/c4_racediff_samples.json` | `0cb559099b6c0c23468329d3a77389a372c2a71438a2301029464439b6bc3e94` |
| `0055c230_census_W2C3/c4_racediff_samples.json` | `d893c7e53bcb6c8cf4f210f61cb53e8fc6c64cdcefd840f85e791db7459c4966` |
| `0055c230_census_W2C4/c4_racediff_samples.json` | `fc2d0a33ed4f2bfb92d5bfb800202ae44f4b4a6bf5f13bef8b9762e36033e61e` |
| `gates2/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates2/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates2/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates2/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates2/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates2/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |

Every game PID in this step was spawned by this step and ended by its own tool (by PID). No MASHED from this step is
running.
