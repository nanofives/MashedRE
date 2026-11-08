"""MATCH-RWR05 results.tsv + relocheck.tsv over the 79 R05 rows. Run from the worktree."""
import csv, hashlib, re, subprocess, sys, pathlib
WT = pathlib.Path('C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR05')
OUT = WT / 'verify/matchrwr05_20261108'
OUT.mkdir(parents=True, exist_ok=True)
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R05.tsv'), delimiter='\t'))
NEW = {  # rows reworked in this slice: class, reason
 '0057ae30': ('identical', '1/3 literal, vertex sum via pointer locals a/b, idx then >> (idx*4)'),
 '00565cd0': ('identical', '0.5f literal, endpoints via pointer locals a/b, (float)sqrt(...) * 0.5f'),
 '005687d0': ('identical', 'one return expression of eight AL16 terms in source order'),
 '00568c80': ('identical', 'arena struct: aligned value written straight to ar->cur'),
 '0056d350': ('identical', 'typed 32-byte element table (*tab)[i].v[k]'),
 '00578b20': ('b', '-1.0f literal fixes fmul order; param_1 copy scheduling left'),
 '00575120': ('b', 'typed structs + struct copy tail; fxch before faddp and private first return left'),
 '005685f0': ('b', 'clean struct C; original pushes edi at entry (no shrink-wrap)'),
 '00579e50': ('b', 'struct S + row pointers fix regs; dot emitted z,x,y anchored at p[i][1] left'),
 '0056fad0': ('a', '-1.0f literal; cross-product operand order canonicalised'),
 '00566b40': ('a', 'literal for DAT_005cc320; x87 operand order left'),
 '00579c00': ('b', 'float literals; reg alloc left'),
 '00578cb0': ('a', 'float literals; dot operand order left'),
 '005667c0': ('a', 'float literals; operand order left'),
 '00578bd0': ('b', 'float literals; strength-reduced pointer regs left'),
 '00566200': ('b', 'float literals'),
 '0057a9a0': ('b', 'float literals'),
 '00566420': ('b', 'float literals'),
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
