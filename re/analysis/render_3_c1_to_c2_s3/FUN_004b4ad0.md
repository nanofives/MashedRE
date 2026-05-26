# FUN_004b4ad0 — 4-element copy wrapper for FUN_004b4a80 (0x004b4ad0)

## Identity
- **RVA**: 0x004b4ad0
- **Body**: 0x004b4ad0 – 0x004b4b16
- **Session**: batch-render-3-s3

## Signature (Ghidra)
```c
void FUN_004b4ad0(undefined4 param_1, undefined4 *param_2, undefined4 param_3, undefined4 param_4)
```

## Decompilation
```c
void FUN_004b4ad0(undefined4 param_1, undefined4 *param_2, undefined4 param_3, undefined4 param_4)
{
  undefined4 local_1c, local_18, local_14, local_10;
  undefined4 local_4;

  local_1c = *param_2;           // copy 4 dwords from param_2 into local
  local_18 = param_2[1];
  local_14 = param_2[2];
  local_10 = param_2[3];
  local_4 = 3;                   // 0x004b4ad0+0x1b — count=3
  FUN_004b4a80(param_1, &local_1c, param_3, param_4);  // 0x004b4ad0+0x21
  return;
}
```

## Mechanics
- Copies 4 dwords from `*param_2` into local stack buffer.
- Sets `local_4 = 3` (likely element count for callback).
- Calls `FUN_004b4a80(param_1, &local_1c, param_3, param_4)`.
- Passes stack copy as `param_2` to insulate caller's buffer.

## Callees
| Address    | Name          |
|------------|---------------|
| 0x004b4a80 | FUN_004b4a80  |

## Callers
| Address    | Name          |
|------------|---------------|
| 0x00422570 | FUN_00422570  |

## C2 promotion evidence
Full decompilation recovered. 4-dword stack copy + count=3 marker + forward to 004b4a80.
