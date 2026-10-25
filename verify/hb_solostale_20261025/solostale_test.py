# HB-SOLOSTALE proof: stale solo tokens are reclaimed, live ones still block.
# Private TEMP dir + private lock name, so no real lock/token file is touched.
#   py -3.12 verify/hb_solostale_20261025/solostale_test.py
import json, os, subprocess, sys, tempfile, time, uuid

priv = tempfile.mkdtemp(prefix='solostale_')
os.environ['TEMP'] = os.environ['TMP'] = priv   # before mashed_lock reads gettempdir()
tempfile.tempdir = None
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 're', 'orchestrator'))
import mashed_lock as ml
assert os.path.normcase(ml.LOCK_DIR) == os.path.normcase(priv), ml.LOCK_DIR
NAME = 'solostale_' + uuid.uuid4().hex[:6]
res = []


def check(what, ok, detail=''):
    res.append(bool(ok))
    print(f"{'PASS' if ok else 'FAIL'}: {what} {detail}", flush=True)


def write_token(**kw):
    o = {'token': uuid.uuid4().hex, 'pid': None, 'at': 'x', 'epoch': int(time.time()),
         'expires': int(time.time() + 14400), 'label': 'fake-train', 'owner_pid': None, 'owner_start': None}
    o.update(kw)
    with open(ml._solo_path(NAME), 'w') as f:
        json.dump(o, f)
    return o


def begin(wait=2):
    t0 = time.time()
    try:
        tok = ml.solo_begin('new', name=NAME, wait=wait, poll=0.2, quiet=True)
    except TimeoutError:
        return None, time.time() - t0
    ml.solo_end(tok, name=NAME, quiet=True)
    return tok, time.time() - t0


# a PID that is certainly dead: a finished subprocess
dead = subprocess.Popen([sys.executable, '-c', 'pass']); dead.wait()
live = subprocess.Popen([sys.executable, '-c', 'import time;time.sleep(60)'])
try:
    time.sleep(0.5)
    # 1 dead owner_pid, pid=None (the run-2 shape), 4 h ttl still running -> reclaimed at once
    write_token(owner_pid=dead.pid, owner_start=1.0)
    tok, dt = begin()
    check('dead owner_pid token reclaimed by next solo_begin', tok and dt < 1.5, f'({dt:.2f}s)')
    # 2 live owner_pid with matching start -> still blocks
    write_token(owner_pid=live.pid, owner_start=ml._pid_start(live.pid))
    tok, dt = begin()
    check('live owner_pid token still blocks (TimeoutError)', tok is None and dt >= 1.8, f'({dt:.2f}s)')
    # 3 PID reuse: live pid but a different recorded start -> dead
    write_token(owner_pid=live.pid, owner_start=ml._pid_start(live.pid) - 3600)
    tok, dt = begin()
    check('reused PID (start time differs) reclaimed', tok and dt < 1.5, f'({dt:.2f}s)')
    # 4 legacy token, no owner, unexpired -> blocks (TTL fallback intact)
    write_token()
    tok, dt = begin()
    check('ownerless token inside its 4 h ttl still blocks', tok is None and dt >= 1.8, f'({dt:.2f}s)')
    # 5 ownerless token past ttl -> reclaimed
    write_token(expires=int(time.time() - 5))
    tok, dt = begin()
    check('ownerless token past ttl reclaimed (fallback)', tok and dt < 1.5, f'({dt:.2f}s)')
    # 6 CLI: --owner-pid records pid and start; status shows it; owner exit frees it
    cli = [sys.executable, os.path.join(ml.__file__ and os.path.dirname(ml.__file__), 'mashed_lock.py'), '--name', NAME]
    out = subprocess.run(cli[:2] + ['solo-begin', '--label', 'cli', '--owner-pid', str(live.pid)] + cli[2:],
                         capture_output=True, text=True, env=os.environ).stdout
    o = json.load(open(ml._solo_path(NAME)))
    check('CLI solo-begin --owner-pid stores owner_pid and owner_start',
          o['owner_pid'] == live.pid and o['owner_start'] and 'TOKEN ' in out, f"({o['owner_pid']}, {o['owner_start']})")
    tok, dt = begin(wait=1)
    check('CLI token blocks while owner alive', tok is None)
    live.kill(); live.wait()
    tok, dt = begin()
    check('same CLI token reclaimed once the owner process is killed', tok and dt < 1.5, f'({dt:.2f}s)')
finally:
    if live.poll() is None:
        live.kill()
    for f in os.listdir(priv):
        os.remove(os.path.join(priv, f))
    os.rmdir(priv)
print(f"RESULT {sum(res)}/{len(res)}")
sys.exit(0 if all(res) and len(res) == 8 else 1)
