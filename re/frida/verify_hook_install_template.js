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
    // Registry entries for ptr_seed_observe / stub_dispatch_observe express their
    // test vectors as {'scalars': [...]} rather than a bare value or array. path2
    // had no case for that shape, so the dispatcher fell through to `fn(input)`
    // and every call died with "bad argument count" BEFORE the function was ever
    // entered — the same class of hole vec3_normalize had. Keyed off the TEST
    // SHAPE, not the arg_type, so it covers every handler that adopts the shape.
    // (orch-iter21.)
    if (input && typeof input === 'object' && Array.isArray(input.scalars)) {
        return fn.apply(null, input.scalars);
    }
    if (CONFIG.arg_type === 'none') {
        // Zero-arg invocation; `input` is a dummy iteration marker.
        return fn();
    }
    if (CONFIG.arg_type === 'vec3_ptr') {
        buf.writeFloat(input[0]);
        buf.add(4).writeFloat(input[1]);
        buf.add(8).writeFloat(input[2]);
        return fn(buf);
    }
    if (CONFIG.arg_type === 'vec3_normalize') {
        // (out*, in*) -> writes 3 floats to out, returns the scale 1/|in| in ST0.
        // Added 2026-07-28: path2 had no case for this arg_type, so the dispatcher fell
        // through to `fn(input)` and every call died with "bad argument count" BEFORE the
        // function was ever entered -- an installer check that verified the JMP bytes and
        // then tested nothing. Needs TWO 12-byte buffers; `buf` is allocated 24 for this
        // arg_type and split here (out = buf, in = buf+12) so out and in never alias.
        const outp = buf, inp = buf.add(12);
        inp.writeFloat(input[0]);
        inp.add(4).writeFloat(input[1]);
        inp.add(8).writeFloat(input[2]);
        outp.writeFloat(0); outp.add(4).writeFloat(0); outp.add(8).writeFloat(0);
        const scale = fn(outp, inp);
        // Report the written vector too -- a hook that returns the right scale while writing
        // nothing to `out` would otherwise pass.
        return [scale, outp.readFloat(), outp.add(4).readFloat(), outp.add(8).readFloat()]
               .join(',');
    }
    if (CONFIG.arg_type === 'void') {
        return fn();
    }
    if (CONFIG.arg_type === 'int_pair') {
        return fn(input[0], input[1]);
    }
    if (CONFIG.arg_type === 'int_ptr2_out') {
        buf.writeU32(0);
        buf.add(4).writeU32(0);
        fn(input, buf, buf.add(4));
        return (buf.readU32() & 0x3f) | ((buf.add(4).readU32() & 0x3f) << 8);
    }
    if (CONFIG.arg_type === 'time_diff_decompose') {
        const ta = input[0] | 0;
        const tb = input[1] | 0;
        buf.writeU32(0);
        buf.add(4).writeS32(0);
        buf.add(8).writeS32(0);
        buf.add(12).writeU32(0);
        fn(ta, tb, buf, buf.add(4), buf.add(8), buf.add(12));
        const sign = buf.readU32() >>> 0;
        const mn   = buf.add(4).readS32();
        const sc   = buf.add(8).readS32();
        const csec = buf.add(12).readU32() >>> 0;
        return [sign, mn, sc, '0x' + csec.toString(16)].join(',');
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

    // stub_at: neutralise callees that cannot survive a force-call. For
    // 0x00482900 the callee 0x004987b0 ends in OutputDebugStringA, whose debug-print
    // SEH exception Frida surfaces, so every force-call errored out before returning
    // — on BOTH sides, which is why it produced no verdict rather than a RED. The
    // A/B path already stubs it (registry 'stub_at'); path2 did not, so path2 could
    // never exercise these rows at all. Same install guard as reg_this_call_observe.
    (CONFIG.stub_at || []).forEach(function (a) {
        try {
            const p = ptr(a);
            Interceptor.replace(p, new NativeCallback(function () {
                return CONFIG.stub_ret | 0;
            }, 'int', new Array(CONFIG.stub_nargs || 3).fill('pointer'), 'mscdecl'));
            send({ type: 'log', msg: 'stubbed callee ' + p });
        } catch (e) {
            send({ type: 'error', msg: 'stub_at ' + a + ' failed: ' + e.message });
        }
    });

    let reimplEntries = 0;
    Interceptor.attach(reimplAddr, {
        onEnter: function () { reimplEntries++; }
    });

    const Patched = new NativeFunction(TARGET_ADDR, CONFIG.signature.ret, CONFIG.signature.args, 'mscdecl');
    const buf = (['vec3_ptr', 'out3_idx'].includes(CONFIG.arg_type)) ? Memory.alloc(12)
              : (['int_with_out_ptr', 'idx_out2', 'int_ptr2_out'].includes(CONFIG.arg_type)) ? Memory.alloc(8)
              : (CONFIG.arg_type === 'time_diff_decompose') ? Memory.alloc(16)
              : (CONFIG.arg_type === 'vec3_normalize') ? Memory.alloc(24)  // out[12] + in[12]
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
