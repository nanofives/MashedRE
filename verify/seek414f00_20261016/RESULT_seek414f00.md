# RESULT seek414f00 (2026-10-16)

Branch `work/seek414f00` from `race/first-frame-parity` 9b024812. Model claude-opus-5-5. PREREG committed before the first run
(`PREREG_seek414f00.md`, commit 5c9271e8). Images: `images_sha.txt` (base = 9b024812 built in this worktree, final = this branch).

## 0. Verdict

`FUN_00414f00` is ported (one body, both targets), C2 -> C3 as `AiTrackObjSeek`. It does **not** move D3 (b) and cannot today:
(b) runs on TRAINING rule 4, which never reaches it, and the exe still has no writer of the words it gates on.
**The premise was inverted.** `0x00414f00` READS `0x0088ff50[v]` and `0x00684de0`, it does not produce them.

## 1. What the bytes show (static, unpatched image)

Files: `decomp_414f00.txt`, `datarefs.txt`, `decomp_451ad0.txt`, `decomp_warzone_obj.txt`, `disasm_414f00.txt`.

- `0x00414f00` reads `fld [0x00684de0]` (`0x00452eb0`), `[0x0088ff50 + v*4]` (`0x00452ea4`) and `[0x00684dac] + 0x30` (`0x00452160`).
  Its only writes are `cand[0]`, `cand[1]`.
- Those are fields of one object at `0x00684d18`: `+0xc8` = `0x00684de0`, `+0x94` = `0x00684dac`. Its state machine `FUN_00451d80`
  (6 cases, jump table `0x00452118`) has one caller, `0x00452ba2` in `0x00452b20`, which is referenced only from `0x005f3578`:
  slot 1 of the per-track hook record whose Course_Id is `0x21` (Warzone, `0x005f3570`).
- The only writer of `0x0088ff50[v]` is `FUN_00451ad0` (`0x00451b47` = 1, `0x00451b5b` = 0), value = `FUN_00451990`, an XZ box test
  of car v in the object's frame. It is called from every case of `FUN_00451d80`. So the word can be non-zero only on Warzone.
- Gate 1 is `0.5 <= h <= 30.0` (`0x005cc32c` = 0.5). A NaN passes both compares (`jnp` `0x00414f13`, `jne` `0x00414f25`).
- Callers `0x00416478` (`FUN_00416250`) and `0x00417fc8` (`FUN_00417da0`), both behind course `0x21`. `FUN_00416a30` (rules 4/9)
  has no call to it.
- The two discarded calls (U-7574) write only stack locals (`0x0046d6ec`, `0x0046d580`, `0x0046d647`, `0x0046d5d6`): dead.

## 2. Does the original ever set 0x0088ff50? Yes, on Warzone. (stock MASHED, `--seek-probe`, 4 cars, mode 10, 120 s)

| arm | track / rule | object rows | 0x0088ff50 != 0 | max 0x00684de0 | 0x00414f00 calls | ret = 1 | PREREG |
|---|---|---:|---|---:|---:|---:|---|
| O1 | Warzone / 0 | 7036 (state 0 only) | 0 frames | 0 | 8494 | 0 (all fail gate 1) | held |
| O2 | Warzone / 4 | 7138 (1 cycle) | car 3: 321 frames | 4.3956 | **0** | - | held (rule 4 never calls it) |
| O3 | Warzone / 8 | 7139 (2 cycles) | car 3: 182 frames | 2.0313 | 20835 (20000 kept) | **29**, all `cand` = object x/z | held |
| OC | Training / 0 | **0** | - | - | **0** | - | held |

The object cycles 0 -> 1 -> 2 -> 3 -> 0 when a car enters its box; its matrix row moves (z 58.45 -> 64.62) in states 2/3 while
`+0xc8` steps by 0.0333 per frame (`CAPTURE_SUMMARY.txt`). Whether it triggers in a given 120 s is run-dependent (O1 no, O3 twice).

## 3. Port

`mashedmod/src/mashed_re/Ai/AiTrackObjSeek.cpp`, in both rsp lists, `RH_ScopedInstall(AiTrackObjSeek, 0x00414f00)`. Thunks per target
(the `AiPostStepBrake.cpp` shape): .asi calls the original `FUN_0046d4a0` / `FUN_004c3ac0` / `FUN_0046c7b0`; the exe reads the record
mirror's `+0x928` ring row at `[+0x9a8]`, `LutSqrt((x*x+y*y)+z*z)` in float (PC_24 form, the `0x004c3ac0` order), `Host::car_alive`, and
literal thresholds. Exe call sites: `ControlStep` at `LAB_00416462` and `ControlStepM8` (`0x00417fc8`), behind `track_index() == 0x21`.
**Default ON**, opt-out `MASHED_NO_SEEK414F00=1`; `MASHED_SEEK_TRACE=<path>` is the call witness.
Renamed from the C1 gloss "powerup seek": the bytes show a track-object reader. `0x00452160` / `0x00452eb0` keep their "Powerup" names
(C3 rows of other lanes); a correction note was added to their rows.

## 4. Gates

| gate | result | file |
|---|---|---|
| path1 `ai_track_obj_seek` (cache_setter_observe + fold_ret) | **GREEN 53/53** on the final .asi: 29 natural `ret=1` rows from O3, 8 natural gate-2 fails, 4 gate-1 fails, 12 contrived (car-0 branch, alive 0, \|d\| <= 0.25, the 1.5 / 0.25 edges, held 0.5 and 30.0 exact and one ulp out, NaN, v = 0). 36 write / 17 sentinel kept | `w2/path1_*.txt`, `w2/diff_*.csv` |
| path2 | **PASS**: E9, rel32, 3/3 entries, outputs as path1 | `w2/path2_*.txt` |
| live, hook installed (Warzone rule 8, 180 s, `--hooks 0x00414f00`) | H1: entry dword `e9 5b 20 54` at arm and drain; object 3 cycles; car 3 committed **mode 10 on 177 frames, 177/177 gate-consistent** (predicate 1 and 0.5 <= held <= 30 in that row). H2: object never triggered, 0 mode 10 | `H_SUMMARY.txt`, `w1/H*.txt` |
| nav boot, full set / only this hook | both reach a race, no detach | `w1/nav_probe_*.txt` |
| full set 45 s | 1291 / 1332 installed incl. `0x00414f00` (index 647), survived | `w1/install_observe_all.txt` |
| **G-CTRL** exe (TRAINING) | `2b4d47bb / 356821c8 / f7a155e1` base = new = N0 | `base_gctrl.log`, `new_gctrl.log`, `n0_gctrl.log` |
| **D3 (b)** per regime, K = 0,1,2,4,26,27,54,57 | all 8 step dumps byte-identical base = new = N0 (base equal to dualcopy3's too). **A 1/3, B 2/3, C 0/3 before and after** | `D3B_base.txt`, `D3B_new.txt` |
| mode-7 entries | 0 in every dump, base and new (the mode-7 arm is default OFF) | `MODE_ENTRIES.txt` |
| exe Warzone (3600 frames) | `b726286f / b6360eef` base = new = N0; 0 mode-10 rows | `*_warzone.log` |
| knob took | new: the port ran on Warzone (>= 6001 calls traced), gate 1 passed 0 times, returned 0 every time (held 0, predicate 0). N0 / base: never called | `new_seektrace.txt` |

## 5. Why (b) does not move

(b) is TRAINING (Course_Id 30) with rule 4. Rule 4 dispatches to `FUN_00416a30`, which has no mode-10 block, and the block is gated on
course `0x21` anyway. On Warzone the exe port is reached but `0x00684de0` is never written in the exe, so `0.0 < 0.5` ends it at gate 1.
For this to matter in the exe, the course-0x21 track object (`0x004527f0` init, `0x00452b20` tick, `FUN_00451d80` state machine,
`FUN_00451ad0` / `FUN_00451990`) has to be ported first. That also unblocks the brake branch of `0x00417640`.

## 6. Trackers (one transaction, `tracker_seek.py`)

`hooks.csv`: `00414f00` -> `AiTrackObjSeek` C3, `file == exe_file`; notes on `00452160`, `00452ea0`, `00452eb0`.
`UNCERTAINTIES.md`: resolved U-7574, U-0422, U-0423, U-0424; new **U-9719** (state 0 -> 1 trigger not decoded). CHANGELOG: one entry.
Plate `re/analysis/bucket_ai_00407a40_00415880/0x00414f00.md` has the corrected reading.

## 7. Hygiene

Two deploy windows (`window.py`, backup + no-foreign-MASHED + restore in `finally`): restored `270e036f` and then `f4449212` (another
session redeployed between the windows), both `matches original: True`. Only PIDs this lane spawned were stopped. One self-inflicted
incident, fixed before any build: a PowerShell rename map flattened single-pair entries and replaced single characters in
`AiStandalone.cpp` / `AiTrackObjSeek.h`. Both were restored from HEAD and re-edited with `rename_fix.py`; the diff is the intended
10 lines. The MB-sized captures and dumps were deleted; `CAPTURES_SHA.txt` keeps their sha8, `*.kept.csv` the informative rows.
`make_vectors.py` ran on the full O3 capture (sha8 `e7f11e0d`).
