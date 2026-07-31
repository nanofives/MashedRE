Good, I now have full context for all 11 handlers. Here are the MECHANISM lines:

---

### fastcall_reg
MECHANISM: Drives __fastcall via a patched `mov ecx; mov edx; jmp` trampoline ÔÇö ECX and/or EDX carry scalar or per-test 64-byte zeroed scratch-buffer-pointer args (CONFIG.fastcall_nargs=1|2, CONFIG.fastcall_ecx_ptr, CONFIG.fastcall_edx_ptr); both sides get identical register values; observes return value only (no buffer read-back). Applies to any 1ÔÇô2 register-arg fastcall leaf.
EVIDENCE: re/frida/diff_template.js:3520-3580

---

### float3_scalar_ret
MECHANISM: Passes three float stack args (input[0], input[1], input[2]) to fn and returns its float return value directly; no buffers, no globals, no save/restore, no CONFIG parameterization beyond signature {ret:'float',args:['float','float','float']}. Observes return value only. Applies to any pure f(float,float,float)->float math leaf with no side effects.
EVIDENCE: re/frida/diff_template.js:231-238

---

### font_ctx_float2
MECHANISM: NARROW: hardcodes FontSys_InitRenderState prelude (0x00552c10) and dirty-flag global (0x00912bd8); two float stack args (sx, sy); writes sentinel 0xDEADBEEF to the flag before each call; observes packed (uint32_ret<<16)|dirty_flag_readback; restores flag between sides. No CONFIG parameterization ÔÇö only fits font-ctx float2 fns requiring that specific prelude and dirty flag.
EVIDENCE: re/frida/diff_template.js:1913-1958

---

### int2_ptr2_out
MECHANISM: Two uint32 stack args plus two 4-byte out-pointers into a single harness-allocated 8-byte buffer (cleared before each call pair); void return not observed; fingerprints both output slots as "hi,lo" hex string. No CONFIG parameterization beyond standard tests=[a,b]. Applies to any void fn(uint,uint,uint*,uint*) that writes exactly two 4-byte results via out-pointers.
EVIDENCE: re/frida/diff_template.js:3200-3224

---

### sort_dispatch_out4
MECHANISM: Allocates 16-byte output buffer pre-filled with -1 sentinel; calls fn(out, input.sel, input.dir) void; reads back 4 u32s packed as hex string; observes output buffer only (return value not checked). Relies on quiescent live game globals being identical for both sides; no CONFIG parameterization. Fits void fn(int*,int,int) sort dispatchers that write exactly 4 int indices.
EVIDENCE: re/frida/diff_template.js:872-891

---

### sprite_table_dispatch
MECHANISM: Patches CONFIG.callee_rva_str (default 0x0040bb90) with a NativeCallback recorder via Interceptor.replace; calls fn(slot>>>0) for each test integer; observes the pointer arg the stub captured ÔÇö both null (no-call path) is a valid match; reverts the callee in finally. Parameterised by CONFIG.callee_rva_str and CONFIG.signature; applies to any int-arg dispatcher that routes to one pointer-taking callee.
EVIDENCE: re/frida/diff_template.js:3944-3985

---

### st0_ret_mat3_ptr
MECHANISM: Seeds a 0x30-byte scratch buffer with 9 f32s in 3 rows at stride 0x10 (pads at 0x0c/1c/2c zeroed); calls fn(buf) with signature.ret='double' (required to drain x87 ST0 via libffi FSTP-qword); fingerprints 64-bit double return as 16 hex digits. No CONFIG parameterization. Fits any f(float*)->ST0 leaf reading a 3-row stride-0x10 matrix; does NOT fit a 4-row layout (use st0_ret_mat4x3_ptr).
EVIDENCE: re/frida/diff_template.js:275-315

---

### vec3_global_mul_observe
MECHANISM: Seeds globals[idx*stride+0..8] as f32 from test.vec3 (CONFIG.target_global_base, CONFIG.target_global_stride); calls fn(idx) or fn() per CONFIG.signature.args.length; fingerprints three post-call global u32s and restores originals between sides; return value NOT observed. CONFIG.crash_equal_ok optional. Applies to any in-place vec3 mutation indexed over a configurable stride-based global array.
EVIDENCE: re/frida/diff_template.js:3595-3640

---

### vec3_ptr
MECHANISM: Writes input[0..2] as f32 into a shared scratch buf at offsets 0/4/8; calls fn(buf) and returns fn's return value directly; post-call buffer state is NOT observed ÔÇö false-GREEN hazard if fn writes results back into the pointer. No CONFIG parameterization. Applies to any fn(float*)->scalar where the vec3 buffer is read-only by the callee.
EVIDENCE: re/frida/diff_template.js:363-368

---

### vtable_table_dispatch
MECHANISM: Per-side holder+table buffers; holder[CONFIG.vtbl_ptr_offset]->table; entry[idx*8]=shared NativeCallback stub ptr, entry[idx*8+4]=aux16; calls fn(a1, holder, idx, a4); observes 3 uint32 args captured by the stub ÔÇö direct comparison since both sides call the same stub address. CONFIG: vtbl_ptr_offset (4 or 8), table_entries; tests: {idx, aux16, a1, a4}. Applies to any fn dispatching through a pointer-at-offset table of 8-byte {fnptr, aux16} entries.
EVIDENCE: re/frida/diff_template.js:2480-2541

---

### alloc_check
MECHANISM: Calls fn(size, CONFIG.alloc_tag) for each test size; encodes result as (ptr_align_mod4├ù256)+(ptrÔêÆheader_ptr_before_allocation); returns -1 on null. Observes alignment and header-distance ONLY ÔÇö NOT allocated content or size, so two allocators differing only in fill are a false-GREEN. No CONFIG beyond alloc_tag.
EVIDENCE: re/frida/diff_template.js:1836-1860
