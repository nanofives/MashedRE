"""RWC-PLUGIN-ATTACH: verbatim naked-asm transcription ("lift") of RW-core functions that have no re/match source.

Used by gen_rwcore_tus.py for closure RVAs that would otherwise be missing stops (the plugin module constructors and
destructors 0x004e7ef0 .. 0x00557e90 registered through 0x004c2d90, their callees, the stream / object callbacks in the
seeded data range). Same method as re/tools/gen_naked_asm.py and the run-38 hand bodies (RwCore_Hand.cpp): the bytes of
original/MASHED.exe.unpatched are disassembled with capstone and re-emitted as MSVC inline asm, instruction for
instruction. Not a source, never C4: a lifted body is a provisional hand body on the MASHED_RWC_CORE knob path only.

Rules (a function the lifter cannot transcribe under them is refused and stays a missing stop, with the reason logged):
  * extent by recursive descent from the entry: conditional branches both ways, direct jumps followed unless the target
    is another known function start (then it is a tail jump to that symbol), stop at ret / indirect jmp;
  * reached instructions are emitted in address order; a branch target becomes a label L_<addr>;
  * `call <rva>` and a tail `jmp <rva>` resolve through the caller's resolver (closure symbol, CRT thunk); an unresolved
    target refuses the function;
  * an immediate that is a code address (0x00401000..0x005cc000) resolves the same way and is emitted as `offset <sym>`;
  * absolute memory operands stay at their original address (`ds:[0xADDR]`): the top-down exe keeps the original's
    globals there. .rdata addresses read by the body are returned so the generator seeds them;
  * indirect jumps (jump tables), calls through an absolute address (the IAT) and loads of code addresses from memory
    refuse the function.
"""
import re

import capstone

TEXT_LO, TEXT_HI = 0x00401000, 0x005cc000
RDATA_LO, RDATA_HI = 0x005cc000, 0x005ea000
X87_1OP = {'fadd', 'fsub', 'fsubr', 'fmul', 'fdiv', 'fdivr'}
_md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)


class Refused(Exception):
    pass


def extent(read, start, known_starts, limit=0x4000):
    """Recursive descent. Returns {addr: insn} of reached instructions and the set of intra-function branch targets."""
    insns, todo, labels = {}, [start], set()
    while todo:
        a = todo.pop()
        while a not in insns:
            if not (start <= a < start + limit):
                raise Refused('descent left the 0x%x-byte window at 0x%08x' % (limit, a))
            code = read(a, 16)
            ins = next(_md.disasm(code, a), None)
            if ins is None:
                raise Refused('undecodable byte at 0x%08x' % a)
            insns[a] = ins
            mn, op = ins.mnemonic, ins.op_str
            if mn in ('ret', 'retf', 'int3', 'hlt'):
                break
            if mn == 'jmp':
                m = re.fullmatch(r'0x([0-9a-f]+)', op)
                if not m:
                    break               # indirect jmp: refused at emit time
                t = int(m.group(1), 16)
                if t != start and t in known_starts:
                    break               # tail jump
                labels.add(t)
                a = t
                continue
            if mn.startswith('j') or mn in ('loop', 'loope', 'loopne', 'jecxz'):
                m = re.fullmatch(r'0x([0-9a-f]+)', op)
                if not m:
                    raise Refused('indirect %s at 0x%08x' % (mn, a))
                t = int(m.group(1), 16)
                labels.add(t)
                todo.append(t)
            a += ins.size
    return insns, labels


def lift(read, start, name, known_starts, resolve, convdecl):
    """Return (text, refs, rdata) for one function. resolve(rva_hex8) -> symbol or None. convdecl = 'int __stdcall'
    style prefix + parameter list for the definition, e.g. ('__stdcall', 8)."""
    insns, labels = extent(read, start, known_starts)
    refs, rdata, used = set(), set(), set()

    def sym(t, what, at):
        s = resolve('%08x' % t)
        if not s:
            raise Refused('unresolved %s target 0x%08x at 0x%08x' % (what, t, at))
        refs.add('%08x' % t)
        return s

    body = []
    for a in sorted(insns):
        ins = insns[a]
        mn, op = ins.mnemonic, ins.op_str.replace('xword ptr', 'tbyte ptr')
        if re.search(r'(movs|stos|lods|scas|cmps)[bwd]$', mn):
            body.append((a, mn))
            continue
        if mn == 'call' or mn == 'jmp' or (mn.startswith('j') and mn != 'jmp'):
            m = re.fullmatch(r'0x([0-9a-f]+)', op)
            if not m:
                if mn == 'call' and not re.search(r'\[0x[0-9a-f]+\]', op):
                    body.append((a, '%s %s' % (mn, op)))      # call reg / call [reg+disp]
                    continue
                # jmp / call through a pointer in .data (0x005ea000 ..): the RW dispatch slots 0x006187a8 .. (seeded and
                # fixed up by the generator) or a pointer set at run time. The IAT (.rdata) stays refused.
                ma = re.fullmatch(r'dword ptr \[0x([0-9a-f]+)\]', op)
                if mn in ('call', 'jmp') and ma and int(ma.group(1), 16) >= 0x005ea000:
                    body.append((a, '%s dword ptr ds:[0x%08x]' % (mn, int(ma.group(1), 16))))
                    continue
                raise Refused('indirect %s %s at 0x%08x' % (mn, op, a))
            t = int(m.group(1), 16)
            if mn == 'call':
                body.append((a, 'call %s' % sym(t, 'call', a)))
            elif t in insns:
                used.add(t)
                body.append((a, '%s L_%x' % (mn, t)))
            elif mn == 'jmp':
                body.append((a, 'jmp %s' % sym(t, 'tail jmp', a)))
            else:
                raise Refused('conditional branch out of the function to 0x%08x at 0x%08x' % (t, a))
            continue

        def mem(mm):
            v = int(mm.group(1), 16)
            if TEXT_LO <= v < TEXT_HI:
                raise Refused('memory operand in .text 0x%08x at 0x%08x' % (v, a))
            if RDATA_LO <= v < RDATA_HI:
                rdata.add(v)
            return '[0x%08x]' % v
        op = re.sub(r'\[0x([0-9a-f]+)\]', lambda mm: 'ds:' + mem(mm), op)
        # displacement forms [reg + 0xADDR]: .rdata tables are seeded, .text tables refused
        for mm in re.finditer(r'\+ 0x([0-9a-f]{6,8})\]', op):
            v = int(mm.group(1), 16)
            if TEXT_LO <= v < TEXT_HI:
                raise Refused('indexed .text operand at 0x%08x' % a)
            if RDATA_LO <= v < RDATA_HI:
                rdata.add(v)

        # immediates (last operand without brackets)
        def imm(mm):
            v = int(mm.group(1), 16)
            if TEXT_LO <= v < TEXT_HI:
                return 'offset ' + sym(v, 'code-pointer immediate', a)
            if RDATA_LO <= v < RDATA_HI:
                rdata.add(v)
            return mm.group(0)
        parts = op.rsplit(', ', 1)
        if parts and '[' not in parts[-1]:
            parts[-1] = re.sub(r'^0x([0-9a-f]+)$', imm, parts[-1])
        op = ', '.join(parts)
        op = op.replace('st(0)', 'st')
        if mn in X87_1OP and re.fullmatch(r'st\(\d\)', op):
            op = 'st, ' + op
        if mn.endswith('p') and mn[:-1] in X87_1OP and re.fullmatch(r'st\(\d\)', op):
            op = op + ', st'
        body.append((a, ('%s %s' % (mn, op)).strip()))

    conv, nb = convdecl
    params = ', '.join('int' for _ in range(nb // 4)) or 'void'
    out = ['// HAND-LIFT 0x%08x %s  (%d instructions, verbatim from the anchored disassembly; lift_rwcore.py)'
           % (start, name, len(insns)),
           'extern "C" __declspec(naked) int %s%s(%s) {' % (conv + ' ' if conv else '', name, params),
           '    __asm {']
    for a, t in body:
        if a in used:
            out.append('    L_%x:' % a)
        out.append('        %-40s // %08x' % (t, a))
    out += ['    }', '}', '']
    return '\n'.join(out), refs, rdata
