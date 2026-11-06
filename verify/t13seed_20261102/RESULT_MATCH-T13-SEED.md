# RESULT MATCH-T13-SEED (2026-11-02)

Verdict: **DECISION.** The knob is built, default OFF, and the default gates are unchanged (G-CTRL 8284be46 / ba407a16 / 31fe043e,
B 1d5bf4f9 x3 = `merge_train.json` refs). The evidence answers the PREREG question, but the answer is **neither 0.75 nor 1.0**.
On the three rule-10 courses the original's seed factor is `f = 1.0 - DAT_0089a360 * 0.1`, with `DAT_0089a360` set per race by
`FUN_00414060` from the championship difficulty table. In the three natural rule-10 events that table gives d = 6 / 4 / 7
(f = 0.4 / 0.6 / 0.3). The exe has no port of `FUN_00414060` and holds `DAT_0089a360 = 2.5` as a constant, so neither knob state
reproduces those events. Recommendation: keep the knob OFF and port the `DAT_0089a360` producer first (decision text below).

PREREG `verify/t13seed_20261102/PREREG_MATCH-T13-SEED.md` (abf2157aa, committed alone) and amendment 1
`PREREG_MATCH-T13-SEED_amend1.md` (8088431c0, committed alone after the four PREREG O arms and before every added arm).

## Change (default OFF, both targets)

| file | change |
|---|---|
| `mashedmod/src/mashed_re/Match/MatchS03_004173a0.cpp` (new) | the matched source `re/match/s03/h1_004173a0.c` is the one body of 0x004173a0 (`Float89a360Get`), both targets. Carries the asi install `RH_ScopedInstall(Float89a360Get, 0x004173a0)` |
| `mashedmod/src/mashed_re/Util/PromoLoop_round35.cpp` | old asi-only `Float89a360Get` body removed (pointer comment left) |
| `mashedmod/{asi,exe}_sources.rsp` | `Match\MatchS03_004173a0.cpp` added |
| `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp` | both `Rule10Seed` call sites (`StartRound` "round", `SetRaceRule` "setrule") go through `Rule10SeedCall`. Knob unset: difficulty `0.f` (unchanged). `MASHED_RULE10_SEED_DIFF=1`: `Float89a360Get()`. `MASHED_RULE10_SEED_LOG=1` (or the knob): one `[rule10-seed]` line per call in `mashed_re.log` |
| `re/frida/scenario_launch.py` | read-only extension of the `FUN_004046a0` oracle record (entry `DAT_00644158`, entry `DAT_0089a360` raw, exit timer raw, `DAT_005ea0ac`, 0x90 bytes at 0x00636b80). `T13SEED_INIT_DIFF_TABLE=1` calls `FUN_00414120` once at the menu (CONTRIVED, amendment 1) |

Match status of 0x004173a0 on this branch: `match.py re/match/s03/h1_004173a0.c --toolchain ddk --bytes` = 100.0% 2/2, 7 B,
0 differing unmasked bytes, BYTE-IDENTICAL (flags `/O2 /GS /Ob1`). `relocheck.py` = OK checked=0 unchecked=1 (`_DAT_0089a360`: the
source declares a plain `extern float`, not an `MGLOBAL`; the image bytes at 0x004173a0 are `D9 05 60 A3 89 00 C3`, operand
0x0089a360, and the wrapper binds `DAT_0089a360` to `*(float*)0x0089a360u`). exe callers: 1 E8 site (`Rule10SeedCall`, 0x0006dbb8 in
the `/MAP` image of this build, `_Float89a360Get` 0x0018bc90 in MatchS03_004173a0.obj), reached only with the knob ON.

Build: worktree, `MASHED_NO_DEPLOY=1`, `=== Build OK ===` both targets, `[rva-lint] ... allowlisted=2 NEW=0`, `[asm-lint] OK`.

## The image: who writes `DAT_0089a360`

Byte scan of `MASHED.exe.unpatched` for operand 0x0089a360: **one store**, `fstp [0x0089a360]` at `0x004140f4` in `FUN_00414060`
(called from `Race::Tick` at `0x0040ffc7`, before the seed calls at `0x00410071` and `0x00410214`, both in a switch on
`DAT_007f0fd0 - 4`). Readers: `0x004141a0`, `0x004148c3`, `0x00415335`, `0x004158ae`, `0x00417370`, `0x004173a0`, `0x004173b0`,
`0x004179d4`, `0x00417a03`, `0x00417b24`.

`FUN_00414060` (disassembly 0x00414060..0x00414119):
- `v = (float)DAT_0089a384[FUN_00413fa0()] + DAT_0089a37c * 0.5` (`0x00414066`..`0x0041407b`, 0.5 at `0x005cc32c`)
- if `FUN_0042f6a0()` (`DAT_0067e9fc`) == 6: `v = (float)FUN_00431d80() * 2.5` (`DAT_0067ea7c`, 2.5 at `0x005cd088`; `0x00414084`)
- if `FUN_0042f6a0()` == 10: `v = (float)FUN_0042fe80() * 2.5` (`DAT_0067ea90`; `0x004140a5`)
- clamp: `v <= 0` -> 0 (`0x004140c3`), `v >= 10` -> 10 (`0x004140da`, 10.0 at `0x005cc55c`); store `0x004140f4`; the same function
  writes `0xbf800000` (-1.0) to `0x0089a870..7c` and -1 to `0x0089a364`.

`FUN_00413fa0` = `DAT_0067f17c * 3` + 1 when mode == 4, + 2 when mode == 5, else + 0 (`0x00413fa1`..`0x00413fcf`;
`FUN_00430790` = `DAT_0067f17c`). `DAT_0089a384[0..38]` is copied from `0x005f2a70` by `FUN_00414120` (`0x00414140`..`0x0041416d`),
which also zeroes `DAT_0089a37c`, `DAT_0089a378`, `DAT_0089a380`. `0x005f2a70` = `1,1,3, 1,2,5, 3,4,3, 3,3,5, 2,3,5, 3,4,6, 3,4,6,
4,9,7, 4,5,7, 4,8,8, 4,8,9, 5,7,10, 5,7,10` (13 rows x modes 3/4/5). `FUN_00414120` callers: `0x0043d3d0` (`FUN_0043d2a0`) and
`0x0043fead` (`FUN_0043dfd0`, championship race launch). `DAT_0089a37c` is accumulated in `FUN_00417750`
(`0x00417770`..`0x0041777e`: `+= DAT_0089a380 / (float)FUN_004241b0()` when that is nonzero). `DAT_0067ea90` writers: `0x00431d10`,
`0x0043b254` (1), `0x0043b26d` (4), `0x004402fd` / `0x00440305` (dec, floor 1), `0x00440859` (inc, cap 4).

Rule-10 events (`kCupEvent`, high word 0x4 at `0x005f65c8`, `Race/RaceModes.cpp`): row 2 mode 4, row 5 mode 5, row 12 mode 4. The
launcher's rows 2 / 5 / 12 load courses 0x27 / 0xb / 0x1a (measured: `DAT_00644158` at seed entry, table below). Their
`0x005f2a70` entries (row*3 + modeOff) are 4 / 6 / 7.

## Arms (original, private root `D:\MashedWT\MATCH-T13-SEED_root`, lock- and test-slot-wrapped)

Scored per call: `seeds_scored.txt` (this folder). First-call summary:

| arm | args | course | `DAT_0089a360` at entry (all calls) | timer bits | bonus bits | calls | vs prediction |
|---|---|---|---|---|---|---|---|
| O_SANDS | t5 m3 r10 | 0xb | `00000000` | 17.5 `418c0000` | 4.65 `4094cccd` | 3 | f = 1.0 on 0 |
| O_CITY | t12 m3 r10 | 0x1a | `00000000` | 20 `41a00000` | 6.3 `40c9999a` | 4 | f = 1.0 on 0 |
| O_NEU | t2 m3 r10 | 0x27 | `00000000` | 19 `41980000` | 4.1 `40833333` | 2 | f = 1.0 on 0 |
| O_TRAIN | t0 m3 r10 | 0x1e | `00000000` | 30 `41f00000` | - | 3 | default branch, control |
| OEV_SANDS | t5 m5 r10 + table init | 0xb | `40c00000` (6.0) | 15.7 `417b3333` | 4.35 `408b3333` | 2 | **bit-equal** |
| OEV_NEU | t2 m4 r10 + table init | 0x27 | `40800000` (4.0) | 17.8 `418e6666` | 3.9 `40799999` | 2 | **bit-equal** |
| OEV_CITY | t12 m4 r10 + table init | 0x1a | `40e00000` (7.0) | 17.9 `418f3333` | 5.95 `40be6667` | 4 | **bit-equal** (incl. 2 segment-end reseeds) |
| O10_SANDS / O10_CITY / O10_NEU | m10 r10 | - | - | - | - | **0** | not measured: no `FUN_004046a0` call |

- **The seed follows the live `DAT_0089a360` (H1-live): PASS** on every recorded call. The O arms read 0 because the launcher
  bypasses `FUN_0043dfd0` (no `FUN_00414120`, so `DAT_0089a384[]` is all zero: `FUN_00414060` default branch -> 0). With the
  table initialised as the championship launch does it, `FUN_00414060` gives 6 / 4 / 7 and the seed is bit-equal to the
  formula on those values. The `[init-diff-table]` lines confirm the table, `DAT_0089a37c = 0` and `DAT_0067f17c` / mode as set.
- Mode 10 (quick race, where `FUN_00414060` gives `DAT_0067ea90 * 2.5` = 2.5): in all three arms the race ran with one SegmentCheck
  (timer 0) that ended the segment, and `FUN_004046a0` was never called. Which branch of `Race::Tick` skips the seed in mode 10 is
  [UNCERTAIN] (the two call sites are cases of the `DAT_007f0fd0 - 4` switch at `0x0041004a` / `0x004101f7`; the gate before
  them was not traced). O10_SANDS and O10_NEU first died after `[agent] ready` with no dump (the two dumps in CrashDumps date from
  23:43 / 23:45 the day before), and the single re-run reached the race.
- PREREG H1 (factor 0.75 = 2.5) is **refuted** on these courses as a constant: no recorded original call read 2.5.
  PREREG H0 (factor 1.0) holds only on the launcher's uninitialised-table path, not in a championship.

## Arms (port, `mashed_re.exe` sha256/16 `28bcff6dd6aa4de9`, `MASHED_ROUND_RULE=10`, 2400 frames)

| arm | course | knob | `0x0089a360` live | timer bits | bonus bits |
|---|---|---|---|---|---|
| P_OFF_SANDS / P_ON_SANDS | 11 | off / on | `40200000` | 17.5 `418c0000` / 16.75 `41860000` | `4094cccd` / 4.525 `4090cccd` |
| P_OFF_CITY / P_ON_CITY | 26 | off / on | `40200000` | 20 `41a00000` / 19.25 `419a0000` | `40c9999a` / 6.175 `40c5999a` |
| P_OFF_NEU / P_ON_NEU | 39 | off / on | `40200000` | 19 `41980000` / 18.25 `41920000` | `40833333` / 3.975 `407e6666` |
| P_OFF_TRAIN / P_ON_TRAIN | 30 | off / on | `40200000` | 30 `41f00000` / 30 `41f00000` | 0 / 0 |

Two calls per arm (`round`, then `setrule`), same value on both. The exe's `0x0089a360` is 2.5 at both calls in every arm (the
AI race reset in `TrackRenderer.cpp` writes the constant). The port is bit-equal to the original's formula: P_OFF equals the O
arms (d = 0) and P_ON equals the amendment's d = 2.5 predictions. So the arithmetic is right in both knob states. What differs from a
championship is the input `DAT_0089a360`: 2.5 (knob ON) or 0 (OFF) against the original's 6 / 4 / 7.

Distance to the natural events (timer): Sands 15.7 vs OFF 17.5 / ON 16.75. Neustein 17.8 vs 19.0 / 18.25. City 17.9 vs 20.0 / 19.25.
ON is closer on all three, and neither state is faithful.

## Gates (knob-OFF exe, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`)

| run | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs `merge_train.json` |
|---|---|---|---|---|
| 1 | `28bcff6dd6aa4de9` (source 0701537b6; later commits touch only `re/frida/` and `verify/`) | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

H_DEF self-exited in 134 s (13962 step lines), B arms 15 / 16 / 26 s (762 lines each), exit 0 for all four.

## -Full

Not run here. The branch touches `D3d9Render/TrackRenderer.cpp`, `Match/` and `Util/` (the knob is OFF, so the default path
changes only by the call through `Rule10SeedCall`, which passes `0.f` as before). The train's `-Full` pass is the train's leg.

## Decision for the lead to file (L-n)

Question: rule-10 seed factor on courses 0xb / 0x1a / 0x27 (`FUN_004046a0`, `f = 1 - DAT_0089a360 * 0.1`). Options:
(a) flip `MASHED_RULE10_SEED_DIFF` default ON now (f = 0.75 from the exe's constant 2.5; closer to the natural events than today,
not faithful: the original gives f = 0.4 / 0.6 / 0.3 in the three championship rule-10 events);
(b) keep `0.f` (f = 1.0; matches the original only when `DAT_0089a384[]` is uninitialised, which a championship never is);
(c) keep the knob OFF and first port the `DAT_0089a360` producer: `FUN_00414060` (0x00414060, the per-mode difficulty),
`FUN_00414120` (0x00414120, table init from 0x005f2a70 at championship launch) and the `DAT_0089a37c` term in `FUN_00417750`
(0x00417750). Then re-run the OEV arms against P_ON and decide the flip on bit-equality.
Evidence: this file. **Recommendation: (c).** The same constant 2.5 also feeds the exe's AI readers of `DAT_0089a360`
(`AiStandalone.cpp`, `AiPreTick.cpp`, the 0x00417370 / 0x004177b0 rows), so the producer port affects AI difficulty in modes 3 / 4 / 5
too. That port is a behaviour change of its own (knob + PREREG). U-D3-DIFF360's "writer not traced" is answered by `0x004140f4`.

## C-level

No `re-classify` in this step. 0x004173a0 is matched (100%, relocheck OK with one unchecked name) and swapped in both targets with
1 exe caller, reached only with the knob ON. Under L-68 a call the default build does not execute leaves it below C4 until the
knob's fate is decided.

## Raw data (`D:/MashedWT/MATCH-T13-SEED_data`, not committed)

`raw_sha256.txt` in this folder lists the SHA-256 of every cited raw file (39 files: oracle json and run logs for the 10 O arms,
8 port logs, the six gate files, the exe, build and driver logs). Gate files:

```
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K0.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K1.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K26.step.csv
28bcff6dd6aa4de9a2fdad4e01f87fb683a0ac5e329f072c9ae6a59b66698792  exe_g1/mashed_re.exe
```

Drivers (copied here): `o_arms.sh`, `o_arms2.sh`, `p_arms.py`, `gates_parms.sh`, `seedtab.py`.
