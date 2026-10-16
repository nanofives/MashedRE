"""d2lateral arm runner (PREREG_d2lateral.md). Worktree build, explicit PIDs only (never by name).
usage: py -3.12 run_arms.py <arm> [<arm> ...]
  H_<tag>[-<rep>]     G-CTRL recipe: 14400 frames, PLAYERTRACE, AI_STEPDUMP, U9186_GATES
  B_<tag>_K<k>        D3 (b) arm: 1200 frames, MASHED_RING_SKIP=k, step dump only
  D2_<tag>_<n>        D2 arm: a8_run_port.py base env (90 s), MEASURE_SOLO / TRACK_SEL=12 / STEER_HOLD_AFTER=0
  BASE_<arm>          the same arm on the untouched base exe (D2L_BASE_EXE), outputs prefixed BASE_
<tag> selects a knob stack from STACK. MASHED_ROOT = D2L_ROOT, a private copy that CONTAINS original\
(memory mashed-root-main-shares-append-logs), so CWD-relative logs land there, never in the main tree.
Every run appends one provenance line (exe sha8, knobs, pid, exit) to RUNLOG.txt.
Recipes are verify/d2engage_20261016/run_arms.py's (which reproduced the base hashes); only the stacks, OUT and the
exe/root paths differ.
"""
import hashlib, os, shutil, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(W, r'verify\d2lateral_20261016')
SCR = r'C:\Users\maria\AppData\Local\Temp\claude\C--Users-maria-Desktop-Proyectos-Mashed\e3a88210-98a6-42d2-aeaa-b0cc1a3255e9\scratchpad'
EXE_NEW = os.environ.get('D2L_EXE', os.path.join(W, r'mashedmod\build\mashed_re.exe'))
EXE_BASE = os.environ.get('D2L_BASE_EXE', os.path.join(SCR, r'exe_base\mashed_re.exe'))
ROOT = os.environ.get('D2L_ROOT', os.path.join(SCR, 'root'))
assert os.path.exists(os.path.join(ROOT, r'original\TOASTART\Common\Frontend.piz')), 'ROOT lacks original\\'
STACK = {'DEF': [], 'B': ['MASHED_POS_MODEL']}
# DIAGNOSTIC stacks (report only): A6a's per-frame dump (act.* at A6a entry, clamp #6 l_60 / grip / k / arm), the
# D2 sink phase marks (w1, a6a_out, a6b_out, a4_out, sub_top, sub_end, snap) and the world-contact fixup log.
LX = ['MASHED_A6ADUMP=@OUT@\\{n}.a6a.log', 'MASHED_D2SINK=@OUT@\\{n}.sink.log',
      'MASHED_WORLD_CONTACT_LOG=@OUT@\\{n}.wc.log']
for n, base in (('DLX', []), ('BLX', ['MASHED_POS_MODEL'])):
    STACK[n] = base + [k.replace('{n}', n) for k in LX]
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


def run(exe, name, env, files, limit, cwd_logs=('mashed_re.log', 'player_trace.log', 'motion_diag.log'), wall=None):
    for f in [os.path.join(ROOT, c) for c in cwd_logs] + files:
        if os.path.exists(f): os.remove(f)
    for k, v in env.items():
        if k.startswith('MASHED_') and v.startswith(OUT) and os.path.exists(v): os.remove(v)
    t0 = time.time()
    p = subprocess.Popen([exe], cwd=ROOT, env=env)
    print(f'=== {name} pid={p.pid} exe={sha8(exe)}', flush=True)
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
    diag = [v for k, v in env.items() if k.startswith('MASHED_') and v.startswith(OUT)]
    for f in files + diag + [os.path.join(OUT, name + '.ptrace.log'), os.path.join(OUT, name + '.motion_diag.log')]:
        if os.path.exists(f):
            n = sum(1 for _ in open(f, 'rb'))
            print(f'    {os.path.basename(f)} lines={n} sha8={sha8(f)}', flush=True)
            shas.append(f'{os.path.basename(f)}:{n}:{sha8(f)}')
    ks = ' '.join(f'{k}={v}' for k, v in sorted(env.items()) if k.startswith('MASHED_') and k != 'MASHED_ROOT')
    with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
        lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={sha8(exe)} root={os.path.basename(ROOT)} pid={p.pid} '
                 f'exit={rc} self_exited={ok} secs={time.time() - t0:.0f} | {ks} | {" ".join(shas)}\n')


for name in sys.argv[1:]:
    exe, arm = (EXE_BASE, name[5:]) if name.startswith('BASE_') else (EXE_NEW, name)
    if arm.startswith('H_'):
        e = base_env(14400); e['MASHED_PLAYERTRACE'] = '1'
        step = os.path.join(OUT, name + '.step.csv'); gates = os.path.join(OUT, name + '.gates.csv')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_U9186_GATES'] = gates
        knobs(e, STACK[tag_of(arm[2:])])
        run(exe, name, e, [step, gates], 900)
    elif arm.startswith('B_'):
        stack, k = arm[2:].rsplit('_K', 1)
        e = base_env(1200); e['MASHED_RING_SKIP'] = str(int(k))
        step = os.path.join(OUT, name + '.step.csv'); e['MASHED_AI_STEPDUMP'] = step
        knobs(e, STACK[stack])
        run(exe, name, e, [step], 600)
    elif arm.startswith('D2_'):
        tag = arm[3:].rsplit('_', 1)[0]
        e = clean_env(); e.update(D2BASE); e['MASHED_TITLE'] = 'd2lateral-' + name
        knobs(e, STACK[tag])
        run(exe, name, e, [], 0, wall=90)
    else:
        print('unknown arm', name)
print('DONE')
