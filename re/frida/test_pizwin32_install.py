# Focused test: load .asi with auto-hook OFF, then directly install ONLY the
# two PizWin32 hooks via raw VirtualProtect + memcpy from Frida. This isolates
# the PizWin32 hooks from the pre-existing auto-hook boot crash so we can
# verify them on their own merits.
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
LOG_FILE   = ROOT / 'original' / 'mashed.log'

# After the .asi is loaded with auto-hook OFF, we look up the two exports,
# then write a 5-byte JMP at each RVA pointing at the export. This is the
# minimal subset of HookSystem::InstallAll for our two hooks.
INJECT_JS = '''
'use strict';
const ASI_PATH = '%ASI%';
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

Process.setExceptionHandler(function (details) {
    // Skip benign 'system' exceptions (KernelBase RaiseException for things
    // like FindFirstFile EOF, MCI status polls). We only want real crashes —
    // access-violation, illegal-instruction, divide-by-zero, etc.
    if (details.type === 'system') return false;
    const ctx = details.context;
    let bytesAt = '';
    try {
        const eip = ptr(details.address.toString());
        for (let i = -8; i < 16; i++) {
            try { bytesAt += eip.add(i).readU8().toString(16).padStart(2,'0') + ' '; }
            catch (e) { bytesAt += '?? '; }
        }
    } catch (e) {}
    send({
        kind: 'crash',
        type: details.type,
        address: details.address.toString(),
        eax: ctx.eax.toString(),
        ecx: ctx.ecx.toString(),
        edx: ctx.edx.toString(),
        esi: ctx.esi.toString(),
        edi: ctx.edi.toString(),
        esp: ctx.esp.toString(),
        bytes_around_eip: bytesAt.trim(),
        mem_address: (details.memory && details.memory.address) ? details.memory.address.toString() : '?',
        mem_op: (details.memory && details.memory.operation) ? details.memory.operation : '?',
    });
    return false;
});

function lutReady() {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return false;
        const off = LUT_OFFSET_ADDR.readU32();
        return ptr(base + off).readU32() !== 0;
    } catch (e) { return false; }
}

function patchJmp(targetRva, replacementPtr) {
    const target = ptr(targetRva);
    const rel = replacementPtr.sub(target).sub(5).toInt32();
    Memory.protect(target, 5, 'rwx');
    target.writeU8(0xE9);
    target.add(1).writeS32(rel);
    Memory.protect(target, 5, 'r-x');
}

function installPizHooks() {
    const m = Process.findModuleByName('mashed_re_dev.asi');
    if (!m) { send({kind:'no_module'}); return; }
    const open  = m.findExportByName('PizWin32Open_Compat');
    const read  = m.findExportByName('PizWin32Read_Compat');
    if (!open || !read) {
        send({kind:'no_exports', open: open && open.toString(), read: read && read.toString()});
        return;
    }
    patchJmp(0x004b6710, open);
    patchJmp(0x004b67e0, read);
    send({kind:'piz_hooks_installed', open: open.toString(), read: read.toString()});
}

function poll(triesLeft) {
    if (lutReady()) {
        try {
            Module.load(ASI_PATH);
            send({ kind: 'loaded', attempts: 150 - triesLeft });
            // Now install only our PizWin32 hooks
            installPizHooks();
        } catch (e) {
            send({ kind: 'load_error', msg: e.message });
        }
        return;
    }
    if (triesLeft <= 0) {
        send({ kind: 'lut_timeout' });
        return;
    }
    setTimeout(function () { poll(triesLeft - 1); }, 200);
}
poll(150);
'''

def main():
    if LOG_FILE.exists(): LOG_FILE.unlink()
    env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}  # disable auto-hook
    CREATE_NEW_PROCESS_GROUP = 0x00000200
    DETACHED_PROCESS         = 0x00000008
    print(f"spawning {MASHED_EXE} (auto-hook disabled)")
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS)
    print(f"  pid = {proc.pid}")

    time.sleep(0.2)
    device = frida.get_local_device()
    try:
        session = device.attach(proc.pid)
    except Exception as e:
        print(f"attach failed: {e}")
        try: proc.kill()
        except Exception: pass
        return 4

    state = {'loaded': False, 'piz_installed': False, 'error': None, 'crash': None}
    def on_message(message, data):
        if message['type'] == 'error':
            print(f"  agent error: {message.get('description')}")
            state['error'] = message; return
        p = message.get('payload', {})
        k = p.get('kind')
        if k == 'loaded':
            print(f"  .asi loaded (LUT-ready after {p['attempts']} attempts)")
            state['loaded'] = True
        elif k == 'piz_hooks_installed':
            print(f"  PIZ hooks patched in:")
            print(f"    PizWin32Open  -> {p['open']}")
            print(f"    PizWin32Read  -> {p['read']}")
            state['piz_installed'] = True
        elif k == 'no_module':
            print("  ERR: mashed_re_dev.asi not found in process modules")
            state['error'] = p
        elif k == 'no_exports':
            print(f"  ERR: missing exports {p}")
            state['error'] = p
        elif k == 'load_error':
            print(f"  ERR: Module.load failed: {p['msg']}")
            state['error'] = p
        elif k == 'lut_timeout':
            print("  ERR: LUT never populated")
            state['error'] = p
        elif k == 'crash':
            print("\n=== CRASH CAUGHT ===")
            for kk, vv in p.items():
                if kk == 'kind': continue
                print(f"  {kk:20s} {vv}")
            state['crash'] = p

    asi_path_escaped = str(ASI_PATH).replace('\\', '\\\\')
    script_text = INJECT_JS.replace('%ASI%', asi_path_escaped)
    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()

    print("  monitoring for 45s (crash, or boot completes)")
    deadline = time.time() + 45
    while time.time() < deadline:
        ec = proc.poll()
        if ec is not None and state['crash'] is None:
            print(f"  process exited code 0x{ec & 0xffffffff:08x} (no caught exception)")
            break
        if state['crash'] is not None:
            print("  killing process")
            try: proc.kill()
            except Exception: pass
            break
        time.sleep(0.2)
    else:
        print("  survived 45s — killing")
        try: proc.kill()
        except Exception: pass

    try: session.detach()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    if LOG_FILE.exists() and LOG_FILE.stat().st_size > 0:
        print(f"\n=== mashed.log ({LOG_FILE.stat().st_size} bytes) — tail informative lines ===")
        with LOG_FILE.open(encoding='utf-8', errors='replace') as f:
            lines = f.readlines()
        interesting = [ln.rstrip() for ln in lines if any(k in ln for k in
            ('RwEngine','Calling','PIZ','Opening piz','Closing piz','Reading texture',
             'Searching piz','Loading','FAILED','BYPASS','compat','Number of subsystems','OK'))]
        for ln in interesting[-50:]: print(f"  {ln}")
    else:
        print("\nNO mashed.log written")

    if state['crash'] is not None:
        print("\nRESULT: CRASH during boot/post-install")
        return 2
    if not state['piz_installed']:
        print("\nRESULT: piz hooks never installed (error path)")
        return 3
    print("\nRESULT: survived — piz hooks installed, no crash within 45s")
    return 0


if __name__ == '__main__':
    sys.exit(main())
