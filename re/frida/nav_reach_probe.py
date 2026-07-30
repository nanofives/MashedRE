#!/usr/bin/env py -3.12
# nav_reach_probe.py — measure the depth 2->3 menu transition in isolation.
#
# The depth 2->3 step (Game Title Screen -> Single-Player mode-select, across
# the async 'Load-Successful' modal) is the ~50% nav-flake point that caps every
# boot lane (STATE batch, Stalker, scenario). This probe boots stock, drives
# title -> depth 2 -> depth 3 with ONE chosen transition, and reports whether it
# reached depth 3, how many confirm presses it cost, and the wall time. Kills
# ONLY the pid it spawned (multi-session hygiene).
#
#   --mode current   the pre-2026-07-29 sequence: confirm_to_depth(2);
#                    sleep .3; press(4); sleep .5; confirm_to_depth(3)
#   --mode hardened  confirm_to_depth(2); advance_past_load_modal(3)
#
# Usage: py -3.12 re/frida/nav_reach_probe.py --mode current|hardened

import os
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import statenav
from statenav import Nav

EXE = statenav.EXE
ORIG = statenav.ORIG


def transition_current(nav):
    nav.confirm_to_depth(2)
    time.sleep(0.3)
    nav.press(4)
    time.sleep(0.5)
    return nav.confirm_to_depth(3)


def transition_hardened(nav):
    nav.confirm_to_depth(2)
    return nav.advance_past_load_modal(3)


def main():
    mode = sys.argv[sys.argv.index('--mode') + 1] if '--mode' in sys.argv else 'hardened'
    transition = {'current': transition_current, 'hardened': transition_hardened}[mode]

    env = dict(os.environ)
    env['MASHED_RE_NO_AUTO_HOOK'] = '1'   # stock; input-drive only
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f'  spawned pid={pid} mode={mode} (kills ONLY this pid)')
    sess = dev.attach(pid)
    scr = sess.create_script(statenav.AGENT)
    scr.on('message', lambda m, d: None)
    scr.load()
    scr.exports_sync.init()

    # count confirm presses issued during the transition
    presses = {'n': 0}
    nav = Nav(scr, pid)
    _raw_press = nav.press
    def _counting_press(c, ms=180):
        if c == 4:
            presses['n'] += 1
        return _raw_press(c, ms)
    nav.press = _counting_press

    reached = False
    t0 = time.time()
    try:
        dev.resume(pid)
        if not nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 18.0, 'title up'):
            print(f'RESULT mode={mode} reached=False presses=0 secs={time.time()-t0:.1f} '
                  f'FAIL=title-never-up depth={nav.depth()} phase={nav.phase()}')
            return 2
        t_trans = time.time()
        reached = transition(nav)
        secs = time.time() - t_trans
        print(f'RESULT mode={mode} reached={reached} presses={presses["n"]} '
              f'trans_secs={secs:.1f} depth={nav.depth()} phase={nav.phase()}')
    finally:
        try: dev.kill(pid)
        except Exception: pass
        print(f'  killed pid={pid}')
    return 0 if reached else 1


if __name__ == '__main__':
    sys.exit(main())
