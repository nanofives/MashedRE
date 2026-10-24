# D4-AUDIO-VOLTODS tracker transaction (re-classify): file move + exe_file for 0x005aeb90, note, one CHANGELOG line.
# Level kept (C3). Line-level edits only (other hooks.csv rows stay byte-identical). Copy of
# verify/audiolink_20261023/apply_trackers_s5.py with the row set changed.
import csv, io, pathlib, sys
W = pathlib.Path(__file__).resolve().parents[2]
F = 'mashedmod/src/mashed_re/Audio/AudioLinearToLog.cpp'
RVAS = {'005aeb90'}
R = 'verify/audiovoltods_20261024/RESULT_voltods.md'
LINK = (f' | D4-AUDIO-VOLTODS 2026-10-24: moved from Audio/TimerGetters_ah5.cpp to Audio/AudioLinearToLog.cpp (both rsp), '
        f'linked into the exe (private .rdata copies of 0x005cc990 / 0x005cd0b8 under MASHED_STANDALONE); called by '
        f'AudioEngine VolToDs only under MASHED_VOLTODS_ORIG=1 (default OFF). path1 re-run GREEN 18/18 on the moved .asi '
        f'body; offline sweep exe body vs the original bytes 0 / 2^32 at PC_53 / PC_24 / PC_64. {R}')

p = W / 'hooks.csv'; raw = p.read_bytes(); nl = b'\r\n' if b'\r\n' in raw else b'\n'
lines = raw.split(nl); hdr = next(csv.reader([lines[0].decode('utf-8')])); ix = {k: i for i, k in enumerate(hdr)}
seen = set()
for n, b in enumerate(lines[1:], 1):
    if not b: continue
    s = b.decode('utf-8'); rva = s.split(',', 1)[0].lower()
    if rva not in RVAS: continue
    row = next(csv.reader([s])); row += [''] * (len(hdr) - len(row))
    assert row[ix['confidence']] in ('C3', 'C4'), (rva, row[ix['confidence']])
    if LINK in row[ix['notes']]: seen.add(rva); continue      # idempotent
    row[ix['file']] = F
    row[ix['exe_file']] = F
    row[ix['notes']] = row[ix['notes']] + LINK
    o = io.StringIO(); csv.writer(o, lineterminator='').writerow(row); lines[n] = o.getvalue().encode('utf-8'); seen.add(rva)
miss = RVAS - seen
if miss: sys.exit(f'missing rows: {sorted(miss)}')
p.write_bytes(nl.join(lines))

c = W / 're/analysis/CHANGELOG.md'; t = open(c, encoding='utf-8', newline='').read()
line = ('2026-10-24  005aeb90  AudioLinearToLog (D4-AUDIO-VOLTODS)  level kept C3  FILE MOVE Audio/TimerGetters_ah5.cpp -> '
        'Audio/AudioLinearToLog.cpp (both rsp), linked into the exe; VolToDs routes through it only under '
        'MASHED_VOLTODS_ORIG=1 (default OFF). path1 18/18 GREEN, sweep S1 0 / 2^32, gates unchanged, lint 0 NEW. ' + R + '\n')
if line.rstrip('\n') not in t:
    m = '<!-- ENTRIES -->\n'; i = t.index(m) + len(m); t = t[:i] + line + t[i:]; open(c, 'w', encoding='utf-8', newline='').write(t)
print('ok', len(seen))
