"""The 9 calls of verify/scr19_parity_20261012/arrow_fix/measure_table.txt, re-run. Reference: the original's
game-thread returns in the committed orig_measure_probe.json (+ boot2 / boot3). Under test: the standalone's
MASHED_DBG_MEASURE `table` lines from the baseline exe (orch/integration + log extension) and the consolidated
exe. Every return is compared as float bits (the original's ST0 values are float-exact)."""
import json, re, struct, sys
A = '../scr19_parity_20261012/arrow_fix/'
refs = [json.load(open(A + f))['game_thread']['calls'] for f in
        ('orig_measure_probe.json', 'orig_measure_probe_boot2.json', 'orig_measure_probe_boot3.json')]
def fbits(x): return struct.unpack('<I', struct.pack('<f', x))[0]
def log(p):
    out = {}
    for m in re.finditer(r'measure: table id=0x([0-9a-f]+) scale=([0-9.]+) ret=\S+ bits=([0-9a-f]{8})', open(p).read()):
        out[(int(m.group(1), 16), m.group(2))] = int(m.group(3), 16)
    return out
base, after = log('sa32_base.log'), log('sa32_after.log')
ok = 0
print(f'{"id":>6} {"scale":>5} {"orig bits":>8} {"base exe":>8} {"after exe":>9}  equal')
for c in refs[0]:
    k = (c['id'], f'{c["scale"]:.2f}'); o = fbits(c['ret'])
    same_boots = all(fbits(r[i]['ret']) == o for r in refs for i, x in enumerate(r) if (x['id'], x['scale']) == (c['id'], c['scale']))
    eq = base.get(k) == o and after.get(k) == o and same_boots
    ok += eq
    print(f'{c["id"]:#6x} {c["scale"]:5.2f} {o:08x} {base.get(k, 0):08x} {after.get(k, 0):08x}  {eq}')
print(f'bit-equal {ok}/{len(refs[0])} (original 3 boots, baseline exe, consolidated exe)')
