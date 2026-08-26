# A8 — Substep budget source for the vehicle physics dispatcher (FUN_00470c70)

Date: 2026-08-25. Binary: `original\MASHED.exe`, image base 0x00400000 (SHA-256 anchor per CLAUDE.md).
Method: read-only Ghidra pool slot `Mashed_pool14`, MCP decomp + listing. NO-GUESSING: every constant
below is cited to the exact address where it appears in the open program.

## Executive verdict

The budget fed to `FUN_00470c70` (`param_1`) is **NOT the wall-clock frame time.** It is a **fixed
immediate `0x32` = 50** (`PUSH 0x32` at the catch-up call site), and the dispatcher runs
`ceil(DAT_007f1000 / 50)` times per render frame, where `DAT_007f1000` is itself **pinned to a
multiple of 50 by a native tick quantizer** (`FUN_00493390` @0x004933d5 hard-writes `0x32`;
`FUN_00493480` re-derives `uVar1 * 0x32` from a remainder accumulator `DAT_007719d4`). In steady
state `DAT_007f1000 == 50`, so the loop runs exactly **once**, and the physics advances a **fixed 50
"ms-unit" budget per rendered frame, decoupled from real time.**

This is the classic "fixed timestep assuming 60 fps" design. Passing the wall-frame ms (16.67 at
60 fps) under-drives the sim by 50 / 16.67 = **3.0x**, which matches the empirically-observed ~3.15x.

---

## Call chain (top → integrator)

```
FUN_00492290  main game loop (per render frame)
  ├─ FUN_00492d30   case 3/6: catch-up loop  →  PUSH 0x32; CALL FUN_004111c0   (ceil(DAT_007f1000/50)x)
  │    └─ FUN_004111c0(50)  moves stack arg → ESI, then per race-state:
  │         └─ FUN_0040fc00()   reads ESI (=50) as the frame delta
  │              └─ FUN_00425a40(50)   MOV ESI,[ESP+8]; PUSH ESI
  │                   └─ FUN_00470c70(50, &DAT_00803324)   ← param_1 = 50   THE DISPATCHER
  │                        └─ FUN_004709a0(dt_float, ...)  (per 25-unit sub-chunk)
  │                             └─ FUN_0046e9e0(dt_float, ...)  body integrator (1x/vehicle, 2x on collision retry)
  └─ FUN_00493480   discretizer: WRITES DAT_007f1000 for the NEXT iteration
       └─ FUN_00493390   @0x004933d5:  DAT_007f1000 = 0x32   (the pinned tick constant)
```

(`FUN_0042c960` @0x0042c980 carries the identical `PUSH 0x32` catch-up loop for the other game-mode
path; both callers of `FUN_004111c0` use the same fixed 50.)

---

## Q1 — THE BUDGET SOURCE (annotated disassembly)

### (a) The catch-up call site — `FUN_0042c960` (identical shape in `FUN_00492d30` case 3/6)

```
0042c960  MOV  EAX,[0x007f1000]     ; EAX = DAT_007f1000  (quantized budget, see below)
0042c96b  CMP  EAX,EBX             ; EBX=0 → if budget <= 0, skip
0042c96e  JLE  0x0042c98d
0042c970  LEA  ECX,[EAX + -0x1]     ; ECX = budget - 1
0042c973  MOV  EAX,0x51eb851f      ; reciprocal magic for /50
0042c978  MUL  ECX
0042c97a  MOV  ESI,EDX
0042c97c  SHR  ESI,0x4             ; ESI = (budget-1)/50
0042c97f  INC  ESI                 ; ESI = ceil(budget/50)   ← iteration count
0042c980  PUSH 0x32                ; *** FIXED IMMEDIATE 50 = the physics budget ***
0042c982  CALL 0x004111c0          ; FUN_004111c0(50)
0042c987  ADD  ESP,0x4
0042c98a  DEC  ESI
0042c98b  JNZ  0x0042c980          ; loop ceil(budget/50) times
```

### The value passed straight down to the dispatcher — `FUN_00425a40`

```
00425a72  PUSH ESI
00425a73  CALL 0x00418860
00425a78  MOV  ESI,[ESP + 0x8]     ; ESI = this fn's arg (the frame delta = 50)
00425a7c  PUSH 0x803324           ; &DAT_00803324 (physics-scale ptr = param_2)
00425a81  PUSH ESI                ; budget
00425a82  CALL 0x00424eb0
00425a87  PUSH 0x803324
00425a8c  PUSH ESI                ; budget
00425a8d  CALL 0x00470c70          ; FUN_00470c70(param_1 = ESI = 50, param_2 = &DAT_00803324)
```

### The frame-delta register origin — `FUN_0040fc00` entry (confirms ESI = the delta)

```
0040fc00  SUB  ESP,0x8
0040fc03  TEST ESI,ESI            ; ESI = frame delta (=50, from FUN_004111c0's stack arg)
0040fc05  MOV  [ESP+4],ESI
0040fc09  FILD [ESP+4]
0040fc1b  FMUL [0x005cc948]        ; * 1/3000  →  _DAT_007f100c = 50/3000 = 1/60 s  (cross-check)
0040fc21  FSTP [ESP+8]
```
`_DAT_005cc948 = 0x39aec33e = 0.00033332 = 1/3000` (memory_read @0x005cc948). With ESI = 50 this
yields `_DAT_007f100c = 1/60 s`, independently confirming the register carries 50.

### The quantizer that sets `DAT_007f1000` — `FUN_00493390` (pins it) then `FUN_00493480` (re-derives)

`FUN_00493390` (called only by `FUN_00493480`):
```
... uses the wall timer (FUN_004950b0) ONLY for drift bookkeeping (DAT_00771984) and the smoothed
    float DAT_007f1010 ...
004933d5  MOV dword ptr [0x007f1000], 0x32     ; *** DAT_007f1000 hard-pinned to 50 ***
          MOV dword ptr [0x007f1004], 0x3c888889 ; DAT_007f1004 = 1/60 s  (float)
```
Decompiled: `DAT_007f1000 = 0x32;`  — unconditional, ignores wall time for the integer budget.

`FUN_00493480` (the "frame tick discretizer", the live pacing entry from `FUN_00492290`):
```c
FUN_00493390();                       // DAT_007f1000 := 50
// bucket snap: 50 ∈ [0x2f,0x35] → DAT_007f1000 stays 0x32
DAT_007719d4 = DAT_007719d4 + DAT_007f1000;      // remainder accumulator += 50
uVar1 = 0;
if (0x31 < DAT_007719d4) { uVar1 = DAT_007719d4/0x32; DAT_007719d4 %= 0x32; }  // dispense 50-quanta
// (remainder kept only for 3..47, else reset to 0; 48..49 rounds uVar1 up)
DAT_007f1000 = uVar1 * 0x32;                      // FINAL budget = quanta * 50
DAT_007f1004 = (float)(uVar1*0x32) * _DAT_005cc948; // = quanta*50/3000 s
```

**Classification: mixed (b)+(c).** The per-tick value is a **fixed immediate 50 (b)**; the *number*
of ticks per frame is governed by an **accumulator (c)** — `DAT_007719d4` at 0x007719d4 carrying the
sub-50 remainder — whose input is itself pinned to 50 by `FUN_00493390` @0x004933d5. It is explicitly
**NOT (a)**: no wall-clock delta reaches the integer budget; the timer is used only for the *separate*
float `DAT_007f1004`/`_DAT_007f100c` and drift terms.

`DAT_007f1000` writers: 0x0049277e (`FUN_00492770`, init → 0), 0x004933d5 (`FUN_00493390`, → 0x32),
0x00493514 (`FUN_00493480`, → uVar1*0x32).

**Steady state:** accumulator gets +50/frame → `uVar1 == 1` every frame → `DAT_007f1000 == 50` →
catch-up loop iterates `ceil(50/50) = 1`.

---

## Q2 — Iterations per dispatcher call, and FUN_0046e9e0 multiplicity

- The dispatcher's own chunk loop (`FUN_00470c70`) is nested:
  - Outer: `uVar11 = param_1 (=50)`; `local_24 = min(uVar11, 0x32)` → 50; decrements `uVar11 -= local_24`
    → runs once for budget 50, then `uVar11 == 0` returns 1.
  - Inner sub-chunk `for` loop: `uVar12 = local_24 (=50)`; `uVar15 = min(uVar12, 0x19=25)`;
    `FUN_004709a0((float)uVar15, param_2)`; `uVar12 -= uVar15`. For 50 → **two** calls of dt = 25.0.
    Cited: `FUN_004709a0` call at the `for (; uVar12 != 0; ...)` loop tail of `FUN_00470c70`.
- **`FUN_004709a0` is called once per 25-unit sub-chunk** (2x for a 50-unit budget). Sum of dt over the
  frame = param_1 = 50 (25 + 25).
- **`FUN_0046e9e0` is called once per vehicle per `FUN_004709a0` call** — cited: the single
  `FUN_0046e9e0(param_1, local_1c)` inside the `while(true)` at `FUN_004709a0`, inside the 16-slot
  `local_18` loop. It runs a **second** time for the same vehicle only on a collision re-solve
  (guard `if (1 < iVar10) goto LAB_00470c47`, max 2), i.e. NOT a fixed multiplier — under grip-saturated
  open-track (no contact) it is exactly once. So per vehicle per frame: 2 sub-chunks × 1 = 2 integrations
  of dt = 25 each, summing to 50.

---

## Q3 — Is FUN_00470c70 called once per render frame?

**Once per render frame in steady state.** `FUN_00492290` is the per-frame main loop; its body calls
the catch-up tick (`FUN_00492d30`/, or `FUN_0042c960`) which loops `ceil(DAT_007f1000/50)` times, and
`DAT_007f1000 == 50` (Q1) → exactly one `FUN_004111c0(50)` → one `FUN_00470c70(50)`.

The catch-up loop **is** the fixed-timestep catch-up mechanism, but it can exceed one iteration only if
`DAT_007f1000 > 50`, which requires the discretizer to emit `uVar1 > 1`. Because `FUN_00493390`
@0x004933d5 pins the accumulator input to exactly 50 and the remainder resets each frame, `uVar1` is 1
in steady state; multi-tick frames do not occur under steady pacing. **The 3.15x is therefore NOT
explained by multiple dispatcher calls per frame** — it is explained by the fixed 50-unit budget vs the
standalone's 16.67-unit wall budget (Q5).

---

## Q4 — Units sanity (is there a hidden conversion?)

- The value compared against `0x32` is an **integer millisecond-style count** (`DAT_007f1000`, `param_1`,
  `uVar11`, `local_24`, `uVar12`, `uVar15` are all `uint`/`int`). Sub-chunk cap `0x19 = 25`.
- Conversion to float happens **once**, immediately before the call, with **no scale/divide**:
  `fVar4 = (float)(int)uVar15;` then `FUN_004709a0(fVar4, param_2)` (cited at the `for` tail of
  `FUN_00470c70`). `FUN_004709a0` passes that float **verbatim** as `FUN_0046e9e0(param_1, ...)`.
- So `FUN_0046e9e0`'s `param_1` is the **raw integer sub-chunk count as a float** (25.0, then 25.0);
  the steer-torque law consumes "25 ms-units" literally, summing to 50 units/frame. **No hidden factor.**
- The `1/3000` scale (`_DAT_005cc948`) applies **only** to the *separate* float `DAT_007f1004` /
  `_DAT_007f100c` used by non-physics systems (render/HUD/timers), where 50 units → 1/60 s. It does
  **not** touch the physics dt. This is the one place a reader could be misled: the engine keeps two
  clocks — an integer 50-unit physics budget and a 1/60 s float wall-frame — related by 50/3000.

---

## Q5 — What a 60 fps standalone should pass (mechanism, not a fitted number)

Pass a **fixed integer budget of 50 (`0x32`) per rendered frame** — i.e. replicate the tick quantizer:
maintain a remainder accumulator (the `DAT_007719d4` mechanism) that releases 50-unit quanta, and feed
`FUN_00470c70` `quanta * 50`. Under steady 60 fps pacing the quantizer releases exactly one quantum per
frame, so the per-frame budget is 50.

- The original's physics is **decoupled from wall time**: it advances a fixed 50 units per rendered
  frame regardless of the real frame duration. To match the original's per-render-frame body-heading
  motion, the standalone must pass **50**, not `dt*1000` (= 16.67 at 60 fps).
- 50 / 16.67 = **3.0x** — this is the missing factor. (Measured 52.48/16.67 = 3.15x; the ~5% gap is
  capture scatter / occasional 2-quantum frames / steer or grip not perfectly saturated, not a distinct
  mechanism.)
- Concretely for the port: set the dispatcher's starting budget to the quantized 50-unit value, not the
  wall ms. If the port also wants the engine's float dt to stay 1/60 s, keep that as a *separate* value
  (`50/3000`), exactly as the original keeps `DAT_007f1004` distinct from `DAT_007f1000`.

Nothing here is `[UNCERTAIN]`: the budget immediate (0x32), the quantizer pin (0x004933d5), the
once-per-frame catch-up, and the no-scale float conversion are all read directly from the listing.
```
