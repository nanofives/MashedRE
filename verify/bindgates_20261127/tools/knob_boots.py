"""BIND-00420420 knob boots (copy of verify/bindgates_20261121/tools/knob_boots.py, data/root/label changed): verify/porttop_20261108/knob_boot.py on the VS2022 exe and the object-build exe, base env
(cp1: MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000) and with every all-knobs sub-knob on
(MASHED_OT_RACEFRAME, MASHED_OT_RENDERFRAME + MASHED_RWC_FRAME, MASHED_OT_RL_D110, MASHED_OT_TITLETICK). Each run is wrapped
as test-slot (outside) + mashed_lock.py run (inside). Exes are the copies kept in the data dir. Run from the worktree root:
    py -3.12 verify/bindgates_20261121/tools/knob_boots.py [label ...]
"""
import os
import subprocess
import sys

DATA = 'D:/MashedWT/BIND-00420420_data'
ROOT = 'D:/MashedWT/BIND-00420420_root'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
BASE = ['MASHED_ORIG_TOP=1', 'MASHED_OT_RACELOAD=1', 'MASHED_DET_FRAMES=3000']
ALL = ['MASHED_OT_RACEFRAME=1', 'MASHED_OT_RENDERFRAME=1', 'MASHED_RWC_FRAME=1', 'MASHED_OT_RL_D110=1',
       'MASHED_OT_TITLETICK=1']
RUNS = [('knob_vs_base', 'mashed_re_vs.exe', []), ('knob_vs_all', 'mashed_re_vs.exe', ALL),
        ('knob_ddk_base', 'mashed_re_ddk.exe', []), ('knob_ddk_all', 'mashed_re_ddk.exe', ALL)]
KEYS = ('rc=', 'self-test', 'returned', 'FATAL', 'test-slot', 'test-hub', 'sub-knob', 'BIND-GATES')


def main():
    sel = sys.argv[1:] or [r[0] for r in RUNS]
    todo = [r for r in RUNS if r[0] in sel]
    for i, (label, exe, extra) in enumerate(todo, 1):
        env = dict(os.environ, KNOB_EXE='%s/%s' % (DATA, exe), PORTTOP_OUT=DATA)
        cmd = ['py', '-3.12', SLOT, 'py', '-3.12', LOCK, 'run', '--label', 'BIND-00420420', '--',
               'py', '-3.12', 'verify/porttop_20261108/knob_boot.py', label, ROOT, '900'] + BASE + extra
        print('[%d/%d] %s %s %s' % (i, len(todo), label, exe, ' '.join(BASE + extra)), flush=True)
        r = subprocess.run(cmd, env=env, capture_output=True, text=True, errors='replace')
        out = r.stdout + r.stderr
        with open('%s/%s.out' % (DATA, label), 'w', encoding='utf-8') as f:
            f.write(out)
        # the wrappers exit 0 whatever the game did: the verdict is the game's own rc (knob_boot.py "rc=N") and the
        # frame count line, not r.returncode
        import re
        m = re.search(r'%s: (\w+).*? rc=(-?\d+)' % label, out)
        game_rc = int(m.group(2)) if m else None
        ok = r.returncode == 0 and game_rc == 0 and m.group(1) == 'exited'
        print('[%d/%d] %s %s (wrapper rc %d, game rc %s)' % (i, len(todo), label, 'PASS' if ok else 'FAIL', r.returncode,
                                                              game_rc), flush=True)
        for ln in out.splitlines():
            if any(k in ln for k in KEYS):
                print('    ' + ln[:260], flush=True)


if __name__ == '__main__':
    main()
