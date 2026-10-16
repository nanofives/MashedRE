"""List the textures in a Mashed TXD (chunk 0x23 container, layout per txd_format_census.py /
TxdDecoder.h): name, top-mip width x height, depth, mip count.

Usage: py -3.12 txd_list.py <file.txd> [name-regex]
"""
import re
import struct
import sys

buf = open(sys.argv[1], "rb").read()
pat = re.compile(sys.argv[2], re.I) if len(sys.argv) > 2 else None
cid, size, ver = struct.unpack_from("<III", buf, 0)
assert cid == 0x23, hex(cid)
ntex, dev = struct.unpack_from("<HH", buf, 12)
p = 16
for _ in range(ntex):
    nmips, = struct.unpack_from("<I", buf, p); p += 4
    top = None
    for m in range(nmips):
        p += 24
        w, h, depth, stride = struct.unpack_from("<IIII", buf, p); p += 16
        if top is None:
            top = (w, h, depth)
        p += stride * h
        if depth < 9:
            p += (1 << depth) * 4
    tcid, tsize, _ = struct.unpack_from("<III", buf, p)
    body = buf[p + 12:p + 12 + tsize]
    p += 12 + tsize
    names = []
    q = 0
    while q + 12 <= len(body):
        c, s, _ = struct.unpack_from("<III", body, q)
        if c == 2:
            names.append(body[q + 12:q + 12 + s].split(b"\0")[0].decode("latin1"))
        q += 12 + s
    nm = names[0] if names else "?"
    if pat is None or pat.search(nm):
        print(f"{nm:24s} {top[0]}x{top[1]} depth={top[2]} mips={nmips}")
