# D4-FU-MENUSPEED .asi arms (PREREG section 3b): re/frida/menu_anim_diff.py against the private root, one boot per arm.
# Usage (wrapped by test-slot + mashed_lock.py): py -3.12 verify/menuspeed_20261027/run_anim_arms.py [ARM ...]
import json, os, subprocess, sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
ROOT = Path('D:/MashedWT/D4-FU-MENUSPEED_root')
OUT = Path('D:/MashedWT/D4-FU-MENUSPEED_data/arms')
OLD_ASI = 'D:/MashedWT/D4-FU-MENUSPEED_data/old_mashed_re_dev.asi'
ARMS = {
    'A0':  {},
    'A1':  {'MENU_ANIM_DT': '0.016666666'},
    'A2':  {'MENU_ANIM_DT': '0.03333333'},
    'A3':  {'MENU_ANIM_DT': '0.3'},
    'A3c': {'MENU_ANIM_DT': '0.3', 'MENU_ANIM_ASI': OLD_ASI},
    'A4':  {'MENU_ANIM_DT': '0.016666666', 'MASHED_MENU_ANIM_SPEED': '6'},
}


def run(name):
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith(('MASHED_', 'MENU_ANIM_'))}
    env['MASHED_ROOT'] = str(ROOT)
    env['MENU_ANIM_OUT'] = str(OUT / (name + '.json'))
    env.update(ARMS[name])
    log = open(OUT / (name + '.log'), 'w')
    rc = subprocess.call([sys.executable, str(WT / 're/frida/menu_anim_diff.py')], cwd=str(WT), env=env,
                         stdout=log, stderr=subprocess.STDOUT, timeout=180)
    log.close()
    try:
        d = json.load(open(OUT / (name + '.json')))
    except Exception:
        return rc, None
    s = d['samples']
    return rc, {'n': len(s), 'green': d['green'], 'settle': sum(r.get('settle', 0) for r in s),
                'first': next((r for r in s if r['diff'] != -1 or r['ra'] != r['rb']), None)}


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    names = sys.argv[1:] or list(ARMS)
    summ = {}
    for i, n in enumerate(names, 1):
        for attempt in range(3):   # piz collisions / Frida spawn deaths (U-9677) are re-runs
            rc, r = run(n)
            if r and r['n'] == 20:
                break
        status = 'NODATA' if not r or r['n'] == 0 else ('GREEN' if r['green'] == r['n'] else 'RED')
        summ[n] = {'rc': rc, 'status': status, 'attempts': attempt + 1, **(r or {})}
        print(f"[{i}/{len(names)}] {n} {status} {r['green'] if r else 0}/{r['n'] if r else 0} settle={r['settle'] if r else 0}", flush=True)
    (OUT / 'summary.json').write_text(json.dumps(summ, indent=1))
    print(json.dumps(summ, indent=1))


if __name__ == '__main__':
    main()
