## RESULT — D2 attempt 17

**`+0xb0c` writers / readers / expression (STEP 1, static, self-checked instruments).**
Writers: A4 `FUN_00470670` **`0x00470724`** (`fstp [edi+0xb0c]`, the formula) and
**`0x0047072c`** (`mov [edi+0xb0c], ebx`, EBX=0, taken when `+0x9e4 == 0.0`); plus an
init-block store `0x0046bc84`. Readers — **exactly two** in `.text`: A6a **`0x004676de`**
(`fld [0x5cd0ac]=1500.0; fsub [esi+0xb0c]; fcom [0x5ccd04]=500.0` -> `fVar5 =
max(1500.0 - b0c, 500.0)`) and the AI accessor `FUN_0046d6a0` **`0x0046d6b6`** (folded
`0x008820ac`). Expression, transcribed byte-exact from `0x004706db..0x00470724`:
`+0xb0c = (1.0 - |dot| / speed) * speed`, `dot = (fwd.y*vel.y + fwd.x*vel.x) + fwd.z*vel.z`,
`1.0 = _DAT_005cc320 = 0x3f800000` read from the anchored exe. Seven inputs: `+0x9b0/b4/b8`,
`+0x9d4/d8/dc`, `+0x9e4`. Port: `VehicleControl.cpp:108-116` (exe), `PhysicsChainHooks.cpp:296-303`
(asi), reader `Integrate2.cpp:145`, accessor `AiStandalone.cpp:833`. The folded sweep
(`re/tools/fold_sweep.py`) **failed its own known-answer check on `+0xbf8` first** and that
negative was discarded; the rewritten sweep recovers attempt 15's `0x0046d7a2` verbatim over
622 511 instructions. Dword-index form `0x2c3`: 0 hits.

**Live test — inputs, n, coverage, known answer.** New `--slide-probe`: entry-only hook on
A4 `0x00470670` (record base in `EAX`), reading all seven inputs plus the standing `+0xb0c`,
with per-row frame and release markers. Coverage `calls 2333 / mine 2333 / skipped 0 /
capped false / err null` — A4 fires once per frame for slot 0 across 2334 captured frames.
Static self-check at arm time: `0x005cc320 = 1.0`, `0x005d757c = 0.0`, `0x005cd0ac = 1500.0`,
`0x005ccd04 = 500.0`. **Gate KA FAILED on BOTH routes** (snapshot `0.948789 / 0.940083 /
0.945329`; live cross-call `0.971698`; threshold 0.99) and was **retired, not amended** —
`+0xb0c` is algebraically `speed - |dot|`, so a result-relative tolerance scores the
cancellation; KA's worst miss is **0.24 of ONE float32 ulp**. Separately pre-registered
**KA2 PASSES 2332/2332**, ratio median 0.1596, p99 1.573, **max 2.034** of a 4-ulp budget
(n=2332, median speed 726.724, `d` = -890..+1443; 890 rows through the zero branch).
**Gate D-0 PASSES**: 0 differing (line, token) pairs over 1625 shared lines.

**The diverging term.** Gate **DR** names **`speed` `+0x9e4` at `d = 0`** (ORIG `0` exact on
890 consecutive rows, PORT `0.65`); `fwdlen` — the only term ordered before it — **never**
exceeds 2 % anywhere in `d = 0..400`. `+0xb0c` first exceeds at `d = 1` (O `0`, P
`0.00831162`), **after** speed, which is the point: it is a **SYMPTOM**. Gate **CB** is
CB-LARGE over `d = 0..400` (66.65 % at `d = 372`) but **3.9892 % restricted to `d = 0..101`**
(n=102), where the two arms' **median speed differs by 0.06 %** (609.07 vs 609.43).

**Fix, and promotion evidence.** **NONE AUTHORED.** No faithful single-producer change
exists: `+0xb0c`'s law is already exact where it could matter, and the only available forms
would be a clamp or a fitted constant, both forbidden. The only `mashedmod/src` change is a
diagnostic `fprintf`, so **`NEW = 0` by construction** and **no C-level moved**
(`0x00470670` stays C3/C2 on its own prior evidence; `0x00467650` untouched). Two tracker
rows opened via `re-classify`: **U-9176** (both port copies use a different x87 association
order than `0x004706db..0x00470701` — float-rounding sized, named, located, left for an
attempt that can carry the full promotion leg on A4) and **U-9177** (the relocated defect).

**STEP 4, three runs, §16.7 arm, participants=1 confirmed from the game's own `MATCH-SEED`
line; `p1`/`p2`/`p3` identical to every digit.** The release frame had to be re-derived:
`a8_launch.py`'s `886` is capture-specific, `orig_sl1.msd` carries `+0xbf8 != 0` at **890**,
and only `R = 890` reproduces the published numbers.
**(a)** `+0xb0c` within tolerance at matched `d`: **FAIL** — first exceeds 2 % at `d = 1`;
no fix was applied, and §1.2/§3 establish it as a symptom.
**(b)** launch: **PASS** — **L = 0 at 0.19 %**, `+0xb14` engages at `d = 15` on both arms,
peak **1835.50 at `d` = 95** vs **1832.40 at `d` = 95**.
**(c)** recovery H1: **INCONCLUSIVE** — **243/400 = 60.8 %** (fraction passes), median
**132.8** (median fails ≥900), vs 398/400 = 99.5 % and 1333.9.
**(d)** metrics vs the unchanged `d81a8df6` bounds: **FAIL 3 of 3** — slip 1500-2000
**0.1983** (n=19, median speed 1665.05, `d`=85); slip 2000-2600 **UNSCORABLE** (n=0);
driving-median **1019.77** (n=76, median speed 1019.77, `d`=79). §26.10's median-frame guard
fires on all three: the ORIGINAL populates them at median `d` **718 / 915 / 835**.

**Next term — named, reported UNFIXED [U-9177].** First durable divergence **`d = 222`**.
Over **`d = 222..250`** (n=28, median speed O **805.7** / P **660.9**): drive force agrees to
**6.2 %** (median `|b14_xz|` 2.381e6 vs 2.233e6), **all four wheels grounded on both sides**
(`+0x9e0 = 4`, exact, every frame), both cars aligned and in the same gear — and the median
**per-frame speed gain is +27.87 against +4.86, a 5.7x deficit**. At `d = 200..222`
immediately before, the PORT is **faster** (+19.06 vs +17.59). So the defect is in what
CONSUMES `+0xb14`/`+0xb1c` — the velocity integration and its clamp chain — not in the
force, not in the contact state, not in `+0xb0c`. The gearbox collapse (port in gear 0 for
283 of 321 frames over `d = 200..520`) **follows** it; gear and timer track exactly through
`d = 0..250`. Unfixed because live confirmation on the original has not been run.

**Collateral.** Leg 1, paired same-side (attempt-15 `r1` vs this build, floors `r2`/`p2`):
**0 of 69 paired fields divergent, all 69 within the measured noise floor on every one of
1626 aligned frames**; unpaired B-only is **exactly** the 6 new diagnostic fields
`vel[0..2]`/`fwd[0..2]`. **No outside-scope rows.** Leg 2, cross-side banded (14 fields
mapped by name): **all SIX bands are `!!` OFF-REGIME** (median frame indices 1023-1556 vs
68-381), so per §26.10 **not one row was read and none is reported** — which is §4's finding
restated. Three fields are within floor in every band and exact on both sides: `msd+0x498`
(40000), `msd+0x49c` (4000), `msd+0x9e0` (4).

**Commits:** `78314357` (STEP 1 + `PREREG_STEP2.md`, unrun), `d9db5e80` (KA fails the
snapshot route; `--slide-probe` + `a17_slide.py` + the diagnostic field, unrun),
`683833a2` (KA fails live too; `PREREG_STEP2B.md`, unrun), `3ece6283` (STEP 2 run,
`RESULT_STEP2.md`), and the handoff commit (U-9176/U-9177, CHANGELOG, NEXT_SESSION,
ROADMAP §D2, info pane, this file). Nothing pushed.

**Still open:** **U-9177** — which term in the velocity integration loses the gain; that is
now D2's blocker. U-9176; U-9173; U-9156; U-9160; U-9171; §20.14's `-0.1` duty cycle;
D1-residue R1. **`+0xb0c` is closed.** AI slots 1+ keep the fitted seed at
`VehiclePhysicsRun.cpp:702` — not touched, D3 work. D3 modes 3/7 hold stands; D2 must close
before it starts.
