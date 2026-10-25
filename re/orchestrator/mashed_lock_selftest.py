#!/usr/bin/env py -3.12
# Self-test for mashed_lock.py: priority requests (HB-LOCKPRIO) and the live-holder
# rule (HB-LOCKBREAK). No game boot. Each acquirer is a real subprocess (priority markers
# are per PID). Everything runs in a private temp dir (ml.LOCK_DIR, and TEMP for pwsh), so
# the live mashed_machine lock and its markers are never read or written.
#   py -3.12 re\orchestrator\mashed_lock_selftest.py
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
import uuid

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mashed_lock as ml

FAKE_DIR = tempfile.mkdtemp(prefix='mashed_lock_selftest_')
ml.LOCK_DIR = FAKE_DIR
NAME = 'selftest_' + uuid.uuid4().hex[:8]
POLL = '0.2'
PRELUDE = "import sys,time;sys.path.insert(0,%r);import mashed_lock as ml;ml.LOCK_DIR=%r\n" % (HERE, FAKE_DIR)
results = []


def child(role, label, hold, events, extra=''):
    """Subprocess: acquire (ordinary or priority), log GOT/DONE with a timestamp, hold."""
    code = PRELUDE + (
        "l=ml.MashedLock(%r,name=%r,poll=%s,quiet=True,priority=%d,wait=30)\n"
        "l.acquire();open(%r,'a').write('GOT %s %%.3f\\n'%%time.time())\n"
        "import os;h=%r\n"
        "while isinstance(h,str) and not os.path.exists(h): time.sleep(0.05)\n"
        "time.sleep(0 if isinstance(h,str) else h);l.release();open(%r,'a').write('DONE %s %%.3f\\n'%%time.time())\n"
    ) % (label, NAME, POLL, 1 if role == 'prio' else 0, events, label, hold, events, label)
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


TOTAL = 14
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
code = PRELUDE + (
    "for i in range(3):\n"
    " l=ml.MashedLock('W%%d'%%i,name=%r,poll=%s,quiet=True,wait=30);l.acquire()\n"
    " open(%r,'a').write('GOT W%%d %%.3f\\n'%%(i,time.time()));time.sleep(3.0);l.release()\n"
) % (NAME, POLL, ev)
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


# 5. HB-LOCKBREAK: a live holder is never broken for age. Reproduces 2026-10-25 02:54:58,
# when D4-DC-82a0-gates broke D3-CF-K357's hold (pid 18904 alive=True age=1801s).
def plant(pid, age, **extra):
    rec = {'pid': pid, 'at': 'selftest', 'epoch': int(time.time() - age), 'label': 'K357-like',
           'host': 'selftest'}
    rec.update(extra)
    with open(ml._lock_path(NAME), 'w', encoding='utf-8') as f:
        f.write(json.dumps(rec))


def try_acquire(wait):
    try:
        with ml.MashedLock('breaker', name=NAME, poll=0.2, quiet=True, wait=wait):
            return 'ACQUIRED'
    except TimeoutError:
        return 'TIMEOUT'


def sleeper(secs):
    p = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(%d)' % secs])
    wait_for(lambda: ml._pid_start(p.pid) is not None, 5)
    return p, ml._pid_start(p.pid)


holder, hstart = sleeper(120)
plant(holder.pid, 1801, owner_start=hstart)
r = try_acquire(3)
o = ml._read_owner(ml._lock_path(NAME))
check('live-holder-age-1801-not-broken', r == 'TIMEOUT' and o.get('pid') == holder.pid,
      f"{r} owner={o.get('pid')}")

plant(holder.pid, 7 * 86400)   # pre-HB-LOCKBREAK record: no owner_start, a week old, PID alive
r = try_acquire(2)
check('legacy-record-live-pid-week-old-not-broken', r == 'TIMEOUT', r)

plant(holder.pid, 5, owner_start=round(hstart - 1000, 1))   # PID reused by a later process
check('pid-reuse-start-mismatch-is-broken', try_acquire(5) == 'ACQUIRED')

# the solo drain uses the same rule: it waits on a live 1801 s holder, proceeds once it dies
plant(holder.pid, 1801, owner_start=hstart)
sb = subprocess.Popen([sys.executable, '-c', PRELUDE +
                       "print('TOKEN', ml.solo_begin(label='st', name=%r, ttl=60, poll=0.2, quiet=True), flush=True)"
                       % NAME], stdout=subprocess.PIPE, text=True)
time.sleep(3)
still = sb.poll() is None and os.path.exists(ml._lock_path(NAME))
holder.kill()
holder.wait()
try:
    out = sb.communicate(timeout=15)[0]
except subprocess.TimeoutExpired:
    sb.kill()
    out = ''
check('solo-drain-waits-on-live-then-breaks-dead', still and out.startswith('TOKEN'),
      f"still={still} out={out.strip()[:12]}")
if os.path.exists(ml._solo_path(NAME)):
    os.remove(ml._solo_path(NAME))

plant(holder.pid, 5)   # holder now dead, fresh record
check('dead-holder-still-broken', try_acquire(5) == 'ACQUIRED')

# an empty lock file is a writer between its O_EXCL create and its write: kept inside the
# grace, broken after it
p = ml._lock_path(NAME)
open(p, 'w').close()
fresh = try_acquire(2)
if not os.path.exists(p):   # broken inside the grace (the pre-HB-LOCKBREAK behaviour)
    open(p, 'w').close()
os.utime(p, (time.time() - 60, time.time() - 60))
check('empty-file-grace-then-broken', fresh == 'TIMEOUT' and try_acquire(5) == 'ACQUIRED', fresh)

# 6. MachineLock.ps1 applies the same rule (TEMP points at the fake dir)
holder, hstart = sleeper(60)
plant(holder.pid, 1801, owner_start=hstart)
ps = (". '%s'; try { $h = Acquire-Lock -Name %s -Label psbreaker -TimeoutSec 4 -PollSec 1 -Quiet; "
      "Release-Lock $h; 'ACQUIRED' } catch { 'TIMEOUT' }") % (os.path.join(HERE, 'MachineLock.ps1'), NAME)
env = dict(os.environ, TEMP=FAKE_DIR, TMP=FAKE_DIR)
try:
    pr = subprocess.run(['pwsh', '-NoProfile', '-Command', ps], env=env, capture_output=True, text=True,
                        timeout=60).stdout.strip()
except Exception as e:
    pr = 'ERR %s' % e
o = ml._read_owner(ml._lock_path(NAME))
check('ps-live-holder-age-1801-not-broken', pr.endswith('TIMEOUT') and o.get('pid') == holder.pid, pr[-40:])
holder.kill()
holder.wait()

# 7. a PowerShell holder's owner_start reads as the same process from Python (no false
# PID-reuse break across the two writers)
if os.path.exists(ml._lock_path(NAME)):
    os.remove(ml._lock_path(NAME))
ps = (". '%s'; $h = Acquire-Lock -Name %s -Label psholder -Quiet; Start-Sleep -Seconds 8; Release-Lock $h"
      ) % (os.path.join(HERE, 'MachineLock.ps1'), NAME)
psh = subprocess.Popen(['pwsh', '-NoProfile', '-Command', ps], env=env)
wait_for(lambda: bool(ml._read_owner(ml._lock_path(NAME))), 20)
o = ml._read_owner(ml._lock_path(NAME))
r = try_acquire(3)
psh.wait(timeout=30)
check('ps-holder-not-broken-by-python', r == 'TIMEOUT' and bool(o.get('owner_start')),
      f"{r} owner_start={o.get('owner_start')}")

shutil.rmtree(FAKE_DIR, ignore_errors=True)
sys.exit(0 if all(results) and len(results) == TOTAL else 1)
