# FUN_0042bfb0 — 0x0042bfb0

## Shape

- **Signature**: `void FUN_0042bfb0(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6)`
- **Calling convention**: unknown (Ghidra)
- **Return type**: void
- **Parameters**: 6 — all undefined4
- **Body size**: 0x0042bfb0–0x0042c002

## Memory reads

- `DAT_0067eab0` — global int32; read at 0x0042bfb0; tested == 0 as guard. Raw address 0x0067eab0.

## Memory writes (conditional — only when `DAT_0067eab0 != 0`)

- `DAT_0067e918` — written `param_1`. Raw address 0x0067e918.
- `DAT_0067e91c` — written `param_2`. Raw address 0x0067e91c.
- `DAT_0067e920` — written `param_3`. Raw address 0x0067e920.
- `DAT_0067e924` — written `param_4`. Raw address 0x0067e924.
- `DAT_0067e928` — written `param_5`. Raw address 0x0067e928.
- `DAT_0067e92c` — written `param_6`. Raw address 0x0067e92c.
- `DAT_0067e930` — written 1 (flag). Raw address 0x0067e930.

## Branches

1. `DAT_0067eab0 == 0`: call FUN_0042bf30() and return (alt-init path).
2. Otherwise: store all 6 params and set flag.

## Callees (1)

| Address | Name | Note |
|---|---|---|
| 0x0042bf30 | FUN_0042bf30 | called only when DAT_0067eab0 == 0 |

## Callers (1)

| Address | Name |
|---|---|
| 0x00432450 | FUN_00432450 |

## Constants

- 0 — guard value for DAT_0067eab0.
- 1 — flag written to DAT_0067e930.

## Notes

- Stores a 6-tuple of state parameters into a contiguous globals block at 0x0067e918–0x0067e930 (stride 4 bytes, 7 words including flag).
- When DAT_0067eab0 == 0, falls back to FUN_0042bf30() without writing the params.
- The 6 globals at 0x0067e918..0x0067e92c plus flag at 0x0067e930 likely form a single struct.
