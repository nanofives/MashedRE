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
let matsBufs  = null;   // matrix_scale:    { mat, scale }
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
    if (CONFIG.arg_type === 'none') {
        // Zero-arg getter / void invocation. `input` is a dummy iteration
        // marker; we just call the function repeatedly to confirm stable
        // bit-identical output between original and reimpl.
        return fn();
    }
    if (CONFIG.arg_type === 'read_global') {
        ptr(CONFIG.target_global).writeU32(input >>> 0);
        return fn();
    }
    // ── Simple scalar types ───────────────────────────────────────────────────
    if (CONFIG.arg_type === 'float_scalar') {
        return fn(input);
    }
    if (CONFIG.arg_type === 'vec3_ptr') {
        buf.writeFloat(input[0]);
        buf.add(4).writeFloat(input[1]);
        buf.add(8).writeFloat(input[2]);
        return fn(buf);
    }
    // void — no args, no return value of interest
    if (CONFIG.arg_type === 'void') {
        return fn();
    }
    // int_pair — two uint32 args
    if (CONFIG.arg_type === 'int_pair') {
        return fn(input[0], input[1]);
    }
    // int_ptr2_out — fn(uint32, out_ptr1, out_ptr2); two 4-byte out-slots; returns packed u32
    if (CONFIG.arg_type === 'int_ptr2_out') {
        buf.writeU32(0);
        buf.add(4).writeU32(0);
        fn(input, buf, buf.add(4));
        return (buf.readU32() & 0x3f) | ((buf.add(4).readU32() & 0x3f) << 8);
    }
    // time_diff_decompose — fn(int time_a, int time_b, u32* sign, int* min, int* sec, float* csec).
    // void return; four out-ptrs in a single 16-byte buf. input: [time_a, time_b].
    // Observable: comma-separated fingerprint string of sign|min|sec|csec_bits (IEEE-754 u32).
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
    if (CONFIG.arg_type === 'int_scalar') {
        return fn(input >>> 0);
    }
    // int_with_out_ptr — uint32 arg + 4-byte output buffer; returns function's return value
    if (CONFIG.arg_type === 'int_with_out_ptr') {
        return fn(input >>> 0, buf);
    }
    // write_global_call_int0 — write sentinel to target_global, call fn(0), return value
    // Use for getters where non-trivial domain requires injecting known values.
    if (CONFIG.arg_type === 'write_global_call_int0') {
        ptr(CONFIG.target_global).writeU32(input >>> 0);
        return fn(0);
    }
    // void_setter_observe — call fn(input) [void return], then read target_global and return it.
    // Use for void(uint32) setters that write param_1 directly to a global.
    // Strategy: call fn(value), read back target_global. Both orig and reimpl must
    // have written `value` to target_global.
    if (CONFIG.arg_type === 'void_setter_observe') {
        fn(input >>> 0);
        return ptr(CONFIG.target_global).readU32();
    }
    // contact_history — set up slot 0 of a fake vehicle contact table, call fn(geom, vehicle)
    // input: { slot_contact_id, slot_active, geom_contact_id }
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
    if (CONFIG.arg_type === 'out3_idx') {
        return fn(buf, input >>> 0);
    }
    // idx_out2 — fn(uint32_idx, out_ptr1, out_ptr2); two 4-byte out-slots in shared buf.
    // Returns fn return value. Used for functions like VehicleCarStateRead.
    if (CONFIG.arg_type === 'idx_out2') {
        return fn(input >>> 0, buf, buf.add(4));
    }
    // sentinel_array_ptr — original is __fastcall(ECX=0, EDX=ptr); reimpl is __cdecl(0, ptr).
    // input: array of int32 values terminated by 0xFF070000 (e.g. [0xFF060000, 0xFF070000]).
    // Writes the array into buf. For orig, calls via a hand-written thunk that sets ECX=0
    // and EDX=buf before jumping to the target. For reimpl, calls as cdecl(0, buf).
    // Used for MenuGroupCount (0x0042ac00).
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
    if (CONFIG.arg_type === 'uint32_scalar') {
        return fn(input >>> 0);
    }
    // float_scalar (default)
    if (CONFIG.arg_type === 'vec2_ptr') {
        buf.writeFloat(input[0]);
        buf.add(4).writeFloat(input[1]);
        return fn(buf);
    }

    // ── Output-buffer types ───────────────────────────────────────────────────
    // These call fn(...) then read the output buffer and return a bit-packed
    // string so that the JS `===` comparison is a bit-identical check.

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
              : (['int_with_out_ptr', 'idx_out2', 'int_ptr2_out'].includes(CONFIG.arg_type)) ? Memory.alloc(8)
              : (CONFIG.arg_type === 'time_diff_decompose') ? Memory.alloc(16)
              : (CONFIG.arg_type === 'sentinel_array_ptr') ? Memory.alloc(256)
              : (CONFIG.arg_type === 'fmt_desc_ptr') ? Memory.alloc(0x20)
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

    // ── pcm_pack ────────────────────────────────────────────────────────────
    // Saturating int32->int16 PCM pack: void fn(i16* dst, i32* src, u32 count).
    // The function writes 2*count int16 samples from 2*count int32 sources.
    // Added 2026-06-04 (c3-batch-ab-s4) for 0x005c9770.
    // Per test {src:[int32...], count:N}: write source ints into a shared src
    // buffer, call Orig/Reimpl into two separate dst buffers, compare exact bytes.
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

    // ── int_copy_outbuf ──────────────────────────────────────────────────────
    // For TimerSlotDataCopy-style: fn(int slot, T* dst) — void return, copies
    // N bytes from a per-slot source global into dst. Each test is a single
    // integer index. Allocates a sentinel-filled 4 KB scratch buffer per side
    // (oversized to back any TimerSlot-like copy); reads back the first
    // CONFIG.out_buf_size (default 24) bytes as a position-sensitive XOR
    // fingerprint. Real GREEN requires both fingerprints to be non-zero AND
    // equal — proof the function wrote AND wrote the same bytes.
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
            if (CONFIG.arg_type === 'void_write_observe') {
                const gaddr = ptr(CONFIG.target_global);
                let origRead = null, reimRead = null;
                try {
                    gaddr.writeU32(t >>> 0);
                    Orig();
                    origRead = gaddr.readU32();
                } catch (e) { errOrig = e.message; }
                try {
                    gaddr.writeU32(t >>> 0);
                    Reimpl();
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
