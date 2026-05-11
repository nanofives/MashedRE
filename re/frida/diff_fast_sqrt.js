// Frida agent: A/B-diff FastSqrt (FUN_004c3b30) against our reimpl.
// Sibling of diff_vec3_magnitude.js / diff_fast_invsqrt.js.
//
// Target signature: `float __cdecl FUN_004c3b30(float)` — single float arg.
// LUT root at +0 inside RW globals (sqrt table, same as Vec3Magnitude uses).
// `biased >> 1` (no NOT) — sqrt exponent contribution.
'use strict';

const ASI_PATH        = 'C:\\Users\\maria\\Desktop\\Proyectos\\Mashed\\mashedmod\\build\\mashed_re_dev.asi';
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');
const ORIGINAL_RVA    = ptr('0x004c3b30');

function readSqrtLutRoot() {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return null;
        const off = LUT_OFFSET_ADDR.readU32();
        const lutPtr = ptr(base + off).readU32();
        if (lutPtr === 0) return null;
        return { base: base, offset: off, root: lutPtr };
    } catch (e) {
        return null;
    }
}

function pollLutThenRun(triesLeft) {
    const lut = readSqrtLutRoot();
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
        send({ type: 'error', msg: 'sqrt LUT root never populated within 10s' });
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
    const reimplAddr = module.findExportByName('FastSqrt');
    if (reimplAddr === null) {
        send({ type: 'error', msg: 'FastSqrt export not found in .asi' });
        return;
    }
    send({ type: 'asi_loaded',
           base: '0x' + module.base.toString(16),
           reimpl_addr: '0x' + reimplAddr.toString(16) });

    const FastSqrtOriginal = new NativeFunction(ORIGINAL_RVA, 'float', ['float'], 'mscdecl');
    const FastSqrtReimpl   = new NativeFunction(reimplAddr,   'float', ['float'], 'mscdecl');

    const tests = [
        0.0,         // zero early-out
        1.0,         // sqrt(1) = 1
        4.0,         //         = 2
        9.0,         //         = 3
        16.0,        //         = 4
        25.0,        //         = 5
        100.0,       //         = 10
        10000.0,     //         = 100
        0.25,        //         = 0.5
        0.5,         //         ~ 0.707
        2.0,         //         ~ 1.414
        3.0,         //         ~ 1.732
        1.0e6,       //         = 1000
        1.0e-6,      //         = 0.001
        6789.0,      //         ~ 82.4
        1234.5,      //         ~ 35.1
        12.34,       //         ~ 3.51
        81.0,        //         = 9
    ];

    const results = [];
    for (let i = 0; i < tests.length; i++) {
        const t = tests[i];
        let orig, reim, errOrig = null, errReim = null;
        try { orig = FastSqrtOriginal(t); } catch (e) { orig = null; errOrig = e.message; }
        try { reim = FastSqrtReimpl(t);   } catch (e) { reim = null; errReim = e.message; }
        results.push({ idx: i, input: t, original: orig, reimpl: reim,
                       match: (orig !== null && reim !== null && orig === reim),
                       err_original: errOrig, err_reimpl: errReim });
    }
    send({ type: 'results', data: results });
}

pollLutThenRun(50);
