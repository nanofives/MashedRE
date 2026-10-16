"""carlod: does reading the world header's format flags change any BSP sector's per-vertex layout?

Track/TrackWorld.cpp `World::Parse` decides each sector's arrays (normals, prelit, UV sets) by SIZE alone,
which is ambiguous: normals alone (12 B/vertex) = prelit + 1 UV set (4 + 8); normals + prelit (16) = 2 UV
sets (16). The world header STRUCT carries the RpWorld `format` at payload +0x24 (0x08 PRELIT, 0x10
NORMALS, bits 16..23 = UV sets). RenderWare stores a SECTOR's normals as 4-byte packed RpVertexNormal (x, y, z,
pad), not 3 floats, so the flags' layout is nv * (4 * normals + 4 * prelit + 8 * uv). This walks every BSP of every
track piz exactly as Parse does and compares the size-only solver's (normals, prelit, uv) with the flags' one on
every NON-EMPTY sector where the flags' layout satisfies the size equation (empty sectors, nv = 0, satisfy every
equation and carry nothing).

    py -3.12 bsp_layout_check.py
"""
import struct
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
ORIG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\TOASTART\TRACKS")
TMP = Path(sys.argv[1]) if len(sys.argv) > 1 else HERE / "_bsp_tmp"


def chunk(d, o):
    cid, size, _ = struct.unpack_from("<III", d, o)
    return cid, o + 12, size


def solver(rem, nv):
    for normals in range(2):
        for prelit in range(2):
            for uv in range(3):
                if rem == normals * nv * 12 + prelit * nv * 4 + uv * nv * 8:
                    return (normals, prelit, uv)
    return None


def walk(d, name):
    cid, wp, wsize = chunk(d, 0)
    sid, sp, ssize = chunk(d, wp)
    fmt, = struct.unpack_from("<I", d, sp + 0x24)
    p = sp + ssize
    mid, mp, msize = chunk(d, p)
    p = mp + msize
    stack = [p]
    diffs, leaves = [], 0
    ff = ((1 if fmt & 0x10 else 0), (1 if fmt & 0x08 else 0), (fmt >> 16) & 0xff)
    while stack:
        o = stack.pop()
        cid, cp, csize = chunk(d, o)
        if cid == 0x0A:
            _, psp, pss = chunk(d, cp)
            left = psp + pss
            lid, lp, ls = chunk(d, left)
            stack.append(lp + ls)
            stack.append(left)
            continue
        assert cid == 0x09, hex(cid)
        _, ap, asz = chunk(d, cp)
        base, nt, nv = struct.unpack_from("<III", d, ap)
        rem = (asz - 0x2C) - (nv * 12 + nt * 8)
        sol = solver(rem, nv)
        leaves += 1
        flags_ok = nv > 0 and ff[2] <= 2 and rem == ff[0] * nv * 4 + ff[1] * nv * 4 + ff[2] * nv * 8
        if flags_ok and sol != ff:
            diffs.append((nv, sol, ff))
        if nv > 0 and sol is None and not flags_ok:
            diffs.append((nv, "UNSOLVED", ff))
    return fmt, ff, leaves, diffs


def main():
    TMP.mkdir(exist_ok=True)
    tot = changed = 0
    for piz in sorted(ORIG.glob("*.piz")):
        out = TMP / piz.stem
        out.mkdir(exist_ok=True)
        subprocess.run([sys.executable, str(ROOT / "re" / "tools" / "piz_extract.py"), "extract", str(piz), "-o", str(out),
                        "--filter", "*.BSP"], capture_output=True, text=True)
        seen = set()
        for f in sorted(list(out.rglob("*.BSP")) + list(out.rglob("*.bsp"))):
            if f.name.lower() in seen:
                continue
            seen.add(f.name.lower())
            try:
                fmt, ff, leaves, diffs = walk(f.read_bytes(), f.name)
            except Exception as e:  # noqa: BLE001
                print(f"{piz.stem:<12}{f.name:<22} walk failed: {e}")
                continue
            tot += leaves
            changed += len(diffs)
            tag = "SAME" if not diffs else f"CHANGES on {len(diffs)} of {leaves} sectors: solver->flags {diffs[0][1]}->{diffs[0][2]}"
            print(f"{piz.stem:<12}{f.name:<22} fmt={fmt:#010x} flags(n,p,uv)={ff} sectors={leaves} {tag}")
    print(f"sectors {tot}, sectors whose layout the flags would change: {changed}")


if __name__ == "__main__":
    main()
