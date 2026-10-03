"""D3 STEP 2 — the AI over-speed budget, boost ON. Offline, committed captures only.

Implements EXACTLY what verify/d3_elim_20261003/PREREG_STEP2.md registered UNRUN.
No rule here may change after a run; a replacement must be stated in the RESULT.

The chain, with the RVAs the port cites (AiStandalone.cpp:913-920, the accel/brake
tail 0x004167d5..0x0041688b):

    default                                        c4=0xff c5=0
    R_B0C   rate0-prev0 > 15.0 && rate0 > 10.0     c4=0    c5=0xff   0x004167eb
    R_XSPD  X > 20.0 && speed > 2000.0             c4=0    c5=0xff   0x00416818
    R_ERRLO 30.0 < err < 180.0                     c4=0xff c5=0xff
    R_ERRHI 180.0 < err < 330.0                    c4=0xff c5=0xff

Inputs, all logged per call on BOTH sides by the existing step dumps:
    T0 err, X   hist_d8 / hist_dc   0x008032d8 / 0x008032dc + v*0x14
    T1 speed    rec_9e4             +0x9e4
    T2 rate0    rec_b0c             +0xb0c   (A6a channel, 0x00470724/0x0047072c)
    T3 rule occupancy (derived)
    T4 c4, c5   logged

DECISION RULE (registered): terms are tested in dependency order T0 -> T1 -> T2 ->
T3 -> T4. For a scalar term, D = median|orig-port| at matched call index and
Dn = D / IQR_orig; the FIRST term with Dn > 0.5, all earlier terms <= 0.5, is the
carrier. For the boolean T3 the statistic is |occupancy fraction difference| and the
threshold is 0.10. No term crossing = a null, reported as a null.

Usage:
  py -3.12 re/tools/ai_speed_budget.py --orig <o.aistep.csv> --port <p.csv>
        [--n 220] [--json OUT.json] [--radius 0.12]
"""
import argparse
import csv
import json
import math
import statistics
import sys
from collections import defaultdict

sys.path.insert(0, __file__.rsplit("\\", 1)[0].rsplit("/", 1)[0])
from ai_band_sim import simulate, decode, car_rows, N_DEFAULT   # noqa: E402

# --- the four rules' constants, by _DAT_ address (AiStandalone.cpp:570-581) ---
K_BRKDEL = 15.0      # _DAT_005cc9b0
K_BRKMIN = 10.0      # _DAT_005cc55c
K20      = 20.0      # _DAT_005ccd6c
K_RATE1  = 2000.0    # _DAT_005cd0b8
K_SPLIT  = 180.0     # _DAT_005cd09c
K_ERRLO  = 30.0      # _DAT_005cc72c
K_ERRHI  = 330.0     # _DAT_005cd0e0

# --- pre-registered thresholds (PREREG_STEP2.md section 2.2) ------------------
DN_THRESHOLD   = 0.5     # scalar terms, D / IQR_orig
OCC_THRESHOLD  = 0.10    # boolean occupancy fractions
KA_THRESHOLD   = 0.95    # section 2.0
CF_THRESHOLD   = 0.10    # section 2.4, "within 0.10 of the original's fraction"
R_MATCH        = 0.12    # world units, U-9183's value, unchanged


def load(path):
    return list(csv.DictReader(open(path, newline="")))


def rules_for(rows, i0, n, speeds=None, rates=None):
    """Per-call (rule set, c4, c5, err, X, speed, rate0) from the logged inputs.

    Replays the accel/brake tail only; the steer bands are ai_band_sim's business.
    `prev0` mirrors the persistent 0x008032e0 + v*0x14 the original rewrites every
    call (0x004167eb), seeded from the first call's own rate0 exactly as
    ai_band_sim does.
    """
    out = []
    prev0 = None
    for j in range(n):
        i = i0 + j
        d = decode(rows, i)
        if d is None:
            out.append(None)
            continue
        br, err, h = d
        X = 0.0
        if br == "lo":
            if h < err:
                X = h
        else:
            if err < h:
                X = 360.0 - h
        speed = float(rows[i]["rec_9e4"]) if speeds is None else speeds[j]
        rate0 = float(rows[i]["rec_b0c"]) if rates is None else rates[j]
        if prev0 is None:
            prev0 = rate0
        fired = []
        c4, c5 = 255, 0
        if K_BRKDEL < rate0 - prev0 and K_BRKMIN < rate0:
            fired.append("R_B0C"); c4, c5 = 0, 255
        if K20 < X and K_RATE1 < speed:
            fired.append("R_XSPD"); c4, c5 = 0, 255
        if err < K_SPLIT and K_ERRLO < err:
            fired.append("R_ERRLO"); c4, c5 = 255, 255
        if K_SPLIT < err and err < K_ERRHI:
            fired.append("R_ERRHI"); c4, c5 = 255, 255
        prev0 = rate0
        out.append({"rules": fired, "c4": c4, "c5": c5, "err": err, "X": X,
                    "speed": speed, "rate0": rate0, "row": rows[i]})
    return out


def ka(rec, rows, i0, n):
    """section 2.0 known-answer: reconstructed c4/c5 vs the LOGGED c4/c5."""
    ok = tot = 0
    for j in range(n):
        r = rec[j]
        if r is None:
            continue
        tot += 1
        lr = rows[i0 + j]
        if r["c4"] == int(lr["c4"]) and r["c5"] == int(lr["c5"]):
            ok += 1
    return ok, tot


def occupancy(rec, n):
    cnt = defaultdict(int)
    tot = 0
    for r in rec:
        if r is None:
            continue
        tot += 1
        if not r["rules"]:
            cnt["none"] += 1
        for k in r["rules"]:
            cnt[k] += 1
    return {k: cnt[k] / tot for k in ("R_B0C", "R_XSPD", "R_ERRLO", "R_ERRHI", "none")}, tot


def iqr(xs):
    s = sorted(xs)
    if len(s) < 4:
        return 0.0
    q1 = s[len(s) // 4]
    q3 = s[(3 * len(s)) // 4]
    return q3 - q1


def term_table(orec, prec, n):
    """section 2.2: scalar terms at matched call index, in dependency order."""
    rows = []
    for name, key in (("T0.err", "err"), ("T0.X", "X"),
                      ("T1.speed", "speed"), ("T2.rate0", "rate0")):
        o = [orec[j][key] for j in range(n) if orec[j] and prec[j]]
        p = [prec[j][key] for j in range(n) if orec[j] and prec[j]]
        if not o:
            continue
        d = statistics.median(abs(a - b) for a, b in zip(o, p))
        q = iqr(o)
        dn = (d / q) if q else float("inf") if d else 0.0
        rows.append({"term": name, "n": len(o), "med_orig": statistics.median(o),
                     "med_port": statistics.median(p), "D": d, "IQR_orig": q,
                     "Dn": dn, "crosses": dn > DN_THRESHOLD})
    return rows


def pos_match(orig, port, v, n, radius):
    """U-9183's rule, verbatim: nearest original row of the same car with the same
    ai_spline_idx, within `radius` world units of the port call's own (x, z)."""
    cand = defaultdict(list)
    for i, r in enumerate(orig):
        if int(r["v"]) == v and r["own_x"] != "":
            cand[r["ai_spline_idx"]].append((i, float(r["own_x"]), float(r["own_z"])))
    prows, pi0 = car_rows(port, v)
    if pi0 is None:
        return []
    pairs = []
    for j in range(n):
        pr = prows[pi0 + j]
        if pr["own_x"] == "":
            continue
        px, pz = float(pr["own_x"]), float(pr["own_z"])
        best, bd = None, 1e9
        for c in cand.get(pr["ai_spline_idx"], ()):
            dd = math.hypot(c[1] - px, c[2] - pz)
            if dd < bd:
                bd, best = dd, c
        if best is None or bd > radius:
            continue
        pairs.append((j, best[0], bd))
    return pairs


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--orig", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--n", type=int, default=N_DEFAULT)
    ap.add_argument("--radius", type=float, default=R_MATCH)
    ap.add_argument("--json", default="")
    a = ap.parse_args(argv)

    orig, port = load(a.orig), load(a.port)
    n = a.n
    rep = {"orig": a.orig, "port": a.port, "n": n, "radius": a.radius,
           "DN_THRESHOLD": DN_THRESHOLD, "OCC_THRESHOLD": OCC_THRESHOLD,
           "cars": {}}

    print("### 2.0 KNOWN-ANSWER CHECK (reconstructed c4/c5 vs logged), >= %.2f required"
          % KA_THRESHOLD)
    print("      THE ORIGINAL IS THE GATE. A failure there voids every occupancy below.")
    ka_fail = False
    recs = {}
    for v in (1, 2, 3):
        recs[v] = {}
        for side, data in (("orig", orig), ("port", port)):
            rows, i0 = car_rows(a.orig if side == "orig" else a.port, v)
            if i0 is None:
                continue
            rec = rules_for(rows, i0, n)
            ok, tot = ka(rec, rows, i0, n)
            recs[v][side] = (rec, rows, i0)
            frac = ok / tot if tot else 0.0
            flag = "PASS" if frac >= KA_THRESHOLD else "*** FAILED ***"
            if side == "orig" and frac < KA_THRESHOLD:
                ka_fail = True
            print("  car %d %-5s  c4+c5 exact %3d/%3d = %.3f  %s"
                  % (v, side, ok, tot, frac, flag))
            rep["cars"].setdefault(v, {}).setdefault("ka", {})[side] = \
                {"ok": ok, "n": tot, "frac": frac}
    if ka_fail:
        print("\n  *** KA FAILED ON THE ORIGINAL -> no rule-occupancy claim is made. STOP.")
        if a.json:
            json.dump(rep, open(a.json, "w"), indent=1)
        return 1

    print("\n### 2.1 RULE OCCUPANCY at matched call index (fraction of %d window calls)" % n)
    print("  %-4s %-5s %8s %8s %9s %9s %7s | %8s %8s"
          % ("car", "side", "R_B0C", "R_XSPD", "R_ERRLO", "R_ERRHI", "none",
             "c4=255", "c5=255"))
    for v in (1, 2, 3):
        for side in ("orig", "port"):
            if side not in recs[v]:
                continue
            rec, rows, i0 = recs[v][side]
            occ, tot = occupancy(rec, n)
            w = rows[i0:i0 + n]
            a255 = sum(int(x["c4"]) == 255 for x in w) / len(w)
            b255 = sum(int(x["c5"]) == 255 for x in w) / len(w)
            print("  %-4d %-5s %8.3f %8.3f %9.3f %9.3f %7.3f | %8.3f %8.3f"
                  % (v, side, occ["R_B0C"], occ["R_XSPD"], occ["R_ERRLO"],
                     occ["R_ERRHI"], occ["none"], a255, b255))
            rep["cars"][v].setdefault("occ", {})[side] = \
                dict(occ, c4_255=a255, c5_255=b255, n=tot)

    print("\n### 2.2 TERM DIVERGENCE at matched call index (threshold Dn > %.1f)"
          % DN_THRESHOLD)
    print("  %-4s %-10s %5s %11s %11s %11s %11s %8s %s"
          % ("car", "term", "n", "med orig", "med port", "D", "IQR orig", "Dn", ""))
    carriers = {}
    for v in (1, 2, 3):
        if "orig" not in recs[v] or "port" not in recs[v]:
            continue
        tt = term_table(recs[v]["orig"][0], recs[v]["port"][0], n)
        rep["cars"][v]["terms_index"] = tt
        named = None
        for t in tt:
            mark = "<= CROSSES" if t["crosses"] else ""
            if t["crosses"] and named is None:
                named = t["term"]
            print("  %-4d %-10s %5d %11.4f %11.4f %11.4f %11.4f %8.3f %s"
                  % (v, t["term"], t["n"], t["med_orig"], t["med_port"], t["D"],
                     t["IQR_orig"], t["Dn"], mark))
        # T3 occupancy
        oo = rep["cars"][v]["occ"]["orig"]
        po = rep["cars"][v]["occ"]["port"]
        for k in ("R_B0C", "R_XSPD", "R_ERRLO", "R_ERRHI"):
            d = abs(oo[k] - po[k])
            mark = "<= CROSSES" if d > OCC_THRESHOLD else ""
            if d > OCC_THRESHOLD and named is None:
                named = "T3." + k
            print("  %-4d %-10s %5s %11.4f %11.4f %11.4f %11s %8s %s"
                  % (v, "T3." + k, "-", oo[k], po[k], d, "-", "-", mark))
        carriers[v] = named
        print("  car %d FIRST CROSSING TERM: %s" % (v, named or "NONE (null)"))
    rep["carriers_index"] = carriers

    print("\n### 2.3 THE SAME, at matched POSITION (R = %.2f, U-9183's rule)" % a.radius)
    carriers_pos = {}
    for v in (1, 2, 3):
        if "orig" not in recs[v] or "port" not in recs[v]:
            continue
        pairs = pos_match(orig, port, v, n, a.radius)
        orows = [r for r in orig if int(r["v"]) == v]
        omap = {}
        _, oi0 = car_rows(a.orig, v)
        orec = recs[v]["orig"][0]
        # absolute row id -> window index on the original
        cv = [i for i, r in enumerate(orig) if int(r["v"]) == v]
        for k, absid in enumerate(cv[oi0:oi0 + n]):
            omap[absid] = k
        prec = recs[v]["port"][0]
        sel = [(pj, omap[oid]) for pj, oid, _ in pairs if oid in omap]
        rep["cars"][v]["pos_matched"] = {"n_pairs": len(pairs), "n_in_window": len(sel)}
        if len(sel) < 20:
            print("  car %d: only %d matched calls inside the original's own window "
                  "-> NOT SCORED (registered minimum 20)" % (v, len(sel)))
            carriers_pos[v] = None
            continue
        named = None
        rowsout = []
        for name, key in (("T0.err", "err"), ("T0.X", "X"),
                          ("T1.speed", "speed"), ("T2.rate0", "rate0")):
            o = [orec[ok][key] for pj, ok in sel if orec[ok] and prec[pj]]
            p = [prec[pj][key] for pj, ok in sel if orec[ok] and prec[pj]]
            if not o:
                continue
            d = statistics.median(abs(x - y) for x, y in zip(o, p))
            q = iqr(o)
            dn = (d / q) if q else (float("inf") if d else 0.0)
            cross = dn > DN_THRESHOLD
            if cross and named is None:
                named = name
            rowsout.append({"term": name, "n": len(o), "med_orig": statistics.median(o),
                            "med_port": statistics.median(p), "D": d, "IQR_orig": q,
                            "Dn": dn, "crosses": cross})
            print("  %-4d %-10s %5d %11.4f %11.4f %11.4f %11.4f %8.3f %s"
                  % (v, name, len(o), statistics.median(o), statistics.median(p),
                     d, q, dn, "<= CROSSES" if cross else ""))
        # occupancy on the matched subset
        for k in ("R_B0C", "R_XSPD", "R_ERRLO", "R_ERRHI"):
            fo = sum(1 for pj, ok in sel if orec[ok] and k in orec[ok]["rules"]) / len(sel)
            fp = sum(1 for pj, ok in sel if prec[pj] and k in prec[pj]["rules"]) / len(sel)
            d = abs(fo - fp)
            cross = d > OCC_THRESHOLD
            if cross and named is None:
                named = "T3." + k
            rowsout.append({"term": "T3." + k, "n": len(sel), "med_orig": fo,
                            "med_port": fp, "D": d, "crosses": cross})
            print("  %-4d %-10s %5d %11.4f %11.4f %11.4f %11s %8s %s"
                  % (v, "T3." + k, len(sel), fo, fp, d, "-", "-",
                     "<= CROSSES" if cross else ""))
        rep["cars"][v]["terms_pos"] = rowsout
        carriers_pos[v] = named
        print("  car %d FIRST CROSSING TERM (matched position, n=%d): %s"
              % (v, len(sel), named or "NONE (null)"))
    rep["carriers_pos"] = carriers_pos

    print("\n### G2-AGREE — matched index vs matched position")
    agree = True
    for v in (1, 2, 3):
        i, p = carriers.get(v), carriers_pos.get(v)
        same = (i == p)
        agree = agree and (same or p is None)
        print("  car %d: index=%s  position=%s  %s"
              % (v, i, p, "AGREE" if same else "DISAGREE"))
    rep["g2_agree"] = agree

    print("\n### 2.4 COUNTERFACTUAL — which logged input carries the c4 command?")
    print("  arm (i) must reproduce the PORT's own logged c4/c5 >= %.2f." % KA_THRESHOLD)
    print("  registered reading: the arm bringing c4=255 within %.2f of the original's."
          % CF_THRESHOLD)
    cf = {}
    for v in (1, 2, 3):
        if "orig" not in recs[v] or "port" not in recs[v]:
            continue
        prows, pi0 = car_rows(a.port, v)
        orows, oi0 = car_rows(a.orig, v)
        o_speed = [float(orows[oi0 + j]["rec_9e4"]) for j in range(n)]
        o_rate = [float(orows[oi0 + j]["rec_b0c"]) for j in range(n)]
        o_err = [decode(orows, oi0 + j) for j in range(n)]
        otgt = rep["cars"][v]["occ"]["orig"]["c4_255"]
        arms = [("(i)   as-is", None, None, None),
                ("(ii)  ORIG speed", o_speed, None, None),
                ("(iii) ORIG rec_b0c", None, o_rate, None),
                ("(iv)  ORIG err/X", None, None, o_err)]
        cf[v] = {}
        for lbl, sp, rt, ev in arms:
            if ev is None:
                rec = rules_for(prows, pi0, n, speeds=sp, rates=rt)
                frac = sum(1 for r in rec if r and r["c4"] == 255) / \
                    sum(1 for r in rec if r)
            else:
                s = simulate(prows, pi0, [float(prows[pi0 + j]["rec_9e4"])
                                          for j in range(n)], n, errseq=ev)
                frac = sum(1 for x in s if x[2] == 255) / len(s)
            delta = abs(frac - otgt)
            cf[v][lbl] = {"c4_255": frac, "orig": otgt, "delta": delta,
                          "within": delta <= CF_THRESHOLD}
            print("  car %d %-20s c4=255 %.3f  (orig %.3f, |d| %.3f) %s"
                  % (v, lbl, frac, otgt, delta,
                     "<= WITHIN" if delta <= CF_THRESHOLD else ""))
        if not any(x["within"] for k, x in cf[v].items() if not k.startswith("(i) ")):
            print("  car %d: NULL -- no single logged input brings c4=255 within %.2f"
                  % (v, CF_THRESHOLD))
    rep["counterfactual"] = cf

    if a.json:
        json.dump(rep, open(a.json, "w"), indent=1, default=str)
        print("\n  -> %s" % a.json)
    return 0


if __name__ == "__main__":
    sys.exit(main())
