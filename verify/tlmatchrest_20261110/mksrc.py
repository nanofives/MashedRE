"""TL-MATCH-REST: write re/match/tlr/<rva>.c = FLAGS line + header comment + the shared MATCH preamble (copied from
re/match/s29/00455400.c) + a body file.
usage: py -3.12 verify/tlmatchrest_20261110/mksrc.py <rva> <flags> <header-comment> <body-file>
"""
import pathlib, sys

REPO = pathlib.Path(__file__).resolve().parents[2]
rva, flags, header, body = sys.argv[1], sys.argv[2], sys.argv[3], pathlib.Path(sys.argv[4]).read_text(encoding='utf-8')
src = (REPO / 're/match/s29/00455400.c').read_text(encoding='utf-8')
a = src.index('/* ---- MATCH-S29 preamble')
b = src.index('/* ---- end preamble ---- */') + len('/* ---- end preamble ---- */')
pre = src[a:b].replace('MATCH-S29 preamble (same text in every re/match/s09 source',
                       'MATCH-TLR preamble (same text in every re/match/tlr source')
out = REPO / 're/match/tlr' / (rva + '.c')
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text('// FLAGS: %s\n/* %s */\n%s\n%s' % (flags, header, pre, body), encoding='utf-8', newline='\n')
print(out)
