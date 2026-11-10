"""exe_snap.py --exe <mashed_re.exe> --root <root> --out <json> --addrs <lo>-<hi>[,<lo>-<hi>] [--env K=V ...]

PORT-SAVE-SPAN-MIRROR diagnostic: the exe nav-demo walk (the env given, e.g. MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1),
Frida-attached at spawn, reads the dword ranges --addrs (original VAs, which the exe maps 1:1) when walk_04_colourselect
.bmp appears under MASHED_VERIFY_OUT, then kills its own pid. PORT-TOP-04D's settings baseline is restored into
<root>/mashed_re_settings.bin before the spawn. Run inside test-slot and mashed_lock.py run.
"""
import argparse, glob, json, os, shutil, time
from pathlib import Path
import frida

ap = argparse.ArgumentParser()
ap.add_argument('--exe', required=True)
ap.add_argument('--root', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--addrs', required=True)
ap.add_argument('--env', action='append', default=[])
ap.add_argument('--limit', type=float, default=120)
a = ap.parse_args()
RANGES = [tuple(int(x, 16) for x in r.split('-')) for r in a.addrs.split(',')]
AGENT = r'''
rpc.exports = { rd: function (lo, hi) { const r = []; for (let x = lo; x < hi; x += 4) { try { r.push(ptr(x).readU32() >>> 0); } catch (e) { r.push(null); } } return r; } };
'''


def main():
    vo = str(Path(a.out).with_suffix('')) + '_verify'
    shutil.rmtree(vo, ignore_errors=True); os.makedirs(vo)
    shutil.copyfile('D:/MashedWT/PORT-TOP-04D_data/settings_baseline.bin', os.path.join(a.root, 'mashed_re_settings.bin'))
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env['MASHED_ROOT'] = a.root
    env['MASHED_VERIFY_OUT'] = vo
    for kv in a.env:
        k, v = kv.split('=', 1); env[k] = v
    res = {'env': {k: v for k, v in env.items() if k.startswith('MASHED_')}, 'snap': {}}
    dev = frida.get_local_device()
    pid = dev.spawn(a.exe, cwd=a.root, env=env)
    print('spawned pid', pid, flush=True)
    try:
        s = dev.attach(pid); sc = s.create_script(AGENT); sc.load(); E = sc.exports_sync
        dev.resume(pid)
        t0 = time.time()
        while time.time() - t0 < a.limit:
            if glob.glob(os.path.join(vo, '**', 'walk_04_colourselect.bmp'), recursive=True):
                break
            time.sleep(0.05)
        res['t'] = round(time.time() - t0, 2)
        for lo, hi in RANGES:
            res['snap']['0x%08x-0x%08x' % (lo, hi)] = E.rd(lo, hi)
        print('snapped at', res['t'], 's', flush=True)
    finally:
        try:
            dev.kill(pid); print('killed own pid', pid, flush=True)
        except Exception as e:
            print('kill', e, flush=True)
        Path(a.out).write_text(json.dumps(res))
        print('->', a.out, flush=True)


if __name__ == '__main__':
    main()
