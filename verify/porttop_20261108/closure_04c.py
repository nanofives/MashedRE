"""PORT-TOP-04C helper (read-only): classify every node of a matched function's callee tree.

state per RVA:
  exe     a body is already compiled into mashed_re.exe (bind by name)
  rw      LibRw/RwApiBind.cpp has a MashedRw_*_<rva> binding
  asitu   only an asi-only TU holds the body (add that TU to the exe list, one body in both targets)
  gen     a matched source exists and no target has a body (generate an exe-only PortTop TU)
  none    no source, no body (stand-in)
usage: py -3.12 verify/porttop_20261108/closure_04c.py 0x00402a40 [--depth N]
"""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
sys.path.insert(0, HERE)
import rva_body_scan as S          # noqa: E402
import survey_tree as T            # noqa: E402

scan = S.scan_all()
exe_tus, asi_tus = scan['targets']['exe'], scan['targets']['asi']
eb, ab = S.rva_to_tus(scan, 'exe'), S.rva_to_tus(scan, 'asi')
rwtxt = open(os.path.join(ROOT, 'mashedmod/src/mashed_re/LibRw/RwApiBind.cpp'), encoding='utf-8').read()
RW = {int(m.group(2), 16): m.group(0) for m in re.finditer(r'MashedRw_(\w+?)_([0-9a-f]{8})\b', rwtxt)}


def state(rva):
    if rva in eb:
        t = eb[rva][0]
        return 'exe', '%s %s' % (scan['bodies'][t][rva][0], t.split('/')[-1])
    if rva in RW:
        return 'rw', RW[rva]
    if rva in ab:
        t = ab[rva][0]
        return 'asitu', '%s %s' % (scan['bodies'][t][rva][0], t.split('/')[-1])
    if T.src_of(rva):
        return 'gen', T.src_of(rva)
    return 'none', '-'


def main():
    root = int(sys.argv[1], 16)
    maxd = int(sys.argv[sys.argv.index('--depth') + 1]) if '--depth' in sys.argv else 2
    seen = {}

    def walk(rva, d, via):
        st, info = state(rva)
        name = (T.rows.get(rva, {}).get('name') or '?')[:26]
        dup = rva in seen
        print('%d %s0x%08x %-26s %-5s %s%s' % (d, '  ' * d, rva, name, st, info, ' (dup)' if dup else ''))
        if dup or d >= maxd:
            return
        seen[rva] = st
        if st in ('exe', 'rw'):
            return
        s = T.src_of(rva)
        if s:
            for n, c in T.callees(s, rva):
                walk(c, d + 1, n)

    walk(root, 0, '-')


if __name__ == '__main__':
    main()


def single_tu(rva):
    """asi-only TU that holds only this RVA's body (Match/MatchSxx_<rva>.cpp shape)."""
    t = ab[rva][0]
    return t if len(scan['bodies'][t]) == 1 else None


def plan(root, maxd=8):
    """Fixpoint: a gen / single-TU asitu node is accepted when each callee is exe, rw or accepted.
    Level-1 callees of root are accepted unconditionally (their unresolved callees become stand-ins)."""
    nodes, kids = {}, {}

    def walk(rva, d):
        if rva in nodes and nodes[rva] <= d:
            return
        nodes[rva] = d
        st, _ = state(rva)
        if st in ('exe', 'rw') or d >= maxd:
            kids.setdefault(rva, [])
            return
        s = T.src_of(rva)
        kids[rva] = [c for _, c in T.callees(s, rva)] if s else []
        for c in kids[rva]:
            walk(c, d + 1)

    walk(root, 0)
    acc = set()
    changed = True
    while changed:
        changed = False
        for r in nodes:
            if r in acc:
                continue
            st, _ = state(r)
            if st == 'gen' or (st == 'asitu' and single_tu(r)):
                if all(state(c)[0] in ('exe', 'rw') or c in acc for c in kids.get(r, [])):
                    acc.add(r)
                    changed = True
    return nodes, kids, acc


if __name__ == '__main__' and '--plan' in sys.argv:
    root = int(sys.argv[1], 16)
    nodes, kids, acc = plan(root)
    lvl1 = kids[root]
    print('accepted closed nodes:', len(acc))
    for r in sorted(acc):
        print('  ACC 0x%08x d=%d %s %s' % (r, nodes[r], state(r)[0], state(r)[1]))
    print('level-1:')
    for c in lvl1:
        st, info = state(c)
        miss = [k for k in kids.get(c, []) if not (state(k)[0] in ('exe', 'rw') or k in acc)]
        print('  L1 0x%08x %-5s %-45s open=%s' % (c, st, info[:45], ' '.join('%08x' % k for k in miss)))
