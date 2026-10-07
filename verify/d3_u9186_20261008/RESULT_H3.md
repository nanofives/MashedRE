# RESULT — U-9186 leg H3: port `FUN_00442a60` (DEFERRED leg B)

Date 2026-10-08. Pre-registration: `PREREG_H3.md`. **RAN.** Default-OFF behind `MASHED_REFDIST`.
No C-level (leg B is a bridge and inherits the 2026-10-06 decision). `original/` untouched,
`.asi` code path untouched.

Unblocked by **USER DECISION (Mariano, 2026-10-08)** lifting D-11072's "DO NOT start leg B",
recorded in `DEFERRED.md` and the CHANGELOG.

## 0. Verdict first

**The producer works. `0x008989b0` carries live per-car distances for the first time.** And, like
E2, it is **INERT at the behaviour level** — nothing outside the output itself changed.

| gate | verdict | figure |
|---|---|---|
| `H3-CONSTS` | **PASS** | all four non-zero, read from `MASHED.exe.unpatched` — 80.0 / 20.0 / 100.0 / 0.8 |
| `H3-WROTE` | **FAIL as registered (100%), PASS at 99.9926% on the gate-passing denominator** | 40,491/40,494; see §2 |
| `H3-PAIR` | **PASS (live)** | 7 distinct `(ref, other)` pairs on the ON arm vs 1 on OFF |
| `H3-DET` | **PASS** | repeats identical within each arm |
| `H3-INERT` | **INERT** | OFF vs ON differ in `refdist`, `ref_idx`, `other_idx`, `exp_dist` and **nothing else** |
| `H3-GATEFIRE` | **NOT MEASURED** | the counter was registered but not built — see §4. No claim is made for it |

Raw: `H3_GATES.txt`, `H3{off,on}_{1,2,3}.gates.csv`.

## 1. The producer is live

ON arm: `refdist` non-zero on **26,994/53,992 rows (49.9963%)** with **19,562 distinct values**;
OFF arm: a single value, `0`. The 50% is the expected shape — per frame, car 0 is gated out and the
reference car's own distance is legitimately `0.0`, leaving two of four non-zero.

`H3-PAIR` shows the selection is live and varying: `(1,2)` 14,956 frames, `(3,2)` 14,384, `(2,3)`
10,196, `(2,1)` 1,480, `(3,1)` 1,076, plus `(0,0)` on 4. On the OFF arm it is `(0,0)` on every row,
because the port never writes `0x008989a8`/`0x008989c8` there.

**The `FUN_0040e180` hazard did not bite.** `PREREG_H3.md` §3 registered that the exe copy
(`RaceCamera::MostSeparatedPair`, DEMOTED C4→C2 for a 7.8% `std::sqrt` disagreement) might select a
different pair. H3 ended up *using* that body rather than duplicating it — see §3 — so the question
became whether the array H3 fills makes it answer the original's question, and the varying,
plausible pair distribution says it does. It is still not a bit-identity claim.

## 2. `H3-WROTE` — failed as written, and the scorer is why

Registered threshold was 100% of rows; measured **74.9963%**. Attribution, measured not argued:

- **13,497 of the 13,500 mismatches are car 0**, with `slot_state == 0`. The port's gate chain
  correctly skips the player slot; my probe's `exp_dist` computes a distance for every car
  regardless. That is a scorer-scope defect, the third instrument miss this session
  (`G-TOOK`, `H1-CONVERGE`, now this) — the denominator was "all dumped rows" where the quantity
  only exists on gate-passing rows.
- On the **gate-passing denominator** the gate is **40,491/40,494 = 99.9926%**.
- **The remaining 3 rows are named, not waived.** All three are **frame 0**, cars 1/2/3, with
  `ref_idx = 0`: on the starting-grid frame cars 1-3 still read position `(0,0)` and the reference
  resolved to car 0, whose gate fails, so the port returned without writing — correct behaviour —
  while the probe computed `1.65133822` from car 0's position against three origins. One frame,
  and the port is the side that is right.

Reported as a registered-threshold failure with the corrected denominator alongside, rather than
re-cutting the denominator and calling it a pass.

## 3. The lint reshaped the port, twice, and improved it

`PREREG_H3.md` assumed H3 would need its own `FUN_0040e180`. `scripts/lint_rva_bodies.py` refused:
the exe already has one (`Race/RaceCamera.cpp:182`). Reading it, that body is a faithful
transcription — loop, `<=` tie rule, and the `0x0040e2f3..0x0040e330` tail fixups — and
`RaceCamCar`'s field comments name exactly the sources the original reads (`pos` = `+0x30/34/38`
via `FUN_0046d4a0`, `active` = `0x0040e370`, `alive` = `FUN_0046c7b0 == 1`, `dead_flag` =
`FUN_0046cbb0` out1). So the duplicate was deleted and H3 now supplies the **array**, filled from
the readers H1a/H1b made live. More faithful, not merely permitted.

The lint then flagged `FillCamCars` as a second body, because it binds a function to the first RVA
token in the preceding comment and the explanation block led with `0x0040e180`. Reworded; the trap
is now documented in the file.

Final: `rva-lint` **122 known, NEW=0**.

## 4. What is NOT claimed

- **`H3-GATEFIRE` was not built.** `PREREG_H3.md` §2 called it "the one that matters" — a
  default-OFF counter on `FUN_00414a70 == 2` and `FUN_004148b0 != 0 && FUN_00416060 != 0`,
  confirming they approach the original's 36 / 64 calls. It is not measured, so **nothing is
  claimed about whether U-9186's branches now fire.** This is the next leg.
- `H3-INERT` being inert is consistent with that: `ControlStep` still hardcodes `mode = 0`
  (`AiStandalone.cpp:844`), so the branches cannot fire regardless of what `0x008989b0` holds.
  H3 removes the *input* blocker, not the stub.
- `H3-KNOBOFF` / `H3-NOREG-E` / `H3-NOREG-B` were not run: the gates dump is a different schema
  from the AI stepdump those scorers take. `H3-INERT` (arms differ only in the four output columns)
  is weaker evidence in the same direction, and is not a substitute.
- Magnitudes are **not** bit-identical to the original: `std::sqrt` stands in for `FUN_004c3ac0`'s
  RW fast-sqrt, the same stand-in `AiStandalone.cpp:770` already uses. No C-level follows.

## 5. Next

1. Build `H3-GATEFIRE` and re-run — the measurement this leg exists to enable.
2. Run `H3-KNOBOFF` / `-NOREG` properly, with a stepdump arm.
3. Only then consider wiring the `FUN_00416250` branches, which is a separate registered leg.
