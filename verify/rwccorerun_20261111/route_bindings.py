"""RWC-PLUGIN-ATTACH run 40: route the librw bindings to their RW core bodies under MASHED_RWC_FRAME.
usage: py -3.12 verify/rwccorerun_20261111/route_bindings.py [--check]

For every single-line definition `<ret> MashedRw_<Name>_<rva>(<params>) {` in mashedmod/src/mashed_re/LibRw/RwApiBind.cpp
whose <rva> RwCore_Bodies.tsv lists with an exe symbol (match100 / provisional / lift / hand / missing), insert as the
first statement

    if (RwCore_FrameRoute()) return reinterpret_cast<<ret> (__cdecl*)(<param types>)>(&<symbol>)(<args>);  // [RWC-FRAME]

so the binding calls the core body through its own signature. RwCore_FrameRoute() (RwCore/RwCore_Knob.cpp) is true only
with MASHED_ORIG_TOP=1, MASHED_RWC_CORE=1 and MASHED_RWC_FRAME=1. Bindings the engine knob already routes, and RVAs
with an exe body or a CALL_VIA entry, are left alone. A missing body is a logged stop (RwCoreMissing_<rva>), so a boot
names the first binding with no core body. Writes ROUTED.tsv (rva, binding, symbol, kind, verdict). Idempotent:
lines tagged [RWC-FRAME] are removed first.
"""
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
SRC = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'LibRw', 'RwApiBind.cpp')
# the engine knob (RWC-CORE-RUN) routes these already; CALL_VIA 0x004c5890 has its own binding entry
SKIP = {'004c32b0', '004c30b0', '004c2fb0', '004c2f60', '004c3040', '004c3270', '004c9f50', '004c9eb0', '004c9f60',
        '00493640', '004c5890'}
DEF = re.compile(r'^(?P<pre>(?:extern "C" )?)(?P<ret>[A-Za-z_][\w\s\*]*?)\s*(?P<cc>__cdecl\s+)?'
                 r'(?P<name>(?:MashedRw_\w+?|PortTop_Port)_(?P<rva>00[0-9a-f]{6}))\((?P<params>[^()]*)\)\s*\{')
TAG = '// [RWC-FRAME]'
# bindings routed to a knob wrapper instead of the bare core body
SPECIAL = {'004c7730': 'RwCore_FrameShowRaster'}   # first Present on the core ends the frame-knob boot


def split_params(p):
    p = p.strip()
    if p in ('', 'void'):
        return [], []
    types, names = [], []
    for a in p.split(','):
        a = a.strip()
        m = re.match(r'^(.*?)(\w+)\s*$', a)
        if not m or not m.group(1).strip():
            return None, None
        types.append(m.group(1).strip())
        names.append(m.group(2))
    return types, names


def main(check):
    bodies = {r['rva'][2:]: r for r in csv.DictReader(open(os.path.join(HERE, 'RwCore_Bodies.tsv'), encoding='utf-8'),
                                                      delimiter='\t')}
    lines = open(SRC, encoding='utf-8').read().split('\n')
    lines = [ln for ln in lines if TAG not in ln]
    out, rows, decl = [], [], set()
    for ln in lines:
        m = DEF.match(ln)
        if not m:
            out.append(ln)
            continue
        rva, name = m.group('rva'), m.group('name')
        b = bodies.get(rva)
        verdict = None
        if rva in SKIP:
            verdict = 'engine knob or CALL_VIA'
        elif not b:
            verdict = 'not in the closure'
        elif b['kind'] == 'exe':
            verdict = 'exe body'
        elif '@' in b['exe_symbol']:
            verdict = 'stdcall symbol'
        types, names = split_params(m.group('params'))
        if verdict is None and types is None:
            verdict = 'parameter list not parsed'
        if verdict:
            rows.append((rva, name, b['exe_symbol'] if b else '', b['kind'] if b else '', 'not routed: ' + verdict))
            out.append(ln)
            continue
        ret = m.group('ret').strip()
        sym = SPECIAL.get(rva, b['exe_symbol'])
        decl.add(sym)
        fn = 'reinterpret_cast<%s (__cdecl*)(%s)>(&%s)' % (ret, ', '.join(types) or 'void', sym)
        call = '%s(%s)' % (fn, ', '.join(names))
        if ret == 'void':
            stmt = '    if (RwCore_FrameRoute()) { %s; return; }   %s' % (call, TAG)
        else:
            stmt = '    if (RwCore_FrameRoute()) return %s;   %s' % (call, TAG)
        i = ln.index('{') + 1
        if ln[i:].strip():   # one-line body: split it
            out.append(ln[:i])   # split once; not tagged, so a re-run keeps the body
            out.append(stmt)
            out.append('   ' + ln[i:].lstrip())
        else:
            out.append(ln)
            out.append(stmt)
        rows.append((rva, name, sym, b['kind'], 'routed'))
    # declarations of the core symbols, after the RwCore_Enabled declaration
    k = next(i for i, ln in enumerate(out) if ln.startswith('bool RwCore_Enabled(void);'))
    block = ['bool RwCore_FrameRoute(void);   ' + TAG,
             'extern "C" { ' + ' '.join('void %s(void);' % s for s in sorted(decl)) + ' }   ' + TAG]
    out[k + 1:k + 1] = block
    routed = sum(1 for r in rows if r[4] == 'routed')
    print('bindings %d, routed %d (%s)' % (len(rows), routed,
          ', '.join('%s %d' % (kd, sum(1 for r in rows if r[4] == 'routed' and r[3] == kd))
                    for kd in ('match100', 'provisional', 'lift', 'hand', 'missing'))))
    for r in rows:
        if r[4] != 'routed':
            print('  %s %s: %s' % (r[0], r[1], r[4]))
    if check:
        return
    open(SRC, 'w', encoding='utf-8', newline='').write('\n'.join(out))
    with open(os.path.join(HERE, 'ROUTED.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('rva\tbinding\tsymbol\tkind\tverdict\n')
        for r in rows:
            f.write('0x%s\t%s\t%s\t%s\t%s\n' % r)


if __name__ == '__main__':
    main('--check' in sys.argv)
