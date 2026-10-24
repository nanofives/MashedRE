# audiolink slice 3 tracker transaction (re-classify): exe_file for 4 RVAs, notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {'005ab380': 'RwsStream', '005ab410': 'RwsStream', '005abf80': 'RwsStream', '005aa0c0': 'RwsStream'}
R = 'verify/audiolink_20261023/RESULT_slice3.md'
LINK = (f' | audiolink slice 3 2026-10-23: linked into the exe (exe_sources.rsp), one body in both targets, exe dead code; '
        f'stream read / skip = the shared Render/RwStreamCore bodies by name in both arms, tree root DAT_009146fc private in '
        f'the exe; path1 re-run GREEN on the negctl fixtures; gates unchanged exe e41769c4. {R}')
EXTRA = {'005ab380': ' Bytes: two arguments (nothing reads [esp+0xc]); the third dword callers push is ignored.',
         '005abf80': ' Bytes: jmp eax at 0x005abf8f is a tail call (slot gets param_1, its EAX is returned, NULL returns 0). Port now uint32 fn(param_1); the old void fn() matched only through MSVC emitting a jmp.'}

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
line = ('2026-10-23  005ab380 005ab410 005abf80 005aa0c0  RwsStream (audiolink slice 3)  levels kept  linked into '
        'the exe, one shared body, exe dead code; stream read / skip on the shared RwStreamCore bodies, private tree root '
        'in the exe. 0x005ab380 takes two arguments; 0x005abf80 tail call made explicit. path1 4/4 GREEN, gates unchanged '
        'exe e41769c4, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
