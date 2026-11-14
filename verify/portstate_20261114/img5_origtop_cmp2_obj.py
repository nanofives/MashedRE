import re, difflib, sys
D = 'D:/MashedWT/IMG-5-ORIGTOP-DEFAULT_data/boots_obj/'
DROP = re.compile(r'COMMIT|--- totals|cfglog:|PC_24 after device|INTRO movie idx|det freeze|\[portstate-|\[portstate-seed\]|\[OT_CODEPTR\]|rdata seed|^\s*(FREE|RESERVE)')
def load(n):
    out = []
    for l in open(D + n + '.log', encoding='latin-1').read().splitlines():
        if DROP.search(l):
            continue
        l = re.sub(r'0x[0-9A-Fa-f]{8}|\b[0-9A-Fa-f]{8}\b', 'H', l)
        l = re.sub(r'tid=\d+', 'tid=N', l)
        l = re.sub(r'\d+(\.\d+)? ?(ms|s)\b', 'T', l)
        out.append(l)
    return out
L = {n: load(n) for n in ['off1', 'on1']}
def cmp(a, b):
    d = [x for x in difflib.unified_diff(L[a], L[b], lineterm='', n=0) if not x.startswith(('---', '+++', '@@'))]
    print('==', a, 'vs', b, ':', len(L[a]), '/', len(L[b]), 'lines, differing', len(d))
    for x in d[:int(sys.argv[1])]:
        print(x[:220])
cmp('off1', 'on1')
