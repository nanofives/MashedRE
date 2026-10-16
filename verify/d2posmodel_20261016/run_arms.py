"""d2posmodel arm runner (PREREG_d2posmodel.md). Worktree build, explicit PIDs only (never by name).
usage: py -3.12 run_arms.py <arm> [<arm> ...]
  H_<tag>[-<rep>]     G-CTRL recipe: 14400 frames, PLAYERTRACE, AI_STEPDUMP, U9186_GATES
  B_<tag>_K<k>        D3 (b) arm: 1200 frames, MASHED_RING_SKIP=k, step dump only
  D_<tag>[-<rep>]     Stage diag: 300 frames, PLAYERTRACE + SETTLE_DIAG (+ PATHPROG trace if the stack has it)
  D2_<tag>_<n>        D2 arm: a8_run_port.py base env (90 s), MEASURE_SOLO / TRACK_SEL=12 / STEER_HOLD_AFTER=0
<tag> selects a knob stack from STACK. The exe runs with MASHED_ROOT = a private copy of original/
(no junction), so CWD-relative logs land in ROOT, never in the shared main tree. Every run appends
one provenance line (exe sha8, knobs, pid, exit) to RUNLOG.txt.
Same recipes as verify/d2settle_20261016/run_arms.py (which reproduced the base hashes); only the
stacks, the output dir, ROOT and the arm-name parsing differ (tags here end in digits, e.g. P1).
"""
import hashlib, os, shutil, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(W, r'verify\d2posmodel_20261016')
EXE = os.path.join(W, r'mashedmod\build\mashed_re.exe')
ROOT = os.environ.get('D2P_ROOT', r'C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed'
                      r'\6f2f5c3b-7736-4b65-b205-39c7e7188055\scratchpad\root')
PP = ['MASHED_PATHPROG']
RG = PP + ['MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_ORDER']
STACK = {'DEF': [],
         'P1': ['MASHED_POS_SEED'], 'P2': ['MASHED_POS_WHEELS'], 'P3': ['MASHED_POS_TRANS'],
         'P23': ['MASHED_POS_WHEELS', 'MASHED_POS_TRANS'],
         'B': ['MASHED_POS_MODEL'], 'BC': ['MASHED_POS_MODEL', 'MASHED_SPEED_REFRESH'],
         'C': ['MASHED_SPEED_REFRESH'], 'RG': RG, 'BRG': ['MASHED_POS_MODEL'] + RG}
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


def tag_of(rest):
    return rest.split('-', 1)[0] or 'DEF'


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
    shas = []
    for f in files + [os.path.join(OUT, name + '.ptrace.log'), os.path.join(OUT, name + '.motion_diag.log')]:
        if os.path.exists(f):
            n = sum(1 for _ in open(f, 'rb'))
            print(f'    {os.path.basename(f)} lines={n} sha8={sha8(f)}', flush=True)
            shas.append(f'{os.path.basename(f)}:{n}:{sha8(f)}')
    ks = ' '.join(f'{k}={v}' for k, v in sorted(env.items()) if k.startswith('MASHED_') and k != 'MASHED_ROOT')
    with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
        lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={sha8(EXE)} pid={p.pid} exit={rc} '
                 f'self_exited={ok} secs={time.time() - t0:.0f} | {ks} | {" ".join(shas)}\n')


for name in sys.argv[1:]:
    if name.startswith('H_'):
        e = base_env(14400); e['MASHED_PLAYERTRACE'] = '1'
        step = os.path.join(OUT, name + '.step.csv'); gates = os.path.join(OUT, name + '.gates.csv')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_U9186_GATES'] = gates
        knobs(e, STACK[tag_of(name[2:])])
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
        e = base_env(300); e['MASHED_PLAYERTRACE'] = '1'
        sd = os.path.join(OUT, name + '.settle.txt'); e['MASHED_SETTLE_DIAG'] = sd
        knobs(e, STACK[tag_of(name[2:])])
        extra = []
        if 'MASHED_PATHPROG' in e:
            e['MASHED_PATHPROG_TRACE'] = os.path.join(OUT, name + '.pathprog.csv'); extra.append(e['MASHED_PATHPROG_TRACE'])
        run(name, e, [sd] + extra, 300)
    elif name.startswith('D2_'):
        tag = name[3:].rsplit('_', 1)[0]
        e = clean_env(); e.update(D2BASE); e['MASHED_TITLE'] = 'd2posmodel-' + name
        knobs(e, STACK[tag])
        run(name, e, [], 0, wall=90)
    else:
        print('unknown arm', name)
print('DONE')
