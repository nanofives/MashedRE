"""filecol_candidates.py -> stdout TSV. PORT-TRACKER-FILECOL: hooks.csv rows whose `file` names a mashedmod source while
the one matched TU in `exe_file` (Match/*.cpp) is also compiled in the .asi (mashedmod/asi_sources.rsp), i.e. the
matched TU is the body in both targets and `file` may be stale. Scopes: the 5 PORT-STATE-02 rows, the 2 PORT-STATE-01
rows, and every RVA named in verify/porttop_20261108/RESULT_PORT-TOP-04{B..G}.md. For each candidate it also reports
whether the old file still holds an RH_ScopedInstall / install line or a `// 0x<rva>` tag for the RVA (comment-only
mentions are not bodies)."""
import csv, re, os, sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
os.chdir(REPO)
rows = list(csv.reader(open('hooks.csv', encoding='utf-8', newline='')))
h = rows[0]
F, E, P = h.index('file'), h.index('exe_file'), h.index('port')
asi = set()
for l in open('mashedmod/asi_sources.rsp', encoding='utf-8', errors='replace'):
    l = l.strip().strip('"').replace(chr(92), '/')
    if l:
        asi.add('mashedmod/src/mashed_re/' + l)
txt = ''
for x in 'BCDEFG':
    p = Path(f'verify/porttop_20261108/RESULT_PORT-TOP-04{x}.md')
    if p.exists():
        txt += p.read_text(encoding='utf-8', errors='replace')
named = {m.lower() for m in re.findall(r'0x(00[0-9a-fA-F]{6})', txt)}
scopes = [('PORT-STATE-02', {'0040e480', '0040ce80', '004769d0', '00476ae0', '00476b30', '0046dd80'}),
          ('PORT-STATE-01', {'00430830', '004309b0'}),
          ('PORT-TOP-04B..G', named)]
print('scope\trva\tname\tconf\tport\tfile\tmatched_tu\told_file_install_hits\texe_file')
for scope, sel in scopes:
    for r in rows[1:]:
        if len(r) <= P:
            continue
        rva = r[0].lower().replace('0x', '').zfill(8)
        if rva not in sel:
            continue
        f, e = r[F], r[E]
        match = [x for x in e.split('|') if x and '/Match/' in x and x in asi]
        if not (f.startswith('mashedmod/') and len(match) == 1 and f != match[0]):
            if scope != 'PORT-TOP-04B..G':
                print(f'{scope}\t{rva}\t{r[1]}\t{r[3]}\t{r[P]}\t{f}\t(no change: {"already " + f if f in e else "exe_file " + e})\t\t{e}')
            continue
        hits = 0
        if os.path.exists(f):
            for line in open(f, encoding='utf-8', errors='replace'):
                low = line.lower()
                if ('0x' + rva) in low and ('rh_scopedinstall' in low or 'install' in low):
                    hits += 1
        print(f'{scope}\t{rva}\t{r[1]}\t{r[3]}\t{r[P]}\t{f}\t{match[0]}\t{hits}\t{e}')
