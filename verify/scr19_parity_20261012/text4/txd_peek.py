# txd_peek.py <piz> <entry> <outdir> name... -- decode mip0 of named textures in a Mashed TXD to PNG (scratch).
import sys, struct
from pathlib import Path
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\text4\re\tools")
from piz_extract import read_archive
from PIL import Image

piz, entry, outdir, names = sys.argv[1], sys.argv[2], Path(sys.argv[3]), [n.lower() for n in sys.argv[4:]]
data, _v, _a, entries, _m = read_archive(Path(piz))
buf = None
for nm, o, ln, _id in entries:
    if nm.upper() == entry.upper():
        buf = data[o:o + ln]
        break
if buf is None:
    print("entry not found"); sys.exit(1)

def hdr(o): return struct.unpack_from("<III", buf, o) + (o + 12,)
cid, size, ver, off = hdr(0)
ntex, dev = struct.unpack_from("<HH", buf, off); off += 4
for t in range(ntex):
    (nm,) = struct.unpack_from("<I", buf, off); off += 4
    mip0 = None
    for m in range(nm):
        cid, isz, _, ip = hdr(off); end = ip + isz
        cid, _, _, sp = hdr(ip)
        w, h, d, st = struct.unpack_from("<IIII", buf, sp)
        if m == 0: mip0 = (w, h, d, st, sp + 16)
        off = end
    cid, tsz, _, tp = hdr(off); tend = tp + tsz
    # first string chunk (id 2) inside the TEXTURE payload = name
    name = None; q = tp
    while q + 12 <= tend:
        c, s, _, p = hdr(q)
        if c == 2:
            name = buf[p:p + s].split(b"\0")[0].decode("latin-1"); break
        q = p if c in (1,) and s > 0x40 else p + s
    off = tend
    if name and name.lower() in names:
        w, h, d, st, po = mip0
        if d in (4, 8):   # PAL4 = one byte per pixel, low nibble (TxdDecoder.h:65-71)
            pal = buf[po + st * h: po + st * h + (1 << d) * 4]
            img = Image.new("RGBA", (w, h))
            px = img.load()
            for y in range(h):
                row = buf[po + y * st: po + y * st + w]
                for x, i in enumerate(row):
                    r, g, b, a = pal[i * 4: i * 4 + 4]
                    px[x, y] = (r, g, b, a)
        elif d == 32:
            img = Image.frombuffer("RGBA", (w, h), buf[po: po + st * h], "raw", "BGRA", st, 1).copy()
        else:
            print(name, "depth", d, "not handled"); continue
        img.save(outdir / f"{name}.png"); print(name, w, h, d)
