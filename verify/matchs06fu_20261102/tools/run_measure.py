"""MATCH-S06-FU arm M: boot each standalone at screen 32 with MASHED_DBG_MEASURE=1 and collect the `measure:` lines.

usage: py -3.12 run_measure.py <root> <outdir> <name>=<exe> [<name>=<exe> ...]

Each exe runs with cwd = <root> and MASHED_ROOT = <root> (the root contains original\\), MASHED_GOTO=32,
MASHED_DBG_MEASURE=1, MASHED_SETTINGS_DEFAULTS=1. The run ends when the `measure: toggle` line appears in
<root>\\mashed_re.log (or after 120 s). Only the PID this script spawned is killed. Writes <outdir>\\<name>.log (the
full mashed_re.log) and <outdir>\\<name>.measure.txt (the measure lines). Prints one "[N/M] name STATUS" line per run.
"""
import os, subprocess, sys, time, shutil

root, outdir = sys.argv[1], sys.argv[2]
runs = [a.split('=', 1) for a in sys.argv[3:]]
os.makedirs(outdir, exist_ok=True)
log = os.path.join(root, 'mashed_re.log')
rc = 0
for k, (name, exe) in enumerate(runs, 1):
    if os.path.exists(log):
        os.remove(log)
    env = dict(os.environ)
    env.update(MASHED_ROOT=root, MASHED_GOTO='32', MASHED_DBG_MEASURE='1', MASHED_SETTINGS_DEFAULTS='1',
               MASHED_WIN_POS='primary-bl')
    p = subprocess.Popen([exe], cwd=root, env=env)
    t0, found = time.time(), False
    while time.time() - t0 < 120 and p.poll() is None:
        time.sleep(0.5)
        try:
            if 'measure: toggle' in open(log, encoding='latin-1').read():
                found = True
                time.sleep(1.0)
                break
        except OSError:
            pass
    if p.poll() is None:
        p.kill()
        p.wait()
    shutil.copyfile(log, os.path.join(outdir, name + '.log')) if os.path.exists(log) else None
    lines = [l.rstrip('\n') for l in open(log, encoding='latin-1')] if os.path.exists(log) else []
    meas = [l for l in lines if l.startswith('measure:')]
    open(os.path.join(outdir, name + '.measure.txt'), 'w').write('\n'.join(meas) + '\n')
    st = 'PASS' if found and len(meas) == 19 else 'FAIL'
    rc |= st != 'PASS'
    print(f'[{k}/{len(runs)}] {name} {st} pid={p.pid} measure_lines={len(meas)} elapsed={time.time() - t0:.1f}s', flush=True)
sys.exit(rc)
