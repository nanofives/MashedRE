# A8 — Writers of `_DAT_0088e610` and `_DAT_0088e5f0`

Session 2026-08-26. Read-only Ghidra (pool slot Mashed_pool0), MASHED.exe image base 0x00400000.
NO-GUESSING: every claim cites an RVA / raw dword. `[UNCERTAIN]` marks missing evidence.

## WHERE 1560 COMES FROM (headline, updated 2026-08-26 pass 2)

**1560 is a hardcoded float literal, not a product and not a runtime computation.**

- Instruction `0x0040d3d2` (raw bytes verified non-circularly):
  `c7 05 24 33 80 00 | 00 00 c3 44` = `MOV dword ptr [0x00803324], 0x44C30000`.
  The immediate `0x44C30000` is IEEE-754 float **1560.0**. It is baked into `.text`;
  no multiply/count/stride builds it.
- It is stored to the global `DAT_00803324` (0x00803324), inside `FUN_0040d270`
  (Course::Finish / course-load-teardown path, at label LAB_0040d3c3). Runs **at course
  load**, once.
- That global is the **sole writer's multiplicand**: the per-frame dispatcher
  `FUN_00470c70` reads it as `*param_2` (its caller passes `&DAT_00803324`) and does
  `FMUL [0x005cea80]`.
- **Sole writer confirmed.** `reference_to 0x00803324` = 1 WRITE (0x0040d3d2) + 11
  address-taken DATA refs. The respawn path `FUN_004704c0` takes `&DAT_00803324` as its
  6th arg but only **reads** it (forwards it to `FUN_0046d400`); it performs no store to
  `*param_6`. Whether `FUN_0046d400` writes through it is `[UNCERTAIN]` (not decoded), but
  the measured value equals the literal to the last bit, so 1560.0 stands unmodified.
- Semantic meaning of 1560 is `[UNCERTAIN]` — reported as a raw literal. Numerically
  1560/360 = 13/3, so `_DAT_0088e610 = 1560 · (1/360) = 13/3 = 4.33333`.

Your port must pass **1560.0** (or the global `DAT_00803324`) into that FMUL, not the
50 ms frame budget. 1560/50 = 31.20 = the measured 31.17× discrepancy.

## Verdict up front

The 2026-06-17 `[SUSPECT]` note is **wrong on the multiplicand**, and refuted here.

- The writer does **not** multiply by `frameMs` (50). It multiplies `*param_2`, which the
  sole caller supplies as `&DAT_00803324`.
- `DAT_00803324` is a **course-load constant** = `0x44c30000` = **1560.0**, stored once in
  `FUN_0040d270` at `0x0040d3d2`. It is not a per-frame delta and it is not 50.
- The constant `_DAT_005cea80 = 0.00277781` **IS present** and **IS** the other factor
  (`FMUL [0x005cea80]` at `0x00470f1e`). The note kept the right constant but paired it with
  the wrong multiplicand.
- `1560.0 × 0.00277781 = 4.3334` = the measured `4.33337`. The note's `1558.3` came from
  dividing by the rounded `0.0027809`; with the true dword the multiplicand is exactly 1560.0.
- Direction confirmed: **`0088e610` is computed first**, then `0088e5f0 = 3000.0 / 0088e610`.

The note's 31.17× error = 1560.0 / 50.0 = 31.2 — i.e. exactly the frameMs-vs-1560.0 swap.

---

## Q1 — THE WRITERS

Each global has exactly **one** WRITE site, both inside `FUN_00470c70`
(entry 0x00470c70; the "16-vehicle physics dispatcher"). Verified via `reference_to`:
`0088e610` → 1 WRITE @0x00470f28; `0088e5f0` → 1 WRITE @0x00470f3a.

Exact FPU stream (bytes from memory, decoded):

```
0x00470f1c  d9 00              FLD   dword ptr [EAX]          ; EAX = param_2  → ST0 = *param_2
0x00470f1e  d8 0d 80ea5c00     FMUL  dword ptr [0x005cea80]   ; ST0 *= 0.00277781
0x00470f28  d9 1d 10e68800     FSTP  dword ptr [0x0088e610]   ; _DAT_0088e610 = *param_2 * 0.00277781   ← WRITE #1
0x00470f2e  d9 05 08cd5c00     FLD   dword ptr [0x005ccd08]   ; ST0 = 3000.0
0x00470f34  d8 35 10e68800     FDIV  dword ptr [0x0088e610]   ; ST0 = 3000.0 / _DAT_0088e610
0x00470f3a  d9 1d f0e58800     FSTP  dword ptr [0x0088e5f0]   ; _DAT_0088e5f0 = 3000.0 / _DAT_0088e610 ← WRITE #2
```

Operands:
| operand | address | raw dword | value |
|---|---|---|---|
| `*param_2` | `[EAX]`, EAX=param_2=`&DAT_00803324` | — | 1560.0 (see Q2) |
| mul const `_DAT_005cea80` | 0x005cea80 | `0x3b360bc0` | 0.00277781 |
| div const `_DAT_005ccd08` | 0x005ccd08 | `0x453b8000` | 3000.0 |

Decompiler form (`FUN_00470c70`):
```c
_DAT_0088e610 = *param_2 * _DAT_005cea80;
_DAT_0088e5f0 = _DAT_005ccd08 / _DAT_0088e610;
```

## Q2 — THE MULTIPLICANDS

`_DAT_0088e610` is the one computed from scratch. Its two inputs:

1. `_DAT_005cea80 = 0x3b360bc0 = 0.00277781` — **present**, used verbatim at `FMUL` 0x00470f1e.
   (This settles the note: the 0.0027809 constant is real; only the other factor was wrong.)
2. `*param_2` = `DAT_00803324` (0x00803324). The sole caller `FUN_00425a40` passes
   `&DAT_00803324` (call at 0x00425a?? → `FUN_00470c70(param_1,&DAT_00803324)`).
   - `DAT_00803324` has **one WRITE** (`reference_to` 0x00803324): `0x0040d3d2`,
     `MOV dword ptr [0x00803324], 0x44c30000` inside `FUN_0040d270` (Course::Finish /
     course-load-teardown path, at label LAB_0040d3c3). `0x44c30000` = **1560.0**.
   - The other 11 refs are address-taken (`&DAT_00803324` handed to `FUN_004704c0` respawn
     paths and to the physics dispatcher). Whether `FUN_004704c0` overwrites it is
     `[UNCERTAIN]` (not decoded), but the measured run held the 1560.0-derived value
     constant, so nothing overwrote it in that scenario.
   - Semantic meaning of `DAT_00803324` is `[UNCERTAIN]`; reported as raw constant 1560.0.

`_DAT_005cea80` is multiplied by `DAT_00803324` (=1560.0), NOT by any frame/dispatcher quantity.

## Q3 — CADENCE

`FUN_00470c70` has exactly **one caller**: `FUN_00425a40` (`function_callers`), whose C1 note reads
"Per-frame physics/world update sequence." Call order there:
`FUN_00423b00 → FUN_00418860 → FUN_00424eb0(dt,&DAT_00803324) → FUN_00470c70(dt,&DAT_00803324)
 → FUN_00422ba0 → FUN_004252c0`.

So the **write executes once per frame**. The stored *value* is nonetheless constant across a
run because its multiplicand `DAT_00803324` is a **course-load constant** (1560.0, set once in
`FUN_0040d270`). This is the distinction the [MEASURED] constancy alone could not make: cadence =
per-frame write; value = load-time constant. Both notes' "once at init" and "per frame" are
each half right — per-frame recompute of a load-time constant.

## Q4 — RECONCILE 4.33337

`_DAT_005cea80` = bits `0x3b360bc0` = **0.00277779996 = 1/360** (project note's `0.0027809`
gloss was a wrong decimal on a right hex — a known trap; our port's `kSuspDtK = 0.0027809f` is
its own 0.11% bug, out of scope here).

```
_DAT_0088e610 = DAT_00803324 * _DAT_005cea80
              = 1560.0       * (1/360)
              = 13/3 = 4.33333…      → float32 0x408aaaf3 = 4.33336782  (EXACT bit match to measured)
_DAT_0088e5f0 = 3000.0 / 4.33336782 = 692.302   (measured 692.302; product = 3000.0 by construction)
```
Both inputs are statically fixed dword constants. 1560/360 = 13/3. Confirmed: `1560 * (1/360)`
reproduces the measured `0x408aaaf3` exactly, and 1560.0 is the literal at 0x0040d3d2. To confirm
live: read float at **0x00803324** — expect 1560.0 (`0x44c30000`).

## Q5 — WHAT OUR PORT SHOULD COMPUTE

Mechanism (not a fitted constant):
```
_DAT_0088e610 = DAT_00803324 * 0.00277781     ; DAT_00803324 = 1560.0, a course-load constant (0x00803324)
_DAT_0088e5f0 = 3000.0 / _DAT_0088e610         ; derived, computed second
```
- Our per-frame recomputation **from frameMs is structurally wrong**: the correct multiplicand is
  `DAT_00803324` (the 1560.0 course-load constant), not the 50 ms dispatcher budget. Swapping in
  1560.0 fixes it; the per-frame write cadence itself matches the original and is fine.
- Simplest faithful port: treat `_DAT_0088e610` as the constant **4.3334** written when the course
  loads (mirrors the game, since `DAT_00803324` is load-time-constant), and derive
  `_DAT_0088e5f0 = 3000.0 / _DAT_0088e610` second. The original recomputes both every frame; doing
  the same is equally faithful **provided** the multiplicand is 1560.0, not frameMs.

[MEASURED, prior] The 31.17× error cancels in all three downstream consumers (each reads the
per-wheel field derived from `0088e610` and multiplies by `0088e5f0`; the two are reciprocal
through 3000.0). So this is a record-fidelity fix, not a live behavioural defect.
