# D2 attempt 13, STEP 1 — RESULT

Pre-registration: [`PREREG_STEP1.md`](PREREG_STEP1.md), committed before any
reduction run and **not amended**. Reducer `re/tools/statediff/a13_l60.py`,
output `step1_l60.txt`. **No game was launched for this step** — `--mag-probe`
and `--fixup-probe` had already captured every channel it needs.

## The gates

| gate | bar | result | verdict |
|---|---|---|---|
| **G1** capture integrity + known-answer | all nine sites populated, `n>=1000` at `004686a9`, vector == record `+0x9b0`/`+0x9b8` on `>=99%` | 20000 rows, nine sites, **1424 of 1424 exact (1.0000)** | **PASS** |
| **G2** pairing, by count | report the distribution, truncate nothing | `004680fb`: 4/frame on 525, 0 on 899. `0046820f`: 4/frame on 541, 0 on 883. **16 of 1424 frames disagree** | reported |
| **G3** port channel known-answer | `act.l60 * act.speed == act.grip` to rel `<=1e-5` on `>=99%` | clamp ran on 1626 of 1627, **1626 of 1626 (1.0000)** | **PASS** |
| **G4** horizontal lateral legitimate | `fwdx²+fwdz² ∈ [0.99,1.01]` on `>=99%` **and** `n>=100` in band 100-150 | unit check **1448 of 1448 (1.0000)**; band 100-150 **n = 45** | **FAIL** |

> **G4 failed, so the registered decision rule D1 / D2 / D3 did NOT execute.
> The gate was not amended and no branch was taken.** The `s` table below is
> printed as an **observation** and is not a decision input.

**Why G4's `n>=100` bar was unmeetable on this arm, and that is a fact about the
bar.** §21.5 already measured that the original passes below 100 horizontal
**once per race, for 6-8 frames of 6658**. The 100-150 band holds 45 grounded
frames in a 2332-frame capture and no re-run of this scenario will raise it —
the bar was set without checking the reference's residency in the band it
governs (memory `reference-may-never-enter-the-band`). A future bar for this
band must be `n>=40` or the scenario must be changed; it is **not** amended
retroactively here.

## 1. The call-site attribution is VERIFIED, and U-9172's premise is refuted

From the disassembly, pre-registered before the run: `l_60`'s slot is **frame
−96**, it has **exactly four accesses** in all of A6a, and its one accumulate is

```
l_60 += mag@0x0046820f * [frame -228]          ; 0x00468220 / 0x0046822b
                         ^ written by 0x004680fb
```

**Those are exactly the two sites §21.9's `a8_l60.py` used.** U-9172's stated
suspicion — "the likely error is call-site misattribution" — is **refuted**,
and G1's known-answer self-check (1424 of 1424 exact, re-verified here rather
than inherited) says the return-address tagging that produced them is sound.

The **value** reproduces too, on a different frame pairing: this reducer gets
`l_60 = 268.587` at 100-150 where §21.9 got `285.243`. Same quantity, same
sites, 6% apart on 45 frames against 35.

## 2. The ORIGINAL's `l_60`, per band — n and median speed on every row

Grounded frames only, `+0x18c == 1.0` on **525 of 525** so `grip == l_60`.

| band | n | med speed | med `ld4` | med `le4` (capped) | **med `l_60`** | **med `l_60 × speed`** | wheels/frame |
|---|---:|---:|---:|---:|---:|---:|---:|
| 100-150 | **45** | **126.2** | 0.62572 | 125.747 | **268.587** | **30 785.1** | 4.0 |
| 150-260 | 71 | 193.0 | 0.36935 | 188.483 | 264.712 | 57 145.1 | 4.0 |
| 260-500 | 50 | 345.7 | 0.30092 | 343.817 | 381.285 | 136 087.1 | 4.0 |
| 500-1000 | 72 | 755.2 | 0.32815 | 744.374 | 808.633 | 619 007.1 | 4.0 |
| 1000-1500 | 74 | 1243.6 | 0.34218 | 1024.000 (cap) | 1 515.331 | 1 879 055.8 | 4.0 |
| 1500-2000 | 84 | 1742.8 | 0.59653 | 1024.000 (cap) | 1 936.219 | 3 357 068.6 | 4.0 |

## 3. The PORT's `l_60`, read DIRECTLY — and the cross-side ratio

`act.l60`, `act.grip`, `act.kvel` and `act.arm` are logged per frame by
`Integrate2.cpp:653-739` / `VehiclePhysicsRun.cpp:1313-1316`. G3 confirms
`act.grip == act.l60 * act.speed` on 1626 of 1626. **Nothing is reconstructed.**

| band | n | med speed | med `act.l60` | med `act.grip` | med `act.kVel` | arm hi/lo/none | clamp ran | med `s` |
|---|---:|---:|---:|---:|---:|---|---:|---:|
| 40-70 | 269 | 47.8 | 38.929 | 1 786.8 | 0.945473 | 0 / 269 / 0 | 269 | 0.651439 |
| 70-100 | 38 | 83.5 | 281.197 | 27 033.6 | 0.199269 | 8 / 30 / 0 | 38 | 0.874589 |
| **100-150** | **19** | **115.3** | **124.737** | **15 042.0** | **0.540954** | 5 / 14 / 0 | 19 | 0.083887 |
| 150-260 | 13 | 194.9 | 166.193 | 31 363.4 | 0.198908 | 6 / 6 / 1 | 12 | 0.127042 |
| 260-500 | 14 | 349.8 | 213.205 | 78 699.1 | 0.198426 | 14 / 0 / 0 | 14 | 0.031351 |
| 500-1000 | 15 | 735.8 | 548.309 | 403 435.7 | 0.191931 | 15 / 0 / 0 | 15 | 0.011181 |
| 1000-1500 | 19 | 1275.5 | 1 281.922 | 1 636 959.5 | 0.167261 | 19 / 0 / 0 | 19 | 0.110146 |
| 1500-2000 | 20 | 1676.5 | 1 751.259 | 2 948 873.4 | 0.141023 | 20 / 0 / 0 | 20 | 0.200669 |

| band | ORIG `l_60` (n, med spd) | PORT `l_60` (n, med spd) | **ratio O/P** |
|---|---|---|---:|
| 100-150 | 268.587 (45, 126.2) | 124.737 (19, 115.3) | **2.15x** |
| 150-260 | 264.712 (71, 193.0) | 166.193 (13, 194.9) | **1.59x** |
| 260-500 | 381.285 (50, 345.7) | 213.205 (14, 349.8) | **1.79x** |
| 500-1000 | 808.633 (72, 755.2) | 548.309 (15, 735.8) | **1.47x** |
| 1000-1500 | 1 515.331 (74, 1243.6) | 1 281.922 (19, 1275.5) | **1.18x** |
| 1500-2000 | 1 936.219 (84, 1742.8) | 1 751.259 (20, 1676.5) | **1.11x** |

> **`l_60` DOES diverge, with a verified attribution on both sides: 2.15x short
> on the port at 100-150, falling monotonically to 1.11x at 1500-2000.** That is
> the measurement U-9172 asked for, and it is consistent with §21.9's 1.84x
> (which used the port's reconstructed 141.24 rather than its logged 124.737).

## 4. §25.3's "`l_60 >= 79 240`" is WITHDRAWN — the original is NOT on the HIGH arm

At 100-150 the ORIGINAL's `grip × |vel|` is **30 785.1** (n=45, median speed
126.2), against the knee `_DAT_005ce9fc = 32768`. **It is BELOW the knee, so the
original takes the LOW arm**, where

```
k = max((32768 - G) * 2^-15, 0.1)  =  max(0.0605, 0.1)  =  0.1  exactly, on the floor
```

and `k` **can never be 0**. §25.3's chain was: "the clamp is a measured no-op ⇒
`k = 0` ⇒ `k = 0` is reachable only in the HIGH arm at `grip >= 1e7` ⇒ the
original's `l_60 >= 79 240`". Its first step assumed the HIGH arm. **The
original is on the LOW arm at 100-150 by direct measurement, so the chain does
not start.**

> **WITHDRAWN: §25.3's "`k = 0` is reachable only in the high arm", its
> "`l_60 >= 1e7/speed` = 79 240 at speed 126", and the 278x conflict that
> followed. U-9172's two-conflicting-numbers framing is dissolved: there is one
> number, `l_60 ≈ 268-285` at 100-150, and the attribution behind it is now
> verified.**

## 5. What replaces it: a THREE-WAY inconsistency, stated and not resolved

The correction in `PREREG_STEP1.md` §3 — clamp #6 subtracts `k × lateral`, so
`|v'|/|v| = sqrt(1 - (2k - k²) s²)` — makes three measurements collide:

| # | measurement | value | coverage |
|---|---|---|---|
| (i) | ORIG `grip × |vel|` at 100-150 ⇒ LOW arm ⇒ **`k = 0.1` exactly** | 30 785.1 | n=45, G1 1424/1424 |
| (ii) | ORIG `s = |lat|/|v|` at the first sample after A6a returns | **0.729** (p05 0.0245, p95 0.9916) | n=45, unit check 1448/1448 |
| (iii) | §25.3's ORIG `+0x9e4 / |v'|` at the substep entry | **0.999991 .. 1.000020** | 2331/2331, 0 misses |

(i) and (ii) **predict** `+0x9e4 / |v'| = 1 / sqrt(1 - 0.19 × 0.729²) =` **1.0547**
— a 5.5% speed cut per frame. (iii) measures **0.002%**. They cannot all be
true, and the registered gate G4 is exactly why this is reported rather than
resolved by picking one.

**One candidate explanation is RULED OUT statically, at zero cost.** "The body
forward axis moved between the clamp and the probe sample, so (ii) is measured
against a rotated axis": A6b `0x00468980..0x00468b34` is **132 instructions**
and writes **no** `+0x9b0..0x9b8`, **no** `+0x9d4/+0x9d8/+0x9dc`, **no**
`+0x9e4` and **no** `+0x928` block — its only record writes are
`+0x9bc`/`+0x9c0`/`+0x9c4` through ECX at `0x00468abb`/`0x00468ac1`/`0x00468ac7`
(angular velocity). So A6b is not it. The gap that remains is whatever A6a's
**caller** executes between A6a's `ret` and `FUN_004709a0`'s entry, which is not
disassembled here.

**[U-9173] NEW.** The next measurement is one capture and it is entry-only
(memory `frida-interceptor-is-entry-only`): add a probe site at **A6b's entry
`0x00468980`**, which is the first function boundary after A6a's clamp, and
sample `+0x9b0..0x9b8`, `+0x9d4..0x9dc` and `+0x9e4` there alongside the
existing site 2 (`0x00467650`, A6a entry). `s` and the `+0x9e4` ratio then
bracket the clamp with nothing in between, and the three-way collision resolves
to one of: the clamp does not run (check the two gates at the same site), `s` is
small at the clamp and grows afterwards, or `+0x9e4` is refreshed downstream.
**Its first gate must be proving that `0x00468980` is actually called once per
A6a call** — that is assumed here and is not established.

## 6. D4 — reported unconditionally, as registered

| band | ORIG arm (from G) | ORIG k | PORT arm (hi/lo) | PORT med `act.kVel` |
|---|---|---|---|---|
| 100-150 | **LOW** (30 785 < 32768) | **0.1**, on the floor | 5 hi / 14 lo | **0.540954** |
| 150-260 | HIGH (57 145) | 0.1885 | 6 hi / 6 lo | 0.198908 |
| 260-500 | HIGH (136 087) | 0.1973 | 14 hi / 0 lo | 0.198426 |
| 1500-2000 | HIGH (3 357 069) | 0.1329 | 20 hi / 0 lo | 0.141023 |

> **At 100-150 both sides are on the LOW arm, and the port's `k` is 5.41x the
> original's** (0.540954 against the 0.1 floor, n=19 / n=45, median speeds
> 115.3 / 126.2). The port removes 5.41x as much lateral velocity per frame
> there as the original does. That is a measured cross-side statement about
> clamp #6 with the attribution verified on both sides, and it is the first one
> this lane has had.

## 7. The scored arm — a NO-CHANGE control, 3 of 3

**No physics source changed in this attempt.** Build `=== Build OK ===`
(219 exe objects, 421 asi objects, all up to date); dual-copy guard re-run
fresh (`log/build_rvalint.log` was stale from 29/09):
**`allowlisted=122 NEW=0`**. `mashed_re.log` reports **`participants=1`**.
Own PIDs only, all muted, `MASHED_WIN_POS=primary-bl`, `MASHED_TITLE` set on
every run.

| metric | port | n | median speed | PASS interval (unchanged, `d81a8df6`) | verdict |
|---|---:|---:|---:|---|---|
| slip 1500-2000 | **0.2033** | 20 | 1676.5 | 0.18855 .. 0.19635 | **FAIL** |
| slip 2000-2600 | **--** | 0 | -- | 0.24488 .. 0.25487 | **UNSCORABLE** |
| driving-median | **1355.66** | 54 | 1355.66 | 1904.70 .. 1982.44 | **FAIL** −30.2% |

Identical to every printed digit on 3 of 3 and equal to §21.6 / §22.2 / §22.4 /
§23.3 / §24.5 / §25.4. **D2 stays REOPENED. FAIL, and no fix authored.**

## 8. Collateral review of THIS run's arms

`collateral.py --mode paired`, attempt 12's scored arm against attempt 13's,
with a same-arm repeat on each side as the floor:

> **0 of 69 paired fields divergent on 1598 aligned frames.** Every
> `motion_diag` channel is bit-identical between the two attempts, which is what
> "no physics source changed" should look like and is now checked rather than
> asserted.

## 9. An instrument defect this surfaced, worth fixing before the next score

The scored output's line `steer over driving frames: min +33.867 ... ` for the
original and `min +1.000 ...` for the port compares **two different
quantities**: `a8_momentum.py` reads the original's `+0x1a8` (an angle in
degrees) and the port's `motion_diag` `steer=` (the input command, saturated at
1.0). §21.10 read that pair as a steer comparison. Step 0's collateral row shows
the like-for-like comparison — both sides' `+0x1a8` — and the port is short in
all six bands. Not fixed here; recorded so the next attempt does not re-read it.

## Artefacts

`PREREG_STEP1.md`, `step1_l60.txt`, `scored.txt`, `sc{1,2,3}/`,
`collateral_step3_prepost.{txt,csv}`; reducer `re/tools/statediff/a13_l60.py`.
