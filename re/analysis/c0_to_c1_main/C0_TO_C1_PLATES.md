# C0→C1 first-pass plates — 4 game-code rows (consolidated)

Authored 2026-05-25 inline. The remaining 4 non-library C0 rows after the
981-row library-band bulk-attest. Each has minimal Ghidra body inspection
satisfying first-pass C1 criteria per CONFIDENCE.md.

---

## 0x00458630  FUN_00458630 — Powerup sprite lookup by type

**Subsystem:** frontend
**Body:** 0x00458630..0x00458741 (273 bytes)

```c
undefined4 FUN_00458630(undefined4 param_1);  // (powerup_type) -> sprite_handle
```

**Body summary:** 13-case switch on `param_1` (powerup type code 2/6/7/9/A/B/C/D/10/11/12/13/15/16); each case calls `FUN_004c5c00(DAT_0068b9ac, "name")` with a string-id selector. Default returns 0.

**String table cited:**
| case | string                          |
|------|----------------------------------|
| 2    | "flamethrower"                   |
| 6/c/d| `&DAT_005ce594`                  |
| 7    | "mortar"                         |
| 9    | "machinegun"                     |
| 10   | "depthcharge"                    |
| 0xb  | "missile"                        |
| 0x10 | "flamethrower" (dup of case 2)   |
| 0x11 | "shotgun"                        |
| 0x12 | "flare"                          |
| 0x13 | `&PTR_DAT_005ce4fc`              |
| 0x15 | "chaos"                          |
| 0x16 | "airstrike"                      |

**Callers:** `FUN_0043af10` (lobby renderer, just promoted to C2).
**Callees:** `FUN_004c5c00` (string-keyed sprite-lookup utility).

Ghidra already had `[C1 2026-05-11]` annotation. CSV catch-up only.

---

## 0x004736c0  FUN_004736c0 — Line/border quad renderer

**Subsystem:** frontend
**Body:** 0x004736c0..0x00473867 (423 bytes)

```c
void FUN_004736c0(float left_x_norm, float top_y_norm,
                  float width_norm, float height_norm,
                  uint base_rgba,
                  byte alpha_a, byte alpha_b, byte alpha_c, byte alpha_d);
```

**Body summary:** Converts normalized (x,y,w,h) to pixel coords via
`FUN_0042b8b0`/`FUN_0042b8c0` (viewport) × `_DAT_005cd5a8`/`_DAT_005cc560`
(scale). Builds 4 RGBA corner colors at `DAT_00898a30/4c/68/84` by packing
`base_rgba` bytes with per-corner alpha_a/b/c/d. Writes 4 vertices to
the `DAT_00898a20..` vertex pool (stride 0x1c). Sets RW render-states 1,
0xc, 10, 0xb via `*(DAT_007d3ff8+0x20)(idx, value)`. Final draw via
`*(DAT_007d3ff8+0x30)(4, &DAT_00898a20, 4)` — DrawPrimitive call.

**Callers:** `FUN_00434720` (championship/cup progress screen, C2).
**Callees:** `FUN_0042b8b0`, `FUN_0042b8c0` (viewport getters);
2 indirect calls through `DAT_007d3ff8` vtable.

Ghidra `[C1 2026-05-11]` annotation. CSV catch-up only.

---

## 0x00474e60  FUN_00474e60 — Float-to-x87-angle converter

**Subsystem:** frontend
**Body:** 0x00474e60..0x00474e6a (10 bytes)

```c
float10 FUN_00474e60(float param_1);  // angle conversion (likely deg->rad or rad->normalized)
```

**Body summary:** Single multiplication —
`return (float10)param_1 * (float10)_DAT_005cd7a8;`.

`_DAT_005cd7a8` is the conversion constant (radians ↔ degrees ↔
1/(2π) — exact value not promoted to a named symbol but is a runtime
constant in `.rdata`).

**Callers:** `FUN_00434720` (championship progress; same caller cluster as
0x004736c0).
**Callees:** none (leaf).

Ghidra `[C1 2026-05-11]` annotation. CSV catch-up only.

---

## 0x005aa1e0  LAB_005aa1e0 — Listing-level "is-equal" comparator wrapper

**Subsystem:** audio
**Bytes:** 0x005aa1e0..0x005aa1f1 (19 bytes), then NOP padding to 0x005aa1f4.
**Not a function** — Listing-level LAB only, no FUNCTION envelope, hence
`function_at` returned no-function. Treated as inline callback per its
existing note ("Listing-level-only inline callback").

**Disassembly:**
```asm
8b 4c 24 08    MOV ECX, [ESP+8]      ; arg2 (a value)
8b 44 24 0c    MOV EAX, [ESP+0xc]    ; arg3 (b value)
50             PUSH EAX               ; push arg3
8b 11          MOV EDX, [ECX]         ; deref arg2 -> EDX
52             PUSH EDX               ; push *arg2
e8 3f 3d 00 00 CALL rel32 (-> +0x3d3f from next-PC)
83 c4 08       ADD ESP, 8             ; cleanup pushed args
f7 d8          NEG EAX                ; eax = -eax
1b c0          SBB EAX, EAX           ; eax = -(CF) = (eax_was_0 ? 0 : -1)
40             INC EAX                ; eax = (was_0 ? 1 : 0)
c3             RET
```

**Semantics:** Comparator-result-to-boolean adapter.
`is_equal(*(void**)a, b) = (cmp(*(void**)a, b) == 0) ? 1 : 0`.
Used as a callback that wraps an inner audio-related comparator into
a 0/1 boolean result.

**Inner CALL target:** rel32 = 0x3D3F means absolute = 0x005aa1ee + 5 + 0x3d3f
= **0x005ade32** (need follow-up disassembly to identify the inner
comparator at C2 time).

**Callers:** indirect — likely passed as a function pointer to a
qsort/bsearch-style API in the audio subsystem. Not yet traced.

**Recommendation at next confidence promotion:** trace callers via
`reference_to 0x005aa1e0` to identify which audio table this is being
searched/sorted into; identify inner comparator at 0x005ade32; consider
promoting to a real Ghidra function definition before C2 work.

---

## Uncertainties filed

U-4309 (this batch): `_DAT_005cd7a8` (used by 0x00474e60) needs an
identifying name — likely PI, PI/180, 180/PI, or 1/(2*PI). Resolution:
read the runtime float bytes via `memory_read` to pin down which.

U-4310: 0x005aa1e0 is a LAB not a function. The inner CALL at
0x005ade32 hasn't been classified. Should be promoted to a real
function definition + linked to its caller-context.

U-4311: 0x00458630 inner string `&DAT_005ce594` (case 6/c/d) and
`&PTR_DAT_005ce4fc` (case 0x13) need identification — likely
"powerupshadow"-class names. Should be resolved during
the 0x0043af10 lobby renderer C3 work since that's a caller.
