r"""D4-FX-REBASE original-side capture: the three committed FX probes in ONE run on the port's recipe, plus a per-frame
car series for the state-7 throttle question.

Probes (CModule source and JS taken verbatim, via ast, from the committed scripts so the counters are the same code):
  A  verify/fxslot2_20261027/orig_fx_gates_count.py  FX calls / emits per car and skid flag, alive split (fx_score inputs)
  B  verify/fxinputs_20261026/orig_fx_inputs.py      FX gate input census (score_orig* inputs)
  C  verify/fxslot2_20261027/orig_alive_series.py    per-frame state / alive / slot / AI-call rows + kill events
  D  (new, below) one row per FUN_0040ca00 call: [0] DAT_0063ba8c, [1] phase byte 0x00771968, [2] alive mask, [3] 0,
     then per car c (32 B): f32 |rec+0x9b0|^2, f32 rec+0x958, f32 rec+0x960, f32 rec+0x9e0, i32 rec+0xcfc,
     u8 ctrl block bytes 0..7 of slot(c) (0x007f1038 + slot*0x4c; 0 when slot < 0), s8 slot word, 3 pad.
     rec(c) = 0x008815a0 + c*0xd04.
Launch: scenario_launch.py --track 0 --mode 10 --cars 4 --car 1 --rule 0 --hold N + extra argv (use
--poke-ctrl-slots --statediff-out <msd>; the statediff launcher sends no input pulses, scenario_launch.py:5455).
usage: py -3.12 orig_fx_rebase.py <hold_s> <out_prefix> [extra scenario_launch args]   (env MASHED_ROOT)
writes <out_prefix>.A.json .B.json .C.json .C.rows.bin .C.events.json .D.rows.bin
"""
import ast, json, os, re, subprocess, sys, threading, time
import frida

HOLD, OUT = int(sys.argv[1]), sys.argv[2]
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SL = os.path.join(W, 're', 'frida', 'scenario_launch.py')


def probe(rel):
    """CSRC string and the JS template (before .replace('CSRC', ...)) of a committed capture script."""
    tree = ast.parse(open(os.path.join(W, rel), encoding='utf-8').read())
    got = {}
    for n in tree.body:
        if isinstance(n, ast.Assign) and len(n.targets) == 1 and isinstance(n.targets[0], ast.Name):
            k = n.targets[0].id
            if k == 'CSRC' and isinstance(n.value, ast.Constant): got['c'] = n.value.value
            if k == 'JS' and isinstance(n.value, ast.Call): got['j'] = n.value.func.value.value
    return got['j'].replace('CSRC', json.dumps(got['c']))


JS_A = probe('verify/fxslot2_20261027/orig_fx_gates_count.py')
JS_B = probe('verify/fxinputs_20261026/orig_fx_inputs.py')
JS_C = probe('verify/fxslot2_20261027/orig_alive_series.py')

CSRC_D = r'''
#include <gum/guminterceptor.h>
extern int frames;
extern unsigned char rows[2112000];
extern unsigned int d[1];
void on_ca00(GumInvocationContext *ic) {
  int c, k;
  unsigned char *r;
  if (frames >= 16000) { frames++; return; }
  r = rows + frames * 132;
  r[0] = *(unsigned char *)(gsize)(0x0063ba8cu + d[0]);
  r[1] = *(unsigned char *)(gsize)(0x00771968u + d[0]);
  r[2] = 0; r[3] = 0;
  for (c = 0; c < 4; c++) {
    unsigned int rec = 0x008815a0u + d[0] + (unsigned int)c * 0xd04u;
    int slot = *(int *)(gsize)(0x007f1a14u + d[0] + (unsigned int)c * 0x10u);
    float *v = (float *)(gsize)(rec + 0x9b0u);
    unsigned char *q = r + 4 + c * 32;
    float *qf = (float *)q;
    if (*(int *)(gsize)(rec + 0x4u) != 0) r[2] |= (unsigned char)(1 << c);
    qf[0] = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    qf[1] = *(float *)(gsize)(rec + 0x958u);
    qf[2] = *(float *)(gsize)(rec + 0x960u);
    qf[3] = *(float *)(gsize)(rec + 0x9e0u);
    *(int *)(q + 16) = *(int *)(gsize)(rec + 0xcfcu);
    for (k = 0; k < 8; k++)
      q[20 + k] = (slot >= 0 && slot < 16) ? *(unsigned char *)(gsize)(0x007f1038u + d[0] + (unsigned int)slot * 0x4cu + (unsigned int)k) : 0;
    q[28] = (unsigned char)(signed char)slot; q[29] = 0; q[30] = 0; q[31] = 0;
  }
  frames++;
}
'''
JS_D = r'''
const IMG = 0x400000;
const m = Process.findModuleByName('MASHED.exe');
const d = m.base.toUInt32() - IMG;
const sym = {};
const sizes = {frames: 4, rows: 2112000, d: 4};
for (const k in sizes) { sym[k] = Memory.alloc(sizes[k]); for (let i = 0; i < sizes[k]; i += 4) sym[k].add(i).writeU32(0); }
sym.d.writeU32(d >>> 0);
const cm = new CModule(CSRC, sym);
Interceptor.attach(ptr(0x0040ca00 + d), cm.on_ca00);
rpc.exports = {
  frames: function () { return sym.frames.readS32(); },
  rows: function () { const n = Math.min(sym.frames.readS32(), 16000); return sym.rows.readByteArray(n * 132); }
};
'''.replace('CSRC', json.dumps(CSRC_D))

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
s = frida.get_local_device().attach(pid)
E = {}
for k, js in (('A', JS_A), ('B', JS_B), ('C', JS_C), ('D', JS_D)):
    sc = s.create_script(js)
    sc.on('message', (lambda kk: lambda m, d: print('  [js%s]' % kk, m, flush=True))(k))
    sc.load(); E[k] = sc.exports_sync
snA, snB, snC = [], [], []
rowsC = evC = rowsD = None
t1 = time.time()
try:
    while time.time() - t1 < HOLD - 8 and p.poll() is None:
        time.sleep(10)
        t = round(time.time() - t1)
        a = json.loads(E['A'].snap()); a['t'] = t; snA.append(a)
        b = json.loads(E['B'].snap()); b['t'] = t; snB.append(b)
        c = json.loads(E['C'].snap()); c['t'] = t; snC.append(c)
        rowsC = E['C'].rows(); evC = E['C'].events(); rowsD = E['D'].rows()   # every snapshot: the launcher can end the game first
        print('  t=%d phase=%d ba8c=%d frames=%d calls=%s emits=%s aic=%s slots=%s' % (t, a['phase'], c['ba8c'], a['frames'],
              a['calls'], a['emits'], c['aic'], c['slots']), flush=True)
except Exception as e:
    print('stopped:', e, flush=True)
json.dump({'pid': pid, 'args': args, 'rows': snA}, open(OUT + '.A.json', 'w'), indent=1)
json.dump({'pid': pid, 'args': args, 'rows': snB}, open(OUT + '.B.json', 'w'), indent=1)
json.dump({'pid': pid, 'args': args, 'snaps': snC}, open(OUT + '.C.json', 'w'), indent=1)
if rowsC is not None:
    open(OUT + '.C.rows.bin', 'wb').write(rowsC)
    ev = [evC[i:i + 6] for i in range(0, len(evC), 6)]
    json.dump([{'frame': e[0], 'fn': '0x%08x' % e[1], 'args': e[2:]} for e in ev], open(OUT + '.C.events.json', 'w'), indent=0)
if rowsD is not None:
    open(OUT + '.D.rows.bin', 'wb').write(rowsD)
print('wrote', OUT, 'rows' if rowsD is not None else 'NO ROWS', flush=True)
try: s.detach()
except Exception: pass
p.wait()
print('scenario_launch exit', p.returncode, flush=True)
