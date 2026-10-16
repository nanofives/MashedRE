#!/usr/bin/env python3
"""Print the RW frame list of a DFF (read-only): per frame the 3x3 rotation, the position, the parent
index, and the frame each atomic is bound to.

Usage: py -3.12 verify/rprops_20261016/dff_frames.py <piz> <ENTRY.DFF>
   or: py -3.12 verify/rprops_20261016/dff_frames.py --file <extracted.dff>
Chunk ids: 0x10 Clump, 0x0e FrameList, 0x01 Struct, 0x14 Atomic.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import piz_extract  # noqa: E402


def chunks(d, o, end):
    while o + 12 <= end:
        t, s, _v = struct.unpack_from('<III', d, o)
        yield t, o + 12, s
        o += 12 + s


def load(args):
    if args[0] == '--file':
        return Path(args[1]).read_bytes()
    data, _v, _a, entries, _m = piz_extract.read_archive(Path(args[0]))
    for e in entries:
        nm = e['name'] if isinstance(e, dict) else getattr(e, 'name', None)
        if nm and nm.upper() == args[1].upper():
            off = e['offset'] if isinstance(e, dict) else e.offset
            sz = e['size'] if isinstance(e, dict) else e.size
            return data[off:off + sz]
    raise SystemExit("entry not found")


def main():
    d = load(sys.argv[1:])
    for t, o, s in chunks(d, 0, len(d)):
        if t != 0x10:
            continue
        atomics = []
        for t2, o2, s2 in chunks(d, o, o + s):
            if t2 == 0x0e:
                for t3, o3, _s3 in chunks(d, o2, o2 + s2):
                    if t3 == 0x01:
                        n, = struct.unpack_from('<I', d, o3)
                        for i in range(n):
                            f = struct.unpack_from('<12f2i', d, o3 + 4 + i * 56)
                            print(f"frame {i}: rot=({f[0]:.4f},{f[1]:.4f},{f[2]:.4f} | {f[3]:.4f},{f[4]:.4f},{f[5]:.4f} | "
                                  f"{f[6]:.4f},{f[7]:.4f},{f[8]:.4f}) pos=({f[9]:.5f},{f[10]:.5f},{f[11]:.5f}) "
                                  f"parent={f[12]} flags={f[13]:#x}")
                        break
            elif t2 == 0x14:
                for t3, o3, _s3 in chunks(d, o2, o2 + s2):
                    if t3 == 0x01:
                        fr, gi, fl, _ = struct.unpack_from('<4I', d, o3)
                        atomics.append((fr, gi, fl))
                        break
        print("atomics (frame, geom, flags):", atomics)


if __name__ == '__main__':
    main()
