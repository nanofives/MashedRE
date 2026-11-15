"""Append the DDKOBJ-UNLISTED-10 rows to re/match/PROVISIONAL.tsv and re/match/ADMITTED.tsv (CRLF, tab-separated).

usage: py -3.12 verify/ddkobj_20261114/unlisted10/append_rows.py   (from the worktree root; refuses duplicates)
"""
import sys

EV = 'verify/ddkobj_20261114/RESULT_DDKOBJ-UNLISTED-10.md'
TOOL = 'equiv.py @ fea5b5483'
TAIL = 'NOT-PROVEN source (L-126), knob path only, never C4 by this status'

PROV = [
    ('0x00401690', 'FUN_00401690', 're/match/s01/00401690.c', '/O2 /GS /Ob1', '60.2',
     'branch-condition: region ours +0xa / orig 0x0040169a (slt compare on the 0x004b3f90 result read)',
     'DDKOBJ-UNLISTED-10: static of Match/PortTop_004025f0.cpp (C2 reconstruction, residual = vertex-loop unroll x8 vs x4, relocheck BAD); object build links ___ddkreg_00401690 (713 bytes differ). ' + TAIL + '; MASHED_ORIG_TOP'),
    ('0x00498810', 'FUN_00498810', 're/match/s16/00498810.c', '/O2 /GS /Ob1', '86.7',
     'call-regargs: region ours +0x21 / orig 0x00498831 call 0x4d52d0 (ECX argument term differs)',
     'DDKOBJ-UNLISTED-10: static of Match/PortTop_00498860.cpp (regs/ops 100%, relocheck OK, 4 bytes differ); object build links ___ddkreg_00498810. ' + TAIL + '; MASHED_ORIG_TOP'),
    ('0x00552890', 'RwCore_00552890', 're/match/rwl04/00552890.c', '/O2 /Ob1 /G5', '91.2',
     'call-memory: region ours +0xc / orig 0x0055289c call 0x4c3dc0: memory differs (x87 fdiv/fmul chain over 0x00912be0 / 0x00912be4)',
     'DDKOBJ-UNLISTED-10: RW core closure, RwCore/RwCore_00552890.cpp; relocheck BAD: +0x3e reads 0x00912be0 where the original reads 0x00912be4 and +0x44 the reverse (operand order of two fmul); 23 bytes differ. ' + TAIL + '; MASHED_ORIG_TOP + MASHED_RWC_CORE'),
    ('0x005530f0', 'RwCore_005530f0', 're/match/rwl04/005530f0.c', '/O2 /Ob1 /G5', '67.8',
     'call-args: region ours +0x0 / orig 0x005530f0 call 0x4c3b30 arg 0: x87 term differs',
     'DDKOBJ-UNLISTED-10: RW core closure, RwCore/RwCore_005530f0.cpp; relocheck BAD; 129 bytes differ. ' + TAIL + '; MASHED_ORIG_TOP + MASHED_RWC_CORE'),
    ('0x005533d0', 'RwCore_005533d0', 're/match/rwl04/005533d0.c', '/O2 /Ob1 /G5', '55.0',
     'x87-region: region ours +0x0 / orig 0x005533d0 x87 instruction 3 differs: fld m4[esp+0x20] | fld m4[esp+0x1c]',
     'DDKOBJ-UNLISTED-10: RW core closure, RwCore/RwCore_005533d0.cpp; relocheck BAD; 349 bytes differ; x87 order, never admitted. ' + TAIL + '; MASHED_ORIG_TOP + MASHED_RWC_CORE'),
    ('0x005538a0', 'RwCore_005538a0', 're/match/rwl04/005538a0.c', '/O2 /Ob1 /G5', '19.7',
     "branch-condition: region ours +0x0 / orig 0x005538a0: P1('sle',(arg32(12)+-0x1),0xffffffff) | P1('sle',0x0,(arg32(12)+-0x1))",
     'DDKOBJ-UNLISTED-10: RW core closure, RwCore/RwCore_005538a0.cpp; relocheck BAD; 805 bytes differ. ' + TAIL + '; MASHED_ORIG_TOP + MASHED_RWC_CORE'),
    ('0x00554940', 'RwCore_00554940', 're/match/s24/00554940.c', '/O2 /Ob1 /G5', '12.5',
     'x87-region: region ours +0x3a / orig 0x0055497f x87 instruction 3 differs: fld m4[ebx+0x8] | fld m4[ebp+0x8]',
     'DDKOBJ-UNLISTED-10: RW core closure, RwCore/RwCore_00554940.cpp; relocheck BAD; 1262 bytes differ; x87 order, never admitted. ' + TAIL + '; MASHED_ORIG_TOP + MASHED_RWC_CORE'),
    ('0x005572c0', 'RwCore_005572c0', 're/match/s24/005572c0.c', '/O2 /Ob1 /G5', '17.9',
     "call-regargs: region ours +0x40 / orig 0x00557309 call 0x553590: (('ebp', ebp_in), ('edx', [..+(arg32(8)+0x70)])) | (('ebp', arg32(4)), ('edx', [..+(arg32(4)+0x10)]))",
     'DDKOBJ-UNLISTED-10: RW core closure, RwCore/RwCore_005572c0.cpp; relocheck BAD; 1071 bytes differ. ' + TAIL + '; MASHED_ORIG_TOP + MASHED_RWC_CORE'),
]

ADM = [
    ('0x00554200', 'FUN_00554200', 're/match/s24/00554200.c', '/O2 /Ob1 /G5 (rw9044)', 'b7414f7ae', 'fea5b5483',
     'EQUIV', 'ORDER', 'A6-paired-pop,A6-pinned-pop,A7-stack-image,jmp-threading',
     'n/a (plain EQUIV; ORDER is integer store order: the original body 0x00554200 holds no x87 instruction (capstone, 144 ins), no x87 rule fired); direct callers 0x00554311, 0x0055461e (rel32 call), no absolute dword reference',
     '_FUN_00553e80 EQUIV [BYTE-IDENTICAL] (relocheck OK, hooks C4)',
     'same=32 inc=0 diff=0', 'same=0 inc=32 diff=0',
     'RwCore/RwCore_00553e80.cpp as RwCore_00554200 (RW core knob path); the object build links the DDK compile (6 bytes differ, first +0x9d, register rotation); was unlisted until DDKOBJ-UNLISTED-10. Not C4',
     EV),
]


def append(path, rows, ncols):
    text = open(path, 'rb').read().decode('utf-8')
    have = {ln.split('\t', 1)[0] for ln in text.splitlines()}
    out = []
    for r in rows:
        if len(r) != ncols:
            sys.exit(f'{path}: {r[0]} has {len(r)} columns, want {ncols}')
        if r[0] in have:
            sys.exit(f'{path}: {r[0]} already listed')
        out.append('\t'.join(r))
    if not text.endswith('\r\n'):
        text += '\r\n'
    text += '\r\n'.join(out) + '\r\n'
    open(path, 'wb').write(text.encode('utf-8'))
    print(f'{path}: +{len(out)}')


prov = [(a, b, c, d, e, 'NOT-PROVEN', f + f' ({TOOL})', 'not run', g, 'not run', 'provisional', EV)
        for a, b, c, d, e, f, g in PROV]
append('re/match/PROVISIONAL.tsv', prov, 12)
append('re/match/ADMITTED.tsv', ADM, 15)
