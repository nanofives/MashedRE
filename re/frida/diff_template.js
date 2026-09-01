// Generic A/B diff agent. Python harness injects a CONFIG block at $CONFIG$.
//
// CONFIG schema (JSON):
//   asi_path        string
//   target_rva      string ("0x004c3ac0")
//   export          string
//   signature       { ret: "float"|"uint32"|"pointer"|"void",
//                      args: ["float"|"int"|"pointer"|...] }
//   arg_type        "float_scalar" | "vec3_ptr" | "void" | "int_scalar" |
//                   "int_pair" | "int_ptr2_out"
//   lut_root_delta  number  (0 or 4 — offset for LUT readiness poll)
//   tests           array; shape depends on arg_type:
//                     float_scalar / int_scalar: flat list of values
//                     vec3_ptr:     list of [x,y,z] tuples
//                     int_pair:     list of [a,b] pairs
//                     void:         list of nulls/zeros (length = call count)
//                     int_ptr2_out: flat list of ints (player indices)
//   signature       { ret: "float"|"uint8"|"uint32"|"pointer"|"void",
//                      args: [...] }
//   arg_type        "float_scalar" | "vec3_ptr" | "read_global" | "none"
//                 | "uint32_scalar"      -- call(uint32) -> uint8/uint32
//                 | "bytes_inplace"      -- call(ptr, len) in-place; compare buffer
//                 | "bytes_inplace_3"    -- call(ptr, len, width) in-place; compare buffer
//                 | "alloc_check"        -- call(size, tag) -> ptr; verify alignment+header
//                 | "free_via_alloc"     -- alloc via alloc_rva then free via hook; no-crash
//   lut_root_delta  number  (0 or 4 — offset for LUT readiness poll)
//   tests           array of test vectors (shape depends on arg_type)
//   target_global   string  (hex addr, only for read_global)
//   alloc_tag       number  (only for alloc_check / free_via_alloc)
//   alloc_rva_str   string  (hex addr, only for free_via_alloc)
//   signature       { ret: "float"|"void", args: [...] }
//   arg_type        "float_scalar" | "vec3_ptr" | "vec2_ptr" |
//                   "transform_point" | "transform_vector" |
//                   "vec2_normalize" | "matrix_scale"
//                 | "eax_implicit_ptr"          -- EAX-implicit pointer arg
//                 | "eax_implicit_int"          -- EAX-implicit integer arg
//                 | "fastcall_reg"              -- __fastcall/__thiscall ECX(+EDX) reg-only leaf
//                 | "vec3_global_mul_observe"   -- 3-float global read/mul/write
//                 | "fmt_desc_pair_compare"     -- 2- or 4-arg fmt-desc comparator
//   lut_root_delta  number  (0 or 4 — offset for LUT readiness poll)
//   tests           array whose element shape depends on arg_type
//
// New (feature/harness-arg-types) — extra CONFIG fields:
//   target_global_base    string (hex addr) — used by vec3_global_mul_observe
//   target_global_stride  number (per-index byte stride) — same
//
// Frontend draw-cluster arg_types (added 2026-05-21):
//   draw_quad_observe  — Im2D quad/sprite draw: fingerprint DAT_00898a20
//                        vertex buffer (112B) after orig+reimpl each call.
//                        Optional CONFIG fields: vbuf_addr_str, vbuf_len.
//   out_buf_fmt_2      — 4-arg void with two char* output buffers.
//                        Compares C-string contents joined by '|'.
//                        Optional CONFIG field: out_buf_size (default 32).
//   trig_text_draw     — 6-arg void; Interceptor.replace on draw-callee captures
//                        (sprite_id, adj_x, adj_y) for parity check.
//                        Optional CONFIG field: draw_callee_rva_str
//                        (default '0x00427ff0' / FontText_DrawTextRotated).
//
// Harness-extension arg_types (added 2026-05-22):
//   spin_angle_observe — void(int p1, int p2): resets DAT_0067d974 spin-angle
//                        accumulator to known seed before EACH sub-call; compares
//                        vertex-buffer fingerprint. Unblocks 0x00428450.
//                        Tests: [[p1, p2, angle_seed_float], ...].
//                        Optional: vbuf_addr_str, vbuf_len, angle_global_str.
//   ptr_ptr_entity_set — void(int p1, uint32 p2): double-deref setter where the
//                        write target is *(*target_global + p1*stride + field_offset).
//                        Read-back goes through both derefs. Unblocks 0x0040e480.
//                        Tests: [[p1, p2], ...].
//                        Required CONFIG: target_global (hex addr of outer ptr).
//                        Optional: entity_byte_stride (default 4), field_offset (default 0).
//   track_record_deref — void/uint32(): fakes DAT_0063d7e4 with scratch 0x48B record;
//                        writes sentinel at field_offset; calls fn(); compares return val.
//                        Unblocks 0x0041e9d0, 0x0041ea90, 0x0041e8b0, 0x0041e970.
//                        Tests: flat list of sentinel uint32 values.
//                        Required CONFIG: field_offset (0x14 or 0x44).
//                        Optional: is_getter (default true), record_global_str.
//   audio_sub_struct_zero — void(pointer): allocates sentinel-filled struct buffer;
//                        calls fn(buf); fingerprints the observed byte range.
//                        Unblocks 0x005be190, 0x005be140.
//                        Tests: flat list (length = call count; values ignored).
//                        Required CONFIG: struct_size, observe_offset, observe_length.
//
// Harness-extension arg_types (added 2026-05-22 session C):
//   teardown_call_pair — teardown/shutdown thunks: zero state_global_str before
//                        EACH call (both orig+reimpl) so both crash symmetrically.
//                        Unblocks 0x00493550, 0x00493560, 0x004938c0.
//                        Tests: flat list (length = call count; values ignored).
//                        Required CONFIG: state_global_str (default '0x007d3ff8').
//   large_buffer_save_restore — snapshot+restore a large live-state buffer before
//                        each call pair. Both sides see identical pre-call state.
//                        Unblocks 0x004924f0 harness-side (C3 still needs callees).
//                        Tests: flat list (length = call count; values ignored).
//                        Required CONFIG: buffer_addr, buffer_size_dwords.
//
// Harness-extension arg_types (added 2026-05-22 session B):
//   allocator_nonnull  — fn() -> pointer: allocator that returns fresh heap ptr each
//                        call; pointer-identity comparison is meaningless. Observable:
//                        both sides agree on null (0) vs non-null (1).
//                        Unblocks 0x004c5890 RwTexDictionaryCreate (demoted frida-sweep-q).
//                        Tests: flat list (length = call count; values ignored).
//   resource_loader_4arg — fn(uint16 nameId, LPCSTR type, uint8** outBuf, uint32* outLen).
//                        Calls Win32 FindResourceA+LoadResource+SizeofResource+LockResource.
//                        Observable: (ret & 1) | ((outBuf_nonnull) << 1).
//                        Unblocks 0x004997b0 Win32ResourceLoader.
//                        Tests: list of { name_id, type_str } objects.
//   struct_three_write — void(ptr, uint32, uint32): sentinel-fill scratch buf, call,
//                        read back observe_offsets (default [0x0c, 0x10, 0x14]) as CSV.
//                        Unblocks 0x005be140 FUN_005be140 (3-field leaf writer).
//                        Tests: list of [val_a, val_b] pairs.
//                        Optional CONFIG: struct_size (default 32), observe_offsets.
//   slot_quad_set      — void(int idx, uint32* arr4): writes arr[0..3] to
//                        slot_base_addr + idx * slot_stride + {0,4,8,12}.
//                        Saves/restores live globals; compare as CSV fingerprint.
//                        Unblocks 0x00422ac0 FUN_00422ac0 (4-word per-slot store).
//                        Tests: list of { idx, vals: [v0,v1,v2,v3] } objects.
//                        Optional CONFIG: slot_base_addr (default '0x006412e8'),
//                          slot_stride (default 0xf40), slot_field_count (default 4).
//
// Phase A1 arg_types (added 2026-05-24):
//   int_copy_outbuf    — void(int slot, T* dst): caller-supplied dst buffer
//                        copy. Allocates a 4 KB sentinel-filled (0xCD) scratch
//                        per side; calls fn(slot, buf); reads back the first
//                        CONFIG.out_buf_size bytes (default 24) as a position-
//                        sensitive XOR fingerprint. Unblocks 0x0041f000
//                        TimerSlotDataCopy. Tests: flat list of slot indices.
//                        Optional CONFIG: out_buf_size (default 24).
//   sprite_table_dispatch — void(int slot): sprite-table dispatcher that
//                        looks up a sprite ptr from slot and tail-calls a
//                        downstream callee with that ptr. Interceptor.replace
//                        the callee with a NativeCallback that captures the
//                        first arg; compare captures between Orig+Reimpl.
//                        Unblocks 0x0042fab0 SpriteSlotDispatch and
//                        0x0042e590 SpriteAnimFrameThunk.
//                        Required CONFIG: callee_rva_str (hex addr of callee
//                        to patch; e.g. '0x0040bb90' / '0x0040bb70').
//                        Tests: flat list of slot indices.
//   audio_sub_struct_link — uint32* fn(uint32* p1, uint32 p2): 12-byte sub-
//                        struct field writer (Link Device or Link Buffer).
//                        Allocates a zeroed 12-byte scratch per side; calls
//                        fn(buf, p2); observes (ret-non-null << 24) | low-
//                        24-bit XOR fingerprint of the buf. Unblocks
//                        0x005ae010 / 0x005adfe0. Tests: flat list of p2.
//   audio_sub_struct_dual — uint32 fn(uint32 p1, uint32 p2, uint32 p3):
//                        delegates to LinkDevice + LinkBuffer. Same 12-byte
//                        scratch + fingerprint strategy as the link arg_type.
//                        Unblocks 0x005ac7b0 AudioSubStructDualInit.
//                        Tests: list of {p2, p3} pairs.
//
'use strict';

const CONFIG = $CONFIG$;
const ASI_PATH        = CONFIG.asi_path;
const ASI_MODULE_NAME = CONFIG.asi_module_name || 'mashed_re_dev.asi';
const TARGET_ADDR     = ptr(CONFIG.target_rva);
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

// contact_history buffers — allocated on demand in runDiff
let vehicleBuf = null;
let geomBuf    = null;
// fmt_desc_ptr / fmt_desc_copy / fmt_table_search / fmt_global_scan buffers
let fmtSrcBuf     = null;  // 0x20-byte source fmt-desc
let fmtDstBuf     = null;  // 0x20-byte dest fmt-desc
let fmtCtxBuf     = null;  // 0x30-byte fake audio context
let fmtEntryPtrBuf = null; // 4-byte pointer slot (array[0])
let fmtKeyBuf     = null;  // 16-byte format key
// Output-buffer types — allocated once, reused per call.
let xformBufs = null;   // transform_point / transform_vector: { out, in, mat }
let v2nBufs   = null;   // vec2_normalize:  { out, in }
let v3nBufs   = null;   // vec3_normalize:  { out, in }
let xfdBufs   = null;   // device_transform_dispatch: { out, mat, in }
let matsBufs  = null;   // matrix_scale:    { mat, scale }
let matrBufs  = null;   // matrix_rotate:   { mat, axis }
let mriBufs   = null;   // matrix_rotate_inner: { mat, axis }
let tmpF32    = null;   // 4-byte scratch for float→U32 extraction

function readLutRoot(delta) {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return null;
        const off = LUT_OFFSET_ADDR.readU32();
        const lutPtr = ptr(base + off + delta).readU32();
        if (lutPtr === 0) return null;
        return { base: base, offset: off, root: lutPtr };
    } catch (e) {
        return null;
    }
}

function pollLutThenRun(triesLeft) {
    const lut = readLutRoot(CONFIG.lut_root_delta);
    if (lut !== null) {
        send({ type: 'lut_ready',
               base:     '0x' + lut.base.toString(16),
               offset:   '0x' + lut.offset.toString(16),
               root:     '0x' + lut.root.toString(16),
               delta:    CONFIG.lut_root_delta,
               attempts: 150 - triesLeft });
        runDiff();
        return;
    }
    if (triesLeft <= 0) {
        send({ type: 'error', msg: 'LUT root never populated within 30s (delta=' + CONFIG.lut_root_delta + ')' });
        return;
    }
    setTimeout(function () { pollLutThenRun(triesLeft - 1); }, 200);
}

// Returns a comparable value for the given call.
// For scalar-return types: returns the function's return value (number).
// For output-buffer types: returns a bit-packed hex string from the output buffer.
function callFn(fn, input, buf) {
    // MECHANISM: Zero-arg call - fn() with no args, no seeding, no buffer, no out-pointers;
    // observable is ONLY fn()'s return value; for a true void function both sides return
    // null/undefined and trivially match - FALSE-GREEN hazard for void side-effect-only functions.
    // CONFIG: none. SECOND AND WORSE HAZARD: if the target ACTUALLY TAKES ARGUMENTS this pushes
    // none, so both sides read the SAME leftover stack bytes and agree; non-degeneracy cannot
    // catch it because the garbage is identical between the two sides. x87 args are invisible to
    // the decompiler - confirm arity from the LISTING before using this.
    if (CONFIG.arg_type === 'none') {
        // Zero-arg getter / void invocation. `input` is a dummy iteration
        // marker; we just call the function repeatedly to confirm stable
        // bit-identical output between original and reimpl.
        return fn();
    }
    // MECHANISM: Writes `input>>>0` as u32 into ptr(CONFIG.target_global) then calls fn() with no
    // args; observable is ONLY fn()'s return value; no save/restore of the global, no buffer, no
    // out-pointers. CONFIG: `target_global`. Broader: any zero-arg function whose single input
    // dimension is one writable 32-bit global. SAME HAZARD AS `none`: it pushes NO arguments, so a
    // target that really takes some reads identical leftover stack bytes on both sides and passes.
    // Confirm arity from the LISTING; x87 args do not appear in decompiler output.
    if (CONFIG.arg_type === 'read_global') {
        ptr(CONFIG.target_global).writeU32(input >>> 0);
        return fn();
    }
    // ── Simple scalar types ───────────────────────────────────────────────────
    // MECHANISM: Passes CONFIG.tests[i] as the sole stack argument and returns the raw function
    // return (fn(input)); no buffer allocation, no global seeding, no post-call observation; the
    // thinnest possible wrapper - fits any single-scalar-arg fn that returns a scalar, regardless
    // of numeric domain or arg type interpretation.
    if (CONFIG.arg_type === 'float_scalar') {
        return fn(input);
    }
    // float3_scalar_ret — three scalar float args in, float return: fn(a, b, t).
    // For pure math leaves whose signature is float f(float,float,float) (e.g. the
    // cosine-ease lerp 0x00422440). input is a [a, b, t] triple; registry signature
    // must be {ret:'float', args:['float','float','float']}. The framework reads the
    // float return and fingerprints it as IEEE-754 bits (ret_kind 'float').
    // MECHANISM: Passes three float stack args (input[0], input[1], input[2]) to fn and returns
    // its float return value directly; no buffers, no globals, no save/restore, no CONFIG
    // parameterization beyond signature {ret:'float',args:['float','float','float']}. Observes
    // return value only. Applies to any pure f(float,float,float)->float math leaf with no side
    // effects.
    if (CONFIG.arg_type === 'float3_scalar_ret') {
        return fn(input[0], input[1], input[2]);
    }
    // st0_ret_global — x87 80-bit ST0 float-return leaf gated by one writable input
    // global. Shape: float10 f(void) whose body is `FLD dword[global_a]; FMUL qword
    // [<const>]; <x87 op>; RET`, leaving the result in the x87 ST0 register (extended
    // precision, no stack cleanup). The 3 sine-getters 0x00431b20/b50/b60 are exactly
    // this: FSIN of a 32-bit-float .data global * a fixed .rdata double constant.
    //
    // CRITICAL: signature.ret MUST be 'double' (NEVER 'void'). A void-declared
    // NativeFunction leaves ST0 unpopped -> x87 stack leak / NaN + FPU corruption
    // (feedback memory x87_st0_float10_return_fnptr). 'double' makes libffi FSTP-qword
    // the ST0 return, truncating 80->64 identically for Orig and Reimpl.
    //
    // The gating global sits at 0.0 at menu-idle, so a bare call is degenerate
    // (0*const -> sin(0)=0). We SEED it per-test: input -> *(float*)global_a (32-bit).
    // Both Orig and Reimpl then read the IDENTICAL seeded value (varied across tests ->
    // non-degenerate AND discriminating). The .rdata operand is a read-only constant
    // (NOT writable -> do not seed it; the leaf reads it internally). The global is
    // snapshotted and restored so live game state is untouched.
    //   CONFIG.global_a  hex addr of the writable 32-bit float operand (seeded)
    //   input:           a single number, stored as f32 into global_a
    // Returns a 16-hex-digit fingerprint of the 64-bit double ST0 return (full mantissa
    // -> catches a non-hardware-FSIN reimpl that a 32-bit read would round away).
    // MECHANISM: Zero stack args (fn() with no args); seeds CONFIG.global_a (hex addr of a
    // writable 32-bit float global) with input as f32, snapshots and restores the 4-byte U32
    // around the call; returns a 16-hex-digit fingerprint of the 64-bit ST0 double (full
    // mantissa); observes nothing else - false-GREEN hazard if ST0 return is always 0 for the
    // seeded input; CONFIG.global_a is the only key; fits any no-stack-arg leaf that reads exactly
    // one f32 global and returns its result in ST0.
    if (CONFIG.arg_type === 'st0_ret_global') {
        const ga = ptr(CONFIG.global_a);
        const sa = ga.readU32() >>> 0;
        ga.writeFloat(input);
        let rv;
        try {
            rv = fn();                          // double (ST0), per signature.ret='double'
        } finally {
            ga.writeU32(sa);
        }
        buf.writeDouble(typeof rv === 'number' ? rv : NaN);
        const lo = buf.readU32() >>> 0, hi = buf.add(4).readU32() >>> 0;
        return '0x' + ('00000000' + hi.toString(16)).slice(-8)
                    + ('00000000' + lo.toString(16)).slice(-8);
    }
    // st0_ret_mat3_ptr — x87 80-bit ST0 float-return leaf taking ONE pointer arg to a
    // 3-row / stride-0x10 float matrix. Shape: float10 f(float* m), body reads the nine
    // dwords at {0x00,0x04,0x08, 0x10,0x14,0x18, 0x20,0x24,0x28} with x87 FLD/FMUL and
    // returns the scalar in ST0 (no stack cleanup beyond the MSVC FSTP ST(3)/FSTP ST(0)
    // /FSTP ST(0) discard idiom).
    //
    // Authored for 0x004c4270 and 0x004c42d0 (HARNESS_BACKLOG #1 follow-on). These were
    // MISLABELLED "RwV3d bbox X/Y/Z accessors" in re/analysis/plans/
    // frontier_shape_refinement_2026-07-24.md; the raw bytes disprove that (see the
    // per-RVA analysis note). They are matrix orthonormality residuals.
    //
    // Why the existing handlers do not fit: st0_ret_global takes NO args and seeds
    // globals; vec3_ptr / float_2ptr_ret seed only offsets 0x00/0x04/0x08, leaving
    // 0x10..0x28 as uninitialised heap garbage -> nondeterministic divergence (this is
    // exactly the vec3_ptr MISFIT recorded for 0x004c4270 in re/PROMOTION_QUEUE.md:285).
    //
    // CRITICAL: signature.ret MUST be 'double' (NEVER 'void') — a void-declared
    // NativeFunction leaves ST0 unpopped -> x87 stack leak / NaN + FPU corruption
    // (feedback memory x87_st0_float10_return_fnptr). 'double' makes libffi FSTP-qword
    // the ST0 return, truncating 80->64 identically for Orig and Reimpl.
    //
    //   input: array of 9 numbers -> rows [0..2], [3..5], [6..8] written as f32 to
    //          offsets 0x00/0x04/0x08, 0x10/0x14/0x18, 0x20/0x24/0x28.
    // The three pad dwords at 0x0c/0x1c/0x2c are zeroed for run-to-run determinism; the
    // leaves never read them. Scratch buffer only — no live game state is touched.
    // Returns a 16-hex-digit fingerprint of the 64-bit double ST0 return (full mantissa).
    // MECHANISM: Seeds a 0x30-byte scratch buffer with 9 f32s in 3 rows at stride 0x10 (pads at
    // 0x0c/1c/2c zeroed); calls fn(buf) with signature.ret='double' (required to drain x87 ST0 via
    // libffi FSTP-qword); fingerprints 64-bit double return as 16 hex digits. No CONFIG
    // parameterization. Fits any f(float*)->ST0 leaf reading a 3-row stride-0x10 matrix; does NOT
    // fit a 4-row layout (use st0_ret_mat4x3_ptr).
    if (CONFIG.arg_type === 'st0_ret_mat3_ptr') {
        for (let r = 0; r < 3; r++) {
            const base = r * 0x10;
            buf.add(base + 0x0).writeFloat(input[r * 3 + 0]);
            buf.add(base + 0x4).writeFloat(input[r * 3 + 1]);
            buf.add(base + 0x8).writeFloat(input[r * 3 + 2]);
            buf.add(base + 0xc).writeU32(0);
        }
        const rv = fn(buf);                     // double (ST0), per signature.ret='double'
        const fp = Memory.alloc(8);
        fp.writeDouble(typeof rv === 'number' ? rv : NaN);
        const lo = fp.readU32() >>> 0, hi = fp.add(4).readU32() >>> 0;
        return '0x' + ('00000000' + hi.toString(16)).slice(-8)
                    + ('00000000' + lo.toString(16)).slice(-8);
    }
    // st0_ret_mat4x3_ptr — same family as st0_ret_mat3_ptr, but the leaf reads FOUR rows
    // (stride 0x10), i.e. TWELVE dwords at {0x00,04,08, 0x10,14,18, 0x20,24,28, 0x30,34,38}.
    // The extra row 3 is the RwMatrix TRANSLATION row.
    //
    // Authored for 0x004c4360 (U-9022). Its role was already fixed by the caller
    // FUN_004c4530 = RwMatrixOptimize (U-9021): its return is compared against tolerance
    // slot [2] and gates bit 0x20000 = rwMATRIXINTERNALIDENTITY, so it is the
    // identity-deviation residual. The body confirms that mechanically — it accumulates
    // ||M - I||^2 with the top-left 3x3 measured against the identity diagonal
    // (each diagonal element minus the 1.0f at 0x005cc320) and the translation row
    // measured against zero:
    //   T0 = (m01^2 + m02^2) + (m00-1)^2      0x004c439c..0x004c43c6
    //   T3 = (m30^2 + m31^2) + m32^2          0x004c43c8..0x004c43e2
    //   T2 = (m20^2 + m21^2) + (m22-1)^2      0x004c43e6..0x004c43fc
    //   T1 = (m10^2 + m12^2) + (m11-1)^2      0x004c4400..0x004c4416
    //   return ((T0 + T3) + T2) + T1          0x004c43e4/0x004c43fe/0x004c4418
    //
    // Why st0_ret_mat3_ptr does NOT fit: it allocates 0x30 and seeds only nine floats, so
    // offsets 0x30/0x34/0x38 — which this leaf genuinely reads — would be uninitialised
    // heap garbage and the diff would diverge run-to-run. That is the same MISFIT class
    // recorded for vec3_ptr against 0x004c4270 in re/PROMOTION_QUEUE.md:285.
    //
    // CRITICAL: signature.ret MUST be 'double', NEVER 'void' — a void-declared
    // NativeFunction leaves ST0 unpopped -> x87 stack leak / NaN + FPU corruption
    // (memory x87_st0_float10_return_fnptr). 'double' makes libffi FSTP-qword the ST0
    // return, truncating 80->64 identically for Orig and Reimpl.
    //
    //   input: array of 12 numbers -> rows [0..2],[3..5],[6..8],[9..11] written as f32 to
    //          0x00/04/08, 0x10/14/18, 0x20/24/28, 0x30/34/38.
    // The four pad dwords at 0x0c/0x1c/0x2c/0x3c are zeroed for run-to-run determinism;
    // the leaf never reads them. Scratch buffer only — no live game state is touched.
    // Returns a 16-hex-digit fingerprint of the 64-bit double ST0 return.
    // MECHANISM: fn(float* mat -> ST0 double): packs the test's 12 floats as 4 rows of 3 at stride
    // 0x10 into a scratch buf and zeroes the 4 pad dwords at 0x0c/0x1c/0x2c/0x3c for run-to-run
    // determinism (the leaf never reads them); returns the x87 ST0 value, so signature.ret must be
    // 'double'. Fits any leaf taking a row-padded 4x3 matrix by pointer and returning a float in
    // ST0.
    if (CONFIG.arg_type === 'st0_ret_mat4x3_ptr') {
        for (let r = 0; r < 4; r++) {
            const base = r * 0x10;
            buf.add(base + 0x0).writeFloat(input[r * 3 + 0]);
            buf.add(base + 0x4).writeFloat(input[r * 3 + 1]);
            buf.add(base + 0x8).writeFloat(input[r * 3 + 2]);
            buf.add(base + 0xc).writeU32(0);
        }
        const rv = fn(buf);                     // double (ST0), per signature.ret='double'
        const fp = Memory.alloc(8);
        fp.writeDouble(typeof rv === 'number' ? rv : NaN);
        const lo = fp.readU32() >>> 0, hi = fp.add(4).readU32() >>> 0;
        return '0x' + ('00000000' + hi.toString(16)).slice(-8)
                    + ('00000000' + lo.toString(16)).slice(-8);
    }
    // MECHANISM: Writes input[0..2] as f32 into a shared scratch buf at offsets 0/4/8; calls
    // fn(buf) and returns fn's return value directly; post-call buffer state is NOT observed -
    // false-GREEN hazard if fn writes results back into the pointer. No CONFIG parameterization.
    // Applies to any fn(float*)->scalar where the vec3 buffer is read-only by the callee.
    if (CONFIG.arg_type === 'vec3_ptr') {
        buf.writeFloat(input[0]);
        buf.add(4).writeFloat(input[1]);
        buf.add(8).writeFloat(input[2]);
        return fn(buf);
    }
    // void — no args, no return value of interest
    // MECHANISM: Calls fn() with no args and returns fn()'s raw return value to the comparison
    // framework; no seeding, no buffer, no out-pointer, no observation beyond the return; for a
    // true void return type both sides produce null/undefined and trivially match - FALSE-GREEN
    // hazard. CONFIG: none. NARROW: zero-arg, no observation beyond return.
    if (CONFIG.arg_type === 'void') {
        return fn();
    }
    // int_pair — two uint32 args
    // MECHANISM: Passes two uint32 stack args (input[0], input[1]) directly to fn and returns fn's
    // return value; no buffer, no globals, no save/restore, no CONFIG parameterization beyond
    // `arg_type`. Applies to any two-integer-in, any-return-type function.
    if (CONFIG.arg_type === 'int_pair') {
        return fn(input[0], input[1]);
    }
    // int_ptr2_out — fn(uint32, out_ptr1, out_ptr2); two 4-byte out-slots; returns packed u32
    // MECHANISM: Passes (input, buf, buf+4) to fn via callFn's shared harness `buf`; zeroes both
    // 4-byte out-slots before the call; observable is `(out[0] & 0x3f) | ((out[1] & 0x3f) << 8)` -
    // ONLY the low 6 bits of each output dword. CONFIG: none. NARROW: observable truncates to 6
    // bits per slot; both output pointers are always 4 bytes apart in the same shared 8-byte
    // buffer; no parameterization.
    if (CONFIG.arg_type === 'int_ptr2_out') {
        buf.writeU32(0);
        buf.add(4).writeU32(0);
        fn(input, buf, buf.add(4));
        return (buf.readU32() & 0x3f) | ((buf.add(4).readU32() & 0x3f) << 8);
    }
    // time_diff_decompose — fn(int time_a, int time_b, u32* sign, int* min, int* sec, float* csec).
    // void return; four out-ptrs in a single 16-byte buf. input: [time_a, time_b].
    // Observable: comma-separated fingerprint string of sign|min|sec|csec_bits (IEEE-754 u32).
    // MECHANISM: Passes fn(input[0]|0, input[1]|0, buf+0, buf+4, buf+8, buf+12) - 2 int32 stack
    // args plus 4 out-pointers into a single 16-byte harness buffer (sign u32 at +0, min s32 at
    // +4, sec s32 at +8, csec float-bits u32 at +12); void return; observable is comma-joined
    // "sign,min,sec,0x<csec_bits>"; no CONFIG beyond `tests`; no globals. Broadly fits any fn(int,
    // int, u32*, int*, int*, float*) with 4 distinct out-pointers packed into one 16-byte buffer.
    if (CONFIG.arg_type === 'time_diff_decompose') {
        const ta = input[0] | 0;
        const tb = input[1] | 0;
        buf.writeU32(0);            // sign_out
        buf.add(4).writeS32(0);     // min_out
        buf.add(8).writeS32(0);     // sec_out
        buf.add(12).writeU32(0);    // csec_out (float bits)
        fn(ta, tb, buf, buf.add(4), buf.add(8), buf.add(12));
        const sign = buf.readU32() >>> 0;
        const mn   = buf.add(4).readS32();
        const sc   = buf.add(8).readS32();
        const csec = buf.add(12).readU32() >>> 0;  // raw float bits
        return [sign, mn, sc, '0x' + csec.toString(16)].join(',');
    }
    // int_scalar — single uint32 arg, any integer return type
    // MECHANISM: Passes one uint32 stack arg (`input>>>0`) directly to fn and returns fn's raw
    // return value; no buffer allocation, no global read-back, no save/restore, and no CONFIG
    // parameterization beyond `arg_type`. Applies to any single-integer-in, any-return-type
    // function regardless of name.
    if (CONFIG.arg_type === 'int_scalar') {
        return fn(input >>> 0);
    }
    // ptr_arg_int_get — fn(ptr) -> int, where the single arg is a POINTER the
    // function DEREFERENCES (enrich derefs_arg=1). int_scalar is unsafe here
    // (a random int deref => AV); instead pass a pointer to a 256-byte scratch
    // buffer filled with a per-test deterministic, non-zero pattern so the
    // getter's return VARIES across test vectors (non-degenerate) while Orig and
    // Reimpl see the IDENTICAL buffer (bit-identity holds). `input` is the
    // per-test seed int. Single-level-deref getters (read *(ptr+k)) compare
    // cleanly; a double-deref getter reads a bad inner pointer and faults — that
    // throws in callFn (caught at the call site -> mismatch) and the candidate
    // is left Queued, never falsely GREEN. NOT in SEEDED_ARG_TYPES: we rely on
    // the natural non-degeneracy of a real deref, so a getter that ignores its
    // arg stays trivial and is correctly rejected.
    // MECHANISM: Allocates one SHARED scratch buffer (size `CONFIG.struct_size` or 256), fills it
    // with a deterministic per-seed dword pattern (`seed + o*0x01010101`), passes the SAME
    // physical pointer to both Orig and Reimpl, returns fn's uint32 return; a double-deref
    // function faults on the bad inner pointer (caught as mismatch, never false GREEN). CONFIG:
    // `struct_size`. Broader: any fn(ptr)->int single-level-deref getter, regardless of struct
    // type.
    if (CONFIG.arg_type === 'ptr_arg_int_get') {
        const sz = (CONFIG.struct_size | 0) || 256;
        const seed = (input >>> 0);
        for (let o = 0; o < sz; o += 4)
            buf.add(o).writeU32(((seed + o * 0x01010101) >>> 0));
        const ret = fn(buf);
        return (ret === null || ret === undefined) ? 0
             : (typeof ret === 'object' ? (ret.toInt32() >>> 0) : (ret >>> 0));
    }
    // str_arg_int_get — fn(const char*) -> int, for string-measure leaves whose
    // arg is walked until a NUL. ptr_arg_int_get is UNSAFE for these: it fills
    // its scratch with seeded dwords and never writes a terminator, so a strlen
    // inside the callee runs off the end of the allocation (this project already
    // ate that crash once — FontText 0x427840). Here each test vector is a JS
    // STRING; the harness writes its bytes plus an explicit NUL into a shared
    // 512-byte buffer before each call, so Orig and Reimpl see identical memory
    // and the read is bounded by construction. Distinct string LENGTHS give
    // distinct returns, so non-degeneracy comes free — but the registry comment
    // must still state the per-seed expected values. Same byte-by-byte idiom as
    // cstr_ret_offset below; Memory.allocUtf8String is deliberately unused here.
    // MECHANISM: Calls fn(const_char_ptr) with a per-side 512-byte scratch buffer; input JS string
    // written byte-by-byte with an explicit NUL appended (Memory.allocUtf8String deliberately
    // avoided); observable is fn's return value coerced to uint32; no out-buffer, no globals. No
    // CONFIG beyond arg_type. Applies to any fn(const char*)->integer pure reader regardless of
    // string content or length.
    if (CONFIG.arg_type === 'str_arg_int_get') {
        const sbuf = Memory.alloc(512);
        const s = (typeof input === 'string') ? input : String(input);
        for (let j = 0; j < s.length; j++)
            sbuf.add(j).writeU8(s.charCodeAt(j) & 0xff);
        sbuf.add(s.length).writeU8(0);
        const ret = fn(sbuf);
        return (ret === null || ret === undefined) ? 0
             : (typeof ret === 'object' ? (ret.toInt32() >>> 0) : (ret >>> 0));
    }
    // stricmp_pair — fn(const char* s1, const char* s2) -> int, for string-compare
    // leaves (e.g. RwStricmp 0x004d8680). Two per-side 512-byte scratch buffers, each
    // seeded byte-by-byte with a test JS string plus an explicit NUL (same idiom as
    // str_arg_int_get; Memory.allocUtf8String avoided). A JS `null` for either operand
    // is passed as a real NULL pointer so the callee's null-guard arm is exercised.
    // Observable is fn's signed int return coerced to uint32; no out-buffer, no globals.
    // Non-degeneracy: the registry vectors must span equal / less / greater / prefix /
    // empty / case-fold-boundary (letters vs the 0x40-0x5b edge) / NULL to expose any
    // folding or sign divergence. tests[i] = { s1:"...", s2:"..." } (either may be null).
    if (CONFIG.arg_type === 'stricmp_pair') {
        function seedStr(sval) {
            if (sval === null) return ptr(0);
            const b = Memory.alloc(512);
            const s = String(sval);
            for (let j = 0; j < s.length; j++) b.add(j).writeU8(s.charCodeAt(j) & 0xff);
            b.add(s.length).writeU8(0);
            return b;
        }
        const p1 = seedStr((input && input.s1 !== undefined) ? input.s1 : '');
        const p2 = seedStr((input && input.s2 !== undefined) ? input.s2 : '');
        const ret = fn(p1, p2);
        return (ret === null || ret === undefined) ? 0
             : (typeof ret === 'object' ? (ret.toInt32() >>> 0) : (ret >>> 0));
    }
    // str_inplace_transform — void fn(char* s) that MUTATES the string in place (e.g.
    // RwStrupr 0x004d86d0 / RwStrlwr 0x004d8700). One per-side 512-byte scratch buffer
    // seeded with test.s + explicit NUL; a JS null is passed as a real NULL pointer to
    // exercise the callee null-guard. Return value is void/ignored; the observable is the
    // post-call NUL-terminated buffer bytes (comma-joined u8s over the seeded length),
    // so a reimpl that transforms wrong OR mutates the wrong span is caught. Both sides
    // see identical memory -> bit-identity. Non-degeneracy: vectors must include lower,
    // upper, mixed, the 'A'-1/'Z'+1 and 'a'-1/'z'+1 fold edges, digits/punctuation (must
    // be untouched), a 0x80+ byte (signed range check, untouched), empty, and NULL.
    // tests[i] = { s:"..." } (s may be null).
    if (CONFIG.arg_type === 'str_inplace_transform') {
        const sval = (input && input.s !== undefined) ? input.s : input;
        if (sval === null) { fn(ptr(0)); return 'NULL'; }
        const s = String(sval);
        const b = Memory.alloc(512);
        for (let j = 0; j < s.length; j++) b.add(j).writeU8(s.charCodeAt(j) & 0xff);
        b.add(s.length).writeU8(0);
        fn(b);
        const out = [];
        for (let j = 0; j <= s.length; j++) out.push(b.add(j).readU8());
        return out.join(',');
    }
    // container_find_scalar — fn(container_ptr, int key) -> int32, where
    // container = [dataPtr@+0, count@+4]. The harness FABRICATES the container
    // per test so no live state is touched: input.data ints are written into a
    // fresh scratch array, container[0]=array / container[1]=input.count
    // (defaults to data.length; may be negative to hit verbatim count<=0
    // quirks), then fn(container, input.key) and the int returns are compared.
    // Both sides see the IDENTICAL container, so bit-identity holds. The local
    // arr/cont handles stay live across the call (no cross-closure storage —
    // cf. feedback_frida_keepalive_scratch_buffers, which bit buffers stored
    // for LATER use only as raw ints).
    //   tests[i] = { data:[ints], key:<int> [, count:<int>] }
    // Added 2026-07-29 (methods-efficiency pilot) for 0x004f3cb0.
    // MECHANISM: Builds a harness-only [ptr@+0, count@+4] container backed by a fresh scratch
    // array of input.data ints; calls fn(container, input.key) and observes int32 return;
    // input.count overrides length (may be negative to exercise count<=0 edge cases); both sides
    // share the identical container (read-only); no CONFIG; broader: any fn(container_ptr,
    // int_key) with this 2-field layout; tests[i] = { data:[int...], key [, count] }.
    if (CONFIG.arg_type === 'container_find_scalar') {
        const data = input.data || [];
        const cnt  = (input.count === undefined) ? data.length : input.count;
        const arr  = Memory.alloc(Math.max(4, data.length * 4));
        for (let k = 0; k < data.length; k++) arr.add(k * 4).writeS32(data[k] | 0);
        const cont = Memory.alloc(8);
        cont.writePointer(arr);
        cont.add(4).writeS32(cnt | 0);
        const ret = fn(cont, input.key | 0);
        return (ret !== null && typeof ret === 'object') ? (ret.toInt32() | 0) : (ret | 0);
    }
    // int_with_out_ptr — uint32 arg + 4-byte output buffer; returns function's return value
    // MECHANISM: fn(uint32, out_ptr): passes tests[i] as u32 first arg and shared 8-byte harness
    // buf as second arg (no pre-poison); observable = return value only - buf contents are never
    // read back or fingerprinted; a reimpl returning the correct scalar while writing garbage to
    // *buf passes silently (false-GREEN hazard identical to out3_idx); no CONFIG beyond tests[].
    if (CONFIG.arg_type === 'int_with_out_ptr') {
        return fn(input >>> 0, buf);
    }
    // cache_roundtrip — getter verification by SEEDING the scattered cache
    // global(s) the getter reads, then comparing BOTH the return value AND the
    // out-slot between orig and reimpl. This makes a getter that reads a single
    // constant at idle (degenerate) discriminating: distinct per-test sentinels
    // force distinct out values. The target is a pure read, so seeding is
    // non-destructive (each test snapshots, seeds, calls, reads, restores — and
    // both Orig and Reimpl see the identical seed). SWEEP-CRITICAL: a registry
    // arg_type with no handler here is FATAL (run_diff pre-flight).
    //   tests[i] = { seed:[{addr:'0x..', val:<u32>}], args:[...] }
    //     args entries: a number is passed verbatim; `null` is the out-ptr slot
    //     (replaced by a poisoned 4-byte buf). signature.args must match (the
    //     out-ptr slot is 'pointer'). Fingerprint = "<ret_hex>:<out_hex>".
    // MECHANISM: Scatter-seeds absolute globals (input.seed=[{addr,val}]); poisons harness buf
    // with 0xCCCCCCCC; calls fn(args: null->buf, others->u32); fingerprints `<ret_hex>:<out_hex>`
    // from return value and buf; save/restores all seeded globals; broader: fits any
    // fn(mixed_u32_args + one_out_ptr) requiring pre-seeded globals for non-degenerate
    // discrimination; no CONFIG keys beyond standard; per-test: seed, args.
    if (CONFIG.arg_type === 'cache_roundtrip') {
        const seeds = input.seed || [];
        const saved = seeds.map(s => ptr(s.addr).readU32() >>> 0);
        for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seeds[s].val >>> 0);
        buf.writeU32(0xCCCCCCCC >>> 0);            // poison out-slot: a no-write stays visible
        const callArgs = (input.args || []).map(a => (a === null ? buf : (a >>> 0)));
        let rv = 0;
        try {
            const ret = fn.apply(null, callArgs);
            rv = (ret === null || ret === undefined) ? 0
               : (typeof ret === 'object' ? (ret.toInt32() >>> 0) : (ret >>> 0));
        } finally {
            for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(saved[s] >>> 0);
        }
        const outv = buf.readU32() >>> 0;
        return ('00000000' + rv.toString(16)).slice(-8) + ':' +
               ('00000000' + outv.toString(16)).slice(-8);
    }
    // cache_setter_observe — setter / flush verification: seed input globals
    // (current cache polarity, gate flags, the dirty-queue counter, queue-slot
    // poison), call fn(...args), then read a list of SCATTERED output globals as
    // a packed fingerprint. Snapshots + restores every touched global so the
    // diff is non-destructive to the live game. Distinct seeded scenarios force
    // distinct queue/store/cache fingerprints (non-degenerate). SWEEP-CRITICAL.
    //   tests[i] = { seed:[{addr:'0x..', val:<u32>}], args:[...], obs:['0x..', ...] }
    //     obs may be omitted to fall back to CONFIG.obs_globals (array of hex
    //     strings). A `null` in args is the out-ptr slot (replaced by buf).
    // MECHANISM: Per-test scatter-seed -> call -> scatter-observe; input.seed=[{addr,val}] written
    // pre-call, input.args=[...] (null entry -> harness buf) passed to fn, input.obs or
    // CONFIG.obs_globals (array of hex addr strings) read post-call and packed as a hex
    // fingerprint; every seeded and observed global is snapshotted and restored; broader than the
    // name: fits any fn whose observable is scattered non-contiguous globals, not just cache/queue
    // setters; CONFIG: obs_globals; per-test: seed, args, obs.
    if (CONFIG.arg_type === 'cache_setter_observe') {
        const seeds = input.seed || [];
        const obs   = input.obs || CONFIG.obs_globals || [];
        const seedSaved = seeds.map(s => ptr(s.addr).readU32() >>> 0);
        const obsSaved  = obs.map(a => ptr(a).readU32() >>> 0);
        for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seeds[s].val >>> 0);
        const callArgs = (input.args || []).map(a => (a === null ? buf : (a >>> 0)));
        let result = '';
        try {
            fn.apply(null, callArgs);
            for (let o = 0; o < obs.length; o++)
                result += ('00000000' + (ptr(obs[o]).readU32() >>> 0).toString(16)).slice(-8);
        } finally {
            for (let o = 0; o < obs.length; o++) ptr(obs[o]).writeU32(obsSaved[o] >>> 0);
            for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seedSaved[s] >>> 0);
        }
        return '0x' + (result || '0');
    }
    // seed_globals_fold_ret — scalar-return function GATED ON READ-ONLY globals.
    // Per-test seed a scattered set of globals ({addr,val}), call fn(...args), and
    // FOLD (compare) the RETURN VALUE; snapshot+restore every seeded global so the
    // diff is non-destructive to live state. This is cache_setter_observe with the
    // observable moved from scattered OUTPUT globals to the return (no obs list) —
    // for functions that READ globals to compute a scalar rather than writing them.
    // Distinct seeds/args MUST produce distinct returns (non-degenerate) or it is a
    // false green ([[scratch-field-false-green]]). SWEEP-CRITICAL: paired handler in
    // verify_hook_install_template.js (both must ride the sweep together).
    //   tests[i] = { seed:[{addr:'0x..', val:<u32>}], args:[<u32>, ...] }
    // (area-frontend r6, for 0x00430670 clean path DAT_0067e9fc==10.)
    if (CONFIG.arg_type === 'seed_globals_fold_ret') {
        const seeds = input.seed || [];
        const seedSaved = seeds.map(s => ptr(s.addr).readU32() >>> 0);
        for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seeds[s].val >>> 0);
        const callArgs = (input.args || []).map(a => (a >>> 0));
        let ret;
        try {
            const r = fn.apply(null, callArgs);
            ret = (r === null || r === undefined) ? 0
                : (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0)
                : (r >>> 0);
        } finally {
            for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seedSaved[s] >>> 0);
        }
        return '0x' + ('00000000' + ret.toString(16)).slice(-8);
    }
    // write_global_call_int0 — write sentinel to target_global, call fn(0), return value
    // Use for getters where non-trivial domain requires injecting known values.
    // MECHANISM: Seeds `CONFIG.target_global` with `input>>>0` (uint32), calls `fn(0)` - one stack
    // arg always literal 0, no out-pointers, no buffers - and returns fn's return value as the
    // sole observable; no post-call global reads. CONFIG: `target_global` only. Broader than name:
    // any fn whose only arg must be 0 and whose result depends on one injectable global.
    if (CONFIG.arg_type === 'write_global_call_int0') {
        ptr(CONFIG.target_global).writeU32(input >>> 0);
        return fn(0);
    }
    // void_setter_observe — call fn(input) [void return], then read target_global and return it.
    // Use for void(uint32) setters that write param_1 directly to a global.
    // Strategy: call fn(value), read back target_global. Both orig and reimpl must
    // have written `value` to target_global.
    // MECHANISM: Calls fn(input>>>0) [void return], then reads `CONFIG.target_global` as uint32
    // observable; no buffer allocation, no save/restore. CONFIG: `target_global` only. Broader
    // than name: any void(uint32) that writes its param to one absolute global - not limited to
    // simple direct-assign setters.
    if (CONFIG.arg_type === 'void_setter_observe') {
        fn(input >>> 0);
        return ptr(CONFIG.target_global).readU32();
    }
    // contact_history — set up slot 0 of a fake vehicle contact table, call fn(geom, vehicle)
    // input: { slot_contact_id, slot_active, geom_contact_id }
    // MECHANISM: Seeds outer-scope shared buffers vehicleBuf (0xC80 b) / geomBuf (0x40 b):
    // vehicleBuf+0xBFC=slot_contact_id, +0xC7C=slot_active; geomBuf+0x34=geom_contact_id; calls
    // fn(geomBuf, vehicleBuf) and observes return value only; NARROW: offsets hardcoded; buffer
    // mutations are never read back - a reimpl returning the correct scalar while miswriting the
    // struct passes silently; CONFIG: none; tests[i] = { slot_contact_id, slot_active,
    // geom_contact_id }.
    if (CONFIG.arg_type === 'contact_history') {
        vehicleBuf.add(0xBFC).writeU32(0);
        vehicleBuf.add(0xC7C).writeU32(0);
        vehicleBuf.add(0xBFC).writeU32(input.slot_contact_id >>> 0);
        vehicleBuf.add(0xC7C).writeU32(input.slot_active ? 1 : 0);
        geomBuf.add(0x34).writeU32(input.geom_contact_id >>> 0);
        return fn(geomBuf.toInt32(), vehicleBuf.toInt32());
    }
    // out3_idx — fn(out_buf_ptr, uint32_idx); buf is first arg (12 bytes); returns fn return value.
    // Used for functions like VehicleVec3At9C8Get where output buffer precedes the index arg.
    // MECHANISM: fn(buf, uint32_idx): passes the harness scratch buffer as the first positional
    // arg and tests[i] as a u32 second arg; returns the function's return value only - buffer
    // contents are never read back or fingerprinted; a reimpl that returns the correct scalar
    // while writing garbage to *buf passes silently; use out1_idx when the written value must also
    // be verified; CONFIG: tests[] list of uint32 indices; no global seeding.
    // RETIRED orch-iter21 2026-07-31. Do NOT reintroduce.
    //
    // The body was `return fn(buf, input >>> 0);` - it passed a scratch buffer and
    // compared the RETURN VALUE ALONE, never reading the buffer back, never
    // fingerprinting it, never even poisoning it. All four rows that used it turned
    // out to be bounds-checked vec3 accessors returning a CONSTANT 1 in range and 0
    // out of range, so the return carried zero information about the data moved and
    // a reimpl whose entire body was `return idx<16 ? 1 : 0` passed 9/9 GREEN on
    // every one of them. Audit: re/analysis/out3_idx_false_green_audit_20260731.md.
    //
    // All four were re-verified under handlers that observe the payload:
    // ptr_out_table_get for the getters, cache_setter_observe for the setter.
    // There is NO call shape for which out3_idx is the right choice - use
    // ptr_out_table_get (observes out[0..span-1] + return), out1_idx (one written
    // dword + return), or idx_out2. This stub stays so the name resolves and fails
    // LOUDLY rather than silently reverting to the old behaviour if an old entry or
    // an old brief resurfaces.
    if (CONFIG.arg_type === 'out3_idx') {
        send({ type: 'error', msg: 'arg_type out3_idx is RETIRED (false-GREEN: it never ' +
              'observes the out buffer). Use ptr_out_table_get / out1_idx / idx_out2 - see ' +
              're/analysis/out3_idx_false_green_audit_20260731.md' });
        throw new Error('out3_idx is retired');
    }
    // out1_idx — fn(out_ptr, uint32_idx) -> int. Same argument order as out3_idx,
    // but the fingerprint INCLUDES THE WRITTEN DWORD, not just the return value.
    // out3_idx returns fn's result alone, so for a bounds-checked writer it only
    // ever observes the 0/1 flag — a port that returns the right flag while
    // writing garbage to *out passes it. The out-slot is poisoned to 0xCCCCCCCC
    // first, so "wrote nothing" is visible rather than reading as a stale match.
    // Fingerprint: "<ret_hex>:<out_hex>".
    // MECHANISM: fn(buf, uint32_idx)->scalar; poisons buf[0] with 0xCCCCCCCC, calls fn(buf,
    // input>>>0), fingerprints as "<ret_hex>:<written_dword_hex>"; observes BOTH the return value
    // and the dword written at buf+0; CONFIG: tests[] of uint32 indices; broader than out3_idx - a
    // reimpl that returns correct flag but writes garbage fails here; only one dword of buf is
    // read back.
    if (CONFIG.arg_type === 'out1_idx') {
        buf.writeU32(0xCCCCCCCC >>> 0);
        const ret = fn(buf, input >>> 0);
        const rv = (ret === null || ret === undefined) ? 0
                 : (typeof ret === 'object' ? (ret.toInt32() >>> 0) : (ret >>> 0));
        return ('00000000' + rv.toString(16)).slice(-8) + ':' +
               ('00000000' + (buf.readU32() >>> 0).toString(16)).slice(-8);
    }
    // idx_out2 — fn(uint32_idx, out_ptr1, out_ptr2); two 4-byte out-slots in shared buf.
    // Returns fn return value. Used for functions like VehicleCarStateRead.
    // MECHANISM: fn(uint32_idx, out_ptr1, out_ptr2): passes tests[i] as u32, shared 8-byte harness
    // buf as out_ptr1, buf+4 as out_ptr2; observable = return value only - neither out-slot is
    // ever read back or fingerprinted; a reimpl returning the correct scalar while writing garbage
    // to both outs passes silently (false-GREEN hazard identical to out3_idx); no CONFIG beyond
    // tests[].
    if (CONFIG.arg_type === 'idx_out2') {
        return fn(input >>> 0, buf, buf.add(4));
    }
    // sentinel_array_ptr — original is __fastcall(ECX=0, EDX=ptr); reimpl is __cdecl(0, ptr).
    // input: array of int32 values terminated by 0xFF070000 (e.g. [0xFF060000, 0xFF070000]).
    // Writes the array into buf. For orig, calls via a hand-written thunk that sets ECX=0
    // and EDX=buf before jumping to the target. For reimpl, calls as cdecl(0, buf).
    // Used for MenuGroupCount (0x0042ac00).
    // MECHANISM: fn(int32[] -> one scratch buf): writes the test's int32 array into a shared
    // scratch buffer and passes it as the first positional arg; the ORIGINAL is reached through a
    // hand-written thunk setting ECX=0 and EDX=buf (fastcall) while the reimpl is called __cdecl,
    // so it compares BEHAVIOUR, not ABI. Observable is the return value. Fits any fn taking a
    // caller-owned int32 array by pointer, whatever the register convention.
    if (CONFIG.arg_type === 'sentinel_array_ptr') {
        const arr = input;  // array of int32 values
        for (let k = 0; k < arr.length; k++) {
            buf.add(k * 4).writeS32(arr[k] | 0);
        }
        // The fn wrapper is invoked via callFn(Orig/Reimpl, ...).
        // For orig (fastcall): fn is the origFastcallThunk (set up in runDiff).
        // For reimpl (cdecl): fn is the NativeFunction directly.
        // We pass the buf as first positional arg; the calling convention decides
        // how it's delivered. For cdecl reimpl, fn(0, buf) is correct.
        // For orig, we use the thunk stored at origThunk.
        return fn(0, buf);
    }
    // void_step_global — fn(int_step); void return; reads cursor side-effect from global.
    // Preps game globals for slot 0 (slotOff40=0, slotOff10=0):
    //   input.raw_bytes: flat byte array written starting at 0x0067ed74.
    //     Bytes [0..3] form the LE int32 limit field at 0x0067ed74.
    //     Bytes [N] at offset N are the validity byte for cursor value N
    //     (since validity is read at 0x0067ed74 + cursor for slot 0).
    //   input.initial_cursor: written to 0x0067ed40 before the call.
    //   input.step: passed as param_1 to fn.
    // Writes DAT_0067e9f8=0 (slot 0). Calls fn(step). Returns cursor at
    // 0x0067ed40 after the call (as int32, compared between orig/reimpl).
    // Used for MenuCursorStep (0x0042aa00).
    // MECHANISM: NARROW - hardcodes three absolute addresses (0x0067e9f8, 0x0067ed74, 0x0067ed40);
    // seeds them from input.raw_bytes (byte array written starting at 0x0067ed74) and
    // input.initial_cursor (written to 0x0067ed40), zeroes DAT_0067e9f8; calls fn(input.step) -
    // single int32 stack arg, void return; observable is 0x0067ed40 read as int32 after the call;
    // no CONFIG for any address; applicable only to functions that read exactly this global
    // cursor/limit layout.
    if (CONFIG.arg_type === 'void_step_global') {
        const raw  = input.raw_bytes;
        const init = input.initial_cursor;
        const step = input.step;
        // slot 0
        ptr(0x0067e9f8).writeS32(0);
        for (let k = 0; k < raw.length; k++) {
            ptr(0x0067ed74 + k).writeU8(raw[k] >>> 0);
        }
        ptr(0x0067ed40).writeS32(init);
        fn(step);
        return ptr(0x0067ed40).readS32();
    }
    // entity_field_set — fn(int param_1, uint32 param_2): void write to global array.
    // input: [param_1, param_2].  Calls fn, then reads back the written address as uint32.
    // Address: CONFIG.target_global + param_1 * CONFIG.entity_byte_stride.
    // MECHANISM: Calls fn(input[0]|0, input[1]>>>0) [void], reads `CONFIG.target_global + input[0]
    // * CONFIG.entity_byte_stride` as uint32 observable; no buffer, no save/restore (assumes
    // idempotent slot write). CONFIG: `target_global`, `entity_byte_stride`. Broader: any
    // fn(int_index, uint32_value) that writes into a strided global array - both the base and
    // stride are fully configurable.
    if (CONFIG.arg_type === 'entity_field_set') {
        const p1 = input[0] | 0;
        const p2 = input[1] >>> 0;
        fn(p1, p2);
        const addr = ptr(CONFIG.target_global).add(p1 * CONFIG.entity_byte_stride);
        return addr.readU32();
    }
    // entity_field_add — fn(int idx, int delta): non-idempotent incrementer.
    // input: [idx, delta].  Address: CONFIG.target_global + idx*CONFIG.entity_byte_stride.
    // Unlike entity_field_set (idempotent absolute write), the body does
    // *(int*)addr += delta, so the back-to-back Orig/Reimpl A/B loop would see
    // the residue of the first call. Snapshot the field, call fn(idx,delta),
    // pack (return_value, post-add field) into a fingerprint, then RESTORE the
    // field so Orig and Reimpl each start from the identical baseline.
    // MECHANISM: fn(int idx, int delta) on a live global array; computes field address as
    // ptr(CONFIG.target_global) + idx x CONFIG.entity_byte_stride; snapshots the 4-byte field
    // before the call and restores it afterward so both Orig and Reimpl start from the same
    // baseline (prevents accumulation); observes both the uint32 return value AND the post-add
    // field value packed as "ret_hex:field_hex"; CONFIG.max_index (default 0xf) guards out-of-
    // range idx - no live write occurs, returns 0:0; CONFIG keys: target_global,
    // entity_byte_stride, max_index; fits any indexed-array incrementer with configurable base and
    // stride.
    if (CONFIG.arg_type === 'entity_field_add') {
        const idx   = input[0] | 0;
        const delta = input[1] | 0;
        const addr  = ptr(CONFIG.target_global).add(idx * CONFIG.entity_byte_stride);
        // For out-of-range idx (guard returns 0 with no write), readback is
        // still safe because the function never touches addr.
        const guarded = idx > (CONFIG.max_index !== undefined ? CONFIG.max_index : 0xf);
        const saved = guarded ? 0 : addr.readU32();
        const ret = fn(idx, delta) >>> 0;
        const field = guarded ? 0 : (addr.readU32() >>> 0);
        if (!guarded) addr.writeU32(saved);  // restore baseline
        // Fingerprint: ret in high 8 bits region + low field bits.
        return ('00000000' + ret.toString(16)).slice(-8) + ':' +
               ('00000000' + field.toString(16)).slice(-8);
    }
    // cursor_back — fn(void): complex void cursor-nav function.
    // input: { row, col, flag, mp_flag } — initial global state to inject before calling.
    // Sets DAT_0067f17c/DAT_0067f184/DAT_0067f1a4/DAT_0067ea68, calls fn, reads back
    // DAT_0067f17c and DAT_0067f184 as observable output packed into a uint32.
    // Also saves/restores DAT_0067e9fc (written by callee FUN_0042f6b0) so it doesn't
    // leak between orig and reimpl calls.
    // MECHANISM: NARROW: hardcodes 5 absolute globals (row=0x67f17c, col=0x67f184, flag=0x67f1a4,
    // mp_flag=0x67ea68, game_mode=0x67e9fc). Call shape fn(void); seeds 4 globals from input
    // {row,col,flag,mp_flag}; observes post-call row/col packed as (row&0xffff)<<16|(col&0xffff);
    // saves/restores all 5 globals. CONFIG: input.row/col/flag/mp_flag only.
    if (CONFIG.arg_type === 'cursor_back') {
        const pRow  = ptr('0x0067f17c');
        const pCol  = ptr('0x0067f184');
        const pFlag = ptr('0x0067f1a4');
        const pMode = ptr('0x0067ea68');
        const pGM   = ptr('0x0067e9fc');  // game-mode global written by FUN_0042f6b0
        // Save originals.
        const savedRow  = pRow.readS32();
        const savedCol  = pCol.readS32();
        const savedFlag = pFlag.readS32();
        const savedMode = pMode.readS32();
        const savedGM   = pGM.readS32();
        // Inject test state.
        pRow.writeS32(input.row     | 0);
        pCol.writeS32(input.col     | 0);
        pFlag.writeS32(input.flag   | 0);
        pMode.writeS32(input.mp_flag | 0);
        // Call (void — no return value used).
        fn();
        // Read back row and col as the observable result.
        const outRow = pRow.readS32();
        const outCol = pCol.readS32();
        // Restore all globals (including game-mode).
        pRow.writeS32(savedRow);
        pCol.writeS32(savedCol);
        pFlag.writeS32(savedFlag);
        pMode.writeS32(savedMode);
        pGM.writeS32(savedGM);
        // Pack outRow and outCol into a comparable uint32 (low 16 bits each).
        return ((outRow & 0xffff) * 0x10000 + (outCol & 0xffff)) >>> 0;
    }
    // ptr_nonnull_check — fn(void) -> pointer.
    // Compares null/non-null status of the returned pointer: both sides must
    // agree on null (0) or non-null (1).  Used for functions that return freshly
    // allocated memory at non-deterministic addresses (e.g. ___crtGetEnvironmentStringsA)
    // where pointer equality is meaningless but null/non-null is the observable.
    // `input` is an optional mode-flag value to pre-write to CONFIG.target_global
    // before each call (resets cached state so the function re-runs its detection).
    // MECHANISM: fn(void)->pointer; optionally writes input>>>0 to CONFIG.target_global (any hex
    // address) before each call to reset cached state; observable is null-vs-non-null only
    // (returns 0 or 1) - actual pointer address is never compared; CONFIG: target_global
    // (optional); broader: any no-arg function returning a non-deterministic pointer where only
    // NULL/non-NULL distinguishes correctness.
    if (CONFIG.arg_type === 'ptr_nonnull_check') {
        if (CONFIG.target_global) {
            ptr(CONFIG.target_global).writeU32(input >>> 0);
        }
        const p = fn();
        return (p && !p.isNull()) ? 1 : 0;
    }

    // slot_block_zero — fn(int slot): void(int) that zeroes/initializes a
    // per-slot block in a global array. The first dword of the block is the
    // observable: pre-write a sentinel value, call fn(slot), read back the
    // first dword. Bit-identical reimpl must produce the same post-call value
    // (typically 0 for memset-class functions).
    //
    // CONFIG fields:
    //   target_global       — hex address of array base (e.g. 0x006403e8)
    //   entity_byte_stride  — bytes per slot (e.g. 0xf40)
    //   sentinel_value      — optional uint32 pre-write (default 0xDEADBEEF)
    // MECHANISM: Passes integer slot to fn(slot); computes base = ptr(CONFIG.target_global) + slot
    // * CONFIG.entity_byte_stride; pre-writes CONFIG.sentinel_value (default 0xDEADBEEF) to
    // base[0]; calls fn; observable is base[0] post-call (typically 0 for a memset-style
    // initializer); saves/restores original dword; fn return value is NOT compared. CONFIG:
    // `target_global`, `entity_byte_stride`, `sentinel_value`.
    if (CONFIG.arg_type === 'slot_block_zero') {
        const slot = input | 0;
        const sentinel = (CONFIG.sentinel_value !== undefined ? CONFIG.sentinel_value : 0xDEADBEEF) >>> 0;
        const base = ptr(CONFIG.target_global).add(slot * CONFIG.entity_byte_stride);
        // Save the original first dword so the test is non-destructive.
        const saved = base.readU32();
        base.writeU32(sentinel);
        fn(slot);
        const result = base.readU32();
        base.writeU32(saved);
        return result;
    }

    // state_machine_observe — fn(void): void() that mutates one or more
    // globals based on the current state of other globals. Inject test
    // values into CONFIG.input_globals, call fn(), read back
    // CONFIG.output_globals as the observable.
    //
    // Save/restore semantics: every global (input + output) is snapshotted
    // before injection and restored after readback, so the test is
    // non-destructive across runs.
    //
    // CONFIG fields:
    //   input_globals  — array of {addr: '0x...', type: 'u8'|'u16'|'u32'|'s8'|'s16'|'s32'}
    //   output_globals — array of {addr: '0x...', type: ...}
    //   input — array of values matching input_globals (or scalar if 1 global)
    //
    // Returns a hex string packing all output globals (32 bits each) so
    // BigInt-sized observables don't lose precision through JSON.
    // MECHANISM: Saves all (input  +  output) globals, writes test values into
    // `CONFIG.input_globals` (array of {addr,type}), calls fn() [void, no args], reads
    // `CONFIG.output_globals` as packed hex fingerprint, then restores everything; per-entry
    // types: u8/u16/u32/s8/s16/s32. CONFIG: `input_globals`, `output_globals`. Broader: any void()
    // function whose entire observable is global-to-global mutation; input and output global sets
    // are fully independent.
    if (CONFIG.arg_type === 'state_machine_observe') {
        const inputs  = CONFIG.input_globals  || [];
        const outputs = CONFIG.output_globals || [];
        const reader = function (p, type) {
            switch (type || 'u32') {
                case 'u8':  return p.readU8();
                case 'u16': return p.readU16();
                case 'u32': return p.readU32();
                case 's8':  return p.readS8();
                case 's16': return p.readS16();
                case 's32': return p.readS32();
                default: return p.readU32();
            }
        };
        const writer = function (p, v, type) {
            switch (type || 'u32') {
                case 'u8':  p.writeU8(v & 0xff); break;
                case 'u16': p.writeU16(v & 0xffff); break;
                case 'u32': p.writeU32(v >>> 0); break;
                case 's8':  p.writeS8((v << 24) >> 24); break;
                case 's16': p.writeS16((v << 16) >> 16); break;
                case 's32': p.writeS32(v | 0); break;
                default:    p.writeU32(v >>> 0);
            }
        };
        // Save all originals (inputs + outputs).
        const all = inputs.concat(outputs);
        const saved = all.map(g => reader(ptr(g.addr), g.type));
        // Inject inputs. `input` is either a scalar (when 1 input global) or
        // an array of values in input_globals order.
        const values = Array.isArray(input) ? input : [input];
        inputs.forEach((g, i) => {
            const v = values[i] !== undefined ? values[i] : 0;
            writer(ptr(g.addr), v, g.type);
        });
        // Call (void).
        fn();
        // Read outputs.
        let result = '';
        outputs.forEach(g => {
            const v = reader(ptr(g.addr), g.type) >>> 0;
            result += ('00000000' + v.toString(16)).slice(-8);
        });
        // Restore everything.
        all.forEach((g, i) => writer(ptr(g.addr), saved[i], g.type));
        return '0x' + (result || '0');
    }

    // multi_arg_global_write — fn(p1..pN): void that writes N params into a
    // contiguous u32 globals block (+ optional flag), gated by a guard global.
    // Set guard != 0 so the write path is taken (not the alt-init callee path),
    // call fn(...params), read back out_count u32s at out_base, restore.
    //
    // CONFIG fields:
    //   guard_global  — hex addr of the guard (set to 1 before the call)
    //   out_base      — hex addr of the first written global
    //   out_count     — number of consecutive u32 slots to read back
    //   input         — array of N param values to pass
    // MECHANISM: fn(p1..pN): void called with input[] as positional u32 args; forces
    // CONFIG.guard_global to 1 before the call; reads back CONFIG.out_count consecutive u32s at
    // CONFIG.out_base as a hex-packed fingerprint; restores both guard and output block afterward;
    // broader than its name: fits any multi-arg void setter that conditionally writes a contiguous
    // memory block when a nonzero guard is present, CONFIG fields: guard_global, out_base,
    // out_count.
    if (CONFIG.arg_type === 'multi_arg_global_write') {
        const guard    = ptr(CONFIG.guard_global);
        const outBase  = ptr(CONFIG.out_base);
        const outCount = CONFIG.out_count | 0;
        const params   = Array.isArray(input) ? input : [input];
        // Save guard + the output block.
        const savedGuard = guard.readU32();
        const savedOut = [];
        for (let i = 0; i < outCount; i++) savedOut.push(outBase.add(i * 4).readU32());
        // Force the write path.
        guard.writeU32(1);
        fn.apply(null, params.map(v => v >>> 0));
        // Read back the written block.
        let result = '';
        for (let i = 0; i < outCount; i++) {
            const v = outBase.add(i * 4).readU32() >>> 0;
            result += ('00000000' + v.toString(16)).slice(-8);
        }
        // Restore.
        guard.writeU32(savedGuard);
        for (let i = 0; i < outCount; i++) outBase.add(i * 4).writeU32(savedOut[i]);
        return '0x' + result;
    }

    // sort_dispatch_out4 — fn(int* out, int sel, int dir): void sort dispatcher
    // that writes 4 sorted player indices into the out buffer based on live
    // player state. Alloc a 4-int out buffer, call fn(out, sel, dir), read back
    // the 4 ints packed as a hex string. Both orig and reimpl read the same
    // live globals at quiescent menu → bit-identical sorted output.
    //
    // input: {sel, dir}
    // MECHANISM: Allocates 16-byte output buffer pre-filled with -1 sentinel; calls fn(out,
    // input.sel, input.dir) void; reads back 4 u32s packed as hex string; observes output buffer
    // only (return value not checked). Relies on quiescent live game globals being identical for
    // both sides; no CONFIG parameterization. Fits void fn(int*,int,int) sort dispatchers that
    // write exactly 4 int indices.
    if (CONFIG.arg_type === 'sort_dispatch_out4') {
        const out = Memory.alloc(16);
        // Pre-fill with a sentinel so an under-write is detectable.
        for (let i = 0; i < 4; i++) out.add(i * 4).writeS32(-1);
        const sel = input.sel | 0;
        const dir = input.dir | 0;
        fn(out, sel, dir);
        let result = '';
        for (let i = 0; i < 4; i++) {
            const v = out.add(i * 4).readU32() >>> 0;
            result += ('00000000' + v.toString(16)).slice(-8);
        }
        return '0x' + result;
    }

    // car_slot_init — fn(int param_1): conditional void struct initialiser.
    // input: { idx, guard_val } — param_1 = idx; guard field at 0x7f105c+idx*0x4c is set to guard_val.
    // Calls fn(idx), then reads back the 4 fields (offsets +0, +0xC, +0x10, +0x14) packed as
    // a 32-bit composite (low 8 bits of each, shifted).  Guard field is restored after.
    // MECHANISM: NARROW - hardcoded to global array at 0x7f1058, stride 0x4c: saves/zeros fields
    // at offsets +0/+0xC/+0x10/+0x14, sets guard at 0x7f105c+idx*0x4c = input.guard_val, calls
    // fn(idx), packs low bytes of 4 fields as uint32 fingerprint, restores all; CONFIG: none;
    // tests[i] = { idx:int, guard_val:uint }.
    if (CONFIG.arg_type === 'car_slot_init') {
        const idx   = input.idx | 0;
        const guard = input.guard_val >>> 0;
        const stride = 0x4c;
        const base   = ptr('0x7f1058');
        const pGuard = ptr('0x7f105c').add(idx * stride);
        const pF0    = base.add(idx * stride);
        const pFC    = base.add(idx * stride + 0x0c);
        const pF10   = base.add(idx * stride + 0x10);
        const pF14   = base.add(idx * stride + 0x14);
        // Save struct state.
        const sGuard = pGuard.readU32();
        const sF0    = pF0.readU32();
        const sFC    = pFC.readU32();
        const sF10   = pF10.readU32();
        const sF14   = pF14.readU32();
        // Set guard.
        pGuard.writeU32(guard);
        // Zero the output fields so writes are visible.
        pF0.writeU32(0); pFC.writeU32(0); pF10.writeU32(0); pF14.writeU32(0);
        // Call.
        fn(idx);
        // Read back.
        const r0  = pF0.readU32();
        const rC  = pFC.readU32();
        const r10 = pF10.readU32();
        const r14 = pF14.readU32();
        // Restore.
        pGuard.writeU32(sGuard);
        pF0.writeU32(sF0); pFC.writeU32(sFC); pF10.writeU32(sF10); pF14.writeU32(sF14);
        // Pack: r0(byte) | rC(byte)<<8 | r10(byte)<<16 | r14(byte)<<24.
        return ((r0 & 0xff) | ((rC & 0xff) << 8) | ((r10 & 0xff) << 16) | ((r14 & 0xff) << 24)) >>> 0;
    }
    // float_scalar
    // MECHANISM: Passes fn(input>>>0) - single uint32 stack arg; returns fn's raw return value; no
    // buffer allocation, no global read-back, no out-pointer observation; no CONFIG beyond
    // `arg_type`. Observes ONLY the return value - a false-GREEN hazard for any function whose
    // primary effect is a side-effect or out-pointer write rather than a return value.
    // Mechanically identical to int_scalar; the different name carries no behavioral distinction
    // in the body.
    if (CONFIG.arg_type === 'uint32_scalar') {
        return fn(input >>> 0);
    }
    // float_scalar (default)
    // MECHANISM: Writes input[0]/input[1] as floats into the shared 8-byte `buf`; calls fn(buf) -
    // single pointer stack arg; returns fn's raw return value unchanged; does NOT read back buf
    // contents after the call; observes ONLY the return value - a false-GREEN hazard for any
    // function that writes its result into the vec2 and returns void or a trivially constant
    // value; no CONFIG beyond `arg_type`. Suitable only for fn(float*vec2) -> scalar getters;
    // wrong for any writer-into-buf.
    if (CONFIG.arg_type === 'vec2_ptr') {
        buf.writeFloat(input[0]);
        buf.add(4).writeFloat(input[1]);
        return fn(buf);
    }

    // ── Output-buffer types ───────────────────────────────────────────────────
    // These call fn(...) then read the output buffer and return a bit-packed
    // string so that the JS `===` comparison is a bit-identical check.

    // MECHANISM: Identical dispatch block as transform_point - uses pre-allocated shared
    // xformBufs.out/in/mat (12B/12B/64B); seeds .in from input.in[0..2] and .mat from
    // input.mat[0..15]; calls fn(out, in, mat) - 3 pointer stack args; observes 3 out-float bits
    // as comma-joined string; ignores fn return value; same physical .in/.mat for both Orig and
    // Reimpl; no CONFIG beyond `tests`; no globals. The two names dispatch to the same code - use
    // either for any 3-pointer (vec3-out, vec3-in, 4x4-mat) transform function.
    if (CONFIG.arg_type === 'transform_point' || CONFIG.arg_type === 'transform_vector') {
        // input: { in: [x,y,z], mat: [m0..m15] }
        xformBufs.in.writeFloat(input.in[0]);
        xformBufs.in.add(4).writeFloat(input.in[1]);
        xformBufs.in.add(8).writeFloat(input.in[2]);
        for (let j = 0; j < 16; j++)
            xformBufs.mat.add(j * 4).writeFloat(input.mat[j]);
        fn(xformBufs.out, xformBufs.in, xformBufs.mat);
        return [
            xformBufs.out.readU32(),
            xformBufs.out.add(4).readU32(),
            xformBufs.out.add(8).readU32(),
        ].join(',');
    }

    // MECHANISM: Uses pre-allocated shared buffers v2nBufs.in (8B) and v2nBufs.out (8B) plus
    // tmpF32 scratch; seeds .in from input[0..1], zeroes .out before each call; calls fn(out, in)
    // -> float; captures return float via tmpF32 for bit-exact IEEE-754 comparison; observes
    // [return_magnitude_bits, out[0]_bits, out[1]_bits] as comma-joined; same .in/.out pointers
    // for both Orig and Reimpl; no CONFIG beyond `tests`; no globals. Broadly fits any
    // fn(float*out2, float*in2) -> float normalise/length shape.
    if (CONFIG.arg_type === 'vec2_normalize') {
        // input: [x, y]
        v2nBufs.in.writeFloat(input[0]);
        v2nBufs.in.add(4).writeFloat(input[1]);
        v2nBufs.out.writeU32(0);
        v2nBufs.out.add(4).writeU32(0);
        const retVal = fn(v2nBufs.out, v2nBufs.in);
        tmpF32.writeFloat(retVal !== undefined ? retVal : 0.0);
        return [
            tmpF32.readU32(),               // magnitude bits
            v2nBufs.out.readU32(),          // out[0] bits
            v2nBufs.out.add(4).readU32(),   // out[1] bits
        ].join(',');
    }

    // MECHANISM: fn(out_ptr, in_ptr); module-level shared buffers v3nBufs.{in,out} (both re-seeded
    // before every callFn call so Orig and Reimpl receive the same pointer and fresh data); seeds
    // input[0..2] as f32 into in-buf, zeroes out-buf (3 dwords); observes [magnitude-f32-bits,
    // out[0..2]-bits] as a 4-element comma string; broader than normalisation: fits any fn(float3*
    // out, float3* in) -> float with a 12-byte output buffer - CONFIG-free, buffer size is
    // hardcoded to 3 floats.
    if (CONFIG.arg_type === 'vec3_normalize') {
        // input: [x, y, z]; fn(out, in) -> magnitude (float). 0x004c39b0 RwV3dNormalize.
        v3nBufs.in.writeFloat(input[0]);
        v3nBufs.in.add(4).writeFloat(input[1]);
        v3nBufs.in.add(8).writeFloat(input[2]);
        v3nBufs.out.writeU32(0);
        v3nBufs.out.add(4).writeU32(0);
        v3nBufs.out.add(8).writeU32(0);
        const retVal = fn(v3nBufs.out, v3nBufs.in);
        tmpF32.writeFloat(retVal !== undefined ? retVal : 0.0);
        return [
            tmpF32.readU32(),               // magnitude bits
            v3nBufs.out.readU32(),          // out[0] bits
            v3nBufs.out.add(4).readU32(),   // out[1] bits
            v3nBufs.out.add(8).readU32(),   // out[2] bits
        ].join(',');
    }

    // MECHANISM: NARROW: shared buffers xfdBufs (same out/mat/in pointers across both sides); call
    // shape fn(out, mat, 1, in) with hardcoded count=1; seeds 16-float mat + 3-float in, zeros
    // out; observes 3 u32s from xfdBufs.out. Both sides dispatch same device globals
    // (0x7d3ff8/0x7d3ffc) -> vtable slot +0x14, so GREEN validates the offset/global selection
    // only, not behavioral divergence - a false-GREEN risk. CONFIG: input.mat (16 floats),
    // input.in (3 floats).
    if (CONFIG.arg_type === 'device_transform_dispatch') {
        // input: { mat: [16 floats], in: [x,y,z] }. 0x004c3df0 RwV3dTransformPoints thunk.
        // fn(out, mat, 1, in) -> out (matches caller FUN_0046d510's call shape). Both orig
        // and reimpl read the same device globals (0x7d3ff8/0x7d3ffc) and dispatch the same
        // slot +0x14, so the result is identical by construction; a GREEN here validates the
        // +0x14 offset and the global selection (a wrong offset would call a different slot).
        for (let j = 0; j < 16; j++) xfdBufs.mat.add(j * 4).writeFloat(input.mat[j]);
        xfdBufs.in.writeFloat(input.in[0]);
        xfdBufs.in.add(4).writeFloat(input.in[1]);
        xfdBufs.in.add(8).writeFloat(input.in[2]);
        xfdBufs.out.writeU32(0);
        xfdBufs.out.add(4).writeU32(0);
        xfdBufs.out.add(8).writeU32(0);
        fn(xfdBufs.out, xfdBufs.mat, 1, xfdBufs.in);
        return [
            xfdBufs.out.readU32(),
            xfdBufs.out.add(4).readU32(),
            xfdBufs.out.add(8).readU32(),
        ].join(',');
    }

    // MECHANISM: fn(mat_ptr, axis_ptr, angle_float, mode_int): shared matrBufs (mat=64 bytes
    // alloc, axis=12 bytes); zeros matrBufs.mat before each call, seeds axis[3] floats; observable
    // = 13 of 16 output floats as u32 bit-exact (pad slots [7]/[11]/[15] excluded - documented
    // uninitialized in original); no CONFIG keys; tests=[{axis:[3], angle, mode}].
    if (CONFIG.arg_type === 'matrix_rotate') {
        // input: { axis: [x,y,z], angle: degrees, mode: int }. 0x004c4d20 RwMatrixRotate.
        // fn(matrix, axis, angle_deg, mode) -> matrix; compare all 16 output floats (bits).
        for (let j = 0; j < 16; j++) matrBufs.mat.add(j * 4).writeU32(0);
        matrBufs.axis.writeFloat(input.axis[0]);
        matrBufs.axis.add(4).writeFloat(input.axis[1]);
        matrBufs.axis.add(8).writeFloat(input.axis[2]);
        fn(matrBufs.mat, matrBufs.axis, input.angle, input.mode | 0);
        // Compare the 12 rotation/translation floats + the flags word at [3].
        // Skip RwMatrix pad slots [7]/[11]/[15]: the Rodrigues inner builder
        // (FUN_004c4a50) never writes them in mode 0, so they read uninitialized
        // stack — caller-dependent garbage in the ORIGINAL too (not real output).
        const out = [];
        for (let j = 0; j < 16; j++) {
            if (j === 7 || j === 11 || j === 15) continue;  // pad — uninitialized
            out.push(matrBufs.mat.add(j * 4).readU32());
        }
        return out.join(',');
    }

    // MECHANISM: fn(mat_ptr, axis_ptr, omc_float, sin_float, mode_int): seeds all 16 input matrix
    // floats to shared mriBufs.mat and axis[3] to mriBufs.axis before each call; observable = 13
    // of 16 floats as u32 bit-exact (skips uninitialized pad [7]/[11]/[15]); richer than
    // matrix_rotate: caller supplies full input matrix (mode 0 pure replace, modes 1/2 use it as
    // concat operand); no CONFIG keys; tests=[{matrix:[16],axis:[3],omc,sin,mode}].
    if (CONFIG.arg_type === 'matrix_rotate_inner') {
        // input: { matrix:[16], axis:[3] normalized, omc:float (1-cos), sin:float, mode:int }
        // 0x004c4a50 RwMatrixRotateInner. fn(matrix, axis_n, 1-cos, sin, mode) -> matrix.
        // mode 0 ignores the input matrix (pure replace); modes 1/2 use it as the concat
        // operand + dispatch the RW device matrix-mult. Skip pad [7]/[11]/[15] (uninitialized).
        for (let j = 0; j < 16; j++) mriBufs.mat.add(j * 4).writeFloat(input.matrix[j]);
        mriBufs.axis.writeFloat(input.axis[0]);
        mriBufs.axis.add(4).writeFloat(input.axis[1]);
        mriBufs.axis.add(8).writeFloat(input.axis[2]);
        fn(mriBufs.mat, mriBufs.axis, input.omc, input.sin, input.mode | 0);
        const out = [];
        for (let j = 0; j < 16; j++) {
            if (j === 7 || j === 11 || j === 15) continue;  // pad — uninitialized
            out.push(mriBufs.mat.add(j * 4).readU32());
        }
        return out.join(',');
    }

    // MECHANISM: fn(mat_buf, scale_buf, mode_int); harness writes input.mat[16 floats] into
    // matsBufs.mat and input.scale[3 floats] into matsBufs.scale, calls fn in-place, reads back 13
    // of 16 dwords (skips indices 3/7/11 as flags) as comma-joined u32 fingerprint; return value
    // not observed; no CONFIG keys - buffer names and skip indices are hardcoded.
    if (CONFIG.arg_type === 'matrix_scale') {
        // input: { mat: [16 floats], scale: [3 floats], mode: int }
        for (let j = 0; j < 16; j++)
            matsBufs.mat.add(j * 4).writeFloat(input.mat[j]);
        matsBufs.scale.writeFloat(input.scale[0]);
        matsBufs.scale.add(4).writeFloat(input.scale[1]);
        matsBufs.scale.add(8).writeFloat(input.scale[2]);
        fn(matsBufs.mat, matsBufs.scale, input.mode);
        // Compare the 15 data floats (skip flags at [3]/[7]/[11]).
        const out = [];
        for (let j = 0; j < 16; j++) {
            if (j === 3 || j === 7 || j === 11) continue;  // flags/pad — skip
            out.push(matsBufs.mat.add(j * 4).readU32());
        }
        return out.join(',');
    }

    // fmt_desc_ptr — fn(ptr_to_fmt_desc) -> int32.
    // input: { f04, f10, f14 } — u32 values written to struct offsets.
    // buf must be pre-allocated (0x20 bytes); zeroed then fields written.
    // MECHANISM: Call shape fn(ptr_to_0x20_struct) -> int32; harness zeroes shared 0x20-byte buf
    // then writes u32 fields at offsets +0x04/+0x10/+0x14; observes return value only - no buffer
    // readback. CONFIG: input.f04/f10/f14. Same buf pointer delivered to both sides (re-zeroed
    // each call); false-GREEN risk if fn stores the ptr as a side-effect and observes nothing
    // beyond the int32 return.
    if (CONFIG.arg_type === 'fmt_desc_ptr') {
        // Zero 0x20 bytes then write test fields
        for (var fz = 0; fz < 0x20; fz++) buf.add(fz).writeU8(0);
        buf.add(0x04).writeU32(input.f04 >>> 0);
        buf.add(0x10).writeU32(input.f10 >>> 0);
        buf.add(0x14).writeU32(input.f14 >>> 0);
        return fn(buf);
    }
    // fmt_desc_copy — fn(src_ptr, dst_ptr, zero_init) -> void.
    // input: { f00, f04, f05, f10, zero_init } — src field values; zero_init flag.
    // Uses two 0x20-byte buffers (fmtSrcBuf, fmtDstBuf).
    // Returns packed u32: dst[+0x04] ^ dst[+0x0c] ^ dst[+0x0d] ^ dst[+0x18].
    // MECHANISM: Call shape fn(src_ptr, dst_ptr, zero_init) -> void; zeroes shared
    // fmtSrcBuf/fmtDstBuf (0x20 bytes each) then seeds src at offsets
    // +0x00/+0x04(u32)/+0x05/+0x10; observes 4 dst fields (+0x04 as u32, +0x0c/+0x0d/+0x18 as u8)
    // XOR-packed into uint32. CONFIG: tests[].f00/f04/f05/f10/zero_init. Same buffer ptrs
    // delivered to both sides (re-zeroed each call); false-GREEN risk if fn stores either buf ptr
    // internally.
    if (CONFIG.arg_type === 'fmt_desc_copy') {
        for (var fs = 0; fs < 0x20; fs++) { fmtSrcBuf.add(fs).writeU8(0); fmtDstBuf.add(fs).writeU8(0); }
        fmtSrcBuf.add(0x00).writeU32(input.f00 >>> 0);
        // Write f04 as u32 first (low byte at +0x04)
        fmtSrcBuf.add(0x04).writeU32(input.f04 >>> 0);
        fmtSrcBuf.add(0x05).writeU8((input.f05 >>> 0) & 0xff);
        fmtSrcBuf.add(0x10).writeU8((input.f10 >>> 0) & 0xff);
        fn(fmtSrcBuf, fmtDstBuf, input.zero_init | 0);
        // Observable: 4 written dst fields packed
        const d04 = fmtDstBuf.add(0x04).readU32();
        const d0c = fmtDstBuf.add(0x0c).readU8();
        const d0d = fmtDstBuf.add(0x0d).readU8();
        const d18 = fmtDstBuf.add(0x18).readU8();
        return ((d04 & 0xffff) ^ ((d0c & 0xff) << 16) ^ ((d0d & 0xff) << 20) ^ ((d18 & 0xff) << 24)) >>> 0;
    }
    // fmt_table_search — fn(ctx_ptr, desc_ptr) -> uint32 (1 match, 0 no-match).
    // input: { count, entry_ptr } — writes count at ctx+0x24, ptr at ctx+0x28.
    // fmtCtxBuf is a 0x30-byte fake audio context.
    // MECHANISM: fn(ctx_ptr, ptr(0)): per test zeroes the 0x30-byte fmtCtxBuf, seeds input.count
    // at +0x24 and a pointer to fmtEntryPtrBuf (holding input.entry_ptr) at +0x28; second arg is
    // always null; observable = return uint32 only (no buffer read-back); NARROW: ctx offsets
    // 0x24/0x28 and 0x30-byte size are hardcoded; no CONFIG keys.
    if (CONFIG.arg_type === 'fmt_table_search') {
        for (var fc = 0; fc < 0x30; fc++) fmtCtxBuf.add(fc).writeU8(0);
        fmtCtxBuf.add(0x24).writeU32(input.count >>> 0);
        // array base: store pointer to fmtEntryPtrBuf (which holds entry_ptr)
        if (input.count > 0) {
            fmtEntryPtrBuf.writePointer(ptr(input.entry_ptr >>> 0));
            fmtCtxBuf.add(0x28).writePointer(fmtEntryPtrBuf);
        }
        return fn(fmtCtxBuf, ptr(0));
    }
    // fmt_global_scan — fn(key_ptr) -> pointer.
    // input: array of 16 u8 bytes forming the format key.
    // Returns pointer value as uint32 (comparable; NULL=0).
    // MECHANISM: Call shape fn(key_ptr) -> pointer; harness zeroes shared 16-byte fmtKeyBuf then
    // writes input[0..15]; observes return pointer value as uint32 (0=NULL) - does NOT dereference
    // the returned pointer. Same fmtKeyBuf ptr delivered to both sides. CONFIG: input = array of
    // up to 16 u8 bytes. Applies to any fn(16-byte-key-ptr) -> ptr doing a lookup into shared
    // global state; false-RED if sides return different-but-equivalent pointers.
    if (CONFIG.arg_type === 'fmt_global_scan') {
        for (var fk = 0; fk < 16; fk++) fmtKeyBuf.add(fk).writeU8(0);
        for (var ki = 0; ki < 16 && ki < input.length; ki++) {
            fmtKeyBuf.add(ki).writeU8(input[ki] & 0xff);
        }
        var pRet = fn(fmtKeyBuf);
        // Return the pointer value as a comparable uint32
        return pRet ? parseInt(pRet.toString(), 16) : 0;
    }
    // Fallback: float_scalar
    return fn(input);
}

// Compute a position-sensitive XOR fingerprint of len bytes starting at buf.
// Bytes at positions 0,4,8,... contribute to bits [7:0]; 1,5,9,... to bits
// [15:8]; etc.  Collisions are possible but sufficient for diverse test inputs.
function bufFingerprint(buf, len) {
    var fp = 0;
    for (var j = 0; j < len; j++) {
        fp ^= (buf.add(j).readU8() << ((j & 3) * 8));
    }
    return fp >>> 0;
}

function runDiff() {
    var module;
    try {
        // Use the handle returned by Module.load directly. findModuleByName
        // matches by basename and can return a *different* mashed_re_dev.asi
        // already auto-loaded by the dinput8 proxy (main checkout's build),
        // which lacks worktree-only exports. Prefer the exact module we loaded.
        const loaded = Module.load(ASI_PATH);
        module = (loaded && loaded.findExportByName) ? loaded
               : Process.findModuleByName('mashed_re_dev.asi');
        if (module === null || module === undefined) {
            send({ type: 'error', msg: 'findModuleByName returned null after Module.load' });
            return;
        }
    } catch (e) {
        send({ type: 'error', msg: 'Module.load failed: ' + e.message });
        return;
    }
    const reimplAddr = module.findExportByName(CONFIG.export);
    if (reimplAddr === null) {
        send({ type: 'error', msg: CONFIG.export + ' export not found in .asi' });
        return;
    }
    send({ type: 'asi_loaded',
           base:         '0x' + module.base.toString(16),
           reimpl_addr:  '0x' + reimplAddr.toString(16),
           export_name:  CONFIG.export });

    const callConv      = CONFIG.calling_convention || 'mscdecl';
    const origCallConv  = CONFIG.orig_calling_convention  || callConv;
    const reimplCallConv = CONFIG.reimpl_calling_convention || callConv;
    const Orig   = new NativeFunction(TARGET_ADDR, CONFIG.signature.ret, CONFIG.signature.args, origCallConv);
    const Reimpl = new NativeFunction(reimplAddr,  CONFIG.signature.ret, CONFIG.signature.args, reimplCallConv);

    const buf = (['vec3_ptr', 'out3_idx', 'vec2_ptr'].includes(CONFIG.arg_type)) ? Memory.alloc(12)
              : (['int_with_out_ptr', 'idx_out2', 'int_ptr2_out', 'cache_roundtrip', 'cache_setter_observe', 'st0_ret_global', 'out1_idx'].includes(CONFIG.arg_type)) ? Memory.alloc(8)
              : (CONFIG.arg_type === 'time_diff_decompose') ? Memory.alloc(16)
              : (CONFIG.arg_type === 'sentinel_array_ptr') ? Memory.alloc(256)
              : (CONFIG.arg_type === 'ptr_arg_int_get') ? Memory.alloc((CONFIG.struct_size | 0) || 256)
              : (CONFIG.arg_type === 'fmt_desc_ptr') ? Memory.alloc(0x20)
              : (CONFIG.arg_type === 'st0_ret_mat3_ptr') ? Memory.alloc(0x30)
              : (CONFIG.arg_type === 'st0_ret_mat4x3_ptr') ? Memory.alloc(0x40)
              : (CONFIG.arg_type === 'ptr_arg_int_get') ? Memory.alloc((CONFIG.struct_size | 0) || 256)
              : null;

    // contact_history: allocate fake vehicle struct (0xC80 bytes) and geom entry (0x40 bytes).
    if (CONFIG.arg_type === 'contact_history') {
        vehicleBuf = Memory.alloc(0xC80);
        geomBuf    = Memory.alloc(0x40);
    }

    // For 'read_global', 'write_global_call_int0', and 'void_setter_observe', save and
    // restore the target global so we don't leave a sentinel in game-state memory after the test.
    let savedGlobal = null;
    if (CONFIG.arg_type === 'read_global' || CONFIG.arg_type === 'write_global_call_int0'
        || CONFIG.arg_type === 'void_setter_observe') {
        try { savedGlobal = ptr(CONFIG.target_global).readU32(); }
        catch (e) { send({ type: 'error', msg: 'failed reading target_global: ' + e.message }); return; }
    }

    if (CONFIG.arg_type === 'transform_point' || CONFIG.arg_type === 'transform_vector') {
        xformBufs = { out: Memory.alloc(12), in: Memory.alloc(12), mat: Memory.alloc(64) };
    }
    if (CONFIG.arg_type === 'vec2_normalize') {
        v2nBufs = { out: Memory.alloc(8), in: Memory.alloc(8) };
        tmpF32  = Memory.alloc(4);
    }
    if (CONFIG.arg_type === 'vec3_normalize') {
        v3nBufs = { out: Memory.alloc(12), in: Memory.alloc(12) };
        tmpF32  = Memory.alloc(4);
    }
    if (CONFIG.arg_type === 'device_transform_dispatch') {
        // generous (64B) so any device-method over-read past the 3-float payload
        // stays inside a mapped allocation rather than faulting.
        xfdBufs = { out: Memory.alloc(64), mat: Memory.alloc(64), in: Memory.alloc(64) };
    }
    if (CONFIG.arg_type === 'matrix_rotate') {
        matrBufs = { mat: Memory.alloc(64), axis: Memory.alloc(12) };
    }
    if (CONFIG.arg_type === 'matrix_rotate_inner') {
        mriBufs = { mat: Memory.alloc(64), axis: Memory.alloc(12) };
    }
    if (CONFIG.arg_type === 'matrix_scale') {
        matsBufs = { mat: Memory.alloc(64), scale: Memory.alloc(12) };
    }
    // fmt_* buffers (declared null at module scope) — allocate on demand.
    // Without these, every fmt_desc_copy / fmt_table_search / fmt_global_scan
    // call throws "cannot read property 'add' of null" (regression: the
    // original authors of these arg_types declared the slots but never
    // wrote the Memory.alloc lines).
    if (CONFIG.arg_type === 'fmt_desc_copy') {
        fmtSrcBuf = Memory.alloc(0x20);
        fmtDstBuf = Memory.alloc(0x20);
    }
    if (CONFIG.arg_type === 'fmt_table_search') {
        fmtCtxBuf      = Memory.alloc(0x30);
        fmtEntryPtrBuf = Memory.alloc(4);
    }
    if (CONFIG.arg_type === 'fmt_global_scan') {
        fmtKeyBuf = Memory.alloc(16);
    }
    const results = [];

    // ── struct_call_observe ─────────────────────────────────────────────────
    // Generic struct-pointer call harness for leaf getters/setters that read
    // and/or write fields of a heap struct (with optional out-pointer args and
    // nested sub-struct pointers). Added 2026-06-04 (c3-batch-ab-s4) for the
    // audio leaves 0x005bfcc0 / 0x005c7500 / 0x005c75b0.
    //
    // Per test: allocate two fresh zeroed struct buffers (Orig + Reimpl side),
    // seed identical field values, wire up nested sub-struct pointers, call
    // fn(structPtr [, out0 [, out1]]), then read back the declared observables
    // (struct fields, out-buffer fields, and/or the return value) and compare.
    //
    // CONFIG:
    //   struct_size  int               bytes to allocate for param_1 (zeroed)
    //   out_ptrs     0|1|2             extra 8-byte out-pointer args after param_1
    //   observe_ret  bool              include the return value in the fingerprint
    //   observe      [{src,off,type}]  src: 'struct'|'out0'|'out1'
    // tests[i]:
    //   { seeds:[{off,type,value}], nested:[{ptr_off,size,fields:[{off,type,value}]}] }
    //   type for seeds: u8|u16|u32|s32|f32|u64 ; for observe: u8|u16|u32|s32|u64
    // MECHANISM: Per side allocates a zeroed struct buffer (`CONFIG.struct_size`) + up to 2 eight-
    // byte out-bufs (`CONFIG.out_ptrs`); seeds fields and nested sub-struct graphs identically on
    // each side; calls fn(struct[,out0[,out1]]); fingerprints selected offsets
    // (u8/u16/u32/s32/u64) from struct or out-bufs and optionally return; each side's pointers
    // differ in address but match in seeded content so relative-offset reads compare cleanly.
    // CONFIG: `struct_size`, `out_ptrs`, `observe_ret`, `observe`; test: `seeds`, `nested`.
    if (CONFIG.arg_type === 'struct_call_observe') {
        const SS   = CONFIG.struct_size || 0x200;
        const nOut = CONFIG.out_ptrs || 0;
        const wr = function (p, off, type, value) {
            const a = p.add(off);
            switch (type) {
                case 'u8':  a.writeU8(value & 0xff); break;
                case 'u16': a.writeU16(value & 0xffff); break;
                case 's32': a.writeS32(value | 0); break;
                case 'f32': a.writeFloat(value); break;
                case 'u64': a.writeU32(value >>> 0); a.add(4).writeU32(0); break;
                default:    a.writeU32(value >>> 0); break;  // u32
            }
        };
        const rd = function (p, off, type) {
            const a = p.add(off);
            switch (type) {
                case 'u8':  return (a.readU8()  >>> 0).toString(16);
                case 'u16': return (a.readU16() >>> 0).toString(16);
                case 's32': return (a.readS32()  | 0).toString(16);
                case 'u64': return (a.readU32() >>> 0).toString(16) + ':' + (a.add(4).readU32() >>> 0).toString(16);
                default:    return (a.readU32() >>> 0).toString(16);  // u32
            }
        };
        const structO = Memory.alloc(SS), structR = Memory.alloc(SS);
        const outsO = [], outsR = [];
        for (let k = 0; k < nOut; k++) { outsO.push(Memory.alloc(8)); outsR.push(Memory.alloc(8)); }
        // Retain every nested sub-buffer for the whole test loop. The nested
        // Memory.alloc()s below are closure-locals whose only surviving reference
        // after the forEach is the raw integer pointer written into the parent
        // struct (NOT a live JS NativePointer) — so Frida can reclaim/reuse that
        // memory before the force-call fires, leaving the ORIG side reading stale
        // heap garbage (nondeterministic partial mismatches). Pushing the handles
        // here keeps them alive for the script's lifetime. (c3-batch-ae-s2.)
        const _keepAlive = [];
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            for (let b = 0; b < SS; b++) { structO.add(b).writeU8(0); structR.add(b).writeU8(0); }
            for (let k = 0; k < nOut; k++) {
                outsO[k].writeU32(0); outsO[k].add(4).writeU32(0);
                outsR[k].writeU32(0); outsR[k].add(4).writeU32(0);
            }
            (t.seeds || []).forEach(function (s) { wr(structO, s.off, s.type, s.value); wr(structR, s.off, s.type, s.value); });
            (t.nested || []).forEach(function (n) {
                const subO = Memory.alloc(n.size), subR = Memory.alloc(n.size);
                _keepAlive.push(subO, subR);   // keep alive past this closure (see note above)
                for (let b = 0; b < n.size; b++) { subO.add(b).writeU8(0); subR.add(b).writeU8(0); }
                (n.fields || []).forEach(function (f) { wr(subO, f.off, f.type, f.value); wr(subR, f.off, f.type, f.value); });
                structO.add(n.ptr_off).writePointer(subO);
                structR.add(n.ptr_off).writePointer(subR);
            });
            let retO = null, retR = null, errO = null, errR = null;
            try {
                if (nOut === 0)      retO = Orig(structO);
                else if (nOut === 1) retO = Orig(structO, outsO[0]);
                else                 retO = Orig(structO, outsO[0], outsO[1]);
            } catch (e) { errO = e.message; }
            try {
                if (nOut === 0)      retR = Reimpl(structR);
                else if (nOut === 1) retR = Reimpl(structR, outsR[0]);
                else                 retR = Reimpl(structR, outsR[0], outsR[1]);
            } catch (e) { errR = e.message; }
            const fpO = [], fpR = [];
            if (CONFIG.observe_ret) {
                fpO.push('r=' + (retO !== null && retO !== undefined ? retO.toString() : 'null'));
                fpR.push('r=' + (retR !== null && retR !== undefined ? retR.toString() : 'null'));
            }
            (CONFIG.observe || []).forEach(function (o) {
                const baseO = o.src === 'out0' ? outsO[0] : o.src === 'out1' ? outsO[1] : structO;
                const baseR = o.src === 'out0' ? outsR[0] : o.src === 'out1' ? outsR[1] : structR;
                fpO.push(o.src + '+' + o.off + '=' + rd(baseO, o.off, o.type));
                fpR.push(o.src + '+' + o.off + '=' + rd(baseR, o.off, o.type));
            });
            const sO = fpO.join('|'), sR = fpR.join('|');
            results.push({ idx: i, input: JSON.stringify(t), original: sO, reimpl: sR,
                           match: (!errO && !errR && sO === sR), err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── ptr_seed_observe ────────────────────────────────────────────────────
    // Generic multi-pointer pure-leaf differ. Allocates one scratch buffer per
    // pointer arg (paired Orig/Reimpl), seeds each per-test, threads positional
    // call args (buffer pointers and/or float/int scalars), runs both sides, and
    // observes buffer offsets as a BIT-IDENTICAL fingerprint (f32 read back as
    // raw u32 bits so a sub-ULP divergence is caught). Authored for the
    // rw-palette-quantizer leaves 0x004d9360 / 0x004d9a60 / 0x004d9ee0
    // (c3-batch-render-p2w1a-s1). All are pure functions of their args + a few
    // absolute-address image constants, so seeded non-zero inputs make the
    // observation non-degenerate without touching any live game state.
    //
    //   CONFIG.num_bufs    int    scratch buffers to allocate (default: count of
    //                             {buf:i} entries in arg_layout)
    //   CONFIG.buf_size    int    bytes per buffer (default 64)
    //   CONFIG.arg_layout  array  positional call args, in order, each one of:
    //                               {buf:i}    -> pointer to scratch buffer i
    //                               {f32:true} -> next value from test.scalars
    //                               {i32:true} -> next value from test.scalars
    //   CONFIG.observe     array  [{buf:i, off:N, type:'f32'|'u8'|'u16'|'u32'|'s32'}]
    //   CONFIG.tests[i] = { seed:[{buf,off,type,value}...], scalars:[...] }
    // MECHANISM: Allocates N paired scratch buffers per side (`CONFIG.num_bufs`x`buf_size`); seeds
    // identically per-test (flat field values or cross-buffer `ptr_to` pointer wires to build
    // struct graphs); calls fn via `CONFIG.arg_layout` ({buf:i}->pointer, {f32}/{i32}->scalar from
    // test.scalars, per-test `null_args`->NULL); fingerprints selected buffer offsets (f32 as raw
    // bits for bit-identity) and optionally return. CONFIG: `num_bufs`, `buf_size`, `arg_layout`,
    // `observe`, `observe_ret`.
    if (CONFIG.arg_type === 'ptr_seed_observe') {
        const layout = CONFIG.arg_layout || [];
        const NB = (CONFIG.num_bufs | 0) ||
                   layout.filter(function (a) { return a && a.buf !== undefined; }).length;
        const BS = (CONFIG.buf_size | 0) || 64;
        const bufsO = [], bufsR = [];
        for (let k = 0; k < NB; k++) { bufsO.push(Memory.alloc(BS)); bufsR.push(Memory.alloc(BS)); }
        const wr = function (p, off, type, value) {
            const a = p.add(off);
            switch (type) {
                case 'u8':  a.writeU8(value & 0xff); break;
                case 'u16': a.writeU16(value & 0xffff); break;
                case 's32': a.writeS32(value | 0); break;
                case 'f32': a.writeFloat(value); break;
                case 'u64': a.writeU32(value >>> 0); a.add(4).writeU32(0); break;
                default:    a.writeU32(value >>> 0); break;  // u32
            }
        };
        const rd = function (p, off, type) {
            const a = p.add(off);
            switch (type) {
                case 'u8':  return (a.readU8()  >>> 0).toString(16);
                case 'u16': return (a.readU16() >>> 0).toString(16);
                case 's32': return (a.readS32()  | 0).toString(16);
                // f32 read as raw bits for a true bit-identity compare.
                case 'f32': return (a.readU32() >>> 0).toString(16);
                default:    return (a.readU32() >>> 0).toString(16);  // u32
            }
        };
        // test.null_args = [argIndex, ...] forces those POSITIONAL args to NULL
        // for that test, overriding whatever the layout says. Added orch-iter20
        // for 0x00411530, whose null-object path is half the function and could
        // not be reached otherwise: a {buf:i} arg always passed a real pointer,
        // and the address cannot be threaded through {i32} because the two sides
        // allocate at different addresses. Purely additive — omitting it keeps
        // the old behaviour exactly.
        const buildArgs = function (bufs, scalars, nulls) {
            const args = [];
            let si = 0;
            for (let k = 0; k < layout.length; k++) {
                const a = layout[k];
                const isNull = nulls && nulls.indexOf(k) !== -1;
                if (a && a.buf !== undefined) args.push(isNull ? NULL : bufs[a.buf]);
                else {
                    const v = scalars[si++];  // f32/i32 -> JS number (NativeFunction promotes)
                    args.push(isNull ? 0 : v);
                }
            }
            return args;
        };
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            for (let k = 0; k < NB; k++) {
                for (let b = 0; b < BS; b++) { bufsO[k].add(b).writeU8(0); bufsR[k].add(b).writeU8(0); }
            }
            // {buf,off,ptr_to:j} writes the ADDRESS of buffer j into buffer
            // s.buf at s.off (each side gets its own side's buffer, so the two
            // graphs are structurally identical but never share memory). Added
            // 2026-07-30 for 0x0055c4f0, a recursive setter that double-derefs
            // its arg — `**(short**)(p+0x5c)` and `*(int**)(p+0x40)` — so a flat
            // literal seed cannot reach the branch at all, it just AVs on a
            // garbage inner pointer. This is what lets a multi-buffer seed build
            // a real struct GRAPH (parent -> tag, parent -> child-array -> child)
            // instead of a flat record. Opt-in: seeds without ptr_to are
            // unchanged, so existing users of this arg_type are unaffected.
            (t.seed || []).forEach(function (s) {
                if (s.ptr_to !== undefined) {
                    bufsO[s.buf].add(s.off).writePointer(bufsO[s.ptr_to]);
                    bufsR[s.buf].add(s.off).writePointer(bufsR[s.ptr_to]);
                } else {
                    wr(bufsO[s.buf], s.off, s.type, s.value);
                    wr(bufsR[s.buf], s.off, s.type, s.value);
                }
            });
            const scalars = t.scalars || [];
            let errO = null, errR = null, retO = null, retR = null;
            try { retO = Orig.apply(null, buildArgs(bufsO, scalars, t.null_args)); }   catch (e) { errO = e.message; }
            try { retR = Reimpl.apply(null, buildArgs(bufsR, scalars, t.null_args)); } catch (e) { errR = e.message; }
            const fpO = [], fpR = [];
            // observe_ret (2026-07-31): fold the RETURN into the fingerprint.
            // Added because this handler otherwise fingerprints buffer contents
            // only, so a function whose entire observable IS its return (e.g.
            // 0x00482900, pure arithmetic over two scalar args) would produce an
            // EMPTY fingerprint — identical on both sides, i.e. a false GREEN.
            // With num_bufs 0 and observe [], observe_ret is the whole test.
            if (CONFIG.observe_ret) {
                const norm = function (v) {
                    return (v === null || v === undefined) ? 'null'
                         : (typeof v === 'object' ? (v.toInt32() | 0) : (v | 0)).toString(16);
                };
                fpO.push('r=' + norm(retO));
                fpR.push('r=' + norm(retR));
            }
            (CONFIG.observe || []).forEach(function (o) {
                fpO.push(o.buf + '+' + o.off + '=' + rd(bufsO[o.buf], o.off, o.type));
                fpR.push(o.buf + '+' + o.off + '=' + rd(bufsR[o.buf], o.off, o.type));
            });
            const sO = fpO.join('|'), sR = fpR.join('|');
            results.push({ idx: i, input: JSON.stringify(t), original: sO, reimpl: sR,
                           match: (!errO && !errR && sO === sR),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── pcm_pack ────────────────────────────────────────────────────────────
    // Saturating int32->int16 PCM pack: void fn(i16* dst, i32* src, u32 count).
    // The function writes 2*count int16 samples from 2*count int32 sources.
    // Added 2026-06-04 (c3-batch-ab-s4) for 0x005c9770.
    // Per test {src:[int32...], count:N}: write source ints into a shared src
    // buffer, call Orig/Reimpl into two separate dst buffers, compare exact bytes.
    // MECHANISM: fn(i16* dst, i32* src, u32 count)->void; writes input.src[] as int32s into ONE
    // shared srcBuf (pointer-equal on both sides), allocates separate dstO/dstR per side, calls
    // Orig(dstO,srcBuf,count) then Reimpl(dstR,srcBuf,count), observes hex dump of nSamples*2
    // bytes from each dst; return value not observed; CONFIG: tests[] of {src:[int32...],count:N};
    // NARROW: src is shared - a function that mutates src corrupts the reimpl call.
    if (CONFIG.arg_type === 'pcm_pack') {
        const SRC_MAX = 8192, DST_MAX = 8192;
        const srcBuf = Memory.alloc(SRC_MAX);
        const dstO = Memory.alloc(DST_MAX), dstR = Memory.alloc(DST_MAX);
        const hexDump = function (p, nbytes) {
            let s = '';
            for (let b = 0; b < nbytes; b++) { const v = p.add(b).readU8(); s += (v < 16 ? '0' : '') + v.toString(16); }
            return s;
        };
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const src = t.src || [];
            const count = t.count >>> 0;
            const nSamples = src.length;       // = 2*count
            const dstBytes = nSamples * 2;     // 2 bytes per int16 output sample
            for (let k = 0; k < SRC_MAX; k++) srcBuf.add(k).writeU8(0);
            for (let k = 0; k < DST_MAX; k++) { dstO.add(k).writeU8(0); dstR.add(k).writeU8(0); }
            for (let k = 0; k < nSamples; k++) srcBuf.add(k * 4).writeS32(src[k] | 0);
            let errO = null, errR = null;
            try { Orig(dstO, srcBuf, count); }   catch (e) { errO = e.message; }
            try { Reimpl(dstR, srcBuf, count); } catch (e) { errR = e.message; }
            const fO = hexDump(dstO, dstBytes), fR = hexDump(dstR, dstBytes);
            results.push({ idx: i, input: JSON.stringify(t), original: fO, reimpl: fR,
                           match: (!errO && !errR && fO === fR), err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }


    // ── bytes_inplace / bytes_inplace_3 ─────────────────────────────────────
    // Allocate a pair of scratch buffers; fill both from test.init before each
    // call; compare buffer fingerprints (not return value, which is void).
    const BUFSIZE = 256;
    // MECHANISM: fn(ptr, int len, int width); shares a single dispatch `if` with bytes_inplace
    // (2-arg); per-side 256-byte scratch buffers (bufA/bufB) filled from tests[i].init (byte
    // array, length tests[i].len) before each call; calls fn(buf, len, width) where width =
    // tests[i].width; observes bufFingerprint(buf, len) - rolling XOR fingerprint of the first len
    // bytes of the output buffer; return value is NOT observed; fits any in-place buffer mutator
    // with signature (ptr, len, width) up to 256 bytes output.
    if (CONFIG.arg_type === 'bytes_inplace' || CONFIG.arg_type === 'bytes_inplace_3') {
        const bufA = Memory.alloc(BUFSIZE);
        const bufB = Memory.alloc(BUFSIZE);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var bytes = t.init || [];
            var len   = t.len  || 0;
            for (var j = 0; j < len; j++) {
                bufA.add(j).writeU8(bytes[j] | 0);
                bufB.add(j).writeU8(bytes[j] | 0);
            }
            var errO = null, errR = null;
            if (CONFIG.arg_type === 'bytes_inplace') {
                try { Orig(bufA, len); }   catch(e) { errO = e.message; }
                try { Reimpl(bufB, len); } catch(e) { errR = e.message; }
            } else {
                var width = t.width | 0;
                try { Orig(bufA, len, width); }   catch(e) { errO = e.message; }
                try { Reimpl(bufB, len, width); } catch(e) { errR = e.message; }
            }
            var fA = bufFingerprint(bufA, len);
            var fB = bufFingerprint(bufB, len);
            var match = (!errO && !errR && fA === fB);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: fA, reimpl: fB, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── bgra_encode ──────────────────────────────────────────────────────────
    // For pixel-format encoder leaves: fn(byte *bgra) -> uint32.
    //   param_1 = pointer to a 4-byte (or 3-byte) input buffer.
    //   Return value = the packed pixel word (uint16 or uint32 depending on format).
    // Strategy: allocate a 4-byte scratch buf; write each test's bytes; call fn(buf);
    //   compare the integer return value (orig vs reimpl).
    // Tests: array of [b0, b1, b2, b3] (4-element; for 3-byte BGR variants b3 is ignored).
    // Harness-extension arg_type added 2026-05-30 (c3-batch-ab-s3).
    // Unblocks: 0x004df8d0 PixEncode1555, 0x004df910 PixEncode4444,
    //           0x004df950 PixEncodeA8R3G3B2, 0x004df980 PixEncodeX4R4G4B4,
    //           0x004df9e0 PixEncodeX8R8G8B8.
    // MECHANISM: fn(byte* buf) -> uint32; harness allocates a single shared 4-byte buffer, writes
    // CONFIG.tests[i]=[b0,b1,b2,b3] as individual bytes before each Orig and Reimpl call (re-
    // seeded between the two), compares the unsigned integer return; same pointer used for both
    // sides; broader than the name: fits any fn(byte*) -> uint that reads <=4 bytes from its sole
    // pointer arg and returns a packed scalar.
    if (CONFIG.arg_type === 'bgra_encode') {
        const encBuf = Memory.alloc(4);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            encBuf.add(0).writeU8((t[0] || 0) & 0xff);
            encBuf.add(1).writeU8((t[1] || 0) & 0xff);
            encBuf.add(2).writeU8((t[2] || 0) & 0xff);
            encBuf.add(3).writeU8((t[3] || 0) & 0xff);
            var origV = null, reimV = null, errO = null, errR = null;
            try { origV = (Orig(encBuf) >>> 0); } catch(e) { errO = e.message; }
            encBuf.add(0).writeU8((t[0] || 0) & 0xff);
            encBuf.add(1).writeU8((t[1] || 0) & 0xff);
            encBuf.add(2).writeU8((t[2] || 0) & 0xff);
            encBuf.add(3).writeU8((t[3] || 0) & 0xff);
            try { reimV = (Reimpl(encBuf) >>> 0); } catch(e) { errR = e.message; }
            var match = (!errO && !errR && origV === reimV);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── cstr_ret_offset ──────────────────────────────────────────────────────
    // find-extension-style fn(char* s) -> char* (a pointer into s). Writes
    // test.str (ASCII, NUL-terminated) into a shared buffer, calls fn(buf), and
    // compares the returned pointer as a byte offset from buf (orig vs reimpl).
    // -1 means the returned pointer was null. test: { str: "filename.ext" }.
    // Harness-extension arg_type added 2026-06-04 (c3_batch_ab s3) for 0x005b73b0.
    // MECHANISM: Call shape fn(char* buf) -> char*; harness allocates 512-byte shared buf, writes
    // test.str as NUL-terminated ASCII, calls each side with same buf ptr; observes return pointer
    // as byte offset from buf start (retptr - buf), reports -1 if NULL. Observes return offset
    // only. CONFIG: tests[].str. Applies to any fn(char*) -> char* returning a pointer into its
    // input buffer (e.g., extension finders, substring locators).
    if (CONFIG.arg_type === 'cstr_ret_offset') {
        const sbuf = Memory.alloc(512);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var s = (t && t.str != null) ? t.str : '';
            for (var j = 0; j < s.length; j++) sbuf.add(j).writeU8(s.charCodeAt(j) & 0xff);
            sbuf.add(s.length).writeU8(0);
            var offO = -1, offR = -1, errO = null, errR = null;
            try { var pO = Orig(sbuf);   if (pO && !pO.isNull()) offO = pO.sub(sbuf).toInt32(); } catch(e) { errO = e.message; }
            try { var pR = Reimpl(sbuf); if (pR && !pR.isNull()) offR = pR.sub(sbuf).toInt32(); } catch(e) { errR = e.message; }
            var match = (!errO && !errR && offO === offR);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: offO, reimpl: offR, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── pcm_sat_add ──────────────────────────────────────────────────────────
    // 16-bit PCM saturated additive mixer: fn(out, srcA, srcB, byteCount).
    // test: { a: [int16...], b: [int16...] } (sample count = min length).
    // Writes a[]/b[] as int16 into shared src buffers, zeroes two out buffers,
    // calls fn(outX, srcA, srcB, n*2), compares out fingerprints (n*2 bytes).
    // Harness-extension arg_type added 2026-06-04 (c3_batch_ab s3) for 0x005bb5b0.
    // MECHANISM: fn(out, srcA, srcB, byteCount)->void; writes test.a[]/test.b[] as int16 into
    // shared srcA/srcB buffers (pointer-equal both sides), allocates separate outO/outR, calls
    // fn(outX,srcA,srcB,n*2), observes bufFingerprint of n*2 bytes; return value not observed;
    // CONFIG: tests[] of {a:[int16...],b:[int16...]}; NARROW: srcA/srcB shared - in-place src
    // mutation corrupts the second call.
    if (CONFIG.arg_type === 'pcm_sat_add') {
        const PCMCAP = 1024;
        const srcA = Memory.alloc(PCMCAP), srcB = Memory.alloc(PCMCAP);
        const outO = Memory.alloc(PCMCAP), outR = Memory.alloc(PCMCAP);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var a = t.a || [], b = t.b || [];
            var n = Math.min(a.length, b.length);
            for (var j = 0; j < n; j++) { srcA.add(j*2).writeS16(a[j] | 0); srcB.add(j*2).writeS16(b[j] | 0); }
            for (var j = 0; j < (n*2 + 4); j++) { outO.add(j).writeU8(0); outR.add(j).writeU8(0); }
            var errO = null, errR = null;
            try { Orig(outO, srcA, srcB, (n*2) >>> 0); }   catch(e) { errO = e.message; }
            try { Reimpl(outR, srcA, srcB, (n*2) >>> 0); } catch(e) { errR = e.message; }
            var fO = bufFingerprint(outO, n*2), fR = bufFingerprint(outR, n*2);
            var match = (!errO && !errR && fO === fR);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: fO, reimpl: fR, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── guid_from_tag ────────────────────────────────────────────────────────
    // DirectShow MEDIASUBTYPE GUID builder: fn(uint32 tag, uint32* out16).
    // test: tag (uint32). Calls fn(tag, outX) into two 16-byte buffers (preset
    // to 0xCC), compares 16-byte fingerprints.
    // Harness-extension arg_type added 2026-06-04 (c3_batch_ab s3) for 0x005bcb80.
    // MECHANISM: fn(uint32_tag, out_ptr): per test allocates separate 16-byte buffers gO/gR preset
    // to 0xCC, calls fn(tag, buf) for each side; observable = 16-byte bufFingerprint (position-
    // sensitive XOR); separate allocs so pointer values differ between sides; CONFIG:
    // tests=[uint32 tags]; broader: any fn(uint32, byte*) writing a fixed 16-byte structure to an
    // out-pointer.
    if (CONFIG.arg_type === 'guid_from_tag') {
        const gO = Memory.alloc(16), gR = Memory.alloc(16);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var tag = CONFIG.tests[i] >>> 0;
            for (var j = 0; j < 16; j++) { gO.add(j).writeU8(0xCC); gR.add(j).writeU8(0xCC); }
            var errO = null, errR = null;
            try { Orig(tag, gO); }   catch(e) { errO = e.message; }
            try { Reimpl(tag, gR); } catch(e) { errR = e.message; }
            var fO = bufFingerprint(gO, 16), fR = bufFingerprint(gR, 16);
            var match = (!errO && !errR && fO === fR);
            results.push({ idx: i, input: JSON.stringify(tag),
                           original: fO, reimpl: fR, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── ptr_zero_pair ────────────────────────────────────────────────────────
    // fn(uint32* p): zeroes p[0] and p[1]. Preload both dwords with a sentinel
    // plus a guard dword at +8 (must stay untouched), call, compare 12 bytes.
    // test: sentinel (uint32). Harness-extension added 2026-06-04 (c3_batch_ab s3) for 0x005bc450.
    // MECHANISM: fn(uint32* p)->void; allocates separate 16-byte buffers per side, preloads p[0]
    // and p[1] with sentinel and p[2] with guard 0xA5A5A5A5, calls Orig/Reimpl, compares
    // bufFingerprint of all 12 bytes; CONFIG: tests[] of sentinel uint32 values; NARROW: hardcodes
    // exactly-2-dword write assumption with fixed guard at +8; no configurable field count or
    // stride.
    if (CONFIG.arg_type === 'ptr_zero_pair') {
        const zO = Memory.alloc(16), zR = Memory.alloc(16);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var sv = CONFIG.tests[i] >>> 0;
            zO.writeU32(sv); zO.add(4).writeU32(sv); zO.add(8).writeU32(0xA5A5A5A5);
            zR.writeU32(sv); zR.add(4).writeU32(sv); zR.add(8).writeU32(0xA5A5A5A5);
            var errO = null, errR = null;
            try { Orig(zO); }   catch(e) { errO = e.message; }
            try { Reimpl(zR); } catch(e) { errR = e.message; }
            var fO = bufFingerprint(zO, 12), fR = bufFingerprint(zR, 12);
            var match = (!errO && !errR && fO === fR);
            results.push({ idx: i, input: JSON.stringify(sv),
                           original: fO, reimpl: fR, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── renderer_field3c_set ─────────────────────────────────────────────────
    // fn(int p1, uint32 v): writes v to *(p1+0x3c); if (*(byte*)(p1+0x78)&8)
    // also mirrors v to *(*(int*)(p1+0x11c)+0x34). Builds a 0x200-byte struct
    // with hwvoice embedded at +0x140 (so +0x11c -> base+0x140, mirror at +0x174).
    // test: { val: uint32, hw: 0|1 }. Observable: [+0x3c]:[hwvoice+0x34] hex.
    // Harness-extension arg_type added 2026-06-04 (c3_batch_ab s3) for 0x005baf40.
    // MECHANISM: fn(struct_ptr, uint32 val)->void; harness allocates a 0x200-byte struct per side,
    // sets bit-3 flag at +0x78 (hw path), embeds self-pointer at +0x11c pointing to hwvoice block
    // at +0x140, seeds 0xDEADBEEF sentinels at +0x3c and +0x174, calls fn(sX,val), observes
    // "[+0x3c_hex]:[+0x174_hex]"; CONFIG: tests[] of {val:uint32,hw:0|1}; NARROW: all offsets
    // hardcoded (0x3c,0x78,0x11c,0x140,0x174), no CONFIG keys to reparameterise.
    if (CONFIG.arg_type === 'renderer_field3c_set') {
        var STRUCTSZ = 0x200, HWOFF = 0x140;
        var sO = Memory.alloc(STRUCTSZ), sR = Memory.alloc(STRUCTSZ);
        var packU32 = function (p) { return ('00000000' + (p.readU32()>>>0).toString(16)).slice(-8); };
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var val = t.val >>> 0, hw = t.hw ? 1 : 0;
            var errO = null, errR = null;
            [sO, sR].forEach(function (s) {
                for (var j = 0; j < STRUCTSZ; j++) s.add(j).writeU8(0);
                s.add(0x78).writeU8(hw ? 0x08 : 0x00);
                s.add(0x11c).writePointer(s.add(HWOFF));
                s.add(0x3c).writeU32(0xDEADBEEF);
                s.add(HWOFF + 0x34).writeU32(0xDEADBEEF);
            });
            try { Orig(sO, val); }   catch(e) { errO = e.message; }
            try { Reimpl(sR, val); } catch(e) { errR = e.message; }
            var fO = packU32(sO.add(0x3c)) + ':' + packU32(sO.add(HWOFF + 0x34));
            var fR = packU32(sR.add(0x3c)) + ':' + packU32(sR.add(HWOFF + 0x34));
            var match = (!errO && !errR && fO === fR);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: fO, reimpl: fR, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── source_loop_set ──────────────────────────────────────────────────────
    // fn(int src, int loop): software path (caps[+0x50]&8==0) sets/clears bit
    // 0x800 in *(src+0x28); hardware path sets/clears bit 0x8 in
    // *(*(int*)(src+0x11c)+0xcc). Builds a 0x200-byte struct with caps embedded
    // at +0x180 (so +0x94 -> base+0x180, caps+0x50 at +0x1d0) and hwvoice at
    // +0x100 (so +0x11c -> base+0x100, control word at +0x1cc).
    // test: { loop: 0|1, hw: 0|1, pre28: uint32, prehw: uint32 }.
    // Observable: [+0x28]:[hwvoice+0xcc] hex.
    // Harness-extension arg_type added 2026-06-04 (c3_batch_ab s3) for 0x005b9410.
    // MECHANISM: fn over a 0x200-byte scratch struct per side, with a nested hardware-voice sub-
    // struct at +0x100 and caps at +0x180; per-test {loop,hw,pre28,prehw} pre-seed [+0x28] and the
    // hw field before the call; observable is [+0x28]:[hwvoice+0xcc] as hex. Both sides get
    // identically seeded but separately allocated structs, so only field CONTENT is compared,
    // never addresses.
    if (CONFIG.arg_type === 'source_loop_set') {
        var SLS_SZ = 0x200, CAPSOFF = 0x180, SLS_HWOFF = 0x100;
        var slO = Memory.alloc(SLS_SZ), slR = Memory.alloc(SLS_SZ);
        var slPack = function (p) { return ('00000000' + (p.readU32()>>>0).toString(16)).slice(-8); };
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var loop = t.loop ? 1 : 0, hw = t.hw ? 1 : 0;
            var pre28 = (t.pre28 != null ? t.pre28 : 0) >>> 0;
            var prehw = (t.prehw != null ? t.prehw : 0) >>> 0;
            var errO = null, errR = null;
            [slO, slR].forEach(function (s) {
                for (var j = 0; j < SLS_SZ; j++) s.add(j).writeU8(0);
                s.add(0x94).writePointer(s.add(CAPSOFF));
                s.add(CAPSOFF + 0x50).writeU8(hw ? 0x08 : 0x00);
                s.add(0x11c).writePointer(s.add(SLS_HWOFF));
                s.add(0x28).writeU32(pre28);
                s.add(SLS_HWOFF + 0xcc).writeU32(prehw);
            });
            try { Orig(slO, loop); }   catch(e) { errO = e.message; }
            try { Reimpl(slR, loop); } catch(e) { errR = e.message; }
            var fO = slPack(slO.add(0x28)) + ':' + slPack(slO.add(SLS_HWOFF + 0xcc));
            var fR = slPack(slR.add(0x28)) + ':' + slPack(slR.add(SLS_HWOFF + 0xcc));
            var match = (!errO && !errR && fO === fR);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: fO, reimpl: fR, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── endian_pack ──────────────────────────────────────────────────────────
    // Tests AudioFieldEndianPack-style fn(int **out_ptr_ptr, uint *src, int size).
    // For each test {src_val, size}: allocate an 8-byte output buffer, write src_val
    // into a 4-byte source slot; construct a pointer-to-pointer (out_ptr_ptr) and
    // call fn. Read the output buffer bytes as a fingerprint and compare orig/reimpl.
    // The out buffer is reset to 0 before each call.
    // MECHANISM: Call shape fn(int** out_ptr_ptr, uint* src, int size) with fully per-side harness
    // buffers; seeds src_val into 4-byte srcSlot, sets ptrSlot -> 8-byte zeroed outBuf, resets
    // outBuf before each call; observes 4-byte XOR fingerprint of outBuf. CONFIG: tests[].src_val,
    // tests[].size. Applies to any fn taking a double-pointer output + uint src + int size that
    // writes <=4 bytes via the out_ptr_ptr chain.
    if (CONFIG.arg_type === 'endian_pack') {
        const outBufA   = Memory.alloc(16);
        const outBufB   = Memory.alloc(16);
        const ptrSlotA  = Memory.alloc(4);
        const ptrSlotB  = Memory.alloc(4);
        const srcSlotA  = Memory.alloc(4);
        const srcSlotB  = Memory.alloc(4);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t       = CONFIG.tests[i];
            var srcVal  = t.src_val >>> 0;
            var size    = t.size | 0;
            // Reset output buffers.
            for (var j = 0; j < 8; j++) { outBufA.add(j).writeU8(0); outBufB.add(j).writeU8(0); }
            ptrSlotA.writePointer(outBufA);
            ptrSlotB.writePointer(outBufB);
            srcSlotA.writeU32(srcVal);
            srcSlotB.writeU32(srcVal);
            var errO = null, errR = null;
            try { Orig(ptrSlotA, srcSlotA, size); }   catch(e) { errO = e.message; }
            try { Reimpl(ptrSlotB, srcSlotB, size); } catch(e) { errR = e.message; }
            var fA = bufFingerprint(outBufA, 4);
            var fB = bufFingerprint(outBufB, 4);
            var match = (!errO && !errR && fA === fB);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: fA, reimpl: fB, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── wavefmt_copy ─────────────────────────────────────────────────────────
    // Tests AudioWaveFmtCopy-style fn(src_ptr, dst_ptr, swap_flag) -> src_ptr.
    // For each test {src:[16 bytes], swap}: write src data into srcBuf,
    // zero dstBuf, call fn, fingerprint dstBuf (16 bytes). Compare orig/reimpl.
    // MECHANISM: Allocates 4 independent 16-byte buffers (srcBufA/B, dstBufA/B); for each test
    // {src:[16 bytes], swap}: fills both src bufs identically, zeroes both dst bufs; calls
    // Orig(srcBufA, dstBufA, swap?dstBufA:ptr(0)) and Reimpl(srcBufB, dstBufB,
    // swap?dstBufB:ptr(0)) - when swap=1 third arg aliases dst (in-place swap path), when swap=0
    // third arg is NULL; observes 16-byte dst fingerprint only; fn return value (src_ptr) is
    // discarded and not compared; no CONFIG beyond `tests`. NARROW: fixed 16-byte struct with no
    // CONFIG.struct_size.
    if (CONFIG.arg_type === 'wavefmt_copy') {
        const srcBufA = Memory.alloc(16);
        const srcBufB = Memory.alloc(16);
        const dstBufA = Memory.alloc(16);
        const dstBufB = Memory.alloc(16);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t    = CONFIG.tests[i];
            var src  = t.src || [];
            var swap = t.swap | 0;
            for (var j = 0; j < 16; j++) {
                srcBufA.add(j).writeU8(src[j] | 0);
                srcBufB.add(j).writeU8(src[j] | 0);
                dstBufA.add(j).writeU8(0);
                dstBufB.add(j).writeU8(0);
            }
            var errO = null, errR = null;
            try { Orig(srcBufA, dstBufA, swap ? dstBufA : ptr(0)); }   catch(e) { errO = e.message; }
            try { Reimpl(srcBufB, dstBufB, swap ? dstBufB : ptr(0)); } catch(e) { errR = e.message; }
            var fA = bufFingerprint(dstBufA, 16);
            var fB = bufFingerprint(dstBufB, 16);
            var match = (!errO && !errR && fA === fB);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: fA, reimpl: fB, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── alloc_check ──────────────────────────────────────────────────────────
    // Call(size, tag) for each test; encode result as (align_mod4 * 256 + header_diff).
    // A correct aligned alloc with 4-byte aligned heap returns result = 4.
    // MECHANISM: Calls fn(size, CONFIG.alloc_tag) for each test size; encodes result as
    // (ptr_align_mod4x256)+(ptr-header_ptr_before_allocation); returns -1 on null. Observes
    // alignment and header-distance ONLY - NOT allocated content or size, so two allocators
    // differing only in fill are a false-GREEN. No CONFIG beyond alloc_tag.
    if (CONFIG.arg_type === 'alloc_check') {
        var allocTag = (CONFIG.alloc_tag | 0);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var size = CONFIG.tests[i] | 0;
            var orig = null, reim = null, errO = null, errR = null;
            function checkAlloc(fn, sz, tag) {
                var p = fn(sz, tag);
                if (!p || p.isNull()) return -1;
                var pInt = parseInt(p.toString(), 16);
                var hInt = parseInt(p.sub(4).readPointer().toString(), 16);
                return (((pInt & 3) * 256) + (pInt - hInt)) >>> 0;
            }
            try { orig = checkAlloc(Orig,   size, allocTag); } catch(e) { errO = e.message; }
            try { reim = checkAlloc(Reimpl, size, allocTag); } catch(e) { errR = e.message; }
            var match = (errO === null && errR === null && orig === reim);
            results.push({ idx: i, input: size,
                           original: orig, reimpl: reim, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── free_via_alloc ───────────────────────────────────────────────────────
    // Allocate two blocks (via alloc_rva), free each via Orig/Reimpl.
    // Success = 1 (no crash), failure = 0 or error.
    // MECHANISM: fn(ptr): allocates separate blocks via AllocFn(size, CONFIG.alloc_tag) using
    // CONFIG.alloc_rva_str, calls Orig(pO) and Reimpl(pR) on their respective blocks; observable =
    // crash-absence flag only (1=ok, 0=error) - no post-free read-back; a reimpl that silently no-
    // ops passes; CONFIG: alloc_rva_str, alloc_tag; tests=[int sizes].
    if (CONFIG.arg_type === 'free_via_alloc') {
        var allocRva = ptr(CONFIG.alloc_rva_str);
        var allocTag = (CONFIG.alloc_tag | 0);
        var AllocFn = new NativeFunction(allocRva, 'pointer', ['int32', 'int32'], 'mscdecl');
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var size = CONFIG.tests[i] | 0;
            var orig = 0, reim = 0, errO = null, errR = null;
            var pO = null, pR = null;
            try { pO = AllocFn(size, allocTag); } catch(e) { errO = 'alloc_failed:' + e.message; }
            try { pR = AllocFn(size, allocTag); } catch(e) { errR = 'alloc_failed:' + e.message; }
            try { if (pO && !pO.isNull()) { Orig(pO);   orig = 1; } } catch(e) { errO = e.message; }
            try { if (pR && !pR.isNull()) { Reimpl(pR); reim = 1; } } catch(e) { errR = e.message; }
            var match = (errO === null && errR === null && orig === reim);
            results.push({ idx: i, input: size,
                           original: orig, reimpl: reim, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── fmt_key_compare ──────────────────────────────────────────────────────
    // For AudioFmtKeyCompare: fn(byte* a, byte* b) → int (-1/0/+1).
    // Each test is { a: [16 bytes], b: [16 bytes] }.
    // Allocates a 32-byte scratch buffer; writes a to [0..15], b to [16..31].
    // Calls fn(buf, buf+16) for both Orig and Reimpl; compares return int.
    // MECHANISM: Call shape fn(byte* a, byte* b) -> int; harness allocates single 32-byte fkBuf,
    // writes a[0..15] at buf+0 and b[0..15] at buf+16, calls fn(buf, buf+16) for each side;
    // observes signed int return. CONFIG: tests[].a (16 bytes), tests[].b (16 bytes). Same fkBuf
    // ptr across both sides; not re-zeroed between orig/reimpl calls but safe if fn is read-only
    // on its inputs. Applies to any pure 16-byte-key comparator returning -1/0/+1.
    if (CONFIG.arg_type === 'fmt_key_compare') {
        const fkBuf = Memory.alloc(32);
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const aBytes = t.a;
            const bBytes = t.b;
            for (let j = 0; j < 16; j++) {
                fkBuf.add(j).writeU8(aBytes[j] >>> 0);
                fkBuf.add(16 + j).writeU8(bBytes[j] >>> 0);
            }
            let orig = 0, reim = 0, errO = null, errR = null;
            try { orig = Orig(fkBuf, fkBuf.add(16));   orig = orig | 0; } catch(e) { errO = e.message; }
            try { reim = Reimpl(fkBuf, fkBuf.add(16)); reim = reim | 0; } catch(e) { errR = e.message; }
            const match = (errO === null && errR === null && orig === reim);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: orig, reimpl: reim, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── font_ctx_float2 ──────────────────────────────────────────────────────
    // For FontCtx_SetScale / FontCtx_SetTranslation: fn(float sx, float sy) → uint32.
    // Strategy: for each test {sx, sy}, write sentinel 0xDEADBEEF to dirty-flag
    // DAT_00912bd8, call fn(sx,sy), read back dirty flag (must be 0 if fn worked).
    // Also read back the uint32 return value (must be 1).
    // Observable = (ret << 16) | dirtyFlagReadback. Both paths must match.
    // Dirty flag is restored to its pre-test value after each pair.
    //
    // Prelude (added 2026-05-24 phase-a1): call MASHED's original
    // FontSys_InitRenderState (0x00552c10) once before the test loop to
    // guarantee g_FontCtxPtrs[0] is allocated. Without this, the function
    // derefs a NULL slot ptr and both sides AV identically at offset 0.
    // MECHANISM: NARROW: hardcodes FontSys_InitRenderState prelude (0x00552c10) and dirty-flag
    // global (0x00912bd8); two float stack args (sx, sy); writes sentinel 0xDEADBEEF to the flag
    // before each call; observes packed (uint32_ret<<16)|dirty_flag_readback; restores flag
    // between sides. No CONFIG parameterization - only fits font-ctx float2 fns requiring that
    // specific prelude and dirty flag.
    if (CONFIG.arg_type === 'font_ctx_float2') {
        const pDirty = ptr('0x00912bd8');
        // Idempotent one-time setup: allocate slot 0.
        const InitRenderState = new NativeFunction(ptr('0x00552c10'), 'uint32', [], 'mscdecl');
        try { InitRenderState(); } catch (e) {
            send({ type: 'error', msg: 'FontSys_InitRenderState prelude failed: ' + e.message });
            return;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const sx = t[0], sy = t[1];
            let origV = null, reimV = null, errO = null, errR = null;
            const savedDirty = pDirty.readU32();
            try {
                pDirty.writeU32(0xDEADBEEF);
                const ret = Orig(sx, sy);
                const df  = pDirty.readU32();
                origV = (((ret >>> 0) & 0xffff) << 16) | (df & 0xffff);
            } catch(e) { errO = e.message; }
            pDirty.writeU32(savedDirty);
            try {
                pDirty.writeU32(0xDEADBEEF);
                const ret = Reimpl(sx, sy);
                const df  = pDirty.readU32();
                reimV = (((ret >>> 0) & 0xffff) << 16) | (df & 0xffff);
            } catch(e) { errR = e.message; }
            pDirty.writeU32(savedDirty);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── font_matrix_push ─────────────────────────────────────────────────────
    // For FontMatrix_Push: fn(void) → bool.
    // Strategy: for each test depth_val, write depth to DAT_00912b04, call fn,
    // read back bool return value and new depth. Pack into uint32 for comparison.
    // Observable = (retBool & 1) | ((new_depth & 0xff) << 8).
    // depth is restored after each pair.
    //
    // Prelude (added 2026-05-24 phase-a1): call MASHED's original
    // FontSys_InitRenderState (0x00552c10) once before the test loop to
    // guarantee g_FontCtxPtrs[0] is allocated. FontMatrix_Push at initial
    // depth=N copies 64 bytes FROM g_FontCtxPtrs[N] — needs that slot valid.
    // Only initial depth 0 (copies from slot 0, valid post-prelude) and
    // initial depth 31 (overflow early-out, doesn't touch ctx) are
    // safely exercisable. Tests with depth in (1..30) deref unallocated
    // slots and AV both sides identically — they're not on the registry's
    // test list, but the prelude is still required for depth=0.
    // MECHANISM: fn() no args: calls original FontSys_InitRenderState(0x00552c10) once as prelude;
    // per test seeds t.depth into hardcoded global 0x00912b04, calls fn(), reads bool ret and new
    // depth packed as (ret&1)|((depth&0xff)<<8), then restores depth; NARROW: globals 0x00912b04
    // and prelude address 0x00552c10 are hardcoded; CONFIG: tests=[{depth}].
    if (CONFIG.arg_type === 'font_matrix_push') {
        const pDepth = ptr('0x00912b04');
        // Idempotent one-time setup: allocate slot 0 (and reset depth to 0).
        const InitRenderState = new NativeFunction(ptr('0x00552c10'), 'uint32', [], 'mscdecl');
        try { InitRenderState(); } catch (e) {
            send({ type: 'error', msg: 'FontSys_InitRenderState prelude failed: ' + e.message });
            return;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];  // { depth } — initial depth to inject
            const testDepth = (t.depth | 0) >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            const savedDepth = pDepth.readU32();
            try {
                pDepth.writeU32(testDepth);
                const ret = Orig();
                const newDepth = pDepth.readU32();
                origV = ((ret ? 1 : 0)) | ((newDepth & 0xff) << 8);
            } catch(e) { errO = e.message; }
            pDepth.writeU32(savedDepth);
            try {
                pDepth.writeU32(testDepth);
                const ret = Reimpl();
                const newDepth = pDepth.readU32();
                reimV = ((ret ? 1 : 0)) | ((newDepth & 0xff) << 8);
            } catch(e) { errR = e.message; }
            pDepth.writeU32(savedDepth);
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── int_outbuf4 ──────────────────────────────────────────────────────────
    // For PlayerColorTableGet-style: fn(int idx, byte* out_buf4) — void return,
    // writes 4 bytes into out_buf4. Each test is a single integer index.
    // Strategy: allocate two 4-byte buffers (one per path), zero each before
    // each call, call fn(idx, buf), read back 4 bytes as packed uint32
    // (little-endian fingerprint). Both paths must produce identical output.
    // MECHANISM: fn(int_idx, out_buf4_ptr): separate 4-byte buffers per side (ioBufA for Orig,
    // ioBufB for Reimpl), zeroed before each call; observable = 4 bytes read back as uint32
    // (little-endian fingerprint); return value is NOT observed (void return assumed); CONFIG:
    // tests=[int indices]; broader: any fn(int, byte*) writing exactly 4 bytes to an out-pointer
    // with void return.
    if (CONFIG.arg_type === 'int_outbuf4') {
        const ioBufA = Memory.alloc(4);
        const ioBufB = Memory.alloc(4);
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const idx = CONFIG.tests[i] | 0;
            let origV = null, reimV = null, errO = null, errR = null;
            // Zero both buffers before each call.
            ioBufA.writeU32(0);
            ioBufB.writeU32(0);
            try {
                Orig(idx, ioBufA);
                origV = ioBufA.readU32();
            } catch(e) { errO = e.message; }
            try {
                Reimpl(idx, ioBufB);
                reimV = ioBufB.readU32();
            } catch(e) { errR = e.message; }
            results.push({ idx: i, input: idx,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── outbuf_only (promote-round-19 harness-ext, SWEEP-CRITICAL) ────────────
    // For single-out-pointer void functions: fn(T* out) — no scalar arg, writes
    // a computed result into the caller buffer. Allocates two CONFIG.out_buf_size
    // (default 16) buffers, zeros each before the call, calls fn(buf), reads the
    // buffer back as a little-endian packed-dword fingerprint, compares.
    // CONFIG.tests sets call count; its values are also used as the seed when
    // CONFIG.seed_global is set. Optional CONFIG:
    //   fold_ret      — XOR the function's return value into the fingerprint
    //                   (for getters that BOTH write *out AND return a value,
    //                    e.g. 00484c70: *out=count, return=base).
    //   seed_global   — hex addr; before each call save it, write tests[i], call
    //                   fn(buf), read buf, restore. Makes a global-copying out
    //                   getter deterministic + discriminating (e.g. 0041da90,
    //                   whose *out = a per-frame-moving DAT_0063d588).
    // Unlocks the single-out-ptr class: SlotSortByModeScore 0x0040b620
    // (round 19), plus 0041da90 / 00484c70 / 00495270 (round 20).
    // MECHANISM: fn(T* out): void; harness allocates two separate out_buf_size-byte bufs (default
    // 16), zeros each before the call, calls fn(buf), reads back as packed-dword fingerprint;
    // CONFIG.fold_ret=true also XORs the return value in (for dual-output getters that both write
    // *out and return a value); CONFIG.seed_global seeds a global with tests[i] before each call
    // (for global-copying getters) and restores it after; broadly fits any fn(ptr) that writes a
    // fixed-size result, regardless of domain.
    if (CONFIG.arg_type === 'outbuf_only') {
        const OB_LEN   = (CONFIG.out_buf_size | 0) || 16;
        const foldRet  = CONFIG.fold_ret ? true : false;
        const seedAddr = CONFIG.seed_global ? ptr(CONFIG.seed_global) : null;
        const obA = Memory.alloc(OB_LEN);
        const obB = Memory.alloc(OB_LEN);
        function fpOb(buf, ret) {
            let s = '';
            for (let k = 0; k + 3 < OB_LEN; k += 4) {
                s += ('00000000' + (buf.add(k).readU32() >>> 0).toString(16)).slice(-8);
            }
            if (foldRet) s += ':' + ('00000000' + ((ret >>> 0)).toString(16)).slice(-8);
            return s;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            let origV = null, reimV = null, errO = null, errR = null;
            for (let k = 0; k < OB_LEN; k++) { obA.add(k).writeU8(0); obB.add(k).writeU8(0); }
            let savedSeed = null;
            if (seedAddr) { savedSeed = seedAddr.readU32(); seedAddr.writeU32(CONFIG.tests[i] >>> 0); }
            try { const r = Orig(obA);    origV = fpOb(obA, r); } catch(e) { errO = e.message; }
            try { const r = Reimpl(obB);  reimV = fpOb(obB, r); } catch(e) { errR = e.message; }
            if (seedAddr) seedAddr.writeU32(savedSeed >>> 0);
            results.push({ idx: i, input: (seedAddr ? (CONFIG.tests[i] >>> 0) : i),
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── int2out (promote-round-25 harness-ext, SWEEP-CRITICAL) ───────────────
    // For two-out-pointer getters: T fn(int idx, U* out_a, U* out_b) — writes a
    // 4-byte value to each out and returns a value. Compares both out buffers
    // AND the return (packed fingerprint "<a>,<b>:<ret>"). CONFIG.tests is a
    // list of int indices. Validated on 0x0046cbb0 (per-car state pair getter).
    // MECHANISM: fn(int_idx, out_a_ptr, out_b_ptr): separate 4-byte buffer pairs per side (a1/a2
    // for Orig, b1/b2 for Reimpl), zeroed before each call; observable = both out-dwords and
    // return value packed as \<a_hex\>,\<b_hex\>:\<ret_hex\> - all three must match; CONFIG:
    // tests=[int indices]; broader: any fn(int, U*, U*) writing 4 bytes to each of two out-
    // pointers with a return value.
    if (CONFIG.arg_type === 'int2out') {
        const a1 = Memory.alloc(4), a2 = Memory.alloc(4);
        const b1 = Memory.alloc(4), b2 = Memory.alloc(4);
        function h8(v) { return ('00000000' + (v >>> 0).toString(16)).slice(-8); }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const idx = CONFIG.tests[i] | 0;
            let origV = null, reimV = null, errO = null, errR = null;
            a1.writeU32(0); a2.writeU32(0); b1.writeU32(0); b2.writeU32(0);
            try { const r = Orig(idx, a1, a2);
                  origV = h8(a1.readU32()) + ',' + h8(a2.readU32()) + ':' + h8(r); } catch(e) { errO = e.message; }
            try { const r = Reimpl(idx, b1, b2);
                  reimV = h8(b1.readU32()) + ',' + h8(b2.readU32()) + ':' + h8(r); } catch(e) { errR = e.message; }
            results.push({ idx: i, input: idx,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── esi_idx_ecx_outbuf4 ───────────────────────────────────────────────────
    // SWEEP-CRITICAL (new handler; frida-sweep does NOT auto-merge diff_template.js).
    // For register-convention LEAVES like FUN_00413bc0: the integer index arrives
    // in ESI and the out-pointer in ECX; the function writes 4 floats to
    // [ECX..ECX+0xc] and ends with a PLAIN ret (register-only, no stack args).
    // Neither int_outbuf4 (passes args cdecl-on-stack) nor fastcall_reg (seeds
    // only ECX/EDX and captures EAX) can drive it. We build an 18-byte trampoline
    // per side that PRESERVES the caller's ESI (callee-saved), seeds ESI=idx and
    // ECX=<16-byte scratch>, CALLs the target, restores ESI, and rets; then read
    // the 4 output floats back as a packed u32x4 fingerprint.
    //
    //   56                 push esi                 ; save caller ESI
    //   BE ?? ?? ?? ??     mov  esi, imm32(idx)     ; imm patched at +2 per test
    //   B9 ?? ?? ?? ??     mov  ecx, imm32(scratch) ; imm patched at +7 (constant)
    //   E8 ?? ?? ?? ??     call rel32 -> target     ; rel32 at +12
    //   5E                 pop  esi                 ; restore caller ESI
    //   C3                 ret
    //
    // CONFIG.tests : scalar integer indices (incl. negatives / >=5 / the 3 case).
    // MECHANISM: Per-side `push esi; mov esi,idx; mov ecx,scratch_ptr; call target; pop esi; ret`
    // trampoline seeds ESI=integer index (imm32 patched per test) and ECX=per-side 16-byte zeroed
    // scratch-buffer ptr (constant per side). Observes all 16 scratch bytes as 4xu32 hex
    // fingerprint; no return value captured. CONFIG: tests[] = integer indices (including
    // negatives/OOB). Applies to any void fn delivering an index in ESI and output-buffer ptr in
    // ECX writing <=16 bytes.
    if (CONFIG.arg_type === 'esi_idx_ecx_outbuf4') {
        const scratchO = Memory.alloc(16);
        const scratchR = Memory.alloc(16);
        function buildEsiTramp(targetAddr, scratchAddr) {
            const code = Memory.alloc(Process.pageSize);
            Memory.patchCode(code, 18, function (cw) {
                const w = new X86Writer(cw, { pc: code });
                w.putU8(0x56);                              // push esi
                w.putBytes([0xBE, 0x00, 0x00, 0x00, 0x00]); // mov esi, 0  (patched +2)
                w.putBytes([0xB9, 0x00, 0x00, 0x00, 0x00]); // mov ecx, 0  (patched +7)
                w.putU8(0xE8);                              // call rel32
                const rel = targetAddr.sub(code.add(16)).toInt32();
                w.putBytes([rel & 0xff, (rel >>> 8) & 0xff,
                            (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
                w.putU8(0x5E);                              // pop esi
                w.putU8(0xC3);                              // ret
                w.flush();
            });
            // ECX imm = scratch address (constant for this side); patch once.
            code.add(7).writeU32(parseInt(scratchAddr.toString(), 16) >>> 0);
            return code;
        }
        const trampO = buildEsiTramp(TARGET_ADDR, scratchO);
        const trampR = buildEsiTramp(reimplAddr, scratchR);
        const FnO = new NativeFunction(trampO, 'void', [], 'mscdecl');
        const FnR = new NativeFunction(trampR, 'void', [], 'mscdecl');
        const packU32x4 = function (p) {
            let s = '';
            for (let k = 0; k < 16; k += 4) {
                const v = p.add(k).readU32() >>> 0;
                s += ('00000000' + v.toString(16)).slice(-8);
            }
            return '0x' + s;
        };
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const idx = CONFIG.tests[i] | 0;
            let origV = null, reimV = null, errO = null, errR = null;
            // Patch the ESI (idx) imm window on both sides.
            trampO.add(2).writeU32(idx >>> 0);
            trampR.add(2).writeU32(idx >>> 0);
            // Zero the 4-float scratch on both sides before each call.
            for (let k = 0; k < 16; k += 4) { scratchO.add(k).writeU32(0); scratchR.add(k).writeU32(0); }
            try { FnO(); origV = packU32x4(scratchO); } catch (e) { errO = e.message; }
            try { FnR(); reimV = packU32x4(scratchR); } catch (e) { errR = e.message; }
            results.push({ idx: i, input: idx,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── eax_ptr_ebx_outbuf (area-frontend r3, SWEEP-CRITICAL) ────────────────
    // Register-convention leaf: EAX = source pointer, EBX = destination pointer,
    // no stack args, plain RET. Same trampoline shape as esi_idx_ecx_outbuf4 but
    // with EAX (per-test seeded src) + EBX (per-side out buffer). Each test is a
    // uint16 array written verbatim into src (the fn reads its own length prefix);
    // src and dst are SEPARATE per side (the fn only reads src / writes dst, never
    // stores src ptr into dst, so no shared-buffer aliasing is needed). Observable
    // is the dst buffer fingerprinted as FP_LEN/2 u16 words. NOTE for frida-sweep:
    // diff_template.js is NOT auto-merged — this handler must land in the sweep.
    // First consumer: 0x004277a0 TextCtrlCodeRemap.
    if (CONFIG.arg_type === 'eax_ptr_ebx_outbuf') {
        const SRC_LEN = (CONFIG.src_len | 0) || 128;
        const DST_LEN = (CONFIG.dst_len | 0) || 128;
        const FP_LEN  = (CONFIG.fp_len  | 0) || 64;
        const srcO = Memory.alloc(SRC_LEN), srcR = Memory.alloc(SRC_LEN);
        const dstO = Memory.alloc(DST_LEN), dstR = Memory.alloc(DST_LEN);
        function buildEaxEbxTramp(targetAddr, srcAddr, dstAddr) {
            const code = Memory.alloc(Process.pageSize);
            Memory.patchCode(code, 18, function (cw) {
                const w = new X86Writer(cw, { pc: code });
                w.putU8(0x53);                              // push ebx
                w.putBytes([0xB8, 0, 0, 0, 0]);            // mov eax, srcAddr (patch +2)
                w.putBytes([0xBB, 0, 0, 0, 0]);            // mov ebx, dstAddr (patch +7)
                w.putU8(0xE8);                              // call rel32
                const rel = targetAddr.sub(code.add(16)).toInt32();
                w.putBytes([rel & 0xff, (rel >>> 8) & 0xff,
                            (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
                w.putU8(0x5B);                              // pop ebx
                w.putU8(0xC3);                              // ret
                w.flush();
            });
            code.add(2).writeU32(parseInt(srcAddr.toString(), 16) >>> 0);
            code.add(7).writeU32(parseInt(dstAddr.toString(), 16) >>> 0);
            return code;
        }
        const trampO = buildEaxEbxTramp(TARGET_ADDR, srcO, dstO);
        const trampR = buildEaxEbxTramp(reimplAddr,  srcR, dstR);
        const FnO = new NativeFunction(trampO, 'void', [], 'mscdecl');
        const FnR = new NativeFunction(trampR, 'void', [], 'mscdecl');
        function seedSrc(p, arr) {
            for (let k = 0; k < SRC_LEN; k += 2) p.add(k).writeU16(0);
            for (let k = 0; k < arr.length; k++) p.add(k * 2).writeU16(arr[k] & 0xffff);
        }
        function fpDst(p) {
            let s = '';
            for (let k = 0; k < FP_LEN; k += 2) {
                s += ('0000' + (p.add(k).readU16() & 0xffff).toString(16)).slice(-4);
            }
            return '0x' + s;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const arr = CONFIG.tests[i];
            let origV = null, reimV = null, errO = null, errR = null;
            seedSrc(srcO, arr);
            for (let k = 0; k < DST_LEN; k += 2) dstO.add(k).writeU16(0);
            try { FnO(); origV = fpDst(dstO); } catch (e) { errO = e.message; }
            seedSrc(srcR, arr);
            for (let k = 0; k < DST_LEN; k += 2) dstR.add(k).writeU16(0);
            try { FnR(); reimV = fpDst(dstR); } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(arr),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── reg_this_callee_stub (orch-iter6, SWEEP-CRITICAL) ─────────────────────
    // FIRST handler that intercepts CALLEES rather than seeding inputs. For the
    // render particle-emitter ctor family (EBX/ESI/EAX-implicit `this`) whose
    // body calls into the LIVE RenderWare object graph and so is not leaf-
    // callable cold: seed the register-`this` + a shared scratch clump, then
    // Interceptor.replace the live-RW callees with deterministic stubs so the
    // ctor produces a fixed `this`-struct that a correct port reproduces
    // bit-for-bit. Design spec + evidence:
    // re/analysis/plans/reg_this_callee_stub_handler_spec.md and
    // re/analysis/callers_c2_unblock/portcap_0x0041ad60.md.
    //
    // Proving case 0x0041ad60 (17-atomic): body is
    //   FUN_004b3fc0(clump, &buf);                       // fills buf[atom_count]
    //   this[0x5c]=clump; this[0x60]=*(clump+4);
    //   FUN_004b6520(this, 0x50);                         // zero-init (noop-stubbed)
    //   for i in 0..atom_count-1:
    //     idx = FUN_004b5190(buf[i],0,0); this[idx*4]=buf[i];
    //
    // Callee stubs (installed once, reverted after; GLOBAL by RVA so BOTH the
    // original ctor and the reimpl export hit the identical stub):
    //   callee_fill  (0x004b3fc0): (clump, buf) => write HANDLE_BASE+i to buf[i].
    //                arg0=clump, arg1=buf per the original's push order
    //                (PUSH buf deeper, PUSH clump topmost => cdecl clump first).
    //   callee_index (0x004b5190): (handle,0,0) => handle - HANDLE_BASE, so the
    //                handles scatter to slots 0..atom_count-1 deterministically
    //                (also short-circuits the real [handle+0x18] deref).
    //   callee_zero  (0x004b6520): noop (both sides pre-zero scratchThis anyway).
    //   callee_color (optional, 0x004b5260 for 0x0041cd20): noop.
    //
    // CONFIG: this_reg ('ebx'|'esi'|'eax'), struct_size, atom_count,
    //   callee_fill_str / callee_index_str / callee_zero_str [/ callee_color_str],
    //   signature {ret:'void', args:[]}, tests = [{clump_frame, handle_base}, ...].
    // The shared scratch clump means this[0x5c] (= clump ADDR) matches across
    // sides; scratchThis is per-side for the diff.
    // MECHANISM: Delivers `this` in CONFIG.this_reg (default 'ebx') via x86 trampoline and seeds
    // EAX with a shared scratch clump address on both sides; stubs up to 4 live callees
    // (callee_fill_str, callee_index_str, callee_zero_str, callee_color_str) with deterministic
    // NativeCallbacks; shared clump ensures stored this[0x5c] pointer is identical on both sides;
    // observable is the full per-side struct fingerprint (all CONFIG.struct_size bytes as packed
    // hex). CONFIG: `this_reg`, `struct_size`, `atom_count`, `callee_fill_str`,
    // `callee_index_str`, `callee_zero_str`, `callee_color_str`.
    if (CONFIG.arg_type === 'reg_this_callee_stub') {
        const structSize = CONFIG.struct_size || 0x80;
        const atomCount  = CONFIG.atom_count  || 17;
        const thisReg    = (CONFIG.this_reg || 'ebx').toLowerCase();
        // mov r32, imm32 opcode by target register (this delivered in a reg).
        const MOV_OP = { eax: 0xB8, ecx: 0xB9, edx: 0xBA, ebx: 0xBB,
                         esp: 0xBC, ebp: 0xBD, esi: 0xBE, edi: 0xBF };
        const thisOp = MOV_OP[thisReg];
        if (thisOp === undefined) { send({ type: 'error', msg: 'bad this_reg ' + thisReg }); return; }

        // Shared scratch clump (SAME address seeded into EAX on BOTH sides so
        // the stored this[0x5c] pointer is identical). +4 holds the per-test
        // frame sentinel.
        const _keep = [];
        const clumpBuf = Memory.alloc(64); _keep.push(clumpBuf);
        const clumpAddr = parseInt(clumpBuf.toString(), 16) >>> 0;

        // Per-side scratch `this` (compared after each call).
        const thisO = Memory.alloc(structSize); _keep.push(thisO);
        const thisR = Memory.alloc(structSize); _keep.push(thisR);

        // Mutable base shared by the stub closures (updated per test).
        let HANDLE_BASE = 0x1000;

        // Install the callee stubs once (reverted at the end).
        const filled = [];
        function replaceCallee(hexStr, cb) {
            if (!hexStr) return;
            const a = ptr(hexStr);
            Interceptor.replace(a, cb);
            filled.push(a);
        }
        const cbFill = new NativeCallback(function (clump, buf) {
            for (let i = 0; i < atomCount; i++) buf.add(i * 4).writeU32((HANDLE_BASE + i) >>> 0);
            return 0;
        }, 'int', ['pointer', 'pointer']);
        const cbIndex = new NativeCallback(function (handle, a, b) {
            return ((handle >>> 0) - (HANDLE_BASE >>> 0)) | 0;
        }, 'int', ['uint32', 'int', 'int']);
        const cbNoop = new NativeCallback(function () { return 0; }, 'int', ['pointer', 'int']);
        const cbColor = new NativeCallback(function () { return 0; }, 'int', ['int', 'pointer']);

        replaceCallee(CONFIG.callee_fill_str,  cbFill);
        replaceCallee(CONFIG.callee_index_str, cbIndex);
        replaceCallee(CONFIG.callee_zero_str,  cbNoop);
        replaceCallee(CONFIG.callee_color_str, cbColor);
        Interceptor.flush();

        // Per-side trampoline:  push ebx/reg? ; mov <thisReg>,thisAddr ;
        //   mov eax,clumpAddr ; call target ; pop ; ret.  We PUSH/POP the this
        // register (callee-saved for ebx/esi/edi) to protect the harness state.
        function buildRegThisTramp(targetAddr, thisAddr) {
            const code = Memory.alloc(Process.pageSize);
            // push reg: 0x50 + regnum, same encoding order as MOV low nibble.
            const REGNUM = { eax:0, ecx:1, edx:2, ebx:3, esp:4, ebp:5, esi:6, edi:7 };
            const pushOp = 0x50 + REGNUM[thisReg];
            Memory.patchCode(code, 18, function (cw) {
                const w = new X86Writer(cw, { pc: code });
                w.putU8(pushOp);                               // push <thisReg>
                w.putBytes([thisOp, 0, 0, 0, 0]);              // mov <thisReg>, thisAddr (patch +2)
                w.putBytes([0xB8, 0, 0, 0, 0]);                // mov eax, clumpAddr      (patch +7)
                w.putU8(0xE8);                                 // call rel32 (opcode @+11, operand @+12..15)
                const rel = targetAddr.sub(code.add(16)).toInt32();  // rel32 is relative to the NEXT insn (@+16)
                w.putBytes([rel & 0xff, (rel >>> 8) & 0xff, (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
                w.putU8(0x58 + REGNUM[thisReg]);               // pop <thisReg>
                w.putU8(0xC3);                                 // ret
                w.flush();
            });
            code.add(2).writeU32(thisAddr >>> 0);              // this in the seeded reg
            code.add(7).writeU32(clumpAddr >>> 0);             // clump (shared) in EAX
            return code;
        }
        const trampO = buildRegThisTramp(TARGET_ADDR, parseInt(thisO.toString(), 16) >>> 0);
        const trampR = buildRegThisTramp(reimplAddr,  parseInt(thisR.toString(), 16) >>> 0);
        const FnO = new NativeFunction(trampO, 'void', [], 'mscdecl');
        const FnR = new NativeFunction(trampR, 'void', [], 'mscdecl');

        function fp(p) {
            let s = '';
            for (let k = 0; k < structSize; k += 4) {
                s += ('00000000' + (p.add(k).readU32() >>> 0).toString(16)).slice(-8);
            }
            return '0x' + s;
        }

        try {
            for (let i = 0; i < CONFIG.tests.length; i++) {
                const t = CONFIG.tests[i] || {};
                HANDLE_BASE = (t.handle_base != null ? t.handle_base : (0x1000 + i * 0x100)) >>> 0;
                const frame = (t.clump_frame != null ? t.clump_frame : (0x3000 + i)) >>> 0;
                clumpBuf.add(4).writeU32(frame);               // *(clump+4) frame sentinel
                for (let k = 0; k < structSize; k += 4) { thisO.add(k).writeU32(0); thisR.add(k).writeU32(0); }
                let origV = null, reimV = null, errO = null, errR = null;
                try { FnO(); origV = fp(thisO); } catch (e) { errO = e.message; }
                try { FnR(); reimV = fp(thisR); } catch (e) { errR = e.message; }
                results.push({ idx: i, input: ('base=0x' + HANDLE_BASE.toString(16) + ' frame=0x' + frame.toString(16)),
                               original: origV, reimpl: reimV,
                               match: (origV !== null && reimV !== null && origV === reimV),
                               err_original: errO, err_reimpl: errR });
            }
        } finally {
            for (const a of filled) Interceptor.revert(a);
            Interceptor.flush();
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── reg_this_call_observe (orch-iter8, SWEEP-CRITICAL) ────────────────────
    // Sibling of reg_this_callee_stub for VOID functions whose only effect is a
    // single call — e.g. the particle-emitter destructors
    //   void fn(this=EAX) { RpClumpDestroy(*(this + field_off)); }
    // The struct is not written, so there is nothing to fingerprint; instead we
    // Interceptor.replace the observed callee (RpClumpDestroy) with a recorder
    // that captures its first argument, seed *(this+field_off)=sentinel per
    // test, call each side, and compare the recorded arg. Non-degenerate: the
    // sentinel varies per test, so a port reading the wrong field records a
    // different value. Evidence: re/analysis/callers_c2_unblock/portcap_dtor_rpclumpdestroy.md.
    //
    // CONFIG: this_reg ('eax' for these), this_field_off, observe_callee_str,
    //   struct_size (>= field_off+4), signature {ret:'void', args:[]},
    //   tests = [sentinel u32, ...].
    // MECHANISM: Delivers `this` via CONFIG.this_reg (eax/ebx/ecx/edx/esi/edi) or as a __cdecl
    // stack arg ('stack'); seeds *(this + CONFIG.this_field_off) with a per-test u32 sentinel;
    // Interceptor.replaces CONFIG.observe_callee_str with a recorder that captures the callee's
    // first uint32 arg; verifies stub installed before running; observable is the recorded arg -
    // fn's return is NOT compared. CONFIG: `this_reg`, `this_field_off`, `observe_callee_str`,
    // `struct_size`. Broader: any fn that conditionally passes a field-derived or hardcoded value
    // to one interceptable callee.
    if (CONFIG.arg_type === 'reg_this_call_observe') {
        const structSize = CONFIG.struct_size || 0x40;
        const thisReg    = (CONFIG.this_reg || 'eax').toLowerCase();
        const fieldOff   = CONFIG.this_field_off || 0;
        const MOV_OP = { eax: 0xB8, ecx: 0xB9, edx: 0xBA, ebx: 0xBB, esi: 0xBE, edi: 0xBF };
        const REGNUM = { eax: 0, ecx: 1, edx: 2, ebx: 3, esp: 4, ebp: 5, esi: 6, edi: 7 };
        // this_reg:'stack' (2026-07-31) — same observe-a-stubbed-callee idea, but
        // the object goes in as a __cdecl STACK argument instead of a register.
        // Added for the audio pool-free cluster 0x005ae380/0x005a6c90/0x005ad8b0:
        //   mov eax,[esp+4]; test byte[eax+off],1; jnz ret;
        //   push eax; push <anchor>; call AudioPoolFree; add esp,8; ret
        // Stubbing the callee is what makes those testable at all — the pool
        // anchor is a hardcoded immediate inside each function, so letting the
        // clear-bit path run for real would hand a fabricated node to the LIVE
        // allocator. With the callee replaced nothing downstream executes, and
        // the recorded arg0 IS the anchor, so one observation checks both the
        // branch and the per-row constant.
        const useStack = (thisReg === 'stack');
        const thisOp = MOV_OP[thisReg];
        if (!useStack && thisOp === undefined) { send({ type: 'error', msg: 'bad this_reg ' + thisReg }); return; }

        const _keep = [];
        const thisBuf = Memory.alloc(structSize); _keep.push(thisBuf);

        let recorded = null;
        const cbObserve = new NativeCallback(function (arg) { recorded = arg >>> 0; return 0; },
                                             'int', ['uint32']);
        const obsAddr = ptr(CONFIG.observe_callee_str);
        // Verify the stub ACTUALLY installed before calling anything. This is a
        // safety gate, not a diagnostic: for a guarded call the whole reason to
        // stub the callee is that letting it run for real would hand fabricated
        // arguments to live engine state. If the replace silently no-ops, that
        // protection is gone and the real callee executes — while the run merely
        // looks like "the branch was never taken".
        //
        // Observed in iter14: three hooks in one batch all replaced the SAME
        // address (0x005ae920) in sequence, each in its own script load/unload,
        // and the MIDDLE one recorded no calls at all on either side. Standalone
        // it was correct and non-degenerate (4/4), so the function was fine; the
        // stub had not taken effect. [UNCERTAIN] whether the cause is the
        // unload of the previous script tearing down the new replacement or
        // something else — the mechanism is not established, only the symptom.
        // The verdict was safe either way (the batch reported INCONCLUSIVE, not
        // GREEN, because every observation was identical), but the unstubbed
        // call is not something to leave to chance.
        const preBytes = [];
        for (let b = 0; b < 5; b++) preBytes.push(obsAddr.add(b).readU8());
        Interceptor.replace(obsAddr, cbObserve);
        Interceptor.flush();
        let patched = false;
        for (let b = 0; b < 5; b++) if (obsAddr.add(b).readU8() !== preBytes[b]) { patched = true; break; }
        if (!patched) {
            send({ type: 'error', msg: 'callee stub did NOT install at ' +
                   CONFIG.observe_callee_str + ' — refusing to run, the REAL ' +
                   'callee would execute with fabricated arguments' });
            return;
        }

        function buildTramp(targetAddr) {
            const code = Memory.alloc(Process.pageSize);
            if (useStack) {
                // push imm32(thisBuf) / call rel32 / add esp,4 / ret
                // = 5 + 5 + 3 + 1 = 14 bytes. Counting this wrong (13) left the
                // trailing RET outside the patched window on the first attempt.
                Memory.patchCode(code, 14, function (cw) {
                    const w = new X86Writer(cw, { pc: code });
                    w.putBytes([0x68, 0, 0, 0, 0]);             // push imm32 (patch +1)
                    w.putU8(0xE8);                              // call rel32 (operand @+6..9)
                    const rel = targetAddr.sub(code.add(10)).toInt32();  // next insn @+10
                    w.putBytes([rel & 0xff, (rel >>> 8) & 0xff, (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
                    w.putBytes([0x83, 0xC4, 0x04]);             // add esp,4  (__cdecl cleanup)
                    w.putU8(0xC3);                              // ret
                    w.flush();
                });
                code.add(1).writeU32(parseInt(thisBuf.toString(), 16) >>> 0);
                return code;
            }
            Memory.patchCode(code, 13, function (cw) {
                const w = new X86Writer(cw, { pc: code });
                w.putU8(0x50 + REGNUM[thisReg]);               // push <thisReg>
                w.putBytes([thisOp, 0, 0, 0, 0]);              // mov <thisReg>, thisAddr (patch +2)
                w.putU8(0xE8);                                 // call rel32 (opcode @+6, operand @+7..10)
                const rel = targetAddr.sub(code.add(11)).toInt32();  // next insn @+11
                w.putBytes([rel & 0xff, (rel >>> 8) & 0xff, (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
                w.putU8(0x58 + REGNUM[thisReg]);               // pop <thisReg>
                w.putU8(0xC3);                                 // ret
                w.flush();
            });
            code.add(2).writeU32(parseInt(thisBuf.toString(), 16) >>> 0);
            return code;
        }
        const trampO = buildTramp(TARGET_ADDR);
        const trampR = buildTramp(reimplAddr);
        const FnO = new NativeFunction(trampO, 'void', [], 'mscdecl');
        const FnR = new NativeFunction(trampR, 'void', [], 'mscdecl');

        try {
            for (let i = 0; i < CONFIG.tests.length; i++) {
                const sentinel = CONFIG.tests[i] >>> 0;
                for (let k = 0; k < structSize; k += 4) thisBuf.add(k).writeU32(0);
                thisBuf.add(fieldOff).writeU32(sentinel);
                let origV = null, reimV = null, errO = null, errR = null;
                // "the callee was not invoked" is a REAL observation, not an
                // absent one — for a guarded call it is exactly what the taken
                // branch looks like. Recording it as null made null==null read
                // as a mismatch, which RED'd three correct ports in iter14
                // (both sides agreed on all four seeds). Same class as the
                // standing rule that a both-sides-identical crash is not a RED.
                // Encoded as a sentinel STRING so it participates in the match
                // test AND in the distinct-value count, which is what actually
                // guards against the degenerate all-null run. Inert for the
                // iter8 users (their callee fires on every seed).
                const NOT_CALLED = 'not-called';
                recorded = null; try { FnO(); origV = (recorded === null ? NOT_CALLED : '0x' + recorded.toString(16)); } catch (e) { errO = e.message; }
                recorded = null; try { FnR(); reimV = (recorded === null ? NOT_CALLED : '0x' + recorded.toString(16)); } catch (e) { errR = e.message; }
                results.push({ idx: i, input: '0x' + sentinel.toString(16),
                               original: origV, reimpl: reimV,
                               match: (!errO && !errR && origV === reimV),
                               err_original: errO, err_reimpl: errR });
            }
        } finally {
            Interceptor.revert(obsAddr);
            Interceptor.flush();
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── vtable_table_dispatch ───────────────────────────────────────────────
    // void fn(a1, holder, idx, a4) that dispatches through a TABLE OF 8-BYTE
    // ENTRIES reached by one indirection off `holder`:
    //     entry = *(holder + vtbl_ptr_offset) + idx*8
    //     entry[0](a1, *(u16*)(entry + 4), a4)
    // i.e. the entry carries both the function pointer and a 16-bit datum that
    // is passed as the middle argument.
    //
    // Nothing real is called: the handler writes its OWN NativeCallback into
    // entry[0], so the dispatch lands on a recorder. That makes the whole
    // structure observable — the recorded triple (a1, aux16, a4) proves the
    // *8 stride, the +4 aux read, the vtbl_ptr_offset indirection, and the
    // argument order all at once. A port with the wrong stride or offset reads
    // a different entry and records a different aux16.
    //
    // Both sides get their own holder/table buffers but SHARE the stub address,
    // so the recorded values are directly comparable without normalisation.
    //
    // CONFIG: vtbl_ptr_offset (4 or 8 for the pair it was written for),
    //         table_entries (default 8).
    // tests[i] = { idx, aux16, a1, a4 }
    // Authored 2026-07-31 for 0x005b10a0 (offset 4) and 0x005b10e0 (offset 8),
    // which are byte-identical apart from that one displacement.
    // MECHANISM: Per-side holder+table buffers; holder[CONFIG.vtbl_ptr_offset]->table;
    // entry[idx*8]=shared NativeCallback stub ptr, entry[idx*8+4]=aux16; calls fn(a1, holder, idx,
    // a4); observes 3 uint32 args captured by the stub - direct comparison since both sides call
    // the same stub address. CONFIG: vtbl_ptr_offset (4 or 8), table_entries; tests: {idx, aux16,
    // a1, a4}. Applies to any fn dispatching through a pointer-at-offset table of 8-byte {fnptr,
    // aux16} entries.
    if (CONFIG.arg_type === 'vtable_table_dispatch') {
        const OFF  = CONFIG.vtbl_ptr_offset | 0;
        const NENT = (CONFIG.table_entries | 0) || 8;
        const holderO = Memory.alloc(0x40), holderR = Memory.alloc(0x40);
        const tableO  = Memory.alloc(NENT * 8), tableR = Memory.alloc(NENT * 8);
        const _keepVT = [holderO, holderR, tableO, tableR];
        let rec = null;
        const stub = new NativeCallback(function (p1, p2, p3) {
            rec = [p1 >>> 0, p2 >>> 0, p3 >>> 0];
            return 0;
        }, 'int', ['uint32', 'uint32', 'uint32']);
        _keepVT.push(stub);
        const fp = function () {
            return rec === null ? 'not-called'
                 : rec.map(function (v) { return ('00000000' + v.toString(16)).slice(-8); }).join(',');
        };
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const idx = t.idx >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            [[holderO, tableO], [holderR, tableR]].forEach(function (pair) {
                const h = pair[0], tb = pair[1];
                for (let b = 0; b < 0x40; b += 4) h.add(b).writeU32(0);
                for (let b = 0; b < NENT * 8; b += 4) tb.add(b).writeU32(0);
                h.add(OFF).writePointer(tb);              // holder -> table
                tb.add(idx * 8).writePointer(stub);       // entry[0] = recorder
                tb.add(idx * 8 + 4).writeU16(t.aux16 & 0xffff);
            });
            rec = null;
            try { Orig(t.a1 >>> 0, holderO, idx, t.a4 >>> 0);   origV = fp(); } catch (e) { errO = e.message; }
            rec = null;
            try { Reimpl(t.a1 >>> 0, holderR, idx, t.a4 >>> 0); reimV = fp(); } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: (!errO && !errR && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── stub_dispatch_observe ───────────────────────────────────────────────
    // Generalises "plant a recorder where the function expects a callee, then
    // compare what it received". vtable_table_dispatch did this for one fixed
    // layout; three more rows needed the same primitive with different plumbing,
    // so the plumbing is now CONFIG:
    //
    //   0x004cbb50  obj -> *obj -> [+8]        stub planted in a fake vtable
    //   0x00550950  stream -> *(s+0x38) -> [+0x30]   ditto, deeper
    //   0x005af200  callback passed DIRECTLY as an argument
    //   0x00482900  callee is a HARDCODED DIRECT CALL — patched via stub_at
    //
    // Nothing real is ever dispatched, which is the point: for a Release thunk
    // or a stream-read dispatch, letting the true callee run would touch live
    // engine objects.
    //
    // CONFIG:
    //   num_bufs, buf_size    scratch buffers (paired per side)
    //   seed[]                {buf,off,type,value} literal
    //                       | {buf,off,ptr_to:j}   address of buffer j
    //                       | {buf,off,stub:true}  address of the recorder
    //   arg_layout[]          {buf:i} | {i32:true} | {f32:true} | {stub:true}
    //   stub_at[]             hex addrs to Interceptor.replace with the recorder
    //                         (for callees reached by a hardcoded direct CALL,
    //                          where there is no pointer to seed)
    //   stub_nargs            how many args the recorder declares (default 3)
    //                         — MUST match what the call site actually pushes.
    //                         Declaring more reads stack garbage past the last
    //                         real argument, which can differ between sides and
    //                         produce a FALSE RED.
    //   stub_abi              'mscdecl' (default) or 'stdcall' (callee-cleans)
    //   stub_ret              int the recorder returns (default 0)
    //   observe_ret           fold the TARGET's return into the fingerprint
    //   observe_calls         fold the sequence of recorder invocations in
    //
    // Pointer arguments the recorder receives are NORMALISED to "b<i>+<off>"
    // against the scratch buffers, because the two sides get different
    // addresses; without that every comparison would trivially differ.
    // MECHANISM: Arbitrary-arity fn driven by num_bufs scratch buffers (each buf_size bytes)
    // seeded per-test via test.seed=[{buf,off,type,value,stub,ptr_to}]; arg_layout[] maps each
    // call position to a buf-ptr, a scalar from test.scalars[], or the recorder stub; stub_at[]
    // Interceptor-replaces hardcoded direct-CALL targets with the recorder; recorder normalises
    // pointer args to "b<i>+<off>" against scratch-buffer bases so per-side addresses compare
    // equal; fingerprint observes return (observe_ret) and/or call sequence with args
    // (observe_calls); stub_ret is per-test so a pass-through return is also tested; CONFIG:
    // num_bufs, buf_size, arg_layout, stub_at, stub_nargs, stub_abi, stub_ret, observe_ret,
    // observe_calls.
    if (CONFIG.arg_type === 'stub_dispatch_observe') {
        const NB = (CONFIG.num_bufs | 0) || 1;
        const BS = (CONFIG.buf_size | 0) || 0x80;
        const NA = (CONFIG.stub_nargs === undefined) ? 3 : (CONFIG.stub_nargs | 0);
        // Per-test return, not a fixed constant. Without this a pure dispatch
        // thunk like 0x004cbb50 — whose entire behaviour is (*obj)[2](obj) — has
        // NOTHING that can vary across seeds, so every fingerprint would be
        // identical and the run degenerate by construction. Varying what the
        // recorder returns also tests the thunk's return PASSTHROUGH, which for
        // 0x004cbb50 is the whole second half of the function (it has no `mov`
        // after the call; EAX simply flows out).
        let curRet = (CONFIG.stub_ret | 0) || 0;
        const layout = CONFIG.arg_layout || [];
        const keep = [];
        const bufsO = [], bufsR = [];
        for (let k = 0; k < NB; k++) {
            bufsO.push(Memory.alloc(BS)); bufsR.push(Memory.alloc(BS));
        }
        keep.push(bufsO, bufsR);

        let calls = [], sideBufs = null;
        const normPtr = function (v) {
            const a = ptr(v >>> 0);
            for (let k = 0; k < NB; k++) {
                const base = sideBufs[k];
                const d = a.sub(base).toInt32();
                if (d >= 0 && d < BS) return 'b' + k + '+' + d;
            }
            return '0x' + (v >>> 0).toString(16);
        };
        const mkStub = function () {
            const types = [];
            for (let k = 0; k < NA; k++) types.push('uint32');
            return new NativeCallback(function () {
                const got = [];
                for (let k = 0; k < arguments.length; k++)
                    got.push(normPtr(arguments[k]));
                calls.push(got.join(','));
                return curRet;
            }, 'int', types, CONFIG.stub_abi || undefined);
        };
        const stub = mkStub();
        keep.push(stub);

        // ── stub_at: plant the recorder at a FIXED DIRECT-CALL address ───────
        // The three iter19 rows all dispatched through data the caller supplies
        // — a fake vtable slot, or the function-pointer argument itself — so
        // seeding a pointer was enough. 0x00482900 is different in kind: it
        // `CALL rel32`s a hardcoded logger at 0x004987b0 and there is NO pointer
        // to seed. Patching the callee's entry is the only way to stop it.
        //
        // This is not a convenience, it is the whole reason the row was
        // unverifiable. 0x004987b0 ends in OutputDebugStringA, which raises a
        // debug-print SEH exception (0x40010006). Windows swallows it when no
        // debugger is attached, but Frida's exception handler surfaces it — so
        // in iter18 all six seeds returned 'system error' on BOTH sides, in the
        // batch AND standalone. Both-sides-identical failure is not evidence of
        // correctness; it is no evidence at all.
        //
        // Install guard is the same one reg_this_call_observe got in iter15:
        // verify the entry bytes actually changed before running anything. A
        // silently uninstalled stub means the REAL callee executes while the run
        // merely looks quiet.
        const stubAt = CONFIG.stub_at || [];
        for (let k = 0; k < stubAt.length; k++) {
            const sa = ptr(stubAt[k]);
            const pre = [];
            for (let b = 0; b < 5; b++) pre.push(sa.add(b).readU8());
            const cb = mkStub();
            keep.push(cb);
            Interceptor.replace(sa, cb);
            Interceptor.flush();
            let patched = false;
            for (let b = 0; b < 5; b++)
                if (sa.add(b).readU8() !== pre[b]) { patched = true; break; }
            if (!patched) {
                send({ type: 'error', msg: 'stub_at did NOT install at ' +
                       stubAt[k] + ' — refusing to run, the REAL callee would ' +
                       'execute' });
                return;
            }
        }

        const wr = function (p, off, type, value) {
            const a = p.add(off);
            switch (type) {
                case 'u8':  a.writeU8(value & 0xff); break;
                case 'u16': a.writeU16(value & 0xffff); break;
                case 's32': a.writeS32(value | 0); break;
                case 'f32': a.writeFloat(value); break;
                default:    a.writeU32(value >>> 0); break;
            }
        };
        const applySeed = function (bufs, seed) {
            for (let k = 0; k < NB; k++)
                for (let b = 0; b < BS; b++) bufs[k].add(b).writeU8(0);
            (seed || []).forEach(function (s) {
                if (s.stub) bufs[s.buf].add(s.off).writePointer(stub);
                else if (s.ptr_to !== undefined)
                    bufs[s.buf].add(s.off).writePointer(bufs[s.ptr_to]);
                else wr(bufs[s.buf], s.off, s.type, s.value);
            });
        };
        const buildArgs = function (bufs, scalars) {
            const args = []; let si = 0;
            for (let k = 0; k < layout.length; k++) {
                const a = layout[k];
                if (a && a.stub) args.push(stub);
                else if (a && a.buf !== undefined) args.push(bufs[a.buf]);
                else args.push(scalars[si++]);
            }
            return args;
        };
        const fpOf = function (ret, errored) {
            const parts = [];
            if (CONFIG.observe_ret)
                parts.push('r=' + (errored ? 'ERR'
                    : (ret === null || ret === undefined) ? 'null'
                    : (typeof ret === 'object' ? ret.toInt32() : (ret | 0)) >>> 0));
            if (CONFIG.observe_calls)
                parts.push('calls[' + calls.length + ']=' + calls.join(';'));
            return parts.join('|');
        };
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const scalars = t.scalars || [];
            curRet = (t.stub_ret !== undefined) ? (t.stub_ret | 0)
                                                : ((CONFIG.stub_ret | 0) || 0);
            let fO = null, fR = null, errO = null, errR = null;
            sideBufs = bufsO; applySeed(bufsO, t.seed); calls = [];
            try { fO = fpOf(Orig.apply(null, buildArgs(bufsO, scalars)), false); }
            catch (e) { errO = e.message; fO = fpOf(null, true); }
            sideBufs = bufsR; applySeed(bufsR, t.seed); calls = [];
            try { fR = fpOf(Reimpl.apply(null, buildArgs(bufsR, scalars)), false); }
            catch (e) { errR = e.message; fR = fpOf(null, true); }
            results.push({ idx: i, input: JSON.stringify(t),
                           original: fO, reimpl: fR,
                           match: (!errO && !errR && fO === fR),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── vec3_lerp (promote-round-22 harness-ext, SWEEP-CRITICAL) ─────────────
    // For pure vec3 math leaves: void fn(float* out3, float* a3, float* b3,
    // float t) — writes a 3-float result computed from two input vec3s and a
    // scalar. No globals -> deterministic, menu-attach-safe. Each test is
    // { a:[x,y,z], b:[x,y,z], t:float }. Fills a/b/t, calls fn(out,a,b,t),
    // reads the 3 out floats as a packed-bits fingerprint, compares.
    // Validated on Vec3Lerp 0x004b4650.
    // MECHANISM: Allocates separate outA and outB (12B each) for Orig and Reimpl respectively;
    // shares aBuf and bBuf (12B each, written once per test, read-only to callee) between both
    // sides; seeds aBuf/bBuf from input.a[0..2]/input.b[0..2], passes input.t as a float scalar
    // 4th arg - fn(out, a, b, t); observes 3 out-float bits as packed hex; no CONFIG beyond
    // `tests`; no globals; deterministic/menu-attach-safe. Broadly fits any pure fn(float*out3,
    // float*a3, float*b3, float) vec3 math leaf.
    if (CONFIG.arg_type === 'vec3_lerp') {
        const outA = Memory.alloc(12), outB = Memory.alloc(12);
        const aBuf = Memory.alloc(12), bBuf = Memory.alloc(12);
        function fpVec(buf) {
            return [0, 4, 8].map(function(o) {
                return ('00000000' + (buf.add(o).readU32() >>> 0).toString(16)).slice(-8);
            }).join(',');
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            let origV = null, reimV = null, errO = null, errR = null;
            for (let k = 0; k < 3; k++) { aBuf.add(k*4).writeFloat(t.a[k]); bBuf.add(k*4).writeFloat(t.b[k]); }
            outA.writeU32(0); outA.add(4).writeU32(0); outA.add(8).writeU32(0);
            outB.writeU32(0); outB.add(4).writeU32(0); outB.add(8).writeU32(0);
            try { Orig(outA, aBuf, bBuf, t.t);   origV = fpVec(outA); } catch(e) { errO = e.message; }
            try { Reimpl(outB, aBuf, bBuf, t.t); reimV = fpVec(outB); } catch(e) { errR = e.message; }
            results.push({ idx: i, input: ('t=' + t.t),
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── int_copy_outbuf ──────────────────────────────────────────────────────
    // For TimerSlotDataCopy-style: fn(int slot, T* dst) — void return, copies
    // N bytes from a per-slot source global into dst. Each test is a single
    // integer index. Allocates a sentinel-filled 4 KB scratch buffer per side
    // (oversized to back any TimerSlot-like copy); reads back the first
    // CONFIG.out_buf_size (default 24) bytes as a position-sensitive XOR
    // fingerprint. Real GREEN requires both fingerprints to be non-zero AND
    // equal — proof the function wrote AND wrote the same bytes.
    // MECHANISM: Passes (int slot, T* dst) to fn; each side gets its own independent 4 KB buffer
    // pre-filled with sentinel byte 0xCD; observable is a position-sensitive XOR fingerprint of
    // the first CONFIG.out_buf_size bytes (default 24); GREEN requires fingerprint non-zero AND
    // equal - non-zero proves the copy executed; fn return value is NOT compared. CONFIG:
    // `out_buf_size`. Broader: any fn(int, ptr) that copies data from a per-slot source global
    // into a caller-allocated output buffer.
    if (CONFIG.arg_type === 'int_copy_outbuf') {
        const BUF_BYTES = 4096;
        const READ_LEN  = (CONFIG.out_buf_size | 0) || 24;
        const SENTINEL  = 0xCD;  // VC malloc-style uninit byte
        const cBufA = Memory.alloc(BUF_BYTES);
        const cBufB = Memory.alloc(BUF_BYTES);
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const slot = CONFIG.tests[i] | 0;
            let origV = null, reimV = null, errO = null, errR = null;
            // Fill both buffers with sentinel before each call.
            for (let k = 0; k < BUF_BYTES; k++) {
                cBufA.add(k).writeU8(SENTINEL);
                cBufB.add(k).writeU8(SENTINEL);
            }
            try {
                Orig(slot, cBufA);
                origV = bufFingerprint(cBufA, READ_LEN);
            } catch(e) { errO = e.message; }
            try {
                Reimpl(slot, cBufB);
                reimV = bufFingerprint(cBufB, READ_LEN);
            } catch(e) { errR = e.message; }
            results.push({ idx: i, input: slot,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── thread_desc_init ────────────────────────────────────────────────────
    // For AudioThreadDescInit (0x005aef00): fn(uint32_t* buf, p2, p3, p4).
    // Writes 5 uint32 fields: buf[0]=0, buf[1]=p2, buf[2]=0, buf[3]=p3, buf[4]=p4.
    // input: [p2, p3, p4] — the three scalar arguments after the pointer.
    // Strategy: allocate 5x4=20 byte scratch buf; fill with sentinel 0xDEAD????;
    // call fn(buf, p2, p3, p4); read back 5 fields; return packed fingerprint.
    // Both orig and reimpl must produce identical field values.
    // MECHANISM: Allocates a 20-byte (5-field) scratch buffer, sentinel-fills with 0xDEADBEEF
    // before each call, passes it as first arg with three uint32 scalars (p2, p3, p4) on the stack
    // - fn(buf, p2, p3, p4); observes all 5 output uint32 fields as a comma-joined fingerprint;
    // ignores fn return value; no CONFIG beyond `tests`; no globals. NARROW: hardcoded
    // 5-field/20-byte struct with no size CONFIG - only fits this exact 4-arg/5-field init layout.
    if (CONFIG.arg_type === 'thread_desc_init') {
        const STRUCT_BUF = Memory.alloc(20);
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i]; // [p2, p3, p4]
            const p2 = t[0] >>> 0, p3 = t[1] >>> 0, p4 = t[2] >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            function readFields(b) {
                const f0 = b.readU32();
                const f1 = b.add(4).readU32();
                const f2 = b.add(8).readU32();
                const f3 = b.add(12).readU32();
                const f4 = b.add(16).readU32();
                return [f0, f1, f2, f3, f4].join(',');
            }
            try {
                // fill sentinel
                for (let k = 0; k < 5; k++) STRUCT_BUF.add(k*4).writeU32(0xDEADBEEF);
                Orig(STRUCT_BUF, p2, p3, p4);
                origV = readFields(STRUCT_BUF);
            } catch(e) { errO = e.message; }
            try {
                for (let k = 0; k < 5; k++) STRUCT_BUF.add(k*4).writeU32(0xDEADBEEF);
                Reimpl(STRUCT_BUF, p2, p3, p4);
                reimV = readFields(STRUCT_BUF);
            } catch(e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── sub_struct_dispatcher ────────────────────────────────────────────────
    // For AudioSubStructTwoCallInit (0x005a9e10): fn(p1, p2, p3) -> p1.
    // Calls FUN_005adfe0(p1,p3) and FUN_005ae010(p1,p2); returns p1 unchanged.
    // input: ignored (we use 3 separate scratch buffers each call).
    // Strategy: allocate 3 scratch buffers, call fn(b0, b1, b2); compare return
    // address == b0 address (return value must equal first arg).
    // Both paths route through same original callees; assertion is return==param_1.
    // MECHANISM: fn(b0, b1, b2): allocates three 64-byte scratch buffers, zeroes all three before
    // each call, and compares the RETURN ADDRESS against b0's address rather than a raw pointer -
    // the address-normalisation that lets per-side allocations compare equal. Fits any 3-pointer
    // dispatcher expected to return its first argument.
    if (CONFIG.arg_type === 'sub_struct_dispatcher') {
        const BUF0 = Memory.alloc(64);
        const BUF1 = Memory.alloc(64);
        const BUF2 = Memory.alloc(64);
        for (let i = 0; i < CONFIG.tests.length; i++) {
            let origV = null, reimV = null, errO = null, errR = null;
            const b0i = BUF0.toInt32() >>> 0;
            try {
                // zero bufs before each call
                for (let k = 0; k < 64; k += 4) { BUF0.add(k).writeU32(0); BUF1.add(k).writeU32(0); BUF2.add(k).writeU32(0); }
                const retO = Orig(BUF0, BUF1, BUF2);
                // encode: 1 if return addr == BUF0 addr, else 0; always 1 for correct impl
                const retOInt = retO ? (parseInt(retO.toString(), 16) >>> 0) : 0;
                origV = (retOInt === b0i) ? 1 : 0;
            } catch(e) { errO = e.message; }
            try {
                for (let k = 0; k < 64; k += 4) { BUF0.add(k).writeU32(0); BUF1.add(k).writeU32(0); BUF2.add(k).writeU32(0); }
                const retR = Reimpl(BUF0, BUF1, BUF2);
                const retRInt = retR ? (parseInt(retR.toString(), 16) >>> 0) : 0;
                reimV = (retRInt === b0i) ? 1 : 0;
            } catch(e) { errR = e.message; }
            results.push({ idx: i, input: i,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── dsound_secondary_init ────────────────────────────────────────────────
    // For AudioDSoundSecondaryInit (0x005bbfc0): fn(void** ppUnk) -> int.
    // Calls vtable[0] (QI), vtable[5] (secondary init), vtable[2] (Release).
    // Strategy: build fake IUnknown with 6-slot vtable; stubs anchored in array
    // to prevent GC. Observable: return value (0) packed with stub call count.
    // MECHANISM: NARROW: builds fixed 2-object stdcall IUnknown fake (outer vtable[0]=QI writes
    // inner obj addr, vtable[2]=Release; inner vtable[5]=secondary-init writes sentinel 3 into
    // 4th-arg slot); call shape fn(void** ppUnk) -> int; resets outer ptr before each side;
    // observes ((ret & 0xffff) | (stub_call_count << 16)). CONFIG.tests[] is a repeat counter only
    // - no per-test input variation; layout hardcoded to this one vtable shape.
    if (CONFIG.arg_type === 'dsound_secondary_init') {
        let dsDsCallCount = 0;
        const _dsDsStubs = [];  // GC anchors for NativeCallback objects

        // Two separate IUnknown-shaped fake objects:
        //   OBJ_DS / VTBL_DS   — the "outer" object the caller passes via *param_1.
        //                        Its vtable[0] (QI) writes OBJ_DS_QI's address into
        //                        *ppOut so the reimpl can deref a valid QI output.
        //                        Its vtable[2] (Release) increments the call counter.
        //   OBJ_DS_QI / VTBL_DS_QI — the "inner" QI-output object. Its vtable[5]
        //                        (secondary init) writes 3 (the expected success
        //                        sentinel — see U-0362 in AudioDSound.cpp) into the
        //                        4th-arg output slot, then returns 0 (S_OK).
        // 2026-05-24 phase-a1: stubs are stdcall to match MSVC virtual-method ABI
        // (orig uses stdcall vtable calls; previously mscdecl stubs caused ESP
        // imbalance only on the orig side, producing a false RED).
        const VTBL_DS    = Memory.alloc(24);
        const OBJ_DS     = Memory.alloc(4);
        const VTBL_DS_QI = Memory.alloc(24);
        const OBJ_DS_QI  = Memory.alloc(4);
        const PPUNK_DS   = Memory.alloc(4);

        // Outer object's Release (slot 2).
        const dsStubRel = new NativeCallback(function() { dsDsCallCount++; return 0; }, 'int32', ['pointer'], 'stdcall');
        // Outer object's QI (slot 0, 3 args): writes the inner OBJ_DS_QI address
        // into the output slot so the reimpl gets a valid interface ptr to
        // route slot 5 through. Returns 0 (S_OK).
        const dsDsQiStub = new NativeCallback(function(self, iid, ppOut) {
            dsDsCallCount++;
            if (ppOut && !ppOut.isNull()) ppOut.writeU32(OBJ_DS_QI.toInt32());
            return 0;
        }, 'int32', ['pointer', 'pointer', 'pointer'], 'stdcall');
        // Inner object's slot 5 (4 args: this, fmtPtr, mode, &outStatus). Writes
        // the success sentinel (3) into the 4th-arg output slot so the orig's
        // CMP [ESP+0x8], 3 evaluates true → return 1. Returns 0 (S_OK).
        const dsSlot5Stub = new NativeCallback(function(self, fmt, mode, pOut) {
            dsDsCallCount++;
            if (pOut && !pOut.isNull()) pOut.writeU32(3);
            return 0;
        }, 'int32', ['pointer', 'pointer', 'int32', 'pointer'], 'stdcall');
        // Generic filler stubs (slots not exercised in the success path).
        const dsStub1 = new NativeCallback(function() { dsDsCallCount++; return 0; }, 'int32', ['pointer'], 'stdcall');
        const dsStub2 = new NativeCallback(function() { dsDsCallCount++; return 0; }, 'int32', ['pointer', 'pointer'], 'stdcall');
        _dsDsStubs.push(dsStubRel, dsDsQiStub, dsSlot5Stub, dsStub1, dsStub2);

        // Outer vtable.
        VTBL_DS.add(0 * 4).writeU32(dsDsQiStub.toInt32());  // slot 0: QI
        VTBL_DS.add(1 * 4).writeU32(dsStub1.toInt32());     // slot 1: AddRef
        VTBL_DS.add(2 * 4).writeU32(dsStubRel.toInt32());   // slot 2: Release
        VTBL_DS.add(3 * 4).writeU32(dsStub2.toInt32());     // slot 3: unused
        VTBL_DS.add(4 * 4).writeU32(dsStub2.toInt32());     // slot 4: unused
        VTBL_DS.add(5 * 4).writeU32(dsStub2.toInt32());     // slot 5: unused on outer

        // Inner vtable (the QI output). Only slot 5 matters; the rest no-op.
        VTBL_DS_QI.add(0 * 4).writeU32(dsStub1.toInt32());  // slot 0 unused
        VTBL_DS_QI.add(1 * 4).writeU32(dsStub1.toInt32());
        VTBL_DS_QI.add(2 * 4).writeU32(dsStub1.toInt32());
        VTBL_DS_QI.add(3 * 4).writeU32(dsStub2.toInt32());
        VTBL_DS_QI.add(4 * 4).writeU32(dsStub2.toInt32());
        VTBL_DS_QI.add(5 * 4).writeU32(dsSlot5Stub.toInt32());  // slot 5: secondary init

        OBJ_DS.writeU32(VTBL_DS.toInt32());
        OBJ_DS_QI.writeU32(VTBL_DS_QI.toInt32());
        PPUNK_DS.writeU32(OBJ_DS.toInt32());

        for (let i = 0; i < CONFIG.tests.length; i++) {
            let origV = null, reimV = null, errO = null, errR = null;
            OBJ_DS.writeU32(VTBL_DS.toInt32());
            PPUNK_DS.writeU32(OBJ_DS.toInt32());
            dsDsCallCount = 0;
            try {
                const ret = Orig(PPUNK_DS);
                const cnt = dsDsCallCount;
                origV = ((ret >>> 0) & 0xffff) | ((cnt & 0xff) << 16);
            } catch(e) { errO = e.message; }
            OBJ_DS.writeU32(VTBL_DS.toInt32());
            PPUNK_DS.writeU32(OBJ_DS.toInt32());
            dsDsCallCount = 0;
            try {
                const ret = Reimpl(PPUNK_DS);
                const cnt = dsDsCallCount;
                reimV = ((ret >>> 0) & 0xffff) | ((cnt & 0xff) << 16);
            } catch(e) { errR = e.message; }
            OBJ_DS.writeU32(VTBL_DS.toInt32());
            PPUNK_DS.writeU32(OBJ_DS.toInt32());
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: i,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_pool_free ──────────────────────────────────────────────────────
    // AudioPoolFree(0x005ae920): fn(pool_ptr, item_addr) → void.
    // Allocate a node via FUN_005ae800(&DAT_009146c0, tag), then free it.
    // Success = 1 (no crash), 0 on crash. Both orig and reimpl must return 1.
    // CONFIG: alloc_rva_str (005ae800), alloc_tag (0x30804), pool_addr_str (009146c0).
    // MECHANISM: Allocates two live pool nodes via `CONFIG.alloc_rva_str`(poolPtr,
    // `CONFIG.alloc_tag`), one per side; calls fn(poolPtr, node) void(ptr,ptr) on live game
    // memory; observable is crash-or-no-crash only (1/0) - a no-op reimpl that doesn't crash
    // passes. CONFIG: `alloc_rva_str`, `alloc_tag`, `pool_addr_str`; tests[] length drives
    // iteration count, test values unused.
    if (CONFIG.arg_type === 'audio_pool_free') {
        var poolPtr    = ptr(CONFIG.pool_addr_str);
        var allocRva   = ptr(CONFIG.alloc_rva_str);
        var allocTag   = (CONFIG.alloc_tag | 0);
        var AllocFn    = new NativeFunction(allocRva, 'pointer', ['pointer', 'int32'], 'mscdecl');
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var orig = 0, reim = 0, errO = null, errR = null;
            var pO = null, pR = null;
            try { pO = AllocFn(poolPtr, allocTag); } catch(e) { errO = 'alloc_O:' + e.message; }
            try { pR = AllocFn(poolPtr, allocTag); } catch(e) { errR = 'alloc_R:' + e.message; }
            try {
                if (pO && !pO.isNull()) { Orig(poolPtr, pO); orig = 1; }
            } catch(e) { errO = e.message; }
            try {
                if (pR && !pR.isNull()) { Reimpl(poolPtr, pR); reim = 1; }
            } catch(e) { errR = e.message; }
            var match = (errO === null && errR === null && orig === reim);
            results.push({ idx: i, input: i,
                           original: orig, reimpl: reim, match: match,
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_list_insert ────────────────────────────────────────────────────
    // AudioListInsertHead(0x005addd0): fn(head_ptr, payload) → void.
    // Build a self-referential 12-byte sentinel in fresh memory (isolated from
    // live game state), call fn(sentinel, payload), read back new head node[2].
    // Observable = payload at node[+8]; -1 if alloc failed (pool not ready).
    // MECHANISM: Allocates two fresh 12-byte isolated sentinel nodes (next=self, prev=self,
    // val=0), one per side; calls fn(sentinel_ptr, int32_payload) void(ptr,int32); fingerprint =
    // int32 at newHead+8 (-1 if no new head). CONFIG: `tests[]` (int32 payloads) only. Broader
    // than name: any insert-head that allocates a node, stores int32 payload at node+8, and writes
    // the new head to sentinel[0].
    if (CONFIG.arg_type === 'audio_list_insert') {
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var payload = CONFIG.tests[i] | 0;
            var origV = null, reimV = null, errO = null, errR = null;
            var sentO = Memory.alloc(12);
            sentO.writePointer(sentO); sentO.add(4).writePointer(sentO); sentO.add(8).writeS32(0);
            var sentR = Memory.alloc(12);
            sentR.writePointer(sentR); sentR.add(4).writePointer(sentR); sentR.add(8).writeS32(0);
            try {
                Orig(sentO, payload);
                var newHead = sentO.readPointer();
                origV = (newHead && !newHead.isNull() && !newHead.equals(sentO))
                      ? newHead.add(8).readS32() : -1;
            } catch(e) { errO = e.message; }
            try {
                Reimpl(sentR, payload);
                var newHead2 = sentR.readPointer();
                reimV = (newHead2 && !newHead2.isNull() && !newHead2.equals(sentR))
                       ? newHead2.add(8).readS32() : -1;
            } catch(e) { errR = e.message; }
            results.push({ idx: i, input: payload,
                           original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_list_remove ────────────────────────────────────────────────────
    // AudioListRemoveByValue(0x005ade10): fn(sentinel_ptr, payload) → int*.
    // Build a fresh sentinel, optionally insert a node via the ORIGINAL
    // FUN_005addd0 (insert_rva_str), then call orig/reimpl to remove.
    // Observable: 1 if found (non-NULL return), 0 if not found (NULL).
    // MECHANISM: Per-side: builds a fresh sentinel-based circular list and optionally inserts one
    // node via the original fn at CONFIG.insert_rva_str; calls fn(sentinel, payload) and observes
    // null vs non-null return (1=found, 0=not-found) only; NARROW: insert always uses the original
    // fn; post-removal list state is never fingerprinted; CONFIG: insert_rva_str; tests[i] = {
    // payload:int, present:bool }.
    if (CONFIG.arg_type === 'audio_list_remove') {
        var insertRva = ptr(CONFIG.insert_rva_str);
        var InsertFn = new NativeFunction(insertRva, 'void', ['pointer', 'int32'], 'mscdecl');
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var payload = t.payload | 0;
            var present = t.present ? true : false;
            var origV = null, reimV = null, errO = null, errR = null;
            var sentO = Memory.alloc(12);
            sentO.writePointer(sentO); sentO.add(4).writePointer(sentO); sentO.add(8).writeS32(0);
            var sentR = Memory.alloc(12);
            sentR.writePointer(sentR); sentR.add(4).writePointer(sentR); sentR.add(8).writeS32(0);
            if (present) {
                try { InsertFn(sentO, payload); } catch(e) { errO = 'ins_O:' + e.message; }
                try { InsertFn(sentR, payload); } catch(e) { errR = 'ins_R:' + e.message; }
            }
            try {
                var retO = Orig(sentO, payload);
                origV = (retO && !retO.isNull()) ? 1 : 0;
            } catch(e) { errO = e.message; }
            try {
                var retR = Reimpl(sentR, payload);
                reimV = (retR && !retR.isNull()) ? 1 : 0;
            } catch(e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_list_drain ──────────────────────────────────────────────────────
    // AudioListDrain(0x005ade90): fn(sentinel_ptr) → void.
    // Build a fresh sentinel, insert N nodes via FUN_005addd0 (original),
    // call orig/reimpl to drain.
    // Observable: 1 = sentinel self-loops (empty) after drain, 0 = not empty.
    // MECHANISM: fn(sentinel_ptr); harness allocates a fresh per-side 12-byte self-looping
    // sentinel (next@+0, prev@+4, count@+8), inserts N nodes by calling the real game function at
    // CONFIG.insert_rva_str (loaded as NativeFunction with 'mscdecl' void(pointer,int32)), then
    // calls orig/reimpl to drain; observes whether the sentinel self-loops post-drain (1=empty,
    // 0=not); return value not observed; NARROW: 12-byte circular-list layout and insert-function
    // calling convention are fixed - only fits list-drain functions using that exact node shape
    // and the game's own insert function.
    if (CONFIG.arg_type === 'audio_list_drain') {
        var insertRva2 = ptr(CONFIG.insert_rva_str);
        var InsertFn2 = new NativeFunction(insertRva2, 'void', ['pointer', 'int32'], 'mscdecl');
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var nodeCount = CONFIG.tests[i] | 0;
            var origV = null, reimV = null, errO = null, errR = null;
            var sentO2 = Memory.alloc(12);
            sentO2.writePointer(sentO2); sentO2.add(4).writePointer(sentO2); sentO2.add(8).writeS32(0);
            var sentR2 = Memory.alloc(12);
            sentR2.writePointer(sentR2); sentR2.add(4).writePointer(sentR2); sentR2.add(8).writeS32(0);
            for (var k = 0; k < nodeCount; k++) {
                if (errO === null) try { InsertFn2(sentO2, k + 1); } catch(e) { errO = 'ins_O:' + e.message; }
                if (errR === null) try { InsertFn2(sentR2, k + 1); } catch(e) { errR = 'ins_R:' + e.message; }
            }
            try {
                Orig(sentO2);
                origV = sentO2.readPointer().equals(sentO2) ? 1 : 0;
            } catch(e) { errO = e.message; }
            try {
                Reimpl(sentR2);
                reimV = sentR2.readPointer().equals(sentR2) ? 1 : 0;
            } catch(e) { errR = e.message; }
            results.push({ idx: i, input: nodeCount,
                           original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── ptr_scratch_field (added 2026-06-04, c3_batch_ab s1) ─────────────────
    // fn(ptr) -> int.  The function reads a field at scratch+field_offset.
    // Allocate one zeroed scratch buffer (read-only target), seed the test byte
    // at CONFIG.field_offset (default 0x54), call Orig/Reimpl, compare returns.
    // Used for AudioByte54Bit3Get (0x005ac540): (*(byte*)(p+0x54) & 8) >> 3.
    // MECHANISM: fn(ptr)->int; harness allocates a 0x80-byte zeroed buffer, writes test byte value
    // at CONFIG.field_offset (default 0x54), calls Orig/Reimpl with that pointer, compares return
    // values as u32; buffer contents after call are not observed; CONFIG: field_offset (byte
    // offset, default 0x54), tests[] (byte values); broader: any fn(ptr)->int that extracts a
    // scalar from a configurable byte offset in a zeroed struct.
    if (CONFIG.arg_type === 'ptr_scratch_field') {
        var PSF_OFF = (CONFIG.field_offset !== undefined) ? (CONFIG.field_offset | 0) : 0x54;
        var PSF_SZ  = 0x80;
        var psfBuf  = Memory.alloc(PSF_SZ);
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var bval = (CONFIG.tests[i] | 0) & 0xff;
            var origV = null, reimV = null, errO = null, errR = null;
            for (var z = 0; z < PSF_SZ; z++) psfBuf.add(z).writeU8(0);
            psfBuf.add(PSF_OFF).writeU8(bval);
            try { origV = Orig(psfBuf) >>> 0; }   catch (e) { errO = e.message; }
            try { reimV = Reimpl(psfBuf) >>> 0; } catch (e) { errR = e.message; }
            results.push({ idx: i, input: bval, original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_list_count (added 2026-06-04, c3_batch_ab s1) ──────────────────
    // fn(anchor) -> int node count.  Hand-build a circular list (next@+4) with
    // N nodes WITHOUT the audio pool (pool is not ready at diff-attach), so the
    // traversal body is actually exercised.  Read-only => one structure, both
    // sides.  Used for AudioListNodeCount (0x005aded0).
    // MECHANISM: Builds a harness-only circular linked list of n nodes with next@+4 and ring
    // closed back to anchor; calls fn(anchor) and observes integer return; both sides share the
    // identical structure (read-only traversal, no per-side copy); no CONFIG fields; broader: any
    // fn(anchor) counting a circular list traversed via next@+4; tests[i] = n (node count).
    if (CONFIG.arg_type === 'audio_list_count') {
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var n = CONFIG.tests[i] | 0;
            var anchor = Memory.alloc(12);
            var nodes = [];
            for (var k = 0; k < n; k++) nodes.push(Memory.alloc(12));
            var prev = anchor;
            for (var k = 0; k < n; k++) { prev.add(4).writePointer(nodes[k]); prev = nodes[k]; }
            prev.add(4).writePointer(anchor);   // close the ring (also handles n==0)
            var origV = null, reimV = null, errO = null, errR = null;
            try { origV = Orig(anchor) | 0; }   catch (e) { errO = e.message; }
            try { reimV = Reimpl(anchor) | 0; } catch (e) { errR = e.message; }
            results.push({ idx: i, input: n, original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_list_find_index (added 2026-06-04, c3_batch_ab s1) ─────────────
    // fn(anchor, key) -> int index-of-key or -1.  Hand-build circular list
    // (next@+4, key@+8) from test.payloads; query test.key.  Read-only.
    // Used for AudioListIndexOfKey (0x005ade60).
    // MECHANISM: Builds a harness-only circular list (next@+4, key@+8 per node) from
    // test.payloads; calls fn(anchor, key) and observes int return (index or -1); both sides share
    // the identical list (read-only, no per-side copy); no CONFIG; broader: any circular-list
    // linear search fn(anchor, int_key) whose nodes carry key@+8 and next@+4; tests[i] = {
    // payloads:[int...], key:int }.
    if (CONFIG.arg_type === 'audio_list_find_index') {
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var pls = t.payloads || [];
            var key = t.key | 0;
            var anchor = Memory.alloc(12);
            var nodes = [];
            for (var k = 0; k < pls.length; k++) {
                var nd = Memory.alloc(12);
                nd.add(8).writeS32(pls[k] | 0);
                nodes.push(nd);
            }
            var prev = anchor;
            for (var k = 0; k < nodes.length; k++) { prev.add(4).writePointer(nodes[k]); prev = nodes[k]; }
            prev.add(4).writePointer(anchor);
            var origV = null, reimV = null, errO = null, errR = null;
            try { origV = Orig(anchor, key) | 0; }   catch (e) { errO = e.message; }
            try { reimV = Reimpl(anchor, key) | 0; } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t), original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── int2_ptr2_out (added 2026-06-04, c3_batch_ab s1) ─────────────────────
    // void fn(uint a, uint b, uint* hi, uint* lo).  Two scalar args + two 4-byte
    // out-slots; observable = "hi,lo" hex fingerprint.  Used for
    // AudioShiftAddMul64 (0x005aeda0).  test = [a, b].
    // MECHANISM: Two uint32 stack args plus two 4-byte out-pointers into a single harness-
    // allocated 8-byte buffer (cleared before each call pair); void return not observed;
    // fingerprints both output slots as "hi,lo" hex string. No CONFIG parameterization beyond
    // standard tests=[a,b]. Applies to any void fn(uint,uint,uint*,uint*) that writes exactly two
    // 4-byte results via out-pointers.
    if (CONFIG.arg_type === 'int2_ptr2_out') {
        var i2Buf = Memory.alloc(8);
        function i2Pack() {
            return ('00000000' + (i2Buf.readU32() >>> 0).toString(16)).slice(-8) + ',' +
                   ('00000000' + (i2Buf.add(4).readU32() >>> 0).toString(16)).slice(-8);
        }
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var a = (CONFIG.tests[i][0]) >>> 0;
            var b = (CONFIG.tests[i][1]) >>> 0;
            var origV = null, reimV = null, errO = null, errR = null;
            i2Buf.writeU32(0); i2Buf.add(4).writeU32(0);
            try { Orig(a, b, i2Buf, i2Buf.add(4)); origV = i2Pack(); }   catch (e) { errO = e.message; }
            i2Buf.writeU32(0); i2Buf.add(4).writeU32(0);
            try { Reimpl(a, b, i2Buf, i2Buf.add(4)); reimV = i2Pack(); } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(CONFIG.tests[i]),
                           original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_list_min_select (added 2026-06-04, c3_batch_ab s1) ─────────────
    // fn(anchor, thresh) -> int (selected payload pointer, or 0).  Hand-build a
    // circular list (next@+4, payload@+8) where each payload+0x54 -> keystruct,
    // keystruct+0x10 = key (from test.keys).  The raw return is a payload
    // pointer; map it back to its index so the A/B compares logical selection
    // (per-side pointer identity is meaningless).  Used for AudioListMinKeySelect
    // (0x005b0700).  test = { keys: [...], thresh: uint }.
    // MECHANISM: Builds a harness-only circular list (next@+4, payload_ptr@+8; payload+0x54 ->
    // keystruct; keystruct+0x10 = key); calls fn(anchor, thresh); maps the raw return pointer back
    // to its payload index (-1=null, -2=unknown ptr) so comparison is logical rather than pointer-
    // identical across sides; no CONFIG; NARROW: node/payload/keystruct layout fully hardcoded;
    // tests[i] = { keys:[uint...], thresh:uint }.
    if (CONFIG.arg_type === 'audio_list_min_select') {
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var keys = t.keys || [];
            var thresh = (t.thresh) >>> 0;
            var anchor = Memory.alloc(12);
            var nodes = [], payloads = [];
            for (var k = 0; k < keys.length; k++) {
                var ks = Memory.alloc(0x14);
                ks.add(0x10).writeU32(keys[k] >>> 0);
                var pl = Memory.alloc(0x58);
                pl.add(0x54).writePointer(ks);
                var nd = Memory.alloc(12);
                nd.add(8).writePointer(pl);
                nodes.push(nd); payloads.push(pl);
            }
            var prev = anchor;
            for (var k = 0; k < nodes.length; k++) { prev.add(4).writePointer(nodes[k]); prev = nodes[k]; }
            prev.add(4).writePointer(anchor);
            var idxOf = function (ret) {
                if (!ret) return -1;
                var rp = ptr(ret >>> 0);
                for (var m = 0; m < payloads.length; m++) { if (payloads[m].equals(rp)) return m; }
                return -2;   // returned a pointer we did not build -> mismatch signal
            };
            var origV = null, reimV = null, errO = null, errR = null;
            try { origV = idxOf(Orig(anchor, thresh) >>> 0); }   catch (e) { errO = e.message; }
            try { reimV = idxOf(Reimpl(anchor, thresh) >>> 0); } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t), original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── arena_block_free_predicate (added 2026-06-04, c3_batch_ab s1) ────────
    // bool fn(block) = **(block+8) == *(block+0xc).  Hand-build a 16-byte block:
    // block+8 -> headNode, block+0xc = end-sentinel value; headNode+0 (its next)
    // == sentinel for the fully-free case, != sentinel otherwise.  No pool.
    // Used for AudioArenaBlockIsFree (0x005ae590).  test = { free: bool }.
    // MECHANISM: Builds a 16-byte block (block+8 -> 12-byte headNode; block+0xc = hardcoded
    // sentinel 0x13572468); headNode+0 = sentinel (free=true) or sentinel^0xff (free=false); calls
    // fn(block) and observes boolean return (1/0); NARROW: offsets +8/+0xc and sentinel value are
    // hardcoded - only correct for a `**(p+8)==*(p+0xc)` predicate; CONFIG: none; tests[i] = {
    // free:bool }.
    if (CONFIG.arg_type === 'arena_block_free_predicate') {
        var ABF_SENT = 0x13572468;
        for (var i = 0; i < CONFIG.tests.length; i++) {
            var t = CONFIG.tests[i];
            var isFree = t.free ? true : false;
            var block = Memory.alloc(0x10);
            var headNode = Memory.alloc(0x10);
            block.add(8).writePointer(headNode);
            block.add(0xc).writeU32(ABF_SENT >>> 0);
            headNode.writeU32((isFree ? ABF_SENT : (ABF_SENT ^ 0xff)) >>> 0);
            var origV = null, reimV = null, errO = null, errR = null;
            try { origV = Orig(block) ? 1 : 0; }   catch (e) { errO = e.message; }
            try { reimV = Reimpl(block) ? 1 : 0; } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t), original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── eax_implicit_ptr / eax_implicit_int ─────────────────────────────────
    // Build a small RWX thunk that seeds EAX=imm32 then JMPs to the target.
    // Layout (10 bytes):
    //   B8 ?? ?? ?? ?? mov eax, imm32   ; bytes [0..4]; imm32 patched per iter
    //   FF 25 ?? ?? ?? ?? jmp [memptr]   ; would be 6 more, but easier:
    //   E9 ?? ?? ?? ?? jmp rel32         ; bytes [5..9]; rel32 patched per target
    // Since the orig and reimpl have different absolute addresses, we allocate
    // ONE thunk per side and emit a fresh rel32 for each. The MOV imm32 byte
    // window (offset 1..4) is rewritten before each call.
    //
    // The target functions consume EAX (Ghidra's `in_EAX`) but otherwise take
    // no stack args and use bare `RET`. After the original function returns,
    // EAX holds the result, which Frida's NativeFunction(thunk, ret_type, [])
    // captures as the integer/pointer return value.
    //
    // CONFIG.signature must declare ret type but EMPTY args list:
    //   {'ret': 'uint32', 'args': []}  — int and pointer both go through EAX.
    //
    // input vector: a list of uint32 sentinels (test_values seeded into EAX).
    // For eax_implicit_ptr, sentinels are addresses of small pre-allocated
    // scratch buffers (so dereferences inside the target don't AV). The
    // harness allocates N scratch buffers (32 bytes each) and rewrites the
    // input list so each test value points to a fresh buffer of zeroed data.
    // MECHANISM: Per-side `mov eax,imm32; jmp target` trampoline pre-loads EAX per test; no stack
    // args (CONFIG.signature.args must be empty). ptr variant substitutes per-test zeroed 64-byte
    // scratch-buffer addresses as the imm32 to prevent AV on dereferences; int variant passes raw
    // uint32. Observes return value only. CONFIG: tests[] (uint32/addr values), signature.ret,
    // crash_equal_ok. Applies to any fn whose sole arg arrives in EAX.
    if (CONFIG.arg_type === 'eax_implicit_ptr' || CONFIG.arg_type === 'eax_implicit_int') {
        // Allocate scratch buffers for ptr mode (32-byte each, zero-init).
        const isPtr   = (CONFIG.arg_type === 'eax_implicit_ptr');
        const SCRATCH = 64;  // each scratch buffer

        // Build a per-side trampoline:  B8 imm32 (mov eax,X)  E9 rel32 (jmp tgt)
        // Total 10 bytes. We use one trampoline per side because the rel32 is
        // computed once per target absolute address.
        function buildTrampoline(targetAddr) {
            const code = Memory.alloc(Process.pageSize);
            Memory.patchCode(code, 10, function (cw) {
                const w = new X86Writer(cw, { pc: code });
                w.putBytes([0xB8, 0x00, 0x00, 0x00, 0x00]);  // mov eax, 0 (patched)
                w.putJmpAddress(targetAddr);                  // jmp rel32 -> target
                w.flush();
            });
            return code;
        }
        const trampO = buildTrampoline(TARGET_ADDR);
        const trampR = buildTrampoline(reimplAddr);
        const FnO = new NativeFunction(trampO, CONFIG.signature.ret, [], 'mscdecl');
        const FnR = new NativeFunction(trampR, CONFIG.signature.ret, [], 'mscdecl');

        // For ptr mode, pre-allocate fresh scratch buffers (one per test).
        const scratchBufs = [];
        if (isPtr) {
            for (let i = 0; i < CONFIG.tests.length; i++) {
                const b = Memory.alloc(SCRATCH);
                // Zero out the buffer (32 bytes worth covers most struct prefixes).
                for (let k = 0; k < SCRATCH; k += 4) b.add(k).writeU32(0);
                scratchBufs.push(b);
            }
        }

        for (let i = 0; i < CONFIG.tests.length; i++) {
            let eaxVal;
            if (isPtr) {
                // Use the scratch buffer address as the EAX value; the raw test
                // entry is recorded as the input marker only.
                eaxVal = parseInt(scratchBufs[i].toString(), 16) >>> 0;
            } else {
                eaxVal = (CONFIG.tests[i] >>> 0);
            }
            // Patch imm32 at offset 1 of each trampoline.
            trampO.add(1).writeU32(eaxVal);
            trampR.add(1).writeU32(eaxVal);

            let origV = null, reimV = null, errO = null, errR = null;
            try {
                const r = FnO();
                origV = (r === undefined || r === null) ? 0
                      : (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0)
                      : (r >>> 0);
            } catch (e) { errO = e.message; }
            try {
                const r = FnR();
                reimV = (r === undefined || r === null) ? 0
                      : (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0)
                      : (r >>> 0);
            } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: CONFIG.tests[i],
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── thiscall_field_get ───────────────────────────────────────────────────
    // SYNTHETIC field-getter harness for the shape
    //     <ret> fn(this)  ->  return *(this + field_off);
    // The function takes a single struct/this pointer (delivered on the stack as
    // __cdecl in MASHED — e.g. 0x005a89a0 `MOV EAX,[ESP+4]; MOV EAX,[EAX+0xD6C];
    // RET`, NOT an ECX thiscall despite the conventional name) and returns one
    // field read from it. We seed the struct OURSELVES so the test works at a
    // PLAIN menu-attach with no live state required.
    //
    // Per test: alloc a fresh zeroed scratch struct (retained in _keep[] so Frida
    // GC can't reclaim it while the raw pointer is the only live ref — same hazard
    // as struct_call_observe), write the test-vector value at field_off, call
    // fn(scratchPtr) on each side, read the return. Varying the seed across vectors
    // makes the return echo the seed -> NON-DEGENERATE (a `return 0;` stub would
    // mismatch). The pointer is passed positionally; CONFIG.signature.args must be
    // ['pointer'] with calling_convention 'mscdecl' (the default).
    //
    // CONFIG.field_off    : int    byte offset of the read field within the struct.
    // CONFIG.ret_kind     : 'u32'|'int'|'float'  how to interpret/seed the value.
    // CONFIG.struct_size  : int    bytes to allocate (>= field_off + 8). Default
    //                              field_off + 64.
    // CONFIG.tests        : flat list of seed values (u32/int/float per ret_kind).
    // MECHANISM: Per-side: allocates a fresh zeroed scratch struct (CONFIG.struct_size, default
    // field_off+64); writes the test seed at CONFIG.field_off (u32/int/float per CONFIG.ret_kind);
    // passes the struct pointer as the sole __cdecl stack arg; observable is fn's return value
    // compared by value. CONFIG: `field_off`, `ret_kind`, `struct_size`. Broader: any
    // fn(ptr)->scalar that reads one field at a configurable byte offset regardless of struct type
    // or register convention.
    if (CONFIG.arg_type === 'thiscall_field_get') {
        const fieldOff = CONFIG.field_off | 0;
        const retKind  = CONFIG.ret_kind || 'u32';
        const SS = CONFIG.struct_size || (fieldOff + 64);
        const _keep = [];
        const seedVal = function (p, v) {
            const a = p.add(fieldOff);
            if (retKind === 'float') a.writeFloat(v);
            else                     a.writeU32(v >>> 0);
        };
        const callSide = function (Fn, structPtr) {
            const r = Fn(structPtr);
            if (retKind === 'float') {
                // Frida returns a JS number for a 'float' NativeFunction ret.
                return (typeof r === 'number') ? r : null;
            }
            // int/u32 — normalise pointer/number to u32.
            if (r === undefined || r === null) return null;
            return (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0) : (r >>> 0);
        };
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const v = CONFIG.tests[i];
            const structO = Memory.alloc(SS), structR = Memory.alloc(SS);
            _keep.push(structO, structR);
            for (let b = 0; b < SS; b += 4) { structO.add(b).writeU32(0); structR.add(b).writeU32(0); }
            seedVal(structO, v); seedVal(structR, v);
            let origV = null, reimV = null, errO = null, errR = null;
            try { origV = callSide(Orig,   structO); } catch (e) { errO = e.message; }
            try { reimV = callSide(Reimpl, structR); } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: v,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── thiscall_nested_field_get ─────────────────────────────────────────────
    // Like thiscall_field_get, but the observed field is ONE pointer-indirection
    // deep: ret = *(*(this + outer_off) + inner_off) [& mask, applied inside fn].
    // thiscall_field_get can't drive these (its zeroed scratch has this+outer_off=0
    // -> the inner deref hits the null page -> AV). Here we allocate BOTH an outer
    // struct AND an inner buffer, write the inner ptr at this+outer_off, then seed
    // the field at inner+inner_off. CONFIG: outer_off, inner_off, ret_kind('u32'|
    // 'float'), struct_size, inner_size. Precedent consumer: 0x004c0b10
    // (*(*(this+0xa0)+3) & 3). Bit-identity is integer-only (no x87 issue).
    // MECHANISM: Calls fn(this_ptr) with per-test outer (CONFIG.struct_size) and inner
    // (CONFIG.inner_size) harness buffers; inner ptr written at outer+CONFIG.outer_off; test value
    // seeded at inner+CONFIG.inner_off; observable is return value only (CONFIG.ret_kind
    // 'u32'|'float'). Generalises thiscall_field_get to one pointer-deref depth - the zero-filled
    // outer alone would AV at the inner deref; CONFIG: outer_off, inner_off, ret_kind,
    // struct_size, inner_size.
    if (CONFIG.arg_type === 'thiscall_nested_field_get') {
        const outerOff = CONFIG.outer_off | 0;
        const innerOff = CONFIG.inner_off | 0;
        const retKind  = CONFIG.ret_kind || 'u32';
        const OS = CONFIG.struct_size || (outerOff + 16);
        const IS = CONFIG.inner_size  || (innerOff + 16);
        const _keep = [];
        const seedInner = function (innerP, v) {
            const a = innerP.add(innerOff);
            if (retKind === 'float') a.writeFloat(v);
            else                     a.writeU32(v >>> 0);
        };
        const callSide = function (Fn, structPtr) {
            const r = Fn(structPtr);
            if (retKind === 'float') return (typeof r === 'number') ? r : null;
            if (r === undefined || r === null) return null;
            return (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0) : (r >>> 0);
        };
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const v = CONFIG.tests[i];
            const outerO = Memory.alloc(OS), outerR = Memory.alloc(OS);
            const innerO = Memory.alloc(IS), innerR = Memory.alloc(IS);
            _keep.push(outerO, outerR, innerO, innerR);
            for (let b = 0; b < OS; b += 4) { outerO.add(b).writeU32(0); outerR.add(b).writeU32(0); }
            for (let b = 0; b < IS; b += 4) { innerO.add(b).writeU32(0); innerR.add(b).writeU32(0); }
            outerO.add(outerOff).writePointer(innerO);
            outerR.add(outerOff).writePointer(innerR);
            seedInner(innerO, v); seedInner(innerR, v);
            let origV = null, reimV = null, errO = null, errR = null;
            try { origV = callSide(Orig,   outerO); } catch (e) { errO = e.message; }
            try { reimV = callSide(Reimpl, outerR); } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: v,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── fastcall_reg ─────────────────────────────────────────────────────────
    // Register-convention force-call for __fastcall / __thiscall LEAF functions
    // whose arguments live ENTIRELY in ECX (+ EDX) with NO stack args. This is
    // the two-register generalization of the eax_implicit trampoline above and
    // unblocks the largest signature-rejected bucket (the ~93 "fastcall ECX/EDX
    // args" candidates flagged by c3_filter_v4).
    //
    // Trampoline (15 bytes, one per side; jmp rel32 fixed at build, the two
    // imm32 windows patched per iteration):
    //   B9 ?? ?? ?? ??   mov ecx, imm32       ; bytes [0..4],  imm patched at +1
    //   BA ?? ?? ?? ??   mov edx, imm32       ; bytes [5..9],  imm patched at +6
    //   E9 ?? ?? ?? ??   jmp rel32 -> target  ; bytes [10..14]
    // The target consumes ECX (and EDX) and ends with a callee-clean RET (RET 0
    // for a 2-register fastcall — no stack args), so wrapping the trampoline as
    // NativeFunction(tramp, ret, [], 'mscdecl') leaves ESP balanced across
    // iterations and captures EAX as the return value (int/ptr/void only — for
    // float/ST0 returns use a dedicated arg_type).
    //
    // CONFIG.signature      : { ret: 'uint32'|'int32'|'pointer'|'void', args: [] }
    //                         (EMPTY args — the registers ARE the arguments).
    // CONFIG.fastcall_nargs : 1 (ECX only; EDX seeded 0) | 2 (ECX+EDX). Default 2.
    // CONFIG.fastcall_ecx_ptr / fastcall_edx_ptr : bool. If set, that register is
    //                         seeded with the address of a fresh 64-byte zeroed
    //                         scratch buffer (one per test) so a deref inside the
    //                         target hits zeroed memory instead of AVing; the raw
    //                         CONFIG.tests entry is kept only as the input marker.
    // CONFIG.tests          : per test, a scalar ECX value (nargs==1, int mode) or
    //                         a 2-element [ecxVal, edxVal] (nargs==2, int mode).
    //                         For a ptr register the value is replaced by a
    //                         scratch-buffer address.
    // MECHANISM: Drives __fastcall via a patched `mov ecx; mov edx; jmp` trampoline - ECX and/or
    // EDX carry scalar or per-test 64-byte zeroed scratch-buffer-pointer args
    // (CONFIG.fastcall_nargs=1|2, CONFIG.fastcall_ecx_ptr, CONFIG.fastcall_edx_ptr); both sides
    // get identical register values; observes return value only (no buffer read-back). Applies to
    // any 1-2 register-arg fastcall leaf.
    if (CONFIG.arg_type === 'fastcall_reg') {
        const nargs   = (CONFIG.fastcall_nargs === 1) ? 1 : 2;
        const ecxPtr  = !!CONFIG.fastcall_ecx_ptr;
        const edxPtr  = !!CONFIG.fastcall_edx_ptr;
        const SCRATCH = 64;

        function buildFastcallTramp(targetAddr) {
            const code = Memory.alloc(Process.pageSize);
            Memory.patchCode(code, 15, function (cw) {
                const w = new X86Writer(cw, { pc: code });
                w.putBytes([0xB9, 0x00, 0x00, 0x00, 0x00]);  // mov ecx, 0 (patched at +1)
                w.putBytes([0xBA, 0x00, 0x00, 0x00, 0x00]);  // mov edx, 0 (patched at +6)
                w.putJmpAddress(targetAddr);                  // jmp rel32 -> target
                w.flush();
            });
            return code;
        }
        const trampO = buildFastcallTramp(TARGET_ADDR);
        const trampR = buildFastcallTramp(reimplAddr);
        const FnO = new NativeFunction(trampO, CONFIG.signature.ret, [], 'mscdecl');
        const FnR = new NativeFunction(trampR, CONFIG.signature.ret, [], 'mscdecl');

        // Fresh zeroed scratch buffer per test for any pointer register.
        const ecxScratch = [], edxScratch = [];
        for (let i = 0; i < CONFIG.tests.length; i++) {
            if (ecxPtr) { const b = Memory.alloc(SCRATCH); for (let k = 0; k < SCRATCH; k += 4) b.add(k).writeU32(0); ecxScratch.push(b); }
            if (edxPtr) { const b = Memory.alloc(SCRATCH); for (let k = 0; k < SCRATCH; k += 4) b.add(k).writeU32(0); edxScratch.push(b); }
        }
        function asU32(v) {
            return (typeof v === 'object') ? (parseInt(v.toString(), 16) >>> 0) : (v >>> 0);
        }

        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t      = CONFIG.tests[i];
            const rawEcx = Array.isArray(t) ? t[0] : t;
            const rawEdx = Array.isArray(t) ? (t[1] | 0) : 0;
            const ecxVal = ecxPtr ? asU32(ecxScratch[i]) : (rawEcx >>> 0);
            const edxVal = (nargs === 2) ? (edxPtr ? asU32(edxScratch[i]) : (rawEdx >>> 0)) : 0;
            trampO.add(1).writeU32(ecxVal); trampO.add(6).writeU32(edxVal);
            trampR.add(1).writeU32(ecxVal); trampR.add(6).writeU32(edxVal);

            let origV = null, reimV = null, errO = null, errR = null;
            try {
                const r = FnO();
                origV = (r === undefined || r === null) ? 0
                      : (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0) : (r >>> 0);
            } catch (e) { errO = e.message; }
            try {
                const r = FnR();
                reimV = (r === undefined || r === null) ? 0
                      : (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0) : (r >>> 0);
            } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: t,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── vec3_global_mul_observe ─────────────────────────────────────────────
    // For functions that read a 3-float vec3 from globals, mutate, write back.
    // Example: 0x0046c570 reads 3 floats at base+stride*idx and multiplies by
    // a damping scalar global.
    //
    // CONFIG.target_global_base   string (hex addr, e.g. '0x00881f50')
    // CONFIG.target_global_stride int    (per-index stride in BYTES)
    // CONFIG.signature.args       ['int32']  (the index argument)
    //
    // Each test: { idx, vec3: [x,y,z] }.
    // Strategy: write test vec3 to globals[idx*stride+0/4/8], save original,
    // call fn(idx), read back globals as 3 u32 fingerprints, restore originals.
    // Both orig and reimpl must produce identical post-call globals.
    // MECHANISM: Seeds globals[idx*stride+0..8] as f32 from test.vec3 (CONFIG.target_global_base,
    // CONFIG.target_global_stride); calls fn(idx) or fn() per CONFIG.signature.args.length;
    // fingerprints three post-call global u32s and restores originals between sides; return value
    // NOT observed. CONFIG.crash_equal_ok optional. Applies to any in-place vec3 mutation indexed
    // over a configurable stride-based global array.
    if (CONFIG.arg_type === 'vec3_global_mul_observe') {
        const base   = ptr(CONFIG.target_global_base);
        const stride = (CONFIG.target_global_stride | 0);
        const Fn1arg = CONFIG.signature.args.length === 1;
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t   = CONFIG.tests[i];
            const idx = t.idx | 0;
            const v   = t.vec3;
            const gx  = base.add(idx * stride + 0);
            const gy  = base.add(idx * stride + 4);
            const gz  = base.add(idx * stride + 8);
            // Save current values so we can restore between orig and reimpl.
            const sx = gx.readU32(), sy = gy.readU32(), sz = gz.readU32();
            let origV = null, reimV = null, errO = null, errR = null;
            try {
                gx.writeFloat(v[0]); gy.writeFloat(v[1]); gz.writeFloat(v[2]);
                if (Fn1arg) { Orig(idx); } else { Orig(); }
                origV = [gx.readU32(), gy.readU32(), gz.readU32()].join(',');
            } catch (e) { errO = e.message; }
            // Restore so reimpl sees the same starting state.
            gx.writeU32(sx); gy.writeU32(sy); gz.writeU32(sz);
            try {
                gx.writeFloat(v[0]); gy.writeFloat(v[1]); gz.writeFloat(v[2]);
                if (Fn1arg) { Reimpl(idx); } else { Reimpl(); }
                reimV = [gx.readU32(), gy.readU32(), gz.readU32()].join(',');
            } catch (e) { errR = e.message; }
            // Final restore.
            gx.writeU32(sx); gy.writeU32(sy); gz.writeU32(sz);
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── fmt_desc_pair_compare ───────────────────────────────────────────────
    // For audio_rws fmt comparator/transformer functions. Generalises:
    //   0x005ac5f0 fn(int* a, int* b)            -> 0/1 match
    //   0x005ac9e0 fn(u32* entry, u32* candidate) -> 0/1 match
    //   0x005acaa0 fn(u32* dst, u32* src, p3, p4) -> dst|null (pack/unpack)
    //
    // Each test: { a: {fNN: u32, ...}, b: {fNN: u32, ...}, p3?: int, p4?: int }.
    // The harness allocates two 0x20-byte scratch buffers (or 4 for 005acaa0
    // when extra-data fixup might write off the end — capped at 0x40).
    // Field writes: each key in `a`/`b` of the form `fXX` -> writes a u32 at
    // offset 0xXX in the corresponding buffer (parsed via parseInt(key.slice(1),16)).
    // Calls fn(aBuf, bBuf [, p3, p4]) then returns a packed fingerprint:
    //   For 2-arg form (signature.args.length == 2): (retU32 << 24) ^
    //     bufA fingerprint ^ (bufB fingerprint << 8) -- both buffers since
    //     some comparators may set flag bits in either side.
    //   For 4-arg form: same shape, with p3/p4 routed through.
    // MECHANISM: fn(bufA, bufB [, p3, p4]); allocates two 0x40-byte scratch bufs, populates sparse
    // fields via test.a/{fXX} -> u32@offset 0xXX notation, bufs zeroed before each call;
    // fingerprint is (retU32_low16, rolling-polynomial-hash(bufA), rolling-polynomial-hash(bufB));
    // 2 vs 4 args driven by CONFIG.signature.args.length; crash_equal_ok flag; broadly fits any
    // fn(ptr, ptr) or fn(ptr, ptr, int, int) that mutates either buffer - domain-agnostic, field
    // layout fully per-test.
    if (CONFIG.arg_type === 'fmt_desc_pair_compare') {
        const SZ = 0x40;  // 64-byte buffers — generous; covers fmt-desc + ext
        const bufA = Memory.alloc(SZ);
        const bufB = Memory.alloc(SZ);
        const argc = CONFIG.signature.args.length;
        // For 4-arg pack/unpack form, dst[+0x18] may have flags from prior
        // calls; we zero both bufs before EACH call to prevent cross-iter
        // contamination.
        function fillBuf(b, fields) {
            for (let k = 0; k < SZ; k += 4) b.add(k).writeU32(0);
            if (fields) {
                for (const key of Object.keys(fields)) {
                    if (!key.startsWith('f')) continue;
                    const off = parseInt(key.slice(1), 16);
                    if (Number.isNaN(off) || off + 4 > SZ) continue;
                    b.add(off).writeU32(fields[key] >>> 0);
                }
            }
        }
        function fingerprint(b) {
            let fp = 0;
            for (let k = 0; k < SZ; k += 4) {
                fp = ((fp * 31) ^ b.add(k).readU32()) >>> 0;
            }
            return fp;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            let origV = null, reimV = null, errO = null, errR = null;
            // ── orig call ──
            try {
                fillBuf(bufA, t.a);
                fillBuf(bufB, t.b);
                let ret;
                if (argc === 4) {
                    ret = Orig(bufA, bufB, (t.p3 | 0), (t.p4 | 0));
                } else {
                    ret = Orig(bufA, bufB);
                }
                const retU = (ret === null || ret === undefined) ? 0
                           : (typeof ret === 'object') ? (parseInt(ret.toString(), 16) >>> 0)
                           : (ret >>> 0);
                origV = [(retU & 0xffff).toString(16),
                         fingerprint(bufA).toString(16),
                         fingerprint(bufB).toString(16)].join(',');
            } catch (e) { errO = e.message; }
            // ── reimpl call ──
            try {
                fillBuf(bufA, t.a);
                fillBuf(bufB, t.b);
                let ret;
                if (argc === 4) {
                    ret = Reimpl(bufA, bufB, (t.p3 | 0), (t.p4 | 0));
                } else {
                    ret = Reimpl(bufA, bufB);
                }
                const retU = (ret === null || ret === undefined) ? 0
                           : (typeof ret === 'object') ? (parseInt(ret.toString(), 16) >>> 0)
                           : (ret >>> 0);
                reimV = [(retU & 0xffff).toString(16),
                         fingerprint(bufA).toString(16),
                         fingerprint(bufB).toString(16)].join(',');
            } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── draw_quad_observe ───────────────────────────────────────────────────
    // For Im2D quad/sprite-draw functions that write a 4-vertex (28 B/vert)
    // buffer at DAT_00898a20 then dispatch through the RW driver vtable at
    // DAT_007d3ff8+0x30. Observable = fingerprint of the 112-byte vertex
    // buffer post-call. Both orig + reimpl invoke the same live vtable, so
    // if the buffer state matches, the geometry built is bit-identical.
    //
    // Works for any arg count — dispatches via CONFIG.signature.args.length.
    // Pointer-typed positional args ('pointer' in signature.args) are passed
    // through ptr() with the test value as the raw address (use 0 for NULL).
    // Float-typed args go through as JS numbers (NativeFunction promotes).
    //
    // Tests: flat array per call whose length matches signature.args.length.
    //   5-arg form: [x, y, w, h, argb]            (ChromeBaseDraw / gradient)
    //   7-arg form: [tex, x, y, w, h, argb, mode] (TextSpriteUVExplicit)
    //  12-arg form: [tex, x, y, w, h, argb, u0, u1, v0, v1, scale_mode, blend]
    //
    // CONFIG fields:
    //   vbuf_addr_str   string (hex) — override DAT_00898a20 if needed
    //   vbuf_len        int          — override 112 if buffer size differs
    // MECHANISM: Passes CONFIG.tests[i] as positional args per CONFIG.signature.args (any count;
    // 'pointer' entries coerced via ptr(), float as-is); zeroes VBUF (ptr(CONFIG.vbuf_addr_str,
    // default 0x00898a20), CONFIG.vbuf_len bytes, default 112) before each side; observable is a
    // polynomial fingerprint of VBUF post-call; fn return value is NOT compared; VBUF restored
    // after. CONFIG: `vbuf_addr_str`, `vbuf_len`, `signature`. Broader: any 5/7/12-arg draw call
    // that fills a known fixed-address vertex buffer.
    if (CONFIG.arg_type === 'draw_quad_observe') {
        const VBUF = ptr(CONFIG.vbuf_addr_str || '0x00898a20');
        const VLEN = (CONFIG.vbuf_len | 0) || 112;
        const sigArgs = CONFIG.signature.args;
        const argc    = sigArgs.length;

        // Save buffer contents so live game state is restored after the test.
        const savedBuf = new Array(VLEN);
        for (let k = 0; k < VLEN; k++) {
            try { savedBuf[k] = VBUF.add(k).readU8(); }
            catch (e) { savedBuf[k] = 0; }
        }
        function fingerprintVbuf() {
            let fp = 0;
            for (let k = 0; k < VLEN; k++) {
                fp = ((fp * 31) ^ VBUF.add(k).readU8()) >>> 0;
            }
            return fp;
        }
        function packArgs(t) {
            const a = [];
            for (let k = 0; k < argc; k++) {
                if (sigArgs[k] === 'pointer') a.push(ptr((t[k] | 0) >>> 0));
                else a.push(t[k]);
            }
            return a;
        }

        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const args = packArgs(t);
            let origV = null, reimV = null, errO = null, errR = null;
            // Zero vbuf, run Orig, fingerprint.
            for (let k = 0; k < VLEN; k++) VBUF.add(k).writeU8(0);
            try { Orig.apply(null, args); origV = fingerprintVbuf(); }
            catch (e) {
                errO = e.message;
                try { origV = fingerprintVbuf(); } catch (_) {}
            }
            // Zero vbuf, run Reimpl, fingerprint.
            for (let k = 0; k < VLEN; k++) VBUF.add(k).writeU8(0);
            try { Reimpl.apply(null, args); reimV = fingerprintVbuf(); }
            catch (e) {
                errR = e.message;
                try { reimV = fingerprintVbuf(); } catch (_) {}
            }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        // Restore live buffer.
        for (let k = 0; k < VLEN; k++) {
            try { VBUF.add(k).writeU8(savedBuf[k]); } catch (_) {}
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── out_buf_fmt_2 ───────────────────────────────────────────────────────
    // For MenusLapTimeFmt-style: void(int p1, uint32 p2, char* outA, char* outB).
    // Both output buffers receive sprintf-style formatted bytes; observable is
    // the C-string contents of each, joined by '|'.
    //
    // Tests: [p1, p2] pair (or a single int for the common p2=0 case).
    // CONFIG.out_buf_size: per-buffer size (default 32). Both buffers zeroed
    // before each call.
    // MECHANISM: fn(int p1, uint32 p2, char* outA, char* outB)->void; harness allocates four
    // separate Memory.alloc buffers (two per side, size CONFIG.out_buf_size, default 32), zeros
    // them before each call, calls Orig/Reimpl independently, observes null-terminated C-string
    // contents of both output buffers joined by '|'; return value not observed; input per test is
    // [p1,p2] or scalar (p2 defaults 0); CONFIG: out_buf_size.
    if (CONFIG.arg_type === 'out_buf_fmt_2') {
        const BUF_SIZE = (CONFIG.out_buf_size | 0) || 32;
        const bufAo = Memory.alloc(BUF_SIZE);
        const bufBo = Memory.alloc(BUF_SIZE);
        const bufAr = Memory.alloc(BUF_SIZE);
        const bufBr = Memory.alloc(BUF_SIZE);
        function readCStr(b, max) {
            let s = '';
            for (let k = 0; k < max; k++) {
                const c = b.add(k).readU8();
                if (c === 0) break;
                s += String.fromCharCode(c);
            }
            return s;
        }
        function zero(b) { for (let k = 0; k < BUF_SIZE; k++) b.add(k).writeU8(0); }

        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const p1 = (typeof t === 'number') ? (t | 0) : (t[0] | 0);
            const p2 = (typeof t === 'number') ? 0     : (t[1] >>> 0);
            let origV = null, reimV = null, errO = null, errR = null;

            zero(bufAo); zero(bufBo);
            try { Orig(p1, p2, bufAo, bufBo);
                  origV = readCStr(bufAo, BUF_SIZE) + '|' + readCStr(bufBo, BUF_SIZE); }
            catch (e) { errO = e.message; }

            zero(bufAr); zero(bufBr);
            try { Reimpl(p1, p2, bufAr, bufBr);
                  reimV = readCStr(bufAr, BUF_SIZE) + '|' + readCStr(bufBr, BUF_SIZE); }
            catch (e) { errR = e.message; }

            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── trig_text_draw ──────────────────────────────────────────────────────
    // For MenusLapTimeCmp-style 6-arg void functions that call:
    //   FUN_004282a0(sprite_id, p5)   — text-measure, leaves width on ST0
    //   FUN_0042b8c0(...)             — compute angle
    //   FUN_0042b8b0()                — screen-width getter
    //   FUN_00427ff0(sid, adj_x, adj_y) — final text-draw (the observable)
    //
    // We Interceptor.replace the final draw callee with a NativeCallback that
    // captures (sid, adj_x, adj_y). Both orig and reimpl call through the same
    // patched address, so bit-identical (adj_x, adj_y) implies bit-identical
    // upstream math (including the ST0-implicit text-width handoff).
    //
    // CONFIG.draw_callee_rva_str: hex addr of the draw callee (default
    // '0x00427ff0'). Signature of the callee is void(uint32, float, float).
    // Tests: [sprite_id, x, y, p4, p5, p6].
    // MECHANISM: Interceptor.replaces the draw callee at CONFIG.draw_callee_rva_str (default
    // 0x00427ff0) with a capture stub before calling fn(uint32, float, float, uint32, uint32,
    // uint32) - 6 stack args from test vector; observable is the 3-tuple (sid, adj_x_bits,
    // adj_y_bits) that the patched draw callee received, NOT any output buffer; reverts replace in
    // finally; CONFIG: `draw_callee_rva_str`, `crash_equal_ok`. Broader: any multi-arg pipeline
    // that terminates in a single capturable callee, provided that callee address is configurable
    // via draw_callee_rva_str.
    if (CONFIG.arg_type === 'trig_text_draw') {
        const drawAddr = ptr(CONFIG.draw_callee_rva_str || '0x00427ff0');
        let capturedArgs = null;
        const captureStub = new NativeCallback(function (sid, ax, ay) {
            capturedArgs = { sid: sid >>> 0, ax: ax, ay: ay };
        }, 'void', ['uint32', 'float', 'float'], 'mscdecl');
        Interceptor.replace(drawAddr, captureStub);

        const fbScratch = Memory.alloc(4);
        function floatBits(f) { fbScratch.writeFloat(f); return fbScratch.readU32(); }

        try {
            for (let i = 0; i < CONFIG.tests.length; i++) {
                const t = CONFIG.tests[i];
                let origV = null, reimV = null, errO = null, errR = null;

                capturedArgs = null;
                try { Orig(t[0] >>> 0, t[1], t[2], t[3] >>> 0, t[4] >>> 0, t[5] >>> 0); }
                catch (e) { errO = e.message; }
                const oa = capturedArgs;
                origV = oa ? [oa.sid, floatBits(oa.ax), floatBits(oa.ay)].join(',')
                           : 'no-call';

                capturedArgs = null;
                try { Reimpl(t[0] >>> 0, t[1], t[2], t[3] >>> 0, t[4] >>> 0, t[5] >>> 0); }
                catch (e) { errR = e.message; }
                const ra = capturedArgs;
                reimV = ra ? [ra.sid, floatBits(ra.ax), floatBits(ra.ay)].join(',')
                           : 'no-call';

                const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
                results.push({ idx: i, input: JSON.stringify(t),
                               original: origV, reimpl: reimV,
                               match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                               err_original: errO, err_reimpl: errR });
            }
        } finally {
            Interceptor.revert(drawAddr);
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── sprite_table_dispatch ───────────────────────────────────────────────
    // For SpriteSlotDispatch (0x0042fab0) and SpriteAnimFrameThunk (0x0042e590):
    // 10-way / lookup-based dispatchers that compute a sprite ptr from
    // sprite_slot, then JMP/CALL a downstream callee with that ptr as arg 0.
    // The downstream callee (FUN_0040bb90 / FUN_0040bb70) dereferences a
    // global linked-list head (DAT_0063b904) that is NULL at diff-attach time
    // — both Orig and Reimpl AV identically at offset 0x8, and AV/AV match
    // is banned as GREEN.
    //
    // Strategy (modeled on trig_text_draw): Interceptor.replace the callee
    // with a NativeCallback that captures the first arg (the sprite ptr) and
    // returns immediately. Both Orig and Reimpl call through the patched
    // address, so the captures reflect the dispatcher's bit-identical
    // sprite-ptr-computation. The callee's downstream code never runs.
    //
    // Out-of-range slots: SpriteSlotDispatch's default case returns without
    // calling the callee; the capture stays null. Both sides null === null
    // is still a real match (both took the no-call path).
    //
    // CONFIG.callee_rva_str: hex addr of the callee to patch
    //   (default '0x0040bb90' for SpriteSlotDispatch;
    //    set to '0x0040bb70' for SpriteAnimFrameThunk).
    // CONFIG.signature: caller-side signature (already routed through CONFIG).
    //   For SpriteAnimFrameThunk, declare 'void(int32)' even though the real
    //   function takes 9 args — only the first arg matters; remaining stack
    //   bytes are garbage but the patched callee only reads its first arg.
    // Tests: flat list of slot indices.
    // MECHANISM: Patches CONFIG.callee_rva_str (default 0x0040bb90) with a NativeCallback recorder
    // via Interceptor.replace; calls fn(slot>>>0) for each test integer; observes the pointer arg
    // the stub captured - both null (no-call path) is a valid match; reverts the callee in
    // finally. Parameterised by CONFIG.callee_rva_str and CONFIG.signature; applies to any int-arg
    // dispatcher that routes to one pointer-taking callee.
    if (CONFIG.arg_type === 'sprite_table_dispatch') {
        const calleeAddr = ptr(CONFIG.callee_rva_str || '0x0040bb90');
        let capturedPtr = null;
        const captureStub = new NativeCallback(function (ptrArg) {
            capturedPtr = ptrArg ? (parseInt(ptrArg.toString(), 16) >>> 0) : 0;
        }, 'void', ['pointer'], 'mscdecl');
        Interceptor.replace(calleeAddr, captureStub);
        try {
            for (let i = 0; i < CONFIG.tests.length; i++) {
                const slot = CONFIG.tests[i] | 0;
                let origV = null, reimV = null, errO = null, errR = null;
                capturedPtr = null;
                try { Orig(slot >>> 0); } catch (e) { errO = e.message; }
                origV = capturedPtr;
                capturedPtr = null;
                try { Reimpl(slot >>> 0); } catch (e) { errR = e.message; }
                reimV = capturedPtr;
                // Real match: both produced the same captured ptr value
                // (real callee invocation) OR both bypassed the callee
                // (out-of-range path → both null). Errors must also match.
                const sameCapture = (origV === reimV);
                const sameErr     = (errO === errR);
                results.push({ idx: i, input: slot,
                               original: origV, reimpl: reimV,
                               match: sameCapture && sameErr,
                               err_original: errO, err_reimpl: errR });
            }
        } finally {
            Interceptor.revert(calleeAddr);
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── spin_angle_observe ──────────────────────────────────────────────────
    // For HudSpinCoinAnim-style: void(int param_1, int param_2).
    // Problem: function increments _DAT_0067d974 (spin angle accumulator) each
    // call, so the orig and reimpl calls would see different accumulator states
    // within one test cycle — draw_quad_observe alone cannot isolate them.
    // Strategy: before EACH sub-call (orig and reimpl), reset DAT_0067d974 to
    // a known sentinel value (the test input cast as float), call fn(p1, p2),
    // then fingerprint the 112-byte vertex buffer at DAT_00898a20.
    // Both paths see the same accumulator seed → bit-identical geometry → match.
    // Tests: [[p1, p2, angle_seed_float], ...] where angle_seed_float is the
    // float32 spin angle to inject before each call (e.g. 0.0, 1.5708, etc.).
    //
    // CONFIG fields:
    //   vbuf_addr_str   string (hex) — vertex buffer base (default '0x00898a20')
    //   vbuf_len        int          — vertex buffer size (default 112)
    //   angle_global_str string (hex) — spin angle accumulator addr (default '0x0067d974')
    // MECHANISM: Calls void fn(int p1, int p2); before each sub-call (both sides separately)
    // zeroes CONFIG.vbuf_len bytes at CONFIG.vbuf_addr_str and injects the per-test float seed
    // (tests[i][2]) into CONFIG.angle_global_str; observable is a 32-bit rolling hash of the
    // vertex buffer post-call. Saves and restores both globals after the batch. Solves per-call
    // accumulator drift that would diverge orig/reimpl within one test cycle.
    if (CONFIG.arg_type === 'spin_angle_observe') {
        const VBUF2   = ptr(CONFIG.vbuf_addr_str  || '0x00898a20');
        const VLEN2   = (CONFIG.vbuf_len  | 0) || 112;
        const AADDR   = ptr(CONFIG.angle_global_str || '0x0067d974');
        const ANGVBUF = Memory.alloc(4);  // scratch for float write
        // Save vbuf contents so live game state is restored after the test.
        const savedBuf2 = new Array(VLEN2);
        for (let k = 0; k < VLEN2; k++) {
            try { savedBuf2[k] = VBUF2.add(k).readU8(); } catch (e) { savedBuf2[k] = 0; }
        }
        // Save the current spin angle so we restore it after the test batch.
        let savedAngle = 0;
        try { savedAngle = AADDR.readU32(); } catch (e) {}
        function fpVbuf2() {
            let fp = 0;
            for (let k = 0; k < VLEN2; k++) {
                fp = ((fp * 31) ^ VBUF2.add(k).readU8()) >>> 0;
            }
            return fp;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t  = CONFIG.tests[i];   // [p1, p2, angle_seed_float]
            const p1 = t[0] | 0;
            const p2 = t[1] | 0;
            const angleSeed = t[2];        // float JS number
            let origV = null, reimV = null, errO = null, errR = null;
            // ── orig call ──
            for (let k = 0; k < VLEN2; k++) VBUF2.add(k).writeU8(0);
            ANGVBUF.writeFloat(angleSeed);
            AADDR.writeU32(ANGVBUF.readU32());   // inject float bits
            try { Orig(p1, p2); origV = fpVbuf2(); } catch (e) { errO = e.message; try { origV = fpVbuf2(); } catch (_) {} }
            // ── reimpl call ──
            for (let k = 0; k < VLEN2; k++) VBUF2.add(k).writeU8(0);
            ANGVBUF.writeFloat(angleSeed);
            AADDR.writeU32(ANGVBUF.readU32());   // inject same float bits again
            try { Reimpl(p1, p2); reimV = fpVbuf2(); } catch (e) { errR = e.message; try { reimV = fpVbuf2(); } catch (_) {} }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        // Restore live vbuf and spin angle.
        for (let k = 0; k < VLEN2; k++) { try { VBUF2.add(k).writeU8(savedBuf2[k]); } catch (_) {} }
        try { AADDR.writeU32(savedAngle); } catch (_) {}
        send({ type: 'results', data: results });
        return;
    }

    // ── ptr_ptr_entity_set ───────────────────────────────────────────────────
    // For CarSlotStateSet-style: void(int param_1, uint32 param_2).
    // Function does: *(*target_global_addr + param_1*stride + field_offset) = param_2.
    // Read-back sequence: outer_ptr = *target_global_addr (a pointer);
    //   effective = outer_ptr + param_1*stride + field_offset; read u32 at effective.
    // Unlike entity_field_set (single-deref), this does one extra deref of the base.
    //
    // CONFIG fields:
    //   target_global        hex string — address that holds the outer pointer
    //   entity_byte_stride   int        — per-index stride (default 4)
    //   field_offset         int        — fixed byte offset after stride*p1 (default 0)
    //
    // If *target_global == NULL at call time, the write crashes — the harness
    // returns 0 for both sides (null-guard observable). Both paths must agree.
    // MECHANISM: fn(int p1, uint32 p2)->void; no harness buffer - reads live outer pointer from
    // CONFIG.target_global at runtime, computes effective=(*target_global)+p1*stride+field_offset
    // and reads u32 there after each call as observable; null outer pointer yields 0/0 match
    // (graceful); CONFIG: target_global (hex string), entity_byte_stride (default 4), field_offset
    // (default 0); broader than entity_field_set by one extra pointer deref of the base.
    if (CONFIG.arg_type === 'ptr_ptr_entity_set') {
        const outerPtrAddr  = ptr(CONFIG.target_global);
        const stride        = (CONFIG.entity_byte_stride | 0) || 4;
        const fieldOff      = (CONFIG.field_offset | 0) || 0;
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t  = CONFIG.tests[i];
            const p1 = t[0] | 0;
            const p2 = t[1] >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            // Read outer pointer; if null both sides will crash identically.
            let outerPtr = null;
            try { outerPtr = outerPtrAddr.readPointer(); } catch (e) { errO = errR = 'null-outer: ' + e.message; }
            if (outerPtr !== null && outerPtr.isNull()) {
                // Outer pointer is NULL — both writes would AV. Report 0/0 (match).
                results.push({ idx: i, input: JSON.stringify(t),
                               original: 0, reimpl: 0, match: true,
                               err_original: 'outer_null', err_reimpl: 'outer_null' });
                continue;
            }
            if (outerPtr === null) {
                results.push({ idx: i, input: JSON.stringify(t),
                               original: null, reimpl: null, match: false,
                               err_original: errO, err_reimpl: errR });
                continue;
            }
            // Compute effective address for read-back.
            const effective = outerPtr.add(p1 * stride + fieldOff);
            // ── orig call ──
            try { Orig(p1, p2); origV = effective.readU32(); } catch (e) { errO = e.message; }
            // ── reimpl call ──
            try { Reimpl(p1, p2); reimV = effective.readU32(); } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── track_record_deref ───────────────────────────────────────────────────
    // For TrackNodeFnPtrGet14/44 and TrackNodeDispatch14/44.
    // Problem: all four dereference DAT_0063d7e4, which is NULL at quiescent
    // main-menu state. Strategy: allocate a fake 0x48-byte record in scratch
    // memory, write sentinel values at the field offsets each function reads
    // (+0x14 and/or +0x44), set DAT_0063d7e4 = fake_record_ptr, call fn,
    // read back the return value (for getters) or compare observable state.
    // Restore DAT_0063d7e4 to original (NULL) after each test pair.
    //
    // Tests shape: flat list of sentinel values to write at field_offset.
    //
    // CONFIG fields:
    //   field_offset      int (hex) — byte offset within record (0x14 or 0x44)
    //   is_getter         bool      — if true, compare return value; if false (dispatcher),
    //                                  use crash_equal_ok (both sides deref through fn-ptr)
    //   record_global_str string    — hex addr of the global pointer (default '0x0063d7e4')
    // MECHANISM: Zero-arg fn; harness allocates a 0x48-byte fake record (zeroed), writes a u32
    // sentinel at CONFIG.field_offset, patches CONFIG.record_global_str (default 0x0063d7e4) to
    // point to it, calls fn() with no args, fingerprints return value (CONFIG.is_getter=true) or
    // crash-equality (false), then restores the global; generalises to any no-arg fn that
    // dereferences a single globally-NULL struct pointer provided the record fits in 0x48 bytes.
    if (CONFIG.arg_type === 'track_record_deref') {
        const RECORD_SIZE   = 0x48;
        const RECGLOBAL     = ptr(CONFIG.record_global_str || '0x0063d7e4');
        const fieldOff2     = (CONFIG.field_offset | 0);
        const isGetter      = (CONFIG.is_getter !== false);  // default true
        // Allocate fake record buffer; zero it entirely.
        const fakeRec = Memory.alloc(RECORD_SIZE);
        for (let k = 0; k < RECORD_SIZE; k++) fakeRec.add(k).writeU8(0);
        // Save and override the global pointer.
        let savedRecPtr = null;
        try { savedRecPtr = RECGLOBAL.readU32(); } catch (e) {}
        RECGLOBAL.writeU32(parseInt(fakeRec.toString(), 16));

        for (let i = 0; i < CONFIG.tests.length; i++) {
            const sentinelU32 = (CONFIG.tests[i] >>> 0);
            let origV = null, reimV = null, errO = null, errR = null;
            // Write sentinel at the field offset both paths will read.
            fakeRec.add(fieldOff2).writeU32(sentinelU32);
            // ── orig call ──
            try {
                const ret = Orig();
                origV = isGetter ? (ret >>> 0) : 1;  // getter: compare return value; dispatcher: compare 1 (no-crash)
            } catch (e) { errO = e.message; origV = isGetter ? null : 0; }
            // Reset sentinel so reimpl sees same state.
            fakeRec.add(fieldOff2).writeU32(sentinelU32);
            // ── reimpl call ──
            try {
                const ret = Reimpl();
                reimV = isGetter ? (ret >>> 0) : 1;
            } catch (e) { errR = e.message; reimV = isGetter ? null : 0; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: sentinelU32,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        // Restore the global pointer.
        try { if (savedRecPtr !== null) RECGLOBAL.writeU32(savedRecPtr); } catch (_) {}
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_sub_struct_zero ────────────────────────────────────────────────
    // For AudioRwsSubZeroInit-style: void(pointer param_1).
    // Function zeroes a sub-region of the struct pointed to by param_1.
    // Strategy: Memory.alloc(struct_size) a pair of buffers (one per path).
    // Fill each with sentinel pattern 0xAA. Call fn(buf). Read back the bytes
    // at [observe_offset..observe_offset+observe_length) and compare as a
    // position-sensitive fingerprint.
    //
    // CONFIG fields:
    //   struct_size      int — total allocation size in bytes
    //   observe_offset   int — byte offset within struct to start comparison
    //   observe_length   int — number of bytes to compare
    // ── audio_sub_struct_link ───────────────────────────────────────────────
    // For AudioSubStructLinkDevice (0x005ae010) and AudioSubStructLinkBuffer
    // (0x005adfe0): uint32* fn(uint32* param_1, uint32 param_2). With a zeroed
    // 12-byte scratch buf, the cleanup callee (FUN_005ae080 / FUN_005ae050) is
    // a no-op, and the function writes param_1[0]=p2 (Device) or param_1[1]=p2
    // (Buffer) and clears a bit in param_1[2]. Per-side allocated scratch buf;
    // fingerprint the 12 bytes plus return-pointer-non-null flag.
    // Prior arg_type 'audio_sub_struct_link' did not exist in this file and
    // fell through to default fn(input), passing a bare uint32 as the pointer
    // arg — both sides AV. Tests: flat list of param_2 values.
    // MECHANISM: fn(ptr, uint32); per-side 12-byte zeroed scratch buffers (separate pointers);
    // tests[i] = flat uint32 p2; fingerprint = (return-ptr-non-null<<24)|(low-24-bit
    // bufFingerprint of all 12 buffer bytes); compares the combined fingerprint across sides -
    // broader than struct-link: fits any fn(ptr, uint32) that writes to a <=12-byte pointer-arg
    // struct and returns a pointer, as long as zeroing the buffer suppresses any cleanup-callee
    // side-effects; 12-byte size is hardcoded.
    if (CONFIG.arg_type === 'audio_sub_struct_link') {
        const BUF_BYTES = 12;
        const sBufA = Memory.alloc(BUF_BYTES);
        const sBufB = Memory.alloc(BUF_BYTES);
        function zero(b) {
            for (let k = 0; k < BUF_BYTES; k++) b.add(k).writeU8(0);
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const p2 = (CONFIG.tests[i] | 0) >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            zero(sBufA);
            try {
                const ret = Orig(sBufA, p2);
                const retNn = (ret && !ret.isNull()) ? 1 : 0;
                const fp = bufFingerprint(sBufA, BUF_BYTES);
                origV = (retNn << 24) | (fp & 0x00ffffff);
            } catch (e) { errO = e.message; }
            zero(sBufB);
            try {
                const ret = Reimpl(sBufB, p2);
                const retNn = (ret && !ret.isNull()) ? 1 : 0;
                const fp = bufFingerprint(sBufB, BUF_BYTES);
                reimV = (retNn << 24) | (fp & 0x00ffffff);
            } catch (e) { errR = e.message; }
            results.push({ idx: i, input: p2,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── audio_sub_struct_dual ───────────────────────────────────────────────
    // For AudioSubStructDualInit (0x005ac7b0): uint32 fn(uint32 param_1,
    // uint32 param_2, uint32 param_3). param_1 is the buffer address passed
    // as uint32; the function casts internally. Calls LinkDevice(p1,p2) then
    // LinkBuffer(p1,p3); returns p1 on success, 0 on failure. With a zeroed
    // 12-byte scratch buf both cleanups are no-ops and both link calls write
    // to the buffer. Tests: list of [p2, p3] pairs.
    // MECHANISM: fn(uint32 p1, uint32 p2, uint32 p3) where p1 is the integer representation of a
    // harness-allocated 12-byte zeroed scratch buffer (callee casts internally); per-side separate
    // buffers allocated at different addresses and both passed as distinct p1 values; tests[i] =
    // {p2, p3} object; fingerprint = (ret-non-null-flag<<24)|(low-24-bit bufFingerprint of 12
    // bytes); broader than the name: fits any 3-uint32-arg fn whose first arg is a buffer address
    // passed as a plain integer (uint32-cast pointer pattern) and which writes to that buffer.
    if (CONFIG.arg_type === 'audio_sub_struct_dual') {
        const BUF_BYTES = 12;
        const sBufA = Memory.alloc(BUF_BYTES);
        const sBufB = Memory.alloc(BUF_BYTES);
        function zero(b) {
            for (let k = 0; k < BUF_BYTES; k++) b.add(k).writeU8(0);
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t  = CONFIG.tests[i];
            const p2 = (t.p2 | 0) >>> 0;
            const p3 = (t.p3 | 0) >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            zero(sBufA);
            try {
                const ret    = Orig(sBufA.toInt32() >>> 0, p2, p3);
                const retNn  = ((ret >>> 0) !== 0) ? 1 : 0;
                const fp     = bufFingerprint(sBufA, BUF_BYTES);
                origV = (retNn << 24) | (fp & 0x00ffffff);
            } catch (e) { errO = e.message; }
            zero(sBufB);
            try {
                const ret    = Reimpl(sBufB.toInt32() >>> 0, p2, p3);
                const retNn  = ((ret >>> 0) !== 0) ? 1 : 0;
                const fp     = bufFingerprint(sBufB, BUF_BYTES);
                reimV = (retNn << 24) | (fp & 0x00ffffff);
            } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── buf_field_set ───────────────────────────────────────────────────────
    // For AudioBufFieldSet (0x005baf60): void(int param_1, int param_2).
    // Writes param_2 to *(p1+0x74), ORs 0x100 into *(p1+0x78). If bit 3 of
    // *(p1+0x78) is set, also writes param_2 to *(*(p1+0x11c)+0x38). With a
    // zeroed CONFIG.buf_size buffer (default 0x120), bit 3 is clear and the
    // COM branch is never taken; the function exercises only the two field
    // writes. Tests: flat list of param_2 values.
    // Required CONFIG: buf_size, field_offsets (default [0x74, 0x78]).
    // MECHANISM: Per-side zero-filled buffer of CONFIG.buf_size (default 0x120); calls fn(buf,
    // param_2_u32); reads dwords at CONFIG.field_offsets[0] and [1] (default [0x74, 0x78]), folds
    // each to 16-bit XOR of its two halves, packs as a uint32 fingerprint; NARROW: only 2 offsets
    // observed - other buffer writes are invisible; broader: parameterized offsets make it usable
    // for different struct layouts; CONFIG: buf_size, field_offsets.
    if (CONFIG.arg_type === 'buf_field_set') {
        const BUF_BYTES = (CONFIG.buf_size | 0) || 0x120;
        const offsets   = CONFIG.field_offsets || [0x74, 0x78];
        const bufA = Memory.alloc(BUF_BYTES);
        const bufB = Memory.alloc(BUF_BYTES);
        function zero(b) {
            for (let k = 0; k < BUF_BYTES; k++) b.add(k).writeU8(0);
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const p2 = (CONFIG.tests[i] | 0) >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            zero(bufA);
            try {
                Orig(bufA, p2);
                // Pack two field reads into a single uint32: low 16 of +0x74 XOR low 16 of +0x78<<16
                const v74 = bufA.add(offsets[0]).readU32();
                const v78 = bufA.add(offsets[1]).readU32();
                origV = ((v74 & 0xffff) ^ ((v74 >>> 16) & 0xffff)) | (((v78 & 0xffff) ^ ((v78 >>> 16) & 0xffff)) << 16);
                origV = origV >>> 0;
            } catch (e) { errO = e.message; }
            zero(bufB);
            try {
                Reimpl(bufB, p2);
                const v74 = bufB.add(offsets[0]).readU32();
                const v78 = bufB.add(offsets[1]).readU32();
                reimV = ((v74 & 0xffff) ^ ((v74 >>> 16) & 0xffff)) | (((v78 & 0xffff) ^ ((v78 >>> 16) & 0xffff)) << 16);
                reimV = reimV >>> 0;
            } catch (e) { errR = e.message; }
            results.push({ idx: i, input: p2,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── semaphore_create ────────────────────────────────────────────────────
    // For AudioSemaphoreCreate (0x005aeea0): uint(void*, LONG, LONG).
    // Calls CreateSemaphoreA(NULL, initial, max, NULL); stores HANDLE at *p1;
    // returns -(handle!=0) & p1 (i.e. p1 if handle non-null else 0).
    // Tests: [initial, max] pairs. Both sides allocate per-side scratch, call
    // fn(buf, init, max), close the resulting handle, and observe:
    //   bit0 = (ret-non-null) ; bit1 = (handle stored at *buf was non-null).
    // Per-side buf addrs differ — ret-pointer-identity is meaningless across
    // sides; but ret-non-null and handle-validity must match.
    // MECHANISM: Calls fn(out_ptr, initial_int, max_int) with a per-side 4-byte scratch as
    // out_ptr; observable is a 2-bit mask (bit0=ret-non-null, bit1=handle-at-*out_ptr-non-null);
    // actual pointer values discarded since buffers differ per side; resulting handle is
    // CloseHandled immediately. No CONFIG beyond arg_type. FALSE-GREEN hazard: observes
    // success/fail only, not handle value identity.
    if (CONFIG.arg_type === 'semaphore_create') {
        const sBufA = Memory.alloc(4);
        const sBufB = Memory.alloc(4);
        // CloseHandle from kernel32 — release the semaphores we leak otherwise.
        // Use instance method (modern Frida API) — Module.findExportByName is
        // not available as a static across versions.
        let CloseHandle = null;
        try {
            const k32Mod = Module.load('kernel32.dll');
            const ch = k32Mod.findExportByName('CloseHandle');
            if (ch && !ch.isNull()) {
                CloseHandle = new NativeFunction(ch, 'int32', ['pointer'], 'stdcall');
            }
        } catch (_) { /* leak semaphores if kernel32 unreachable */ }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const init = (t[0] | 0);
            const max  = (t[1] | 0);
            let origV = null, reimV = null, errO = null, errR = null;
            sBufA.writeU32(0);
            try {
                const ret  = Orig(sBufA, init, max);
                const h    = sBufA.readU32();
                if (CloseHandle && h !== 0) CloseHandle(ptr(h));
                origV = ((((ret >>> 0) !== 0) ? 1 : 0)) | (((h !== 0) ? 1 : 0) << 1);
            } catch (e) { errO = e.message; }
            sBufB.writeU32(0);
            try {
                const ret  = Reimpl(sBufB, init, max);
                const h    = sBufB.readU32();
                if (CloseHandle && h !== 0) CloseHandle(ptr(h));
                reimV = ((((ret >>> 0) !== 0) ? 1 : 0)) | (((h !== 0) ? 1 : 0) << 1);
            } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── music_vol_set ───────────────────────────────────────────────────────
    // For MusicGroupVolumeSet (0x005baf00): void(int* p1, float vol).
    // Writes vol to *(p1+0x38); walks circular linked list at p1+0xc setting
    // bit 6 on each node's +0x14; if *(p1+0x11c) != 0 writes vol's raw bits
    // to *(*(p1+0x11c)+0x30). With a zeroed CONFIG.buf_size buf (default
    // 0x120) and a sentinel self-loop (write p1+0xc to itself), the loop
    // runs zero iterations and the secondary branch is skipped. Tests: flat
    // list of float volume values. Observable: low-24 fingerprint of buf
    // packed with (sentinel-still-self-loop ? 1 : 0).
    // MECHANISM: Allocates a zeroed `CONFIG.buf_size`-byte (default 0x120) struct, sets buf+0x0c
    // as a self-loop (empty circular list); calls fn(buf_ptr, float) void(ptr,float); fingerprint
    // = (buf[+0x38]&0xffffff)|(sentinel_intact<<24). List-walk and secondary-pointer (buf+0x11c=0)
    // paths are both skipped by the zeroed+self-loop setup. CONFIG: `buf_size`, `tests[]`
    // (floats). NARROW: only the +0x38 direct-write path is exercised.
    if (CONFIG.arg_type === 'music_vol_set') {
        const BUF_BYTES = (CONFIG.buf_size | 0) || 0x120;
        const bufA = Memory.alloc(BUF_BYTES);
        const bufB = Memory.alloc(BUF_BYTES);
        function setupEmpty(b) {
            for (let k = 0; k < BUF_BYTES; k++) b.add(k).writeU8(0);
            // Sentinel self-loop: *(b+0xc) = b+0xc  (head of empty circular list).
            b.add(0x0c).writePointer(b.add(0x0c));
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const vol = +CONFIG.tests[i];
            let origV = null, reimV = null, errO = null, errR = null;
            setupEmpty(bufA);
            try {
                Orig(bufA, vol);
                const v38 = bufA.add(0x38).readU32();      // vol raw bits at +0x38
                const sentinel = bufA.add(0x0c).readPointer().equals(bufA.add(0x0c)) ? 1 : 0;
                origV = ((v38 & 0xffffff) | (sentinel << 24)) >>> 0;
            } catch (e) { errO = e.message; }
            setupEmpty(bufB);
            try {
                Reimpl(bufB, vol);
                const v38 = bufB.add(0x38).readU32();
                const sentinel = bufB.add(0x0c).readPointer().equals(bufB.add(0x0c)) ? 1 : 0;
                reimV = ((v38 & 0xffffff) | (sentinel << 24)) >>> 0;
            } catch (e) { errR = e.message; }
            results.push({ idx: i, input: vol,
                           original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // MECHANISM: fn(ptr); per-side scratch buffer of CONFIG.struct_size bytes (default 24) filled
    // with sentinel 0xAA before each call; test values are ignored (iteration markers only);
    // observes rolling XOR/multiply hash of bytes [CONFIG.observe_offset, +CONFIG.observe_length)
    // (both default to the full buffer); return value is NOT observed; crash_equal_ok supported;
    // no comment above dispatch - mechanism purely from body; broader than the name: fits any
    // fn(ptr) that mutates a configurable-size buffer with no return value observable, with
    // configurable observation window.
    if (CONFIG.arg_type === 'audio_sub_struct_zero') {
        const sSize   = (CONFIG.struct_size   | 0) || 24;
        const obsOff  = (CONFIG.observe_offset | 0) || 0;
        const obsLen  = (CONFIG.observe_length | 0) || sSize;
        const sBufA   = Memory.alloc(sSize);
        const sBufB   = Memory.alloc(sSize);
        function fillSentinel(b) {
            for (let k = 0; k < sSize; k++) b.add(k).writeU8(0xAA);
        }
        function fpRange(b, off, len) {
            let fp = 0;
            for (let k = 0; k < len; k++) {
                fp = ((fp * 31) ^ b.add(off + k).readU8()) >>> 0;
            }
            return fp;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            // tests: each entry is ignored (we just call fn with fresh sentinel each time)
            let origV = null, reimV = null, errO = null, errR = null;
            fillSentinel(sBufA);
            try { Orig(sBufA); origV = fpRange(sBufA, obsOff, obsLen); } catch (e) { errO = e.message; }
            fillSentinel(sBufB);
            try { Reimpl(sBufB); reimV = fpRange(sBufB, obsOff, obsLen); } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: i,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── allocator_nonnull ────────────────────────────────────────────────────
    // For functions that allocate heap memory and return a pointer.
    // Pointer-identity comparison is meaningless (addresses differ each call).
    // Observable: both sides agree on null vs non-null — that is the correctness
    // signal (allocation succeeded or failed identically).
    //
    // Strategy: call fn() [no args], check (ret !== null && !ret.isNull()).
    //   orig nonnull + reimpl nonnull → GREEN (both 1).
    //   orig null    + reimpl null    → GREEN (both 0).
    //   mismatch                      → RED.
    //
    // CONFIG fields: none beyond standard.
    // Tests: flat list (length = call count; values ignored).
    // Unblocks: 0x004c5890 RwTexDictionaryCreate (demoted in frida-sweep-q).
    // MECHANISM: Calls fn() with zero arguments and no seeds; observes only null vs non-null of
    // the return pointer (1=non-null, 0=null) - a reimpl returning any non-null pointer passes
    // even if size or contents are wrong; CONFIG: none beyond standard; tests[i] values are
    // ignored (list length = call count).
    if (CONFIG.arg_type === 'allocator_nonnull') {
        for (let i = 0; i < CONFIG.tests.length; i++) {
            let origV = null, reimV = null, errO = null, errR = null;
            try {
                const p = Orig();
                origV = (p && !p.isNull()) ? 1 : 0;
            } catch (e) { errO = e.message; }
            try {
                const p = Reimpl();
                reimV = (p && !p.isNull()) ? 1 : 0;
            } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: i,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── resource_loader_4arg ─────────────────────────────────────────────────
    // For Win32ResourceLoader-style: fn(uint16 nameId, LPCSTR type, uint8** outBuf, uint32* outLen).
    // Calls the Win32 FindResourceA/LoadResource/SizeofResource/LockResource chain.
    // Both outBuf and outLen are written on success (returns 1) or untouched on failure (returns 0).
    //
    // Strategy: allocate 4-byte outBuf slot and 4-byte outLen slot.
    //   Call orig(nameId, typeName, pOutBuf, pOutLen).
    //   Call reimpl(nameId, typeName, pOutBuf, pOutLen).
    //   Observable: return value (0 or 1) packed with nullness of *outBuf.
    //   Encoding: (ret & 1) | (((*pOutBuf == 0) ? 0 : 1) << 1).
    //   Both sides must agree on success/failure and whether a buffer was returned.
    //   The actual resource bytes are from the same MASHED.exe module, so the
    //   pointer returned by LockResource will be identical (same module = same addr).
    //
    // Tests: list of { name_id: uint16, type_str: string } objects.
    // type_str is embedded as a NUL-terminated UTF-8 string in scratch memory.
    // CONFIG fields: none beyond standard.
    // Unblocks: 0x004997b0 Win32ResourceLoader.
    // MECHANISM: fn(uint16 nameId, char* typeName, uint8** pOutBuf, uint32* pOutLen)->int; harness
    // allocates NUL-terminated string for typeName and two 4-byte out-pointer slots (zeroed before
    // each call), observable is (ret&1)|(bufNonNull<<1) - success/failure bit and whether *pOutBuf
    // is non-null; actual resource bytes are NOT compared because both sides call into the same
    // MASHED.exe module and LockResource returns the same address; CONFIG: tests[] of
    // {name_id:uint16,type_str:string}, no other keys.
    if (CONFIG.arg_type === 'resource_loader_4arg') {
        const outBufSlot = Memory.alloc(4);   // uint8** outBuf (4-byte slot holding the pointer)
        const outLenSlot = Memory.alloc(4);   // uint32* outLen
        // Pre-allocate type-string buffers (one per test, up to 64 chars).
        const typeStrBufs = CONFIG.tests.map(function(t) {
            const s = t.type_str || '';
            const b = Memory.alloc(s.length + 1);
            for (let k = 0; k < s.length; k++) b.add(k).writeU8(s.charCodeAt(k) & 0xff);
            b.add(s.length).writeU8(0);
            return b;
        });
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const nameId = (t.name_id >>> 0);
            const typeBuf = typeStrBufs[i];
            let origV = null, reimV = null, errO = null, errR = null;
            // Reset output slots before each call.
            outBufSlot.writeU32(0);
            outLenSlot.writeU32(0);
            try {
                const ret = Orig(nameId, typeBuf, outBufSlot, outLenSlot);
                const retU = (ret === null || ret === undefined) ? 0 : (ret >>> 0);
                const bufNonNull = (outBufSlot.readU32() !== 0) ? 1 : 0;
                origV = (retU & 1) | (bufNonNull << 1);
            } catch (e) { errO = e.message; }
            // Reset for reimpl.
            outBufSlot.writeU32(0);
            outLenSlot.writeU32(0);
            try {
                const ret = Reimpl(nameId, typeBuf, outBufSlot, outLenSlot);
                const retU = (ret === null || ret === undefined) ? 0 : (ret >>> 0);
                const bufNonNull = (outBufSlot.readU32() !== 0) ? 1 : 0;
                reimV = (retU & 1) | (bufNonNull << 1);
            } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── struct_three_write ───────────────────────────────────────────────────
    // For leaf 3-field-write functions: void(ptr param_1, uint32 param_2, uint32 param_3).
    // Writes param_2 → param_1+0x14, param_3 → param_1+0x10, 0 → param_1+0x0c.
    //
    // Strategy: allocate a scratch buffer of struct_size bytes (default 32).
    //   Pre-fill with sentinel 0xDEADBEEF. Call fn(buf, val_a, val_b).
    //   Read back bytes at observe_offsets (default [0x0c, 0x10, 0x14]) as uint32s.
    //   Compare as comma-separated fingerprint. Both paths must agree.
    //   Restores nothing (fresh scratch each call pair).
    //
    // CONFIG fields:
    //   struct_size      int (default 32)
    //   observe_offsets  array of byte offsets to read back (default [12, 16, 20])
    // Tests: list of [val_a, val_b] pairs.
    // Unblocks: 0x005be140 FUN_005be140.
    // MECHANISM: fn(harness_ptr, uint32, uint32); per-side harness-allocated scratch of
    // CONFIG.struct_size bytes (default 32) sentinel-filled with 0xDEADBEEF before each call;
    // calls fn(buf, tests[i][0], tests[i][1]); fingerprints uint32s at CONFIG.observe_offsets
    // (default [0x0c,0x10,0x14]) as a comma-joined string; return value is NOT observed;
    // observe_offsets is fully configurable so the handler is not restricted to the default layout
    // - any fn(ptr,u32,u32) that writes to configurable offsets in its first arg fits.
    if (CONFIG.arg_type === 'struct_three_write') {
        const stSize   = (CONFIG.struct_size | 0) || 32;
        const stOffs   = CONFIG.observe_offsets || [0x0c, 0x10, 0x14];
        const stBufA   = Memory.alloc(stSize);
        const stBufB   = Memory.alloc(stSize);
        function fillSentinelSt(b) {
            for (let k = 0; k + 3 < stSize; k += 4) b.add(k).writeU32(0xDEADBEEF);
        }
        function fpSt(b) {
            return stOffs.map(function(off) {
                return b.add(off).readU32() >>> 0;
            }).join(',');
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t    = CONFIG.tests[i];
            const valA = t[0] >>> 0;
            const valB = t[1] >>> 0;
            let origV = null, reimV = null, errO = null, errR = null;
            fillSentinelSt(stBufA);
            try { Orig(stBufA, valA, valB); origV = fpSt(stBufA); } catch (e) { errO = e.message; }
            fillSentinelSt(stBufB);
            try { Reimpl(stBufB, valA, valB); reimV = fpSt(stBufB); } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── slot_quad_set ────────────────────────────────────────────────────────
    // For slot-indexed 4-dword writers: void(int param_1, uint32* param_2).
    // Function writes param_2[0..3] to four consecutive globals at
    //   DAT_006412e8 + param_1 * 0xf40 + {0, 4, 8, 12}.
    //
    // Strategy: allocate a 16-byte array, fill with test values, call fn(idx, arr).
    //   Read back the 4 dwords from the live globals (DAT_006412e8 + idx*0xf40).
    //   Save originals before; restore after each test pair so both sides start clean.
    //   Compare as comma-separated fingerprint.
    //
    // CONFIG fields:
    //   slot_base_addr   string (hex, default '0x006412e8') — base of the global array.
    //   slot_stride      int (default 0xf40 = 3904) — per-index stride in bytes.
    //   slot_field_count int (default 4) — number of dwords to read back.
    // Tests: list of { idx: int, vals: [v0, v1, v2, v3] } objects.
    // Unblocks: 0x00422ac0 FUN_00422ac0.
    // MECHANISM: Calls void fn(int idx, uint32* arr); arr is a scratch buffer loaded with
    // CONFIG.slot_field_count uint32 test values; observes those dwords read back from live
    // globals at CONFIG.slot_base_addr + idx*CONFIG.slot_stride; saves and restores live globals
    // around each orig/reimpl pair. CONFIG: slot_base_addr (default 0x006412e8), slot_stride
    // (default 0xf40), slot_field_count (default 4).
    if (CONFIG.arg_type === 'slot_quad_set') {
        const sqBase   = ptr(CONFIG.slot_base_addr || '0x006412e8');
        const sqStride = (CONFIG.slot_stride | 0) || 0xf40;
        const sqCount  = (CONFIG.slot_field_count | 0) || 4;
        const sqArrBuf = Memory.alloc(sqCount * 4);
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t   = CONFIG.tests[i];
            const idx = (t.idx | 0);
            const vals = t.vals;  // array of sqCount uint32 values
            // Compute effective write base.
            const effective = sqBase.add(idx * sqStride);
            // Save live globals.
            const saved = [];
            for (let k = 0; k < sqCount; k++) saved.push(effective.add(k * 4).readU32());
            let origV = null, reimV = null, errO = null, errR = null;
            // ── orig call ──
            // Write test values into scratch array.
            for (let k = 0; k < sqCount; k++) sqArrBuf.add(k * 4).writeU32((vals[k] >>> 0));
            try {
                Orig(idx, sqArrBuf);
                origV = [];
                for (let k = 0; k < sqCount; k++) origV.push(effective.add(k * 4).readU32() >>> 0);
                origV = origV.join(',');
            } catch (e) { errO = e.message; }
            // Restore so reimpl sees same starting state.
            for (let k = 0; k < sqCount; k++) effective.add(k * 4).writeU32(saved[k]);
            // ── reimpl call ──
            for (let k = 0; k < sqCount; k++) sqArrBuf.add(k * 4).writeU32((vals[k] >>> 0));
            try {
                Reimpl(idx, sqArrBuf);
                reimV = [];
                for (let k = 0; k < sqCount; k++) reimV.push(effective.add(k * 4).readU32() >>> 0);
                reimV = reimV.join(',');
            } catch (e) { errR = e.message; }
            // Restore live globals.
            for (let k = 0; k < sqCount; k++) effective.add(k * 4).writeU32(saved[k]);
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: JSON.stringify(t),
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── teardown_call_pair ───────────────────────────────────────────────────
    // For teardown/shutdown thunks: zero a known engine-state global before
    // EACH invocation (both orig AND reimpl) so both sides see the same
    // already-torn-down state. This makes idx=0 crash symmetrically instead
    // of orig succeeding (fresh state) while reimpl crashes (state torn down by
    // orig's prior call).
    //
    // Strategy: before every call (orig and reimpl), write 0 to the global at
    // CONFIG.state_global_str. After each call, the function may crash — that
    // is expected and handled by crash_equal_ok=True on the registry entry.
    // The original value of the global is saved once before the loop and
    // restored after all tests (best-effort; process may crash anyway).
    //
    // CONFIG fields:
    //   state_global_str  string (hex addr) — address of the engine-state pointer
    //                     to NULL before each call pair. Default '0x007d3ff8'
    //                     (RW engine vtable base, confirmed as the crash target).
    // Tests: flat list (length = call count; values ignored).
    // Unblocks: engine_stop_dispatch (0x00493550), hw_exit_dispatch (0x00493560),
    //           engine_stop_helper (0x004938c0).
    // MECHANISM: Zero-arg fn(); before EVERY call (both Orig and Reimpl) writes 0 to
    // CONFIG.state_global_str (default 0x007d3ff8) so both sides start from the same already-torn-
    // down state; saves the global once before the loop and restores best-effort after; observes
    // return value (normalised to uint32) or matched crash message via crash_equal_ok; NARROW:
    // only one configurable global is corrupted, no buffer setup - fits only no-arg
    // teardown/shutdown thunks where the sole pre-condition is one specific global = 0.
    if (CONFIG.arg_type === 'teardown_call_pair') {
        const tdGlobalAddr = ptr(CONFIG.state_global_str || '0x007d3ff8');
        // Save the global's current value for best-effort restore.
        let tdSavedVal = 0;
        try { tdSavedVal = tdGlobalAddr.readU32(); } catch (_) {}

        for (let i = 0; i < CONFIG.tests.length; i++) {
            let origV = null, reimV = null, errO = null, errR = null;
            // Pre-corrupt engine state before orig call.
            try { tdGlobalAddr.writeU32(0); } catch (_) {}
            try {
                const ret = Orig();
                origV = (ret === undefined || ret === null) ? 0
                      : (typeof ret === 'object') ? (parseInt(ret.toString(), 16) >>> 0)
                      : (ret >>> 0);
            } catch (e) { errO = e.message; }
            // Pre-corrupt engine state before reimpl call (state may already be 0).
            try { tdGlobalAddr.writeU32(0); } catch (_) {}
            try {
                const ret = Reimpl();
                reimV = (ret === undefined || ret === null) ? 0
                      : (typeof ret === 'object') ? (parseInt(ret.toString(), 16) >>> 0)
                      : (ret >>> 0);
            } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            results.push({ idx: i, input: i,
                           original: origV, reimpl: reimV,
                           match: crashEqual || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        // Best-effort restore.
        try { tdGlobalAddr.writeU32(tdSavedVal); } catch (_) {}
        send({ type: 'results', data: results });
        return;
    }

    // ── large_buffer_save_restore ────────────────────────────────────────────
    // For functions that zero-fill a large live-state buffer (hundreds of KB).
    // Without save/restore, the buffer is permanently zeroed after the first
    // call (orig), so reimpl runs against a different pre-call state.
    //
    // Strategy: snapshot the buffer into JS-side Uint8Array before EACH call
    // pair; restore from snapshot between orig and reimpl, and restore again
    // after reimpl. Both sides see the same pre-call buffer state.
    // The function is called with zero args (arg_type='none' semantics).
    // Observable: both sides should produce the same crash or same void return.
    //
    // CONFIG fields:
    //   buffer_addr          string (hex addr) — base address of the buffer.
    //   buffer_size_dwords   int  — number of 4-byte dwords in the buffer.
    //                               Total bytes = buffer_size_dwords * 4.
    // Tests: flat list (length = call count; values ignored).
    // Unblocks (harness side): data_zero_fill (0x004924f0) — NOTE: C3 promotion
    //   still blocked by anti-island rule (5 of 6 callees at C1). This arg_type
    //   is infrastructure for when callees are promoted.
    // MECHANISM: fn() no args: snapshots CONFIG.buffer_addr (CONFIG.buffer_size_dwords*4 bytes)
    // once, restores before each Orig and Reimpl call so both sides see identical pre-call buffer
    // state; observable = return value; void return always matches if signature.ret='void';
    // matched crashes pass when CONFIG.crash_equal_ok; CONFIG: buffer_addr, buffer_size_dwords,
    // crash_equal_ok; broader: any zero-arg fn mutating a large global buffer.
    if (CONFIG.arg_type === 'large_buffer_save_restore') {
        const lbAddr  = ptr(CONFIG.buffer_addr);
        const lbDwords = (CONFIG.buffer_size_dwords | 0);
        const lbBytes  = lbDwords * 4;

        // Snapshot the buffer contents (read lbBytes bytes into JS ArrayBuffer).
        // Use NativePointer.readByteArray(n) — consistent with codebase convention.
        let lbSnapshot = null;
        try {
            lbSnapshot = lbAddr.readByteArray(lbBytes);
        } catch (e) {
            send({ type: 'error', msg: 'large_buffer_save_restore: failed to snapshot buffer: ' + e.message });
            return;
        }
        function lbRestore() {
            try { lbAddr.writeByteArray(lbSnapshot); } catch (_) {}
        }

        for (let i = 0; i < CONFIG.tests.length; i++) {
            let origV = null, reimV = null, errO = null, errR = null;
            // Restore before orig call so orig sees the original buffer state.
            lbRestore();
            try {
                const ret = Orig();
                origV = (ret === undefined || ret === null) ? 0
                      : (typeof ret === 'object') ? (parseInt(ret.toString(), 16) >>> 0)
                      : (ret >>> 0);
            } catch (e) { errO = e.message; }
            // Restore before reimpl call so reimpl sees the same pre-call state.
            lbRestore();
            try {
                const ret = Reimpl();
                reimV = (ret === undefined || ret === null) ? 0
                      : (typeof ret === 'object') ? (parseInt(ret.toString(), 16) >>> 0)
                      : (ret >>> 0);
            } catch (e) { errR = e.message; }
            const crashEqual = CONFIG.crash_equal_ok && errO !== null && errR !== null && errO === errR;
            const voidMatch  = (CONFIG.signature.ret === 'void') && errO === null && errR === null;
            results.push({ idx: i, input: i,
                           original: origV, reimpl: reimV,
                           match: crashEqual || voidMatch || (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        // Final restore.
        lbRestore();
        send({ type: 'results', data: results });
        return;
    }

    // ── seed_field_read_field (c3_batch_ag harness-ext 2026-06-08) ───────────
    // void fn(structPtr). Seed one field with a per-vector value, call, read a
    // (possibly different) field back. Proves a field-copy / derive path ran.
    // CONFIG: { struct_size, seed_off, read_off, read_size }.  Tests: list of
    //   seed values (uint32). Observable = the read_off bytes after the call.
    // Unlocks: 0x00483a30 Replay_Rewind (seed_off=0x18, read_off=0x1c, 4 bytes:
    //   copies *(p+0x18)->*(p+0x1c)).
    // MECHANISM: Calls fn(structPtr) with a harness-zeroed CONFIG.struct_size scratch buffer;
    // seeds uint32 at CONFIG.seed_off before the call, reads CONFIG.read_size bytes at
    // CONFIG.read_off afterward as the observable. Both sides get identically-seeded separate
    // buffers; no globals touched. Drives any fn(struct_ptr) that derives one field from another
    // within the same struct.
    if (CONFIG.arg_type === 'seed_field_read_field') {
        const SFSZ   = (CONFIG.struct_size | 0) || 0x40;
        const seedOff = CONFIG.seed_off | 0;
        const readOff = CONFIG.read_off | 0;
        const readSz  = (CONFIG.read_size | 0) || 4;
        const sfBufA  = Memory.alloc(SFSZ);
        const sfBufB  = Memory.alloc(SFSZ);
        function sfRead(b) {
            let v = '';
            for (let j = 0; j < readSz; j++) v += ('0' + b.add(readOff + j).readU8().toString(16)).slice(-2);
            return v;
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const sv = (CONFIG.tests[i] >>> 0);
            let origV = null, reimV = null, errO = null, errR = null;
            for (let z = 0; z < SFSZ; z++) sfBufA.add(z).writeU8(0);
            sfBufA.add(seedOff).writeU32(sv);
            try { Orig(sfBufA); origV = sfRead(sfBufA); } catch (e) { errO = e.message; }
            for (let z = 0; z < SFSZ; z++) sfBufB.add(z).writeU8(0);
            sfBufB.add(seedOff).writeU32(sv);
            try { Reimpl(sfBufB); reimV = sfRead(sfBufB); } catch (e) { errR = e.message; }
            results.push({ idx: i, input: sv, original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── structptr_seeded_array (c3_batch_ag harness-ext 2026-06-08) ──────────
    // int fn(structPtr, arrayPtr). Zero the struct (so a gate field reads 0 and a
    // C2 callee path is skipped), seed an input array, call, read back N struct
    // fields. CONFIG: { struct_size, array_vals_len (#u32 in array), read_offs[] }.
    // Tests: list of arrays of u32 (the seeded array values, raw bits — note the
    //   callee may re-read them as float). Observable = concat of read_offs bytes.
    // Unlocks: 0x004c1c80 ViewportDimsSet (struct_size>=0x78, gate@+0x04 stays 0,
    //   array = 2 u32 dims, read_offs=[0x68,0x6c,0x70,0x74]).
    // MECHANISM: Calls fn(structPtr, arrayPtr); struct is harness-zeroed to CONFIG.struct_size
    // (keeps internal gate fields at 0, suppressing conditional branches); array holds
    // CONFIG.array_vals_len uint32 test values written raw (callee may reinterpret as float);
    // observable is comma-joined hex of CONFIG.read_offs[] dwords read from the struct after the
    // call. CONFIG: struct_size, array_vals_len, read_offs[].
    if (CONFIG.arg_type === 'structptr_seeded_array') {
        const SASZ   = (CONFIG.struct_size | 0) || 0x80;
        const readOffs = CONFIG.read_offs || [];
        const saStructA = Memory.alloc(SASZ);
        const saStructB = Memory.alloc(SASZ);
        function saRead(b) {
            return readOffs.map(function(off) {
                return ('00000000' + (b.add(off | 0).readU32() >>> 0).toString(16)).slice(-8);
            }).join(',');
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const vals = CONFIG.tests[i] || [];
            const arr = Memory.alloc(Math.max(4, vals.length * 4));
            for (let k = 0; k < vals.length; k++) arr.add(k * 4).writeU32(vals[k] >>> 0);
            let origV = null, reimV = null, errO = null, errR = null;
            for (let z = 0; z < SASZ; z++) saStructA.add(z).writeU8(0);
            try { Orig(saStructA, arr); origV = saRead(saStructA); } catch (e) { errO = e.message; }
            for (let z = 0; z < SASZ; z++) saStructB.add(z).writeU8(0);
            try { Reimpl(saStructB, arr); reimV = saRead(saStructB); } catch (e) { errR = e.message; }
            results.push({ idx: i, input: JSON.stringify(vals), original: origV, reimpl: reimV,
                           match: (origV !== null && reimV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── scalars_to_scattered_globals (c3_batch_ag harness-ext 2026-06-08) ────
    // void|int fn(s1[,s2[,s3]]). Writes a set of NON-contiguous globals/arrays
    // (multi_arg_global_write only handles ONE contiguous block + a guard).
    // Save the observed windows, call Orig, snapshot, restore; call Reimpl,
    // snapshot, restore — so both sides see an identical baseline (works even for
    // functions that mutate shared global state).
    // CONFIG: { observe:[{addr,len}], idx_call_str?, idx_arrays:[{base,stride,elem_len}],
    //   fold_ret? (XOR the return value into the fingerprint) }.
    // Tests: list of { args:[...] } (or a raw array of args).
    // Unlocks: 0x00417450/0x00417530 sparse-grid (fixed globals + 1/0 return),
    //   0x004299d0 TimeRecord (3 scalar globals + 3 track arrays at idx_call=0x430790).
    // MECHANISM: Calls fn(t.args... as uint32 scalars); saves/restores arbitrary non-contiguous
    // byte-windows (`CONFIG.observe`:{addr,len}) and stride-indexed array slots
    // (`CONFIG.idx_arrays`:{base,stride,elem_len}) under a live index cursor
    // (`CONFIG.idx_call_str`); fingerprints with FNV-1a; optionally pre-fills windows, runs a prep
    // call, or folds the return value. CONFIG: `observe`, `idx_arrays`, `idx_call_str`,
    // `fold_ret`, `prep_call_str`, `pre_fill_byte`.
    if (CONFIG.arg_type === 'scalars_to_scattered_globals') {
        const observe   = CONFIG.observe || [];
        const idxArrays = CONFIG.idx_arrays || [];
        const foldRet   = CONFIG.fold_ret ? true : false;
        const idxCall   = CONFIG.idx_call_str
            ? new NativeFunction(ptr(CONFIG.idx_call_str), 'int32', [], 'mscdecl') : null;
        // Optional prep: run the ORIGINAL of some helper (e.g. the grid writer) to
        // populate state BEFORE calling the target, so a clear/erase target has
        // something to act on. Same prep both sides => isolates the target's diff.
        const prepFn = CONFIG.prep_call_str
            ? new NativeFunction(ptr(CONFIG.prep_call_str), 'void',
                  (CONFIG.prep_arg_types || ['uint32', 'uint32', 'uint32']), 'mscdecl') : null;
        // Optional pre-fill: set every observed window to a byte BEFORE each call
        // (inside the save/restore bracket, so it is reverted). Used when the
        // function only acts on a sentinel-initialised region — e.g. the sparse
        // grid treats 0xff as "free", but the region is 0x00 at diff-attach, so
        // without this the writer finds no free slot and is a constant no-op.
        const preFillByte = (CONFIG.pre_fill_byte !== undefined && CONFIG.pre_fill_byte !== null)
            ? (CONFIG.pre_fill_byte & 0xff) : null;
        function curIdx() { return idxCall ? (idxCall() | 0) : 0; }
        function windows() {
            const ws = observe.map(function(w) { return { a: ptr(w.addr), len: w.len | 0, fill: w.fill }; });
            const idx = curIdx();
            idxArrays.forEach(function(ar) {
                ws.push({ a: ptr(ar.base).add(idx * (ar.stride | 0)), len: ar.elem_len | 0 });
            });
            return ws;
        }
        const hasFill = (preFillByte !== null) || observe.some(function(w) { return w.fill !== undefined && w.fill !== null; });
        // FNV-1a (non-cancelling) — a position-XOR fold would cancel values that
        // the function writes to two observed windows (e.g. TimeRecord writes the
        // same min/sec/frac to both the scalar globals AND the track arrays).
        function snapFp(ret) {
            let fp = 0x811c9dc5 | 0;
            windows().forEach(function(w) {
                const ba = new Uint8Array(w.a.readByteArray(w.len));   // bulk read (fast)
                for (let j = 0; j < ba.length; j++) { fp = Math.imul(fp ^ ba[j], 0x01000193); }
            });
            if (foldRet && ret !== null && ret !== undefined) {
                const r = ret >>> 0;
                for (let b = 0; b < 4; b++) fp = Math.imul(fp ^ ((r >>> (b * 8)) & 0xff), 0x01000193);
            }
            return fp >>> 0;
        }
        function saveW() { return windows().map(function(w) { return w.a.readByteArray(w.len); }); }
        function restoreW(s) { windows().forEach(function(w, ix) { w.a.writeByteArray(s[ix]); }); }
        function fillW(byte) {
            windows().forEach(function(w) {
                const fb = (w.fill !== undefined && w.fill !== null) ? (w.fill & 0xff) : byte;
                if (fb === null) return;   // idx_arrays / un-filled windows are left as-is
                const b = new Uint8Array(w.len); b.fill(fb); w.a.writeByteArray(b.buffer);
            });
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            const args = (t && t.args) ? t.args : (Array.isArray(t) ? t : [t]);
            const a = args.map(function(v) { return v >>> 0; });
            const prepArgs = (t && t.prep_args) ? t.prep_args.map(function(v) { return v >>> 0; }) : null;
            let origV = null, reimV = null, errO = null, errR = null;
            const saved = saveW();
            try { if (hasFill) fillW(preFillByte); if (prepFn && prepArgs) prepFn.apply(null, prepArgs); const r = Orig.apply(null, a); origV = snapFp(r); } catch (e) { errO = e.message; }
            restoreW(saved);
            try { if (hasFill) fillW(preFillByte); if (prepFn && prepArgs) prepFn.apply(null, prepArgs); const r = Reimpl.apply(null, a); reimV = snapFp(r); } catch (e) { errR = e.message; }
            restoreW(saved);
            results.push({ idx: i, input: JSON.stringify(args), original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── count_header_list_ring (c3_batch_ag harness-ext 2026-06-08) ──────────
    // Intrusive list with a COUNT-HEADER (NOT the bare sentinel that
    // audio_list_insert/find_index assume). Header: count@[0], embedded
    // sentinel@header+4 (sentinel.next@+0). Object: link node@object+node_link_off
    // (object = node - node_link_off), compared field@object+cmp_field_off.
    // The structure is BUILT with the ORIGINAL Init+PushBack (init_rva_str /
    // pushback_rva_str), then the target op (Orig vs Reimpl) is exercised.
    // CONFIG: { list_op:'init'|'pushback'|'find'|'at', node_link_off, cmp_field_off,
    //   object_size, init_rva_str, pushback_rva_str }.
    // Observables are ADDRESS-NORMALIZED (count + cmp-field value / found field /
    // -1), never raw pointers, so per-side allocations compare cleanly.
    // Unlocks: 0x005b3580 Init, 0x005b35a0 PushBack, 0x005b3670 Find, 0x005b36b0 At.
    // MECHANISM: Builds an intrusive count-header ring list with the ORIGINAL Init+PushBack
    // primitives (init_rva_str / pushback_rva_str as NativeFunctions), then exercises
    // CONFIG.list_op ('init'|'pushback'|'find'|'at') on Orig vs Reimpl; observables are address-
    // normalised (count u32, cmp-field s32 values) so per-side allocations compare cleanly;
    // CONFIG: node_link_off, cmp_field_off, object_size; applies to any intrusive ring list
    // sharing this header shape (count@0, sentinel@+4 self-loop).
    if (CONFIG.arg_type === 'count_header_list_ring') {
        const LINK  = (CONFIG.node_link_off | 0) || 0x20;
        const CMP   = (CONFIG.cmp_field_off | 0) || 0x18;
        const OBJSZ = (CONFIG.object_size | 0) || 0x40;
        const HDRSZ = 0x10;
        const op    = CONFIG.list_op;
        const InitFn = CONFIG.init_rva_str
            ? new NativeFunction(ptr(CONFIG.init_rva_str), 'void', ['pointer'], 'mscdecl') : null;
        const PushFn = CONFIG.pushback_rva_str
            ? new NativeFunction(ptr(CONFIG.pushback_rva_str), 'void', ['pointer', 'pointer'], 'mscdecl') : null;
        // keepAlive: Memory.alloc buffers referenced only by raw pointers inside the
        // list nodes (target memory) get GC'd by Frida between the Orig and Reimpl
        // calls — the second call then walks freed memory (AV / garbage). Retain
        // every allocation here for the whole handler run.
        const _keep = [];
        // Build a populated list using the ORIGINAL primitives.
        function buildList(fieldVals) {
            const hdr = Memory.alloc(HDRSZ); _keep.push(hdr);
            for (let z = 0; z < HDRSZ; z++) hdr.add(z).writeU8(0);
            InitFn(hdr);
            for (let k = 0; k < fieldVals.length; k++) {
                const o = Memory.alloc(OBJSZ); _keep.push(o);
                for (let z = 0; z < OBJSZ; z++) o.add(z).writeU8(0);
                o.add(CMP).writeS32(fieldVals[k] | 0);
                PushFn(hdr, o);
            }
            return hdr;
        }
        // Read the cmp field of the object owning the first node (sentinel.next).
        function firstField(hdr) {
            const node = hdr.add(4).readPointer();           // header[1] = sentinel.next
            if (node.isNull() || node.equals(hdr.add(4))) return null;  // empty (self-loop)
            return node.sub(LINK).add(CMP).readS32();
        }
        function retField(ret) {
            if (!ret || ret.isNull()) return -1;
            return ret.add(CMP).readS32();                    // ret = object base
        }
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            let origV = null, reimV = null, errO = null, errR = null;
            if (op === 'init') {
                // Prefill 3 header words with a per-vector sentinel; call Init; read
                // normalized empty-state (count, self-loop next, self-loop tail).
                const sv = (t >>> 0) || 0xDEADBEEF;
                const hA = Memory.alloc(HDRSZ), hB = Memory.alloc(HDRSZ);
                const h0 = function (h, s) { h.writeU32(s); h.add(4).writeU32(s); h.add(8).writeU32(s); h.add(12).writeU32(0); };
                const initObs = function (h) {
                    const count = h.readU32() >>> 0;
                    const selfN = h.add(4).readPointer().equals(h.add(4)) ? 1 : 0;
                    const selfT = h.add(8).readPointer().equals(h.add(4)) ? 1 : 0;
                    return count + ':' + selfN + selfT;
                };
                h0(hA, sv); try { Orig(hA);  origV = initObs(hA); } catch (e) { errO = e.message; }
                h0(hB, sv); try { Reimpl(hB); reimV = initObs(hB); } catch (e) { errR = e.message; }
            } else if (op === 'pushback') {
                const pre = t.pre || [];
                const pf  = t.push_field | 0;
                const hA = buildList(pre), hB = buildList(pre);
                const oA = Memory.alloc(OBJSZ), oB = Memory.alloc(OBJSZ);
                for (let z = 0; z < OBJSZ; z++) { oA.add(z).writeU8(0); oB.add(z).writeU8(0); }
                oA.add(CMP).writeS32(pf); oB.add(CMP).writeS32(pf);
                try { Orig(hA, oA);   origV = (hA.readU32() >>> 0) + ':' + firstField(hA); } catch (e) { errO = e.message; }
                try { Reimpl(hB, oB); reimV = (hB.readU32() >>> 0) + ':' + firstField(hB); } catch (e) { errR = e.message; }
            } else if (op === 'find') {
                const hdr = buildList(t.field_vals || []);
                const key = t.key | 0;
                try { origV = retField(Orig(hdr, key)); }   catch (e) { errO = e.message; }
                try { reimV = retField(Reimpl(hdr, key)); } catch (e) { errR = e.message; }
            } else if (op === 'at') {
                const hdr = buildList(t.field_vals || []);
                const pos = t.pos | 0;
                try { origV = retField(Orig(hdr, pos)); }   catch (e) { errO = e.message; }
                try { reimV = retField(Reimpl(hdr, pos)); } catch (e) { errR = e.message; }
            }
            results.push({ idx: i, input: JSON.stringify(t), original: origV, reimpl: reimV,
                           match: (errO === null && errR === null && origV !== null && origV === reimV),
                           err_original: errO, err_reimpl: errR });
        }
        send({ type: 'results', data: results });
        return;
    }

    // ── standard scalar / vec3 / read_global / none loop ────────────────────
    try {
        for (let i = 0; i < CONFIG.tests.length; i++) {
            const t = CONFIG.tests[i];
            let orig, reim, errOrig = null, errReim = null;

            // void_write_observe — void(void) functions that write to globals.
            // Strategy: write sentinel `t` to target_global, call fn (void),
            // then read target_global back as the observable. The observed
            // read-back is compared between orig and reimpl.
            // This detects whether both functions write the same value to the
            // same address; the sentinel also confirms the function actually
            // touches that address (if it doesn't, the sentinel survives).
            // MECHANISM: Per-test writes sentinel `t` to `CONFIG.target_global`, optionally seeds
            // `CONFIG.seed_globals` (array of {addr,val}) for both sides, calls fn with no args or fixed
            // `CONFIG.call_args`, reads `target_global` back as uint32; no save/restore (idempotent
            // sentinel pre-write); `crash_equal_ok` counts identical crashes as pass. CONFIG:
            // `target_global`, `seed_globals`, `call_args`, `crash_equal_ok`.
            if (CONFIG.arg_type === 'void_write_observe') {
                const gaddr = ptr(CONFIG.target_global);
                let origRead = null, reimRead = null;
                // Optional CONFIG.seed_globals: array of {addr:'0x..', val:<u32>}
                // written before EACH call so a one-shot guard (e.g.
                // DAT_0067eca4==0) or an accumulator slot is reset to a known
                // state for both Orig and Reimpl — without it a guarded fn runs
                // its body on the first call only and the second side no-ops,
                // producing a spurious RED. Backward-compatible: absent => skip.
                const seedGlobals = CONFIG.seed_globals || [];
                const seedAll = function () {
                    for (let s = 0; s < seedGlobals.length; s++) {
                        ptr(seedGlobals[s].addr).writeU32(seedGlobals[s].val >>> 0);
                    }
                };
                // Optional CONFIG.call_args: fixed integer args passed to both
                // sides so the write address is deterministic for functions that
                // index by param_1 (else param_1 is uncontrolled stack garbage).
                // signature.args must match the arg count. Absent => call with
                // no args (the historical void(void) behavior).
                const callArgs = CONFIG.call_args || null;
                const invoke = function (fn) {
                    if (callArgs) { return fn.apply(null, callArgs); }
                    return fn();
                };
                try {
                    seedAll();
                    gaddr.writeU32(t >>> 0);
                    invoke(Orig);
                    origRead = gaddr.readU32();
                } catch (e) { errOrig = e.message; }
                try {
                    seedAll();
                    gaddr.writeU32(t >>> 0);
                    invoke(Reimpl);
                    reimRead = gaddr.readU32();
                } catch (e) { errReim = e.message; }
                // crash_equal_ok: if both sides throw the same error string, count as match
                // (e.g. functions with implicit EAX pointer that crash identically in
                // NativeFunction context where EAX isn't controllable).
                const crashEqualVWO = CONFIG.crash_equal_ok && errOrig !== null && errReim !== null && errOrig === errReim;
                results.push({ idx: i, input: t,
                               original: origRead, reimpl: reimRead,
                               match: crashEqualVWO || (origRead !== null && reimRead !== null && origRead === reimRead),
                               err_original: errOrig, err_reimpl: errReim });
                continue;
            }

            // For 'read_global' we write the sentinel BEFORE each call so the
            // global has the right value when the function reads it; the
            // write is repeated for both orig and reim in case orig somehow
            // mutated the field.
            try { orig = callFn(Orig,   t, buf); } catch (e) { orig = null; errOrig = e.message; }
            try { reim = callFn(Reimpl, t, buf); } catch (e) { reim = null; errReim = e.message; }
            const origN = (orig !== null && orig !== undefined) ?
                (typeof orig === 'object' ? orig.toString() : orig) : null;
            const reimN = (reim !== null && reim !== undefined) ?
                (typeof reim === 'object' ? reim.toString() : reim) : null;
            // crash_equal_ok: if both sides throw the same error string, count as match.
            const crashEqual = CONFIG.crash_equal_ok && errOrig !== null && errReim !== null && errOrig === errReim;
            // void_match: for void-return functions (origN===null, reimN===null, no errors), count as match.
            const voidMatch = (CONFIG.signature.ret === 'void') && errOrig === null && errReim === null && origN === null && reimN === null;
            results.push({ idx: i, input: t, original: origN, reimpl: reimN,
                           match: crashEqual || voidMatch || (origN !== null && reimN !== null && origN === reimN),
                           err_original: errOrig, err_reimpl: errReim });
        }
    } finally {
        if (savedGlobal !== null) {
            try { ptr(CONFIG.target_global).writeU32(savedGlobal); }
            catch (e) { /* best effort — process is about to exit anyway */ }
        }
    }
    send({ type: 'results', data: results });
}

pollLutThenRun(150);
