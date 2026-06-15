#!/usr/bin/env py -3.12
"""frontier_classify.py — auto-classify promote_frontier.tsv leaves for the
capstone-frontier promotion lane (round 218+, 2026-06-15).

For each frontier RVA, disassemble the function body directly from
original/MASHED.exe.unpatched (no Ghidra) and tag it CLEAN or SKIP<reason>:

  SKIP reasons (disqualify from early_window suspended-spawn bit-identity):
    CALL          - makes a call (callee gate / side-effect risk)
    TAILJMP       - jmp to an external target (tail call)
    TRANSC        - fsin/fcos/fptan/fsincos/fpatan/fyl2x/f2xm1 (transcendental)
    FLDSTN        - fld st(N) / fxch (x87 register juggling; compiler won't
                    reliably reproduce bit-identically)
    REGCONV       - reads esi/edi/ebp/ecx/edx as an INPUT before writing it
                    (non-__cdecl register convention)
    BIGFRAME      - sub esp,>0x40 (large locals; likely complex)

  CLEAN = none of the above -> a __cdecl leaf reading args from [esp+N] and
  globals/struct-fields only; promotable via a focused early_window handler.

Output: re/analysis/plans/frontier_classified.tsv (rva,size,subsys,verdict,first_op)
Prints the CLEAN candidates (smallest first) to stdout.
"""
import struct, sys, os
import capstone

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, 'original', 'MASHED.exe.unpatched')
FRONTIER = os.path.join(ROOT, 're', 'analysis', 'plans', 'promote_frontier.tsv')
OUT = os.path.join(ROOT, 're', 'analysis', 'plans', 'frontier_classified.tsv')
IB = 0x400000

f = open(EXE, 'rb').read()
pe = struct.unpack_from('<I', f, 0x3c)[0]
nsec = struct.unpack_from('<H', f, pe + 6)[0]
opt = struct.unpack_from('<H', f, pe + 20)[0]
sect = pe + 24 + opt


def rva2off(rva):
    for i in range(nsec):
        b = sect + i * 40
        va = struct.unpack_from('<I', f, b + 12)[0]
        vs = struct.unpack_from('<I', f, b + 8)[0]
        raw = struct.unpack_from('<I', f, b + 20)[0]
        if va <= rva < va + vs:
            return raw + (rva - va)
    return None


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
TRANSC = {'fsin', 'fcos', 'fptan', 'fsincos', 'fpatan', 'fyl2x', 'f2xm1', 'fyl2xp1'}
# registers that, if read before written, imply a non-cdecl input convention
REGIN = ('esi', 'edi', 'ebx', 'ecx', 'edx', 'ebp')


def classify(va, size):
    off = rva2off(va - IB)
    if off is None:
        return 'SKIP:NOMAP', ''
    n = max(size, 16)
    body = f[off:off + min(n + 16, 0x400)]
    written = set()
    first = ''
    saw_call = saw_tailjmp = saw_transc = saw_fldstn = saw_bigframe = saw_regconv = False
    for ins in md.disasm(body, va):
        m, ops = ins.mnemonic, ins.op_str
        if not first:
            first = f'{m} {ops}'
        if m in ('ret', 'retn'):
            break
        if ins.address - va > size + 8:
            break
        if m == 'call':
            saw_call = True
        elif m == 'jmp':
            # external tail jmp (target not a local label inside the body)
            if ops.startswith('0x'):
                tgt = int(ops, 16)
                if not (va <= tgt < va + size + 8):
                    saw_tailjmp = True
            else:
                saw_tailjmp = True  # indirect jmp
        elif m in TRANSC:
            saw_transc = True
        elif (m == 'fld' and ops.startswith('st')) or m == 'fxch' or (m.startswith('f') and 'st(' in ops and m not in ('faddp','fsubp','fmulp','fdivp','fsubrp','fdivrp')):
            saw_fldstn = True
        elif m == 'sub' and ops.startswith('esp,'):
            try:
                amt = int(ops.split(',')[1].strip(), 16)
                if amt > 0x40:
                    saw_bigframe = True
            except Exception:
                pass
        # register-convention input detection: a read of a callee-saved/arg reg
        # before it was written in this function => input via register
        # (skip push, which just saves it)
        if m not in ('push',):
            for r in REGIN:
                # crude: operand mentions the reg as a source and it wasn't written yet
                if r in ops and r not in written:
                    # is it being written (destination) by this insn?
                    dst = ops.split(',')[0].strip()
                    if dst == r and m in ('mov', 'lea', 'xor', 'pop', 'movzx', 'movsx'):
                        pass  # this insn writes it
                    else:
                        saw_regconv = True
        # mark written destination
        dst = ops.split(',')[0].strip() if ',' in ops else ops.strip()
        if dst in REGIN and m in ('mov', 'lea', 'xor', 'pop', 'movzx', 'movsx', 'add', 'sub', 'and', 'or'):
            written.add(dst)
    reasons = []
    if saw_call: reasons.append('CALL')
    if saw_tailjmp: reasons.append('TAILJMP')
    if saw_transc: reasons.append('TRANSC')
    if saw_fldstn: reasons.append('FLDSTN')
    if saw_regconv: reasons.append('REGCONV')
    if saw_bigframe: reasons.append('BIGFRAME')
    return ('CLEAN' if not reasons else 'SKIP:' + '+'.join(reasons)), first


rows = []
with open(FRONTIER) as fh:
    hdr = fh.readline()
    for line in fh:
        p = line.rstrip('\n').split('\t')
        if len(p) < 4:
            continue
        rva = int(p[0], 16)
        size = int(p[3])
        subsys = p[2]
        verdict, first = classify(rva, size)
        rows.append((p[0], size, subsys, verdict, first))

with open(OUT, 'w') as fh:
    fh.write('rva\tsize\tsubsys\tverdict\tfirst_op\n')
    for r in rows:
        fh.write('\t'.join(str(x) for x in r) + '\n')

clean = sorted([r for r in rows if r[3] == 'CLEAN'], key=lambda r: r[1])
print(f"total {len(rows)} | CLEAN {len(clean)} | SKIP {len(rows)-len(clean)}")
print("\n=== CLEAN candidates (smallest first) ===")
for r in clean:
    print(f"  {r[0]} {r[1]:4}B {r[2]:9} {r[4]}")
