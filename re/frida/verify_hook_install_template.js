// Generic hook-installer verification agent. CONFIG injected at $CONFIG$.
//
// Spawns with auto-hook ENABLED (no MASHED_RE_NO_AUTO_HOOK). Verifies that
// HookSystem::InstallAll() patches CONFIG.target_rva with E9 rel32 -> our
// exported reimpl, that the rel32 math is correct, and that calls to the
// patched RVA route through our reimpl (counted via Interceptor.attach).
'use strict';

const CONFIG = $CONFIG$;
const ASI_PATH        = CONFIG.asi_path;
const TARGET_ADDR     = ptr(CONFIG.target_rva);
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

function bytesHex(p, n) {
    const arr = [];
    for (let i = 0; i < n; i++) arr.push(p.add(i).readU8().toString(16).padStart(2, '0'));
    return arr.join(' ');
}

function readLutRoot(delta) {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return null;
        const off = LUT_OFFSET_ADDR.readU32();
        const lutPtr = ptr(base + off + delta).readU32();
        if (lutPtr === 0) return null;
        return lutPtr;
    } catch (e) {
        return null;
    }
}

function pollLutThenRun(triesLeft) {
    if (readLutRoot(CONFIG.lut_root_delta) !== null) {
        runVerification();
        return;
    }
    if (triesLeft <= 0) {
        send({ type: 'error', msg: 'LUT root never populated within 10s' });
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
    if (CONFIG.arg_type === 'int_with_out_ptr') {
        return fn(input >>> 0, buf);
    }
    if (CONFIG.arg_type === 'out3_idx') {
        return fn(buf, input >>> 0);
    }
    if (CONFIG.arg_type === 'idx_out2') {
        return fn(input >>> 0, buf, buf.add(4));
    }
    return fn(input);
}

function runVerification() {
    const preBytes = bytesHex(TARGET_ADDR, 8);
    send({ type: 'pre_snapshot', target: TARGET_ADDR.toString(), bytes: preBytes });

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
           module_base: module.base.toString(),
           reimpl_addr: reimplAddr.toString(),
           export_name: CONFIG.export });

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

    const Patched = new NativeFunction(TARGET_ADDR, CONFIG.signature.ret, CONFIG.signature.args, 'mscdecl');
    const buf = (['vec3_ptr', 'out3_idx'].includes(CONFIG.arg_type)) ? Memory.alloc(12)
              : (['int_with_out_ptr', 'idx_out2'].includes(CONFIG.arg_type)) ? Memory.alloc(8)
              : null;
    const results = [];
    const beforeCount = reimplEntries;
    for (let i = 0; i < CONFIG.tests.length; i++) {
        const t = CONFIG.tests[i];
        let got, err = null;
        try { got = callFn(Patched, t, buf); } catch (e) { got = null; err = e.message; }
        results.push({ idx: i, input: t, got: got, err: err });
    }
    const afterCount = reimplEntries;

    send({ type: 'results',
           reimpl_calls_observed: afterCount - beforeCount,
           reimpl_calls_expected: CONFIG.tests.length,
           cases: results });
}

pollLutThenRun(150);
