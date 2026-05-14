// Generic A/B diff agent. Python harness injects a CONFIG block at $CONFIG$.
//
// CONFIG schema (JSON):
//   asi_path        string
//   target_rva      string ("0x004c3ac0")
//   export          string
//   signature       { ret: "float", args: ["float" | "pointer"] }
//   arg_type        "float_scalar" | "vec3_ptr"
//   lut_root_delta  number  (0 or 4 — offset for LUT readiness poll)
//   tests           array of either floats (scalar) or [x,y,z] tuples (vec3)
'use strict';

const CONFIG = $CONFIG$;
const ASI_PATH        = CONFIG.asi_path;
const TARGET_ADDR     = ptr(CONFIG.target_rva);
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

// contact_history buffers — allocated on demand in runDiff
let vehicleBuf = null;
let geomBuf    = null;

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
               base: '0x' + lut.base.toString(16),
               offset: '0x' + lut.offset.toString(16),
               root: '0x' + lut.root.toString(16),
               delta: CONFIG.lut_root_delta,
               attempts: 50 - triesLeft });
        runDiff();
        return;
    }
    if (triesLeft <= 0) {
        send({ type: 'error', msg: 'LUT root never populated within 10s (delta=' + CONFIG.lut_root_delta + ')' });
        return;
    }
    setTimeout(function () { pollLutThenRun(triesLeft - 1); }, 200);
}

function callFn(fn, input, buf) {
    if (CONFIG.arg_type === 'none') {
        // Zero-arg getter / void invocation. `input` is a dummy iteration
        // marker; we just call the function repeatedly to confirm stable
        // bit-identical output between original and reimpl.
        return fn();
    }
    if (CONFIG.arg_type === 'read_global') {
        // Drive the target global with a sentinel value `input`, then call
        // the zero-arg getter. Proves orig and reim read the SAME address
        // (otherwise the sentinel write only affects one). Caller must save
        // and restore the global outside this loop.
        ptr(CONFIG.target_global).writeU32(input >>> 0);
        return fn();
    }
    if (CONFIG.arg_type === 'vec3_ptr') {
        buf.writeFloat(input[0]);
        buf.add(4).writeFloat(input[1]);
        buf.add(8).writeFloat(input[2]);
        return fn(buf);
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
    // float_scalar
    return fn(input);
}

function runDiff() {
    let module;
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
           base: '0x' + module.base.toString(16),
           reimpl_addr: '0x' + reimplAddr.toString(16),
           export_name: CONFIG.export });

    const Orig   = new NativeFunction(TARGET_ADDR, CONFIG.signature.ret, CONFIG.signature.args, 'mscdecl');
    const Reimpl = new NativeFunction(reimplAddr,  CONFIG.signature.ret, CONFIG.signature.args, 'mscdecl');

    const buf = (['vec3_ptr', 'int_with_out_ptr', 'out3_idx', 'idx_out2'].includes(CONFIG.arg_type)) ? Memory.alloc(12) : null;

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

    const results = [];
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
            results.push({ idx: i, input: t, original: orig, reimpl: reim,
                           match: (orig !== null && reim !== null && orig === reim),
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
