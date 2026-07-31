Now I have all the handler bodies. Let me compile the MECHANISM lines.

---

### track_record_deref
MECHANISM: Zero-arg fn; harness allocates a 0x48-byte fake record (zeroed), writes a u32 sentinel at CONFIG.field_offset, patches CONFIG.record_global_str (default 0x0063d7e4) to point to it, calls fn() with no args, fingerprints return value (CONFIG.is_getter=true) or crash-equality (false), then restores the global; generalises to any no-arg fn that dereferences a single globally-NULL struct pointer provided the record fits in 0x48 bytes.
EVIDENCE: diff_template.js:4021-4077

---

### bgra_encode
MECHANISM: fn(byte* buf) ÔåÆ uint32; harness allocates a single shared 4-byte buffer, writes CONFIG.tests[i]=[b0,b1,b2,b3] as individual bytes before each Orig and Reimpl call (re-seeded between the two), compares the unsigned integer return; same pointer used for both sides; broader than the name: fits any fn(byte*) ÔåÆ uint that reads Ôëñ4 bytes from its sole pointer arg and returns a packed scalar.
EVIDENCE: diff_template.js:1491-1524

---

### float_scalar
MECHANISM: Passes CONFIG.tests[i] as the sole stack argument and returns the raw function return (fn(input)); no buffer allocation, no global seeding, no post-call observation; the thinnest possible wrapper ÔÇö fits any single-scalar-arg fn that returns a scalar, regardless of numeric domain or arg type interpretation.
EVIDENCE: diff_template.js:223-226

---

### fmt_desc_pair_compare
MECHANISM: fn(bufA, bufB [, p3, p4]); allocates two 0x40-byte scratch bufs, populates sparse fields via test.a/{fXX} ÔåÆ u32@offset 0xXX notation, bufs zeroed before each call; fingerprint is (retU32_low16, rolling-polynomial-hash(bufA), rolling-polynomial-hash(bufB)); 2 vs 4 args driven by CONFIG.signature.args.length; crash_equal_ok flag; broadly fits any fn(ptr, ptr) or fn(ptr, ptr, int, int) that mutates either buffer ÔÇö domain-agnostic, field layout fully per-test.
EVIDENCE: diff_template.js:3562-3649

---

### multi_arg_global_write
MECHANISM: fn(p1..pN): void called with input[] as positional u32 args; forces CONFIG.guard_global to 1 before the call; reads back CONFIG.out_count consecutive u32s at CONFIG.out_base as a hex-packed fingerprint; restores both guard and output block afterward; broader than its name: fits any multi-arg void setter that conditionally writes a contiguous memory block when a nonzero guard is present, CONFIG fields: guard_global, out_base, out_count.
EVIDENCE: diff_template.js:789-821

---

### cache_setter_observe
MECHANISM: Per-test scatter-seed ÔåÆ call ÔåÆ scatter-observe; input.seed=[{addr,val}] written pre-call, input.args=[...] (null entry ÔåÆ harness buf) passed to fn, input.obs or CONFIG.obs_globals (array of hex addr strings) read post-call and packed as a hex fingerprint; every seeded and observed global is snapshotted and restored; broader than the name: fits any fn whose observable is scattered non-contiguous globals, not just cache/queue setters; CONFIG: obs_globals; per-test: seed, args, obs.
EVIDENCE: diff_template.js:500-526

---

### count_header_list_ring
MECHANISM: Builds an intrusive count-header ring list with the ORIGINAL Init+PushBack primitives (init_rva_str / pushback_rva_str as NativeFunctions), then exercises CONFIG.list_op ('init'|'pushback'|'find'|'at') on Orig vs Reimpl; observables are address-normalised (count u32, cmp-field s32 values) so per-side allocations compare cleanly; CONFIG: node_link_off, cmp_field_off, object_size; applies to any intrusive ring list sharing this header shape (count@0, sentinel@+4 self-loop).
EVIDENCE: diff_template.js:4833-4926

---

### out3_idx
MECHANISM: fn(buf, uint32_idx): passes the harness scratch buffer as the first positional arg and tests[i] as a u32 second arg; returns the function's return value only ÔÇö buffer contents are never read back or fingerprinted; a reimpl that returns the correct scalar while writing garbage to *buf passes silently; use out1_idx when the written value must also be verified; CONFIG: tests[] list of uint32 indices; no global seeding.
EVIDENCE: diff_template.js:551-555

---

### outbuf_only
MECHANISM: fn(T* out): void; harness allocates two separate out_buf_size-byte bufs (default 16), zeros each before the call, calls fn(buf), reads back as packed-dword fingerprint; CONFIG.fold_ret=true also XORs the return value in (for dual-output getters that both write *out and return a value); CONFIG.seed_global seeds a global with tests[i] before each call (for global-copying getters) and restores it after; broadly fits any fn(ptr) that writes a fixed-size result, regardless of domain.
EVIDENCE: diff_template.js:1979-2024

---

### stub_dispatch_observe
MECHANISM: Arbitrary-arity fn driven by num_bufs scratch buffers (each buf_size bytes) seeded per-test via test.seed=[{buf,off,type,value,stub,ptr_to}]; arg_layout[] maps each call position to a buf-ptr, a scalar from test.scalars[], or the recorder stub; stub_at[] Interceptor-replaces hardcoded direct-CALL targets with the recorder; recorder normalises pointer args to "b<i>+<off>" against scratch-buffer bases so per-side addresses compare equal; fingerprint observes return (observe_ret) and/or call sequence with args (observe_calls); stub_ret is per-test so a pass-through return is also tested; CONFIG: num_bufs, buf_size, arg_layout, stub_at, stub_nargs, stub_abi, stub_ret, observe_ret, observe_calls.
EVIDENCE: diff_template.js:2492-2651
