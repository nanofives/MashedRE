#!/usr/bin/env py -3.12
# mashed_lock_concurrency_test.py — multi-process hammer for mashed_lock.py (HB-MASHEDLOCK-RACE).
# Stdlib only, no MASHED.exe. Uses a private MASHED_LOCK_DIR temp dir, never the live files.
#
#   py -3.12 re/orchestrator/mashed_lock_concurrency_test.py [--procs 8] [--secs 12] [--impl <mashed_lock.py>]
#
# Workers loop over: game-lock acquire/hold/release, status reads (solo_owner, prio_pending),
# solo windows (short ttl so expiry removal races, some left to expire), a solo file whose
# owner PID is dead (dead-owner removal race), and a half-written solo file (create, pause,
# write). Fails (exit 1) on any worker exception, any overlap of the game-lock critical
# section, or a hung worker. --impl runs the same hammer against another copy of mashed_lock.py
# (baseline: `git show orch/integration:re/orchestrator/mashed_lock.py > old.py`). The impl
# uses MASHED_LOCK_DIR, or the redirected TEMP dir for copies that predate it; one that still
# resolves elsewhere is refused instead of touching live files.
import importlib.util
import json
import multiprocessing as mp
import os
import random
import shutil
import sys
import tempfile
import time
import traceback

HERE = os.path.dirname(os.path.abspath(__file__))


def _load(impl):
    spec = importlib.util.spec_from_file_location('mashed_lock_under_test', impl)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


def worker(idx, lockdir, impl, secs, q):
    os.environ['MASHED_LOCK_DIR'] = lockdir
    for v in ('TEMP', 'TMP', 'TMPDIR'):   # baseline copies lack MASHED_LOCK_DIR: they use the temp dir
        os.environ[v] = lockdir
    tempfile.tempdir = lockdir
    stats = {'acq': 0, 'solo': 0, 'status': 0, 'timeouts': 0, 'overlap': 0, 'errors': []}
    try:
        ml = _load(impl)
        if os.path.abspath(ml.LOCK_DIR) != os.path.abspath(lockdir):
            stats['errors'].append(f'impl does not lock inside the private dir (LOCK_DIR={ml.LOCK_DIR}); refusing')
            q.put((idx, stats))
            return
    except Exception:
        stats['errors'].append('load: ' + traceback.format_exc(limit=4))
        q.put((idx, stats))
        return
    name = ml.DEFAULT_NAME
    rng = random.Random(idx * 7919 + int(time.time()))
    inside = os.path.join(lockdir, 'inside.marker')
    end = time.time() + secs
    while time.time() < end:
        op = rng.choice(['acq', 'acq', 'status', 'status', 'solo', 'solo_dead', 'solo_half'])
        try:
            if op == 'acq':
                with ml.MashedLock(f'w{idx}', wait=20, poll=0.005, quiet=True):
                    try:
                        fd = os.open(inside, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                        os.close(fd)
                    except FileExistsError:
                        stats['overlap'] += 1
                    time.sleep(rng.random() * 0.01)
                    try:
                        os.remove(inside)
                    except OSError:
                        pass
                stats['acq'] += 1
            elif op == 'status':
                ml.solo_owner(name)
                ml.prio_pending(name)
                ml._read_owner(ml._lock_path(name))
                stats['status'] += 1
            elif op == 'solo':
                tok = ml.solo_begin(f'w{idx}', name=name, ttl=rng.choice([1, 2, 30]), wait=20,
                                    poll=0.005, quiet=True)
                time.sleep(rng.random() * 0.02)
                if rng.random() < 0.7:   # else leave it to expire: races the expiry removal
                    ml.solo_end(tok, name=name, quiet=True)
                stats['solo'] += 1
            elif op == 'solo_dead':
                p = ml._solo_path(name)
                try:
                    fd = os.open(p, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                    os.write(fd, json.dumps({'token': 'dead%d' % idx, 'pid': 99999999,
                                             'expires': int(time.time() + 30)}).encode())
                    os.close(fd)
                except FileExistsError:
                    pass
            else:   # solo_half: create, pause, write (a reader sees an empty file)
                p = ml._solo_path(name)
                try:
                    fd = os.open(p, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
                except FileExistsError:
                    continue
                time.sleep(rng.random() * 0.05)
                try:
                    os.write(fd, json.dumps({'token': 'half%d' % idx, 'pid': None,
                                             'expires': int(time.time() + 1)}).encode())
                finally:
                    os.close(fd)
        except TimeoutError:
            stats['timeouts'] += 1
        except Exception:
            stats['errors'].append(f'{op}: ' + traceback.format_exc(limit=4))
            if len(stats['errors']) >= 3:
                break
    q.put((idx, stats))


def main():
    a = sys.argv[1:]

    def opt(f, d):
        return a[a.index(f) + 1] if f in a else d
    procs, secs = int(opt('--procs', 8)), float(opt('--secs', 12))
    impl = os.path.abspath(opt('--impl', os.path.join(HERE, 'mashed_lock.py')))
    lockdir = tempfile.mkdtemp(prefix='mashed_lock_conc_')
    q = mp.Queue()
    ps = [mp.Process(target=worker, args=(i, lockdir, impl, secs, q)) for i in range(procs)]
    for p in ps:
        p.start()
    results = []
    deadline = time.time() + secs + 90
    while len(results) < procs and time.time() < deadline:
        try:
            results.append(q.get(timeout=1))
        except Exception:
            pass
    for p in ps:
        p.join(5)
        if p.is_alive():
            p.terminate()
    tot = {k: sum(r[1][k] for r in results) for k in ('acq', 'solo', 'status', 'timeouts', 'overlap')}
    errs = [e for r in results for e in r[1]['errors']]
    hung = procs - len(results)
    print(f"impl={impl}\nprocs={procs} secs={secs} lockdir={lockdir}\n"
          f"acquires={tot['acq']} solo_windows={tot['solo']} status_reads={tot['status']} "
          f"timeouts={tot['timeouts']} overlaps={tot['overlap']} errors={len(errs)} hung_workers={hung}")
    for e in errs[:3]:
        print(e)
    ok = not errs and not tot['overlap'] and not hung and tot['acq'] > 0
    print('PASS' if ok else 'FAIL')
    shutil.rmtree(lockdir, ignore_errors=True)
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
