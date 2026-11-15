#!/usr/bin/env py -3.12
"""callee_arity_lint.py - text-only lint over re/match sources.

Lists every callee FUN_xxxxxxxx declared (prototype line, no body) with a
different parameter count or a different normalised prototype in different
sources. Arity is taken from the declarations as written (NO-GUESSING).
A callee whose own source (re/match/<dir>/<rva>.c) defines it and whose
hooks.csv notes carry c4=match uses that definition as the reference.

Usage: py -3.12 re/tools/match/callee_arity_lint.py [--out FILE.md] [--tsv FILE.tsv]
"""
import csv, os, re, sys, argparse, collections

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
MATCH = os.path.join(ROOT, 're', 'match')

# one prototype or definition head: <ret> FUN_xxxxxxxx(<params>) followed by ; or {
PROTO = re.compile(r'^[ \t]*([A-Za-z_][\w \t\*]*?[ \t\*])(FUN_[0-9a-fA-F]{8})[ \t]*\(([^)]*)\)[ \t]*(;|\{|$)', re.M)


def split_params(s):
    s = s.strip()
    if s in ('', 'void'):
        return [], False
    var = False
    out = []
    depth = 0
    cur = ''
    for ch in s:
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip()); cur = ''
        else:
            cur += ch
    out.append(cur.strip())
    if out and out[-1] == '...':
        var = True
        out = out[:-1]
    return out, var


def norm_type(p):
    p = re.sub(r'\b(const|volatile|struct)\b', '', p)
    # drop trailing identifier (param name)
    m = re.match(r'^(.*?[\s\*])([A-Za-z_]\w*)\s*(\[\d*\])?$', p.strip())
    if m and m.group(1).strip():
        base = m.group(1)
    else:
        base = p
    return re.sub(r'\s+', '', base)


def scan(path):
    try:
        txt = open(path, encoding='utf-8', errors='replace').read()
    except OSError:
        return []
    res = []
    for m in PROTO.finditer(txt):
        ret = re.sub(r'\s+', '', m.group(1))
        if ret in ('return', 'else') or ret.endswith('return'):
            continue
        params, var = split_params(m.group(3))
        unspec = m.group(3).strip() == ''
        line = txt.count('\n', 0, m.start()) + 1
        res.append(dict(rva=m.group(2)[4:].lower(), ret=ret, n=(-1 if unspec else len(params)), var=var,
                        types=tuple(norm_type(p) for p in params),
                        kind='def' if m.group(4) == '{' or m.group(4) == '' else 'decl',
                        line=line))
    return res


def c4_match_rvas():
    out = set()
    with open(os.path.join(ROOT, 'hooks.csv'), encoding='utf-8', newline='') as f:
        for r in csv.DictReader(f):
            if 'c4=match' in (r.get('notes') or ''):
                out.add(r['rva'].lower().zfill(8))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out')
    ap.add_argument('--tsv')
    a = ap.parse_args()
    refs = c4_match_rvas()
    byrva = collections.defaultdict(list)  # rva -> [(relpath, rec)]
    nsrc = 0
    for d, _, fs in os.walk(MATCH):
        for fn in fs:
            if not fn.endswith('.c'):
                continue
            nsrc += 1
            p = os.path.join(d, fn)
            rel = os.path.relpath(p, ROOT).replace('\\', '/')
            seen = set()
            for rec in scan(p):
                k = (rec['rva'], rec['n'], rec['types'], rec['ret'], rec['var'])
                if k in seen:
                    continue
                seen.add(k)
                byrva[rec['rva']].append((rel, rec))
    rows = []
    for rva, lst in sorted(byrva.items()):
        arities = {(r['n'], r['var']) for _, r in lst if r['n'] >= 0}  # n=-1: '()' unspecified, not counted
        protos = {(r['ret'], r['types'], r['var']) for _, r in lst}
        if len(protos) < 2 or (len(arities) < 2 and len(protos) < 2):
            continue
        refsrc = None
        # reference: a definition in its own source with c4=match
        for rel, r in lst:
            if r['kind'] == 'def' and os.path.basename(rel) == rva + '.c':
                refsrc = (rel, r)
        is_c4 = rva in refs
        cls = 'ARITY' if len(arities) > 1 else 'PROTO'
        rows.append(dict(rva=rva, cls=cls, c4ref=bool(refsrc and is_c4), refsrc=refsrc, lst=lst))
    # order: arity+c4 reference first
    rows.sort(key=lambda r: (not (r['cls'] == 'ARITY' and r['refsrc']), r['cls'] != 'ARITY', r['rva']))
    L = []
    L.append(f'# callee arity lint ({nsrc} sources under re/match, {len(byrva)} callees declared)\n')
    ar = [r for r in rows if r['cls'] == 'ARITY']
    L.append(f'ARITY mismatches: {len(ar)} ({sum(1 for r in ar if r["refsrc"])} with an own-source reference, '
             f'{sum(1 for r in ar if r["c4ref"])} with c4=match reference); PROTO-only (same count, different types/return): {len(rows)-len(ar)}\n')
    def fmt(rel, r):
        return f'{rel}:{r["line"]} {r["kind"]} {r["ret"]}({",".join(r["types"])}{",..." if r["var"] else ""}) n={r["n"]}{"+var" if r["var"] else ""}'
    for r in rows:
        ref = ('REF ' + fmt(*r['refsrc']) + (' [c4=match]' if r['c4ref'] else ' [own source, not c4=match]')) if r['refsrc'] else 'no own source'
        L.append(f'## {r["cls"]} 0x{r["rva"]}  ({ref})')
        for rel, rec in r['lst']:
            L.append('- ' + fmt(rel, rec))
        L.append('')
    outtxt = '\n'.join(L)
    if a.out:
        open(a.out, 'w', encoding='utf-8').write(outtxt)
    if a.tsv:
        with open(a.tsv, 'w', encoding='utf-8', newline='') as f:
            w = csv.writer(f, delimiter='\t')
            w.writerow(['rva', 'class', 'reference', 'c4_match_ref', 'n_sources', 'arities'])
            for r in rows:
                w.writerow([r['rva'], r['cls'], r['refsrc'][0] if r['refsrc'] else '', int(r['c4ref']),
                            len(r['lst']), sorted({(x['n']) for _, x in r['lst']})])
    print(f'sources={nsrc} callees={len(byrva)} arity_mismatch={len(ar)} proto_only={len(rows)-len(ar)}')
    for r in ar[:40]:
        print(r['rva'], 'ref' if r['refsrc'] else '-', 'c4' if r['c4ref'] else '-',
              sorted({(x['n']) for _, x in r['lst']}))


if __name__ == '__main__':
    main()
