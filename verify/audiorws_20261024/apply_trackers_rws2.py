# D4-AUDIO-RWS slice 2 tracker transaction (re-classify): file move + exe_file for 7 RVAs, notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
RVAS = ('005ae650', '005ae800', '005ae780', '005ae920', '005addd0', '005ade10', '005ade90')
FILES = {r: 'AudioPool' for r in RVAS}
MOVE = set(FILES)   # from Audio/AudioRws.cpp (two file columns were analysis notes)
R = 'verify/audiorws_20261024/RESULT_D4-AUDIO-RWS.md'
LINK = (f' | D4-AUDIO-RWS slice 2 2026-10-24: moved to Audio/AudioPool.cpp (both rsp), linked into the exe on private '
        f'zero storage (the pre-open BSS state) and the CRT heap, exe dead code; path1 re-run GREEN; gates unchanged. {R}')
EXTRA = {'005addd0': ' Re-reads *param_1 after the node stores (0x005addf9) and returns the node the bytes leave in EAX '
                     '(was void; static evidence, the harness does not read it).',
         '005ade90': ' Returns param_1[1] (== param_1 at exit), the EAX of the bytes (was void; static evidence).'}

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
line = ('2026-10-24  005ae650 005ae800 005ae780 005ae920 005addd0 005ade10 005ade90  AudioRws pool + list family '
        '(D4-AUDIO-RWS slice 2)  levels kept  FILE MOVE Audio/AudioRws.cpp -> Audio/AudioPool.cpp (both rsp), linked into '
        'the exe on private zero storage + CRT heap; 0x005addd0 re-reads the head and returns the node, 0x005ade90 returns '
        'its EAX. path1 7/7 GREEN, gates unchanged, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
