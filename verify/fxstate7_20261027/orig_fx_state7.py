r"""D4-FX-STATE7 original-side capture, on the D4-FX-REBASE recipe (--poke-ctrl-slots --statediff-out, no pulses).

Probes in ONE run:
  A  verify/fxslot2_20261027/orig_fx_gates_count.py, CModule and JS taken verbatim via ast (FX calls / emits / colours / ids)
  E  (new) wheel-colour x skid histogram per car: FUN_0040c690 entry (arg 3 = colour, arg 4 = skid word, arg 5 = car)
     -> calls[car][colour][skid]; FUN_0048cf90 entry returning into 0x0040c690..0x0040c9fa -> emits[car][colour][skid],
     attributed to the preceding FUN_0040c690 call (the same attribution as probe A).
  F  (new) one row per FUN_0040ca00 call (136 B): [0] DAT_0063ba8c, [1] phase byte 0x00771968, [2] rec+0x4 != 0 mask,
     [3] poke done, [4] [5] bytes 4 / 5 of the block 0x007f19b8, [6] [7] 0; then per car c (32 B):
       i32 rec+0xcfc, i32 rec+0x10, i32 rec+0x14, i32 rec+0x9f0,
       u16 FUN_0046e9e0 entries for this car since the previous row (car = (ESI - 0x008815a0) / 0xd04),
       u16 hits at 0x0046eaf2 (the `mov [esi+0xcfc], 0` reached past the rec+0x10 test at 0x0046eab3) since the previous row,
       u8 / u8 bytes 4 / 5 of FUN_0046e9e0's block argument (2nd stack arg, read at entry) on the last entry,
       u8 1 if that block was 0x007f19b8, u8 (block - 0x007f1038) / 0x4c (0xff when not a slot block),
       f32 |rec+0x9b0..+0x9b8|^2, u8 / u8 bytes 4 / 5 of slot(c)'s block (0x007f1038 + slot*0x4c), s8 slot word, u8 0.
     rec(c) = 0x008815a0 + c*0xd04; slot word = [0x007f1a14 + c*0x10].
  --poke-cfc (consumed here, not passed on): on the first FUN_0040ca00 call with DAT_0063ba8c == 7, write rec+0xcfc = 1 for
     cars 0..3 once (an intervention: tests whether the state-7 path rewrites rec+0xcfc per car).
Launch: scenario_launch.py --track 0 --mode 10 --cars 4 --car 1 --rule 0 --hold N + extra argv.
usage: py -3.12 orig_fx_state7.py <hold_s> <out_prefix> [--poke-cfc] [extra scenario_launch args]   (env MASHED_ROOT)
writes <out_prefix>.A.json .E.json .F.rows.bin
"""
import ast, json, os, re, subprocess, sys, threading, time
import frida

HOLD, OUT = int(sys.argv[1]), sys.argv[2]
EXTRA = sys.argv[3:]
POKE = '--poke-cfc' in EXTRA
EXTRA = [a for a in EXTRA if a != '--poke-cfc']
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SL = os.path.join(W, 're', 'frida', 'scenario_launch.py')


def probe(rel):
    tree = ast.parse(open(os.path.join(W, rel), encoding='utf-8').read())
    got = {}
    for n in tree.body:
        if isinstance(n, ast.Assign) and len(n.targets) == 1 and isinstance(n.targets[0], ast.Name):
            k = n.targets[0].id
            if k == 'CSRC' and isinstance(n.value, ast.Constant): got['c'] = n.value.value
            if k == 'JS' and isinstance(n.value, ast.Call): got['j'] = n.value.func.value.value
    return got['j'].replace('CSRC', json.dumps(got['c']))


JS_A = probe('verify/fxslot2_20261027/orig_fx_gates_count.py')

CSRC_E = r'''
#include <gum/guminterceptor.h>
extern unsigned int ekey[32];   /* 4 cars x 8 colour keys */
extern int ecnt[128];           /* (car*8+k)*4 + {0 calls ns, 1 calls sk, 2 emits ns, 3 emits sk} */
extern int est[4];              /* [0] last index, [1] last skid, [2] pending */
extern unsigned int elo[1];
void on_c690(GumInvocationContext *ic) {
  unsigned int col = (unsigned int)(gsize)gum_invocation_context_get_nth_argument(ic, 3);
  int skid = ((int)(gsize)gum_invocation_context_get_nth_argument(ic, 4)) != 0;
  int car = (int)(gsize)gum_invocation_context_get_nth_argument(ic, 5);
  int k;
  if (car < 0 || car > 3) car = 3;
  for (k = 0; k < 7; k++) {
    int i = car * 8 + k;
    if ((ecnt[i * 4] | ecnt[i * 4 + 1]) == 0 || ekey[i] == col) break;
  }
  ekey[car * 8 + k] = col;
  ecnt[(car * 8 + k) * 4 + skid]++;
  est[0] = car * 8 + k; est[1] = skid; est[2] = 1;
}
void on_cf90(GumInvocationContext *ic) {
  unsigned int ra = *(unsigned int *)(gsize)ic->cpu_context->esp;
  if (est[2] && ra - elo[0] < 0x36a) { ecnt[est[0] * 4 + 2 + est[1]]++; est[2] = 0; }
}
'''
JS_E = r'''
const IMG = 0x400000;
const m = Process.findModuleByName('MASHED.exe');
const d = m.base.toUInt32() - IMG;
const sym = {};
const sizes = {ekey: 128, ecnt: 512, est: 16, elo: 4};
for (const k in sizes) { sym[k] = Memory.alloc(sizes[k]); for (let i = 0; i < sizes[k]; i += 4) sym[k].add(i).writeU32(0); }
sym.elo.writeU32(0x0040c690 + d);
const cm = new CModule(CSRC, sym);
Interceptor.attach(ptr(0x0040c690 + d), cm.on_c690);
Interceptor.attach(ptr(0x0048cf90 + d), cm.on_cf90);
rpc.exports = {
  snap: function () {
    const out = [];
    for (let c = 0; c < 4; c++) { const h = {};
      for (let k = 0; k < 8; k++) { const i = c * 8 + k; const n = [0, 1, 2, 3].map(j => sym.ecnt.add(4 * (i * 4 + j)).readS32());
        if (n[0] || n[1]) h['0x' + (sym.ekey.add(4 * i).readU32() >>> 0).toString(16)] = n; }
      out.push(h); }
    return JSON.stringify(out);
  }
};
'''.replace('CSRC', json.dumps(CSRC_E))

CSRC_F = r'''
#include <gum/guminterceptor.h>
extern int frames;
extern unsigned char rows[2176000];
extern unsigned int d[1];
extern int cnt[8];              /* per car: [c] e9e0 entries, [4+c] 0x0046eaf2 hits, since the previous row */
extern unsigned int lb[4];      /* per car: last block pointer (image-relative) */
extern unsigned char lbb[16];   /* per car: bytes 4, 5 of the last block at entry */
extern int pk[2];               /* [0] poke requested, [1] poke done */
static int car_of(unsigned int esi) {
  unsigned int o = esi - (0x008815a0u + d[0]);
  if (o >= 4u * 0xd04u) return -1;
  return (int)(o / 0xd04u);
}
void on_e9e0(GumInvocationContext *ic) {
  int c = car_of((unsigned int)ic->cpu_context->esi);
  unsigned char *b;
  if (c < 0) return;
  cnt[c]++;
  b = (unsigned char *)gum_invocation_context_get_nth_argument(ic, 1);
  lb[c] = (unsigned int)(gsize)b - d[0];
  lbb[c * 4] = b[4]; lbb[c * 4 + 1] = b[5];
}
void on_eaf2(GumInvocationContext *ic) {
  int c = car_of((unsigned int)ic->cpu_context->esi);
  if (c >= 0) cnt[4 + c]++;
}
void on_ca00(GumInvocationContext *ic) {
  int c;
  unsigned char *r;
  unsigned char st = *(unsigned char *)(gsize)(0x0063ba8cu + d[0]);
  if (pk[0] && !pk[1] && st == 7) {
    for (c = 0; c < 4; c++) *(int *)(gsize)(0x008815a0u + d[0] + (unsigned int)c * 0xd04u + 0xcfcu) = 1;
    pk[1] = 1;
  }
  if (frames >= 16000) { frames++; return; }
  r = rows + frames * 136;
  r[0] = st;
  r[1] = *(unsigned char *)(gsize)(0x00771968u + d[0]);
  r[2] = 0; r[3] = (unsigned char)pk[1];
  r[4] = *(unsigned char *)(gsize)(0x007f19b8u + d[0] + 4u);
  r[5] = *(unsigned char *)(gsize)(0x007f19b8u + d[0] + 5u);
  r[6] = 0; r[7] = 0;
  for (c = 0; c < 4; c++) {
    unsigned int rec = 0x008815a0u + d[0] + (unsigned int)c * 0xd04u;
    int slot = *(int *)(gsize)(0x007f1a14u + d[0] + (unsigned int)c * 0x10u);
    float *v = (float *)(gsize)(rec + 0x9b0u);
    unsigned char *q = r + 8 + c * 32;
    int *qi = (int *)q;
    unsigned int blk = lb[c];
    if (*(int *)(gsize)(rec + 0x4u) != 0) r[2] |= (unsigned char)(1 << c);
    qi[0] = *(int *)(gsize)(rec + 0xcfcu);
    qi[1] = *(int *)(gsize)(rec + 0x10u);
    qi[2] = *(int *)(gsize)(rec + 0x14u);
    qi[3] = *(int *)(gsize)(rec + 0x9f0u);
    *(unsigned short *)(q + 16) = (unsigned short)(cnt[c] > 65535 ? 65535 : cnt[c]);
    *(unsigned short *)(q + 18) = (unsigned short)(cnt[4 + c] > 65535 ? 65535 : cnt[4 + c]);
    q[20] = lbb[c * 4]; q[21] = lbb[c * 4 + 1];
    q[22] = blk == 0x007f19b8u ? 1 : 0;
    q[23] = (blk >= 0x007f1038u && blk < 0x007f19b8u && (blk - 0x007f1038u) % 0x4cu == 0) ? (unsigned char)((blk - 0x007f1038u) / 0x4cu) : 0xff;
    *(float *)(q + 24) = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    q[28] = (slot >= 0 && slot < 16) ? *(unsigned char *)(gsize)(0x007f1038u + d[0] + (unsigned int)slot * 0x4cu + 4u) : 0;
    q[29] = (slot >= 0 && slot < 16) ? *(unsigned char *)(gsize)(0x007f1038u + d[0] + (unsigned int)slot * 0x4cu + 5u) : 0;
    q[30] = (unsigned char)(signed char)slot; q[31] = 0;
    cnt[c] = 0; cnt[4 + c] = 0;
  }
  frames++;
}
'''
JS_F = r'''
const IMG = 0x400000;
const m = Process.findModuleByName('MASHED.exe');
const d = m.base.toUInt32() - IMG;
const sym = {};
const sizes = {frames: 4, rows: 2176000, d: 4, cnt: 32, lb: 16, lbb: 16, pk: 8};
for (const k in sizes) { sym[k] = Memory.alloc(sizes[k]); for (let i = 0; i < sizes[k]; i += 4) sym[k].add(i).writeU32(0); }
sym.d.writeU32(d >>> 0);
sym.pk.writeU32(POKE);
const cm = new CModule(CSRC, sym);
Interceptor.attach(ptr(0x0046e9e0 + d), cm.on_e9e0);
Interceptor.attach(ptr(0x0046eaf2 + d), cm.on_eaf2);
Interceptor.attach(ptr(0x0040ca00 + d), cm.on_ca00);
rpc.exports = {
  frames: function () { return sym.frames.readS32(); },
  poked: function () { return sym.pk.add(4).readS32(); },
  rows: function () { const n = Math.min(sym.frames.readS32(), 16000); return sym.rows.readByteArray(n * 136); }
};
'''.replace('CSRC', json.dumps(CSRC_F)).replace('POKE', '1' if POKE else '0')

args = [sys.executable, SL, '--track', '0', '--mode', '10', '--cars', '4', '--car', '1', '--rule', '0', '--hold', str(HOLD)] + EXTRA
print('launch:', ' '.join(args), '| poke_cfc =', POKE, '| MASHED_ROOT =', os.environ.get('MASHED_ROOT'), flush=True)
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
for k, js in (('A', JS_A), ('E', JS_E), ('F', JS_F)):
    sc = s.create_script(js)
    sc.on('message', (lambda kk: lambda m, d: print('  [js%s]' % kk, m, flush=True))(k))
    sc.load(); E[k] = sc.exports_sync
snA, snE = [], []
rowsF = None
t1 = time.time()
try:
    while time.time() - t1 < HOLD - 8 and p.poll() is None:
        time.sleep(10)
        t = round(time.time() - t1)
        a = json.loads(E['A'].snap()); a['t'] = t; snA.append(a)
        e = json.loads(E['E'].snap()); snE.append({'t': t, 'cars': e})
        rowsF = E['F'].rows()
        print('  t=%d phase=%d frames=%d calls=%s emits=%s poked=%d' % (t, a['phase'], a['frames'], a['calls'], a['emits'],
              E['F'].poked()), flush=True)
except Exception as ex:
    print('stopped:', ex, flush=True)
json.dump({'pid': pid, 'args': args, 'poke_cfc': POKE, 'rows': snA}, open(OUT + '.A.json', 'w'), indent=1)
json.dump({'pid': pid, 'args': args, 'poke_cfc': POKE, 'snaps': snE}, open(OUT + '.E.json', 'w'), indent=1)
if rowsF is not None: open(OUT + '.F.rows.bin', 'wb').write(rowsF)
print('wrote', OUT, 'rows' if rowsF is not None else 'NO ROWS', flush=True)
try: s.detach()
except Exception: pass
p.wait()
print('scenario_launch exit', p.returncode, flush=True)
