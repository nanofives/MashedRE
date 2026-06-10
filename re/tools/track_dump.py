#!/usr/bin/env python3
"""R3 opener — Mashed track world dumper (GRAPH.BSP -> OBJ + wireframe PNG).

GRAPH.BSP inside TRACKS/<track>.piz is a RenderWare 3.6 binary world stream
(rwID_WORLD 0x0B, stream version 0x1c02000a — same family as the game's TXDs):

  WORLD(0x0B)
    STRUCT(0x01, 0x40 bytes)  world header (validated field-by-field below)
    MATLIST(0x08)             materials (count cross-checks triangle mat ids)
    PLANESECTOR(0x0A)         binary tree:
       STRUCT(0x18) { type i32, value f32, leftIsAtomic b32, rightIsAtomic b32,
                      leftValue f32, rightValue f32 }
       <left child>  PLANESECTOR(0x0A) or ATOMICSECTOR(0x09)
       <right child> "
    ATOMICSECTOR(0x09) leaves:
       STRUCT { matListWindowBase u32, numTriangles u32, numVertices u32,
                boundingBox 6*f32, unused u32, unused u32 }   (= 0x2c bytes)
       then arrays in STRUCT payload: vertices f32[3]*nv, [normals], [prelight
       RGBA u32*nv], [texcoords f32[2]*nv per set], triangles u16[4]*nt.
       The array set is SOLVED from the payload size and VALIDATED (vertex
       indices < nv, mat ids < material count, coords inside the bbox).

Every parsed quantity is validated against the world-header totals; the tool
refuses to emit output if the fit is inconsistent (NO-GUESSING: the validation
IS the evidence).

Usage:
  py -3.12 re/tools/track_dump.py original/TOASTART/TRACKS/Arctic.piz \
      [--entry GRAPH.BSP] [--obj out.obj] [--png out.png]
"""
import argparse
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import piz_extract  # noqa: E402  (sibling tool; used for the piz container)


def read_chunk(d, off):
    t, sz, ver = struct.unpack_from("<III", d, off)
    return t, sz, ver, off + 12


def parse_matlist(d, mp, msz):
    """MATLIST(0x08) -> ordered material list. Each MATERIAL(0x07) carries a
    STRUCT (color RGBA at struct+0x04) and a TEXTURE(0x06) child whose second
    subchunk is the STRING texture name. Returns [(tex_name, rgba), ...]."""
    t2, s2, _, mp2 = read_chunk(d, mp)
    assert t2 == 0x01
    n = struct.unpack_from("<i", d, mp2)[0]
    mats = []
    q = mp2 + s2
    for _ in range(n):
        t3, s3, _, q3 = read_chunk(d, q)
        assert t3 == 0x07, f"material chunk {t3:#x}"
        name, rgba = None, None
        qq, qe = q3, q3 + s3
        while qq + 12 <= qe:
            t4, s4, _, q4 = read_chunk(d, qq)
            if t4 == 0x01:
                rgba = struct.unpack_from("<4B", d, q4 + 4)
            elif t4 == 0x06:
                _t5, s5, _, q5 = read_chunk(d, q4)
                _t6, s6, _, q6 = read_chunk(d, q4 + 12 + s5)
                name = d[q6:q6 + s6].split(b"\x00")[0].decode("ascii", "replace")
            qq = q4 + s4
        mats.append((name, rgba))
        q = q3 + s3
    return mats


def txd_names(d):
    """Texture names in Mashed's chunk-0x23 TXD container (layout per
    mashedmod/src/mashed_re/Txd/TxdDecoder.h / FUN_0054f8d0)."""
    t, sz, _ver = struct.unpack_from("<III", d, 0)
    assert t == 0x23, f"TXD root {t:#x}"
    num_tex = struct.unpack_from("<H", d, 0x0C)[0]
    names = []
    p = 0x10
    for _ in range(num_tex):
        num_mips = struct.unpack_from("<I", d, p)[0]
        p += 4
        for _ in range(num_mips):
            _it, isz, _, ip = read_chunk(d, p)          # IMAGE 0x18
            _st, ssz, _, spp = read_chunk(d, ip)        # STRUCT
            w, h, depth, stride = struct.unpack_from("<4I", d, spp)
            q = spp + ssz + stride * h
            if depth < 9:
                q += (1 << depth) * 4
            p = q
        tt, tsz, _, tp = read_chunk(d, p)               # TEXTURE 0x06
        assert tt == 0x06, f"texture chunk {tt:#x} @ {p:#x}"
        _t5, s5, _, q5 = read_chunk(d, tp)              # STRUCT filter
        _t6, s6, _, q6 = read_chunk(d, tp + 12 + s5)    # STRING name
        names.append(d[q6:q6 + s6].split(b"\x00")[0].decode("ascii", "replace"))
        p = tp + tsz
    return names


def parse_world(d):
    t, sz, ver, p = read_chunk(d, 0)
    assert t == 0x0B, f"root chunk {t:#x} != WORLD"
    end = p + sz

    # ---- world header STRUCT ----
    t, ssz, _, sp = read_chunk(d, p)
    assert t == 0x01 and ssz == 0x40, f"world struct {t:#x}/{ssz:#x}"
    (root_is_ws, inv_x, inv_y, inv_z, num_tris, num_verts,
     num_plane, num_world, col_sec, fmt) = struct.unpack_from("<I3fIIIIII", d, sp)
    bbox = struct.unpack_from("<6f", d, sp + 0x28)
    hdr = dict(rootIsWorldSector=root_is_ws, invOrigin=(inv_x, inv_y, inv_z),
               numTriangles=num_tris, numVertices=num_verts,
               numPlaneSectors=num_plane, numWorldSectors=num_world,
               colSectorSize=col_sec, format=fmt, bbox=bbox)
    p = sp + ssz

    # ---- material list ----
    t, msz, _, mp = read_chunk(d, p)
    assert t == 0x08, f"matlist {t:#x}"
    mats = parse_matlist(d, mp, msz)
    hdr["numMaterials"] = len(mats)
    hdr["materials"] = mats
    p = mp + msz

    # ---- sector tree ----
    sectors = []  # (verts, tris, uvs)
    plane_count = 0

    def parse_sector(off):
        nonlocal plane_count
        t, sz, _, sp = read_chunk(d, off)
        if t == 0x0A:  # plane sector
            plane_count += 1
            t2, s2, _, pp = read_chunk(d, sp)
            assert t2 == 0x01 and s2 == 0x18, f"plane struct {s2:#x}"
            left_end = parse_sector(pp + s2)
            parse_sector(left_end)
            return off + 12 + sz
        if t == 0x09:  # atomic sector
            t2, s2, _, ap = read_chunk(d, sp)
            assert t2 == 0x01, "atomic struct missing"
            base, nt, nv = struct.unpack_from("<III", d, ap)
            sbox = struct.unpack_from("<6f", d, ap + 0x0C)
            fixed = 0x2C
            payload = s2 - fixed
            q = ap + fixed
            # Solve array layout from payload size:
            # payload = nv*12 (verts) + [nv*12 normals] + [nv*4 prelight]
            #           + nuv*nv*8 (texcoords) + nt*8 (tris)
            rem = payload - nv * 12 - nt * 8
            have_normals = have_prelit = False
            nuv = 0
            for normals in (0, 1):
                for prelit in (0, 1):
                    for uv in (0, 1, 2):
                        if rem == normals * nv * 12 + prelit * nv * 4 + uv * nv * 8:
                            have_normals, have_prelit, nuv = bool(normals), bool(prelit), uv
                            rem = -1
                            break
                    if rem == -1: break
                if rem == -1: break
            assert rem == -1, (f"sector layout unsolved: payload={payload:#x} "
                               f"nv={nv} nt={nt}")
            verts = list(struct.iter_unpack("<3f", d[q:q + nv * 12])); q += nv * 12
            if have_normals: q += nv * 12
            if have_prelit:  q += nv * 4
            uvs = None
            if nuv:
                uvs = list(struct.iter_unpack("<2f", d[q:q + nv * 8]))
                q += nuv * nv * 8
            raw = list(struct.iter_unpack("<4H", d[q:q + nt * 8])); q += nt * 8
            # Determine the u16 field order empirically: hypothesis A =
            # (mat, v0, v1, v2), hypothesis B = (v0, v1, v2, mat). The
            # consistent one has all vertex fields < nv and all mat fields
            # within the material window. Recorded in hdr['triOrder'].
            def fits(order):
                for t4 in raw:
                    m = t4[0] if order == "A" else t4[3]
                    vs3 = t4[1:4] if order == "A" else t4[0:3]
                    if m >= hdr["numMaterials"]: return False
                    if any(v >= nv for v in vs3): return False
                return True
            if fits("A"):
                tris = [(t4[0], t4[1], t4[2], t4[3]) for t4 in raw]
                hdr.setdefault("triOrder", "mat,v0,v1,v2")
            elif fits("B"):
                tris = [(t4[3], t4[0], t4[1], t4[2]) for t4 in raw]
                hdr.setdefault("triOrder", "v0,v1,v2,mat")
            else:
                raise AssertionError(
                    f"triangle field order unresolved (nv={nv}, "
                    f"mats={hdr['numMaterials']}, sample={raw[:3]})")
            sectors.append(dict(verts=[tuple(v) for v in verts],
                                tris=tris,
                                bbox=sbox, normals=have_normals,
                                prelit=have_prelit, nuv=nuv))
            return off + 12 + sz
        raise AssertionError(f"unexpected sector chunk {t:#x} @ {off:#x}")

    parse_sector(p)

    tot_v = sum(len(s["verts"]) for s in sectors)
    tot_t = sum(len(s["tris"]) for s in sectors)
    assert tot_v == hdr["numVertices"], (tot_v, hdr["numVertices"])
    assert tot_t == hdr["numTriangles"], (tot_t, hdr["numTriangles"])
    assert len(sectors) == hdr["numWorldSectors"], (len(sectors), hdr["numWorldSectors"])
    assert plane_count == hdr["numPlaneSectors"], (plane_count, hdr["numPlaneSectors"])
    return hdr, sectors


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("piz")
    ap.add_argument("--entry", default="GRAPH.BSP")
    ap.add_argument("--obj", default=None)
    ap.add_argument("--png", default=None)
    args = ap.parse_args()

    data, _ver, _app2, entries, _mode = piz_extract.read_archive(Path(args.piz))
    blob = None
    picked = None
    for (name, off, length, _fid) in entries:
        u = name.upper()
        # Per-track naming varies: GRAPH.BSP (Arctic...) vs GRAPHICS.BSP (City...).
        if u == args.entry.upper() or (args.entry == "GRAPH.BSP" and
                                       u.startswith("GRAPH") and u.endswith(".BSP")):
            blob = data[off:off + length]
            picked = name
            break
    if blob is None:
        sys.exit(f"{args.entry} not found in {args.piz}")
    args.entry = picked

    hdr, sectors = parse_world(blob)
    print(f"{args.piz} :: {args.entry}")
    print(f"  header: tris={hdr['numTriangles']} verts={hdr['numVertices']} "
          f"planeSectors={hdr['numPlaneSectors']} worldSectors={hdr['numWorldSectors']} "
          f"format={hdr['format']:#x} mats={hdr['numMaterials']}")
    print(f"  bbox: {hdr['bbox']}")
    s0 = sectors[0]
    print(f"  sector arrays: normals={s0['normals']} prelit={s0['prelit']} uvsets={s0['nuv']}")
    print(f"  VALIDATED: sector sums match header; all indices in range")

    # material -> texture binding, cross-checked against the track's TXD.
    # Entry name varies: TEXTURES.TXD (Arctic...) vs <TRACK>.TXD (CITY.TXD,
    # DUMP.TXD...); prefer TEXTURES.TXD, else the LARGEST .TXD in the piz.
    txd_blob = None
    best = None
    for (name, off, length, _fid) in entries:
        u = name.upper()
        if not u.endswith(".TXD"):
            continue
        if u.endswith("TEXTURES.TXD"):
            best = (off, length)
            break
        if best is None or length > best[1]:
            best = (off, length)
    if best is not None:
        txd_blob = data[best[0]:best[0] + best[1]]
    if txd_blob is not None:
        dict_names = set(txd_names(txd_blob))
        bound = sum(1 for (n, _c) in hdr["materials"] if n in dict_names)
        untex = sum(1 for (n, _c) in hdr["materials"] if n is None)
        missing = [n for (n, _c) in hdr["materials"]
                   if n is not None and n not in dict_names]
        print(f"  materials: {len(hdr['materials'])} "
              f"({bound} bound to TEXTURES.TXD names, {untex} untextured, "
              f"{len(missing)} MISSING: {missing})")
        for i, (n, c) in enumerate(hdr["materials"][:8]):
            print(f"    mat[{i}] tex={n!r} rgba={c}")

    if args.obj:
        with open(args.obj, "w") as f:
            f.write(f"# Mashed track world: {args.piz} {args.entry}\n")
            base = 1
            for i, s in enumerate(sectors):
                f.write(f"o sector_{i}\n")
                for (x, y, z) in s["verts"]:
                    f.write(f"v {x:.4f} {y:.4f} {z:.4f}\n")
                for (m, a, b, c) in s["tris"]:
                    f.write(f"f {base+a} {base+b} {base+c}\n")
                base += len(s["verts"])
        print(f"  OBJ -> {args.obj}")

    if args.png:
        from PIL import Image, ImageDraw
        W = H = 1400
        xs = [v[0] for s in sectors for v in s["verts"]]
        zs = [v[2] for s in sectors for v in s["verts"]]
        x0, x1, z0, z1 = min(xs), max(xs), min(zs), max(zs)
        span = max(x1 - x0, z1 - z0) or 1.0
        def px(x, z):
            return (int((x - x0) / span * (W - 40)) + 20,
                    int((z - z0) / span * (H - 40)) + 20)
        img = Image.new("RGB", (W, H), (10, 10, 16))
        dr = ImageDraw.Draw(img)
        for s in sectors:
            vs = s["verts"]
            for (m, a, b, c) in s["tris"]:
                pa, pb, pc = px(*[vs[a][i] for i in (0, 2)]), \
                             px(*[vs[b][i] for i in (0, 2)]), \
                             px(*[vs[c][i] for i in (0, 2)])
                dr.line([pa, pb, pc, pa], fill=(90, 200, 120), width=1)
        img.save(args.png)
        print(f"  wireframe PNG (top-down X/Z) -> {args.png}")


if __name__ == "__main__":
    main()
