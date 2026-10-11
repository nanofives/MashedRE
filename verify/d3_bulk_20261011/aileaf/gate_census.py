"""Gate census over G-AILEAF rows (probe or exe liverows): for each FUN_00414570 call, which of its
acceptance gates the inputs pass, per candidate car (FUN_00414570 0x00414570..0x004148a4, plate
re/analysis/bucket_ai_00407a40_00415880/0x00414570.md). The curvature and angle gates need the spline /
the body; they are reported from the row's own ret only. Usage: gate_census.py <rows.tsv> [...]"""
import math, struct, sys, collections
for path in sys.argv[1:]:
    c = collections.Counter(); spd = []; best = None
    for ln in open(path):
        k = ln.rstrip("\n").split("\t")
        if len(k) < 15 or k[0] != "570": continue
        v = int(k[2]); cb = bytes.fromhex(k[7]); g = struct.unpack("<iiifii", bytes.fromhex(k[8]))
        cars = [struct.unpack_from("<ffffffIiiiffffii", cb, i * 64) for i in range(4)]
        nodes = g[1]; own = cars[v]
        c["calls"] += 1
        for t in range(4):
            if t == v: continue
            x = cars[t]
            if x[6] == 0: continue
            c["alive"] += 1
            if x[9] != 1: continue
            c["slot1"] += 1
            if x[7] != 0: continue
            c["state0"] += 1
            d = x[10] - own[10]
            if not (d > 0 or d < (5 - nodes)) or not (d < 2.0): continue
            c["ahead<2"] += 1
            dist = math.hypot(x[0] - own[0], x[1] - own[1])
            spd.append(x[2])
            ok_sp = (own[2] - x[2] < 1000) and own[2] > 2500 and x[2] > 2500
            if ok_sp: c["speeds"] += 1
            if ok_sp and dist < 7.0:
                c["dist<7"] += 1
            if best is None or dist < best[0]: best = (dist, own[2], x[2], k[1])
        if k[11] == "1": c["ret1"] += 1
    print(path.split("/")[-1], dict(c), "target-speed max", round(max(spd), 1) if spd else None, "closest (dist, own_sp, tgt_sp, n)", best)
