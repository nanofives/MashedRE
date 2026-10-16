"""pontoon arm runner (adapted from verify/d2settle_20261016/run_arms.py). Explicit exe, explicit PIDs only (never by name).
usage: py -3.12 run_arms.py <exe> <arm> [<arm> ...]
  H_<tag><n>     G-CTRL recipe: TRAINING, 14400 frames, PLAYERTRACE, AI_STEPDUMP, U9186_GATES -> sha8 step / gates / ptrace
  D2_<tag>_<n>   D2 arm: a8_run_port.py base env (90 s wall), MEASURE_SOLO / TRACK_SEL=12 / STEER_HOLD_AFTER=0
  W_<tag><n>     Warzone live: course 33, rule 8, 3600 frames, MASHED_PONTOON_TRACE + MASHED_SEEK_TRACE (+ AI_STEPDUMP)
<tag> selects the knob stack: DEF (none), PA (MASHED_PONTOON_ANIM), PS (MASHED_PONTOON_STATE), PC (MASHED_PONTOON_CARRY).
MASHED_ROOT = the lane's private copy of original/ (D:\\MashedWT\\pontoon_root), so CWD-relative logs never touch the shared tree.
Outputs: verify/pontoon_20261016/arms/<arm>.*  (MB-sized traces are deleted before the commit; the sha8 lines stay in the logs).
"""
import hashlib, os, shutil, subprocess, sys, time
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'arms')
ROOT = r'D:\MashedWT\pontoon_root'
STACK = {'DEF': [], 'PA': ['MASHED_PONTOON_ANIM'], 'PS': ['MASHED_PONTOON_STATE'], 'PC': ['MASHED_PONTOON_CARRY']}
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


def base_env(frames, track='Training', rule='4'):
    e = clean_env()
    e.update(MASHED_MUTE='1', MASHED_TRACK_VIEW=track, MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE=rule, MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=str(frames))
    return e


def knobs(e, ks):
    for k in ks:
        if '=' in k:
            a, b = k.split('=', 1); e[a] = b
        else:
            e[k] = '1'


def run(exe, name, env, files, limit, cwd_logs=('mashed_re.log', 'player_trace.log', 'motion_diag.log'), wall=None):
    for f in [os.path.join(ROOT, c) for c in cwd_logs] + files:
        if os.path.exists(f): os.remove(f)
    t0 = time.time()
    p = subprocess.Popen([exe], cwd=ROOT, env=env)
    print(f'=== {name} exe={exe} pid={p.pid}', flush=True)
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


def main():
    exe = os.path.abspath(sys.argv[1])
    os.makedirs(OUT, exist_ok=True)
    print(f'exe {exe} sha8 {sha8(exe)}', flush=True)
    for name in sys.argv[2:]:
        if name.startswith('H_'):
            e = base_env(14400); e['MASHED_PLAYERTRACE'] = '1'
            step = os.path.join(OUT, name + '.step.csv'); gates = os.path.join(OUT, name + '.gates.csv')
            e['MASHED_AI_STEPDUMP'] = step; e['MASHED_U9186_GATES'] = gates
            knobs(e, STACK[name[2:].rstrip('0123456789') or 'DEF'])
            run(exe, name, e, [step, gates], 900)
        elif name.startswith('D2_'):
            tag = name[3:].rsplit('_', 1)[0]
            e = clean_env(); e.update(D2BASE); e['MASHED_TITLE'] = 'pontoon-' + name
            knobs(e, STACK[tag])
            run(exe, name, e, [], 0, wall=90)
        elif name.startswith('W_'):
            e = base_env(3600, 'Warzone', '8')
            step = os.path.join(OUT, name + '.step.csv'); pt = os.path.join(OUT, name + '.pontoon.txt')
            sk = os.path.join(OUT, name + '.seek.txt')
            e['MASHED_AI_STEPDUMP'] = step; e['MASHED_PONTOON_TRACE'] = pt; e['MASHED_SEEK_TRACE'] = sk
            knobs(e, STACK[name[2:].rstrip('0123456789') or 'DEF'])
            run(exe, name, e, [step, pt, sk], 900)
        else:
            print('unknown arm', name)
    print('DONE')


if __name__ == '__main__':
    main()
