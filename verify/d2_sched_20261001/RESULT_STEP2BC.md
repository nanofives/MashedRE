# D2 attempt 14, STEPS 2B and 2C — RESULT

Pre-registrations [`PREREG_STEP2B.md`](PREREG_STEP2B.md) (commit `f643b522`) and
[`PREREG_STEP2C.md`](PREREG_STEP2C.md) (commit `6c863bc1`), both committed **before** their
first run and **neither amended**. Raw output: [`step2b_scan.txt`](step2b_scan.txt),
[`step2c_readout.txt`](step2c_readout.txt), [`scored_diagnostic.txt`](scored_diagnostic.txt),
[`scored_diagnostic_medframe.txt`](scored_diagnostic_medframe.txt),
[`collateral_boosthold.txt`](collateral_boosthold.txt) / [`.csv`](collateral_boosthold.csv).

---

# STEP 2B — **GF FAILED.** The two arms occupy DISJOINT state space, and that is the result.

## 1. The gates

| gate | bar | measured | verdict |
|---|---|---|---|
| **GE** known-answer anchor | `B` within 3 of ORIG frame **980** (§16.6) and PORT frame **81** (§20.14) | ORIG **982** (`d=96`, Δ2), PORT **82** (`d=81`, Δ1) | **PASS** |
| **GH** matched steer in the window | `+0x1a8 == 33.86719` on every frame of `d`[119,300], both arms | **0** frames off, both arms | **PASS** |
| **GG** passing baseline | arm B = `orig_solo4` reports **no** diverging response channel | **0** diverging channels | **PASS** |
| **GF** occupancy | `>= 3` buckets with `n >= 20` on **BOTH** sides | **0** readable buckets | **FAIL** |

> **GF failed, so the registered first-divergence rule DID NOT EXECUTE and was NOT amended.**
> The script's trailing line `FIRST DIVERGING TERM: NONE` is **vacuous over an empty readable
> set** and is explicitly **not** read as "the two sides agree". Verdict **(B)** of
> `PREREG_STEP2B.md` §9 did not fire either; neither branch did.

**C1, the confounder the window was built to avoid, measured:** steer angle at each arm's own
bounce is **30.62158** (ORIG) against **28.50488** (PORT), **6.91 %** apart. Bounce-aligning
would have compared those two directly. The `d`[119,300] window avoids it entirely — GH proves
both arms carry `33.86719` on all 182 frames.

## 2. The occupancy table IS the finding

Window `d`[119,300], grounded, **182 valid frames on each side**:

| | frames at `cos(fwd,vel) < -0.1` | frames at speed `>= 150` | median speed |
|---|---:|---:|---|
| **ORIGINAL** | **1 / 182 (0.5 %)** | **150 / 182 (82.4 %)** | 150-1638 across its buckets |
| **PORT** | **86 / 182 (47.3 %)** | **0 / 182 (0.0 %)** | 21-103 across its buckets |

The ORIGINAL's three largest buckets are `cos` `[0.95,1.01)` × speed `[1000,3000)` (n=58,
median speed 1637.9, median frame `d`=272), `[0.95,1.01)` × `[400,1000)` (n=24, 670.8, `d`=231)
and `[0.70,0.95)` × `[150,400)` (n=33, 187.3, `d`=182). The PORT's are `[0.95,1.01)` × `[0,50)`
(n=74, 21.3, `d`=224), `[-1.01,-0.50)` × `[0,50)` (n=42, 29.7, `d`=238) and `[-1.01,-0.50)` ×
`[50,150)` (n=38, 58.4, `d`=220). **They do not overlap in a single bucket.**

> **Three independent instruments in this attempt have now returned "there is no overlap":**
> §26.9's regime count (**5** common frames), STEP 2A's cross-side banded collateral (**0**
> readable rows, all 7 bands `!!`-flagged), and STEP 2B's state-matched response test (**0**
> readable buckets). **On this arm, after the first bounce, no cross-side comparison can have
> power, however it is instrumented.** That is not an instrument failure — it is a measurement
> about the arm, and it is why STEP 2C had to be an experiment rather than another reduction.

---

# STEP 2C — the discriminating DIAGNOSTIC. **The latency closes completely; the recovery does not.**

## 1. The instrument, and what it is not

`MASHED_D2_BOOSTHOLD=1` (default **OFF**) performs one write per car per race, on the first
frame that car consumes a nonzero steer byte: `+0xbf8 = 2`, `+0xbf4 = 3000`
(`mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:316-352`). The 15-frame hold that follows is
produced entirely by the **original's own transcribed `+0xbf8 == 2` arm**
(`Integrate2.cpp:407-414`, from `0x00467def`..`0x00467e44`).

> **The TRIGGER is FITTED, not transcribed. This is a measurement harness, it earns no
> C-level, it mutates no tracker row, it is absent from every scored arm, and it may not ship
> while U-9174 is open.**

## 2. The gates

| gate | bar | measured | verdict |
|---|---|---|---|
| **D-0** default build untouched | knob-OFF run bit-identical to `s1` | **bit-identical on all 1629 shared lines** | **PASS** |
| **D-1** the knob took | `b14` exactly 0 on `d`=0..14, nonzero at `d`=15 | **0** nonzero frames in `d`[0,14]; engages at `d`=15 — on **all three** runs | **PASS** |
| **D-2** determinism | 3 runs agree | `bh1` vs `bh2` and `bh1` vs `bh3`: **0 of 1626/1625** frames differ in `sp` | **PASS** |
| build | `=== Build OK ===`, dual-copy `NEW=0` | `allowlisted=122 **NEW=0**` | **PASS** |

`participants=1` in `mashed_re.log` on every run; muted; `MASHED_WIN_POS=primary-bl`;
`MASHED_TITLE` set; own PIDs only.

**D-1's positive witness, against the ORIGINAL's measured signature** (memory
`verify-the-harness-knob-actually-took`):

| | `b14` at engagement | engagement `d` |
|---|---|---:|
| **ORIGINAL** | `(-265866, 0, -762420)` | **15** |
| **PORT, knob ON** | `(-270030, 0.0218758, -761363)` | **15** |
| PORT, knob OFF | `(-238892, 0.0216498, -763499)` | **0** |

`x` within **1.57 %**, `z` within **0.14 %**.

## 3. M1 / M2 — the latency closes COMPLETELY

Best-fit integer lag against `orig_solo3` over `d = 16..95`, n=80, by STEP 2A's method:

| arm | L=0 | L=1 | L=14 | L=15 | L=16 | **BEST** |
|---|---:|---:|---:|---:|---:|---|
| PORT knob OFF | 89.07 % | 86.73 % | 5.17 % | **1.44 %** | 1.83 % | **L = 15** |
| PORT knob ON | **0.15 %** | 3.37 % | 46.57 % | 49.21 % | 51.74 % | **L = 0** |

> **The 15-frame lag goes to zero, and the launch error improves tenfold — from 1.44 % at
> `L = 15` to 0.15 % at `L = 0`.** The peak moves to the ORIGINAL's own frame: **1835.50 at
> `d = 95`** against the original's **1832.40 at `d = 95`**, **0.17 %** apart, where knob-OFF
> peaked at 1856.57 at `d = 80`.

This is the fourth independent confirmation of the `3000 / 200 = 15` mechanism (after the
countdown arithmetic, the `b14` engagement frame and the knob-OFF lag fit), and it is the
strongest: **supplying only the trigger reproduces the original's launch to 0.15 %.**

## 4. M3 — THE DISCRIMINATOR. Registered verdict: **INCONCLUSIVE.**

Peak, trough, and the **400 frames after the trough** (the registered window):

| arm | peak | at `d` | trough | at `d` | `>= 100` | % | median | max |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| **ORIGINAL** | 1832.40 | 95 | 85.45 | 101 | **397 / 400** | **99.2 %** | **1333.9** | 2478.3 |
| **PORT knob ON** | 1835.50 | **95** | 83.43 | 106 | **243 / 400** | **60.8 %** | **132.8** | 737.9 |
| PORT knob OFF | 1856.57 | 80 | 13.40 | 151 | **0 / 400** | **0.0 %** | 31.5 | 68.7 |

*(STEP 2A §3 quoted the original's **full** tail — 1313 of 1346, median 1828.5, max 2562.8.
`PREREG_STEP2C` §4 registered a **400-frame** window, so the 400-frame numbers above are the
registered ones. Both are correct over their own windows; neither contradicts the other.)*

> **Registered rule:** H1 iff `>= 50 %` **AND** median `>= 900`; H2 iff `< 10 %`; otherwise
> INCONCLUSIVE.
> **Measured: 60.8 % and median 132.8.** The fraction clause passes, the median clause fails.
> # >> VERDICT: **INCONCLUSIVE.** No third branch is added.

**What the numbers say descriptively, without reinterpreting the rule.** Supplying the hold
takes the port from **never** recovering (0.0 % of frames above 100, median 31.5, max 68.7) to
**partially** recovering (60.8 %, median 132.8, max 737.9). It does not reach the original
(99.2 %, median 1333.9, max 2478.3). **Both surviving hypotheses have support and neither is
complete**: the line matters, and something else matters too. That is exactly the state the
registered INCONCLUSIVE branch exists to record, and it is more informative than either clean
verdict would have been.

Speed against release-relative frame, all three arms:

| `d` | 15 | 40 | 80 | **95** | 119 | 150 | 200 | 250 | 300 | 500 | 1100 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| **ORIGINAL** | 14.94 | 484.97 | 1591.34 | **1832.40** | 109.11 | 187.32 | 334.66 | 1212.20 | 1989.67 | 479.47 | 1996.86 |
| **knob ON** | 15.69 | 485.37 | 1595.33 | **1835.50** | 99.06 | 157.13 | 284.78 | 537.38 | 168.02 | 17.60 | 371.80 |
| knob OFF | 290.88 | 971.07 | 1856.57 | 139.06 | 98.56 | 27.98 | 11.48 | 42.56 | 11.11 | 9.42 | 46.13 |

The knob-ON arm tracks the original to three or four digits through `d = 95`, then tracks it
loosely to `d ~ 250`, then falls away.

## 5. M4 — the three scored metrics on the DIAGNOSTIC arm. **All three still FAIL.**

**LABELLED DIAGNOSTIC. This is not a scored re-close and cannot be one** — the trigger is
fitted, so these numbers are not evidence that the port is faithful. The §3 bounds are
unchanged and not renegotiable. 3 of 3 runs identical to every printed digit.

| metric | PASS interval | **knob ON** | n | median speed | median frame (`d`) | knob OFF | verdict |
|---|---|---:|---:|---:|---:|---:|---|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.1983** | 19 | 1665.1 | **86** (`d`=85) | 0.2033 | **FAIL** by 0.00195 |
| slip 2000-2600 | 0.24488 .. 0.25487 | **UNSCORABLE** | **0** | — | — | UNSCORABLE | **FAIL** |
| driving-median | 1904.70 .. 1982.44 | **1019.77** | 76 | 1019.8 | **80** (`d`=79) | 1355.66 | **FAIL** |

Two move in opposite directions and both are worth recording:

- **slip 1500-2000 improves and nearly lands.** `0.2033 -> 0.1983` against a mean of `0.19245`:
  the error halves from **+5.6 %** to **+3.0 %**, and the miss against the upper bound is
  **0.00195**. It remains outside.
- **driving-median gets worse**, `1355.66 -> 1019.77`, while its `n` rises `54 -> 76`. The
  partially-recovering car spends more frames above the 500 floor **at low speed**, which pulls
  the median down. A metric moving the wrong way while its population grows is a property of
  the statistic, not a new defect. **[UNCERTAIN]** whether the knob-ON driving-median would
  move back up on an arm where the recovery completes; nothing here settles that.

The ORIGINAL's rows, for the median-frame guard: slip 1500-2000 `n=315`, median speed 1789.9,
median frame **1605** (`d`=719); slip 2000-2600 `n=529`, 2227.1, **1794** (`d`=908);
driving-median `n=1208`, 1896.7, **1683** (`d`=797). **The guard still fires** — the port
populates these bands at `d` 85 / — / 79.

---

# 6. COLLATERAL — same-side A/B, knob OFF against knob ON

`collateral.py --mode paired`, arms `s1` vs `bh1`, floors `s2` and `bh2` (a same-arm repeat on
**both** sides, both exactly 0). This is the right mode: same build, same arm, one knob — the
case `--mode paired` is documented for.

- **69 paired fields; 47 divergent; 22 within the noise floor on every one of 1627 frames.**
- **Within floor:** `in[0]`, `in[1]`, `in[2]`, `in[3]`, `steer`, `gnd`, `p15`, `p16`,
  `gt[0..5]`, `gb498`, `gb49c`, `fl[0..3]`, `reseed`, `susp`.
  > **No input channel moved.** All four descriptor bytes and the commanded `io.steer` are
  > bit-identical across the knob, which is the knob's charter verified rather than asserted.
  > The gearbox table `gt[0..5]` and its inputs `+0x498`/`+0x49c` are untouched too.
- **First divergence: `b14[0]` and `b14[2]` at frame 2** — the knob's one intended target, and
  nothing else on that frame. **Everything else first diverges at frame 3**: `wf[0..7]`,
  `ftot[0..2]`, `wle4`, `wld4`, `wax`, `w1b`, `b0c`, `horiz`, `sp`, `velH`, `bodyH`, `slip`,
  `d[]`, `gear`, `gtmr`. A clean one-frame causal ordering: the knob moves `+0xb14` and only
  `+0xb14`, and the rest is downstream of it.

**Outside-scope rows: none.** `re/tools/statediff/scope_a6a.txt` is keyed to `msd+0xNNN` field
names and does **not** apply to a `kv`-vs-`kv` pairing, so the tool printed every row
`outside`; that column is not meaningful here and is not read. Classified by hand against
`PREREG_STEP2.md` §3's tier table, which cites every `motion_diag` field's record offset from
`VehiclePhysicsRun.cpp:1226-1270`: all 47 divergent fields sit in tiers **T1-T7, downstream of
T2 `+0xb14`**. The only fields that would lie **outside** any causal chain from `+0xb14` — the
four input bytes, `steer`, and the static tables `gt[]`/`gb498`/`gb49c`/`p15`/`p16` — are all
**within floor**.

---

# 7. What this attempt now knows, and what is still open

**Settled by STEP 2C.** The `+0xbf8 == 2` hold is, to 0.15 %, the whole of the launch
difference. Supplying only the trigger makes the port's launch the original's frame-for-frame,
peak included (`1835.50` vs `1832.40`, both at `d = 95`). The mechanism recorded in STEP 2A is
not merely consistent with the data — it **reproduces** it.

**Not settled, and registered as such.** Whether the line explains the non-recovery.
**INCONCLUSIVE**: 60.8 % against a 50 % bar and a median of 132.8 against a 900 bar. The port
goes from never recovering to partially recovering and still does not reach the original.

**Consequences for the map.**
1. **U-9174 is on D2's critical path, and is no longer only a fidelity row.** Locating the
   `+0xbf8 = 2` writer is now worth a session on its own: it is the difference between a port
   whose launch is 1.44 % off with a 15-frame lag and one that is 0.15 % off with none.
2. **The knob gives the next attempt what every instrument this session lacked: overlapping
   state.** With it on, the two arms share the whole launch and much of `d`[95,250]. A
   state-matched response test re-run on `bh1` instead of `s1` would have readable buckets.
   **It must be pre-registered before it is run**, and its results carry the fitted-trigger
   caveat.
3. **The residual recovery gap is a real second defect** and is the remaining D2 lane.

**Still open:** U-9174; the recovery residual; `+0xb0c` (STEP 2A's EXPLORATORY T1 row, `d`=1);
U-9173; U-9156; U-9160; U-9171; §20.14's 40-70 residency and `-0.1` duty cycle; D1-residue R1.

# 8. The instrument lessons these two steps paid for

**When three instruments all say "no overlap", stop instrumenting and run an experiment.**
§26.9, STEP 2A's collateral and STEP 2B's buckets all failed the same way for the same reason.
The information was not in the captures, so no fourth reduction would have found it. A
default-OFF harness knob that supplies one unlocated trigger produced more in three runs than
three reductions did.

**A fitted trigger is a legitimate instrument and an illegitimate fix.** It answered the
question precisely *because* it was labelled — the knob earns no C-level, enters no scored arm,
and is excluded from the default build by a gate (`D-0`) rather than by intention.

**Register the "in between" branch before you run.** The discriminator landed between its two
thresholds. Having INCONCLUSIVE pre-committed is what kept 60.8 % from being written up as a
confirmation of H1.

**A metric can move the wrong way for a right reason.** `driving-median` fell from 1355.66 to
1019.77 while its `n` rose from 54 to 76, because a partially-recovering car adds frames just
above the 500 floor. The population has to be read with the median, every time.
