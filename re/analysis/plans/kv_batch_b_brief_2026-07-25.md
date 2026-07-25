# KV Batch-B port brief — KV1 scene callbacks (2026-07-25)

Produced read-only by an account2 child session spawned from the account3 parent. Sources:
`B5e_SOLVER_ISLAND_2026-07-15.md` §2 item 3 / §3 KV1 row; `re/analysis/b5e/island_vtable_reach.md`
§1.3 + uncertainties; `re/analysis/b5e/K13_PORT_RECON_2026-07-18.md`; plates
`bucket_00565d50/0x0056a450|adb0|aae0|ac40.md`; `island_vtable_targets.tsv`; `hooks.csv` 4401-4413.

## 0. Roster

| RVA | slot | install condition | Ghidra-defined | plate | hooks.csv |
|---|---|---|---|---|---|
| `0x0056b310` | `+0x404` | **always** | **NO** (span-est 1168 B) | none | **no row** |
| `0x0056a450` | `+0xf4` | `scene+0x58 == 2` | yes (835 B) | yes | C1 mapped (4404) |
| `0x0056adb0` | `+0xf8` | `scene+0x58 == 2` | yes (1369 B) | yes | C1 mapped (4408) |
| `0x00569140` | `+0xf4` | else | **NO** (span-est 1712 B) | none | **no row** |
| `0x005697f0` | `+0xf8` | else | **NO** (span-est 3168 B) | none | **no row** |
| `0x0056aae0` | callee of adb0 | — | yes (342 B) | yes | C1 mapped (4406) |
| `0x0056ac40` | callee of adb0 | — | yes (357 B) | yes | C1 mapped (4407) |

All five slot targets carry `island_called_slot = 1` (the island really dispatches them).
`size_kind=span-est` = Ghidra-undefined, size is an upper bound only.

## 1. Install sites — scene ctor `FUN_0055f800` (body 0x55f800..0x55fdcd)

- `0x0055fd8e`: `[ESI+0x404] = 0x0056b310` — **unconditional**
- branch `CMP [ESI+0x58],2` @ `0x0055fd84`:
  - **equal**: `0x0055fd9c` → `[ESI+0xf4] = 0x0056a450`; `0x0055fda6` → `[ESI+0xf8] = 0x0056adb0`
  - **else**: `0x0055fdb5` → `[ESI+0xf4] = 0x00569140`; `0x0055fdbf` → `[ESI+0xf8] = 0x005697f0`

`FUN_00560260` (sole caller `FUN_00561040`) receives `param_11 = *(scene+0xf4)`,
`param_12 = *(scene+0xf8)`, `param_13 = *(scene+0x404)`.

**[UNCERTAIN] `scene+0x58`** — only the mechanical compare against literal `2` is in evidence;
the field's meaning is unstated. Report it as an int selector at `+0x58`, nothing more.

## 2. Invocation map — and a trap worth reading twice

| slot | param | target (==2 / else) | invoked at |
|---|---|---|---|
| `+0xf4` | `param_11` | `0x0056a450` / `0x00569140` | **1×** @ `0x560a9c` |
| `+0xf8` | `param_12` | `0x0056adb0` / `0x005697f0` | **NONE OBSERVED** |
| `+0x404` | `param_13` | `0x0056b310` (always) | **2×** @ `0x560c5a`, `0x560d90` |

**TRAP:** `param_12` is installed pairwise and passed to `FUN_00560260`, but the decomp shows
**no `(*param_12)(…)` call line** (`island_vtable_reach.md:86-88,147-149`). Installation pairs
a450+adb0; *invocation* is proven only for the `+0xf4` target. **Do not assume `0x0056adb0`
takes the p11 frame.** Missing evidence: full re-read of `FUN_00560260` decomp for a masked
call site.

## 3. Callback ABI frames (K13_PORT_RECON, DISASM-VERIFIED 2026-07-18, pool0 read-only)

Model each as a by-value struct: `((void(__cdecl*)(struct KvNArgs))cb)(frame)` — ABI-identical
to N separate dword args, filled in ascending-address order. All three frames return **void**.
Register map: `EBX = param_3 (puVar14)`, `EAX = param_4`, `[ESP+0x60-region] = param_5`.

### 3.1 `param_11` — `CALL [ESP+0x14c]` @ `0x560a9c` — 53 dwords (212 B), all by-value
```
1-3    param_5[0x25], param_5[0x26], param_5[0x27]
4-37   param_5[0..0x21]                              (REP MOVSD 0x22)
38-40  param_3[4], param_3[5], param_3[6]
41-43  param_3[7], param_3[8], param_3[9]
44-46  param_3[0xa], param_3[0xb], param_3[0xc]
47-49  param_3[0x46], param_3[0x47], param_3[0x48]   (*puVar1 vec3)
50-52  param_3[0x43], param_3[0x44], param_3[0x45]
53     param_3[0x49]
```

### 3.2 `param_13` CALL #1 — `CALL [ESP+0x194]` @ `0x560c5a`
Side effects BEFORE the call (**KEEP in the port**):
```
param_3[0x4e] = param_9[1];   param_3[0x50] = param_9[0x17];   _DAT_00913284 = 0;
```
Explicit args 1-4: `param_4`, `param_4+6 (=+0x18)`, `param_4+0x15 (=+0x54)`, `param_6`
Tail (args 5+, build order, each cited to its store):
```
param_3[0x49..0x56]  (14w, REP MOVSD @0x560bb1)
param_3 ptr          (PUSH EBX @0x560bb3)
param_3[0x1c..0x1e]  (@0x560bba, LEA [EBX+0x70])
param_5[0x22..0x24]  (@0x560bcf, [param_5+0x88])
param_3[0x10..0x12]  (@0x560bf1, [EBX+0x40])
param_3[0xd..0xf]    (@0x560c07, [EBX+0x34])
param_3[0x46..0x48]  (@0x560c1d vec3 from *puVar1)
param_5[0..0x21]     (34w, REP MOVSD @0x560c4a)
param_12             (PUSH @0x560ba3, sits at frame top)
```

### 3.3 `param_13` CALL #2 — `CALL [ESP+0x194]` @ `0x560d90` (same fn, 2nd body set)
Side effects BEFORE (**KEEP**):
```
param_3[0x4e] = param_9[2];   param_3[0x50] = param_9[0x18];   _DAT_00913284 = 1;
```
Explicit args 1-4: `param_4+3 (=+0xc)`, `param_4+9 (=+0x24)`, `param_4+0x18 (=+0x60)`, `param_6`
Tail:
```
param_3[0x49..0x56]  (14w, REP MOVSD @0x560cde)
param_3 ptr          (PUSH EBX @0x560ce0)
param_3[0x1f..0x21]  (@0x560ce7, LEA [EBX+0x7c])
param_5[0x25..0x27]  (@0x560cfc, [param_5+0x94])
param_3[0x16..0x18]  (@0x560d1d, [EBX+0x58])
param_3[0x13..0x15]  (@0x560d33, [EBX+0x4c])
param_3[0x46..0x48]  (@0x560d49 vec3)
param_5[0..0x21]     (34w, REP MOVSD @0x560d7e)
param_12             (PUSH @0x560cc9)
```
`param_9` is `int*`/`undefined4*` read with per-field `(float)` casts — the selector compare
`param_9[0x16]==2` @`0x560835` is an INTEGER compare (integer MOVs, no FLD/FST).

**Consequence:** `0x0056a450`/`0x00569140` (`+0xf4`) must accept the 53-word p11 frame;
`0x0056b310` (`+0x404`) must accept the (4 explicit ptr + ~62-word tail) p13 frame.

## 4. Plate summaries (the two portable targets + callees)

- **`0x0056a450`** — `void(undefined4 *p1, int p2, undefined4 p3, int p4)` + heavy
  `in_stack_000000d4/c8/bc` reads (consistent with the 53-word frame). Modified Cholesky /
  LDL^T factorization + substitution; inverts diagonal `1.0 / in_stack_000000d4`, accumulates
  `M^-1 * J^T * impulse`. Const `0x005cc320 = 1.0`. **No callees.**
- **`0x0056adb0`** — `void(int *p1, int *p2)` + `in_stack_000000bc/b8/110`. GJK / sub-polytope
  iteration loop; calls `FUN_0056ac40` (best axis) then `FUN_0056aae0` (substitution), converges
  on `local_88 == -1` or budget. Consts `0x005cc320=1.0`, `0x005cc32c=0.5`,
  `0x005cc568=[UNCERTAIN]`. **Callees: `0x0056aae0`, `0x0056ac40`.**
- **`0x0056aae0`** — block PGS row update; `accum = clamp(target - in_stack_000000d0,
  in_stack_000000c8, in_stack_000000cc)`; writes `0` / `0xffffffff` sign flags.
  **Callee: `0x0056a7a0`.**
- **`0x0056ac40`** — **returns `float10` (x87 ST0)**. SIMD separating-axis search, 4 axes per
  register, `movmskps` lane extraction; consts `_DAT_005e5a40..4c` (per-lane step counters
  0.0/1.0/2.0/3.0) and `_DAT_005e5a70..7c` (lane sign masks). Returns squared norm.

## 5. Callee verdicts (extern vs thunk)

| callee | C-level | verdict |
|---|---|---|
| `0x0056aae0` | C1 mapped | **un-ported → RVA-forwarding thunk** |
| `0x0056ac40` | C1 mapped | **un-ported → RVA-forwarding thunk** |
| `0x0056a7a0` | C1 mapped | **un-ported → thunk**; one level deeper (`adb0→aae0→a7a0`), NOT in the named 7 |

**Nothing in KV1 can be `extern`'d — every callee is C1.** Contrast Batch A, where the main
callee `FUN_0055bd80` was already ported (K5).

## 6. Blockers before this batch can be ported

1. **Three targets need a Ghidra disassembly/create-function pass** — `0x0056b310`,
   `0x00569140`, `0x005697f0` are undefined with no plate and no row. `0x0056b310` is the
   priority (always-installed, invoked twice). NB Batch A avoided this by reading raw bytes via
   `memory_read` + offline capstone — viable here too for the smaller ones, but 1712/3168-byte
   bodies are past the point where hand-disassembly beats a real Ghidra pass.
2. **The `+0xf8` invocation question** (§2 trap) must be settled before porting `0x0056adb0` /
   `0x005697f0`.
3. **SSE/float10 traps** in `0056ac40` (float10 return + `movmskps` + the `_DAT_005e5a**` band)
   and `0056aae0` (`0xFFFFFFFF` sign flags) — same class as the K8 SSE work already solved.
