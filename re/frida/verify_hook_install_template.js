// Generic hook-installer verification agent. CONFIG injected at $CONFIG$.
//
// Spawns with auto-hook ENABLED (no MASHED_RE_NO_AUTO_HOOK). Verifies that
// HookSystem::InstallAll() patches CONFIG.target_rva with E9 rel32 -> our
// exported reimpl, that the rel32 math is correct, and that calls to the
// patched RVA route through our reimpl (counted via Interceptor.attach).
'use strict';

const CONFIG = $CONFIG$;

// ── gamesave.bin write guard (2026-10-11) ───────────────────────────────────
// original\gamesave.bin was zeroed twice on 2026-10-11 (15:30:45, 18:00:57) by
// harness runs of save_write. At the warm menu the 0x24fa0-byte save buffer at
// 0x00803358 is all zero (boot never loads the save into it), and the original
// SaveWrite 0x00404f50 (or the port, which writes the same buffer through the
// same file layer) writes it to "C:gamesave.bin" in the game's cwd = original\.
// Repro: verify/gamesave_zero_20261011/REPRO.md. Any open of *gamesave.bin with
// GENERIC_WRITE is sent to a per-pid file in %TEMP%; reads are untouched, so a
// harness can read the shared save but never mutate it.
(function guardGamesaveWrites() {
  const k32 = Process.getModuleByName('kernel32.dll');
  const tmp = Memory.alloc(1040);
  new NativeFunction(k32.findExportByName('GetTempPathW'), 'uint32', ['uint32', 'pointer'])(520, tmp);
  const dst = tmp.readUtf16String() + 'harness_gamesave_' + Process.id + '.bin';
  for (const nm of ['CreateFileA', 'CreateFileW']) {
    const W = nm.endsWith('W');
    Interceptor.attach(k32.findExportByName(nm), { onEnter(a) {
      if ((a[1].toUInt32() & 0x40000000) === 0) return;          // GENERIC_WRITE only
      const p = W ? a[0].readUtf16String() : a[0].readAnsiString();
      if (!p || !p.toLowerCase().endsWith('gamesave.bin')) return;
      this.buf = W ? Memory.allocUtf16String(dst) : Memory.allocAnsiString(dst);
      a[0] = this.buf;
    } });
  }
})();
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

// Roots for NativeCallbacks and scratch buffers created per call by the
// buffer-argument handlers below. Frida collects a NativeCallback as soon as JS
// drops the last reference, and a collected recorder becomes a wild pointer the
// moment the target dispatches through it — so anything handed to native code
// has to stay reachable for the life of the agent.
var verifyKeep = [];

function callFn(fn, input, buf) {
    // Registry entries for ptr_seed_observe / stub_dispatch_observe express their
    // test vectors as {'scalars': [...]} rather than a bare value or array. path2
    // had no case for that shape, so the dispatcher fell through to `fn(input)`
    // and every call died with "bad argument count" BEFORE the function was ever
    // entered — the same class of hole vec3_normalize had. Keyed off the TEST
    // SHAPE, not the arg_type, so it covers every handler that adopts the shape.
    // (orch-iter21.)
    //
    // NARROWED 2026-09-12 (round 252). Keying off the test SHAPE made this
    // branch SHADOW the layout-aware handlers added below: a ptr_seed_observe
    // entry whose signature declares a pointer argument carries its buffer in
    // arg_layout, NOT in scalars, so applying scalars alone passes too few
    // arguments and still dies "bad argument count" — the very symptom this
    // branch was written to cure, now for the opposite reason. Same shadowing
    // class as U-9067 (the 0-arg guard that had to move to the bottom).
    // It now defers whenever an arg_layout is present; entries without one keep
    // the orch-iter21 behaviour exactly.
    const _hasLayout = Array.isArray(CONFIG.arg_layout) && CONFIG.arg_layout.length > 0;
    if (!_hasLayout && input && typeof input === 'object' && Array.isArray(input.scalars)) {
        return fn.apply(null, input.scalars);
    }
    if (CONFIG.arg_type === 'scalars_to_scattered_globals' && input && typeof input === 'object'
        && !Array.isArray(input) && Array.isArray(input.args)) {
        // negctl race fixtures (2026-10-11) express path2 vectors as {args, seed}, with {scratch: off} addresses and
        // values. path2 fell through to fn(input) and died "bad argument count" before entry (0x004922e0,
        // 2026-10-11). Mirrors diff_template's seed / scratch semantics with the PATCHED entry as the only arm:
        // save observe + seed bytes, apply seeds, call, fingerprint the observe windows (FNV-1a), restore.
        const sz = CONFIG.scratch_size | 0;
        const scratch = sz > 0 ? Memory.alloc(sz) : null;
        verifyKeep.push(scratch);
        if (scratch) { const z = new Uint8Array(sz); z.fill((CONFIG.scratch_fill | 0) & 0xff); scratch.writeByteArray(z.buffer); }
        // `recorder` {args, abi, off, max_calls}: mirror of diff_template's scalars_to_scattered_globals recorder. A seed
        // value {recorder: true} resolves to its address (e.g. a fake RwGlobals device slot). Without it path2 called
        // through an unfilled slot and AV'd at 0x0 after entry (0x00403160, 2026-10-12).
        const recCfg = CONFIG.recorder || null;
        let recorder = null;
        if (recCfg && scratch) {
            const nA = recCfg.args | 0, rOff = recCfg.off | 0, maxC = (recCfg.max_calls | 0) || 8;
            const types = []; for (let k = 0; k < nA; k++) types.push('uint32');
            recorder = new NativeCallback(function () {
                const base = scratch.add(rOff), c = base.readU32();
                if (c < maxC) for (let k = 0; k < nA; k++) base.add(4 + (c * nA + k) * 4).writeU32(arguments[k] >>> 0);
                base.writeU32(c + 1);
                return 0;
            }, 'int', types, recCfg.abi || 'stdcall');
            verifyKeep.push(recorder);
        }
        // `dialog` / `eax_arg` (reearn/settings fixtures, 2026-10-12): lean mirror of diff_template. A hidden STATIC
        // popup (id 0) with WS_CHILD controls is created per call on this thread, {dialog: id} resolves to its HWND,
        // and the tree is destroyed after the call. No readback: path2 proves install + entry + return only; the
        // behaviour is measured by run_diff. Without it the arg read as NULL / "expected a pointer" before entry.
        const dlgCfg = CONFIG.dialog || null;
        let dlg = null;
        if (dlgCfg) {
            if (!globalThis.__p2U) {
                const u32 = Process.getModuleByName('user32.dll');
                const nf = function (n, r, a) { return new NativeFunction(u32.findExportByName(n), r, a, 'stdcall'); };
                globalThis.__p2U = { create: nf('CreateWindowExA', 'pointer', ['uint32', 'pointer', 'pointer', 'uint32', 'int', 'int',
                                                                                'int', 'int', 'pointer', 'pointer', 'pointer', 'pointer']),
                                     destroy: nf('DestroyWindow', 'int', ['pointer']),
                                     send: nf('SendMessageA', 'int', ['pointer', 'uint32', 'pointer', 'pointer']) };
            }
            const U = globalThis.__p2U;
            const parent = U.create(0, Memory.allocUtf8String('STATIC'), Memory.allocUtf8String(''), 0x80000000,
                                    0, 0, 400, 300, NULL, NULL, NULL, NULL);
            if (parent.isNull()) throw new Error('dialog: CreateWindowExA(parent) failed');
            const kids = {};
            (dlgCfg.controls || []).forEach(function (c) {
                const h = U.create(0, Memory.allocUtf8String(c.cls), Memory.allocUtf8String(c.text || ''),
                                   (0x40000000 | (c.style | 0)) >>> 0, 0, 0, 100, 100, parent, ptr(c.id | 0), NULL, NULL);
                if (h.isNull()) throw new Error('dialog: CreateWindowExA(' + c.cls + ', ' + c.id + ') failed');
                kids[c.id] = h;
                if (c.check) U.send(h, 0xf1, ptr(c.check | 0), NULL);
                (c.items || []).forEach(function (it) {
                    const pos = U.send(h, 0x143, NULL, Memory.allocUtf8String(it[0]));
                    U.send(h, 0x151, ptr(pos), ptr(it[1] >>> 0));
                });
                if (c.sel !== undefined && c.sel !== null) U.send(h, 0x14e, ptr(c.sel | 0), NULL);
            });
            dlg = { parent: parent, kids: kids, U: U };
        }
        const dlgHwnd = function (id) {
            if (!dlg) throw new Error('dialog arg without a dialog config');
            if ((id | 0) === 0) return dlg.parent;
            const h = dlg.kids[id | 0];
            if (h === undefined) throw new Error('dialog: no control ' + id);
            return h;
        };
        const rA = function (x) {
            if (x !== null && typeof x === 'object' && x.dialog !== undefined) return dlgHwnd(x.dialog);
            return (x !== null && typeof x === 'object' && x.scratch !== undefined) ? scratch.add(x.scratch | 0) : ptr(x);
        };
        // `call_log` (followup/a2-stubat 2026-10-12): lean mirror of diff_template. Each named entry becomes a callback
        // returning its `ret` (or the test's stub_rets[name], per call); `at` entries are Interceptor.replace'd once,
        // on the first test, after freeze_main stops the game's first thread, and reverted after the last test
        // (see the test loop below). A value {call_log: name} resolves to that callback. No logging: path2 proves
        // install + entry + return; run_diff measures the call sequence.
        const clCfg = CONFIG.call_log || [];
        if (clCfg.length && !globalThis.__p2CallLog) {
            const fns = {}, counts = {};
            clCfg.forEach(function (ce) {
                // ret_type 'float' (drawstream 2026-10-12): return in ST0, as diff_template does.
                const fRet = ce.ret_type === 'float';
                fns[ce.name] = new NativeCallback(function () {
                    const k = counts[ce.name] | 0; counts[ce.name] = k + 1;
                    const t = globalThis.__p2CallLogTest;
                    const sr = (t && t.stub_rets) ? t.stub_rets[ce.name] : undefined;
                    let rv = fRet ? +(ce.ret || 0) : ce.ret;
                    if (Array.isArray(sr)) rv = sr[Math.min(k, sr.length - 1)];
                    else if (sr !== undefined) rv = sr;
                    return fRet ? +rv : (rv | 0);
                }, fRet ? 'float' : 'int', new Array(ce.nargs | 0).fill('uint32'), ce.abi || 'mscdecl');
                verifyKeep.push(fns[ce.name]);
            });
            const at = clCfg.filter(function (ce) { return ce.at !== undefined && ce.at !== null; });
            globalThis.__p2CallLog = { fns: fns, counts: counts, at: at, frozen: false };
            if (at.length) {
                if (!CONFIG.freeze_main) throw new Error('call_log `at` entries require freeze_main');
                __freeze.thread();
                globalThis.__p2CallLog.frozen = true;
                at.forEach(function (ce) {
                    const sa = ptr(ce.at), pre = new Uint8Array(sa.readByteArray(5));
                    Interceptor.replace(sa, fns[ce.name]);
                    Interceptor.flush();
                    const post = new Uint8Array(sa.readByteArray(5));
                    let patched = false;
                    for (let b = 0; b < 5; b++) if (post[b] !== pre[b]) { patched = true; break; }
                    if (!patched) throw new Error('call_log did NOT install at ' + sa);
                });
            }
        }
        if (globalThis.__p2CallLog) {
            globalThis.__p2CallLogTest = input;
            Object.keys(globalThis.__p2CallLog.counts).forEach(function (k) { delete globalThis.__p2CallLog.counts[k]; });
        }
        const rV = function (x) {
            if (x !== null && typeof x === 'object' && x.call_log !== undefined) return globalThis.__p2CallLog.fns[x.call_log].toUInt32();
            if (x !== null && typeof x === 'object' && x.dialog !== undefined) return dlgHwnd(x.dialog).toUInt32();
            if (x !== null && typeof x === 'object' && x.scratch !== undefined) return scratch.add(x.scratch | 0).toUInt32();
            if (x !== null && typeof x === 'object' && x.recorder) return recorder.toUInt32();
            return x;
        };
        const obs = (CONFIG.observe || []).map(function (w) { return { a: rA(w.addr), len: (w.len === undefined || w.len === null) ? 4 : (w.len | 0) }; });
        const seeds = (input.seed || []).map(function (g) { return { a: rA(g.addr), v: rV(g.val), n: (g.size | 0) || 4 }; });
        const obsSaved = obs.map(function (w) { return w.a.readByteArray(w.len); });
        const seedSaved = seeds.map(function (g) { return g.a.readByteArray(g.n); });
        // Pointer-typed signature args need a NativePointer: a bare number failed "expected a pointer" before entry
        // (replay_get_best_time / _current_time path2, 2026-10-12).
        const args = input.args.slice(0, CONFIG.signature.args.length).map(function (v, ix) {
            return CONFIG.signature.args[ix] === 'pointer' ? rA(v) : (rV(v) >>> 0);
        });
        let fp = 0x811c9dc5 | 0, ret;
        try {
            seeds.forEach(function (g) {
                if (g.n === 1) g.a.writeU8(g.v & 0xff); else if (g.n === 2) g.a.writeU16(g.v & 0xffff); else g.a.writeU32(g.v >>> 0);
            });
            let callFnS = fn;
            if (CONFIG.eax_arg !== undefined && CONFIG.eax_arg !== null) {
                // the target takes arg k in EAX: mov eax,[esp+4+4k]; jmp TARGET (through the installed JMP)
                if (!globalThis.__p2EaxTramp) {
                    const k = CONFIG.eax_arg | 0, code = Memory.alloc(Process.pageSize);
                    Memory.patchCode(code, 9, function (cw) {
                        const w = new X86Writer(cw, { pc: code });
                        w.putBytes([0x8B, 0x44, 0x24, (4 + 4 * k) & 0xff]);
                        w.putU8(0xE9);
                        const rel = TARGET_ADDR.sub(code.add(9)).toInt32();
                        w.putBytes([rel & 0xff, (rel >>> 8) & 0xff, (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
                        w.flush();
                    });
                    globalThis.__p2EaxTramp = new NativeFunction(code, CONFIG.signature.ret, CONFIG.signature.args,
                                                                 CONFIG.orig_calling_convention || 'mscdecl');
                    globalThis.__p2EaxKeep = code;
                }
                callFnS = globalThis.__p2EaxTramp;
            }
            ret = callFnS.apply(null, args);
            obs.forEach(function (w) {
                const ba = new Uint8Array(w.a.readByteArray(w.len));
                for (let j = 0; j < ba.length; j++) fp = Math.imul(fp ^ ba[j], 0x01000193);
            });
        } finally {
            if (dlg) { dlg.U.destroy(dlg.parent); dlg = null; }
            obs.forEach(function (w, ix) { w.a.writeByteArray(obsSaved[ix]); });
            seeds.forEach(function (g, ix) { g.a.writeByteArray(seedSaved[ix]); });
        }
        return (CONFIG.signature.ret === 'void' ? '' : ((ret >>> 0).toString(16) + ',')) + (fp >>> 0).toString(16);
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
    if (CONFIG.arg_type === 'struct_str_set') {
        // fn(void* struct, const char* name) -> struct ptr; writes name into struct via
        // the live RW strncpy vtable. Mirror diff_template: fresh zeroed struct + seeded
        // name buffer; observable is CONFIG.observe_len struct bytes from CONFIG.observe_off.
        // (area-loop render round 6, 2026-09-01 — DEMOTED-row re-promotion.)
        const SZ  = (CONFIG.struct_size | 0) || 0x60;
        const off = (CONFIG.observe_offset | 0);
        const len = (CONFIG.observe_length | 0) || 0x20;
        const st = Memory.alloc(SZ);
        for (let k = 0; k < SZ; k += 4) st.add(k).writeU32(0);
        const nm = Memory.alloc(512);
        const s = String(input.s);
        for (let j = 0; j < s.length; j++) nm.add(j).writeU8(s.charCodeAt(j) & 0xff);
        nm.add(s.length).writeU8(0);
        fn(st, nm);
        const out = [];
        for (let k = 0; k < len; k++) out.push(st.add(off + k).readU8());
        return out.join(',');
    }
    if (CONFIG.arg_type === 'str_char_search') {
        // fn(char* s, int c) -> char* into s (or NULL). Mirror diff_template: one 512-byte
        // scratch seeded with test.s + NUL, low byte of test.c; observable is return as a
        // byte offset from buf start (-1 for NULL). path2 had no case for the {s,c} test-dict
        // shape (same "bad argument count" hole class). (area-loop render round 5, 2026-09-01.)
        const b = Memory.alloc(512);
        const s = String(input.s);
        for (let j = 0; j < s.length; j++) b.add(j).writeU8(s.charCodeAt(j) & 0xff);
        b.add(s.length).writeU8(0);
        const c = (input.c | 0) & 0xff;
        const ret = fn(b, c);
        const rp = (ret === null || ret === undefined) ? NULL
                 : (typeof ret === 'object' ? ret : ptr(ret >>> 0));
        return rp.isNull() ? -1 : rp.sub(b).toInt32();
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
        // The immediates are emitted INSIDE Memory.patchCode, not written afterwards.
        // diff_template.js patches them after the callback returns and that works on the
        // path1 lane, but here the post-hoc `code.add(2).writeU32()` did NOT take effect:
        // the reimpl was entered (interceptor 2/2) and immediately AV'd "accessing 0x0",
        // i.e. it executed `mov si,[eax]` with EAX still the placeholder 0. Emitting the
        // real pointer bytes inside the callback removes the second write entirely, so
        // there is no window in which the page can be remapped or the write discarded.
        // (frida-sweep parent lane, 2026-09-01, after the U-9065 install fix made the
        // call-through reachable for the first time and exposed this.)
        const u32le = function (p) {
            const v = parseInt(p.toString(), 16) >>> 0;
            return [v & 0xff, (v >>> 8) & 0xff, (v >>> 16) & 0xff, (v >>> 24) & 0xff];
        };
        const code = Memory.alloc(Process.pageSize);
        Memory.patchCode(code, 18, function (cw) {
            const w = new X86Writer(cw, { pc: code });
            w.putU8(0x53);                                       // push ebx
            w.putBytes([0xB8].concat(u32le(src)));               // mov eax, src
            w.putBytes([0xBB].concat(u32le(dst)));               // mov ebx, dst
            w.putU8(0xE8);                                       // call rel32
            const rel = TARGET_ADDR.sub(code.add(16)).toInt32();
            w.putBytes([rel & 0xff, (rel >>> 8) & 0xff,
                        (rel >>> 16) & 0xff, (rel >>> 24) & 0xff]);
            w.putU8(0x5B);                                       // pop ebx
            w.putU8(0xC3);                                       // ret
            w.flush();
        });
        const Fn = new NativeFunction(code, 'void', [], 'mscdecl');
        Fn();
        let s = '';
        for (let k = 0; k < FP_LEN; k += 2) {
            s += ('0000' + (dst.add(k).readU16() & 0xffff).toString(16)).slice(-4);
        }
        return '0x' + s;
    }
    if (CONFIG.arg_type === 'cache_setter_observe') {
        // Per-test scatter-seed -> call -> scatter-observe, mirroring the path1 handler in
        // diff_template.js exactly. input.seed=[{addr,val}] written pre-call, input.args
        // passed to fn (a null entry gets the harness buf), input.obs (or CONFIG.obs_globals)
        // read post-call as a hex fingerprint; every seeded AND observed global is snapshotted
        // and restored so path2 leaves live state untouched.
        //
        // ADDED 2026-09-01 (parent lane). This handler had existed in diff_template.js ONLY,
        // so any row using it passed path1 and then died in path2 with "expected an integer" —
        // callFn fell through to `fn(input)` and handed the whole test DICT to a NativeFunction.
        // Caught on float_slider_step (0x0045db50), whose path2 reported interceptor 0/2 with
        // the JMP correctly installed. This is the SAME one-template-only class that already
        // cost two rows (fmt_desc_pair_compare, draw_quad_observe): a new arg_type must land in
        // BOTH templates or the install check silently tests nothing.
        //
        // CONFIG.fold_ret (added 2026-10-01, retrofit lane, DEFAULTED OFF) — mirror of
        // the diff_template.js change landed in the same commit. Folds fn's return into
        // the fingerprint for functions whose primary output IS the return (0x00458e00
        // returns the pool index it stored at, or -1). `fold_ret` is already in
        // run_diff.py's forwarding whitelist; run_verify_hook.py's config builder is a
        // SEPARATE whitelist and had to be taught it too.
        const seeds = input.seed || [];
        const obs   = input.obs || CONFIG.obs_globals || [];
        const seedSaved = seeds.map(s => ptr(s.addr).readU32() >>> 0);
        const obsSaved  = obs.map(a => ptr(a).readU32() >>> 0);
        for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seeds[s].val >>> 0);
        const callArgs = (input.args || []).map(a => (a === null ? buf : (a >>> 0)));
        let result = '';
        try {
            const r = fn.apply(null, callArgs);
            if (CONFIG.fold_ret) {
                const rv = (r === null || r === undefined) ? 0
                         : (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0)
                         : (r >>> 0);
                result += ('00000000' + rv.toString(16)).slice(-8);
            }
            for (let o = 0; o < obs.length; o++)
                result += ('00000000' + (ptr(obs[o]).readU32() >>> 0).toString(16)).slice(-8);
        } finally {
            for (let o = 0; o < obs.length; o++) ptr(obs[o]).writeU32(obsSaved[o] >>> 0);
            for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seedSaved[s] >>> 0);
        }
        return '0x' + (result || '0');
    }
    if (CONFIG.arg_type === 'seed_globals_fold_ret') {
        // Scalar-return fn GATED ON READ-ONLY globals. Seed a scattered set of globals
        // per test ({addr,val}), call through the installed site fn(...args), and FOLD
        // (compare) the RETURN value; snapshot+restore each seeded global so live state
        // is untouched between tests in the same process. Mirrors the diff_template.js
        // path1 handler exactly. EXPLICIT case, positioned ABOVE the 0-arg fallback
        // guard (U-9067: an explicit handler must never depend on falling through).
        //   input = { seed:[{addr:'0x..', val:<u32>}], args:[<u32>, ...] }
        // (area-frontend r6, for 0x00430670 clean path DAT_0067e9fc==10.)
        const seeds = input.seed || [];
        const seedSaved = seeds.map(s => ptr(s.addr).readU32() >>> 0);
        for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seeds[s].val >>> 0);
        const callArgs = (input.args || []).map(a => (a >>> 0));
        let ret;
        try {
            const r = fn.apply(null, callArgs);
            ret = (r === null || r === undefined) ? 0
                : (typeof r === 'object') ? (parseInt(r.toString(), 16) >>> 0)
                : (r >>> 0);
        } finally {
            for (let s = 0; s < seeds.length; s++) ptr(seeds[s].addr).writeU32(seedSaved[s] >>> 0);
        }
        return '0x' + ('00000000' + ret.toString(16)).slice(-8);
    }
    if (CONFIG.arg_type === 'per_vehicle_record_seed') {
        // fn(uint slot, float* inN) writes a derived block into the strided per-vehicle
        // record at base+slot*stride+out_off. Call slot 0 with a seeded input so the
        // install call-through has valid args; SAVE/RESTORE the touched region so path2
        // does not perturb live state. Mirrors the diff_template.js handler. (area-vehicle r4.)
        const base   = ptr(CONFIG.slot_base_addr || '0x008815a0');
        const outOff = (CONFIG.out_off | 0);
        const outLen = (CONFIG.out_len | 0) || 0xa8;
        const inN    = (CONFIG.in_floats | 0) || 6;
        const inBuf  = Memory.alloc(inN * 4);
        for (let k = 0; k < inN; k++) inBuf.add(k * 4).writeFloat(1.0 + k);
        const region = base.add(outOff);            // slot 0
        const saved = new Array(outLen);
        for (let k = 0; k < outLen; k++) saved[k] = region.add(k).readU8();
        let ret;
        try { ret = fn(0, inBuf); }
        finally { for (let k = 0; k < outLen; k++) region.add(k).writeU8(saved[k]); }
        return ret;
    }
    if (CONFIG.arg_type === 'render_state_seq_observe') {
        // Group-R install check: seed the mode/replay globals, patch the device
        // vtable slot with a no-op recorder so nothing real dispatches, call fn()
        // once (fires the interceptor), then RESTORE the slot + globals. Stays ABOVE
        // the 0-arg fallback guard (U-9067). Mirrors diff_template's handler.
        // (area-vehicle r6.)
        const devG    = ptr(CONFIG.device_global_str || '0x007d3ff8');
        const slotOff = (CONFIG.slot_off | 0) || 0x20;
        const NA      = (CONFIG.stub_nargs === undefined) ? 2 : (CONFIG.stub_nargs | 0);
        const seeds   = CONFIG.seed_globals || [];
        const savedG = [];
        for (const s of seeds) { const a = ptr(s.addr); savedG.push(a.readU32()); a.writeU32(s.val >>> 0); }
        const types = []; for (let k = 0; k < NA; k++) types.push('uint32');
        const rec = new NativeCallback(function () {}, 'void', types, CONFIG.stub_abi || 'mscdecl');
        let slot = null, origFn = null;
        try { const dev = devG.readPointer(); slot = dev.add(slotOff); origFn = slot.readPointer(); slot.writePointer(rec); } catch (e) {}
        let ret;
        try { ret = fn(); }
        finally {
            try { if (slot && origFn) slot.writePointer(origFn); } catch (e) {}
            for (let k = 0; k < seeds.length; k++) { try { ptr(seeds[k].addr).writeU32(savedG[k]); } catch (e) {} }
        }
        return ret;
    }
    // ── buffer-argument handlers (round 252, 2026-09-12) ────────────────────
    // bgra_encode / ptr_seed_observe / stub_dispatch_observe existed in
    // diff_template.js ONLY, so every row using them passed path1 and then died
    // in path2 with "bad argument count" or "expected a pointer": callFn fell
    // through to `fn(input)` and handed the whole test DICT to a NativeFunction
    // whose signature declares pointer arguments. Install verified, call-through
    // falsely FAILED — the exact one-template-only class this file already
    // records for cache_setter_observe, fmt_desc_pair_compare and
    // draw_quad_observe. It cost r250 (0x004c2c90) a full path2 and then all
    // FOUR r252 rows in one run, which is what finally made the count worth
    // measuring: 15 registry entries carry a {buf:...} position in arg_layout.
    //
    // These MUST sit above the 0-arg fallback (U-9067): an explicit handler must
    // never depend on falling through.
    //
    // Buffers are per-call and freshly zeroed, matching the path1 handlers. This
    // is an INSTALL check, so only the call needs to succeed and route through
    // the reimpl — the fingerprint comparison is path1's job, and nothing here
    // is compared against the original.
    if (CONFIG.arg_type === 'bgra_encode') {
        // fn(byte* buf) -> uint32; tests[i] = [b0,b1,b2,b3] written as bytes.
        const b = Memory.alloc(4);
        const t = Array.isArray(input) ? input : [0, 0, 0, 0];
        for (let k = 0; k < 4; k++) b.add(k).writeU8((t[k] | 0) & 0xff);
        return fn(b);
    }
    if (CONFIG.arg_type === 'ptr_seed_observe' ||
        CONFIG.arg_type === 'stub_dispatch_observe') {
        // Mirrors the path1 buildArgs/applySeed pair: arg_layout maps each call
        // position to a scratch-buffer pointer, a scalar from input.scalars, or
        // (stub_dispatch_observe only) the recorder stub. input.seed supports
        // flat field writes and ptr_to cross-buffer wiring so a real struct GRAPH
        // can be built — without it a double-deref target just AVs on a garbage
        // inner pointer and the call-through fails for the wrong reason.
        const layout = CONFIG.arg_layout || [];
        const NB = (CONFIG.num_bufs | 0) ||
                   layout.filter(function (a) { return a && a.buf !== undefined; }).length || 1;
        const BS = (CONFIG.buf_size | 0) || 64;
        const bufs = [];
        for (let k = 0; k < NB; k++) {
            const nb = Memory.alloc(BS);
            for (let z = 0; z < BS; z++) nb.add(z).writeU8(0);
            bufs.push(nb);
        }
        // A recorder for the stub positions. Its return is CONFIG.stub_ret (or
        // the per-test override), matching path1 so the callee-declines branch
        // is the one exercised here too.
        const NA = (CONFIG.stub_nargs === undefined) ? 3 : (CONFIG.stub_nargs | 0);
        const stubRet = (input && input.stub_ret !== undefined)
            ? (input.stub_ret | 0) : ((CONFIG.stub_ret | 0) || 0);
        const stubTypes = [];
        for (let k = 0; k < NA; k++) stubTypes.push('uint32');
        const stub = new NativeCallback(function () { return stubRet; },
                                        'int', stubTypes, CONFIG.stub_abi || undefined);
        if (!verifyKeep) verifyKeep = [];
        verifyKeep.push(stub, bufs);
        // Per-stub spec (G2), mirrors diff_template.js: seed {stub: {nargs, abi,
        // ret, name}} gets its own recorder with its own arity/ABI/return.
        const stubFor = function (spec) {
            if (spec === true) return stub;
            const n = (spec.nargs === undefined) ? NA : (spec.nargs | 0);
            const r = (spec.name && input && input.stub_rets &&
                       input.stub_rets[spec.name] !== undefined)
                ? (input.stub_rets[spec.name] | 0)
                : (spec.ret !== undefined ? (spec.ret | 0) : stubRet);
            const cb = new NativeCallback(function () { return r; }, 'int',
                new Array(n).fill('uint32'), spec.abi || CONFIG.stub_abi || undefined);
            verifyKeep.push(cb);
            return cb;
        };
        const wrv = function (p, off, type, value) {
            const a = p.add(off);
            switch (type) {
                case 'u8':  a.writeU8(value & 0xff); break;
                case 'u16': a.writeU16(value & 0xffff); break;
                case 's32': a.writeS32(value | 0); break;
                case 'f32': a.writeFloat(value); break;
                default:    a.writeU32(value >>> 0); break;
            }
        };
        ((input && input.seed) || []).forEach(function (s) {
            if (s.stub) bufs[s.buf].add(s.off).writePointer(stubFor(s.stub));
            else if (s.ptr_to !== undefined) bufs[s.buf].add(s.off).writePointer(bufs[s.ptr_to]);
            else wrv(bufs[s.buf], s.off, s.type, s.value);
        });
        const scalars = (input && input.scalars) || [];
        const args = [];
        let si = 0;
        for (let k = 0; k < layout.length; k++) {
            const a = layout[k];
            if (a && a.stub) args.push(stub);
            else if (a && a.buf !== undefined) args.push(bufs[a.buf]);
            else args.push(scalars[si++]);
        }
        return fn.apply(null, args);
    }

    // FALLBACK, and it must stay LAST. A function with zero declared parameters must be
    // called with zero args, regardless of arg_type: the NativeFunction at TARGET_ADDR is
    // built from CONFIG.signature.args, so honor that same ground truth. Without this,
    // self-emitting 0-arg handlers (e.g. allocator_nonnull) fell through to `fn(input)`
    // and died "bad argument count" BEFORE entry — install verified, call-through falsely
    // FAILED. (area-loop round 1, 2026-08-31.)
    //
    // MOVED FROM THE TOP OF callFn TO HERE, 2026-09-01 (frida-sweep parent lane). While it
    // sat above the arg_type chain it SHADOWED every handler whose function takes zero
    // STACK arguments but still needs registers set up by a trampoline. text_ctrl_code_remap
    // (0x004277a0) declares args: [] because it is EAX/EBX register-convention, so this
    // guard returned `fn()` and the eax_ptr_ebx_outbuf trampoline below was never reached:
    // the reimpl was entered with EAX = 0 and AV'd "accessing 0x0" on `mov si,[eax]`. The
    // interceptor still fired, so the failure looked like a broken reimpl rather than a
    // harness dispatch bug. Explicit arg_type handlers must win; this is only the default.
    if (CONFIG.signature && Array.isArray(CONFIG.signature.args) && CONFIG.signature.args.length === 0) {
        return fn();
    }
    return fn(input);
}

function runVerification() {
    const preBytes = bytesHex(TARGET_ADDR, 8);
    send({ type: 'pre_snapshot', target: TARGET_ADDR.toString(), bytes: preBytes });

    let module;
    try {
        // Prefer the module Module.load returns (same as run_diff.py's agent).
        // From a worktree, MASHED.exe's ASI loader has already autoloaded the
        // MAIN checkout's original\mashed_re_dev.asi; findModuleByName then
        // returns THAT copy, which lacks any export added on the worktree's
        // branch ("export not found"). In the main checkout both resolve to the
        // same module, so the main-side behaviour is unchanged.
        const loaded = Module.load(ASI_PATH);
        module = (loaded && loaded.findExportByName) ? loaded
               : Process.findModuleByName('mashed_re_dev.asi');
        if (module === null || module === undefined) {
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
            // G2: an entry may be {addr, nargs, abi, ret, name} (see diff_template.js).
            const se = (a && typeof a === 'object') ? a : null;
            const p = ptr(se ? se.addr : a);
            Interceptor.replace(p, new NativeCallback(function () {
                if (se && se.ret !== undefined) return se.ret | 0;
                return CONFIG.stub_ret | 0;
            // stub_abi (L-INST round 2): a __stdcall callee (0x004a1570, 5 args)
            // replaced by an mscdecl recorder left 20 bytes on the stack and the
            // caller AV'd at 0x0 on return. run_diff's handler already honours it.
            }, 'int', new Array((se && se.nargs !== undefined) ? (se.nargs | 0)
                                  : (CONFIG.stub_nargs || 3)).fill('pointer'),
               (se && se.abi) || CONFIG.stub_abi || 'mscdecl'));
            send({ type: 'log', msg: 'stubbed callee ' + p });
        } catch (e) {
            send({ type: 'error', msg: 'stub_at ' + a + ' failed: ' + e.message });
        }
    });

    // Count only entries from THIS thread (the one making the test calls). With the hook live, the game's own
    // threads may call the target during the window: 0x004943d0 (a destructor) was entered once by the game between
    // two test calls (2026-10-11), and the old all-thread count turned that into "fired 3 times, expected 2".
    // Other-thread entries are reported separately: they show the installed port running in the game's own flow.
    let reimplEntries = 0, otherThreadEntries = 0;
    const abTid = Process.getCurrentThreadId();
    Interceptor.attach(reimplAddr, {
        onEnter: function () { if (this.threadId === abTid) reimplEntries++; else otherThreadEntries++; }
    });

    const Patched = new NativeFunction(TARGET_ADDR, CONFIG.signature.ret, CONFIG.signature.args,
                                       CONFIG.orig_calling_convention || 'mscdecl');
    const buf = (['vec3_ptr'].includes(CONFIG.arg_type)) ? Memory.alloc(12)  /* out3_idx retired */
              : (['int_with_out_ptr', 'idx_out2', 'int_ptr2_out'].includes(CONFIG.arg_type)) ? Memory.alloc(8)
              : (CONFIG.arg_type === 'time_diff_decompose') ? Memory.alloc(16)
              : (CONFIG.arg_type === 'vec3_normalize') ? Memory.alloc(24)  // out[12] + in[12]
              : null;
    const results = [];
    const beforeCount = reimplEntries;
    // standin_object_call (plan item #3): the fixture + heap/import stand-ins of re/frida/js/standins.js, the same
    // objectCall path1 runs, with the PATCHED entry as the only arm.
    if (CONFIG.arg_type === 'standin_object_call') {
        try {
            const res = __standins.objectCall([['P', Patched]], CONFIG, module);
            res.rows.forEach(function (r, i) { results.push({ idx: i, input: CONFIG.tests[i], got: r.P.obs, err: r.P.err }); });
            send({ type: 'log', msg: 'standins forwarded (other threads): ' + JSON.stringify(res.forwarded) });
        } catch (e) {
            send({ type: 'error', msg: 'standin_object_call: ' + e.message });
        }
    }
    for (let i = 0; i < CONFIG.tests.length && CONFIG.arg_type !== 'standin_object_call'; i++) {
        const t = CONFIG.tests[i];
        let got, err = null;
        try { got = callFn(Patched, t, buf); } catch (e) { got = null; err = e.message; }
        results.push({ idx: i, input: t, got: got, err: err });
    }
    if (globalThis.__p2CallLog && globalThis.__p2CallLog.frozen) {
        globalThis.__p2CallLog.at.forEach(function (ce) { try { Interceptor.revert(ptr(ce.at)); } catch (e) {} });
        Interceptor.flush();
        __freeze.flush();
    }
    const afterCount = reimplEntries;

    send({ type: 'results',
           reimpl_calls_observed: afterCount - beforeCount,
           reimpl_calls_expected: CONFIG.tests.length,
           reimpl_calls_other_threads: otherThreadEntries,
           cases: results });
}

pollLutThenRun(150);
