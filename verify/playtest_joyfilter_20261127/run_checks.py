"""PLAYTEST-JOYFILTER checks (run inside test-slot + mashed_lock).

usage: py -3.12 verify/playtest_joyfilter_20261127/run_checks.py nav|gates
  nav   : two nav-demo runs (MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1) of the worktree exe, filter on (default) and
          MASHED_ALL_CONTROLLERS=1 (the original's count rule). Prints the walk_04 (colour select) hash of each and the
          new joystick-filter log lines.
  gates : run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26, then sha256[:8] as merge_train.ps1 (G-CTRL: H_DEF step /
          gates / ptrace, B: each arm's .step.csv).
"""
import hashlib, os, shutil, subprocess, sys

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
ROOT = 'D:/MashedWT/playtest-joyfilter_root'
DATA = 'D:/MashedWT/playtest-joyfilter_data'
BASE = 'D:/MashedWT/PORT-TOP-04E_data/settings_baseline.bin'


def h8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8] if os.path.exists(p) else 'MISSING'


def nav():
    arms = [('filter_on', []), ('all_controllers', ['MASHED_ALL_CONTROLLERS=1'])]
    for n, (lab, extra) in enumerate(arms, 1):
        out = os.path.join(DATA, 'nav', lab)
        os.makedirs(out, exist_ok=True)
        shutil.copyfile(BASE, ROOT + '/mashed_re_settings.bin')
        env = dict(os.environ, PORTTOP_OUT=os.path.join(DATA, 'nav'), PYTHONWARNINGS='ignore')
        env.pop('MASHED_ALL_CONTROLLERS', None)
        kv = ['MASHED_NAV_DEMO=1', 'MASHED_DETERMINISTIC=1', 'MASHED_VERIFY_OUT=' + out] + extra
        p = subprocess.run(['py', '-3.12', 'verify/porttop_20261108/knob_boot.py', lab, ROOT, '300'] + kv, cwd=W,
                           env=env, capture_output=True, text=True)
        print(p.stdout.strip().splitlines()[:2])
        caps = sorted(x for x in os.listdir(out) if x.startswith('walk_') and x.endswith('.bmp'))
        w4 = [x for x in caps if x.startswith('walk_04_')]
        w4h = hashlib.sha256(open(os.path.join(out, w4[0]), 'rb').read()).hexdigest()[:16] if w4 else 'MISSING'
        print('[%d/2] %s captures %d walk_04 %s %s' % (n, lab, len(caps), w4[0] if w4 else '-', w4h), flush=True)
        logf = os.path.join(DATA, 'nav', lab + '.log')
        if os.path.exists(logf):
            for ln in open(logf, encoding='latin-1'):
                if 'game controller' in ln or 'input slots' in ln:
                    print('   log:', ln.rstrip())


def gates():
    out = os.path.join(DATA, 'gates')
    os.makedirs(out, exist_ok=True)
    env = dict(os.environ, A4_EXE=os.path.join(W, 'mashedmod', 'build', 'mashed_re.exe'), A4_ROOT=ROOT, A4_OUT=out)
    arms = ['H_DEF', 'B_DEF_K0', 'B_DEF_K1', 'B_DEF_K26']
    rc = subprocess.run(['py', '-3.12', os.path.join(W, 'verify', 'savelink_20261023', 'run_arms.py')] + arms,
                        cwd=W, env=env).returncode
    print('run_arms rc', rc)
    print('G-CTRL', ' / '.join(h8(os.path.join(out, f)) for f in ('H_DEF.step.csv', 'H_DEF.gates.csv', 'H_DEF.ptrace.log')))
    print('B', ' / '.join(h8(os.path.join(out, a + '.step.csv')) for a in arms[1:]))


if __name__ == '__main__':
    {'nav': nav, 'gates': gates}[sys.argv[1]]()
