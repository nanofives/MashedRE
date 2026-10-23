# audiolink slice 4 tracker transaction (re-classify): exe_file for 3 RVAs (+ file move 0x005adf30), notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {'005adf30': 'AudioFmtKey', '005ac5f0': 'RwsFmt', '005ac9e0': 'RwsFmt'}
MOVE = {'005adf30'}   # file column moves from Audio/AudioRws.cpp
R = 'verify/audiolink_20261023/RESULT_slice4.md'
LINK = (f' | audiolink slice 4 2026-10-23: linked into the exe (exe_sources.rsp), one body in both targets, exe dead code; '
        f'the key compare 0x005adf30 lives in Audio/AudioFmtKey.cpp (both rsp) and RwsFmt calls it by name; path1 re-run '
        f'GREEN; gates unchanged exe 4004ebb1. {R}')
EXTRA = {'005adf30': ' Moved from Audio/AudioRws.cpp. Bytes: repe cmpsb faults on the param_2 byte first (measured, two unmapped key pointers); the port now loads param_2 before param_1 (volatile), which turned audio_fmt_desc_equal idx 2 GREEN (the first run was RED 9/10).'}

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
line = ('2026-10-23  005adf30 005ac5f0 005ac9e0  AudioFmtKeyCompare split + RwsFmt (audiolink slice 4)  levels kept  '
        'FILE MOVE 0x005adf30 Audio/AudioRws.cpp -> Audio/AudioFmtKey.cpp (both rsp); RwsFmt linked into the exe, key '
        'compare by name. First run RED desc_equal idx 2 (cmpsb faults on param_2 first, the port on param_1); load order '
        'fixed, path1 3/3 GREEN, gates unchanged exe 4004ebb1, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
