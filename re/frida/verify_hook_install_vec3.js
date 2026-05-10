// Frida agent: verify the inline-JMP hook installer end-to-end on Vec3Magnitude.
//
// Strategy:
//  1. Snapshot first 8 bytes at 0x004c3ac0 BEFORE Module.load — these are the
//     unpatched prologue bytes from MASHED.exe.
//  2. Wait for the RW3 fast-sqrt LUT to populate (so subsequent calls don't
//     access uninitialised globals).
//  3. Module.load() our mashed_re_dev.asi WITHOUT the MASHED_RE_NO_AUTO_HOOK
//     env var — DllMain runs InjectHooks() runs HookSystem::InstallAll() runs
//     the 5-byte E9 rel32 patcher.
//  4. Snapshot first 8 bytes at 0x004c3ac0 AFTER Module.load. Verify:
//        - byte[0] == 0xE9                                  (JMP rel32 opcode)
//        - dword at byte[1..4] == reimpl_addr - 0x004c3ac0 - 5
//        - hash mismatch vs pre-snapshot                    (sanity)
//  5. Use Interceptor.attach at the reimpl's address to count entries.
//  6. Force-call 0x004c3ac0 (the patched entry) with test vectors. Each call
//     must (a) not crash, (b) produce correct magnitudes, (c) increment the
//     interceptor count by 1.
//
// PASS criteria (all must hold):
//   - First byte at 0x004c3ac0 is 0xE9 after hook install.
//   - rel32 in the patch matches the math `reimpl - target - 5`.
//   - Interceptor at the reimpl fires once per call to the patched entry.
//   - Outputs from patched entry match the magnitudes we already verified in
//     run_diff_vec3.py (e.g., (3,4,0) -> 5.0).
'use strict';

const ASI_PATH        = 'C:\\Users\\maria\\Desktop\\Proyectos\\Mashed\\mashedmod\\build\\mashed_re_dev.asi';
const TARGET_ADDR     = ptr('0x004c3ac0');
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

function bytesHex(p, n) {
    const arr = [];
    for (let i = 0; i < n; i++) arr.push(p.add(i).readU8().toString(16).padStart(2, '0'));
    return arr.join(' ');
}

function readLutRoot() {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return null;
        const off = LUT_OFFSET_ADDR.readU32();
        const lutPtr = ptr(base + off).readU32();
        if (lutPtr === 0) return null;
        return lutPtr;
    } catch (e) {
        return null;
    }
}

function pollLutThenRun(triesLeft) {
    if (readLutRoot() !== null) {
        runVerification();
        return;
    }
    if (triesLeft <= 0) {
        send({ type: 'error', msg: 'LUT globals never populated within 10s' });
        return;
    }
    setTimeout(function () { pollLutThenRun(triesLeft - 1); }, 200);
}

function runVerification() {
    // 1. Pre-install snapshot of target bytes.
    const preBytes = bytesHex(TARGET_ADDR, 8);
    send({ type: 'pre_snapshot', target: TARGET_ADDR.toString(), bytes: preBytes });

    // 2. Module.load — no env var, so InjectHooks auto-runs and patches.
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
           module_base: module.base.toString(),
           reimpl_addr: reimplAddr.toString() });

    // 3. Post-install snapshot.
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

    // 4. Interceptor at the reimpl to count entries triggered by the patched JMP.
    let reimplEntries = 0;
    Interceptor.attach(reimplAddr, {
        onEnter: function () { reimplEntries++; }
    });

    // 5. Force-call the patched entry with vectors we've already verified.
    const Vec3MagPatched = new NativeFunction(TARGET_ADDR, 'float', ['pointer'], 'mscdecl');
    const tests = [
        { v: [0.0, 0.0, 0.0], expected: 0.0    },
        { v: [3.0, 4.0, 0.0], expected: 5.0    },
        { v: [1.0, 1.0, 1.0], expected: 1.7320507764816284 },
        { v: [2.0, 3.0, 6.0], expected: 7.0    },
        { v: [10.0, 0.0, 0.0], expected: 10.0  },
    ];
    const buf = Memory.alloc(12);
    const results = [];
    const beforeCount = reimplEntries;
    for (let i = 0; i < tests.length; i++) {
        const t = tests[i];
        buf.writeFloat(t.v[0]);
        buf.add(4).writeFloat(t.v[1]);
        buf.add(8).writeFloat(t.v[2]);
        let got, err = null;
        try { got = Vec3MagPatched(buf); } catch (e) { got = null; err = e.message; }
        results.push({ idx: i, input: t.v, expected: t.expected, got: got,
                       match: got === t.expected, err: err });
    }
    const afterCount = reimplEntries;

    send({ type: 'results',
           reimpl_calls_observed: afterCount - beforeCount,
           reimpl_calls_expected: tests.length,
           cases: results });
}

pollLutThenRun(50);
