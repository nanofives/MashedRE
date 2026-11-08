import csv, re, hashlib, pathlib
V = 'verify/matchrwl06_20261108/'
rows = list(csv.DictReader(open(V + 'results1.tsv'), delimiter='\t'))
cxx = {}
for l in open('D:/MashedWT/MATCH-RWL06_data/cxx_all2.txt', encoding='utf-8', errors='replace'):
    m = re.match(r'(CXXOK|CXXFAIL) (\w+)', l)
    if m: cxx[m.group(2)] = m.group(1)
C = {
'0055ae50': ('b', 'tail call into 0x00565550, load order of the two loads'),
'00559c00': ('a', 'original has an inline ret after jne, ours shares the final ret'),
'00564100': ('b', 'memset intrinsic, movsx vs mov al for the byte arg'),
'0055c380': ('b', '0x3e4ccccd constant register vs the [+0xc] RMW'),
'00559560': ('a', '(x+0x1f)>>5<<5 folds to and ~0x1f for every spelling'),
'00563df0': ('a', 'lea [p2+p3*2+2]; shl 3 folded to *8+0x10'),
'005584c0': ('b', 'first return-0 block layout'),
'0055ad30': ('b', 'ebx used as the zero constant for the DAT stores'),
'0055dca0': ('b', 'ebp used as a shared zero constant'),
'0055c2d0': ('a', 'x87 operand order in rows 0,2,3'),
'0055ac50': ('b', 'xor ecx zero-extend hoisted above the test in the original'),
'00558030': ('b', 'return-0 block placement'),
'00557ec0': ('b', 'same lazy-init block residue as 0x00558030'),
'0055b940': ('b', 'nosize row, not worked'),
'00563b00': ('b', 'nosize row, not worked'),
'0055b650': ('b', 'not worked, raw form'),
'005645b0': ('b', 'not worked, raw form'),
'0055f670': ('b', 'not worked, raw form'),
'00558240': ('b', 'not worked, raw form'),
'00558df0': ('b', 'not worked, raw form'),
'00564310': ('b', 'not worked, raw form'),
'0055b080': ('b', 'not worked, raw form'),
'0055f800': ('b', 'not worked, raw form (permuter timed out)'),
'0055fe40': ('a', 'original loads ptr arg into eax before the value arg; every spelling tried'),
'00565570': ('a', 'arg-load register assignment eax/ecx/dx vs edx/eax/cx'),
'00564130': ('b', 'orig keeps the byte in the param_3 stack slot, uVar1 in esi across the div'),
'0055ade0': ('a', 'push edi hoisted to prologue in the original (shrink-wrap)'),
'00558cb0': ('b', 'orig has an 8-byte local frame not reproduced'),
'0055c230': ('a', 'first dot product scheduling (fld before mov eax)'),
'0055c3e0': ('c', 'orig uses the C++ bool conversion neg/sbb/neg then je, no C spelling gives it'),
'0055b750': ('b', 'flags /O1 /Ob1 /G5; orig frameless with st(n) reuse, ours builds an ebp frame'),
'0055ded0': ('b', 'ebx<->edi swap and one store operand order remain'),
'00562380': ('b', 'cross-product term order, fld DAT vs fld x order'),
'00563840': ('b', 'and/lea chain scheduling'),
'00561280': ('b', 'orig has a 0xc local frame spilling the third dot product'),
'005586f0': ('b', 'permuter-improved raw form'),
'0055a9a0': ('b', 'raw form did not compile; return type fixed only, not worked'),
'0055b4a0': ('b', 'sqrt intrinsic form, residue not worked'),
'0055bde0': ('b', 'untouched raw form (/O1)'),
'00559c40': ('b', 'untouched raw form'),
'0055f520': ('b', 'raw converter body, not worked'),
'00562010': ('b', 'raw converter body (game flags won the raw run), not worked'),
'00558860': ('b', 'raw form with a placeholder for an unresolved -NAN operand, not worked'),
'005595d0': ('b', 'untouched raw form'),
'0055bae0': ('a', 'point transform: original spills one sum to [esp+8] and pops two dead st(0)'),
'005551d0': ('b', 'original zeroes +0x14..+0x20 through a second xor edx,edx'),
'0055a140': ('b', 'same instruction stream, registers differ only (param_1 eax, param_2 esi)'),
'00564040': ('b', 'original reloads the ushort param with and 0xffff, count in a stack slot'),
'00562ed0': ('b', 'register allocation ebx/ebp/edi with per-field copies through scratch regs'),
'0055c000': ('a', 'x87 class as 0x0055bae0'),
'00559ee0': ('b', 'original keeps the 1<<n mask in ebp and rebuilds it, ours spills it'),
'00562660': ('b', 'different branch layout, sub esp 0x20 at entry'),
'00563c80': ('b', 'raw converter body, only flags tried'),
'0055bbf0': ('a', 'x87 class as 0x0055bae0, raw body'),
'00563940': ('b', 'ushort edge-index loads and frame layout differ'),
'005592f0': ('b', 'raw converter body, not worked (167 vs 129 instructions)'),
'00559040': ('a', 'x87 scheduling differs throughout'),
'00554390': ('b', 'raw converter body, not worked'),
}
out = open(V + 'results.tsv', 'w', newline='')
w = csv.writer(out, delimiter='\t')
w.writerow(['rva', 'size', 'best%', 'regs%', 'flags', 'by', 'sha256', 'class', 'reason', 'cxx'])
miss = []
for r in rows:
    va8 = r['rva'][2:]
    src = pathlib.Path('re/match/rwl06/%s.c' % va8)
    fl = src.read_text(encoding='latin-1').split('\n')[0].replace('// FLAGS:', '').strip()
    sha = hashlib.sha256(src.read_bytes()).hexdigest()[:16]
    best = r['best%']
    if best == '100.0':
        cl, rs = '-', 'identical'
    else:
        cl, rs = C.get(va8, ('b', 'see header'))
        if va8 not in C: miss.append(va8)
    w.writerow([r['rva'], r['size'], best, r['regs%'], fl, r['by'], sha, cl, rs, cxx.get(va8, '?')])
print('missing', miss)
