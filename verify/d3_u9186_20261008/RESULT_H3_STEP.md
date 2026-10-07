# RESULT — U-9186 leg H3, stepdump arm + the `H3-GATEFIRE` withdrawal

Date 2026-10-08. Pre-registration: `AMEND_H3.md` (written before the run), amending
`PREREG_H3.md` §2. **RAN.** No C-level, nothing default-ON, `original/` untouched, `.asi`
code path untouched. Raw: `H3STEP_GATES.txt`, `H3STEP_RUN.txt`, `H3step.csv`, `H3step_r2.csv`.

## 0. Verdict first

**The three gates `RESULT_H3.md` §4 left unrun all PASS: H3's default build is unchanged.**
**`H3-GATEFIRE` is WITHDRAWN** — it is a port leg, not an instrumentation leg, and nothing is
claimed for it. The two branches it would count are **not equally blocked**; the census is §3.

| gate | verdict | figure |
|---|---|---|
| `H3-KNOBOFF` (primary) | **PASS** | `H1step.csv` vs `H3step.csv`: **IDENTICAL over the whole overlap**, 19,418 shared keys, `R-PREFIX = 13498` |
| `H3-KNOBOFF-LONG` | **PASS** | vs `E2off_1.csv` `--common-cols`: identical, 19,418 keys. **No column dropped** — no schema drift since E2 |
| `H3-NOREG-E` | **PASS** | 3/3 cars, all six digits equal to the committed baseline |
| `H3-NOREG-B` | **PASS (unchanged)** | `v1`/`v3` pass; `v2` fails the same **5** bands with the same numbers as baseline |
| `H3-DET` (stepdump arm) | **PASS** | `H3step` vs `H3step_r2`: identical over the whole overlap, 19,418 keys |
| `G-BANDS-UNEDITED` | **PASS** | `git status --porcelain` + `git diff --stat` empty on all three scorers, before and after |
| `H3-GATEFIRE` | **WITHDRAWN** | not buildable as registered; §3 |

## 1. The knob-off gates

`det_prefix.py` finds zero disagreeing cells over all 19,418 shared `(frame,seq,v)` keys. Stronger
than that: **all four captures share one SHA-256**, 11,496,941 bytes each —

```
86b7b2bbc88f808cd0f853c744a9a51a7b0bbd11162117bd955cba5043981457
  verify/d3_consumer_20261007/E2off_1.csv     (E2 build, 2026-10-07)
  verify/d3_u9186_20261008/H1step.csv         (pre-H3 build)
  verify/d3_u9186_20261008/H3step.csv         (HEAD, this run)
  verify/d3_u9186_20261008/H3step_r2.csv      (HEAD, repeat)
```

So the default build is bit-identical across three builds and two repeats, and H3's exe-side edit
is inert on the default path.

**`H3step.csv` / `H3step_r2.csv` are deliberately NOT committed**: they hash to an
already-tracked blob, so committing them would add 23 MB carrying no information the hash above
does not. They are reproducible with `run_h3.ps1 -Step`. This is the opposite case from
original-side captures, which are not bit-reproducible and must always be force-added.

That edit was real, not nominal: commit `1560986e` added `Race/SpectatorDistances.cpp` to
`exe_sources.rsp` and +42 lines to `D3d9Render/TrackRenderer.cpp`, including a
`RaceComputeDistancesTick` call site that **executes every frame in the default build** and is inert
only because `SpectatorDistances.cpp:206`'s `static` `getenv("MASHED_REFDIST")` misses. This is
therefore **not** the near-tautology H1a's `H1-KNOBOFF` was (`RESULT_H1.md`: `H1-CALLERS = 0`, the
subject had no standalone caller at all). Here the subject is called 14,400 times and still changes
nothing.

`H3-NOREG-E` and `H3-NOREG-B` are **implied** by that identity — identical input rows cannot yield
different derived statistics — but both were run rather than argued:

```
v1 launch=1426.4 ft_median_m0=2550.6     v2 launch=2053.0 ft_median_m0=2053.0
v3 launch=2055.2 ft_median_m0=2278.2                       -> 3/3 (e) PASS
```

all six equal to `PREREG_CARCAR.md:72`. For (b), `v2`'s five failing bands are reported rather than
waived, and they are the **same five with the same numbers** as the committed baseline and as the
pre-H3 capture — `c0_distinct=39`, `c1_distinct=75`, `steer_distinct=113`, `c1_median=5.5`,
`abs_steer_median=44.5`. Re-running `ai_ctrl_window.py --check` on `H1step.csv` reproduces that
line character-for-character. The registered threshold is "the same 5 bands", so this is PASS as
*unchanged*; it is **not** a claim that criterion (b) is satisfied. Criterion (b) is still failing
on `v2`, which is what U-9186 exists to fix.

## 2. Determinism

`H3-DET` **PASS**: `H3step.csv` vs `H3step_r2.csv` is identical over all 19,418 shared keys,
`R-PREFIX = 13498`. A failure here would have invalidated §1 rather than indicted H3, since F2
established the scenario is bit-reproducible under
`MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400`; three captures taken today
(`H3step`, `H3step_r2`, and the pre-H3 `H1step`) are now mutually identical, which re-confirms it
on a third build.

## 3. `H3-GATEFIRE` — withdrawn, with the blocker measured

`PREREG_H3.md` §2 called it "the one that matters": a default-OFF counter on `FUN_00414a70 == 2`
and `FUN_004148b0 != 0 && FUN_00416060 != 0`, against the original's **36 / 64** calls over the
220-call window (`verify/d3_elim_20261003/RESULT_STEP2.md:106`, `:108`, `:178`).

**The registration assumed instrumentation. There is nothing to instrument**: none of the three
predicates is on the standalone's code path, and `ControlStep` calls none of them —
`AiStandalone.cpp:844` is a hardcoded `int mode = 0;` with the targeting chain elided at source
level, not stubs returning 0. Full table in `AMEND_H3.md` §3; the short form is `FUN_00414a70` has
**no body anywhere** (`hooks.csv:645`, `C2,mapped`, empty impl), and `FUN_004148b0` /
`FUN_00416060` have bodies in `.asi`-only TUs saturated with absolute reads into the original image.

**The two branches are not equally blocked** (static census, `AMEND_H3.md` §3.1):

- The **64-branch's LOS half is already live**. `TrackRenderer.cpp:359` loads the `.AI` tile grid
  into `0x007f1a9c` via `Ai::AiData_LoadInto`, and `AiStandalone.cpp:282-288` — exe-linked —
  already implements `FUN_00416060`'s exact test (`word [0x007f1a9c + cell*2]`, `0 < s < 0x200`,
  sub-cell `in {0,3}`). What remains is `FUN_004148b0`'s own substrate: `0x0089a4c4`,
  `0x00442cc0` and `0x0040e470` have exe-side references; **`0x0089a4c8` and `0x005f2dd8` have
  none**. `0x005f2dd8` is the `0x005f2770` class — a `.data` table the standalone never loads,
  extractable from `MASHED.exe.unpatched` exactly as `H3-CONSTS` did for the four floats.
- The **36-branch is a port from zero**: `FUN_00414a70` plus its unported callee `FUN_00414300`.

**[UNCERTAIN]** — the census is *static*. An exe-side reference proves the address is addressed by
compiled code, **not** that the value is live during a race. The registered risk at
`RESULT_STEP2.md:223` — that `LeaderTimer`'s rank/progress reads are `.bss` zeros standalone,
making a wiring inert rather than correct — is **narrowed, not closed**. A runtime substrate census
must be the first gate of whatever leg picks this up, or the counter reports a number with no
meaning: the `_abs`-column failure mode `RESULT_H1.md` recorded, and the shape of
"schedule-derived counts are not evidence".

This withdrawal changes no H3 verdict. `RESULT_H3.md` already stated that nothing is claimed about
whether U-9186's branches fire, and `H3-INERT` remains expected rather than defective.

## 4. What is still NOT claimed

- Whether U-9186's branches fire standalone. Unmeasured, and now explicitly a port leg.
- Any correctness claim for the H3 producer beyond `RESULT_H3.md`'s `H3-WROTE`, which stands as a
  registered-threshold failure (74.9963%) with the corrected gate-passing denominator
  (40,491/40,494 = 99.9926%) alongside, and 3 named frame-0 residual rows.
- Criterion (b) itself. `v2` still fails five bands; §1 shows only that H3 did not move them.

## 5. Next

1. A user decision on `H3-GATEFIRE`'s scope — the 64-branch alone (cheaper, better understood,
   needs a runtime substrate census first) or both branches.
2. **H2** — `MASHED_SLOTSTATE_SEED` default-ON, already priced by `H1-ALL` at 75% of rows.
3. Independent of this lane: **U-9191** item (b), **U-9195**, the `0x00409b0e` jumptable.
