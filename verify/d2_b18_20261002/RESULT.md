## RESULT — D2 attempt 16

**Live test (STEP 1), on the RUNNING original, `--axis-probe`, pre-registered at `7e9cbf7c`, no gate amended:**
- Self-check PASS (`DAT_00614708 == [0,0,1]`, `rec == 0x8815a0`); coverage `a6a=2179`, `drive=1275`, `skipped=0`, `err=null`.
- Original body forward-Y `+0x9d8` = **one distinct value `0`** across all 2179 A6a frames; all four wheel axis-Ys `+0x224/2e8/3ac/470` = `0.0` (`axYnz=0`); A6a leaves `+0xb18 == 0` at A6b entry (`b18nzPost=0`).
- Port (`port_a6adump.log`, 1633 frames): forward-Y nonzero 1632/1633, median **3.05e-08**, max 3.83e-08; `+0xb18` snapshot nonzero 1618/1633, median **0.0282**; `omega.x` (+0x9bc) median **5.44e-10**, `omega.z` (+0x9c4) median **6.86e-10**.

**Both writers, by RVA:** `+0xb18` by A6a `FUN_00467650` `0x00467cc5`/`0x00467d97`; wheel axis-Y by A5 `FUN_0046ddb0` `0x0046de74` (`[esi+0xb8]=[edi+0x9d8]`, steer==0) from body forward written `0x0046ddc9` (`xform*(0,0,1)`); A4 `0x0047072c` zeroes b14/18/1c per frame. **n = 2179 original / 1633 port; coverage as above.**

**Cause:** `+0xb18 = Σ axisY*force`, so a zero axis-Y zeroes it at any force — the original's zero is **STRUCTURAL** (H-later refuted). The original's omega.x/z are exactly 0 on flat ground, so `BodyOrientationIntegrate FUN_0046e9e0` keeps the car matrix at-row Y bit-exact 0; the port's omega.x/z carry ~5e-10 FP noise from the contact torque sum, drifting at.y to ~3e-08. The port's 0.0282 snapshot is that epsilon amplified by the large drive/boost multipliers (`ff=5e6`).

**Fix:** NONE. STEP 2 not entered per `PREREG_STEP1.md` §5 — the divergence is diffuse float noise with no single faithfully-portable producer; forcing at.y=0 is a forbidden clamp; and the velocity-Y effect (~5e-7/frame) cannot be the ~10x X/Z recovery deficit. No value changed; no knob added. No C-level change. **U-9175 RESOLVED.**

**STEP 3, build byte-identical to attempt 15** (`git diff 29bd7619..HEAD -- mashedmod/src` empty), reproducing attempt 15 to every digit:
- **a** `+0xb18 == 0` every post-release port frame: **FAIL** (nonzero 1618/1633, median 0.0282 — no fix applied).
- **b** launch: **PASS** — L=0 (0.19%), `+0xb14` at `d`=15, peak **1835.50 at `d`=95** vs 1832.40.
- **c** recovery (H1 ≥50% AND median ≥900): **INCONCLUSIVE** — **243/400 = 60.8%** (fraction passes), median **132.8** (median fails) vs 99.5% / 1333.9.
- **d** three metrics vs `d81a8df6` bounds, participants=1: **FAIL 3/3** — slip 1500-2000 **0.1983** (n=19, median speed 1665.05, median frame 86 = `d`85); slip 2000-2600 **UNSCORABLE** (n=0); driving-median **1019.77** (n=76, median speed 1019.77, median frame 80 = `d`79).

**Next term:** `+0xb0c` (first diverging at `d`=1, errs both directions — no single-constant fix); the recovery root is the X/Z-plane velocity/clamp collapse (`+0xb14`/`+0xb1c` first diverge at the engagement frame `d`=15 by only 1.57%/1.03%, so the launch force is faithful). Not re-opened this attempt.

**Collateral:** no behavioral change possible — only the read-only `--axis-probe` + docs changed; the port run matches attempt 15's `r1` launch numbers to every digit. **No outside-scope rows.**

**Commits:** `7e9cbf7c` (PRE-REGISTER STEP 1 + `--axis-probe`, unrun), `ac1fe00b` (STEP 1 result + U-9175 RESOLVED + CHANGELOG), and the handoff commit (NEXT_SESSION, ROADMAP §D2, info pane, RESULT.md). Nothing pushed.

**Still open:** `+0xb0c`; the recovery gap (X/Z plane); U-9173; U-9156; U-9160; U-9171; §20.14's `-0.1` duty cycle; D1-residue R1. D3 modes 3/7 hold stands; D2 must close before it starts.
