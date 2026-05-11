# Pure-Frida in-memory binary patch: skip the powerups.piz section of the
# boot sequence in MASHED.exe.
#
# UAL is incompatible with MASHED on this machine (0xc0000005 at load).
# Frida-injected .asi survives the inject but the process is unstable.
# This script avoids both — it spawns MASHED, attaches Frida, and writes
# 25 NOPs over the call sequence at 0x0040295d..0x00402975. No DLL is
# loaded into the target process at all (Frida-internal injection only).
#
# Patch range (covers `push str; call open; call helper1; call helper2;
# call close`):
#   0x0040295d  PUSH 0x5cc3b4    (powerups.piz string ptr)
#   0x00402962  CALL 0x00495280  (FUN_00495280 piz open)
#   0x00402967  CALL 0x0045bae0
#   0x0040296c  CALL 0x00418980  (thunk_FUN_0041a060)
#   0x00402971  CALL 0x004952f0  (FUN_004952f0 close)
#   0x00402976  PUSH 0x5cc390    (next section: panel.piz — DO NOT TOUCH)
#
# All 4 callees treat their stack as stdcall-clean (no `add esp` between
# them in the original asm), so replacing 25 contiguous bytes with NOPs
# leaves the stack balanced.
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'

AGENT_JS = r'''
'use strict';
const PATCH_ADDR = ptr('0x0040295d');
const PATCH_LEN  = 25;
const NOPS = new Uint8Array(PATCH_LEN);
for (let i = 0; i < PATCH_LEN; i++) NOPS[i] = 0x90;

try {
    const pre = new Uint8Array(PATCH_LEN);
    for (let i = 0; i < PATCH_LEN; i++) pre[i] = PATCH_ADDR.add(i).readU8();

    Memory.patchCode(PATCH_ADDR, PATCH_LEN, function (codeAddr) {
        codeAddr.writeByteArray(NOPS);
    });

    const post = new Uint8Array(PATCH_LEN);
    for (let i = 0; i < PATCH_LEN; i++) post[i] = PATCH_ADDR.add(i).readU8();

    let allNop = true;
    for (let i = 0; i < PATCH_LEN; i++) if (post[i] !== 0x90) { allNop = false; break; }

    send({ kind: 'patched',
           pre_first10:  Array.from(pre.slice(0, 10)).map(b => b.toString(16).padStart(2,'0')).join(' '),
           post_first10: Array.from(post.slice(0, 10)).map(b => b.toString(16).padStart(2,'0')).join(' '),
           all_nop: allNop });
} catch (e) {
    send({ kind: 'patch_error', msg: e.message });
}
'''


def main():
    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")

    log = ROOT / 'original' / 'mashed.log'
    if log.exists(): log.unlink()

    print(f"spawning {MASHED_EXE}")
    # Detached stdio to mimic Explorer-style launch
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

    state = {'done': False, 'ok': False}
    def on_message(message, data):
        if message['type'] == 'error':
            print(f"  agent error: {message.get('description')}")
            state['done'] = True; return
        p = message.get('payload', {})
        if p.get('kind') == 'patched':
            print(f"  patched 25 bytes at 0x0040295d")
            print(f"    pre:  {p['pre_first10']} ...")
            print(f"    post: {p['post_first10']} ...")
            print(f"    all 25 bytes are 0x90: {p['all_nop']}")
            state['ok'] = p['all_nop']; state['done'] = True
        elif p.get('kind') == 'patch_error':
            print(f"  patch failed: {p['msg']}")
            state['done'] = True

    script = session.create_script(AGENT_JS)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 5
    while not state['done'] and time.time() < deadline:
        time.sleep(0.05)

    if not state['ok']:
        print("patch did not complete; killing")
        try: proc.kill()
        except Exception: pass
        return 1

    print("  detaching Frida; watching MASHED for 30s")
    try: session.detach()
    except Exception: pass

    deadline = time.time() + 30
    while time.time() < deadline:
        ec = proc.poll()
        if ec is not None:
            print(f"  process exited (code 0x{ec & 0xffffffff:08x}) at t+{30 - int(deadline - time.time())}s")
            break
        time.sleep(0.5)
    else:
        print("  still running at 30s — game appears to have reached a stable state. killing")
        try: proc.kill()
        except Exception: pass

    try: proc.wait(timeout=3)
    except Exception: pass

    if log.exists():
        size = log.stat().st_size
        print(f"\n=== mashed.log ({size} bytes), tail interesting lines ===")
        with log.open(encoding='utf-8', errors='replace') as f:
            lines = f.readlines()
        keys = ('RwEngine','Calling','PIZ','Opening piz','Closing piz','Reading texture',
                'Searching piz','Loading','FAILED','BYPASS','OK','Number of subsystems','Saving video')
        kept = [ln.rstrip() for ln in lines if any(k in ln for k in keys)]
        for ln in kept[-30:]: print(f"  {ln}")
    else:
        print("\nNO mashed.log written")
    return 0


if __name__ == '__main__':
    sys.exit(main())
