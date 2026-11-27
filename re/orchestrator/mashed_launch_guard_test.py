"""Test for mashed_launch_guard (HB-LOCK-BARE-LAUNCH-SURVEY). Private MASHED_LOCK_DIR, real subprocesses, no game."""
import os
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
LOCK_PY = os.path.join(HERE, 'mashed_lock.py')
PY = sys.executable
PASS = []
FAIL = []

D = tempfile.mkdtemp(prefix='guardtest_')
ENV = dict(os.environ, MASHED_LOCK_DIR=D, MASHED_LOCK_MARKER_DIR=os.path.join(D, 'markers'))
ENV.pop('MASHED_LOCK_SOLO_TOKEN', None)
ENV.pop('MASHED_LOCK_SKIP_WAIT', None)

CHILD = ('import sys, subprocess; sys.path.insert(0, r"%s"); import mashed_launch_guard as g, mashed_lock as ml\n'
         'a = sys.argv[1]\n'
         'if a == "game":\n'
         '    p = subprocess.Popen([sys.executable, "-c", "pass", "MASHED.exe"]); p.wait()\n'
         '    print("OWNER", ml._read_owner(ml._lock_path(ml.DEFAULT_NAME)).get("pid"), "ME", __import__("os").getpid())\n'
         'elif a == "nongame":\n'
         '    p = subprocess.Popen([sys.executable, "-c", "pass"]); p.wait()\n'
         '    import os; print("LOCKFILE", os.path.exists(ml._lock_path(ml.DEFAULT_NAME)))\n'
         'elif a == "two":\n'
         '    for _ in range(2):\n'
         '        subprocess.Popen([sys.executable, "-c", "pass", "mashed_re.exe"]).wait()\n'
         '    print("TWO-DONE")\n') % HERE


def check(name, ok, extra=''):
    (PASS if ok else FAIL).append(name)
    print(('PASS ' if ok else 'FAIL ') + name + (' ' + extra if extra else ''), flush=True)


def child(arg, **kw):
    return subprocess.Popen([PY, '-c', CHILD, arg], env=ENV, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, **kw)


def status():
    return subprocess.run([PY, LOCK_PY, 'status'], env=ENV, capture_output=True, text=True).stdout


# 1 a game launch takes the lock: owner is the child itself
p = child('game')
out = p.communicate(timeout=60)[0]
toks = out.split()
own = toks[toks.index('OWNER') + 1] if 'OWNER' in toks else None
me = toks[toks.index('ME') + 1] if 'ME' in toks else None
check('game launch takes lock (owner==self)', own is not None and own == me, out.strip()[-120:])
# 5 released at exit
check('lock free after the script exits', status().startswith('free'), status().strip()[:60])
# 2 non-game Popen does not take it
out = child('nongame').communicate(timeout=60)[0]
check('non-game launch leaves lock file absent', 'LOCKFILE False' in out, out.strip()[-80:])
# 3 under `mashed_lock.py run` (ancestor holds): no deadlock, finishes
t0 = time.time()
r = subprocess.run([PY, LOCK_PY, 'run', '--label', 'ancestor', '--', PY, '-c', CHILD, 'two'], env=ENV,
                   capture_output=True, text=True, timeout=60)
check('under mashed_lock run: no deadlock, two launches', 'TWO-DONE' in r.stdout and time.time() - t0 < 30,
      '%.1fs rc=%s' % (time.time() - t0, r.returncode))
# 4 lock held by someone else: waits, then proceeds after release
holder = subprocess.Popen([PY, LOCK_PY, 'run', '--label', 'other', '--', PY, '-c', 'import time; time.sleep(6)'],
                          env=ENV, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
time.sleep(1.5)
p = child('game')
time.sleep(3.0)
check('waits while another process holds the lock', p.poll() is None)
out = p.communicate(timeout=60)[0]
check('proceeds after the holder releases', 'OWNER' in out and 'WAITING' in out, out.strip()[-100:])
holder.communicate(timeout=30)
# 6 PRIORITY-HOLD makes a guarded launch wait
hold = subprocess.Popen([PY, '-c', 'import time; time.sleep(8)'])
subprocess.run([PY, LOCK_PY, 'prio-hold', '--owner-pid', str(hold.pid), '--label', 'hold'], env=ENV,
               capture_output=True, text=True)
p = child('game')
time.sleep(3.0)
check('waits under PRIORITY-HOLD', p.poll() is None and 'PRIORITY-HOLD' in status())
hold.kill()
out = p.communicate(timeout=60)[0]
check('proceeds when the hold owner is gone', 'OWNER' in out, out.strip()[-100:])

print('%d pass, %d fail' % (len(PASS), len(FAIL)))
sys.exit(1 if FAIL else 0)
