"""Byte-level library-membership check for rows the tracker labels first-party inside library address ranges.

Ruling 2026-10-11 (Mariano, relayed by the plan-#3 orchestrator): the census rows labelled first-party inside the
RenderWare core, libpng and CRT-tail ranges are relabelled after a byte check by the method of
re/tools/dshow_band_check.py; a row is relabelled only when it makes no call into first-party game code and touches no
game-owned data. Rows that fail stay as they are.

Tests, per candidate row (body = row start .. next hooks.csv row start):
  1. calls first-party code: a call/jmp rel32 leaving the body whose target's owner is neither a library row
     (`third-party-library[...]`, or a CRT helper named `_`/`FID_`) nor a still-passing candidate;
  2. touches game-owned data: an absolute data address in the body (any operand, read or write) that first-party code
     WRITES somewhere in .text. Addresses never written by code (constants, vtables, strings) and state written only by
     library code (RwGlobals 0x007d3ff8 and the RW module globals are read by the game but written by RW) pass. This is
     the DirectShow test's "shared and not a constant" with the ownership made mechanical: the dshow band had no
     library-owned mutable state shared with the game, RenderWare does.
The candidate set is iterated to a fixed point: a row that fails is removed from the library side, which can make a
caller of it fail test 1 on the next pass.

Library side: a row is on the library side when it is labelled `third-party-library[...]` / `_` / `FID_`, OR lies in
a --band range (any label), OR is a still-passing candidate. As in dshow_band_check.py (`inr()`), code inside the band
is not "first-party" for either test: the tracker's `render` / `util` / `boot` labels inside these ranges are what the
ruling corrects, so they cannot be the evidence against it. First-party = everything else.

Usage (worktree or repo root):
  py -3.12 re/tools/lib_band_check.py <candidates.txt> <out.tsv> --band 0x004c0700-0x00558c50 [--band lo-hi ...]
  candidates.txt: one `rva<TAB>label` per line (label = the third-party-library[...] subsystem to assign).
"""
import bisect
import csv
import os
import re
import struct
import sys
from collections import defaultdict

import capstone
import pefile

pe = pefile.PE(next(x for x in ('original/MASHED.exe.unpatched', '../../original/MASHED.exe.unpatched')
                    if os.path.exists(x)))
base = pe.OPTIONAL_HEADER.ImageBase
img = pe.get_memory_mapped_image()
secs = [(s.Name.rstrip(b'\0').decode(), base + s.VirtualAddress, s.Misc_VirtualSize) for s in pe.sections]
text = [s for s in secs if s[0] == '.text'][0]
TB, TE = text[1], text[1] + text[2]
dsecs = [s for s in secs if s[0] != '.text']


def in_data(v):
    return any(va <= v < va + sz for _, va, sz in dsecs)


hooks = {}
for r in csv.DictReader(open('hooks.csv', encoding='utf-8')):
    if re.fullmatch(r'[0-9a-fA-F]{8}', r['rva'] or ''):
        v = int(r['rva'], 16)
        hooks[v + 0x400000 if v < 0x400000 else v] = r
starts = sorted(hooks)


def owner(va):
    i = bisect.bisect_right(starts, va) - 1
    return starts[i] if i >= 0 else None


def body(f):
    i = starts.index(f)
    return f, (starts[i + 1] if i + 1 < len(starts) else f + 0x40)


def is_lib_row(fn):
    r = hooks.get(fn)
    return bool(r) and (r['subsystem'].startswith('third-party-library') or r['name'].startswith('_')
                        or r['name'].startswith('FID_'))


WRITE1 = {'mov', 'movzx', 'movsx', 'add', 'sub', 'and', 'or', 'xor', 'adc', 'sbb', 'inc', 'dec', 'not', 'neg',
          'shl', 'shr', 'sar', 'rol', 'ror', 'xchg', 'fstp', 'fst', 'fistp', 'fist', 'fnstsw', 'fnstcw', 'setne',
          'sete', 'setg', 'setl', 'setge', 'setle', 'seta', 'setb', 'setae', 'setbe', 'movss', 'movsd', 'movd',
          'movq', 'movaps', 'movups', 'movdqa', 'movdqu', 'cmpxchg', 'bts', 'btr', 'btc'}
ADDR = re.compile(r'0x([0-9a-f]{6,8})\b')

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.skipdata = True
calls = []                       # (site, target)
refs = []                        # (site, addr, is_write)
for ins in md.disasm(img[TB - base:TE - base], TB):
    b = ins.bytes
    if ins.mnemonic in ('call', 'jmp') and b[0] in (0xE8, 0xE9) and len(b) == 5:
        calls.append((ins.address, (ins.address + 5 + struct.unpack('<i', b[1:])[0]) & 0xffffffff))
        continue
    ops = ins.op_str
    first = ops.split(',')[0]
    for m in ADDR.finditer(ops):
        v = int(m.group(1), 16)
        if not in_data(v):
            continue
        w = ('ptr [' in first and m.start() < len(first) and ins.mnemonic in WRITE1
             and not ins.mnemonic.startswith('movzx') and not ins.mnemonic.startswith('movsx'))
        refs.append((ins.address, v, w))

writers = defaultdict(set)       # addr -> owners of writing sites
for site, v, w in refs:
    if w:
        writers[v].add(owner(site))


BANDS = []


def in_band(fn):
    return fn is not None and any(lo <= fn < hi for lo, hi in BANDS)


def main():
    args = sys.argv[1:]
    while '--band' in args:
        i = args.index('--band')
        lo, hi = args[i + 1].split('-')
        BANDS.append((int(lo, 16), int(hi, 16)))
        del args[i:i + 2]
    sys.argv[1:] = args
    cand = {}
    for line in open(sys.argv[1], encoding='utf-8'):
        if line.strip():
            rva, label = line.rstrip('\n').split('\t')[:2]
            v = int(rva, 16)
            cand[v + 0x400000 if v < 0x400000 else v] = label
    passing = set(cand)
    reason = {}
    while True:
        lib = lambda fn: is_lib_row(fn) or in_band(fn) or fn in passing
        failed = set()
        for f in passing:
            lo, hi = body(f)
            gc = sorted({t for s, t in calls if lo <= s < hi and not (lo <= t < hi) and not lib(owner(t))})
            game_data = []
            for s, v, w in refs:
                if lo <= s < hi:
                    fpw = sorted(o for o in writers.get(v, ()) if o is not None and not lib(o))
                    if fpw:
                        game_data.append((v, fpw[:3]))
            if gc or game_data:
                failed.add(f)
                reason[f] = (gc, sorted(set((hex(v), tuple(hex(x) for x in o)) for v, o in game_data)))
        if not failed:
            break
        passing -= failed
    w = csv.writer(open(sys.argv[2], 'w', newline='', encoding='utf-8'), delimiter='\t', lineterminator='\n')
    w.writerow(['rva', 'name', 'subsystem', 'label', 'fits', 'first_party_callees', 'game_owned_data(addr: first-party writers)'])
    for f in sorted(cand):
        r = hooks[f]
        gc, gd = reason.get(f, ([], []))
        w.writerow([f'{f:08x}', r['name'], r['subsystem'], cand[f], 'yes' if f in passing else 'NO',
                    ' '.join(f'{t:#010x}:{hooks[owner(t)]["name"] if owner(t) in hooks else "?"}' for t in gc),
                    ' '.join(f'{a}:{"/".join(o)}' for a, o in gd)])
    print(f'{len(passing)} of {len(cand)} fit; {len(cand) - len(passing)} fail')


if __name__ == '__main__':
    main()
