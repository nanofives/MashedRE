"""PORT-TOP-KNOBON-CAPFLAKE driver (call it inside test-slot + mashed_lock).

usage: capflake_runs.py <arm> <spec> [<spec> ...]
  spec = ON | OFF | ON@<ms> | OFF@<ms> | ON@dflt | OFF@dflt
         ON/OFF = MASHED_ORIG_TOP=1 or unset; @<ms> = MASHED_DET_MPG_PIN=<ms> (no @ = pin OFF)
Each spec is one nav-demo run (MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1) with the settings baseline restored first.
Appends one row per run to <D>/runs.tsv: arm, run, top knob, pin, freeze line (mode/pos/hash), capture class, 12 hashes.
Capture class: E = the 04E list, X = the 04G f2 / 04J p1 outlier list, '?' = neither (n of 12 equal to E is listed).
"""
import hashlib, os, re, shutil, subprocess, sys

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
D = os.environ.get('CAPFLAKE_DATA', 'D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_data')
R = os.environ.get('CAPFLAKE_ROOT', 'D:/MashedWT/PORT-TOP-KNOBON-CAPFLAKE_rootK')
BASE = 'D:/MashedWT/PORT-TOP-04E_data/settings_baseline.bin'
E = ['83c8cbb913899494', '24249b644e3e8057', 'feb2c803554cd028', '2871487f1fcef51c', '9f02cb91d09e0500',
     'b1f095c07072b28d', '3465159db8526481', '843e3fc49d166754', '5db0164fb2c6df8c', 'ca2f64dc5e7a9407',
     '536f710d21e50619', '6d41bc3ac8ad8d8a']
X = ['83c8cbb913899494', '2b5487e980ea0a9f', 'e2a771a4c1404c0b', '618605e4c0b573dc', 'e4adbce36b493a28',
     'b90fc1e3ecf29079', '0fcd519ce249dd6c', '8a597ad0454c531b', '7857383c6a3a7b3d', '9bf9a687b0b89709',
     'ea58b5762132eec1', '27d967126f477737']


def caps(out):
    hs = []
    for i in range(1, 13):
        k = 'walk_%02d_' % i
        f = [x for x in os.listdir(out) if x.startswith(k) and x.endswith('.bmp')] if os.path.isdir(out) else []
        hs.append(hashlib.sha256(open(os.path.join(out, f[0]), 'rb').read()).hexdigest()[:16] if f else 'MISSING')
    return hs


def main():
    arm, specs = sys.argv[1], sys.argv[2:]
    ad = os.path.join(D, arm)
    os.makedirs(ad, exist_ok=True)
    tsv = os.path.join(D, 'runs.tsv')
    if not os.path.exists(tsv):
        open(tsv, 'w').write('arm\trun\ttop\tpin\trc\tfreeze\tclass\tn_eq_E\thashes\n')
    for n, spec in enumerate(specs, 1):
        knob, _, pin = spec.partition('@')
        lab = 'r%02d_%s%s' % (n, knob, ('_pin' + pin) if pin else '')
        out = os.path.join(ad, lab)
        os.makedirs(out, exist_ok=True)
        shutil.copyfile(BASE, R + '/mashed_re_settings.bin')
        kv = ['MASHED_NAV_DEMO=1', 'MASHED_DETERMINISTIC=1', 'MASHED_VERIFY_OUT=' + out]
        if knob == 'ON':
            kv.append('MASHED_ORIG_TOP=1')
        if pin != 'dflt':   # @dflt = leave it to knob_boot.py (HB-CAPTURE-MPGPIN default: 0)
            kv.append('MASHED_DET_MPG_PIN=' + pin)   # empty = off
        env = dict(os.environ, PORTTOP_OUT=ad, PYTHONWARNINGS='ignore')
        env.pop('MASHED_ORIG_TOP', None)
        env.pop('MASHED_DET_MPG_PIN', None)
        p = subprocess.run(['py', '-3.12', 'verify/porttop_20261108/knob_boot.py', lab, R, '300'] + kv, cwd=W, env=env,
                           capture_output=True, text=True)
        m = re.search(r'rc=(-?\d+)', p.stdout)
        rc = m.group(1) if m else '?'
        logf = os.path.join(ad, lab + '.log')
        fz = '-'
        if os.path.exists(logf):
            for ln in open(logf, encoding='latin-1'):
                if 'F1 frontend.mpg det freeze:' in ln:
                    fz = ln.split('det freeze:', 1)[1].strip()
        hs = caps(out)
        cls = 'E' if hs == E else ('X' if hs == X else '?')
        neq = sum(a == b for a, b in zip(hs, E))
        row = [arm, lab, knob, pin or '-', rc, fz, cls, str(neq), ','.join(hs)]
        open(tsv, 'a').write('\t'.join(row) + '\n')
        print('[%d/%d] %s rc=%s %s class=%s eqE=%d' % (n, len(specs), lab, rc, fz, cls, neq), flush=True)


if __name__ == '__main__':
    main()
