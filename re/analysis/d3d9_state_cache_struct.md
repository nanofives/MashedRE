# D3D9 deferred render-/texture-/sampler-state cache — struct contract

Binary anchor: `original/MASHED.exe.unpatched`
SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
Decompilation source: Ghidra slot `Mashed_pool11`, session
`2794c0b887f649f0b145142852a49bc2`, 2026-06-25 (read-only). Every address below
was read from that session; no value is inferred.

This document is the shared contract for the `Render/D3d9StateCache.cpp`
reimplementation cluster (RVAs `0x004d5480..0x004d7c5d`). It is derived
**mechanically** from the decompilation of the flush (`FUN_004d53b0`), the
30-case getter (`FUN_004d6910`, the "rosetta stone"), the cold-init
(`FUN_004d5bc0`), the device-reset (`FUN_004d6200`), the dispatcher
(`FUN_004d7480`), and the leaf setters. Where two functions independently
confirm an address/stride it is marked **(cross-confirmed)**.

---

## 1. The big picture

The subsystem is a **deferred state cache** sitting in front of the D3D9 device
(`IDirect3DDevice9* @ DAT_007d4110`). There are three sub-caches:

1. **Render-state (RS)** — deferred. Setters MARK a state dirty and queue its
   index; a later flush DRAINS the queue, sending only changed states to the
   device via `SetRenderState`.
2. **Texture-stage-state (TSS)** — deferred. Same shape, keyed by
   `stage*0x21 + state`; flushed via `SetTextureStageState`.
3. **Sampler-state** — *direct* (NOT deferred). `FUN_004d5570` writes the
   device immediately via `SetSamplerState`, guarded by a value cache.

In addition, the dispatcher (`FUN_004d7480`) and several setters maintain
per-control **"input caches"** (one global per logical control) holding the last
*input parameter* seen, used purely for change-detection. The 30-case getter
`FUN_004d6910` reads these input caches.

### Device vtable offsets (cross-confirmed across flush + leaves)

| Offset | Method | Confirmed at |
|--------|--------|--------------|
| `+0xe4`  | `SetRenderState(State, Value)`              | `FUN_004d53b0` RS drain |
| `+0x10c` | `SetTextureStageState(Stage, Type, Value)`  | `FUN_004d53b0` TSS drain |
| `+0x114` | `SetSamplerState(Sampler, Type, Value)`     | `FUN_004d5570`, `FUN_004d6b90`, dispatcher cases 2/3/4/0xd |
| `+0x104` | `SetTexture(Stage, pTexture)`               | `FUN_004d6ce0`, init/reset stage loops |
| `+0x124` | `SetPaletteEntries`-class (palette stage)   | `FUN_004d6ce0` non-null branch |

`DAT_007d4110` = `IDirect3DDevice9*`. `*DAT_007d4110` = its vtable.

---

## 2. Render-state (RS) cache — the UNIFIED desired/dirty/applied model

The single most important finding: **the per-setter "value-store" globals
(0x5840, 0x5890, 0x5998, …) are NOT separate variables — they are aliases into
one contiguous desired/dirty array `DAT_007d57f8`, indexed by `opcode*8`.**

The flush (`FUN_004d53b0`) treats every queued entry as an RS *state index*
`iVar1` and applies it uniformly:

```c
// FUN_004d53b0 RS path (0x004d53b0), drained when DAT_007d6c14 != 0:
iVar1   = (&DAT_007d5168)[uVar7];          // queued state index
desired = (&DAT_007d57f8)[iVar1 * 2];      // DAT_007d57f8 + iVar1*8
applied = (&DAT_007d54b0)[iVar1];          // DAT_007d54b0 + iVar1*4
(&DAT_007d57fc)[iVar1 * 2] = 0;            // clear dirty flag (DAT_007d57f8 + iVar1*8 + 4)
if (applied != desired) {
    (&DAT_007d54b0)[iVar1] = desired;
    device->vtbl[0xe4](device, iVar1, desired);   // SetRenderState
}
```

### RS array layout

| Region | Base | Stride | Element | Meaning |
|--------|------|--------|---------|---------|
| **desired / dirty (interleaved)** | `0x007d57f8` | **8 bytes / state** | `[+0]`=desired value, `[+4]`=dirty flag | per-RS-state pending value + one-shot dirty flag |
| **applied** | `0x007d54b0` | **4 bytes / state** | last value actually sent to device | change-detect vs desired during flush |
| **dirty queue** | `0x007d5168` | 4 bytes (uint32) | list of dirtied state indices | drained FIFO by flush |
| **dirty-queue count** | `0x007d6c14` | — | running write index/count | reset to 0 after RS drain |

**Verification of the alias identity** (`0x57f8 + opcode*8` == per-setter store):

| Setter (RVA) | opcode | store addr | dirty addr | `0x57f8+op*8` | `0x57fc+op*8` |
|--------------|--------|-----------|-----------|---------------|---------------|
| `FUN_004d7bc0` filter   | `0x09` | `0x7d5840` | `0x7d5844` | `0x5840` ✓ | `0x5844` ✓ |
| `FUN_004d6c40`          | `0x13` | `0x7d5890` | `0x7d5894` | `0x5890` ✓ | `0x5894` ✓ |
| `FUN_004d6c90`          | `0x14` | `0x7d5898` | `0x7d589c` | `0x5898` ✓ | `0x589c` ✓ |
| `FUN_004d7c10` address  | `0x16` | `0x7d58a8` | `0x7d58ac` | `0x58a8` ✓ | `0x58ac` ✓ |
| `FUN_004d7ac0` fog (op7) | `0x07` | `0x7d5830` | `0x7d5834` | `0x5830` ✓ | `0x5834` ✓ |
| `FUN_004d7ac0` (op0x17) | `0x17` | `0x7d58b0` | `0x7d58b4` | `0x58b0` ✓ | `0x58b4` ✓ |
| `FUN_004d7100` (op0x1b) | `0x1b` | `0x7d58d0` | `0x7d58d4` | `0x58d0` ✓ | `0x58d4` ✓ |
| `FUN_004d7100` (op0xf)  | `0x0f` | `0x7d5870` | `0x7d5874` | `0x5870` ✓ | `0x5874` ✓ |
| `FUN_004d7200`          | `0x34` | `0x7d5998` | `0x7d599c` | `0x5998` ✓ | `0x599c` ✓ |
| `FUN_004d7340`          | `0x38` | `0x7d59b8` | `0x7d59bc` | `0x59b8` ✓ | `0x59bc` ✓ |
| `FUN_004d7430`          | `0x3b` | `0x7d59d0` | `0x7d59d4` | `0x59d0` ✓ | `0x59d4` ✓ |

`FUN_004d5480` (`Mark4d5480`) is the *generic* form of all of the above: it
takes `(i, v)` and does `DAT_007d57f8[i*2]=v; if(!dirty) queue i; dirty=1`. The
per-opcode setters are specializations that table-map their argument first.

### Known RS state indices (opcodes) used by the cluster

From the dispatcher + init/reset enqueues (mechanical, no semantic inference):
`0x07, 0x09, 0x0e, 0x0f, 0x13, 0x14, 0x16, 0x17, 0x19, 0x1b, 0x1c, 0x22, 0x23,
0x24, 0x25, 0x26, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x8b, 0x8c,
0x8e, 0x8f, 0x89, 0x91, 0x92, 0x93, 0x1a, 0x1d, 0x16, 0x18`.
The `desired/applied` arrays are sized for index `0xd2` (`FUN_004d5bc0` zero
loop `iVar3 = 0xd2`, i.e. 210 dwords of applied at `0x54b0`; the desired/dirty
loop runs `uVar2 < 0x348` = 210 entries × 4 bytes input → 210 × 8-byte slots).

---

## 3. Texture-stage-state (TSS) cache

Same deferred shape, keyed by `slot = stage*0x21 + state` (stage*33+state).

```c
// FUN_004d53b0 TSS path (0x004d53b0), drained when DAT_007d6c18 != 0:
stage   = (&DAT_007d62a8)[uVar7*2];        // pair-queue entry: stage
state   = (&DAT_007d62ac)[uVar7*2];        // pair-queue entry: state (0x62a8+4)
slot    = stage + state + stage*0x20;      // = stage*0x21 + state
desired = (&DAT_007d4720)[slot*2];         // DAT_007d4720 + slot*8
applied = (&DAT_007d5e88)[slot];           // DAT_007d5e88 + slot*4
(&DAT_007d4724)[slot*2] = 0;               // clear dirty (DAT_007d4720 + slot*8 + 4)
if (applied != desired) {
    (&DAT_007d5e88)[slot] = desired;
    device->vtbl[0x10c](device, stage, state, desired);   // SetTextureStageState
}
```

### TSS array layout

| Region | Base | Stride | Element | Meaning |
|--------|------|--------|---------|---------|
| **desired / dirty (interleaved)** | `0x007d4720` | **8 bytes / slot** | `[+0]`=desired, `[+4]`=dirty | per-(stage,state) pending value + dirty |
| **applied** | `0x007d5e88` | **4 bytes / slot** | last value sent to device | change-detect during flush |
| **pair-queue** | `0x007d62a8` | **8 bytes / entry** | `[+0]`=stage, `[+4]`=state | dirtied (stage,state) pairs |
| **pair-queue count** | `0x007d6c18` | — | running count | reset to 0 after TSS drain |

`slot` range: 8 stages × 0x21 states = `0x108` slots (`FUN_004d5bc0` zero loop
`iVar3 = 0x108` over applied `0x5e88`; desired/dirty loop is `8 × 0x21`).

`FUN_004d54f0` (`Set4d54f0`) is the generic TSS marker: `(a,b,v)` →
`slot=(a*33+b)`; `DAT_007d4720[slot*2]=v`; if `!dirty` queue `(a,b)` into
`0x62a8`, bump `0x6c18`. **(cross-confirmed: flush slot formula ==
setter slot formula == getter `FUN_004d5550` read offset
`DAT_007d4720[stage*0x42 + state*2]`.)**

---

## 4. Sampler-state cache (DIRECT, not deferred)

```c
// FUN_004d5570(sampler, state, value):
idx = state + sampler*0xe;                 // 14 states per sampler
if ((&DAT_007d4f60)[idx] != value) {       // DAT_007d4f60 + idx*4
    (&DAT_007d4f60)[idx] = value;
    device->vtbl[0x114](device, sampler, state, value);   // SetSamplerState immediately
}
```

| Region | Base | Stride | Meaning |
|--------|------|--------|---------|
| **sampler value cache** | `0x007d4f60` | 4 bytes / `(state + sampler*0xe)` | last value sent to `SetSamplerState`; `0xffffffff`-initialized by init/reset |

`FUN_004d5bc0` zeroes `0x4f60` with loop count `0x70` (= 112 = 8 samplers × 14).

The stage-0 filter setter `FUN_004d6b90` writes a parallel set of *named*
sampler caches (`0x7d4f74/78/7c/88`) and the per-stage filter-index cache
`DAT_007d6b3c`, reading min/mag filter values from tables
`0x005d8ca8` (min, stride 8) and `0x005d8cac` (mag, stride 8).

---

## 5. Per-control "input caches" (read by the 30-case getter `FUN_004d6910`)

`FUN_004d6910(enum, out)` is a pure dispatch-read: `*out = DAT_...; return 1`
(case 2 is special — returns 0 with `*out=0` when `DAT_007d6b34 != DAT_007d6b38`).
These globals hold the **last input parameter** to the corresponding control,
NOT the mapped/applied value. This is the enum→offset rosetta:

| enum | global | enum | global | enum | global |
|------|--------|------|--------|------|--------|
| 1  | `DAT_007d6b30` | 0xc  | `DAT_007d6b10` | 0x17 | `DAT_007d6af8` |
| 2  | `DAT_007d6b34`==`DAT_007d6b38`? | 0xd | `DAT_007d6b40` | 0x18 | `DAT_007d6afc` |
| 3  | `DAT_007d6b34` | 0xe  | `DAT_007d6b1c` | 0x19 | `DAT_007d6b00` |
| 4  | `DAT_007d6b38` | 0xf  | `DAT_007d6b28` | 0x1a | `DAT_007d6b04` |
| 6  | `DAT_007d6aec` | 0x10 | `DAT_007d6b20` | 0x1b | `DAT_007d6b08` |
| 7  | `DAT_007d6b2c` | 0x11 | `DAT_007d6b24` | 0x1c | `DAT_007d6b0c` |
| 8  | `DAT_007d6ae8` | 0x14 | `DAT_007d6b18` | 0x1d | `DAT_007d6bf8` |
| 9  | `DAT_007d6b3c` | 0x15 | `DAT_007d6af0` | 0x1e | `DAT_007d58b8` |
| 0xa| `DAT_007d6bf0` | 0x16 | `DAT_007d6af4` |      |  |
| 0xb| `DAT_007d6bf4` |      |        |      |  |

`FUN_004d71f0` returns `DAT_007d6b10` (same as case 0xc) — the texturing/fog
input cache, written by `FUN_004d7100`.

The per-stage texture cache `DAT_007d6b30[stage*6]` (stride 0x18) is the case-1
read AND the `FUN_004d6ce0` SetTexture cache (cross-confirmed); the stage record
is 6 dwords: `[0]`=texture, `[+0xc]`=filter-index (`0x6b3c`), `[+0x10]`/`[+0x14]`
addr modes, `[+0x18]`=border color, `[+0x1c]`=aniso (seen in init/reset loop
`piVar8 += 6`).

---

## 6. Lookup tables (.rdata, image-resident; read-only)

| Table base | Stride | Read by | Note |
|-----------|--------|---------|------|
| `0x005d8c48` | 4 | `FUN_004d7bc0`, reset (`DAT_007d6b2c`) | RS filter-mode map |
| `0x005d8c54` | 4 | dispatcher case 0x10, init/reset (`DAT_007d6b20`) | shade-mode map |
| `0x005d8c64` | 4 | `FUN_004d6c40/6c90`, reset | blend-index map |
| `0x005d8c94` | 4 | dispatcher cases 2/3/4, reset stage loop | TSS arg map |
| `0x005d8ca8` | 8 | `FUN_004d6b90`, reset | min-filter map |
| `0x005d8cac` | 8 | `FUN_004d6b90`, reset | mag-filter map (`0x5d8ca8+4`) |
| `0x005d8d18` | 4 | `FUN_004d7c10`, reset (`DAT_007d6b18`) | address-mode map |
| `0x005d8d28` | 4 | `FUN_004d7250/72a0/72f0`, reset | blend-op map |
| `0x005d8d4c` | 4 | `FUN_004d7340`, dispatcher 0x1d, reset | `0x5d8d28+0x24` map |

---

## 7. Lifecycle (init / reset)

- **`FUN_004d5bc0` (cold init)** — caller `FUN_004c7a70` (C2, `_rwDeviceSystemFn`).
  Fills applied `0x54b0` (`0xd2` dwords) and TSS applied `0x5e88` (`0x108`) and
  sampler cache `0x4f60` (`0x70`) with `0xffffffff`; rebuilds the
  desired/dirty arrays from applied; zeroes all queue counts; sets hardcoded
  default input caches; enqueues defaults via `FUN_004d5480`/`54f0`/`5570`;
  per-stage `SetTexture(stage,0)` loop; ends with `FUN_004d53b0()` flush.
  Reads hardware caps `DAT_00911fc4` (bits 1/0x100/0x100000) and `DAT_00911fe0`
  (bit 0x200). Contains decompiler `extraout_EDX` artifacts (register values
  flowing from preceding calls into `DAT_007d6af0`/`6b04`/`6bf0`/`6bf8`).
- **`FUN_004d6200` (device-reset save/restore)** — caller `FUN_004c9cd0` (C2,
  `RESET_FN`). Saves three RS values (`0x5918/5920/58b8`), re-zeroes the applied
  arrays to `0xffffffff`, then re-queues every control from its *current input
  cache* (so the post-reset device gets the full state re-pushed); ends with
  `FUN_004d53b0()`. Also hardware-cap dependent.

---

## 8. Cluster call graph + caller-gate (Ghidra session, 2026-06-25)

```
FUN_004c7a70 (C2) ── calls ──> FUN_004d5bc0 (init) ──> 53b0, 5480, 54f0, 5570
FUN_004c9cd0 (C2) ── calls ──> FUN_004d6200 (reset) ──> 53b0, 5480, 54f0, 5570
FUN_004d07b0 (C2) ── calls ──> FUN_004d7480 (dispatcher) ──> 17 cluster setters
                   └─ calls ──> FUN_004d6910 (getter), 54d0, 6ce0, 7100
FUN_004db770/00499d90/004cb2f0 (C2) ── call ──> FUN_004d53b0 (flush)
FUN_004d3090 (C2) ── calls ──> FUN_004d6ce0
```

Dispatcher `FUN_004d7480` callees (all 17 cluster members): `5480, 6b90, 6c40,
6c90, 6ce0, 7100, 7200, 7250, 72a0, 72f0, 7340, 7390, 73e0, 7430, 7ac0, 7bc0,
7c10` — 14 already C3 + 3 in-scope (`6b90, 6ce0, 7ac0`).

### Caller-gate verdict per in-scope C2 function

| RVA | role | callers | C2+ caller? | verdict |
|-----|------|---------|-------------|---------|
| `004d6910` | rosetta getter | `004d07b0` (C2) | yes | **promote** |
| `004d7ac0` | fog toggle | `004d7480` (in-module→C3) | yes | **promote** |
| `004d5570` | sampler set | `5bc0`,`6200` (in-module→C3) | yes | **promote** |
| `004d6b90` | filter set | `004d7480` (in-module→C3) | yes | **promote** |
| `004d53b0` | flush | `db770`/`499d90`/`cb2f0` (C2) + in-module | yes | **promote** |
| `004d6ce0` | SetTexture binder | `07b0`/`3090` (C2) | yes | gate OK; verification = texture-deref (only null path clean) |
| `004d7480` | dispatcher | `004d07b0` (C2) | yes | gate OK; verification = 600B/12 inline device cases |
| `004d5bc0` | init | `004c7a70` (C2) | yes | gate OK; verification = HW-cap + 30+ device calls |
| `004d6200` | reset | `004c9cd0` (C2) | yes | gate OK; verification = HW-cap + 30+ device calls |
| `004d6e70` | per-stage binder | all callers C1 (`0x540xxx` third-party RW) | **NO** | **QUEUE** (caller-gate) |
| `004d5550` | TSS getter | *none* | **NO** | **QUEUE** (no caller) |
| `004d55b0` | material color | *none* | **NO** | **QUEUE** (no caller) |
| `004d71f0` | texturing getter | `005405c0`/`005412d0`/`00541d40` all C1 | **NO** | **QUEUE** (caller-gate; per workstream note) |

QUEUE rationale follows the caller-gate / leaf-exemption ruling
([[feedback_c3_caller_gate_leaf_exemption]]): a verified leaf getter whose
callers are all C1 (or which has no caller) must be QUEUED, not promoted.

---

## 9. Promotion ledger (workstream reimpl/d3d9-state-cache, 2026-06-25)

**PROMOTED C2->C3 (5)** — reimpl in `Render/D3d9StateCache.cpp`, each a SOUND
non-degenerate `run_diff` (exit 0). New arg_types `cache_roundtrip` /
`cache_setter_observe` (SWEEP-CRITICAL; in `diff_template.js` + `run_diff.py`):

| RVA | name | arg_type | diff | CSV |
|-----|------|----------|------|-----|
| 0x004d6910 | D3d9State_Get | cache_roundtrip | 10/10 GREEN | log/diff_d3d9_state_get.csv |
| 0x004d7ac0 | D3d9State_FogBlendToggle | cache_setter_observe | 5/5 GREEN | log/diff_d3d9_fog_blend_toggle.csv |
| 0x004d5570 | D3d9State_SetSampler | cache_setter_observe | 4/4 GREEN | log/diff_d3d9_set_sampler.csv |
| 0x004d6b90 | D3d9State_SetStage0Filter | cache_setter_observe | 4/4 GREEN | log/diff_d3d9_set_stage0_filter.csv |
| 0x004d53b0 | D3d9State_Flush | cache_setter_observe | 3/3 GREEN | log/diff_d3d9_state_flush.csv |

The method (reimpl-first + synthetic cache control) defeats the menu-attach
degeneracy that stalls leaf-picking: each function's own cache state is seeded
before the call, so getters/setters/flush that read a constant at idle become
discriminating. Device-calling functions (5570/6b90/53b0) verify soundly via
cache-observe — the SetSamplerState/SetRenderState/SetTSS arguments are
byte-derived from the same locals that index+fill the observed cache slots, so
the cache observation fully constrains the device-call args; the device call
itself is a benign side-effect on the throwaway diff process.

**DEFERRED — reimpl + promotion pending, cited reasons (4):**

| RVA | name | why deferred |
|-----|------|--------------|
| 0x004d6ce0 | SetTexture binder | non-null path derefs a LIVE texture object (`DAT_00911ae4+param_1` -> D3D texture @+0, palette @+4, alpha flag @+8); menu-attach synthetic harness can't faithfully populate it without a fake-texture builder. Only the null-clear + per-stage-cache paths are cleanly diffable -> partial verification would overclaim. Needs a `texture_bind_observe` arg_type (alloc fake tex, point `DAT_00911ae4`+offset at it). |
| 0x004d7480 | dispatcher | 600 B, 12 inline cases. Cases 2/3/4/0xd call SetSamplerState (table 0x5d8c94); 0xe/0x10 branch on HW caps `DAT_00911fc4` (&0x180/&0x100/&0x100000); 0x11 derefs param_2 as float*; **case 0x1d has a register artifact** (`param_2 = extraout_EDX` after `FUN_004d5480(0xf,..)` — EDX value flows into the 0x5d8d4c index, not modellable as clean C). Needs a per-case HW-cap-seeded device diff matrix + naked-asm for case 0x1d. Callee+caller gate already fine. |
| 0x004d5bc0 | cold init | ~1.6 KB; 30+ device calls; HW-cap-dependent (`DAT_00911fc4`/`DAT_00911fe0`); decompiler `extraout_EDX` artifacts feeding `DAT_007d6af0`/`6b04`/`6bf0`/`6bf8`. Needs naked-asm + cap injection. Reimpl-for-standalone tracked separately. |
| 0x004d6200 | device-reset | ~1.8 KB; same HW-cap dependence + re-queue-from-current-cache structure as init. Same deferral basis. |

**QUEUED — caller-gate-blocked (4):** `0x004d6e70` (callers all C1 third-party
0x540xxx), `0x004d5550` (no callers), `0x004d55b0` (no callers), `0x004d71f0`
(callers all C1). Verified leaves; promote only after a C2+ caller is reimpl'd
(identified-caller clause) or via a QUEUE-not-promote ruling.
