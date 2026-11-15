# Indirect-transfer target census on the ORIGINAL (stock spawn, no dev asi).
#
# Attaches ONE Interceptor at an indirect jmp/call site, computes the transfer target
# [<reg> + <disp>] in onEnter and keeps a per-target count plus the first few register
# snapshots per target. Built for L156-ECX-RUNTIME-TARGETS (site 0x00556cb0,
# `jmp dword ptr [eax + 0x138]`), but the site/reg/disp are arguments.
#
# Hot-path rule (CLAUDE.md "Frida overhead on hot paths"): the listener detaches itself
# after --max-hits hits, so a hot site costs at most a bounded burst. The per-second hit
# count is printed, so the first run doubles as the call-rate check.
#
# The script is attached before the first resume, so hits from boot on are counted.
# MASHED_ROOT selects a private game root (dev_asi.original_exe). The pid this script
# spawns is the only one it kills.
#
# Usage:
#   py -3.12 re/frida/probe_icall_targets.py --site 0x00556cb0 --reg eax --disp 0x138 \
#       --seconds 20 --max-hits 3000 --out <file.json>
import argparse
import json
import sys
import time
from pathlib import Path

import frida
import psutil

sys.path.insert(0, str(Path(__file__).resolve().parent))
import dev_asi  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent.parent

AGENT = r'''
'use strict';
var SITE = ptr('%SITE%'), REG = '%REG%', DISP = %DISP%, MAXH = %MAXH%, KEEP = 4;
var total = 0, targets = {}, rets = {}, samples = {}, detached = false, lst = null;
lst = Interceptor.attach(SITE, { onEnter: function () {
    var c = this.context, base = c[REG], t;
    try { t = base.add(DISP).readU32(); } catch (e) { t = -1; }
    var k = (t >>> 0).toString(16);
    total++;
    targets[k] = (targets[k] || 0) + 1;
    var rk = k + '<-' + (c.esp.readU32() >>> 0).toString(16) + '@b' + base.toString();
    rets[rk] = (rets[rk] || 0) + 1;
    if (!samples[k]) samples[k] = [];
    if (samples[k].length < KEEP) {
        samples[k].push({ n: total, base: base.toString(), eax: c.eax.toString(),
                          ecx: c.ecx.toString(), edx: c.edx.toString(),
                          esp: c.esp.toString(),
                          ret: c.esp.readU32().toString(16),
                          arg1: c.esp.add(4).readU32().toString(16) });
    }
    if (total >= MAXH && !detached) { detached = true; lst.detach(); }
}});
rpc.exports = { snap: function () {
    return { total: total, targets: targets, rets: rets, samples: samples, detached: detached }; } };
send({ kind: 'ready' });
'''


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--site', required=True)
    ap.add_argument('--reg', default='eax')
    ap.add_argument('--disp', default='0x138')
    ap.add_argument('--seconds', type=int, default=20)
    ap.add_argument('--max-hits', type=int, default=3000)
    ap.add_argument('--out', required=True)
    ap.add_argument('--race', action='store_true',
                    help='drive a Quick Battle race with nav_agent.js (same recipe as nav_to_race.py); '
                         '--seconds then counts from the race start')
    a = ap.parse_args()

    exe = dev_asi.original_exe(ROOT / 'original' / 'MASHED.exe')
    if not (exe.parent / 'd3d9.dll').exists():
        sys.exit(f'FATAL: d3d9 shim missing beside {exe}')
    device = frida.get_local_device()
    proc, session, _ = dev_asi.spawn_original(exe, hooks=False, device=device, resume=False)
    pid = proc.pid
    print(f'spawned {exe} pid {pid}', flush=True)
    det = {}
    session.on('detached', lambda r, c: det.update(reason=r, crash=str(c)[:400] if c else None))
    js = (AGENT.replace('%SITE%', a.site).replace('%REG%', a.reg)
               .replace('%DISP%', str(int(a.disp, 0))).replace('%MAXH%', str(a.max_hits)))
    script = session.create_script(js)
    ready = {}
    script.on('message', lambda m, d: ready.update(ok=True) if m.get('payload', {}).get('kind') == 'ready'
              else print('agent:', m, flush=True))
    script.load()
    t0 = time.time()
    while not ready and time.time() - t0 < 5:
        time.sleep(0.05)
    if not ready:
        device.kill(pid)
        sys.exit('agent never ready')
    device.resume(pid)
    nav = None
    if a.race:
        nav = session.create_script((Path(__file__).resolve().parent / 'nav_agent.js').read_text(encoding='utf-8'))
        nav.load()
        E = nav.exports_sync
        E.init()
        def wait(pred, t):
            end = time.time() + t
            while time.time() < end:
                if pred():
                    return True
                time.sleep(0.1)
            return False
        def press(c, ms=180):
            E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)
        def confirm_to(target, tries=6):
            for _ in range(tries):
                if E.depth() >= target:
                    return True
                press(4)
                if wait(lambda: E.depth() >= target, 2.0):
                    return True
            return E.depth() >= target
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 60):
            print('never reached title', flush=True)
        time.sleep(1.0)
        menu_snap = script.exports_sync.snap()
        print(f'at title: total={menu_snap["total"]} targets={menu_snap["targets"]}', flush=True)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        E.setsel(1); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3:
                break
            press(4); time.sleep(1.5)
        print(f'race phase={E.phase()} ({"in race" if E.phase() != 3 else "NOT in race"})', flush=True)
    rate = []
    last = 0
    snap = {}
    start = time.time()
    while time.time() - start < a.seconds:
        time.sleep(1.0)
        if not psutil.pid_exists(pid):
            print(f'process gone: {det}', flush=True)
            break
        try:
            snap = script.exports_sync.snap()
        except Exception as e:
            print(f'snap err {e}', flush=True)
            break
        t = round(time.time() - start, 1)
        rate.append([t, snap['total'] - last])
        ph = ''
        if nav is not None:
            try:
                ph = f' phase={nav.exports_sync.phase()}'
            except Exception:
                pass
        print(f'[{len(rate)}/{a.seconds}] t={t}{ph} hits/s={snap["total"] - last} total={snap["total"]} '
              f'targets={snap["targets"]} detached={snap["detached"]}', flush=True)
        last = snap['total']
    out = {'exe': str(exe), 'pid': pid, 'site': a.site, 'reg': a.reg, 'disp': a.disp,
           'max_hits': a.max_hits, 'seconds': a.seconds, 'rate_per_1s': rate,
           'race': a.race, 'final': snap, 'detach': det}
    try:
        session.detach()
    except Exception:
        pass
    try:
        proc.kill()
    except Exception:
        pass
    Path(a.out).parent.mkdir(parents=True, exist_ok=True)
    Path(a.out).write_text(json.dumps(out, indent=1), encoding='utf-8')
    print(f'written {a.out}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
