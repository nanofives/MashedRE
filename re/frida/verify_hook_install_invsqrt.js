// Frida agent: verify the inline-JMP hook installer end-to-end on FastInvSqrt.
// Sibling of verify_hook_install_vec3.js — same checks, different RVA + signature.
'use strict';

const ASI_PATH        = 'C:\\Users\\maria\\Desktop\\Proyectos\\Mashed\\mashedmod\\build\\mashed_re_dev.asi';
const TARGET_ADDR     = ptr('0x004c3b90');
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

function bytesHex(p, n) {
    const arr = [];
    for (let i = 0; i < n; i++) arr.push(p.add(i).readU8().toString(16).padStart(2, '0'));
    return arr.join(' ');
}

function readInvSqrtLutRoot() {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return null;
        const off = LUT_OFFSET_ADDR.readU32();
        const lutPtr = ptr(base + off + 4).readU32();
        if (lutPtr === 0) return null;
        return lutPtr;
    } catch (e) {
        return null;
    }
}

function pollLutThenRun(triesLeft) {
    if (readInvSqrtLutRoot() !== null) {
        runVerification();
        return;
    }
    if (triesLeft <= 0) {
        send({ type: 'error', msg: 'Inv-sqrt LUT root never populated within 10s' });
        return;
    }
    setTimeout(function () { pollLutThenRun(triesLeft - 1); }, 200);
}

function runVerification() {
    const preBytes = bytesHex(TARGET_ADDR, 8);
    send({ type: 'pre_snapshot', target: TARGET_ADDR.toString(), bytes: preBytes });

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
           module_base: module.base.toString(),
           reimpl_addr: reimplAddr.toString() });

    const postBytes = bytesHex(TARGET_ADDR, 8);
    const opcode = TARGET_ADDR.readU8();
    const rel32  = TARGET_ADDR.add(1).readS32();
    const expected_rel32 = reimplAddr.toInt32() - TARGET_ADDR.toInt32() - 5;
    send({ type: 'post_snapshot',
           bytes: postBytes,
           opcode_hex: '0x' + opcode.toString(16),
           rel32: rel32,
           rel32_hex: '0x' + (rel32 >>> 0).toString(16),
           expected_rel32: expected_rel32,
           expected_rel32_hex: '0x' + (expected_rel32 >>> 0).toString(16),
           opcode_ok: opcode === 0xE9,
           rel32_ok:  rel32 === expected_rel32,
           bytes_changed: preBytes !== postBytes });

    let reimplEntries = 0;
    Interceptor.attach(reimplAddr, {
        onEnter: function () { reimplEntries++; }
    });

    // Force-call the patched entry. Original is non-deterministic vs analytic
    // 1/sqrt (LUT approximation), so we don't compare to expected analytic
    // values — we just confirm the call doesn't crash and the interceptor
    // fires once per call. The diff harness (run_diff_invsqrt.py) already
    // proved bit-identity vs original.
    const FastInvSqrtPatched = new NativeFunction(TARGET_ADDR, 'float', ['float'], 'mscdecl');
    const inputs = [0.0, 1.0, 4.0, 0.25, 100.0];
    const results = [];
    const beforeCount = reimplEntries;
    for (let i = 0; i < inputs.length; i++) {
        let got, err = null;
        try { got = FastInvSqrtPatched(inputs[i]); } catch (e) { got = null; err = e.message; }
        results.push({ idx: i, input: inputs[i], got: got, err: err });
    }
    const afterCount = reimplEntries;

    send({ type: 'results',
           reimpl_calls_observed: afterCount - beforeCount,
           reimpl_calls_expected: inputs.length,
           cases: results });
}

pollLutThenRun(50);
