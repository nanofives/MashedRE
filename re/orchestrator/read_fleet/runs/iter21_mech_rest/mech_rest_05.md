I have all the reads I need. Let me now compose the MECHANISM lines.

---

### cstr_ret_offset
MECHANISM: Call shape fn(char* buf) ÔåÆ char*; harness allocates 512-byte shared buf, writes test.str as NUL-terminated ASCII, calls each side with same buf ptr; observes return pointer as byte offset from buf start (retptr ÔêÆ buf), reports ÔêÆ1 if NULL. Observes return offset only. CONFIG: tests[].str. Applies to any fn(char*) ÔåÆ char* returning a pointer into its input buffer (e.g., extension finders, substring locators).
EVIDENCE: diff_template.js:1592-1615

---

### cursor_back
MECHANISM: NARROW: hardcodes 5 absolute globals (row=0x67f17c, col=0x67f184, flag=0x67f1a4, mp_flag=0x67ea68, game_mode=0x67e9fc). Call shape fn(void); seeds 4 globals from input {row,col,flag,mp_flag}; observes post-call row/col packed as (row&0xffff)<<16|(col&0xffff); saves/restores all 5 globals. CONFIG: input.row/col/flag/mp_flag only.
EVIDENCE: diff_template.js:687-722

---

### device_transform_dispatch
MECHANISM: NARROW: shared buffers xfdBufs (same out/mat/in pointers across both sides); call shape fn(out, mat, 1, in) with hardcoded count=1; seeds 16-float mat + 3-float in, zeros out; observes 3 u32s from xfdBufs.out. Both sides dispatch same device globals (0x7d3ff8/0x7d3ffc) ÔåÆ vtable slot +0x14, so GREEN validates the offset/global selection only, not behavioral divergence ÔÇö a false-GREEN risk. CONFIG: input.mat (16 floats), input.in (3 floats).
EVIDENCE: diff_template.js:994-1013

---

### dsound_secondary_init
MECHANISM: NARROW: builds fixed 2-object stdcall IUnknown fake (outer vtable[0]=QI writes inner obj addr, vtable[2]=Release; inner vtable[5]=secondary-init writes sentinel 3 into 4th-arg slot); call shape fn(void** ppUnk) ÔåÆ int; resets outer ptr before each side; observes ((ret & 0xffff) | (stub_call_count << 16)). CONFIG.tests[] is a repeat counter only ÔÇö no per-test input variation; layout hardcoded to this one vtable shape.
EVIDENCE: diff_template.js:2883-2966

---

### eax_implicit_ptr
MECHANISM: Per-side `mov eax,imm32; jmp target` trampoline pre-loads EAX per test; no stack args (CONFIG.signature.args must be empty). ptr variant substitutes per-test zeroed 64-byte scratch-buffer addresses as the imm32 to prevent AV on dereferences; int variant passes raw uint32. Observes return value only. CONFIG: tests[] (uint32/addr values), signature.ret, crash_equal_ok. Applies to any fn whose sole arg arrives in EAX.
EVIDENCE: diff_template.js:3302-3387

---

### endian_pack
MECHANISM: Call shape fn(int** out_ptr_ptr, uint* src, int size) with fully per-side harness buffers; seeds src_val into 4-byte srcSlot, sets ptrSlot ÔåÆ 8-byte zeroed outBuf, resets outBuf before each call; observes 4-byte XOR fingerprint of outBuf. CONFIG: tests[].src_val, tests[].size. Applies to any fn taking a double-pointer output + uint src + int size that writes Ôëñ4 bytes via the out_ptr_ptr chain.
EVIDENCE: diff_template.js:1766-1800

---

### esi_idx_ecx_outbuf4
MECHANISM: Per-side `push esi; mov esi,idx; mov ecx,scratch_ptr; call target; pop esi; ret` trampoline seeds ESI=integer index (imm32 patched per test) and ECX=per-side 16-byte zeroed scratch-buffer ptr (constant per side). Observes all 16 scratch bytes as 4├ùu32 hex fingerprint; no return value captured. CONFIG: tests[] = integer indices (including negatives/OOB). Applies to any void fn delivering an index in ESI and output-buffer ptr in ECX writing Ôëñ16 bytes.
EVIDENCE: diff_template.js:2133-2194

---

### fmt_desc_copy
MECHANISM: Call shape fn(src_ptr, dst_ptr, zero_init) ÔåÆ void; zeroes shared fmtSrcBuf/fmtDstBuf (0x20 bytes each) then seeds src at offsets +0x00/+0x04(u32)/+0x05/+0x10; observes 4 dst fields (+0x04 as u32, +0x0c/+0x0d/+0x18 as u8) XOR-packed into uint32. CONFIG: tests[].f00/f04/f05/f10/zero_init. Same buffer ptrs delivered to both sides (re-zeroed each call); false-GREEN risk if fn stores either buf ptr internally.
EVIDENCE: diff_template.js:1081-1099

---

### fmt_desc_ptr
MECHANISM: Call shape fn(ptr_to_0x20_struct) ÔåÆ int32; harness zeroes shared 0x20-byte buf then writes u32 fields at offsets +0x04/+0x10/+0x14; observes return value only ÔÇö no buffer readback. CONFIG: input.f04/f10/f14. Same buf pointer delivered to both sides (re-zeroed each call); false-GREEN risk if fn stores the ptr as a side-effect and observes nothing beyond the int32 return.
EVIDENCE: diff_template.js:1070-1080

---

### fmt_global_scan
MECHANISM: Call shape fn(key_ptr) ÔåÆ pointer; harness zeroes shared 16-byte fmtKeyBuf then writes input[0..15]; observes return pointer value as uint32 (0=NULL) ÔÇö does NOT dereference the returned pointer. Same fmtKeyBuf ptr delivered to both sides. CONFIG: input = array of up to 16 u8 bytes. Applies to any fn(16-byte-key-ptr) ÔåÆ ptr doing a lookup into shared global state; false-RED if sides return different-but-equivalent pointers.
EVIDENCE: diff_template.js:1113-1124

---

### fmt_key_compare
MECHANISM: Call shape fn(byte* a, byte* b) ÔåÆ int; harness allocates single 32-byte fkBuf, writes a[0..15] at buf+0 and b[0..15] at buf+16, calls fn(buf, buf+16) for each side; observes signed int return. CONFIG: tests[].a (16 bytes), tests[].b (16 bytes). Same fkBuf ptr across both sides; not re-zeroed between orig/reimpl calls but safe if fn is read-only on its inputs. Applies to any pure 16-byte-key comparator returning ÔêÆ1/0/+1.
EVIDENCE: diff_template.js:1886-1911
