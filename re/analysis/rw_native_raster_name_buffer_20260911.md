# A stream-supplied length drives a write into a fixed 128-byte stack buffer

**Date:** 2026-09-11
**Rows:** U-5162 (CONFIRMED)
**Functions:** `0x004cf5a0` (caller, two buffers), `0x004d8810` (the writer), `0x004cc400`
(supplies the length)
**Status:** code-path claim, confirmed against decompilation. **No overflow has been
observed at runtime** and none is asserted here.

## What the code does

`0x004cf5a0` declares two fixed buffers in its own frame:

```c
undefined1 local_100 [128];
undefined1 local_80  [128];
```

and passes each as the **destination** argument of `FUN_004d8810`:

```c
iVar2 = FUN_004d8810(local_80, param_1);    // line 45
iVar2 = FUN_004d8810(local_100, param_1);   // line 51
```

`FUN_004d8810(undefined1 *param_1, undefined4 param_2)` takes `param_1` as the destination
and `param_2` as the stream. It obtains the byte count from the chunk header:

```c
iVar1 = FUN_004cc400(param_2, &local_84, &local_9c, local_90, 0);
```

`local_9c` is the length and `local_84` is a type tag. Both come from the stream. Neither
is compared against 128, or against any caller-supplied capacity, anywhere in the
609-byte body. `FUN_004d8810` has no capacity parameter at all.

Two copy paths consume it:

- `local_84 == 2` writes `local_9c` bytes into `puVar4`, which starts at `param_1`.
- `local_84 == 0x13` writes `local_9c / 2` bytes (it takes every second source byte:
  `puVar4[uVar3] = local_80[uVar3 * 2]`, advancing `puVar4 += uVar5 >> 1`).

## The clamp that looks like a bound, and is not

`FUN_004d8810` does contain a 0x80 clamp:

```c
uVar5 = 0x80;
if (uVar6 < 0x81) { uVar5 = uVar6; }
uVar3 = FUN_004cbd30(param_2, local_80, uVar5);
```

This bounds **`FUN_004d8810`'s own `local_80` scratch buffer**, which is where each read
chunk lands before being copied out. The loop then keeps going until `uVar6` (initialised
from `local_9c`) is exhausted, advancing the destination cursor each pass. The clamp
limits the chunk size, not the total, and not the destination.

This is the trap: `FUN_004d8810` has a `local_80[128]` of its own, and so does its caller
`0x004cf5a0`. A clamp protecting the former reads as if it protects the latter.

## Why this matters for the port

When `param_1` is non-NULL the function writes into caller storage with a
stream-controlled length. There is a NULL path where it allocates instead
(`(**(code **)(DAT_007d3ff8 + 0x108))(local_9c, 0x30002)`), and **that** path is sized
correctly. Only the caller-supplied-buffer path is unbounded.

A verbatim port reproduces the unbounded write. That is correct behaviour for a diffing
port and should not be "fixed" silently -- a bounds check would be a behavioural
divergence and would show up in an A/B. If a guard is ever wanted it belongs behind an
explicit opt-in flag, documented as a deliberate departure.

## What is NOT established

- **[UNCERTAIN]** whether any shipped `TOASTART` asset supplies `local_9c > 128` on this
  path. Untested. Settling it means either scanning the archives for the chunk type or
  instrumenting `FUN_004cc400`'s `local_9c` during a load.
- The RenderWare name of the chunk types 2 and 0x13 is not assigned here.

## UPDATE 2026-09-11 — the header reader does NO validation, and that is now read

`FUN_004cc400` had no Ghidra function until the master repair the same day. It does now,
and it is `RwStreamReadChunkHeader`:

```c
iVar1 = FUN_004cbd30(param_1, &local_20, 0xc);   // read a fixed 12-byte header
if (iVar1 != 0xc) { ...; return 0; }             // the ONLY check: did 12 bytes arrive
...
local_10 = local_1c;                             // second dword, verbatim
if (param_3 != (undefined4 *)0x0) { *param_3 = local_10; }
```

In the caller that `param_3` is `&local_9c`. So **the byte count driving the write into a
128-byte stack buffer is the second dword of the file's chunk header, passed through
unchecked**. Across all 231 bytes of `FUN_004cc400` there is no comparison of that value
against any capacity, and it has no capacity parameter to compare against.

This closes the last open link in the chain. The remaining unknown is only whether a
shipped asset actually carries an oversized value — the code path itself is now fully
established end to end: file header → `local_9c` → `FUN_004d8810` → `local_80[128]`.

## How it was found

Pass-3 of the uncertainty drain returned U-5162 as RESOLVED-and-safe, citing the 0x80
clamp. The quoted expression did not appear in `0x004cf5a0` at all, which is what
prompted reading `FUN_004d8810` directly. **The lesson is the check, not the bug:**
confirm that a claim's quoted code is in the function the row is about before accepting
the conclusion. See also the register-argument pair in the same pass, where one row's
hypothesis was confirmed (U-5584) and a look-alike was refuted (U-5427).
