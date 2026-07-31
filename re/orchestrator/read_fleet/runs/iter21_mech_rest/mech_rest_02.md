I now have all the data needed. Let me compile the MECHANISM lines.

---

### st0_ret_global
MECHANISM: Zero stack args (fn() with no args); seeds CONFIG.global_a (hex addr of a writable 32-bit float global) with input as f32, snapshots and restores the 4-byte U32 around the call; returns a 16-hex-digit fingerprint of the 64-bit ST0 double (full mantissa); observes nothing else ÔÇö false-GREEN hazard if ST0 return is always 0 for the seeded input; CONFIG.global_a is the only key; fits any no-stack-arg leaf that reads exactly one f32 global and returns its result in ST0.
EVIDENCE: re/frida/diff_template.js:260-274, re/frida/diff_template.js:250-259

---

### struct_three_write
MECHANISM: fn(harness_ptr, uint32, uint32); per-side harness-allocated scratch of CONFIG.struct_size bytes (default 32) sentinel-filled with 0xDEADBEEF before each call; calls fn(buf, tests[i][0], tests[i][1]); fingerprints uint32s at CONFIG.observe_offsets (default [0x0c,0x10,0x14]) as a comma-joined string; return value is NOT observed; observe_offsets is fully configurable so the handler is not restricted to the default layout ÔÇö any fn(ptr,u32,u32) that writes to configurable offsets in its first arg fits.
EVIDENCE: re/frida/diff_template.js:4540-4583

---

### teardown_call_pair
MECHANISM: Zero-arg fn(); before EVERY call (both Orig and Reimpl) writes 0 to CONFIG.state_global_str (default 0x007d3ff8) so both sides start from the same already-torn-down state; saves the global once before the loop and restores best-effort after; observes return value (normalised to uint32) or matched crash message via crash_equal_ok; NARROW: only one configurable global is corrupted, no buffer setup ÔÇö fits only no-arg teardown/shutdown thunks where the sole pre-condition is one specific global = 0.
EVIDENCE: re/frida/diff_template.js:4647-4701

---

### vec3_normalize
MECHANISM: fn(out_ptr, in_ptr); module-level shared buffers v3nBufs.{in,out} (both re-seeded before every callFn call so Orig and Reimpl receive the same pointer and fresh data); seeds input[0..2] as f32 into in-buf, zeroes out-buf (3 dwords); observes [magnitude-f32-bits, out[0..2]-bits] as a 4-element comma string; broader than normalisation: fits any fn(float3* out, float3* in) -> float with a 12-byte output buffer ÔÇö CONFIG-free, buffer size is hardcoded to 3 floats.
EVIDENCE: re/frida/diff_template.js:976-992, re/frida/diff_template.js:169-170

---

### audio_list_drain
MECHANISM: fn(sentinel_ptr); harness allocates a fresh per-side 12-byte self-looping sentinel (next@+0, prev@+4, count@+8), inserts N nodes by calling the real game function at CONFIG.insert_rva_str (loaded as NativeFunction with 'mscdecl' void(pointer,int32)), then calls orig/reimpl to drain; observes whether the sentinel self-loops post-drain (1=empty, 0=not); return value not observed; NARROW: 12-byte circular-list layout and insert-function calling convention are fixed ÔÇö only fits list-drain functions using that exact node shape and the game's own insert function.
EVIDENCE: re/frida/diff_template.js:3090-3119

---

### audio_sub_struct_dual
MECHANISM: fn(uint32 p1, uint32 p2, uint32 p3) where p1 is the integer representation of a harness-allocated 12-byte zeroed scratch buffer (callee casts internally); per-side separate buffers allocated at different addresses and both passed as distinct p1 values; tests[i] = {p2, p3} object; fingerprint = (ret-non-null-flag<<24)|(low-24-bit bufFingerprint of 12 bytes); broader than the name: fits any 3-uint32-arg fn whose first arg is a buffer address passed as a plain integer (uint32-cast pointer pattern) and which writes to that buffer.
EVIDENCE: re/frida/diff_template.js:4233-4266

---

### audio_sub_struct_link
MECHANISM: fn(ptr, uint32); per-side 12-byte zeroed scratch buffers (separate pointers); tests[i] = flat uint32 p2; fingerprint = (return-ptr-non-null<<24)|(low-24-bit bufFingerprint of all 12 buffer bytes); compares the combined fingerprint across sides ÔÇö broader than struct-link: fits any fn(ptr, uint32) that writes to a Ôëñ12-byte pointer-arg struct and returns a pointer, as long as zeroing the buffer suppresses any cleanup-callee side-effects; 12-byte size is hardcoded.
EVIDENCE: re/frida/diff_template.js:4193-4224

---

### audio_sub_struct_zero
MECHANISM: fn(ptr); per-side scratch buffer of CONFIG.struct_size bytes (default 24) filled with sentinel 0xAA before each call; test values are ignored (iteration markers only); observes rolling XOR/multiply hash of bytes [CONFIG.observe_offset, +CONFIG.observe_length) (both default to the full buffer); return value is NOT observed; crash_equal_ok supported; no comment above dispatch ÔÇö mechanism purely from body; broader than the name: fits any fn(ptr) that mutates a configurable-size buffer with no return value observable, with configurable observation window.
EVIDENCE: re/frida/diff_template.js:4408-4439, re/frida/diff_template.js:4407 (no comment block above)

---

### bytes_inplace_3
MECHANISM: fn(ptr, int len, int width); shares a single dispatch `if` with bytes_inplace (2-arg); per-side 256-byte scratch buffers (bufA/bufB) filled from tests[i].init (byte array, length tests[i].len) before each call; calls fn(buf, len, width) where width = tests[i].width; observes bufFingerprint(buf, len) ÔÇö rolling XOR fingerprint of the first len bytes of the output buffer; return value is NOT observed; fits any in-place buffer mutator with signature (ptr, len, width) up to 256 bytes output.
EVIDENCE: re/frida/diff_template.js:1517-1550

---

### eax_implicit_int
MECHANISM: Zero stack args; per-side x86 trampoline (10 bytes: `mov eax, imm32; jmp target`) injected into writable executable memory ÔÇö the imm32 is patched per-test to set EAX to the test uint32 before jumping to the real target; NativeFunction declared with empty args list and CONFIG.signature.ret; observes EAX return value normalised to uint32; eax_implicit_ptr sub-mode substitutes a fresh zeroed 64-byte scratch buffer address as the EAX value; CONFIG.signature.ret is the only required CONFIG key; fits any function whose sole input is a uint32 (or pointer) already in EAX with NO stack args.
EVIDENCE: re/frida/diff_template.js:3319-3386

---

### entity_field_add
MECHANISM: fn(int idx, int delta) on a live global array; computes field address as ptr(CONFIG.target_global) + idx ├ù CONFIG.entity_byte_stride; snapshots the 4-byte field before the call and restores it afterward so both Orig and Reimpl start from the same baseline (prevents accumulation); observes both the uint32 return value AND the post-add field value packed as "ret_hex:field_hex"; CONFIG.max_index (default 0xf) guards out-of-range idx ÔÇö no live write occurs, returns 0:0; CONFIG keys: target_global, entity_byte_stride, max_index; fits any indexed-array incrementer with configurable base and stride.
EVIDENCE: re/frida/diff_template.js:665-686
