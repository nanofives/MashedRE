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
'use strict';

const CONFIG = $CONFIG$;
const ASI_PATH        = CONFIG.asi_path;
const TARGET_ADDR     = ptr(CONFIG.target_rva);
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

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
    if (CONFIG.arg_type === 'vec3_ptr') {
        buf.writeFloat(input[0]);
        buf.add(4).writeFloat(input[1]);
        buf.add(8).writeFloat(input[2]);
        return fn(buf);
    }
    if (CONFIG.arg_type === 'void') {
        return fn();
    }
    if (CONFIG.arg_type === 'int_scalar') {
        return fn(input);
    }
    if (CONFIG.arg_type === 'int_pair') {
        return fn(input[0], input[1]);
    }
    if (CONFIG.arg_type === 'int_ptr2_out') {
        // allocate two uint32 out-slots in buf[0..7], call, pack result
        buf.writeU32(0);
        buf.add(4).writeU32(0);
        fn(input, buf, buf.add(4));
        return (buf.readU32() & 0x3f) | ((buf.add(4).readU32() & 0x3f) << 8);
    }
    // float_scalar (default)
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

    let bufBytes = 0;
    if (CONFIG.arg_type === 'vec3_ptr')     bufBytes = 12;
    else if (CONFIG.arg_type === 'int_ptr2_out') bufBytes = 8;
    const buf = bufBytes > 0 ? Memory.alloc(bufBytes) : null;
    const results = [];
    for (let i = 0; i < CONFIG.tests.length; i++) {
        const t = CONFIG.tests[i];
        let orig, reim, errOrig = null, errReim = null;
        try { orig = callFn(Orig,   t, buf); } catch (e) { orig = null; errOrig = e.message; }
        try { reim = callFn(Reimpl, t, buf); } catch (e) { reim = null; errReim = e.message; }
        // Normalise to string for equality: NativePointer objects are
        // object-identity-distinct even when holding the same address.
        const origN = (orig !== null && orig !== undefined) ?
            (typeof orig === 'object' ? orig.toString() : orig) : null;
        const reimN = (reim !== null && reim !== undefined) ?
            (typeof reim === 'object' ? reim.toString() : reim) : null;
        results.push({ idx: i, input: t, original: origN, reimpl: reimN,
                       match: (origN !== null && reimN !== null && origN === reimN),
                       err_original: errOrig, err_reimpl: errReim });
    }
    send({ type: 'results', data: results });
}

pollLutThenRun(150);
