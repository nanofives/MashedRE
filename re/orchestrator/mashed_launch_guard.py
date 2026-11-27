"""mashed_launch_guard: route a script's direct game launches through MashedLock (HB-LOCK-BARE-LAUNCH-SURVEY).

A script adds one line near its imports and keeps its own Popen / frida spawn code:
    import mashed_launch_guard   # (with re/orchestrator on sys.path)
The guard wraps subprocess.Popen and frida Device.spawn. The first launch whose command line names
MASHED.exe or mashed_re.exe takes MashedLock (label from the script name) and holds it until the process
exits. It does nothing when:
  - this process already holds the lock (second launch in the same script),
  - the lock file's owner is an ancestor of this process (the script runs under `mashed_lock.py run`,
    which already holds the lock; taking it again would deadlock),
  - the command line does not name a game exe.
MashedLock itself waits on a SOLO window and a PRIORITY-HOLD (HB-LOCK-ENFORCE), so a guarded launch does too.
"""
import atexit
import os
import re
import subprocess
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.append(_HERE)   # appended, never first: no module in this folder may shadow the caller's own
import mashed_lock as _ml  # noqa: E402

GAME = re.compile(r'(MASHED\.exe|mashed_re\.exe)', re.I)
_state = {'lock': None, 'installed': False}


def _label():
    return 'guard:' + os.path.basename(sys.argv[0] or 'python')


def _names_game(obj):
    try:
        if isinstance(obj, (list, tuple)):
            return any(_names_game(x) for x in obj)
        return bool(GAME.search(os.fspath(obj) if isinstance(obj, os.PathLike) else str(obj)))
    except Exception:
        return False


def ensure_lock():
    """Take the game lock unless this process or an ancestor already holds it. Returns what it did."""
    if _state['lock'] is not None:
        return 'held-by-self'
    owner = _ml._read_owner(_ml._lock_path(_ml.DEFAULT_NAME)) if os.path.exists(_ml._lock_path(_ml.DEFAULT_NAME)) else {}
    if owner and owner.get('pid') in _ml._ancestor_pids() and not _ml._holder_stale(_ml._lock_path(_ml.DEFAULT_NAME), owner):
        return 'held-by-ancestor'
    lk = _ml.MashedLock(_label())
    lk.acquire()
    _state['lock'] = lk
    atexit.register(release)
    return 'acquired'


def release():
    lk = _state['lock']
    if lk is not None:
        _state['lock'] = None
        lk.release()


def install():
    if _state['installed']:
        return
    _state['installed'] = True
    orig_init = subprocess.Popen.__init__

    def init(self, args, *a, **kw):
        if _names_game(args):
            ensure_lock()
        return orig_init(self, args, *a, **kw)
    subprocess.Popen.__init__ = init
    try:
        import frida.core as _fc
        orig_spawn = _fc.Device.spawn

        def spawn(self, program, *a, **kw):
            if _names_game(program):
                ensure_lock()
            return orig_spawn(self, program, *a, **kw)
        _fc.Device.spawn = spawn
    except Exception:
        pass   # frida not installed in this interpreter: Popen path only


install()
