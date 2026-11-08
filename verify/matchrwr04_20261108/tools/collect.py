"""MATCH-RWR04 results.tsv + relocheck.tsv over the 79 R04 rows. Run from the worktree.
Adapted from the R01 collect.py (D:/MashedWT/MATCH-RWLIB-2_data/r01_tools/collect.py)."""
import csv, hashlib, re, subprocess, sys, pathlib
WT = pathlib.Path('C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR04')
OUT = WT / 'verify/matchrwr04_20261108'
OUT.mkdir(parents=True, exist_ok=True)
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R04.tsv'), delimiter='\t'))
NEW = {  # rows reworked in this slice: class, reason
 '0055fe40': ('identical', 'returns param 1 (Ghidra typed void)'),
 '00565570': ('identical', 'returns param 1 (Ghidra typed void)'),
 '0055a140': ('identical', 'returns param 1 (Ghidra typed void)'),
 '00552720': ('identical', 'returns param 1 (Ghidra typed void)'),
 '0055ade0': ('identical', 'return param_2 duplicated inside the if block (no shrink-wrap)'),
 '00551550': ('identical', 'returns int: callback result on that path, param_1 at the end'),
 '0055ac50': ('identical', 'index/bit expressions inline in the test (no uVar3/iVar2 temps)'),
 '0055ad30': ('identical', 'ret = param_2 assigned before the bit set'),
 '00559c00': ('identical', 'returns piVar1 (Ghidra typed void); return 0 reuses eax'),
 '0055dca0': ('identical', 'natural for loop over a {int a,b} array'),
 '00564100': ('identical', 'fill value typed int'),
 '00564130': ('identical', 'aligned alloc returning p + pad; pad byte held in param_3 slot'),
 '00550670': ('identical', 'error block inline in the then-branch, NULL test at the found label'),
 '005507b0': ('identical', 'same error-block shape as 0x00550670'),
 '00558030': ('identical', 'offset var first, else-return, zeroing for loop (not memset)'),
 '00557ec0': ('identical', 'same three fixes as 0x00558030'),
 '005551d0': ('identical', '4-iteration zeroing for loop (own zero register)'),
 '0055c380': ('identical', 'permuter store order (independent field stores)'),
 '0055ded0': ('b', 'separate loop counter fixed regs; [eax+edx] vs [edx+eax] on one store remains'),
 '00550a20': ('a', 'ops/off hoisted (87.1); edi shrink-wrap remains'),
 '00552890': ('b', 'permuter v.y store move (91.2); zero-store and dot-product schedule remain'),
 '00552920': ('b', 'duplicated products (79.8); x87 temp slots remain'),
 '00563840': ('b', 'source used puVar3 before the alloc (fixed, semantics now right); scheduling remains'),
 '005584c0': ('a', 'two return-0 paths: original keeps an inline ret with eax reuse, ours tail-merges to xor'),
 '00551840': ('a', 'same return-0 tail-merge class as 0x005584c0'),
 '00551c40': ('a', 'same return-0 tail-merge class as 0x005584c0, plus edi shrink-wrap'),
}
res = open(OUT / 'results.tsv', 'w', newline='')
res.write('rva\tsize\tbest%\tregs%\tflags\tby\tsha256\tclass\treason\tbefore%\n')
rel = open(OUT / 'relocheck.tsv', 'w', newline='')
n100 = 0
for r in rows:
    rva = r['rva']
    src = WT / r['source'].replace(chr(92), '/')
    if not src.exists():
        res.write(f"0x{rva}\t{r['size']}\t-1\t-1\t\t\t\tno source\t{r['reason']}\t{r['best_pct']}\n"); continue
    out = subprocess.run([sys.executable, str(WT / 're/tools/match/match.py'), str(src), '--only', '0x' + rva],
                         capture_output=True, text=True, cwd=WT).stdout
    m = re.search(r'0x%s \S+: ([\d.]+)%% \(regs ([\d.]+)%%.*flags=(.*?) tc=' % rva, out)
    if not m:
        res.write(f"0x{rva}\t{r['size']}\t-1\t-1\t\t\t\tcompile fail\t{r['reason']}\t{r['best_pct']}\n"); continue
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
    print(f'[{rows.index(r) + 1}/{len(rows)}] {rva} {r["best_pct"]} -> {pct}', flush=True)
print('n100', n100)
