r"""D4-WSJ-REF port-side audio arm: the standalone's default TRAINING race (the run_arms.py base_env recipe:
Training, car 1, round 1, rule 0, game mode 10, deterministic) for DET_FRAMES frames, with the logging-only
MASHED_AUDIO_EVENT_LOG knob pointed at <OUT>/<name>.audio_events.csv. MASHED_MUTE=1 as every arm (the log is
written at the API entry, before the device check). Explicit PID only, never by name.
usage: py -3.12 run_port.py <name> [<frames>]
"""
import hashlib, os, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(W, r'mashedmod\build\mashed_re.exe')
ROOT = 'D:/MashedWT/D4-WSJ-REF_root'
OUT = 'D:/MashedWT/D4-WSJ-REF_data/port'
os.makedirs(OUT, exist_ok=True)
name = sys.argv[1]
frames = sys.argv[2] if len(sys.argv) > 2 else '6600'
e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
         MASHED_ROUND_RULE='0', MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
         MASHED_DET_FRAMES=frames, MASHED_AUDIO_EVENT_LOG=os.path.join(OUT, name + '.audio_events.csv'))
log = os.path.join(ROOT, 'mashed_re.log')
if os.path.exists(log): os.remove(log)
exe_sha = hashlib.sha256(open(EXE, 'rb').read()).hexdigest()[:8]
t0 = time.time()
p = subprocess.Popen([EXE], cwd=ROOT, env=e)
print(f'=== {name} pid={p.pid} exe={exe_sha} frames={frames}', flush=True)
try:
    rc = p.wait(900)
except subprocess.TimeoutExpired:
    print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait()
if os.path.exists(log):
    import shutil; shutil.copy2(log, os.path.join(OUT, name + '.log'))
print(f'    exit={rc} secs={time.time() - t0:.0f}', flush=True)
with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
    lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={exe_sha} pid={p.pid} exit={rc} frames={frames}\n')
