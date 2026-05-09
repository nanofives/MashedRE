// Frida agent: A/B-diff Vec3Magnitude (FUN_004c3ac0) against our reimpl.
//
// Strategy:
//  1. Wait for RW3 fast-sqrt LUT globals at 0x007d3ff8 / 0x007d3ffc to populate
//     (RwEngineStart fills them — happens during the splash sequence).
//  2. Module.load() our mashed_re_dev.asi (env var MASHED_RE_NO_AUTO_HOOK=1
//     suppresses InjectHooks(), so the original at 0x004c3ac0 stays intact).
//  3. For each test vector, call BOTH the unpatched original AND our exported
//     Vec3Magnitude — both reading the same live LUT in MASHED's process.
//  4. send({...}) the results back to the Python harness for diffing.

'use strict';

const ASI_PATH        = 'C:\\Users\\maria\\Desktop\\Proyectos\\Mashed\\mashedmod\\build\\mashed_re_dev.asi';
const LUT_BASE_ADDR   = ptr('0x007d3ff8');  // RW globals base pointer
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');  // offset to LUT root inside RW globals
const ORIGINAL_RVA    = ptr('0x004c3ac0');  // FUN_004c3ac0

function readLutRoot() {
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
    const lut = readLutRoot();
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
        send({ type: 'error', msg: 'LUT globals never populated within 10s — RwEngineStart likely never reached.' });
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
    const reimplAddr = module.findExportByName('Vec3Magnitude');
    if (reimplAddr === null) {
        send({ type: 'error', msg: 'Vec3Magnitude export not found in .asi' });
        return;
    }
    send({ type: 'asi_loaded',
           base: '0x' + module.base.toString(16),
           reimpl_addr: '0x' + reimplAddr.toString(16) });

    const Vec3MagOriginal = new NativeFunction(ORIGINAL_RVA, 'float', ['pointer'], 'mscdecl');
    const Vec3MagReimpl   = new NativeFunction(reimplAddr,   'float', ['pointer'], 'mscdecl');

    const tests = [
        [0.0,     0.0,     0.0    ],   // zero
        [1.0,     0.0,     0.0    ],   // x-unit
        [0.0,     1.0,     0.0    ],   // y-unit
        [0.0,     0.0,     1.0    ],   // z-unit
        [3.0,     4.0,     0.0    ],   // 3-4-5
        [-3.0,    4.0,     0.0    ],   // negative x
        [3.0,    -4.0,     0.0    ],   // negative y
        [1.0,     1.0,     1.0    ],   // unit cube diagonal
        [-1.0,   -1.0,    -1.0    ],   // negative cube diagonal
        [2.0,     3.0,     6.0    ],   // 2-3-6-7
        [10.0,    0.0,     0.0    ],   // length 10
        [100.0,   100.0,   100.0  ],   // larger
        [0.001,   0.001,   0.001  ],   // tiny
        [12.34,  -56.78,   90.12  ],   // arbitrary
        [1.5,     2.5,     3.5    ],   // mixed
        [-99.99,  99.99,  -99.99  ],   // large mixed signs
        [1e-6,    1e-6,    1e-6   ],   // very tiny
        [1234.5,  6789.0, -4321.0 ],   // large
    ];

    const buf = Memory.alloc(12);
    const results = [];
    for (let i = 0; i < tests.length; i++) {
        const t = tests[i];
        buf.writeFloat(t[0]);
        buf.add(4).writeFloat(t[1]);
        buf.add(8).writeFloat(t[2]);
        let orig, reim, errOrig = null, errReim = null;
        try { orig = Vec3MagOriginal(buf); } catch (e) { orig = null; errOrig = e.message; }
        try { reim = Vec3MagReimpl(buf);   } catch (e) { reim = null; errReim = e.message; }
        results.push({ idx: i, input: t, original: orig, reimpl: reim,
                       match: (orig !== null && reim !== null && orig === reim),
                       err_original: errOrig, err_reimpl: errReim });
    }
    send({ type: 'results', data: results });
}

// Kick off polling. RW init usually finishes within 1–2s after spawn-resume.
pollLutThenRun(50);
