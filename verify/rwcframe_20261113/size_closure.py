"""RWC-FRAME-492E90: size and slice the game-side closure of 0x00492e90 over re/match sources.

usage: py -3.12 verify/rwcframe_20261113/size_closure.py [root ...]
Walk: from the root(s), every FUN_/LAB_ name in the picked source (gen_rwcore_tus.py's picker), game RVAs only
(outside the RW bands); RW-band callees are listed but not walked. Per member: best_pct, the INT3 census
(re/coverage/reach.tsv reached_menu / reached_race), the exe TUs that already compile its re/match source, its
depth, and its callers in the closure. Writes CLOSURE_492E90.tsv beside this file.
"""
import csv, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'verify', 'rwccorerun_20261111'))
import gen_rwcore_tus as G  # noqa: E402

def rwband(v):
    return 0x4c0700 <= v < 0x516000 or 0x52df70 <= v < 0x558c50 or 0x5cb000 <= v < 0x5cc000

def main():
    roots = [a.lower().replace('0x', '').zfill(8) for a in sys.argv[1:]] or ['00492e90']
    G.load_named()
    led = G.ledger()
    srcs = G.all_sources()
    def pick(a):
        if a in led: return led[a][1]
        s = srcs.get(a)
        return s[0] if s else None
    reach = {}
    for r in csv.DictReader(open(os.path.join(ROOT, 're', 'coverage', 'reach.tsv'), encoding='utf-8'), delimiter='\t'):
        reach[r['rva'].lower()] = (r['reached_menu'], r['reached_race'])
    # exe TUs that include a re/match source
    inc = {}
    rsp = [l.strip().strip('"') for l in open(os.path.join(ROOT, 'mashedmod', 'exe_sources.rsp'), encoding='utf-8')
           if l.strip() and not l.startswith('#')]
    for t in rsp:
        p = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', t.replace(chr(92), os.sep))
        if not os.path.exists(p) or os.path.basename(p).startswith('RwCore_'):
            continue
        for m in re.finditer(r'#include\s+"[^"]*re/match/[^"]*?([0-9a-f]{8})\.c"', open(p, encoding='utf-8', errors='replace').read()):
            inc.setdefault(m.group(1), []).append(t)
    seen, depth, callers, rwcallees, order = {}, {}, {}, {}, []
    st = [(r, 0, None) for r in roots]
    while st:
        a, d, c = st.pop(0)
        if c: callers.setdefault(a, set()).add(c)
        if a in seen:
            continue
        seen[a] = pick(a)
        depth[a] = d
        order.append(a)
        s = seen[a]
        if not s:
            continue
        t = open(os.path.join(ROOT, s), encoding='utf-8', errors='replace').read()
        own = {r_ for r_, _ in G.defs_of(t)}
        body = G.body_text(t)
        names = set(m.group(1) for m in G.NAME.finditer(body)) | {w for w in re.findall(r'[A-Za-z_]\w*', body) if w in G.NAMED}
        for n in names:
            r = G.NAMED.get(n) or G.rva_of(n)
            if not r or r in own:
                continue
            if rwband(int(r, 16)):
                rwcallees.setdefault(r, set()).add(a)
                continue
            st.append((r, d + 1, a))
    out = os.path.join(HERE, 'CLOSURE_492E90.tsv')
    with open(out, 'w', encoding='utf-8', newline='\n') as f:
        f.write('rva\tdepth\tpct\tsource\treach_menu\treach_race\texe_tus\tcallers\n')
        for a in order:
            p = led.get(a, (None,))[0]
            rm, rr = reach.get(a, ('?', '?'))
            f.write('%s\t%d\t%s\t%s\t%s\t%s\t%s\t%s\n' % (a, depth[a], '' if p is None else p, seen[a] or '', rm, rr,
                    ','.join(inc.get(a, [])), ','.join(sorted(callers.get(a, [])))))
        for r in sorted(rwcallees):
            rm, rr = reach.get(r, ('?', '?'))
            f.write('%s\tRW\t%s\t%s\t%s\t%s\t\t%s\n' % (r, led.get(r, ('',))[0], pick(r) or '', rm, rr, ','.join(sorted(rwcallees[r]))))
    game = [a for a in order]
    def cnt(pred): return sum(1 for a in game if pred(a))
    ex = lambda a: reach.get(a, ('?',))[0] == '1'
    full = lambda a: led.get(a, (0,))[0] == 100.0
    print('game members %d: 100%% %d, below %d, no source %d' % (len(game), cnt(full), cnt(lambda a: seen[a] and not full(a)), cnt(lambda a: not seen[a])))
    print('executed at menu %d (100%% %d, below %d, no source %d); not at menu %d; census-unknown %d' % (
        cnt(ex), cnt(lambda a: ex(a) and full(a)), cnt(lambda a: ex(a) and seen[a] and not full(a)), cnt(lambda a: ex(a) and not seen[a]),
        cnt(lambda a: reach.get(a, ('?',))[0] == '0'), cnt(lambda a: a not in reach)))
    print('RW-band callees %d (menu-executed %d)' % (len(rwcallees), sum(1 for r in rwcallees if reach.get(r, ('?',))[0] == '1')))
    print('wrote', out)

main()
