# RESULT D4-FX-STATE7: state-7 `rec+0xcfc` hold and the car 1 emit gap, both explained (2026-10-27)

**Verdict: GREEN.** Both open items from `verify/fxrebase_20261027/RESULT_D4-FX-REBASE.md` are closed by measurement.
No behaviour change. The one source edit is a diagnostic colour census on the `MASHED_FX_DEBUG` line, and the default
gates are unchanged on this exe.

1. **`rec+0xcfc` in state 7.** In state 7 a car with `rec+0x10 != 0` (every eliminated car) still enters `FUN_0046e9e0`,
   but it jumps past the `+0xcfc` write. So its `+0xcfc` keeps the value it had when it was eliminated. The survivor
   (`rec+0x10 == 0`) rewrites `+0xcfc` on every substep from the zero block `0x007f19b8`, so its value is 0. An
   intervention rep that sets `+0xcfc = 1` on all four cars at the first state-7 frame shows this. The 3 eliminated cars
   hold 1 for all 13,001 state-7 frames and reproduce fA's car 1 FX signature to 3 digits. The survivor clears it
   1 frame later.
2. **Car 1 emits per call (0.91x).** The wheel-colour mix is not the lever. The port's per-car colour mix matches the
   original's within 6 calls per colour. The port's per-colour rates, weighted by the original's mix, give 44.8 car 1
   emits against the original's 44 (rep o1). The 42 vs 46..54 gap is Bernoulli noise on about 480 calls with
   p 0.05..0.7.

Branch `orch/D4-FX-STATE7` off `orch/integration` @ `725474f9c`. Private root `D:\MashedWT\D4-FX-STATE7_root`. Every game
run (2 original reps, 6 port arms) went through `proyectos_test_slot.py` and `mashed_lock.py run --label D4-FX-STATE7`.
The launchers killed only the PIDs they spawned. No U-9677 signature in any run. Exe built with `MASHED_NO_DEPLOY=1`
(sha256 `300be6731f07ff6d...`). Nothing deployed.

## 1. Static chain (decomp_pc.py, read-only slot `Mashed_pool10`; disassembly `D:/MashedWT/D4-FX-REBASE_data/dis_46e9e0.txt`)

- `FUN_0046e9e0` has one caller, `FUN_004709a0` (`decomp1.txt`), not `FUN_00470670` (A4). The REBASE candidate
  "`FUN_0046e9e0` sits under A4" is refuted. `FUN_004709a0`'s caller is `FUN_00470c70` (`decomp2.txt`).
- `FUN_004709a0` picks the block per car `local_18` = `0x007f1038 + slot*0x4c` and replaces it with `&DAT_007f19b8`
  when `rec+0x9f0 != 0`, when `rec+0x10 != 0`, or when `FUN_0040e350()` is not 6 / 0xb / 0xa. It calls
  `FUN_0046e9e0(param_1, block)` only when `rec+0x14 == 0 && (rec+0x4 != 0 || rec+0x10 == 1)` and `rec+0x9f0 != 0xff`.
  (Record fields read as `DAT_008815a0 + car*0xd04` + offset: `0x00881f90` = +0x9f0, `0x008815b0` = +0x10,
  `0x008815b4` = +0x14, `0x008815a4` = +0x4.)
- `FUN_0046e9e0`: `mov eax, [esi+0x10]` / `test` / `jne 0x0046ed62` at `0x0046eaab..0x0046eab3`. Taken, it skips both
  `+0xcfc` writes: `mov [esi+0xcfc], 0` at `0x0046eaf2`, then `mov [esi+0xcfc], 1` at `0x0046eb0c` when block bytes
  4 (`0x0046eafc`) and 5 (`0x0046eb03`) are both nonzero.

## 2. Probe (`orig_fx_state7.py`, matched recipe `--poke-ctrl-slots --statediff-out`, no pulses)

Probe F writes one row per `FUN_0040ca00` call with, per car: `+0xcfc`, `+0x10`, `+0x14`, `+0x9f0`, `FUN_0046e9e0`
entries (car from ESI), hits at `0x0046eaf2`, and the block argument with its bytes 4 / 5. Probe E is a colour x skid
histogram on `FUN_0040c690` (args 3 / 4 / 5) with `FUN_0048cf90` emit attribution. Probe A is the committed counter.
Rep o1 is plain. Rep p1 adds `--poke-cfc`. Summaries: `state7_score.txt`, `o1_cfc_check.txt`, `p1_cfc_check.txt`,
`msd_car0.txt`.

**o1 (no intervention), `state7_score.txt`.** The block `0x007f19b8` bytes 4 / 5 are (0, 0) on all 13,754 rows.
- States 3 / 4 / 5: all four cars enter on the zero block, 2 entries per frame, every entry reaches `0x0046eaf2`.
- State 6 (220 frames): every car is alive with `+0x10 = 0`, on its slot block. Each entry reaches `0x0046eaf2`
  (440..442 per car). `+0xcfc != 0` on 16 / 5 / 1 frames for cars 1 / 2 / 3, matching the slot-block (0xff, 0xff)
  frames (16 / 5 / 1).
- State 7 (13,093 frames): cars 0 / 1 / 2 have `+0x4 = 0`, `+0x10 = 1`, 26,186 entries each on the zero block, and
  **0 hits at `0x0046eaf2`**. Car 3 has `+0x4 = 1`, `+0x10 = 0`, 26,186 entries and **26,186 hits**. `+0x14 = 0` and
  `+0x9f0 = 0` on every frame for every car. Every car's `+0xcfc` was 0 on the last state-6 frame, so all stay 0.

**p1 (`+0xcfc := 1` on cars 0..3 at the first state-7 frame).** State 6 has the same per-car counts and block-byte
histograms as o1.
- Cars 0 / 1 / 2 (`+0x10 = 1`): `+0xcfc != 0` on 13,001 of 13,001 state-7 frames, 0 hits at `0x0046eaf2`.
- Car 3 (`+0x10 = 0`): `+0xcfc` is 1 on the poke frame, then 0 from the next frame (`[0, 1, 0, 0]`, first change at +1).
- FX after t = 20 s (`p1_cfc_check.txt`): cars 0 / 1 / 2 have 0 non-skid calls and skid-frame shares
  **0.7020 / 0.7006 / 0.6988**. fA's car 1 had 0 non-skid calls and **0.7008** (`verify/fxrebase_20261027/fa_cfc_check.txt`).
  o1 has 0 calls on every car in the same window (`o1_cfc_check.txt`).
- Car 0's own record dump agrees (`msd_car0.txt`): in p1, `+0x4 = 0 +0x10 = 1 +0xcfc = 1` from its elimination
  frame to the end. In fA / fA2 / fA3 / r1 / o1 it is `+0x4 = 0 +0x10 = 1 +0xcfc = 0`.

**Answer to the REBASE UNCERTAIN.** A state-7 `+0xcfc != 0` needs a car that `FUN_0046e9e0` does not rewrite. The
`0x0046eaf2` write ran on 26,186 of 26,186 (o1) and 26,002 of 26,002 (p1) entries of the `+0x10 = 0` car, from a
(0, 0) block. So fA's car 1 held `+0xcfc = 1` from a frame where `FUN_0046e9e0` stopped rewriting it. The only measured
state with that property is `+0x10 = 1`, the eliminated state. The other no-rewrite routes in `FUN_004709a0`
(`+0x14 != 0`, `+0x9f0 == 0xff`, `+0x4 == 0` with `+0x10 != 1`) occurred on 0 frames in o1 and p1. Car 1 is the car
with the most both-pedals frames in state 6 (16 of 220, against 5 and 1).
- [UNCERTAIN] fA's car 1 alive word and `+0x10` were not recorded. fA had probe A only, and its `.msd` is car 0's
  record. `RESULT_D4-FX-SLOT2.md` section 3 calls fA's car 1 "the winner". That conflicts with the measured mechanism,
  since a car with `+0x10 = 0` cannot keep `+0xcfc != 0` in state 7. Missing evidence: one fA-like rep with probe F.
- fA's round lasted longer: car 0 eliminated at msd frame 1510 against 1039..1114 in the 5 other captures.
- SLOT2's open question "how the state-7 winner reaches the 62,500 speed gate" is moot. Those calls take the
  `+0xcfc != 0` branch (`0x0040cabe jne 0x0040cb42`), which has no speed test.

**Port.** `BodyOrientationIntegrate.cpp:254-255` writes `+0xcfc` on every call. Nothing in `mashedmod/src` writes
`rec+0x10` (`BodyOrientationIntegrate.cpp:64`). So the port has no `0x0046eab3` skip and cannot hold an eliminated
car's `+0xcfc`. On this recipe that makes no difference. In the 6 unpoked matched captures (r1..r3, fA2, fA3, o1), no car has more
than 491 FX calls in the whole run, while a held `+0xcfc = 1` gives about 26,000 (p1). Probe F reads `+0xcfc = 0` at
elimination on every car in o1 and p1. It matters only on a run where a car is
eliminated on a both-pedals frame. Then the original gives that car about 0.7 x 3 FX calls per frame for the rest of
the race and the port gives none. [UNCERTAIN] how often that happens on the default recipe: 1 of 8 original runs (fA)
so far.

## 3. Wheel-colour mix (`colour_score.txt`)

Port = `run_arms.py H_FXD` on this exe with the new `[sN col]` census. Original = o1 probe E. Per colour:
calls non-skid / skid, emits non-skid / skid.

| car | colour | port calls | port emits | o1 calls | o1 emits |
|---|---|---|---|---|---|
| 0 | `0xffc88080` | 308 / 0 | 17 / 0 | 307 / 0 | 16 / 0 |
| 1 | `0xffc88080` | 343 / 109 | 11 / 13 | 343 / 115 | 12 / 10 |
| 1 | `0xffb48080` | 7 / 23 | 2 / 16 | 7 / 26 | 3 / 19 |
| 2 | `0xffc88080` | 399 / 51 | 18 / 5 | 397 / 48 | 19 / 3 |
| 3 | `0xffc88080` | 143 / 236 | 6 / 21 | 143 / 233 | 9 / 18 |
| 3 | `0xffb48080` | 0 / 31 | 0 / 23 | 0 / 31 | 0 / 25 |

- The port fills `+0x1f0` with the same colours, on the same wheels and calls, within 6 calls per cell. No
  `0xff968080` (id 6) and no third colour on either side. `FUN_0040c690` thresholds per call (`0x0040c6e6..0x0040c734`):
  `0xffc88080` 0.05 non-skid [0x005cc9a0] / 0.1 skid [0x005cc56c], any other colour 0.4 [0x005ccac0] / 0.7 [0x005cc340].
- Car 1 totals: port 42 emits / 482 calls (0.087), o1 44 / 491 (0.090). The port's per-colour rates weighted by o1's
  call mix give 44.8. At the threshold table, the port's 482 calls have expectation 46.95 emits with SD 5.71 (sum of
  n·p·(1-p)). 42 is -0.87 SD. o1's expectation is 49.65 and its 44 is -0.99 SD.
- With o1 added, the original's car 1 band is 6 reps: 110.8 (o1) .. 138.1 emits per 1000 active frames. The port's
  106.6 is 0.96x of the lowest rep, against 0.91x before.
- The REBASE "next lever" (D4-FX-GATES section 1 UNCERTAIN, "the port has no colour histogram") is closed. It has one
  now, and it matches.

## 4. Change, gates, trackers

- Source, diagnostic only: `TrackRenderer.h` (`fx_col_key_`, `fx_col_n_`) and `TrackRenderer.cpp`
  (`EmitCarFxOrigGates` census plus one `[sN col]` line inside the existing `MASHED_FX_DEBUG` block). These are not
  read back and do not touch the private roll stream. No knob and no PREREG, since there is no behaviour change.
- Gates on exe `300be673` (`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`): G-CTRL `5943c5f1 / 364350ae / 52052890`,
  B `3a03670c / e85ea6fb / 53778664`. These equal `merge_train.json` `gctrl.ref` / `b.ref`. H_FXD's step csv is the
  same `5943c5f1`.
- Files in this folder: `orig_fx_state7.py`, `run_reps.sh`, `state7_score.py`, `colour_score.py`, `msd_car0.py`, and
  the summaries `state7_score.txt`, `colour_score.txt`, `msd_car0.txt`, `o1_cfc_check.txt`, `p1_cfc_check.txt`. The
  arm runner is `verify/fxrebase_20261027/run_arms.py` with `A4_OUT` / `A4_ROOT` pointed here.
- Trackers: none touched. Two candidates for a `re-classify` row if the lead wants them tracked: (a) the port gap in
  section 2 (no `rec+0x10` skip at `0x0046eab3`, so an eliminated car's `+0xcfc` is not held), and (b) the fA
  "winner" conflict [UNCERTAIN].

## 5. Next

1. FX parity on this recipe needs no further step. All four cars match per colour, and the remaining gaps are roll noise.
2. Optional port step: a default-OFF knob that holds `+0xcfc` (skips the `BodyOrientationIntegrate.cpp:254` rewrite)
   for an eliminated car, citing `0x0046eaab..0x0046eab3`. It needs a recipe where a car is eliminated on a
   both-pedals frame to show any effect, because the matched recipe never produces one.
3. To close the fA [UNCERTAIN] directly: matched reps with probe F until one ends with a car eliminated at
   `+0xcfc = 1` (1 of 8 so far).

## Raw data (D:/MashedWT/D4-FX-STATE7_data, not committed), SHA-256

```
a587fada10f60d457cb122487195155768b5345fa76c6f56c457eda3322942d0 o1.A.json
6d7084d67511df92ec623659af4cd0e1e0144fbc9c7e3fd660c1f62e3dc20a9b o1.E.json
9d6f4d2def8467e99f9c163b413144382aa64cb7b54b4c4d299edd8bcc961b23 o1.F.rows.bin
a0546ff56061d895848228357c3699f39abf9f47f8d4026dde55fd1dc617aa79 o1.msd
087b39ad563d3607b8e3dffd9ad9062e98ecd3732082d3e105b455579270d977 p1.A.json
9250326bd4d6da89d2ec43b53cfb7d4b114b0b8119f3074b737e0928c1439ba0 p1.E.json
470702ee19acabbacde0272894744cf87fb90a489fe008c95fc109121083ede2 p1.F.rows.bin
ff76e3daf19cba0b74a7cdfeed348612b25392ca34f2d11dda1343c75701f281 p1.msd
6b0b4ce348bc14c106fedf2e0eba56263ba67015201f48f8cdd7af7939c554e3 o1_run.txt
97afa83492ee06d49667960d544f44315ea2a43a23ba1dd64975c5b6eb33b848 p1_run.txt
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 arms/H_DEF.ptrace.log
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms/H_FXD.step.csv
25bb9bff0ced96c17d518e225853c93575d1bb5bde4cec5529a0262ba3a2c0ef arms/H_FXD.fx_debug.txt
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d arms/B_DEF_K26.step.csv
b34cc2a222285c916f3ccf02ea53e2b56a691e1a0fc4a228a940e37d31d521d8 arms_run.txt   (H_DEF H_FXD B_DEF_K0 B_DEF_K1 B_DEF_K26; H_FXD wrote no fx_debug: root had no log\)
36918ff9b7cd8ec72a59c675785b465313e7fbc3c16d145d2f84027837ea3d37 arms_run2.txt  (H_FXD re-run with D:\MashedWT\D4-FX-STATE7_root\log present)
70deed380d2dd53b3f08bb1d32ade79ea5258269c28a7c6faa4761b2408b839f decomp1.txt    (decomp_pc.py 0x0046e9e0 0x00470670 0x0040c690 --callers)
62f7a7b1c92d383c20b81abd67a95c3dc22d562de8634e2937b81f7d1f9d1b9c decomp2.txt    (decomp_pc.py 0x004709a0 0x00470c70 --callers)
8968f423fb0681a1557499ccd0f40a18923f9174c0c263da5786862faf6768ca build1.log
b5ece79b6f3850d71781c1af57ea23e6d7ba38b3f67de0ab8d006123496062f3 D:/MashedWT/D4-FX-SLOT2_data/fA.json
a15c0ff7fb2792e9fff8e4a6c0e0adc77a130b99b9434a60d78c7019290f5d8c D:/MashedWT/D4-FX-SLOT2_data/fA.msd
bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e D:/MashedWT/D4-FX-STATE7_root/original/MASHED.exe.unpatched
```
