# AI control-output byte→axis map — RESOLVED (WS-C2 step 1, 2026-06-16)

Closes the **hard blocker** flagged in `re/analysis/ai_controller.md` §5 / §11
(U-0407, U-0413, U-7587): *"the C3 position diff is meaningless if accel and steer
are swapped — decompile the DirectInput writer + physics reader and reconcile."*

Anchored to `original/MASHED.exe` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
All facts from Ghidra MCP read-only decompilation on `Mashed_pool5` this session
(pool4 was a corrupt 43 MB half-clone with no analysis; fell back to pool5 per
[[feedback-mcp-leaked-project-lock]]).

## The control block

Per-player / per-AI-vehicle **current-frame input record**, base `0x007f1038`,
stride **0x4c B = 0x13 DWORDs**, indexed by an output-slot id
`slot = *(int*)(0x007f1a14 + v*0x10)`:

- AI writes it: `FUN_00418560` (`EDI = 0x007f1038 + slot*0x4c`).
- Human DirectInput cook writes it: `FUN_00496530` (`0x007f1038 + player*0x4c`).
- Physics reads it: `FUN_00470c70` passes `puVar18 = &DAT_007f1038 + slot*0x13`
  (= `0x007f1038 + slot*0x4c`, same slot table) to `FUN_00470670`
  (in game modes 6/0xb/10; otherwise redirected to the neutral block `0x007f19b8`).

So the AI is a **virtual gamepad**: it writes the identical byte block the human
writes, and the same physics path consumes it.

## Resolved byte map (offsets from block base 0x007f1038 + slot*0x4c)

| byte | role | evidence (writer ⇄ consumer) |
|---|---|---|
| **[0]** | **steer cmd, sign A** | written by `FUN_00416250` from the `FUN_00415e20` signed steering-angle error (`__ftol` of the magnitude in ST0) when the error is in band A |
| **[1]** | **steer cmd, sign B** | same, error band B; also the mode-2 ram path writes `[1]` |
| **[3]** | **fire / powerup button** | written by `FUN_00415220` (mode-8 weapon fire) |
| **[4]** | **accelerator** (0x00 off / 0x40 partial / 0xff full) | writer `FUN_00416250` (`param_3[4]`) + writer `FUN_00418560` mode-5 (`[EDI+4]=0xff`) + override `FUN_00417640` (`param_2[4]`) ⇄ **consumer `FUN_00467650`** reads `*(byte*)(param_4+4)` as forward drive magnitude (wheel drive torque `unaff_ESI+0xb14/b18/b1c`) |
| **[5]** | **brake / reverse** (0x00 / 0x40 / 0xff) | writer `FUN_00416250` (`param_3[5]`) + override `FUN_00417640` (`param_2[5]=0xff`) ⇄ **consumer `FUN_00467650`**: `*(param_4+5)!=0 && *(param_4+4)==0` ⇒ `fVar5 = -(…)` = negative (reverse) wheel force; also `FUN_00470670` gates the accel curve on `(float)param_3[5] <= _DAT_005cc9d0` |
| [6], [7] | zeroed scratch | `FUN_00418560` zeroes `[EDI+6]`,`[EDI+7]`; no control step writes them |

(Human-cook-only bytes, not used by the AI: analog axes [0]–[3] with active flags
[0x0c]–[0x0f], digital buttons [0x04],[0x05],[0x07]–[0x0b],[0x11],[0x3f], and the
derived steer/throttle floats at [0x14] and [0x18]. These belong to
`FUN_00496530` and are out of scope for the AI port.)

## Why the two prior plates "disagreed" (the conflict, dissolved)

- The C1 doc's **AI-side plate** (`[0]/[1]=steer, [4]/[5]=accel/brake`) is **CORRECT**.
- The C1 doc's **physics-side plate** (`[0]=accel, [1]=brake`) came from `FUN_00470670`,
  which *does* read `*param_3`/`param_3[1]` as a +/− longitudinal **body impulse**
  (`vehicle+0x1a8` force ring) — but that is a **secondary channel**, not the primary
  throttle. The **primary** accel/brake consumer is `FUN_00467650` (the per-wheel
  drive/brake torque integrator), which reads **[4]/[5]**, matching the AI writer and
  the `FUN_00417640` override.
- **Accel and steer are NOT swapped.** Accel = [4], brake = [5] (three independent
  witnesses); steer = [0]/[1] (writer `FUN_00416250` ⇄ steering source `FUN_00415e20`).

## Note on `FUN_004a2c48`

`FUN_004a2c48()` is the MSVC **`__ftol`** float→long helper (no args; reads x87 ST0,
returns `ROUND(ST0)` with the negative-bias correction). Every
`x = FUN_004a2c48()` in this cluster is `x = (long)<float currently in ST0>`. The AI
control step relies on this implicit-ST0 convention when storing steer/accel bytes —
the port must keep the producing float expression on FPU-equivalent ordering or call
an explicit `(int)` cast on the just-computed value.

## Impact on the port (the real point)

A **verbatim** port reproduces `FUN_00416250`/`FUN_00418560`'s writes **by byte
index**, so the downstream semantic of each byte does not change the port. The byte
map matters for two things only:
1. **C3 diff interpretation** — so we don't mislabel columns. The honest C3 is a
   **byte-for-byte diff of the produced control block** (or per-function decision
   diffs), which is semantics-independent.
2. **Wiring AI → physics** — only matters once the *ported* physics (`FUN_00470670`
   cluster, WS-A8) consumes it; the ported physics interprets bytes identically to
   the original, so still semantics-independent. We never feed AI bytes into the
   scaffold kinematic `UpdateCar` (that scaffold is being removed in C3).

**Residual [U-0413] (downgraded, non-blocking):** the *label* of the [0]/[1] channel
("steer command" vs the longitudinal-impulse interpretation `FUN_00470670` applies)
is a downstream-physics question, not an AI-port question. Carried, does not block
WS-C2.
