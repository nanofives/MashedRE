"""IMG-4-GHIDRA-DATAREF: select the code-pointer slots of the file-backed image from the Ghidra export.

Input : re/tools/image/dataref_raw.tsv (ghidra_dataref_export.py: every memory reference from the image ranges into an
        executable block, with Ghidra's data type at the source).
Rule  : positive evidence only (RESULT_PORTSTATE-IMAGE.md section 3). A slot is listed when Ghidra holds a reference
        from it into code, the data at the slot is pointer-typed (a type name ending in '*', or 'pointer'), the slot is
        4-aligned, and the file's dword at the slot (MASHED.exe.unpatched) equals the reference target.
Output: re/tools/image/CODEPTR_IMAGE.tsv  {slot, target, ghidra_type, src_label, target_fn, in_codeptr_slots, exe_symbol, bind,
        evidence} (evidence ghidra-ref, or codeptr-slots for a CODEPTR_SLOTS slot Ghidra holds no reference from)
        with --write also mashedmod/src/mashed_re/Compat/PortTopCodePtrImage.inc: CP_IMG_FIX(slot, sym, orig) for every
        listed slot NOT already in verify/portorigtop_20261109/CODEPTR_SLOTS.tsv (those stay in PortTopCodePtr.inc).
        only for a target with an exe body (the symbol BINDINGS.tsv records for it). A new slot whose target has no exe
        body is listed with bind=trap and NOT written: it keeps the original value and MASHED_CODE_TRAP reports the
        call (spec section 3, choice (b)). A stop thunk there would end the process before trap mode 2 can list the
        later calls. Slots a PortTop wrapper rebinds at run time are listed with bind=excluded (EXCLUDE below).

usage: py -3.12 re/tools/image/codeptr_image.py [--write]
"""
import csv
import os
import re
import sys

import pefile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
RAW = os.path.join(HERE, 'dataref_raw.tsv')
OUT_TSV = os.path.join(HERE, 'CODEPTR_IMAGE.tsv')
CP_SLOTS = os.path.join(ROOT, 'verify', 'portorigtop_20261109', 'CODEPTR_SLOTS.tsv')
CP_INC = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'Compat', 'PortTopCodePtr.inc')
OUT_INC = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re', 'Compat', 'PortTopCodePtrImage.inc')
BIND = os.path.join(ROOT, 'verify', 'porttop_20261108', 'BINDINGS.tsv')
EXE = os.environ.get('MASHED_UNPATCHED', r'C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched')
# Exe symbols usable as a code-pointer target: a body the exe compiles for that RVA (the kinds PortTopCodePtr.inc binds).
BIND_KINDS = {'matched', 'bound', 'port', 'rw', 'crt', 'near-match', 'shim'}
# Slots a PortTop wrapper rebinds itself at run time, only while they hold 0 or their original target: rewriting them
# earlier defeats that check. Match/PortTop_0045bae0.cpp:183-196 (PortTop_PWI_Call0045bae0) stores s_pwiInit[i] in
# 0x005f9998 + 0x40*i + 0x14 for i 0..8 when the slot is 0 or s_pwiOrig[i]. Found by the IMG-4 knob boot: with these
# slots rewritten, entry 2 (0x00455400, no exe body) was called through its stop at boot.
EXCLUDE = {0x005F9998 + 0x40 * i + 0x14: 'PortTop_PWI_Call0045bae0 rebinds it (Match/PortTop_0045bae0.cpp:183-196)'
           for i in range(9)}


def is_pointer(t):
    return t.endswith('*') or t == 'pointer'


def main():
    pe = pefile.PE(EXE, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase

    def file_dword(va):
        for s in pe.sections:
            lo = base + s.VirtualAddress
            if lo <= va and va + 4 <= lo + s.SizeOfRawData:
                return pe.get_dword_at_rva(va - base)
        return None

    raw = list(csv.DictReader(open(RAW, encoding='utf-8'), delimiter='\t'))
    cps = {int(r['slot'], 16): int(r['target'], 16) for r in csv.DictReader(open(CP_SLOTS, encoding='utf-8'), delimiter='\t')}
    bind = {}
    for r in csv.DictReader(open(BIND, encoding='utf-8'), delimiter='\t'):
        if r['kind'] in BIND_KINDS and r['exe_symbol'] and re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', r['exe_symbol']):
            bind[int(r['rva'], 16)] = r['exe_symbol']

    rows, refused = [], []
    for r in raw:
        slot, tgt = int(r['slot'], 16), int(r['target'], 16)
        why = None
        if not is_pointer(r['data_type']):
            why = 'type ' + r['data_type']
        elif slot & 3:
            why = 'unaligned'
        elif r['component'] != '-':
            why = None  # pointer component of a struct / array: still a pointer-typed dword
        if why is None:
            fd = file_dword(slot)
            if fd != tgt:
                why = 'file dword %s != target' % ('none' if fd is None else '%08x' % fd)
        if why:
            refused.append((slot, tgt, why))
            continue
        incp = slot in cps
        sym = bind.get(tgt, '')
        kind = 'cp-slots' if incp else ('excluded' if slot in EXCLUDE else ('bound' if sym else 'trap'))
        rows.append((slot, tgt, r['data_type'], r['src_label'], r['target_fn'], 'yes' if incp else 'no', sym, kind,
                     'ghidra-ref'))
    # CODEPTR_SLOTS slots Ghidra holds no reference from (undefined bytes there): kept on their own evidence (Ghidra
    # labels of SEED2_ADDED.tsv, codeptr_inventory.py), when the file dword still equals the recorded target.
    have = {r[0] for r in rows}
    for s, t in sorted(cps.items()):
        if s not in have and file_dword(s) == t:
            rows.append((s, t, 'undefined1 (no Ghidra ref)', '-', '-', 'yes', bind.get(t, ''), 'cp-slots',
                         'codeptr-slots'))
    rows.sort()
    seen = {}
    for row in rows:
        if row[0] in seen and seen[row[0]] != row[1]:
            raise SystemExit('slot 0x%08x has two targets' % row[0])
        seen[row[0]] = row[1]
    slots = {r[0] for r in rows if r[8] == 'ghidra-ref'}
    miss = sorted(s for s in cps if s not in slots)
    wrong = sorted(s for s in cps if s in seen and seen[s] != cps[s])
    absent = sorted(s for s in cps if s not in seen)
    new = [r for r in rows if r[5] == 'no']
    fix = [r for r in new if r[7] == 'bound']
    print('export rows %d, listed %d (refused %d), CODEPTR_SLOTS %d: present %d, missing %d, target differs %d'
          % (len(raw), len(rows), len(refused), len(cps), len(cps) - len(miss), len(miss), len(wrong)))
    print('CODEPTR_IMAGE.tsv rows %d (ghidra-ref %d, codeptr-slots only %d); CODEPTR_SLOTS absent from it: %d'
          % (len(rows), sum(1 for r in rows if r[8] == 'ghidra-ref'), sum(1 for r in rows if r[8] == 'codeptr-slots'), len(absent)))
    print('new slots %d (bound %d, trap %d, excluded %d), distinct new targets %d'
          % (len(new), len(fix), sum(1 for r in new if r[7] == 'trap'), sum(1 for r in new if r[7] == 'excluded'),
             len({r[1] for r in new})))
    for s in miss:
        print('  CODEPTR_SLOTS slot missing from the export: 0x%08x -> 0x%08x' % (s, cps[s]))
    for s, t, w in refused:
        print('  refused 0x%08x -> 0x%08x: %s' % (s, t, w))
    if '--write' not in sys.argv:
        return
    with open(OUT_TSV, 'w', encoding='utf-8', newline='\n') as f:
        f.write('slot\ttarget\tghidra_type\tsrc_label\ttarget_fn\tin_codeptr_slots\texe_symbol\tbind\tevidence\n')
        for r in rows:
            f.write('0x%08x\t0x%08x\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' % r)
    syms = sorted({r[6] for r in fix})
    L = ['// GENERATED by re/tools/image/codeptr_image.py --write (IMG-4-GHIDRA-DATAREF). Do not edit.',
         '// Code-pointer slots of the file-backed image from the Ghidra data-reference export (re/tools/image/CODEPTR_IMAGE.tsv)',
         '// that PortTopCodePtr.inc does not already fix: %d new slots; %d bound here to an exe body named in BINDINGS.tsv;'
         % (len(new), len(fix)),
         '// %d with no exe body are NOT written (they keep the original value and MASHED_CODE_TRAP reports a call);'
         % sum(1 for r in new if r[7] == 'trap'),
         '// %d excluded (a PortTop wrapper rebinds them at run time).' % sum(1 for r in new if r[7] == 'excluded'),
         '// Consumer: Compat/PortTopCodePtr.cpp, applied only when the image is loaded (MASHED_PORTSTATE_IMAGE=1) and',
         '// MASHED_OT_CODEPTR=1; each slot is rewritten only when its image dword equals the original target.',
         '#define PORTTOP_CODEPTR_IMG_SLOTS %d' % len(fix)]
    L.append('#ifdef CP_IMG_EXTERN')
    L += ['CP_IMG_EXTERN(%s)' % s for s in syms]
    L.append('#endif')
    L.append('#ifdef CP_IMG_FIX')
    for r in fix:
        L.append('CP_IMG_FIX(0x%08x, %s, 0x%08x)   // %s, %s'
                 % (r[0], r[6], r[1], r[2], r[4] if r[4] != '-' else 'LAB_%08x' % r[1]))
    L.append('#endif')
    with open(OUT_INC, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(L) + '\n')
    print('wrote', OUT_TSV, 'and', OUT_INC, '(%d CP_IMG_FIX, extern syms %d)' % (len(fix), len(syms)))


if __name__ == '__main__':
    main()
