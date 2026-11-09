"""PORT-TOP-04C helper: walk a matched function's callee tree through the matched sources and hooks.csv.
For every RVA prints depth, name, the matched source (from the hooks.csv 'match.py <path>' note), the exe body
(exe_file / port columns) and the callees the source declares. Read-only.
usage: py -3.12 verify/porttop_20261108/survey_tree.py 0x00402a40 [--depth N]
"""
import csv, os, re, sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
rows = {}
with open(os.path.join(ROOT, 'hooks.csv'), encoding='utf-8', newline='') as f:
    for r in csv.DictReader(f):
        try:
            rows[int(r['rva'], 16)] = r
        except ValueError:
            pass

MP = re.compile(r'match\.py (re/match/\S+?\.c)')
DECL = re.compile(r'^[\w\s\*]+?\b((?:thunk_)?FUN_([0-9a-f]{8}))\s*\(', re.M)


def src_of(rva):
    r = rows.get(rva)
    if r:
        m = MP.search(r.get('notes', ''))
        if m and os.path.exists(os.path.join(ROOT, m.group(1))):
            return m.group(1)
    return None


def callees(path, rva):
    txt = open(os.path.join(ROOT, path), encoding='utf-8', errors='replace').read()
    out = []
    for m in DECL.finditer(txt):
        c = int(m.group(2), 16)
        if c != rva and (m.group(1), c) not in out:
            out.append((m.group(1), c))
    return out


def main():
    root = int(sys.argv[1], 16)
    maxd = int(sys.argv[sys.argv.index('--depth') + 1]) if '--depth' in sys.argv else 4
    seen = set()

    def walk(rva, d, via):
        r = rows.get(rva, {})
        s = src_of(rva)
        tag = ' (dup)' if rva in seen else ''
        print('%s0x%08x %-28s via=%-22s src=%-28s exe=%s port=%s%s' % (
            '  ' * d, rva, (r.get('name') or '?')[:28], via, s or '-', r.get('exe_file') or '-',
            (r.get('port') or '-')[:6], tag))
        if rva in seen or d >= maxd or not s:
            return
        seen.add(rva)
        for n, c in callees(s, rva):
            walk(c, d + 1, n)

    walk(root, 0, '-')


if __name__ == '__main__':
    main()
