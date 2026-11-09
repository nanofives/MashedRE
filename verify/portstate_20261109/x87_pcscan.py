"""x87_pcscan.py [--image <pe>] [--base 0x400000] [--inv x87_inventory.tsv] [--out x87_pcscan.tsv] [--rva 0x..]

PORT-X87-PC24-AUDIT, static leg. For every RVA in the inventory, walks the function's x87 instruction
stream (recursive descent over the CFG of the given PE image; MASHED.exe.unpatched by default) and
tags every x87 stack value:

  E   exactly a float: fld m32, fild m16, fld1/fldz, fld m64 of a read-only constant whose value is a float
  R1  one arithmetic result (+ - * / sqrt) of two E operands, not yet stored
  NF  any other arithmetic result whose leaves are all E (a float chain held in the register)
  NW  anything with a wide leaf: fld m64 / m80 of a non-float value, fild m32/m64, fldpi & co, a
      transcendental result, or an R1/NF/NW passed through one

Events (each one is an instruction address in the image):
  chain     arithmetic consumes R1/NF (a value held in the register across a second op)
  wideop    arithmetic consumes a wide leaf (NW or the m64 / int operand itself)
  cmp       a compare consumes a non-E value
  intconv   fist/fistp, or a call that consumes ST0 (_ftol), on a non-E value
  widestore fst/fstp m64/m80 of a non-E value
  retst0    ret with a non-E value in ST0
  trans     fsin/fcos/fsincos/fptan/fpatan/fyl2x/fyl2xp1/f2xm1 (and on what input)
  fldcw     the body loads its own control word (note only)

Rules the verdict uses (PORT-X87-PC24-AUDIT RESULT section 2):
  * PC only affects FADD/FSUB/FMUL/FDIV/FSQRT results. One such op on two float operands rounds to the same
    float at PC_24 and PC_53 once stored to m32 (53 >= 2*24+2, double rounding is innocuous). So a body
    with NO chain / wideop / cmp / intconv / widestore / retst0 event is PC-insensitive.
  * SSE2 (/arch:SSE2, float math rounded to float per op) equals x87 at PC_24 for float-only data flow.
    It is NOT proven equal when a wideop or trans event exists (double or int leaves, CRT transcendental).
"""
import argparse, struct, sys
from pathlib import Path
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_MEM, CS_OP_REG, CS_OP_IMM

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
ap = argparse.ArgumentParser()
ap.add_argument('--image', default=str(ROOT.parents[1] / 'original' / 'MASHED.exe.unpatched')
                if (ROOT.parents[1] / 'original').exists() else str(ROOT / 'original' / 'MASHED.exe.unpatched'))
ap.add_argument('--inv', default=str(HERE / 'x87_inventory.tsv'))
ap.add_argument('--out', default=str(HERE / 'x87_pcscan.tsv'))
ap.add_argument('--rva', default='')
ap.add_argument('-v', action='store_true')
ap.add_argument('--map', default='', help='scan the port bodies of --image (a mashed_re.exe) found by name in this .map')
ap.add_argument('--names', default=str(HERE / 'x87_names.tsv'), help='tu, rva, port symbol (x87_names.tsv)')
a = ap.parse_args()

pe = pefile.PE(a.image)
BASE = pe.OPTIONAL_HEADER.ImageBase
IMG = pe.get_memory_mapped_image()
SECS = [(BASE + s.VirtualAddress, BASE + s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData),
         s.Name.rstrip(b'\0').decode(), s.Characteristics) for s in pe.sections]
TEXT = [s for s in SECS if s[3] & 0x20000000]


def in_text(va):
    return any(lo <= va < hi for lo, hi, _, _ in TEXT)


ORIGPE = Path(ROOT.parents[1] / 'original' / 'MASHED.exe.unpatched')
_orig = pefile.PE(str(ORIGPE)) if ORIGPE.exists() and Path(a.image).resolve() != ORIGPE.resolve() else None
_oimg = _orig.get_memory_mapped_image() if _orig else None


def ro_const(va, n):
    if _orig is not None and 0x00400000 <= va < 0x00400000 + len(_oimg):
        for s in _orig.sections:   # a matched body's literal at its original address (MGLOBAL)
            lo = 0x00400000 + s.VirtualAddress
            if lo <= va < lo + s.Misc_VirtualSize and not (s.Characteristics & 0x80000000):
                return _oimg[va - 0x00400000: va - 0x00400000 + n]
        return None
    for lo, hi, name, ch in SECS:
        if lo <= va and va + n <= hi and not (ch & 0x80000000):   # not writable
            return IMG[va - BASE: va - BASE + n]
    return None


md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = True
RANK = {'E': 0, 'I': 1, 'D': 2, 'R1': 3, 'NF': 4, 'NW': 5}
LEAF = ('E', 'I', 'D')
INV = {v: k for k, v in RANK.items()}


def worst(*t):
    return INV[max(RANK[x] for x in t)]


def insn_at(va):
    b = IMG[va - BASE: va - BASE + 16]
    for i in md.disasm(bytes(b), va, 1):
        return i
    return None


ARITH = {'fadd', 'faddp', 'fsub', 'fsubp', 'fsubr', 'fsubrp', 'fmul', 'fmulp', 'fdiv', 'fdivp', 'fdivr', 'fdivrp',
         'fiadd', 'fisub', 'fisubr', 'fimul', 'fidiv', 'fidivr'}
CMP = {'fcom', 'fcomp', 'fcompp', 'fucom', 'fucomp', 'fucompp', 'fcomi', 'fcomip', 'fucomi', 'fucomip', 'ftst',
       'ficom', 'ficomp'}
TRANS = {'fsin', 'fcos', 'fsincos', 'fptan', 'fpatan', 'fyl2x', 'fyl2xp1', 'f2xm1'}
CONSTS = {'fldpi', 'fldl2e', 'fldl2t', 'fldlg2', 'fldln2'}


def streg(op, i):
    n = i.reg_name(op.reg)
    if n.startswith('st'):
        return int(n[3]) if '(' in n else 0
    return None


def mem_tag(i, op, intform):
    sz = op.size
    if intform:
        return 'E' if sz == 2 else 'I'
    if sz == 4:
        return 'E'
    if sz == 8:
        m = op.mem
        if m.base == 0 and m.index == 0:
            b = ro_const(m.disp & 0xffffffff, 8)
            if b is not None:
                d = struct.unpack('<d', b)[0]
                try:
                    if struct.unpack('<f', struct.pack('<f', d))[0] == d:
                        return 'E'
                except OverflowError:
                    pass
        return 'D'
    return 'D'


SUMMARY = {}


def summarize(entry, depth=0):
    """Callee summary for a call site: (consume, net, ret_tag, trans)."""
    if entry in SUMMARY:
        return SUMMARY[entry]
    if depth > 5 or not in_text(entry):
        return (0, 0, 'NW', False)
    SUMMARY[entry] = (0, 0, 'NW', False)   # recursion guard
    r = analyze(entry, depth + 1, callee=True)
    SUMMARY[entry] = (r['consume'], r['net'], r['ret_tag'], r['trans_on_args'])
    return SUMMARY[entry]


def analyze(entry, depth=0, callee=False):
    # state: list of tags, ST0 first; for callee mode, args below entry are 'A<n>' placeholders (tag E)
    ev = []
    notes = set()
    seen = {}
    work = [(entry, ())]
    min_depth = 0
    nets = []
    ret_tags = []
    trans_on_args = False
    n_insn = 0
    fp_ops = 0
    while work:
        va, st = work.pop()
        st = list(st)
        while True:
            key = va
            if key in seen:
                old = seen[key]
                if len(old) == len(st) and all(RANK[x] >= RANK[y] for x, y in zip(old, st)):
                    break
                if len(old) != len(st):
                    notes.add('depthmerge@%08x' % va)
                    break
                st = [worst(x, y) for x, y in zip(old, st)]
            seen[key] = tuple(st)
            i = insn_at(va)
            if i is None:
                notes.add('baddis@%08x' % va); break
            n_insn += 1
            if n_insn > 20000:
                notes.add('toolong'); work = []; break
            mn = i.mnemonic
            ops = i.operands
            nxt = va + i.size

            def pop(k=1):
                nonlocal min_depth
                for _ in range(k):
                    if st:
                        st.pop(0)
                    else:
                        pass
                    # depth relative to entry
                d = len(st) - 0
                return d

            def get(k):
                nonlocal min_depth, trans_on_args
                if k < len(st):
                    return st[k]
                # below entry depth: an argument passed in the x87 stack (callee consume)
                need = k + 1 - len(st)
                for _ in range(need):
                    st.append('E')
                rel = -(need)
                return 'E'

            def ev_add(kind, tag=''):
                ev.append((kind, va, tag))

            if mn.startswith('f') and mn not in ('fwait',):
                fp_ops += 1
            if mn in ('fld', 'fild'):
                if ops and ops[0].type == CS_OP_MEM:
                    st.insert(0, mem_tag(i, ops[0], mn == 'fild'))
                else:
                    k = streg(ops[0], i)
                    st.insert(0, get(k))
            elif mn in ('fld1', 'fldz'):
                st.insert(0, 'E')
            elif mn in CONSTS:
                st.insert(0, 'NW')
            elif mn in ARITH:
                intform = mn.startswith('fi')
                popf = mn.endswith('p') and not mn in ('fsubp',) or mn in ('fsubp',)
                popf = mn.endswith('p')
                if len(ops) == 1 and ops[0].type == CS_OP_MEM:
                    a0 = get(0); b0 = mem_tag(i, ops[0], intform)
                    dst = 0
                elif len(ops) == 2:
                    d = streg(ops[0], i); s = streg(ops[1], i)
                    a0 = get(d); b0 = get(s); dst = d
                else:   # faddp etc. with no operands = st(1) op st(0), pop
                    a0 = get(1); b0 = get(0); dst = 1; popf = True
                ins = (a0, b0)
                if any(x not in LEAF for x in ins):
                    ev_add('chain', mn + ' ' + '/'.join(ins))
                    res = 'NW' if any(x in ('NW', 'D', 'I') for x in ins) else 'NF'
                else:
                    if 'D' in ins:
                        ev_add('dblop', mn + ' ' + '/'.join(ins))
                    if 'I' in ins:
                        ev_add('intop', mn + ' ' + '/'.join(ins))
                    res = 'R1'
                st[dst] = res
                if popf:
                    st.pop(0)
            elif mn == 'fsqrt':
                x = get(0)
                if x not in LEAF:
                    ev_add('chain', 'fsqrt ' + x); st[0] = 'NW' if x == 'NW' else 'NF'
                else:
                    if x == 'D': ev_add('dblop', 'fsqrt D')
                    if x == 'I': ev_add('intop', 'fsqrt I')
                    st[0] = 'R1'
            elif mn in ('fchs', 'fabs'):
                get(0)
            elif mn == 'frndint':
                x = get(0)
                if x not in LEAF:
                    ev_add('intconv', 'frndint ' + x)
            elif mn in TRANS:
                x = get(0)
                ev_add('trans', mn + ':' + x)
                if callee:
                    trans_on_args = True
                if x not in LEAF:
                    ev_add('chain', 'into %s %s' % (mn, x))
                if mn in ('fpatan', 'fyl2x', 'fyl2xp1'):
                    y = get(1)
                    if y not in LEAF:
                        ev_add('chain', 'into %s %s' % (mn, y))
                    st.pop(0); st[0] = 'NW'
                elif mn in ('fsincos', 'fptan'):
                    st[0] = 'NW'; st.insert(0, 'NW')
                else:
                    st[0] = 'NW'
            elif mn in ('fscale',):
                x = get(0); get(1)
                st[0] = worst(x, 'E')
            elif mn in ('fprem', 'fprem1'):
                get(0); get(1)
            elif mn == 'fxtract':
                x = get(0); st.insert(0, x)
            elif mn == 'fxch':
                k = streg(ops[0], i) if ops else 1
                get(k)
                st[0], st[k] = st[k], st[0]
            elif mn in ('fst', 'fstp'):
                if ops[0].type == CS_OP_MEM:
                    x = get(0)
                    if ops[0].size != 4 and x not in LEAF:
                        ev_add('widestore', '%s m%d' % (x, ops[0].size * 8))
                    if mn == 'fstp':
                        st.pop(0)
                else:
                    k = streg(ops[0], i)
                    x = get(0); get(k)
                    st[k] = x
                    if mn == 'fstp':
                        st.pop(0)
            elif mn in ('fist', 'fistp', 'fisttp'):
                x = get(0)
                if x not in LEAF:
                    ev_add('intconv', mn + ' ' + x)
                if mn != 'fist':
                    st.pop(0)
            elif mn in CMP:
                if mn == 'ftst':
                    xs = [get(0)]
                elif ops and ops[0].type == CS_OP_MEM:
                    xs = [get(0), mem_tag(i, ops[0], mn.startswith('fi'))]
                elif len(ops) == 2:
                    xs = [get(streg(ops[0], i)), get(streg(ops[1], i))]
                elif len(ops) == 1:
                    xs = [get(0), get(streg(ops[0], i))]
                else:
                    xs = [get(0), get(1)]
                bad = [x for x in xs if x not in LEAF]
                if bad:
                    ev_add('cmp', '/'.join(xs))
                npop = 2 if mn.endswith('pp') else (1 if mn.endswith('p') else 0)
                for _ in range(npop):
                    st.pop(0)
            elif mn in ('ffree', 'ffreep'):
                if mn == 'ffreep':
                    st.pop(0)
            elif mn in ('fldcw',):
                notes.add('fldcw@%08x' % va); ev_add('fldcw')
            elif mn in ('fninit', 'finit'):
                st = []; notes.add('finit')
            elif mn in ('fnsave', 'fsave', 'frstor', 'fldenv', 'fnstenv', 'fstenv', 'fdecstp', 'fincstp'):
                notes.add('%s@%08x' % (mn, va))
            elif mn.startswith('fcmov'):
                get(0); get(streg(ops[1], i) if len(ops) == 2 else 1)
            # control flow
            if mn == 'call':
                if ops[0].type == CS_OP_IMM:
                    tgt = ops[0].imm & 0xffffffff
                    c, net, rt, tr = summarize(tgt, depth)
                    consumed = [get(k) for k in range(c)]
                    if any(x not in LEAF for x in consumed):
                        ev_add('intconv', 'arg to %08x: %s' % (tgt, '/'.join(consumed)))
                    if tr:
                        ev_add('trans', 'callee %08x' % tgt)
                    for _ in range(c):
                        st.pop(0)
                    for _ in range(c + net):
                        st.insert(0, rt)
                    if c + net < 0:
                        notes.add('negcall@%08x' % va)
                else:
                    notes.add('indcall@%08x' % va)
                va = nxt
                continue
            if mn in ('ret', 'retn'):
                if st and st[0] not in LEAF and not callee:
                    ev_add('retst0', st[0])
                ret_tags.append(st[0] if st else None)
                nets.append(st[:])
                break
            if mn == 'jmp':
                if ops[0].type == CS_OP_IMM:
                    tgt = ops[0].imm & 0xffffffff
                    if tgt < entry or tgt > entry + 0x4000:
                        # tail call
                        c, net, rt, tr = summarize(tgt, depth)
                        for _ in range(c):
                            st.pop(0)
                        for _ in range(c + net):
                            st.insert(0, rt)
                        nets.append(st[:])
                        ret_tags.append(st[0] if st else None)
                        notes.add('tailjmp@%08x' % va)
                        break
                    va = tgt
                    continue
                notes.add('indjmp@%08x' % va)
                break
            if mn.startswith('j') or mn in ('loop', 'loope', 'loopne', 'jecxz'):
                if ops and ops[0].type == CS_OP_IMM:
                    work.append((ops[0].imm & 0xffffffff, tuple(st)))
                va = nxt
                continue
            if mn in ('int3', 'hlt', 'ud2'):
                break
            va = nxt
    # callee summary. Args consumed = how many placeholder slots were appended beyond entry depth.
    # We approximate: consume = max over exits of (entry-depth slots touched); net from the first exit.
    res = {'events': ev, 'notes': sorted(notes), 'fp_ops': fp_ops, 'n_insn': n_insn,
           'consume': 0, 'net': 0, 'ret_tag': 'E', 'trans_on_args': trans_on_args}
    if callee:
        res.update(callee_effect(entry))
        rt = [t for t in ret_tags if t]
        if res['net'] + res['consume'] > 0 and rt:
            res['ret_tag'] = worst(*rt)
        res['trans_on_args'] = trans_on_args and res['consume'] > 0
    return res


def callee_effect(entry):
    """Stack effect by depth counting (pure push/pop accounting, separate from tags)."""
    PUSH = {'fld', 'fild', 'fld1', 'fldz', 'fbld', 'fxtract', 'fsincos', 'fptan'} | CONSTS
    seen = set()
    work = [(entry, 0)]
    mind = 0
    exits = []
    tags_at_exit = []
    n = 0
    while work:
        va, d = work.pop()
        while True:
            if va in seen:
                break
            seen.add(va)
            i = insn_at(va)
            n += 1
            if i is None or n > 5000:
                break
            mn = i.mnemonic; ops = i.operands
            reads = 0
            if mn in PUSH:
                if mn == 'fld' and ops and ops[0].type == CS_OP_REG:
                    reads = streg(ops[0], i) + 1
                d += 1 if mn not in ('fxtract', 'fsincos', 'fptan') else 1
            elif mn in ARITH:
                if len(ops) == 2:
                    reads = max(streg(ops[0], i), streg(ops[1], i)) + 1
                elif len(ops) == 0:
                    reads = 2
                else:
                    reads = 1
                if mn.endswith('p') or len(ops) == 0:
                    d -= 1
            elif mn in ('fst', 'fstp', 'fist', 'fistp', 'fisttp', 'fsqrt', 'fchs', 'fabs', 'frndint', 'fsin', 'fcos',
                        'f2xm1', 'ftst'):
                reads = 1
                if ops and ops[0].type == CS_OP_REG:
                    reads = max(reads, streg(ops[0], i) + 1)
                if mn in ('fstp', 'fistp', 'fisttp'):
                    d -= 1
            elif mn in ('fpatan', 'fyl2x', 'fyl2xp1'):
                reads = 2; d -= 1
            elif mn in CMP:
                reads = 1
                if len(ops) >= 1 and ops[-1].type == CS_OP_REG:
                    reads = max(streg(o, i) for o in ops if o.type == CS_OP_REG) + 1
                elif not ops and mn != 'ftst':
                    reads = 2
                d -= 2 if mn.endswith('pp') else (1 if mn.endswith('p') else 0)
            elif mn == 'fxch':
                reads = (streg(ops[0], i) if ops else 1) + 1
            elif mn in ('fscale', 'fprem', 'fprem1'):
                reads = 2
            # depth before this insn's pushes is d(before); reads beyond (d_before) mean arg slots
            dbefore = d
            if mn in PUSH:
                dbefore = d - 1
            elif mn in ARITH and (mn.endswith('p') or len(ops) == 0):
                dbefore = d + 1
            elif mn in ('fstp', 'fistp', 'fisttp', 'fpatan', 'fyl2x', 'fyl2xp1'):
                dbefore = d + 1
            elif mn in CMP:
                dbefore = d + (2 if mn.endswith('pp') else (1 if mn.endswith('p') else 0))
            mind = min(mind, dbefore - reads, d)
            if mn == 'call':
                if ops[0].type == CS_OP_IMM:
                    tgt = ops[0].imm & 0xffffffff
                    if tgt != entry:
                        c, net, rt, tr = SUMMARY.get(tgt, (0, 0, 'NW', False))
                        mind = min(mind, d - c)
                        d += net
                va += i.size; continue
            if mn in ('ret', 'retn'):
                exits.append(d); break
            if mn == 'jmp':
                if ops[0].type == CS_OP_IMM:
                    tgt = ops[0].imm & 0xffffffff
                    if tgt < entry or tgt > entry + 0x4000:
                        c, net, rt, tr = SUMMARY.get(tgt, (0, 0, 'NW', False))
                        mind = min(mind, d - c)
                        exits.append(d + net); break
                    va = tgt; continue
                break
            if mn.startswith('j') or mn.startswith('loop'):
                if ops and ops[0].type == CS_OP_IMM:
                    work.append((ops[0].imm & 0xffffffff, d))
                va += i.size; continue
            if mn in ('int3', 'hlt'):
                break
            va += i.size
    consume = -mind if mind < 0 else 0
    net = max(exits) if exits else 0
    return {'consume': consume, 'net': net, 'ret_tag': 'NW' if net + consume > 0 else 'E'}


def verdict(r):
    kinds = {e[0] for e in r['events']}
    pcs = kinds & {'chain', 'cmp', 'intconv', 'widestore', 'retst0', 'dblop'}
    sse = kinds & {'dblop', 'intop', 'trans'}
    if r['fp_ops'] == 0:
        cls = 'NOFP'
    elif not pcs:
        cls = 'PC-INSENSITIVE'
    else:
        cls = 'PC-SENSITIVE'
    return cls, sorted(kinds - {'fldcw'}), ('equal' if not sse else 'differs:' + ','.join(sorted(sse)))


NAMES, MAPSYM = {}, {}


def main():
    if a.map:
        for l in Path(a.names).read_text().splitlines():
            p = l.split('	')
            if len(p) >= 3 and p[2]:
                NAMES[(p[0], p[1])] = p[2]
        for l in Path(a.map).read_text(errors='replace').splitlines():
            p = l.split()
            if len(p) >= 4 and ':' in p[0] and len(p[2]) == 8:
                try:
                    MAPSYM.setdefault(p[1], int(p[2], 16))
                except ValueError:
                    pass
    rows = [l.split('\t') for l in Path(a.inv).read_text().splitlines()[1:]]
    if a.rva:
        rows = [r for r in rows if r[3] == a.rva] or [['adhoc', '', '', a.rva, '']]
    out = ['tu\tsection\trva\tclass\tsse2_vs_pc24\tkinds\tn_events\tfirst_events\tfp_ops\tnotes']
    for r in rows:
        tu, sect, src, rva, how = r[:5]
        if not rva:
            out.append('\t'.join([tu, sect, '', 'NO_RVA', '', '', '', '', '', '']))
            continue
        if a.map:
            nm = NAMES.get((tu, rva)) or 'FUN_' + rva[2:]
            va = MAPSYM.get('_' + nm) or MAPSYM.get(nm)
            if va is None:
                out.append('	'.join([tu, sect, rva, 'NOT_IN_IMAGE', '', '', '', nm, '', '']))
                continue
            res = analyze(va)
        else:
            res = analyze(int(rva, 16))
        cls, kinds, sse = verdict(res)
        evs = ['%s@%08x(%s)' % (k, v, t) for k, v, t in res['events'] if k != 'fldcw'][:6]
        out.append('\t'.join([tu, sect, rva, cls, sse, ','.join(kinds), str(len(res['events'])), ' '.join(evs),
                              str(res['fp_ops']), ' '.join(res['notes'])]))
        if a.v:
            for k, v, t in res['events']:
                print('  %s %08x %s' % (k, v, t))
    Path(a.out).write_text('\n'.join(out) + '\n')
    print('\n'.join(out[:3] if not a.rva else out))
    from collections import Counter
    print(Counter(l.split('\t')[3] for l in out[1:]))


if __name__ == '__main__':
    main()
