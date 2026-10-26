r"""D4-WSJ-WIRE port arms (copy of verify/wsjmusic_20261026/run_port.py, menu path). PREREG_D4-WSJ-WIRE.md.
usage: py -3.12 run_port.py <name> menu [<secs>]   D2BASE menu path (RACE_DEMO, GOTO=6, TRACK_SEL=12, CAR_SEL=0,
                                                   DRIVE_HOLD), all cars, killed at <secs> wall (default 110), explicit PID
env:   ARM_KNOB=1      -> MASHED_AUDIO_WIRE=1
       ARM_SELFTEST=1  -> MASHED_AUDIO_WIRE_SELFTEST=1 (rows written at boot, when the permdict bank loads)
Wrap every call in: py -3.12 <orch-wt>\re\orchestrator\mashed_lock.py run --label D4-WSJ-WIRE -- <this>
"""
import hashlib, os, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(W, r'mashedmod\build\mashed_re.exe')
ROOT = 'D:/MashedWT/D4-WSJ-WIRE_root'
OUT = 'D:/MashedWT/D4-WSJ-WIRE_data/port'
os.makedirs(OUT, exist_ok=True)
name = sys.argv[1]
if len(sys.argv) < 3 or sys.argv[2] != 'menu':
    sys.exit('usage: run_port.py <name> menu [<secs>]')
wall = int(sys.argv[3]) if len(sys.argv) > 3 else 110
e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_REAL_PHYSICS='1', MASHED_RACE_DEMO='1', MASHED_PLAY_DEMO='1',
         MASHED_GOTO='6', MASHED_TRACK_SEL='12', MASHED_CAR_SEL='0', MASHED_DRIVE_HOLD='1', MASHED_WIN_POS='primary-bl',
         MASHED_AUDIO_EVENT_LOG=os.path.join(OUT, name + '.audio_events.csv'))
if os.environ.get('ARM_KNOB'):
    e['MASHED_AUDIO_WIRE'] = os.environ['ARM_KNOB']
if os.environ.get('ARM_SELFTEST'):
    e['MASHED_AUDIO_WIRE_SELFTEST'] = os.environ['ARM_SELFTEST']
log = os.path.join(ROOT, 'mashed_re.log')
if os.path.exists(log):
    os.remove(log)
exe_sha = hashlib.sha256(open(EXE, 'rb').read()).hexdigest()[:8]
t0 = time.time()
p = subprocess.Popen([EXE], cwd=ROOT, env=e)
print(f'=== {name} pid={p.pid} exe={exe_sha} wall={wall} knob={e.get("MASHED_AUDIO_WIRE", "")} '
      f'selftest={e.get("MASHED_AUDIO_WIRE_SELFTEST", "")}', flush=True)
time.sleep(wall)
if p.poll() is None:
    print(f'    wall {wall}s reached - killing pid {p.pid} ONLY', flush=True)
    p.kill()
rc = p.wait()
if os.path.exists(log):
    import shutil
    shutil.copy2(log, os.path.join(OUT, name + '.log'))
print(f'    exit={rc} secs={time.time() - t0:.0f}', flush=True)
with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
    lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={exe_sha} pid={p.pid} exit={rc} wall={wall}\n')
