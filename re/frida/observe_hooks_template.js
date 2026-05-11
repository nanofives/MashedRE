// Canonical-scenario observation agent for C4 evidence collection.
//
// Snapshots bytes at the three target RVAs, loads mashed_re_dev.asi (which
// runs DllMain -> HookSystem::InstallAll -> three inline-JMP patches), then
// re-snapshots and reports. NO Interceptor anywhere — hot-path overhead
// destabilizes Mashed in ~6s (FastSqrt fires ~2,700/s at main menu).
//
// The Python driver then idles for N seconds with the .asi loaded; an RPC
// export lets it re-poll the byte state at the end to confirm the patches
// stayed installed throughout the observation window.
'use strict';

const CONFIG = $CONFIG$;
const HOOKS = CONFIG.hooks;     // [ {name, rva, export}, ... ]
const ASI_PATH = CONFIG.asi_path;

function bytesHex(p, n) {
    const arr = [];
    for (let i = 0; i < n; i++) arr.push(p.add(i).readU8().toString(16).padStart(2, '0'));
    return arr.join(' ');
}

function snapshot(label) {
    const out = {};
    for (let i = 0; i < HOOKS.length; i++) {
        const h = HOOKS[i];
        const addr = ptr(h.rva);
        out[h.name] = {
            rva: h.rva,
            bytes: bytesHex(addr, 8),
            opcode: '0x' + addr.readU8().toString(16),
        };
    }
    return out;
}

rpc.exports = {
    poll: function () { return snapshot('poll'); },
};

const pre = snapshot('pre');
send({ type: 'pre_snapshot', hooks: pre });

let module;
try {
    Module.load(ASI_PATH);
    module = Process.findModuleByName('mashed_re_dev.asi');
    if (module === null) {
        send({ type: 'error', msg: 'findModuleByName returned null after Module.load' });
        throw new Error('module not found');
    }
} catch (e) {
    send({ type: 'error', msg: 'Module.load failed: ' + e.message });
    throw e;
}

// Resolve each export and compute expected rel32.
const reimpl = {};
for (let i = 0; i < HOOKS.length; i++) {
    const h = HOOKS[i];
    const exp = module.findExportByName(h.export);
    if (exp === null) {
        send({ type: 'error', msg: h.export + ' export not found in .asi' });
        throw new Error('missing export ' + h.export);
    }
    reimpl[h.name] = exp.toString();
}

// Explicitly invoke InjectHooks() — don't rely on DllMain's env-var gate,
// which can silently no-op if MASHED_RE_NO_AUTO_HOOK leaks into the env.
const injectAddr = module.findExportByName('InjectHooks');
if (injectAddr === null) {
    send({ type: 'error', msg: 'InjectHooks export not found in .asi' });
    throw new Error('missing InjectHooks');
}
try {
    const Inject = new NativeFunction(injectAddr, 'void', [], 'mscdecl');
    Inject();
    send({ type: 'inject_called', export_addr: injectAddr.toString() });
} catch (e) {
    send({ type: 'error', msg: 'InjectHooks() call threw: ' + e.message });
    throw e;
}

const post = snapshot('post');
const verified = [];
for (let i = 0; i < HOOKS.length; i++) {
    const h = HOOKS[i];
    const addr = ptr(h.rva);
    const opcode = addr.readU8();
    const rel32 = addr.add(1).readS32();
    const expected_rel32 = ptr(reimpl[h.name]).toInt32() - addr.toInt32() - 5;
    verified.push({
        name: h.name,
        rva: h.rva,
        reimpl_addr: reimpl[h.name],
        pre_bytes: pre[h.name].bytes,
        post_bytes: post[h.name].bytes,
        opcode_hex: '0x' + opcode.toString(16),
        opcode_ok: opcode === 0xE9,
        rel32_hex: '0x' + (rel32 >>> 0).toString(16),
        expected_rel32_hex: '0x' + (expected_rel32 >>> 0).toString(16),
        rel32_ok: rel32 === expected_rel32,
        bytes_changed: pre[h.name].bytes !== post[h.name].bytes,
    });
}
send({ type: 'post_snapshot',
       module_base: module.base.toString(),
       module_size: module.size,
       hooks: verified });
