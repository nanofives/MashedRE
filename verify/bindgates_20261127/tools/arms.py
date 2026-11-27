"""BIND-00420420 arms: count_body.py under test-slot (outside) + mashed_lock.py run (inside), private root with the staged
original/mashed_image.bin (SHA-256 81cc7775..., KNOBON-BOOTS-G5's), object build and VS2022 exes kept in the data dir.
Envs are the ones `A4_PLAN=1 run_arms.py RO_G5D110 / RO_G5D110C` prints (ROLOADRWC + MASHED_OT_RL_D110=1 [+ count stops]).
    py -3.12 verify/bindgates_20261127/tools/arms.py [label ...]
"""
import subprocess
import sys

DATA = 'D:/MashedWT/BIND-00420420_data'
ROOT = r'D:\MashedWT\BIND-00420420_root'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
SYMS = '_Orig_0040d270,_PortTop_Bound_0040d110,_PortTop_Bound_00420420,_PortTop_RL_0040d110,_PortTop_Bound_0047b8d0'
WT = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\BIND-00420420'
BASE = ['MASHED_DETERMINISTIC=1', 'MASHED_DET_FRAMES=600', 'MASHED_ORIG_TOP=1', 'MASHED_OT_RACELOAD=1', 'MASHED_OT_RL_D110=1',
        'MASHED_PORTSTATE_IMAGE=1', r'MASHED_REPLAY=%s\verify\replayoracle_20261113\replay_trace.json' % WT,
        'MASHED_REPLAY_TRACE_CKPT=1,150,299,301,450,600', 'MASHED_RWC_CORE=1', 'MASHED_RWC_FRAME=1',
        'MASHED_RWC_FRAME_N=100000', 'MASHED_WIN_POS=primary-bl']
RUNS = [('RO_G5D110_ddk', 'mashed_re_ddk', []), ('RO_G5D110C_ddk', 'mashed_re_ddk', ['MASHED_OT_RL_STOPS=count']),
        ('RO_G5D110C_vs', 'mashed_re_vs', ['MASHED_OT_RL_STOPS=count'])]


def main():
    sel = sys.argv[1:] or [r[0] for r in RUNS]
    todo = [r for r in RUNS if r[0] in sel]
    for i, (label, exe, extra) in enumerate(todo, 1):
        env = BASE + extra + [r'MASHED_REPLAY_TRACE=D:\MashedWT\BIND-00420420_data\arms\%s' % label]
        cmd = ['py', '-3.12', SLOT, 'py', '-3.12', LOCK, 'run', '--label', 'BIND-00420420', '--', 'py', '-3.12',
               'verify/bindgates_20261127/tools/count_body.py', '%s/%s.exe' % (DATA, exe), '%s/%s.map' % (DATA, exe), ROOT,
               '900', '%s/count_%s.txt' % (DATA, label), SYMS] + env
        print('[%d/%d] %s start' % (i, len(todo), label), flush=True)
        r = subprocess.run(cmd, capture_output=True, text=True, errors='replace')
        out = r.stdout + r.stderr
        try:
            log = open(ROOT + r'\mashed_re.log', encoding='utf-8', errors='replace').read()
            open('%s/arms/%s.log' % (DATA, label), 'w', encoding='utf-8').write(log)
        except OSError:
            pass
        keep = [l for l in out.splitlines() if l.startswith(('COUNT', 'ended', 'timeout', '[mashed-lock]', '[test-'))]
        print('[%d/%d] %s done (wrapper rc %d)' % (i, len(todo), label, r.returncode), flush=True)
        for l in keep:
            print('    ' + l, flush=True)


if __name__ == '__main__':
    main()
