# Particle Emitter Structs — Mashed Effects System

**Session:** effects_particle_d4-20260512
**Source sessions:** effects_particle d1/d2/d3/d4 (2026-05-02 … 2026-05-12)
**Status:** initial draft — three ring-buffer entry layouts partially mapped; render pool fully mapped; emitter channel partially mapped; staging globals fully mapped

---

## Overview

The Mashed particle system has three separate simulation ring buffers (explosion/fireball, debris, slow-decay), each with its own per-entry struct. All three funnel through shared global staging variables and a common render-pool/batch struct for GPU submission. The emitter channel struct is a RpPrtStd-style layer above the render pool.

**STOP-AND-ASK note:** Three distinct code paths confirmed (smoke/slow-decay, debris, explosion/fireball). Each is documented separately below.

---

## 1. Staging Globals (particle working variables)

These globals are written by the setter functions before each `FUN_00476d00` call. They act as a per-particle staging area.

| Global address | Type | Setter | Role |
|----------------|------|--------|------|
| 0x006924d8 | float (1 dword) | FUN_00476a30 | scalar / angle [UNCERTAIN U-0591] |
| 0x006924dc | float | FUN_004769d0 | velocity X |
| 0x006924e0 | float | FUN_004769d0 | velocity Y |
| 0x006924e4–0x006924e7 | 4 bytes | not covered | [UNCERTAIN — gap between velocity and matrix block] |
| 0x006924e8 | float[16] = 64 bytes | not covered by a setter | matrix block (4×4); source for FUN_00476d00 channel +0x1c |
| 0x00692528 | float | FUN_004769a0 | position X |
| 0x0069252c | float | FUN_004769a0 | position Y |
| 0x00692530 | float | FUN_004769a0 | position Z |
| 0x00692534 | dword[8] = 32 bytes | not covered by a setter | 8-dword block; source for FUN_00476d00 channel +0x28 |
| 0x00692554 | dword (1) | FUN_004769f0 | colour/alpha packed dword |
| 0x00692598 | float | FUN_00476a40 | colour R |
| 0x0069259c | float | FUN_00476a40 | colour G |
| 0x006925a0 | float | FUN_00476a40 | colour B |
| 0x006925a4 | float | FUN_00476a40 | colour A |
| 0x006925a8 | float[3] | — | default position (used by FUN_004769a0 when param_1 == NULL) |

Default values (when setter receives NULL param):
- Default velocity: DAT_00613288 (2 floats)
- Default colour/alpha: DAT_00613290 (1 dword)
- Default RGBA tuple: DAT_00613294 (4 floats)

---

## 2. Render Pool / Batch Struct

Pointer passed as param_1 to **FUN_00476d00** (0x00476d00). Carries separate arrays for each particle attribute channel; `FUN_00476d00` appends one entry per call.

The six render pools instantiated in `FUN_00490500` (the main update loop):

| Pool global | Used for |
|-------------|----------|
| DAT_00766978 | slow-decay particles |
| DAT_0076d940 | fire-column sub-particles |
| DAT_007668f8 | debris particles |
| DAT_00766f00 | smoke trail particles |
| DAT_007706d8 | fireball particles |
| DAT_0076d900 | [UNCERTAIN U-0593] sixth pool |

### Render pool struct layout

| Offset | Type | Role |
|--------|------|------|
| +0x00 | dword | [UNCERTAIN — not accessed by FUN_00476d00] |
| +0x04 | dword | [UNCERTAIN — not accessed by FUN_00476d00] |
| +0x08 | int | capacity (max entries); write guard `*(+0xc) < *(+0x08)` |
| +0x0c | int | write index (current particle count); incremented by FUN_00476d00 |
| +0x10 | float* | position array; stride 0x0c (3 floats: X, Y, Z); source: DAT_00692528/2c/30 |
| +0x14 | float* | velocity array; stride 0x08 (2 floats: X, Y); source: DAT_006924dc/e0 |
| +0x18 | dword* | colour/alpha array; stride 0x04 (1 packed dword); source: DAT_00692554 |
| +0x1c | float* | matrix block array; stride 0x40 (16 dwords = 4×4 float matrix); source: DAT_006924e8 |
| +0x20 | dword* | scalar array; stride 0x04 (1 dword); source: DAT_006924d8 |
| +0x24 | float* | RGBA array; stride 0x10 (4 floats: R, G, B, A); source: DAT_00692598/9c/a0/a4 |
| +0x28 | dword* | 8-dword block array; stride 0x20 (8 dwords); source: DAT_00692534 |

Any pointer field that is null causes that channel to be skipped (no write).

Flushed to GPU by **FUN_00476df0** (0x00476df0) which calls FUN_00535700 (channel buffer selector) and FUN_00535910 (commit pending channels) per emitter.

---

## 3. Emitter Channel Struct (RpPrtStd-style)

Base: `*(DAT_007dc57c + emitter_index)`. Indexed by emitter_index (exact stride unknown — U-0968).

Source functions: FUN_00535700 (read/select), FUN_00535910 (commit).

### Layout

| Offset | Type | Role |
|--------|------|------|
| +0x3c | uint | pending dirty channel mask (written by FUN_00535700 when param_4 & 0x40000000; cleared by FUN_00535910) |
| +0x40 | uint | accumulated committed channels (OR-accumulator; FUN_00535910 ORs +0x3c into here) |
| +0x4c | ptr | channel 0x001 data pointer |
| +0x50 | int | channel 0x001 count |
| +0x54 | ptr | channel 0x008 data pointer |
| +0x58 | int | channel 0x008 count |
| +0x5c | ptr | channel 0x010 data pointer |
| +0x60 | int | channel 0x010 count |
| +0x64 | ptr | channel 0x004 data pointer |
| +0x68 | int | channel 0x004 count |
| +0x6c | ptr | channel 0x002 data pointer |
| +0x70 | int | channel 0x002 count |
| +0x74 | ptr | channel 0x040 data pointer |
| +0x78 | int | channel 0x040 count |
| +0x7c | ptr | channel 0x020 data pointer |
| +0x80 | int | channel 0x020 count |
| +0x84 | ptr | channel 0x080 data pointer |
| +0x88 | int | channel 0x080 count |
| +0x8c | ptr | channel 0x100 data pointer |
| +0x90 | int | channel 0x100 count |
| +0x9c | int | count override (if non-zero, replaces per-channel count in FUN_00535700) |
| +0xa0 | uint | capability bitmask (channel-enable flags; AND with param_3 gates FUN_00535700 early return) |

Minimum struct size: ≥ 0xa4 bytes. Stride unknown (U-0968).

---

## 4. Explosion / Fireball Ring Buffer Entry

Base: **DAT_0076d99c**. Stride: **0x122 dwords = 0x488 bytes** per entry. End address: **0x770854**.

Driven by `FUN_0048ebf0` (fire-column burst spawn) and iterated by `FUN_00490500` (main update loop, Loop 1).

### Partially mapped offsets (relative to entry base)

| Offset | Type | Role | Source |
|--------|------|------|--------|
| −0x07 dwords (pfVar6[−7]) = entry−28 | float | active flag: ≠ 0.0 = entry live | FUN_00490500 loop1 |
| +0x00 (entry base) | float | inner fire timer (zeroed by FUN_0048ebf0) | FUN_0048ebf0 |
| +0x04 (dword 1) | int | spawn-count / ring index (incremented by FUN_0048ebf0) | FUN_0048ebf0 |
| +0x08 (pfVar6[3]) | int | fire-column particle count | FUN_00490500 loop1 |
| +0x10 (pfVar6[4]) | int | smoke-trail particle count | FUN_00490500 loop1 |
| +pfVar6[3]*stride offsets | float[3] | fire-column particle positions (pfVar6+0xeb/0x10b/0xbd) | FUN_00490500 loop1 |
| +smoke offsets | float[3] | smoke-trail particle positions (pfVar6+0x8b/0xb3/0x5c) | FUN_00490500 loop1 |

Sub-arrays at pfVar6+0x8b, +0xb3, +0x5c (smoke): age float at pfVar9[−8]; 15-phase table at DAT_006145b0 stride 0x10.
Sub-arrays at pfVar6+0xeb, +0x10b, +0xbd (fire): position + velocity; phase float accumulator; phase types 1/2/3 as IEEE754 denormals.

### Known globals
| Address | Role |
|---------|------|
| 0x0076d99c | ring base |
| 0x0076d9ac | `[param_3*0x122]` — fire-column particle slot index |
| 0x0076d998 | `[param_3*0x122]` — spawn-count / "has queued re-use position" flag |
| 0x0076daf0/f4/f8 | first-time spawn position (x/y/z) per slot |
| 0x0076dafc | velocity seed per slot |
| 0x0076db00 | secondary velocity seed per slot |
| 0x0076db04 | counter (zeroed on first-time spawn) |
| 0x0076dc88 | saved re-use position array |
| 0x0076ddc8 | age byte array for re-use positions |
| 0x006145b0 | 15-phase colour/alpha LUT (smoke), stride 0x10 |
| 0x00614530 | fire-phase colour LUT, stride 0x10 |

[UNCERTAIN U-0588]: full 0x488-byte layout not mapped; sub-field offsets pfVar6+0xeb, +0x10b, +0xbd, +0x8b, +0xb3, +0x5c are cited relative to loop pointer but exact byte positions require further analysis.

---

## 5. Debris Ring Buffer Entry

Base: **DAT_00770718**. Stride: **9 dwords = 36 bytes** per entry. Capacity: **100 entries**. End: **0x771528**.

Allocated by `FUN_0048f290`; iterated by `FUN_00490500` (Loop 2).

### Entry layout (byte offsets from entry base = `DAT_00770718 + index * 36`)

| Byte offset | Type | Role |
|-------------|------|------|
| +0x00 | int | active flag (0 = free; 1 = live) |
| +0x04 | float | position X |
| +0x08 | float | position Y |
| +0x0c | float | position Z |
| +0x10 | float | param_2 copy #1 (size/intensity) |
| +0x14 | float | param_2 copy #2 (size/intensity) |
| +0x18 | float | param_3 (lifetime or scale variant) |
| +0x19 | byte | colour R |
| +0x1a | byte | colour G |
| +0x1b | byte | colour B |
| +0x1c | byte (DAT_00770737 series) | age byte (also LRU candidate field) |
| +0x1d–0x1f | ? | [UNCERTAIN — not mapped by analyzed functions] |

Note: FUN_0048f290 references `DAT_00770737 + entry_offset` for the age byte field, and separately `DAT_00770718[index × 9]` for the active flag. The stride cited is 9 dwords (36 bytes) at address 0x0048f29e.

---

## 6. Slow-Decay Particle Buffer Entry

Base: **DAT_00766a1c** (age field base). Stride: **8 dwords = 32 bytes** per entry. Capacity: **40 entries** (0x28). End: **0x766f18**.

Allocated by `FUN_0048f420`; iterated by `FUN_00490500` (Loop 3).

The FUN_0048f420 plate references relative to `DAT_00766a18` (the age base); FUN_00490500 iterates from `DAT_00766a1c`. The per-entry layout deduced from FUN_0048f420:

| Dword index (relative to piVar1[−7] anchor) | Offset | Role |
|----------------------------------------------|--------|------|
| −7 | −28 | type field (piVar1[−6] in FUN_0048f420 loop; checked as zero = free) |
| −6 | −24 | position X DAT_00766a04 series |
| −5 | −20 | position Y DAT_00766a08 series |
| −4 | −16 | position Z DAT_0076a00c series |
| −3 | −12 | param_2 copy #1 DAT_00766a10 |
| −2 | −8  | param_2 copy #2 DAT_00766a14 |
| −1 | −4  | phase index (pfVar11[−1] in Loop 3) |
| 0 | 0   | age accumulator (DAT_00766a18/0x766a1c; zeroed on alloc) |
| +1 | +4  | phase accumulator (DAT_00766a1c; zeroed on alloc) |

[UNCERTAIN U-0587]: exact byte boundary offsets require cross-checking FUN_0048f420 globals against FUN_00490500 loop3 pointer arithmetic.

---

## Powerups Linkage

The explosion ring buffer and debris ring buffer are populated by powerup detonation paths. From `FUN_00490500`:
- `FUN_0048f290` (debris allocator) is called probabilistically from loop1 fire-column transitions.
- `FUN_0048f420` (slow-decay allocator) is called probabilistically from loop1 smoke trail transitions.

The powerups subsystem (see `re/analysis/powerups_d2/`) has effect-setter functions at 0x00476cb0 and 0x00476c10 that write into the emitter channel struct at `DAT_007dc57c`. The exact call chain from "powerup detonates" to "explosion ring entry allocated" has not yet been traced — no powerup→particle entry-point function has been identified in the current bucket. [UNCERTAIN — needs powerups_d3 or powerups_d4 sweep to close.]
