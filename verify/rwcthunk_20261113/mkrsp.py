import sys
base, out = sys.argv[1], sys.argv[2]
lines = open(base, encoding='ascii').read().splitlines()
res = []
for l in lines:
    if l.startswith('# RWC-CORE-RUN generated end'):
        res.append('"RwCore' + chr(92) + 'RwCore_004c7a60.cpp"')
    res.append(l)
open(out, 'w', newline='\r\n', encoding='ascii').write('\n'.join(res) + '\n')
