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

import json
import os
import socket
import sys
import time
import tempfile

LOCK_DIR = tempfile.gettempdir()
DEFAULT_NAME = 'mashed_machine'
MAX_AGE_SEC = 30 * 60   # a lock older than this whose PID looks alive is still broken


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
        while True:
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
    if cmd == 'status':
        if os.path.exists(path):
            o = _read_owner(path)
            alive = _pid_alive(o.get('pid'))
            print(f"HELD '{name}' pid={o.get('pid')} alive={alive} label={o.get('label')} "
                  f"since={o.get('at')}  ({path})")
        else:
            print(f"free '{name}'  ({path})")
        return 0
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
        with MashedLock(label=label, name=name, wait=wait):
            return subprocess.call(sub)
    sys.exit(f"unknown command '{cmd}' (status|break|run)")


if __name__ == '__main__':
    sys.exit(_cli())
