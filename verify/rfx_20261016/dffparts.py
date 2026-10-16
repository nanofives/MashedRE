"""rfx: dump a DFF's frame hierarchy and atomics with their geometry user-data part code.

usage: py -3.12 verify/rfx_20261016/dffparts.py <piz> <ENTRY.DFF> [--json out.json]

Per atomic: frame index, the frame's local matrix (rot rows right/up/at + pos, as stored in the
RW frame list) and its clump-space LTM (parent chain), geometry user-data sets (rpUSERDATA 0x11F;
the part code is set '0.tv_part_id', memory part-code-is-geometry-user-data). Read-only.
"""
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "tools"))
import piz_extract  # noqa: E402


def chunks(b, off, end):
    while off + 12 <= end:
        t, s, v = struct.unpack_from("<III", b, off)
        yield t, off + 12, off + 12 + s
        off += 12 + s


def find(b, off, end, typ):
    for t, a, z in chunks(b, off, end):
        if t == typ:
            return a, z
    return None


def userdata(b, a, z):
    n, = struct.unpack_from("<i", b, a); p = a + 4
    out = {}
    for _ in range(n):
        ln, = struct.unpack_from("<i", b, p); p += 4
        name = b[p:p + ln].split(b"\0")[0].decode("latin-1"); p += ln
        typ, ne = struct.unpack_from("<ii", b, p); p += 8
        vals = []
        for _ in range(ne):
            if typ == 1:
                vals.append(struct.unpack_from("<i", b, p)[0]); p += 4
            elif typ == 2:
                vals.append(struct.unpack_from("<f", b, p)[0]); p += 4
            elif typ == 3:
                sl, = struct.unpack_from("<i", b, p); p += 4
                vals.append(b[p:p + sl].split(b"\0")[0].decode("latin-1")); p += sl
        out[name] = vals
    return out


def mul(local, parent):
    # RW row-vector: ltm = local * parent ; rows right, up, at, pos
    r = []
    for i in range(3):
        r.append([sum(local[i][k] * parent[k][j] for k in range(3)) for j in range(3)])
    pos = [sum(local[3][k] * parent[k][j] for k in range(3)) + parent[3][j] for j in range(3)]
    return r + [pos]


def parse(b):
    a, z = find(b, 0, len(b), 0x10)
    st = find(b, a, z, 1)
    fl = find(b, a, z, 0x0E)
    fs = find(b, fl[0], fl[1], 1)
    nf, = struct.unpack_from("<i", b, fs[0])
    frames = []
    for i in range(nf):
        o = fs[0] + 4 + i * 56
        v = struct.unpack_from("<12fii", b, o)
        rows = [list(v[0:3]), list(v[3:6]), list(v[6:9]), list(v[9:12])]
        frames.append({"local": rows, "parent": v[12]})
    for f in frames:
        chain, cur = [], f
        m = f["local"]
        p = f["parent"]
        while p >= 0:
            m = mul(m, frames[p]["local"])
            p = frames[p]["parent"]
        f["ltm"] = m
    gl = find(b, a, z, 0x1A)
    geos = []
    for t, ga, gz in chunks(b, gl[0], gl[1]):
        if t != 0x0F:
            continue
        ud = {}
        ext = find(b, ga, gz, 3)
        if ext:
            u = find(b, ext[0], ext[1], 0x11F)
            if u:
                ud = userdata(b, *u)
        gst = find(b, ga, gz, 1)
        flags, nt, nv, nm = struct.unpack_from("<Iiii", b, gst[0])
        geos.append({"ud": ud, "tris": nt, "verts": nv, "flags": flags})
    atoms = []
    for t, aa, az in chunks(b, a, z):
        if t != 0x14:
            continue
        s = find(b, aa, az, 1)
        fi, gi, fl_, _ = struct.unpack_from("<iiii", b, s[0])
        atoms.append({"frame": fi, "geo": gi, "flags": fl_, "ud": geos[gi]["ud"],
                      "tris": geos[gi]["tris"], "local": frames[fi]["local"],
                      "ltm": frames[fi]["ltm"], "parent": frames[fi]["parent"]})
    return frames, atoms


def entry_bytes(piz, entry):
    data, _v, _a, entries, _m = piz_extract.read_archive(Path(piz))
    for name, off, length, _fid in entries:
        if name.upper() == entry.upper():
            return data[off:off + length]
    raise SystemExit(f"{entry} not in {piz}")


def main():
    piz, entry = sys.argv[1], sys.argv[2]
    frames, atoms = parse(entry_bytes(piz, entry))
    for i, a in enumerate(atoms):
        print(f"atom {i:2d} frame {a['frame']:2d} parent {a['parent']:2d} tris {a['tris']:4d} ud {a['ud']} "
              f"local_pos {[round(x, 4) for x in a['local'][3]]} ltm_pos {[round(x, 4) for x in a['ltm'][3]]}")
    if "--json" in sys.argv:
        Path(sys.argv[sys.argv.index("--json") + 1]).write_text(json.dumps(
            {"frames": frames, "atoms": atoms}, indent=1))


if __name__ == "__main__":
    main()
