---
rva: 0x004fddbc
name_in_ghidra: FUN_004fddbc
size_bytes: 125
confidence: C1
subsystem: shader-compiler
callees_depth1: [FUN_004fdda7, FUN_004fcbca]
callers_noted: [FUN_004fdf42, FUN_004fe9cf]
opened_in_slot: Mashed_pool8
session_date: 2026-05-18
---

## Mechanical description

Macro-table **insert** (replace existing if name matches; otherwise head-insert).

Signature: `FUN_004fddbc(undefined4* macro_node)` — but `extraout_ECX` carries `this` (compiler context).

1. `iVar3 = FUN_004fdda7(*macro_node)` → returns 0; side-effect ECX preserved.
2. `piVar1 = (int*)(this + 0x4c + 0)` — bucket head (the `*4` term degenerates since iVar3=0).
3. Walk the chain at `*piVar1`:
   - Per-node-pointer is `*(int*)(node + 0xc)`.
   - For each non-NULL node:
     - byte-compare `node[0]` (first char of macro name) against `macro_node[0]` (first char of new name). The full compare uses pairwise dword loads ending at NUL.
     - If `iVar3 < 0` (new < existing): break, head-insert before this node — `LAB_004fde28`.
     - If `iVar3 == 0` (equal names): **replace.** Splice: `*piVar1 = *(node + 0xc)` (unlink), clear `*(node + 0xc) = 0`, `FUN_004fcbca(1)` (destroy old node), then `LAB_004fde28` to install new.
     - Else (new > existing): `piVar1 = node + 3` (move down to that node's chain ptr; +0xC offset), continue.
4. **`LAB_004fde28` head/middle-insert:** `macro_node[3] = *piVar1; *piVar1 = macro_node; return 0;`.

Bucket layout: head at `*(this+0x4c)`. Per-node `[0]` is name-ptr, `[3]` (offset +0xC) is "next-in-bucket".

## Constants

| Constant | Site | Note |
|---|---|---|
| +0x4c | this offset | macro-table bucket-head |
| +0 (in node) | name pointer |  |
| +0xc (in node) | next-in-bucket | also used for delete-chain |

## Uncertainties

- [UNCERTAIN] Whether the `*4` shift in `this + 0x4c + iVar3*4` is intentional (hash-bucket indexing through FUN_004fdda7) or dead arithmetic with the current FUN_004fdda7 always returning 0.

## Stubs encountered

- [STUB] `FUN_004fdda7` — degenerate strlen/hash, in-bucket.
- [STUB] `FUN_004fcbca` — large-class deleter (medium frame).
