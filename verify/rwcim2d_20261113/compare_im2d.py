"""RWC-FIRST-FRAME-IM2D: compare the first loading-disc draw 0x00428450 on the RW core (exe log, [IM2D] lines from
Match/PortTop_00493710.cpp) against the original (orig_im2d_boot.py), field for field.

usage: py -3.12 verify/rwcim2d_20261113/compare_im2d.py <orig.log> <exe mashed_re.log> [--out <tsv>]

Compared exactly: the call's arguments, [0x0067d974] before / after (hex), the screen dims, every RenderStateSet (state,
value, result) in order, the Im2DRenderPrimitive type / count / result, and each vertex's seven dwords (hex). Masked
(process addresses, not values the code computes): RwGlobals, [0x00771960], the texture pointer and the verts pointer.
The texture is compared as "nonzero and equal to RenderStateSet(1)'s value on the same side".
Exit 0 = IDENTICAL, 1 = DIFFERENT, 2 = a side has no draw.
"""
import re
import sys

HEAD = re.compile(r'0x00428450\((-?\d+), (-?\d+)\) .*?\[0x0067d974\] ([0-9A-F]{8}).*?-> tex ([0-9A-F]{8}), screen '
                  r'\[0x0067ea54\] (\d+) x \[0x0067ea56\] (\d+)')
RS = re.compile(r'RenderStateSet\((\d+), 0x([0-9A-F]{8})\) = (-?\d+)')
PRIM = re.compile(r'Im2DRenderPrimitive\(type (\d+), verts [0-9A-F]{8}, n (\d+)\)')
PRIMR = re.compile(r'Im2DRenderPrimitive = (-?\d+)')
VTX = re.compile(r'\] v(\d) hex ((?:[0-9A-F]{8} ){6}[0-9A-F]{8})')
RET = re.compile(r'0x00428450 returned; \[0x0067d974\] ([0-9A-F]{8})')


def parse(path):
    ev, tex = [], None
    for ln in open(path, encoding='latin-1'):
        if '[IM2D]' not in ln:
            continue
        m = HEAD.search(ln)
        if m:
            tex = m.group(4)
            ev.append(('call args', '%s, %s' % (m.group(1), m.group(2))))
            ev.append(('[0x0067d974] before', m.group(3)))
            ev.append(('screen', '%s x %s' % (m.group(5), m.group(6))))
            ev.append(('tex nonzero', str(tex != '00000000')))
            continue
        m = RS.search(ln)
        if m:
            v = m.group(2)
            if m.group(1) == '1' and tex and v == tex:
                v = 'tex (= the texture read from [0x00771960])'
            ev.append(('RenderStateSet', '(%s, %s) = %s' % (m.group(1), v, m.group(3))))
            continue
        m = PRIM.search(ln)
        if m:
            ev.append(('Im2DRenderPrimitive', 'type %s n %s' % m.groups()))
            continue
        m = PRIMR.search(ln)
        if m:
            ev.append(('Im2DRenderPrimitive result', m.group(1)))
            continue
        m = VTX.search(ln)
        if m:
            ev.append(('vertex %s' % m.group(1), m.group(2)))
            continue
        m = RET.search(ln)
        if m:
            ev.append(('[0x0067d974] after', m.group(1)))
            break   # first draw only
    return ev


def main():
    a, b = parse(sys.argv[1]), parse(sys.argv[2])
    out = sys.argv[sys.argv.index('--out') + 1] if '--out' in sys.argv else None
    if not a or not b:
        print('no draw on the %s side' % ('original' if not a else 'exe'))
        return 2
    rows, ndiff = [], 0
    for i in range(max(len(a), len(b))):
        x = a[i] if i < len(a) else ('-', '-')
        y = b[i] if i < len(b) else ('-', '-')
        same = x == y
        ndiff += not same
        rows.append((i, x[0], x[1], y[0] if y[0] != x[0] else '', y[1], 'same' if same else 'DIFF'))
    for r in rows:
        print('%2d %-28s orig %-48s exe %s%-48s %s' % (r[0], r[1], r[2], (r[3] + ': ') if r[3] else '', r[4], r[5]))
    verdict = 'IDENTICAL' if ndiff == 0 else 'DIFFERENT (%d of %d fields)' % (ndiff, len(rows))
    print('verdict: %s, %d fields compared' % (verdict, len(rows)))
    if out:
        with open(out, 'w', encoding='utf-8', newline='\n') as f:
            f.write('n\tfield\toriginal\texe_field\texe\tresult\n')
            for r in rows:
                f.write('\t'.join(str(c) for c in r) + '\n')
            f.write('# verdict: %s\n' % verdict)
    return 0 if ndiff == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
