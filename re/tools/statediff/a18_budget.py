#!/usr/bin/env python3
"""a18_budget.py - D2 attempt 18, STEP 1: the per-frame velocity budget at MATCHED `d`.

Registered in verify/d2_budget_20261002/PREREG_STEP1.md BEFORE this file was run.

U-9177: over `d = 222..250` the drive force agrees to 6.2 %, all four wheels are grounded on
both sides, the cars are aligned and in the same gear, and the median per-frame speed gain is
ORIG +27.87 against PORT +4.86. This tool decomposes that gain into the writes that produce
it, in CALL ORDER, by RVA.

TERMS (PREREG section 2). Per-frame physics order is identical on both sides
(FUN_00470c70's decode, VehiclePhysicsRun.cpp:1003-1008, D2 section 21.3):

    A4   0x00470670   head
    A5   0x0046ddb0   wheel axes + Phase-4 drag
    A6a  0x00467650     accum  0x0046833a..0x00468625
                        W1     0x0046862d..0x004686a2   v += linTerm*(ctrl + accum)
                        store  0x004686cc               |v| -> +0x9e4
                        clamp6 0x004687f0..0x0046897b   lateral damp, W2 HIGH / W3 LOW
    A6b  0x00468980   writes NO velocity (section 26.4, static)
    sub  0x004709a0 x N

    s_mid(f)  = +0x9e4(f)                         after W1, before clamp #6
    s_post(f) = |+0x9b0..b8|(f)                   the render-tick snapshot

    T_drive = linTerm * (ctrl(f) . u(f)),  u = v(f-1)/s_post(f-1)      W1's drive share
    T_rest  = (s_mid(f) - s_post(f-1)) - T_drive                       A4+A5+accum residue
    T_post  = s_post(f) - s_mid(f)                                     clamp #6 + A6b + substeps

    T_drive + T_rest + T_post == s_post(f) - s_post(f-1)   -- CONSTRUCTION identity, not evidence

`linTerm`, `KDT`, `M54` and the (a)/(b) decision thresholds are IMPORTED from a10_gain.py so
the reused attempt-10 rule is literally the reused code.

`d` = frames from release. ORIG R=890 (the capture's own +0xbf8 marker, 0x0046d7a2);
PORT R=1. Port frame ordinals count MATCHING lines only, 0-based, exactly as
a17_slide.py:90-118 does, so `d` means the same frame in both tools.

Usage:
  py -3.12 re/tools/statediff/a18_budget.py --orig <msd> --port <motion_diag.log>
      [--orig-release 890] [--port-release 1] [--csv <out.csv>]
"""
import argparse
import math
import os
import re
import statistics
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m          # noqa: E402  load_msd / f32
import a10_gain as g10           # noqa: E402  KDT / M54 / DT_FRAME / LIN_DEFAULT

VEL = (0x9B0, 0x9B4, 0x9B8)
FWD = (0x9D4, 0x9D8, 0x9DC)
SPEED = 0x9E4
GND = 0x9E0
CTRL = (0xB14, 0xB18, 0xB1C)
M54_OFF = 0x54
BF8 = 0xBF8
GEAR = 0x490

# PREREG section 6: attempt 10's thresholds, reused verbatim (a10_gain.py:249-250)
TOL_A = 0.20       # |mp - mo| >= TOL_A * |median T_W1 on the ORIGINAL|
TOL_B = 0.30       # |mp - mo| >= TOL_B * |median dS_O - median dS_P|

# PREREG section 4
WINDOWS = [(200, 222, "control"), (222, 250, "DEFECT"), (250, 260, "shape")]
TERMS = ("T_drive", "T_rest", "T_post")        # CALL ORDER. Do not reorder.

# PREREG section 5, gate KA-R
KAR_RANGE = (0.9990, 1.0010)
KAR_MED = (0.9999, 1.0001)
KAR_FRAC = 0.99
KAR_D = (0, 400)
# PREREG section 5, gate KA-M
KAM_VALUE = 0.0010000000474974513
# PREREG section 3
PORT_SP_QUANT = 0.01

RE_F = {
    "sp": re.compile(r"\bsp=([-+0-9.eE]+)"),
    "gnd": re.compile(r"\bgnd=([-+0-9.eE]+)"),
    "gear": re.compile(r"\bgear=([-+0-9]+)"),
    "vel": re.compile(r"\bvel=\[([^\]]*)\]"),
    "fwd": re.compile(r"\bfwd=\[([^\]]*)\]"),
    "b14": re.compile(r"\bb14=\[([^\]]*)\]"),
}


def _f(x):
    try:
        return float(x)
    except (TypeError, ValueError):
        return float("nan")


def med(xs):
    return statistics.median(xs) if xs else float("nan")


def mag3(v):
    return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])


# ----------------------------------------------------------------- loaders

def load_orig(path):
    _, _, frames = m.load_msd(path)
    out = []
    for i in sorted(frames):
        p = frames[i]
        vel = tuple(m.f32(p, o) for o in VEL)
        out.append(dict(frame=i, vel=vel, s_post=mag3(vel),
                        s_mid=m.f32(p, SPEED),
                        fwd=tuple(m.f32(p, o) for o in FWD),
                        ctrl=tuple(m.f32(p, o) for o in CTRL),
                        gnd=m.f32(p, GND), m54=m.f32(p, M54_OFF),
                        bf8=struct.unpack_from("<i", p, BF8)[0],
                        gear=struct.unpack_from("<i", p, GEAR)[0]))
    return out


def load_port(path):
    """motion_diag.log. Ordinal over MATCHING lines only (a17_slide.py:99-106)."""
    out, i = [], -1
    with open(path, "r", errors="replace") as fh:
        for line in fh:
            if "sp=" not in line:
                continue
            i += 1

            def grp(k):
                mt = RE_F[k].search(line)
                return mt.group(1) if mt else None

            v = grp("vel")
            vel = tuple(_f(x) for x in v.split(",")) if v else (float("nan"),) * 3
            fw = grp("fwd")
            b1 = grp("b14")
            b14 = tuple(_f(x) for x in b1.split(",")) if b1 else (float("nan"),) * 3
            out.append(dict(frame=i, vel=vel, s_post=mag3(vel),
                            s_mid=_f(grp("sp")),
                            fwd=tuple(_f(x) for x in fw.split(",")) if fw
                            else (float("nan"),) * 3,
                            ctrl=b14, gnd=_f(grp("gnd")),
                            m54=g10.M54, bf8=0, gear=int(_f(grp("gear")) or 0)))
    return out


# ----------------------------------------------------------------- budget

def steps(rows, release, lin):
    """-> {d: step dict}. Drops are COUNTED, not silent (gate CO)."""
    by = {r["frame"]: r for r in rows}
    out, drop = {}, dict(no_prev=0, zero_prev=0, nonfinite=0)
    for r in rows:
        pr = by.get(r["frame"] - 1)
        if pr is None:
            drop["no_prev"] += 1
            continue
        if not (pr["s_post"] > 1e-6):
            drop["zero_prev"] += 1
            continue
        vals = list(r["vel"]) + list(r["ctrl"]) + list(pr["vel"]) + [r["s_mid"], r["s_post"]]
        if any(not math.isfinite(x) for x in vals):
            drop["nonfinite"] += 1
            continue
        s0 = pr["s_post"]
        u = tuple(c / s0 for c in pr["vel"])
        t_w1 = r["s_mid"] - s0
        t_dr = lin * sum(a * b for a, b in zip(r["ctrl"], u))
        t_po = r["s_post"] - r["s_mid"]
        out[r["frame"] - release] = dict(
            d=r["frame"] - release, frame=r["frame"], s_from=s0, s_to=r["s_post"],
            s_mid=r["s_mid"], dS=r["s_post"] - s0, T_W1=t_w1,
            T_drive=t_dr, T_rest=t_w1 - t_dr, T_post=t_po,
            gnd=r["gnd"], gear=r["gear"],
            ctrl_xz=math.hypot(r["ctrl"][0], r["ctrl"][2]),
            resid=t_dr + (t_w1 - t_dr) + t_po - (r["s_post"] - s0))
    return out, drop


# ----------------------------------------------------------------- gates

def gate_kar(rows, release):
    lo, hi = KAR_RANGE
    sel = [r for r in rows
           if KAR_D[0] <= r["frame"] - release <= KAR_D[1] and r["s_post"] > 1.0
           and math.isfinite(r["s_mid"]) and math.isfinite(r["s_post"])]
    rat = [r["s_mid"] / r["s_post"] for r in sel]
    inr = sum(1 for x in rat if lo <= x <= hi)
    mm = med(rat)
    frac = (inr / len(rat)) if rat else 0.0
    ok = bool(rat) and frac >= KAR_FRAC and KAR_MED[0] <= mm <= KAR_MED[1]
    return dict(n=len(rat), inrange=inr, frac=frac, median=mm,
                p25=(statistics.quantiles(rat, n=4)[0] if len(rat) >= 4 else float("nan")),
                p75=(statistics.quantiles(rat, n=4)[2] if len(rat) >= 4 else float("nan")),
                above102=sum(1 for x in rat if x > 1.02), ok=ok)


def gate_kam(rows):
    vals = sorted({r["m54"] for r in rows})
    ok = len(vals) == 1 and vals[0] == KAM_VALUE
    return dict(distinct=vals[:8], count=len(vals), ok=ok)


def gate_ev(so, sp, lo, hi):
    o = [s for d, s in so.items() if lo <= d <= hi]
    p = [s for d, s in sp.items() if lo <= d <= hi]
    r = dict(nO=len(o), nP=len(p),
             gnd4_O=sum(1 for s in o if s["gnd"] == 4.0),
             gnd4_P=sum(1 for s in p if s["gnd"] == 4.0),
             ctrl0_O=sum(1 for s in o if not s["ctrl_xz"] > 0.0),
             ctrl0_P=sum(1 for s in p if not s["ctrl_xz"] > 0.0))
    r["ok"] = (r["nO"] >= 25 and r["nP"] >= 25
               and r["gnd4_O"] == r["nO"] and r["gnd4_P"] == r["nP"]
               and r["ctrl0_O"] == 0 and r["ctrl0_P"] == 0)
    return r


# ----------------------------------------------------------------- report

def window_table(so, sp, lo, hi, tag):
    o = [s for d, s in sorted(so.items()) if lo <= d <= hi]
    p = [s for d, s in sorted(sp.items()) if lo <= d <= hi]
    if not o or not p:
        print("  d %d..%d (%s): ORIG n=%d PORT n=%d -- NOT COMPARABLE"
              % (lo, hi, tag, len(o), len(p)))
        return None
    w1o = med([s["T_W1"] for s in o])
    dso, dsp = med([s["dS"] for s in o]), med([s["dS"] for s in p])
    resid = abs(dso - dsp)
    bar_a, bar_b = TOL_A * abs(w1o), TOL_B * resid
    print("\n  === d %d..%d (%s) ===  ORIG n=%d med speed %.1f | PORT n=%d med speed %.1f"
          % (lo, hi, tag, len(o), med([s["s_from"] for s in o]),
             len(p), med([s["s_from"] for s in p])))
    print("      median T_W1 (ORIG) %+.4f -> bar(a) %.4f ; |dS_O - dS_P| %.4f -> bar(b) %.4f"
          % (w1o, bar_a, resid, bar_b))
    print("      %-9s %12s %12s %12s   (a)    (b)   OUT" % ("term", "ORIG", "PORT", "|delta|"))
    res = {}
    for t in TERMS:
        mo, mp = med([s[t] for s in o]), med([s[t] for s in p])
        dl = abs(mp - mo)
        a_ok, b_ok = dl >= bar_a, dl >= bar_b
        res[t] = dict(orig=mo, port=mp, delta=dl, a=a_ok, b=b_ok, out=a_ok and b_ok)
        print("      %-9s %+12.5f %+12.5f %12.5f   %-5s  %-5s  %s"
              % (t, mo, mp, dl, "PASS" if a_ok else "no", "PASS" if b_ok else "no",
                 "<<< OUT" if (a_ok and b_ok) else ""))
    print("      %-9s %+12.5f %+12.5f %12.5f   (the residual being explained)"
          % ("dS", dso, dsp, resid))
    print("      %-9s %+12.5f %+12.5f" % ("T_W1", w1o, med([s["T_W1"] for s in p])))
    # construction identity, labelled
    ri = max(max(abs(s["resid"]) for s in o), max(abs(s["resid"]) for s in p))
    print("      identity max|resid| %.3e  [CONSTRUCTION identity, not evidence]" % ri)
    # gate SS accounting
    nzO = sum(1 for s in o if abs(s["T_post"]) > 1e-6)
    nzP = sum(1 for s in p if abs(s["T_post"]) > 1e-6)
    print("      SS  T_post != 0 : ORIG %d/%d med %+.5f | PORT %d/%d med %+.5f"
          % (nzO, len(o), med([s["T_post"] for s in o if abs(s["T_post"]) > 1e-6]),
             nzP, len(p), med([s["T_post"] for s in p if abs(s["T_post"]) > 1e-6])))
    print("      gear   ORIG %s | PORT %s"
          % (sorted({s["gear"] for s in o}), sorted({s["gear"] for s in p})))
    print("      |ctrl_xz| med  ORIG %.4g | PORT %.4g"
          % (med([s["ctrl_xz"] for s in o]), med([s["ctrl_xz"] for s in p])))
    res["_meta"] = dict(nO=len(o), nP=len(p), w1o=w1o, dso=dso, dsp=dsp, resid=resid,
                        bar_a=bar_a, bar_b=bar_b, identity=ri,
                        medspO=med([s["s_from"] for s in o]),
                        medspP=med([s["s_from"] for s in p]))
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--orig-release", type=int, default=890)
    ap.add_argument("--port-release", type=int, default=1)
    ap.add_argument("--csv")
    a = ap.parse_args()

    orows, prows = load_orig(a.orig), load_port(a.port)
    lin = g10.LIN_DEFAULT
    print("a18_budget  linTerm %.17g = %.17g * %.17g * %.17g (imported from a10_gain)"
          % (lin, g10.DT_FRAME, g10.M54, g10.KDT))
    print("ORIG %s  frames %d   release %d" % (a.orig, len(orows), a.orig_release))
    print("PORT %s  frames %d   release %d" % (a.port, len(prows), a.port_release))
    print("port sp quantization %.2f -> %.3g of a term of 25 ; %.3g relative at speed 660"
          % (PORT_SP_QUANT, PORT_SP_QUANT / 25.0, PORT_SP_QUANT / 660.0))

    # --- release witness on the original, from the capture itself
    relw = [r["frame"] for r in orows if r["bf8"] != 0]
    print("ORIG release witness: first +0xbf8 != 0 at frame %s  (--orig-release %d)"
          % (relw[0] if relw else "NONE", a.orig_release))

    print("\n--- GATE KA-M (known answer, scored on the ORIGINAL) ---")
    kam = gate_kam(orows)
    print("  +0x54 distinct values: %d %s   expected exactly 1 == %.17g"
          % (kam["count"], kam["distinct"], KAM_VALUE))
    print("  KA-M %s" % ("PASS" if kam["ok"] else "FAIL"))

    print("\n--- GATE KA-R (known answer, scored on the ORIGINAL) ---")
    kar = gate_kar(orows, a.orig_release)
    print("  s_mid/s_post over d %d..%d, s_post > 1.0 : n=%d" % (KAR_D[0], KAR_D[1], kar["n"]))
    print("  median %.9f  p25 %.9f  p75 %.9f  >1.02 : %d"
          % (kar["median"], kar["p25"], kar["p75"], kar["above102"]))
    print("  in [%.4f,%.4f] : %d/%d = %.2f%%  (bar %.0f%%)"
          % (KAR_RANGE[0], KAR_RANGE[1], kar["inrange"], kar["n"], 100.0 * kar["frac"],
             100.0 * KAR_FRAC))
    print("  KA-R %s" % ("PASS" if kar["ok"] else "FAIL"))

    so, dropO = steps(orows, a.orig_release, lin)
    sp, dropP = steps(prows, a.port_release, lin)
    print("\n--- GATE CO (coverage) ---")
    print("  ORIG steps %d  drops %s" % (len(so), dropO))
    print("  PORT steps %d  drops %s" % (len(sp), dropP))

    print("\n--- GATE EV (the capture carries the event) ---")
    evs = {}
    for lo, hi, tag in WINDOWS:
        ev = gate_ev(so, sp, lo, hi)
        evs[(lo, hi)] = ev
        print("  d %3d..%3d  nO %3d nP %3d  gnd4 %3d/%3d %3d/%3d  ctrl==0 O %d P %d   %s"
              % (lo, hi, ev["nO"], ev["nP"], ev["gnd4_O"], ev["nO"], ev["gnd4_P"], ev["nP"],
                 ev["ctrl0_O"], ev["ctrl0_P"], "PASS" if ev["ok"] else "FAIL"))

    gates_ok = kam["ok"] and kar["ok"] and evs[(222, 250)]["ok"]
    print("\n--- BUDGET ---")
    tabs = {}
    for lo, hi, tag in WINDOWS:
        tabs[(lo, hi)] = window_table(so, sp, lo, hi, tag)

    print("\n--- DECISION RULE (PREREG section 6, call order %s) ---" % " -> ".join(TERMS))
    if not gates_ok:
        print("  GATES FAILED (KA-M %s, KA-R %s, EV@222-250 %s) -- the rule does NOT execute."
              % (kam["ok"], kar["ok"], evs[(222, 250)]["ok"]))
    t = tabs[(222, 250)]
    if t:
        named, upstream_out = None, []
        for term in TERMS:
            if t[term]["out"]:
                if named is None:
                    named = term
                else:
                    upstream_out.append(term)
        if named is None:
            print("  NO TERM NAMED in d 222..250.")
        else:
            print("  DIVERGING TERM: %s   ORIG %+.5f  PORT %+.5f  |delta| %.5f  (n O %d / P %d)"
                  % (named, t[named]["orig"], t[named]["port"], t[named]["delta"],
                     t["_meta"]["nO"], t["_meta"]["nP"]))
            if upstream_out:
                print("  also OUT, downstream of it: %s" % ", ".join(upstream_out))
            ctrl = tabs[(200, 222)]
            if ctrl:
                c = ctrl[named]
                print("  CONTROL d 200..222: %s is %s there (ORIG %+.5f PORT %+.5f |d| %.5f)"
                      % (named, "OUT OF TOLERANCE" if c["out"] else "IN tolerance",
                         c["orig"], c["port"], c["delta"]))
            if named == "T_post":
                print("  GATE SS: T_post is a LUMP (clamp #6 + A6b + substeps). The live leg in")
                print("           PREREG section 7 is BLOCKING before any fix.")
            if named == "T_rest":
                print("  GATE SS: T_rest is a LUMP (A4 + A5 + accum + W1 non-drive share). The")
                print("           live leg in PREREG section 7 is BLOCKING before any fix.")

    if a.csv:
        import csv as _csv
        keys = ["side", "d", "frame", "s_from", "s_to", "s_mid", "dS", "T_W1",
                "T_drive", "T_rest", "T_post", "gnd", "gear", "ctrl_xz", "resid"]
        with open(a.csv, "w", newline="") as fh:
            w = _csv.DictWriter(fh, fieldnames=keys)
            w.writeheader()
            for side, ss in (("O", so), ("P", sp)):
                for d in sorted(ss):
                    if not (WINDOWS[0][0] - 20 <= d <= WINDOWS[-1][1] + 20):
                        continue
                    row = dict(ss[d]); row["side"] = side
                    w.writerow({k: row[k] for k in keys})
        print("\n  wrote %s" % a.csv)


if __name__ == "__main__":
    main()
