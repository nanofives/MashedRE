# RESULT PORT-TOP-KNOBON-CAPFLAKE (2026-11-09)

Verdict: **GREEN.** Cause found and reproduced on demand. Fix shipped as a default-OFF knob, and the gates are unchanged.
PREREG: `PREREG_PORT-TOP-KNOBON-CAPFLAKE.md` (committed alone, `baf5b0db2`, before any arm ran).
Branch `orch/PORT-TOP-KNOBON-CAPFLAKE` from `orch/PORT-TOP-04J` (`6a2edc84e`). Exe `409560ed`.

## Cause

The difference is not a draw and not a render state. It is the **contents of the frontend.mpg backdrop texture**.
The outlier runs froze the video on **frame 1** instead of frame 0. The TOP knob plays no part.

- `MpegVideoTexture::Update` (`D3d9Render/MpegVideoTexture.cpp`, the R10b freeze) uploads in `MASHED_DETERMINISTIC`
  mode once, from whatever buffer `ISampleGrabber::GetCurrentBuffer` holds at the first `Update()`. The graph is
  started at asset load (`exe_main.cpp`, `g_menu_video.Open`). The first `Update()` comes when the main menu first
  draws the backdrop (walk_02; walk_01 = title, no backdrop). So the frame depends on how far the real-time graph
  got by then.
- The 04G f2 and 04J p1 outliers are the same capture set, byte for byte (walk_02 `2b5487e980ea0a9f`, walk_12
  `27d967126f477737` in both). Against the normal set: walk_02 7588 pixels differ (max 3), walk_03..12 27084 to 33048
  pixels (max 9). Bounding box x 0..509, y 12..479 in all of them, which is the 512x512 backdrop quad drawn 1:1 at the
  origin. walk_01 has no difference.
- **Why only knob-ON:** the slot, not the knob. Every driver ran knob, hand, hand, knob, so the first nav run after
  the gate arms was always a knob-ON run. A1 below: 12 runs, ON and OFF alternated, all on frame 0. A2: frame 1
  pinned with the knob **OFF** gives the outlier set.
- The draw list does not see this. The backdrop quad's vertices and state do not change. Only the texture bits do,
  so `drawlist_diff.py` has nothing to report. The texture-level evidence is the frame hash probe plus the capture
  hashes (imgdiff bounding box above).

## Arms (every run inside test-slot + `mashed_lock.py run --label PORT-TOP-KNOBON-CAPFLAKE`, root `D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_rootK`)

Class: E = the 04E list (12/12), X = the 04G / 04J outlier list. Table: `capflake_runs.tsv` (committed, 30 rows).

| arm | runs | freeze (probe) | captures |
|---|---|---|---|
| A1 pin OFF, TOP ON/OFF/OFF/ON x3 | 12 | `live pos=0 hash=28538c74` in 12/12 | E in 12/12 |
| A2 pin 0 ms (TOP OFF) | 1 | `pinned pos=55 hash=28538c74` (frame 0) | E |
| A2 pin 28 ms | 1 | `pinned pos=333388 hash=eecc6844` (frame 1) | **X**, the outlier, byte-identical |
| A2 pin 62 / 95 / 128 / 162 ms | 4 | frames 2..5, `34ab675b` / `cecfaac9` / `4a1a62fd` / `0efe65f3` | four other sets (1 of 12 = E, walk_01 only) |
| A3 pin 0 ms, TOP ON/OFF/OFF/ON x3 | 12 | `pinned pos=55 hash=28538c74` in 12/12 | **E in 12/12** |

A1 hash = A2 frame-0 hash, so the normal outcome is frame 0. Prediction checks from the PREREG: same hash means same
class in every run, ON or OFF (held). One pinned frame gives E and matches the A1 hash (held). The frame next to it
gives X (held). A3 passes (12/12 runs class E).

The outlier did not recur in the 12 live A1 runs. That is expected: it needs the graph to advance one frame before
the first `Update()`, which happened 2 times in about 30 earlier runs, both in the first run after the gate arms.
[UNCERTAIN] why the live graph reports position 0 at the first `Update()` in 12/12 runs, seconds after `Run()`, and
what moves it one frame in that slot. Not traced, because the pinned path removes the dependency on it.

## Change (default OFF)

- `MASHED_DET_MPG_PIN=<ms>` (deterministic mode, looping graphs only, so only the frontend backdrop). The grabber's
  `BufferCB` copies the first sample whose start time is >= `<ms>`. The graph runs with no reference clock
  (`IMediaFilter::SetSyncSource(NULL)`), so no sample is ever late or dropped. `Update()` uploads that copy and stops
  the graph. `MASHED_DET_MPG_PIN=0` = frame 0 = the 04E list.
- Probe, log only, deterministic mode: `F1 frontend.mpg det freeze: live|pinned pos=<100ns> hash=<fnv1a>`, once.
- Files: `D3d9Render/MpegVideoTexture.{h,cpp}`, `exe_main.cpp` (probe line). No RVA, no hook, no tracker row.

## Acceptance

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | OK, `original\mashed_re_dev.asi` not deployed |
| rva-lint | `OK -- no NEW duplicate bodies (1 known)` |
| asm-lint | OK |
| gates, pin unset (`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`) | G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 = merge_train.json refs, all exit 0 and self-exit |
| port deltas | none (no body, no caller change) |

## Recommendation (the lead's call)

1. Add `MASHED_DET_MPG_PIN=0` to the env of every 12-capture driver (the `run_all_04*.py` / `knob_boot.py` nav calls,
   `re/tools/port_batch_bisect.py`). With it the capture check no longer depends on machine load, and the 04E list
   stays the reference. This is a harness setting, not a game default, so no decision row is needed.
2. Do **not** make the pin the deterministic-mode default without a gate check. The gate arms run with
   `MASHED_DETERMINISTIC=1`. With the pin, the frontend graph decodes at free speed from `Open()` until the first
   `Update()`. If an arm never draws the menu backdrop, the graph decodes the whole 38 MB file on a spare core during
   the arm. Gates with the pin set were not measured here.

## Raw data (`D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_data`, not committed)

| file | SHA-256 |
|---|---|
| runs.tsv (= committed capflake_runs.tsv) | 8aa6e15aea09e2a95e44714a375281e6c662ddee330e6586a081d1ca57ff39a8 |
| build1.log | a44dba7a3ccd7d3eed3c42c5bda937f28cc4a415be3833ec60c7538e7dced59c |
| A1_run.log | bbff8c5978bd2191e6617231311b5a4b487dd32cae026137efd91d6d7bc5aeb9 |
| A3_gates_run.log | 212078e2c237675b9163692b504e16bc5d049f866ce5e96289164386e24391c5 |
| A1/r01_ON.log | 68dbcb99376502b1c77f78515b24e1f87a067674eb3d06d0d3271dc2f64b915a |
| A2/r01_OFF_pin0.log | d03f295637092d8676952fa27d413181d9a2583324cf0867ae2789bf8b13902e |
| A2/r02_OFF_pin28.log | eaa0fab96d9fd50b863931c15f41a3c51822016d881a6c3ba3fa04152e442368 |
| A2/r01_OFF_pin0/walk_02_mainmenu.bmp | 24249b644e3e8057271f3f1c252514c7820abec3f3da15d316f39dbf156a9066 |
| A2/r02_OFF_pin28/walk_02_mainmenu.bmp | 2b5487e980ea0a9f4a25aec20e3ab66cd3e5595350047d7f69a59accc79242ea |
| arms/H_DEF.step.csv / .gates.csv / .ptrace.log | 8284be46b3c6... / 6793183492e0... / 31fe043e261d... (full in A3_gates_run.log) |
| mashedmod/build/mashed_re.exe | 409560eddcb7ab419cba04cc8b4bac3af53f111ae21f3d52268edac52ececff4 |

Outlier references: `D:/MashedWT/PORT-TOP-04J_data/p1/navdemo_knob1/` and `D:/MashedWT/PORT-TOP-04G_data/f2/navdemo_knob1/`.

Machine rules: the build waited out the merge-train PRIORITY-HOLD / SOLO from 19:20 to 20:02. Every game launch ran
inside test-slot (OUTSIDE) and `mashed_lock.py run` (INSIDE). Kills: none needed. Root `original\` is a junction to
the 04J root's copy, and the exe writes nothing there.
