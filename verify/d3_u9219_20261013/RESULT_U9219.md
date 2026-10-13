# RESULT: U-9219, the mode-7 refdist bridge

Pre-registration: `PREREG_U9219.md` (`a5a904e1`), amendment A1 (`48c72263`, committed before `o_rd3` existed).
Scorer: `score_u9219.py` -> `SCORE_U9219.txt`. Branch `work/u9219`, base `0bc7ccd0`.

## 0. Verdict first

> **The bridge is not the defect.** Fed the original's own per-call inputs, the port's `0x00442a60` body picks the
> original's reference and other index on **220 / 220** calls and reproduces `refdist[0..3]` on **880 / 880** values
> (confirmatory run `o_rd3`; exploratory `o_rd1` 630 / 631 and 2521 / 2524, `o_rd2` 220 / 220 and 880 / 880). Pre-registered
> decision 1 fires: **no bridge change**. U-9219's premise ("the bridge's zeros admit cars the original keeps out") is
> refuted twice. (1) The original's car 2 does get refdist 0: on **129 / 341** driving consumer calls in `o_rd1`, a
> longer round of the same recipe. It still returns 2 on **0** of them. (2) What keeps the original's car 2 out of
> mode 7 is its **held word**. It collects a power-up at driving call **27** in every original run (`0x0088fc88 +
> 2*0xb4` = `0x5f9a58` from then on), and `FUN_00414490` returns 0 for any nonzero held word (`0x00414490`, `(&DAT_0088fc88)[v *
> 0x2d] == 0`). The exe's car 2 holds nothing until call 1,742, and **52 / 52** of its returns of 2 have held == 0.
> This is consistent with RESULT_M3 finding 2, the car-2 opening-pickup miss. [UNCERTAIN: which pool the original's
> car 2 collects at call 27 is not read here. The cb trace of a `--cb-probe` run would name it.] Car 3's 43 exe returns all fall after the
> original's round has ended: **0 vs 0** inside the original's window.

| gate | result |
|---|---|
| G-ORIG-DET | **FAIL** -> floor. Car-2 refdist-0 producer calls 246 (`o_rd1`) / 12 / 12. `o_rd2` == `o_rd3` digit for digit, `o_rd1` is a longer round (W = 631 producer calls vs 220) |
| G-PROBE (as registered) | **FAIL** 476 / 1082, 201 / 561, 201 / 561. Decision 3 fired: no verdict from `o_rd1` / `o_rd2`. Cause: the alignment rule, not the probe (A1) |
| **G-PROBE'** (A1) | **PASS** `o_rd3` 561 / 561 (exploratory `o_rd1` 1082 / 1082, `o_rd2` 561 / 561): the consumer at clock `c` reads the producer output of `c - 50` |
| **G-REPLAY-SEL'** (A1) | **PASS** `o_rd3` 220 / 220 |
| **G-REPLAY-DIST'** (A1) | **PASS** `o_rd3` 880 / 880, rel. error median 3.97e-05, max 1.19e-04 (bit-equal 221 / 880: `std::sqrt` vs `FUN_004c3ac0`) |
| G-REFDIST | PASS, **uninformative**: the G-ORIG-DET floor is 2 x 125 for car 2, so it cannot fail (A1 says so in advance). Raw: exe car 2 refdist-0 80 / 341 vs original 129 / 4 / 4 |
| G-MODE7 | **FAIL**, unchanged: exe car 2 52, car 3 43 (original 0, 0). Inside the original's window: car 2 **20** vs 0, car 3 **0** vs 0 |
| G-CTRL (default) | **PASS** `U_CTRL` `ae67b13a / c8317ce2 / 31db9384` (the refactor is value-identical) |
| G-CTRL (M4 knob-ON) | **PASS** `U_M4ON` `d57dd28d / b18f69f2 / 803b179c` = `M4_ON`, and its consumer trace `U_M4ON.c30.tsv` = `M4_ON.c30.tsv` (`7442be97`) |
| (b) regime A | unchanged by construction (byte-identical step / gates / ptrace on both stacks): default 2/3, M4 knob-ON 0/3 (RESULT_B2FLIP, RESULT_M4). Not re-scored |
| collateral | same side: **0 fields changed** on either stack (identical hashes). Cross-side banded (`COLLATERAL_U9219*.txt`, A = `o_rd3`, floor `o_rd1`, B = `U_M4ON`, floor `M4_ON`): **not a measurement**, because no speed channel is common to both AI step dumps and the only shared candidate, `s.v1.rec_b0c`, has disjoint ranges (original within ±0.07, exe up to 16.75), so 0 bands are shared |

## 1. What the original does (bytes, `re/tools/decomp_pc.py`, read-only pool slot)

- `FUN_00442a60` (`0x00442a60`, 531 B): zeroes `0x008989b0..bc`, `FUN_0040e180(&a, &b)` (out1 = inner, out2 = outer,
  `<=` so the later pair wins ties, 3-D `FUN_004c3ac0` length), `pa = FUN_00408ad0(a)`, `pb = FUN_00408ad0(b)`
  (`0x008a96ec + v*0x30c`), wrap rule (`_DAT_005cc730` 80.0, `_DAT_005ccd6c` 20.0, `_DAT_005cc568` 100.0). The higher
  race_pct of the pair is the reference if its `FUN_0046cbb0` out1 (`0x00881f90 + v*0xd04`) is 0, else the other if
  its out1 is 0, else no store. Then `0x008989a8 = ref`, `0x008989c8 = other`. For each car with `FUN_0040e370`
  (`*(*0x005f2770 + 0x34 + 4v) != 0`) and `FUN_0046c7b0` (`0x008815a4 + v*0xd04`) == 1:
  `refdist[v] = 0.8 (_DAT_005cc9bc, bits 0x3f4ccccd) * |XZ(ref) - XZ(v)|`.
- Only caller: `FUN_00446520`, in the race branch after the `FUN_0041ef60` loop, itself under `FUN_00448220`.
  Measured: one producer call per 50-unit clock tick, and the AI consumer at tick `c` reads the value written at
  `c - 50` (G-PROBE', 561 / 561).
- Consumer arm, `FUN_00414c30`: per registry slot, `FUN_004c3ac0(car - slot) < _DAT_005cc9f4` (8.0) and dy below the
  slot radius, then mode 7 needs **all** of `FUN_00442cc0(v) == DAT_005d757c` (0.0), `FUN_00414490(...) != 0`
  (which requires `FUN_0040e350() != 0xb`, `!= 10`, the held word `(&DAT_0088fc88)[v*0x2d] == 0`, and a heading dot
  above `_DAT_005cd090`), and dy in (`_DAT_005cd0b0` -0.75, `_DAT_005cc950` 0.75). Constants read from
  `MASHED.exe.unpatched`.

## 2. Per-car tables

Original driving window W (first producer call with `DAT_0063ba8c == 6` until the first call where any car's
`0x00881f90` state is nonzero). The 734 phase-3 calls (`ba8c == 3`, cars parked, reference car 0) that made up most
of RESULT_M4's "0 / 890" are outside it.

Reference index `0x008989a8` over W (producer calls):

| run | W | car 0 | car 1 | car 2 | car 3 |
|---|---|---|---|---|---|
| `o_rd1` | 631 | 15 | 349 | **246** | 21 |
| `o_rd2` | 220 | 15 | 164 | 12 | 29 |
| `o_rd3` | 220 | 15 | 164 | 12 | 29 |

Consumer calls in W (`FUN_00414c30` entry):

| side / run | calls c1 / c2 / c3 | refdist 0 c1 / c2 / c3 | returns 2 c1 / c2 / c3 |
|---|---|---|---|
| `o_rd1` | 532 / 341 / 209 | 347 / **129** / 0 | 60 / **0** / 0 |
| `o_rd2` = `o_rd3` | 204 / 175 / 182 | 164 / 4 / 29 | 49 / 0 / 0 |
| exe `M4_ON`, `o_rd1` length | 532 / 341 / 209 | 260 / 80 / 47 | 25 / 20 / 0 |
| exe `M4_ON`, `o_rd3` length | 204 / 175 / 182 | 102 / 47 / 20 | 25 / 20 / 0 |
| exe `M4_ON`, whole run | 1716 / 13496 / 1977 | 925 / 5136 / 528 | 25 / 52 / 43 |

On refdist-0 driving calls (`attrib_c30.py`, `held_timeline.py`):

| | held word | non-car slot within XZ 8 | first held != 0 |
|---|---|---|---|
| orig car 2 (`o_rd1`, 129 calls) | `0x5f9a58` on 129 / 129 | 0 / 129 | driving call 27 (both runs) |
| orig car 3 (`o_rd3`, 29 calls) | 0 on 29 / 29 | 0 / 29 | never |
| exe car 2 (5136 calls) | 0 on 168, `0xc` on 4968 | 3827 / 5136 | call 1742 |
| exe car 3 (528 calls) | 0 on 316 | 225 / 528 | call 642 |

Every exe return of 2 has held == 0 (car 1 25 / 25, car 2 52 / 52, car 3 43 / 43). Every original return of 2 too
(car 1 49 / 49, 60 / 60). The exe's nonzero held values are small ids (`0xc`, `0x13`) where the original's are
pointers (`0x5f9a58`, `0x5f9b58`). Only `== 0` is tested at `0x00414490`, so the format difference does not change
the arm.

## 3. Cause, with addresses

1. The refdist zeros are a faithful function of the exe's car state. The reference is the leading member of the
   most-separated pair, and in the exe car 2 keeps pace with car 1 (z -18.80 vs -18.71 at t = 6750) where the original's
   `o_c30a` car 2 falls behind (-12.44 vs -17.81 at t = 6900). That is upstream AI pace, already U-9185 / U-9186.
2. Car 2's mode-7 entries are gated by the held word, not by refdist. The exe's car 2 misses the opening pickup the
   original's car 2 collects at driving call 27 (RESULT_M3 finding 2: by 0.065). From then on the original's
   `FUN_00414490` returns 0 at its held test.
3. Car 3's 43 exe entries come after the original's round has ended (0 inside W). The exe's round runs longer, so
   they belong to the round-length gap, not to mode 7.
4. **Ordering difference found (not fixed, decision 1):** the original's consumer reads the previous tick's refdist.
   The port computes refdist in `UpdateRace` (`TrackRenderer.cpp:5836`) BEFORE `Ai_Standalone_Tick` in the same frame,
   so its consumer reads the current tick's value. The port's own comment (`:5832-5835`) states the same-frame
   placement as intended. It also reads race_pct one frame old, because the race_pct bridge writes after it
   (`:5858-5869`). Effect on the counts not measured. [UNCERTAIN: whether the one-tick lead changes any mode-7 entry.
   The replay feeds equal inputs, so it cannot see this. It needs a knob-gated "compute after the AI tick" arm.]

## 4. Code

`Race/SpectatorDistances.cpp`: the `0x00442a60` body split into `ComputeDistancesCore(const RefDistIn&, RefDistOut*)`
(the same expressions) plus the live fill `FillRefDistIn`. Added an env-gated replay `MASHED_REFDIST_REPLAY=<rd.tsv>[;...]`
-> `MASHED_REFDIST_REPLAY_OUT`, run once at the first race tick, no game state written. `re/frida/scenario_launch.py`:
read-only `--refdist-probe` (entry + leave on `0x00442a60`, ~20 calls/s) -> `<out>.rd.tsv`. No knob default changed.
No C-level change: `0x00442a60` has no `.asi` body (EXE-ONLY TU), so there is no run_diff / path2 leg.

## 5. Proposed tracker transaction (for re-classify, not applied here)

- **U-9219 -> resolved (refuted premise).** Evidence `verify/d3_u9219_20261013/RESULT_U9219.md`. The bridge reproduces
  `0x00442a60` on the original's inputs (G-REPLAY-SEL' 220 / 220, G-REPLAY-DIST' 880 / 880). Car 2's mode-7 entries
  follow from its held word (the opening-pickup miss), and car 3's from the round length.
- **New row (semantic):** the car-2 opening-pickup miss, if no row holds it yet. Gate it with the held-word timeline:
  original first held != 0 at driving call 27, exe at 1742. Carrier: RESULT_M3 finding 2.
- **New row (structural, minor):** the refdist one-tick ordering (§3.4), knob-gated test arm named.
- hooks.csv: no change.

Proposed CHANGELOG line:
`2026-10-13 U-9219 refuted: the refdist bridge reproduces 0x00442a60 on the original's inputs (220/220 selection,
880/880 distances, o_rd3). Car 2's mode-7 entries follow its held word (orig picks up at driving call 27, exe at 1742),
car 3's fall after the original's round ends. No bridge change. verify/d3_u9219_20261013/RESULT_U9219.md`

## 6. Raw

`o_rd{1,2,3}.msd*` (original, `--refdist-probe` + `--c30-probe`, provenance json each; the `.msd` are force-added),
`replay_port.tsv`, `replay_port_rd3.tsv`, `U_CTRL.{step,gates}.csv`, `U_M4ON.{step,gates}.csv`, `U_M4ON.c30.tsv`
(the exe ptrace logs are gitignored, their hashes are above), `score_u9219.py`, `probe_align.py`, `attrib_c30.py`,
`held_timeline.py`, `pivot_aistep.py`, `SCORE_U9219.txt`, `COLLATERAL_U9219*.txt`.

Exe recipe: `run_h4.ps1`'s env (Training, car 1, round 1, rule 4, `MASHED_DETERMINISTIC=1`, 14400 frames,
PLAYERTRACE, AI_STEPDUMP, U9186_GATES) run inline from the worktree. `U_M4ON` adds the `M4_ON` knob set. Replay:
same env with `MASHED_DET_FRAMES=600` and `MASHED_REFDIST_REPLAY`. Original: the `o_c30a` argv plus `--refdist-probe`.
No collision AV occurred in the three original runs.
