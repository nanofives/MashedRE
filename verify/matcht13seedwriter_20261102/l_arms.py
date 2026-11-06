"""MATCH-T13-SEED-WRITER port arms (PREREG_MATCH-T13-SEED-WRITER.md). usage: py -3.12 l_arms.py NAME ...
NAME = {L|C}_{SANDS|NEU|CITY}. Base env = T13-SEED p_arms.py. Explicit PID only."""
import hashlib, os, shutil, subprocess, sys, time
D = 'D:/MashedWT/MATCH-T13-SEED-WRITER_data'
EXE = os.environ.get('A4_EXE', D + '/exe/mashed_re.exe')
ROOT = os.path.normpath('D:/MashedWT/MATCH-T13-SEED-WRITER_root')
OUT = os.path.join(D, 'larms'); os.makedirs(OUT, exist_ok=True)
ARM = {'SANDS': ('sands', '5', '5'), 'NEU': ('Neustein', '4', '2'), 'CITY': ('City', '4', '12')}
FRAMES = os.environ.get('P_FRAMES', '2400')


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8]


for name in sys.argv[1:]:
    kind, trk = name.split('_')
    view, mode, row = ARM[trk]
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW=view, MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='10', MASHED_GAME_MODE=mode, MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=FRAMES, MASHED_RULE10_SEED_LOG='1', MASHED_RULE10_SEED_DIFF='1',
             MASHED_DIFF360_ROW=row, MASHED_DIFF360_INIT_TABLE='1')
    if kind == 'L':
        e['MASHED_DIFF360_LIVE'] = '1'
    log = os.path.join(ROOT, 'mashed_re.log')
    if os.path.exists(log): os.remove(log)
    t0 = time.time()
    p = subprocess.Popen([EXE], cwd=ROOT, env=e)
    print(f'=== {name} pid={p.pid} exe={sha8(EXE)}', flush=True)
    try:
        rc = p.wait(600)
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait()
    dst = os.path.join(OUT, name + '.log')
    if os.path.exists(log): shutil.copy2(log, dst)
    print(f'    exit={rc} secs={time.time() - t0:.0f}', flush=True)
    if os.path.exists(dst):
        for l in open(dst, encoding='latin-1'):
            if l.startswith('[rule10-seed]') or l.startswith('[diff360]'):
                print('    ' + l.rstrip(), flush=True)
