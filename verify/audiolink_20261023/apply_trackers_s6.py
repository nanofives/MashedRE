# audiolink slice 6 tracker transaction (re-classify): exe_file for 7 RVAs, file move 0x005a9e10, U-0360 / U-0362, notes, one CHANGELOG line. Line-level edits only (other hooks.csv rows are left byte-identical).
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
A = 'mashedmod/src/mashed_re/Audio/'
FILES = {'005baf60': 'AudioDSound', '005baf90': 'AudioDSound', '005bc400': 'AudioDSound', '005aeea0': 'AudioDSound',
         '005bbfc0': 'AudioDSound', '005aef00': 'AudioDSound', '005a9e40': 'AudioDSound', '005a9e10': 'AudioRws'}
MOVE = {'005a9e10', '005a9e40'}   # 005a9e10 moves from AudioDSound.cpp; 005a9e40 file column was an analysis note (drift)
ASI_ONLY = {'005a9e10'}   # no exe_file
R = 'verify/audiolink_20261023/RESULT_slice6.md'
LINK = (f' | audiolink slice 6 2026-10-23: linked into the exe (exe_sources.rsp), one body in both targets, exe dead code; '
        f'GUIDs as image-byte constants; path1 re-run GREEN; gates unchanged exe 36811066. {R}')
LINK_ASI = f' | audiolink slice 6 2026-10-23: moved from Audio/AudioDSound.cpp to Audio/AudioRws.cpp (asi-only: it calls LinkBuffer / LinkDevice of the pool family). {R}'
EXTRA = {'005bbfc0': ' FIXED from the bytes: a failed QI returns 0 with no Release (jl 0x005bc014 at 0x005bbfe0) and the failure Release is on the QI output ks ([esp] at 0x005bc00a); the port released pUnk on both. Harness gained modes (QI fail / QuerySupport fail / non-3) and per-object Release counters: old body RED 7/10, fixed 10/10. U-0362 refuted (the ==3 test reads the QuerySupport support word).',
         '005bc400': ' U-0360 resolved: 0x005d09dc = IID_IDirectSound3DListener, 0x005d09bc = IID_IKsPropertySet (byte-equal to dsound.h:861 / :1212).',
         '005aeea0': ' Bytes: neg / sbb full 32-bit mask (comment corrected).',
         '005a9e40': ' file column fixed (was the analysis note).'}

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
    if LINK in row[ix['notes']] or LINK_ASI in row[ix['notes']]: seen.add(rva); continue      # idempotent
    if rva in ASI_ONLY:
        row[ix['notes']] = row[ix['notes']] + LINK_ASI
    else:
        row[ix['exe_file']] = f
        row[ix['notes']] = row[ix['notes']] + LINK + EXTRA.get(rva, '')
    o = io.StringIO(); csv.writer(o, lineterminator='').writerow(row); lines[n] = o.getvalue().encode('utf-8'); seen.add(rva)
miss = set(FILES) - seen
if miss: sys.exit(f'missing rows: {sorted(miss)}')
p.write_bytes(nl.join(lines))

u = W / 'UNCERTAINTIES.md'; t = open(u, encoding='utf-8', newline='').read()
for uid, old_t, new_t in (
    ('U-0360', '| U-0360 | structural |', '| U-0360 | ~~structural~~ **RESOLVED 2026-10-23** (audiolink slice 6: the 16 bytes at 0x005d09dc are IID_IDirectSound3DListener, byte-equal to Windows SDK dsound.h:861; the second QI target 0x005d09bc is IID_IKsPropertySet, dsound.h:1212; ' + R + ') |'),
    ('U-0362', '| U-0362 | structural |', '| U-0362 | ~~structural~~ **RESOLVED 2026-10-23** (audiolink slice 6: the branch is reachable. QuerySupport (IKsPropertySet slot 5) writes its support word through the 4th argument, the param_1 slot of the caller [esp+8] (0x005bbfe6), and 0x005bbffc compares that word with 3; ' + R + ') |')):
    if old_t in t: t = t.replace(old_t, new_t, 1)
    else: assert ('| ' + uid + ' | ~~structural~~ **RESOLVED 2026-10-23**') in t, uid
open(u, 'w', encoding='utf-8', newline='').write(t)

c = W / 're/analysis/CHANGELOG.md'; t = open(c, encoding='utf-8', newline='').read()
line = ('2026-10-23  005baf60 005baf90 005bc400 005aeea0 005bbfc0 005aef00 005a9e40 005a9e10  AudioDSound (audiolink slice 6)  '
        'levels kept  linked into the exe (7), GUIDs from the image bytes; 0x005bbfc0 two Release defects fixed (control RED 7/10, '
        'fixed 10/10); FILE MOVE 0x005a9e10 -> Audio/AudioRws.cpp (asi-only). U-0360 resolved, U-0362 refuted. path1 7/7 '
        'GREEN, gates unchanged exe 36811066, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
