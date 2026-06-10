#!/usr/bin/env python3
"""R3 — Mashed vehicle/prop DFF dumper (RW clump -> OBJ).

DFFs in VEHICLES/*.piz (and track props) are standard RenderWare 3.6 clump
streams (ver 0x1c02000a). Layout cross-referenced against the vendored librw
(re/prior_art/renderware/librw/src/{clump,frame,geometry}.cpp) and validated
against the bytes (counts + index ranges asserted before any output):

  CLUMP(0x10): STRUCT {numAtomics i32 [, numLights, numCameras]}
    FRAMELIST(0x0E): STRUCT { numFrames i32; frames[0x38 each]:
        f32x9 rotation, f32x3 position, i32 parent, i32 flags } + EXT per frame
    GEOMETRYLIST(0x1A): STRUCT {numGeometries i32} then GEOMETRY(0x0F) each:
        STRUCT { flags u32, numTris i32, numVerts i32, numMorphTargets i32;
                 [surfProps f32x3 only when version < 3.4]
                 if !(flags & NATIVE 0x01000000):
                   [PRELIT(0x08): RGBA u8x4 * nv]
                   texcoords f32x2 * nv per set (sets = (flags>>16)&0xff or
                     TEXTURED(0x04)->1 / TEXTURED2(0x80)->2)
                   tris u32x2 * nt: v0=hi16(w0) v1=lo16(w0) v2=hi16(w1)
                     mat=lo16(w1)
                 morphTargets: { sphere f32x4, hasVerts i32, hasNormals i32,
                   [verts f32x3*nv], [normals f32x3*nv] } }
        MATLIST(0x08) (same shape as the world's; texture names inside)
    ATOMIC(0x14): STRUCT {frameIndex, geoIndex, flags, unused}

OBJ export applies each atomic's frame WORLD transform (parent chain).

Usage:
  py -3.12 re/tools/dff_dump.py original/TOASTART/VEHICLES/Advantag.piz \
      --entry ADVANTAGE0.DFF [--obj out.obj] [--png out.png]
"""
import argparse
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import piz_extract  # noqa: E402
from track_dump import parse_matlist  # noqa: E402


def rc(d, off):
    t, sz, ver = struct.unpack_from("<III", d, off)
    return t, sz, ver, off + 12


def find_child(d, off, end, want):
    while off + 12 <= end:
        t, sz, _, p = rc(d, off)
        if t == want:
            return off
        off = p + sz
    return None


def parse_clump(d):
    t, sz, _, p = rc(d, 0)
    assert t == 0x10, f"root {t:#x} != CLUMP"
    end = p + sz
    t, ssz, _, sp = rc(d, p)
    assert t == 0x01
    num_atomics = struct.unpack_from("<i", d, sp)[0]
    p = sp + ssz

    # FRAMELIST
    fl = find_child(d, p, end, 0x0E)
    t, flsz, _, flp = rc(d, fl)
    t, fssz, _, fsp = rc(d, flp)
    assert t == 0x01
    nframes = struct.unpack_from("<i", d, fsp)[0]
    frames = []
    q = fsp + 4
    for _ in range(nframes):
        vals = struct.unpack_from("<12f2i", d, q)
        rot = vals[0:9]; pos = vals[9:12]; parent = vals[12]
        frames.append(dict(rot=rot, pos=pos, parent=parent))
        q += 0x38
    assert q - fsp == 4 + nframes * 0x38 <= fssz + 4, "framelist size"

    # GEOMETRYLIST
    gl = find_child(d, fl + 12 + flsz, end, 0x1A)
    t, glsz, _, glp = rc(d, gl)
    t, gssz, _, gsp = rc(d, glp)
    assert t == 0x01
    ngeo = struct.unpack_from("<i", d, gsp)[0]
    geos = []
    q = gsp + gssz
    for _gi in range(ngeo):
        t, gsz, _, gp = rc(d, q)
        assert t == 0x0F, f"geometry chunk {t:#x}"
        t, s2, _, g2 = rc(d, gp)
        assert t == 0x01
        flags, ntris, nverts, nmorph = struct.unpack_from("<Iiii", d, g2)
        r = g2 + 16
        verts, tris, uvs = [], [], None
        if not (flags & 0x01000000):  # !NATIVE
            if flags & 0x08:          # PRELIT
                r += nverts * 4
            nuv = (flags >> 16) & 0xFF
            if nuv == 0:
                nuv = 2 if (flags & 0x80) else (1 if (flags & 0x04) else 0)
            if nuv:
                uvs = list(struct.iter_unpack("<2f", d[r:r + nverts * 8]))
                r += nuv * nverts * 8
            for _ in range(ntris):
                w0, w1 = struct.unpack_from("<II", d, r); r += 8
                v0, v1 = w0 >> 16, w0 & 0xFFFF
                v2, mat = w1 >> 16, w1 & 0xFFFF
                assert v0 < nverts and v1 < nverts and v2 < nverts, "tri idx"
                tris.append((mat, v0, v1, v2))
            for mt in range(nmorph):
                r += 16  # bounding sphere
                has_v, has_n = struct.unpack_from("<ii", d, r); r += 8
                if has_v:
                    vv = list(struct.iter_unpack("<3f", d[r:r + nverts * 12]))
                    r += nverts * 12
                    if mt == 0:
                        verts = [tuple(v) for v in vv]
                if has_n:
                    r += nverts * 12
        assert r <= gp + s2 + 12 + gsz, "geometry overrun"
        # material list (names)
        ml = find_child(d, g2 + s2, gp + gsz, 0x08)
        mats = []
        if ml is not None:
            t, mlsz, _, mlp = rc(d, ml)
            mats = parse_matlist(d, mlp, mlsz)
        geos.append(dict(flags=flags, verts=verts, tris=tris, uvs=uvs,
                         mats=mats, ntris=ntris, nverts=nverts))
        assert len(verts) == nverts and len(tris) == ntris, "geo counts"
        q = gp + gsz

    # ATOMICs
    atomics = []
    q2 = gl + 12 + glsz
    while q2 + 12 <= end:
        t, asz, _, ap = rc(d, q2)
        if t == 0x14:
            t2, s2, _, a2 = rc(d, ap)
            fi, gi, fl_, _u = struct.unpack_from("<4i", d, a2)
            assert fi < nframes and gi < ngeo, "atomic indices"
            atomics.append((fi, gi))
        q2 += 12 + asz
    assert len(atomics) == num_atomics, (len(atomics), num_atomics)
    return frames, geos, atomics


def world_matrix(frames, idx):
    # column-major 3x3 rot + pos, parent chain
    def apply(m, v):
        return (m[0][0]*v[0] + m[0][3]*v[1] + m[0][6]*v[2] + m[1][0],
                m[0][1]*v[0] + m[0][4]*v[1] + m[0][7]*v[2] + m[1][1],
                m[0][2]*v[0] + m[0][5]*v[1] + m[0][8]*v[2] + m[1][2])
    chain = []
    i = idx
    while i >= 0:
        chain.append(i)
        i = frames[i]["parent"]
    def xform(v):
        for fi in chain:
            f = frames[fi]
            r, p = f["rot"], f["pos"]
            v = (r[0]*v[0] + r[3]*v[1] + r[6]*v[2] + p[0],
                 r[1]*v[0] + r[4]*v[1] + r[7]*v[2] + p[1],
                 r[2]*v[0] + r[5]*v[1] + r[8]*v[2] + p[2])
        return v
    return xform


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("piz")
    ap.add_argument("--entry", required=True)
    ap.add_argument("--obj", default=None)
    args = ap.parse_args()
    data, _v, _a, entries, _m = piz_extract.read_archive(Path(args.piz))
    blob = None
    for (name, off, length, _f) in entries:
        if name.upper() == args.entry.upper():
            blob = data[off:off + length]
            break
    if blob is None:
        sys.exit(f"{args.entry} not found")
    frames, geos, atomics = parse_clump(blob)
    tot_v = sum(g["nverts"] for g in geos)
    tot_t = sum(g["ntris"] for g in geos)
    print(f"{args.piz} :: {args.entry}")
    print(f"  frames={len(frames)} geometries={len(geos)} atomics={len(atomics)}")
    print(f"  totals: verts={tot_v} tris={tot_t}")
    mats0 = [n for (n, _c) in (geos[0]['mats'] or [])][:6]
    print(f"  geo[0]: flags={geos[0]['flags']:#x} v={geos[0]['nverts']} "
          f"t={geos[0]['ntris']} mats={mats0}")
    print(f"  VALIDATED: counts + index ranges + atomic bindings")
    if args.obj:
        with open(args.obj, "w") as f:
            f.write(f"# {args.piz} {args.entry}\n")
            base = 1
            for (fi, gi) in atomics:
                xf = world_matrix(frames, fi)
                g = geos[gi]
                f.write(f"o atomic_f{fi}_g{gi}\n")
                for v in g["verts"]:
                    x, y, z = xf(v)
                    f.write(f"v {x:.4f} {y:.4f} {z:.4f}\n")
                for (m, a, b, c) in g["tris"]:
                    f.write(f"f {base+a} {base+b} {base+c}\n")
                base += len(g["verts"])
        print(f"  OBJ -> {args.obj}")


if __name__ == "__main__":
    main()
