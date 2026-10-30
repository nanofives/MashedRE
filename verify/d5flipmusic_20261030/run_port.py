r"""D5-FLIP-MUSIC-NOCDAUDIO copy of verify/wsjmusic_20261026/run_port.py (EXE / ROOT / OUT changed; ARM_NOKNOB=1 sets MASHED_NO_MUSIC_NO_RACE_CDAUDIO). D4-WSJ-MUSIC copy of the D4-WSJ-REF run_port.py (menu path used); ARM_KNOB=1 sets MASHED_MUSIC_NO_RACE_CDAUDIO. Original header: D4-WSJ-REF port-side audio arm: the standalone's default TRAINING race (the run_arms.py base_env recipe:
Training, car 1, round 1, rule 0, game mode 10, deterministic) for DET_FRAMES frames, with the logging-only
MASHED_AUDIO_EVENT_LOG knob pointed at <OUT>/<name>.audio_events.csv. MASHED_MUTE=1 as every arm (the log is
written at the API entry, before the device check). Explicit PID only, never by name.
usage: py -3.12 run_port.py <name> [<frames>]      deterministic track-view race (MASHED_TRACK_VIEW)
       py -3.12 run_port.py <name> menu [<secs>]   the menu path: run_arms.py D2BASE (RACE_DEMO, GOTO=6, TRACK_SEL=12,
                                                   CAR_SEL=0, DRIVE_HOLD) without MEASURE_SOLO / STEER_HOLD (all cars),
                                                   through GameFlow_RequestRace -> RaceSession::Begin; killed at <secs>
                                                   wall (default 110), explicit PID
"""
import hashlib, os, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = 'D:/MashedWT/D5-FLIP-MUSIC-NOCDAUDIO_data/mashed_re_on.exe'
ROOT = 'D:/MashedWT/D5-FLIP-MUSIC-NOCDAUDIO_root'
OUT = 'D:/MashedWT/D5-FLIP-MUSIC-NOCDAUDIO_data/port'
os.makedirs(OUT, exist_ok=True)
name = sys.argv[1]
KNOB = os.environ.get('ARM_KNOB', '')
frames = sys.argv[2] if len(sys.argv) > 2 else '6600'
e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
menu = frames == 'menu'
wall = int(sys.argv[3]) if menu and len(sys.argv) > 3 else 110
if menu:
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_REAL_PHYSICS='1', MASHED_RACE_DEMO='1', MASHED_PLAY_DEMO='1',
             MASHED_GOTO='6', MASHED_TRACK_SEL='12', MASHED_CAR_SEL='0', MASHED_DRIVE_HOLD='1', MASHED_WIN_POS='primary-bl',
             MASHED_AUDIO_EVENT_LOG=os.path.join(OUT, name + '.audio_events.csv'))
else:
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='0', MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=frames, MASHED_AUDIO_EVENT_LOG=os.path.join(OUT, name + '.audio_events.csv'))
if KNOB: e['MASHED_MUSIC_NO_RACE_CDAUDIO'] = KNOB
if os.environ.get('ARM_NOKNOB'): e['MASHED_NO_MUSIC_NO_RACE_CDAUDIO'] = os.environ['ARM_NOKNOB']
log = os.path.join(ROOT, 'mashed_re.log')
if os.path.exists(log): os.remove(log)
exe_sha = hashlib.sha256(open(EXE, 'rb').read()).hexdigest()[:8]
t0 = time.time()
p = subprocess.Popen([EXE], cwd=ROOT, env=e)
print(f'=== {name} pid={p.pid} exe={exe_sha} frames={frames}', flush=True)
try:
    if menu:
        time.sleep(wall)
        if p.poll() is None:
            print(f'    wall {wall}s reached - killing pid {p.pid} ONLY', flush=True); p.kill()
        rc = p.wait()
    else:
        rc = p.wait(900)
except subprocess.TimeoutExpired:
    print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait()
if os.path.exists(log):
    import shutil; shutil.copy2(log, os.path.join(OUT, name + '.log'))
print(f'    exit={rc} secs={time.time() - t0:.0f}', flush=True)
with open(os.path.join(OUT, 'RUNLOG.txt'), 'a') as lf:
    lf.write(f'{time.strftime("%Y-%m-%d %H:%M:%S")} {name} exe={exe_sha} pid={p.pid} exit={rc} frames={frames}\n')
