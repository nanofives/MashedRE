r"""D5-INTROMOVIES-I1B driver. Re-runs run_intro.py I1b I1c (then I0 I2 I3 and the default gates) on an idle machine.
usage: py -3.12 i1b_driver.py wait     poll system CPU until an idle window (outside the lock and the test slot)
       py -3.12 i1b_driver.py inner    run inside mashed_lock.py run --solo + test-slot: log load, run the arms
Idle window = 30 s of 2 s samples with mean system CPU < IDLE_MEAN and max < IDLE_MAX, no other game running.
Every stage prints a wall-clock timestamp, so queue waits (slot, lock, idle) are separable from arm time.
"""
import os, subprocess, sys, time
import psutil

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
IDLE_MEAN, IDLE_MAX, WINDOW, MAX_WAIT = 40.0, 65.0, 15, 100 * 60
ROOT = 'D:/MashedWT/D5-INTROMOVIES-I1B_root'
DATA = 'D:/MashedWT/D5-INTROMOVIES-I1B_data'
GAMES = ('mashed.exe', 'mashed_re.exe')


def ts():
    return time.strftime('%Y-%m-%d %H:%M:%S')


def games():
    n = 0
    for p in psutil.process_iter(['name']):
        if (p.info['name'] or '').lower() in GAMES:
            n += 1
    return n


def window():
    v = [psutil.cpu_percent(interval=2) for _ in range(WINDOW)]
    return sum(v) / len(v), max(v), games()


def wait():
    t0 = time.time()
    while True:
        mean, mx, g = window()
        print(f'{ts()} idle-check mean={mean:.1f}% max={mx:.1f}% other_games={g} waited={time.time() - t0:.0f}s', flush=True)
        if mean < IDLE_MEAN and mx < IDLE_MAX and g == 0:
            print(f'{ts()} IDLE window reached after {time.time() - t0:.0f}s', flush=True)
            return 0
        if time.time() - t0 > MAX_WAIT:
            print(f'{ts()} IDLE NOT reached in {MAX_WAIT}s, running anyway (load recorded per arm)', flush=True)
            return 0


def run(cmd, env):
    t0 = time.time()
    print(f'{ts()} START {" ".join(cmd)}', flush=True)
    rc = subprocess.call(cmd, env=env, cwd=W)
    print(f'{ts()} END rc={rc} secs={time.time() - t0:.0f}', flush=True)
    return rc


def inner():
    print(f'{ts()} INNER lock+slot held, pid={os.getpid()} prio={psutil.Process().nice()}', flush=True)
    mean, mx, g = window()
    print(f'{ts()} pre-arm load mean={mean:.1f}% max={mx:.1f}% other_games={g}', flush=True)
    env = dict(os.environ, A4_ROOT=ROOT, A4_OUT=DATA + '/arms')
    run(['py', '-3.12', 'verify/intromovies_20261028/run_intro.py', 'I1b', 'I1c'], env)
    run(['py', '-3.12', 'verify/intromovies_20261028/run_intro.py', 'I0', 'I2', 'I3'], env)
    genv = dict(os.environ, A4_ROOT=ROOT, A4_OUT=DATA + '/gates')
    run(['py', '-3.12', 'verify/savelink_20261023/run_arms.py', 'H_DEF', 'B_DEF_K0', 'B_DEF_K1', 'B_DEF_K26'], genv)
    print(f'{ts()} INNER done', flush=True)


if __name__ == '__main__':
    sys.exit(wait() if sys.argv[1] == 'wait' else inner())
