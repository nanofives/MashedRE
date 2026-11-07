"""MATCH-S23 LTCG probe: compile libpng 1.0.12 + zlib 1.1.3 with /GL, link a DLL with /LTCG (DDK link 7.00),
then compare every map-file function against the original at the slice rows (DLL base relocations masked).
    py -3.12 ltcg.py "<flags>" --tag t [--exports a,b]   (run from the MATCH-S23 worktree)"""
import argparse, os, pathlib, re, struct, subprocess, sys, csv
W = pathlib.Path.cwd(); sys.path.insert(0, str(W / "re/tools/match"))
import coffpe, period_cc
S = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\tools\_download\pngzlib\src")
Z = S / "zlib-1.1.3"; P = S / "libpng-1.0.12"
ZS = "adler32 compress crc32 deflate gzio infblock infcodes inffast inflate inftrees infutil trees uncompr zutil".split()
PS = "png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil".split()
D = pathlib.Path("D:/MashedWT/MATCH-S23_data")
ap = argparse.ArgumentParser(); ap.add_argument("flags"); ap.add_argument("--tag", required=True)
ap.add_argument("--exports", default=""); ap.add_argument("--linkflags", default="")
a = ap.parse_args()
out = D / "ltcg" / a.tag; out.mkdir(parents=True, exist_ok=True)
e1 = period_cc.compile_all([Z / f"{n}.c" for n in ZS], out, a.flags, [Z])
e2 = period_cc.compile_all([P / f"{n}.c" for n in PS], out, a.flags, [P, Z])
for e in (e1 + e2)[:4]: print(str(e)[:300])
LINK = period_cc.CL.parent / "link.exe"
objs = sorted(str(p) for p in out.glob("*.obj"))
dll = out / "t.dll"
cmd = [str(LINK), "/nologo", "/DLL", "/NOENTRY", "/NODEFAULTLIB", "/FORCE:UNRESOLVED", "/OPT:NOREF", "/OPT:NOICF",
       f"/MAP:{out/'t.map'}", f"/OUT:{dll}", *a.linkflags.split()] + [f"/EXPORT:{x}" for x in a.exports.split(",") if x] + objs
env = dict(os.environ, LIB="", LINK="", PATH=str(LINK.parent) + os.pathsep + os.environ["PATH"])
p = subprocess.run(cmd, capture_output=True, text=True, env=env)
if not dll.exists(): print(p.stdout[-1500:]); sys.exit(1)
b = dll.read_bytes()
pe = struct.unpack_from("<I", b, 0x3c)[0]; nsec = struct.unpack_from("<H", b, pe + 6)[0]; opt = struct.unpack_from("<H", b, pe + 20)[0]
base = struct.unpack_from("<I", b, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + opt + 40 * i
    secs.append((b[o:o+8].rstrip(b"\0").decode(), *struct.unpack_from("<IIII", b, o + 8)))
def rd(rva, n):
    for nm, vs, va, rs, raw in secs:
        if va <= rva < va + vs: return b[raw + rva - va: raw + rva - va + n]
    return b""
relocs = set()
for nm, vs, va, rs, raw in secs:
    if nm == ".reloc":
        d = b[raw:raw + vs]; i = 0
        while i + 8 <= len(d):
            pg, sz = struct.unpack_from("<II", d, i)
            if sz == 0: break
            for j in range(8, sz, 2):
                e = struct.unpack_from("<H", d, i + j)[0]
                if e >> 12 == 3: relocs.add(pg + (e & 0xfff))
            i += sz
# map: " 0001:00000000       _adler32                   10001000 f   adler32.obj"
fns = []
for ln in open(out / "t.map", errors="replace"):
    m = re.match(r"\s*0001:([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\s+f?\s", ln)
    if m: fns.append((int(m.group(3), 16) - base, m.group(2)))
fns = sorted(set(fns)); text = [s for s in secs if s[0] == ".text"][0]
ends = [x[0] for x in fns[1:]] + [text[2] + text[1]]
img = coffpe.Image()
slice_vas = [int(x, 16) for x in open(D / "slice_rvas.txt")]
sys.path.insert(0, str(W / "re/tools/match")); import match
hit = {}
for (rva, name), end in zip(fns, ends):
    body = rd(rva, end - rva).rstrip(b"\xcc")
    if len(body) < 3: continue
    rel = [r - rva for r in relocs if rva <= r < rva + len(body)]
    mask = set()
    for r in rel: mask.update(range(r, r + 4))
    for va in slice_vas:
        o = img.read(va, len(body))
        if len(o) == len(body) and all(o[k] == body[k] or k in mask for k in range(len(body))):
            # pc-relative call/jmp targets are not relocated: E8/E9 rel32 differ unless layout matches -> mask too
            hit.setdefault(va, name)
    # second pass with rel32 masked
fuzzy = {}
import capstone
cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def norm(data, mask):
    out = []
    for ins in cs.disasm(data, 0):
        s = ins.mnemonic
        if ins.mnemonic in ("call", "jmp") and ins.size == 5 and data[ins.address] in (0xe8, 0xe9): s += " REL"
        else: s += " " + ins.op_str
        out.append((ins.size, s if not any(k in mask for k in range(ins.address, ins.address + ins.size)) else ins.mnemonic + " RELOC"))
    return out
for (rva, name), end in zip(fns, ends):
    body = rd(rva, end - rva).rstrip(b"\xcc")
    if len(body) < 3: continue
    mask = set()
    for r in relocs:
        if rva <= r < rva + len(body): mask.update(range(r - rva, r - rva + 4))
    nb = norm(body, mask)
    for va in slice_vas:
        if va in hit or va in fuzzy: continue
        o = img.read(va, len(body))
        if len(o) != len(body): continue
        no = norm(o, set(k for k, (x, y) in enumerate(zip(o, body)) if k in mask))
        if [x for x in no] == nb or [s.split(" 0x")[0] for _, s in no] == [s.split(" 0x")[0] for _, s in nb]:
            fuzzy[va] = name
print(f"{a.tag}: map fns {len(fns)}  slice exact {len(hit)}  slice exact-but-rel32/imm {len(fuzzy)}  of {len(slice_vas)}")
with open(D / "ltcg" / f"{a.tag}.tsv", "w") as fo:
    for va in slice_vas: fo.write(f"0x{va:08x}\t{hit.get(va, '')}\t{fuzzy.get(va, '')}\n")
