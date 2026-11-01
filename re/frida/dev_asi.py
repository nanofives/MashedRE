"""Load mashed_re_dev.asi into an original MASHED.exe without the dinput8 proxy.

Until D5-SHIM-DINPUT8-SPAWNERS (2026-11-01) every hooks-ON spawner got the dev asi
from original\\dinput8.dll: the proxy LoadLibrary's it at import-resolve time
(mashedmod/src/dinput8_shim/dinput8_shim.cpp:61-96) unless MASHED_RE_NO_AUTO_HOOK
is set, and the asi's DllMain then runs InjectHooks()
(mashedmod/src/mashed_re/dll_main.cpp:20-28). Removing the proxy (D5 shim removal,
step 3) needs every spawner to load the asi itself. This module is that one path.

Contract
  * The spawn env ALWAYS carries MASHED_RE_NO_AUTO_HOOK=1. While the proxy is still
    deployed it therefore never maps the asi (no double map, no Windows base-name
    dedup against our Module.load), and the asi's DllMain never auto-installs.
  * Hooks ON = Module.load the asi, then call its InjectHooks export once
    (HookSystem::InstallAll, which still honours MASHED_HOOK_ONLY / _SKIP /
    _MANIFEST / _LO / _HI from the spawn env). Hooks OFF = no load at all.
  * spawn_original() loads the asi BEFORE the first resume, so the inline JMPs are
    in place before any MASHED.exe code runs, the same point the proxy reached.
  * The asi path follows the proxy's search order (dinput8_shim.cpp:84-96):
    <exe dir>\\mashed_re_dev.asi, then <exe dir>\\..\\mashedmod\\build\\mashed_re_dev.asi.
    MASHED_DEV_ASI (or the older MASHED_VERIFY_ASI) overrides it.

mashed_qol.asi (the other proxy load) is not handled here: it is inert unless its
own MASHED_NO_SAVE / MASHED_UNLOCK / MASHED_SPEED vars are set, and no diff
spawner sets them.
"""
from __future__ import annotations

import json
import os
import time
from pathlib import Path

import frida

NO_AUTO = 'MASHED_RE_NO_AUTO_HOOK'
ASI_NAME = 'mashed_re_dev.asi'

_LOAD_JS = r'''
'use strict';
var PATH = %PATH%;
var INSTALL = %INSTALL%;
var r = { kind: 'dev_asi' };
try {
    var pre = Process.findModuleByName('%NAME%');
    if (pre !== null) {
        // Mapped already (proxy run without the var, or a second call): never
        // install twice. The caller sees preloaded=true and decides.
        r.preloaded = true; r.base = pre.base.toString(); r.path = pre.path;
        r.installed = false;
    } else {
        var m = Module.load(PATH);
        r.preloaded = false; r.base = m.base.toString(); r.path = m.path;
        r.installed = false;
        if (INSTALL) {
            new NativeFunction(m.getExportByName('InjectHooks'), 'void', [])();
            r.installed = true;
        }
    }
} catch (e) { r.error = '' + e; }
send(r);
'''


def original_exe(default) -> Path:
    """MASHED_ROOT\\original\\MASHED.exe when MASHED_ROOT is set (private game roots,
    WORKER_RULES 'Builds and game runs'), else the caller's own resolution."""
    root = os.environ.get('MASHED_ROOT', '').strip()
    if root:
        cand = Path(root) / 'original' / 'MASHED.exe'
        if cand.exists():
            return cand
    return Path(default)


def resolve_asi(mashed_exe) -> Path:
    """Path of the dev asi, in the dinput8 proxy's search order (see module doc)."""
    for var in ('MASHED_DEV_ASI', 'MASHED_VERIFY_ASI'):
        v = os.environ.get(var, '').strip()
        if v:
            p = Path(v)
            if not p.is_absolute():
                p = Path(__file__).resolve().parents[2] / p
            return p.resolve()
    exe_dir = Path(mashed_exe).resolve().parent
    local = exe_dir / ASI_NAME
    if local.exists():
        return local
    return (exe_dir.parent / 'mashedmod' / 'build' / ASI_NAME).resolve()


def stock_env(base=None) -> dict:
    """Environment for any original-side spawn: the proxy (if present) loads nothing."""
    env = dict(os.environ if base is None else base)
    env[NO_AUTO] = '1'
    return env


def load_asi(session, asi_path, install=True, timeout=20.0) -> dict:
    """Module.load the asi into an attached session; call InjectHooks when install.

    Raises RuntimeError on a load error or timeout. Returns the agent's record
    (base, path, preloaded, installed)."""
    src = (_LOAD_JS.replace('%PATH%', json.dumps(str(asi_path)))
                   .replace('%INSTALL%', 'true' if install else 'false')
                   .replace('%NAME%', ASI_NAME))
    got = {}

    def on_msg(m, _d):
        if m.get('type') == 'error':
            got['error'] = m.get('description')
        else:
            got.update(m.get('payload') or {})
        got['_done'] = True

    scr = session.create_script(src)
    scr.on('message', on_msg)
    scr.load()
    t0 = time.time()
    while not got.get('_done') and time.time() - t0 < timeout:
        time.sleep(0.02)
    try:
        scr.unload()
    except Exception:
        pass
    if not got.get('_done'):
        raise RuntimeError(f'dev_asi: no reply from load agent within {timeout}s')
    if got.get('error'):
        raise RuntimeError(f'dev_asi: load of {asi_path} failed: {got["error"]}')
    got.pop('_done', None)
    return got


class SpawnedProc:
    """The subset of subprocess.Popen the spawners use, over a frida-spawned pid."""

    def __init__(self, pid):
        import psutil
        self.pid = pid
        self.returncode = None
        try:
            self._p = psutil.Process(pid)
        except psutil.NoSuchProcess:
            self._p = None

    def poll(self):
        if self.returncode is not None or self._p is None:
            return self.returncode if self._p is not None else (self.returncode or 0)
        import psutil
        try:
            self.returncode = self._p.wait(timeout=0)
        except psutil.TimeoutExpired:
            return None
        except psutil.NoSuchProcess:
            self.returncode = 0
        return self.returncode

    def wait(self, timeout=None):
        import subprocess
        import psutil
        if self._p is None:
            return 0
        try:
            self.returncode = self._p.wait(timeout=timeout)
        except psutil.TimeoutExpired:
            raise subprocess.TimeoutExpired(str(self.pid), timeout)
        except psutil.NoSuchProcess:
            self.returncode = 0
        return self.returncode

    def kill(self):
        if self._p is None:
            return
        import psutil
        try:
            self._p.kill()
        except psutil.NoSuchProcess:
            pass

    terminate = kill


def spawn_original(mashed_exe, hooks=True, env=None, argv=None, cwd=None,
                   device=None, asi_path=None, resume=True, keep_session=True,
                   priority_class=None):
    """Spawn MASHED.exe suspended, load the dev asi before it runs, resume.

    hooks  True: Module.load + InjectHooks before the first instruction of
           MASHED.exe. False: plain stock spawn (asi never mapped).
    env    full environment (default os.environ); MASHED_RE_NO_AUTO_HOOK=1 is
           forced in either case.
    priority_class  e.g. 0x8000 (ABOVE_NORMAL), what the Popen callers passed in
           creationflags; set on the pid before resume.
    Returns (proc, session, info): proc is a SpawnedProc, session the frida
    session (None when keep_session=False), info the load record or {} for
    hooks=False. The caller owns the pid: kill only this one."""
    mashed_exe = Path(mashed_exe)
    device = device or frida.get_local_device()
    envp = stock_env(env)
    program = str(mashed_exe)
    pid = device.spawn([program] + list(argv or []), envp=envp,
                       cwd=str(cwd or mashed_exe.parent))
    proc = SpawnedProc(pid)
    info = {}
    session = None
    try:
        session = device.attach(pid)
        if hooks:
            asi = Path(asi_path) if asi_path else resolve_asi(mashed_exe)
            if not asi.exists():
                raise RuntimeError(f'dev_asi: {asi} not found (build mashedmod first)')
            info = load_asi(session, asi, install=True)
            if info.get('preloaded'):
                raise RuntimeError(f'dev_asi: {ASI_NAME} was already mapped at spawn '
                                   f'({info.get("path")}); refusing a second install')
    except Exception:
        try:
            device.kill(pid)
        except Exception:
            pass
        raise
    if priority_class is not None and proc._p is not None:
        try:
            proc._p.nice(priority_class)
        except Exception:
            pass
    if resume:
        device.resume(pid)
    if not keep_session and session is not None:
        try:
            session.detach()
        except Exception:
            pass
        session = None
    return proc, session, info


def _boot_cli(argv):
    """`py -3.12 re/frida/dev_asi.py boot [--seconds N] [--no-hooks] [--exe PATH]`

    For the .ps1 boot bisects (scripts/bisect_asi_boot.ps1, bisect_hook_index.ps1):
    spawn the original with the asi loaded before resume, detach Frida (the inline
    JMPs stay), wait N seconds, then print one line and exit:
        BOOTS <t>              alive at the threshold (this pid is killed), exit 0
        DIED <t> 0x<code>      exited first with that code, exit 1
    The MASHED_HOOK_* env of the caller reaches InstallAll unchanged."""
    import argparse
    ap = argparse.ArgumentParser(prog='dev_asi.py boot')
    ap.add_argument('--seconds', type=float, default=20.0)
    ap.add_argument('--no-hooks', action='store_true')
    ap.add_argument('--exe', default=None)
    a = ap.parse_args(argv)
    root = Path(__file__).resolve().parents[2]
    exe = Path(a.exe) if a.exe else original_exe(root / 'original' / 'MASHED.exe')
    proc, _s, _i = spawn_original(exe, hooks=not a.no_hooks, keep_session=False)
    t0 = time.time()
    try:
        code = proc.wait(timeout=a.seconds)
    except Exception:
        code = None
    t = round(time.time() - t0, 2)
    if code is None:
        proc.kill()
        print(f'BOOTS {t}', flush=True)
        return 0
    print(f'DIED {t} 0x{code & 0xFFFFFFFF:08X}', flush=True)
    return 1


if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == 'boot':
        sys.exit(_boot_cli(sys.argv[2:]))
    sys.exit('usage: dev_asi.py boot [--seconds N] [--no-hooks] [--exe PATH]')
