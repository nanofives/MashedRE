# The build is SSE2, not x87 — a load-bearing comment is false (2026-09-10)

**DECISION REQUIRED — architecture-level, not applied.** This note measures the problem and
sizes it. It does not change `build.bat`.

## What the code claims

`mashedmod/src/mashed_re/Collision/RwpSolverIntegrate6.cpp` header, and the same claim in
`Ai/AiTargeting.cpp`:

> x87 note: 80-bit ST0 chains carry the accepted ≤1-ULP angular floor under MSVC's 64-bit
> long double. **Build is x87 (no /arch:SSE2)**, so the plain-float product/sum expressions below
> accumulate in 80-bit and round once per store, matching the original FMUL/FADDP/FSTP stream.

Every "verbatim x87 transcription" in the physics ports rests on that sentence.

## It is false, on both counts

**1. There is no `/arch:` flag anywhere in the build.** The only occurrence in
`mashedmod/build.bat` is a *comment* at line 26 about the qhull static lib; neither the exe line
(194) nor the asi line (214) nor any of the per-TU lines (43, 45, 81–93) carries one:

```
cl /nologo /EHa  /W3 /O2 /DMASHED_STANDALONE ...        (exe)
cl /nologo /EHsc /W3 /O2 /LD ...                        (asi)
```

MSVC's x86 default has been `/arch:SSE2` since VS2012, so the port compiles **to SSE2**, which is
the exact opposite of the comment.

**2. The generated code confirms it.** `FUN_0055b750_impl` in the shipped
`mashed_re_dev.asi` (resolved via `mashed_re_dev.map`, `?FUN_0055b750_impl@...` at `0x10048bf0`):

```
10048C0C  movss    xmm7,dword ptr [eax]
10048C22  cvtps2pd xmm0,xmm0
10048C39  subsd    xmm7,xmm0
10048C66  mulsd    xmm2,xmm7
10048C78  subsd    xmm1,xmm0
10048C80  cvtpd2ps xmm0,xmm1
10048C84  addss    xmm0,dword ptr [esi]
```

Instruction census over that function: **13 `movss`, 10 `cvtps2pd`, 6 `subsd`, 6 `mulsd`,
3 `cvtpd2ps`, 2 `addss` — and zero x87 instructions.**

Compiling the *same source file* with `/arch:IA32` added and nothing else changed
(`/nologo /EHsc /W3 /O2 /arch:IA32 /c /FAcs`, output to scratch — `build.bat` untouched) gives
the x87 family instead: **11 `fld`, 5 `fstp`, 3 `fxch`, 3 `fsub`, 3 `fmul`, 3 `fadd`,
1 `fsubp`.** So the flag is the whole difference.

## Consequences

1. **`float10` is a 64-bit double computed in SSE2.** `typedef long double float10`
   (`RwpSolverBroadphase3.cpp:87`) and MSVC's `long double` == `double`. So a chain the original
   keeps in 80-bit x87 registers is evaluated at a **53-bit mantissa** in the port. The
   `DIVERGENT_FLOAT10` label the lane already uses is accurate — but its cause is the *build*,
   not the transcription.
2. **Plain `float` expressions do NOT accumulate in 80-bit.** Under SSE2, `mulss`/`addss` round
   to 32 bits after *every* operation. The original's `FMUL`/`FADDP` chains hold intermediates at
   80 bits and round once at the `FSTP`. So a multi-term float expression transcribed
   instruction-for-instruction from an x87 stream will still differ wherever intermediate
   rounding matters. That is the opposite of what the comment promises, and it is a systemic
   accuracy gap, not a per-function bug.
3. Association still matters, so the corrections that *were* made stay valid — `0x0056bce0` went
   48/48 CLEAN only after its denominator tree was fixed (`bce0_edx_contract_20260910.md`), and
   under SSE2 each node rounds, so getting the tree right is if anything *more* important.

## Worked case: `0x0055b750`, DIVERGENT 13/48

The divergence lands at region `+04` (×6) and `+08` (×6), both once — and **never at `+00`**.
Traced the original at `0x0055b750..0x0055b7fc` and the port matches everywhere I can check:

- args: `[esp+0x10]`→arg1, `[esp+0x1c]`→arg2, `[esp+0x20]`→arg3 (accounting for
  `sub esp,0xc` + 2 pushes) — all three map onto `(int*, float*, float*)`.
- `s0 = r5*d2 − r6*d1` (`0x0055b787..0x0055b791`), `s1 = r6*d0 − r4*d2`
  (`0x0055b797..0x0055b7a1`), `s2 = r4*d1 − r5*d0` (`0x0055b7a9..0x0055b7b3`) — all three
  formulas and operand orders identical to the port.
- the rounding map: `s0`/`s1` are `fstp dword` to `[esp+8]`/`[esp+0xc]`, i.e. rounded to float;
  `s2` survives via `fstp st(2)` + `fstp st(0)` and is added at 80 bits by
  `0x0055b7f2 fadd st(1)` — which is exactly what the port encodes (`float s0`, `float s1`,
  `float10 s2`).
- the per-component pointer **re-derivation** at `0x0055b7c5` and `0x0055b7e0` — the port
  reproduces it verbatim.

**The discriminator is `d0`.** `s0` is the only output that does not use it, and `+00` is the
only offset that never diverges; `s1` and `s2` both use `d0` and both diverge. `d0` also has the
longest live range — the original holds it in `ST2`/`ST3` untouched from `0x0055b774` all the way
to `0x0055b7b1`. In the port it is a `float10` local, so it lives at 53 bits and every product
built from it is a `mulsd`. Nothing in the transcription is wrong; the **float model** is.

So `0x0055b750` should be reclassified `DIVERGENT_FLOAT10` (build-caused), not chased as a
transcription defect — which was the plan recorded under item C in `re/NEXT_SESSION.md`.

## Blast radius

| measure | count |
|---|---:|
| TUs containing `float10` | **85** |
| `hooks.csv` rows whose `file` is one of those TUs | **88** (C4 10, C3 76, C2 2) |
| `DIVERGENT*` rows in `re/parity/shadow_results.tsv` | **17** (5 already labelled `_FLOAT10`) |

## DECIDED 2026-09-10 (owner instruction): per-TU `/arch:IA32`, measured first

The decision was made **from a measurement, not from the argument below.** Recompiled
`RwpSolverBroadphase3.cpp` alone with `/arch:IA32` (per-TU object swap + relink, `build.bat`
untouched) and re-ran every shadow site in that TU:

| RVA | before | after |
|---|---|---|
| `0x0055b750` | DIVERGENT **13/48** | **CLEAN 48/48** |
| `0x0055c2d0` | DIVERGENT **5/24** | **CLEAN 24/24** |
| `0x0055a1f0` | CLEAN 48/48 | CLEAN 48/48 (no regression) |
| `0x0055bae0` | CLEAN 48/48 | CLEAN 48/48 (no regression) |

**2 divergences fixed, 0 regressions.** Codegen for the TU went from
13 `movss` / 10 `cvtps2pd` / 6 `subsd` / 6 `mulsd` to 11 `fld` / 5 `fstp` / `fmul` / `fadd` /
`fsub` / `fxch`, i.e. the instruction family the original uses.

Note `0x0055c2d0` also cleared, which the "`/GS` frame artefact" reading above did not predict —
that reading correctly explained *why it diverged under SSE2 codegen*, but the artefact does not
survive the x87 build. Recorded rather than quietly dropped.

### What was implemented

**Option 3 — per-TU, not global.** `/arch:IA32` globally would disable SSE2 for librw (the
shipping renderer, gate D2) and every non-physics TU: a broad performance and codegen change with
no measured benefit outside the physics lane.

- `mashedmod/build_objs.ps1` gained **`-X87List <file>`**: TU basenames that get `/arch:IA32`
  appended. Implemented as a second `cl` invocation because `/MP` shares one flag set across its
  response file. **The list's content is part of the flag stamp**, so moving a TU in or out
  invalidates the object cache exactly like a flag change.
- `mashedmod/x87_tus.txt` holds the set, with the measurement and caveats inline.
- **Both** targets are wired, deliberately: all 10 TUs appear in `exe_sources.rsp` *and*
  `asi_sources.rsp`, so compiling them SSE2 in the exe while the `.asi` used x87 would make the
  two targets compute physics differently — an A/B verified under the `.asi` would not transfer
  to the standalone.

### The 10-TU trial, and why the list is now 4 — the pilot did NOT generalise

The first list was the pilot TU plus the 9 TUs holding the 15 remaining `DIVERGENT*` rows.
Measured it properly: **all 47 affected rows** (the 15 divergences plus the **32 CLEAN rows in
those same TUs**, i.e. the regression surface) at `--group 1`, 47 boots. Result:

```
CLEAN=28  CRASH=7  DIVERGENT=3  DIVERGENT_FLOAT10=2  VOID=3
```

Against the pre-change verdicts: **4 divergences fixed, 4 divergences turned into CRASHes, and
6 of the 32 CLEAN rows stopped being CLEAN** (3 CRASH, 3 VOID). Net: 7 new crashes and 3 VOIDs
against 4 fixes. **A blanket application would have been a regression**, and the pilot's
"2 fixed, 0 regressions" did not carry to the other nine TUs.

Attributing every outcome to its TU separates benefit from damage cleanly:

| TU | fixed | held | still-div | CRASH | VOID | |
|---|--:|--:|--:|--:|--:|---|
| `RwpSolverBroadphase3` | 2 | 2 | 0 | 0 | 0 | **keep** (pilot) |
| `RwpSolverCore17` | 2 | 1 | 2 | 0 | 0 | **keep** |
| `RwpSolverCore14` | 1 | 2 | 1 | 0 | 0 | **keep** |
| `RwpSolverCore15` | 1 | 0 | 0 | 0 | 0 | **keep** |
| `RwpSolverCore16` | 0 | 2 | 2 | 0 | 0 | drop — buys nothing |
| `RwpSolverCore18` | 0 | 4 | 0 | 1 | 0 | drop — damage |
| `RwpSolverCore19` | 0 | 1 | 0 | 1 | 0 | drop — damage |
| `RwpSolverCore20` | 0 | 1 | 0 | 1 | 0 | drop — damage |
| `RwpSolverCore23` | 0 | 7 | 0 | 3 | 0 | drop — damage (2 were CLEAN) |
| `RwpSolverLeaves1` | 0 | 8 | 2 | 1 | 3 | drop — damage (4 were CLEAN) |

**Final list: 4 TUs — 6 divergences fixed, 0 regressions.** This is also the argument for having
built the mechanism per-TU rather than flipping a global flag: the per-TU list is what let the
measurement pick winners instead of forcing an all-or-nothing choice.

### CORRECTION to the table above: the "damage" column is NOT established

Re-ran the 4-TU list over the 14 keeper rows and the 10 rows the trial had damaged. Two things
came out that undo part of my own reasoning:

1. **Only 4 of the 10 damaged rows recovered**, even though their TUs are now back to SSE2. If a
   row's TU is compiled SSE2 and it *still* fails, the flag never caused that failure.
2. **The failures come in consecutive runs** — positions 4-5-6 and 15-16-17 of 24 — with no
   batch-position trend (mean failure position 10.1 against a midpoint of 12, so it is not
   degradation over the run either).

And the clincher, checking which TU each failure belongs to:

| pos | rva | TU | on x87 list? | verdict |
|--:|---|---|---|---|
| 4 | `0055bae0` | `RwpSolverBroadphase3` | **yes** | NO_SAMPLES |
| 5 | `0055c2d0` | `RwpSolverBroadphase3` | **yes** | CRASH |
| 6 | `0055fea0` | `RwpSolverCore23` | no | CRASH |
| 8, 10 | `00563f60`, `00565120` | `RwpSolverLeaves1` | no | VOID |
| 15, 16, 17 | `00574ad0`, `005752b0`, `00575560` | Core18/19/20 | no | CRASH |

**6 of the 8 failures are in TUs compiled SSE2**, and the two x87-TU failures sit in the same
consecutive window as an SSE2 one. **A failure window that hits x87 and SSE2 TUs alike cannot be
attributed to the flag.**

So the trial's per-TU "damage" attribution was **over-read**. Dropping those 6 TUs still costs
nothing — none of them fixed anything — but the reason is "no measured benefit", **not** "measured
damage". Corrected here rather than left standing.

### What actually survives repetition

| RVA | TU | evidence |
|---|---|---|
| `0055b750` | Broadphase3 | DIVERGENT 13/48 → **CLEAN 48/48, twice** (pilot + narrowed) |
| `00577be0` | Core14 | DIVERGENT 38 → **CLEAN, twice** (trial + narrowed) |
| `00576880` | Core15 | DIVERGENT 2 → **CLEAN, twice** |
| `00578b20` | Core17 | DIVERGENT_FLOAT10 26 → **CLEAN, twice** |
| `00578bd0` | Core17 | DIVERGENT_FLOAT10 15 → CLEAN (**one** run) |
| `00577cb0`, `00578cb0`, `00578ff0` | Core14/17 | still DIVERGENT, consistently — the flag does not fix everything |

**Four repeated DIVERGENT→CLEAN conversions, one single-run one, no repeated regression in a
keeper TU.** That is the basis for keeping the list, and it is deliberately a weaker claim than
the "6 fixed, 0 regressions" I wrote an hour ago.

### The caveat that outlives this decision

**The lane produces transient windows of consecutive failing boots, hitting x87 and SSE2 TUs
alike.** A single-boot verdict is therefore not reliable evidence about a row. That applies beyond
`/arch`: several of this session's earlier conclusions rest on one boot each. The ones reproduced
across two independent runs — `0x0056bce0` 48/48, `0x00560260` twice, `0x005a6e10` twice,
`0x0055b750` twice — are solid; the single-boot ones are weaker than they were presented.

**Top open question for this lane:** what causes the consecutive-failure windows. Until that is
understood, promotion evidence should require two independent boots, not one.

**Adding a TU later:** measure it. Run its rows before and after; keep it only if it fixes
something and regresses nothing. The list's content is in the object-cache flag stamp, so editing
it forces a full rebuild of both targets. The other ~16 `Collision/` TUs carrying `float10`/x87
markers remain unlisted.

### Standalone side: partly verified, and the gap is stated

The 10 TUs are in `exe_sources.rsp` too, so `mashed_re.exe` changed as well. What was checked:

- **Builds clean**, both targets, and the exe reports `-> 10 TU(s) with /arch:IA32 (x87)`.
- **Frontend unaffected:** `MASHED_PARITY=1` walk, 17 screens, **15/17 byte-identical** against
  the pre-x87 baseline (`verify/x87_regr/` vs `verify/chalsel_dot_B/`). The two that differ are
  `s6`/`s7`, the pair carrying the pulsing category sprite that the same-build B-vs-B2 control
  already showed varies run to run. Expected: the changed TUs are physics, and the frontend walk
  does not run the solver.

**[UNCERTAIN] the standalone's PHYSICS under x87 is NOT verified.** A `MASHED_RACE_DEMO=1` run was
attempted and did **not** reach a race in ~4.5 minutes — it sat at `phase=0 screen=1`. That is
**not** reported as a regression: the nav demo is known to be fragile about desktop focus and
leaked keystrokes (memory `nav-demo-bypasses-focus-gate`), and this session was issuing shell
commands throughout, so the run is uninterpretable rather than negative. There is also no
pre-x87 race-demo baseline from today to compare against.

What would close it: a `MASHED_RACE_DEMO=1` capture on a quiet desktop, once before and once
after the flag, compared with `re/tools/imgdiff.py`. Note race captures are not bit-reproducible
(the camera rolls on a 1024-tick sine, memory `race-camera-rolls-30deg-sine`), so judge it on
gross behaviour — does it race, do the cars stay grounded — not pixel equality.

The `.asi` side *is* being verified directly, by the 47-row A/B batch below; that establishes the
codegen change is correct in kind, and both targets now compile these TUs identically.

### The argument that was on the table before the measurement

Adding `/arch:IA32` to `build.bat` would restore x87 codegen and make the "verbatim x87
transcription" claim true for intermediates. It is an **architecture-level change** touching every
TU, so per `CLAUDE.md` it is not applied here. Points to weigh:

- **For:** it is the only way a verbatim x87 transcription can be bit-identical; it would likely
  resolve a good share of the 17 `DIVERGENT*` rows; and it makes 85 TUs' stated premise true.
- **Against:** `/arch:IA32` disables SSE2 for the *whole* build, including the librw renderer and
  every non-physics TU, so it is a performance and codegen change far outside the physics lane.
  It also does **not** make `long double` 80-bit — that is an ABI decision MSVC does not expose —
  so a *named* `float10` local still stores 64 bits; only register-resident intermediates recover
  80-bit width.
- **Middle option:** apply `/arch:IA32` per-TU to the physics/math files only, the way
  `QhullBridge` is already compiled separately (`build.bat:43/45`). Mixing `/arch` across TUs in
  one image is legal; the cost is build-script complexity and a per-TU rule to maintain.

Until this is decided, the honest position is: **the two comments are corrected to say SSE2**
(done in this commit — a false statement about the build is worse than a known gap), and the
17 `DIVERGENT*` rows stay unpromoted with this note as their cause.
