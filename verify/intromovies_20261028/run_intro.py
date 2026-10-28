r"""D5-INTROMOVIES boot arms (PREREG_D5-INTROMOVIES.md). Standalone only, private root, explicit PID.
usage: py -3.12 run_intro.py <arm> [<arm> ...]     arms: I0 I1 I2 I3
Movies: I1/I2 copy the real files from the private root's own movies\backup\ into movies\;
I3 copies the 167,414-byte skip files from the shared install (read only). I0 leaves them as they are.
Each arm prints '[n/N] <arm> PASS|FAIL' and appends its scored row to <OUT>/intro_score.txt.
"""
import hashlib, json, os, re, shutil, struct, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.environ.get('A4_EXE', os.path.join(W, r'mashedmod\build\mashed_re.exe'))
ROOT = os.path.normpath(os.environ.get('A4_ROOT', 'D:/MashedWT/D5-INTROMOVIES_root'))
OUT = os.path.normpath(os.environ.get('A4_OUT', 'D:/MashedWT/D5-INTROMOVIES_data/arms'))
SHARED_MOV = os.path.normpath(os.path.join(W, '..', '..', 'original', 'toastart', 'pc', 'movies'))
MOV = os.path.join(ROOT, 'original', 'toastart', 'pc', 'movies')
FIVE = ['empire', 'supersonic', 'renderware', 'intro', 'small']
DUR = {1: 7.78, 2: 9.48, 3: 8.04, 4: 63.73}          # PREREG table (idx -> seconds)
TEAL = (0x00, 0x80, 0x80)
ARMS = {'I0': ({}, None, 25), 'I1': ({'MASHED_INTRO_MOVIES': '1', 'MASHED_INTRO_SHOT': '90'}, 'real', 140),
        'I2': ({'MASHED_INTRO_MOVIES': '1', 'MASHED_INTRO_PRESS': '30'}, 'real', 60),
        'I3': ({'MASHED_INTRO_MOVIES': '1'}, 'skip', 30)}
# Re-runs of I1 added after the first I1 missed its band on movie 1 (not PREREG rows; RESULT reports both).
ARMS['I1b'] = ARMS['I1c'] = ARMS['I1']


def sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest().upper()


def place(kind):
    src = os.path.join(MOV, 'backup') if kind == 'real' else SHARED_MOV
    for n in FIVE:
        shutil.copy2(os.path.join(src, n + '.mpg'), os.path.join(MOV, n + '.mpg'))
    return {n: (os.path.getsize(os.path.join(MOV, n + '.mpg')), sha(os.path.join(MOV, n + '.mpg'))[:16]) for n in FIVE}


def bmp_stats(p):
    b = open(p, 'rb').read()
    off, w, h = struct.unpack_from('<I', b, 10)[0], struct.unpack_from('<i', b, 18)[0], struct.unpack_from('<i', b, 22)[0]
    row = (w * 3 + 3) & ~3
    n = teal = 0
    s = [0.0, 0.0, 0.0]; s2 = [0.0, 0.0, 0.0]
    for y in range(0, abs(h), 2):
        r = b[off + y * row: off + y * row + w * 3]
        for x in range(0, w * 3, 6):
            bb, gg, rr = r[x], r[x + 1], r[x + 2]
            n += 1
            teal += (rr, gg, bb) == TEAL
            for i, v in enumerate((rr, gg, bb)):
                s[i] += v; s2[i] += v * v
    sd = [max(0.0, s2[i] / n - (s[i] / n) ** 2) ** 0.5 for i in range(3)]
    return {'w': w, 'h': abs(h), 'teal_frac': round(teal / n, 4), 'sd': [round(v, 1) for v in sd], 'sha8': sha(p)[:8]}


def score(arm, log, shots):
    lines = [l.rstrip() for l in log.splitlines()]
    intro = [l for l in lines if l.startswith('INTRO movie')]
    small = [l for l in lines if l.startswith('INTRO small.mpg')]
    f1 = [i for i, l in enumerate(lines) if l.startswith('F1 frontend.mpg: RUNNING')]
    last_intro = max([i for i, l in enumerate(lines) if l.startswith('INTRO')], default=-1)
    rows = []
    for l in intro:
        m = re.match(r'INTRO movie idx=(\d) (\S+): (\S+?)(?:hr=\S+)? frames=(\d+) ms=(\d+) video=(\S+) tex=(\S+) pow2=(\d) audio_hr=(\S+)', l)
        rows.append(dict(idx=int(m[1]), why=m[3], frames=int(m[4]), ms=int(m[5]), video=m[6], tex=m[7], pow2=int(m[8]),
                         audio_hr=m[9]) if m else dict(raw=l))
    why = []
    arm = arm[:2]   # I1b / I1c score as I1
    if arm == 'I0':
        ok = not intro and not small and bool(f1)
        if intro or small: why.append('INTRO lines present with knob OFF')
        if not f1: why.append('no F1 RUNNING')
    else:
        ok = [r.get('idx') for r in rows] == [1, 2, 3, 4]
        if not ok: why.append('order/count %s' % [r.get('idx') for r in rows])
        for r in rows:
            if 'raw' in r: ok = False; why.append('unparsed ' + r['raw']); continue
            if arm == 'I1':
                lo, hi = 0.85 * DUR[r['idx']] * 1000, 1.35 * DUR[r['idx']] * 1000
                r['band'] = [int(lo), int(hi)]
                if r['why'] != 'complete' or not lo <= r['ms'] <= hi:
                    ok = False; why.append('idx%d %s ms=%d band [%d,%d]' % (r['idx'], r['why'], r['ms'], lo, hi))
            if arm == 'I2' and (r['why'] != 'skip-test' or r['frames'] != 30):
                ok = False; why.append('idx%d %s frames=%d' % (r['idx'], r['why'], r['frames']))
            if arm == 'I3' and (r['why'] != 'complete' or r['ms'] >= 3000):
                ok = False; why.append('idx%d %s ms=%d' % (r['idx'], r['why'], r['ms']))
        if not small or 'BUILT' not in small[0]: ok = False; why.append('small.mpg %s' % small)
        if not f1 or f1[0] < last_intro: ok = False; why.append('F1 RUNNING missing or before INTRO lines')
        if arm == 'I1':
            if sorted(shots) != [1, 2, 3, 4]: ok = False; why.append('shots %s' % sorted(shots))
            for k, st in shots.items():
                if st['teal_frac'] >= 0.5 or max(st['sd']) <= 0: ok = False; why.append('shot%d %s' % (k, st))
    return ok, rows, small, why


GAMES = {'mashed.exe', 'mashed_re.exe'}


def sample_load(p, t0, wall, logp, csvp):
    """Machine load at 1 Hz while the arm runs (added for I1b/I1c). phase = movie playing, from the
    count of 'INTRO movie' lines already in the log (1..4, then 'post'). cpu values: sys/permax in %
    of the machine / busiest core, ours/others in cores (100 = one core). Raw rows go to csvp."""
    import psutil
    ncpu = psutil.cpu_count()
    me = psutil.Process(p.pid)
    prio = None
    try:
        prio = str(me.nice())
    except Exception:
        pass
    psutil.cpu_percent(None); psutil.cpu_percent(None, percpu=True); me.cpu_percent(None)
    procs = {}
    for q in psutil.process_iter(['name']):
        try: q.cpu_percent(None); procs[q.pid] = q
        except Exception: pass
    rows, cum = [], {}
    while time.time() - t0 < wall and p.poll() is None:
        time.sleep(1.0)
        try:
            n = open(logp, encoding='latin1').read().count('INTRO movie') if os.path.exists(logp) else 0
        except OSError:
            n = -1
        sysc = psutil.cpu_percent(None); per = psutil.cpu_percent(None, percpu=True)
        try: ours = me.cpu_percent(None)
        except Exception: ours = 0.0
        games, top = 0, []
        for q in psutil.process_iter(['name']):
            if q.pid not in procs: procs[q.pid] = q
            nm = (q.info['name'] or '?').lower()
            if q.pid != p.pid and nm in GAMES: games += 1
            try:
                c = procs[q.pid].cpu_percent(None)
            except Exception:
                continue
            if q.pid not in (p.pid, 0) and nm != 'system idle process':
                top.append((c, nm)); cum[nm] = cum.get(nm, 0.0) + c
        top.sort(reverse=True)
        others = max(0.0, sysc * ncpu - ours)
        phase = 'post' if n >= 4 else str(n + 1)
        rows.append(dict(t=round(time.time() - t0, 1), phase=phase, sys=sysc, permax=max(per), ours=ours,
                         others=round(others, 1), other_games=games,
                         top=';'.join('%s:%.0f' % (nm, c) for c, nm in top[:3])))
    with open(csvp, 'w') as f:
        f.write('t,phase,sys,permax,ours,others,other_games,top\n')
        for r in rows:
            f.write('{t},{phase},{sys},{permax},{ours},{others},{other_games},{top}\n'.format(**r))
    summ = dict(ncpu=ncpu, nice=prio, samples=len(rows), csv_sha16=sha(csvp)[:16], phase={})
    for ph in ['1', '2', '3', '4', 'post']:
        rs = [r for r in rows if r['phase'] == ph]
        if not rs: continue
        s = sorted(r['sys'] for r in rs)
        summ['phase'][ph] = dict(n=len(rs), sys_mean=round(sum(s) / len(s), 1), sys_p95=s[int(0.95 * (len(s) - 1))],
                                 permax_mean=round(sum(r['permax'] for r in rs) / len(rs), 1),
                                 ours_mean=round(sum(r['ours'] for r in rs) / len(rs), 1),
                                 others_mean=round(sum(r['others'] for r in rs) / len(rs), 1),
                                 other_games_max=max(r['other_games'] for r in rs))
    summ['top_others'] = [(nm, round(c / max(1, len(rows)), 1)) for nm, c in sorted(cum.items(), key=lambda x: -x[1])[:6]]
    return summ


def main():
    arms = sys.argv[1:]
    os.makedirs(OUT, exist_ok=True)
    for n, arm in enumerate(arms, 1):
        env_k, kind, wall = ARMS[arm]
        placed = place(kind) if kind else None
        for f in ['mashed_re.log'] + ['intro_shot_%d.bmp' % i for i in range(1, 5)]:
            p = os.path.join(ROOT, f)
            if os.path.exists(p): os.remove(p)
        env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
        env.update(MASHED_ROOT=ROOT, MASHED_WIN_POS='primary-bl', MASHED_MUTE='1', **env_k)
        t0 = time.time()
        p = subprocess.Popen([EXE], cwd=ROOT, env=env)
        print(f'=== {arm} pid={p.pid} exe={sha(EXE)[:16]} wall={wall}s', flush=True)
        load = sample_load(p, t0, wall, os.path.join(ROOT, 'mashed_re.log'), os.path.join(OUT, arm + '_load.csv'))
        alive = p.poll() is None
        if alive:
            p.kill()   # only the PID this script spawned
        rc = p.wait()
        log = open(os.path.join(ROOT, 'mashed_re.log'), encoding='latin1').read() if os.path.exists(os.path.join(ROOT, 'mashed_re.log')) else ''
        shutil.copy2(os.path.join(ROOT, 'mashed_re.log'), os.path.join(OUT, arm + '.log')) if log else None
        shots = {}
        for i in range(1, 5):
            sp = os.path.join(ROOT, 'intro_shot_%d.bmp' % i)
            if os.path.exists(sp):
                shutil.copy2(sp, os.path.join(OUT, '%s_shot_%d.bmp' % (arm, i)))
                shots[i] = bmp_stats(sp)
        ok, rows, small, why = score(arm, log, shots)
        rec = dict(arm=arm, pass_=ok, pid=p.pid, alive_at_wall=alive, exit=rc, secs=round(time.time() - t0),
                   env=env_k, movies=placed, intro=rows, small=small, shots=shots, why=why,
                   log_sha16=sha(os.path.join(OUT, arm + '.log'))[:16] if log else None, load=load)
        with open(os.path.join(OUT, 'intro_score.txt'), 'a') as f:
            f.write(json.dumps(rec) + '\n')
        print(json.dumps(rec, indent=1), flush=True)
        print(f'[{n}/{len(arms)}] {arm} {"PASS" if ok else "FAIL"}', flush=True)


if __name__ == '__main__':
    main()
