"""Dump RenderWare material lists (DFF / BSP) with their MatFX (0x120) material extensions.

Usage: py -3.12 rwmat_dump.py <file.dff|file.bsp> [...]

Walks the RW chunk tree, and for every Material (0x07) prints its colour, texture name and
the decoded MatFX material extension (rwID_MATERIALEFFECTSPLUGIN 0x120): effect type, and per
effect slot the DUAL blend modes + texture / ENVMAP coefficient + texture / BUMP fields.
Also prints Geometry (0x0F) flags and numTexCoordSets, Atomic (0x14) MatFX extension, and
World (0x0B) struct flags. Field layout follows librw src/matfx.cpp readMaterialMatFX.
"""
import struct
import sys

NAMES = {0x01: "STRUCT", 0x02: "STRING", 0x03: "EXT", 0x06: "TEXTURE", 0x07: "MATERIAL",
         0x08: "MATLIST", 0x09: "ATOMICSECT", 0x0A: "PLANESECT", 0x0B: "WORLD", 0x0E: "FRAMELIST",
         0x0F: "GEOMETRY", 0x10: "CLUMP", 0x14: "ATOMIC", 0x1A: "GEOMLIST", 0x120: "MATFX",
         0x135: "UVANIM", 0x116: "SKIN", 0x50E: "BINMESH"}


def chunks(buf, off, end):
    while off + 12 <= end:
        cid, size, lib = struct.unpack_from("<III", buf, off)
        yield cid, off + 12, min(off + 12 + size, end), lib
        off += 12 + size


def tex_info(buf, a, b):
    out = {}
    strs = []
    for cid, p, e, _ in chunks(buf, a, b):
        if cid == 0x01 and e - p >= 4:
            fl, = struct.unpack_from("<I", buf, p)
            out["filter"] = fl & 0xFF
            out["addrU"] = (fl >> 8) & 0xF
            out["addrV"] = (fl >> 12) & 0xF
        elif cid == 0x02:
            strs.append(buf[p:e].split(b"\0")[0].decode("latin1"))
    out["name"] = strs[0] if strs else ""
    out["mask"] = strs[1] if len(strs) > 1 else ""
    return out


def matfx(buf, p, e):
    """librw matfx.cpp readMaterialMatFX: u32 type, then 2 x (u32 effType, effect body)."""
    typ, = struct.unpack_from("<I", buf, p)
    q = p + 4
    res = {"type": typ, "slots": []}
    for _ in range(2):
        if q + 4 > e:
            break
        et, = struct.unpack_from("<I", buf, q)
        q += 4
        slot = {"eff": et}
        if et in (1,):  # BUMPMAP: float bumpiness, i32 hasBumpTex, [tex], i32 hasTex, [tex]
            slot["bumpiness"], = struct.unpack_from("<f", buf, q); q += 4
            for key in ("bumpedTex", "tex"):
                has, = struct.unpack_from("<i", buf, q); q += 4
                if has:
                    cid, size, _ = struct.unpack_from("<III", buf, q)
                    slot[key] = tex_info(buf, q + 12, q + 12 + size); q += 12 + size
        elif et == 2:  # ENVMAP: float coef, i32 fbAlpha, i32 hasTex, [tex]
            slot["coef"], slot["fbAlpha"], has = struct.unpack_from("<fii", buf, q); q += 12
            if has:
                cid, size, _ = struct.unpack_from("<III", buf, q)
                slot["tex"] = tex_info(buf, q + 12, q + 12 + size); q += 12 + size
        elif et == 4:  # DUAL: i32 src, i32 dst, i32 hasTex, [tex]
            slot["src"], slot["dst"], has = struct.unpack_from("<iii", buf, q); q += 12
            if has:
                cid, size, _ = struct.unpack_from("<III", buf, q)
                slot["tex"] = tex_info(buf, q + 12, q + 12 + size); q += 12 + size
        elif et == 5:  # UVTRANSFORM: no payload
            pass
        res["slots"].append(slot)
    return res


def material(buf, a, b, idx):
    col = None
    tex = None
    exts = []
    for cid, p, e, _ in chunks(buf, a, b):
        if cid == 0x01:
            fl, r, g, bl, al, unused, textured = struct.unpack_from("<IBBBBIi", buf, p)
            col = (r, g, bl, al)
            amb = struct.unpack_from("<fff", buf, p + 16) if e - p >= 28 else None
        elif cid == 0x06:
            tex = tex_info(buf, p, e)
        elif cid == 0x03:
            for xc, xp, xe, _ in chunks(buf, p, e):
                if xc == 0x120:
                    exts.append(("MATFX", matfx(buf, xp, xe)))
                else:
                    exts.append((NAMES.get(xc, hex(xc)), xe - xp))
    print(f"    mat[{idx}] rgba={col} surf={tuple(round(x, 3) for x in amb) if amb else None} "
          f"tex={tex and tex['name']!r} mask={tex and tex['mask']!r} "
          f"filt={tex and tex.get('filter')} addr={tex and (tex.get('addrU'), tex.get('addrV'))}")
    for nm, v in exts:
        print(f"      ext {nm}: {v}")


def walk(buf, a, b, depth, ctx):
    for cid, p, e, lib in chunks(buf, a, b):
        nm = NAMES.get(cid, hex(cid))
        if cid == 0x08:
            print("  " * depth + f"MATLIST ({ctx})")
            idx = 0
            for mc, mp, me, _ in chunks(buf, p, e):
                if mc == 0x07:
                    material(buf, mp, me, idx)
                    idx += 1
            continue
        if cid == 0x0F:
            for sc, sp, se, _ in chunks(buf, p, e):
                if sc == 0x01:
                    flags, nt, nv, nm_ = struct.unpack_from("<IIII", buf, sp)
                    print("  " * depth + f"GEOMETRY flags=0x{flags:x} numTexCoordSets={(flags >> 16) & 0xff} tris={nt} verts={nv}")
                    break
            walk(buf, p, e, depth + 1, "geometry")
            continue
        if cid == 0x0B:
            for sc, sp, se, _ in chunks(buf, p, e):
                if sc == 0x01:
                    # rpWorld struct: i32 rootIsWorldSector, V3d invOrigin, then (3.5+) numTris,
                    # numVerts, numPlaneSectors, numWorldSectors, colSectorSize, format flags
                    vals = struct.unpack_from("<i3fIIIIII", buf, sp)
                    print("  " * depth + f"WORLD struct {vals} format=0x{vals[-1]:x} "
                          f"numTexCoordSets={(vals[-1] >> 16) & 0xff}")
                    break
            walk(buf, p, e, depth + 1, "world")
            continue
        if cid == 0x14:
            for sc, sp, se, _ in chunks(buf, p, e):
                if sc == 0x03:
                    for xc, xp, xe, _ in chunks(buf, sp, se):
                        if xc == 0x120:
                            print("  " * depth + f"ATOMIC MatFX enabled={struct.unpack_from('<I', buf, xp)[0]}")
            continue
        if cid in (0x10, 0x1A, 0x03, 0x0A, 0x09) and cid != 0x09:
            walk(buf, p, e, depth + 1, nm.lower())
        elif cid == 0x09:
            pass  # atomic sectors: geometry lives inline, matlist is world-level


for path in sys.argv[1:]:
    buf = open(path, "rb").read()
    print(f"== {path} ({len(buf)} bytes)")
    walk(buf, 0, len(buf), 0, "top")
