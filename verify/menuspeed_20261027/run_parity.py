# D4-FU-MENUSPEED PREREG section 3d: menu parity, screen 1. Standalone draw streams at frame 60 (mid-animation) and
# 400 (settled) for the default speed and MASHED_MENU_ANIM_SPEED=6, one original burst (settle 6 s) from the private
# root, then drawlist_diff: re-vs-re (like for like) and original-vs-re. Kills only the PIDs it spawns.
# Usage (under test-slot + mashed_lock.py): py -3.12 verify/menuspeed_20261027/run_parity.py [re|orig|diff ...]
import json, os, subprocess, sys, time
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
ROOT = Path('D:/MashedWT/D4-FU-MENUSPEED_root')
OUT = Path('D:/MashedWT/D4-FU-MENUSPEED_data/parity')
EXE = WT / 'mashedmod/build/mashed_re.exe'
MAP = WT / 'mashedmod/build/mashed_re.map'
RE_RUNS = [(f'{tag}_f{f}', spd, f) for tag, spd in (('def', None), ('x6', '6')) for f in (60, 400)]


def clean_env():
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith(('MASHED_', 'MENU_ANIM_'))}
    e['MASHED_ROOT'] = str(ROOT)
    return e


def wait_stable(p, since, timeout):
    end = time.time() + timeout; last = -1
    while time.time() < end:
        if p.exists() and p.stat().st_mtime >= since:
            sz = p.stat().st_size
            if sz > 0 and sz == last:
                return True
            last = sz
        time.sleep(1.0)
    return False


def run_re():
    for i, (name, spd, f) in enumerate(RE_RUNS, 1):
        out = OUT / f're_{name}.json'
        if out.exists(): out.unlink()
        e = clean_env()
        e.update(MASHED_GOTO='1', MASHED_DBG_DRAWSTREAM=f'{f}:{f + 1}', MASHED_DBG_DRAWSTREAM_OUT=str(out),
                 MASHED_SETTINGS_DEFAULTS='1', MASHED_MUTE='1', MASHED_WIN_POS='primary-bl')
        if spd: e['MASHED_MENU_ANIM_SPEED'] = spd
        since = time.time()
        p = subprocess.Popen([str(EXE)], cwd=str(ROOT), env=e, stdin=subprocess.DEVNULL,
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        ok = wait_stable(out, since, 60)
        try: p.kill()
        except Exception: pass
        p.wait(timeout=10)
        print(f'[{i}/{len(RE_RUNS)}] re {name} pid={p.pid} {"OK" if ok else "NODATA"}', flush=True)


def run_orig():
    for attempt in range(3):   # U-9677 piz collisions / Frida spawn deaths are re-runs
        out = OUT / 'orig_s1.json'
        e = clean_env(); e.pop('MASHED_ROOT'); e['MASHED_ROOT'] = str(ROOT)
        rc = subprocess.call([sys.executable, str(WT / 're/frida/menu_draw_burst.py'), '--screen', '1', '--frames', '4',
                              '--settle', '6', '--out', str(out)], cwd=str(WT), env=e, timeout=240,
                             stdout=open(OUT / f'orig_s1.attempt{attempt}.log', 'w'), stderr=subprocess.STDOUT)
        ok = out.exists() and out.stat().st_size > 0
        print(f'[orig attempt {attempt + 1}] rc={rc} {"OK" if ok else "NODATA"}', flush=True)
        if ok: return


def diff(a, b, label, extra):
    js = OUT / f'diff_{label}.json'
    cmd = [sys.executable, str(WT / 're/tools/drawlist_diff.py'), str(a), str(b), '--map', str(MAP), '--json', str(js)] + extra
    p = subprocess.run(cmd, cwd=str(WT), capture_output=True, text=True, errors='replace')
    (OUT / f'diff_{label}.txt').write_text(p.stdout + p.stderr, encoding='utf-8')
    tail = [l for l in (p.stdout + p.stderr).splitlines() if l.strip()][-3:]
    print(f'diff {label} rc={p.returncode} :: ' + ' | '.join(tail), flush=True)


def run_diff():
    like = ['--scale-a', '1', '--scale-b', '1']
    orig = ['--scale-b', '0.8', '--exclude-tex', '9', '--rotate-a', '0x42e65a', '--tol-anim', '4']
    for f in (60, 400):
        diff(OUT / f're_def_f{f}.json', OUT / f're_x6_f{f}.json', f're_def_vs_x6_f{f}', like)
    for tag in ('def', 'x6'):
        diff(OUT / 'orig_s1.json', OUT / f're_{tag}_f400.json', f'orig_vs_{tag}_f400', orig)


if __name__ == '__main__':
    OUT.mkdir(parents=True, exist_ok=True)
    steps = sys.argv[1:] or ['re', 'orig', 'diff']
    for s in steps:
        {'re': run_re, 'orig': run_orig, 'diff': run_diff}[s]()
