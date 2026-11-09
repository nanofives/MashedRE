"""PORT-TOP-04C: give a matched source the MATCH-S02 preamble so the port can compile it as C++ (MATCH_PORT).

  - a g2s raw preamble (typedef undefined ... #define SEXT48) is replaced by the MATCH-S02 preamble text; a source
    with no raw preamble gets the MATCH-S02 preamble after its FLAGS / TOOLCHAIN header lines
  - `extern T DAT_xxxxxxxx;` -> MGLOBAL(T, DAT_xxxxxxxx, 0xxxxxxxxx);  `extern T DAT_x[N];` -> MGLOBALA(...)
Under C (the DDK, match.py) MGLOBAL expands to the same `extern` declaration, so the object code does not change.
Re-run re/tools/match/match.py <src> --bytes after converting. usage: py -3.12 mglobal_convert.py <src.c> [...]
"""
import os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
REF = os.path.join(ROOT, 're', 'match', 's02', '0040bd00.c')


def preamble():
    t = open(REF, encoding='utf-8').read()
    a = t.index('/* ---- MATCH-S02 preamble')
    b = t.index('/* ---- end preamble ---- */') + len('/* ---- end preamble ---- */')
    return t[a:b]


RAW = re.compile(r'^typedef unsigned char undefined;.*?^#define SEXT48[^\n]*\n', re.M | re.S)
EXT = re.compile(r'^extern\s+(.+?)\s*\b((?:_?DAT|PTR)_([0-9a-f]{8}))\s*(\[(\w+)\])?\s*;[ \t]*$', re.M)


def convert(path):
    raw = open(path, encoding='utf-8', newline='').read()
    crlf = '\r\n' in raw
    t = raw.replace('\r\n', '\n')
    if 'MGLOBAL(' in t:
        print('skip (has MGLOBAL):', path)
        return
    pre = preamble() + '\n'
    if RAW.search(t):
        t = RAW.sub(lambda m: pre, t, count=1)
    else:
        lines = t.split('\n')
        i = 0
        while i < len(lines) and re.match(r'^//\s*(FLAGS|TOOLCHAIN):', lines[i]):
            i += 1
        lines.insert(i, pre.rstrip('\n'))
        t = '\n'.join(lines)

    def rep(m):
        ty, name, addr, arr, n = m.group(1).strip(), m.group(2), m.group(3), m.group(4), m.group(5)
        if arr:
            return 'MGLOBALA(%s, %s, %s, 0x%s);' % (ty, name, n, addr)
        return 'MGLOBAL(%s, %s, 0x%s);' % (ty, name, addr)

    t, k = EXT.subn(rep, t)
    open(path, 'w', encoding='utf-8', newline='').write(t.replace('\n', '\r\n') if crlf else t)
    print('converted %s (%d globals)' % (path, k))


if __name__ == '__main__':
    for p in sys.argv[1:]:
        convert(p)
