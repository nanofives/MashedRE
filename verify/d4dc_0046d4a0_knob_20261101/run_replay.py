"""PREREG amendment 2 arms: the mode 7 M3 callback replay (synth_unicorn.cb.tsv) with the knob OFF / ON.

Recipe = verify/d3_mode7_m3_20261010/run_m3_replay.ps1 (synth set, PC_24), on the worktree exe and the private root.
Spawns and waits on ONLY its own PID. usage: py -3.12 run_replay.py RP_DEF RP_PK46
"""
import os, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(W, 'mashedmod', 'build', 'mashed_re.exe')
ROOT = os.path.normpath(os.environ.get('A4_ROOT', 'D:/MashedWT/D4-DC-INC-0046d4a0-KNOB_root'))
OUT = os.path.normpath(os.environ.get('A4_OUT', 'D:/MashedWT/D4-DC-INC-0046d4a0-KNOB_data/arms'))
SYNTH = os.path.join(W, 'verify', 'd3_mode7_m3_20261010', 'synth_unicorn.cb.tsv')

for name in sys.argv[1:]:
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='4', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1', MASHED_DET_FRAMES='60',
             MASHED_PICKUPS_PC='24', MASHED_PKCB_REPLAY=SYNTH,
             MASHED_PKCB_REPLAY_OUT=os.path.join(OUT, name + '.replay.txt'),
             MASHED_PK_ONEBODY_46D4A0_WIT=os.path.join(OUT, name + '.pk46.tsv'))
    if name == 'RP_PK46':
        e['MASHED_PK_ONEBODY_46D4A0'] = '1'
    t0 = time.time()
    p = subprocess.Popen([EXE], cwd=ROOT, env=e)
    try:
        rc = p.wait(600); ok = True
    except subprocess.TimeoutExpired:
        p.kill(); rc = p.wait(15); ok = False
    line = '%s %s pid=%d exit=%s self_exited=%s secs=%.0f knob=%s' % (time.strftime('%Y-%m-%d %H:%M:%S'), name, p.pid,
                                                                       rc, ok, time.time() - t0, e.get('MASHED_PK_ONEBODY_46D4A0', '0'))
    print(line, flush=True)
    open(os.path.join(OUT, 'RUNLOG_replay.txt'), 'a').write(line + '\n')
