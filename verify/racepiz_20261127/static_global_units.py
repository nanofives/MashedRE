# RACE-PIZ-DEVICE: re/match sources that declare an original-address global (DAT_<8 hex>) as a C file-static
# (`static T DAT_xxxxxxxx;` at file scope, the C branch of `#ifndef __cplusplus`), crossed with the object build's
# BYTECHECK.tsv: each such source linked as a DDK object reads a private zero-initialised copy instead of the original
# address (the object build's 0x0047ab30 AV, RESULT section 3).
#   py -3.12 verify/racepiz_20261127/static_global_units.py [mashedmod/build/ddkobj/BYTECHECK.tsv]
import csv, pathlib, re, sys

WT = pathlib.Path(__file__).resolve().parent.parent.parent
bc = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else WT / 'mashedmod/build/ddkobj/BYTECHECK.tsv'
pat = re.compile(r'^static\s+[A-Za-z_][\w\s\*]*?\b(DAT_[0-9a-fA-F]{8})\s*(?:\[[^\]]*\])?\s*;', re.M)
srcs = {}
for p in sorted((WT / 're/match').rglob('*.c')):
    names = pat.findall(p.read_text(encoding='utf-8', errors='replace'))
    if names:
        srcs[p.relative_to(WT).as_posix()] = sorted(set(names))
rows = list(csv.reader(open(bc, encoding='utf-8'), delimiter='\t'))
hdr = rows[0]
linked = {}
for r in rows[1:]:
    if r and r[-1] in srcs:
        linked.setdefault(r[-1], []).append((r[0], r[hdr.index('verdict')] if 'verdict' in hdr else r[7]))
print('sources with a static DAT_ global:', len(srcs))
print('of them linked as DDK units in', bc.as_posix(), ':', len(linked))
for s in sorted(srcs):
    print('%-28s %-40s %s' % (s, ','.join(srcs[s]), ';'.join('%s %s' % x for x in linked.get(s, [])) or 'not linked'))
