"""MATCH-RWR01 results.tsv + relocheck.tsv over the 79 R01 rows. Run from the worktree."""
import csv, hashlib, re, subprocess, sys, pathlib
WT = pathlib.Path('C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR01')
OUT = WT / 'verify/matchrwr01_20261108'
OUT.mkdir(parents=True, exist_ok=True)
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R01.tsv'), delimiter='\t'))
NEW = {  # rows reworked in this slice: class, reason
 '004e15f0': ('identical', 'tail block through the typed end pointer (base/index order)'),
 '004ccc50': ('identical', 'byte pointer to the bitmap set before the loop (index/base order)'),
 '004ccba0': ('identical', 'one bitmap byte pointer + fresh for-loop index'),
 '004c5a60': ('identical', 'refcount-- then test, ++ on the free path'),
 '004e4380': ('identical', 'returns param_1 (Ghidra void)'),
 '004e5820': ('identical', 'returns p; /O2 /Ob1 /G5 like the slice'),
 '004e4130': ('identical', 'permuter swap: cur = nx before the free callback'),
 '004e4f70': ('identical', 'double recursion, second call tail-looped by the compiler'),
 '004e55d0': ('identical', 'stack pop cur = stack[sp--], push via saved node; ff8 + 71cc global order'),
 '004e5700': ('identical', 'same stack walk as 004e55d0; flag test before the +0x1c load'),
 '004e5c30': ('identical', 'count hoisted to a local, array pointer reloaded per iteration'),
 '004c3e20': ('(a)/(b)', 'loads fixed via typed char* base; two stores after the call keep [eax+edx]'),
 '004e5bf0': ('b', 'head first, head != cur, early return; param_1 load scheduling'),
 '004e5d30': ('b', 'nested if chain; original keeps one ret per failing call'),
}
res = open(OUT / 'results.tsv', 'w', newline='')
res.write('rva\tsize\tbest%\tregs%\tflags\tby\tsha256\tclass\treason\tbefore%\n')
rel = open(OUT / 'relocheck.tsv', 'w', newline='')
n100 = 0
for r in rows:
    rva = r['rva']
    src = WT / r['source'].replace('\\', '/')
    if not src.exists():
        res.write(f"0x{rva}\t{r['size']}\t-1\t-1\t\t\t\tno source\t\t{r['best_pct']}\n"); continue
    out = subprocess.run([sys.executable, str(WT / 're/tools/match/match.py'), str(src), '--only', '0x' + rva],
                         capture_output=True, text=True, cwd=WT).stdout
    m = re.search(r'0x%s \S+: ([\d.]+)%% \(regs ([\d.]+)%%.*flags=(.*?) tc=' % rva, out)
    if not m:
        res.write(f"0x{rva}\t{r['size']}\t-1\t-1\t\t\t\tcompile fail\t\t{r['best_pct']}\n"); continue
    pct, regs, flags = m.group(1), m.group(2), m.group(3)
    sha = hashlib.sha256(src.read_bytes()).hexdigest()[:16]
    cls, reason = NEW.get(rva, (r['class'], r['reason']))
    if float(pct) == 100.0:
        n100 += 1
        rc = subprocess.run([sys.executable, str(WT / 're/tools/match/relocheck.py'), str(src)],
                            capture_output=True, text=True, cwd=WT).stdout
        line = [l for l in rc.splitlines() if l.startswith('0x' + rva)]
        rel.write((line[0] if line else f'0x{rva}\tMISSING') + '\n')
        cls = 'identical'
    res.write(f"0x{rva}\t{r['size']}\t{pct}\t{regs}\t{flags}\tinsn\t{sha}\t{cls}\t{reason}\t{r['best_pct']}\n")
    print(rva, r['best_pct'], '->', pct)
print('n100', n100)
