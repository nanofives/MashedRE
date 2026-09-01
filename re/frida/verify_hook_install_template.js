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
    // A function with zero declared parameters must be called with zero args,
    // regardless of arg_type. The NativeFunction at TARGET_ADDR is built from
    // CONFIG.signature.args (below), so honor that same ground truth here. Without
    // this, self-emitting 0-arg handlers (e.g. allocator_nonnull) fell through to
    // `fn(input)` and died with "bad argument count" BEFORE entry — the install
    // verified but call-through falsely FAILED. Keyed on the signature, not the
    // arg_type name, so it covers every 0-arg handler. (area-loop round 1, 2026-08-31.)
    if (CONFIG.signature && Array.isArray(CONFIG.signature.args) && CONFIG.signature.args.length === 0) {
        return fn();
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
    // RETIRED orch-iter21 2026-07-31 — see diff_template.js and
    // re/analysis/out3_idx_false_green_audit_20260731.md. It passed a scratch buffer
    // and compared the RETURN VALUE ALONE, never reading the buffer back; all four
    // rows using it returned a CONSTANT in range, so a reimpl that moved no data at
    // all passed. Throws rather than silently reverting if an old entry resurfaces.
    if (CONFIG.arg_type === 'out3_idx') {
        throw new Error('arg_type out3_idx is retired (false-GREEN: never observes ' +
                        'the out buffer); use ptr_out_table_get / out1_idx / idx_out2');
    }
    if (CONFIG.arg_type === 'idx_out2') {
        return fn(input >>> 0, buf, buf.add(4));
    }
    if (CONFIG.arg_type === 'fmt_desc_pair_compare') {
        // fn(bufA, bufB [, p3, p4]); mirror diff_template exactly: two 0x40 scratch
        // buffers, sparse `fNN` -> u32@offset writes, fingerprint BOTH buffers plus
        // the (void->0) return. path2 had no case for the {a,b} test-dict shape, so
        // it fell through to fn(input) and every call died "bad argument count"
        // BEFORE entry -- the JMP bytes verified but the call-through falsely FAILED
        // and the interceptor fired 0 times (same class as the 0-arg / vec3_normalize
        // holes). Self-allocates its buffers so it does not depend on `buf` sizing.
        // (area-loop render round 2, 2026-09-01.)
        const SZ = 0x40;
        const bufA = Memory.alloc(SZ), bufB = Memory.alloc(SZ);
        function fillBuf(b, fields) {
            for (let k = 0; k < SZ; k += 4) b.add(k).writeU32(0);
            if (fields) for (const key of Object.keys(fields)) {
                if (key[0] !== 'f') continue;
                const off = parseInt(key.slice(1), 16);
                if (!Number.isNaN(off) && off + 4 <= SZ) b.add(off).writeU32(fields[key] >>> 0);
            }
        }
        function fp(b) { let f = 0; for (let k = 0; k < SZ; k += 4) f = ((f * 31) ^ b.add(k).readU32()) >>> 0; return f; }
        fillBuf(bufA, input && input.a); fillBuf(bufB, input && input.b);
        const argc = CONFIG.signature.args.length;
        const ret = (argc === 4) ? fn(bufA, bufB, (input.p3 | 0), (input.p4 | 0))
                                 : fn(bufA, bufB);
        const retU = (ret === null || ret === undefined) ? 0
                   : (typeof ret === 'object') ? (parseInt(ret.toString(), 16) >>> 0)
                   : (ret >>> 0);
        return [(retU & 0xffff).toString(16), fp(bufA).toString(16), fp(bufB).toString(16)].join(',');
    }
    if (CONFIG.arg_type === 'stricmp_pair') {
        // fn(const char* s1, const char* s2) -> int. Mirror diff_template exactly:
        // two self-allocated 512-byte scratch buffers seeded byte-by-byte + NUL; a JS
        // `null` operand is passed as a real NULL pointer to hit the callee null-guard.
        // path2 had no case for the {s1,s2} test-dict shape, so it would fall through to
        // fn(input) and die "bad argument count" BEFORE entry (same class as the 0-arg /
        // vec3_normalize / fmt_desc_pair_compare holes). Observable is the signed int
        // return coerced to uint32. (area-loop render round 3, 2026-09-01.)
        function seedStr(sval) {
            if (sval === null) return ptr(0);
            const b = Memory.alloc(512);
            const s = String(sval);
            for (let j = 0; j < s.length; j++) b.add(j).writeU8(s.charCodeAt(j) & 0xff);
            b.add(s.length).writeU8(0);
            return b;
        }
        const p1 = seedStr((input && input.s1 !== undefined) ? input.s1 : '');
        const p2 = seedStr((input && input.s2 !== undefined) ? input.s2 : '');
        const ret = fn(p1, p2);
        return (ret === null || ret === undefined) ? 0
             : (typeof ret === 'object' ? (ret.toInt32() >>> 0) : (ret >>> 0));
    }
    if (CONFIG.arg_type === 'str_inplace_transform') {
        // void fn(char* s) in-place mutator (RwStrupr/RwStrlwr). Mirror diff_template:
        // one 512-byte scratch seeded with test.s + NUL, JS null -> NULL ptr; observable
        // is the post-call NUL-terminated buffer bytes. Void return ignored. path2 had no
        // case for the {s} test-dict shape (same "bad argument count" hole class as the
        // other self-allocating handlers). (area-loop render round 4, 2026-09-01.)
        const sval = (input && input.s !== undefined) ? input.s : input;
        if (sval === null) { fn(ptr(0)); return 'NULL'; }
        const s = String(sval);
        const b = Memory.alloc(512);
        for (let j = 0; j < s.length; j++) b.add(j).writeU8(s.charCodeAt(j) & 0xff);
        b.add(s.length).writeU8(0);
        fn(b);
        const out = [];
        for (let j = 0; j <= s.length; j++) out.push(b.add(j).readU8());
        return out.join(',');
    }
    if (CONFIG.arg_type === 'draw_quad_observe') {
        // 12-arg Im2D quad draw: `input` is the full arg vector. path2 had no case
        // for it, so the dispatcher fell through to `fn(input)` and every call died
        // with "bad argument count" BEFORE entry -- the JMP install verified but the
        // call-through falsely FAILED (same class as the 0-arg allocator_nonnull hole
        // and the scalars hole). Marshal exactly as diff_template.js packArgs does:
        // 'pointer' args via ptr((v|0)>>>0), all others passed raw. Void return.
        // (area-loop frontend round 1, 2026-09-01.)
        const sigArgs = CONFIG.signature.args;
        const a = [];
        for (let k = 0; k < sigArgs.length; k++) {
            if (sigArgs[k] === 'pointer') a.push(ptr((input[k] | 0) >>> 0));
            else a.push(input[k]);
        }
        return fn.apply(null, a);
    }
    if (CONFIG.arg_type === 'eax_ptr_ebx_outbuf') {
        // Register-convention leaf: EAX=src-ptr, EBX=dst-ptr, no stack args. The
        // NativeFunction at TARGET_ADDR (`fn`) cannot set EAX/EBX, so build a
        // trampoline to TARGET_ADDR (the installed JMP routes it to the reimpl,
        // firing the interceptor). `input` is a u16 array written verbatim into
        // src; observe the dst buffer as FP_LEN/2 u16 words. Mirrors the path1
        // diff_template.js handler. (area-frontend r3.)
        const SRC_LEN = 128, DST_LEN = 128, FP_LEN = 64;
        const src = Memory.alloc(SRC_LEN), dst = Memory.alloc(DST_LEN);
        for (let k = 0; k < SRC_LEN; k += 2) src.add(k).writeU16(0);
        for (let k = 0; k < input.length; k++) src.add(k * 2).writeU16(input[k] & 0xffff);
        for (let k = 0; k < DST_LEN; k += 2) dst.add(k).writeU16(0);
        const code = Memory.alloc(Process.pageSize);
        Memory.patchCode(code, 18, function (cw) {
            const w = new X86Writer(cw, { pc: code });
            w.putU8(0x53);                              // push ebx
            w.putBytes([0xB8, 0, 0, 0, 0]);            // mov eax, src (patch +2)
            w.putBytes([0xBB, 0, 0, 0, 0]);            // mov ebx, dst (patch +7)
            w.putU8(0xE8);                              // call rel32
            const rel = TARGET_ADDR.sub(code.add(16)).toInt32();
            w.putBytes([rel & 0xff, (rel >>> 8) & 0xff,
                        (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
            w.putU8(0x5B);                              // pop ebx
            w.putU8(0xC3);                              // ret
            w.flush();
        });
        code.add(2).writeU32(parseInt(src.toString(), 16) >>> 0);
        code.add(7).writeU32(parseInt(dst.toString(), 16) >>> 0);
        const Fn = new NativeFunction(code, 'void', [], 'mscdecl');
        Fn();
        let s = '';
        for (let k = 0; k < FP_LEN; k += 2) {
            s += ('0000' + (dst.add(k).readU16() & 0xffff).toString(16)).slice(-4);
        }
        return '0x' + s;
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
    const buf = (['vec3_ptr'].includes(CONFIG.arg_type)) ? Memory.alloc(12)  /* out3_idx retired */
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
