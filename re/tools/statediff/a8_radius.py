import sys, math
sys.path.insert(0, 're/tools/statediff')
from a8_momentum import samples_original, samples_port, wrap

BANDS = ((500, 1000), (1000, 1500), (1500, 2000), (2000, 2600))


def radius(rows, label, steermin, dropres):
    print(f"--- {label} ---")
    ps = []
    for a, b in zip(rows, rows[1:]):
        if b["idx"] != a["idx"] + 1:
            continue
        if dropres and b["reseed"]:
            continue
        if a["gnd"] < 3.5 or b["gnd"] < 3.5:
            continue
        if a["steer"] < steermin:
            continue
        ps.append((a, b))
    print(f"  usable pairs {len(ps)}")
    out = {}
    for lo, hi in BANDS:
        sel = [(a, b) for a, b in ps if lo <= a["horiz"] < hi]
        if len(sel) < 8:
            print(f"  {lo}-{hi:<5} n={len(sel):>4}  (too few)")
            continue
        r, dth = [], []
        for a, b in sel:
            d = abs(wrap(b["velH"] - a["velH"]))
            if d < 1e-6:
                continue
            r.append(a["horiz"] / d)
            dth.append(d)
        if len(r) < 8:
            continue
        r.sort(); dth.sort()
        idxs = sorted(a["idx"] for a, b in sel)
        med = r[len(r) // 2]
        out[(lo, hi)] = med
        print(f"  {lo}-{hi:<5} n={len(r):>4}  radius(med)={med:>10.0f}"
              f"  |dvelH|/fr={dth[len(dth)//2]:.5f}"
              f"  frameidx p10={idxs[len(idxs)//10]} med={idxs[len(idxs)//2]}"
              f" p90={idxs[9*len(idxs)//10]}")
    return out


o = samples_original('verify/a8_steer_20260824/orig_steerR.msd')
p = samples_port('verify/a8_velvec_20260825/cleanhold_motion.log')
ro = radius(o, 'ORIGINAL (steerAng0 >= 33)', 33.0, False)
rp = radius(p, 'PORT held-lock (io.steer >= 0.9)', 0.9, True)

print("\n=== TURN RADIUS RATIO (port / orig); >1 means the port turns WIDER ===")
for b in BANDS:
    if b in ro and b in rp:
        print(f"  {b[0]}-{b[1]:<7} {rp[b]/ro[b]:.3f}")

print("\n=== ORIGINAL: where in the capture does the 500-1000 band sit? ===")
d = [x for x in o if x["horiz"] >= 500 and x["gnd"] >= 3.5 and x["steer"] >= 33]
lo = [x for x in d if x["horiz"] < 1000]
ai = sorted(x["idx"] for x in d)
print(f"  full-lock grounded driving frames: {len(d)}, spanning idx {ai[0]}..{ai[-1]}")
if lo:
    i = sorted(x["idx"] for x in lo)
    print(f"  of which horiz<1000: {len(i)}, idx min {i[0]} med {i[len(i)//2]} max {i[-1]}")
    early = sum(1 for x in i if x < ai[0] + 200)
    print(f"  how many of those are in the FIRST 200 frames of the full-lock window: {early}/{len(i)}")

print("\n=== PORT: what do reseeds correlate with? ===")
res = [k for k, x in enumerate(p) if x["reseed"]]
print(f"  reseeds {len(res)} in {len(p)} frames")
for w in (1, 2, 3, 5):
    g = [p[k - w]["gnd"] for k in res if k - w >= 0]
    if not g:
        continue
    ung = sum(1 for v in g if v < 3.5)
    print(f"  {w} frame(s) BEFORE a reseed: gnd<3.5 in {ung}/{len(g)}"
          f"  (median gnd {sorted(g)[len(g)//2]:.1f})")
allg = [x["gnd"] for x in p]
print(f"  baseline: gnd<3.5 in {sum(1 for v in allg if v<3.5)}/{len(allg)} of ALL frames")
sp = sorted(p[k]["horiz"] for k in res)
allsp = sorted(x["horiz"] for x in p)
print(f"  speed AT reseed: median {sp[len(sp)//2]:.0f}   all frames: median {allsp[len(allsp)//2]:.0f}")
