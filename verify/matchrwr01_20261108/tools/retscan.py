"""For each R01 row: print pct, rva, signature return type, and the original's last 3 insns before each ret."""
import csv, subprocess, sys, re
WT = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR01'
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R01.tsv'), delimiter='\t'))
for r in rows:
    rva = r['rva']
    src = WT + '/' + r['source'].replace('\\', '/')
    try:
        s = open(src).read()
    except OSError:
        print(rva, 'NOFILE'); continue
    m = re.search(r'^([^\n/]*?)\bFUN_%s\s*\(' % rva, s, re.M)
    sig = m.group(1).strip() if m else '?'
    if not sig.startswith('void'):
        continue
    out = subprocess.run([sys.executable, WT + '/re/tools/match/match.py', src, '--show', '--only', '0x' + rva],
                         capture_output=True, text=True, cwd=WT).stdout
    right = [l.split('|', 1)[1].strip() for l in out.splitlines() if '|' in l and l.startswith('   ')]
    tails = []
    for i, ins in enumerate(right):
        if ins.startswith('ret'):
            tails.append(' ; '.join(right[max(0, i - 3):i]))
    print(r['best_pct'], rva, sig, '||', ' ## '.join(tails)[:300])
