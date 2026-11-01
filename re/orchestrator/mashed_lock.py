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
# reused PID is caught by the holder's recorded process start time (owner_start).
# A live holder is NEVER broken for age (HB-LOCKBREAK, 2026-10-25: the old 30 min
# max-age rule broke D3-CF-K357's 11-arm hold, pid 18904 alive, at age 1801 s).
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
# PRIORITY REQUESTS, added 2026-10-24 (HB-LOCKPRIO). A priority acquirer (the merge
# train) writes a pending marker <name>_prio_<pid>.lock before it waits. While any live,
# unexpired marker of another process exists, ordinary acquires WAIT even if the lock is
# free, so a worker that releases between arms cannot re-take it ahead of the train. A
# worker already holding the lock is never preempted. The marker goes away when the
# priority acquirer takes the lock (or gives up, or dies: dead PID or expiry = stale).
# Priority acquirers do not wait for each other's markers (they queue on the lock itself),
# and a process admitted by a solo token bypasses markers (the window already excludes
# others, and waiting would deadlock the train against that window).
#   MashedLock('x', priority=1)                         import form
#   mashed_lock.py run [--solo] --priority --label X -- <cmd>     CLI form
#   mashed_lock.py solo-begin --priority --label X                CLI form
# Default priority 0 = every existing call site and CLI form, unchanged.
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
#       --owner-pid <pid>: HB-SOLOSTALE, the token dies as soon as that process is gone
#       (start time recorded against PID reuse); --ttl stays the fallback
#   py -3.12 mashed_lock.py solo-end --token <hex>
#       session-scoped form (no live PID to bind to, so it expires at --ttl)

import glob
import json
import os
import socket
import sys
import time
import tempfile
import uuid

# MASHED_LOCK_DIR redirects every lock/solo/priority file (HB-MASHEDLOCK-RACE: the
# concurrency self-test must not touch the live files other sessions hold).
LOCK_DIR = os.environ.get('MASHED_LOCK_DIR') or tempfile.gettempdir()
DEFAULT_NAME = 'mashed_machine'
EMPTY_GRACE_SEC = 10     # an empty/unparsable lock file younger than this is a writer mid-create
SOLO_ENV = 'MASHED_LOCK_SOLO_TOKEN'
SOLO_TTL_SEC = 4 * 3600  # default solo window when not bound to a live PID
REMOVE_TRIES = 8         # HB-MASHEDLOCK-RACE: Windows sharing violations on remove clear in ms
PRIO_GRACE_SEC = 300     # a priority marker outlives its requester's wait by this much


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


def _pid_start(pid):
    """Creation time (epoch seconds) of a live PID, or None when it cannot be read.
    Guards PID reuse: a token whose recorded start differs from the live process's is dead."""
    if not pid:
        return None
    try:
        import psutil
        return round(psutil.Process(int(pid)).create_time(), 1)
    except Exception:
        pass
    try:
        import ctypes
        from ctypes import wintypes
        h = ctypes.windll.kernel32.OpenProcess(0x1000, False, int(pid))
        if not h:
            return None
        try:
            c, e, k, u = (wintypes.FILETIME() for _ in range(4))
            if not ctypes.windll.kernel32.GetProcessTimes(h, ctypes.byref(c), ctypes.byref(e),
                                                          ctypes.byref(k), ctypes.byref(u)):
                return None
            ticks = (c.dwHighDateTime << 32) | c.dwLowDateTime
            return round(ticks / 1e7 - 11644473600, 1)
        finally:
            ctypes.windll.kernel32.CloseHandle(h)
    except Exception:
        return None


def _owner_dead(o):
    """HB-SOLOSTALE: True when the token's owning process (bound `pid`, else `owner_pid`)
    is gone, or the PID now belongs to a process that started after the token was written."""
    pid = o.get('pid') or o.get('owner_pid')
    if not pid:
        return False
    if not _pid_alive(pid):
        return True
    rec = o.get('owner_start')
    now = _pid_start(pid)
    return bool(rec) and now is not None and abs(now - rec) > 2.0


def _holder_stale(path, o):
    """HB-LOCKBREAK: True when the game-lock record at `path` (parsed as `o`) has no live
    holder. Liveness alone decides, never age. PID reuse is caught by `owner_start`
    (records written before HB-LOCKBREAK lack it, so a live PID keeps them held).
    An empty or unparsable file is a writer between its O_EXCL create and its write
    until EMPTY_GRACE_SEC has passed."""
    if not o:
        try:
            return time.time() - os.path.getmtime(path) > EMPTY_GRACE_SEC
        except OSError:
            return True
    if not o.get('pid'):
        return True
    return _owner_dead(o)


def _read_owner(path):
    try:
        with open(path, 'r', encoding='utf-8') as f:
            return json.loads(f.read() or '{}')
    except Exception:
        return {}


def _remove(path, tries=REMOVE_TRIES):
    """HB-MASHEDLOCK-RACE: remove `path`, tolerating the races between concurrent
    readers/removers. True when the file is gone (including already gone), False when
    it is still there after `tries` attempts (Windows PermissionError/WinError 32 while
    another process has it open, or any other OSError). Never raises OSError."""
    for i in range(tries):
        try:
            os.remove(path)
            return True
        except FileNotFoundError:
            return True
        except OSError:
            time.sleep(0.01 * (i + 1))
    return not os.path.exists(path)


def _remove_if_same(path, o):
    """Remove `path` only if it still holds the record `o` we judged stale, so a lock
    re-created by another process between our read and our remove is not broken."""
    if _read_owner(path) != o:
        return False
    return _remove(path)


def _young(path):
    """True when `path` was modified within EMPTY_GRACE_SEC (a writer mid-create)."""
    try:
        return time.time() - os.path.getmtime(path) <= EMPTY_GRACE_SEC
    except OSError:
        return False


def _solo_path(name):
    return os.path.join(LOCK_DIR, name + '_solo.lock')


def _prio_path(name, pid=None):
    return os.path.join(LOCK_DIR, f"{name}_prio_{pid or os.getpid()}.lock")


def prio_pending(name=DEFAULT_NAME, ignore_pid=None):
    """Live priority markers for `name`, as dicts. Stale ones (dead PID or past
    expiry) are removed. `ignore_pid` skips the caller's own marker."""
    out = []
    pat = os.path.join(glob.escape(LOCK_DIR), glob.escape(name) + '_prio_*.lock')
    for path in glob.glob(pat):
        o = _read_owner(path)
        if not o and _young(path):
            continue   # marker being written (prio_mark opens with 'w'), not stale
        if (not o) or (not _pid_alive(o.get('pid'))) or (o.get('expires') or 0) < time.time():
            _remove_if_same(path, o)
            continue
        if ignore_pid is not None and o.get('pid') == ignore_pid:
            continue
        out.append(o)
    return out


def prio_mark(name, label, ttl):
    with open(_prio_path(name), 'w', encoding='utf-8') as f:
        f.write(json.dumps({'pid': os.getpid(), 'at': _now_iso(), 'epoch': int(time.time()),
                            'expires': int(time.time() + ttl), 'label': label,
                            'host': socket.gethostname()}))


def prio_clear(name):
    _remove(_prio_path(name))


def solo_owner(name=DEFAULT_NAME):
    """The open solo window for `name` as a dict, or None. Breaks a stale one
    (bound PID dead, or past its expiry)."""
    path = _solo_path(name)
    if not os.path.exists(path):
        return None
    o = _read_owner(path)
    if not o:
        # HB-MASHEDLOCK-RACE: empty/unparsable/unreadable. A writer between its O_EXCL
        # create and its write looks exactly like this, so it is a busy window, not a
        # stale one, until EMPTY_GRACE_SEC has passed.
        if _young(path):
            return dict(_BUSY_SOLO)
    dead = _owner_dead(o)
    expired = (o.get('expires') or 0) < time.time()
    if not o or dead or expired:
        if _remove_if_same(path, o):
            return None
        if not os.path.exists(path):
            return None
        return dict(_BUSY_SOLO)   # could not remove (sharing violation): retry, not stale
    return o


_BUSY_SOLO = {'token': None, 'label': '(solo window busy or unreadable)', 'at': '?'}


def _solo_admits_me(owner):
    tok = owner.get('token') if owner else None
    return bool(tok) and os.environ.get(SOLO_ENV) == tok


def solo_begin(label='', name=DEFAULT_NAME, ttl=SOLO_TTL_SEC, pid=None, wait=1800,
               poll=3.0, quiet=False, priority=0, owner_pid=None):
    """Open a solo window and return its token. Waits while another window is
    open, then drains the current <name> holder so no foreign boot overlaps.
    priority=1 holds a pending-priority marker for the duration of that wait."""
    if priority:
        prio_mark(name, label, wait + PRIO_GRACE_SEC + 300)
    try:
        return _solo_begin(label, name, ttl, pid, wait, poll, quiet, owner_pid)
    finally:
        if priority:
            prio_clear(name)


def _solo_begin(label, name, ttl, pid, wait, poll, quiet, owner_pid=None):
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
                'owner_pid': owner_pid, 'owner_start': _pid_start(owner_pid),
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
        if _holder_stale(gpath, go):
            if _remove_if_same(gpath, go) or not os.path.exists(gpath):
                break
        time.sleep(poll)
    log(f"SOLO HELD '{name}'")
    return token


def solo_end(token, name=DEFAULT_NAME, quiet=False):
    path = _solo_path(name)
    o = _read_owner(path) if os.path.exists(path) else {}
    ok = bool(o) and o.get('token') == token
    if ok:
        _remove(path)
    if not quiet:
        print(f"[mashed-lock] {'SOLO CLOSED' if ok else 'solo not held by that token:'} "
              f"'{name}'", flush=True)
    return ok


class MashedLock:
    def __init__(self, label='', name=DEFAULT_NAME, wait=1800, poll=3.0, quiet=False,
                 priority=0):
        self.priority = priority
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
        if not self.priority:
            return self._acquire()
        prio_mark(self.name, self.label, self.wait + PRIO_GRACE_SEC)
        try:
            return self._acquire()
        finally:
            prio_clear(self.name)

    def _acquire(self):
        start = time.time()
        waited_note = False
        solo_note = False
        prio_note = False
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
            if not self.priority and not _solo_admits_me(so):
                pend = prio_pending(self.name, ignore_pid=os.getpid())
                if pend:
                    if not prio_note:
                        self._log(f"WAITING — priority request pending for '{self.name}' "
                                  f"({pend[0].get('label')}) since {pend[0].get('at')}")
                        prio_note = True
                    time.sleep(self.poll)
                    start = time.time()   # bounded by the marker expiry, not by self.wait
                    continue
            try:
                fd = os.open(self.path, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                os.write(fd, json.dumps({
                    'pid': os.getpid(), 'at': _now_iso(), 'epoch': int(time.time()),
                    'label': self.label, 'host': socket.gethostname(),
                    'owner_start': _pid_start(os.getpid()),
                }).encode('utf-8'))
                os.close(fd)
                self._held = True
                self._log(f"HELD '{self.name}' pid={os.getpid()} ({self.label})")
                return self
            except FileExistsError:
                owner = _read_owner(self.path)
                opid = owner.get('pid')
                age = time.time() - (owner.get('epoch') or 0)
                if _holder_stale(self.path, owner):
                    self._log(f"breaking STALE '{self.name}' (pid={opid} alive="
                              f"{_pid_alive(opid)} age={int(age)}s)")
                    if _remove_if_same(self.path, owner) or not os.path.exists(self.path):
                        continue
                    time.sleep(self.poll)   # could not remove right now: retry, do not spin
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
        for _ in range(REMOVE_TRIES):   # a transient unreadable read must not leak our lock
            if owner or not os.path.exists(self.path):
                break
            time.sleep(0.02)
            owner = _read_owner(self.path)
        if owner.get('pid') == os.getpid():
            _remove(self.path)
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
        for pm in prio_pending(name):
            print(f"PRIORITY-PENDING '{name}' pid={pm.get('pid')} label={pm.get('label')} "
                  f"since={pm.get('at')}")
        so = solo_owner(name)
        if so:
            left = int((so.get('expires') or 0) - time.time())
            print(f"SOLO '{name}' label={so.get('label')} pid={so.get('pid')} owner_pid={so.get('owner_pid')} "
                  f"since={so.get('at')} expires_in={left}s")
        return 0
    if cmd == 'solo-begin':
        tok = solo_begin(label=opt('--label', 'cli'), name=name,
                         ttl=int(opt('--ttl', SOLO_TTL_SEC)), wait=int(opt('--wait', 1800)),
                         priority=1 if '--priority' in args else 0,
                         owner_pid=int(opt('--owner-pid')) if opt('--owner-pid') else None)
        print(f"TOKEN {tok}")
        return 0
    if cmd == 'solo-end':
        if not opt('--token'):
            sys.exit("solo-end needs --token <hex>")
        return 0 if solo_end(opt('--token'), name=name) else 1
    if cmd == 'break':
        if os.path.exists(path):
            print(f"broke '{name}'" if _remove(path) else f"could not remove '{name}' (in use)")
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
        prio = 1 if '--priority' in args[:args.index('--')] else 0
        if '--solo' in args[:args.index('--')]:
            tok = solo_begin(label=label, name=name, pid=os.getpid(),
                             ttl=int(opt('--ttl', 12 * 3600)), wait=wait, priority=prio)
            env = dict(os.environ)
            env[SOLO_ENV] = tok
            try:
                return subprocess.call(sub, env=env)
            finally:
                solo_end(tok, name=name)
        with MashedLock(label=label, name=name, wait=wait, priority=prio):
            return subprocess.call(sub)
    sys.exit(f"unknown command '{cmd}' (status|break|run|solo-begin|solo-end)")


if __name__ == '__main__':
    sys.exit(_cli())
