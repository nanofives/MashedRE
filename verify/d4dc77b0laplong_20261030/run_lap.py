r"""D4-DC-77b0-LAPLONG arm runner (copy of verify/d4dc77b0lap_20261030/run_lap.py, paths changed). D4-DC-77b0-LAP arm runner: the standalone with MASHED_PATHPROG=1 + MASHED_PATHPROG_TRACE on several tracks.

usage: py -3.12 run_lap.py <track> [<track> ...]      (track = a MASHED_TRACK_VIEW basename, e.g. Egypt)
       py -3.12 run_lap.py --score <dir>               (laps column check on every *.pathprog.csv in <dir>)

Recipe = verify/savelink_20261023/run_arms.py base_env (rule 0, game mode 10, deterministic) with the track swapped and
LAP_FRAMES (default 7200) frames. MEASUREMENT ONLY: spawns and stops ONLY its own PID; MASHED_ROOT = a private root.
"""
import csv, glob, hashlib, os, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.normpath(os.environ.get('A4_OUT', 'D:/MashedWT/D4-DC-77b0-LAPLONG_data/lap'))
ROOT = os.path.normpath(os.environ.get('A4_ROOT', 'D:/MashedWT/D4-DC-77b0-LAPLONG_root'))
EXE = os.environ.get('A4_EXE', os.path.join(W, r'mashedmod\build\mashed_re.exe'))
FRAMES = int(os.environ.get('LAP_FRAMES', '7200'))


def sha(p, n=64):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:n]


def run(track):
    gl = os.environ.get('LAP_GATELOG') == '1'   # + MASHED_PATHPROG_GATE_LOG (both step gates, first 400 calls)
    # LAP_EXTRA="K=V;K=V" + LAP_TAG=<suffix>: extra existing knobs (e.g. MASHED_RACE_MODE=L, the laps-mode dev override)
    extra = [kv.split('=', 1) for kv in os.environ.get('LAP_EXTRA', '').split(';') if '=' in kv]
    name = 'L_' + track + ('_G' if gl else '') + os.environ.get('LAP_TAG', '')
    trace = os.path.join(OUT, name + '.pathprog.csv')
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW=track, MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='0', MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=str(FRAMES), MASHED_PATHPROG='1', MASHED_PATHPROG_TRACE=trace)
    if gl: e['MASHED_PATHPROG_GATE_LOG'] = os.path.join(OUT, name + '.gatelog.txt')
    e.update(dict(extra))
    for f in (trace, os.path.join(ROOT, 'mashed_re.log')):
        if os.path.exists(f): os.remove(f)
    t0 = time.time()
    p = subprocess.Popen([EXE], cwd=ROOT, env=e)
    print(f'=== {name} pid={p.pid} exe={sha(EXE, 8)}', flush=True)
    ok = True
    try:
        rc = p.wait(900)
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait(); ok = False
    log = os.path.join(ROOT, 'mashed_re.log')
    if os.path.exists(log):
        with open(log, 'rb') as a, open(os.path.join(OUT, name + '.log'), 'wb') as b: b.write(a.read())
    n = sum(1 for _ in open(trace, 'rb')) if os.path.exists(trace) else 0
    print(f'    exit={rc} self_exited={ok} secs={time.time() - t0:.0f} trace_lines={n}', flush=True)
    ks = ' '.join(f'{k}={v}' for k, v in sorted(e.items()) if k.startswith('MASHED_') and k != 'MASHED_ROOT')
    with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
        lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={sha(EXE, 8)} pid={p.pid} exit={rc} self_exited={ok} '
                 f'| {ks} | trace:{n}:{sha(trace, 8) if n else "-"}\n')


def score(d):
    """Per file and car: laps non-decreasing frame to frame, the set of values, steps of +1 only, first frame of each lap."""
    allok = True
    for f in sorted(glob.glob(os.path.join(d, '*.pathprog.csv'))):
        rows = list(csv.DictReader(open(f)))
        frames = len({r['frame'] for r in rows})
        print(f'{os.path.basename(f)} rows={len(rows)} frames={frames} sha256={sha(f)}')
        for v in sorted({r['v'] for r in rows}):
            seq = [(int(r['frame']), int(r['laps']), int(r['node'])) for r in rows if r['v'] == v]
            dec = [(a, b) for a, b in zip(seq, seq[1:]) if b[1] < a[1]]
            big = [(a, b) for a, b in zip(seq, seq[1:]) if b[1] > a[1] + 1]
            firsts = [b[0] for a, b in zip(seq, seq[1:]) if b[1] > a[1]]
            vals = sorted({s[1] for s in seq})
            nodes = len({s[2] for s in seq})
            ok = not dec
            allok &= ok
            print(f'  v={v} laps {seq[0][1]}->{seq[-1][1]} values={vals} increments_at={firsts[:12]} '
                  f'decreases={len(dec)} jumps>1={len(big)} distinct_nodes={nodes} {"NONDEC" if ok else "DECREASING first=" + str(dec[0])}')
    print('ALL NONDECREASING' if allok else 'SOME DECREASING')


if __name__ == '__main__':
    if sys.argv[1:2] == ['--score']:
        score(sys.argv[2])
    else:
        os.makedirs(OUT, exist_ok=True)
        for t in sys.argv[1:]:
            run(t)
        print('DONE')
