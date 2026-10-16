#!/usr/bin/env python3
"""Model of RenderWare's camera-sync sector list (MASHED.exe 0x004e38a0) checked against
the original's recorded draw order.

What 0x004e38a0 does, read from the bytes (log/rstates/dis_4e3800.txt):
  * calls the camera's original sync (ext+0x18) first, so the frustum planes
    (camera+0x94, 6 x 0x14: normal xyz, distance, closestX/Y/Z bytes) and the
    frustum bound box (camera+0x10c sup, +0x118 inf) are fresh;
  * eye = RwFrameGetLTM(camera frame)+0x30 (0x004e38f9);
  * back2front = (world+0xc == 2) (0x004e394d);
  * walks the tree from world+0x1c with an explicit stack (slot 0 never read):
      plane sector (type >= 0, type = 0/4/8 = x/y/z byte offset):
        a = frustumInf[c] - leftValue(+0x10)   b = rightValue(+0x14) - frustumSup[c]
        a < 0 and b < 0  -> both children; near child first: eye[c] > value(+4)
                            -> right(+0xc) first, else left(+8) first (front2back);
                            the far child is pushed (0x004e3a5e..0x004e3aa4)
        a >= 0           -> right only        (0x004e3aa6..0x004e3ab3)
        else             -> left only
      atomic sector (type < 0): 6 planes, corner = bbox[closest] (+0x48 sup, +0x54 inf),
        dot(n, corner) - d > 0 on any plane -> skipped, else appended (0x004e396b..0x004e3a2b)
  * sign tests are integer tests on the float bits (`test reg,reg`), so -0.0 counts as < 0.

The per-sector callback 0x004e5680 then draws each listed sector (FUN_004f0900) followed by
its tied atomics (sector+0x38 list, RENDER flag, renderFrame != current, sphere in frustum).

Usage:
  py -3.12 bsp_order.py <track.piz> <bsp entry> <trace.csv> <row> <far> <orig draws.json>
"""
import json
import math
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1] / "re" / "tools"))
import piz_extract  # noqa: E402


def chunk(d, off):
    t, sz, ver = struct.unpack_from("<III", d, off)
    return t, sz, off + 12


def parse_bsp(d):
    t, sz, p = chunk(d, 0)
    assert t == 0x0B
    t, ssz, sp = chunk(d, p)
    assert t == 0x01 and ssz == 0x40
    nmat_chunk_t, msz, mp = chunk(d, sp + ssz)
    assert nmat_chunk_t == 0x08
    # material names (same walk as track_dump.parse_matlist)
    t2, s2, mp2 = chunk(d, mp)
    n = struct.unpack_from("<i", d, mp2)[0]
    mats = []
    q = mp2 + s2
    for _ in range(n):
        t3, s3, q3 = chunk(d, q)
        name = None
        qq, qe = q3, q3 + s3
        while qq + 12 <= qe:
            t4, s4, q4 = chunk(d, qq)
            if t4 == 0x06:
                _t5, s5, q5 = chunk(d, q4)
                _t6, s6, q6 = chunk(d, q4 + 12 + s5)
                name = d[q6:q6 + s6].split(b"\x00")[0].decode("ascii", "replace")
            qq = q4 + s4
        mats.append(name)
        q = q3 + s3
    sectors = []

    def node(off):
        t, sz, sp = chunk(d, off)
        if t == 0x0A:
            t2, s2, pp = chunk(d, sp)
            typ, value, lia, ria, lv, rv = struct.unpack_from("<ifiiff", d, pp)
            left, lend = node(pp + s2)
            right, _ = node(lend)
            return ({"plane": True, "type": typ, "value": value, "lv": lv, "rv": rv,
                     "left": left, "right": right, "lia": lia, "ria": ria}, off + 12 + sz)
        assert t == 0x09
        t2, s2, ap = chunk(d, sp)
        base, nt, nv = struct.unpack_from("<III", d, ap)
        box = struct.unpack_from("<6f", d, ap + 0x0C)
        payload = s2 - 0x2C
        rem = payload - nv * 12 - nt * 8
        layout = None
        for nrm in (0, 1):
            for pl in (0, 1):
                for uv in (0, 1, 2):
                    if rem == nrm * nv * 12 + pl * nv * 4 + uv * nv * 8:
                        layout = (nrm, pl, uv)
        assert layout, "layout"
        q = ap + 0x2C
        verts = list(struct.iter_unpack("<3f", d[q:q + nv * 12]))
        q += nv * 12 + layout[0] * nv * 12 + layout[1] * nv * 4 + layout[2] * nv * 8
        raw = list(struct.iter_unpack("<4H", d[q:q + nt * 8]))
        # u16 field order decided by the data, as track_dump.py does: the order whose
        # vertex fields are all < nv and material fields all inside the list wins.
        fitsA = all(r[0] + base < len(mats) and max(r[1:4]) < nv for r in raw)
        fitsB = all(r[3] + base < len(mats) and max(r[0:3]) < nv for r in raw)
        assert fitsA or fitsB, "triangle order"
        tris = ([(r[0] + base, r[1], r[2], r[3]) for r in raw] if fitsA else
                [(r[3] + base, r[0], r[1], r[2]) for r in raw])  # (mat, v0, v1, v2)
        s = {"plane": False, "id": len(sectors), "box": box, "verts": verts, "tris": tris,
             "base": base}
        sectors.append(s)
        return s, off + 12 + sz

    root, _ = node(mp + msz)
    # world header bbox (STRUCT +0x28, 6 floats) -> per-sector BSP cell boxes
    wb = struct.unpack_from("<6f", d, sp + 0x28)
    winf, wsup, _ = bbox_of(wb)

    def cells(n, inf, sup):
        if not n["plane"]:
            n["cell"] = (list(inf), list(sup))
            return
        c = n["type"] // 4
        lsup = list(sup); lsup[c] = n["lv"]
        rinf = list(inf); rinf[c] = n["rv"]
        cells(n["left"], inf, lsup)
        cells(n["right"], rinf, sup)

    cells(root, winf, wsup)
    return root, sectors, mats


def bbox_of(box6):
    """Stream box -> (inf, sup), decided by the numbers, not by assumption."""
    a, b = box6[:3], box6[3:]
    if all(x <= y for x, y in zip(a, b)):
        return list(a), list(b), "inf,sup"
    if all(x >= y for x, y in zip(a, b)):
        return list(b), list(a), "sup,inf"
    raise ValueError(f"mixed bbox order {box6}")


class Cam:
    def __init__(self, row, far, near=0.1):
        g = lambda k: float(row[k])
        self.r = [g("M_rx"), g("M_ry"), g("M_rz")]
        self.u = [g("M_ux"), g("M_uy"), g("M_uz")]
        self.a = [g("M_ax"), g("M_ay"), g("M_az")]
        self.p = [g("M_px"), g("M_py"), g("M_pz")]
        vx, vy = g("rvwx"), g("rvwy")
        corners = []
        for z in (near, far):
            for sx in (-1, 1):
                for sy in (-1, 1):
                    corners.append([self.p[i] + self.a[i] * z + self.r[i] * sx * vx * z +
                                    self.u[i] * sy * vy * z for i in range(3)])
        self.inf = [min(c[i] for c in corners) for i in range(3)]
        self.sup = [max(c[i] for c in corners) for i in range(3)]
        dot = lambda x, y: sum(i * j for i, j in zip(x, y))
        planes = []
        # near / far
        planes.append(([-x for x in self.a], -(dot(self.a, self.p) + near)))
        planes.append((list(self.a), dot(self.a, self.p) + far))
        # sides: through the eye, outward
        for sx in (-1, 1):
            n = [sx * self.r[i] - vx * self.a[i] for i in range(3)]
            L = math.sqrt(dot(n, n)); n = [x / L for x in n]
            planes.append((n, dot(n, self.p)))
        for sy in (-1, 1):
            n = [sy * self.u[i] - vy * self.a[i] for i in range(3)]
            L = math.sqrt(dot(n, n)); n = [x / L for x in n]
            planes.append((n, dot(n, self.p)))
        self.planes = planes

    def box_out(self, inf, sup, skip=()):
        for k, (n, d) in enumerate(self.planes):
            if k in skip:
                continue
            c = [inf[i] if n[i] >= 0 else sup[i] for i in range(3)]
            if sum(n[i] * c[i] for i in range(3)) - d > 0:
                return True
        return False


BOX_MODE = "stream"   # "stream" = the sector's own stream box, "cell" = BSP cell box


def traverse(root, cam, back2front=False, skip=()):
    out, stack = [], []
    node = root
    while True:
        if node["plane"]:
            c = node["type"] // 4
            a = cam.inf[c] - node["lv"]
            b = node["rv"] - cam.sup[c]
            neg = lambda v: v < 0 or (v == 0 and math.copysign(1, v) < 0)
            if neg(a) and neg(b):
                cam_gt = cam.p[c] > node["value"]
                first, second = (("right", "left") if cam_gt else ("left", "right"))
                if back2front:
                    first, second = second, first
                stack.append(node[second])
                node = node[first]
                continue
            node = node["right"] if not neg(a) else node["left"]
            continue
        inf, sup = (node["cell"] if BOX_MODE == "cell" else bbox_of(node["box"])[:2])
        if not cam.box_out(inf, sup, skip):
            out.append(node["id"])
        if not stack:
            return out
        node = stack.pop()


def sector_meshes(s, mats):
    """Per material: (name, tri count, AABB) of the sector's triangles."""
    per = {}
    for m, v0, v1, v2 in s["tris"]:
        if v0 == v1 or v1 == v2 or v0 == v2:
            continue
        e = per.setdefault(m, [0, [1e9] * 3, [-1e9] * 3])
        e[0] += 1
        for v in (v0, v1, v2):
            for i in range(3):
                e[1][i] = min(e[1][i], s["verts"][v][i])
                e[2][i] = max(e[2][i], s["verts"][v][i])
    return {m: (mats[m] if m < len(mats) else None, e[0], e[1] + e[2]) for m, e in per.items()}


_CACHE = {}


def load_world(piz, entry):
    if (piz, entry) not in _CACHE:
        data, _v, _a, ents, _m = piz_extract.read_archive(Path(piz))
        blob = None
        for (nm, o, l, _i) in ents:
            nm = nm if isinstance(nm, str) else nm.decode("latin1")
            if nm.lower().endswith(entry.lower()):
                blob = data[o:o + l]
        assert blob, entry
        _CACHE[(piz, entry)] = parse_bsp(blob)
    return _CACHE[(piz, entry)]


def compare(piz, entry, trace, row, far, draws, skip=(), verbose=True):
    root, sectors, mats = load_world(piz, entry)
    import csv
    rows = list(csv.DictReader(open(trace)))
    cam = Cam(rows[row], far)
    model = traverse(root, cam, skip=skip)
    return orig_sequence(sectors, mats, draws), [s for s in model
                                                 if sector_meshes(sectors[s], mats)]


def orig_sequence(sectors, mats, draws):
    # map the original's draws to (sector, material)
    index = {}
    for s in sectors:
        for m, (nm, n, bb) in sector_meshes(s, mats).items():
            index.setdefault(((nm or "").lower(), n), []).append((s["id"], m, bb))
    import drawlist3d
    seq = []
    for e in drawlist3d.cut_frame(json.load(open(draws))):
        if not e.get("tri"):
            continue
        key = ((e.get("tex") or "").lower(), e.get("tri"))
        cands = index.get(key, [])
        best = None
        for sid, m, bb in cands:
            dd = max(abs(x - y) for x, y in zip(bb, e["bb"])) if e.get("bb") else 1e9
            if dd < 0.05 and (best is None or dd < best[0]):
                best = (dd, sid, m)
        seq.append((best[1] if best else None, e.get("tex"), e.get("tri")))
    orig = []
    for sid, _t, _n in seq:
        if sid is not None and (not orig or orig[-1] != sid):
            orig.append(sid)
    return orig


def report(orig, model_d, tag=""):
    ok = orig == model_d
    print(f"{tag} {'EXACT' if ok else 'DIFFER'} orig={orig} model={model_d}")
    if not ok:
        so, sm = set(orig), set(model_d)
        common_o = [x for x in orig if x in sm]
        common_m = [x for x in model_d if x in so]
        print(f"    only orig {sorted(so - sm)} only model {sorted(sm - so)} "
              f"common-order-equal {common_o == common_m}")
    return ok


if __name__ == "__main__":
    piz, entry, trace, row, far, draws = sys.argv[1:7]
    report(*compare(piz, entry, trace, int(row), float(far), draws))
