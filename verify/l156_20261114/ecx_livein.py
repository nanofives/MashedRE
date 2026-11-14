#!/usr/bin/env python3
"""ECX live-in walk over original/MASHED.exe.unpatched (L156-ECX-556CA0).

For a function entry, follows the CFG instruction by instruction (capstone, i386)
with one state per instruction: is ECX fully written on every path from entry?
Prints every instruction that reads ECX/CX/CL/CH while ECX is not fully written
on some path, every full write of ECX, every partial write (CL/CH/CX), and every
call reached while ECX is not fully written. Direct callees are walked the same
way (recursively, depth-limited) and the caller's state after the call is
"fully written" only if the callee fully writes ECX on every path to its ret.
Indirect calls/jumps are listed as UNRESOLVED.

  py -3.12 verify/l156_20261114/ecx_livein.py 0x5547f0 0x554940 [--depth N]
"""
import argparse
import struct
import sys
from pathlib import Path

import capstone
from capstone import x86_const as X

ROOT = Path(__file__).resolve().parents[2]
EXE = ROOT.parents[1] / 'original' / 'MASHED.exe.unpatched' if (ROOT.parents[1] / 'original').exists() else ROOT / 'original' / 'MASHED.exe.unpatched'

ECX_ALL = {X.X86_REG_ECX, X.X86_REG_CX, X.X86_REG_CL, X.X86_REG_CH}


def load(path):
    d = open(path, 'rb').read()
    e = struct.unpack_from('<I', d, 0x3c)[0]
    coff = e + 4
    nsec, = struct.unpack_from('<H', d, coff + 2)
    opt = coff + 20
    sect_off = opt + struct.unpack_from('<H', d, coff + 16)[0]
    ib, = struct.unpack_from('<I', d, opt + 28)
    secs = []
    for i in range(nsec):
        so = sect_off + i * 40
        vsz, vrva, rsz, rptr = struct.unpack_from('<IIII', d, so + 8)
        secs.append((vrva, vsz, rptr, rsz))
    return d, ib, secs


DATA, IB, SECS = load(EXE)
MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True
_cache = {}


def insn_at(va):
    if va in _cache:
        return _cache[va]
    rva = va - IB
    for vrva, vsz, rptr, rsz in SECS:
        if vrva <= rva < vrva + rsz:
            off = rptr + rva - vrva
            ins = next(MD.disasm(DATA[off:off + 16], va), None)
            _cache[va] = ins
            return ins
    _cache[va] = None
    return None


def classify(ins):
    """(reads_ecx, full_write, partial_write)"""
    try:
        rd, wr = ins.regs_access()
    except capstone.CsError:
        rd, wr = [], []
    rd, wr = set(rd), set(wr)
    ops = ins.operands
    # zero idioms read nothing
    if ins.id in (X.X86_INS_XOR, X.X86_INS_SUB) and len(ops) == 2 and \
            ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_REG and ops[0].reg == ops[1].reg:
        rd.discard(ops[0].reg)
    reads = bool(rd & ECX_ALL)
    full = X.X86_REG_ECX in wr
    partial = bool(wr & {X.X86_REG_CX, X.X86_REG_CL, X.X86_REG_CH}) and not full
    return reads, full, partial


summaries = {}   # entry -> (events, writes_on_all_paths)


def walk(entry, depth, maxdepth, out, indent=''):
    """Returns True if ECX is fully written on every path to every ret."""
    if entry in summaries:
        ev, allw = summaries[entry]
        out.append(f'{indent}(callee 0x{entry:08x} already walked: full-write-on-all-rets={allw})')
        return allw
    summaries[entry] = (None, False)  # recursion guard: conservative
    state = {}       # va -> defined(bool) at instruction entry (AND-merged)
    work = [(entry, False)]
    events = []
    ret_states = []
    seen_calls = {}
    while work:
        va, d = work.pop()
        if va in state:
            if state[va] is False or d is True:
                continue        # no change (False is bottom: already least)
            # state True, incoming False -> lower to False and re-walk
        state[va] = d if va not in state else (state[va] and d)
        ins = insn_at(va)
        if ins is None:
            events.append((va, 'UNDECODABLE', ''))
            continue
        reads, full, partial = classify(ins)
        txt = f'{ins.mnemonic} {ins.op_str}'.strip()
        cur = state[va]
        if reads and not cur:
            events.append((va, 'READ-before-full-write', txt))
        if full:
            events.append((va, 'FULL-WRITE', txt))
        elif partial:
            events.append((va, 'PARTIAL-WRITE', txt))
        nxt = va + ins.size
        after = cur or full
        g = ins.group
        if g(capstone.CS_GRP_RET):
            ret_states.append((va, cur))
            continue
        if ins.id == X.X86_INS_CALL:
            op = ins.operands[0]
            if op.type == X.X86_OP_IMM:
                tgt = op.imm
                if not cur:
                    events.append((va, 'CALL-while-not-written', txt))
                    if depth < maxdepth:
                        sub = []
                        cw = walk(tgt, depth + 1, maxdepth, sub, indent + '    ')
                        seen_calls[va] = (tgt, cw, sub)
                        after = cw
                    else:
                        events.append((va, 'CALL-NOT-WALKED (depth limit)', txt))
                        after = False
                else:
                    after = True
            else:
                events.append((va, 'UNRESOLVED-INDIRECT-CALL', txt) if not cur else (va, 'indirect call (ECX written)', txt))
                after = cur  # conservative: unknown
            work.append((nxt, after))
            continue
        if ins.id == X.X86_INS_JMP:
            op = ins.operands[0]
            if op.type == X.X86_OP_IMM:
                work.append((op.imm, after))
            else:
                events.append((va, 'UNRESOLVED-INDIRECT-JMP' + ('' if after else ' while-not-written'), txt))
            continue
        if g(capstone.CS_GRP_JUMP):
            work.append((ins.operands[0].imm, after))
            work.append((nxt, after))
            continue
        work.append((nxt, after))
    allw = bool(ret_states) and all(s for _, s in ret_states)
    summaries[entry] = (events, allw)
    out.append(f'{indent}== 0x{entry:08x}: {len(state)} insns reached, rets={[hex(v) for v, _ in ret_states]}, '
               f'ECX full-written at every ret={allw}')
    # dedupe events by (va, kind)
    seen = set()
    for va, kind, txt in sorted(events):
        if (va, kind) in seen:
            continue
        seen.add((va, kind))
        out.append(f'{indent}  0x{va:08x}  {kind:32s} {txt}')
        if va in seen_calls:
            tgt, cw, sub = seen_calls[va]
            out.extend(sub)
    return allw


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('entries', nargs='+')
    ap.add_argument('--depth', type=int, default=4)
    a = ap.parse_args()
    print(f'# image {EXE}')
    for e in a.entries:
        out = []
        walk(int(e, 16), 0, a.depth, out)
        print('\n'.join(out))
        print()


if __name__ == '__main__':
    sys.exit(main())
