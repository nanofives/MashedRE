"""U-9684 arm runner (PREREG_U9684.md section 1 + section 3). Worktree build, explicit PIDs only.
usage: py -3.12 run_arms.py <arm> [<arm> ...]
H4 arms: run_h4.ps1 recipe (14400 frames, PLAYERTRACE, AI_STEPDUMP, U9186_GATES).
B arms (name B_<stack>_K<k>): run_fk.ps1 recipe (1200 frames, RING_SKIP=k, step dump only).
"""
import hashlib, os, shutil, subprocess, sys, time
W = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\u9684'
OUT = os.path.join(W, r'verify\d3_u9684_20261013')
EXE = os.path.join(W, r'mashedmod\build\mashed_re.exe')
PP = ['MASHED_PATHPROG']
TGT = PP + ['MASHED_TGT3']
RG = PP + ['MASHED_ROUND_GRID', 'MASHED_ROUND_GRID_ORDER']
STACK = {'DEF': [], 'PP': PP, 'TGT': TGT, 'RG': RG, 'TGT_RG': RG + ['MASHED_TGT3']}
H4 = {
    'U_DEF': [], 'U_DEF2': [], 'U_PP': PP,
    'U_TGT': TGT + ['MASHED_TGT3_TRACE=@OUT@\\U_TGT.tgt3.csv'],
    'U_TGT2': TGT + ['MASHED_TGT3_TRACE=@OUT@\\U_TGT2.tgt3.csv'],
    'U_RG': RG,
    'U_TGT_RG': RG + ['MASHED_TGT3', 'MASHED_TGT3_TRACE=@OUT@\\U_TGT_RG.tgt3.csv',
                      'MASHED_PATHPROG_TRACE=@OUT@\\U_TGT_RG.pathprog.csv'],
}

def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8]

def base_env(frames):
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
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

def run(name, env, files, limit):
    log = os.path.join(W, 'mashed_re.log'); pt = os.path.join(W, 'player_trace.log')
    for f in [log, pt] + files:
        if os.path.exists(f): os.remove(f)
    t0 = time.time()
    p = subprocess.Popen([EXE], cwd=W, env=env)
    print(f'=== {name} pid={p.pid}', flush=True)
    try:
        rc = p.wait(limit); ok = True
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait(); ok = False
    if os.path.exists(log): shutil.copy2(log, os.path.join(OUT, name + '.log'))
    if os.path.exists(pt): shutil.copy2(pt, os.path.join(OUT, name + '.ptrace.log'))
    print(f'    exit={rc} self_exited={ok} secs={time.time() - t0:.0f}', flush=True)
    for f in files + [os.path.join(OUT, name + '.ptrace.log')]:
        if os.path.exists(f):
            n = sum(1 for _ in open(f, 'rb'))
            print(f'    {os.path.basename(f)} lines={n} sha8={sha8(f)}', flush=True)
        elif not f.endswith('.ptrace.log') or name in H4:
            print(f'    MISSING {os.path.basename(f)}', flush=True)

for name in sys.argv[1:]:
    if name in H4:
        e = base_env(14400); e['MASHED_PLAYERTRACE'] = '1'
        step = os.path.join(OUT, name + '.step.csv'); gates = os.path.join(OUT, name + '.gates.csv')
        e['MASHED_AI_STEPDUMP'] = step; e['MASHED_U9186_GATES'] = gates
        knobs(e, H4[name])
        extra = [v for k, v in e.items() if k.endswith('_TRACE') and k != 'MASHED_PLAYERTRACE']
        run(name, e, [step, gates] + extra, 900)
    elif name.startswith('B_'):
        stack, k = name[2:].rsplit('_K', 1)
        e = base_env(1200); e['MASHED_RING_SKIP'] = str(int(k))
        step = os.path.join(OUT, name + '.step.csv'); e['MASHED_AI_STEPDUMP'] = step
        knobs(e, STACK[stack])
        run(name, e, [step], 600)
    elif name == 'REPLAY':
        e = base_env(300); e['MASHED_PATHPROG'] = '1'
        e['MASHED_TGT3_REPLAY'] = os.path.join(W, r'verify\d3_modes37_20261009\o_tg1.msd.tgt.csv')
        rp = os.path.join(OUT, 'G_REPLAY_pc24.txt'); e['MASHED_TGT3_REPLAY_OUT'] = rp
        run(name, e, [rp], 300)
        if os.path.exists(rp): print('    ' + open(rp).read().strip())
    elif name.startswith('CF_'):
        e = base_env(300); e['MASHED_PATHPROG'] = '1'
        e['MASHED_TGT3_REPLAY'] = os.path.join(OUT, name + '.tgt.csv')
        rp = os.path.join(OUT, name + '.replay.txt'); e['MASHED_TGT3_REPLAY_OUT'] = rp
        run(name, e, [rp], 300)
        if os.path.exists(rp): print('    ' + open(rp).read().strip().splitlines()[-1])
    else:
        print('unknown arm', name)
print('DONE')
