# Fully automated: spawn MASHED → mute its audio session → attach Frida →
# instrument the 3 C3 hooks → idle on main menu for N seconds → snapshot
# counts → kill MASHED → report.
#
# Requires:
#   - All 5 disk patches applied (so MASHED boots to main menu without dialogs).
#   - original/videocfg.bin pinned to a windowed mode.
#   - pycaw + comtypes (per-app mute via Windows Core Audio API).
#
# Optional CLI args:
#   --hooks=name1,name2,...   Comma-separated registry key names from hooks_registry.py
#                             (snake_case). Overrides the hardcoded HOOKS dict above.
#   --scenario=tag            Arbitrary tag appended to output filename for this run.
import argparse
import os
import subprocess
import sys
import time
from pathlib import Path

import frida
import psutil

from pycaw.pycaw import AudioUtilities

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
OUT_FILE   = ROOT / 'log' / 'auto_count_at_menu.txt'

sys.path.insert(0, str(ROOT / 're' / 'frida'))

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
rpc.exports = { snapshot: function () { return counters; } };
send({ kind: 'ready' });
'''


def mute_pid_audio(pid: int) -> bool:
    """Mute the audio session belonging to a specific PID. Returns True if found+muted."""
    try:
        sessions = AudioUtilities.GetAllSessions()
        for sess in sessions:
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception as e:
        print(f"  mute failed: {e}")
    return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seconds', type=int, default=30, help='how long to idle at main menu')
    parser.add_argument('--no-mute', action='store_true')
    parser.add_argument('--hooks', type=str, default=None,
                        help='Comma-separated registry key names (snake_case) from hooks_registry.py')
    parser.add_argument('--scenario', type=str, default=None,
                        help='Scenario tag appended to output file name')
    args = parser.parse_args()

    # Resolve --hooks if provided
    global HOOKS, OUT_FILE
    if args.hooks:
        from hooks_registry import HOOKS as REG_HOOKS
        requested = [h.strip() for h in args.hooks.split(',') if h.strip()]
        resolved = {}
        for name in requested:
            if name not in REG_HOOKS:
                sys.exit(f"--hooks: unknown hook {name!r}; registered: {', '.join(REG_HOOKS.keys())}")
            entry = REG_HOOKS[name]
            export_name = entry.get('export', name)
            resolved[export_name] = f"0x{entry['rva']:08x}"
        HOOKS = resolved

    if args.scenario:
        OUT_FILE = ROOT / 'log' / f'auto_count_at_menu_{args.scenario}.txt'

    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    log = ROOT / 'original' / 'mashed.log'
    if log.exists():
        try: log.unlink()
        except PermissionError: pass  # locked by a previous MASHED instance; harmless

    print(f"spawning {MASHED_EXE}")
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent),
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=0x00000200 | 0x00000008)
    pid = proc.pid
    print(f"  pid = {pid}")

    # Mute as soon as the audio session shows up (DirectSound init takes ~1s).
    if not args.no_mute:
        muted = False
        deadline = time.time() + 8
        while time.time() < deadline:
            if mute_pid_audio(pid):
                print(f"  muted Mashed audio session at t+{8 - int(deadline - time.time())}s")
                muted = True
                break
            time.sleep(0.2)
        if not muted:
            print("  could not find audio session within 8s — game may still produce sound")

    # Attach Frida and count.
    time.sleep(0.2)
    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f"attach failed: {e}")
        try: proc.kill()
        except Exception: pass
        return 4

    import json
    js = AGENT_JS.replace('$HOOKS$', json.dumps(HOOKS))
    script = session.create_script(js)
    ready = {'value': False}
    def on_message(msg, data):
        if msg['type'] == 'error':
            print(f"  agent error: {msg.get('description')}"); return
        p = msg.get('payload', {})
        if p.get('kind') == 'ready':
            ready['value'] = True
        elif p.get('kind') == 'attach_fail':
            print(f"  FAILED to attach {p['name']}: {p['err']}")
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 5
    while not ready['value'] and time.time() < deadline:
        time.sleep(0.1)
    if not ready['value']:
        print("  agent never ready"); proc.kill(); return 5

    print(f"  hooks instrumented; idling at main menu for {args.seconds}s")
    print(f"  {'t':>4s}  {'Vec3Mag':>10s}  {'FastSqrt':>10s}  {'FastInvSqrt':>13s}")
    last = {k: 0 for k in HOOKS}
    start = time.time()
    while time.time() - start < args.seconds:
        if not psutil.pid_exists(pid):
            print("  process gone before idle complete"); break
        try:
            snap = script.exports_sync.snapshot()
            elapsed = int(time.time() - start)
            print(f"  {elapsed:>4d}  {snap.get('Vec3Magnitude',0):>10d}  {snap.get('FastSqrt',0):>10d}  {snap.get('FastInvSqrt',0):>13d}")
            last = snap
        except Exception as e:
            print(f"  snapshot err: {e}"); break
        time.sleep(2)

    final = last
    try: final = script.exports_sync.snapshot()
    except Exception: pass

    print()
    print("=== FINAL ===")
    for name, v in final.items():
        print(f"  {name:<18s}  {v}")

    # Cleanup
    try: session.detach()
    except Exception: pass
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    OUT_FILE.write_text(
        f"final counts after {args.seconds}s at main menu:\n" +
        '\n'.join(f"  {k:<18s}  {v}" for k, v in final.items()) + '\n',
        encoding='utf-8')
    print(f"\nwritten to {OUT_FILE}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
