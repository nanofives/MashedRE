#!/usr/bin/env python3
"""a18_sink.py - D2 attempt 18, STEP 1B §2.1/§3.1/§4: the three checks that refute §23.2's
attribution of the port's `T_post` to grip-clamp #6.

Registered in verify/d2_budget_20261002/PREREG_STEP1B.md. Reduction only -- no game run, no
source change. Reads the two committed captures and nothing else.

WHAT IT PRINTS, per matched-`d` window:

 (1) ORIGINAL gate fields, from the .msd's own payload. These decide what the interval
     [the +0x9e4 store at 0x004686cc .. the render-tick snapshot] CONTAINS on the original:
        +0x9f0   A4's tail parked damp 0x00470948 fires only when == 2
                 (port VehicleControl.cpp:278-282)
        +0x9ec   0x00470aef's test; VehicleContactFixup 0x0046ef70 runs only when != 0
        +0x9e0   clamp #6's gate 0x00468761 needs exactly 0x40800000 (4.0f)
        +0x18c   grip = l_60 / this; 1.0 => grip == l_60
        +0x2c/+0x34  the two grip multipliers at Integrate2.cpp:657-658 (0 => inert)
        +0x1f0   track id, against the five literals at Integrate2.cpp:647-655
        +0xb20   the full-stop block needs == 0 AND speed < 16
 (2) PER-FRAME BODY-HEADING CHANGE, both sides. 0x0046e9e0 integrates the body orientation
     INSIDE the substep loop, i.e. AFTER A6a, so the snapshot's forward axis is NOT the axis
     clamp #6 used. This is the measurement that withdraws a18_clampinv.py's inversion.
 (3) The PORT's own `l_60` from its own log (`wle4`/`wld4`, Integrate2.cpp:434/:449,
     accumulated :473), put through clamp #6's transcribed arms, against the MEASURED
     1-R^2 = s^2(2k-k^2). Constants read as bit patterns, not decimal glosses
     (memory `plate-hex-gloss-authoritative`):
        0x005ce9fc 32768.0        knee          0x004687df
        0x005ce9f8 10000000.0     HIGH arm      0x004687f0
        0x005ce9f4 0x33d6bf95     HIGH slope    0x004687f6
        0x005ce9f0 0x38000000     LOW slope = exactly 2^-15
        LOW floor 0.1, HIGH scale 0.2 (k*0.1 + k*0.1, Integrate2.cpp:734)

Usage:
  py -3.12 re/tools/statediff/a18_sink.py --orig <msd> --port <motion_diag.log>
      [--orig-release 890] [--port-release 1]
"""
import argparse
import math
import os
import re
import statistics
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m          # noqa: E402
import a18_budget as b18         # noqa: E402  WINDOWS

K32768 = 32768.0
K1E7 = 10000000.0
KHISLOPE = struct.unpack('<f', struct.pack('<I', 0x33d6bf95))[0]
KLOSLOPE = struct.unpack('<f', struct.pack('<I', 0x38000000))[0]     # exactly 2^-15
KLOFLOOR = 0.1
KHISCALE = 0.2
LE4_CAP = 1024.0
# Integrate2.cpp:647-655, the five track-id literals
TRACK_LITS = (-0x5f7f80, -0x557f80, -0x69e1a6, -0xe17f4c, -0x37e1a6)

GATES = (("+0x9f0", 0x9f0, "i"), ("+0x9ec", 0x9ec, "i"), ("+0x9e0", 0x9e0, "i"),
         ("+0x18c", 0x18c, "f"), ("+0x2c", 0x2c, "i"), ("+0x34", 0x34, "i"),
         ("+0x1f0", 0x1f0, "i"), ("+0xb20", 0xb20, "i"))

RE_F = {k: re.compile(p) for k, p in (
    ("sp", r"\bsp=([-+0-9.eE]+)"), ("fwd", r"\bfwd=\[([^\]]*)\]"),
    ("vel", r"\bvel=\[([^\]]*)\]"), ("wle4", r"\bwle4=\[([^\]]*)\]"),
    ("wld4", r"\bwld4=\[([^\]]*)\]"))}


def med(x):
    return statistics.median(x) if x else float("nan")


def wrap(a):
    while a > math.pi:
        a -= 2 * math.pi
    while a < -math.pi:
        a += 2 * math.pi
    return a


def clamp6_k(G):
    """clamp #6's k from G = grip*speed, both arms, as transcribed."""
    if G > K32768:                               # 0x004687ea jne -> LOW, so > takes HIGH
        k = (K1E7 - G) * KHISLOPE
        if k < 0.0:
            k = 0.0
        return k * KHISCALE, "HIGH"
    k = (K32768 - G) * KLOSLOPE
    if k < KLOFLOOR:
        k = KLOFLOOR
    return k, "LOW"


def load_orig(path, R):
    _, _, fr = m.load_msd(path)
    out = {}
    for i in sorted(fr):
        p = fr[i]
        vel = struct.unpack_from("<3f", p, 0x9b0)
        out[i - R] = dict(d=i - R, raw=p, vel=vel,
                          fwd=struct.unpack_from("<3f", p, 0x9d4),
                          s_mid=struct.unpack_from("<f", p, 0x9e4)[0],
                          s_post=math.sqrt(sum(c * c for c in vel)))
    return out


def load_port(path, R):
    out, i = {}, -1
    with open(path, "r", errors="replace") as fh:
        for ln in fh:
            if "sp=" not in ln:
                continue
            i += 1

            def fl(k):
                mt = RE_F[k].search(ln)
                return [float(x) for x in mt.group(1).split(",")] if mt else None

            vel = fl("vel") or [float("nan")] * 3
            out[i - R] = dict(d=i - R, vel=tuple(vel), fwd=tuple(fl("fwd") or [float("nan")] * 3),
                              s_mid=float(RE_F["sp"].search(ln).group(1)),
                              s_post=math.sqrt(sum(c * c for c in vel)),
                              le4=fl("wle4"), ld4=fl("wld4"))
    return out


def slip(r):
    sp = r["s_post"]
    if not (sp > 0.0):
        return float("nan")
    dot = (r["fwd"][1] * r["vel"][1] + r["fwd"][0] * r["vel"][0]) + r["fwd"][2] * r["vel"][2]
    c = dot / sp
    return math.sqrt(max(0.0, 1.0 - c * c))


def yaw_table(rows, lo, hi, tag):
    sel = [r for d, r in sorted(rows.items()) if lo <= d <= hi and (d - 1) in rows]
    if not sel:
        return
    dh = [math.degrees(abs(wrap(math.atan2(r["fwd"][2], r["fwd"][0])
                               - math.atan2(rows[r["d"] - 1]["fwd"][2],
                                            rows[r["d"] - 1]["fwd"][0])))) for r in sel]
    print("    %-5s n=%2d  d(bodyHeading)/frame med %.4f deg  max %.4f deg"
          % (tag, len(sel), med(dh), max(dh)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--orig-release", type=int, default=890)
    ap.add_argument("--port-release", type=int, default=1)
    a = ap.parse_args()
    O = load_orig(a.orig, a.orig_release)
    P = load_port(a.port, a.port_release)
    print("a18_sink  KHISLOPE %.17g (0x33d6bf95)  KLOSLOPE %.17g (0x38000000 = 2^-15)"
          % (KHISLOPE, KLOSLOPE))
    print("ORIG %s (%d rows)   PORT %s (%d rows)" % (a.orig, len(O), a.port, len(P)))

    for lo, hi, tag in b18.WINDOWS:
        o = [r for d, r in sorted(O.items()) if lo <= d <= hi]
        p = [r for d, r in sorted(P.items()) if lo <= d <= hi]
        if not o or not p:
            continue
        print("\n=== d %d..%d (%s)  ORIG n=%d med speed %.1f | PORT n=%d med speed %.1f ==="
              % (lo, hi, tag, len(o), med([r["s_mid"] for r in o]),
                 len(p), med([r["s_mid"] for r in p])))

        print("  (1) ORIGINAL gate fields -- what the interval CONTAINS on the original")
        for nm, off, kind in GATES:
            if kind == "f":
                vals = sorted({struct.unpack_from("<f", r["raw"], off)[0] for r in o})
                shown = ["%g" % v for v in vals[:6]]
            else:
                vals = sorted({struct.unpack_from("<i", r["raw"], off)[0] for r in o})
                shown = [hex(v & 0xffffffff) for v in vals[:6]]
            note = ""
            if nm == "+0x1f0":
                note = ("  MATCHES a track literal" if any(v in TRACK_LITS for v in vals)
                        else "  matches NONE of the five track literals")
            print("      %-8s distinct %2d  %s%s" % (nm, len(vals), shown, note))

        print("  (2) per-frame body-heading change -- 0x0046e9e0 runs AFTER A6a, inside the"
              " substep loop")
        yaw_table(O, lo, hi, "ORIG")
        yaw_table(P, lo, hi, "PORT")

        print("  (3) the PORT's own l_60 through clamp #6's own arms, vs the MEASURED 1-R^2")
        l60, G, ks, arms, sl, meas = [], [], [], {"HIGH": 0, "LOW": 0}, [], []
        for r in p:
            if not r["le4"] or not r["ld4"]:
                continue
            L = sum(min(e, LE4_CAP) * d for e, d in zip(r["le4"], r["ld4"]))
            g = L * r["s_mid"]
            k, arm = clamp6_k(g)
            arms[arm] += 1
            s = slip(r)
            l60.append(L); G.append(g); ks.append(k); sl.append(s)
            meas.append(1.0 - (r["s_post"] / r["s_mid"]) ** 2)
        pred = [s * s * (2 * k - k * k) for s, k in zip(sl, ks)]
        mp, mm = med(pred), med(meas)
        print("      n=%d  l_60 med %.4f  G med %.1f  arm HIGH %d / LOW %d  k med %.6f"
              % (len(l60), med(l60), med(G), arms["HIGH"], arms["LOW"], med(ks)))
        print("      s'(snapshot) med %.6f   clamp cost s'^2(2k-k^2) med %.4e"
              "   MEASURED 1-R^2 med %.4e   short by %.1fx"
              % (med(sl), mp, mm, (mm / mp) if mp else float("inf")))
        # the original's side of the same comparison, as a BOUND (its l_60 is an A6a local)
        omeas = med([1.0 - (r["s_post"] / r["s_mid"]) ** 2 for r in o if r["s_mid"] > 0])
        osl = med([slip(r) for r in o])
        print("      ORIG  s'(snapshot) med %.6f   MEASURED 1-R^2 med %.4e"
              "   [l_60 is an A6a local -- not in the .msd]" % (osl, omeas))


if __name__ == "__main__":
    main()
