# DDKOBJ-STATIC-GLOBALS: every re/match source with a file-scope `static ... DAT_<8 hex>` (any form, initialised or not),
# with the C view (DDK) and the C++/port view (VS2022) declaration of each such name.
#   py -3.12 verify/ddkobj_staticglobals_20261127/inventory.py
import pathlib, re, sys
WT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(WT / 're/tools/ddkobj')); sys.path.insert(0, str(WT / 're/tools/match'))
import ddkobj as D
ST = re.compile(r'^static\s+(?:const\s+|volatile\s+)*[A-Za-z_][\w\s\*]*?\b(DAT_[0-9a-fA-F]{8})\s*(\[[^\]]*\])?\s*(=[^;]*)?;')
for p in sorted((WT / 're/match').rglob('*.c')):
    t = p.read_text(encoding='utf-8', errors='replace')
    cview = list(D.active_lines(t, {"_MSC_VER", "_WIN32"}))
    hits = {}
    for l in cview:
        m = ST.match(l.strip())
        if m:
            hits[m.group(1)] = ('init' if m.group(3) else 'zero') + (' array' if m.group(2) else '')
    if not hits:
        continue
    info = D.parse_source(p, cxx=True)
    pview = list(D.active_lines(t, {"MATCH_PORT", "MASHED_STANDALONE", "_MSC_VER", "_WIN32", "__cplusplus"}))
    for n, k in sorted(hits.items()):
        pv = [l.strip() for l in pview if n in l and ('MGLOBAL' in l or l.strip().startswith(('static', 'extern')))]
        a = info['globals'].get(n)
        print('%s\t%s\t%s\t%s\t%s' % (p.relative_to(WT).as_posix(), n, k, ('0x%08x' % a) if a is not None else '-',
                                       ' | '.join(pv)[:120] or '-'))
