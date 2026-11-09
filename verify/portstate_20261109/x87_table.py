"""x87_table.py --cw <orig cw json>... --cwexe <exe cw json>... --emu <emu json>... -> X87AUDIT_TABLE.tsv + markdown on stdout

PORT-X87-PC24-AUDIT deliverable table (PREREG_X87AUDIT.md section 5). Joins x87_inventory.tsv, arm 1 (CW), arm 2 (static
scans x87_pcscan.tsv / x87_pcscan_exe.tsv), arm 3 (emulation) and the hooks.csv port column.
"""
import argparse, csv, json
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
ap = argparse.ArgumentParser()
ap.add_argument('--cw', nargs='*', default=[])
ap.add_argument('--cwexe', nargs='*', default=[])
ap.add_argument('--emu', nargs='*', default=[])
ap.add_argument('--exescan', default=str(HERE / 'x87_pcscan_exe.tsv'))
a = ap.parse_args()

inv = [l.split('\t') for l in (HERE / 'x87_inventory.tsv').read_text().splitlines()[1:]]
scan = {(p[0], p[2]): p for p in (l.split('\t') for l in (HERE / 'x87_pcscan.tsv').read_text().splitlines()[1:])}
exes = {(p[0], p[2]): p for p in (l.split('\t') for l in Path(a.exescan).read_text().splitlines()[1:])} \
    if Path(a.exescan).exists() else {}
rows = [r for r in csv.reader(open(REPO / 'hooks.csv', encoding='utf-8', errors='replace')) if r and not r[0].startswith('#')]
pi = rows[0].index('port')
port = {'0x' + r[0].lower().replace('0x', '').zfill(8): r[pi] for r in rows[1:] if len(r) > pi}


def cw_of(files, key_of):
    c = defaultdict(lambda: [0, 0, 0])
    fails = set()
    for f in files:
        d = json.load(open(f))
        last = d['snaps'][-1]['c']
        for i, k in enumerate(d['targets']):
            kk = key_of(k)
            for j in range(3):
                c[kk][j] += last[i][j]
            if d.get('attach') and d['attach'][i] != 1:
                fails.add(kk)
    out = {}
    for k, v in c.items():
        s = []
        if v[0]: s.append('0x007f x%d' % v[0])
        if v[1]: s.append('0x027f x%d' % v[1])
        if v[2]: s.append('other x%d' % v[2])
        out[k] = ' + '.join(s) if s else ('ATTACH FAIL' if k in fails else 'not reached')
    return out


VOID_PORT = {'0x004282a0': 'port legs VOID: X53/X24/S return NaN-indefinite in the emulator (artefact, not separated)'}
cwo = cw_of(a.cw, lambda k: k)
cwe = cw_of(a.cwexe, lambda k: k.split(':')[0])
emu = defaultdict(lambda: {'n': 0, 'ref': 0, 'nat': 0, 'v': defaultdict(lambda: [0, 0]), 'vf': defaultdict(lambda: [0, 0]),
                           'why': set()})
for f in a.emu:
    for s in json.load(open(f))['samples']:
        e = emu[s['rva']]
        e['n'] += 1
        if 'eq' not in s:
            e['ref'] += 1
            e['why'].update(v.split(' @')[0] for v in s.get('refused', {}).values())
            continue
        if 'O53' in s['eq'] and not s['eq']['O53'] or 'O24' in s['eq'] and not s['eq']['O24']:
            e['nat'] += 1
        for k, v in s['eq'].items():
            e['v'][k][0 if v else 1] += 1
        for k, v in s['eqf'].items():
            e['vf'][k][0 if v else 1] += 1
        if s.get('refused'):
            e['why'].update(k + ':' + v.split(' @')[0] for k, v in s['refused'].items())


def g(e, k):
    ok, bad = e['v'][k]
    if ok + bad == 0:
        return '-'
    return 'GREEN %d' % ok if bad == 0 else 'RED %d/%d' % (bad, ok + bad)


out = ['rva\ttu\tport\torig_cw\texe_cw\tstatic_orig\tsse2_static\tstatic_exe\temu_n\tnatural_deltas\tX53\tX24\tS\tS_f32ret'
       '\tmatch_modes\trecommendation\tscope\tnote']
for tu, sect, src, rva, how in inv:
    if not rva:
        continue
    sc = scan.get((tu, rva), [''] * 10)
    ex = exes.get((tu, rva), [''] * 10)
    scope = 'matched' if how.startswith('match:') else 'transcription (CW only)'
    if sect == 'CONTROL':
        scope = 'control (SSE2 TU, not in x87_tus.txt)'
    e = emu.get(rva)
    ocw = cwo.get(rva, 'not probed')
    cls, sse = sc[3], sc[4]
    exe_cls = ex[3] if ex[3] else 'not scanned'
    pc53_ok = pc24_ok = sse_ok = None
    basis = 'static'
    note = ''
    if e and (e['n'] - e['ref']) > 0:
        basis = 'emu'

        def ok(k, f32=False):
            p = (e['vf'] if f32 else e['v'])[k]
            return (p[1] == 0) if sum(p) else None
        void_port = rva in VOID_PORT
        if void_port or ok('X53') is None:
            # no port leg (port entry ABI differs, or VOID): the original's own bytes at the other PC stand in
            basis = 'emu, original bytes as proxy'
            pc53_ok, pc24_ok, sse_ok = ok('O53'), ok('O24'), None
            note = VOID_PORT.get(rva, 'port legs not run (port entry reads a non-pointer arg as a pointer)')
        else:
            pc53_ok, pc24_ok, sse_ok = ok('X53', True), ok('X24', True), ok('S', True)
            if ok('X53') != ok('X53', True) or ok('S') != ok('S', True):
                note = 'raw 80-bit ST0 differs, f32 equal (returns scored as the caller stores them, fstp m32)'
            if e['nat'] == 0 and cls == 'PC-SENSITIVE':
                note = 'no natural delta in %d calls (O53 = O); static: a discriminating input exists' % (
                    e['n'] - e['ref'])
            if ok('X24', True) is False and ok('S', True) is False:
                note = 'port body differs at every precision: the DDK reassociated float adds the C writes '                        'left to right (VS2022 /fp:precise keeps source order)'
    else:
        insens = cls in ('NOFP', 'PC-INSENSITIVE') and exe_cls in ('NOFP', 'PC-INSENSITIVE', 'NOT_IN_IMAGE', 'not scanned')
        pc53_ok = pc24_ok = True if insens else None
        if cls == 'PC-SENSITIVE' or exe_cls == 'PC-SENSITIVE':
            if '0x027f' in ocw and '0x007f' not in ocw:
                pc53_ok, pc24_ok = True, None
            elif '0x007f' in ocw and '0x027f' not in ocw:
                pc53_ok, pc24_ok = False, True
        sse_ok = (sse == 'equal') if (insens or pc24_ok) else None
        if '0x027f' in ocw and '0x007f' not in ocw and cls == 'PC-SENSITIVE':
            sse_ok = False if sse == 'equal' else None
    modes = []
    for nm, ok in (('x87@PC_53', pc53_ok), ('x87@PC_24', pc24_ok), ('SSE2', sse_ok)):
        modes.append(nm + ('=yes' if ok else ('=no' if ok is False else '=?')))
    pr = port.get(rva, '?')
    ecw = cwe.get(rva, 'not probed')
    # the mode the exe runs today: x87 at whatever CW the exe's call site has (arm 1 exe; PC_53 unless a scope)
    today = pc24_ok if ('0x007f' in ecw and '0x027f' not in ecw) else (
        (pc53_ok and pc24_ok) if ('0x007f' in ecw and '0x027f' in ecw) else pc53_ok)
    if '0x007f' in ecw:
        note = (note + '; ' if note else '') + 'the exe already calls it at 0x007f (a local PC_24 scope)'
    if 'every precision' in note:
        rec = 'FIX THE BODY FIRST: rewrite the expression in the DDK order; no mode matches today'
    elif today:
        rec = 'keep x87 (matches today)'
    elif today is False and pr != 'called':
        rec = 'no runtime effect today (port=%s); PC_24 or SSE2 when it is called' % pr
    elif today is False and sse_ok:
        rec = 'PROPOSED: SSE2 or game-thread PC_24 (both match)'
    elif today is False and pc24_ok:
        rec = 'PROPOSED: game-thread PC_24 (SSE2 not proven)'
    else:
        rec = 'undetermined (%s)' % ('not reached' if 'not reached' in ocw else 'see notes')
    if scope != 'matched':
        rec = '-' if sect != 'CONTROL' else 'control: SSE2 since PORT-STATE-03 (L-102), confirmed'
    out.append('\t'.join([rva, tu, pr, ocw, cwe.get(rva, 'not probed'), cls, sse, exe_cls,
                          str(e['n'] - e['ref']) if e else '0', str(e['nat']) if e else '-',
                          g(e, 'X53') if e else '-', g(e, 'X24') if e else '-', g(e, 'S') if e else '-',
                          (lambda ok, bad: '-' if ok + bad == 0 else ('GREEN' if bad == 0 else 'RED %d' % bad))(*e['vf']['S']) if e else '-',
                          ' '.join(modes) + ' [' + basis + ']', rec, scope, note]))
(HERE / 'X87AUDIT_TABLE.tsv').write_text('\n'.join(out) + '\n')
print(len(out) - 1, 'rows -> X87AUDIT_TABLE.tsv')
