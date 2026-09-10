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

## The decision

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
