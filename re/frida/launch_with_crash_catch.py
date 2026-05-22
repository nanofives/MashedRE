# Launch MASHED.exe with our .asi injected AND an exception handler armed.
# Captures crash EIP/registers if the process dies during boot.
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

INJECT_JS = '''
'use strict';
const ASI_PATH = '%ASI%';
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

Process.setExceptionHandler(function (details) {
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
        ebx: ctx.ebx.toString(),
        ecx: ctx.ecx.toString(),
        edx: ctx.edx.toString(),
        esi: ctx.esi.toString(),
        edi: ctx.edi.toString(),
        ebp: ctx.ebp.toString(),
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

function poll(triesLeft) {
    if (lutReady()) {
        try {
            Module.load(ASI_PATH);
            const m = Process.findModuleByName('mashed_re_dev.asi');
            send({ kind: 'loaded', base: m ? m.base.toString() : null,
                   attempts: 150 - triesLeft });
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
    env = {**os.environ}
    CREATE_NEW_PROCESS_GROUP = 0x00000200
    DETACHED_PROCESS         = 0x00000008
    print(f"spawning {MASHED_EXE}")
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

    state = {'loaded': False, 'error': None, 'crash': None}
    def on_message(message, data):
        if message['type'] == 'error':
            print(f"  agent error: {message.get('description')}")
            state['error'] = message; return
        p = message.get('payload', {})
        if p.get('kind') == 'loaded':
            print(f"  .asi loaded at {p['base']}  (LUT-ready after {p['attempts']} attempts)")
            state['loaded'] = True
        elif p.get('kind') == 'load_error':
            print(f"  Module.load failed: {p['msg']}")
            state['error'] = p
        elif p.get('kind') == 'lut_timeout':
            print("  LUT never populated")
            state['error'] = p
        elif p.get('kind') == 'crash':
            print("\n=== CRASH CAUGHT ===")
            for k, v in p.items():
                if k == 'kind': continue
                print(f"  {k:20s} {v}")
            state['crash'] = p

    asi_path_escaped = str(ASI_PATH).replace('\\', '\\\\')
    script_text = INJECT_JS.replace('%ASI%', asi_path_escaped)
    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()

    # Wait for either: crash, or 30s of survival
    print("  waiting up to 60s for crash or boot completion")
    deadline = time.time() + 60
    while time.time() < deadline:
        ec = proc.poll()
        if ec is not None and state['crash'] is None:
            # If process died without crash signal (Frida missed it)
            print(f"  process exited code 0x{ec & 0xffffffff:08x} without exception caught")
            break
        if state['crash'] is not None:
            print("  crash captured, killing process")
            try: proc.kill()
            except Exception: pass
            break
        time.sleep(0.2)
    else:
        print("  survived 60s — killing")
        try: proc.kill()
        except Exception: pass

    try: session.detach()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    if LOG_FILE.exists() and LOG_FILE.stat().st_size > 0:
        print(f"\n=== mashed.log ({LOG_FILE.stat().st_size} bytes) — tail ===")
        with LOG_FILE.open(encoding='utf-8', errors='replace') as f:
            lines = f.readlines()
        for ln in lines[-30:]:
            print(f"  {ln.rstrip()}")

    return 0 if state['crash'] is None else 1


if __name__ == '__main__':
    sys.exit(main())
