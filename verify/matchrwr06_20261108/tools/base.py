import csv, subprocess, sys, re, glob
WT = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR06'
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R06.tsv'), delimiter='\t'))
for r in rows:
    rva = r['rva'][2:]
    fs = glob.glob(WT + '/re/match/rwl0*/' + rva + '.c')
    if not fs:
        print(rva, 'NOFILE'); continue
    src = fs[0]; s = open(src).read()
    m = re.search(r'^([^\n/]*?)\bFUN_%s\s*\(' % rva, s, re.M)
    sig = m.group(1).strip() if m else '?'
    out = subprocess.run([sys.executable, WT + '/re/tools/match/match.py', src, '--show', '--only', '0x' + rva],
                         capture_output=True, text=True, cwd=WT).stdout
    hm = re.search(r'0x%s \S+: (.*?) flags' % rva, out)
    right = [l.split('|', 1)[1].strip() for l in out.splitlines() if '|' in l and l.startswith('   ')]
    tails = []
    if sig.startswith('void'):
        for i, ins in enumerate(right):
            if ins.startswith('ret'):
                tails.append(' ; '.join(right[max(0, i - 3):i]))
    print(rva, r['size'], hm.group(1) if hm else 'FAIL', '|', sig, '||', ' ## '.join(tails)[:250])
