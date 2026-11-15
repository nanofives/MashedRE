"""AUDIO-STACK-5-UNMATCHED-BOUND: append 5 PROVISIONAL.tsv rows and correct BINDINGS.tsv kind/note (run from the worktree)."""
import os, sys
os.chdir(sys.argv[1])
RES = 'verify/audio_20261114/RESULT_AUDIO-STACK-5-UNMATCHED-BOUND.md'
EQ = 'equiv.py @ fea5b5483 (no diff to orch/integration), full line verify/audio_20261114/equiv_%s.txt'
RP = 'IDENTICAL-FOR-2724 (%s, verify/provinsitu_20261111/PROV_VERDICTS.tsv; source unchanged since bf44aae7d)'
rows = [
    ('0045cd30', 're/match/s10/0045cd30.c', '/O2 /GS /Ob1', '62.8',
     'branch-condition: region ours +0x146 / orig 0x0045ce7d: sle(idivq32(...)+3 calls 0x4b44f0, ...) | slt(..., idivq32(...)) (' + EQ % '0045cd30' + ')',
     RP % 'run t50_191034_008_2',
     'equiv stops at the branch at orig 0x0045ce7d (verdict line); cause not diagnosed [UNCERTAIN]. Bound since PORT-TOP-AUDIOTICK-STOPS as c4=frida-replay; match.py 62.8% relocheck BAD (L0 run 46). Unit keeps the DDK object: 0x0045c820 in the same source is IDENTICAL'),
    ('004647f0', 're/match/s10/004647f0.c', '/O2 /GS /Ob1', '83.3',
     'call-args: region ours +0x91 / orig 0x00464881 call 0x45e0f0 arg 1: x87 operand order of the fmul differs (fld m4 / fmul (x - 0x3ccccccd) vs fld (x - 0x3ccccccd) / fmul m4) (' + EQ % '004647f0' + ')',
     RP % 'run t80_171707_008_1',
     'x87 operand order of one fmul (never admitted, L-126). match.py 83.3% relocheck BAD (L0 run 46)'),
    ('005a6d60', 're/match/s25/005a6d60.c', '/O2 /Ob1 /G5', '50.0',
     'call-regargs: region ours +0x0 / orig 0x005a6d60 indirect call through the 8-byte table entry: ours ecx=*arg4, edx=zx16(entry+4) | orig ecx=entry address, edx=*arg4 (' + EQ % '005a6d60' + ')',
     RP % 'run t50_191034_025_2',
     'ECX/EDX at the indirect call differ (verdict line); --relax-icall not tried (would not admit, L-126). match.py 50.0% relocheck OK (L0 run 46)'),
    ('005aab70', 're/match/s25/005aab70.c', '/O2 /Ob1 /G5', '75.6',
     "branch-condition: region ours +0x55 / orig 0x005aab92: P1('eq',(arg32(4)+0xc),ld32(M00(),(arg32(4)+0xc))) | P1('ne',0x0,arg32(4)) (" + EQ % '005aab70' + ')',
     RP % 'run t50_191034_036_2',
     'branch conditions differ at orig 0x005aab92 (ours: arg+0xc == *(arg+0xc), orig: arg != 0, verdict line); source cause not diagnosed [UNCERTAIN]. match.py 75.6% relocheck OK (L0 run 46)'),
    ('005acda0', 're/match/s26/005acda0.c', '/O2 /Op /Ob1 /G5', '44.3',
     'call-sequence: region ours +0x4e / orig 0x005acdea: 1 vs 0 calls (' + EQ % '005acda0' + ')',
     RP % 'run t0_195814_016_1',
     'the DDK calls _ftol where the original truncates inline (fnstcw / or ah,0xc / fldcw / fistp), source header class (a). match.py 44.3% relocheck BAD (L0 run 46)'),
]
p = 're/match/PROVISIONAL.tsv'
raw = open(p, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
txt = raw.decode()
have = {l.split('\t')[0].lower() for l in txt.split(nl)}
add = []
for rva, src, fl, pct, eq, rp, diag in rows:
    if '0x' + rva in have:
        continue
    add.append('\t'.join(['0x' + rva, 'FUN_' + rva, src, fl, pct, 'NOT-PROVEN', eq, rp, diag, 'not run', 'provisional', RES]))
if not txt.endswith(nl): txt += nl
txt += nl.join(add) + (nl if add else '')
open(p, 'wb').write(txt.encode())
print('PROVISIONAL rows added', len(add))
# BINDINGS.tsv
TAG = 'AUDIO-STACK-5-UNMATCHED-BOUND: '
PROVN = 'L-126 PROVISIONAL body, NOT matched (re/match/PROVISIONAL.tsv, port axis only, never C4 by this status; %s): %s, Match/PortTop_%s.cpp'
fix = {r[0]: ('near-match', PROVN % ('hooks.csv C4 by c4=frida-replay is the RE axis, equiv NOT-PROVEN, ' + RES, r[1], r[0])) for r in rows}
for rva, src in (('005a6ea0', 're/match/s25/005a6ea0.c'), ('005a6f30', 're/match/s25/005a6f30.c'), ('005aa0c0', 're/match/s25/005aa0c0.c'),
                 ('005a5e70', 're/match/s30/005a5e70.c'), ('005ae650', 're/match/s26/005ae650.c')):
    fix[rva] = ('near-match', PROVN % ('bound through PROV_BIND, ' + RES, src, rva))
fix['005ad320'] = ('matched', 'L-126 admitted, re/match/ADMITTED.tsv (EQUIV), not a byte match (match.py not 100%%; hooks.csv C4 by c4=frida-replay): re/match/s26/005ad320.c, Match/PortTop_005ad320.cpp; ' + RES)
fix['0042f530'] = ('near-match', 'L-126 PROVISIONAL body, NOT matched (re/match/PROVISIONAL.tsv, C2 source 96.9%%, port axis only, never C4 by this status; ' + RES + '): Match/PortTop_0042f530.cpp (gen_top04b_tus.py)')
p = 'verify/porttop_20261108/BINDINGS.tsv'
raw = open(p, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
lines = raw.decode().split(nl)
n = 0
for i, l in enumerate(lines):
    f = l.split('\t')
    k = f[0][2:].lower() if f[0].startswith('0x') else None
    if k in fix and len(f) >= 5:
        kind, note = fix[k]
        if k != '0042f530' and k != '005ad320':
            note = TAG + note.replace('%%', '%')
        else:
            note = TAG + note.replace('%%', '%')
        if f[2] != kind or f[4] != note:
            f[2], f[4] = kind, note
            lines[i] = '\t'.join(f); n += 1
open(p, 'wb').write(nl.join(lines).encode())
print('BINDINGS rows changed', n)
