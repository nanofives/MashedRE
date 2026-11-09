"""orig_walk04.py --root <root> --out <dir> [--tag <name>] [--snap 0x...,...]

PORT-TOP-04D-WALK04: the original's side of the exe nav-demo walk up to walk_04_colourselect (exe_main.cpp
RunNavDemoStep steps 0..6: title -> Enter -> main menu -> Enter (Single Player, cursor 0) -> Enter (Challenge Cup,
cursor 0) -> colour select). Boots <root>/original/MASHED.exe (hooks OFF, MASHED_RE_NO_AUTO_HOOK=1), presses confirm
(control 4, player 0) through re/frida/nav_agent.js (the in-process FUN_00497310 return override), and dumps each
settled screen through the d3d9 shim's on-demand channel (MASHED_ORIG_BBDUMP_REQ): walk_01_title, walk_02_mainmenu,
walk_03_singleplayer, walk_04_colourselect (.bmp, 640x480 backbuffer, the same channel class as the exe's
MASHED_DBG_BBDUMP). At each dump it also snapshots the dwords listed by --snap (default: the 0x007f0a40 table head,
0x007f0f10, 0x007f0f2c..0x007f0f3c), so the 0x004924f0 defaults can be read on the original.
Spawned pid only, killed by pid. Run inside test-slot and mashed_lock.py run.
"""
import argparse, json, os, time
from pathlib import Path
import frida

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[2]
ap = argparse.ArgumentParser()
ap.add_argument('--root', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--tag', default='orig1')
ap.add_argument('--snap', default='')
a = ap.parse_args()
OUT = Path(a.out) / a.tag
OUT.mkdir(parents=True, exist_ok=True)
REQ = OUT / 'dump.req'
SNAP = [int(x, 16) for x in a.snap.split(',') if x] or (
    [0x007f0a40 + 4 * i for i in range(16)] + [0x007f0f10, 0x007f0f2c, 0x007f0f30, 0x007f0f34, 0x007f0f38, 0x007f0f3c])


def main():
    res = {'argv': [a.root, a.tag], 'steps': []}
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env.update({'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_MUTE': '1', 'MASHED_ORIG_BBDUMP_REQ': str(REQ)})
    if REQ.exists():
        REQ.unlink()
    exe = Path(a.root) / 'original' / 'MASHED.exe'
    dev = frida.get_local_device()
    pid = dev.spawn(str(exe), cwd=str(exe.parent), env=env)
    res['pid'] = pid
    print('spawned pid', pid, flush=True)
    try:
        s = dev.attach(pid)
        nav = s.create_script((REPO / 're' / 'frida' / 'nav_agent.js').read_text(encoding='utf-8'))
        nav.load()
        N = nav.exports_sync
        N.init()
        dev.resume(pid)

        def wait(pred, t):
            end = time.time() + t
            while time.time() < end:
                if pred():
                    return True
                time.sleep(0.1)
            return False

        def dump(name, settle=2.0):
            time.sleep(settle)
            bmp = OUT / f'walk_{name}.bmp'
            if bmp.exists():
                bmp.unlink()
            REQ.write_text(str(bmp) + '\n')
            ok = wait(lambda: not REQ.exists() and bmp.exists(), 6.0)
            step = {'name': name, 'ok': ok, 'phase': N.phase(), 'depth': N.depth(),
                    'snap': {('0x%08x' % x): v for x, v in zip(SNAP, N.snap(SNAP))}}
            res['steps'].append(step)
            print('dump', name, 'ok' if ok else 'FAILED', 'phase', step['phase'], 'depth', step['depth'], flush=True)

        def confirm_to(target, tries=6):
            for _ in range(tries):
                if N.depth() >= target:
                    return True
                N.press(4, 180)
                time.sleep(0.5)
                if wait(lambda: N.depth() >= target, 2.5):
                    return True
            return N.depth() >= target

        if not wait(lambda: N.phase() == 3 and N.depth() >= 1, 60):
            raise SystemExit('never reached the title (phase 3)')
        d0 = N.depth()
        res['title_depth'] = d0
        dump('01_title')
        confirm_to(d0 + 1)                     # press button -> main menu
        dump('02_mainmenu', settle=3.0)
        d1 = N.depth()
        confirm_to(d1 + 1)                     # main menu cursor 0 -> Single Player
        dump('03_singleplayer')
        d2 = N.depth()
        confirm_to(d2 + 1)                     # Single Player cursor 0 -> Challenge Cup -> colour select
        dump('04_colourselect')
    finally:
        try:
            dev.kill(pid)
            print('killed own pid', pid, flush=True)
        except Exception as e:
            print('kill', e, flush=True)
        (OUT / 'walk.json').write_text(json.dumps(res, indent=1))
        print('->', OUT / 'walk.json', flush=True)


if __name__ == '__main__':
    main()
