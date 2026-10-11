"""Python mirror of FUN_0040e590's mode-4 slot assignment (sections C, D, E; rule 0, DAT_0067ea64 == 0), checked
against the original-bytes oracle slotorder_m4.tsv. The C++ port (Race/RoundGrid.cpp) is the same transcription."""
import sys
def assign(c):
    m = {2: 3, 1: 2, -1: 1, -2: 0}
    s = [m.get(x, 1000) for x in c]                        # section C, case 4
    l44 = all(v == 1000 for v in s)                        # local_44
    for i in range(4):                                     # section D, mode 4 fill
        if c[i] not in m:
            for v in range(4):
                if v not in s:
                    s[i] = v; break
    while True:                                            # section E, mode 4
        for a, b in ((0, 1), (1, 2), (2, 3)):
            while s.count(a) >= 2:
                s[s.index(a)] = b
        if s.count(3) < 2: break
        s[s.index(3)] = 0
    return [i + 4 for i in range(4)] if l44 else s
ok = n = 0
for ln in open(sys.argv[1]):
    if ln.startswith("#"): continue
    v = list(map(int, ln.split()))
    n += 1; got = assign(v[:4]); ok += got == v[4:]
    if got != v[4:] and n - ok <= 10: print("DIFF", v[:4], "mirror", got, "oracle", v[4:])
print(f"mirror vs oracle {ok}/{n}")
