"""run_oppcol.py orig|exe   (run from the worktree root, inside test-slot; each boot takes mashed_lock itself)

PORT-COLOURSEL-OPPCOL arm drivers.
orig = arm 1b: menu_draw_burst.py --nav to the cup-flow colour select, choice N, the colour confirm (0xff1d0000: commit
  0x0043e56c..0x0043e57e, FUN_00431d00 at 0x0043e58e, DAT_0067ea78 = 0 at 0x0043e593), then 3 s on the challenge select
  (no launch). "pre1" stops before the confirm. Peeks the FUN_00431d00 globals, DAT_0067ea78, the slot colours, the
  choice, depth and mode.
exe = arms 2/3: the keys of PORT-COLOURSEL-COMMIT arm 2 (MASHED_GOTO=1, e,e to the cup-flow colour select, the moves,
  e = colour confirm, e = challenge confirm, 41 'x' pads so the race loads), oppcol_exe_poll.py recording the
  FUN_00431d00 globals, DAT_0067ea78, 0x007f1a1c/2c/3c/4c and the livery lines.
"""
import subprocess, sys
D = 'D:/MashedWT/PORT-COLOURSEL-OPPCOL_data'
ROOT = 'D:/MashedWT/PORT-COLOURSEL-OPPCOL_root'
LOCK = ['py', '-3.12', 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py',
        'run', '--label', 'PORT-COLOURSEL-OPPCOL', '--']
PEEK = '0x0067ea74,0x0067ea78,0x0067ea90,0x0067ea94,0x0067ea98,0x0067ea9c,0x0067eaa0,0x0067eaac,0x007f1a1c,0x007f1a2c,0x0067f0c0,0x0067e9f8,0x0067e9fc'


def run(cmd, log, env=None):
    import os
    e = dict(os.environ); e.update(env or {})
    with open(log, 'w') as f:
        rc = subprocess.call(LOCK + cmd, stdout=f, stderr=subprocess.STDOUT, env=e)
    return rc


def orig(labels):
    specs = {'pre1': 'confirm,confirm,confirm,wait:3',
             'cs1': 'confirm,confirm,confirm,confirm,wait:3',
             'cs3': 'confirm,confirm,confirm,right,right,confirm,wait:3'}
    for lab in labels or specs:
        rc = run(['py', '-3.12', 're/frida/menu_draw_burst.py', '--nav', specs[lab], '--frames', '2', '--label', lab,
                  '--out', f'{D}/orig_{lab}.json', '--peek', PEEK], f'{D}/orig_{lab}.log', {'MASHED_ROOT': ROOT})
        print(lab, 'rc=%d' % rc, flush=True)
        for l in open(f'{D}/orig_{lab}.log', errors='replace'):
            if 'peek after nav' in l: print(l.rstrip(), flush=True)


def exe(labels):
    pad = ','.join(['x'] * 41)
    specs = {'on_n1': ('on', ''), 'on_n3': ('on', 'r,r,'), 'on_n6': ('on', 'r,' * 7), 'on_n5l': ('on', 'r,' * 6 + 'l,'),
             'off_n1': ('off', ''), 'off_n3': ('off', 'r,r,')}
    for lab in labels or specs:
        knob, mv = specs[lab]
        cmd = ['py', '-3.12', 'verify/portstate_20261109/coloursel/oppcol_exe_poll.py', '--exe', f'{D}/exe/mashed_re.exe',
               '--root', ROOT, '--out', f'{D}/exe_{lab}.json', '--keys', f'e,e,{mv}e,e,{pad}', '--limit', '90']
        if knob == 'on': cmd += ['--env', 'MASHED_COLOURSEL_ICON=1']
        rc = run(cmd, f'{D}/exe_{lab}.log')
        print(lab, 'rc=%d' % rc, flush=True)
        for l in open(f'{D}/exe_{lab}.log', errors='replace'):
            if 'livery: slot 0' in l or 'livery: slot 1' in l: print(l.rstrip(), flush=True)


if __name__ == '__main__':
    {'orig': orig, 'exe': exe}[sys.argv[1]](sys.argv[2:])
