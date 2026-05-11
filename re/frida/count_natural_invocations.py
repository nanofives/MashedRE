# Count natural invocations of our 3 verified-C3 hooks while user runs the
# game manually. Helps decide which canonical scenario produces meaningful
# trace evidence for C4 promotion.
#
# Procedure:
#   1. Run this script (it polls for MASHED.exe and attaches).
#   2. Launch MASHED.exe via Explorer.
#   3. Navigate the main menu / submenus / settings / wherever.
#   4. Press Ctrl-C in this terminal when done — script reports counts.
#
# No .asi is loaded; we attach Interceptor at the ORIGINAL function RVAs.
import signal
import sys
import time
from pathlib import Path

import frida

try:
    import psutil
except ImportError:
    sys.exit("psutil required")

HOOKS = {
    'Vec3Magnitude':  '0x004c3ac0',
    'FastSqrt':       '0x004c3b30',
    'FastInvSqrt':    '0x004c3b90',
}

AGENT_JS = '''
'use strict';
const HOOKS = $HOOKS$;
const counters = {};
Object.keys(HOOKS).forEach(function (name) {
    counters[name] = 0;
    try {
        Interceptor.attach(ptr(HOOKS[name]), {
            onEnter: function () { counters[name]++; }
        });
    } catch (e) {
        send({ kind: 'attach_fail', name: name, err: e.message });
    }
});
rpc.exports = {
    snapshot: function () { return counters; },
};
send({ kind: 'ready', hooks: Object.keys(HOOKS) });
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
    print("waiting for MASHED.exe — launch it now from Explorer")
    deadline = time.time() + 120
    pid = None
    while time.time() < deadline:
        pid = find_mashed_pid()
        if pid: break
        time.sleep(0.1)
    if not pid:
        sys.exit("timeout")
    print(f"  found pid={pid}")

    time.sleep(0.05)
    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        sys.exit(f"attach failed: {e}")

    import json
    js = AGENT_JS.replace('$HOOKS$', json.dumps({k: v for k, v in HOOKS.items()}))
    script = session.create_script(js)
    def on_message(msg, data):
        if msg['type'] == 'error':
            print(f"  agent error: {msg.get('description')}")
            return
        p = msg.get('payload', {})
        if p.get('kind') == 'ready':
            print(f"  hooks attached: {', '.join(p['hooks'])}")
            print(f"  now navigate the game; press Ctrl-C here to snapshot + exit")
        elif p.get('kind') == 'attach_fail':
            print(f"  FAILED to attach {p['name']}: {p['err']}")
    script.on('message', on_message)
    script.load()

    interrupted = {'value': False}
    def handler(sig, frame): interrupted['value'] = True
    signal.signal(signal.SIGINT, handler)

    last_snap = {k: 0 for k in HOOKS}
    print()
    print("counts (refreshed every 2s; Ctrl-C to stop):")
    print(f"  {'function':<18s}  {'total':>8s}  {'delta':>8s}")
    while not interrupted['value']:
        try:
            if not psutil.pid_exists(pid):
                print("  MASHED.exe gone")
                break
            snap = script.exports_sync.snapshot()
            line = []
            for name in HOOKS:
                t = snap.get(name, 0)
                d = t - last_snap[name]
                line.append(f"  {name:<18s}  {t:>8d}  {d:>+8d}")
                last_snap[name] = t
            print('\n'.join(line))
            print('---')
            time.sleep(2)
        except KeyboardInterrupt:
            break
        except Exception:
            break

    try:
        final = script.exports_sync.snapshot()
        print()
        print("=== FINAL COUNTS ===")
        for name, count in final.items():
            print(f"  {name:<18s}  {count}")
    except Exception:
        pass
    try: session.detach()
    except Exception: pass
    return 0


if __name__ == '__main__':
    sys.exit(main())
