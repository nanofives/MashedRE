I have all the dispatch bodies. Now I'll compose the MECHANISM lines.

---

### thread_desc_init
MECHANISM: Allocates a 20-byte (5-field) scratch buffer, sentinel-fills with 0xDEADBEEF before each call, passes it as first arg with three uint32 scalars (p2, p3, p4) on the stack ÔÇö fn(buf, p2, p3, p4); observes all 5 output uint32 fields as a comma-joined fingerprint; ignores fn return value; no CONFIG beyond `tests`; no globals. NARROW: hardcoded 5-field/20-byte struct with no size CONFIG ÔÇö only fits this exact 4-arg/5-field init layout.
EVIDENCE: diff_template.js:2805-2843

---

### time_diff_decompose
MECHANISM: Passes fn(input[0]|0, input[1]|0, buf+0, buf+4, buf+8, buf+12) ÔÇö 2 int32 stack args plus 4 out-pointers into a single 16-byte harness buffer (sign u32 at +0, min s32 at +4, sec s32 at +8, csec float-bits u32 at +12); void return; observable is comma-joined "sign,min,sec,0x<csec_bits>"; no CONFIG beyond `tests`; no globals. Broadly fits any fn(int, int, u32*, int*, int*, float*) with 4 distinct out-pointers packed into one 16-byte buffer.
EVIDENCE: diff_template.js:387-403

---

### transform_point
MECHANISM: Uses three pre-allocated shared buffers (xformBufs.out 12B, .in 12B, .mat 64B); seeds .in from input.in[0..2] and .mat from input.mat[0..15]; calls fn(out, in, mat) ÔÇö 3 pointer stack args; observes 3 out-float bits as comma-joined string; ignores fn return value; same physical .in/.mat pointers for both Orig and Reimpl (pure read inputs, no aliasing hazard); no CONFIG beyond `tests`; no globals. Both transform_point and transform_vector share this identical dispatch block.
EVIDENCE: diff_template.js:946-958

---

### transform_vector
MECHANISM: Identical dispatch block as transform_point ÔÇö uses pre-allocated shared xformBufs.out/in/mat (12B/12B/64B); seeds .in from input.in[0..2] and .mat from input.mat[0..15]; calls fn(out, in, mat) ÔÇö 3 pointer stack args; observes 3 out-float bits as comma-joined string; ignores fn return value; same physical .in/.mat for both Orig and Reimpl; no CONFIG beyond `tests`; no globals. The two names dispatch to the same code ÔÇö use either for any 3-pointer (vec3-out, vec3-in, 4├ù4-mat) transform function.
EVIDENCE: diff_template.js:946-958

---

### trig_text_draw
MECHANISM: Interceptor.replaces the draw callee at CONFIG.draw_callee_rva_str (default 0x00427ff0) with a capture stub before calling fn(uint32, float, float, uint32, uint32, uint32) ÔÇö 6 stack args from test vector; observable is the 3-tuple (sid, adj_x_bits, adj_y_bits) that the patched draw callee received, NOT any output buffer; reverts replace in finally; CONFIG: `draw_callee_rva_str`, `crash_equal_ok`. Broader: any multi-arg pipeline that terminates in a single capturable callee, provided that callee address is configurable via draw_callee_rva_str.
EVIDENCE: diff_template.js:3870-3925

---

### uint32_scalar
MECHANISM: Passes fn(input>>>0) ÔÇö single uint32 stack arg; returns fn's raw return value; no buffer allocation, no global read-back, no out-pointer observation; no CONFIG beyond `arg_type`. Observes ONLY the return value ÔÇö a false-GREEN hazard for any function whose primary effect is a side-effect or out-pointer write rather than a return value. Mechanically identical to int_scalar; the different name carries no behavioral distinction in the body.
EVIDENCE: diff_template.js:932-934

---

### vec2_normalize
MECHANISM: Uses pre-allocated shared buffers v2nBufs.in (8B) and v2nBufs.out (8B) plus tmpF32 scratch; seeds .in from input[0..1], zeroes .out before each call; calls fn(out, in) ÔåÆ float; captures return float via tmpF32 for bit-exact IEEE-754 comparison; observes [return_magnitude_bits, out[0]_bits, out[1]_bits] as comma-joined; same .in/.out pointers for both Orig and Reimpl; no CONFIG beyond `tests`; no globals. Broadly fits any fn(float*out2, float*in2) ÔåÆ float normalise/length shape.
EVIDENCE: diff_template.js:961-974

---

### vec2_ptr
MECHANISM: Writes input[0]/input[1] as floats into the shared 8-byte `buf`; calls fn(buf) ÔÇö single pointer stack arg; returns fn's raw return value unchanged; does NOT read back buf contents after the call; observes ONLY the return value ÔÇö a false-GREEN hazard for any function that writes its result into the vec2 and returns void or a trivially constant value; no CONFIG beyond `arg_type`. Suitable only for fn(float*vec2) ÔåÆ scalar getters; wrong for any writer-into-buf.
EVIDENCE: diff_template.js:935-939

---

### vec3_lerp
MECHANISM: Allocates separate outA and outB (12B each) for Orig and Reimpl respectively; shares aBuf and bBuf (12B each, written once per test, read-only to callee) between both sides; seeds aBuf/bBuf from input.a[0..2]/input.b[0..2], passes input.t as a float scalar 4th arg ÔÇö fn(out, a, b, t); observes 3 out-float bits as packed hex; no CONFIG beyond `tests`; no globals; deterministic/menu-attach-safe. Broadly fits any pure fn(float*out3, float*a3, float*b3, float) vec3 math leaf.
EVIDENCE: diff_template.js:2734-2763

---

### void_step_global
MECHANISM: NARROW ÔÇö hardcodes three absolute addresses (0x0067e9f8, 0x0067ed74, 0x0067ed40); seeds them from input.raw_bytes (byte array written starting at 0x0067ed74) and input.initial_cursor (written to 0x0067ed40), zeroes DAT_0067e9f8; calls fn(input.step) ÔÇö single int32 stack arg, void return; observable is 0x0067ed40 read as int32 after the call; no CONFIG for any address; applicable only to functions that read exactly this global cursor/limit layout.
EVIDENCE: diff_template.js:626-649

---

### wavefmt_copy
MECHANISM: Allocates 4 independent 16-byte buffers (srcBufA/B, dstBufA/B); for each test {src:[16 bytes], swap}: fills both src bufs identically, zeroes both dst bufs; calls Orig(srcBufA, dstBufA, swap?dstBufA:ptr(0)) and Reimpl(srcBufB, dstBufB, swap?dstBufB:ptr(0)) ÔÇö when swap=1 third arg aliases dst (in-place swap path), when swap=0 third arg is NULL; observes 16-byte dst fingerprint only; fn return value (src_ptr) is discarded and not compared; no CONFIG beyond `tests`. NARROW: fixed 16-byte struct with no CONFIG.struct_size.
EVIDENCE: diff_template.js:1803-1833
