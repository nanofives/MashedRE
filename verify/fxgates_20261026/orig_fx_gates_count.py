r"""D4-FX-GATES original-side counter (PREREG_fxgates.md section 3, arm 3).

Launches re/frida/scenario_launch.py (Training, 4 cars, mode 10, rule 0, car 1) against MASHED_ROOT, reads the pid it
prints, and attaches entry-only counters written as a CModule (no JS per call, the hot-path rule):
  FUN_0040ca00 entries                     -> frames (one call per race frame from FUN_0040ce00)
  FUN_0040c690 entries                     -> calls[car][skid], colour histogram per car (arg 4 = rec+0x1f0+w*0xc4)
  FUN_0048cf90 entries returning into FUN_0040c690 (0x0040c690..0x0040c9fa)
                                           -> emits[car][skid], effect-id histogram per car, attributed to the
                                              preceding FUN_0040c690 call (single-threaded call chain)
Samples every 10 s; also reads [0x00636adc] (FUN_0040c690's early-out word) and the phase byte 0x00771968.
Kills nothing itself: scenario_launch kills only the pid it launched.
usage: py -3.12 orig_fx_gates_count.py <hold_s> <out.json>   (env MASHED_ROOT = private root that contains original\)
"""
import json, os, re, subprocess, sys, threading, time
import frida

HOLD, OUT = int(sys.argv[1]), sys.argv[2]
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SL = os.path.join(W, 're', 'frida', 'scenario_launch.py')

CSRC = r'''
#include <gum/guminterceptor.h>
extern int frames;
extern int calls[8];
extern int emits[8];
extern unsigned int cols[64];   /* 4 cars x 16 (colour) */
extern int colsn[64];
extern int ids[128];            /* 4 cars x 32 */
extern unsigned int lo;
extern int st[4];               /* last_car, last_skid, pending: CModule statics are not writable */
#define last_car st[0]
#define last_skid st[1]
#define pending st[2]
void on_ca00(GumInvocationContext *ic) { frames++; }
void on_c690(GumInvocationContext *ic) {
  unsigned int col = (unsigned int)(gsize)gum_invocation_context_get_nth_argument(ic, 3);
  int skid = ((int)(gsize)gum_invocation_context_get_nth_argument(ic, 4)) != 0;
  int car = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 5);
  int i;
  if (car < 0 || car > 3) car = 3;
  calls[car * 2 + skid]++;
  last_car = car; last_skid = skid; pending = 1;
  for (i = 0; i < 16; i++) {
    int k = car * 16 + i;
    if (colsn[k] == 0) { cols[k] = col; colsn[k] = 1; break; }
    if (cols[k] == col) { colsn[k]++; break; }
  }
}
void on_cf90(GumInvocationContext *ic) {
  unsigned int ra = *(unsigned int *)(gsize)ic->cpu_context->esp;   /* probe context: [esp] = return address */
  st[3]++;
  int id = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 2);
  if (pending && ra - lo < 0x36a) {
    emits[last_car * 2 + last_skid]++;
    ids[last_car * 32 + (id & 31)]++;
    pending = 0;
  }
}
'''

JS = r'''
const IMG = 0x400000;
const m = Process.findModuleByName('MASHED.exe');
const d = m.base.toUInt32() - IMG;
const sym = {};
const sizes = {frames: 4, calls: 32, emits: 32, cols: 256, colsn: 256, ids: 512, lo: 4, st: 16};
for (const k in sizes) { sym[k] = Memory.alloc(sizes[k]); for (let i = 0; i < sizes[k]; i += 4) sym[k].add(i).writeU32(0); }
sym.lo.writeU32(0x0040c690 + d);
Process.setExceptionHandler(function (ex) { send({exc: ex.type, addr: ex.address.toString(), rel: '0x' + (ex.address.toUInt32() - d).toString(16), mem: ex.memory ? ex.memory.address.toString() : ''}); return false; });
const cm = new CModule(CSRC, sym);
Interceptor.attach(ptr(0x0040ca00 + d), cm.on_ca00);
Interceptor.attach(ptr(0x0040c690 + d), cm.on_c690);
Interceptor.attach(ptr(0x0048cf90 + d), cm.on_cf90);
function arr(p, n) { const a = []; for (let i = 0; i < n; i++) a.push(p.add(4 * i).readS32()); return a; }
rpc.exports = {
  snap: function () {
    const cols = [];
    for (let c = 0; c < 4; c++) { const h = {};
      for (let i = 0; i < 16; i++) { const n = sym.colsn.add(4 * (c * 16 + i)).readS32();
        if (n) h['0x' + (sym.cols.add(4 * (c * 16 + i)).readU32() >>> 0).toString(16)] = n; }
      cols.push(h); }
    return JSON.stringify({frames: sym.frames.readS32(), calls: arr(sym.calls, 8), emits: arr(sym.emits, 8), cf90_all: sym.st.add(12).readS32(),
      cols: cols, ids: arr(sym.ids, 128), phase: ptr(0x00771968 + d).readU8(), w636adc: ptr(0x00636adc + d).readU32(),
      course: ptr(0x00644158 + d).readS32()});
  }
};
'''.replace('CSRC', json.dumps(CSRC))

args = [sys.executable, SL, '--track', '0', '--mode', '10', '--cars', '4', '--car', '1', '--rule', '0', '--hold', str(HOLD)]
print('launch:', ' '.join(args), '| MASHED_ROOT =', os.environ.get('MASHED_ROOT'), flush=True)
p = subprocess.Popen(args, cwd=W, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
pid = None; lines = []
def pump():
    global pid
    for ln in p.stdout:
        lines.append(ln); print('  [sl]', ln.rstrip(), flush=True)
        mm = re.search(r'scenario_launch\s+pid=(\d+)', ln)
        if mm and pid is None: pid = int(mm.group(1))
threading.Thread(target=pump, daemon=True).start()
t0 = time.time()
while pid is None and time.time() - t0 < 120 and p.poll() is None: time.sleep(0.5)
if pid is None: print('no pid'); sys.exit(2)
print('attach pid', pid, flush=True)
s = frida.get_local_device().attach(pid); sc = s.create_script(JS)
sc.on('message', lambda m, d: print('  [js]', m, flush=True)); sc.load(); E = sc.exports_sync
rows = []; t1 = time.time()
try:
    while time.time() - t1 < HOLD - 8 and p.poll() is None:
        time.sleep(10)
        r = json.loads(E.snap()); r['t'] = round(time.time() - t1); rows.append(r)
        print('  t=%d phase=%d frames=%d calls=%s emits=%s' % (r['t'], r['phase'], r['frames'], r['calls'], r['emits']), flush=True)
except Exception as e:
    print('stopped:', e, flush=True)
json.dump({'pid': pid, 'args': args, 'rows': rows}, open(OUT, 'w'), indent=1)
print('wrote', OUT, flush=True)
try: s.detach()
except Exception: pass
p.wait()
print('scenario_launch exit', p.returncode, flush=True)
