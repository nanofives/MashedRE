"""Print each Geometry's extension chunks in a DFF and decode rpUSERDATA (0x11F) arrays.

Usage: py -3.12 geo_userdata.py <file.dff>
Per geometry (in GEOMLIST order): flags, numTexCoordSets, extension chunk ids, and every user
data array as (name, format, values). Layout per RW rpUserData stream: i32 numArrays, then per
array: i32 nameLen + name bytes, i32 format (1 int, 2 real, 3 string), i32 numElements, values.
Also prints each Atomic's (frameIndex, geometryIndex) so a part code can be tied to an atomic.
"""
import struct
import sys


def chunks(buf, off, end):
    while off + 12 <= end:
        cid, size, lib = struct.unpack_from("<III", buf, off)
        yield cid, off + 12, min(off + 12 + size, end)
        off += 12 + size


def userdata(buf, p, e):
    n, = struct.unpack_from("<i", buf, p); p += 4
    out = []
    for _ in range(n):
        ln, = struct.unpack_from("<i", buf, p); p += 4
        name = buf[p:p + ln].split(b"\0")[0].decode("latin1"); p += ln
        fmt, cnt = struct.unpack_from("<ii", buf, p); p += 8
        vals = []
        for _ in range(cnt):
            if fmt == 1:
                vals.append(struct.unpack_from("<i", buf, p)[0]); p += 4
            elif fmt == 2:
                vals.append(round(struct.unpack_from("<f", buf, p)[0], 4)); p += 4
            elif fmt == 3:
                sl, = struct.unpack_from("<i", buf, p); p += 4
                vals.append(buf[p:p + sl].split(b"\0")[0].decode("latin1")); p += sl
        out.append((name, fmt, vals))
    return out


buf = open(sys.argv[1], "rb").read()
for cid, p, e in chunks(buf, 0, len(buf)):
    if cid != 0x10:
        continue
    gi = 0
    for c2, p2, e2 in chunks(buf, p, e):
        if c2 == 0x1A:
            for c3, p3, e3 in chunks(buf, p2, e2):
                if c3 != 0x0F:
                    continue
                flags = None
                exts = []
                ud = None
                for c4, p4, e4 in chunks(buf, p3, e3):
                    if c4 == 0x01:
                        flags, = struct.unpack_from("<I", buf, p4)
                    elif c4 == 0x03:
                        for c5, p5, e5 in chunks(buf, p4, e4):
                            exts.append(hex(c5))
                            if c5 == 0x11F:
                                ud = userdata(buf, p5, e5)
                print(f"geo[{gi}] flags=0x{flags:x} uvsets={(flags >> 16) & 0xff} ext={exts} userdata={ud}")
                gi += 1
        elif c2 == 0x14:
            for c3, p3, e3 in chunks(buf, p2, e2):
                if c3 == 0x01:
                    fi, gidx = struct.unpack_from("<ii", buf, p3)
                    print(f"atomic frame={fi} geometry={gidx}")
                    break
