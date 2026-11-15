"""HB-MASHEDLOCK-DEFAULT-MARKERDIR: default marker dir is outside %TEMP%, legacy %TEMP% markers stay visible
during the changeover window and are dropped after it. Private LOCALAPPDATA + TEMP, never the live files."""
import json, os, subprocess, sys, tempfile, time, shutil

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = tempfile.mkdtemp(prefix='mlk_md_')
TMP = os.path.join(ROOT, 'temp'); LAD = os.path.join(ROOT, 'localappdata')
os.makedirs(TMP); os.makedirs(LAD)
res = []
TOTAL = 5
def check(n, ok, d=''):
    res.append(ok); print(f"[{len(res)}/{TOTAL}] {n} {'PASS' if ok else 'FAIL'} {d}", flush=True)

env = dict(os.environ); env.pop('MASHED_LOCK_DIR', None); env.pop('MASHED_LOCK_MARKER_DIR', None); env.pop('MASHED_LOCK_LEGACY_SCAN', None)
env['TEMP'] = env['TMP'] = TMP; env['LOCALAPPDATA'] = LAD

def py(code):
    r = subprocess.run([sys.executable, '-c', "import sys;sys.path.insert(0,%r);import mashed_lock as ml\n%s" % (HERE, code)],
                       capture_output=True, text=True, env=env)
    return r.stdout.strip(), r.stderr.strip()

# 1. default marker dir is <LOCALAPPDATA>\mashed_lock\markers, not TEMP; marker written there
out, err = py("ml.prio_mark('mdt','t',600);print(ml._prio_path('mdt'))")
exp = os.path.join(LAD, 'mashed_lock', 'markers')
check('default-marker-dir-outside-temp', os.path.dirname(out) == exp and os.path.exists(out) and not os.listdir(TMP), f"{out} {err}")

# 2. a marker a pre-change tool left in TEMP (live pid, unexpired) is seen inside the changeover window
old = os.path.join(TMP, 'mdt2_prio_%d.lock' % os.getpid())
json.dump({'pid': os.getpid(), 'epoch': int(time.time()), 'expires': int(time.time()) + 600, 'label': 'legacy'}, open(old, 'w'))
out, err = py("print([o.get('label') for o in ml.prio_pending('mdt2')])")
check('legacy-temp-marker-visible-in-window', out == "['legacy']", f"{out} {err}")

# 3. after the window (stamp aged 2 h) the TEMP marker is no longer listed
stamp = os.path.join(exp, 'changeover.epoch')
open(stamp, 'w').write(str(int(time.time()) - 7200))
out, err = py("print([o.get('label') for o in ml.prio_pending('mdt2')])")
check('legacy-temp-marker-dropped-after-window', out == '[]', f"{out} {err}")

# 4. MASHED_LOCK_LEGACY_SCAN=1 forces it back
env['MASHED_LOCK_LEGACY_SCAN'] = '1'
out, err = py("print([o.get('label') for o in ml.prio_pending('mdt2')])")
check('legacy-scan-forced', out == "['legacy']", f"{out} {err}")
env.pop('MASHED_LOCK_LEGACY_SCAN')

# 5. isolated MASHED_LOCK_DIR keeps markers beside its locks (selftest/concurrency contract)
env['MASHED_LOCK_DIR'] = TMP
out, err = py("print(ml._marker_dir_cfg())")
check('isolated-lock-dir-keeps-markers', os.path.normcase(out) == os.path.normcase(TMP), f"{out} {err}")

shutil.rmtree(ROOT, ignore_errors=True)
sys.exit(0 if all(res) and len(res) == TOTAL else 1)
