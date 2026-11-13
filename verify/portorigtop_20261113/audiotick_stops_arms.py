"""PORT-TOP-AUDIOTICK-STOPS arms: knob boots of this row's two exes (VS2022 and the object build).
Run inside test-slot -> mashed_lock.py run. Kills only the PIDs knob_boot.py spawns (on its own timeout).
usage: py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py [arm ...]   (default: all arms)"""
import os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
WT = os.path.normpath(os.path.join(HERE, '..', '..'))
STEP = 'PORT-TOP-AUDIOTICK-STOPS'
ROOT = 'D:/MashedWT/%s_root' % STEP
DDK = os.path.join(WT, 'mashedmod', 'build', 'ddkobj', 'mashed_re_ddk.exe')
COMMON = ['MASHED_WIN_POS=primary-bl', 'MASHED_DET_FRAMES=600', 'MASHED_ORIG_TOP=1', 'MASHED_OT_TIMER=1']
ARMS = [
    # name, object build?, extra knobs
    ('vs_audio', False, ['MASHED_OT_AUDIOTICK=1']),                              # control: tick only (gate closed)
    ('ddk_audio', True, ['MASHED_OT_AUDIOTICK=1']),
    ('vs_init_audio', False, ['MASHED_OT_AUDIOINIT=1', 'MASHED_OT_AUDIOTICK=1']),  # the row: both knobs together
    ('ddk_init_audio', True, ['MASHED_OT_AUDIOINIT=1', 'MASHED_OT_AUDIOTICK=1']),
]

def main(argv):
    want = set(argv) or {a[0] for a in ARMS}
    env = dict(os.environ, PORTTOP_OUT='D:/MashedWT/%s_data' % STEP)
    rc_all = 0
    for name, ddk, extra in ARMS:
        if name not in want:
            continue
        e = dict(env)
        if ddk:
            e['KNOB_EXE'] = DDK
        cmd = [sys.executable, 'verify/porttop_20261108/knob_boot.py', name, ROOT, '180'] + COMMON + extra
        print('[arm] %s: %s' % (name, ' '.join(cmd[2:])), flush=True)
        rc = subprocess.call(cmd, cwd=WT, env=e)
        print('[arm] %s rc=%d' % (name, rc), flush=True)
        rc_all = rc_all or rc
    return rc_all

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
