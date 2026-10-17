r"""dflip arm runner (copied from verify/d3b_20261016/run_arms.py, same H_ / B_ / D2_ recipes; the W_ recipe is
verify/pontoon_20261016/run_arms.py's). Worktree build, explicit PIDs only (never by name).
usage: py -3.12 run_arms.py <arm> [<arm> ...]
  H_<tag>[-<rep>]     G-CTRL recipe: 14400 frames, PLAYERTRACE, AI_STEPDUMP, U9186_GATES
  B_<tag>_K<k>        D3 (b) arm: 1200 frames, MASHED_RING_SKIP=k, step dump only
  D2_<tag>_<n>        D2 arm: a8_run_port.py base env (90 s), MEASURE_SOLO / TRACK_SEL=12 / STEER_HOLD_AFTER=0
  W_<tag>[-<rep>]     Warzone live (pontoon recipe): rule 8, 3600 frames, PONTOON_TRACE + SEEK_TRACE + AI_STEPDUMP
  WL_<tag>[-<rep>]    the same at 18000 frames (300 s, the length of the original's OP3 capture): full-race no-crash arm
<tag> selects a knob stack from STACK. Tag DEF = the plain (new, dflip) default, no knob.
The exe runs with MASHED_ROOT = a private copy of original/ (the root CONTAINS original\), so CWD-relative
logs land in ROOT, never in the shared main tree. Every run appends one provenance line to RUNLOG.txt.
"""
import hashlib, os, shutil, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.environ.get('DFLIP_OUT', os.path.join(W, r'verify\dflip_20261017\arms'))
EXE_NEW = os.environ.get('DFLIP_EXE', os.path.join(W, r'mashedmod\build\mashed_re.exe'))
EXE_BASE = EXE_NEW
ROOT = os.environ.get('DFLIP_ROOT', r'D:\MashedWT\dflip_root')
OPTOUT = ['MASHED_NO_HEADING_ASSIST', 'MASHED_NO_PONTOON_CARRY', 'MASHED_NO_PONTOON_STATE', 'MASHED_NO_PONTOON_ANIM']
STACK = {'DEF': [],
         # every dflip opt-out: must give the pre-dflip default (964c31f04)
         'OPT': OPTOUT,
         'NOHA': ['MASHED_NO_HEADING_ASSIST'], 'NOPC': ['MASHED_NO_PONTOON_CARRY'], 'NOP': ['MASHED_NO_PONTOON_ANIM'],
         # the AI RW-LUT lookahead stacked on the new default (report only, not flipped)
         'L5': ['MASHED_AI_LUT=5']}
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


def run(name, env, files, limit, exe, cwd_logs=('mashed_re.log', 'player_trace.log', 'motion_diag.log'), wall=None):
    for f in [os.path.join(ROOT, c) for c in cwd_logs] + files:
        if os.path.exists(f): os.remove(f)
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
    for f in files + [os.path.join(OUT, name + '.ptrace.log'), os.path.join(OUT, name + '.motion_diag.log')]:
        if os.path.exists(f):
            n = sum(1 for _ in open(f, 'rb'))
            print(f'    {os.path.basename(f)} lines={n} sha8={sha8(f)}', flush=True)
            shas.append(f'{os.path.basename(f)}:{n}:{sha8(f)}')
    ks = ' '.join(f'{k}={v}' for k, v in sorted(env.items()) if k.startswith('MASHED_') and k != 'MASHED_ROOT')
    with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
        lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={sha8(exe)} pid={p.pid} exit={rc} '
                 f'self_exited={ok} secs={time.time() - t0:.0f} | {ks} | {" ".join(shas)}\n')


def exe_for(tag):
    return EXE_NEW


for name in sys.argv[1:]:
    if name.startswith('H_'):
        tag = tag_of(name[2:])
        e = base_env(14400); e['MASHED_PLAYERTRACE'] = '1'
        step = os.path.join(OUT, name + '.step.csv'); gates = os.path.join(OUT, name + '.gates.csv')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_U9186_GATES'] = gates
        knobs(e, STACK[tag])
        run(name, e, [step, gates], 900, exe_for(tag))
    elif name.startswith('B_'):
        stack, k = name[2:].rsplit('_K', 1)
        e = base_env(1200); e['MASHED_RING_SKIP'] = str(int(k))
        step = os.path.join(OUT, name + '.step.csv'); e['MASHED_AI_STEPDUMP'] = step
        knobs(e, STACK[stack])
        run(name, e, [step], 600, exe_for(stack))
    elif name.startswith('D2_') or name.startswith('P_'):
        tag = name.split('_', 1)[1].rsplit('_', 1)[0]
        e = clean_env(); e.update(D2BASE); e['MASHED_TITLE'] = 'dflip-' + name
        knobs(e, STACK[tag])
        files = []
        if name.startswith('P_'):
            e['MASHED_BATCH_DUMP'] = os.path.join(OUT, name + '.batch.csv'); files.append(e['MASHED_BATCH_DUMP'])
        run(name, e, files, 0, exe_for(tag), wall=90)
    elif name.startswith('W_') or name.startswith('WL_'):
        tag = tag_of(name.split('_', 1)[1])
        e = base_env(18000 if name.startswith('WL_') else 3600)
        e['MASHED_TRACK_VIEW'] = 'Warzone'; e['MASHED_ROUND_RULE'] = '8'
        step = os.path.join(OUT, name + '.step.csv'); pt = os.path.join(OUT, name + '.pontoon.txt')
        sk = os.path.join(OUT, name + '.seek.txt')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_PONTOON_TRACE'] = pt; e['MASHED_SEEK_TRACE'] = sk
        knobs(e, STACK[tag])
        run(name, e, [step, pt, sk], 2400 if name.startswith('WL_') else 900, exe_for(tag))
    elif name.startswith('LR_'):
        # d3b look replay (PREREG G-REPLAY-LOOK): the original's aistep rows through SplineLookahead, exact and LUT
        e = base_env(1200)
        e['MASHED_AI_LOOK_REPLAY'] = ';'.join(os.path.join(W, 'verify', 'd3_regimes_20261013', f'{r}.msd.aistep.csv') for r in ('o_reg_r1', 'o_reg_r13', 'o_reg_r19'))
        e['MASHED_AI_LOOK_REPLAY_OUT'] = os.path.join(OUT, name + '.look.csv')
        if name.startswith('LR_M'):
            e['MASHED_AI_LOOK_REPLAY_MASK'] = name[4:]   # LR_M5 -> mode 1 runs MASHED_AI_LUT=5
        run(name, e, [e['MASHED_AI_LOOK_REPLAY_OUT']], 300, exe_for('DEF'))
    elif name.startswith('R_'):
        tag = tag_of(name[2:])
        e = base_env(120)
        e['MASHED_BSP_REPLAY'] = os.environ['D2B_REPLAY_IN']
        e['MASHED_BSP_REPLAY_OUT'] = os.path.join(OUT, name + '.replay.csv')
        knobs(e, STACK[tag])
        run(name, e, [e['MASHED_BSP_REPLAY_OUT']], 300, exe_for(tag))
    else:
        print('unknown arm', name)
print('DONE')
