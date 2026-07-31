I have all the information I need. Here are the drafted MECHANISM lines for all 11 handlers.

---

### none
MECHANISM: Zero-arg call ÔÇö fn() with no args, no seeding, no buffer, no out-pointers; observable is ONLY fn()'s return value; for a true void function both sides return null/undefined and trivially match ÔÇö FALSE-GREEN hazard for void side-effect-only functions. CONFIG: none.
EVIDENCE: re/frida/diff_template.js:213-218

---

### read_global
MECHANISM: Writes `input>>>0` as u32 into ptr(CONFIG.target_global) then calls fn() with no args; observable is ONLY fn()'s return value; no save/restore of the global, no buffer, no out-pointers. CONFIG: `target_global`. Broader: any zero-arg function whose single input dimension is one writable 32-bit global.
EVIDENCE: re/frida/diff_template.js:219-222

---

### void
MECHANISM: Calls fn() with no args and returns fn()'s raw return value to the comparison framework; no seeding, no buffer, no out-pointer, no observation beyond the return; for a true void return type both sides produce null/undefined and trivially match ÔÇö FALSE-GREEN hazard. CONFIG: none. NARROW: zero-arg, no observation beyond return.
EVIDENCE: re/frida/diff_template.js:369-372

---

### draw_quad_observe
MECHANISM: Passes CONFIG.tests[i] as positional args per CONFIG.signature.args (any count; 'pointer' entries coerced via ptr(), float as-is); zeroes VBUF (ptr(CONFIG.vbuf_addr_str, default 0x00898a20), CONFIG.vbuf_len bytes, default 112) before each side; observable is a polynomial fingerprint of VBUF post-call; fn return value is NOT compared; VBUF restored after. CONFIG: `vbuf_addr_str`, `vbuf_len`, `signature`. Broader: any 5/7/12-arg draw call that fills a known fixed-address vertex buffer.
EVIDENCE: re/frida/diff_template.js:3738-3816

---

### reg_this_call_observe
MECHANISM: Delivers `this` via CONFIG.this_reg (eax/ebx/ecx/edx/esi/edi) or as a __cdecl stack arg ('stack'); seeds *(this + CONFIG.this_field_off) with a per-test u32 sentinel; Interceptor.replaces CONFIG.observe_callee_str with a recorder that captures the callee's first uint32 arg; verifies stub installed before running; observable is the recorded arg ÔÇö fn's return is NOT compared. CONFIG: `this_reg`, `this_field_off`, `observe_callee_str`, `struct_size`. Broader: any fn that conditionally passes a field-derived or hardcoded value to one interceptable callee.
EVIDENCE: re/frida/diff_template.js:2340-2452

---

### thiscall_field_get
MECHANISM: Per-side: allocates a fresh zeroed scratch struct (CONFIG.struct_size, default field_off+64); writes the test seed at CONFIG.field_off (u32/int/float per CONFIG.ret_kind); passes the struct pointer as the sole __cdecl stack arg; observable is fn's return value compared by value. CONFIG: `field_off`, `ret_kind`, `struct_size`. Broader: any fn(ptr)->scalar that reads one field at a configurable byte offset regardless of struct type or register convention.
EVIDENCE: re/frida/diff_template.js:3389-3447

---

### bytes_inplace
MECHANISM: Allocates two INDEPENDENT 256-byte scratch buffers (one per side); fills both from test.init[0..len-1] before each call; calls fn(buf, len) [bytes_inplace] or fn(buf, len, width) [bytes_inplace_3]; observable is a byte fingerprint of the first `len` bytes post-call; fn return value is NOT compared. CONFIG: none beyond arg_type; tests supply {init:[...], len:N [, width:W]}. Broader: any in-place buffer transform of Ôëñ256 bytes with a (ptr,len) or (ptr,len,width) signature.
EVIDENCE: re/frida/diff_template.js:1517-1550

---

### int_copy_outbuf
MECHANISM: Passes (int slot, T* dst) to fn; each side gets its own independent 4 KB buffer pre-filled with sentinel byte 0xCD; observable is a position-sensitive XOR fingerprint of the first CONFIG.out_buf_size bytes (default 24); GREEN requires fingerprint non-zero AND equal ÔÇö non-zero proves the copy executed; fn return value is NOT compared. CONFIG: `out_buf_size`. Broader: any fn(int, ptr) that copies data from a per-slot source global into a caller-allocated output buffer.
EVIDENCE: re/frida/diff_template.js:2766-2803

---

### int_ptr2_out
MECHANISM: Passes (input, buf, buf+4) to fn via callFn's shared harness `buf`; zeroes both 4-byte out-slots before the call; observable is `(out[0] & 0x3f) | ((out[1] & 0x3f) << 8)` ÔÇö ONLY the low 6 bits of each output dword. CONFIG: none. NARROW: observable truncates to 6 bits per slot; both output pointers are always 4 bytes apart in the same shared 8-byte buffer; no parameterization.
EVIDENCE: re/frida/diff_template.js:380-386

---

### reg_this_callee_stub
MECHANISM: Delivers `this` in CONFIG.this_reg (default 'ebx') via x86 trampoline and seeds EAX with a shared scratch clump address on both sides; stubs up to 4 live callees (callee_fill_str, callee_index_str, callee_zero_str, callee_color_str) with deterministic NativeCallbacks; shared clump ensures stored this[0x5c] pointer is identical on both sides; observable is the full per-side struct fingerprint (all CONFIG.struct_size bytes as packed hex). CONFIG: `this_reg`, `struct_size`, `atom_count`, `callee_fill_str`, `callee_index_str`, `callee_zero_str`, `callee_color_str`.
EVIDENCE: re/frida/diff_template.js:2196-2313

---

### slot_block_zero
MECHANISM: Passes integer slot to fn(slot); computes base = ptr(CONFIG.target_global) + slot * CONFIG.entity_byte_stride; pre-writes CONFIG.sentinel_value (default 0xDEADBEEF) to base[0]; calls fn; observable is base[0] post-call (typically 0 for a memset-style initializer); saves/restores original dword; fn return value is NOT compared. CONFIG: `target_global`, `entity_byte_stride`, `sentinel_value`.
EVIDENCE: re/frida/diff_template.js:739-760
