# piz_fsmanager_handler — Analysis Report

**Session**: piz_fsmanager_handler-20260507  
**Slot**: Mashed_pool13 (pre-assigned)  
**Subsystem**: util  
**Date**: 2026-05-07  

---

## Inputs

| Input | Address | Description |
|---|---|---|
| `PTR_DAT_005cfee0` | 0x005cfee0 | Third argument to FUN_00551190; bytes `75 6e 63 00` = ASCII string `"unc\0"` |
| `FUN_00551190` | 0x00551190 | RW RtFSManager install routine (body: 0x00551190–0x0055128a) |
| `HardwareInstallFileSystem` | 0x004955d0 | Caller that passes `&PTR_DAT_005cfee0` as third arg at 0x00495639 |

---

## Correction: PTR_DAT_005cfee0 Is a String, Not a Vtable

Ghidra labeled address `0x005cfee0` as `PTR_DAT_005cfee0` with `data_type: /pointer` and `value: 00636e75`.
However, the raw bytes at `0x005cfee0` are `75 6e 63 00` — ASCII `"unc"` + NUL.

**Cited evidence**: `listing_data_at 0x005cfee0` → `value: 00636e75`;
`memory_read 0x005cfee0 length:4` → `data_hex: 75 6e 63 00`.

Ghidra interpreted those four bytes as a little-endian pointer to `0x00636e75`; `memory_read 0x00636e75`
returns all zeros (uninitialized data segment). The correct interpretation is the four bytes ARE
the string `"unc\0"` stored directly at `0x005cfee0`.

**How the third arg is used** (cited from `decomp_function 0x00551190`):
```c
iVar1 = FUN_005504d0(param_3);  // at 0x00551197
if (iVar1 != 0) { return 0; }  // duplicate check — bail if already installed
```
`FUN_005504d0` (0x005504d0) walks a linked list at `DAT_007dc754`, comparing each node's name
field (`puVar1 + 0x14`) against `param_1` via an RW vtable comparator
(`(**(code **)(DAT_007d3ff8 + 0xe8))(puVar1+0x14, param_1)`). It returns the matching node or
NULL. The third arg is therefore a **FS handler registration name string**, not a vtable pointer.

---

## FUN_00551190 — FS Handler Constructor

**Cited**: `decomp_function 0x00551190`.

`FUN_00551190(param_1=0x14, param_2=path_string, param_3=name_string)`:

1. Calls `FUN_005504d0(param_3)` to detect duplicate name. If non-zero → return 0.
2. Calls `(**(code **)(DAT_007d3ff8 + 0x108))(0x5c, 0x401be)` — RW allocator for 0x5c-byte struct.
3. Populates 12 function pointers into the allocated struct at offsets +0x14..+0x4c
   (cited from listing at 0x005511cc–0x0055121f).
4. Calls `(**(code **)(DAT_007d3ff8 + 0x114))(param_1, 0x60, 0x401be)` — allocates file-slot
   array: `param_1` (=0x14=20) slots × 0x60 bytes each. Stores result at struct+0x58.
5. Calls `FUN_00551330(handler_struct, param_1, param_3, param_2)` — completes registration.
6. On failure: frees both allocations via RW free (`DAT_007d3ff8 + 0x10c`), returns 0.

The **actual handler vtable** is populated inline by this function, not stored at `PTR_DAT_005cfee0`.

---

## Vtable Entry Enumeration

All 12 entries confirmed via `listing_code_units_list` and `decomp_function`. Each is a cdecl
routine taking a file-slot struct pointer as first argument.

| struct+offset | RVA | Ghidra label | Win32 import | Role | calls_.piz |
|---|---|---|---|---|---|
| +0x14 | 0x00150c30 | LAB_00550c30 | none | **get_slot**: `index*0x60 + slot_array_base`; returns file-slot ptr | no |
| +0x18 | 0x00151290 | LAB_00551290 | RW vtable +0x10c (free) | **destroy**: replaces arg[0] with `handler[+0x58]` (slot array), tail-calls RW free | no |
| +0x28 | 0x00150c50 | LAB_00550c50 | FUN_00551410, FUN_00551460 | **open**: normalises path (split on `\`, 0x100-byte stack buf), opens file via helpers | no |
| +0x2c | 0x00150d80 | LAB_00550d80 | `CloseHandle` [0x005cc088] | **close**: sets `slot[+0x20]=1`, calls `CloseHandle(slot[+0x40])` | no |
| +0x30 | 0x00150da0 | LAB_00550da0 | `CreateEventA` [0x005cc08c] + `ReadFile` [0x005cc1a4] + `GetLastError` [0x005cc0cc] | **read_async**: `CreateEventA(0,0,0,0)`, then `ReadFile` with `OVERLAPPED` at `slot[+0x44]` | no |
| +0x34 | 0x00150f20 | LAB_00550f20 | `CreateEventA` [0x005cc08c] + `WriteFile` [0x005cc1a0] + `GetLastError` [0x005cc0cc] | **write_async**: `CreateEventA`, then `WriteFile` with `OVERLAPPED` at `slot[+0x44]` | no |
| +0x38 | 0x00151050 | LAB_00551050 | `SetFilePointer` [0x005cc17c] | **seek**: maps mode (1→0, 2→1, other→2) then `SetFilePointer(slot[+0x40], offset, 0, mode)` | no |
| +0x3c | 0x00151090 | FUN_00551090 | `GetOverlappedResult` | **poll**: `GetOverlappedResult(slot[+0x10], &slot[+0x11], &nread, wait)`; handles `ERROR_IO_INCOMPLETE` (0x3e4) and `ERROR_IO_PENDING` (0x3e5) | no |
| +0x40 | 0x00151180 | LAB_00551180 | none | **cancel**: `slot[+0x20]=1; RET` (marks closed, no OS call) | no |
| +0x44 | 0x001514e0 | LAB_005514e0 | none | **eof**: compare `slot[+0x8]` (pos_low) vs `slot[+0x0]`/`slot[+0x4]` boundaries; return 1 if at/past end | no |
| +0x48 | 0x001512b0 | FUN_005512b0 | (via FUN_00551090) | **get_status**: if `slot[+0x20]!=1` call poll(slot,0); return `slot[+0x20]` | no |
| +0x4c | 0x001512d0 | LAB_005512d0 | `GetFileAttributesA` [0x005cc1b4] | **stat**: processes path (0x100-byte stack buf, FUN_00551410/FUN_00551460), `GetFileAttributesA`; return 0 if `==INVALID_FILE_ATTRIBUTES` | no |

**File-slot struct layout** (inferred from seek, close, read_async, eof):

| slot offset | content |
|---|---|
| +0x00 | upper boundary (used by eof) |
| +0x04 | total count / size (used by eof) |
| +0x08 | current position (low) / read byte count |
| +0x0c | current position (high) |
| +0x10 | OS HANDLE (duplicate of +0x40?) |
| +0x11..+0x1c | OVERLAPPED struct (17 bytes or ~) |
| +0x18 | mode/flags field (set by open) |
| +0x20 | status flag: 0=open, 1=closed/cancelled |
| +0x24 | callback fn pointer (FUN_00551510) |
| +0x28 | error code (set by open error path) |
| +0x38 | pointer to handler struct (back-ref) |
| +0x40 | OS HANDLE (CloseHandle target) |
| +0x44 | OVERLAPPED struct (for async I/O) |
| +0x50 | event/context area |
| +0x58 | ... |

Stride between slots = 0x60 bytes (cited: `get_slot`: `EAX = index*3*32 = index*0x60` at 0x00550c40–0x00550c48).

---

## .piz Integration Check

Searched for `.piz`-related strings; found 14 matches. Key string `"Setting up PIZ file system\n"`
at `0x005cc478` is referenced from `0x0040278b` inside `FUN_00402750` (0x00402750).

### .piz Loading Chain

```
FUN_00402750 (0x00402750)  — game init, "Setting up PIZ file system\n"
  └─ FUN_00495280 (0x00495280)  — OpenPizFile(path): path-transform + log + open
       └─ FUN_004b6570 (0x004b6570)  — thin bool wrapper
            └─ FUN_004b6940 (0x004b6940)  — piz open+parse (reads directory table)
                 ├─ FUN_004b6710 (0x004b6710)  — ← BYPASS SITE: opens file via CreateFileA
                 └─ FUN_004b67e0 (0x004b67e0)  — ← BYPASS SITE: reads data via ReadFile
FUN_004952f0 (0x004952f0)  — ClosePizFile: gets name, logs, calls thunk_FUN_004b67a0
FUN_00426e10 (0x00426e10)  — track loader: builds path, calls FUN_00495280(track.piz)
```

### FUN_004b6710 — Piz File Open (cited: decomp 0x004b6710)

```c
if ((DAT_007d3e50 & 1) != 0) {
    DAT_007d3e48 = FUN_004a4541();  // alternate open (bit 0 of flag set)
    return DAT_007d3e48;
}
DAT_007d3e48 = CreateFileA(in_EAX, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                            FILE_FLAG_NO_BUFFERING|FILE_FLAG_OVERLAPPED, NULL);
```
**Direct Win32 `CreateFileA` call. Does NOT go through RW VFS.**  
Handle stored at global `DAT_007d3e48`.  
Flag `DAT_007d3e50 & 1`: alternate path via `FUN_004a4541` (not yet reversed; `[UNCERTAIN]`).

### FUN_004b67e0 — Piz Data Reader (cited: decomp 0x004b67e0)

```c
if ((DAT_007d3e50 & 1) != 0) {
    fseek(DAT_007d3e48, param_1, 0);
    *param_4 = fread(param_2, 1, param_3, DAT_007d3e48);
    return;
}
// async path:
local_14.u.s.Offset = param_1;
ReadFile(DAT_007d3e48, param_2, param_3, NULL, &local_14);
GetLastError();
do {
    if (DAT_007d3e5c) (*DAT_007d3e5c)(...); // progress callback
    BVar3 = GetOverlappedResult(DAT_007d3e48, &local_14, param_4, 0);
    // retry on ERROR_IO_INCOMPLETE (0x3e4)
} while (...);
```
**Direct Win32 `ReadFile` + `GetOverlappedResult` call. Does NOT go through RW VFS.**

### FUN_004b6940 — Piz Open+Parse (cited: decomp 0x004b6940)

Key globals modified:
- `DAT_008ab7e0` — current piz name (0x80-byte string, zeroed then strcpy'd)
- `DAT_008ad8a0` — "previously opened piz" name (for change detection)
- `DAT_007d3e48` — OS HANDLE to open piz file
- `DAT_007d3e4c` — 1 when piz is open
- `DAT_0090dac0` — piz directory table (0x1000 dwords = 16KB, zeroed; entries read via FUN_004b67e0)
- `DAT_008ad9a0/a4/a8/ac` — piz header fields (4 DWORDs at header read at offset 0)
- `DAT_007d3e58` — saved RW FS handle (`FUN_005507a0()` = returns `DAT_007dc76c`)
- `DAT_007d3e64/68` — entry count fields (zeroed when switching piz)

After filling directory table, calls `FUN_00550790(DAT_007d3e54)` — this is the RW FS detach/close
noted in the prior `HardwareInstallFileSystem` analysis. The call here closes a previously held
RW FS stream object (`DAT_007d3e54`), NOT the piz file itself.

---

## Shim Verdict

### Path C — no .piz integration via FSManager

The RW RtFSManager Win32 handler (`FUN_00551190` and its 12 vtable functions) is used for
**general-purpose disk file access** through the RenderWare VFS (e.g., texture loading, audio
streaming, Lua scripts via `RwStream*` APIs). `.piz` archives are opened and read through a
**completely separate code path** that calls Win32 directly:

- `FUN_004b6710` (0x004b6710): `CreateFileA` with `FILE_FLAG_OVERLAPPED` — the piz file open
- `FUN_004b67e0` (0x004b67e0): `ReadFile` + `GetOverlappedResult` — the piz data reader

**Functions to reimplement for a .piz shim** (in order from outermost to innermost):

| Priority | RVA | Name | What to replace |
|---|---|---|---|
| 1 (outermost) | 0x00095280 | `OpenPizFile` (FUN_00495280) | Replace entire piz open flow |
| 1 (outermost) | 0x000952f0 | `ClosePizFile` (FUN_004952f0) | Replace entire piz close flow |
| 2 (mid) | 0x000b6940 | `PizOpenAndParse` (FUN_004b6940) | Replaces parse + directory build |
| 3 (bypass) | 0x000b6710 | `PizWin32Open` (FUN_004b6710) | Direct CreateFileA replacement |
| 3 (bypass) | 0x000b67e0 | `PizWin32Read` (FUN_004b67e0) | Direct ReadFile replacement |

**Separate .piz call sites** (bypass the FSManager entirely):
- `FUN_00402750` at 0x00402750: opens font36.piz, powerups.piz, Panel.piz, perm.piz, led.piz via `FUN_00495280`
- `FUN_00426e10` at 0x00426e10: opens track .piz files via `FUN_00495280`
- `FUN_004b6710` at 0x004b6710: the actual `CreateFileA` call site

---

## Uncertainties

| ID | Address | What is unknown |
|---|---|---|
| [UNCERTAIN-1] | 0x004b6710 | `FUN_004a4541`: alternate piz open path taken when `DAT_007d3e50 & 1`; flag meaning and trigger unknown |
| [UNCERTAIN-2] | 0x005cfee0 | Ghidra labels bytes as pointer (PTR_DAT_); actual content is string `"unc\0"`. Ghidra misclassification; no further ambiguity. |
| [UNCERTAIN-3] | 0x004b6940 | `DAT_007d3e5c` progress callback: set somewhere, called during async read; not traced |

---

## hooks.csv Rows Added

All 12 vtable functions + 6 .piz-layer functions = 18 rows, subsystem=util, status=mapped, confidence=C1.

See `hooks.csv` for the full rows. Bucket: `re/analysis/piz_fsmanager_handler/REPORT.md`.
