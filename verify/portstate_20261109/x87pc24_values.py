"""x87pc24_values.py --exe <mashed_re.exe> --map <map> --root <root> --out <json> [--env K=V ...] [--limit S]

PORT-X87-PC24 arm A4b (PREREG_X87PC24.md section 4, per-row values). Spawns the exe on the G-CTRL Training recipe
(the audit's exe CW recipe) and records, per call, the bytes each PC-sensitive port body writes, so a knob-OFF and a
knob-ON run on the same deterministic inputs can be compared call by call:
  TriangleFaceNormal 0x0046c5f0  (apex, p1, p2, out)  -> the 3 inputs and the 3 output floats
  CamCarMidpoint441c80 0x00441c80 (out, cam)          -> the 3 output floats
  CamEntryBuild441b30 0x00441b30 (e, row)             -> 0x80 bytes of e after the call
  CamEntryType0Step445aa0 0x00445aa0 (e, cam)         -> 0x80 bytes of e after the call
Interceptor on these four only (<= ~90 calls/s, below the hot-path limit). Spawned pid only, killed by pid.
Run under test-slot (outside) and mashed_lock.py run (inside).
"""
import argparse, hashlib, json, os, sys, time
from pathlib import Path
import frida

ap = argparse.ArgumentParser()
ap.add_argument('--exe', required=True); ap.add_argument('--map', required=True)
ap.add_argument('--root', required=True); ap.add_argument('--out', required=True)
ap.add_argument('--env', action='append', default=[]); ap.add_argument('--limit', type=float, default=180)
a = ap.parse_args()

WANT = {'?TriangleFaceNormal@Collision@mashed_re@@YAXPBM00PAM@Z': 'tri', '_CamCarMidpoint441c80': 'mid',
        '_CamEntryBuild441b30': 'build', '_CamEntryType0Step445aa0': 'step0'}
sym, pref = {}, 0x400000
for l in Path(a.map).read_text(errors='replace').splitlines():
    p = l.split()
    if 'Preferred load address is' in l:
        pref = int(l.split()[-1], 16)
    if len(p) >= 4 and ':' in p[0] and p[1] in WANT:
        sym.setdefault(WANT[p[1]], int(p[2], 16) - pref)
assert len(sym) == 4, sym

JS = r"""
const base = Process.getModuleByName('%s').base;
const R = %s;
const buf = {tri: [], mid: [], build: [], step0: []};
function hx(p, n) { return Array.from(new Uint8Array(p.readByteArray(n))).map(b => ('0' + b.toString(16)).slice(-2)).join(''); }
Interceptor.attach(base.add(R.tri), { onEnter(args) { this.a = [args[0], args[1], args[2], args[3]]; this.i = hx(args[0], 12) + hx(args[1], 12) + hx(args[2], 12); },
  onLeave() { buf.tri.push(this.i + ':' + hx(this.a[3], 12)); } });
Interceptor.attach(base.add(R.mid), { onEnter(args) { this.o = args[0]; }, onLeave() { buf.mid.push(hx(this.o, 12)); } });
Interceptor.attach(base.add(R.build), { onEnter(args) { this.o = args[0]; }, onLeave() { buf.build.push(hx(this.o, 0x80)); } });
Interceptor.attach(base.add(R.step0), { onEnter(args) { this.o = args[0]; }, onLeave() { buf.step0.push(hx(this.o, 0x80)); } });
rpc.exports = { take() { const r = {tri: buf.tri.splice(0), mid: buf.mid.splice(0), build: buf.build.splice(0), step0: buf.step0.splice(0)}; return r; } };
"""

env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
env['MASHED_ROOT'] = a.root
env.update({'MASHED_TRACK_VIEW': 'Training', 'MASHED_CAR': '1', 'MASHED_ROUND': '1', 'MASHED_ROUND_RULE': '0',
            'MASHED_GAME_MODE': '10', 'MASHED_DETERMINISTIC': '1', 'MASHED_DET_FRAMES': '1200',
            'MASHED_DET_NO_AMBIENT_KEYS': '1', 'MASHED_MUTE': '1', 'MASHED_WIN_POS': 'primary-bl'})
for kv in a.env:
    k, _, v = kv.partition('='); env[k] = v or '1'
dev = frida.get_local_device()
pid = dev.spawn(a.exe, cwd=a.root, env=env)
print(f'spawned pid={pid}', flush=True)
res = {k: [] for k in WANT.values()}
try:
    s = dev.attach(pid)
    sc = s.create_script(JS % (Path(a.exe).name, json.dumps(sym)))
    sc.load()
    dev.resume(pid)
    end = time.time() + a.limit
    while time.time() < end:
        time.sleep(2)
        try:
            r = sc.exports_sync.take()
        except Exception as e:
            print('process gone:', type(e).__name__, flush=True); break
        for k in res: res[k] += r[k]
finally:
    try: dev.kill(pid); print(f'killed own pid {pid}', flush=True)
    except Exception: pass
out = {'env': {k: v for k, v in env.items() if k.startswith('MASHED_')}, 'exe_sha8': hashlib.sha256(open(a.exe, 'rb').read()).hexdigest()[:8],
       'counts': {k: len(v) for k, v in res.items()}, 'calls': res}
json.dump(out, open(a.out, 'w'))
print('counts', out['counts'], flush=True)
