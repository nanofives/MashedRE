"""PREREG section 3 replay arms (mode 7 M3 callback replay, synth set) for D4-DC-INC-0046d4a0-REPLAYORDER.

Recipe = verify/d4dc_0046d4a0_knob_20261101/run_replay.py (itself verify/d3_mode7_m3_20261010/run_m3_replay.ps1, synth
set), with the exe chosen per arm. Spawns and waits on ONLY its own PID.
usage: py -3.12 run_replay.py RP_FIX_OFF RP_FIX_ON RP_FLIP RP_FLIP53   (A4_DET_FRAMES=600 for the PREREG fallback)
"""
import os, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DATA = 'D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data'
ROOT = os.path.normpath(os.environ.get('A4_ROOT', 'D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_root'))
OUT = os.path.normpath(os.environ.get('A4_OUT', DATA + '/arms'))
SYNTH = os.path.join(W, 'verify', 'd3_mode7_m3_20261010', 'synth_unicorn.cb.tsv')
EXE = {'FIX': os.path.normpath(DATA + '/exe_fix/mashed_re.exe'), 'FLIP': os.path.normpath(DATA + '/exe_flip/mashed_re.exe')}
ARMS = {  # name: (exe, extra env)
    'RP_FIX_OFF': ('FIX', {'MASHED_PK_ONEBODY_46D4A0_WIT': '@OUT@/@NAME@.pk46.tsv'}),
    'RP_FIX_ON':  ('FIX', {'MASHED_PK_ONEBODY_46D4A0': '1', 'MASHED_PK_ONEBODY_46D4A0_WIT': '@OUT@/@NAME@.pk46.tsv'}),
    'RP_FLIP':    ('FLIP', {}),
    'RP_FLIP53':  ('FLIP', {'MASHED_PICKUPS_PC': '53'}),
}
os.makedirs(OUT, exist_ok=True)
for name in sys.argv[1:]:
    tag, extra = ARMS[name]
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='4', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=os.environ.get('A4_DET_FRAMES', '60'),
             MASHED_PICKUPS_PC='24', MASHED_PKCB_REPLAY=SYNTH,
             MASHED_PKCB_REPLAY_OUT=os.path.join(OUT, name + '.replay.txt'))
    for k, v in extra.items():
        e[k] = v.replace('@OUT@', OUT).replace('@NAME@', name)
    rep = os.path.join(OUT, name + '.replay.txt')
    if os.path.exists(rep):
        os.remove(rep)
    t0 = time.time()
    p = subprocess.Popen([EXE[tag]], cwd=ROOT, env=e)
    try:
        rc = p.wait(600); ok = True
    except subprocess.TimeoutExpired:
        p.kill(); rc = p.wait(15); ok = False
    line = '%s %s exe=%s pid=%d exit=%s self_exited=%s secs=%.0f det=%s | %s' % (
        time.strftime('%Y-%m-%d %H:%M:%S'), name, tag, p.pid, rc, ok, time.time() - t0, e['MASHED_DET_FRAMES'],
        ' '.join('%s=%s' % (k, e[k]) for k in sorted(e) if k.startswith('MASHED_') and k not in ('MASHED_ROOT',)))
    print(line, flush=True)
    open(os.path.join(OUT, 'RUNLOG_replay.txt'), 'a').write(line + '\n')
    if os.path.exists(rep):
        print(''.join(l for l in open(rep) if l.startswith(('G-M3-REPLAY', 'arms:', 'MISMATCH')))[:2000], flush=True)
    else:
        print('  no replay report', flush=True)
