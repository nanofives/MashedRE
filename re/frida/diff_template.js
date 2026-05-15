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
//   lut_root_delta  number  (0 or 4 — offset for LUT readiness poll)
//   tests           array whose element shape depends on arg_type
'use strict';

const CONFIG = $CONFIG$;
const ASI_PATH        = CONFIG.asi_path;
const TARGET_ADDR     = ptr(CONFIG.target_rva);
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

// contact_history buffers — allocated on demand in runDiff
let vehicleBuf = null;
let geomBuf    = null;
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
    // int_out24 — fn(int32_idx, out_ptr); 24-byte output buffer; returns fingerprint of 6 dwords.
    // Used for SlotDataCopy (0x0041f000): copies 6 dwords from live game state to *param_2.
    if (CONFIG.arg_type === 'int_out24') {
        for (var j = 0; j < 6; j++) buf.add(j * 4).writeU32(0);
        fn(input | 0, buf);
        return [
            buf.readU32(),        buf.add(4).readU32(),
            buf.add(8).readU32(), buf.add(12).readU32(),
            buf.add(16).readU32(),buf.add(20).readU32(),
        ].join(',');
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
        Module.load(ASI_PATH);
        module = Process.findModuleByName('mashed_re_dev.asi');
        if (module === null) {
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
              : (CONFIG.arg_type === 'sentinel_array_ptr') ? Memory.alloc(256)
              : (CONFIG.arg_type === 'int_out24') ? Memory.alloc(24) : null;

    // contact_history: allocate fake vehicle struct (0xC80 bytes) and geom entry (0x40 bytes).
    if (CONFIG.arg_type === 'contact_history') {
        vehicleBuf = Memory.alloc(0xC80);
        geomBuf    = Memory.alloc(0x40);
    }

    // For 'read_global' and 'write_global_call_int0', save and restore the target global so
    // we don't leave a sentinel in game-state memory after the test.
    let savedGlobal = null;
    if (CONFIG.arg_type === 'read_global' || CONFIG.arg_type === 'write_global_call_int0') {
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
    const results = [];


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

    // ── font_ctx_float2 ──────────────────────────────────────────────────────
    // For FontCtx_SetScale / FontCtx_SetTranslation: fn(float sx, float sy) → uint32.
    // Strategy: for each test {sx, sy}, write sentinel 0xDEADBEEF to dirty-flag
    // DAT_00912bd8, call fn(sx,sy), read back dirty flag (must be 0 if fn worked).
    // Also read back the uint32 return value (must be 1).
    // Observable = (ret << 16) | dirtyFlagReadback. Both paths must match.
    // Dirty flag is restored to its pre-test value after each pair.
    if (CONFIG.arg_type === 'font_ctx_float2') {
        const pDirty = ptr('0x00912bd8');
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
    if (CONFIG.arg_type === 'font_matrix_push') {
        const pDepth = ptr('0x00912b04');
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
                results.push({ idx: i, input: t,
                               original: origRead, reimpl: reimRead,
                               match: (origRead !== null && reimRead !== null && origRead === reimRead),
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
            results.push({ idx: i, input: t, original: origN, reimpl: reimN,
                           match: crashEqual || (origN !== null && reimN !== null && origN === reimN),
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
