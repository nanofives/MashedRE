# Boot / CRT Globals Layout — MSVC CRT Environment

**First authored:** struct_extract_phase5_pt2-20260513  
**Sources:** boot_crt_env_cont1 plates (0x004a4554, 0x004a45cf, 0x004a45c6, 0x004a4660, 0x004a7800, 0x004aefd0, 0x004af2ad), boot_crt_exit_d3_cont1 plates (0x004a2bb8, 0x004a583a, 0x004a7800, 0x004ae635)  
**Library match:** Visual Studio 2003 Release CRT (confirmed via Ghidra "Library Function" annotations)

---

## CRT Critical Section Table — `DAT_00616408`

Array of CRITICAL_SECTION pointers. Indexed by lock ID. Each slot is 8 bytes (one CRITICAL_SECTION* per entry). Access pattern: `DAT_00616408 + lock_id * 2` (int-array access, stride 8 bytes per lock slot).

```c
// CRT lock table (VS2003 CRTIMP internals)
// DAT_00616408[lock_id * 2] = CRITICAL_SECTION* for that lock ID
static CRITICAL_SECTION *_crtheap_locks[];  // base: DAT_00616408
```

### Known lock IDs in use

| Lock ID | Usage | Source |
|---------|-------|--------|
| 1 | Stream/file table lock (`__fcloseall`) | 0x004ad3e3 |
| 4 | SBH (small block heap) alloc/free lock | 0x004a45c6, 0x004a4660 |
| 8 | atexit-table lock (`__onexit`) | 0x004a31ea, 0x004a4158 |
| 10 | Lock-slot initialization serializer | 0x004a7800 |
| 13 | `setmbcp` (set multibyte code page) lock | 0x004af2ad |

Lazy initialization: `FUN_004a7800(lock_id)` checks `DAT_00616408[lock_id*2] != 0`; if zero, allocates 0x18 bytes and calls `___crtInitCritSecAndSpinCount` with spin count 4000. First cite: 0x004a780d.

---

## SBH (Small Block Heap) Globals

Cluster at `DAT_008aa68c..0x008aa6a0`. These are VS2003 SBH module-level statics.

| Address | Type | Name | Notes | Source |
|---------|------|------|-------|--------|
| `DAT_008aa68c` | int | SBH max block size | Used in fast-path check: `_Size <= DAT_008aa68c` | 0x004a4554 |
| `DAT_008aa69c` | HANDLE | Win32 heap handle | `HeapAlloc(DAT_008aa69c, 0, _Size)` fallback path | 0x004a4554 |
| `DAT_008aa6a0` | int | SBH mode flag | 3 = SBH active (fast-path), 1 = alignment-only mode (round to 16B) | 0x004a4554 |

---

## Security Cookie

| Address | Type | Name | Notes | Source |
|---------|------|------|-------|--------|
| `DAT_00616038` | int/uint | `__security_cookie` | Stack-overflow guard; checked by `__security_check_cookie` (0x004a2be9) on return; mismatch → `FUN_004a583a(1)` + `ExitProcess(3)` | 0x004a2bb8 |

---

## Multibyte / Locale Globals

Cluster at `DAT_008aa330..0x008aa458`. VS2003 `_setmbcp` / locale subsystem statics.

| Address | Type | Name | Notes | Source |
|---------|------|------|-------|--------|
| `DAT_008aa330` | fn ptr | `__mb_cur_max_func` | Set to `FUN_004aed77()` (code-page determination fn) on `setmbcp` | 0x004aefd0 |
| `DAT_008aa338` | int | `__mb_cur_max` | 1 = SBCS; ≥ 2 = MBCS (from `GetCPInfo.MaxCharSize`) | 0x004aefd0 |
| `DAT_008aa340` | char[0x101] | `__ctype` char-class table | 0x101 bytes; cleared on CP change; filled with byte-range flags (4 categories) from builtin table | 0x004aefd0 |
| `DAT_008aa444` | UINT | current MBCP (code page) | Set to `param_1` (code page argument to `__setmbcp`) | 0x004aefd0 |
| `DAT_008aa450..0x8aa458` | varies | locale code-page data | Copied from builtin table `DAT_00616d74..7c[iVar9]` on CP match | 0x004aefd0 |

---

## Code Page / Locale Builtin Tables — `DAT_00616d70` cluster

Static read-only data cluster at 0x00616d24..0x00616d70. VS2003 locale internals.

| Address | Type / Size | Notes | Source |
|---------|-------------|-------|--------|
| `DAT_00616d24` | char* | Locale-string buffer pointer; allocated 0x351 bytes on demand; NULL = use "C" locale | 0x004ae635 |
| `DAT_00616d2c` | ptr | LC_COLLATE name pointer | part of 6-category array (stride 3 ptrs per category: name, canonical, current) | 0x004ae635 |
| `DAT_00616d30` | ptr[] | LC_* value pointer array base; iterated in steps of 3 ptrs for 6 categories | 0x004ae635 |
| `DAT_00616d3c` | char* | Static "C" locale string; returned when all categories equal | 0x004ae635 |
| `DAT_00616d68` | byte[] | Char-class flag table for builtin code pages (bitmask flags per range) | 0x004aefd0 |
| `DAT_00616d70` | struct[8] | Builtin code-page descriptor table; 8 entries, stride 0x30 bytes | 0x004aefd0 |
| `DAT_00616d74..7c` | varies | Per-entry locale data fields within DAT_00616d70 table | 0x004aefd0 |
| `DAT_00773d70` | int | Unknown flag checked when GetCPInfo fails; gates `setSBCS()` fallback | 0x004aefd0 |

---

## atexit Handler Table — `DAT_008ab6d0` / `DAT_008ab6cc` (resolves U-3728)

**Evidence sources:** boot_crt_exit_d3_cont1 plates, promote_c2_boot_crt plates.

| Address | Type | Name | Notes |
|---------|------|------|-------|
| `DAT_008ab6d0` | fn ptr[] | `__atexit_table_base` | atexit function pointer array; base address of `__onexit` registration array; first cite 0x004a407e |
| `DAT_008ab6cc` | fn ptr* | `__atexit_write_ptr` | Write pointer into the array; incremented after each `__onexit` append; managed under lock 8 |

**Realloc strategy:** `__msize(DAT_008ab6d0)` used to check capacity. Growth: doubles by +0x800 bytes; falls back to +0x10 bytes on failure. Lock 8 is held across the full read-check-write sequence. Accessed by `__onexit_lk` at 0x004a407e.

**U-3728 RESOLVED.** atexit table is at `DAT_008ab6d0`; write cursor at `DAT_008ab6cc`.

---

## SBH Region Table — `DAT_008aa688` extended layout

**Evidence sources:** promote_c2_boot_crt plates, boot_crt_env_cont1 plates.

Extended from the base cluster; region table is a separate struct array:

| Address | Type | Notes |
|---------|------|-------|
| `DAT_008aa680` | ptr | Free-region pointer; used in `___sbh_free_block` |
| `DAT_008aa684` | int | Current region count; read by `___sbh_find_block` at 0x004aa4a0 |
| `DAT_008aa688` | struct ptr[] | SBH region table base; stride **0x14** (20 bytes) per region; first cite 0x004aa49d |
| `DAT_008aa694` | int | Max region count (capacity check at 0x004aa4d7 in `___sbh_alloc_new_region`) |
| `DAT_008aa698` | int | Page index tracker; used in `___sbh_free_block` for VirtualFree page decommit |

### SBH Region Header layout (stride 0x14 bytes per entry, base: DAT_008aa688)

| Offset | Width | Tentative name | Notes |
|--------|-------|----------------|-------|
| +0x00 | 4 | `region_field0` | Region slot field 0 |
| +0x04 | 4 | `region_field1` | Region slot field 1 |
| +0x08 | 4 | `region_va_base` | Virtual address base for this 1 MB region block |
| +0x0c | 4 | `region_va_range_start` | VA range start; used in `___sbh_find_block` pointer membership test |
| +0x10 | 4 | `region_meta_ptr` | Header pointer to SBH group metadata (free-list heads + bitmaps for this region) |

**SBH constants:**
- Region size: 0x100000 (1 MB)
- Group size per region: 0x8000 (32 KB); allocated via VirtualAlloc in `___sbh_alloc_new_group`
- Groups per region: 32 (= 0x100000 / 0x8000)
- Free-list head count per group: 0x40 (64 bins); stride 0x8 each (doubly-linked)
- Free-list base within group: `entry[0x10] + 0x144`
- Bin lookup formula: `(block_size >> 4) − 1`, capped at 0x3f (in `___sbh_free_block`)

---

## CRT File Handle Blocks — `DAT_008aa580` cluster

**Evidence sources:** promote_c2_boot_crt plates.

| Address | Type | Notes |
|---------|------|-------|
| `DAT_008aa580` | ptr[] | Block array base; array of pointers, each pointing to a 0x480-byte block of 32 handle slots |
| `DAT_008aa560` | int | Total slots tracker; incremented by 0x20 per new block allocation |

**Per-block layout:**
- Block size: 0x480 bytes = 32 slots × 0x24 bytes per slot
- Slot stride: 0x24 (36 bytes)
- Slot +0x00 (4 bytes): in-use marker; 0xFFFFFFFF = occupied
- Slot +0x02 (1 byte): init flag bit 0; 0 = free; incremented on init
- Slot +0x0c (0x18 bytes): CRITICAL_SECTION; target of `___crtInitCritSecAndSpinCount` at per-slot CritSec init

---

## CRT Security Error Handler — `DAT_007739f4`

**Evidence sources:** boot_crt_exit_d3_cont1 plates.

| Address | Type | Notes |
|---------|------|-------|
| `DAT_007739f4` | fn ptr | Custom security error handler; non-NULL → call handler; NULL → fall back to MessageBox |

Default behavior (no handler): displays one of two messages:
- param_1 == 1: "A buffer overrun has been detected…"
- other: "A security error of unknown cause…"

Exit: calls `__exit(3)` → `ExitProcess(3)`.

---

## S-DoD impact

These globals block the following S-DoD gates:

| Subsystem | Gate | Status |
|-----------|------|--------|
| boot_crt_env | S-DoD #3 (struct documentation) | **Substantially met** — all SBH, lock, locale, security-cookie globals documented |
| boot_crt_exit | S-DoD #3 | **Partially met** — lock table + security cookie documented; atexit table internals (DAT_006xxxxx for atexit array) still open |

---

## S-DoD update (session 96)

| Subsystem | Gate | Status |
|-----------|------|--------|
| boot_crt_exit | S-DoD #3 | **Substantially met** — atexit table located (U-3728 resolved), CRT security handler documented, file handle block layout complete |
| boot_crt_env | S-DoD #3 | **Substantially met** (unchanged from phase 5 — SBH extended with region header layout) |

---

## Uncertainties

| ID | Question |
|----|---------|
| U-3726 | `DAT_00616d68` — exact flag bit layout for char-class categories (4 categories confirmed, bit positions not decoded) |
| U-3727 | `DAT_00773d70` — exact role; appears to be an SBCS-forcing override flag but not confirmed |
| ~~U-3728~~ | **RESOLVED (session 96):** atexit table at `DAT_008ab6d0`; write cursor at `DAT_008ab6cc` |
