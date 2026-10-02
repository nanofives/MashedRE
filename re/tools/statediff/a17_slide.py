#!/usr/bin/env python3
"""D2 attempt 17 reducer for the slide measure `+0xb0c` and its SEVEN inputs.

STANDING TOOL. `+0xb0c` is not an independent quantity: `A4 FUN_00470670` computes it
from seven record fields and nothing else (expression transcribed byte-exact in
`verify/d2_b0c_20261002/RESULT_STEP1.md` section 2.1):

    +0xb0c = 0                                     if  +0x9e4 == 0.0     0x0047072c
    +0xb0c = (1.0 - |dot| / +0x9e4) * +0x9e4       otherwise             0x00470724
      dot  = (+0x9d8 * +0x9b4 + +0x9d4 * +0x9b0) + +0x9dc * +0x9b8
      1.0  = _DAT_005cc320 = 0x3f800000  (read from original/MASHED.exe.unpatched)

So a `+0xb0c` gap is an INPUT gap. This tool measures which input, at which `d`.

The ORIGINAL side needs no new Frida hook: the `.msd` capture is the raw 0xd04 record
(`FORMAT.md`), so all seven inputs and the stored value are already in it. What a
snapshot capture does NOT give is the writer's own phase, so every use of it is gated on
`--ka`, the known-answer recompute (gate KA of `PREREG_STEP2.md`).

Usage
  py -3.12 re/tools/statediff/a17_slide.py --ka --orig a.msd [b.msd ...]
  py -3.12 re/tools/statediff/a17_slide.py --ka --port <dir>/motion_diag.log
  py -3.12 re/tools/statediff/a17_slide.py --cross --orig o.msd --port p/motion_diag.log
        [--orig-release 886] [--port-release 1] [--lag 0] [--dmax 400] [--tol 0.02]
"""
import argparse
import math
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

VEL = (0x9B0, 0x9B4, 0x9B8)
FWD = (0x9D4, 0x9D8, 0x9DC)
SPEED = 0x9E4
B0C = 0xB0C

# gate DR floors / table order, PREREG_STEP2.md section 5
DR_ORDER = ("fwdlen", "speed", "vellen", "mis")
DR_FLOOR = {"fwdlen": 1e-6, "speed": 1.0, "vellen": 1.0, "mis": 1e-3}

RE_FIELD = {
    "sp": re.compile(r"\bsp=([-+0-9.eE]+)"),
    "b0c": re.compile(r"\bb0c=([-+0-9.eEa-z]+)"),
    "vel": re.compile(r"\bvel=\[([^\]]*)\]"),
    "fwd": re.compile(r"\bfwd=\[([^\]]*)\]"),
}


def _f(x):
    try:
        return float(x)
    except ValueError:
        return float("nan")


def dot_original_order(fwd, vel):
    """(fy*vy + fx*vx) + fz*vz -- the ORIGINAL's x87 association, 0x004706db..0x00470701."""
    return (fwd[1] * vel[1] + fwd[0] * vel[0]) + fwd[2] * vel[2]


def predict_b0c(fwd, vel, speed):
    if speed == 0.0:
        return 0.0
    d = dot_original_order(fwd, vel)
    if d < 0.0:
        d = -d
    return (1.0 - d / speed) * speed


def samples_orig(path):
    """-> list of per-frame dicts in frame order, straight off the raw record."""
    _, _, frames = m.load_msd(path)
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        out.append(dict(
            frame=idx,
            vel=tuple(m.f32(p, o) for o in VEL),
            fwd=tuple(m.f32(p, o) for o in FWD),
            speed=m.f32(p, SPEED),
            b0c=m.f32(p, B0C),
        ))
    return out


def samples_port(path):
    """-> list of per-frame dicts from motion_diag.log. One line per frame, in order.

    `vel=` / `fwd=` are the fields appended by attempt 17 (PREREG_STEP2.md section 3).
    A log written before that change has neither, and this returns rows with vel/fwd
    None so the caller can report the gap rather than silently scoring zeros.
    """
    out = []
    with open(path, "r", errors="replace") as fh:
        for i, line in enumerate(fh):
            if "sp=" not in line:
                continue
            mv = RE_FIELD["vel"].search(line)
            mf = RE_FIELD["fwd"].search(line)
            ms = RE_FIELD["sp"].search(line)
            mb = RE_FIELD["b0c"].search(line)
            out.append(dict(
                frame=i,
                vel=tuple(_f(x) for x in mv.group(1).split(",")) if mv else None,
                fwd=tuple(_f(x) for x in mf.group(1).split(",")) if mf else None,
                speed=_f(ms.group(1)) if ms else float("nan"),
                b0c=_f(mb.group(1)) if mb else float("nan"),
            ))
    return out


def ka(rows, label):
    """GATE KA. -> (best_lam, {lam: (hits, scored)}) ; prints the table.

    Pairs the STORED b0c at frame n with the inputs at frame n+lam, recomputes, and
    scores rel = |pred - b0c| / max(|b0c|, 1e-3) <= 1e-4.
    """
    res = {}
    missing = sum(1 for r in rows if r["vel"] is None or r["fwd"] is None)
    for lam in (-1, 0, +1):
        hits = scored = skip_zero = skip_nan = 0
        for n in range(len(rows)):
            j = n + lam
            if j < 0 or j >= len(rows):
                continue
            src, dst = rows[j], rows[n]
            if src["vel"] is None or src["fwd"] is None:
                continue
            vals = list(src["vel"]) + list(src["fwd"]) + [src["speed"], dst["b0c"]]
            if any(not math.isfinite(v) for v in vals):
                skip_nan += 1
                continue
            if src["speed"] == 0.0:
                skip_zero += 1
                continue
            pred = predict_b0c(src["fwd"], src["vel"], src["speed"])
            rel = abs(pred - dst["b0c"]) / max(abs(dst["b0c"]), 1e-3)
            scored += 1
            if rel <= 1e-4:
                hits += 1
        res[lam] = (hits, scored, skip_zero, skip_nan)
    print(f"--- GATE KA  {label}   rows={len(rows)}  rows missing vel/fwd={missing}")
    best, bestf = None, -1.0
    for lam in (-1, 0, +1):
        h, s, sz, sn = res[lam]
        f = (h / s) if s else 0.0
        print(f"    lam={lam:+d}  hit {h}/{s} = {f:.6f}"
              f"   skipped: speed==0 {sz}, non-finite {sn}")
        if f > bestf:
            best, bestf = lam, f
    print(f"    best lam = {best:+d}  fraction {bestf:.6f}"
          f"   -> {'PASS' if bestf >= 0.99 else 'FAIL'} at the 0.99 threshold")
    return best, bestf, res


def terms(r):
    """The four DR terms for one sample row."""
    vel, fwd, sp = r["vel"], r["fwd"], r["speed"]
    vellen = math.sqrt(sum(c * c for c in vel))
    fwdlen = math.sqrt(sum(c * c for c in fwd))
    d = abs(dot_original_order(fwd, vel))
    den = vellen * fwdlen
    mis = (1.0 - d / den) if den > 0.0 else float("nan")
    return dict(fwdlen=fwdlen, speed=sp, vellen=vellen, mis=mis, b0c=r["b0c"])


def gap(a, b, floor):
    if not (math.isfinite(a) and math.isfinite(b)):
        return float("nan")
    return abs(a - b) / max(abs(a), abs(b), floor)


def fvar5(b0c):
    """A6a 0x004676cd..0x00467721:  max(1500.0 - b0c, 500.0)."""
    return max(1500.0 - b0c, 500.0)


def cross(orows, prows, R_o, R_p, lag, dmax, tol):
    """GATES DR and CB."""
    O = {r["frame"] - R_o: r for r in orows}
    P = {r["frame"] - R_p + lag: r for r in prows}
    ds = [d for d in range(0, dmax + 1) if d in O and d in P
          and P[d]["vel"] is not None and P[d]["fwd"] is not None]
    print(f"--- matched d:  {len(ds)} of {dmax + 1}  (orig R={R_o}, port R={R_p}, lag={lag})")
    if not ds:
        print("    NO matched d -- cross comparison VOID")
        return

    rows = []
    for d in ds:
        to, tp = terms(O[d]), terms(P[d])
        g = {k: gap(to[k], tp[k], DR_FLOOR[k]) for k in DR_ORDER}
        rows.append((d, to, tp, g))

    # ---- GATE DR
    named = None
    for d, to, tp, g in rows:
        for i, k in enumerate(DR_ORDER):
            if not math.isfinite(g[k]):
                continue
            if g[k] > tol:
                if all(math.isfinite(g[e]) and g[e] <= tol for e in DR_ORDER[:i]):
                    named = (d, k, to, tp, g)
                break
        if named:
            break
    print(f"--- GATE DR  tol={tol:.0%}  order={DR_ORDER}")
    if named is None:
        print("    NO DIVERGING INPUT in d=0..%d  -> +0xb0c is float noise" % dmax)
    else:
        d, k, to, tp, g = named
        print(f"    FIRST diverging input: {k}  at d={d}"
              f"   O={to[k]:.8g}  P={tp[k]:.8g}  gap={g[k]:.4%}")
        print("      all four gaps at that d: "
              + "  ".join(f"{e}={g[e]:.4%}" for e in DR_ORDER))
        print(f"      context at d={d}: "
              f"O speed={to['speed']:.6g} vellen={to['vellen']:.6g} "
              f"fwdlen={to['fwdlen']:.8g} mis={to['mis']:.6g} b0c={to['b0c']:.6g}")
        print(f"                     P speed={tp['speed']:.6g} vellen={tp['vellen']:.6g} "
              f"fwdlen={tp['fwdlen']:.8g} mis={tp['mis']:.6g} b0c={tp['b0c']:.6g}")

    # ---- per-term first exceedance, reported regardless of DR's ordering
    print("--- first d at which each term alone exceeds tol (diagnostic, not the gate)")
    for k in DR_ORDER + ("b0c",):
        fl = DR_FLOOR.get(k, 1e-3)
        hit = next(((d, to, tp) for d, to, tp, _ in rows
                    if math.isfinite(gap(to[k], tp[k], fl))
                    and gap(to[k], tp[k], fl) > tol), None)
        if hit is None:
            print(f"    {k:7s} never")
        else:
            d, to, tp = hit
            print(f"    {k:7s} d={d:<4d} O={to[k]:.8g}  P={tp[k]:.8g}"
                  f"  gap={gap(to[k], tp[k], fl):.4%}")

    # ---- GATE CB
    mo = max(abs(to["b0c"]) for _, to, _, _ in rows)
    mp = max(abs(tp["b0c"]) for _, _, tp, _ in rows)
    gf, gfd = 0.0, None
    for d, to, tp, _ in rows:
        fo, fp = fvar5(to["b0c"]), fvar5(tp["b0c"])
        gg = abs(fo - fp) / max(fo, fp)
        if gg > gf:
            gf, gfd = gg, d
    clamp_o = sum(1 for _, to, _, _ in rows if to["b0c"] >= 1000.0)
    clamp_p = sum(1 for _, _, tp, _ in rows if tp["b0c"] >= 1000.0)
    print(f"--- GATE CB   max|b0c| over matched d:  O={mo:.6g}  P={mp:.6g}")
    print(f"    frames with b0c >= 1000 (the 500.0 clamp engages): O={clamp_o}  P={clamp_p}")
    print(f"    max |fVar5_O - fVar5_P| / max(...) = {gf:.4%} at d={gfd}"
          f"   -> {'CB-SMALL' if gf < 0.05 else 'CB-LARGE'} at the 5% threshold")

    # ---- the "put n, median speed and d next to every metric" requirement
    sps_o = sorted(to["speed"] for _, to, _, _ in rows)
    sps_p = sorted(tp["speed"] for _, _, tp, _ in rows)
    med = lambda a: a[len(a) // 2] if a else float("nan")  # noqa: E731
    print(f"--- window: n={len(rows)}  d={ds[0]}..{ds[-1]}  "
          f"median speed O={med(sps_o):.6g}  P={med(sps_p):.6g}")
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ka", action="store_true")
    ap.add_argument("--cross", action="store_true")
    ap.add_argument("--orig", nargs="*", default=[])
    ap.add_argument("--port", nargs="*", default=[])
    ap.add_argument("--orig-release", type=int, default=886)
    ap.add_argument("--port-release", type=int, default=1)
    ap.add_argument("--lag", type=int, default=0)
    ap.add_argument("--dmax", type=int, default=400)
    ap.add_argument("--tol", type=float, default=0.02)
    ap.add_argument("--dump", default="", help="write the matched-d table to this CSV")
    a = ap.parse_args()

    if a.ka:
        fracs = []
        for p in a.orig:
            best, f, _ = ka(samples_orig(p), f"ORIG {os.path.basename(p)}")
            fracs.append((p, best, f))
        for p in a.port:
            best, f, _ = ka(samples_port(p), f"PORT {p}")
            fracs.append((p, best, f))
        if len(fracs) > 1:
            lams = {b for _, b, _ in fracs}
            allpass = all(f >= 0.99 for _, _, f in fracs)
            print(f"=== KA verdict: single lam across all inputs = "
                  f"{'YES ' + str(lams.pop()) if len(lams) == 1 else 'NO ' + str(sorted(lams))}"
                  f" ; all fractions >= 0.99 = {allpass}")

    if a.cross:
        orows = samples_orig(a.orig[0])
        prows = samples_port(a.port[0])
        rows = cross(orows, prows, a.orig_release, a.port_release,
                     a.lag, a.dmax, a.tol)
        if a.dump and rows:
            with open(a.dump, "w", newline="") as fh:
                fh.write("d,O_speed,P_speed,O_vellen,P_vellen,O_fwdlen,P_fwdlen,"
                         "O_mis,P_mis,O_b0c,P_b0c\n")
                for d, to, tp, _ in rows:
                    fh.write(f"{d},{to['speed']:.9g},{tp['speed']:.9g},"
                             f"{to['vellen']:.9g},{tp['vellen']:.9g},"
                             f"{to['fwdlen']:.9g},{tp['fwdlen']:.9g},"
                             f"{to['mis']:.9g},{tp['mis']:.9g},"
                             f"{to['b0c']:.9g},{tp['b0c']:.9g}\n")
            print(f"--- wrote {a.dump}")


if __name__ == "__main__":
    main()
