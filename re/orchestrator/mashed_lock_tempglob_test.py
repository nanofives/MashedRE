"""HB-MASHEDLOCK-TEMPGLOB + HB-MASHEDLOCK-STATUS-PIDREUSE tests (own MASHED_LOCK_DIR)."""
import json, os, subprocess, sys, tempfile, time, shutil

HERE = os.path.dirname(os.path.abspath(__file__))
D = tempfile.mkdtemp(prefix='mlk_tg_')
os.environ['MASHED_LOCK_DIR'] = D
sys.path.insert(0, HERE)
import mashed_lock as ml

res = []
def check(n, ok, d=''):
    res.append(ok); print(f"[{len(res)}/4] {n} {'PASS' if ok else 'FAIL'} {d}", flush=True)

# 1. status ignores a reused PID: live pid (this process), owner_start far from real start
NAME = 'tgtest'
rec = {'pid': os.getpid(), 'at': 'x', 'label': 'reused', 'owner_start': ml._pid_start(os.getpid()) - 5000}
json.dump(rec, open(ml._lock_path(NAME), 'w'))
out = subprocess.run([sys.executable, os.path.join(HERE, 'mashed_lock.py'), 'status', '--name', NAME],
                     capture_output=True, text=True, env=os.environ).stdout
check('status-reused-pid-not-alive', 'alive=False' in out, out.strip().splitlines()[0] if out else '')

# 2. matching owner_start stays alive
rec['owner_start'] = ml._pid_start(os.getpid())
json.dump(rec, open(ml._lock_path(NAME), 'w'))
out = subprocess.run([sys.executable, os.path.join(HERE, 'mashed_lock.py'), 'status', '--name', NAME],
                     capture_output=True, text=True, env=os.environ).stdout
check('status-matching-start-alive', 'alive=True' in out, out.strip().splitlines()[0] if out else '')

# 3. dead pid -> alive=False via _status_alive
check('status-dead-pid', not ml._status_alive({'pid': 99999999}))

# 4. opt-in MASHED_LOCK_MARKER_DIR: a huge LOCK_DIR (fake temp) is never listed.
#    Fresh process, MASHED_LOCK_DIR unset, TMP/TEMP = big fake temp.
BIG = tempfile.mkdtemp(prefix='mlk_big_')
N = 20000
for i in range(N):
    open(os.path.join(BIG, f'junk_{i}.tmp'), 'w').close()
code = ("import sys,time,os;sys.path.insert(0,%r);import mashed_lock as ml;ml._pid_alive(1);"
        "ml.prio_mark('bn','t',600);t=time.time();a=ml.prio_pending('bn');b=ml.prio_lingering('bn');"
        "dt=time.time()-t;ml.prio_clear('bn');print(len(a),round(dt,3),os.path.dirname(ml._prio_path('bn')))") % HERE
env = dict(os.environ); env.pop('MASHED_LOCK_DIR', None); env['TMP'] = env['TEMP'] = BIG; env['MASHED_LOCK_MARKER_DIR'] = os.path.join(BIG, 'mashed_lock_markers')
out = subprocess.run([sys.executable, '-c', code], capture_output=True, text=True, env=env).stdout.split()
ok = len(out) >= 3 and out[0] == '1' and float(out[1]) < 0.5 and out[2].endswith('mashed_lock_markers')
check('markers-in-subdir-no-big-listing', ok, f"entries={N} out={out}")
shutil.rmtree(BIG, ignore_errors=True)

shutil.rmtree(D, ignore_errors=True)
sys.exit(0 if all(res) and len(res) == 4 else 1)
