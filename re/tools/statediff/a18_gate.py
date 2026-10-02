#!/usr/bin/env python3
"""a18_gate.py - D2 attempt 18, STEP 2: does grip-clamp #6's gate 0x00468761 let the clamp
run on the ORIGINAL at matched `d`?

Registered in verify/d2_budget_20261002/PREREG_STEP2.md BEFORE this file was run.

WHY AN ENTRY HOOK ANSWERS IT. A6a 0x00467650 reads +0x9e0 TWICE (0x00468117, 0x00468761,
both `cmp dword ptr [esi+0x9e0], 0x40800000`) and WRITES IT ZERO TIMES over all 1243
instructions of 0x00467650..0x0046897b. So +0x9e0 read at A6a's ENTRY is bit-identical to
the value the clamp's gate reads. The render-tick snapshot cannot be used instead: A5
0x0046ddb0 zeroes +0x9e0 at 0x0046ddd1 one call before A6a and rebuilds it per wheel, so the
snapshot carries the POST-SUBSTEP value, a different quantity.

Input: `<out>.latbracket.csv` from `scenario_launch.py --lat-bracket`, sites
    0 = A6a entry 0x00467650 (ESI-filtered)
    2 = A6b entry 0x00468980
    1 = substep entry 0x004709a0
plus the paired `<out>.msd` for gate KA-B and the release marker.

Gates: CV (coverage + the 0,2,1,1 per-frame pattern = U-9160's original-side substep count),
KA-B (known answer, ORIGINAL: A6b-entry speed and |v| == the same frame's .msd values to
4 ulps), EV (n and median speed vs the reference capture).

Usage:
  py -3.12 re/tools/statediff/a18_gate.py --csv <out>.latbracket.csv --msd <out>.msd
      [--ref-speed 820.5]
"""
import argparse
import csv
import math
import os
import statistics
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m          # noqa: E402
import a18_budget as b18         # noqa: E402  WINDOWS

GND_4 = 0x40800000               # 4.0f, the gate's literal at 0x00468761
PATTERN = (0, 2, 1, 1)           # A6a, A6b, substep, substep
CV_MIN = dict(a6a=1200, a6b=1200, sub=2400)
CV_PATTERN_FRAC = 0.99
KAB_ULPS = 4.0
KAB_FRAC = 0.99
EV_N = 25
EV_SPEED_TOL = 0.15
G4_BRANCH = 0.80


def med(x):
    return statistics.median(x) if x else float("nan")


def ulps(a, b):
    """|a-b| in ulps of max(|a|,|b|,1) at float32."""
    scale = max(abs(a), abs(b), 1.0)
    e = math.frexp(scale)[1] - 1
    return abs(a - b) / (2.0 ** (e - 23))


def f2bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def _slip(r):
    """|lat|/|v| with the ORIGINAL's x87 association order (0x00468771..0x00468793)."""
    v, w = r["vel"], r["fwd"]
    sp = math.sqrt(sum(c * c for c in v))
    if not (sp > 0.0):
        return float("nan")
    dot = (w[1] * v[1] + w[0] * v[0]) + w[2] * v[2]
    return math.sqrt(max(0.0, 1.0 - (dot / sp) ** 2))


def load_csv(path):
    out = []
    with open(path, newline="") as fh:
        for r in csv.DictReader(fh):
            out.append(dict(seq=int(r["seq"]), site=int(r["site"]),
                            vel=(float(r["velx"]), float(r["vely"]), float(r["velz"])),
                            fwd=(float(r["fwdx"]), float(r["fwdy"]), float(r["fwdz"])),
                            speed=float(r["speed"]), gnd=float(r["gnd"])))
    return out


def frames(rows):
    """-> list of per-frame dicts, each the 0,2,1,1 group, in order. Pattern VERIFIED."""
    out, cur, bad = [], None, 0
    for r in rows:
        if r["site"] == 0:
            if cur is not None:
                out.append(cur)
            cur = dict(sites=[0], a6a=r, a6b=None, subs=[])
            continue
        if cur is None:
            bad += 1
            continue
        cur["sites"].append(r["site"])
        if r["site"] == 2:
            cur["a6b"] = r
        else:
            cur["subs"].append(r)
    if cur is not None:
        out.append(cur)
    for i, f in enumerate(out):
        f["idx"] = i
        f["ok"] = tuple(f["sites"]) == PATTERN
    return out, bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True)
    ap.add_argument("--msd", required=True)
    ap.add_argument("--ref-speed", type=float, default=820.5)
    ap.add_argument("--out")
    a = ap.parse_args()

    rows = load_csv(a.csv)
    fr, orphan = frames(rows)
    _, _, msd = m.load_msd(a.msd)

    print("a18_gate  clamp #6 gate 0x00468761 `cmp [esi+0x9e0], 0x%08X`" % GND_4)
    print("  csv %s  rows %d -> frames %d (orphans %d)" % (a.csv, len(rows), len(fr), orphan))

    # --- release marker from THIS capture's own +0xbf8 (attempt 17 §2 discipline)
    relw = [i for i in sorted(msd) if struct.unpack_from("<i", msd[i], 0xbf8)[0] != 0]
    R = relw[0] if relw else None
    print("  msd %s  frames %d   release: first +0xbf8 != 0 at frame %s" % (a.msd, len(msd), R))
    if R is None:
        raise SystemExit("no release marker in this capture -- STOP")

    # --- GATE CV
    nA = sum(1 for r in rows if r["site"] == 0)
    nB = sum(1 for r in rows if r["site"] == 2)
    nS = sum(1 for r in rows if r["site"] == 1)
    okp = sum(1 for f in fr if f["ok"])
    pf = okp / len(fr) if fr else 0.0
    subpf = nS / nA if nA else float("nan")
    cv = (nA >= CV_MIN["a6a"] and nB >= CV_MIN["a6b"] and nS >= CV_MIN["sub"]
          and pf >= CV_PATTERN_FRAC and orphan == 0)
    print("\n--- GATE CV (coverage + the 0,2,1,1 pattern = U-9160 original side) ---")
    print("  a6a %d  a6b %d  sub %d  orphans %d" % (nA, nB, nS, orphan))
    print("  pattern 0,2,1,1 on %d/%d = %.2f%%  (bar %.0f%%)"
          % (okp, len(fr), 100.0 * pf, 100.0 * CV_PATTERN_FRAC))
    print("  SUBSTEPS PER FRAME on the ORIGINAL: %.4f   [U-9160 expects exactly 2]" % subpf)
    print("  CV %s" % ("PASS" if cv else "FAIL"))

    # --- GATE KA-B: A6b entry vs the .msd, same frame
    print("\n--- GATE KA-B (known answer, scored on the ORIGINAL) ---")
    pairs, sp_bad, v_bad, worst = 0, 0, 0, 0.0
    for f in fr:
        if f["a6b"] is None or f["idx"] not in msd:
            continue
        p = msd[f["idx"]]
        msp = struct.unpack_from("<f", p, 0x9e4)[0]
        mv = struct.unpack_from("<3f", p, 0x9b0)
        mvl = math.sqrt(sum(c * c for c in mv))
        pvl = math.sqrt(sum(c * c for c in f["a6b"]["vel"]))
        u1 = ulps(f["a6b"]["speed"], msp)
        u2 = ulps(pvl, mvl)
        worst = max(worst, u1, u2)
        pairs += 1
        if u1 > KAB_ULPS:
            sp_bad += 1
        if u2 > KAB_ULPS:
            v_bad += 1
    frac = ((pairs - max(sp_bad, v_bad)) / pairs) if pairs else 0.0
    kab = pairs > 0 and frac >= KAB_FRAC
    print("  matched frames %d   speed off > %.0f ulps: %d   |v| off > %.0f ulps: %d"
          % (pairs, KAB_ULPS, sp_bad, KAB_ULPS, v_bad))
    print("  within budget on %.2f%%  (bar %.0f%%)   worst %.3f ulps"
          % (100.0 * frac, 100.0 * KAB_FRAC, worst))
    print("  KA-B %s" % ("PASS" if kab else "FAIL"))

    # --- windows
    print("\n--- GATE EV + THE REGISTERED READING (G4 at A6a entry) ---")
    res = {}
    for lo, hi, tag in b18.WINDOWS:
        sel = [f for f in fr if lo <= f["idx"] - R <= hi and f["ok"]]
        if not sel:
            print("  d %3d..%3d (%s): n=0" % (lo, hi, tag))
            continue
        sp = med([f["a6a"]["speed"] for f in sel])
        g4 = sum(1 for f in sel if f2bits(f["a6a"]["gnd"]) == GND_4)
        g4f = g4 / len(sel)
        # A6a-alone |v| change: [A6a entry -> A6b entry]
        ia = []
        for f in sel:
            if f["a6b"] is None:
                continue
            va = math.sqrt(sum(c * c for c in f["a6a"]["vel"]))
            vb = math.sqrt(sum(c * c for c in f["a6b"]["vel"]))
            ia.append(vb - va)
        gnds = sorted({f["a6a"]["gnd"] for f in sel})
        ev = len(sel) >= EV_N and abs(sp - a.ref_speed) <= EV_SPEED_TOL * a.ref_speed
        print("  d %3d..%3d (%s)  n=%2d  med speed at A6a entry %8.1f   EV %s"
              % (lo, hi, tag, len(sel), sp,
                 "PASS" if ev else ("n/a (control window)" if hi != 250 else "FAIL")))
        print("      +0x9e0 at A6a ENTRY distinct: %s" % ["%g" % v for v in gnds[:8]])
        print("      G4 (+0x9e0 == 4.0f) TRUE on %d/%d = %.2f%%"
              % (g4, len(sel), 100.0 * g4f))
        print("      d|v| across [A6a entry -> A6b entry] (A6a ALONE): median %+.5f  n=%d"
              % (med(ia), len(ia)))
        res[(lo, hi)] = dict(n=len(sel), speed=sp, g4=g4, g4f=g4f, ia=med(ia), ev=ev)

    # --- POST-HOC DIAGNOSTIC, labelled as such: the clamp's ratio at its OWN phase.
    # NOT pre-registered. Chosen after the branch fired. A6b entry is the first sample after
    # A6a returns, and +0x9e4 is written ONLY pre-clamp (0x004686cc; the only other writers
    # are A6a's entry fstp 0x00467673 and the spawn-init 0x0046bc36, both earlier), so
    #     R_c = |v|@A6b_entry / (+0x9e4)@A6b_entry
    # is EXACTLY grip-clamp #6's magnitude ratio, with the clamp's own forward axis (A6a
    # writes no forward axis; A5 wrote it before A6a). The render-tick snapshot cannot give
    # this: 0x0046e9e0 rotates the body inside the substep loop, AFTER A6a.
    print("\n--- DIAGNOSTIC (post-hoc, NOT a pre-registered gate): the clamp at its OWN"
          " phase ---")
    KMIN = 0.1249        # structural floor, RESULT_STEP1B §2.2
    for lo, hi, tag in b18.WINDOWS:
        sel = [f for f in fr if lo <= f["idx"] - R <= hi and f["ok"] and f["a6b"]]
        if not sel:
            continue
        rc, sb, sa, pr, dem = [], [], [], [], []
        for f in sel:
            b = f["a6b"]
            sp = b["speed"]
            if not (sp > 1.0):
                continue
            vl = math.sqrt(sum(c * c for c in b["vel"]))
            r = vl / sp
            s_b = _slip(b)
            rc.append(r); sb.append(s_b); sa.append(_slip(f["a6a"]))
            pr.append(1.0 - r * r)
            dem.append(s_b * s_b * (2.0 * KMIN - KMIN * KMIN))
        if not rc:
            continue
        mp, md = med(pr), med(dem)
        print("  d %3d..%3d (%s) n=%2d med speed@A6b %7.1f" % (lo, hi, tag, len(rc),
                                                               med([f["a6b"]["speed"]
                                                                    for f in sel])))
        print("      R_c med %.9f   1-R_c med %.4e   1-R_c^2 med %.4e   R_c>1 on %d/%d"
              % (med(rc), med([1.0 - x for x in rc]), mp,
                 sum(1 for x in rc if x > 1.0), len(rc)))
        print("      slip @A6a entry med %.6f   @A6b entry (POST-clamp) med %.6f" %
              (med(sa), med(sb)))
        print("      demanded at k >= %.4f with s = slip@A6b: med %.4e  -> measured/demanded"
              " %.2f" % (KMIN, md, (mp / md) if md else float("inf")))
    print("      [compare the RENDER-TICK split on the same capture: a18_sink.py reports"
          " 1-R^2 = 1.2194e-05 at d 222..250]")

    print("\n--- BRANCH (PREREG_STEP2 §4) ---")
    if not (cv and kab):
        print("  GATES FAILED (CV %s, KA-B %s) -- the reading does NOT execute." % (cv, kab))
    t = res.get((222, 250))
    if t:
        if t["g4f"] <= 1.0 - G4_BRANCH:
            print("  BRANCH CLOSED: +0x9e0 != 4.0f at A6a entry on %.2f%% of frames"
                  " (n=%d, med speed %.1f, d=222..250)."
                  % (100.0 * (1.0 - t["g4f"]), t["n"], t["speed"]))
            print("  => grip-clamp #6's velocity stores do NOT execute on the original here.")
            print("  => the next term is the REBUILD of +0x9e0 (ForceIntegrator.cpp:59,")
            print("     original A5 0x0046ddb0 after its zero at 0x0046ddd1), not the clamp")
            print("     and not l_60.")
        elif t["g4f"] >= G4_BRANCH:
            print("  BRANCH OPEN: +0x9e0 == 4.0f at A6a entry on %.2f%% of frames"
                  " (n=%d, med speed %.1f, d=222..250)."
                  % (100.0 * t["g4f"], t["n"], t["speed"]))
            print("  => the clamp DOES run and still costs 31x less than its arithmetic")
            print("     demands. Next: the original's s and l_60 at the clamp's OWN phase,")
            print("     via --mag-probe 004686a9,004680fb,0046820f.")
        else:
            print("  NEITHER BRANCH: G4 true on %.2f%% (n=%d) -- duty cycle reported, no"
                  " branch named." % (100.0 * t["g4f"], t["n"]))

    if a.out:
        with open(a.out, "w", newline="") as fh:
            w = csv.writer(fh)
            w.writerow(["d", "frame", "pattern_ok", "gnd_a6a", "g4", "speed_a6a",
                         "speed_a6b", "vlen_a6a", "vlen_a6b"])
            for f in fr:
                d = f["idx"] - R
                if not (180 <= d <= 280):
                    continue
                va = math.sqrt(sum(c * c for c in f["a6a"]["vel"]))
                vb = (math.sqrt(sum(c * c for c in f["a6b"]["vel"]))
                      if f["a6b"] else float("nan"))
                w.writerow([d, f["idx"], int(f["ok"]), f["a6a"]["gnd"],
                            int(f2bits(f["a6a"]["gnd"]) == GND_4), f["a6a"]["speed"],
                            f["a6b"]["speed"] if f["a6b"] else "", va, vb])
        print("\n  wrote %s" % a.out)


if __name__ == "__main__":
    main()
