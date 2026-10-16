"""librwrest item U: which track assets carry RW UV-animation material bindings
(rwID_UVANIMPLUGIN 0x135 inside a material's extension), and what the track's .UVA dictionary
(rwID_UVANIMDICT 0x2B) holds. Walks every chunk of every GRAPH*.BSP / *.DFF / *.UVA in each
original/TOASTART/TRACKS/*.piz. Read-only.

    py -3.12 uvanim_scan.py [out.txt]
"""
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent
sys.path.insert(0, str(REPO / "re" / "tools"))
import piz_extract as P  # noqa: E402

ROOT = next((p for p in (REPO, *REPO.parents) if (p / "original" / "TOASTART").exists()), REPO)
TRACKS = ROOT / "original" / "TOASTART" / "TRACKS"
CONTAINERS = {0x0E, 0x03, 0x0F, 0x10, 0x14, 0x07, 0x08, 0x1A, 0x0B, 0x09, 0x0A, 0x2B}


def walk(d, off, end, depth, out, path):
    while off + 12 <= end:
        cid, size, ver = struct.unpack_from("<III", d, off)
        p = off + 12
        if size > end - p:
            return
        if cid == 0x135:
            # UVAnim material extension: a STRUCT chunk (0x01) holding u32 mask, then per set
            # bit a 32-byte animation name
            q0 = p
            if size >= 12 and struct.unpack_from("<I", d, p)[0] == 0x01:
                q0 = p + 12
            mask = struct.unpack_from("<I", d, q0)[0] if q0 + 4 <= p + size else 0
            names = []
            q = q0 + 4
            for b in range(8):
                if mask & (1 << b) and q + 32 <= p + size:
                    names.append(d[q:q + 32].split(b"\0")[0].decode("latin1"))
                    q += 32
            out.append((path, "binding", mask, names))
        elif cid == 0x1B and depth >= 1:
            # RtAnimAnimation inside a UVA dict: ver, scheme, nframes, flags, duration, ?, name[32]
            if size >= 56:
                ver_, scheme, nf, fl, dur = struct.unpack_from("<IIIIf", d, p)
                name = d[p + 24:p + 56].split(b"\0")[0].decode("latin1")
                out.append((path, "uvanim", scheme, f"{name} frames={nf} dur={dur:.3f}"))
        if cid in CONTAINERS and size >= 12:
            walk(d, p, p + size, depth + 1, out, path)
        off = p + size


def main():
    lines = []
    for pz in sorted(TRACKS.glob("*.piz")):
        try:
            data, _v, _a, ents, _m = P.read_archive(pz)
        except Exception as e:
            lines.append(f"{pz.name}: read failed {e}")
            continue
        hits = []
        for (n, o, l, i) in ents:
            nm = (n.decode("latin1") if isinstance(n, bytes) else n).rstrip("\0")
            u = nm.upper()
            if not (u.endswith(".BSP") or u.endswith(".DFF") or u.endswith(".UVA")):
                continue
            blob = data[o:o + l]
            walk(blob, 0, len(blob), 0, hits, nm)
        bsp_bind = [h for h in hits if h[1] == "binding" and h[0].upper().endswith(".BSP")]
        dff_bind = [h for h in hits if h[1] == "binding" and h[0].upper().endswith(".DFF")]
        anims = [h for h in hits if h[1] == "uvanim"]
        lines.append(f"== {pz.name}: world-BSP bindings={len(bsp_bind)} DFF bindings={len(dff_bind)} "
                     f"UVA anims={len(anims)}")
        for h in bsp_bind + dff_bind + anims:
            lines.append(f"   {h[0]:<24} {h[1]:<8} {h[2]} {h[3]}")
    s = "\n".join(lines)
    print(s)
    if len(sys.argv) > 1:
        Path(sys.argv[1]).write_text(s + "\n")


if __name__ == "__main__":
    main()
