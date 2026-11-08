"""Insert (or replace) a MATCH-RWR04 header line after the FLAGS line of each touched source."""
import pathlib
WT = pathlib.Path('C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR04')
OK = '100% (regs 100%), relocheck OK, C++ check OK, was {}%. {} Supersedes the header below.'
H = {
 'rwl05/0055fe40.c': OK.format('25.0', 'Returns param 1 (Ghidra typed it void).'),
 'rwl05/00565570.c': OK.format('16.7', 'Returns param 1 (Ghidra typed it void).'),
 'rwl05/0055a140.c': OK.format('12.3', 'Returns param 1 (Ghidra typed it void).'),
 'rwl04/00552720.c': OK.format('6.7', 'Returns param 1 (Ghidra typed it void).'),
 'rwl05/0055ade0.c': OK.format('90.2', 'return param_2 duplicated inside the if block, so the shared epilogue pushes edi at entry.'),
 'rwl04/00551550.c': OK.format('89.2', 'Returns int: the callback result on that path, param_1 at the end.'),
 'rwl05/0055ac50.c': OK.format('88.3', 'Word index and bit mask written inline in the test (no uVar3/iVar2 temps).'),
 'rwl05/0055ad30.c': OK.format('87.7', 'ret = param_2 assigned before the bit set (ebx stops being the zero constant).'),
 'rwl05/00559c00.c': OK.format('83.3', 'Returns piVar1 (Ghidra typed it void); the return 0 reuses eax.'),
 'rwl05/0055dca0.c': OK.format('80.6', 'Natural for loop over a {int a,b} array instead of the Ghidra do-while.'),
 'rwl05/00564100.c': OK.format('81.0', 'Fill value typed int.'),
 'rwl05/00564130.c': OK.format('48.5', 'Aligned alloc that returns p + pad; the pad byte is held in the dead param_3 slot (*(unsigned char *)&param_3).'),
 'rwl04/00550670.c': OK.format('70.9', 'Error block written inline in the then-branch (tail-merged with the end copy), NULL test at the found label.'),
 'rwl04/005507b0.c': OK.format('82.0', 'Same error-block shape as 0x00550670.'),
 'rwl05/00558030.c': OK.format('75.0', 'Offset variable read first, if/else return shape, zeroing for loop instead of memset.'),
 'rwl05/00557ec0.c': OK.format('64.9', 'Same three fixes as 0x00558030.'),
 'rwl05/005551d0.c': OK.format('84.8', '4-iteration zeroing for loop (unrolled with its own zero register).'),
 'rwl05/0055c380.c': OK.format('85.2', 'match_permute.py store order: independent field stores reordered (0x3e4ccccd stores first).'),
 'rwl05/0055ded0.c': '98.4% (regs 100%), was 70.3%. Separate loop counter k fixes the ebx/edi swap. Remaining: one store keeps [edx+eax-0xc] where the original has [eax+edx-0xc] (8 spellings, pointer temp, permuter 711 compiles). Class (b).',
 'rwl04/00550a20.c': '87.1%, was 82.4%. ops pointer and i - n hoisted before the flag test. Remaining: the original pushes edi at entry, ours shrink-wraps it past the first return (nest, while/for, return shapes tried). Class (a).',
 'rwl04/00552890.c': '91.2%, was 82.4%. match_permute.py moved the v.y store after v.x. Remaining: y/z zero-store order and the dot-product load schedule (48 order/spelling combinations tried). Class (b).',
 'rwl04/00552920.c': '79.8%, was 78.0%. Products written out (no in[3] = in[0] copies). Remaining: the original keeps w*s and h*s as stack temps (h*s reuses the scale slot). Class (b).',
 'rwl05/00563840.c': '70.3%, was 75.8% on a source that wrote puVar3[1] before puVar3 was assigned (uninitialised pointer). The store now follows the alloc, so the semantics are right and the score dropped. Remaining: param reload / lea scheduling. Class (b).',
}
for rel, text in H.items():
    p = WT / 're/match' / rel
    raw = p.read_bytes().decode('latin-1')
    crlf = '\r\n' in raw
    lines = raw.replace('\r\n', '\n').split('\n')
    assert lines[0].startswith('// FLAGS:'), rel
    if lines[1].startswith('/* MATCH-RWR04'):
        lines[1] = '/* MATCH-RWR04 2026-11-08: ' + text + ' */'
    else:
        lines.insert(1, '/* MATCH-RWR04 2026-11-08: ' + text + ' */')
    out = '\n'.join(lines)
    if crlf:
        out = out.replace('\n', '\r\n')
    p.write_bytes(out.encode('latin-1'))
    print('ok', rel)
