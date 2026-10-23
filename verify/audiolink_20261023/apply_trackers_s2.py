# audiolink slice 2 tracker transaction (re-classify): exe_file for 5 RVAs, notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {'005aea10': 'AudioMemory', '005aea40': 'AudioMemory', '005aec00': 'AudioMemory', '005aee20': 'AudioMemory',
         '005aec30': 'AudioMemory'}
R = 'verify/audiolink_20261023/RESULT_slice2.md'
LINK = (f' | audiolink slice 2 2026-10-23: linked into the exe (exe_sources.rsp), one body in both targets, exe dead code; '
        f'raw alloc / free behind AudioRawAlloc / AudioRawFree (exe: CRT malloc / free, tag dropped; .asi: 0x005aea00 / '
        f'0x004522d0); bytes match, path1 re-run GREEN; gates unchanged exe b2f7fba6. {R}')
EXTRA = {'005aee20': ' Bytes: the result is AL at the ret 0x005aee3a (xor al,al 0x005aee24 / inc al 0x005aee34), the question U-0352 left open.',
         '005aea40': ' Bytes: rewrites its argument slot and tail-jumps (0x005aea47 / 0x005aea4b); the 7 direct call sites pop the slot and do not read EAX.'}

p = W / 'hooks.csv'; raw = p.read_bytes(); nl = b'\r\n' if b'\r\n' in raw else b'\n'
lines = raw.split(nl); hdr = next(csv.reader([lines[0].decode('utf-8')])); ix = {k: i for i, k in enumerate(hdr)}
seen = set()
for n, b in enumerate(lines[1:], 1):
    if not b: continue
    s = b.decode('utf-8'); rva = s.split(',', 1)[0].lower()
    if rva not in FILES: continue
    row = next(csv.reader([s])); row += [''] * (len(hdr) - len(row))
    f = A + FILES[rva] + '.cpp'
    assert row[ix['file']] == f, (rva, row[ix['file']])
    assert row[ix['confidence']] in ('C3', 'C4'), (rva, row[ix['confidence']])
    if LINK in row[ix['notes']]: seen.add(rva); continue      # idempotent
    row[ix['exe_file']] = f
    row[ix['notes']] = row[ix['notes']] + LINK + EXTRA.get(rva, '')
    o = io.StringIO(); csv.writer(o, lineterminator='').writerow(row); lines[n] = o.getvalue().encode('utf-8'); seen.add(rva)
miss = set(FILES) - seen
if miss: sys.exit(f'missing rows: {sorted(miss)}')
p.write_bytes(nl.join(lines))

c = W / 're/analysis/CHANGELOG.md'; t = open(c, encoding='utf-8', newline='').read()
line = ('2026-10-23  005aea10 005aea40 005aec00 005aee20 005aec30  AudioMemory (audiolink slice 2)  levels kept  '
        'linked into the exe, one shared body, exe dead code; raw alloc / free = CRT malloc / free in the exe, the original '
        'trampolines in the .asi. Bytes match (no body change), path1 5/5 GREEN, gates unchanged exe b2f7fba6, lint 0 NEW. '
        + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
