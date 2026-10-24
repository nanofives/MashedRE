#!/usr/bin/env py -3.12
# mashed_lock.py — machine-wide MASHED run queue.
#
# THE PROBLEM: MASHED / the d3d9 shim / the GPU are a SINGLE shared resource.
# Two processes booting MASHED at once contend (measured ~1.12x, and
# simultaneous frida.spawn collides). When several orchestrator lanes or
# independent child sessions each want to run the game, they must take turns.
#
# THE FIX: a cross-process, cross-language file lock any game-spawning script
# ACQUIRES before frida.spawn and RELEASES after it kills its pid. If the lock
# is held, acquire() BLOCKS (polls) until the holder finishes — i.e. a FIFO-ish
# wait queue: instance 2 waits until instance 1 is done, then proceeds.
#
# Machine-global (lives in the user temp dir, not the repo) so it serializes
# across worktrees and across separate Claude sessions on the same machine.
# Same lock file + JSON schema as MachineLock.ps1, so a PowerShell spawner and a
# Python spawner queue against each other.
#
# Stale-safe: if the holder PID is dead, the lock is broken and reclaimed. A
# reused PID is bounded by a hard max-age break.
#
# Usage (import — the normal path):
#   from mashed_lock import MashedLock
#   with MashedLock('state_batch'):      # blocks here until it's our turn
#       pid = dev.spawn(...); ...; dev.kill(pid)
#
# Usage (CLI):
#   py -3.12 mashed_lock.py status
#   py -3.12 mashed_lock.py break          # force-remove a stale lock
#   py -3.12 mashed_lock.py run --label X -- <command...>   # hold while running
#
# EXCLUSIVE (SOLO) MODE, added 2026-10-24 for the roadmap loop (wall-clock-bound
# steps such as D2 arms, memory d2-arms-are-wall-clock-bound). A solo window is a
# second file, <name>_solo.lock, that spans MANY game boots. While it is open,
# every acquire() of <name> waits, EXCEPT processes that carry the window's token
# in env MASHED_LOCK_SOLO_TOKEN (children of the solo holder inherit it). The
# solo holder still takes <name> normally for each boot, so its own boots queue.
# Callers that know nothing about solo mode keep working unchanged: they wait
# while a window is open, and that wait is bounded by the window's expiry, not by
# their own `wait`.
#
#   py -3.12 mashed_lock.py run --solo --label d2arms [--ttl S] -- <command...>
#       open a window for the command's lifetime (bound to this PID), export the
#       token to the command, drain the current game holder first
#   py -3.12 mashed_lock.py solo-begin --label X [--ttl 14400]   -> "TOKEN <hex>"
#   py -3.12 mashed_lock.py solo-end --token <hex>
#       session-scoped form (no live PID to bind to, so it expires at --ttl)

import json
import os
import socket
import sys
import time
import tempfile
import uuid

LOCK_DIR = tempfile.gettempdir()
DEFAULT_NAME = 'mashed_machine'
MAX_AGE_SEC = 30 * 60   # a lock older than this whose PID looks alive is still broken
SOLO_ENV = 'MASHED_LOCK_SOLO_TOKEN'
SOLO_TTL_SEC = 4 * 3600  # default solo window when not bound to a live PID


def _lock_path(name):
    return os.path.join(LOCK_DIR, name + '.lock')


def _now_iso():
    return time.strftime('%Y-%m-%dT%H:%M:%S', time.localtime())


def _pid_alive(pid):
    if not pid:
        return False
    if pid == os.getpid():
        return True
    try:
        import psutil
        return psutil.pid_exists(int(pid))
    except Exception:
        pass
    try:
        import ctypes
        PROCESS_QUERY_LIMITED = 0x1000
        h = ctypes.windll.kernel32.OpenProcess(PROCESS_QUERY_LIMITED, False, int(pid))
        if h:
            ctypes.windll.kernel32.CloseHandle(h)
            return True
        return False
    except Exception:
        return True   # can't tell -> assume alive (safer: wait, don't steal)


def _read_owner(path):
    try:
        with open(path, 'r', encoding='utf-8') as f:
            return json.loads(f.read() or '{}')
    except Exception:
        return {}


def _solo_path(name):
    return os.path.join(LOCK_DIR, name + '_solo.lock')


def solo_owner(name=DEFAULT_NAME):
    """The open solo window for `name` as a dict, or None. Breaks a stale one
    (bound PID dead, or past its expiry)."""
    path = _solo_path(name)
    if not os.path.exists(path):
        return None
    o = _read_owner(path)
    dead = bool(o.get('pid')) and not _pid_alive(o.get('pid'))
    expired = (o.get('expires') or 0) < time.time()
    if not o or dead or expired:
        try:
            os.remove(path)
        except FileNotFoundError:
            pass
        return None
    return o


def _solo_admits_me(owner):
    return owner is not None and os.environ.get(SOLO_ENV) == owner.get('token')


def solo_begin(label='', name=DEFAULT_NAME, ttl=SOLO_TTL_SEC, pid=None, wait=1800,
               poll=3.0, quiet=False):
    """Open a solo window and return its token. Waits while another window is
    open, then drains the current <name> holder so no foreign boot overlaps."""
    def log(m):
        if not quiet:
            print(f"[mashed-lock] {m}", flush=True)
    path = _solo_path(name)
    token = uuid.uuid4().hex
    start = time.time()
    noted = False
    while True:
        try:
            fd = os.open(path, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            os.write(fd, json.dumps({
                'token': token, 'pid': pid, 'at': _now_iso(), 'epoch': int(time.time()),
                'expires': int(time.time() + ttl), 'label': label,
                'host': socket.gethostname(),
            }).encode('utf-8'))
            os.close(fd)
            break
        except FileExistsError:
            o = solo_owner(name)
            if o is None:
                continue
            if time.time() - start > wait:
                raise TimeoutError(f"waited {wait}s for solo '{name}' held by "
                                   f"({o.get('label')}) since {o.get('at')}")
            if not noted:
                log(f"WAITING for solo '{name}' held by ({o.get('label')})")
                noted = True
            time.sleep(poll)
    log(f"SOLO OPEN '{name}' ({label}) ttl={ttl}s; draining the current holder")
    gpath = _lock_path(name)
    while os.path.exists(gpath):
        go = _read_owner(gpath)
        gage = time.time() - (go.get('epoch') or 0)
        if not go or (not _pid_alive(go.get('pid'))) or gage > MAX_AGE_SEC:
            try:
                os.remove(gpath)
            except FileNotFoundError:
                pass
            break
        time.sleep(poll)
    log(f"SOLO HELD '{name}'")
    return token


def solo_end(token, name=DEFAULT_NAME, quiet=False):
    path = _solo_path(name)
    o = _read_owner(path) if os.path.exists(path) else {}
    ok = bool(o) and o.get('token') == token
    if ok:
        try:
            os.remove(path)
        except FileNotFoundError:
            pass
    if not quiet:
        print(f"[mashed-lock] {'SOLO CLOSED' if ok else 'solo not held by that token:'} "
              f"'{name}'", flush=True)
    return ok


class MashedLock:
    def __init__(self, label='', name=DEFAULT_NAME, wait=1800, poll=3.0, quiet=False):
        self.label = label
        self.name = name
        self.wait = wait
        self.poll = poll
        self.quiet = quiet
        self.path = _lock_path(name)
        self._held = False

    def _log(self, msg):
        if not self.quiet:
            print(f"[mashed-lock] {msg}", flush=True)

    def acquire(self):
        start = time.time()
        waited_note = False
        solo_note = False
        while True:
            so = solo_owner(self.name)
            if so is not None and not _solo_admits_me(so):
                if not solo_note:
                    self._log(f"WAITING — solo window '{self.name}' open "
                              f"({so.get('label')}) since {so.get('at')}")
                    solo_note = True
                time.sleep(self.poll)
                start = time.time()   # bounded by the solo expiry, not by self.wait
                continue
            try:
                fd = os.open(self.path, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                os.write(fd, json.dumps({
                    'pid': os.getpid(), 'at': _now_iso(), 'epoch': int(time.time()),
                    'label': self.label, 'host': socket.gethostname(),
                }).encode('utf-8'))
                os.close(fd)
                self._held = True
                self._log(f"HELD '{self.name}' pid={os.getpid()} ({self.label})")
                return self
            except FileExistsError:
                owner = _read_owner(self.path)
                opid = owner.get('pid')
                age = time.time() - (owner.get('epoch') or 0)
                stale = (not _pid_alive(opid)) or (age > MAX_AGE_SEC)
                if stale:
                    self._log(f"breaking STALE '{self.name}' (pid={opid} alive="
                              f"{_pid_alive(opid)} age={int(age)}s)")
                    try:
                        os.remove(self.path)
                    except FileNotFoundError:
                        pass
                    continue
                if time.time() - start > self.wait:
                    raise TimeoutError(
                        f"waited {self.wait}s for '{self.name}' held by pid={opid} "
                        f"({owner.get('label')}) since {owner.get('at')}")
                if not waited_note:
                    self._log(f"WAITING — '{self.name}' held by pid={opid} "
                              f"({owner.get('label')}); queueing...")
                    waited_note = True
                time.sleep(self.poll)

    def release(self):
        if not self._held:
            return
        owner = _read_owner(self.path)
        if owner.get('pid') == os.getpid():
            try:
                os.remove(self.path)
            except FileNotFoundError:
                pass
        self._held = False
        self._log(f"released '{self.name}' pid={os.getpid()}")

    def __enter__(self):
        return self.acquire()

    def __exit__(self, *exc):
        self.release()
        return False


def _cli():
    args = sys.argv[1:]
    cmd = args[0] if args else 'status'
    name = DEFAULT_NAME
    if '--name' in args:
        name = args[args.index('--name') + 1]
    path = _lock_path(name)

    def opt(flag, default=None):
        return args[args.index(flag) + 1] if flag in args else default
    if cmd == 'status':
        if os.path.exists(path):
            o = _read_owner(path)
            alive = _pid_alive(o.get('pid'))
            print(f"HELD '{name}' pid={o.get('pid')} alive={alive} label={o.get('label')} "
                  f"since={o.get('at')}  ({path})")
        else:
            print(f"free '{name}'  ({path})")
        so = solo_owner(name)
        if so:
            left = int((so.get('expires') or 0) - time.time())
            print(f"SOLO '{name}' label={so.get('label')} pid={so.get('pid')} "
                  f"since={so.get('at')} expires_in={left}s")
        return 0
    if cmd == 'solo-begin':
        tok = solo_begin(label=opt('--label', 'cli'), name=name,
                         ttl=int(opt('--ttl', SOLO_TTL_SEC)), wait=int(opt('--wait', 1800)))
        print(f"TOKEN {tok}")
        return 0
    if cmd == 'solo-end':
        if not opt('--token'):
            sys.exit("solo-end needs --token <hex>")
        return 0 if solo_end(opt('--token'), name=name) else 1
    if cmd == 'break':
        if os.path.exists(path):
            os.remove(path)
            print(f"broke '{name}'")
        else:
            print(f"'{name}' already free")
        return 0
    if cmd == 'run':
        label = args[args.index('--label') + 1] if '--label' in args else 'cli'
        wait = int(args[args.index('--wait') + 1]) if '--wait' in args else 1800
        if '--' not in args:
            sys.exit("run needs: --label X [--wait N] -- <command...>")
        sub = args[args.index('--') + 1:]
        import subprocess
        if '--solo' in args[:args.index('--')]:
            tok = solo_begin(label=label, name=name, pid=os.getpid(),
                             ttl=int(opt('--ttl', 12 * 3600)), wait=wait)
            env = dict(os.environ)
            env[SOLO_ENV] = tok
            try:
                return subprocess.call(sub, env=env)
            finally:
                solo_end(tok, name=name)
        with MashedLock(label=label, name=name, wait=wait):
            return subprocess.call(sub)
    sys.exit(f"unknown command '{cmd}' (status|break|run|solo-begin|solo-end)")


if __name__ == '__main__':
    sys.exit(_cli())
