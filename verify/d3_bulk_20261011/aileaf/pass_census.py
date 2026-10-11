"""For each AI car v (1..3) in a G-AILEAF row file: the first call where it is ahead of the player (car 0)
in progress after having been behind, with both cars' speeds and the progress gap at that call.
Progress = 0x008a96e8 + c*0x30c (wraps at the node count, 0x0066d6d8). Usage: pass_census.py <rows.tsv> [...]"""
import struct, sys
for path in sys.argv[1:]:
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.startswith("570")]
    seen = {}
    out = []
    for k in rows:
        v = int(k[2]); cb = bytes.fromhex(k[7]); nodes = struct.unpack("<iiifii", bytes.fromhex(k[8]))[1]
        c = [struct.unpack_from("<ffffffIiiiffffii", cb, i * 64) for i in range(4)]
        d = c[v][10] - c[0][10]
        if d > nodes / 2: d -= nodes
        if d < -nodes / 2: d += nodes
        st = "behind" if d < 0 else "ahead"
        if seen.get(v) == "behind" and st == "ahead" and v not in [o[0] for o in out]:
            out.append((v, k[1], round(c[0][2]), round(c[v][2]), round(d, 3)))
        seen[v] = st
    print(path.split("/")[-1], "first pass of the player (v, call n, player speed, AI speed, gap):", out)
