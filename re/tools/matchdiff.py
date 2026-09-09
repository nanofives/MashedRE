#!/usr/bin/env python3
"""matchdiff.py -- byte/instruction match of a recompiled function vs the original.

Spike tool (2026-09-09) for the "matching decompilation" question: compile a ported
function with a period MSVC (7.x) and compare its machine code against the bytes at
the original RVA in MASHED.exe.unpatched.

    py -3.12 re/tools/matchdiff.py <obj> <symbol> <rva_hex> <size> [--exe PATH]

Method
  * the .obj is parsed as COFF (sections, symbols, relocations) -- no external deps
  * the original bytes come from the PE at the RVA (pefile maps VA -> file offset)
  * both are disassembled with capstone; instructions are compared pairwise as
    mnemonic + operand string, with immediates/displacements that are RELOCATED in
    the .obj (call targets, absolute data refs via relocations) masked on both sides.
    Non-relocated absolute constants (e.g. `mov eax,[0x899f7c]`) are compared exactly.
  * output: per-instruction table, count of matching instructions, byte-level
    identity after masking, first divergence.

Verdict scale used by the spike: BYTE-MATCH (identical after masking), INSTR-MATCH
(same instruction stream, encoding differs), PARTIAL (some divergence), NO-MATCH.
"""
import argparse
import collections
import difflib
import re
import struct
import sys

import capstone
import pefile

IMAGE_REL_I386_DIR32 = 0x0006
IMAGE_REL_I386_REL32 = 0x0014


def parse_coff(data):
    (machine, nsect, _ts, symoff, nsyms, opthdr, _chars) = struct.unpack_from('<HHIIIHH', data, 0)
    assert machine == 0x14C, 'not an i386 COFF object (machine=0x%x)' % machine
    sections = []
    off = 20 + opthdr
    for i in range(nsect):
        name, vsize, vaddr, rawsize, rawptr, relptr, lnptr, nrel, nln, chars = \
            struct.unpack_from('<8sIIIIIIHHI', data, off)
        sections.append(dict(idx=i + 1, name=name.rstrip(b'\0').decode(), size=rawsize,
                             rawptr=rawptr, relptr=relptr, nrel=nrel, chars=chars))
        off += 40
    strtab_off = symoff + nsyms * 18
    strtab_size = struct.unpack_from('<I', data, strtab_off)[0]
    strtab = data[strtab_off:strtab_off + strtab_size]
    symbols = []
    i = 0
    while i < nsyms:
        raw = data[symoff + i * 18: symoff + (i + 1) * 18]
        if raw[:4] == b'\0\0\0\0':
            soff = struct.unpack_from('<I', raw, 4)[0]
            name = strtab[soff:strtab.index(b'\0', soff)].decode()
        else:
            name = raw[:8].rstrip(b'\0').decode()
        value, secnum, typ, cls, naux = struct.unpack_from('<IhHBB', raw, 8)
        symbols.append(dict(name=name, value=value, sec=secnum, type=typ, cls=cls, index=i))
        i += 1 + naux
    relocs = {}
    for s in sections:
        lst = []
        for r in range(s['nrel']):
            va, symidx, typ = struct.unpack_from('<IIH', data, s['relptr'] + r * 10)
            lst.append(dict(off=va, sym=symidx, type=typ))
        relocs[s['idx']] = lst
    return sections, symbols, relocs


def function_bytes_from_obj(data, symbol):
    sections, symbols, relocs = parse_coff(data)
    cands = [s for s in symbols if s['name'] in (symbol, '_' + symbol) and s['sec'] > 0]
    if not cands:
        names = sorted(set(s['name'] for s in symbols if s['sec'] > 0 and s['cls'] == 2))
        raise SystemExit('symbol %s not found; external symbols: %s' % (symbol, names[:40]))
    sym = cands[0]
    sec = sections[sym['sec'] - 1]
    # function end = next symbol in the same section after this one, else section end
    later = sorted(s['value'] for s in symbols if s['sec'] == sym['sec'] and s['value'] > sym['value'] and s['cls'] in (2, 3))
    end = later[0] if later else sec['size']
    # COMDAT (/Gy) puts each function in its own section; end == section size then.
    start = sym['value']
    body = data[sec['rawptr'] + start: sec['rawptr'] + end]
    rel = [(r['off'] - start, r['type']) for r in relocs[sec['idx']] if start <= r['off'] < end]
    return body, rel, sec['name']



def function_relocs_named(data, symbol):
    """[(offset_within_function, type, target_symbol_name)] for one function.

    Normal calls to other TUs are COFF relocations, not absolute literals, so a
    triage that only looks at literal call targets is blind to them (that is why the
    first INLINED test fired 3 times out of 155).
    """
    sections, symbols, relocs = parse_coff(data)
    cands = [x for x in symbols if x['name'] in (symbol, '_' + symbol) and x['sec'] > 0]
    if not cands:
        return []
    sym = cands[0]
    sec = sections[sym['sec'] - 1]
    later = sorted(x['value'] for x in symbols
                   if x['sec'] == sym['sec'] and x['value'] > sym['value'] and x['cls'] in (2, 3))
    end = later[0] if later else sec['size']
    by_index = {x['index']: x['name'] for x in symbols}
    out = []
    for r in relocs[sec['idx']]:
        if sym['value'] <= r['off'] < end:
            out.append((r['off'] - sym['value'], r['type'], by_index.get(r['sym'], '')))
    return out

def original_bytes(exe_path, rva, size):
    pe = pefile.PE(exe_path, fast_load=True)
    va = rva - pe.OPTIONAL_HEADER.ImageBase
    return pe.get_data(va, size)


def disasm(code, base):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    return list(md.disasm(code, base))


def mask_relocs(code, relocs):
    b = bytearray(code)
    for off, _t in relocs:
        b[off:off + 4] = b'\0\0\0\0'
    return bytes(b)


def norm_insn(insn, reloc_offsets, base):
    """Return a comparable string: mnemonic + operands with relocated fields masked."""
    off = insn.address - base
    text = insn.op_str
    hit = [r for r in reloc_offsets if off <= r < off + insn.size]
    if hit:
        # mask every immediate/displacement in the operand string
        text = re.sub(r'0x[0-9a-fA-F]+', 'REL', text)
    return '%s %s' % (insn.mnemonic, text)


# --- operand correspondence -------------------------------------------------
# Register allocation and scheduling drift with the compiler version, but the
# ABSOLUTE ADDRESSES a function touches and the IMMEDIATES it uses come from the
# source. Comparing those two multisets is compiler-version-independent, so it
# catches transcription defects (wrong global, wrong stride/offset -- the U-9085
# class) without needing a period-correct toolchain.

SECURITY_COOKIE = 0x00616038   # __security_check_cookie global; we build /GS-, some
                               # original TUs are /GS. Build-flag artifact, not a defect.

# Section ranges of the anchored original (pefile dump of MASHED.exe.unpatched,
# 2026-09-09; image base 0x00400000):
#   .text   00401000-005cb000   _rwcseg 005cb000-005cc000
#   .rdata  005cc000-005ea000   .data   005ea000-00914704
TEXT = (0x00401000, 0x005cc000)
RDATA = (0x005cc000, 0x005ea000)
DATA = (0x005ea000, 0x00914704)


def operand_facts(insns, reloc_offsets, base, func_entries=frozenset()):
    """-> (data_addrs, code_addrs, rdata_addrs, imms), each a sorted multiset.

    ONLY `data_addrs` (.data globals) is fit to gate on. Calibrated 2026-09-09 over
    1,086 already-verified C3/C4 rows: a single combined address+immediate set failed
    56% of them, and every dominant failure class turned out to be an artifact:

    * CODE addresses (.text) are call targets and jump tables. The original encodes a
      call as E8 rel32 -- position-relative, no absolute operand -- while a port calling
      `reinterpret_cast<fn*>(0x00xxxxxx)` materialises the target as a literal; and a
      switch compiles to `jmp [idx*4 + <table in .text>]` whose table address is
      position-dependent. Comparing either compares ENCODINGS, not source facts.
      183 of 608 failures were purely call targets; jump tables dominated another bucket.
    * RDATA addresses are compiler-pooled literals (floats, strings). Ours land in our
      own .rdata behind a COFF relocation, which is masked, so they can never correspond.
    * IMMEDIATES drift with constant folding and control-flow shape: the original may
      build an ARGB literal byte-by-byte into a stack buffer where we write one packed
      dword (`-0x3a,0x3d,0x4e +0xff3d3a98`), and `xor r,r` vs `mov r,0` changes whether
      a 0 appears at all. 217 of 608 failures were immediates-only.

    .data addresses are what a transcription defect gets wrong (the U-9085 class: wrong
    global, wrong base, wrong stride applied to a global), and they come from the source
    rather than the optimiser. The other three rows are returned for triage, not gating.
    """
    data, code, rdata, imms = [], [], [], []

    def note_addr(v):
        if v == SECURITY_COOKIE:
            return
        if DATA[0] <= v < DATA[1]:
            data.append(v)
        elif RDATA[0] <= v < RDATA[1]:
            rdata.append(v)
        elif TEXT[0] <= v < TEXT[1]:
            code.append(v)

    for ins in insns:
        off = ins.address - base
        if any(off <= r < off + ins.size for r in reloc_offsets):
            continue                      # relocated field: not a source constant
        for op in ins.operands:
            if op.type == capstone.x86.X86_OP_MEM:
                note_addr(op.mem.disp & 0xffffffff)
            elif op.type == capstone.x86.X86_OP_IMM:
                if ins.group(capstone.x86.X86_GRP_JUMP) or ins.group(capstone.x86.X86_GRP_CALL):
                    continue              # branch targets are position-dependent
                v = op.imm & 0xffffffff
                if TEXT[0] <= v < DATA[1]:
                    note_addr(v)
                else:
                    imms.append(op.imm)
    return sorted(data), sorted(code), sorted(rdata), sorted(imms)



def load_func_entries(path=None):
    """Function entry points from the FuncBoundsPC.java cache (empty if absent)."""
    import os
    if path is None:
        path = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                            '..', 'console', 'cache', 'func_bounds.csv')
    try:
        with open(path, encoding='utf-8') as fh:
            next(fh)
            return frozenset(int(ln.split(',')[0], 16) for ln in fh if ln.strip())
    except OSError:
        return frozenset()


def multiset_report(name, ours, orig):
    """PASS/FAIL on the DISTINCT set; multiplicity is advisory only.

    Calibrated 2026-09-09 on C4 0x00423e60: the original computes `imul eax,eax,0x138`
    once per branch (twice), MSVC 2022 hoists it above the branch and computes it once.
    A multiset gate calls that a defect; it is a CSE artifact. A missing/extra DISTINCT
    value is what indicates a real transcription error.
    """
    so, sg = set(ours), set(orig)
    only_ours, only_orig = sorted(so - sg), sorted(sg - so)
    ok = not only_ours and not only_orig
    note = ''
    if ok and collections.Counter(ours) != collections.Counter(orig):
        note = '   (multiplicity differs -- CSE/duplication, advisory)'
    print('   %-12s %d/%d distinct values%s%s' %
          (name + ':', len(so & sg), max(len(so), len(sg)),
           '' if ok else '   MISMATCH', note))
    if only_ours:
        print('      only in OURS: %s' % ', '.join(hex(x) if x > 9 else str(x) for x in only_ours[:12]))
    if only_orig:
        print('      only in ORIG: %s' % ', '.join(hex(x) if x > 9 else str(x) for x in only_orig[:12]))
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('obj')
    ap.add_argument('symbol')
    ap.add_argument('rva', type=lambda s: int(s, 16))
    ap.add_argument('size', type=lambda s: int(s, 0))
    ap.add_argument('--exe', default='original/MASHED.exe.unpatched')
    ap.add_argument('--quiet', action='store_true')
    a = ap.parse_args()

    obj = open(a.obj, 'rb').read()
    ours, relocs, secname = function_bytes_from_obj(obj, a.symbol)
    orig = original_bytes(a.exe, a.rva, a.size)

    reloc_offs = [r[0] for r in relocs]
    di_ours = disasm(ours, a.rva)
    di_orig = disasm(orig, a.rva)

    # relocation mask applied to the original at the SAME offsets so a rel32 call
    # target / dir32 address does not count as a mismatch
    m_ours = mask_relocs(ours, relocs)
    m_orig = mask_relocs(orig, relocs) if len(orig) >= len(ours) else orig

    # instruction streams, aligned with an LCS (a single inserted instruction must
    # not make every later instruction read as a mismatch)
    s_ours = [norm_insn(i, reloc_offs, a.rva) for i in di_ours]
    s_orig = [norm_insn(i, [], a.rva) for i in di_orig]
    sm = difflib.SequenceMatcher(a=s_ours, b=s_orig, autojunk=False)
    match = sum(b.size for b in sm.get_matching_blocks())
    n = max(len(s_ours), len(s_orig))

    byte_ident = (len(ours) == len(orig) and m_ours == m_orig[:len(m_ours)])
    if byte_ident:
        verdict = 'BYTE-MATCH'
    elif s_ours == s_orig:
        verdict = 'INSTR-MATCH'
    elif match:
        verdict = 'PARTIAL'
    else:
        verdict = 'NO-MATCH'

    print('== %s @ 0x%08x  ours=%d bytes (%s, %d relocs)  orig=%d bytes' %
          (a.symbol, a.rva, len(ours), secname, len(relocs), len(orig)))
    print('   instructions: ours=%d orig=%d  aligned=%d/%d (%.0f%%)' %
          (len(di_ours), len(di_orig), match, n, 100.0 * match / n if n else 0))
    print('   VERDICT: %s' % verdict)

    # compiler-version-independent correspondence (data addresses are the gate;
    # code addresses and immediates are reported but never gate -- see operand_facts)
    ents = load_func_entries()
    fa_ours = operand_facts(di_ours, reloc_offs, a.rva, ents)
    fa_orig = operand_facts(di_orig, [], a.rva, ents)
    ok_d = multiset_report('.data addrs', fa_ours[0], fa_orig[0])
    multiset_report('.text addrs', fa_ours[1], fa_orig[1])
    multiset_report('.rdata addrs', fa_ours[2], fa_orig[2])
    multiset_report('immediates', fa_ours[3], fa_orig[3])
    print('   DATA-ADDRESS CORRESPONDENCE: %s%s' %
          ('PASS' if ok_d else 'FAIL',
           '   (.text/.rdata/imm rows are advisory only)'))

    if not a.quiet:
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            for k in range(max(i2 - i1, j2 - j1)):
                so = s_ours[i1 + k] if i1 + k < i2 else ''
                sg = s_orig[j1 + k] if j1 + k < j2 else ''
                flag = '  ' if tag == 'equal' else '!!'
                print('   %s %-42s | %s' % (flag, so, sg))
    return 0 if verdict in ('BYTE-MATCH', 'INSTR-MATCH') else 1


if __name__ == '__main__':
    sys.exit(main())
