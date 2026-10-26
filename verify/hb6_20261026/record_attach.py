#!/usr/bin/env python3
"""HB-6 recording driver: scenario_launch.py drives a stock-original race (hooks OFF), and
record_race.py --attach-pid records the registry functions inside it by explicit PID.

Run inside ONE mashed_lock hold:
  py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label HB-6 -- \
      py -3.12 verify/hb6_20261026/record_attach.py --track 2 --funcs rwp_normalize3

scenario_launch.py spawns, owns and kills its MASHED; this script never kills a game.
"""
import argparse
import re
import subprocess
import sys
import time
from pathlib import Path

W = Path(__file__).resolve().parents[2]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--track', type=int, default=2)
    ap.add_argument('--cars', type=int, default=4)
    ap.add_argument('--hold', type=int, default=60)
    ap.add_argument('--funcs', required=True)
    ap.add_argument('--settle', type=float, default=5.0)
    ap.add_argument('--window', type=float, default=30.0)
    ap.add_argument('--max', type=int, default=1200)
    ap.add_argument('--log', required=True, help='scenario_launch stdout copy')
    a = ap.parse_args()
    scen = [sys.executable, str(W / 're' / 'frida' / 'scenario_launch.py'), '--track', str(a.track),
            '--cars', str(a.cars), '--hold', str(a.hold)]
    log = open(a.log, 'w', encoding='utf-8')
    p = subprocess.Popen(scen, cwd=str(W), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
                         encoding='utf-8', errors='replace')
    pid = None
    t_end = time.time() + 240
    for line in p.stdout:
        log.write(line); log.flush()
        m = re.search(r'scenario_launch\s+pid=(\d+)', line)
        if m:
            pid = int(m.group(1))
        if 'RACE RUNNING' in line:
            break
        if time.time() > t_end:
            break
    if pid is None or p.poll() is not None:
        print('scenario_launch did not reach a race'); p.wait(); return 2
    print(f'race running in pid={pid} (scenario_launch owns it)')
    rr = subprocess.run([sys.executable, str(W / 're' / 'tools' / 'veccap' / 'record_race.py'),
                         '--attach-pid', str(pid), '--funcs', a.funcs, '--settle', str(a.settle),
                         '--window', str(a.window), '--max', str(a.max),
                         '--scenario', f'scenario_launch.py --track {a.track} --cars {a.cars} --hold {a.hold} '
                                       '(stock original, MASHED_RE_NO_AUTO_HOOK=1)'], cwd=str(W))
    for line in p.stdout:
        log.write(line)
    p.wait()
    log.close()
    print(f'record_race rc={rr.returncode} scenario_launch rc={p.returncode}')
    return rr.returncode


if __name__ == '__main__':
    sys.exit(main())
