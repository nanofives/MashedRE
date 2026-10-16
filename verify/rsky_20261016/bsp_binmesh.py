"""rsky: list, per atomic sector of an RW world stream (.bsp), the BinMesh PLG (chunk 0x50E) mesh order.

    py -3.12 verify\\rsky_20261016\\bsp_binmesh.py <file.bsp>

Chunks: 0x0B WORLD, 0x09 ATOMIC SECTOR, 0x0C PLANE SECTOR, 0x03 EXTENSION, 0x08 MATLIST, 0x07 MATERIAL.
BinMesh (extension of a sector): u32 flags, u32 numMeshes, u32 totalIndices, then per mesh u32 numIndices,
u32 matIndex, indices. Prints the sector number (stream order of atomic sectors), the mesh matIndex order and
each mesh's triangle-list index count / 3, so the order can be compared with the original's draw sequence.
"""
import struct
import sys

CONTAINERS = {0x0B, 0x09, 0x0A, 0x0C, 0x03, 0x08, 0x07}


def walk(d, o, end, out, state):
    while o + 12 <= end:
        t, sz, v = struct.unpack_from("<III", d, o)
        p = o + 12
        if t == 0x09:
            state["sector"] += 1
            state["cur"] = state["sector"]
        if t == 0x50E and p + 12 <= end:
            flags, nm, ni = struct.unpack_from("<III", d, p)
            q = p + 12
            meshes = []
            for _ in range(nm):
                cnt, mat = struct.unpack_from("<II", d, q)
                meshes.append((mat, cnt))
                q += 8 + 4 * cnt
            out.append((state["cur"], flags, meshes))
        elif t in CONTAINERS:
            walk(d, p, p + sz, out, state)
        o = p + sz


def main():
    d = open(sys.argv[1], "rb").read()
    out = []
    walk(d, 0, len(d), out, {"sector": -1, "cur": -1})
    for sec, flags, meshes in out:
        print(f"atomic sector #{sec} flags=0x{flags:x}: " + " ".join(f"m{m}x{c // 3}" for m, c in meshes))
    print(f"{len(out)} BinMesh chunks")


if __name__ == "__main__":
    main()
