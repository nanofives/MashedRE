# D4-FU-MENUSPEED amendment 1 arms E0-E2: exe menu anim timing via MASHED_MENU_ANIM_LOG, MASHED_GOTO=8, private root.
# Kills only the PID it spawns. Usage (under test-slot + mashed_lock.py): py -3.12 verify/menuspeed_20261027/run_exe_timing.py
import json, os, subprocess, sys, time
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
ROOT = Path('D:/MashedWT/D4-FU-MENUSPEED_root')
OUT = Path('D:/MashedWT/D4-FU-MENUSPEED_data/exe_timing')
EXE = WT / 'mashedmod/build/mashed_re.exe'
ARMS = [('E0', {}), ('E1', {'MASHED_MENU_CLOCK_FAITHFUL': '1'}),
        ('E2', {'MASHED_MENU_CLOCK_FAITHFUL': '1', 'MASHED_MENU_ANIM_SPEED': '6'})]


def parse(path):
    # last "# screen 8" block: rows (ms, dt, [(tag, slide, type)])
    blocks, cur = [], None
    for line in open(path):
        if line.startswith('# screen'):
            cur = {'screen': int(line.split()[2]), 'rows': []}; blocks.append(cur); continue
        if cur is None: continue
        f = line.split()
        cur['rows'].append((float(f[0]), float(f[1]), [tuple(int(v, 16) if j == 0 else int(v) for j, v in enumerate(c.split(':'))) for c in f[2:]]))
    b = [x for x in blocks if x['screen'] == 8]
    return b[-1] if b else None


def summarize(blk):
    rows = blk['rows']; res = {'calls': len(rows), 'dt_nonzero': sum(1 for r in rows if r[1] != 0.0)}
    for want, key in ((0xff040000, 'item'), (0xff000000, 'back')):
        t1 = next((i for i, r in enumerate(rows) if any(c[0] == want and c[2] == 1 for c in r[2])), None)
        fz = next((i for i, r in enumerate(rows) if t1 is not None and i > t1 and
                   all(c[2] == 0x1000 for c in r[2] if c[0] == want)), None)
        # rows are logged AFTER the tick: ticks applied at row i = number of nonzero-dt rows in 0..i
        tk = lambda i: None if i is None else sum(1 for r in rows[:i + 1] if r[1] != 0.0)
        res[key] = {'settle_call': None if t1 is None else t1 + 1, 'settle_ticks': tk(t1), 'settle_ms': None if t1 is None else rows[t1][0],
                    'freeze_call': None if fz is None else fz + 1, 'freeze_ticks': tk(fz), 'freeze_ms': None if fz is None else rows[fz][0]}
    return res


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    summ = {}
    for i, (name, extra) in enumerate(ARMS, 1):
        log = OUT / f'{name}.animlog.txt'
        if log.exists(): log.unlink()
        e = {k: v for k, v in os.environ.items() if not k.upper().startswith(('MASHED_', 'MENU_ANIM_'))}
        e.update(MASHED_ROOT=str(ROOT), MASHED_GOTO='8', MASHED_MENU_ANIM_LOG=str(log), MASHED_SETTINGS_DEFAULTS='1',
                 MASHED_MUTE='1', MASHED_WIN_POS='primary-bl'); e.update(extra)
        p = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=e, stdin=subprocess.DEVNULL,
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(12)
        try: p.kill()
        except Exception: pass
        p.wait(timeout=10)
        blk = parse(log) if log.exists() else None
        summ[name] = summarize(blk) if blk and blk['rows'] else None
        print(f'[{i}/{len(ARMS)}] {name} pid={p.pid} ' + json.dumps(summ[name]), flush=True)
    (OUT / 'summary.json').write_text(json.dumps(summ, indent=1))


if __name__ == '__main__':
    main()
