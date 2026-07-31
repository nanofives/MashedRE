I now have all the data needed. Here are the eleven MECHANISM lines:

---

### seed_field_read_field
MECHANISM: Calls fn(structPtr) with a harness-zeroed CONFIG.struct_size scratch buffer; seeds uint32 at CONFIG.seed_off before the call, reads CONFIG.read_size bytes at CONFIG.read_off afterward as the observable. Both sides get identically-seeded separate buffers; no globals touched. Drives any fn(struct_ptr) that derives one field from another within the same struct.
EVIDENCE: diff_template.js:4771-4805

---

### semaphore_create
MECHANISM: Calls fn(out_ptr, initial_int, max_int) with a per-side 4-byte scratch as out_ptr; observable is a 2-bit mask (bit0=ret-non-null, bit1=handle-at-*out_ptr-non-null); actual pointer values discarded since buffers differ per side; resulting handle is CloseHandled immediately. No CONFIG beyond arg_type. FALSE-GREEN hazard: observes success/fail only, not handle value identity.
EVIDENCE: diff_template.js:4314-4362

---

### sentinel_array_ptr
NARROW: Hardcoded __fastcall-vs-cdecl calling-convention split ÔÇö orig dispatched via origFastcallThunk (ECX=0, EDX=buf); reimpl called as cdecl(0, buf). Input int32 array (including 0xFF070000 sentinel terminator) written into buf before each call; observable is fn's return value only. No CONFIG parameterisation; valid only for __fastcall ECX=0/EDX=ptr originals.
EVIDENCE: diff_template.js:608-624

---

### slot_quad_set
MECHANISM: Calls void fn(int idx, uint32* arr); arr is a scratch buffer loaded with CONFIG.slot_field_count uint32 test values; observes those dwords read back from live globals at CONFIG.slot_base_addr + idx*CONFIG.slot_stride; saves and restores live globals around each orig/reimpl pair. CONFIG: slot_base_addr (default 0x006412e8), slot_stride (default 0xf40), slot_field_count (default 4).
EVIDENCE: diff_template.js:4585-4645

---

### source_loop_set
NARROW: Builds a hardcoded 0x200-byte struct with two internal self-pointers (caps ptr at +0x94ÔåÆ+0x180; hwvoice ptr at +0x11cÔåÆ+0x100); seeds +0x28 and hwvoice+0xcc from test {pre28, prehw, hw, loop}; calls fn(structPtr, loop_int); observable is "+0x28-hex:hwvoice+0xcc-hex". All struct offsets are hardcoded constants; no CONFIG parameterisation.
EVIDENCE: diff_template.js:1727-1764

---

### spin_angle_observe
MECHANISM: Calls void fn(int p1, int p2); before each sub-call (both sides separately) zeroes CONFIG.vbuf_len bytes at CONFIG.vbuf_addr_str and injects the per-test float seed (tests[i][2]) into CONFIG.angle_global_str; observable is a 32-bit rolling hash of the vertex buffer post-call. Saves and restores both globals after the batch. Solves per-call accumulator drift that would diverge orig/reimpl within one test cycle.
EVIDENCE: diff_template.js:3988-4051

---

### st0_ret_mat4x3_ptr
NARROW: Calls fn(mat_ptr) with a harness-allocated 4-row├ù16-byte buffer; 12 input floats written as f32 at offsets 0x00/04/08, 0x10/14/18, 0x20/24/28, 0x30/34/38 (pad dwords at +0x0c/1c/2c/3c zeroed for determinism). signature.ret MUST be 'double' to pop ST0 and avoid x87 stack leak; observable is a 16-hex-digit 64-bit double fingerprint. Matrix layout hardcoded; no CONFIG.
EVIDENCE: diff_template.js:325-362

---

### str_arg_int_get
MECHANISM: Calls fn(const_char_ptr) with a per-side 512-byte scratch buffer; input JS string written byte-by-byte with an explicit NUL appended (Memory.allocUtf8String deliberately avoided); observable is fn's return value coerced to uint32; no out-buffer, no globals. No CONFIG beyond arg_type. Applies to any fn(const char*)ÔåÆinteger pure reader regardless of string content or length.
EVIDENCE: diff_template.js:440-458

---

### structptr_seeded_array
MECHANISM: Calls fn(structPtr, arrayPtr); struct is harness-zeroed to CONFIG.struct_size (keeps internal gate fields at 0, suppressing conditional branches); array holds CONFIG.array_vals_len uint32 test values written raw (callee may reinterpret as float); observable is comma-joined hex of CONFIG.read_offs[] dwords read from the struct after the call. CONFIG: struct_size, array_vals_len, read_offs[].
EVIDENCE: diff_template.js:4807-4815 (comment), 4815-4840 (body)

---

### sub_struct_dispatcher
NARROW: Calls fn(p1, p2, p3) with three 64-byte harness-zeroed scratch buffers; observable is ONLY whether the return value equals p1's address (encoded as 1 or 0). No buffer content observed after the call. FALSE-GREEN hazard: passes for any fn that returns its first arg regardless of whether internal dispatch ran correctly. No CONFIG beyond arg_type.
EVIDENCE: diff_template.js:2846-2881

---

### thiscall_nested_field_get
MECHANISM: Calls fn(this_ptr) with per-test outer (CONFIG.struct_size) and inner (CONFIG.inner_size) harness buffers; inner ptr written at outer+CONFIG.outer_off; test value seeded at inner+CONFIG.inner_off; observable is return value only (CONFIG.ret_kind 'u32'|'float'). Generalises thiscall_field_get to one pointer-deref depth ÔÇö the zero-filled outer alone would AV at the inner deref; CONFIG: outer_off, inner_off, ret_kind, struct_size, inner_size.
EVIDENCE: diff_template.js:3450-3498
