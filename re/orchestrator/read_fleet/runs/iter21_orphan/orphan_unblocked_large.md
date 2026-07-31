---

## 0x005bfa20 (32 bytes) ÔåÆ 005be260:C2

**1. Plate:** `re/analysis/bucket_audio_005bf4d0_005c9770/0x005bfa20.md`

**2. Plate content (frontmatter + mechanical description):**
- Frontmatter signature field: absent ÔÇö plate has no typed C declaration.
- Mechanics: 2-arg method. Reads `*(param_1 + 0x140)` (COM interface ptr). If NULL returns `0x80040209` (VFW_E_NO_ALLOCATOR). Else calls `(**(code **)(*piVar1 + 0x18))(piVar1, param_2)` ÔÇö vtable slot +0x18 on that interface.
- Globals read: none (only reads `param_1+0x140`).
- Globals written: none.
- Callees: `vtable[+0x18] on field+0x140 COM interface`.

**3. hooks.csv:** `005bfa20,FUN_005bfa20,audio,C2,mapped,re/analysis/bucket_audio_005bf4d0_005c9770/0x005bfa20.md,batch-x-s6,,batch_x s6 RwaDSRenderer DirectShow filter; audio backend implemented via DirectShow filter graph (RW audio over DS) | C1->C2 batch_ai sweep-20260602-0347`

**4. Classification: NEEDS_GHIDRA**

The plate has no typed C signature declaration (no calling convention, no explicit arg types ÔÇö only "2-arg method"). The `param_1`/`param_2` labels imply stack-based ABI, but the type of `param_2` (passed opaquely to the vtable call) is not stated. Without the formal typed signature, handler selection is not deterministic. Additionally, non-NULL-path testing requires planting a 2-level COM vtable stub (obj at `param_1+0x140` ÔåÆ vtable array ÔåÆ callable stub at slot 6), which no existing handler supports.

---

## 0x00407550 (35 bytes) ÔåÆ 00481a30:C2

**1. Plate:** `re/analysis/bucket_gameplay_00405400_00407620/0x00407550.md`

**2. Plate content:**
- Signature: `undefined * FUN_00407550(void)` ÔÇö but plate explicitly states key argument is `unaff_ESI`.
- Mechanics: linear scan of `DAT_00639d80[]`, stride `0xec`, count `DAT_0063a5d0`. Match condition: `*(puVar1 + 0x44) == unaff_ESI`. Returns record pointer or NULL.
- Globals read: `DAT_00639d80` (table base), `DAT_0063a5d0` (count).
- Globals written: none.
- Callees: none.

**3. hooks.csv:** `00407550,FUN_00407550,gameplay,C2,mapped,re/analysis/bucket_gameplay_00405400_00407620/0x00407550.md,batch-t-s1,,batch_t s1 MULTI[...] | C1->C2 batch_al sweep-20260603-0334`

**4. Classification: NEEDS_NEW_HANDLER**

Signature is known and complete (ESI=search_key, linear search at entry+0x44, return record ptr or NULL). No global writes. Handlers considered and rejected:

- **`esi_global_search`** (early-window-only): Shape matches ABI (ESI=key, linear search of global table, cdecl reimpl). Fails: its description is "for `entry[+0]==key`" ÔÇö key field hardcoded at offset 0, not offset 0x44. The seeding strategy seeds `table[idx*stride]` (offset 0). Cannot match this function's `puVar1 + 0x44` comparison without a new handler.
- **`container_find_scalar`**: `int fn(key, table*, count)` ÔÇö cdecl, not ESI register; different ABI entirely.
- **`arg_table_linear_search`**: `int fn(key, table*, count)` ÔÇö cdecl, not ESI; and takes table/count as stack args not globals.
- **`read_global`**: zero-arg global reader; not applicable.
- **`ptr_arg_int_get`**: `fn(ptr)` stack arg, not ESI implicit.

Needed handler: ESI=key implicit, linear search of globals (base `DAT_00639d80`, count `DAT_0063a5d0`, stride `0xec`), match at `entry+0x44`, return matched record pointer or NULL. Early-window lane (reads live globals). Would extend `esi_global_search` to support a configurable `key_field_offset` parameter.

---

## 0x004722e0 (37 bytes) ÔåÆ 00472380:C2

**1. Plate:** `re/analysis/bucket_gameplay_00471430_0047b6b0/0x004722e0.md`

**2. Plate content:**
- Signature: `void FUN_004722e0(int param_1)`.
- Mechanics: saves `*(param_1 + 0x48)` into `DAT_0086ecd0`; installs `&LAB_004721e0` into `*(param_1 + 0x48)`.
- Globals read: none directly.
- Globals written: **`DAT_0086ecd0`** (saved-previous-handler slot, cited at `0x004722e6`).
- Also writes: `*(param_1 + 0x48)` (caller-owned object field).
- Callees: none.

**3. hooks.csv:** `004722e0,FUN_004722e0,gameplay,C2,mapped,re/analysis/bucket_gameplay_00471430_0047b6b0/0x004722e0.md,batch-y-s2,,batch_y s2 MIXED gameplay: ... | C1->C2 batch_an sweep-20260603-1259`

**4. Classification: MUTATOR_LANE**

Writes global `DAT_0086ecd0` and caller-owned memory at `*(param_1 + 0x48)`.

---

## 0x00497470 (62 bytes) ÔåÆ 00498510:C2

**1. Plate:** `re/analysis/frontend_c1_to_c2_followup_s3/FUN_00497470.md`

**2. Plate content:**
- Signature: `undefined4 __fastcall FUN_00497470(HWND param_1, int param_2)` with `in_EAX` = dialog control ID.
- Mechanics: calls `GetDlgItem(param_1, in_EAX)`, `SendMessageA` with `CB_GETCURSEL` (0x147), `SendMessageA` with `CB_GETITEMDATA` (0x150), then stores result into `(&DAT_007e96fc)[param_2 * 0x80]`. Returns 1.
- Globals read: `DAT_007e96fc` (base of per-player array, for addressing).
- Globals written: **`DAT_007e96fc[param_2 * 0x80]`** (per-player data slot).
- Callees: `GetDlgItem`, `SendMessageA`.

**3. hooks.csv:** `00497470,FUN_00497470,frontend,C2,new,re/analysis/cluster_0049_first_pass/00497470.md,,,cluster_0049_first_pass batch_t-s3; ... | batch-frontend-followup-s3 plate at re/analysis/frontend_c1_to_c2_followup_s3/FUN_00497470.md`

**4. Classification: MUTATOR_LANE**

Writes global `DAT_007e96fc` (per-player input mode array, at index `param_2 * 0x80`).

---

## 0x00497b10 (74 bytes) ÔåÆ 004982a0:C2

**1. Plate:** `re/analysis/frontend_c1_to_c2_followup_s3/FUN_00497b10.md`

**2. Plate content:**
- Signature: `void FUN_00497b10(void)` with implicit `in_EAX` = control ID, `unaff_ESI` = control index, `unaff_EDI` = HWND dialog parent.
- Mechanics: checks `(&DAT_007e96fc)[DAT_007730b0 * 0x80] != 1 || in_EAX < 9 || 0xc < in_EAX`. If condition true: writes `unaff_ESI` ÔåÆ **`DAT_0077315c`**, writes `in_EAX` ÔåÆ **`DAT_00773160`**, calls `SetTimer`, `GetDlgItem`, `SetFocus`.
- Globals read: `DAT_007e96fc`, `DAT_007730b0`.
- Globals written: **`DAT_0077315c`** (saved control index), **`DAT_00773160`** (saved control ID).
- Callees: `GetDlgItem`, `SetTimer`, `SetFocus`.

**3. hooks.csv:** `00497b10,FUN_00497b10,frontend,C2,new,re/analysis/cluster_0049_first_pass/00497b10.md,,,cluster_0049_first_pass batch_t-s3; ... | batch-frontend-followup-s3 plate at re/analysis/frontend_c1_to_c2_followup_s3/FUN_00497b10.md`

**4. Classification: MUTATOR_LANE**

Writes globals `DAT_0077315c` and `DAT_00773160`.

---

**0 READY / 3 MUTATOR_LANE / 1 NEEDS_GHIDRA / 1 NEEDS_NEW_HANDLER**
