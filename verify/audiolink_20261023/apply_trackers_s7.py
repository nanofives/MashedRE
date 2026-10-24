# audiolink slice 7 tracker transaction (re-classify): exe_file / notes for 3 RVAs, notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {'005ad2e0': 'AudioQueuePop', '005b0f40': 'AudioQueuePop', '005aed20': 'AudioVecLength'}
MOVE = {'005ad2e0', '005b0f40'}   # both file columns were analysis notes; the bodies live in AudioQueuePop.cpp
R = 'verify/audiolink_20261023/RESULT_slice7.md'
LINK = (f' | audiolink slice 7 2026-10-23: no MASHED.exe address in the exe arm; gates unchanged exe d50a46de. {R}')
EXTRA = {'005ad2e0': ' Ported from the bytes as AudioQueueConsume (exported, NOT installed): count +0xc != 0 -> dec, return 0; else unlink (prev [+0x18] <- next [+0x14], both re-read, next+4 <- prev), free unless byte +0x10 bit 0 (0x004522d0 in the .asi, CRT free in the exe), return 1. path1 audio_queue_consume 10/10 GREEN (new handler queue_node_consume; free path not covered). Level kept C2 (no promotion requested).',
         '005b0f40': ' Calls AudioQueueConsume by name in both arms. path1 audio_queue_pop_fixture 10/10 GREEN (obj+0x28 = node). The live-capture entry audio_queue_pop never captured args (all 0, both fault at 0x28). Level kept C2.',
         '005aed20': ' Exe arm reads private copies of 0x005d757c (0.0f) and the 256 x s16 table 0x00633b48 (file-backed, equal to the runtime bytes at the menu); .asi arm unchanged. path1 12/12 GREEN. Level kept C2.'}

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
    assert row[ix['confidence']] in ('C2', 'C3', 'C4'), (rva, row[ix['confidence']])   # levels are not moved
    if LINK in row[ix['notes']]: seen.add(rva); continue      # idempotent
    row[ix['exe_file']] = f
    row[ix['notes']] = row[ix['notes']] + LINK + EXTRA.get(rva, '')
    o = io.StringIO(); csv.writer(o, lineterminator='').writerow(row); lines[n] = o.getvalue().encode('utf-8'); seen.add(rva)
miss = set(FILES) - seen
if miss: sys.exit(f'missing rows: {sorted(miss)}')
p.write_bytes(nl.join(lines))

c = W / 're/analysis/CHANGELOG.md'; t = open(c, encoding='utf-8', newline='').read()
line = ('2026-10-23  005ad2e0 005b0f40 005aed20  AudioQueuePop / AudioVecLength precedents (audiolink slice 7)  levels kept '
        '(C2)  no MASHED.exe address in either exe arm: 0x005ad2e0 ported as AudioQueueConsume (path1 10/10), AudioQueuePop '
        'fixture 10/10, AudioVecLength exe arm on private copies of 0x005d757c / 0x00633b48 (runtime bytes equal), path1 12/12; '
        'gates unchanged exe d50a46de, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
