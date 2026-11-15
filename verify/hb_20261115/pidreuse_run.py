"""HB-MASHEDLOCK-STATUS-PIDREUSE-RUN: stage a lock file whose pid is LIVE (a sleeping child this script spawns, so not
the lock's real owner) with an owner_start that does not match that process, then run `mashed_lock.py status` as a
fresh process. Private MASHED_LOCK_DIR + MASHED_LOCK_MARKER_DIR, never the live files. Expect alive=False."""
import json, os, subprocess, sys, tempfile, shutil, time
HERE = os.path.dirname(os.path.abspath(__file__))
ML = os.path.normpath(os.path.join(HERE, '..', '..', 're', 'orchestrator', 'mashed_lock.py'))
sys.path.insert(0, os.path.dirname(ML))
import mashed_lock as ml
root = tempfile.mkdtemp(prefix='mlk_pidreuse_')
lockdir = os.path.join(root, 'locks'); markers = os.path.join(root, 'markers'); os.makedirs(lockdir); os.makedirs(markers)
kid = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(120)'])
try:
    time.sleep(0.5)
    real_start = ml._pid_start(kid.pid)
    rec = {'pid': kid.pid, 'at': '2026-11-15T00:00:00', 'label': 'reused-pid-stage', 'host': 'x', 'owner_start': real_start - 5000}
    name = 'pidreuse_run'
    json.dump(rec, open(os.path.join(lockdir, name + '.lock'), 'w'))
    print('staged pid=%d (live=%s) real_start=%s owner_start=%s' % (kid.pid, ml._pid_alive(kid.pid), real_start, rec['owner_start']))
    env = dict(os.environ, MASHED_LOCK_DIR=lockdir, MASHED_LOCK_MARKER_DIR=markers)
    r = subprocess.run([sys.executable, ML, 'status', '--name', name], capture_output=True, text=True, env=env)
    print('status stdout:', r.stdout.strip()); print('status rc:', r.returncode)
    # control: matching owner_start must read alive=True
    rec['owner_start'] = real_start
    json.dump(rec, open(os.path.join(lockdir, name + '.lock'), 'w'))
    c = subprocess.run([sys.executable, ML, 'status', '--name', name], capture_output=True, text=True, env=env)
    print('control (matching owner_start) stdout:', c.stdout.strip())
    ok = 'alive=False' in r.stdout and 'alive=True' in c.stdout
    print('RESULT', 'PASS' if ok else 'FAIL')
    sys.exit(0 if ok else 1)
finally:
    kid.kill(); shutil.rmtree(root, ignore_errors=True)
