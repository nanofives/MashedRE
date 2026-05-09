# Frida A/B harness for Vec3Magnitude (FUN_004c3ac0) vs our reimpl.
#
# Spawns MASHED.exe with MASHED_RE_NO_AUTO_HOOK=1 so our .asi can be loaded
# without patching the original. The agent waits for the RW fast-sqrt LUT
# globals to populate, then calls both the original and our reimpl on a
# battery of test vectors against the same live LUT. Results are diffed
# bit-exact.
#
# Usage:
#   py -3.12 re/frida/run_diff_vec3.py
import csv
import os
import struct
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
AGENT_JS   = ROOT / 're' / 'frida' / 'diff_vec3_magnitude.js'
LOG_DIR    = ROOT / 'log'
CSV_OUT    = LOG_DIR / 'diff_vec3_magnitude.csv'

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
              f"root={payload['root']}  attempts={payload['attempts']}")
    elif kind == 'asi_loaded':
        print(f"  [agent] .asi loaded  base={payload['base']}  reimpl@{payload['reimpl_addr']}")
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
    if f is None:
        return None
    return struct.unpack('<I', struct.pack('<f', float(f)))[0]


def main():
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")
    if not AGENT_JS.exists():
        sys.exit(f"agent script not found at {AGENT_JS}")

    print(f"spawning {MASHED_EXE}")
    device = frida.get_local_device()
    pid = device.spawn(
        [str(MASHED_EXE)],
        cwd=str(MASHED_EXE.parent),
        env={**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'},
    )
    print(f"  pid = {pid}")

    session = device.attach(pid)
    script = session.create_script(AGENT_JS.read_text(encoding='utf-8'))
    script.on('message', on_message)
    script.load()
    device.resume(pid)

    timeout_s = 30
    deadline = time.time() + timeout_s
    while not done_flag['done'] and time.time() < deadline:
        time.sleep(0.1)

    try:
        session.detach()
    except Exception:
        pass
    try:
        device.kill(pid)
    except Exception:
        pass

    if errors_received and not results_received:
        print("\nNO RESULTS — agent reported errors above. Game may have crashed before LUT populated.")
        return 2

    if not results_received:
        print(f"\nTIMEOUT after {timeout_s}s with no results.")
        return 3

    mismatches = 0
    with CSV_OUT.open('w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh)
        w.writerow(['idx', 'x', 'y', 'z',
                    'original', 'original_bits',
                    'reimpl',   'reimpl_bits',
                    'match', 'err_original', 'err_reimpl'])
        for r in results_received:
            ob = float_bits(r['original'])
            rb = float_bits(r['reimpl'])
            ob_s = f"0x{ob:08x}" if ob is not None else ''
            rb_s = f"0x{rb:08x}" if rb is not None else ''
            w.writerow([r['idx'], *r['input'],
                        r['original'], ob_s,
                        r['reimpl'],   rb_s,
                        r['match'],
                        r.get('err_original') or '',
                        r.get('err_reimpl') or ''])
            if not r['match']:
                mismatches += 1

    print(f"\nResults written to {CSV_OUT}")
    print(f"  total cases: {len(results_received)}")
    print(f"  mismatches:  {mismatches}")
    if mismatches == 0:
        print("\nGREEN: every test vector produced bit-identical output.")
        return 0
    print("\nRED: at least one mismatch — see CSV for details.")
    for r in results_received:
        if not r['match']:
            print(f"  idx={r['idx']}  v=({r['input'][0]}, {r['input'][1]}, {r['input'][2]})  "
                  f"orig={r['original']}  reimpl={r['reimpl']}")
    return 1


if __name__ == '__main__':
    sys.exit(main())
