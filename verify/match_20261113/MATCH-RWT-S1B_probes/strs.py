import re, sys
d = open(sys.argv[1], "rb").read()
pat = re.compile(rb"[\x20-\x7e]{%d,}" % int(sys.argv[2]))
flt = re.compile(sys.argv[3], re.I) if len(sys.argv) > 3 else None
for m in pat.finditer(d):
    s = m.group().decode()
    if flt is None or flt.search(s):
        print(hex(m.start()), s)
