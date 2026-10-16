"""d2settle arm runner (PREREG_d2settle.md). Worktree build, explicit PIDs only (never by name).
usage: py -3.12 run_arms.py <arm> [<arm> ...]
  H_<tag>          G-CTRL recipe: 14400 frames, PLAYERTRACE, AI_STEPDUMP, U9186_GATES (run_h4 / U-9684 base env)
  B_<tag>_K<k>     D3 (b) arm: 1200 frames, MASHED_RING_SKIP=k, step dump only
  D_<tag>          Stage-0 diag: 300 frames, PLAYERTRACE + SETTLE_DIAG + A3_LOG
  D2_<tag>_<n>     D2 arm: a8_run_port.py base env (90 s), MEASURE_SOLO / TRACK_SEL=12 / STEER_HOLD_AFTER=0
<tag> selects a knob stack from STACK. The exe runs with MASHED_ROOT = a private copy of original/
(no junction), so CWD-relative logs land in ROOT, never in the shared main tree.
"""
import hashlib, os, shutil, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(W, r'verify\d2settle_20261016')
EXE = os.path.join(W, r'mashedmod\build\mashed_re.exe')
ROOT = os.environ.get('D2S_ROOT', r'C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed'
                      r'\fca5502d-23ca-4cf1-b9f8-f9f257352834\scratchpad\root')
PP = ['MASHED_PATHPROG']
RG = PP + ['MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_ORDER']
OFF = ['MASHED_NO_A3_LUT', 'MASHED_NO_SETTLE_MOVE', 'MASHED_NO_SPEED_REFRESH']
STACK = {'DEF': [], 'PP': PP, 'RG': RG, 'OFF': OFF, 'OFFRG': OFF + RG,
         'NOA3': ['MASHED_NO_A3_LUT'], 'NOMV': ['MASHED_NO_SETTLE_MOVE'], 'NOSR': ['MASHED_NO_SPEED_REFRESH'],
         # D0-c mechanism test (PREREG amendment 2): real wheel points at the original's frame-1 y
         'RGX': RG + ['MASHED_RW_WHEELPOS', 'MASHED_D2S_RINGY=0.4374268054962158'],
         # FINAL build (after 2026-10-16 rebuild): fix C is opt-in MASHED_SPEED_REFRESH=1, so
         # 'FDEF' = shipped default (fix A only), 'FOFF' = fix A opted out (= base), 'FC' = A + C.
         'FDEF': [], 'FOFF': ['MASHED_NO_A3_LUT'], 'FC': ['MASHED_SPEED_REFRESH'],
         # D0-e (PREREG amendment 4): D0-c + FUN_0046baa0's wheel seeds before call 1
         'RGXS': RG + ['MASHED_RW_WHEELPOS', 'MASHED_D2S_RINGY=0.4374268054962158', 'MASHED_D2S_SEED1']}
D2BASE = {'MASHED_REAL_PHYSICS': '1', 'MASHED_RACE_DEMO': '1', 'MASHED_PLAY_DEMO': '1', 'MASHED_GOTO': '6',
          'MASHED_TRACK_SEL': '12', 'MASHED_CAR_SEL': '0', 'MASHED_DRIVE_HOLD': '1', 'MASHED_WIN_POS': 'primary-bl',
          'MASHED_MOTION_DIAG': '1', 'MASHED_STEER_HOLD': '1', 'MASHED_STEER_HOLD_AFTER': '0', 'MASHED_MUTE': '1',
          'MASHED_MEASURE_SOLO': '1'}


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8]


def clean_env():
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e['MASHED_ROOT'] = ROOT
    return e


def base_env(frames):
    e = clean_env()
    e.update(MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='4', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=str(frames))
    return e


def knobs(e, ks):
    for k in ks:
        if '=' in k:
            a, b = k.split('=', 1); e[a] = b.replace('@OUT@', OUT)
        else:
            e[k] = '1'


def run(name, env, files, limit, cwd_logs=('mashed_re.log', 'player_trace.log', 'motion_diag.log'), wall=None):
    for f in [os.path.join(ROOT, c) for c in cwd_logs] + files:
        if os.path.exists(f): os.remove(f)
    t0 = time.time()
    p = subprocess.Popen([EXE], cwd=ROOT, env=env)
    print(f'=== {name} pid={p.pid}', flush=True)
    ok = True
    try:
        if wall:
            time.sleep(wall)
            if p.poll() is None:
                print(f'    wall {wall}s reached - killing pid {p.pid} ONLY', flush=True); p.kill()
            rc = p.wait(); ok = False
        else:
            rc = p.wait(limit)
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait(); ok = False
    for c, ext in (('mashed_re.log', '.log'), ('player_trace.log', '.ptrace.log'), ('motion_diag.log', '.motion_diag.log')):
        src = os.path.join(ROOT, c)
        if os.path.exists(src): shutil.copy2(src, os.path.join(OUT, name + ext))
    print(f'    exit={rc} self_exited={ok} secs={time.time() - t0:.0f}', flush=True)
    for f in files + [os.path.join(OUT, name + '.ptrace.log'), os.path.join(OUT, name + '.motion_diag.log')]:
        if os.path.exists(f):
            n = sum(1 for _ in open(f, 'rb'))
            print(f'    {os.path.basename(f)} lines={n} sha8={sha8(f)}', flush=True)


for name in sys.argv[1:]:
    if name.startswith('H_'):
        e = base_env(14400); e['MASHED_PLAYERTRACE'] = '1'
        step = os.path.join(OUT, name + '.step.csv'); gates = os.path.join(OUT, name + '.gates.csv')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_U9186_GATES'] = gates
        tag = name[2:].rstrip('0123456789') or 'DEF'
        knobs(e, STACK[tag])
        extra = []
        if 'MASHED_PATHPROG' in e:
            e['MASHED_PATHPROG_TRACE'] = os.path.join(OUT, name + '.pathprog.csv'); extra.append(e['MASHED_PATHPROG_TRACE'])
        run(name, e, [step, gates] + extra, 900)
    elif name.startswith('B_'):
        stack, k = name[2:].rsplit('_K', 1)
        e = base_env(1200); e['MASHED_RING_SKIP'] = str(int(k))
        step = os.path.join(OUT, name + '.step.csv'); e['MASHED_AI_STEPDUMP'] = step
        knobs(e, STACK[stack])
        run(name, e, [step], 600)
    elif name.startswith('D_'):
        tag = name[2:].rstrip('0123456789') or 'DEF'
        e = base_env(300); e['MASHED_PLAYERTRACE'] = '1'
        sd = os.path.join(OUT, name + '.settle.txt'); a3 = os.path.join(OUT, name + '.a3.txt')
        e['MASHED_SETTLE_DIAG'] = sd; e['MASHED_A3_LOG'] = a3
        knobs(e, STACK[tag])
        extra = []
        if 'MASHED_PATHPROG' in e:
            e['MASHED_PATHPROG_TRACE'] = os.path.join(OUT, name + '.pathprog.csv'); extra.append(e['MASHED_PATHPROG_TRACE'])
        run(name, e, [sd, a3] + extra, 300)
    elif name.startswith('D2_'):
        tag = name[3:].rsplit('_', 1)[0]
        e = clean_env(); e.update(D2BASE); e['MASHED_TITLE'] = 'd2settle-' + name
        knobs(e, STACK[tag])
        run(name, e, [], 0, wall=90)
    else:
        print('unknown arm', name)
print('DONE')
