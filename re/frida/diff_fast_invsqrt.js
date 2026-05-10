// Frida agent: A/B-diff FastInvSqrt (FUN_004c3b90) against our reimpl.
//
// Same shape as diff_vec3_magnitude.js but for the inverse-sqrt sibling:
//  - target signature: `float __cdecl FUN_004c3b90(float)` — single float arg by
//    value, not a pointer.
//  - LUT root reads at +4 inside the RW globals (vs +0 for sqrt).
//  - exponent contribution computed from `~biased >> 1` (vs `biased >> 1`).
//  - hook is bypassed via MASHED_RE_NO_AUTO_HOOK so both impls run side-by-side
//    against the same live LUT.
'use strict';

const ASI_PATH        = 'C:\\Users\\maria\\Desktop\\Proyectos\\Mashed\\mashedmod\\build\\mashed_re_dev.asi';
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');
const ORIGINAL_RVA    = ptr('0x004c3b90');

function readInvSqrtLutRoot() {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return null;
        const off = LUT_OFFSET_ADDR.readU32();
        const lutPtr = ptr(base + off + 4).readU32();   // +4 — inv-sqrt table
        if (lutPtr === 0) return null;
        return { base: base, offset: off, root: lutPtr };
    } catch (e) {
        return null;
    }
}

function pollLutThenRun(triesLeft) {
    const lut = readInvSqrtLutRoot();
    if (lut !== null) {
        send({ type: 'lut_ready',
               base: '0x' + lut.base.toString(16),
               offset: '0x' + lut.offset.toString(16),
               root: '0x' + lut.root.toString(16),
               attempts: 50 - triesLeft });
        runDiff();
        return;
    }
    if (triesLeft <= 0) {
        send({ type: 'error', msg: 'Inv-sqrt LUT root never populated within 10s' });
        return;
    }
    setTimeout(function () { pollLutThenRun(triesLeft - 1); }, 200);
}

function runDiff() {
    let module;
    try {
        Module.load(ASI_PATH);
        module = Process.findModuleByName('mashed_re_dev.asi');
        if (module === null) {
            send({ type: 'error', msg: 'Module.load succeeded but findModuleByName returned null' });
            return;
        }
    } catch (e) {
        send({ type: 'error', msg: 'Module.load failed: ' + e.message });
        return;
    }
    const reimplAddr = module.findExportByName('FastInvSqrt');
    if (reimplAddr === null) {
        send({ type: 'error', msg: 'FastInvSqrt export not found in .asi' });
        return;
    }
    send({ type: 'asi_loaded',
           base: '0x' + module.base.toString(16),
           reimpl_addr: '0x' + reimplAddr.toString(16) });

    const FastInvSqrtOriginal = new NativeFunction(ORIGINAL_RVA, 'float', ['float'], 'mscdecl');
    const FastInvSqrtReimpl   = new NativeFunction(reimplAddr,   'float', ['float'], 'mscdecl');

    const tests = [
        0.0,            // zero early-out
        1.0,            // 1/sqrt(1) = 1
        4.0,            //          = 0.5
        16.0,           //          = 0.25
        0.25,           //          = 2
        0.5,            //          ~ 1.414
        2.0,            //          ~ 0.707
        9.0,            //          ~ 0.333
        100.0,          //          = 0.1
        81.0,           //          ~ 0.111
        1024.0,         //          ~ 0.03125
        1.0e6,          //          ~ 0.001
        1.0e-6,         //          ~ 1000
        0.001,          //          ~ 31.6
        1.5,            //          ~ 0.816
        12345.6,        //          ~ 0.009
        0.000123,       //          ~ 90.2
        7.7,            //          ~ 0.36
    ];

    const results = [];
    for (let i = 0; i < tests.length; i++) {
        const t = tests[i];
        let orig, reim, errOrig = null, errReim = null;
        try { orig = FastInvSqrtOriginal(t); } catch (e) { orig = null; errOrig = e.message; }
        try { reim = FastInvSqrtReimpl(t);   } catch (e) { reim = null; errReim = e.message; }
        results.push({ idx: i, input: t, original: orig, reimpl: reim,
                       match: (orig !== null && reim !== null && orig === reim),
                       err_original: errOrig, err_reimpl: errReim });
    }
    send({ type: 'results', data: results });
}

pollLutThenRun(50);
