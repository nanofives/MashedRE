# Catch the crash and report exact EIP + register state.
#
# Frida.Process.setExceptionHandler intercepts access violations before
# Windows kills the process. We log the EIP, surrounding bytes, and key
# register state — then let the exception propagate so the process still
# dies (we don't want to mask the bug).
#
# Spawn path (D5-SHIM-STEP3-FRIDA5, 2026-11-02): re/frida/dev_asi.spawn_original. The game
# used to get mashed_re_dev.asi from the dinput8 proxy autoload; now the asi is
# Module.load'ed and InjectHooks'ed before the first resume (MASHED_HOOK_ONLY / _SKIP still
# honoured), and the exception handler is armed before resume too. --no-hooks = stock game.
# MASHED_ROOT selects a private game root. The pid this script spawns is the only one it kills.
import argparse
import json
import os
import sys
import time
from pathlib import Path

import frida

sys.path.insert(0, str(Path(__file__).resolve().parent))
import dev_asi  # noqa: E402

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = dev_asi.original_exe(ROOT / 'original' / 'MASHED.exe')
LOG_DIR    = ROOT / 'log'
OUT_FILE   = LOG_DIR / 'crash_eip.txt'

AGENT_JS = r'''
'use strict';
const INCLUDE_SYSTEM = %INCLUDE_SYSTEM%;
Process.setExceptionHandler(function (details) {
    // type 'system' = any code Frida has no name for, e.g. the first-chance
    // DBG_PRINTEXCEPTION_C that OutputDebugStringA raises through RaiseException and
    // catches itself. Counted, not reported as a crash, unless --include-system.
    if (details.type === 'system' && !INCLUDE_SYSTEM) {
        send({ kind: 'system_exc', address: details.address.toString() });
        return false;
    }
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
    ap = argparse.ArgumentParser(description='crash EIP catcher on a spawned original')
    ap.add_argument('--no-hooks', action='store_true', help='stock game, asi never mapped')
    ap.add_argument('--seconds', type=float, default=25.0, help='watch window')
    ap.add_argument('--include-system', action='store_true',
                    help="report type 'system' exceptions (first-chance RaiseException) as crashes")
    args = ap.parse_args()

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    log = MASHED_EXE.parent / 'mashed.log'
    if log.exists():
        try: log.unlink()
        except PermissionError: pass

    print(f"spawning {MASHED_EXE} (hooks {'OFF' if args.no_hooks else 'ON'})")
    device = frida.get_local_device()
    try:
        proc, session, info = dev_asi.spawn_original(
            MASHED_EXE, hooks=not args.no_hooks, device=device, resume=False)
    except Exception as e:
        print(f"spawn failed: {e}")
        return 4
    print(f"  pid = {proc.pid}")
    if info:
        print(f"  dev asi loaded at {info.get('base')} installed={info.get('installed')}")

    crash_info = {'captured': False, 'payload': None, 'system': {}}
    def on_message(message, data):
        if message['type'] == 'error':
            print(f"  agent error: {message.get('description')}")
            return
        p = message.get('payload', {})
        if p.get('kind') == 'ready':
            print("  exception handler armed")
        elif p.get('kind') == 'system_exc':
            a = p.get('address')
            crash_info['system'][a] = crash_info['system'].get(a, 0) + 1
        elif p.get('kind') == 'crash':
            print("\n=== CRASH CAUGHT ===")
            for k, v in p.items():
                if k == 'kind': continue
                print(f"  {k}: {v}")
            crash_info['captured'] = True
            crash_info['payload'] = p
            OUT_FILE.write_text(json.dumps(p, indent=2), encoding='utf-8')

    script = session.create_script(AGENT_JS.replace(
        '%INCLUDE_SYSTEM%', 'true' if args.include_system else 'false'))
    script.on('message', on_message)
    script.load()
    device.resume(proc.pid)

    print(f"  watching up to {args.seconds:g}s for crash")
    deadline = time.time() + args.seconds
    while time.time() < deadline:
        ec = proc.poll()
        if ec is not None:
            print(f"  process exited (code 0x{ec & 0xffffffff:08x})")
            break
        time.sleep(0.2)
    else:
        print(f"  still running at {args.seconds:g}s, no crash; killing pid {proc.pid}")
        try: proc.kill()
        except Exception: pass

    try: session.detach()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    for a, n in sorted(crash_info['system'].items()):
        print(f"  ignored {n} type-'system' exception(s) at {a} (first-chance; --include-system to report)")
    if not crash_info['captured']:
        print("\nNO CRASH CAPTURED (process exited but handler didn't fire)")
        return 1
    print(f"\nwritten to {OUT_FILE}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
