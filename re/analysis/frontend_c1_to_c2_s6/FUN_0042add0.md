# FUN_0042add0 — 0x0042add0

## Shape

- **Signature**: `int FUN_0042add0(void)`
- **Calling convention**: unknown (Ghidra)
- **Return type**: int
- **Parameters**: none declared
- **Body size**: 0x0042add0–0x0042ae0f

## Memory reads

- `DAT_0067e9f8` — global int32; used as row-selector into the table pointer array. Raw address 0x0067e9f8.
- `(&DAT_0067ed38)[DAT_0067e9f8 * 0x10]` — selects a table pointer from a pointer array at 0x0067ed38, stride 0x10 (16 bytes) per row. Result used as table base `piVar1`.
- `*piVar1` — initial key read.
- `piVar1[iVar3 + 1]` — successive entries; `iVar3` increments by 1 per iteration.

## Memory writes

- None.

## Branches

1. `iVar2 == -0xf90000` (sentinel 0xff070000; signed -16318464 decimal): return -1.
2. `iVar2 == unaff_EBX` (primary key match from register EBX [UNCERTAIN U-5861]): if `unaff_EDI == iVar4` (secondary key from EDI [UNCERTAIN U-5862]), return `piVar1[iVar3 + 1]`.
3. Otherwise advance: `iVar2 = piVar1[iVar3 + 1]; iVar3++`.

## Local variables

- `piVar1` — pointer to selected table entry (int*).
- `iVar2` — current key read.
- `iVar3` — running word index (step +1).
- `iVar4` — running secondary-key match counter (step +1 on primary match).
- `unaff_EBX`, `unaff_EDI` — implicit search keys [UNCERTAIN U-5861] [UNCERTAIN U-5862].

## Callees

- None (pure leaf).

## Callers (1)

| Address | Name |
|---|---|
| 0x00432b30 | FUN_00432b30 |

## Constants

- `0x10` (16) — stride in bytes for the mode pointer array at 0x0067ed38.
- `-0xf90000` = 0xff070000 — table sentinel (signed -16318464 decimal).
- Array base: 0x0067ed38.
- Mode index global: 0x0067e9f8.

## Notes

- Variant of FUN_0042ad90 (0x0042ad90): same sentinel-terminated table walk but with the table pointer sourced from `(&DAT_0067ed38)[DAT_0067e9f8 * 0x10]` (current-mode selection) rather than an explicit parameter.
- Implicit register keys (EBX, EDI) still apply; exact calling site must be inspected to resolve the calling convention [UNCERTAIN U-5861].
