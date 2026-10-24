# audiolink slice 1 tracker transaction (re-classify): exe_file for 29 RVAs, file fix for 0x005aefa0, notes for the
# fixed bodies, U-9789, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {
 '005b1160': 'RingHeaderInit', '005aefa0': 'AtomicExchangeStore', '005b4060': 'AudioLeaves_ab2', '005af200': 'AudioChainWalk',
 '005b10a0': 'AudioTableDispatch', '005b10e0': 'AudioTableDispatch', '005b3580': 'AudioList_ag', '005b35a0': 'AudioList_ag',
 '005b3670': 'AudioList_ag', '005b36b0': 'AudioList_ag', '005ac540': 'AudioLeaves_ab1', '005ade60': 'AudioLeaves_ab1',
 '005aded0': 'AudioLeaves_ab1', '005ae590': 'AudioLeaves_ab1', '005aeda0': 'AudioLeaves_ab1', '005b0700': 'AudioLeaves_ab1',
 '005b73b0': 'AudioLeaves_ab3', '005b9410': 'AudioLeaves_ab3', '005baf40': 'AudioLeaves_ab3', '005bb5b0': 'AudioLeaves_ab3',
 '005bc450': 'AudioLeaves_ab3', '005bcb80': 'AudioLeaves_ab3', '005bf660': 'AudioLeaves_ab4', '005bfcc0': 'AudioLeaves_ab4',
 '005c7500': 'AudioLeaves_ab4', '005c75b0': 'AudioLeaves_ab4', '005c9380': 'AudioLeaves_ab4', '005c9770': 'AudioLeaves_ab4',
 '005baf00': 'AudioMusic'}
R = 'verify/audiolink_20261023/RESULT_slice1.md'
LINK = f' | audiolink slice 1 2026-10-23: linked into the exe (exe_sources.rsp), one body in both targets, exe dead code; path1 re-run GREEN on the shared body; gates unchanged exe 71587212. {R}'
EXTRA = {
 '005b4060': ' FIXED from the bytes: both fistp truncate (or ah,0xc 0x005b40e7 / 0x005b4125, Ghidra ROUND() was wrong) and the product order is (ch*bits)*rate*size (0x005b4087..0x005b40d2). Old body RED on 2 new vectors (1092 vs 1090, 2 vs 1); fixed body 15/15. U-9789 (rate or size >= 2^24).',
 '005baf00': ' FIXED: the FLD/FSTP m32 copies quiet a signaling NaN (0x005baf04/0e, 0x005baf16/1f); early_window_leaf_diff now seeds node+0x1c from a bit table, old body RED t=1/4/6, fixed 7/7. struct+0x38 SNaN not covered (JS float arg).',
 '005b10a0': ' Now forwards the target EDX:EAX (nothing touches EAX/EDX after call [ecx] 0x005b10c0); was void.',
 '005b10e0': ' Now forwards the target EDX:EAX (nothing touches EAX/EDX after call [ecx] 0x005b1100); was void.',
 '005ae590': ' Returns int 0/1 (sete cl / mov eax,ecx 0x005ae5a2..0x005ae5a5); was bool.',
 '005bb5b0': ' Byte order kept: 8 loads before the 4 stores per block (0x005bb609..0x005bb62d); was per-sample (differs only on offset-overlapping buffers, not covered).',
 '005bfcc0': ' Byte order kept: out1 stored, then +0x15c/+0x160 re-read (0x005bfcd6/dc); differs only on aliasing, not covered.',
 '005aefa0': ' file column fixed (was the analysis note).'}

p = W / 'hooks.csv'; raw = p.read_bytes(); nl = b'\r\n' if b'\r\n' in raw else b'\n'
lines = raw.split(nl); hdr = next(csv.reader([lines[0].decode('utf-8')])); ix = {k: i for i, k in enumerate(hdr)}
seen = set()
for n, b in enumerate(lines[1:], 1):
    if not b: continue
    s = b.decode('utf-8'); rva = s.split(',', 1)[0].lower()
    if rva not in FILES: continue
    row = next(csv.reader([s])); row += [''] * (len(hdr) - len(row))
    f = A + FILES[rva] + '.cpp'
    if rva == '005aefa0': row[ix['file']] = f
    assert row[ix['file']] == f, (rva, row[ix['file']])
    assert row[ix['confidence']] in ('C3', 'C4'), (rva, row[ix['confidence']])
    if LINK in row[ix['notes']]: seen.add(rva); continue      # idempotent
    row[ix['exe_file']] = f
    row[ix['notes']] = row[ix['notes']] + LINK + EXTRA.get(rva, '')
    o = io.StringIO(); csv.writer(o, lineterminator='').writerow(row); lines[n] = o.getvalue().encode('utf-8'); seen.add(rva)
miss = set(FILES) - seen
if miss: sys.exit(f'missing rows: {sorted(miss)}')
p.write_bytes(nl.join(lines))

u = W / 'UNCERTAINTIES.md'; t = open(u, encoding='utf-8', newline='').read()
if 'U-9789' not in t:
    anchor = '| U-9788 |'; i = t.index(anchor)
    row = ('| U-9789 | precision | `0x005b4060` AudioFmtConvertByteLength (shared `Audio/AudioLeaves_ab2.cpp`) | The port does each '
           'x87 step in double from exact integer operands and rounds to float, which equals one PC_24 rounding while every '
           'operand fits 24 bits (53 >= 2*24+2). A sample rate or data size of 2^24 or more is not an exact float, and for '
           'the fimul by size the product can need 55 bits, so the argument does not hold there. | a path1 vector with a rate '
           'or size >= 2^24 and a non-integer product | add such vectors to `audio_fmt_convert_byte_length` (the 16777216 '
           'vector is an integer product) and compare; if RED, carry the size as an exact x87 step | nothing (exe dead code; '
           'C3 stands on 15/15) |\n')
    t = t[:i] + row + t[i:]; open(u, 'w', encoding='utf-8', newline='').write(t)

c = W / 're/analysis/CHANGELOG.md'; t = open(c, encoding='utf-8', newline='').read()
line = ('2026-10-23  005b1160 005aefa0 005b4060 005af200 005b10a0 005b10e0 005b3580 005b35a0 005b3670 005b36b0 005ac540 '
        '005ade60 005aded0 005ae590 005aeda0 005b0700 005b73b0 005b9410 005baf40 005bb5b0 005bc450 005bcb80 005bf660 '
        '005bfcc0 005c7500 005c75b0 005c9380 005c9770 005baf00  audiolink slice 1 (10 Audio files)  levels kept  linked into '
        'the exe, one shared body, exe dead code. Bytes vs port: 0x005b4060 truncates (was round-to-nearest) + product '
        'order, 0x005baf00 SNaN quieting; 0x005b10a0/e0 u64 pass-through, 0x005ae590 int, 0x005bb5b0/0x005bfcc0 byte order. '
        'Controls RED on the old bodies, path1 29/29 GREEN, gates unchanged exe 71587212, lint 0 NEW. U-9789. ' + R + '\n')
if line not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
