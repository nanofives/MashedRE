"""MATCH-RWR06 results.tsv + relocheck.tsv over the 51 R06 rows. Run from the worktree."""
import csv, glob, hashlib, pathlib, re, subprocess, sys
sys.path.insert(0, 're/tools/match')
import match
WT = pathlib.Path('.').resolve()
OUT = WT / 'verify/matchrwr06_20261108'
OUT.mkdir(parents=True, exist_ok=True)
rows = list(csv.DictReader(open('D:/MashedWT/MATCH-RWLIB-2_data/R06.tsv'), delimiter='\t'))
# Ghidra-missed rows: original extent read from the disassembly (ret followed by 90 padding)
NOSIZE = {'0052df20': 19, '00537d20': 71, '00537b20': 84, '00537e60': 105, '00537d70': 106,
          '00536780': 443, '00536e90': 330, '00536fe0': 528}
NEW = {
 '0052df20': ('identical', 'new source from the disassembly; returns param_1 (eax = param_1 at the ret)'),
 '00537d20': ('identical', 'new source; color packed as ((((a<<8)|r)<<8)|g)<<8|b, count-down for loop'),
 '00537b20': ('identical', 'new source; per-particle color from a strided byte array'),
 '00537e60': ('identical', 'new source; constant uv read through uv = param_3 + 0x24 (loads param_3 before the count test)'),
 '00537d70': ('identical', 'new source; per-particle uv through two pointers a = src, b = src + 2'),
 '004eb9e0': ('identical', 'index read into a ushort local; range stored before min'),
 '005333a0': ('identical', 'offset read into a plain local (compiler homes it in param_2 slot); typed entry array; returns param_2'),
 '00536e90': ('a', 'new source, 99.1: posStride reload scheduled 3 slots later; p + (a - b) vertex form keeps the fchs'),
 '00536780': ('b', 'new source, 21.8: original keeps single-bit (f & C) == C as mov/and/cmp dl; DDK folds it to test (15 forms tried, see RESULT)'),
 '00536fe0': ('-', 'no source: 528 B x87 billboard with fsincos, sin/cos homed in param slots; not attempted (budget)'),
 '00533d00': ('a', 'typed rewrite, 97.6 (ops 100): compiler p3-p2 delta homed in param_1 slot, original param_5 slot'),
 '0052eb40': ('a', '88.0: copy loop while (n--) with index*0x40; param_4 loaded into ebp+shl instead of ecx + lea ebp,[ecx*4]'),
 '0052ea70': ('b', 'unchanged 71.8; guarded do-while with explicit row var reaches ops 97 but the row pointer gets a new slot (original: copy homed in param_3 slot)'),
 '005300d0': ('b', 'unchanged; early-fail returns class (4 shapes tried, 53.4 best but ops 31)'),
 '00535700': ('b', 'unchanged; same and/cmp single-bit class as 00536780 + ecx/edx swap'),
}
res = open(OUT / 'results.tsv', 'w', newline='')
res.write('rva\tsize\tbest%\tregs%\tflags\tby\tsha256\tclass\treason\tbefore%\n')
rel = open(OUT / 'relocheck.tsv', 'w', newline='')
n100 = 0
for r in rows:
    rva = r['rva'][2:]; va = int(rva, 16)
    fs = glob.glob(f're/match/rwl0*/{rva}.c')
    size = r['size'] if r['size'] != '?' else str(NOSIZE.get(rva, '?'))
    cls, reason = NEW.get(rva, (r['class'], r['reason']))
    if not fs:
        res.write(f"0x{rva}\t{size}\t-1\t-1\t\t\t\tno source\t{reason}\t{r['best_pct']}\n"); continue
    src = pathlib.Path(fs[0])
    if rva in NOSIZE:
        match.function_table()[va] = NOSIZE[rva]
    try:
        x = [y for y in match.compare(src, '', quiet=True, only=va)][0]
    except SystemExit as e:
        res.write(f"0x{rva}\t{size}\t-1\t-1\t\t\t\tcompile fail\t{reason}\t{r['best_pct']}\n"); continue
    pct, regs = round(100 * x['acc'], 1), round(100 * x['regs'], 1)
    sha = hashlib.sha256(src.read_bytes()).hexdigest()[:16]
    by = 'insn'
    if pct == 100.0:
        n100 += 1
        by = 'insn+bytes' if not x['bytes_bad'] else 'insn'
        rc = subprocess.run([sys.executable, 're/tools/match/relocheck.py', str(src)], capture_output=True, text=True).stdout
        line = [l for l in rc.splitlines() if l.startswith('0x' + rva)]
        rel.write((line[0] if line else f'0x{rva}\tMISSING') + f"\tbytes_bad={len(x['bytes_bad'])}\n")
        cls = 'identical'
    res.write(f"0x{rva}\t{size}\t{pct}\t{regs}\t{x['flags']}\t{by}\t{sha}\t{cls}\t{reason}\t{r['best_pct']}\n")
    print(rva, r['best_pct'], '->', pct)
print('n100', n100)
