"""RWC-THUNK-BIND-AUDIT: for every re/match source that names a `thunk_FUN_<t>` callee, read the original bytes of the
source's own MATCH function(s) in MASHED.exe.unpatched and report where each direct call lands.

A `thunk_FUN_<t>` name binds in gen_rwcore_tus.py to <t> (rva_of). If the original's call lands on a jmp stub whose
target is <t> (E9 rel32 -> t), the source must reach the stub instead (THUNK_BIND), else ddkobj check compares a
different call target.

usage: py -3.12 verify/rwcthunk_20261113/audit_thunk_bind.py   -> writes THUNK_AUDIT.tsv beside this file
Function extent: linear sweep from the MATCH address until a ret with no forward branch target beyond it (capstone).
"""
import glob
import os
import re
import struct
import sys

import capstone

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
ANCHOR = os.environ.get('MASHED_UNPATCHED', os.path.normpath(os.path.join(ROOT, '..', '..', 'original',
                                                                           'MASHED.exe.unpatched')))
D = open(ANCHOR, 'rb').read()
_pe = struct.unpack_from('<I', D, 0x3c)[0]
_n = struct.unpack_from('<H', D, _pe + 6)[0]
_o = struct.unpack_from('<H', D, _pe + 20)[0]
BASE = struct.unpack_from('<I', D, _pe + 24 + 28)[0]
SECS = []
for i in range(_n):
    s = _pe + 24 + _o + i * 40
    vsz, va, rsz, roff = struct.unpack_from('<IIII', D, s + 8)
    SECS.append((BASE + va, max(vsz, rsz), roff, rsz))


def rd(va, n):
    for lo, sz, off, rsz in SECS:
        if lo <= va < lo + sz:
            n = min(n, lo + sz - va)
            return D[off + va - lo: off + va - lo + n]
    return b''


MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = False


def func_calls(start):
    """direct call targets (E8) of the function at start, plus its end."""
    code = rd(start, 0x4000)
    calls, far, end = [], start, start
    for ins in MD.disasm(code, start):
        end = ins.address + ins.size
        m = ins.mnemonic
        if m == 'call' and ins.op_str.startswith('0x'):
            calls.append((ins.address, int(ins.op_str, 16)))
        if (m.startswith('j')) and ins.op_str.startswith('0x'):
            t = int(ins.op_str, 16)
            if t > far and t < start + 0x4000:
                far = t
        if m in ('ret', 'jmp') and ins.address >= far and (m == 'ret' or not ins.op_str.startswith('0x')
                                                           or int(ins.op_str, 16) < start or
                                                           int(ins.op_str, 16) >= end):
            if m == 'ret' and ins.address >= far:
                break
            if m == 'jmp' and ins.address >= far:
                break
    return calls, end


def stub_target(a):
    b = rd(a, 5)
    if len(b) == 5 and b[0] == 0xE9:
        return (a + 5 + struct.unpack('<i', b[1:])[0]) & 0xFFFFFFFF
    return None


MATCHLINE = re.compile(r'^// MATCH: 0x([0-9a-f]{8}) (\S+)', re.M)
THUNK = re.compile(r'\bthunk_FUN_([0-9a-f]{6,8})\b')
rows = []
for p in sorted(glob.glob(os.path.join(ROOT, 're', 'match', '**', '*.c'), recursive=True)):
    txt = open(p, encoding='utf-8', errors='replace').read()
    th = sorted({m.group(1).zfill(8) for m in THUNK.finditer(txt)})
    if not th:
        continue
    rel = os.path.relpath(p, ROOT).replace('\\', '/')
    owns = [int(m.group(1), 16) for m in MATCHLINE.finditer(txt)]
    sites = {}   # target t -> set of (own, call site, call target)
    for own in owns:
        calls, _ = func_calls(own)
        for site, tgt in calls:
            st = stub_target(tgt)
            if st is not None:
                sites.setdefault('%08x' % st, set()).add((own, site, tgt, 'stub'))
            else:
                sites.setdefault('%08x' % tgt, set()).add((own, site, tgt, 'direct'))
    for t in th:
        hit = sorted(sites.get(t, []))
        stubs = sorted({'%08x' % h[2] for h in hit if h[3] == 'stub'})
        direct = [h for h in hit if h[3] == 'direct']
        if stubs and not direct:
            verdict = 'STUB'      # original calls the stub only: THUNK_BIND needed
        elif stubs and direct:
            verdict = 'MIXED'
        elif direct:
            verdict = 'DIRECT'    # original calls <t> itself: default binding right
        else:
            verdict = 'NOCALL'    # no E8 call to t or a stub of t found in the own function(s)
        rows.append((rel, 'thunk_FUN_' + t, verdict, ','.join(stubs), len(hit),
                     ';'.join('%08x@%08x' % (h[0], h[1]) for h in hit)))

out = os.path.join(HERE, 'THUNK_AUDIT.tsv')
with open(out, 'w', encoding='utf-8', newline='') as f:
    f.write('source\tname\tverdict\tstub_rva\tn_sites\tsites(own@callsite)\n')
    for r in rows:
        f.write('\t'.join(str(x) for x in r) + '\n')
from collections import Counter
print(Counter(r[2] for r in rows), len(rows), 'rows')
print(out)
