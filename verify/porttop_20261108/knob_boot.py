"""PORT-TOP-02 knob boot: run the worktree mashed_re.exe once with a given env, wait for it to exit, keep its log.

usage: py -3.12 verify/porttop_20261108/knob_boot.py <label> <root> <timeout_s> [KEY=VALUE ...]
Kills only the PID it spawned (on timeout). Copies <root>/mashed_re.log to D:/MashedWT/PORT-TOP-02_data/<label>.log
and prints the exit code plus the [ORIG_TOP] / NAV_KEYS / B17-SUMMARY lines.
"""
import os
import shutil
import subprocess
import sys
import time

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
EXE = os.environ.get('KNOB_EXE', os.path.join(W, 'mashedmod', 'build', 'mashed_re.exe'))   # PORT-BATCH-GAMEPLAY: KNOB_EXE overrides
OUT = os.environ.get('PORTTOP_OUT', 'D:/MashedWT/PORT-TOP-02_data')   # PORT-TOP-03: PORTTOP_OUT overrides


def main():
    label, root, tmo = sys.argv[1], sys.argv[2], float(sys.argv[3])
    env = dict(os.environ)
    env['MASHED_ROOT'] = root
    for kv in sys.argv[4:]:
        k, v = kv.split('=', 1)
        env[k] = v
    log = os.path.join(root, 'mashed_re.log')
    if os.path.exists(log):
        os.remove(log)
    t0 = time.time()
    p = subprocess.Popen([EXE], cwd=root, env=env)
    print('spawned pid', p.pid, flush=True)
    try:
        rc = p.wait(timeout=tmo)
        how = 'exited'
    except subprocess.TimeoutExpired:
        p.kill()
        rc = p.wait()
        how = 'TIMEOUT (killed own pid %d)' % p.pid
    dt = time.time() - t0
    dst = os.path.join(OUT, label + '.log')
    if os.path.exists(log):
        shutil.copyfile(log, dst)
    print('%s: %s rc=%d (0x%08X) after %.1f s, log -> %s' % (label, how, rc, rc & 0xffffffff, dt, dst))
    if os.path.exists(dst):
        keep = ('[ORIG_TOP]', '[RW_BIND]', 'NAV_KEYS', 'NAV_DEMO', 'B17-SUMMARY', 'FATAL', '[7] MainLoopInit', 'Milestone B10')
        lines = [ln.rstrip() for ln in open(dst, encoding='latin-1') if any(k in ln for k in keep)]
        for ln in lines[:60]:
            print('  ' + ln)
        if len(lines) > 60:
            print('  ... %d more; last:' % (len(lines) - 60))
            for ln in lines[-6:]:
                print('  ' + ln)


if __name__ == '__main__':
    main()
