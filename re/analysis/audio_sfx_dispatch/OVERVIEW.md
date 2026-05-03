# audio_sfx_dispatch — bucket overview
session: audio_sfx_dispatch-20260503-0601
slot: Mashed_pool10
confidence: C1 (mechanically described from decompilation)

## Architecture finding

Mashed does NOT use a dynamic DirectSound voice pool. Instead:

- **Static channel pool**: 200 channel structs in `DAT_0068f640` (stride-8 pointer array), all pre-allocated at DSound init
- **Per-entity channel tables**: `DAT_00604570[entity_idx * 0x25]` = base channel index; `DAT_00604574[entity_idx * 0x25]` = count. Each entity owns a contiguous slice of the 200-channel pool
- **Per-vehicle global indices**: stored in `DAT_0088e670 + fixed_offset` (e.g. +0x500, +0x590, +0x6b0, +0x740, +0x860...) — one pre-assigned channel per continuous sound emitter
- **Free-slot check**: `(*(byte *)(channel_ptr + 0x34) & 2) == 0` means channel is free to trigger

## Channel struct layout (partial, from decompilation)

```
channel_ptr+0x30  → IDirectSound3DBuffer* (or NULL if no 3D)
channel_ptr+0x34  → flags byte; bit 1 = currently playing
channel_ptr+0x3c  → frequency (float)
channel_ptr+0x54  → (written on stop)
channel_ptr+0x58  → 3D position X (float)
channel_ptr+0x5c  → 3D position Y (float)
channel_ptr+0x60  → 3D position Z (float)
```

## Common primitives (used by every dispatcher)

- `FUN_005a66d0(ch_ptr, 1)` — stop channel (param_2=1) or release (param_2=0)
- `FUN_0045e0f0(ch_idx, vol)` — set volume on channel by index
- `FUN_005a6dc0(3d_ptr, 4, 1, freq)` — set 3D buffer frequency
- `FUN_005a6d60(3d_ptr, 0, 1, pos_vec)` — set 3D buffer position
- `FUN_0045daf0(ch_idx, entity_idx)` — update channel 3D pos from entity world position

## Guard flags

- `DAT_0068f4c8` — audio system init flag (0 = not ready, skip dispatch)
- `DAT_008aa254` — player/game-mode count; dispatchers guard `< 2`
- `FUN_00492d10() == 3` — game state check (race in progress)
- `FUN_0040e350() == 5` — another game state gate

## Key frequency constant

- `0x46ac4400` (raw float) = appears in almost every dispatcher as the "default" or "max" frequency
- Randomized pitch via `FUN_00472650(0, max_range)` — returns uniform random in [0, max]

## SFX slot allocator (closest to voice pool)

`FUN_00465ca0` (U-1089):
- Takes entity_index + position_vec
- Scans entity's channel range `[base, base+count)` for first free slot
- Plays on first free; does nothing if all busy
- `param_1 == 0x1c` additionally increments kill-counter `DAT_008aa2d0`

## Deferred (cont1 bucket)

14 functions not yet classified (D-2985..D-2998). The 4 very large ones
(00460350, 00460df0, 00461650, 004661f0) are the highest priority for cont1
as they likely hold per-vehicle engine/transmission SFX logic.
