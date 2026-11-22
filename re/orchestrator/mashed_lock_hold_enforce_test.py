#!/usr/bin/env py -3.12
# HB-LOCK-ENFORCE test: the wrappers wait while mashed_lock status shows PRIORITY-HOLD or SOLO,
# print a line when they wait, and an arm started under a hold does not launch until release.
# No game boot. Everything runs in a private MASHED_LOCK_DIR (the live lock and markers are never
# read or written). Subprocesses are real processes, as in the live failure (arm ws_00412190,
# re/frida/stalker_write_surface.py:168, a plain MashedLock with the game lock free).
#   py -3.12 re\orchestrator\mashed_lock_hold_enforce_test.py
import os
import shutil
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
FAKE = tempfile.mkdtemp(prefix='mashed_lock_hold_test_')
ENV = dict(os.environ, MASHED_LOCK_DIR=FAKE, MASHED_LOCK_MARKER_DIR=FAKE)
ENV.pop('MASHED_LOCK_SKIP_WAIT', None)
ENV.pop('MASHED_LOCK_SOLO_TOKEN', None)
sys.path.insert(0, HERE)
os.environ['MASHED_LOCK_DIR'] = FAKE
os.environ['MASHED_LOCK_MARKER_DIR'] = FAKE
import mashed_lock as ml  # noqa: E402
ml.LOCK_DIR = FAKE
NAME = ml.DEFAULT_NAME
LOCK = [sys.executable, os.path.join(HERE, 'mashed_lock.py')]
results = []
TOTAL = 9


def check(name, ok, detail=''):
    results.append(ok)
    print('[%d/%d] %s %s %s' % (len(results), TOTAL, name, 'PASS' if ok else 'FAIL', detail), flush=True)


def popen(cmd, env=None):
    return subprocess.Popen(cmd, env=env or ENV, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)


def stays_running(p, secs):
    t0 = time.time()
    while time.time() - t0 < secs:
        if p.poll() is not None:
            return False
        time.sleep(0.1)
    return True


def finish(p, timeout=20):
    try:
        out, _ = p.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        p.kill()
        out, _ = p.communicate()
        out += '\n[TIMEOUT]'
    return out


# the hold owner: a live process the hold is bound to (the train's pwsh in production)
owner = subprocess.Popen([sys.executable, '-c', 'import time;time.sleep(120)'])
try:
    # 1. clear: wait-clear returns at once
    r = subprocess.run(LOCK + ['wait-clear', '--label', 't1', '--poll', '0.2'], env=ENV, capture_output=True, text=True, timeout=30)
    check('clear-returns-at-once', r.returncode == 0 and 'WAITING' not in r.stdout, r.stdout.strip())

    # 2. hold written, as the train does: status shows PRIORITY-HOLD
    ok = ml.prio_hold(NAME, 'train-pass', owner.pid)
    st = subprocess.run(LOCK + ['status'], env=ENV, capture_output=True, text=True, timeout=30).stdout
    check('status-shows-hold', ok and 'PRIORITY-HOLD' in st, st.strip().splitlines()[-1] if st.strip() else '')

    # 3. the live failure: a plain MashedLock arm with the game lock FREE does not launch under the hold
    arm_code = ("import sys,time;sys.path.insert(0,%r);import mashed_lock as ml\n"
                "l=ml.MashedLock('arm',poll=0.2,wait=30);l.acquire();print('LAUNCHED %%.3f'%%time.time(),flush=True);l.release()\n") % HERE
    arm = popen([sys.executable, '-c', arm_code])
    held_back = stays_running(arm, 2.5)
    check('arm-under-hold-does-not-launch', held_back, 'still waiting after 2.5 s' if held_back else 'LAUNCHED under the hold')

    # 4. wait-clear under the hold prints a line and waits
    wc = popen(LOCK + ['wait-clear', '--label', 't4', '--poll', '0.2'])
    held_wc = stays_running(wc, 2.0)
    check('wait-clear-waits-under-hold', held_wc)

    # 5. ddkobj.py check (the real wrapper) waits under the hold, before touching any build dir
    dd = popen([sys.executable, os.path.join(ROOT, 're', 'tools', 'ddkobj', 'ddkobj.py'), 'check', '--out', os.path.join(FAKE, 'nodir')])
    held_dd = stays_running(dd, 2.5)
    check('ddkobj-check-waits-under-hold', held_dd)

    # 6. the hold owner's own descendant is not blocked, and MASHED_LOCK_SKIP_WAIT=1 skips (train's children)
    skip = subprocess.run(LOCK + ['wait-clear', '--label', 't6', '--wait', '5', '--poll', '0.2'],
                          env=dict(ENV, MASHED_LOCK_SKIP_WAIT='1'), capture_output=True, text=True, timeout=30)
    check('skip-env-bypasses', skip.returncode == 0 and 'WAITING' not in skip.stdout)

    # 7. bounded wait: --wait 1 under the hold exits 3 with a line (no silent hang)
    bw = subprocess.run(LOCK + ['wait-clear', '--label', 't7', '--wait', '1', '--poll', '0.2'], env=ENV, capture_output=True, text=True, timeout=30)
    check('bounded-wait-exits-3', bw.returncode == 3 and 'WAITING' in bw.stdout, 'rc=%s' % bw.returncode)

    # release: the hold becomes a linger (status PRIORITY-LINGER, not PRIORITY-HOLD); everything proceeds
    ml.prio_release(NAME, owner.pid, linger=0)
    out_arm = finish(arm)
    out_wc = finish(wc)
    out_dd = finish(dd)
    check('arm-launches-after-release', 'LAUNCHED' in out_arm and 'WAITING' in out_arm, out_arm.strip().replace('\n', ' | ')[:200])
    check('waiters-print-line-and-clear',
          'WAITING' in out_wc and 'clear after' in out_wc and 'WAITING' in out_dd and 'clear after' in out_dd,
          (out_wc.strip() + ' || ' + out_dd.strip()).replace('\n', ' | ')[:300])
finally:
    owner.kill()
    owner.wait()
    shutil.rmtree(FAKE, ignore_errors=True)

print('hold-enforce: %d/%d PASS' % (sum(results), len(results)))
sys.exit(0 if all(results) and len(results) == TOTAL else 1)
