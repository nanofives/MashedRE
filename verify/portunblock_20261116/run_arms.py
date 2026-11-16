"""PORT-UNBLOCK-RWGLOBALS arms: knob boots on the RW core frame route, VS2022 and object build, MASHED_RWC_RWGSWAP off / on.

usage: py -3.12 verify/portunblock_20261116/run_arms.py [arm ...]
Each arm: test-slot outside, mashed_lock inside, verify/porttop_20261108/knob_boot.py on the private root. Exes are the
copies saved under D:/MashedWT/PORT-UNBLOCK-RWGLOBALS_data/{vs,ddk}. Logs go to D:/MashedWT/PORT-UNBLOCK-RWGLOBALS_data.
"""
import os
import subprocess
import sys

W = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
ORCH = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration'
DATA = 'D:/MashedWT/PORT-UNBLOCK-RWGLOBALS_data'
ROOT = r'D:\MashedWT\PORT-UNBLOCK-RWGLOBALS_root'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
BASE = ['MASHED_ORIG_TOP=1', 'MASHED_RWC_CORE=1', 'MASHED_RWC_FRAME=1', 'MASHED_RWC_FRAME_N=120', 'MASHED_OT_TIMER=1',
        'MASHED_DET_FRAMES=600']
ARMS = {
    'vs_off': ('vs', []), 'vs_on': ('vs', ['MASHED_RWC_RWGSWAP=1']),
    'ddk_off': ('ddk', []), 'ddk_on': ('ddk', ['MASHED_RWC_RWGSWAP=1']),
}


def main(names):
    for n in names or list(ARMS):
        b, extra = ARMS[n]
        env = dict(os.environ, KNOB_EXE=os.path.join(DATA, b, 'mashed_re.exe'), PORTTOP_OUT=DATA)
        cmd = [sys.executable, SLOT, sys.executable, os.path.join(ORCH, 're', 'orchestrator', 'mashed_lock.py'), 'run',
               '--label', 'PORT-UNBLOCK-RWGLOBALS', '--', sys.executable,
               os.path.join(W, 'verify', 'porttop_20261108', 'knob_boot.py'), 'rwg_' + n, ROOT, '600'] + BASE + extra
        print('[arm] %s: %s' % (n, ' '.join(cmd)), flush=True)
        rc = subprocess.call(cmd, cwd=W, env=env)
        print('[arm] %s rc %d' % (n, rc), flush=True)


if __name__ == '__main__':
    main(sys.argv[1:])
