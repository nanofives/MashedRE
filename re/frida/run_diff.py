# Generic Frida A/B diff harness for any registered C3+ hook.
#
# Usage:  py -3.12 re/frida/run_diff.py <hook_name>
#   e.g.: py -3.12 re/frida/run_diff.py vec3_magnitude
#   e.g.: py -3.12 re/frida/run_diff.py fast_sqrt
#
# Reads hook config from re/frida/hooks_registry.py. To verify a new hook,
# add an entry to that registry — no new harness code required.
import csv
import json
import os
import struct
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent

sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS

MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'diff_template.js'
LOG_DIR    = ROOT / 'log'

results_received = []
errors_received  = []
done_flag        = {'done': False}


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'), message.get('stack'))
        errors_received.append(message)
        done_flag['done'] = True
        return
    payload = message.get('payload', {})
    kind = payload.get('type')
    if kind == 'lut_ready':
        print(f"  [agent] LUT ready  base={payload['base']}  offset={payload['offset']}  "
              f"root={payload['root']}  delta={payload['delta']}  attempts={payload['attempts']}")
    elif kind == 'asi_loaded':
        print(f"  [agent] .asi loaded  base={payload['base']}  {payload['export_name']}@{payload['reimpl_addr']}")
    elif kind == 'results':
        results_received.extend(payload['data'])
        done_flag['done'] = True
    elif kind == 'error':
        print(f"  [agent] ERROR: {payload['msg']}")
        errors_received.append(payload)
        done_flag['done'] = True
    else:
        print('  [agent]', payload)


def float_bits(f):
    if f is None: return None
    try:
        return struct.unpack('<I', struct.pack('<f', float(f)))[0]
    except (OverflowError, struct.error, ValueError, TypeError):
        # Non-float return value (uint32, pointer string '0x…', packed int…)
        if isinstance(f, str) and f.startswith('0x'):
            return int(f, 16) & 0xffffffff
        try:
            return int(f) & 0xffffffff
        except (ValueError, TypeError):
            return None


def main():
    if len(sys.argv) < 2:
        sys.exit(f"usage: {sys.argv[0]} <hook_name>\n  registered: {', '.join(HOOKS.keys())}")
    name = sys.argv[1]
    if name not in HOOKS:
        sys.exit(f"unknown hook {name!r}; registered: {', '.join(HOOKS.keys())}")

    hook = HOOKS[name]
    config = {
        'asi_path':       str(ASI_PATH).replace('\\', '\\\\'),
        'target_rva':     f"0x{hook['rva']:08x}",
        'export':         hook['export'],
        'signature':      hook['signature'],
        'arg_type':       hook['arg_type'],
        'lut_root_delta': hook['lut_root_delta'],
        'tests':          hook['path1_tests'],
    }

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    csv_out = LOG_DIR / f'diff_{name}.csv'

    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")
    if not ASI_PATH.exists():
        sys.exit(f"build artifact not found at {ASI_PATH} — run mashedmod\\build.bat first")

    print(f"hook: {name}  rva={config['target_rva']}  export={config['export']}")
    print(f"spawning {MASHED_EXE} via subprocess (hook BYPASSED)")
    # AppCompat shim is set in HKCU\...\AppCompatFlags\Layers — see scripts/
    # setup_mashed_compat.ps1 / README. The RUNASINVOKER token in that layer
    # suppresses the elevation that WIN98RTM/HIGHDPIAWARE would otherwise
    # trigger, so subprocess.Popen works from a non-elevated shell.
    env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
    proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env)
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

    script_text = AGENT_JS.read_text(encoding='utf-8').replace('$CONFIG$', json.dumps(config))
    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 60
    while not done_flag['done'] and time.time() < deadline:
        time.sleep(0.1)

    try: session.detach()
    except Exception: pass
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    if errors_received and not results_received:
        print("\nNO RESULTS — agent reported errors above.")
        return 2
    if not results_received:
        print("\nTIMEOUT.")
        return 3

    mismatches = 0
    with csv_out.open('w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh)
        w.writerow(['idx', 'input',
                    'original', 'original_bits',
                    'reimpl',   'reimpl_bits',
                    'match', 'err_original', 'err_reimpl'])
        for r in results_received:
            ob, rb = float_bits(r['original']), float_bits(r['reimpl'])
            ob_s = f"0x{ob:08x}" if ob is not None else ''
            rb_s = f"0x{rb:08x}" if rb is not None else ''
            inp = r['input']
            inp_repr = json.dumps(inp) if isinstance(inp, list) else inp
            w.writerow([r['idx'], inp_repr,
                        r['original'], ob_s,
                        r['reimpl'],   rb_s,
                        r['match'],
                        r.get('err_original') or '',
                        r.get('err_reimpl') or ''])
            if not r['match']:
                mismatches += 1

    print(f"\nResults written to {csv_out}")
    print(f"  total cases: {len(results_received)}")
    print(f"  mismatches:  {mismatches}")
    if mismatches == 0:
        print("\nGREEN: every test value produced bit-identical output.")
        return 0
    print("\nRED: at least one mismatch.")
    for r in results_received:
        if not r['match']:
            print(f"  idx={r['idx']}  input={r['input']}  orig={r['original']}  reimpl={r['reimpl']}")
    return 1


if __name__ == '__main__':
    sys.exit(main())
