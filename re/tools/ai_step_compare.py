"""D3 AI (b) diagnosis: compare the FUN_00416250 INPUTS car by car, not just its
output bytes.

The criterion-(b) checker (re/tools/ai_ctrl_window.py) scores the ctrl bytes. When a
car fails it, this tool answers WHY, from the columns both captures gained on
2026-09-27: the lookahead target (look_x/look_z), the FUN_00443440 curvature (curv),
the car's own position (own_x/own_z), and the two steer-history globals
(hist_d8 = 0x008032d8 + v*0x14, hist_dc = 0x008032dc + v*0x14).

Steering error and band, decoded from the two globals exactly as FUN_00416250 writes
them (no inference — both writes are unconditional inside their band):
  band 1, err < 180 : 0x008032d8 := 360.0 @0x004165cc, then 0x008032dc := err @0x004165f7
  band 2, err > 180 : 0x008032dc := 0.0   @0x004166db, then 0x008032d8 := err @0x0041670c
so hist_d8 == 360.0 -> band 1 with err = hist_dc, and hist_dc == 0.0 -> band 2 with
err = hist_d8. Rows matching neither (err == 180.0 exactly, which enters no band) are
counted separately as `band0` rather than guessed at.

Band 1 commits ctrl[0] and replays ctrl[1]; band 2 commits ctrl[1] and replays ctrl[0]
(0x00416648 / 0x00416623 and 0x0041675c / 0x00416738). So the c0/c1 distinct-count
split the tolerance tests is a direct readout of the band mix.

Usage:
  py -3.12 re/tools/ai_step_compare.py <orig.aistep.csv> <standalone.csv> [--n 220]
"""
import csv, statistics, sys


def load(path):
    return list(csv.DictReader(open(path, newline="")))


def fnum(r, k):
    v = r.get(k, "")
    if v is None or v == "":
        return None
    try:
        return float(v)
    except ValueError:
        return None


def band(r):
    """(band, err) per the two globals; band 0 = neither write fired this call."""
    d8, dc = fnum(r, "hist_d8"), fnum(r, "hist_dc")
    if d8 is None or dc is None:
        return (None, None)
    if d8 == 360.0:
        return (1, dc)
    if dc == 0.0:
        return (2, d8)
    return (0, None)


def window(rows, v, n):
    r = [x for x in rows if int(x["v"]) == v]
    i0 = next((i for i, x in enumerate(r) if int(x["c4"]) != 0), None)
    return [] if i0 is None else r[i0:i0 + n]


def med(xs):
    return round(statistics.median(xs), 3) if xs else None


def stats(w):
    b = [band(r) for r in w]
    b1 = [e for (k, e) in b if k == 1 and e is not None]
    b2 = [e for (k, e) in b if k == 2 and e is not None]
    b0 = sum(1 for (k, _) in b if k == 0)
    bn = sum(1 for (k, _) in b if k is None)
    curv = [c for c in (fnum(r, "curv") for r in w) if c is not None]
    # distance from the car to its lookahead target
    dist = []
    for r in w:
        lx, lz, ox, oz = (fnum(r, "look_x"), fnum(r, "look_z"),
                          fnum(r, "own_x"), fnum(r, "own_z"))
        if None not in (lx, lz, ox, oz):
            dist.append(((lx - ox) ** 2 + (lz - oz) ** 2) ** 0.5)
    spd = [s for s in (fnum(r, "rec_9e4") for r in w) if s is not None]
    return {
        "n": len(w),
        "band1": len(b1), "band2": len(b2), "band0": b0, "bandNA": bn,
        "b1frac": round(len(b1) / len(w), 3) if w else None,
        "err1_med": med(b1), "err2_med": med(b2),
        # how far inside its band the error sits: band 1 is graded over (0.05, 180),
        # band 2 over (180, 359.95) -> 360-err is the mirror magnitude (0x0041675c)
        "err1_p90": med(sorted(b1)[int(len(b1) * 0.9):]) if b1 else None,
        "mirror2_med": med([360.0 - e for e in b2]) if b2 else None,
        "curv_med": med(curv), "curv_max": round(max(curv), 2) if curv else None,
        "curv_gt20": round(sum(c > 20.0 for c in curv) / len(curv), 3) if curv else None,
        "tgtdist_med": med(dist),
        "spd_med": med(spd),
        "c0_distinct": len({int(r["c0"]) for r in w}),
        "c1_distinct": len({int(r["c1"]) for r in w}),
        "brakefrac": round(sum(int(r["c5"]) != 0 for r in w) / len(w), 3) if w else None,
        "modes": dict(sorted({int(r["ai_mode"]): sum(1 for x in w if int(x["ai_mode"]) == int(r["ai_mode"]))
                              for r in w}.items())),
    }


KEYS = ["n", "band1", "band2", "band0", "b1frac", "err1_med", "err2_med", "mirror2_med",
        "curv_med", "curv_max", "curv_gt20", "tgtdist_med", "spd_med",
        "c0_distinct", "c1_distinct", "brakefrac", "modes"]


def main(argv):
    n = 220
    if "--n" in argv:
        n = int(argv[argv.index("--n") + 1])
    paths = [a for i, a in enumerate(argv) if not a.startswith("--") and (i == 0 or argv[i - 1] != "--n")]
    data = {p: load(p) for p in paths}
    for v in (1, 2, 3):
        print("== car %d, %d-call racing window ==" % (v, n))
        cols = {}
        for p in paths:
            w = window(data[p], v, n)
            cols[p] = stats(w) if w else {}
        w = max(len(k) for k in KEYS) + 1
        print("  " + "field".ljust(w) + "".join(p.split("/")[-1].rjust(26) for p in paths))
        for k in KEYS:
            print("  " + k.ljust(w) + "".join(str(cols[p].get(k, "-")).rjust(26) for p in paths))
        print()


if __name__ == "__main__":
    main(sys.argv[1:])
