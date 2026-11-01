"""Count direct call/jmp rel32 sites in mashed_re.exe that target each swapped exe symbol (linker map), outside the
symbol's own body. A binary witness next to the source-level caller count (L-68 exe-callers).
    py -3.12 exe_callsites.py <mashed_re.exe> <mashed_re.map> <symbol> ..."""
import re, struct, sys, bisect
exe, mp, syms = sys.argv[1], sys.argv[2], sys.argv[3:]
base = 0x10000
ents = []
for l in open(mp, errors="replace"):
    m = re.match(r"\s*0001:([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})", l)
    if m: ents.append((int(m.group(3), 16), m.group(2)))
ents.sort()
addrs = [a for a, _ in ents]
d = open(exe, "rb").read()
pe = struct.unpack_from("<I", d, 0x3c)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]; opt = struct.unpack_from("<H", d, pe + 20)[0]
ib = struct.unpack_from("<I", d, pe + 24 + 28)[0]
sec = pe + 24 + opt
for i in range(nsec):
    nm, vs, va, rs, ro = struct.unpack_from("<8sIIII", d, sec + 40 * i)[:5]
    if nm.rstrip(b"\0") == b".text": tva, traw, tsz = va, ro, rs
txt = d[traw:traw + tsz]
want = {}
for s in syms:
    hit = [a for a, n in ents if n in ("_" + s, s) or n.startswith("_" + s + "@")]
    want[s] = hit[0] if hit else None
tgt = {v: k for k, v in want.items() if v}
cnt = {s: [] for s in syms}
for i in range(len(txt) - 5):
    if txt[i] in (0xE8, 0xE9):
        src = ib + tva + i
        t = (src + 5 + struct.unpack_from("<i", txt, i + 1)[0]) & 0xffffffff
        if t in tgt:
            j = bisect.bisect_right(addrs, src) - 1
            fn = ents[j][1] if j >= 0 else "?"
            if ents[j][0] != t: cnt[tgt[t]].append(f"{fn}+0x{src - ents[j][0]:x}")
for s in syms:
    print(f"{s}\t{'0x%08x' % want[s] if want[s] else 'NOT-IN-MAP'}\t{len(cnt[s])}\t{';'.join(cnt[s][:6])}")
