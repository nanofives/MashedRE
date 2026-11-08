"""Insert a MATCH-RWR01 header line after the FLAGS line of each touched source."""
import pathlib
WT = pathlib.Path('C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR01')
H = {
 'rwl01/004e15f0.c': '100% (regs 100%), relocheck OK, was 96.2%. The tail block now goes through the typed `end` pointer like the first block (end = *param_2 + param_2[1] - 0x20; end->f4 = blk; blk->f0 = end), which gives the [ecx+eax] base/index order. Supersedes the header below.',
 'rwl01/004ccc50.c': '100% (regs 100%), relocheck OK, was 98.2%. The bitmap sum reads through a byte pointer m = (byte *)(piVar4 + 2) set before the loop (m[uVar5]); that gives the [ecx+eax+8] index/base order. Supersedes the header below.',
 'rwl01/004ccba0.c': '100% (regs 100%), relocheck OK, was 95.9%. One byte pointer pm = (unsigned char *)(puVar1 + 2) for both bitmap accesses, and a fresh loop index ui in a for loop for the sum; together they give both address operand orders. Supersedes the header below.',
 'rwl01/004c5a60.c': '100% (regs 100%), relocheck OK, was 83.7%. Refcount written as param_1[0x15]--; if (param_1[0x15] <= 0) { param_1[0x15]++; ... } (the edx copy before the test). Supersedes the header below.',
 'rwl01/004c3e20.c': '92.6% (regs 100%), was 85.2%. The two loads use ((G *)((char *)DAT_007d3ff8 + DAT_007d3ffc))->field (orders now right); the two stores after the free call keep [eax+edx] where the original has [edx+eax] under every spelling tried (16 load/store spelling pairs, char* typed global, permuter 375 compiles). Class (a)/(b).',
 'rwl02/004e4380.c': '100% (regs 100%), relocheck OK, was 72.2%. The function returns param_1 (eax holds it at the ret); Ghidra typed it void, which made the compiler duplicate the tail. Supersedes the header below.',
 'rwl02/004e5820.c': '100% (regs 100%), relocheck OK, was 57.1% under /O1. Under the slice flags /O2 /Ob1 /G5: if (cb == 0) cb = FUN_004e5190; p->cb = cb; return p; (the function returns p). Supersedes the header below.',
 'rwl02/004e4130.c': '100% (regs 100%), relocheck OK, was 67.6%. match_permute.py swap: cur = nx moves before the free callback (o is already computed, same semantics). Supersedes the header below.',
 'rwl02/004e4f70.c': '100% (regs 100%), relocheck OK, was 81.8%. Source is a double recursion (if (*p != -1) { if (p[2]) F(p[2]); if (p[3]) F(p[3]); return; }); the compiler turns the second call into the loop Ghidra showed. Supersedes the header below.',
 'rwl02/004e55d0.c': '100% (regs 100%), relocheck OK, was 88.2%. Stack walk: pop is cur = stack[sp--]; push keeps the old node (piVar4 = cur; cur = cur[2]; stack[++sp] = piVar4[3]). Global order DAT_007d3ff8 + DAT_007d71cc + 4 (the other spelling scores 100% but relocheck BAD, globals swapped). Supersedes the header below.',
 'rwl02/004e5700.c': '100% (regs 100%), relocheck OK, was 89.8%. Same stack-walk shape as 004e55d0, globals as DAT_007d3ff8 + DAT_007d71cc, and the flag byte test before the +0x1c load (the +0x1c field is read inside each branch). Supersedes the header below.',
 'rwl02/004e5c30.c': '100% (regs 100%), relocheck OK, was 76.7%. Count hoisted into a local (n = world->n, spilled to the param slot) and the atomics array pointer loaded inside the loop. Supersedes the header below.',
 'rwl02/004e5bf0.c': '78.6% (regs 78.6%), was 70.6%. head then cur, compare head != cur, early return param_1 inside the loop; remaining: the original loads param_1 into eax before the pushes and uses lea for head (ours keeps it in edi). Class (b).',
 'rwl02/004e5d30.c': '94.1% (regs 94.1%), was 91.2%. Nested if chain returning r; remaining: the original keeps one ret per failing call where ours jumps to a shared ret (12 return shapes tried, incl. && chains and separate result variables). Class (b).',
}
for rel, text in H.items():
    p = WT / 're/match' / rel
    raw = p.read_bytes().decode('latin-1')
    crlf = '\r\n' in raw
    lines = raw.replace('\r\n', '\n').split('\n')
    assert lines[0].startswith('// FLAGS:'), rel
    if lines[1].startswith('/* MATCH-RWR01'):
        lines[1] = '/* MATCH-RWR01 2026-11-08: ' + text + ' */'
    else:
        lines.insert(1, '/* MATCH-RWR01 2026-11-08: ' + text + ' */')
    if rel == 'rwl01/004e15f0.c':
        lines = [l for l in lines if l not in ('  int iVar4;', '  int iVar6;')]
    out = '\n'.join(lines)
    if crlf:
        out = out.replace('\n', '\r\n')
    p.write_bytes(out.encode('latin-1'))
    print('ok', rel)
