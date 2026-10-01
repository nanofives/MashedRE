# D2 attempt 12 — STEP 3 result: the sink is **NAMED**, and it is the PORT's trailing clamp-#6 velocity write. The ORIGINAL's is a measured no-op at every speed.

Pre-registration [`PREREG_3.md`](PREREG_3.md), commit `a17cf0c0`, **not amended**.
Tools `re/tools/statediff/a12_split.py`, `a12_clamp.py`, `a12_mult.py` (all read-only, none
executes a game). Raw output: [`step3_split.txt`](step3_split.txt),
[`step3_clamp.txt`](step3_clamp.txt), [`step3_mult.txt`](step3_mult.txt),
[`a6a_disasm.txt`](a6a_disasm.txt).

## 1. The gates

| gate | bar | result | verdict |
|---|---|---|---|
| **T-C** known-answer | PORT `median(\|residA\|) <= 0.01` every band | worst **0.0001** (1000-1500) | **PASS** |
| **T-A** | `dIn` ORIG/PORT in [0.5, 2.0] | **0.371** at 100-150; 0.63..1.06 elsewhere | **FAIL**, 1 band |
| **T-B** | `total` ORIG/PORT in [0.5, 2.0] | **0.369** at 100-150; 0.63..1.05 elsewhere | **FAIL**, 1 band |
| **T-D** | `ratio9e4` ORIG vs PORT, diff <= 0.01 | **0.219104** at 100-150; 0.00006..0.0054 elsewhere | **FAIL**, 1 band |

**T-A and T-D both fail, in the same single band.** The registered rule enumerates R3 (T-A fails)
and R4 (T-A passes, T-D fails) but not "both". Stated plainly rather than forced into a branch:
**both triggers are met at 100-150 and nowhere else**, and §5 below shows they are the same
event seen twice, because `dIn` and `total` are built on `C` and the port inflates `C` relative
to its own final velocity by exactly the clamp's share.

## 2. The budget, per band, both sides

`A = |snapVel(f-1)|`, `B = |vel at A6a entry|`, `C = s_mid(f) = +0x9e4`,
`net = |snapVel(f)| - A` (frame-to-frame, and the only one immune to where `+0x9e4` is latched).

| band | side | n | A | total | dPre | dIn | residA | ratio9e4 | **net** |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 100-150 | ORIG | 45 | 126.2 | 6.1410 | -0.0655 | 6.1943 | -12.8909 | 1.000012 | **+6.148** |
| 100-150 | PORT | 19 | 115.3 | 16.6581 | -0.0609 | 16.7132 | -0.0000 | **1.219115** | **-14.940** |
| 150-260 | ORIG | 73 | 189.5 | 13.6682 | -0.1531 | 13.8107 | -8.3356 | 1.000022 | +9.397 |
| 150-260 | PORT | 13 | 194.9 | 21.7335 | -0.1720 | 21.8904 | -0.0000 | 1.005395 | +19.670 |
| 260-500 | ORIG | 60 | 371.1 | 20.0119 | -0.5826 | 20.7312 | -3.4809 | 1.000010 | +19.782 |
| 260-500 | PORT | 14 | 349.8 | 19.0469 | -0.5596 | 19.5507 | -0.0000 | 1.000427 | +18.890 |
| 500-1000 | ORIG | 159 | 821.6 | 27.2825 | -2.9222 | 28.9981 | -1.2670 | 1.000001 | +27.266 |
| 500-1000 | PORT | 15 | 735.8 | 33.1855 | -2.2260 | 36.4771 | -0.0001 | 1.000058 | +33.131 |
| 1000-1500 | ORIG | 200 | 1280.9 | 27.3972 | -6.9934 | 32.9661 | -0.8400 | 1.000005 | +27.326 |
| 1000-1500 | PORT | 19 | 1275.5 | 29.1206 | -6.6972 | 35.8178 | -0.0001 | 1.001316 | +27.406 |
| 1500-2000 | ORIG | 339 | 1778.7 | 15.2224 | -13.5587 | 28.4037 | -6.1644 | 0.999995 | +15.056 |
| 1500-2000 | PORT | 20 | 1676.5 | 23.7211 | -11.6080 | 33.8182 | -0.0001 | 1.004422 | +17.094 |

> **At 100-150 the original GAINS 6.148 of speed per frame and the port LOSES 14.940. Opposite
> sign.** That is the trap, as one number, for the first time. Everywhere at and above 260 the
> frame-to-frame net agrees within **0.82x..1.05x** at matched median speed.

`net` is not a registered statistic; it is reported as such, and it is reported because it is the
only one of the five that does not depend on the `+0x9e4` write phase.

## 3. The named sink, by RVA

**The trailing grip-clamp-#6 lateral-removal write inside A6a `0x00467650`.** Every store read
this session from `MASHED.exe.unpatched`, and the `[edi]` alias is included — `0x00468633
LEA EDI,[ESI+0x9b0]` makes `[edi]`, `[esi+0x9b4]`, `[esi+0x9b8]` the three components, which an
offset grep alone would have missed (`offset-grep-misses-dword-index`).

| role | RVA | port counterpart |
|---|---|---|
| grip `*` multiplicand | `0x004687db  FMUL [ESP+0x20]` | `Integrate2.cpp:660` `grip = grip * speed` |
| arm selection | `0x004687df  FCOM [0x005ce9fc]` (= 32768.0) / `0x004687ea JNE 0x46888f` | `:722` `if (k32768 < grip)` |
| gate | `0x0046874c  FLD [ESP+0x20]` / `FCOMP [0x005d757c]` (= 0.0) | `:655` `speed != 0.0f` |
| grounded gate | `0x00468761  CMP [ESI+0x9e0],0x40800000` | `:655` `Ri(v,0x9e0) == 0x40800000` |
| **HIGH arm stores** | `0x00468833` (`+0x9b0` via EDI) / `0x00468840` / `0x00468854` | `:726` |
| **LOW arm stores** | `0x004688ca` / `0x004688d4` / `0x004688e8` | `:735` |
| full-stop test / stores | `0x00468939` / `0x0046894a` / `0x0046894e` / `0x00468954` | `:742-744` |

Constants re-read from the binary this session (`audit-annotated-consts-against-the-binary`):
`0x005ce9fc = 0x47000000 = 32768`, `0x005ce9f8 = 10000000`, `0x005ce9f4 = 1.000000012e-07`
(the port's name is `k9p9998e8`; the binary value is exactly 1e-7), `0x005cc56c = 0.1`,
`0x005ce9f0 = 3.051757812e-05 = 2^-15`, `0x005d757c = 0`.

## 4. The naming bar, all three parts met

**1. RVA, cited.** Above, from `MASHED.exe.unpatched`, over a capstone sweep bounded by A6a's own
last `RET` at `0x0046897b` (the sweep ran to `0x00468989`, i.e. into A6b, with no bad-byte stop).

**2. Gate checked ON THE RUNNING ORIGINAL, with a count.** `--fixup-probe` samples `+0x9e4` and
`+0x9b0..0x9b8` together at each entry site, so `speed/|vel|` is a **single-row** statistic needing
no join. Site 1 (`0x004709a0`, the substep entry) is the first sample after A6a returns:

| band | n | ORIG `+0x9e4 / \|vel\|` at the substep entry |
|---|---:|---:|
| 70-100 | 12 | 0.999970 |
| 100-150 | 90 | **0.999991** |
| 150-260 | 143 | 1.000020 |
| 260-500 | 119 | 1.000010 |
| 500-1000 | 311 | 1.000001 |
| 1000-1500 | 395 | 1.000004 |
| 1500-2000 | 678 | 0.999996 |

Coverage, checked and not assumed (`zero-of-n-needs-a-coverage-check`): **2331 of 2331** site-2
rows have a following site-1 row before the next site-2, **0 without**. And nothing can refresh
`+0x9e4` in between: a full sweep of A6a finds **exactly two** writers, `0x00467673` and
`0x004686cc`, both through ESI, and a full sweep of **A6b `0x00468980..0x00468b34`** finds **no
`+0x9e4` store and no `+0x9b0..0x9b8` store at all**.

> **The original's clamp does not change the velocity, at any speed.** Per sample, the original's
> `C - |snapVel(f)|` is **0.0012 .. 0.0056** against speeds of 126 .. 1779, i.e. a relative
> `1e-5`. Solving the clamp's own geometry with the measured perpendicular fraction (0.727 at
> 100-150) gives **`k` = 1.2e-4** there and **`k` ~ 0** at 1500-2000.

**3. Magnitude, within a factor of 2, in the one band where the divergence exceeds the T-A bound.**

| band | n(o)/n(p) | ORIG clamp loss | PORT clamp loss | net divergence | (P-O)/div |
|---|---:|---:|---:|---:|---:|
| **100-150** | 45/19 | **0.0014** | **18.5779** | **21.0888** | **0.881** |
| 150-260 | 73/13 | 0.0038 | 0.9595 | -10.2732 | -0.093 |
| 260-500 | 60/14 | 0.0031 | 0.1573 | 0.8922 | 0.173 |
| 500-1000 | 159/15 | 0.0012 | 0.0495 | -5.8651 | -0.008 |
| 1000-1500 | 200/19 | 0.0056 | 1.7143 | -0.0801 | -21.329 |
| 1500-2000 | 339/20 | -0.0090 | 7.5134 | -2.0375 | -3.692 |

**0.881 at 100-150** — the port's clamp loss accounts for 88% of the whole net divergence there,
a factor of **1.135**. The other bands are not scored by this bar because their divergence is
inside T-A.

**SINK NAMED.**

## 5. Why the port's clamp bites and the original's does not — what is measured, and what is not

The port's own clamp telemetry (`a6a_dump.log`, `act.grip` / `act.arm` / `act.kvel`), banded:

| band | n | clampRan | arm=0 (low) | arm=1 (high) | med `grip` | med `kVel` |
|---|---:|---:|---:|---:|---:|---:|
| 40-70 | 269 | 269 | **269** | 0 | **1 787** | **0.945473** |
| 70-100 | 38 | 38 | 30 | 8 | 2.703e+04 | 0.199269 |
| 100-150 | 19 | 19 | 14 | 5 | 1.504e+04 | 0.540954 |
| 150-260 | 13 | 12 | 6 | 6 | 3.136e+04 | 0.198908 |
| 260-500 | 14 | 14 | 0 | 14 | 7.87e+04 | 0.198426 |
| 500-1000 | 15 | 15 | 0 | 15 | 4.034e+05 | 0.191931 |
| 1000-1500 | 19 | 19 | 0 | 19 | 1.637e+06 | 0.167261 |
| 1500-2000 | 20 | 20 | 0 | 20 | 2.949e+06 | 0.141023 |

The port's `grip` crosses the 32768 knee at about 150-260, so below it the **low arm** runs, whose
`k` reaches **0.945** — removing 94.5% of the lateral velocity every frame. The high arm's `k` is
capped at 0.2 and reaches 0 only at `grip >= 1e7`, which the port never approaches.

**Confounds ruled out BY MEASUREMENT, not by argument:**

- `+0x18c` is **1.0** on both sides (ORIG all 2332 frames, PORT all 1627), so `grip = l_60/+0x18c = l_60`.
- `+0x2c` and `+0x34` are **0** on both sides, so neither optional scaling at `0x00468712` /
  `0x0046872f` fires.
- Track grip scaling does **not** fire on either side: the ORIGINAL's `+0x1f0` is `0xffc88080`
  (`-0x377f80`, 1983 frames) and `0xffb48080` (`-0x4b7f80`, 348), the PORT's is `0` on all 1627,
  and **none** of those matches any of the five literals the scaling compares against
  (`0xffa08080`, `0xffaa8080`, `0xff961e5a`, `0xff1e80b4`, `0xffc81e5a`, read at
  `0x004686c7`..`0x00468707`).

**[UNCERTAIN U-12A] What the original's clamp multiplicand actually is.** An ESP-delta walk over
A6a puts `0x004685d0` / `0x0046874c` / `0x004687db` all at frame slot **`+0x110`**, whose only
writers are the entry init `0x004676a0 MOV [ESP+0x20],0` and three `FSTP` sites **inside the
per-wheel loop** (`0x004680ca`, `0x004680f2`, `0x004684b7`, loop bounds `0x00467b45`..`0x00468544`
by the `ADD EDI,0xc4` / `JL 0x467b45` back edge). **No instruction writes frame+0x110 from the
post-W1 length computed at `0x004686a4`**, which is the value stored to `+0x9e4`. The port binds
all three sites to `speed = Vec3Mag3(+0x9b0)` (`Integrate2.cpp:633`, used at `:655`, `:660`,
`:742`). Missing evidence: a decompiler-level resolution of frame+0x110's identity — the linear
ESP walk is unreliable across the loop's branches and cannot be trusted for the writers'
operands.

**[REFUTED, this session] The candidate "the multiplicand is the length latched at
`0x0046820f`".** Raised from an **ESP-naive** grep for `[esp+0x20]`. `0x0046820f` sits one push
deeper (`ADD ESP,4` at `0x0046821a`), so it writes frame **+0x114**, not +0x110. Kept because the
running-original numbers are informative: the magprobe (`--mag-probe` records the RETURN site)
shows that call returning **0.043 .. 0.752** where `+0x9e4` is **126 .. 1743** — 3 to 4 orders of
magnitude apart — and firing exactly 4 times per frame on 541 of 1424 frames and 0 times on 883.

**[UNCERTAIN U-12B] Conflict with §21.9.** §21.9 measured the ORIGINAL's `grip*speed` at
**33 157.4** at band 100-150 (n=35, `verify/d2_magpr_20260930/`). The clamp's own arithmetic plus
the measured no-op force `k ~ 0`, which the high arm reaches only at `grip >= 1e7` and the low
arm (floor 0.1) can never reach. **33 157 and ">= 1e7" cannot both be true.** §21.9's figure is
an inference about one of the clamp's inputs from a shared-callee probe; this session's is a
direct observation of the clamp's net effect with 2331/2331 coverage. Neither is discarded here.
Resolving it is the first job of the next lane.

## 6. No fix is authored, and why

The naming bar is met, so the **writer** is named. The **input** is not. §21.5 proved clamp #6's
arithmetic byte-faithful and this session re-read it instruction by instruction and agrees, so
there is nothing faithful to change **at** the named RVA: a correct fix must change what reaches
`0x004687db`, and that quantity is U-12A. Forcing `k` to 0, or gating the clamp off, would be a
deliberately unfaithful knob. **No speculative fix.**

## 7. A second, separate port defect recorded in passing

The port's `+0x1f0` (track id) is **0** on all 1627 frames where the original's is a track
literal. It is **inert for the clamp** (neither value matches a scaling constant, measured above)
and inert for `fVar3sel` (both fall to the same `else`), but it is a real cross-side difference
and it gates `Integrate2.cpp:214-217` (the V1 velocity scalar `f7`) and `:249` (the drive-force
boost), neither of which can fire on either side with these values.

Also from step 1 and repeated here for the record: the port's `local_70` is **0.850000024** where
the original's is **1.000** at 1000-1500, because the port's per-car contact count `iVar11` is 0
on every frame where the original's is 1 (`ForceIntegrator.cpp:141-143`, `:153`). Constant in
speed, and the sign makes the port **faster**, so it is not the trap.
