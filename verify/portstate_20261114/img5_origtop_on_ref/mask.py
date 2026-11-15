import re, sys
# same filter as img5_origtop_cmp2.py; usage: mask.py in.log out.txt
DROP = re.compile(r'COMMIT|--- totals|cfglog:|PC_24 after device|INTRO movie idx|det freeze|\[portstate-|\[portstate-seed\]|\[OT_CODEPTR\]|rdata seed|^\s*(FREE|RESERVE)')
out = []
for l in open(sys.argv[1], encoding='latin-1').read().splitlines():
    if DROP.search(l): continue
    l = re.sub(r'0x[0-9A-Fa-f]{8}|\b[0-9A-Fa-f]{8}\b', 'H', l)
    l = re.sub(r'tid=\d+', 'tid=N', l)
    l = re.sub(r'\d+(\.\d+)? ?(ms|s)\b', 'T', l)
    out.append(l)
open(sys.argv[2], 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
