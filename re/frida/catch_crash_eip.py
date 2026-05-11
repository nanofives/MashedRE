# Catch the crash and report exact EIP + register state.
#
# Frida.Process.setExceptionHandler intercepts access violations before
# Windows kills the process. We log the EIP, surrounding bytes, and key
# register state — then let the exception propagate so the process still
# dies (we don't want to mask the bug).
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
LOG_DIR    = ROOT / 'log'
OUT_FILE   = LOG_DIR / 'crash_eip.txt'

AGENT_JS = r'''
'use strict';
Process.setExceptionHandler(function (details) {
    const ctx = details.context;
    let bytesBefore = '', bytesAt = '';
    try {
        const eip = ptr(details.address.toString());
        const arr = new Uint8Array(32);
        for (let i = -16; i < 16; i++) {
            try { arr[i + 16] = eip.add(i).readU8(); }
            catch (e) { arr[i + 16] = 0; }
        }
        for (let i = 0; i < 16; i++) bytesBefore += arr[i].toString(16).padStart(2,'0') + ' ';
        for (let i = 16; i < 32; i++) bytesAt   += arr[i].toString(16).padStart(2,'0') + ' ';
    } catch (e) {}

    send({
        kind:    'crash',
        type:    details.type,
        address: details.address.toString(),
        eax: ctx.eax ? ctx.eax.toString() : '?',
        ebx: ctx.ebx ? ctx.ebx.toString() : '?',
        ecx: ctx.ecx ? ctx.ecx.toString() : '?',
        edx: ctx.edx ? ctx.edx.toString() : '?',
        esi: ctx.esi ? ctx.esi.toString() : '?',
        edi: ctx.edi ? ctx.edi.toString() : '?',
        ebp: ctx.ebp ? ctx.ebp.toString() : '?',
        esp: ctx.esp ? ctx.esp.toString() : '?',
        eip: ctx.pc  ? ctx.pc.toString()  : '?',
        bytes_before_eip: bytesBefore.trim(),
        bytes_at_eip:     bytesAt.trim(),
        mem_info:         (details.memory && details.memory.address) ? details.memory.address.toString() : '?',
        mem_op:           (details.memory && details.memory.operation) ? details.memory.operation : '?',
    });
    return false;  // let the exception propagate (process still dies)
});
send({ kind: 'ready' });
'''


def main():
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    log = ROOT / 'original' / 'mashed.log'
    if log.exists(): log.unlink()

    print(f"spawning {MASHED_EXE}")
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent),
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=0x00000200 | 0x00000008)
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

    crash_info = {'captured': False, 'payload': None}
    def on_message(message, data):
        if message['type'] == 'error':
            print(f"  agent error: {message.get('description')}")
            return
        p = message.get('payload', {})
        if p.get('kind') == 'ready':
            print("  exception handler armed")
        elif p.get('kind') == 'crash':
            print("\n=== CRASH CAUGHT ===")
            for k, v in p.items():
                if k == 'kind': continue
                print(f"  {k}: {v}")
            crash_info['captured'] = True
            crash_info['payload'] = p
            OUT_FILE.write_text(json.dumps(p, indent=2), encoding='utf-8')

    script = session.create_script(AGENT_JS)
    script.on('message', on_message)
    script.load()

    print("  watching up to 25s for crash")
    deadline = time.time() + 25
    while time.time() < deadline:
        ec = proc.poll()
        if ec is not None:
            print(f"  process exited (code 0x{ec & 0xffffffff:08x})")
            break
        time.sleep(0.2)
    else:
        print("  still running at 25s — no crash; killing")
        try: proc.kill()
        except Exception: pass

    try: session.detach()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    if not crash_info['captured']:
        print("\nNO CRASH CAPTURED (process exited but handler didn't fire)")
        return 1
    print(f"\nwritten to {OUT_FILE}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
