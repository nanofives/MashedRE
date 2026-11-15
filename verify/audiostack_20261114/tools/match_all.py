"""Run match.py --bytes and relocheck.py on every re/match source behind a BINDINGS.tsv kind=matched row.
usage: py -3.12 verify/audiostack_20261114/tools/match_all.py <out.tsv>
Source per row: the re/match paths in the BINDINGS note; else the re/match file whose name is the rva; else every
re/match file carrying a '// MATCH: 0x<rva>' annotation. Prints one '[N/M] <src> STATUS' line per source."""
import csv, glob, os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor

out_path = sys.argv[1]
idx = {}
for p in glob.glob('re/match/**/*.c*', recursive=True):
    p = p.replace(os.sep, '/')
    t = open(p, encoding='utf-8', errors='replace').read()
    for m in re.finditer(r'//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x([0-9a-fA-F]+)', t):
        idx.setdefault(int(m.group(1), 16), []).append(p)
rows = []
for r in csv.DictReader(open('verify/porttop_20261108/BINDINGS.tsv', encoding='utf-8'), delimiter='\t'):
    if r['kind'] != 'matched':
        continue
    rva = int(r['rva'], 16)
    srcs = [s for s in re.findall(r're/match/[\w/]+\.c(?:pp)?', r['note']) if rva in [k for k, v in idx.items() if s in v]]
    how = 'note'
    if not srcs:
        cand = idx.get(rva, [])
        named = [c for c in cand if os.path.basename(c).split('.')[0].lower().endswith('%08x' % rva)]
        srcs, how = (named, 'name') if named else (cand, 'annot')
    rows.append((rva, r['exe_symbol'], srcs, how))
files = sorted({s for _, _, ss, _ in rows for s in ss})
MRE = re.compile(r'^0x([0-9a-f]{8}) (\S+): ([\d.]+)%')
BRE = re.compile(r'bytes: .*-> (.*)$')
res = {}
def run(f):
    m = subprocess.run([sys.executable, 're/tools/match/match.py', '--bytes', f], capture_output=True, text=True)
    rc = subprocess.run([sys.executable, 're/tools/match/relocheck.py', f], capture_output=True, text=True)
    per = {}
    cur = None
    for line in m.stdout.splitlines():
        mm = MRE.match(line.strip())
        if mm:
            cur = int(mm.group(1), 16); per[cur] = {'pct': mm.group(3), 'bytes': '-', 'reloc': '-'}
            continue
        bb = BRE.search(line)
        if bb and cur is not None:
            per[cur]['bytes'] = bb.group(1).strip()
    for line in rc.stdout.splitlines():
        p = line.split('\t')
        if p and p[0].startswith('0x') and len(p) > 1:
            a = int(p[0], 16)
            per.setdefault(a, {'pct': '-', 'bytes': '-', 'reloc': '-'})['reloc'] = p[1] + ('' if p[1] == 'OK' else ' ' + (p[-1][:160] if len(p) > 5 else ''))
        elif len(p) > 1 and p[1] == 'ERROR':
            per.setdefault(-1, {'pct': '-', 'bytes': '-', 'reloc': 'ERROR ' + p[2]})
    if not m.stdout.strip():
        per.setdefault(-2, {'pct': 'ERR', 'bytes': (m.stderr.strip().splitlines() or ['?'])[-1][:160], 'reloc': '-'})
    return f, per
with ThreadPoolExecutor(6) as ex:
    for i, (f, per) in enumerate(ex.map(run, files), 1):
        res[f] = per
        bad = [a for a, v in per.items() if v['pct'] != '100.0' or 'IDENTICAL' not in v['bytes'] or v['reloc'] != 'OK']
        print(f'[{i}/{len(files)}] {f} {"PASS" if not bad else "CHECK"}', flush=True)
with open(out_path, 'w', encoding='utf-8', newline='') as o:
    o.write('rva\texe_symbol\tsrc\tsrc_from\tpct\tbytes\trelocheck\n')
    for rva, sym, srcs, how in rows:
        if not srcs:
            o.write(f'0x{rva:08x}\t{sym}\t-\tnone\t-\t-\t-\n'); continue
        for s in srcs:
            v = res[s].get(rva) or res[s].get(-2) or res[s].get(-1) or {'pct': 'NOT-IN-OUTPUT', 'bytes': '-', 'reloc': '-'}
            o.write(f'0x{rva:08x}\t{sym}\t{s}\t{how}\t{v["pct"]}\t{v["bytes"]}\t{v["reloc"]}\n')
print('rows', len(rows), 'files', len(files))
