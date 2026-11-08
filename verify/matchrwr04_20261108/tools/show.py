"""show.py <rva> [ctx] : print the function source (from MATCH: line) and the match diff with context around '!' rows."""
import csv, subprocess, sys
WT = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR04'
rva = sys.argv[1].lower().replace('0x', '').zfill(8)
ctx = int(sys.argv[2]) if len(sys.argv) > 2 else 2
rows = {r['rva']: r for r in csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R04.tsv'), delimiter='\t')}
src = rows[rva]['source'].replace(chr(92), '/')
s = open(WT + '/' + src).read()
print(src)
print(s[s.find('// MATCH:'):] if '// MATCH:' in s else s[-3000:])
out = subprocess.run([sys.executable, WT + '/re/tools/match/match.py', src, '--show', '--only', '0x' + rva],
                     capture_output=True, text=True, cwd=WT).stdout.splitlines()
print(out[0] if out else 'NO OUTPUT')
L = out[1:]
keep = set()
for i, l in enumerate(L):
    if l.strip().startswith('!'):
        keep.update(range(max(0, i - ctx), min(len(L), i + ctx + 1)))
last = -2
for i in sorted(keep):
    if i != last + 1:
        print('   ...')
    print('%3d %s' % (i, L[i]))
    last = i
