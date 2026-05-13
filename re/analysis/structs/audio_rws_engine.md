# Audio RWS Engine — Node System & Channel Dispatch Globals

**Session:** struct_extract_phase5 (2026-05-12)
**Evidence sources:** audio_sfx_dispatch_d3 SESSION_END.md (15 C1 plates, D-7380..D-7394), audio_sfx_dispatch/OVERVIEW.md, audio_sfx_dispatch_d3/0x005a5f00.md, audio_sfx_dispatch_d3/0x004627b0.md, audio_sfx_dispatch_d3/0x005a71f0.md, audio_sfx_dispatch_d3/0x005a8890.md
**Confidence:** C1 — mechanically confirmed from decompilation; no Frida verification

---

## Audio engine state globals

| Address | Width | Type | Writer RVA | Notes |
|---------|-------|------|-----------|-------|
| `0x007dc94c` | 4 | int32 | FUN_005a5f00 (D-7389) body | Audio engine initialized flag; set to 1 on successful RWS engine creation |
| `0x007dc958` | 4 | int32 | FUN_005a5f00 body | Cleared to 0 on engine init success |
| `0x007dc95c` | 4 | int32 | FUN_005a5f00 body | Cleared to 0 on engine init success |
| `0x00773920` | 4 | int32 | FUN_00499890 (boot_app_init_d2) | OS version code — doubles as audio device state enum in FUN_004627b0 (D-7380): 2/3/4/7 → audio available |
| `0x007dcae0` | 4 | HANDLE | Windows CreateSemaphore call | Win32 semaphore/mutex handle for stream access (WaitForSingleObject / ReleaseSemaphore in FUN_005a8890) |
| `0x007dc8c0` | 4 | int32 | FUN_0055ad30 body | Constraint-overflow or attach-error flag (1 = overflow; 0 = ok) |

### OS version / audio device state enum (0x00773920)

Written by `FUN_00499890` (OS version detector) to one of these values:

| Value | OS | Audio available? |
|-------|----|-----------------|
| 1 | Windows NT | No |
| 2 | Windows 95 | Yes |
| 3 | Windows 98 | Yes |
| 4 | Windows Me | Yes |
| 5 | Windows 2000 | No |
| 6 | Windows XP (Whistler) | No |
| 7 | Win32s | Yes |

Cross-reference: also used in `FUN_00402750` (PIZ filesystem mode selector, cases 1/5/6 → pcVar4=0, cases 2/3/4/7 → pcVar4=1).

---

## RWS audio node constructor family (FUN_005aa560 wrappers)

Four thin 5-arg wrappers over the same RWS audio node constructor `FUN_005aa560`, each binding a different per-type descriptor pointer as arg0.

| D-ID | RVA | Type descriptor global | Node type |
|------|-----|----------------------|-----------|
| D-7381 | 0x005a7f70 | `DAT_007dcae8` | RWS stream node |
| D-7387 | 0x005a6280 | `DAT_007dc980` | Virtual voice node |
| D-7385 | 0x005b8570 | `DAT_007dde70` | Physical voice node |
| D-7394 | 0x005a8e70 | `DAT_007dcb78` | Output mixing node (result → `DAT_0068f61c`) |

`FUN_005aa560` constructor signature: `(descriptor_ptr, ?, ?, ?, ?)` — 5 args; first arg is the per-type descriptor.

---

## Voice / tree registration globals (FUN_005a71f0 — D-7386)

Voice array registration: allocs tree, calls FUN_005a6b70 with root + N.

| Address | Notes |
|---------|-------|
| `DAT_007dca34` | Registered voice count / registration state A |
| `DAT_007dca20` | Registration state B |
| `DAT_007dca38` | Registration state C |

---

## Audio root node and stream pool

| Address | Notes |
|---------|-------|
| `DAT_0069049c` | Audio root node ptr (passed as arg to FUN_005a73b0 teardown and node construction calls) |
| `DAT_006904f0` | 4-stream pool / stream table (cleared by FUN_00462ec0 DestroyStreams at D-7383) |
| `DAT_0068f61c` | Output mixing node result (written by D-7394 FUN_005a8e70 wrapper) |

---

## DSound channel pool (static, from audio_sfx_dispatch/OVERVIEW.md)

200-channel pre-allocated pool. All channels are allocated at DSound init; no runtime allocation.

| Address | Stride | Count | Notes |
|---------|--------|-------|-------|
| `DAT_0068f640` | 8 (pointer) | 200 | Static channel pool — stride-8 pointer array; all slots pre-allocated |
| `DAT_00604570` | `entity_idx × 0x25` | — | Base channel index per entity slice |
| `DAT_00604574` | `entity_idx × 0x25` | — | Channel count per entity slice |
| `DAT_0088e670` + fixed offsets | — | — | Per-vehicle global channel indices (e.g. +0x500, +0x590, +0x6b0, +0x740, +0x860) |

Channel free check: `(*(byte*)(channel_ptr + 0x34) & 2) == 0` means channel is free.

---

## Channel struct layout (partial, from OVERVIEW + FUN_005a8890)

| Offset | Width | Type | Notes |
|--------|-------|------|-------|
| +0x14 | 4 | ptr | Doubly-linked list next (stream node context) |
| +0x18 | 4 | ptr | Doubly-linked list prev (stream node context) |
| +0x30 | 4 | ptr | IDirectSound3DBuffer* (NULL if no 3D) |
| +0x34 | 1 | byte | Flags; bit 1 = currently playing |
| +0x3c | 4 | float | Frequency |
| +0x54 | 4 | ? | Written on stop |
| +0x58 | 4 | float | 3D position X |
| +0x5c | 4 | float | 3D position Y |
| +0x60 | 4 | float | 3D position Z |
| +0xc0 | 28 | ? | Stream path copy slot (7 × int32; written by FUN_005a8890) |
| +0xd78 | 36 | ? | Stream secondary path slot (9 × int32; written by FUN_005a8890) |
| +0xd80 | 4 | ptr | Pointer back to +0xc0 region |
| +0xd9c | 4 | uint32 | Stream status flags; bit 0 set on attach |

---

## Audio guard / system-level flags

| Address | Notes |
|---------|-------|
| `DAT_0068f4c8` | Audio system init flag (0 = not ready; dispatchers return early if 0) |
| `DAT_008aa254` | Player/game-mode count; dispatchers guard `< 2` |

Common gate pattern: `if (DAT_0068f4c8 == 0) return`. Second gate: `FUN_00492d10() == 3` (race in progress check).

---

## Source path

`\toast\Code\src\AppCode\AUDIO\soundengine.c` — confirmed leaked in FUN_00462ec0 DestroyStreams debug string (D-7383).

---

## Open uncertainties

| Gap | Evidence missing |
|-----|-----------------|
| `DAT_007dc958` and `DAT_007dc95c` semantic meaning | Cleared on init but no reader seen; may be pre-init error counters |
| `FUN_005aa560` full 5-arg signature | Only arg0 (descriptor ptr) confirmed; args 1-4 vary by call site |
| Per-type node descriptor layout at e.g. `DAT_007dcae8` | Passed opaquely to FUN_005aa560; internal layout not decomped |
