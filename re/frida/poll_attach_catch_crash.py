# Poll-attach harness: waits for MASHED.exe to appear (user launches manually
# via Explorer), then attaches Frida and arms an exception handler. Catches
# the crash EIP/registers so we can see exactly where the piz reader dies.
#
# Usage:
#   1. Run this script.
#   2. Double-click original\MASHED.exe in Explorer.
#   3. Click OK on the resolution selector when it appears.
#   4. Wait — when the game crashes, the script prints crash details.
import json
import sys
import time
from pathlib import Path

import frida

try:
    import psutil
except ImportError:
    sys.exit("psutil required: py -3.12 -m pip install psutil")

ROOT     = Path(__file__).resolve().parent.parent.parent
LOG_DIR  = ROOT / 'log'
OUT_FILE = LOG_DIR / 'crash_eip.txt'

AGENT_JS = r'''
'use strict';
// Module ranges so we can classify EIP + each stack return-address candidate
// as MASHED.exe code vs our .asi vs a system DLL.
function moduleTable() {
    const out = [];
    Process.enumerateModules().forEach(function (m) {
        out.push({ name: m.name, base: m.base.toString(),
                   end: m.base.add(m.size).toString(), size: m.size });
    });
    return out;
}
Process.setExceptionHandler(function (details) {
    const ctx = details.context;
    let bytesBefore = '', bytesAt = '';
    try {
        const eip = ptr(details.address.toString());
        for (let i = -16; i < 0; i++) {
            try { bytesBefore += eip.add(i).readU8().toString(16).padStart(2,'0') + ' '; }
            catch (e) { bytesBefore += '?? '; }
        }
        for (let i = 0; i < 16; i++) {
            try { bytesAt += eip.add(i).readU8().toString(16).padStart(2,'0') + ' '; }
            catch (e) { bytesAt += '?? '; }
        }
    } catch (e) {}

    // Stack dump: read 48 dwords from ESP. Any value inside MASHED.exe's
    // code range is a candidate return address — the topmost is the caller
    // of the faulting (leaf) function.
    let stack = [];
    try {
        const esp = ptr(ctx.esp.toString());
        for (let i = 0; i < 48; i++) {
            try { stack.push({ off: i*4, val: esp.add(i*4).readU32().toString(16) }); }
            catch (e) { break; }
        }
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
        mem_address:      (details.memory && details.memory.address) ? details.memory.address.toString() : '?',
        mem_op:           (details.memory && details.memory.operation) ? details.memory.operation : '?',
        stack:            stack,
        modules:          moduleTable(),
    });
    return false;
});
send({ kind: 'ready' });
'''


def find_mashed_pid():
    for proc in psutil.process_iter(['name', 'pid']):
        try:
            if proc.info['name'] and proc.info['name'].lower() == 'mashed.exe':
                return proc.info['pid']
        except Exception:
            continue
    return None


def main():
    LOG_DIR.mkdir(parents=True, exist_ok=True)

    print("waiting for MASHED.exe to appear — launch it from Explorer now")
    pid = None
    deadline = time.time() + 120
    while time.time() < deadline:
        pid = find_mashed_pid()
        if pid is not None:
            print(f"  found MASHED.exe pid={pid}")
            break
        time.sleep(0.1)
    if pid is None:
        sys.exit("timeout waiting for MASHED.exe")

    # Tiny delay so the process is past CreateProcess but not too far.
    time.sleep(0.05)

    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        sys.exit(f"attach failed: {e}")
    print("  attached")

    captured = {'value': None}
    def on_message(message, data):
        if message['type'] == 'error':
            print(f"  agent error: {message.get('description')}")
            return
        p = message.get('payload', {})
        if p.get('kind') == 'ready':
            print("  exception handler armed — click through the selector now")
        elif p.get('kind') == 'crash':
            print("\n=== CRASH CAUGHT ===")
            for k, v in p.items():
                if k == 'kind': continue
                print(f"  {k:18s} {v}")
            captured['value'] = p

    script = session.create_script(AGENT_JS)
    script.on('message', on_message)
    script.load()

    print("  watching up to 90s for the crash")
    deadline = time.time() + 90
    while time.time() < deadline:
        if captured['value'] is not None:
            break
        try:
            if not psutil.pid_exists(pid):
                print("  MASHED.exe process gone (maybe killed without crash handler firing)")
                break
        except Exception:
            pass
        time.sleep(0.2)
    else:
        print("  timeout — no crash seen in 90s")

    try: session.detach()
    except Exception: pass

    if captured['value']:
        OUT_FILE.write_text(json.dumps(captured['value'], indent=2), encoding='utf-8')
        print(f"\nwritten to {OUT_FILE}")
        return 0
    return 1


if __name__ == '__main__':
    sys.exit(main())
