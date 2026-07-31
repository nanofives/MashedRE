Now I have all the dispatch bodies. Let me draft the MECHANISM lines.

---

### matrix_scale
MECHANISM: fn(mat_buf, scale_buf, mode_int); harness writes input.mat[16 floats] into matsBufs.mat and input.scale[3 floats] into matsBufs.scale, calls fn in-place, reads back 13 of 16 dwords (skips indices 3/7/11 as flags) as comma-joined u32 fingerprint; return value not observed; no CONFIG keys ÔÇö buffer names and skip indices are hardcoded.
EVIDENCE: diff_template.js:1053-1068

### out1_idx
MECHANISM: fn(buf, uint32_idx)->scalar; poisons buf[0] with 0xCCCCCCCC, calls fn(buf, input>>>0), fingerprints as "<ret_hex>:<written_dword_hex>"; observes BOTH the return value and the dword written at buf+0; CONFIG: tests[] of uint32 indices; broader than out3_idx ÔÇö a reimpl that returns correct flag but writes garbage fails here; only one dword of buf is read back.
EVIDENCE: diff_template.js:595-601

### out_buf_fmt_2
MECHANISM: fn(int p1, uint32 p2, char* outA, char* outB)->void; harness allocates four separate Memory.alloc buffers (two per side, size CONFIG.out_buf_size, default 32), zeros them before each call, calls Orig/Reimpl independently, observes null-terminated C-string contents of both output buffers joined by '|'; return value not observed; input per test is [p1,p2] or scalar (p2 defaults 0); CONFIG: out_buf_size.
EVIDENCE: diff_template.js:3826-3867

### pcm_pack
MECHANISM: fn(i16* dst, i32* src, u32 count)->void; writes input.src[] as int32s into ONE shared srcBuf (pointer-equal on both sides), allocates separate dstO/dstR per side, calls Orig(dstO,srcBuf,count) then Reimpl(dstR,srcBuf,count), observes hex dump of nSamples*2 bytes from each dst; return value not observed; CONFIG: tests[] of {src:[int32...],count:N}; NARROW: src is shared ÔÇö a function that mutates src corrupts the reimpl call.
EVIDENCE: diff_template.js:1487-1514

### pcm_sat_add
MECHANISM: fn(out, srcA, srcB, byteCount)->void; writes test.a[]/test.b[] as int16 into shared srcA/srcB buffers (pointer-equal both sides), allocates separate outO/outR, calls fn(outX,srcA,srcB,n*2), observes bufFingerprint of n*2 bytes; return value not observed; CONFIG: tests[] of {a:[int16...],b:[int16...]}; NARROW: srcA/srcB shared ÔÇö in-place src mutation corrupts the second call.
EVIDENCE: diff_template.js:1623-1644

### ptr_nonnull_check
MECHANISM: fn(void)->pointer; optionally writes input>>>0 to CONFIG.target_global (any hex address) before each call to reset cached state; observable is null-vs-non-null only (returns 0 or 1) ÔÇö actual pointer address is never compared; CONFIG: target_global (optional); broader: any no-arg function returning a non-deterministic pointer where only NULL/non-NULL distinguishes correctness.
EVIDENCE: diff_template.js:724-737

### ptr_ptr_entity_set
MECHANISM: fn(int p1, uint32 p2)->void; no harness buffer ÔÇö reads live outer pointer from CONFIG.target_global at runtime, computes effective=(*target_global)+p1*stride+field_offset and reads u32 there after each call as observable; null outer pointer yields 0/0 match (graceful); CONFIG: target_global (hex string), entity_byte_stride (default 4), field_offset (default 0); broader than entity_field_set by one extra pointer deref of the base.
EVIDENCE: diff_template.js:4054-4106

### ptr_scratch_field
MECHANISM: fn(ptr)->int; harness allocates a 0x80-byte zeroed buffer, writes test byte value at CONFIG.field_offset (default 0x54), calls Orig/Reimpl with that pointer, compares return values as u32; buffer contents after call are not observed; CONFIG: field_offset (byte offset, default 0x54), tests[] (byte values); broader: any fn(ptr)->int that extracts a scalar from a configurable byte offset in a zeroed struct.
EVIDENCE: diff_template.js:3121-3143

### ptr_zero_pair
MECHANISM: fn(uint32* p)->void; allocates separate 16-byte buffers per side, preloads p[0] and p[1] with sentinel and p[2] with guard 0xA5A5A5A5, calls Orig/Reimpl, compares bufFingerprint of all 12 bytes; CONFIG: tests[] of sentinel uint32 values; NARROW: hardcodes exactly-2-dword write assumption with fixed guard at +8; no configurable field count or stride.
EVIDENCE: diff_template.js:1669-1690

### renderer_field3c_set
MECHANISM: fn(struct_ptr, uint32 val)->void; harness allocates a 0x200-byte struct per side, sets bit-3 flag at +0x78 (hw path), embeds self-pointer at +0x11c pointing to hwvoice block at +0x140, seeds 0xDEADBEEF sentinels at +0x3c and +0x174, calls fn(sX,val), observes "[+0x3c_hex]:[+0x174_hex]"; CONFIG: tests[] of {val:uint32,hw:0|1}; NARROW: all offsets hardcoded (0x3c,0x78,0x11c,0x140,0x174), no CONFIG keys to reparameterise.
EVIDENCE: diff_template.js:1692-1724

### resource_loader_4arg
MECHANISM: fn(uint16 nameId, char* typeName, uint8** pOutBuf, uint32* pOutLen)->int; harness allocates NUL-terminated string for typeName and two 4-byte out-pointer slots (zeroed before each call), observable is (ret&1)|(bufNonNull<<1) ÔÇö success/failure bit and whether *pOutBuf is non-null; actual resource bytes are NOT compared because both sides call into the same MASHED.exe module and LockResource returns the same address; CONFIG: tests[] of {name_id:uint16,type_str:string}, no other keys.
EVIDENCE: diff_template.js:4481-4536
