"""vgen.py <base.c> <spec.py> <out.txt>: spec.py defines VARIANTS = {name: [(old, new), ...]} applied to the body of
base.c (from the '// MATCH:' line). Each replacement must hit exactly once. Writes a vrun.py variants file."""
import sys
base, spec, out = sys.argv[1:4]
body = '// MATCH:' + open(base, encoding='utf-8').read().split('// MATCH:', 1)[1]
ns = {}
exec(open(spec, encoding='utf-8').read(), ns)
parts = []
for name, reps in ns['VARIANTS'].items():
    b = body
    for old, new in reps:
        n = b.count(old)
        if n != 1:
            sys.exit(f'{name}: {n} hits for {old!r}')
        b = b.replace(old, new)
    parts.append(f'@@ {name}\n{b}')
open(out, 'w', encoding='utf-8').write('\n'.join(parts))
