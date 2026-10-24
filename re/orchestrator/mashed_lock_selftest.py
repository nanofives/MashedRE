#!/usr/bin/env py -3.12
# Self-test for the priority request in mashed_lock.py (HB-LOCKPRIO). No game boot.
# Each acquirer is a real subprocess (priority markers are per PID), on a private
# lock name so it never touches the live mashed_machine lock.
#   py -3.12 re\orchestrator\mashed_lock_selftest.py
import json
import os
import subprocess
import sys
import time
import uuid

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mashed_lock as ml

NAME = 'selftest_' + uuid.uuid4().hex[:8]
POLL = '0.2'
results = []


def child(role, label, hold, events, extra=''):
    """Subprocess: acquire (ordinary or priority), log GOT/DONE with a timestamp, hold."""
    code = (
        "import sys,time;sys.path.insert(0,%r);import mashed_lock as ml\n"
        "l=ml.MashedLock(%r,name=%r,poll=%s,quiet=True,priority=%d,wait=30)\n"
        "l.acquire();open(%r,'a').write('GOT %s %%.3f\\n'%%time.time())\n"
        "import os;h=%r\n"
        "while isinstance(h,str) and not os.path.exists(h): time.sleep(0.05)\n"
        "time.sleep(0 if isinstance(h,str) else h);l.release();open(%r,'a').write('DONE %s %%.3f\\n'%%time.time())\n"
    ) % (HERE, label, NAME, POLL, 1 if role == 'prio' else 0, events, label, hold, events, label)
    return subprocess.Popen([sys.executable, '-c', code])


def order(events):
    got = []
    if os.path.exists(events):
        for ln in open(events):
            p = ln.split()
            if p[0] == 'GOT':
                got.append(p[1])
    return got


def wait_for(pred, timeout=15.0):
    t0 = time.time()
    while time.time() - t0 < timeout:
        if pred():
            return True
        time.sleep(0.05)
    return False


def check(name, ok, detail=''):
    results.append(ok)
    print(f"[{len(results)}/{TOTAL}] {name} {'PASS' if ok else 'FAIL'} {detail}", flush=True)


TOTAL = 6
ev = os.path.join(ml.LOCK_DIR, NAME + '_events.txt')
if os.path.exists(ev):
    os.remove(ev)

# 1. default: ordinary acquire works, status shows no priority marker
with ml.MashedLock('plain', name=NAME, quiet=True):
    pass
check('default-acquire-unchanged', not ml.prio_pending(NAME))

# 2. A holds, T (priority) waits, B (worker) arrives after T and must not jump it
flag = os.path.join(ml.LOCK_DIR, NAME + '_release_a')
a = child('worker', 'A', flag, ev)   # A holds until the marker is seen (no timing guess)
wait_for(lambda: order(ev) == ['A'])
t = child('prio', 'T', 0.3, ev)
seen = wait_for(lambda: bool(ml.prio_pending(NAME)))
b = child('worker', 'B', 0.3, ev)
time.sleep(1.5)   # let B reach its wait too
open(flag, 'w').close()
check('priority-marker-visible-while-waiting', seen)
for p in (a, t, b):
    p.wait()
check('order-A-then-T-then-B', order(ev) == ['A', 'T', 'B'], str(order(ev)))
check('marker-cleared-after-acquire', not ml.prio_pending(NAME))

# 3. worker that releases and immediately re-acquires cannot re-take ahead of T
os.remove(ev)
code = (
    "import sys,time;sys.path.insert(0,%r);import mashed_lock as ml\n"
    "for i in range(3):\n"
    " l=ml.MashedLock('W%%d'%%i,name=%r,poll=%s,quiet=True,wait=30);l.acquire()\n"
    " open(%r,'a').write('GOT W%%d %%.3f\\n'%%(i,time.time()));time.sleep(3.0);l.release()\n"
) % (HERE, NAME, POLL, ev)
w = subprocess.Popen([sys.executable, '-c', code])
wait_for(lambda: order(ev) == ['W0'])
t = child('prio', 'T', 0.2, ev)
w.wait()
t.wait()
o = order(ev)
check('worker-cannot-retake-ahead-of-train', o[:2] == ['W0', 'T'], str(o))

# 4. stale markers (dead pid, expired) do not wedge workers
for name_, pid_, exp_ in (('dead', 999999, time.time() + 600), ('expired', os.getpid(), time.time() - 5)):
    path = ml._prio_path(NAME, pid_ if name_ == 'dead' else os.getpid() + 1)
    with open(path, 'w') as f:
        json.dump({'pid': pid_, 'epoch': int(time.time()), 'expires': int(exp_), 'label': name_}, f)
t0 = time.time()
with ml.MashedLock('after-stale', name=NAME, poll=0.2, quiet=True, wait=10):
    pass
check('stale-markers-expire', time.time() - t0 < 3 and not ml.prio_pending(NAME))

for f in (ev, flag):
    if os.path.exists(f):
        os.remove(f)
sys.exit(0 if all(results) and len(results) == TOTAL else 1)
