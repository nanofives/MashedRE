# D2 attempt 12 — STEP 1 result: the PORT's `G` and `local_70` are MEASURED. **R4 fires.**

Pre-registration [`PREREG.md`](PREREG.md), commit `26b859fd`, **not amended**.
Source change: `mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp`, one default-OFF
`MASHED_A5GDIAG` block at the end of A5 Phase 4. Build `=== Build OK ===` (1 of 219 exe TUs
recompiled, 421 asi objects up to date); dual-copy guard **`allowlisted=122 NEW=0`**.
Tool: `re/tools/statediff/a12_g.py` (read-only, executes no game).

## 1. The output-channel control — all three gates pass

Two back-to-back 35 s runs, identical recipe, only `MASHED_A5GDIAG` differing. Own PIDs only
(**40076** control, **37756** armed), both muted, `MASHED_WIN_POS=primary-bl`,
`MASHED_TITLE=d2-sink-ctl` / `d2-sink-arm`. `mashed_re.log` reports **`participants=1`** on both.

| gate | bar | result | verdict |
|---|---|---|---|
| **C-CTL** | unarmed run produces no `a5g_diag.log` | file **absent** | **PASS** |
| **C-ARM** | armed run produces n >= 500 lines | **1627** | **PASS** |
| **C-CNT** | within +/-5% of `a6a_dump.log` | **1627 vs 1627**, exact | **PASS** |

`friction_diag.log` is also 1627 on the armed run, so all three channels are the same run.

### The pairing is proven, not assumed

A5 reads `+0x9e4` at entry; that value was written by the **previous** frame's A6a, so it must
equal the snapshot's `s_mid` of frame `f-1` if the positional pairing is right.

| test | n | median abs | median rel |
|---|---:|---:|---:|
| `a5g[k].sp` vs `dump[k-1].s_mid` | 1626 | **2.472e-07** | **7.621e-09** |
| control, one frame off (`dump[k]`) | 1627 | **1.102e+01** | — |

Seven orders of magnitude apart, so the frame marker is real (memory
`next-sample-pairing-needs-a-frame-marker`). One outlier of 216.7 absolute exists in 1626.

## 2. KA-1 and KA-2 both PASS. The back-out of §24.4 is validated.

| gate | bar | worst band | value | n | verdict |
|---|---|---|---|---:|---|
| **KA-1** `sigma` logged vs backed out | abs <= 1e-3 | 150-260 | **1.814e-05** | 13 | **PASS** |
| **KA-2** `l70*G*half` vs `l70G` | rel <= 0.10 | 40-70 | **0.0533** | 269 | **PASS** |

| band | n | `sigma` log | `sigma` back-out | `l70G` log | `l70G` back-out | med speed |
|---|---:|---:|---:|---:|---:|---:|
| 40-70 | 269 | 0.9998112 | 0.9998201 | 0.246713 | 0.234229 | 47.8 |
| 70-100 | 38 | 0.9995993 | 0.9996037 | 0.246713 | 0.245586 | 83.5 |
| 100-150 | 19 | 0.9994679 | 0.9994747 | 0.246713 | 0.246198 | 115.3 |
| 150-260 | 13 | 0.9991312 | 0.9991131 | 0.246713 | 0.246526 | 194.9 |
| 260-500 | 14 | 0.9984833 | 0.9984834 | 0.246713 | 0.246689 | 349.8 |
| 500-1000 | 15 | 0.9969746 | 0.9969751 | 0.246713 | 0.246672 | 735.8 |
| 1000-1500 | 19 | 0.9947493 | 0.9947500 | 0.246713 | 0.246678 | 1275.5 |
| 1500-2000 | 20 | 0.9930763 | 0.9930772 | 0.246713 | 0.246677 | 1676.5 |

## 3. What is now MEASURED on the port (direct reads, no back-out)

Every one of these is **one distinct value over all 1627 A5 calls**:

```
g0 = 0.150000006   g1 = 1.50000012   g2 = 1.28999996      (bytes 0x150 / 0x154 / 0x158)
G  = 0.290250033   l70 = 0.850000024   m54 = 0.001   dt = 50.000004
l70 * G = 0.246713                      gnd = 0x40800000 on 1626 of 1627
```

> **The port's `G` is EXACTLY the original's.** §24.4 measured the original's `G` as one distinct
> value `0.15 * 1.5 * 1.29 = 0.29025` on 1446/1446; the port logs `0.290250033` on 1627/1627.
> **The whole of §24.4's 0.85 cross-side `l70G` ratio is `local_70`, and none of it is `G`.**

| side | `G` | `local_70` | `l70G` | source |
|---|---:|---:|---:|---|
| ORIGINAL @1000-1500 | 0.29025 | **1.000** | 0.2904 | §24.4 back-out, n=200 |
| PORT @1000-1500 | 0.290250033 | **0.850000024** | 0.246713 | **this log, n=19** |

### Where the port's 0.85 comes from, by line

`ForceIntegrator.cpp:141-143` + `:153`. `iVar11` is the maximum per-car contact count over the
active cars (`carCount(c)` = `DAT_008815a8 + c*0xd04`). The branch is
`iVar11==0 -> fVar5=-0.15`, `==1 -> 0`, `==2 -> +0.5`, `==3 -> +1.0`, and `:153` applies
`local_70 = (fVar5*local_6c + 1) * local_70`. With `local_6c = 1`:

- PORT `local_70 = 0.850` exactly => `fVar5 = -0.15` => **`iVar11 == 0`**, i.e. the port's
  per-car contact count is **0** on every one of 1627 frames.
- ORIGINAL `local_70 = 1.000` exactly at 1000-1500 => `fVar5 = 0` => **`iVar11 == 1`**.

**Sign note, and it matters.** `local_70` smaller means *less* drag, so this difference makes the
port **faster**, not slower, at speed. It is a real cross-side defect in the B2/B3 contact-count
output, and it is **not** the sub-500 sink: it is constant in speed by construction and points the
wrong way. Recorded, not fixed here.

## 4. What step 1 does NOT settle

Nothing about the original's sub-500 sink, nothing about U-9170, and nothing about whether the
original's pre-A6a change is a pure scalar. The port's `sigma` at 70-100 is **0.99960** — C1 on
the port removes 0.04% of the speed per frame there, where the original's `resid` needs
**182x** more. No value of `G` or `local_70` closes that, which is what §24.4 already concluded
and this step confirms from the other side.
