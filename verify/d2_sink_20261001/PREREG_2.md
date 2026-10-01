# D2 attempt 12 — STEP 2 pre-registration: the ORIGINAL's A6a-entry velocity as a MEASUREMENT

Written and committed **BEFORE** any reduction is run. Base HEAD `76215e9d`. Never amended.

## 0. The hook already exists, and the capture already carries it

§24.6 job 2 asks for "an entry hook on `0x00467650` snapshotting `+0x9b0..0x9b8` before any
write". **That hook is already in the harness and is already armed in the attempt-10 original
capture.** `re/frida/scenario_launch.py`'s `--fixup-probe` has three entry sites, and **site 2 is
`0x00467650`** (`const FP_A6A = 0x00467650`), sampling `+0x9b0/+0x9b4/+0x9b8`, `+0x9e4`, `+0x9e0`
at A6a entry behind an `ESI == FP_REC` filter. Entry-only, no mid-function probe.

`verify/d2_bounce_20260930/orig_fp2.msd.fixupprobe.csv` holds **2331 site-2 rows** from the **same
run** as `orig_fp2.msd` (one `scenario_launch.py` invocation, provenance JSON alongside). Using
it is strictly better than a fresh run, because a fresh run would not pair with the reference
`.msd` every number in §21-§24 is measured against.

So step 2 **adds no new hook and runs no new game**. It is the reduction, with the gates that
decide whether the channel may be believed.

### Two static facts read this session from `MASHED.exe.unpatched` (`re/tools/disasm_va.py`)

- A6a entry is `0x00467650 SUB ESP,0xe4` and **the vehicle record is in `ESI`**: `0x00467660
  LEA EAX,[ESI+0x9b0]` / `0x0046766e CALL 0x4c3ac0` (RwV3dLength) / `0x00467673 FSTP
  [ESI+0x9e4]`.
- `+0x9e4` is written **twice** in A6a: at entry `0x00467673` (= `|vel|` at entry) and again at
  `0x004686cc` (`MOV [ESI+0x9e4],EDX`, the post-W1 speed, the port's `Integrate2.cpp:636`).
  **The render-tick `.msd` therefore carries the LATER one**, which is `a11_accum`'s `s_mid`.
- `+0x9b0..0x9b8` stores inside A6a: `0x00467aee/afc/b08`, `0x00468692/869e`, `0x00468840/8854`,
  `0x004688d4/88e8`, `0x0046894e/8954`. All are after the entry sample.

## 1. The three gates, count-first, all STOP gates

| gate | bar | failure action |
|---|---|---|
| **G1 count** | site-2 rows **>= 1000**, and within **+/-2%** of the `.msd` frame count | STOP, report both counts, channel unused |
| **G2 join, known-answer** | `csv.site2[k].speed` (= `+0x9e4` read at A6a entry, so the PREVIOUS frame's post-W1 speed) vs `msd[f-1].s_mid`: **median relative <= 1e-5**, AND the one-frame-off control must be worse by **>= 100x** | STOP, the join is unproven, no number is used |
| **G3 non-degeneracy** | the site-2 velocity must differ from `msd[f-1].vel` on **>= 50%** of paired frames by more than 1e-6 relative | STOP, the probe is sampling the same phase as the snapshot and measures nothing |

G2 is the exact mirror of the pairing proof step 1 ran on the port (median rel 7.6e-09 against
11.0 one frame off), and it is a known-answer check because `+0x9e4`'s producer is known by RVA.

## 2. The measurements, only if all three gates pass

Banded by `|msd[f-1].vel|` using `a11_accum.BANDS`, all-four-grounded on both frames, n >= 10.

- **M1 purity.** Per-component ratio `entryVel(f)[k] / msdVel(f-1)[k]`, spread = `max-min` over
  the three components. Report median and max over all paired frames with `|v| > 10`.
  **Verdict wording fixed in advance:** "pure scalar" only if the **median spread <= 1e-5**;
  otherwise report the number and call it **not pure**.
- **M2 sigma.** `sigma_orig(f) = |entryVel(f)| / |msdVel(f-1)|`, per band, against step 1's
  directly-logged PORT `sigma`, per band, at matched median speed.
- **M3 known-answer against attempt 11.** `resid_direct(f) = |entryVel(f)| - |msdVel(f-1)|`
  per band against §24.3's ORIGINAL `resid` column (`-7.8859` at 70-100, `-12.9566` at 100-150,
  `-8.5290` at 150-260, `-4.0489` at 260-500, `-4.3643` at 500-1000, `-7.8728` at 1000-1500,
  `-19.4471` at 1500-2000). Bar: **relative difference <= 0.25** in every band with n >= 10 on
  both sides.
- **M4 slip.** For every band, report the median of `|vel_perp| / |vel|` against the body
  forward axis (`+0x9d4/+0x9d8/+0x9dc`), from the `.msd` at frame `f-1`, so the result can say
  **explicitly** whether speed and slip are separated in this capture (U-9170).

## 3. Decision rule

- **R1.** G1, G2 or G3 fails -> **STOP**. Report which, with both counts / both medians. The
  original's pre-A6a velocity stays a back-out and step 3 proceeds without it.
- **R2.** All gates pass and **M3 agrees** -> §24.3's ORIGINAL `resid` is **confirmed by direct
  measurement**; `sigma_orig` becomes a measurement. Report M1, M2, M4 and proceed to step 3.
- **R3.** All gates pass and **M3 disagrees** -> the `a11_accum`-chain back-out is wrong on the
  original side. §24.3's ORIGINAL `resid` column is **withdrawn** and the direct number replaces
  it. Report the discrepancy in full; proceed to step 3 on the direct number.
- **R4.** M1 says **not pure** -> §24.3's "pure scalar multiply" claim holds for the PORT only;
  the original's pre-A6a change has a perpendicular part and the search in step 3 must include
  non-collinear writers. Record it; it does not stop anything.

**No branch of this rule authorises a source fix.** Step 2 is measurement only.

## 4. What step 2 cannot settle, stated in advance

U-9170 stays undecided by this step **by construction**: every original sample below 500 in this
capture is the single post-bounce pass. M4 only *quantifies* the confound, it cannot remove it.
Removing it needs a capture whose low-speed frames are **straight**, which is step 3's business.
