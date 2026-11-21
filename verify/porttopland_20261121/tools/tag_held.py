"""PORT-TOP-LAND-06-RF: tag the all-marked held groups `lint_rva_bodies.py --seed` lists as ORIGTOP-HELD (L-179).
An untagged allowlist line with the same key gets the tag prefixed (its comment kept); a key with no line gets the seed line.
py -3.12 verify/porttopland_20261121/tools/tag_held.py [--apply]"""
import subprocess, sys

ALLOW = 're/tools/dual_copy_allowlist.txt'
seed = subprocess.run([sys.executable, 'scripts/lint_rva_bodies.py', '--seed'], capture_output=True, text=True).stdout
raw = open(ALLOW, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
lines = raw.decode().split(nl)
idx = {l.split('#')[0].strip(): i for i, l in enumerate(lines)}
want = [l for l in seed.splitlines() if l.startswith('ORIGTOP-HELD ')]
add = []
for l in want:
    k = l.split('#')[0].strip()
    if k in idx:
        continue
    plain = k[len('ORIGTOP-HELD '):]
    if plain in idx:
        i = idx[plain]
        lines[i] = 'ORIGTOP-HELD ' + lines[i]
        print('tagged', plain.split(':')[1])
    else:
        add.append(k + '   # PORT-TOP-LAND-06-RF (L-111 (a), L-179), 2026-11-21: ' + l.split('#', 1)[1].strip()
                   + '. Every body is a static held copy in an ORIGTOP-ONLY marked wrapper; deleted at the ORIG_TOP flip.'
                   + ' Evidence: verify/porttopland_20261121/RESULT_PORT-TOP-LAND-06-RF.md.')
        print('added', plain.split(':')[1])
if add:
    if lines and lines[-1] == '':
        lines.pop()
    lines += add + ['']
if '--apply' in sys.argv:
    open(ALLOW, 'wb').write(nl.join(lines).encode())
