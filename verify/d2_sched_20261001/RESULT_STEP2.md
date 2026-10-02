# D2 attempt 14, STEP 2A — RESULT

Pre-registration: [`PREREG_STEP2.md`](PREREG_STEP2.md), committed as `386cf0c6` **before** any
reduction run and before the one game launch this step used. **Not amended.** Raw output:
[`step2a_scan.txt`](step2a_scan.txt), [`step2a_launch.txt`](step2a_launch.txt),
[`step2a_bounce.txt`](step2a_bounce.txt), [`step2a_lag.txt`](step2a_lag.txt),
[`step2a_gate.txt`](step2a_gate.txt), [`step2a_bf8_scan.txt`](step2a_bf8_scan.txt),
[`step2a_bf8_disasm.txt`](step2a_bf8_disasm.txt).

---

## 1. THE GATES. GB, GC, GD passed. **GA FAILED AS WRITTEN**; its own registered remedy was executed.

| gate | bar | measured | verdict |
|---|---|---|---|
| **GD** window populated | >= 131 frames at `d >= 0` on both arms | orig **1447**, solo4 **1447**, port **1626** | **PASS** |
| **GB** floor usable | finite `floor99` on >= 90 % of fields | **48 of 48** finite; **every one is exactly 0** | **PASS** |
| **GC** passing baseline | arm B = `orig_solo4.msd` reports **no** divergence in T0-T4 in `d`[0,130] | **no divergence anywhere**, any tier | **PASS** |
| **GA** phase robustness | the rule names the same tier **and** field under `phi = 0` and `phi = +1` | `phi=0` -> **T2 `b14`**; `phi=+1` -> **T1 `b0c`** | **FAIL** |

**GA's registered remedy, executed verbatim** (`PREREG_STEP2.md` §6: *"the lane stops until the
phase is resolved by direct measurement"*). One port run on the §16.7 arm with the
`MASHED_A6ADUMP` channel on (`verify/d2_sched_20261001/ph1/`, muted,
`MASHED_TITLE=d2a14-phase-ph1`, `MASHED_WIN_POS=primary-bl`, own PID only, `participants=1`):

- `a6a_dump.log` `f=1` has `snap.sp = 216.66841125488281`; `motion_diag.log` line 1 has
  `sp=216.67`. `f=2` has `snap.sp = 13.973201751708984`; line 2 has `sp=13.97`. **The two port
  channels share one index**: `a6a f=N` <-> `motion_diag` 0-based frame `N-1`.
- `a6a f=2` is the first frame with `snap.steer != 0` (`= F(r,0x1a8)`,
  `VehiclePhysicsRun.cpp:1304`, declared at `:1284-1285` to be read *"at the render-tick phase
  the original's `.msd` capture sees them"*). That is `motion_diag` 0-based frame **1**, which
  is `R_port`.
- **Therefore `phi = 0`, measured.** The port's `motion_diag` frame `R_port + d` and the
  original's `.msd` frame `R_orig + d` are the same phase of the same release-relative frame.

Determinism of that run: `ph1/motion_diag.log` is **bit-identical to attempt 13's
`sc1/motion_diag.log` on all 1627 shared lines** (it has one extra line because `sc1` was
killed a frame earlier). The `A6ADUMP` channel does not perturb the arm, and §26.1's
exactly-zero port noise floor reproduces on this attempt's own captures.

> **The rule is therefore read at `phi = 0` only.** It was not re-run, re-scoped or re-tuned.

---

## 2. THE FIRST DIVERGING TERM: **T2, `+0xb14` / `+0xb18` / `+0xb1c`, at `d = 0`**

At `d = 0` every T0 and T1 field is inside tolerance (`gear`, `gtmr`, `gt[0..5]`, `gb498`,
`gb49c`, `b0c` all equal; `b0c` first diverges at `d = 1`), and the three T2 fields are not:

| field | ORIG `d=0` | PORT `d=0` | ratio to floor |
|---|---:|---:|---:|
| `+0xb1c` | `0` | `-763499` | 432386.5 |
| `+0xb14` | `0` | `-238892` | 94682.9 |
| `+0xb18` | `0` | `0.0216498` | 21649.8 |

### 2a. It is an ENGAGEMENT LATENCY, not a magnitude error

| | first `d` with `\|b14\|> 1e-6` | value there |
|---|---:|---|
| ORIGINAL | **15** | `(-265866, 0, -762420)` |
| PORT | **0** | `(-238892, 0.0216498, -763499)` |

`port(d=0) / orig(d=15)` = **x 0.8985**, **z 1.0014**. The port's drive accumulator has
**the original's magnitude, 15 frames early.**

### 2b. The whole launch is the original's, shifted 15 frames, to **1.44 %**

Best-fit integer lag `L` minimising the median `|orig(d) - port(d-L)| / orig(d)` over
`d = 16..95` (the original's monotone accelerating phase up to its peak), `n = 80`:

| L | 12 | 13 | 14 | **15** | 16 | 17 | 18 |
|---|---:|---:|---:|---:|---:|---:|---:|
| median rel err | 13.23 % | 9.04 % | 5.17 % | **1.44 %** | 1.83 % | 5.03 % | 8.38 % |

A single sharp minimum at **L = 15**, p90 **1.65 %**. Across the whole 80-frame launch the
residual is flat, not growing: `-4.01 %` at `d=16`, `+0.18 %` at `d=26`, `+1.66 %` at `d=51`,
`+1.32 %` at the original's peak `d=95`. The same `L = 15` on the other launch channels
(median `|rel err|`, `n=80`): `w1b[0]` **0.01 %**, `horiz` **1.20 %**, `wf[5]` **1.96 %**,
`b14[0]` **2.01 %**, `b14[2]` **3.03 %**, `wf[1]` **6.15 %**.

> **The port's launch physics is faithful.** Three independent measurements give the same 15:
> the `+0xbf4` countdown (`3000 / 200` per frame), the `b14` engagement frame, and the
> speed-profile lag fit.

And the same lag explains **nothing** after the crash: `d = peak+10 .. peak+400`, `n = 391`,
median `|rel err|` **97.4 %**, p90 **99.4 %**. The shift is a launch-phase statement only.

### 2c. The MECHANISM, proven on the ORIGINAL's own captures before any code was considered

`+0xbf8` is a boost state machine; its `== 2` arm `0x00467def..0x00467e44` **zeroes the drive
accumulator** and runs a timer down:

```
00467def  cmp   dword ptr [esi + 0xbf8], 2
00467df6  jne   0x467e4e
00467df8  mov   eax, dword ptr [esi + 0xbf4]
00467e04  je    0x467e44                      ; bf4 == 0 -> leave state 2
00467e0a  xor   eax, eax
00467e0c  mov   dword ptr [esi + 0xb1c], eax  ; <-- ZEROES the drive accumulator
00467e12  mov   dword ptr [esi + 0xb18], eax
00467e1f  mov   dword ptr [esi + 0xb14], eax
00467e18  fsub  dword ptr [esp + 0xf8]        ; bf4 - dt
00467e25  call  0x4a2c48                      ; _ftol2
00467e2c  mov   dword ptr [esi + 0xbf4], eax
```

Transcribed in the port at `mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:357-372`.

**Measured on `orig_solo3.msd`, `orig_solo4.msd` and `orig_fp1.msd` — three captures, every
digit identical:**

| `d` | -1 | **0** | 1 | ... | 13 | **14** | **15** |
|---|---:|---:|---:|---|---:|---:|---:|
| `+0xbf8` | 0 | **2** | 2 | 2 | 2 | **0** | 0 |
| `+0xbf4` | 3000 | 2800 | 2600 | `-200`/frame | 200 | **0** | 0 |
| `+0xb14` | 0 | 0 | 0 | 0 | 0 | 0 | **-265866** |
| `+0x9e4` | 0.000 | 0.000 | 0.637 | | 2.003 | 2.115 | **14.935** |

- `+0xbf8` takes exactly **two** values in the whole 2333-frame capture: `0` and `2`. It is `2`
  on exactly **14** frames, `d = 0..13`, starting on the release frame.
- The gate identity **`(+0xbf8 == 2) <=> (+0xb14 == 0)`** holds on **1446 of 1447** frames
  after release. The single exception is `d = 14`, where `+0xbf8` has already been cleared but
  `b14` is still 0 — exactly the store-then-test ordering at `0x00467e2c`/`0x00467e32`.
- `3000 / 200 = 15`. The timer *is* the latency.

> **So the ORIGINAL spends its first 15 frames after the green light in boost state 2, with its
> drive force held at exactly zero, and the PORT does not.**

### 2d. The port's side, and the `[UNCERTAIN]` that blocks a fix

`Integrate2.cpp:358-364` already records that **nothing in the standalone sets `+0xbf8 = 2`**,
so that arm is unreachable there. Re-checked here against `MASHED.exe.unpatched` rather than
inherited: a byte scan for the displacement `F8 0B 00 00` found **7** occurrences in the file,
**6** in `.text`, and each was **disassembled** rather than assumed (memory
`capstone-sweep-stops-at-bad-byte`). The 7th, at `0x0050312e`, decodes as
`c7 45 f8 0b 00 00 00` = `mov dword [ebp-8], 0xb` — a local, **not** a `+0xbf8` reference, and
is discarded. The six real ones:

| RVA | instruction | writes |
|---|---|---|
| `0x00467d2e` | `cmp dword ptr [esi + 0xbf8], 1` | — (read) |
| `0x00467dd7` | `mov dword ptr [esi + 0xbf8], eax` | **0** (`xor eax, eax` at `0x00467dd5`) |
| `0x00467de5` | `mov dword ptr [esi + 0xbf8], 0` | **0** |
| `0x00467def` | `cmp dword ptr [esi + 0xbf8], 2` | — (read) |
| `0x00467e36` | `mov dword ptr [esi + 0xbf8], eax` | **0** (`xor eax, eax` at `0x00467e34`) |
| `0x00467e44` | `mov dword ptr [esi + 0xbf8], 0` | **0** |

> **Every writer of `+0xbf8` reachable through a literal displacement writes ZERO.** Nothing in
> the image sets it to 1 or 2 that way. Yet the record reads `2` for 14 frames on three
> captures. **[UNCERTAIN U-9174] The writer that sets `+0xbf8 = 2` at the green light is NOT
> LOCATED.** It must use a computed base — memories `findoffset-blind-to-computed-bases`
> ("proves addressing mode, not absence of a writer") and `offset-grep-misses-dword-index`.

**Therefore NO FIX IS AUTHORED.** The mechanism is proven and the magnitude is right, but the
trigger is unlocated, and a port-side trigger fitted to "the frame control is enabled" would be
a **fitted** condition, not a transcribed one. That is exactly what the NO-GUESSING rule
forbids, and §26.10's lesson is that a wrong finding stays cheap only while nothing is built on
it. The next command is in §5.

---

## 3. THE REFRAMING, and it is bigger than the named term

Release-aligned, frame-indexed, no band anywhere:

| | peak `+0x9e4` | at `d` | trough | at `d` | frames after trough | of those `>= 100` | median | max |
|---|---:|---:|---:|---:|---:|---|---:|---:|
| **ORIGINAL** | **1832.40** | 95 | **85.45** | 101 | 1346 | **1313 (97.5 %)** | **1828.5** | 2562.8 |
| **PORT** | **1856.57** | 80 | **85.81** | 139 | 1488 | **0 (0.0 %)** | **24.7** | **91.4** |

- The two peaks are **1.3 % apart**, and the port's comes **15 frames earlier** — the same 15.
- **Both sides crash.** The original's first wall hit is its frame 980 = `d = 94` (§16.6's
  twelve-hit list); the port's is its frame 81 = `d = 80` (§20.14). Both lose ~95 % of their
  speed within ~6 frames of their peak.
- The original is back over **2000** by `d = 300` and holds a median of **1828.5** for the
  remaining 1346 frames. **The port's maximum over the 400 frames after its trough is 91.36**,
  and it never again exceeds 100 in 1488 frames.

> **D2's defect is not the launch and not the crash. It is the RECOVERY.** Both arms accelerate
> the same, reach the same speed, and hit the wall. The original drives away; the port does
> not.

This is §20.14's loop (`bounce -> velocity anti-parallel to the nose -> the `-0.1` gate latches
-> steer inverted -> nose turns into the wall -> bounce`, duty cycle **25x**) with a hard
frame-aligned count attached for the first time. §20.14 is **not** superseded; it is
**quantified**.

## 3b. What this costs the three previous attempts

Everything measured inside A6a on this arm — `l_60`, `ld4`, `grip*speed`, the clamp arms, the
wheel axes — was measured at or after the crash, where the port holds a median of 24.7. The
launch, which is the only stretch where the two sides are in the same regime, is faithful to
**1.44 %**. Attempt 13 §26.9 called those bands off-regime; this step says **what the regime
actually is**, and it is 80 frames long.

---

## 4. THE SCORED ARM — a no-change control, 3 of 3, with median frame index on every row

No physics source was changed in this attempt. Build not re-run because nothing was edited;
the `.exe` used is proven identical to attempt 13's by the bit-identical `ph1` vs `sc1` diff.
`participants=1` in `mashed_re.log` on every run (memory
`verify-the-harness-knob-actually-took`). Arm §16.7, muted, `MASHED_WIN_POS=primary-bl`,
`MASHED_TITLE=d2-a14-s{1,2,3}`, own PIDs only.

| metric | PASS interval (§3a, unchanged) | **port** | n | median speed | **median frame** (`d`) | verdict |
|---|---|---:|---:|---:|---:|---|
| slip 1500-2000 | 0.18855 .. 0.19635 | **0.2033** | 20 | 1683.5 | **71** (`d=70`) | **FAIL** |
| slip 2000-2600 | 0.24488 .. 0.25487 | **UNSCORABLE** | **0** | — | — | **FAIL** |
| driving-median | 1904.70 .. 1982.44 | **1355.66** | 54 | 1355.7 | **54** (`d=53`) | **FAIL** (-30.2 %) |

Identical to every printed digit on runs 1, 2 and 3, so §3b's determinism precondition is
satisfied (3 of 3 agree exactly). Identical to attempts 11, 12 and 13.

The same rows on the ORIGINAL, for the median-frame guard: slip 1500-2000 `n=315`, median speed
1789.9, **median frame 1605 (`d=719`)**; slip 2000-2600 `n=529`, median speed 2227.1, **median
frame 1794 (`d=908`)**; driving-median `n=1208`, median speed 1896.7, **median frame 1683
(`d=797`)**.

> **The scored metrics themselves fail §26.10's median-frame guard** — `d=719/908/797` against
> `d=70/—/53`. The port populates these bands **only during its 15-frame-early launch**, and
> the original populates them only long after its recovery. The §3 bounds are **not
> renegotiable** and are not touched; this is recorded as a property of the arm that the next
> attempt must confront. **D2 stays REOPENED. FAIL, and no fix authored.**

---

## 5. THE COLLATERAL REVIEW (memory `collateral-review-after-every-attempt`)

Standing tool `re/tools/statediff/collateral.py`, this attempt's own captures, 45 paired fields
mapped port->record offset, `--scope re/tools/statediff/scope_a6a.txt`, floors from a same-arm
repeat on **both** sides (`orig_solo4.msd`; `s2/motion_diag.log`).

### 5a. Cross-side `--mode banded` — **ZERO readable rows**

[`collateral_cross.txt`](collateral_cross.txt) / [`.csv`](collateral_cross.csv). 35 of 45
paired fields exceed their floor in some band. **Every band of every field is `!!`-flagged**:
median frame A **988-1554** against median frame B **22-130**, in all 7 bands from 70-100 to
1500-2000. Per §26.10's guard and `PREREG_STEP2.md` §8, **not one row is read**. The port floor
is **exactly 0** on this attempt's own captures, reproducing §26.1.

> That is the result, not a failure to produce one: **on this arm the cross-side speed-banded
> review has no readable content at all.** Only release-aligned frame-index comparison works.

### 5b. Lag-anchored `--mode paired` — `--anchor msd+0xb14:ne:0`

[`collateral_lagaligned.txt`](collateral_lagaligned.txt) / [`.csv`](collateral_lagaligned.csv).
The anchor independently reproduces §2a: **arm A rebases at frame 901 (`d = 15`), arm B at
frame 2 (`d = 0`)**. 1432 aligned frames, 32 of 39 paired fields divergent.

**Within the noise floor on every one of the 1432 aligned frames — 7 fields:** `+0x1a0`,
`+0x264`, `+0x328`, `+0x3ec` (the four per-wheel `fl` ints), `+0x498`, `+0x49c` (the gearbox
upshift inputs, `40000` / `4000` on both), `+0x9e0` (ground contact, `4` on both).

**OUTSIDE-SCOPE divergent rows** — the point of the exercise, reported, **not acted on**:

| field | scope | med A | med B | note |
|---|---|---:|---:|---|
| `+0xb0c` | outside | 38.48 | 7.396 | during the launch specifically it is ORIG `~1e-5` vs PORT `0.59` — four orders of magnitude, and it is the first **T1** divergence (`d = 1`) |
| `+0x210` / `+0x2d4` / `+0x398` / `+0x45c` | outside | 1090 / 1082 / 1085 / 536.6 | 1084 / 1084 / 1082 / 540.8 | `w1b`, within 0.5 % |
| `+0x220` `+0x228` `+0x2e4` `+0x2ec` `+0x3a8` `+0x3b0` `+0x46c` `+0x474` | outside | — | — | `wax`, the wheel axes; §26.9 already proved these follow each side's **own** steer angle exactly |
| `+0x1f8` / `+0x1fc` | outside | 0.15 / 0.0125 | 0.15 / 0.0125 | differ by `6e-9` / `1.9e-10`, i.e. equal |

**Caveat on 5b, stated rather than hidden:** the 1432-frame window spans both the faithful
launch and the post-crash regime, so these medians **mix two regimes** and no single number in
the table is a clean statement about either. They are listed as a map of what moved, which is
all a collateral review claims. **`+0xb0c` is the one row that bears on the named term, and it
must be pre-registered before anything is built on it.**

---

## 6. WHAT IS OPEN, and the next command

1. **[U-9174] Locate the writer of `+0xbf8 = 2`.** Six literal-displacement sites exist and all
   write 0, so the writer uses a computed base. A literal scan **cannot** find it; the
   registered next instrument is a **hardware write-watchpoint on `record + 0xbf8`** across the
   original's green light (note: this is **not** an `Interceptor` entry hook, so the session
   rule "Frida entry hooks only" has to be settled with the user before it is run), or a
   Ghidra data-xref pass once the MCP is back up — it was **down for this whole session**.
   Until then the 15-frame latency is **measured but not portable**.
2. **The recovery, which is D2's actual defect.** Both arms crash from the same speed at the
   same release-relative moment; the original recovers to a median of 1828.5 and the port's
   ceiling is 91.4. §20.14 named the loop and measured its 25x duty cycle. The next lane is
   that loop, frame-indexed, **not** another quantity inside A6a.
3. **`+0xb0c`**, the first T1 divergence (`d = 1`, ORIG `~1e-5` vs PORT `0.59` during the
   launch). EXPLORATORY; pre-register before acting.
4. Carried: **U-9173** (the clamp-#6 three-way collision), **U-9156**, **U-9160**, **U-9171**,
   D1-residue R1.

## 7. The instrument lessons this step paid for

**Resolve a channel-phase question with a channel that carries both quantities on one line.**
GA's two-phase run was the right guard and it fired correctly. The resolution cost one game run
because the port's `A6ADUMP` writes `+0x1a8` and the velocity from the same record read, which
is what makes the pairing exact. The `.msd` always had that property; the port's `motion_diag`
did not.

**A tolerance has no concept of latency.** The registered rule named `b14` at `d = 0` and was
right to, but "diverges" was the whole of what it could say. The 15-frame engagement delay, the
matching magnitude, and the `3000/200` timer behind it all came from reading the named field's
**trajectory**, not its value at the flagged frame. A first-divergence rule should be read as
"look here", never as "this is the error".

**A byte search finds candidates; only a decode finds instructions.** One of the seven
`F8 0B 00 00` hits was `mov [ebp-8], 0xb`. Had it been counted, "`+0xbf8` is written outside
A6a" would have been published.
