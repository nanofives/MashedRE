r"""D4-FX-SLOT2 original-side per-frame alive / slot-word series (extends verify/fxinputs_20261026/orig_fx_inputs.py).

Same launch recipe (scenario_launch.py Training, 4 cars, mode 10, rule 0, car 1). CModule probes, no JS per call:
  FUN_0040ca00 entry -> one 24-byte row per call (the census counted one call per frame):
       [0] DAT_0063ba8c (low byte)        [1] DAT_00771968 phase byte    [2] alive mask, bit c = rec(c)+0x4 != 0
       [3] +0x10 mask, bit c = rec(c)+0x10 == 1                          [4..7] slot word [0x007f1a14 + c*0x10] (s8)
       [8..11] ctrl byte 5 of block slot(c) (0x007f1038 + slot*0x4c + 5; 0 when slot < 0)
       [12..15] ctrl byte 0xa of the same block                          [16..23] AI step calls per v (u16, v = 0..3)
     rec(c) = 0x008815a0 + c*0xd04 (FUN_0046d570's base, orig_fx_inputs.py).
  FUN_00416250 entry (spline, v, blk), phase byte == 3 -> AI step calls per v (the aistep probe's gate).
  Events (frame, rva tag, arg0, arg1, arg2, arg3): FUN_00422fd0 entry, FUN_0040eee0 entry, FUN_0040d590 entry.
usage: py -3.12 orig_alive_series.py <hold_s> <out_prefix> [extra scenario_launch args]   (env MASHED_ROOT = private root that contains original\)
writes <out_prefix>.rows.bin (24 B per frame), <out_prefix>.events.json, <out_prefix>.json (snapshots + meta)
"""
import json, os, re, subprocess, sys, threading, time
import frida

HOLD, OUT = int(sys.argv[1]), sys.argv[2]
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SL = os.path.join(W, 're', 'frida', 'scenario_launch.py')
NMAX = 20000
EMAX = 512

CSRC = r'''
#include <gum/guminterceptor.h>
extern int frames;
extern unsigned char rows[480000];
extern unsigned int aic[4];
extern unsigned int d[1];
extern int nev;
extern int ev[3072];
void on_ca00(GumInvocationContext *ic) {
  int c;
  unsigned char *r;
  if (frames >= 20000) { frames++; return; }
  r = rows + frames * 24;
  r[0] = *(unsigned char *)(gsize)(0x0063ba8cu + d[0]);
  r[1] = *(unsigned char *)(gsize)(0x00771968u + d[0]);
  r[2] = 0; r[3] = 0;
  for (c = 0; c < 4; c++) {
    unsigned int rec = 0x008815a0u + d[0] + (unsigned int)c * 0xd04u;
    int slot = *(int *)(gsize)(0x007f1a14u + d[0] + (unsigned int)c * 0x10u);
    if (*(int *)(gsize)(rec + 0x4u) != 0) r[2] |= (unsigned char)(1 << c);
    if (*(int *)(gsize)(rec + 0x10u) == 1) r[3] |= (unsigned char)(1 << c);
    r[4 + c] = (unsigned char)(signed char)slot;
    if (slot >= 0 && slot < 16) {
      unsigned int blk = 0x007f1038u + d[0] + (unsigned int)slot * 0x4cu;
      r[8 + c] = *(unsigned char *)(gsize)(blk + 5u);
      r[12 + c] = *(unsigned char *)(gsize)(blk + 0xau);
    } else { r[8 + c] = 0; r[12 + c] = 0; }
    r[16 + 2 * c] = (unsigned char)(aic[c] & 0xff);
    r[17 + 2 * c] = (unsigned char)((aic[c] >> 8) & 0xff);
  }
  frames++;
}
void on_step(GumInvocationContext *ic) {
  int v = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 1);
  if (*(unsigned char *)(gsize)(0x00771968u + d[0]) != 3) return;
  if (v >= 0 && v < 4) aic[v]++;
}
static void put(int tag, GumInvocationContext *ic) {
  int *e;
  if (nev >= 512) return;
  e = ev + nev * 6;
  e[0] = frames; e[1] = tag;
  e[2] = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 0);
  e[3] = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 1);
  e[4] = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 2);
  e[5] = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 3);
  nev++;
}
void on_kill(GumInvocationContext *ic) { put(0x422fd0, ic); }
void on_eee0(GumInvocationContext *ic) { put(0x40eee0, ic); }
void on_d590(GumInvocationContext *ic) { put(0x40d590, ic); }
'''

JS = r'''
const IMG = 0x400000;
const m = Process.findModuleByName('MASHED.exe');
const d = m.base.toUInt32() - IMG;
const sym = {};
const sizes = {frames: 4, rows: 480000, aic: 16, d: 4, nev: 4, ev: 12288};
for (const k in sizes) { sym[k] = Memory.alloc(sizes[k]); for (let i = 0; i < sizes[k]; i += 4) sym[k].add(i).writeU32(0); }
sym.d.writeU32(d >>> 0);
Process.setExceptionHandler(function (ex) { send({exc: ex.type, addr: ex.address.toString(), rel: '0x' + (ex.address.toUInt32() - d).toString(16)}); return false; });
const cm = new CModule(CSRC, sym);
Interceptor.attach(ptr(0x0040ca00 + d), cm.on_ca00);
Interceptor.attach(ptr(0x00416250 + d), cm.on_step);
Interceptor.attach(ptr(0x00422fd0 + d), cm.on_kill);
Interceptor.attach(ptr(0x0040eee0 + d), cm.on_eee0);
Interceptor.attach(ptr(0x0040d590 + d), cm.on_d590);
function arr(p, n) { const a = []; for (let i = 0; i < n; i++) a.push(p.add(4 * i).readS32()); return a; }
rpc.exports = {
  snap: function () {
    return JSON.stringify({frames: sym.frames.readS32(), aic: arr(sym.aic, 4), nev: sym.nev.readS32(),
      phase: ptr(0x00771968 + d).readU8(), ba8c: ptr(0x0063ba8c + d).readS32(), fd0: ptr(0x007f0fd0 + d).readS32(),
      slots: [0, 1, 2, 3].map(function (v) { return ptr(0x007f1a14 + d + v * 0x10).readS32(); })});
  },
  rows: function () { const n = Math.min(sym.frames.readS32(), 20000); return sym.rows.readByteArray(n * 24); },
  events: function () { return arr(sym.ev, sym.nev.readS32() * 6); }
};
'''.replace('CSRC', json.dumps(CSRC))

args = [sys.executable, SL, '--track', '0', '--mode', '10', '--cars', '4', '--car', '1', '--rule', '0', '--hold', str(HOLD)] + sys.argv[3:]
print('launch:', ' '.join(args), '| MASHED_ROOT =', os.environ.get('MASHED_ROOT'), flush=True)
p = subprocess.Popen(args, cwd=W, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
pid = None
def pump():
    global pid
    for ln in p.stdout:
        print('  [sl]', ln.rstrip(), flush=True)
        mm = re.search(r'scenario_launch\s+pid=(\d+)', ln)
        if mm and pid is None: pid = int(mm.group(1))
threading.Thread(target=pump, daemon=True).start()
t0 = time.time()
while pid is None and time.time() - t0 < 120 and p.poll() is None: time.sleep(0.5)
if pid is None: print('no pid'); sys.exit(2)
print('attach pid', pid, flush=True)
s = frida.get_local_device().attach(pid); sc = s.create_script(JS)
sc.on('message', lambda m, d: print('  [js]', m, flush=True)); sc.load(); E = sc.exports_sync
snaps = []; t1 = time.time(); rows = None; evs = None
try:
    while time.time() - t1 < HOLD - 8 and p.poll() is None:
        time.sleep(10)
        r = json.loads(E.snap()); r['t'] = round(time.time() - t1); snaps.append(r)
        print('  t=%d phase=%d ba8c=%d frames=%d aic=%s slots=%s nev=%d' % (r['t'], r['phase'], r['ba8c'], r['frames'], r['aic'], r['slots'], r['nev']), flush=True)
        rows = E.rows(); evs = E.events()   # every snapshot: a --statediff-out launcher can end the game before the loop does
except Exception as e:
    print('stopped:', e, flush=True)
if rows is not None:
    open(OUT + '.rows.bin', 'wb').write(rows)
    ev = [evs[i:i + 6] for i in range(0, len(evs), 6)]
    json.dump([{'frame': e[0], 'fn': '0x%08x' % e[1], 'args': e[2:]} for e in ev], open(OUT + '.events.json', 'w'), indent=0)
json.dump({'pid': pid, 'args': args, 'snaps': snaps}, open(OUT + '.json', 'w'), indent=1)
print('wrote', OUT, 'rows' if rows is not None else 'NO ROWS', flush=True)
try: s.detach()
except Exception: pass
p.wait()
print('scenario_launch exit', p.returncode, flush=True)
