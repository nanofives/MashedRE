"""lr_exe_poll.py --exe <mashed_re.exe> --root <root> --out <json> --keys e,e,r [--env K=V ...] [--limit 60]

PORT-COLOURSEL-LEFTRIGHT arm 6 (exe side). Spawns the exe with MASHED_GOTO=1 (main menu) and MASHED_NAV_KEYS=<keys>
(exe_main.cpp RunNavKeysStep: one key per 30 frames once the menu is up, then the exe quits), PORT-TOP-04D's settings
baseline restored first. Frida-attached, it polls 0x0067f0c0 (the choice), 0x0067ea98 (the exe colour cursor) and
0x0067e9f8 (menu depth) every 20 ms and records each change. Spawned pid only. Run inside test-slot + mashed_lock.py run.
"""
import argparse, json, os, shutil, time
from pathlib import Path
import frida

ap = argparse.ArgumentParser()
ap.add_argument('--exe', required=True)
ap.add_argument('--root', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--keys', required=True)
ap.add_argument('--env', action='append', default=[])
ap.add_argument('--limit', type=float, default=60)
a = ap.parse_args()
ADDRS = [0x0067f0c0, 0x0067ea98, 0x0067e9f8]
AGENT = r'''
rpc.exports = { rd: function (l) { return l.map(function (x) { try { return ptr(x).readS32(); } catch (e) { return null; } }); } };
'''


def main():
    shutil.copyfile('D:/MashedWT/PORT-TOP-04D_data/settings_baseline.bin', os.path.join(a.root, 'mashed_re_settings.bin'))
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env.update({'MASHED_ROOT': a.root, 'MASHED_GOTO': '1', 'MASHED_NAV_KEYS': a.keys})
    for kv in a.env:
        k, v = kv.split('=', 1); env[k] = v
    res = {'env': {k: v for k, v in env.items() if k.startswith('MASHED_')}, 'changes': []}
    dev = frida.get_local_device()
    pid = dev.spawn(a.exe, cwd=a.root, env=env)
    print('spawned pid', pid, flush=True)
    try:
        s = dev.attach(pid); sc = s.create_script(AGENT); sc.load(); E = sc.exports_sync
        dev.resume(pid)
        t0 = time.time(); last = None
        while time.time() - t0 < a.limit:
            try:
                v = E.rd(ADDRS)
            except Exception:
                break
            if v != last:
                res['changes'].append([round(time.time() - t0, 2)] + v)
                print('t=%.2f f0c0=%s ea98=%s depth=%s' % (time.time() - t0, v[0], v[1], v[2]), flush=True)
                last = v
            time.sleep(0.02)
    finally:
        try:
            dev.kill(pid); print('killed own pid', pid, flush=True)
        except Exception as e:
            print('kill', e, flush=True)
        Path(a.out).write_text(json.dumps(res, indent=1))
        print('->', a.out, flush=True)


if __name__ == '__main__':
    main()
