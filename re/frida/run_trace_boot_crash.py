# Boot-crash tracer. Spawns MASHED.exe, attaches Frida, instruments the
# candidate functions around the FONT36.PIZ load. Records the call
# sequence and reports the function that didn't return (= crash site).
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
AGENT_JS   = ROOT / 're' / 'frida' / 'trace_boot_crash.js'
LOG_DIR    = ROOT / 'log'
TRACE_OUT  = LOG_DIR / 'boot_crash_trace.txt'

events = []
ready  = {'value': False}


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'))
        return
    payload = message.get('payload', {})
    if not isinstance(payload, dict): return
    kind = payload.get('kind')
    if kind == 'ready':
        ready['value'] = True
        print(f"  [agent] {payload['count']} interceptors attached")
    elif kind in ('enter', 'leave'):
        events.append(payload)
    elif kind == 'attach_fail':
        print(f"  [agent] FAILED to attach {payload['name']}: {payload['err']}")


def main():
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    # frida.spawn-suspended breaks Mashed's display-enum (game exits during
    # enum, never reaches the interesting boot path). Use subprocess.Popen +
    # short delay before attach — we miss the very first events but the
    # late-boot crash site stays intact.
    _shim = MASHED_EXE.parent / 'd3d9.dll'
    if not _shim.exists():
        sys.exit(f"FATAL: {_shim} missing (d3d9 windowed shim). "
                 f"Run `mashedmod\\build_d3d9_shim.bat`, then retry.")
    print(f"spawning {MASHED_EXE} via subprocess")
    proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent))
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
    script = session.create_script(AGENT_JS.read_text(encoding='utf-8'))
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 30
    while not ready['value'] and time.time() < deadline:
        time.sleep(0.05)
    if not ready['value']:
        print("agent never reported ready")
        try: proc.kill()
        except Exception: pass
        return 5

    print("  tracing until process exit or 20s timeout")
    deadline = time.time() + 20
    while time.time() < deadline:
        ec = proc.poll()
        if ec is not None:
            print(f"  process exited (code {ec}) after {len(events)} events")
            break
        time.sleep(0.2)
    else:
        print(f"  still running at 20s; killing. {len(events)} events captured")

    try: session.detach()
    except Exception: pass
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    # Analyze: pair enters with leaves, find unmatched (= crash site).
    lines = []
    lines.append('=== boot crash trace ===')
    lines.append(f'total events: {len(events)}')
    lines.append('')
    lines.append('full sequence:')
    for e in events:
        lines.append(f"  [{e['seq']:4d}] {e['kind']:5s}  {e['name']}")

    open_stack = []
    for e in events:
        if e['kind'] == 'enter':
            open_stack.append(e)
        elif e['kind'] == 'leave':
            for i in range(len(open_stack) - 1, -1, -1):
                if open_stack[i]['name'] == e['name']:
                    open_stack.pop(i)
                    break

    lines.append('')
    if open_stack:
        lines.append('UNMATCHED ENTERS (likely crash candidates, innermost last):')
        for e in open_stack:
            lines.append(f"  enter [{e['seq']}]  {e['name']}")
    else:
        lines.append('every enter had a matching leave — crash happened OUTSIDE traced functions')

    report = '\n'.join(lines)
    TRACE_OUT.write_text(report, encoding='utf-8')
    print()
    print(report)
    print()
    print(f"trace written to {TRACE_OUT}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
