# bankpath FLIP tracker transaction (L-3 = A) (re-classify): exe_file + one note for 0x004625b0, one CHANGELOG line. Level kept (C3).
# Line-level edit only (other hooks.csv rows are left byte-identical). Copy of verify/audiolink_20261023/apply_trackers_r1.py.
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
F = 'mashedmod/src/mashed_re/Audio/AudioCharacterBankPaths.cpp'
R = 'verify/bankpath_20261024/RESULT_bankpath.md'
EXE = sys.argv[1]
LINK = (f' | bankpath flip 2026-10-24 (L-3 = A): MASHED_BANKPATH_CORE removed, the lowercase AudioEngine body and '
        f'CharacterBankName deleted; CharacterBankPathCore is the only body, Audio::CharacterBankPath its thin wrapper; '
        f'path1 GREEN 5/5, gates unchanged exe {EXE}. {R}')

p = W / 'hooks.csv'; raw = p.read_bytes(); nl = b'\r\n' if b'\r\n' in raw else b'\n'
lines = raw.split(nl); hdr = next(csv.reader([lines[0].decode('utf-8')])); ix = {k: i for i, k in enumerate(hdr)}
seen = 0
for n, b in enumerate(lines[1:], 1):
    if not b: continue
    s = b.decode('utf-8')
    if s.split(',', 1)[0].lower() != '004625b0': continue
    row = next(csv.reader([s])); row += [''] * (len(hdr) - len(row))
    assert row[ix['file']] == F, row[ix['file']]
    assert row[ix['confidence']] == 'C3', row[ix['confidence']]
    seen += 1
    if LINK in row[ix['notes']]: continue                     # idempotent
    assert row[ix['exe_file']] == F, row[ix['exe_file']]
    row[ix['notes']] = row[ix['notes']] + LINK
    o = io.StringIO(); csv.writer(o, lineterminator='').writerow(row); lines[n] = o.getvalue().encode('utf-8')
if seen != 1: sys.exit(f'rows found: {seen}')
p.write_bytes(nl.join(lines))

c = W / 're/analysis/CHANGELOG.md'; t = open(c, encoding='utf-8', newline='').read()
line = ('2026-10-24  004625b0  AudioCharacterBankPathsBuild (bankpath flip, L-3 = A)  level kept C3  knob '
        'MASHED_BANKPATH_CORE removed, lowercase exe body + CharacterBankName deleted, one body left (the shared slot core). '
        f'path1 5/5 GREEN, gates unchanged exe {EXE}, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok')
