# D4-AUDIO-RWS3 slice 3 tracker transaction (re-classify): exe_file for 13 RVAs (+ 0x005ad540's file column), notes,
# one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
F = 'mashedmod/src/mashed_re/Audio/AudioRws.cpp'
RVAS = ('005acd10', '005acd60', '005ae080', '005ae050', '005ae030', '005a9e10', '005ae010', '005adfe0', '005ac7b0',
        '005abcb0', '005ac740', '005ac900', '005ad540')
MOVE = {'005ad540'}   # its file column was the analysis note; the body has been in AudioRws.cpp since wf_b0f68acd r42
R = 'verify/audiorws3_20261024/RESULT_D4-AUDIO-RWS3.md'
LINK = (f' | D4-AUDIO-RWS3 slice 3 2026-10-24: Audio/AudioRws.cpp linked into the exe (both rsp), callees by name, '
        f'private zero pools / CRT free / private 0x00633674 key table in the exe arm, exe dead code; path1 re-run GREEN; '
        f'gates unchanged. {R}')
EXTRA = {'005ac900': ' Exe arm passes a byte-port of LAB_005ac930 (U-1730, static evidence only, no row); .asi arm passes '
                     'the original 0x005ac930.'}

p = W / 'hooks.csv'; raw = p.read_bytes(); nl = b'\r\n' if b'\r\n' in raw else b'\n'
lines = raw.split(nl); hdr = next(csv.reader([lines[0].decode('utf-8')])); ix = {k: i for i, k in enumerate(hdr)}
seen = set()
for n, b in enumerate(lines[1:], 1):
    if not b: continue
    s = b.decode('utf-8'); rva = s.split(',', 1)[0].lower()
    if rva not in RVAS: continue
    row = next(csv.reader([s])); row += [''] * (len(hdr) - len(row))
    if rva in MOVE: row[ix['file']] = F
    assert row[ix['file']] == F, (rva, row[ix['file']])
    assert row[ix['confidence']] in ('C3', 'C4'), (rva, row[ix['confidence']])
    if LINK in row[ix['notes']]: seen.add(rva); continue      # idempotent
    row[ix['exe_file']] = F
    row[ix['notes']] = row[ix['notes']] + LINK + EXTRA.get(rva, '')
    o = io.StringIO(); csv.writer(o, lineterminator='').writerow(row); lines[n] = o.getvalue().encode('utf-8'); seen.add(rva)
miss = set(RVAS) - seen
if miss: sys.exit(f'missing rows: {sorted(miss)}')
p.write_bytes(nl.join(lines))

c = W / 're/analysis/CHANGELOG.md'; t = open(c, encoding='utf-8', newline='').read()
line = ('2026-10-24  ' + ' '.join(RVAS) + '  AudioRws.cpp last 13 installs (D4-AUDIO-RWS3 slice 3)  levels kept  '
        'Audio/AudioRws.cpp linked into the exe (both rsp), callees by name, exe arm on private zero pools 0x007dda28 / '
        '0x007dd634, CRT free, a private copy of the 0x00633674 key table, and a byte-port of LAB_005ac930 (U-1730, '
        'static only); 0x005ad540 file column -> the .cpp. path1 13/13 GREEN, gates unchanged, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    eol = '\r\n' if '<!-- ENTRIES -->\r\n' in t else '\n'   # the checkout may hold CRLF
    m = '<!-- ENTRIES -->' + eol; i = t.index(m) + len(m); t = t[:i] + line.replace('\n', eol) + t[i:]
    open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
