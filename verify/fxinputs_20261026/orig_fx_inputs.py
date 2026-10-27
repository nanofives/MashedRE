r"""D4-FX-INPUTS original-side input census (extends verify/fxgates_20261026/orig_fx_gates_count.py).

Same launch recipe (scenario_launch.py Training, 4 cars, mode 10, rule 0, car 1). CModule probes, no JS per call:
  FUN_0040ca00 entry  -> frames
  FUN_0040c690 entry  -> on the FIRST call per (car, frame), i.e. once per frame that passed FUN_0040ca00's outer gates
                         with +0xcfc == 0 or the 0.3 roll, read the gate's own inputs:
       b0c   = rec+0xb0c                        (FUN_0046d6a0, tested at 0x0040caf5 against [0x005cc568] 100.0)
       brk   = [0x007f103c + slot*0x4c + 1]     (0x0040cb21; slot = [0x007f1a14 + car*0x10])
       rng   = (rec+0x9b0 . rec+0x9d4) > 0      (FUN_0046d570 acos(dot(norm(+0x9b0), +0x9d4)) * [0x005ccae0] < 90.0
                                                  <=> dot > 0; the normalisation is a positive scale)
       skid  = arg 4 of FUN_0040c690
  record base rec = 0x008815a0 + car*0xd04 (FUN_0046d570's 0x00881f50 - 0x9b0, stride 0x341 dwords).
Counters per car: act, skid, legA (b0c>100), legB (rng && brk>0x40), brk>0x40, rng, cfc (+0xcfc != 0), frozen (+0x9b0..b8
bit-identical to that car's previous counted frame), plus a b0c histogram (25-unit bins, 0..8) and a brake-byte class
histogram (0, 1..0x40, 0x41..0xfe, 0xff). Snapshots every 10 s.
usage: py -3.12 orig_fx_inputs.py <hold_s> <out.json>   (env MASHED_ROOT = private root that contains original\)
"""
import json, os, re, subprocess, sys, threading, time
import frida

HOLD, OUT = int(sys.argv[1]), sys.argv[2]
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SL = os.path.join(W, 're', 'frida', 'scenario_launch.py')

CSRC = r'''
#include <gum/guminterceptor.h>
extern int frames;
extern int cnt[64];       /* car*16 + k: 0 act 1 skid 2 legA 3 legB 4 brk 5 rng 6 cfc 7 frozen */
extern int hb[64];        /* car*16 + bin: b0c bins 0..8 (bin 0 includes negatives) */
extern int hk[16];        /* car*4 + class: brake 0, 1..0x40, 0x41..0xfe, 0xff */
extern int lastf[4];
extern unsigned int prevv[12];
extern unsigned int d[1];
extern int g[32];         /* car*8 + k: 0 slot<0 1 (int)+0x9e0<=1 2 |v|^2<62500 3 pass(cfc==0) 4 cfc!=0 5 vel frozen vs last frame */
extern unsigned int gprev[12];
extern int gd[32];        /* car*8 + k: g classes 0..5 on rec+0x4 == 0 frames, 6 dead frames, 7 dead with |v|^2 == 0 */
void on_ca00(GumInvocationContext *ic) {
  int car;
  frames++;
  for (car = 0; car < 4; car++) {
    unsigned int rec = 0x008815a0u + d[0] + (unsigned int)car * 0xd04u;
    unsigned int *vb = (unsigned int *)(gsize)(rec + 0x9b0u);
    float *vel = (float *)(gsize)(rec + 0x9b0u);
    int *gg = g + car * 8;
    int slot = *(int *)(gsize)(0x007f1a14u + d[0] + (unsigned int)car * 0x10u);
    int *dd = gd + car * 8;   /* same classes, only frames with rec+0x4 (FUN_0046c7b0 alive word) == 0 */
    int dead = *(int *)(gsize)(rec + 0x4u) == 0;
    int fz = vb[0] == gprev[car * 3] && vb[1] == gprev[car * 3 + 1] && vb[2] == gprev[car * 3 + 2];
    float v2 = vel[2] * vel[2] + vel[0] * vel[0] + vel[1] * vel[1];
    if (fz) { gg[5]++; if (dead) dd[5]++; }
    if (dead) { dd[6]++; if (v2 == 0.0f) dd[7]++; }
    gprev[car * 3] = vb[0]; gprev[car * 3 + 1] = vb[1]; gprev[car * 3 + 2] = vb[2];
    if (slot < 0) { gg[0]++; if (dead) dd[0]++; continue; }
    if (*(float *)(gsize)(rec + 0x9e0u) < 2.0f) { gg[1]++; if (dead) dd[1]++; continue; }   /* (int)f <= 1 <=> f < 2.0 (truncation) */
    if (*(int *)(gsize)(rec + 0xcfcu) != 0) { gg[4]++; if (dead) dd[4]++; continue; }
    if (v2 < 62500.0f) { gg[2]++; if (dead) dd[2]++; continue; }
    gg[3]++; if (dead) dd[3]++;
  }
}
void on_c690(GumInvocationContext *ic) {
  int skid = ((int)(gsize)gum_invocation_context_get_nth_argument(ic, 4)) != 0;
  int car = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 5);
  if (car < 0 || car > 3) return;
  if (lastf[car] == frames) return;
  lastf[car] = frames;
  unsigned int rec = 0x008815a0u + d[0] + (unsigned int)car * 0xd04u;
  float *vel = (float *)(gsize)(rec + 0x9b0u);
  float *fwd = (float *)(gsize)(rec + 0x9d4u);
  unsigned int *vb = (unsigned int *)(gsize)(rec + 0x9b0u);
  float b0c = *(float *)(gsize)(rec + 0xb0cu);
  int cfc = *(int *)(gsize)(rec + 0xcfcu);
  int slot = *(int *)(gsize)(0x007f1a14u + d[0] + (unsigned int)car * 0x10u);
  unsigned int brk = 0;
  if (slot >= 0 && slot < 16) brk = *(unsigned char *)(gsize)(0x007f103cu + d[0] + (unsigned int)slot * 0x4cu + 1u);
  float dot = vel[0] * fwd[0] + vel[1] * fwd[1] + vel[2] * fwd[2];
  int rng = dot > 0.0f;
  int *c = cnt + car * 16;
  int bin;
  c[0]++;
  if (skid) c[1]++;
  if (b0c > 100.0f) c[2]++;
  if (rng && brk > 0x40) c[3]++;
  if (brk > 0x40) c[4]++;
  if (rng) c[5]++;
  if (cfc != 0) c[6]++;
  if (vb[0] == prevv[car * 3] && vb[1] == prevv[car * 3 + 1] && vb[2] == prevv[car * 3 + 2]) c[7]++;
  prevv[car * 3] = vb[0]; prevv[car * 3 + 1] = vb[1]; prevv[car * 3 + 2] = vb[2];
  bin = 0;   /* no int<->float casts: tcc lacks __fixsfdi */
  { float edge = 25.0f; while (bin < 8 && b0c >= edge) { bin++; edge += 25.0f; } }
  hb[car * 16 + bin]++;
  hk[car * 4 + (brk == 0 ? 0 : brk <= 0x40 ? 1 : brk < 0xff ? 2 : 3)]++;
}
'''

JS = r'''
const IMG = 0x400000;
const m = Process.findModuleByName('MASHED.exe');
const d = m.base.toUInt32() - IMG;
const sym = {};
const sizes = {frames: 4, cnt: 256, hb: 256, hk: 64, lastf: 16, prevv: 48, d: 4, g: 128, gprev: 48, gd: 128};
for (const k in sizes) { sym[k] = Memory.alloc(sizes[k]); for (let i = 0; i < sizes[k]; i += 4) sym[k].add(i).writeU32(0); }
for (let i = 0; i < 4; i++) sym.lastf.add(4 * i).writeS32(-1);
sym.d.writeU32(d >>> 0);
Process.setExceptionHandler(function (ex) { send({exc: ex.type, addr: ex.address.toString(), rel: '0x' + (ex.address.toUInt32() - d).toString(16)}); return false; });
const cm = new CModule(CSRC, sym);
Interceptor.attach(ptr(0x0040ca00 + d), cm.on_ca00);
Interceptor.attach(ptr(0x0040c690 + d), cm.on_c690);
function arr(p, n) { const a = []; for (let i = 0; i < n; i++) a.push(p.add(4 * i).readS32()); return a; }
rpc.exports = {
  snap: function () {
    return JSON.stringify({frames: sym.frames.readS32(), cnt: arr(sym.cnt, 64), hb: arr(sym.hb, 64), hk: arr(sym.hk, 16), g: arr(sym.g, 32), gd: arr(sym.gd, 32),
      phase: ptr(0x00771968 + d).readU8(),
      slots: [0, 1, 2, 3].map(function (v) { return ptr(0x007f1a14 + d + v * 0x10).readS32(); })});
  }
};
'''.replace('CSRC', json.dumps(CSRC))

args = [sys.executable, SL, '--track', '0', '--mode', '10', '--cars', '4', '--car', '1', '--rule', '0', '--hold', str(HOLD)]
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
rows = []; t1 = time.time()
try:
    while time.time() - t1 < HOLD - 8 and p.poll() is None:
        time.sleep(10)
        r = json.loads(E.snap()); r['t'] = round(time.time() - t1); rows.append(r)
        print('  t=%d phase=%d frames=%d act=%s skid=%s' % (r['t'], r['phase'], r['frames'],
              [r['cnt'][c * 16] for c in range(4)], [r['cnt'][c * 16 + 1] for c in range(4)]), flush=True)
except Exception as e:
    print('stopped:', e, flush=True)
json.dump({'pid': pid, 'args': args, 'rows': rows}, open(OUT, 'w'), indent=1)
print('wrote', OUT, flush=True)
try: s.detach()
except Exception: pass
p.wait()
print('scenario_launch exit', p.returncode, flush=True)
