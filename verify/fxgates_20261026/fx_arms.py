r"""D4-FX-GATES port FX arms (PREREG_fxgates.md section 3, arm 2). The G-CTRL recipe of
verify/savelink_20261023/run_arms.py (H arm: base_env(14400) + PLAYERTRACE + AI_STEPDUMP + U9186_GATES) plus
MASHED_FX_DEBUG, so each arm also yields the three G-CTRL files (a knob-ON run must leave them unchanged).
  F0  knob OFF        F1  MASHED_FX_ORIG_GATES        F2  F1 + MASHED_FX_ORIG_SURFACE=<hex> (argv: F2=<hex>)
  <arm>R4             the same with MASHED_ROUND_RULE=4 (exploratory, not in the PREREG)
usage: py -3.12 fx_arms.py F0 F1 F2=ff968080   (env A4_ROOT, A4_OUT, A4_EXE as run_arms.py)
Explicit PIDs only; kills only the pid it started (on timeout)."""
import hashlib, os, shutil, subprocess, sys, time
OUT = os.path.normpath(os.environ['A4_OUT']); ROOT = os.path.normpath(os.environ['A4_ROOT']); EXE = os.environ['A4_EXE']


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8]


def env_for(name, surf):
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='0', MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES='14400', MASHED_PLAYERTRACE='1', MASHED_FX_DEBUG='1',
             MASHED_AI_STEPDUMP=os.path.join(OUT, name + '.step.csv'), MASHED_U9186_GATES=os.path.join(OUT, name + '.gates.csv'))
    if not name.startswith('F0'): e['MASHED_FX_ORIG_GATES'] = '1'
    if name.endswith('R4'): e['MASHED_ROUND_RULE'] = '4'   # exploratory: the 2026-10-16 RESULT_FX recipe's round rule
    if surf: e['MASHED_FX_ORIG_SURFACE'] = surf
    return e


for arg in sys.argv[1:]:
    name, _, surf = arg.partition('=')
    e = env_for(name, surf)
    logs = {'mashed_re.log': '.log', 'player_trace.log': '.ptrace.log', os.path.join('log', 'fx_debug.txt'): '.fx_debug.txt'}
    for c in logs:
        if os.path.exists(os.path.join(ROOT, c)): os.remove(os.path.join(ROOT, c))
    for f in (e['MASHED_AI_STEPDUMP'], e['MASHED_U9186_GATES']):
        if os.path.exists(f): os.remove(f)
    os.makedirs(os.path.join(ROOT, 'log'), exist_ok=True)
    t0 = time.time(); p = subprocess.Popen([EXE], cwd=ROOT, env=e)
    print(f'=== {name} pid={p.pid} exe={sha8(EXE)} surf={surf or "-"}', flush=True)
    try:
        rc = p.wait(900)
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait()
    for c, ext in logs.items():
        if os.path.exists(os.path.join(ROOT, c)): shutil.copy2(os.path.join(ROOT, c), os.path.join(OUT, name + ext))
    print(f'    exit={rc} secs={time.time() - t0:.0f}', flush=True)
    for f in (e['MASHED_AI_STEPDUMP'], e['MASHED_U9186_GATES'], os.path.join(OUT, name + '.ptrace.log'), os.path.join(OUT, name + '.fx_debug.txt')):
        if os.path.exists(f):
            print(f'    {os.path.basename(f)} lines={sum(1 for _ in open(f, "rb"))} sha8={sha8(f)}', flush=True)
    with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
        ks = ' '.join(f'{k}={v}' for k, v in sorted(e.items()) if k.startswith('MASHED_') and k != 'MASHED_ROOT')
        lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={sha8(EXE)} pid={p.pid} exit={rc} | {ks}\n')
