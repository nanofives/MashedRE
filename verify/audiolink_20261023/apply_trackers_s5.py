# audiolink slice 5 tracker transaction (re-classify): file move + exe_file for 4 RVAs, notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {'005a89a0': 'AudioFieldGetters', '005a89b0': 'AudioFieldGetters', '005a89c0': 'AudioFieldGetters',
         '005ab370': 'AudioFieldGetters'}
MOVE = set(FILES)   # file column moves from Audio/TimerGetters_ah5.cpp
R = 'verify/audiolink_20261023/RESULT_slice5.md'
LINK = (f' | audiolink slice 5 2026-10-23: moved from Audio/TimerGetters_ah5.cpp to Audio/AudioFieldGetters.cpp (both rsp), '
        f'linked into the exe, one body, exe dead code; bytes identical, path1 re-run GREEN 12/12; gates unchanged exe 439495bc. {R}')
EXTRA = {}

p = W / 'hooks.csv'; raw = p.read_bytes(); nl = b'\r\n' if b'\r\n' in raw else b'\n'
lines = raw.split(nl); hdr = next(csv.reader([lines[0].decode('utf-8')])); ix = {k: i for i, k in enumerate(hdr)}
seen = set()
for n, b in enumerate(lines[1:], 1):
    if not b: continue
    s = b.decode('utf-8'); rva = s.split(',', 1)[0].lower()
    if rva not in FILES: continue
    row = next(csv.reader([s])); row += [''] * (len(hdr) - len(row))
    f = A + FILES[rva] + '.cpp'
    if rva in MOVE: row[ix['file']] = f
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
line = ('2026-10-23  005a89a0 005a89b0 005a89c0 005ab370  field getters (audiolink slice 5)  levels kept  FILE MOVE '
        'Audio/TimerGetters_ah5.cpp -> Audio/AudioFieldGetters.cpp (both rsp), linked into the exe, exe dead code. Bytes '
        'identical, path1 4/4 GREEN, gates unchanged exe 439495bc, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
