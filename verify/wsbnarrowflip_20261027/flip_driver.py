"""D4-WSB-NARROW-FLIP driver (copy of verify/fxflip_20261027/flip_driver.py; adds the NONF opt-out control arms): merge_train Gate-Default + Gate-Full on one worktree (mirrors merge_train.ps1)."""
import hashlib, json, os, subprocess, sys, time
WT = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-WSB-NARROW-FLIP'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
DATA = r'D:\MashedWT\D4-WSB-NARROW-FLIP_data'
ROOT = r'D:\MashedWT\D4-WSB-NARROW-FLIP_root'
ROOT13 = r'D:\MashedWT\D4-WSB-NARROW-FLIP_root13'
CFG = json.load(open(os.path.join(WT, r're\orchestrator\merge_train.json')))
F = CFG['full']; EXE = os.path.join(WT, r'mashedmod\build\mashed_re.exe'); OUT = os.path.join(DATA, 'arms')
os.makedirs(OUT, exist_ok=True)
def say(*a): print(time.strftime('%H:%M:%S'), *a, flush=True)
def sha8(p): return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8] if os.path.exists(p) else 'MISSING'
def run(cmd, **kw):
    say('$', ' '.join(cmd)); r = subprocess.run(cmd, capture_output=True, text=True, **kw)
    for l in (r.stdout + r.stderr).splitlines(): print('   ', l, flush=True)
    return r
CTRL = os.path.join(WT, 'verify', 'wsbnarrowflip_20261027', 'run_arms.py')
def arms(names, solo=False, runner=None):
    env = dict(os.environ, A4_EXE=EXE, A4_ROOT=ROOT, A4_OUT=OUT)
    la = ['run', '--label', 'D4-WSB-NARROW-FLIP', '--wait', '7200'] + (['--solo'] if solo else [])
    return run(['py.exe', '-3.12', LOCK] + la + ['--', 'py.exe', '-3.12', runner or os.path.join(WT, CFG['runner'])] + names, env=env)
stage = sys.argv[1] if len(sys.argv) > 1 else 'all'
if stage in ('all', 'gates'):
    arms([CFG['gctrl']['arm']] + CFG['b']['arms'])
    g = [sha8(os.path.join(OUT, f)) for f in CFG['gctrl']['files']]; b = [sha8(os.path.join(OUT, a + '.step.csv')) for a in CFG['b']['arms']]
    say('GATES G-CTRL', '/'.join(g), 'B', '/'.join(b))
    steps = [os.path.join(OUT, a + '.step.csv') for a in CFG['b']['arms']]
    v = run(['py.exe', '-3.12', os.path.join(WT, F['vsum'])] + steps); open(os.path.join(DATA, 'vsum_b.txt'), 'w').write(v.stdout)
    e = run(['py.exe', '-3.12', os.path.join(WT, F['ai_speed_env']), '--check', steps[0]]); open(os.path.join(DATA, 'e_check.txt'), 'w').write(e.stdout)
if stage in ('all', 'ctrl'):
    arms(['H_NONF', 'B_NONF_K0', 'B_NONF_K1', 'B_NONF_K26'], runner=CTRL)
    g = [sha8(os.path.join(OUT, f.replace('H_DEF', 'H_NONF'))) for f in CFG['gctrl']['files']]; b = [sha8(os.path.join(OUT, a + '.step.csv')) for a in ('B_NONF_K0', 'B_NONF_K1', 'B_NONF_K26')]
    say('CTRL (opt-out) G-CTRL', '/'.join(g), 'B', '/'.join(b))
if stage in ('all', 'd2'):
    d2 = F['d2_arms']; arms(d2, solo=True)
    def short(): return [a for a in d2 if not os.path.exists(os.path.join(OUT, a + '.motion_diag.log')) or
                         sum(1 for _ in open(os.path.join(OUT, a + '.motion_diag.log'))) < F['d2_min_lines']]
    tries = {a: 0 for a in d2}
    while True:
        s = [a for a in short() if tries[a] < F['d2_max_reruns']]
        if not s: break
        for a in s: tries[a] += 1; say('D2 rerun', a, tries[a])
        arms(s, solo=True)
    say('D2 short after reruns:', short(), 'lines', {a: sum(1 for _ in open(os.path.join(OUT, a + '.motion_diag.log'))) for a in d2})
    d = run(['py.exe', '-3.12', os.path.join(WT, F['d2_score']), 'port'] + [os.path.join(OUT, a + '.motion_diag.log') for a in d2])
    open(os.path.join(DATA, 'score_d2.txt'), 'w').write(d.stdout + d.stderr)
if stage in ('all', 'tracks'):
    if not os.path.exists(os.path.join(ROOT13, r'original\MASHED.exe')):
        run(['robocopy', r'C:\Users\maria\Desktop\Proyectos\Mashed\original', os.path.join(ROOT13, 'original'), '/E', '/NFL', '/NDL', '/NJH', '/NJS', '/NP'])
    o13 = os.path.join(DATA, 'tracks')
    t = run(['py.exe', '-3.12', LOCK, 'solo-begin', '--label', 'D4-WSB-NARROW-FLIP-13', '--ttl', '14400', '--owner-pid', str(os.getpid())])
    tok = [l[6:] for l in t.stdout.splitlines() if l.startswith('TOKEN ')][0]
    try:
        run(['py.exe', '-3.12', os.path.join(WT, F['run13']), 'run', '--exe', EXE, '--root', ROOT13, '--out', o13],
            env=dict(os.environ, MASHED_LOCK_SOLO_TOKEN=tok))
    finally:
        run(['py.exe', '-3.12', LOCK, 'solo-end', '--token', tok])
    s13 = run(['py.exe', '-3.12', os.path.join(WT, F['run13']), 'score', '--out', o13, '--cite', os.path.join(WT, F['run13_cites'])])
    open(os.path.join(DATA, 'tracks_score.txt'), 'w').write(s13.stdout)
    ref = [l for l in open(os.path.join(WT, F['run13_reference'])) if l[:2].isdigit() and l[2] == '_']
    got = [l + '\n' for l in s13.stdout.splitlines() if l[:2].isdigit() and l[2:3] == '_']
    say('13 tracks', 'identical' if [x.rstrip() for x in ref] == [x.rstrip() for x in got] else 'DIFFER',
        'gates', 'PASS' if 'INSTRUMENT GATES: PASS' in s13.stdout else 'FAIL')
say('DONE', stage)
