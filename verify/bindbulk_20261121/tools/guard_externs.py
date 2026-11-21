"""PORT-OT-RACE: put a source's plain `extern <T> DAT_/LAB_<addr>...;` data declarations under #ifndef MATCH_PORT and
bind the port (MATCH_PORT, C++) to the original address instead. The DDK never defines MATCH_PORT, so its object is
unchanged (checked with re/tools/match/match.py after the edit). usage: py -3.12 guard_externs.py <src.c> [...]"""
import re, sys
PAT = re.compile(r'^(\s*)extern\s+((?:const\s+)?(?:struct\s+)?[A-Za-z_]\w*(?:\s*\*+)?)\s*(_?(?:DAT|LAB)_([0-9a-fA-F]{8}))\s*(\[\s*([0-9xXa-fA-F]*)\s*\])?\s*;\s*$')
for p in sys.argv[1:]:
    raw = open(p, 'rb').read()
    nl = '\r\n' if b'\r\n' in raw else '\n'
    lines = raw.decode('utf-8').split(nl)
    out, n = [], 0
    for l in lines:
        m = PAT.match(l)
        if not m or 'MATCH_PORT' in l:
            out.append(l); continue
        ind, T, name, addr, arr, N = m.groups()
        T = ' '.join(T.split())
        if arr is not None:
            N = N or '1'
            d = '#define %s (*(%s (*)[%s])0x%su)' % (name, T, N, addr.lower())
        else:
            d = '#define %s (*(%s *)0x%su)' % (name, T, addr.lower())
        out += ['#ifdef MATCH_PORT   /* PORT-OT-RACE: the port reads it at its original address */', d, '#else', l, '#endif']
        n += 1
    open(p, 'wb').write(nl.join(out).encode('utf-8'))
    print(p, 'guarded', n)
