# FUN_0042bde0 — 0x0042bde0

## Shape

- **Signature**: `void FUN_0042bde0(int param_1, byte param_2)`
- **Calling convention**: unknown (Ghidra)
- **Return type**: void
- **Parameters**: 2
  - `param_1` (int) — base row index; used as float offset via `(float)(param_1 + -0xd)`
  - `param_2` (byte) — colour/alpha value; used to build packed colour argument
- **Body size**: 0x0042bde0–0x0042bf2b

## Memory reads

- `_DAT_005cc35c` — global float; read at 0x005cc35c; addend in `fVar2 = _DAT_005cc35c + 534.0`. Raw address 0x005cc35c.
- `_DAT_005cc574` — global float; read at 0x005cc574; used in `fVar3 = 26.0 - _DAT_005cc574`. Raw address 0x005cc574.
- `_DAT_005cd784` — global float; read at 0x005cd784; used in fifth call `_DAT_005cd784 + 534.0`. Raw address 0x005cd784.

## Memory writes

- None (draw-only function).

## Branches

- None (linear).

## Callees (1 unique)

| Address | Name | Called |
|---|---|---|
| 0x00472c60 | FUN_00472c60 | 5× with varying float-rect and colour args |

## Constants (hex → float)

- 0x42400000 = 48.0 (float)
- 0x44058000 = 534.0 (float)
- 0x41d00000 = 26.0 (float)
- 0x42380000 = 46.0 (float)
- 0x40000000 = 2.0 (float)
- 534.0 — appears as immediate in two calls
- 26.0 — appears as immediate and in `fVar3`
- `CONCAT13(param_2 >> 1, 0x146ef0)` — packed colour: low 3 bytes 0x146ef0, high byte = `param_2 >> 1`
- `CONCAT13(param_2, 0x1050b4)` — packed colour: low 3 bytes 0x1050b4, high byte = `param_2`

## Arguments to FUN_00472c60 (5 calls)

1. `(0x42400000, fVar1, 0x44058000, 0x41d00000, CONCAT13(param_2>>1, 0x146ef0))`
2. `(0x42380000, fVar1, 0x40000000, 0x41d00000, CONCAT13(param_2, 0x1050b4))`
3. `(0x42380000, fVar1, fVar2, 0x40000000, CONCAT13(param_2, 0x1050b4))`
4. `(0x42380000, fVar1+fVar3, fVar2, 0x40000000, CONCAT13(param_2, 0x1050b4))` where `fVar3 = 26.0 - _DAT_005cc574`
5. `(_DAT_005cd784 + 534.0, fVar1-fVar3, 0x40000000, 0x41d00000, CONCAT13(param_2, 0x1050b4))`

## Callers (1)

| Address | Name |
|---|---|
| 0x004335f0 | FUN_004335f0 |

## Notes

- Emits 5 HUD rectangles via FUN_00472c60 using float-coordinate arguments derived from `param_1` and globals.
- `fVar1 = (float)(param_1 - 13)` is the base Y offset for calls 1–3.
- param_2 controls opacity/colour blending in the colour packs.
