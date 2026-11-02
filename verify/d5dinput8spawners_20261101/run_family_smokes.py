"""One hooks-ON smoke per rewritten spawner family, in a game root with NO dinput8 proxy.

  py -3.12 run_family_smokes.py <root> <data_dir> [family ...]

<root> holds original\\ without dinput8.dll / dinput8_real.dll, so the only way the
dev asi reaches MASHED.exe is re/frida/dev_asi.py. Each family runs its rewritten
script once with hooks ON, full output to <data_dir>/smoke_<family>.log, and one
"[N/M] family STATUS rc=.. s=.." line here. STATUS is rc-based; the RESULT reads
each log's own verdict lines. A timed-out script has its own process tree killed
(only PIDs this driver started).
"""
import os
import subprocess
import sys
import time
from pathlib import Path

import psutil

WT = Path(__file__).resolve().parents[2]
PY = sys.executable
root = Path(sys.argv[1]).resolve()
data = Path(sys.argv[2]).resolve()
data.mkdir(parents=True, exist_ok=True)
MAIN_LOG = WT.parents[1] / 'log'      # <main>/log (ttd captures live there)
RVA = '0x0042b770'                    # MenuButtonDetectE, installed by the default asi

FAMILIES = [
    # run_verify_hook defaults to the worktree's own build output; point it at the root's
    # asi (the one every other family loads) so the smoke needs no build.
    ('run_verify_hook', [PY, 're/frida/run_verify_hook.py', 'menu_button_detect_e'],
     {'MASHED_VERIFY_ASI': str(root / 'original' / 'mashed_re_dev.asi')}, 240),
    ('canonical_c4', [PY, 're/frida/canonical_c4_verify.py', RVA, '--seconds', '20'], {}, 240),
    ('c4_scenario', [PY, 're/frida/c4_detect_e_scenario.py', '--arm', 'on',
                     '--out', str(data / 'c4_detect_e')], {}, 420),
    ('scenario_launch', [PY, 're/frida/scenario_launch.py', '--hooks', RVA, '--hold', '10'], {}, 300),
    ('statenav', [PY, 're/frida/statenav.py', '--hooks', '--seconds', '45',
                  '--shot-dir', str(data / 'statenav_shots')],
     {'MASHED_HOOK_ONLY': RVA}, 300),
    ('phys_c4', [PY, 're/frida/phys_c4_telemetry.py', 'hooked', 'A4_Entry,0x00470670'], {}, 420),
    ('popen_probes', [PY, 're/frida/pickup_pos_probe2.py', '--hooks', RVA,
                      '--out', str(data / 'pickup_probe2.bmp')], {}, 420),
    ('ttd', [PY, 'scripts/ttd/ttd_reimpl_diff.py', str(MAIN_LOG / 'ttd' / 'calls_004c3b30.csv'),
             '--reimpl', 'asi:FastSqrt'], {}, 300),
]
want = set(sys.argv[3:])
todo = [f for f in FAMILIES if not want or f[0] in want]


def kill_tree(pid):
    try:
        p = psutil.Process(pid)
    except psutil.NoSuchProcess:
        return
    for c in p.children(recursive=True):
        try:
            c.kill()
        except psutil.NoSuchProcess:
            pass
    try:
        p.kill()
    except psutil.NoSuchProcess:
        pass


fails = 0
for i, (name, argv, extra, tmo) in enumerate(todo, 1):
    env = dict(os.environ)
    env['MASHED_ROOT'] = str(root)
    env.setdefault('MASHED_MUTE', '1')
    env.update(extra)
    log = data / f'smoke_{name}.log'
    t0 = time.time()
    with open(log, 'w', encoding='utf-8', errors='replace') as fh:
        fh.write(f'# argv: {argv}\n# extra env: {extra}\n# MASHED_ROOT={root}\n')
        fh.flush()
        p = subprocess.Popen(argv, cwd=str(WT), env=env, stdout=fh, stderr=subprocess.STDOUT)
        try:
            rc = p.wait(timeout=tmo)
        except subprocess.TimeoutExpired:
            kill_tree(p.pid)
            rc = 'TIMEOUT'
    status = 'PASS' if rc == 0 else 'FAIL'
    fails += status != 'PASS'
    print(f'[{i}/{len(todo)}] {name} {status} rc={rc} s={time.time() - t0:.0f}', flush=True)
sys.exit(1 if fails else 0)
