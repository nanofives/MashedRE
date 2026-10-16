r"""d2close all-tracks replica check. For each TRACK_SEL 0..12: boot the D2 recipe (D2BASE of run_arms.py) for WALL seconds
with MASHED_BATCH_DUMP (player slot), then report from <ROOT>\mashed_re.log whether the COLLI*.BSP collision replica built
("[d2bsp] collision replica: ..." vs "FAILED"), and from the dump how many solver queries returned a non-empty batch.
Explicit PIDs only (kills its own child). usage: py -3.12 tracks_check.py <tag> [<sel> ...]
env: D2C_EXE (exe), D2C_ROOT (private root CONTAINING original\), D2C_OUT, D2C_EXTRA (space separated KEY=VAL knobs)"""
import os, re, shutil, statistics, subprocess, sys, time
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.environ.get('D2C_OUT', os.path.join(W, r'verify\d2close_20261016'))
EXE = os.environ['D2C_EXE']
ROOT = os.environ['D2C_ROOT']
WALL = int(os.environ.get('D2C_WALL', '45'))
D2BASE = {'MASHED_REAL_PHYSICS': '1', 'MASHED_RACE_DEMO': '1', 'MASHED_PLAY_DEMO': '1', 'MASHED_GOTO': '6',
          'MASHED_CAR_SEL': '0', 'MASHED_DRIVE_HOLD': '1', 'MASHED_WIN_POS': 'primary-bl',
          'MASHED_MOTION_DIAG': '1', 'MASHED_STEER_HOLD': '1', 'MASHED_STEER_HOLD_AFTER': '0', 'MASHED_MUTE': '1',
          'MASHED_MEASURE_SOLO': '1'}
tag = sys.argv[1]
sels = [int(x) for x in sys.argv[2:]] or list(range(13))
os.makedirs(OUT, exist_ok=True)
rows = []
for sel in sels:
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e['MASHED_ROOT'] = ROOT
    e.update(D2BASE)
    e['MASHED_TRACK_SEL'] = str(sel)
    e['MASHED_TITLE'] = f'd2close-trk-{tag}-{sel}'
    dump = os.path.join(OUT, f'TRK_{tag}_{sel:02d}.batch.csv')
    e['MASHED_BATCH_DUMP'] = dump
    for kv in os.environ.get('D2C_EXTRA', '').split():
        k, v = kv.split('=', 1); e[k] = v
    for f in (dump, os.path.join(ROOT, 'mashed_re.log'), os.path.join(ROOT, 'motion_diag.log')):
        if os.path.exists(f): os.remove(f)
    p = subprocess.Popen([EXE], cwd=ROOT, env=e)
    time.sleep(WALL)
    if p.poll() is None: p.kill()
    rc = p.wait()
    log = os.path.join(OUT, f'TRK_{tag}_{sel:02d}.log')
    if os.path.exists(os.path.join(ROOT, 'mashed_re.log')):
        shutil.copy2(os.path.join(ROOT, 'mashed_re.log'), log)
    txt = open(log, errors='replace').read() if os.path.exists(log) else ''
    built = re.findall(r'\[d2bsp\] collision replica: (\d+) planes, (\d+) sectors, (\d+) trees, (\d+) materials', txt)
    failed = re.findall(r'\[d2bsp\] collision replica FAILED: (.*)', txt)
    counts = []
    if os.path.exists(dump):
        with open(dump, errors='replace') as fh:
            next(fh, None)
            for ln in fh:
                f = ln.split(',', 9)
                if len(f) > 7 and f[2] == 'Q':
                    try: counts.append(int(f[7]))
                    except ValueError: pass
    nz = sum(1 for c in counts if c > 0)
    row = (f'sel={sel:2d} pid={p.pid} rc={rc} replica={"BUILT " + "/".join(built[-1]) if built else ("FAILED " + failed[-1] if failed else "NO LINE")} '
           f'queries={len(counts)} nonempty={nz} ({(100.0 * nz / len(counts)) if counts else 0:.1f} %) '
           f'median_count={statistics.median(counts) if counts else "NA"} max_count={max(counts) if counts else "NA"}')
    print(row, flush=True)
    rows.append(row)
open(os.path.join(OUT, f'TRACKS_{tag}.txt'), 'w').write('\n'.join(rows) + '\n')
print('DONE')
