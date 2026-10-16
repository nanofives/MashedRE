# RESULT camribbon: the camera-path ribbon corners come from the original's node classification

**Verdict: closed, default ON** with the opt-out `MASHED_NO_CAM_RIBBON_FIX=1`. Every gate of `PREREG_camribbon.md` and
its two addenda passed, the controls failed where they had to, and G-CTRL is unchanged. All numbers are in `scores.txt`.
Branch `work/camribbon`, base `race/first-frame-parity` 118fb99d.

## 1. What was wrong, from the bytes

`FUN_00426d00(n, k)` selects nothing. It returns `0x00663670 + n*0x4c + k*0xc` (hooks.csv C3, `BatchAA_s6.cpp:96-99`,
asm `PathProgStep_naked.inc:1490`), a slot of the node record whose base `0x00663658` the track load fills. The
filler is `FUN_00478660`'s per-node body: the node's four vertices go through the inverse of the node matrix, then
local `x < 0 & y < 0 -> +0x18 (k=0)`, `x < 0 & y > 0 -> +0x24`, `x > 0 & y > 0 -> +0x30`, `x > 0 & y < 0 -> +0x3c (k=3)`
(`0x0047897c`, already ported as `PathNodes.cpp` `BuildNode`, byte-identical to the original's live block under gate
G-P1). So `c0` and `c3` are the two bottom corners. The standalone's ribbon built them another way
(`TrackRenderer.cpp`, gate vertex indices sorted ascending, `c0 = gv[0]`, `c3 = gv[3]`), which gives a top vertex for `c3`.

`FUN_00441820` reads `c0`, `c3` (and `FUN_00426cc0(n)`, record `+0x00`) only when the LED elevation dword is `-1.0f`
(`0x00441872` / `0x004418e9..0x00441984`): out_dir = node dir rotated by `0xc1c80000` (-25.0f) about `c0 - c3`.

## 2. The change

- `Race/PathNodes.{h,cpp}`: `PathNodes_CameraNodes(world, out)` runs the existing `BuildNode` (untouched) under the same
  PC_24 scope on a local zeroed 0x4c record per AI node and keeps `dir` (+0x00), `c0` (+0x18), `c3` (+0x3c). Not gated by
  `MASHED_PATHPROG`, does not write the course block.
- `TrackRenderer.cpp`: `cam_nodes_` comes from it by default. `MASHED_NO_CAM_RIBBON_FIX=1` keeps the sorted-index corners
  (A/B only, logs one line naming itself).
- `RaceCamera.h`: `has_led_elev(node)` (diagnostic). `MASHED_CAM_DUMP` also writes `<csv>.ribbon.txt` (resolution used, LED elevation per node).
- `re/frida/camera_probe.py`: `--track N` (override at `FUN_0040d440` onEnter, since writing the menu globals stalled the
  Quick Battle confirm), `--force-prog P` (pins the per-car path progress floats at `0x008a96e8 + i*0x30c` for the span of
  one director call, restores them after the trace row is built), `--nav-wait`, node cap 200 -> 400.

## 3. Node-level gates (no instrumentation, the original's live node records)

| gate | result |
|---|---|
| **G-C3-TRAIN** vs `orig2_nodes.txt` (live capture) and `o_course_t0.bin` | `c3` bit-equal 30 / 30 on both. `c0` 30 / 30 |
| **G-DIR-TRAIN** | `dir` bit-equal 30 / 30 (the old normalised dir was 7.6e-5 off) |
| **G-NODE-ARCTIC** vs `o_course_t3.bin` | `dir`, `c0`, `c3` bit-equal 94 / 94 |
| **G-NODE-CITY** vs the original's live ribbon (`orig_city_nodes.txt`, 117 nodes, read by the probe) | `dir`, `c0`, `c3` bit-equal 117 / 117 |
| **C-OPTOUT** (must fail) | `c3` equal 0 / 30 (max 1.50), 0 / 94 (Arctic), 0 / 117 (City, max 2.66). The TRAINING opt-out nodes file is byte-identical to d1r2's `standalone_nodes.txt` (sha8 `84d906f8`), so the opt-out is the previous behaviour exactly |
| **N0** | two default runs: nodes file sha8 `24992f1d` twice |

Three tracks, three different node counts, bit-equal to the original on every field. `FUN_00478660` is the only
writer of those records, and the ported classification reproduces it.

## 4. G-SCAN: all 13 tracks (`scan_tracks.txt`)

All 13 load (1,119 nodes in total). `c3` differs between the arms on 1,118 of 1,119 nodes, `c0` on 1 (rouabout), `dir` on
all 1,119 (the original's record dir is LUT-normalised, the old one exactly normalised). Nodes with no LED elevation:

| track | Course_Id | nodes | no-LED nodes |
|---|---:|---:|---|
| City | 26 | 117 | **25, 26, 27** |
| the other 12 (Arctic, dump, Egypt, Forest, Highway, Neustein, rouabout, sands, Storm, SuperG, training, Warzone) | | 30 to 136 | none |

The original's own override table agrees (probe, City): entries 24 = (14, 60), 25 / 26 / 27 = (-1, -1, -1), 28 = (14, 45).
So the c3 corner reaches the camera on City nodes 25 to 27 only, and TRAINING (all 30 nodes have an LED elevation) is inert, as d1r2 said.

## 5. The camera on City, before and after, against the original (G-NOLED)

Method. The director samples a node pair from each car's path progress. The cars start at node 0 and the racing span of a
round without input is 30 to 161 rows (section 8), so the probe pins the four progress floats for the span of each
`FUN_00446520` call (`--force-prog`) to reach nodes 25 to 27. That pin is the only instrumentation; everything the
director computes from it is the original's own. A natural race reaches the same code with real progress values, which
is not what was captured.
The standalone replays that trace (`MASHED_CAM_REPLAY`) and the published camera pose is scored against the original's
RwFrame, with the d1r2 tolerances (eye median <= 0.001, p95 <= 0.01, at-axis and right-axis median <= 0.005 deg, p95 <= 0.05 deg).
161 comparable racing rows per capture. Self-checks: `track_type` = 26 and `nodecount` = 117 on every row.

| capture (nodes sampled) | arm | eye median / p95 | at-axis median / p95 (deg) | right-axis median / p95 (deg) | gate |
|---|---|---|---|---|---|
| pin 25.5 (25, 26: no LED) | after | 0.00031 / 0.0020 | 0.0010 / 0.0063 | 0.00011 / 0.0046 | PASS |
| | before | 2.49 / 2.63 | 13.4 / 18.7 | 18.9 / 19.1 | FAIL |
| pin 26.5 (26, 27: no LED) | after | 0.00031 / 0.0025 | 0.0012 / 0.0074 | 0.00019 / 0.032 | PASS |
| | before | 2.49 / 2.64 | 13.2 / 18.6 | 18.3 / 18.8 | FAIL |
| pin 24.5 (24 LED, 25 none), report-only | after | 0.00057 / 0.0023 | 0.0018 / 0.0068 | 0.00008 / 0.0025 | PASS |
| | before | 1.31 / 1.43 | 6.9 / 9.1 | 8.9 / 10.8 | FAIL |
| pin 27.5 (27 none, 28 LED), report-only | after | 0.00042 / 0.0025 | 0.0016 / 0.0061 | 0.00005 / 0.0014 | PASS |
| | before | 1.29 / 1.38 | 7.2 / 9.6 | 9.6 / 11.2 | FAIL |
| pin 25.5, frame capture (below) | after / before | 0.00031 / 0.0020 vs 2.49 / 2.63 | 0.0010 / 0.0063 vs 13.0 / 18.3 | 0.00006 / 0.0038 vs 18.4 / 18.5 | PASS / FAIL |
| **G-PIN-CTRL** Training pin 10.5 (both nodes have LED), 102 rows | after and before | 0.000048 / 0.00015 | 0.0002 / 0.0008 | 0 / 0.00003 | both PASS, identical |

The discriminator held: the before arm exceeds the tolerance by three orders of magnitude, so the pin does exercise
`c3` (G-NOLED is not vacuous), and the control shows the pin and the replay do not distort anything where `c3` is unused.

Frames (G-FRAME-AFTER / G-FRAME-BEFORE-WORSE, new capture with `--shots-race 30,100`, shots at trace rows 810 and 880):

| K | orig@K vs after@K | after@K-60 | after@K+60 | orig@K vs before@K |
|---|---:|---:|---:|---:|
| 810 | 11.86 | 20.98 | 27.89 | 23.75 |
| 880 | 11.04 | 22.37 | 19.52 | 21.89 |

Sharp minimum on both K, and the before arm is about twice as far. `sbs3_row_00810.jpg` / `sbs3_row_00880.jpg` (in
`frames/`) show original | before | after: the after frame has the original's framing (same building and road
geometry), the before frame looks down a different street. The standalone's cars, HUD and shadows are its own, as in d1r2.

## 6. Control and collateral

- **G-CTRL** (`run_cam.py gctrl`, default arm, step / gates / ptrace): baseline exe `991a5fff / be0d47ea / bc75b405`, new
  exe default `991a5fff / be0d47ea / bc75b405`, new exe opt-out `991a5fff / be0d47ea / bc75b405`, final exe (the one the code in this commit builds) `991a5fff / be0d47ea / bc75b405`.
- **G-P1-REGRESS**: `MASHED_PATHPROG_DUMP` course block, baseline exe vs new exe: Training `0393688d` twice, Arctic `38238db4` twice.
- Warnings: TrackRenderer.cpp +1 C4996 `fopen`, +1 C4996 `getenv` (the classes already there), PathNodes.cpp unchanged, no new class.
- Race-demo flow (`MASHED_RACE_DEMO=1 MASHED_GOTO=6`, its track is Arctic, 94 nodes): exit 0, 8 captures, 770 / 770 camera rows from the director (`src` 1).
- `MASHED_CAM_POSE` 12 floats on City: `src` 3 on 599 / 599. `MASHED_SUBSTATE MASHED_MODE5_RESET` on City: no crash, 632 rows from entry 0, 567 from the director.

## 7. Item 3 of the brief: filed, not fixed (U-9687)

The byte scan of `MASHED.exe.unpatched` for direct references to `0x007f101c` gives 23 in `.text`: one `inc` at `0x004111c1`
(entry of `FUN_004111c0`, once per call), two register stores (`0x0040ff17`, the `0x00492770` boot reset), 20 reads, of
which entry 0's oscillators at `0x00445f1a` and `0x004463bd`. The exe never writes the address, so under
`MASHED_SUBSTATE` (default OFF) entry 0's roll phase stays 0. Porting the increment needs the exe tick that equals one
`FUN_004111c0` call (the original runs it N = (`DAT_007f1000` - 1) / 0x32 + 1 times per frame), plus a scorer for entry
0's matrix on the 765 phase-3 rows of `orig2_trace_v3.csv` (`f101c`, `E_*` columns exist, no exe-side scorer does).
That is a gate, not a one-liner, and the default build never executes it, so it is filed with that path. The
`0x004481e5` tgt copy has no known reader in the default build either (readers of `cam+0x10` not enumerated).

## 8. Found on the way

- `camera_probe.py` self-check (`|y| max < 1e-3` means "offsets wrong") fires on every City capture: City's cars sit at Y 0.0
  (gate bottoms at Y 0.035; Training's at 0.425). The replay matching the original's frame to 0.0003 proves the captured
  positions are what the director read, so the message is a false alarm there. Left as is.
- `camera_probe.py --shots*` needs an absolute `--out`: the request file path is used inside the game process, whose CWD is
  `original/`. With a relative path nothing fires and nothing says so. (Four captures lost to it.)
- The racing span of a pinned City capture was 30 rows in one capture and 161 in five others (round-end timing), so
  shot offsets must be chosen after seeing the span, or the capture repeated.
- Process note: my first capture piped the probe through `Select-Object -First 40`, which killed the python half and
  left one original `MASHED.exe` (pid 18832, mine) running. Stopped by PID only. The first build also deployed the
  `.asi` into this worktree's own `original/` (not the shared one: `original\mashed_re_dev.asi` there is still the
  13 Oct 20:39 file); later builds ran with `MASHED_NO_DEPLOY=1`.

## 9. Files

`PREREG_camribbon.md`, `PREREG_camribbon_addendum1.md`, `PREREG_camribbon_addendum2.md` (committed before the runs they
gate), `scores.txt`, `scan_tracks.txt`, `nodes/` (every nodes file scored), `orig_city_*_trace_v3.csv` + `_static.json`
(the original captures, pins 24.5 / 25.5 / 26.5 / 27.5, and `orig_train_p105_*`), `frames/` (the frame capture, trace,
side-by-sides, `orig_row_*.bmp` gitignored), `gctrl_*.txt`, scripts `run_cam.py`, `score_camribbon.py`, `frame_score.py`,
`scan_led.py`, `scan_tracks.py`, `orig_city_nodes.txt`.
