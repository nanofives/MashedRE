#!/usr/bin/env python3
"""rstates F7 check: per geometry of a track DFF, the material texture order (material index order,
which DffModel's batches follow) against the stream's BinMesh PLG (chunk 0x50E) mesh order.

    py -3.12 binmesh_order.py <track piz name> [texture substring filter]
"""
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1] / "re" / "tools"))
import piz_extract  # noqa: E402

TR = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\TOASTART\TRACKS")
CONTAINERS = {0x10, 0x1A, 0x0F, 0x08, 0x07, 0x06, 0x03, 0x14, 0x0E, 0x0B, 0x09, 0x0A}


def chunks(buf, off, end):
    while off + 12 <= end:
        t, sz, _ver = struct.unpack_from("<III", buf, off)
        yield t, off + 12, min(off + 12 + sz, end)
        off += 12 + sz


def find(buf, off, end, typ):
    return [(s, e) for t, s, e in chunks(buf, off, end) if t == typ]


def tex_name(buf, s, e):
    for ts, te in find(buf, s, e, 0x06):
        for t, cs, ce in chunks(buf, ts, te):
            if t == 0x02:
                return buf[cs:ce].split(b"\0")[0].decode("latin-1")
    return "-"


def geometry(buf, s, e):
    mats = []
    for ms, me in find(buf, s, e, 0x08):
        sub = list(chunks(buf, ms, me))
        st = [x for x in sub if x[0] == 0x01][0]
        n = struct.unpack_from("<i", buf, st[1])[0]
        idx = struct.unpack_from("<%di" % n, buf, st[1] + 4)
        mobj = [x for x in sub if x[0] == 0x07]
        k = 0
        for i in range(n):
            if idx[i] < 0:
                mats.append(tex_name(buf, mobj[k][1], mobj[k][2])); k += 1
            else:
                mats.append(mats[idx[i]])
    order = None
    for xs, xe in find(buf, s, e, 0x03):
        for bs, be in find(buf, xs, xe, 0x50E):
            _fl, nm, _tot = struct.unpack_from("<III", buf, bs)
            p, order = bs + 12, []
            for _ in range(nm):
                ni, mi = struct.unpack_from("<II", buf, p)
                order.append(mi)
                p += 8 + 4 * ni   # stream (non-native) binmesh: u32 indices
    return mats, order


def main():
    piz, filt = sys.argv[1], (sys.argv[2] if len(sys.argv) > 2 else "")
    data, _v, _a, entries, _m = piz_extract.read_archive(TR / f"{piz}.piz")
    for (n, o, ln, _i) in entries:
        name = n.decode("latin-1") if isinstance(n, bytes) else n
        if not name.upper().endswith(".DFF"):
            continue
        buf = data[o:o + ln]
        for cs, ce in find(buf, 0, len(buf), 0x10):
            for gl, gle in find(buf, cs, ce, 0x1A):
                for gi, (gs, ge) in enumerate(find(buf, gl, gle, 0x0F)):
                    mats, order = geometry(buf, gs, ge)
                    if filt and not any(filt in m for m in mats):
                        continue
                    same = order == sorted(order or [])
                    print(f"{name} geo{gi}: mats={mats}")
                    print(f"    binmesh order={order} -> {[mats[i] for i in (order or [])]}  "
                          f"{'== material order' if same else 'DIFFERS from material order'}")


if __name__ == "__main__":
    main()
