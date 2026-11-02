"""Exe liveness for swapped symbols: direct call/jmp rel32 graph of mashed_re.exe (linker map), BFS from WinMain.
For each symbol prints: direct exe call sites (outside its own body), reachable-from-WinMain over direct edges (yes/no),
the shortest direct chain from WinMain, and whether its address appears as an absolute dword anywhere in the image
(address-taken: a function-pointer use the direct graph cannot see).
    py -3.12 exe_reach.py <mashed_re.exe> <mashed_re.map> <symbol> ..."""
import re, struct, sys, bisect
from collections import deque
exe, mp, syms = sys.argv[1], sys.argv[2], sys.argv[3:]
ents = []
for l in open(mp, errors="replace"):
    m = re.match(r"\s*0001:([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})", l)
    if m: ents.append((int(m.group(3), 16), m.group(2)))
ents.sort()
addrs = [a for a, _ in ents]
start = set(addrs)
name_at = dict(ents)
d = open(exe, "rb").read()
pe = struct.unpack_from("<I", d, 0x3c)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]; opt = struct.unpack_from("<H", d, pe + 20)[0]
ib = struct.unpack_from("<I", d, pe + 24 + 28)[0]
sec = pe + 24 + opt
secs = []
for i in range(nsec):
    nm, vs, va, rs, ro = struct.unpack_from("<8sIIII", d, sec + 40 * i)[:5]
    secs.append((nm.rstrip(b"\0"), va, ro, rs))
    if nm.rstrip(b"\0") == b".text": tva, traw, tsz = va, ro, rs
txt = d[traw:traw + tsz]


def owner(a):
    j = bisect.bisect_right(addrs, a) - 1
    return addrs[j] if j >= 0 else None


edges = {}
callers = {}
for i in range(len(txt) - 5):
    if txt[i] in (0xE8, 0xE9):
        src = ib + tva + i
        t = (src + 5 + struct.unpack_from("<i", txt, i + 1)[0]) & 0xffffffff
        if t in start:
            o = owner(src)
            if o is None or o == t: continue
            edges.setdefault(o, set()).add(t)
            callers.setdefault(t, []).append(f"{name_at[o]}+0x{src - o:x}")
# absolute references (function pointers in tables / push imm32 / mov imm32)
absref = set()
for nm, va, ro, rs in secs:
    blob = d[ro:ro + rs]
    for i in range(0, len(blob) - 3):
        v = struct.unpack_from("<I", blob, i)[0]
        if v in start: absref.add(v)
roots = [a for a, n in ents if n in ("_WinMain@16",)]
prev = {r: None for r in roots}
q = deque(roots)
while q:
    a = q.popleft()
    for t in edges.get(a, ()):
        if t not in prev:
            prev[t] = a; q.append(t)


def chain(a):
    out = []
    while a is not None:
        out.append(name_at[a]); a = prev[a]
    return " <- ".join(out)


print("symbol\taddr\tdirect_sites\treach_WinMain\taddr_taken\tchain_or_callers")
for s in syms:
    hit = [a for a, n in ents if n in ("_" + s, s) or n.startswith("_" + s + "@")]
    if not hit:
        print(f"{s}\tNOT-IN-MAP\t-\t-\t-\t-"); continue
    a = hit[0]
    cs = callers.get(a, [])
    r = a in prev
    print(f"{s}\t0x{a:08x}\t{len(cs)}\t{'yes' if r else 'no'}\t{'yes' if a in absref else 'no'}\t"
          f"{chain(a) if r else ';'.join(cs[:8])}")
