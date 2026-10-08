"""Branch-2 FIRING-CALL scorer: what ctrl pose does the branch produce, on each side?

Motivation (RESULT_FIRESHAPE.md, 2026-10-08). `ai_ctrl_window.py` scores a fixed
220-call window anchored at i0 = first call with c4 != 0. The original's branch-2
firings land at offsets 149-219 and the port's at 558-660 -- DISJOINT -- so no single
offset window contains both sides' firings, and moving the window later trades away the
original's firings to gain the port's. This tool sidesteps the window: it scores the
calls where the branch ACTUALLY FIRES, on each side, in that side's own regime, and
prints a control arm so the pose can be told apart from the generic one.

Fire predicate is schema-driven:
  original aistep (o_t3): the committed family tuple
      (ret14a70, ret14c30, ret150e0, ret16060) == (0,0,0,1)
    -- use the TUPLE, not `ret148b0 != 0`: the latter counts the -1 "not evaluated"
       sentinel and yields 176 rather than 64 (SCOPE_CALLWISE.md section 1).
  port stepdump (CW_port.step.csv): b2_ret == 1
    -- b2_ret uses -1 for "the wired block did not evaluate"; only 1 is a firing.

Usage: py -3.12 re/tools/ai_fire_shape.py <csv> [<csv> ...] [--window 220] [--json]
"""
import csv, json, os, sys
from collections import Counter, defaultdict

POSE = ("c0", "c1", "c4", "c5")
ORIG_TUPLE = ("ret14a70", "ret14c30", "ret150e0", "ret16060")


def detect(cols):
    """Return (kind, predicate) for this capture's schema, or (None, None)."""
    if "b2_ret" in cols:
        return "port", lambda r: iv(r, "b2_ret") == 1
    if all(c in cols for c in ORIG_TUPLE):
        return "orig", lambda r: tuple(iv(r, c) for c in ORIG_TUPLE) == (0, 0, 0, 1)
    return None, None


def iv(r, k):
    return int(float(r[k]))


def score(path, window):
    with open(path, newline="") as f:
        rd = csv.DictReader(f)
        cols = rd.fieldnames or []
        rows = list(rd)
    kind, fires_on = detect(cols)
    if kind is None:
        return {"error": "no branch-2 column: need b2_ret (port) or %s (original)"
                         % "/".join(ORIG_TUPLE)}
    by = defaultdict(list)
    for r in rows:
        by[iv(r, "v")].append(r)

    out = {"kind": kind, "cars": {}}
    for v in sorted(by):
        rr = by[v]
        i0 = next((i for i, x in enumerate(rr) if iv(x, "c4") != 0), None)
        fires = [(i, x) for i, x in enumerate(rr) if fires_on(x)]
        car = {"calls": len(rr), "i0": i0, "fires": len(fires)}
        if i0 is not None:
            win = rr[i0:i0 + window]
            car["fires_in_window"] = sum(1 for x in win if fires_on(x))
            nf = [x for x in win if not fires_on(x)]
            car["control_n"] = len(nf)
            car["control"] = {k: {"distinct": len(set(iv(x, k) for x in nf))} for k in POSE}
        if fires:
            car["pose"] = {k: dict(sorted(Counter(iv(x, k) for _, x in fires).items()))
                           for k in POSE}
            car["pose_unanimous"] = {k: (len(car["pose"][k]) == 1) for k in POSE}
            if i0 is not None:
                offs = [i - i0 for i, _ in fires]
                car["fire_offsets"] = [min(offs), max(offs)]
            car["fire_frames"] = [iv(fires[0][1], "frame"), iv(fires[-1][1], "frame")]
            # Entry values, when the capture carries them (original aistep does).
            for k in ("c4_in", "c5_in"):
                if k in cols:
                    car.setdefault("entry", {})[k] = dict(
                        sorted(Counter(iv(x, k) for _, x in fires).items()))
        out["cars"][v] = car
    return out


def main(argv):
    window = 220
    if "--window" in argv:
        window = int(argv[argv.index("--window") + 1])
    skip = {"--window", "--json", str(window)}
    paths = [a for a in argv if a not in skip and not a.startswith("--")]
    res = {p: score(p, window) for p in paths}
    if "--json" in argv:
        print(json.dumps(res, indent=1)); return
    for p, r in res.items():
        print("%s  [%s]" % (os.path.basename(p), r.get("kind", "?")))
        if "error" in r:
            print("   %s" % r["error"]); continue
        for v, c in r["cars"].items():
            print("   v%d calls=%d i0=%s fires=%d (in window: %s)" % (
                v, c["calls"], c["i0"], c["fires"], c.get("fires_in_window", "n/a")))
            if not c["fires"]:
                continue
            if "fire_offsets" in c:
                print("      offsets %d..%d  frames %d..%d" % (
                    c["fire_offsets"][0], c["fire_offsets"][1],
                    c["fire_frames"][0], c["fire_frames"][1]))
            print("      POSE    %s" % "  ".join(
                "%s=%s" % (k, c["pose"][k]) for k in POSE))
            if "entry" in c:
                print("      ENTRY   %s" % "  ".join(
                    "%s=%s" % (k, vv) for k, vv in c["entry"].items()))
            print("      CONTROL n=%d distinct %s" % (c["control_n"], "  ".join(
                "%s=%d" % (k, c["control"][k]["distinct"]) for k in POSE)))


if __name__ == "__main__":
    main(sys.argv[1:])
