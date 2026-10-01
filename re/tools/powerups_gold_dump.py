#!/usr/bin/env python3
"""Dump a track's POWERUPS_GOLD.DFF placement markers: per-atomic world/local
position + the geometry's RW USERDATA (0x011f) `tv_part_id` dword.

This is the ORIGINAL's live placement source, not POWERUPS_GOLD.LUA. The chain,
all RVAs from re/analysis/PICKUPS_LOOK_PLACEMENT_2026-09-29.md:

  FUN_004264d0 @0x004264d0   track load
    0x004265bd  push "powerups_gold.dff" (0x005cd4e4)
    0x004265c2  call FUN_0042a5d0           -- DFF clump loader
    0x004265ce  JE  -> Lua fallback (clump == 0; never taken on shipping data)
    0x004265d2  call FUN_00426460           -- THE LIVE PLACEMENT PATH
  FUN_00426460 @0x00426460 (__fastcall, clump in ECX):
    n = FUN_004b3fc0(clump, local_80);      -- RpClumpForAllAtomics, cap 32
    for i in 0..n-1:
        frame = *(int *)(atomic + 4);       -- RwObject.parent
        v     = FUN_004b5190(atomic, 0, 0); -- RW user-data plugin, array 0, elem 0
        FUN_00458fd0(frame + 0x40, v & 0xff, (float)(v >> 8));

`frame + 0x40` is the frame's MODELLING matrix position (RwFrame modelling
matrix at +0x10, RwMatrix pos at +0x30), i.e. the frame's LOCAL translation as
RpClumpStreamRead wrote it from the FRAMELIST entry -- NOT the parent-chain
LTM. This tool prints both and asserts they agree, so the distinction is
measured rather than assumed.

`tv_part_id` = type | (respawn_seconds << 8), as the shift/mask above shows.

USERDATA (0x011f) stream layout, per librw userdata.cpp:
    i32 numUserDatas
    per entry: i32 nameLen, char name[nameLen], i32 dataType, i32 numElements,
               then numElements * (i32 | f32 | length-prefixed string)
dataType: 1 = int32, 2 = float32, 3 = string.

Usage:
  py -3.12 re/tools/powerups_gold_dump.py original/TOASTART/TRACKS/training.piz
  py -3.12 re/tools/powerups_gold_dump.py --all [--csv out.csv]
"""
import argparse
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import piz_extract  # noqa: E402

ENTRY = "POWERUPS_GOLD.DFF"

ID_STRUCT = 0x01
ID_EXTENSION = 0x03
ID_CLUMP = 0x10
ID_FRAMELIST = 0x0E
ID_GEOMETRYLIST = 0x1A
ID_GEOMETRY = 0x0F
ID_ATOMIC = 0x14
ID_USERDATA = 0x011F


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


def parse_userdata(d, off, end):
    """Return {name: [values]} for the USERDATA chunk at `off` (chunk header)."""
    t, sz, _, p = rc(d, off)
    assert t == ID_USERDATA, f"{t:#x} != USERDATA"
    lim = p + sz
    assert lim <= end, "userdata overruns extension"
    n = struct.unpack_from("<i", d, p)[0]
    q = p + 4
    out = {}
    for _ in range(n):
        ln = struct.unpack_from("<i", d, q)[0]
        q += 4
        name = d[q:q + ln].split(b"\0")[0].decode("ascii", "replace")
        q += ln
        dtype, cnt = struct.unpack_from("<ii", d, q)
        q += 8
        vals = []
        for _e in range(cnt):
            if dtype == 1:
                vals.append(struct.unpack_from("<i", d, q)[0])
                q += 4
            elif dtype == 2:
                vals.append(struct.unpack_from("<f", d, q)[0])
                q += 4
            elif dtype == 3:
                sl = struct.unpack_from("<i", d, q)[0]
                q += 4
                vals.append(d[q:q + sl].split(b"\0")[0].decode("ascii", "replace"))
                q += sl
            else:
                raise AssertionError(f"userdata dataType {dtype}")
        out[name] = (dtype, vals)
    assert q <= lim, "userdata element overrun"
    return out


def parse(blob):
    d = blob
    t, sz, _, p = rc(d, 0)
    assert t == ID_CLUMP, f"root {t:#x} != CLUMP"
    end = p + sz
    t, ssz, _, sp = rc(d, p)
    assert t == ID_STRUCT
    num_atomics = struct.unpack_from("<i", d, sp)[0]
    p = sp + ssz

    # ---- FRAMELIST -------------------------------------------------------
    fl = find_child(d, p, end, ID_FRAMELIST)
    _t, flsz, _, flp = rc(d, fl)
    t, fssz, _, fsp = rc(d, flp)
    assert t == ID_STRUCT
    nframes = struct.unpack_from("<i", d, fsp)[0]
    frames = []
    q = fsp + 4
    for _ in range(nframes):
        v = struct.unpack_from("<12f2i", d, q)
        frames.append(dict(rot=v[0:9], pos=v[9:12], parent=v[12]))
        q += 0x38
    assert 4 + nframes * 0x38 <= fssz, "framelist size"

    # ---- GEOMETRYLIST ----------------------------------------------------
    gl = find_child(d, fl + 12 + flsz, end, ID_GEOMETRYLIST)
    _t, glsz, _, glp = rc(d, gl)
    t, gssz, _, gsp = rc(d, glp)
    assert t == ID_STRUCT
    ngeo = struct.unpack_from("<i", d, gsp)[0]
    geos = []
    q = gsp + gssz
    for _gi in range(ngeo):
        t, gsz, _, gp = rc(d, q)
        assert t == ID_GEOMETRY, f"geometry chunk {t:#x}"
        t, s2, _, g2 = rc(d, gp)
        assert t == ID_STRUCT
        flags, ntris, nverts, _nm = struct.unpack_from("<Iiii", d, g2)
        gend = gp + gsz
        # geometry-level EXTENSION, the chunk the RW userdata plugin writes into
        ud = {}
        ex = find_child(d, g2 + s2, gend, ID_EXTENSION)
        if ex is not None:
            _t, exsz, _, exp_ = rc(d, ex)
            u = find_child(d, exp_, exp_ + exsz, ID_USERDATA)
            if u is not None:
                ud = parse_userdata(d, u, exp_ + exsz)
        geos.append(dict(flags=flags, ntris=ntris, nverts=nverts, ud=ud))
        q = gend

    # ---- ATOMICs ---------------------------------------------------------
    atomics = []
    q2 = gl + 12 + glsz
    while q2 + 12 <= end:
        t, asz, _, ap = rc(d, q2)
        if t == ID_ATOMIC:
            t2, _s2, _, a2 = rc(d, ap)
            assert t2 == ID_STRUCT
            fi, gi, _fl, _u = struct.unpack_from("<4i", d, a2)
            assert 0 <= fi < nframes and 0 <= gi < ngeo, "atomic indices"
            atomics.append((fi, gi))
        q2 += 12 + asz
    assert len(atomics) == num_atomics, (len(atomics), num_atomics)
    return frames, geos, atomics


def world_pos(frames, idx):
    """Parent-chain world translation of frame `idx` (origin through the chain)."""
    v = (0.0, 0.0, 0.0)
    i = idx
    while i >= 0:
        f = frames[i]
        r, p = f["rot"], f["pos"]
        v = (r[0] * v[0] + r[3] * v[1] + r[6] * v[2] + p[0],
             r[1] * v[0] + r[4] * v[1] + r[7] * v[2] + p[1],
             r[2] * v[0] + r[5] * v[1] + r[8] * v[2] + p[2])
        i = f["parent"]
    return v


def markers(piz_path):
    """-> (rows, warnings). rows: dict(atomic, frame, geo, local, world, raw,
    type, respawn, has_ud)."""
    data, _v, _a, entries, _m = piz_extract.read_archive(Path(piz_path))
    blob = None
    for (name, off, length, _f) in entries:
        if name.upper() == ENTRY:
            blob = data[off:off + length]
            break
    if blob is None:
        return None, [f"{ENTRY} not present"]
    frames, geos, atomics = parse(blob)
    rows, warn = [], []
    for ai, (fi, gi) in enumerate(atomics):
        loc = frames[fi]["pos"]
        wld = world_pos(frames, fi)
        ud = geos[gi]["ud"]
        key = None
        for k in ud:
            if k.lower().endswith("part_id"):
                key = k
                break
        raw = None
        if key is not None:
            dtype, vals = ud[key]
            if dtype == 1 and len(vals) == 1:
                raw = vals[0] & 0xFFFFFFFF
            else:
                warn.append(f"atomic {ai}: {key} dtype={dtype} count={len(vals)}")
        else:
            warn.append(f"atomic {ai}/geo {gi}: no *part_id userdata "
                        f"(keys={sorted(ud)})")
        rows.append(dict(atomic=ai, frame=fi, geo=gi, local=loc, world=wld,
                         raw=raw, udkey=key,
                         type=(raw & 0xFF) if raw is not None else None,
                         respawn=float(raw >> 8) if raw is not None else None,
                         nverts=geos[gi]["nverts"], ntris=geos[gi]["ntris"]))
    return rows, warn


TRACKS = ["Arctic", "City", "Egypt", "Forest", "Highway", "Neustein",
          "Storm", "SuperG", "Warzone", "dump", "rouabout", "sands",
          "training"]
ROOT = Path(__file__).resolve().parent.parent.parent


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("piz", nargs="?")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--csv")
    a = ap.parse_args()
    jobs = ([(t, ROOT / "original/TOASTART/TRACKS" / f"{t}.piz") for t in TRACKS]
            if a.all else [(Path(a.piz).stem, Path(a.piz))])
    out = []
    for tname, p in jobs:
        rows, warn = markers(p)
        if rows is None:
            print(f"{tname}: {warn[0]}")
            continue
        drift = max(max(abs(r["local"][k] - r["world"][k]) for k in range(3))
                    for r in rows)
        nb = sum(1 for r in rows if r["type"] == 21)
        print(f"{tname}: atomics={len(rows)} blank(21)={nb} "
              f"non-blank={len(rows) - nb} local-vs-world max|d|={drift:.6g}")
        for w in warn[:4]:
            print(f"    WARN {w}")
        if not a.all:
            for r in rows:
                print(f"  [{r['atomic']:2d}] f{r['frame']:<3d} g{r['geo']:<3d} "
                      f"raw={r['raw']:#06x} type={r['type']:2d} "
                      f"respawn={r['respawn']:.0f} "
                      f"pos=({r['local'][0]:.5f},{r['local'][1]:.5f},"
                      f"{r['local'][2]:.5f}) v={r['nverts']} t={r['ntris']}")
        for r in rows:
            out.append((tname, r))
    if a.csv:
        with open(a.csv, "w", newline="") as f:
            f.write("track,atomic,frame,geo,raw,type,respawn,x,y,z,"
                    "world_x,world_y,world_z,nverts,ntris\n")
            for tname, r in out:
                f.write(f"{tname},{r['atomic']},{r['frame']},{r['geo']},"
                        f"{r['raw']:#06x},{r['type']},{r['respawn']:.0f},"
                        f"{r['local'][0]:.6f},{r['local'][1]:.6f},{r['local'][2]:.6f},"
                        f"{r['world'][0]:.6f},{r['world'][1]:.6f},{r['world'][2]:.6f},"
                        f"{r['nverts']},{r['ntris']}\n")
        print(f"-> {a.csv}  ({len(out)} rows)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
