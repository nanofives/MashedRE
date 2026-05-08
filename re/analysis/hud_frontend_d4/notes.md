# hud_frontend_d4 analysis notes
Session: hud_frontend_d4-20260508
Pool slot: Mashed_pool14 (read-only)
Parent bucket: hud_frontend_d3-cont1 (D-6160..D-6184)
Cap: 18 analyzed, 7 deferred → hud_frontend_d4-cont1 (D-9400..D-9406)
Subsystem: frontend

---

## Functions analyzed (18)

### 0x0042a940 FUN_0042a940 — powerup type → sprite-index table lookup
U-3167 | C1

```c
undefined4 FUN_0042a940(undefined4 param_1)
{
  // DAT_005f6748: head of stride-3 int table; each entry = [type_id, ?, sprite_idx]
  // FUN_0040ce80(param_1) → type key via double-indirect ptr table at PTR_PTR_005f2770
  iVar2 = FUN_0040ce80(param_1);
  iVar3 = 0;
  iVar1 = DAT_005f6748;           // 0x005f6748: first entry type_id
  while (iVar1 != -1) {
    if (iVar1 == iVar2) break;
    iVar1 = (&DAT_005f6754)[iVar3]; // 0x005f6754+iVar3*4: next type_id
    iVar3 += 3;
  }
  return *(undefined4 *)(iVar3 * 4 + 0x5f6750); // 0x5f6750+iVar3*4: sprite index
}
```

- Linear search through a sentinel-terminated (−1) table at 0x005f6748.
- Stride: 3 ints per entry. First field = type key at offset 0, sprite index at offset 2 (0x5f6750 = 0x5f6748+8 relative to stride start).
- Converts selection `param_1` to type key via `FUN_0040ce80` (S-3160, new stub).
- Returns 0 if type not found.

---

### 0x0042ac00 FUN_0042ac00 — slot count scanner (sentinel walk)
U-3168 | C1

```c
int __fastcall FUN_0042ac00(undefined4 param_1, int *param_2)
// param_1 unused in output; param_2: ptr to sentinel-terminated int array
```

- Sentinel values: −0xfa0000 (0xFF060000) marks an entry start; −0xf90000 (0xFF070000) marks end.
- Counts the number of "−0xfa0000" separators encountered before a "−0xf90000" terminator.
- Returns iVar3 = count of entries (= number of player/slot records in the array).
- Note: param_1 is passed in EAX (__fastcall) but not used — possibly a `this` pointer discarded by the decompiler.

---

### 0x0042ac50 FUN_0042ac50 — Y-center layout base (N items)
U-3169 | C1

```c
int __fastcall FUN_0042ac50(int param_1)
// in_EAX = item count N; param_1 = item height/spacing
```

- 0xf0 = 240 (screen half-height for 480-line display).
- Even N: `return 0xf0 - param_1/2 - ((N-1) >> 1) * param_1`
- Odd  N: `return 0xf0 - ((N-1) >> 1) * param_1`
- Computes the Y origin so N items of height `param_1` are centered on y=240.
- Called to set up vertical layout for N-player HUD slots.

---

### 0x0042bcb0 FUN_0042bcb0 — input icon draw (player, position, button-name, device)
U-3170 | C1

```c
void FUN_0042bcb0(int param_1, float param_2, float param_3,
                  undefined4 param_4, int param_5)
// param_1: player index
// param_2, param_3: normalized X, Y position
// param_4: ARGB color
// param_5: append-suffix flag
```

Key operations (0x0042bcb0..0x0042bdde):
- Screen X = `param_2 * _DAT_005cd5a8 + _DAT_005cd18c` (0x0042bcdb / 0x0042bce4)
- Screen Y = `_DAT_005cc724 + param_3 * _DAT_005cc560` (0x0042bcf2 / 0x0042bcfa)
- Reads `(&DAT_007e96fc)[param_1 * 0x80]` (0x0042bd0e): input device type.
  - ==2: keyboard; string from `s_keyboard_005cd778` (0x0042bd13..0x0042bd34)
  - ==1: gamepad; string from `DAT_005cd774` (0x0042bd36..0x0042bd48)
- If `param_5 != 0`: appends suffix `DAT_005cd76c` (0x0042bd72..0x0042bd7e)
- `FUN_004c5c00(DAT_00636ac8, local_14)` (0x0042bd84): looks up sprite handle by button name string
- `FUN_004b5750(*DAT_007d3ff8, &local_1c, &local_24, &param_4, uVar4)` (0x0042bdb3): draws textured quad (S-3161)
- Uses __security_check_cookie; stack frame size significant.

[UNCERTAIN U-3170a]: Device type codes: 2=keyboard inferred from string literal at 0x005cd778; 1=gamepad inferred by exclusion. No disassembly cross-check yet.

---

### 0x0042d290 FUN_0042d290 — lap-time two-component formatter (minutes + seconds)
U-3171 | C1

```c
void FUN_0042d290(int param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4)
// param_1: minutes component
// param_3: output buffer / draw target for minutes
// param_4: output buffer / draw target for seconds
```

- `FUN_004a2b60` (already C1: sprintf variant) formats to param_3/param_4.
- param_1 < 10: uses zero-pad format `DAT_005cd798` with constant `DAT_005cd7a0` (leading "0").
- param_1 >= 10: uses direct format `DAT_005cd794` with param_1.
- `FUN_004a2c48()` (0x0042d2c8, already C1: FPU round) retrieves seconds component implicitly.
- Same <10 guard applied to seconds component.
- Two-segment time display: minutes (param_1) and seconds (FUN_004a2c48 result).

---

### 0x0042d300 FUN_0042d300 — lap-time delta decomposer
U-3172 | C1

```c
void FUN_0042d300(int param_1, int param_2,
                  undefined4 *param_3,   // out: comparison result (0=tie,1=A<B,2=A==B)
                  int *param_4,          // out: minutes component
                  int *param_5,          // out: seconds component
                  float *param_6)        // out: centiseconds (remainder)
```

- Time unit: centiseconds (1/100 s). 6000 = 1 minute, 100 = 1 second.
- `param_1 - param_2`: delta (signed).
  - ==0 → *param_3=2 (equal)
  - <0  → *param_3=1 (param_1 faster), delta = |delta|
  - else → *param_3=0 (param_2 faster)
- `*param_4 = delta / 6000` (minutes)
- `*param_5 = (delta % 6000) / 100` (seconds)
- `*param_6 = (float)(delta - (*param_4 * 60 + *param_5) * 100)` (centiseconds remainder)
- Used for lap-time comparison display (delta breakdown).

---

### 0x0042ebe0 FUN_0042ebe0 — slot occupancy check (type 10/11/12)
U-3173 | C1

```c
bool FUN_0042ebe0(int param_1)
// param_1: slot type (10=0xa, 11=0xb, 12=0xc)
```

- Base array: `DAT_007f0a48`, stride 0x78 (= 120 dwords = 480 bytes), iterates until 0x7f0c28.
- For param_1=10: checks offsets [−1], [+0xb..+0x6b] within each record.
- For param_1=11: checks offsets [0], [+0xc..+0x6c].
- For param_1=12: checks offsets [+3], [+0xf..+0x6f].
- `bVar1` is set to true if any checked field is zero (slot empty).
- Final: returns `(&DAT_007f0a44)[param_1 * 0xc] != 0 && !bVar1`
  = header-active AND all slots occupied.
- [UNCERTAIN U-3173a]: Slot type codes 10/11/12 semantics. Values suggest 10=AI?, 11=remote?, 12=CPU?. No corroborating evidence.

---

### 0x0042ee00 FUN_0042ee00 — vehicle icon getter (slot 0–2)
U-3174 | C1

```c
undefined4 FUN_0042ee00(int param_1)
// param_1: slot index 0, 1, or 2
```

- Cases 0, 1, 2: all call `FUN_0040bb90()` (C1: wrapper FUN_004c5c00(DAT_0063b904, param_1)).
- Returns 0 for param_1 > 2.
- [UNCERTAIN U-3174a]: Decompiler collapsed all three cases to identical calls; actual param passing to FUN_0040bb90 may differ. The three cases likely pass different constants as implicit args. No param visible in decompiled output.

---

### 0x0042ee40 FUN_0042ee40 — vehicle sprite getter (game mode + slot type)
U-3175 | C1

```c
undefined4 FUN_0042ee40(int param_1)
// param_1: slot type index; may carry 1000-offset for "virtual" slot numbering
// DAT_0067e9fc: game mode
```

- game mode 2: direct call FUN_0040bb90().
- game mode 3/4/5:
  - If `DAT_0067f17c > 9` (fleet size > 9): call for param_1 ∈ {0,1,2}.
  - Otherwise: strip 1000-offset (`param_1 -= 1000` if param_1 > 999), then call for 0/1/2.
- game mode 6/7/8/9: call only for param_1 == 0.
- game mode 10: unconditional call.
- Returns 0 if no match.
- DAT_0067f17c at 0x0042ee59: vehicle count global.
- Slot-type virtual numbering (1000-offset) appears unique to modes 3–5.

---

### 0x0042ef40 FUN_0042ef40 — vehicle unlock flag reader
U-3176 | C1

```c
undefined4 FUN_0042ef40(int param_1, int param_2)
// param_1: vehicle index
// param_2: slot type (raw: 2,3,4,5,10 or 1000-offset: 0x3e9,0x3ea,0x3ed,0x3f3)
```

Array base: `DAT_007f0e50`, stride 0xc per vehicle (12 bytes).
Byte offsets by slot type:
- param_2=2    (or default): +0 from DAT_007f0e50 (0x0042ef6e)
- param_2=3    / 0x3e9:     +1 from DAT_007f0e51 (0x0042ef78 / 0x0042ef85)
- param_2=4    / 0x3ea:     +2 from DAT_007f0e52 (0x0042ef8e / 0x0042ef98)
- param_2=5    / 0x3ed:     +5 from DAT_007f0e55 (0x0042efa5 / 0x0042efb3)
- param_2=10   / 0x3f3:     +11 from DAT_007f0e5b (0x0042efbd / 0x0042efc5)
Returns 1 if flag byte == 0x01, else 0.
- The 1000-offset variants (0x3e9 = 1001, etc.) map to same offsets as raw values 3,4,5,10.
- [UNCERTAIN U-3176a]: Vehicle count (max param_1); no bounds check seen.

---

### 0x0042f8d0 FUN_0042f8d0 — bordered background rect (5-quad)
U-3177 | C1

```c
void FUN_0042f8d0(float param_1, float param_2, float param_3, float param_4)
// (x1, y1, x2, y2) in screen space
```

Calls `FUN_00472c60` (C1: RwIm2D filled-quad draw) 5 times:
1. Full rect: (param_1, param_2, param_3, param_4), color `CONCAT13(in_AL>>1, 0x146ef0)` (0x0042f8e5)
2. Left edge:  (param_1 − _DAT_005cc574, param_2, 2.0, param_4), dark color (0x0042f8fe)
3. Top edge:   (param_1 − _DAT_005cc574, param_2, param_3 + _DAT_005cc35c, 2.0) (0x0042f910)
4. Bottom edge: offset by _DAT_005cc574 (0x0042f928)
5. Right edge: (param_3 + param_1 + _DAT_005cc574, ...) (0x0042f950)
- _DAT_005cc574: border margin constant (global); _DAT_005cc35c: second border margin.
- Color `0x146ef0` = dark fill; `0x1050b4` = border color (raw ARGB bytes).

---

### 0x0042fab0 FUN_0042fab0 — vehicle sprite lookup by slot 0–9
U-3178 | C1

```c
void FUN_0042fab0(undefined4 param_1)
// param_1: slot index 0..9
```

- Switch on param_1 (cases 0–9): each calls `FUN_0040bb90()` (C1: FUN_004c5c00(DAT_0063b904, …)).
- Default: return (no call).
- [UNCERTAIN U-3178a]: Same decompiler collapse as FUN_0042ee00 — actual per-slot argument to FUN_0040bb90 not visible. Likely passes slot index as implicit param.
- Depth-3 in call chain: FUN_004335f0 / FUN_0043a610 / FUN_00434720 / FUN_0043aa30.

---

### 0x00430760 FUN_00430760 — game mode ∈ {2,3,4,5,10} predicate
U-3179 | C1

```c
undefined4 FUN_00430760(void)
// DAT_0067e9fc: game mode
```

- Returns 1 if mode ∈ {2, 3, 4, 5, 10}, else 0. (0x00430762..0x0043077e)
- Mode set: multiplayer/network (2,3,4,5) + mode 10 (unknown but related).
- Used as guard in call chains involving vehicle selection screens.
- [UNCERTAIN U-3179a]: Mode 10 semantics unknown; grouped with multiplayer modes.

---

### 0x00430830 FUN_00430830 — slot data reader (mode-keyed array)
U-3180 | C1

```c
undefined4 FUN_00430830(int param_1)
// param_1: slot index; DAT_0067e9fc: game mode
```

Base addresses by game mode (stride 0xc=48 bytes per slot):
- Mode 2,3,10: `(&DAT_007f0a44)[param_1 * 0xc]`  (0x00430843/0x0043084e/0x004308b4)
- Mode 4:      `(&DAT_007f0a48)[param_1 * 0xc]`  (0x00430858)
- Mode 5:      `(&DAT_007f0a54)[param_1 * 0xc]`  (0x00430863)
- Mode 6:      `(&DAT_007f0a4c)[param_1 * 0xc]`  (0x0043086e)
- Mode 7:      `*(uint32*)(&DAT_007f0a60 + param_1 * 0x30)` (0x0043087d)
- Mode 8:      `*(uint32*)(&DAT_007f0a64 + param_1 * 0x30)` (0x0043088e)
- Mode 9:      `(&DAT_007f0a68)[param_1 * 0xc]`  (0x0043089f)
- Default: 0
- DEFERRED label "split-screen track check" may be imprecise; function reads from the slot-state array, not track data per se.
- [UNCERTAIN U-3180a]: Semantics of array values at DAT_007f0a44 family; not yet traced.

---

### 0x00430a10 FUN_00430a10 — slot-type-0 for current game mode
U-3181 | C1

```c
int FUN_00430a10(void)
// DAT_0067e9fc: game mode
```

Returns slot type code for "player slot 0" given current game mode:
- mode 2      → 0
- mode 3/4/5  → 1
- mode 6/7/8/9→ 3
- mode 10     → 11 (0xb)
Formula: switch on `DAT_0067e9fc - 2`. (0x00430a17..0x00430a36)

---

### 0x00430a60 FUN_00430a60 — slot-type-1 for current game mode
U-3182 | C1

```c
int FUN_00430a60(void)
// DAT_0067e9fc: game mode
```

Returns slot type code for "player slot 1":
- mode 2 / 6/7/8/9/10 → 0
- mode 3/4/5          → 2
Formula: switch on `DAT_0067e9fc - 2`. (0x00430a67..0x00430a7a)

---

### 0x00430ab0 FUN_00430ab0 — slot-type-2 for current game mode
U-3183 | C1

```c
int FUN_00430ab0(void)
// DAT_0067e9fc: game mode
```

Returns slot type code for "player slot 2":
- mode 2 / 6/7/8/9/10 → 0
- mode 3/4/5          → 5
Formula: switch on `DAT_0067e9fc - 2`. (0x00430ab7..0x00430aca)

Group note: FUN_00430a10, FUN_00430a60, FUN_00430ab0 form a triplet — each returns the
slot-type code for a different player/viewport slot given the current game mode.

---

### 0x00430b30 FUN_00430b30 — lap-time record reader (3 out-params)
U-3184 | C1

```c
void __thiscall FUN_00430b30(int param_1,
                              undefined4 *param_2,   // out: field 0 (minutes?)
                              undefined4 *param_3,   // out: field 4 (seconds?)
                              undefined4 *param_4)   // out: field 8 (centisecs?)
// in_EAX: this (record-array base index)
// param_1: lap index
```

- Index = `(in_EAX + param_1 * 2) * 0xc` (0x00430b38)
- Array base: `DAT_008989e0`; record size 0xc (12 bytes).
- *param_2 ← `*(uint32*)(&DAT_008989e0 + index)` (0x00430b46)
- *param_3 ← `*(uint32*)(&DAT_008989e4 + index)` (0x00430b51)
- *param_4 ← `*(uint32*)(&DAT_008989e8 + index)` (0x00430b5a)
- Reads 3 consecutive 4-byte fields from a lap-time struct array.
- The `in_EAX` implicit `this` suggests method on a lap-record object; param_1 selects a lap within the object's range.

---

## New stubs filed

### S-3160: FUN_0040ce80 (0x0040ce80)
```c
undefined4 FUN_0040ce80(int param_1)
// returns *(*(PTR_PTR_005f2770 + param_1*4) + 4)
// double-indirect: ptr table at 0x005f2770; reads +4 from pointed object
// = "get type key" for powerup/object at index param_1
```
16 bytes; no callees. Callee of FUN_0042a940 (U-3167).

### S-3161: FUN_004b5750 (0x004b5750)
```c
void FUN_004b5750(int param_1, float *param_2, float *param_3,
                  undefined1 *param_4, undefined4 param_5)
// Textured quad renderer (2D sprite draw)
// param_1: sprite/texture object (+0x60 → tex data, +0x80 → scale)
// param_2: UV start (float[2], default {0,0})
// param_3: UV end   (float[2], default {1,1})
// param_4: RGBA[4]  (default white)
// param_5: texture handle / blend-mode token
```
Uses vtable at DAT_007d3ff8+0x20 (SetRenderState), +0x2c (DrawIndexed 2 tris).
Callee of FUN_0042bcb0 (U-3170), subsystem frontend. ~110 bytes.

---

## Deferred → hud_frontend_d4-cont1 (D-9400..D-9406)

| D-ID  | RVA        | Original D-ID | Description |
|-------|------------|---------------|-------------|
| D-9400 | 0x004368e0 | D-6178 | player alpha/color setup |
| D-9401 | 0x00436810 | D-6179 | local player slot occupancy check |
| D-9402 | 0x004391b0 | D-6180 | powerup/overlay sprite draw |
| D-9403 | 0x00458630 | D-6181 | powerup sprite lookup by type |
| D-9404 | 0x00473870 | D-6182 | sprite draw 7-param |
| D-9405 | 0x004736c0 | D-6183 | line/border renderer |
| D-9406 | 0x00474e60 | D-6184 | float-to-x87-angle converter |

---

## Uncertainties

| U-ID    | Function     | Question |
|---------|--------------|----------|
| U-3170a | FUN_0042bcb0 | Input device type codes: 2=keyboard inferred from string literal at 0x005cd778; 1=gamepad by exclusion. Needs cross-check in input subsystem. |
| U-3173a | FUN_0042ebe0 | Slot type codes 10/11/12 semantics unknown — AI/remote/CPU guessed but unverified. |
| U-3174a | FUN_0042ee00 | Decompiler collapsed all 3 cases to identical FUN_0040bb90() calls; actual per-case argument invisible. |
| U-3176a | FUN_0042ef40 | No bounds check on vehicle index param_1; max vehicle count unknown. |
| U-3178a | FUN_0042fab0 | Same decompiler collapse as U-3174a for slot 0–9 cases. |
| U-3179a | FUN_00430760 | Mode 10 semantics unknown; grouped with multiplayer {2,3,4,5} only by this predicate. |
| U-3180a | FUN_00430830 | Semantics of values in DAT_007f0a44 family arrays not yet traced. |

---

## Global constants observed

| Address      | Value observed  | Context |
|--------------|-----------------|---------|
| DAT_005f6748 | int (type key)  | Powerup sprite-index table head (FUN_0042a940) |
| DAT_0067e9fc | int (game mode) | Game mode switch (FUN_0042ee40, FUN_00430760, FUN_00430830, FUN_00430a10..ab0) |
| DAT_0067f17c | int (vehicle count) | > 9 guard in FUN_0042ee40 |
| DAT_007f0a44 | int array       | Slot-state array; stride 0xc; mode 2/3/10 base |
| DAT_007f0e50 | byte array      | Vehicle unlock flag table; stride 0xc |
| DAT_008989e0 | struct array    | Lap-time records; stride 0xc; 3×uint32 per record |
| DAT_007e96fc | int array       | Input device type per player; stride 0x80 |
| DAT_00636ac8 | ptr             | Sprite atlas handle (button name lookup) |
