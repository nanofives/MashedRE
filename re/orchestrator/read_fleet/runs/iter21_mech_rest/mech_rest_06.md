I have all the data needed. Here are the MECHANISM lines:

---

### fmt_table_search
MECHANISM: fn(ctx_ptr, ptr(0)): per test zeroes the 0x30-byte fmtCtxBuf, seeds input.count at +0x24 and a pointer to fmtEntryPtrBuf (holding input.entry_ptr) at +0x28; second arg is always null; observable = return uint32 only (no buffer read-back); NARROW: ctx offsets 0x24/0x28 and 0x30-byte size are hardcoded; no CONFIG keys.
EVIDENCE: re/frida/diff_template.js:1100-1112, re/frida/diff_template.js:164-165, re/frida/diff_template.js:1235-1236

---

### font_matrix_push
MECHANISM: fn() no args: calls original FontSys_InitRenderState(0x00552c10) once as prelude; per test seeds t.depth into hardcoded global 0x00912b04, calls fn(), reads bool ret and new depth packed as (ret&1)|((depth&0xff)<<8), then restores depth; NARROW: globals 0x00912b04 and prelude address 0x00552c10 are hardcoded; CONFIG: tests=[{depth}].
EVIDENCE: re/frida/diff_template.js:1961-2011

---

### free_via_alloc
MECHANISM: fn(ptr): allocates separate blocks via AllocFn(size, CONFIG.alloc_tag) using CONFIG.alloc_rva_str, calls Orig(pO) and Reimpl(pR) on their respective blocks; observable = crash-absence flag only (1=ok, 0=error) ÔÇö no post-free read-back; a reimpl that silently no-ops passes; CONFIG: alloc_rva_str, alloc_tag; tests=[int sizes].
EVIDENCE: re/frida/diff_template.js:1862-1884

---

### guid_from_tag
MECHANISM: fn(uint32_tag, out_ptr): per test allocates separate 16-byte buffers gO/gR preset to 0xCC, calls fn(tag, buf) for each side; observable = 16-byte bufFingerprint (position-sensitive XOR); separate allocs so pointer values differ between sides; CONFIG: tests=[uint32 tags]; broader: any fn(uint32, byte*) writing a fixed 16-byte structure to an out-pointer.
EVIDENCE: re/frida/diff_template.js:1646-1667

---

### idx_out2
MECHANISM: fn(uint32_idx, out_ptr1, out_ptr2): passes tests[i] as u32, shared 8-byte harness buf as out_ptr1, buf+4 as out_ptr2; observable = return value only ÔÇö neither out-slot is ever read back or fingerprinted; a reimpl returning the correct scalar while writing garbage to both outs passes silently (false-GREEN hazard identical to out3_idx); no CONFIG beyond tests[].
EVIDENCE: re/frida/diff_template.js:603-607, re/frida/diff_template.js:1174-1175

---

### int2out
MECHANISM: fn(int_idx, out_a_ptr, out_b_ptr): separate 4-byte buffer pairs per side (a1/a2 for Orig, b1/b2 for Reimpl), zeroed before each call; observable = both out-dwords and return value packed as \<a_hex\>,\<b_hex\>:\<ret_hex\> ÔÇö all three must match; CONFIG: tests=[int indices]; broader: any fn(int, U*, U*) writing 4 bytes to each of two out-pointers with a return value.
EVIDENCE: re/frida/diff_template.js:2098-2122

---

### int_outbuf4
MECHANISM: fn(int_idx, out_buf4_ptr): separate 4-byte buffers per side (ioBufA for Orig, ioBufB for Reimpl), zeroed before each call; observable = 4 bytes read back as uint32 (little-endian fingerprint); return value is NOT observed (void return assumed); CONFIG: tests=[int indices]; broader: any fn(int, byte*) writing exactly 4 bytes to an out-pointer with void return.
EVIDENCE: re/frida/diff_template.js:2013-2039

---

### int_with_out_ptr
MECHANISM: fn(uint32, out_ptr): passes tests[i] as u32 first arg and shared 8-byte harness buf as second arg (no pre-poison); observable = return value only ÔÇö buf contents are never read back or fingerprinted; a reimpl returning the correct scalar while writing garbage to *buf passes silently (false-GREEN hazard identical to out3_idx); no CONFIG beyond tests[].
EVIDENCE: re/frida/diff_template.js:483-486, re/frida/diff_template.js:1174-1175

---

### large_buffer_save_restore
MECHANISM: fn() no args: snapshots CONFIG.buffer_addr (CONFIG.buffer_size_dwords*4 bytes) once, restores before each Orig and Reimpl call so both sides see identical pre-call buffer state; observable = return value; void return always matches if signature.ret='void'; matched crashes pass when CONFIG.crash_equal_ok; CONFIG: buffer_addr, buffer_size_dwords, crash_equal_ok; broader: any zero-arg fn mutating a large global buffer.
EVIDENCE: re/frida/diff_template.js:4710-4769

---

### matrix_rotate
MECHANISM: fn(mat_ptr, axis_ptr, angle_float, mode_int): shared matrBufs (mat=64 bytes alloc, axis=12 bytes); zeros matrBufs.mat before each call, seeds axis[3] floats; observable = 13 of 16 output floats as u32 bit-exact (pad slots [7]/[11]/[15] excluded ÔÇö documented uninitialized in original); no CONFIG keys; tests=[{axis:[3], angle, mode}].
EVIDENCE: re/frida/diff_template.js:1015-1033, re/frida/diff_template.js:173, re/frida/diff_template.js:1217

---

### matrix_rotate_inner
MECHANISM: fn(mat_ptr, axis_ptr, omc_float, sin_float, mode_int): seeds all 16 input matrix floats to shared mriBufs.mat and axis[3] to mriBufs.axis before each call; observable = 13 of 16 floats as u32 bit-exact (skips uninitialized pad [7]/[11]/[15]); richer than matrix_rotate: caller supplies full input matrix (mode 0 pure replace, modes 1/2 use it as concat operand); no CONFIG keys; tests=[{matrix:[16],axis:[3],omc,sin,mode}].
EVIDENCE: re/frida/diff_template.js:1035-1051, re/frida/diff_template.js:174, re/frida/diff_template.js:1220
