---

## 0x004b6b00 (3 bytes) ÔåÆ 004b6640:C2

**Plate:** `re/analysis/render_3_c1_to_c2_s6/FUN_004b6b00.md` (primary; bucket plate at `bucket_004b4a80` says C1 but the C2 promotion plate is authoritative).

**Signature:** `void __fastcall FUN_004b6b00(undefined4 *param_1)` ÔÇö ECX = param_1, implicit `in_EAX` read on entry, stores EAX into `*param_1`. Leaf; 3 bytes.
**Globals read:** none. **Globals written:** none. **Caller-memory written:** `*param_1`. **Callees:** none.

**hooks.csv:** `004b6b00,FUN_004b6b00,render,C2,mapped,...,__fastcall store-EAX helper: *param_1=in_EAX; 2-byte body; leaf;`

**Verdict: NEEDS_NEW_HANDLER**

Signature is complete and unambiguous (C2 promotion plate: "behavior unambiguous"). Writes no named global. The block is: no handler can both (a) control the implicit EAX register before the call, (b) pass ECX = a fresh scratch buffer, and (c) observe `*ECX` after. Handlers considered and rejected:

- `eax_implicit_int` / `eax_implicit_ptr` (line 3238): both handlers have EAX as a *source pointer or source int whose derived result is observed in a global or return value ÔÇö they do not model the pattern where the harness-controlled EAX value is stored into *ECX and observed there.
- `ptr_seed_observe` (line 1314): allocates a buffer for the declared ptr arg (ECX) and observes it, but calls the function via normal C dispatch ÔÇö does not set EAX before the call, so the stored value is the harness runtime's stale EAX (non-deterministic).
- `void_write_observe` (line 4941): monitors a specific absolute address for writes; does not address controlling EAX.

A dedicated handler is required: set EAX = test_value, ECX = scratch_buf, call, compare `*scratch_buf` with test_value.

---

## 0x00407580 (17 bytes) ÔåÆ 00459000:C2

**Plate:** `re/analysis/bucket_gameplay_00405400_00407620/0x00407580.md`

**Signature:** `undefined4 FUN_00407580(int param_1)`
**Globals read:** `DAT_00639dc4` (record-array base; `(&DAT_00639dc4)[param_1 * 0x3b]` returned). **Globals written:** none. **Callees:** none. Leaf pure getter.

**hooks.csv:** `00407580,FUN_00407580,gameplay,C2,mapped,...`

**Verdict: READY**

Handler: `int_scalar` ÔÇö `| int_scalar | 398 | 141 | int_scalar ÔÇö single uint32 arg, any integer return type |`

No writes; single int arg; integer return; handler covers the argument shape exactly.

**Test vectors (4, non-degenerate):**
- param_1=0 vs param_1=1: hits `DAT_00639dc4 + 0*0xec` vs `+1*0xec` ÔÇö different memory locations ÔåÆ different key-field values. Concrete reason: `0x00407550` linearly scans these keys for a target; if all keys were equal the scan would be non-discriminating, so the game data requires distinct keys per record.
- param_1=1 vs param_1=2: same argument ÔÇö distinct record slots.
- param_1=0 vs param_1=2: spans 2 record strides; maximum chance of different runtime values.
- param_1=3 vs param_1=0: access record 3 vs record 0 ÔÇö distinct slots, distinct keys.

All four input pairs access different array elements at `0x00639dc4 + n*0xec`; distinct outputs are guaranteed by the game data invariant (records are a scannable key-indexed collection).

---

## 0x00471430 (22 bytes) ÔåÆ 00426340,0045bae0,0047a020,0047a0f0,0047a130 (all C2)

**Plate:** `re/analysis/bucket_gameplay_00471430_0047b6b0/0x00471430.md`

**Signature:** `void FUN_00471430(void)` ÔÇö no params, void return.
**Globals read:** `DAT_006905c8` (current-record index). **Globals written:** `(&DAT_0069064c)[DAT_006905c8 * 0x23]` ÔÇö stores 0 into the count field of the selected record. **Callees:** none.

**hooks.csv:** `00471430,FUN_00471430,gameplay,C2,mapped,...`

**Verdict: MUTATOR_LANE**

Writes global `DAT_0069064c` (element indexed by `DAT_006905c8 * 0x23`).

---

## 0x0047cde0 (27 bytes) ÔåÆ 0045bba0:C2

**Plate:** `re/analysis/bucket_gameplay_0047ba20_0047f380/0047cde0.md`

**Signature:** `void FUN_0047cde0(int param_1, undefined4 param_2)` ÔÇö bounds-checked setter.
**Globals read:** none (bound 200 is an immediate). **Globals written:** `DAT_006c9438` ÔÇö writes `param_2` to `(&DAT_006c9438)[param_1]` when `0 Ôëñ param_1 < 200`; no-op otherwise. **Callees:** none (leaf).

**hooks.csv:** `0047cde0,FUN_0047cde0,gameplay,C2,mapped,...`

**Verdict: MUTATOR_LANE**

Writes global `DAT_006c9438`.

---

## 0x005b1160 (29 bytes) ÔåÆ 005b74c0:C2, 005bce80:C2

**Plate:** `re/analysis/bucket_audio_005af070_005b2190/0x005b1160.md`

**Signature:** `void FUN_005b1160(undefined1 *param_1, undefined4 param_2, undefined1 param_3)` ÔÇö writes a deterministic 8-byte ring/cursor header into `*param_1`: `[+0]=0, [+1]=0, [+2]=0, [+3]=param_3, [+4..+7]=param_2`.
**Globals read:** none. **Globals written:** none. **Caller-memory written:** `*param_1` (the header struct, harness-allocatable buffer). **Callees:** none (leaf).

**hooks.csv:** `005b1160,FUN_005b1160,audio,C2,mapped,...`

**Verdict: READY**

Handler: `ptr_seed_observe` ÔÇö `| ptr_seed_observe | 1314 | 7 | {i32:true} -> next value from test.scalars CONFIG.observe array [{buf:i, off:N, type:'f32'|'u8'|'u16'|'u32'|'s32'}] CONFIG.tests[i] = { seed:[{buf,off,t... |`

The handler allocates a fresh buffer for param_1, passes param_2 and param_3 as scalar args, and snapshots byte-offset fields in the buffer. No globals written; the buffer writes go into the harness scratch allocation only.

**Test vectors (4, non-degenerate):**
- param_2=0x10000000, param_3=4: observed `[+3]=0x04, [+4..+7]=0x10000000`.
- param_2=0x20000000, param_3=8: observed `[+3]=0x08, [+4..+7]=0x20000000` ÔÇö both fields differ from vector 1.
- param_2=0x10000000, param_3=16: same param_2 as #1, different param_3 ÔåÆ `[+3]` differs (16 vs 4).
- param_2=0x40000000, param_3=4: same param_3 as #1, different param_2 ÔåÆ `[+4..+7]` differs (0x40000000 vs 0x10000000).

`[+0..+2]` are always 0 (hard-coded); `[+3]` isolates param_3 variation; `[+4..+7]` isolates param_2 variation. Each vector pair differs on at least one observed field.

---

## 0x005bfb90 (30 bytes) ÔåÆ 005be260:C2

**Plate:** `re/analysis/bucket_audio_005bf4d0_005c9770/0x005bfb90.md`

**Signature:** `void fn(param_1)` ÔÇö single pointer arg (COM object). Calls `ReleaseSemaphore((HANDLE)param_1[0x54], 1, NULL)` then `(**(code **)(*param_1 + 8))(param_1)` (vtable slot +8, Release).
**Globals read:** none. **Globals written:** none. **Callees:** `ReleaseSemaphore` (kernel32), `vtable[+8]` Release.

**hooks.csv:** `005bfb90,FUN_005bfb90,audio,C2,mapped,...`

**Verdict: NEEDS_NEW_HANDLER**

Signature complete; no direct global write. Calls OS API with a real HANDLE stored inside the object. No existing handler can construct a test object with a live semaphore handle at `+0x150` and a vtable with a safe Release stub. Handlers considered and rejected:

- `dsound_secondary_init` (line 2807): builds fake IUnknown for DS secondary buffer init with stub vtable; does not model ReleaseSemaphore on a real HANDLE, and its teardown sequence (QIÔåÆinitÔåÆRelease) does not match this function's signature.
- `teardown_call_pair` (line 4575): dispatches void() no-arg calls; cannot handle a 1-arg ptr function or OS handle setup.
- `ptr_arg_int_get` (line 413): read-only getter returning int; this function is void with OS side effects.
- `ptr_nonnull_check` (line 694): observes null/non-null only; ReleaseSemaphore side effect is not observable that way.

A dedicated handler is required: create a real semaphore via `CreateSemaphore`, store the handle at `param_1+0x150`, install a no-op Release stub at vtable[+8], call the function, verify the semaphore count incremented (WaitForSingleObject(handle, 0) returns WAIT_OBJECT_0).

---

**2 READY / 2 MUTATOR_LANE / 0 NEEDS_GHIDRA / 2 NEEDS_NEW_HANDLER**
