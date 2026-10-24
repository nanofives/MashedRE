# audiolink AudioRws slice 1 tracker transaction (re-classify): file move + exe_file for 7 RVAs, notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {r: 'AudioRwsLeaves' for r in ('005aca80', '005ac980', '005ae0b0', '005aeca0', '005ae0c0', '005be190', '005be140')}
MOVE = set(FILES)   # from Audio/AudioRws.cpp (three file columns were analysis notes)
R = 'verify/audiolink_20261023/RESULT_rws1.md'
LINK = (f' | audiolink AudioRws slice 1 2026-10-23: moved to Audio/AudioRwsLeaves.cpp (both rsp), linked into the exe, one '
        f'body, exe dead code; bytes match, path1 re-run GREEN 10/10; gates unchanged exe ed93b4fd. {R}')
EXTRA = {r: ' Now returns the pointer the bytes leave in EAX (was void; static evidence, the harness does not read it).'
         for r in ('005ac980', '005ae0b0', '005be190', '005be140')}

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
line = ('2026-10-23  005aca80 005ac980 005ae0b0 005aeca0 005ae0c0 005be190 005be140  AudioRws leaves (audiolink AudioRws slice 1)  '
        'levels kept  FILE MOVE Audio/AudioRws.cpp -> Audio/AudioRwsLeaves.cpp (both rsp), linked into the exe; four void ports '
        'return the EAX pointer of the bytes. path1 7/7 GREEN, gates unchanged exe ed93b4fd, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
