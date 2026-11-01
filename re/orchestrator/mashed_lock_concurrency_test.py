#!/usr/bin/env py -3.12
# mashed_lock_concurrency_test.py — multi-process hammer for mashed_lock.py (HB-MASHEDLOCK-RACE).
# Stdlib only, no MASHED.exe. Uses a private MASHED_LOCK_DIR temp dir, never the live files.
#
#   py -3.12 re/orchestrator/mashed_lock_concurrency_test.py [--procs 8] [--secs 12] [--impl <mashed_lock.py>]
#       [--skip-scenarios] [--scenarios-only]
#
# Before the hammer, deterministic SOLO-PRIORITY scenarios (HB-MASHEDLOCK-SOLO-PRIORITY) run in
# their own private dir, one "[N/M] name PASS|FAIL" line each: an open window survives a pending
# priority claim, a new solo open waits while the claim is pending and through the linger after
# its release, ordinary acquires ignore the linger, a priority solo open does not wait on another
# priority marker, and both default expiries (solo_begin, `run --solo`) are at most 1 h.
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
import subprocess
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
    if hasattr(ml, 'PRIO_SOLO_LINGER_SEC'):
        ml.PRIO_SOLO_LINGER_SEC = 0   # the scenarios cover the linger; here it would only stall solo ops
    rng = random.Random(idx * 7919 + int(time.time()))
    inside = os.path.join(lockdir, 'inside.marker')
    end = time.time() + secs
    while time.time() < end:
        op = rng.choice(['acq', 'acq', 'prio', 'status', 'status', 'solo', 'solo_dead', 'solo_half'])
        try:
            if op in ('acq', 'prio'):
                with ml.MashedLock(f'w{idx}', wait=20, poll=0.005, quiet=True,
                                   priority=1 if op == 'prio' else 0):
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


SC_CHILD = (
    "import importlib.util,os,sys,time\n"
    "sp=importlib.util.spec_from_file_location('mlx',%r);ml=importlib.util.module_from_spec(sp);sp.loader.exec_module(ml)\n"
    "assert os.path.abspath(ml.LOCK_DIR)==os.path.abspath(%r)\n"
    "ev=%r\n"
    "def log(t): open(ev,'a').write('%%s %%.3f\\n'%%(t,time.time()))\n"
)


def _sc_child(impl, lockdir, ev, body, env_extra=None):
    env = dict(os.environ, MASHED_LOCK_DIR=lockdir)
    env.pop('MASHED_LOCK_SOLO_TOKEN', None)
    env.update(env_extra or {})
    return subprocess.Popen([sys.executable, '-c', SC_CHILD % (impl, lockdir, ev) + body], env=env)


def _sc_events(ev):
    out = {}
    if os.path.exists(ev):
        for ln in open(ev):
            p = ln.split()
            if len(p) == 2:
                out.setdefault(p[0], float(p[1]))
    return out


def _wait_for(pred, timeout=20.0):
    t0 = time.time()
    while time.time() - t0 < timeout:
        if pred():
            return True
        time.sleep(0.05)
    return False


def scenarios(impl):
    """HB-MASHEDLOCK-SOLO-PRIORITY deterministic cases. Returns the number of FAILs."""
    lockdir = tempfile.mkdtemp(prefix='mashed_lock_solo_prio_')
    os.environ['MASHED_LOCK_DIR'] = lockdir
    os.environ.pop('MASHED_LOCK_SOLO_TOKEN', None)
    ml = _load(impl)
    if os.path.abspath(ml.LOCK_DIR) != os.path.abspath(lockdir):
        print(f'impl does not lock inside the private dir (LOCK_DIR={ml.LOCK_DIR}); refusing')
        return 1
    name = ml.DEFAULT_NAME
    ev = os.path.join(lockdir, 'events.txt')
    TOTAL = 9
    res = []
    kids = []

    def check(nm, ok, detail=''):
        res.append(bool(ok))
        print(f"[{len(res)}/{TOTAL}] {nm} {'PASS' if ok else 'FAIL'} {detail}".rstrip(), flush=True)

    def kid(body, env_extra=None):
        k = _sc_child(impl, lockdir, ev, body, env_extra)
        kids.append(k)
        return k

    def prio_kid(tag, hold, linger):
        return kid("ml.PRIO_SOLO_LINGER_SEC=%r\n"
                   "l=ml.MashedLock(%r,poll=0.1,quiet=True,priority=1,wait=60)\n"
                   "l.acquire();log(%r)\ntime.sleep(%r);l.release();log(%r)\n"
                   % (linger, tag, tag + '_GOT', hold, tag + '_DONE'))

    def clear_lingers():
        for f in os.listdir(lockdir):
            if 'priolinger' in f:
                ml._remove(os.path.join(lockdir, f))
    try:
        # 1. a window that is ALREADY open survives a pending priority claim, and its token
        #    holder still gets the game lock past the claim
        tok = ml.solo_begin('W-open', name=name, ttl=60, poll=0.1, quiet=True)
        t1 = prio_kid('T1', 0.2, 0)
        seen = _wait_for(lambda: bool(ml.prio_pending(name)))
        time.sleep(1.0)
        so = ml.solo_owner(name)
        kid("l=ml.MashedLock('W-child',poll=0.1,quiet=True,wait=10)\n"
            "l.acquire();log('WC_GOT');l.release()\n", {'MASHED_LOCK_SOLO_TOKEN': tok})
        wc = _wait_for(lambda: 'WC_GOT' in _sc_events(ev), 10)
        kept = bool(so and so.get('token') == tok)
        check('open-window-survives-pending-priority',
              seen and kept and wc and 'T1_GOT' not in _sc_events(ev),
              f"(marker={seen} window_kept={kept} token_child_got={wc})")
        ml.solo_end(tok, name=name, quiet=True)
        _wait_for(lambda: 'T1_DONE' in _sc_events(ev), 10)
        t1.wait(10)
        time.sleep(0.2)
        clear_lingers()

        # 2-5. a NEW non-priority solo open waits while the claim is pending and through the
        #      linger after the priority holder releases; ordinary acquires ignore the linger
        held = ml.MashedLock('H', poll=0.1, quiet=True, wait=10).acquire()
        prio_kid('T2', 1.0, 3)
        seen = _wait_for(lambda: bool(ml.prio_pending(name)))
        kid("t=ml.solo_begin('S-new',ttl=60,poll=0.1,quiet=True,wait=60);log('S_OPEN')\n"
            "time.sleep(0.3);ml.solo_end(t,quiet=True);log('S_END')\n")
        time.sleep(1.5)
        e = _sc_events(ev)
        check('new-solo-waits-while-priority-pending',
              seen and 'S_OPEN' not in e and not os.path.exists(ml._solo_path(name)),
              f"(marker={seen} solo_returned={'S_OPEN' in e} window_file={os.path.exists(ml._solo_path(name))})")
        held.release()
        _wait_for(lambda: 'T2_DONE' in _sc_events(ev), 15)
        t0 = time.time()
        try:
            h2 = ml.MashedLock('O', poll=0.05, quiet=True, wait=2).acquire()
            got_ord = round(time.time() - t0, 2)
            h2.release()
        except TimeoutError:
            got_ord = None
        e = _sc_events(ev)
        check('ordinary-acquire-ignores-linger',
              got_ord is not None and got_ord < 1.0 and 'S_OPEN' not in e,
              f"(ordinary acquire {got_ord} s, solo still waiting={'S_OPEN' not in e})")
        _wait_for(lambda: 'S_END' in _sc_events(ev), 20)
        e = _sc_events(ev)
        gap = round(e['S_OPEN'] - e['T2_DONE'], 2) if ('S_OPEN' in e and 'T2_DONE' in e) else None
        check('new-solo-not-opened-during-priority-hold',
              'S_OPEN' in e and 'T2_DONE' in e and e['S_OPEN'] > e['T2_DONE'],
              f"(T2_GOT={e.get('T2_GOT')} T2_DONE={e.get('T2_DONE')} S_OPEN={e.get('S_OPEN')})")
        check('new-solo-waits-through-linger', gap is not None and gap >= 1.5,
              f"(S_OPEN - T2_DONE = {gap} s, linger 3 s)")
        clear_lingers()

        # 6. a priority solo open (the train) does not wait on another priority marker
        held = ml.MashedLock('H2', poll=0.1, quiet=True, wait=10).acquire()
        prio_kid('T3', 0.1, 0)
        seen = _wait_for(lambda: bool(ml.prio_pending(name)))
        kid("t=ml.solo_begin('P-train',ttl=60,poll=0.1,quiet=True,wait=30,priority=1);log('P_OPEN')\n"
            "ml.solo_end(t,quiet=True);log('P_END')\n")
        opened = _wait_for(lambda: os.path.exists(ml._solo_path(name)) or 'P_OPEN' in _sc_events(ev), 5)
        check('priority-solo-ignores-other-marker', seen and opened, f"(marker={seen} opened={opened})")
        held.release()
        _wait_for(lambda: 'P_END' in _sc_events(ev) and 'T3_DONE' in _sc_events(ev), 15)
        time.sleep(0.2)
        clear_lingers()

        # 7. with no claim pending a new solo open does not wait
        t0 = time.time()
        tok = ml.solo_begin('free', name=name, ttl=60, poll=0.05, quiet=True, wait=5)
        ml.solo_end(tok, name=name, quiet=True)
        dt = round(time.time() - t0, 2)
        check('solo-open-unblocked-without-claims', dt < 2.0, f"({dt} s)")

        # 8-9. default expiries are near one gate run (<= 1 h), not 4 h / 12 h
        tok = ml.solo_begin('dflt', name=name, poll=0.05, quiet=True, wait=5)
        o = ml._read_owner(ml._solo_path(name))
        ttl = (o.get('expires') or 0) - (o.get('epoch') or 0)
        ml.solo_end(tok, name=name, quiet=True)
        check('solo-begin-default-ttl-le-1h', 0 < ttl <= 3600, f"(ttl={ttl}s)")
        probe = ("import json,os;o=json.load(open(os.path.join(os.environ['MASHED_LOCK_DIR'],"
                 "'mashed_machine_solo.lock')));print('TTL',o['expires']-o['epoch'])")
        out = subprocess.run([sys.executable, impl, 'run', '--solo', '--label', 'dflt-run', '--',
                              sys.executable, '-c', probe], capture_output=True, text=True,
                             env=dict(os.environ, MASHED_LOCK_DIR=lockdir), timeout=60).stdout
        rttl = next((int(ln.split()[1]) for ln in out.splitlines() if ln.startswith('TTL ')), None)
        check('run-solo-default-ttl-le-1h', rttl is not None and 0 < rttl <= 3600, f"(ttl={rttl}s)")
    except Exception:
        traceback.print_exc()
        while len(res) < TOTAL:
            check('scenario-aborted', False)
    finally:
        for k in kids:
            if k.poll() is None:
                k.kill()
        shutil.rmtree(lockdir, ignore_errors=True)
    fails = res.count(False)
    print(f"scenarios: {len(res) - fails}/{len(res)} PASS", flush=True)
    return fails


def main():
    a = sys.argv[1:]

    def opt(f, d):
        return a[a.index(f) + 1] if f in a else d
    procs, secs = int(opt('--procs', 8)), float(opt('--secs', 12))
    impl = os.path.abspath(opt('--impl', os.path.join(HERE, 'mashed_lock.py')))
    sc_fails = 0 if '--skip-scenarios' in a else scenarios(impl)
    if '--scenarios-only' in a:
        print('PASS' if not sc_fails else 'FAIL')
        return 0 if not sc_fails else 1
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
    ok = not errs and not tot['overlap'] and not hung and tot['acq'] > 0 and not sc_fails
    print('PASS' if ok else 'FAIL')
    shutil.rmtree(lockdir, ignore_errors=True)
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
