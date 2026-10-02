#!/usr/bin/env python3
"""a19_split.py - D2 attempt 19, STEP 1: split the PORT's `T_post` by PRODUCER and SUBSTEP.

Registered in verify/d2_sink_20261002/PREREG_STEP1.md BEFORE this file was run.

`T_post = s_post(f) - s_mid(f)` with `s_mid = +0x9e4` (stored at 0x004686cc, after W1, before
grip-clamp #6) and `s_post = |+0x9b0..b8|` at the render tick -- a18_budget.py's own
definition, reused, not restated.

This tool reads the MASHED_D2SINK log (Vehicle/D2SinkProbe.cpp), which samples |velocity| at
every producer's own phase inside that interval, and telescopes the deltas:

    D_clamp6 = m(a6a_out) - m(w1)                  0x004687f0..0x0046897b
    D_a6b    = m(a6b_out) - m(a6a_out)             0x00468980
    D_damp   = m(a4_out)  - m(a6b_out)             0x00470948, gate +0x9f0 == 2
    D_gap    = m(sub_top,0) - m(a4_out)
    D_sub[s] = m(sub_end,s) - m(sub_top,s)         0x00471106..0x00471141, s = 0..N-1
      D_orient[s] = m(sub_orient,s) - m(sub_top,s)       0x0046e9e0
      D_wheel[s]  = m(sub_wheel,s)  - m(sub_orient,s)    0x0046f6c0
      D_fixup[s]  = m(sub_end,s)    - m(sub_wheel,s)     0x0046ef70 (fx_* legs inside)
    D_tail   = m(snap) - m(sub_end,N-1)

Gates KA1 / KA2 / KA3 / CV / EV and the decision rule are PREREG sections 2.2, 2.3 and 3.

Usage:
  py -3.12 re/tools/statediff/a19_split.py --sink <sink.log> --port <motion_diag.log>
      [--release 1] [--lo 222] [--hi 250] [--csv <out.csv>]
"""
import argparse
import math
import os
import re
import statistics
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# PREREG 2.3
EV_N = 25
EV_GND = 0.90
EV_CTRL = 0.90
EV_SPEED_REF = 633.2          # attempt 18 budget.csv, PORT, d = 222..250
EV_SPEED_TOL = 0.25
# PREREG 2.2
KA1_FRAC = 1.0
KA2_REL = 1e-5
KA2_FRAC = 0.99
KA3_REL = 1e-6
KA3_FRAC = 0.99
# PREREG 3
CARRIER_SHARE = 0.50

ONCE_TAGS = ("w1", "a6a_out", "a6b_out", "a4_out", "snap")
LINE = re.compile(
    r"f=(?P<f>-?\d+) tag=(?P<tag>\w+) sub=(?P<sub>-?\d+) pass=(?P<pass>-?\d+) "
    r"v=\((?P<vx>[^,]+),(?P<vy>[^,]+),(?P<vz>[^)]+)\) mag=(?P<mag>\S+) r9e4=(?P<r9e4>\S+) "
    r"r9e0=(?P<r9e0>\S+) r9f0=(?P<r9f0>-?\d+) r9ec=(?P<r9ec>-?\d+) key0=(?P<key0>-?\d+) "
    r"chunk=(?P<chunk>\S+) rem=(?P<rem>\S+) fx=(?P<fx>-?\d+)")
RE_VEL = re.compile(r"\bvel=\[([^\]]*)\]")
RE_SP = re.compile(r"\bsp=([-+0-9.eE]+)")
RE_GND = re.compile(r"\bgnd=([-+0-9.eE]+)")
RE_B14 = re.compile(r"\bb14=\[([^\]]*)\]")


def med(xs):
    return statistics.median(xs) if xs else float("nan")


def _f(x):
    try:
        return float(x)
    except (TypeError, ValueError):
        return float("nan")


def load_sink(path):
    """-> {frame: {'once': {tag: row}, 'sub': {s: {tag: row}}, 'bad': n}}"""
    frames, bad = {}, 0
    with open(path, "r", errors="replace") as fh:
        for line in fh:
            mt = LINE.match(line.strip())
            if not mt:
                if line.strip():
                    bad += 1
                continue
            g = mt.groupdict()
            f = int(g["f"])
            row = dict(tag=g["tag"], sub=int(g["sub"]), pas=int(g["pass"]),
                       v=(_f(g["vx"]), _f(g["vy"]), _f(g["vz"])), mag=_f(g["mag"]),
                       r9e4=_f(g["r9e4"]), r9e0=_f(g["r9e0"]), r9f0=int(g["r9f0"]),
                       r9ec=int(g["r9ec"]), key0=int(g["key0"]),
                       chunk=_f(g["chunk"]), rem=_f(g["rem"]), fx=int(g["fx"]))
            fr = frames.setdefault(f, dict(once={}, sub={}, seq=[]))
            fr["seq"].append(row)
            if row["tag"] in ONCE_TAGS:
                fr["once"].setdefault(row["tag"], []).append(row)
            else:
                fr["sub"].setdefault(row["sub"], {}).setdefault(row["tag"], []).append(row)
    return frames, bad


def load_port(path):
    """motion_diag.log, ordinal over MATCHING lines only -- a18_budget.py:126-129's rule."""
    out, i = [], -1
    with open(path, "r", errors="replace") as fh:
        for line in fh:
            if "sp=" not in line:
                continue
            i += 1
            v = RE_VEL.search(line)
            vel = tuple(_f(x) for x in v.group(1).split(",")) if v else (float("nan"),) * 3
            b = RE_B14.search(line)
            b14 = tuple(_f(x) for x in b.group(1).split(",")) if b else (float("nan"),) * 3
            sp = RE_SP.search(line)
            gn = RE_GND.search(line)
            out.append(dict(frame=i, vel=vel,
                            s_post=math.sqrt(sum(c * c for c in vel)),
                            s_mid=_f(sp.group(1)) if sp else float("nan"),
                            gnd=_f(gn.group(1)) if gn else float("nan"),
                            ctrl_xz=math.hypot(b14[0], b14[2])))
    return out


def split_frame(fr):
    """-> dict of producer deltas, or None if a required tag is missing."""
    one = {}
    for t in ONCE_TAGS:
        rows = fr["once"].get(t)
        if not rows:
            return None, f"missing {t}"
        one[t] = rows[-1] if t != "w1" else rows[0]
    subs = sorted(fr["sub"].keys())
    subs = [s for s in subs if s >= 0]
    if not subs:
        return None, "no substep"
    out = dict(nsub=len(subs))
    out["D_clamp6"] = one["a6a_out"]["mag"] - one["w1"]["mag"]
    out["D_a6b"] = one["a6b_out"]["mag"] - one["a6a_out"]["mag"]
    out["D_damp"] = one["a4_out"]["mag"] - one["a6b_out"]["mag"]
    out["r9f0"] = one["a4_out"]["r9f0"]
    out["r9e0"] = one["w1"]["r9e0"]
    out["m_w1"] = one["w1"]["mag"]
    out["r9e4_w1"] = one["w1"]["r9e4"]
    out["m_snap"] = one["snap"]["mag"]
    out["T_post_probe"] = one["snap"]["mag"] - one["w1"]["mag"]
    prev = one["a4_out"]["mag"]
    first = fr["sub"][subs[0]].get("sub_top")
    if not first:
        return None, "no sub_top"
    out["D_gap"] = first[0]["mag"] - prev
    out["per_sub"] = []
    out["fx_total"] = 0
    last_end = None
    for s in subs:
        t = fr["sub"][s]
        top = t.get("sub_top")
        end = t.get("sub_end")
        if not top or not end:
            return None, f"substep {s} incomplete"
        # the LAST pass of the retry loop is what survives into the next substep
        m_top = top[0]["mag"]
        m_end = end[-1]["mag"]
        ori = t.get("sub_orient")
        whe = t.get("sub_wheel")
        d_ori = (ori[-1]["mag"] - m_top) if ori else 0.0
        d_whe = (whe[-1]["mag"] - ori[-1]["mag"]) if (whe and ori) else 0.0
        d_fix = m_end - (whe[-1]["mag"] if whe else m_top)
        legs = {}
        for leg in ("fx_bounce", "fx_damp", "fx_slide"):
            rows = t.get(leg)
            legs[leg] = len(rows) if rows else 0
        nfx = legs["fx_bounce"]
        out["fx_total"] += nfx
        # within the fixup, split by write when all three legs are present
        d_b = d_d = d_s = float("nan")
        if nfx and whe:
            fb = t.get("fx_bounce")
            fd = t.get("fx_damp")
            fs = t.get("fx_slide")
            base = whe[-1]["mag"]
            if fb:
                d_b = fb[-1]["mag"] - base
            if fb and fd:
                d_d = fd[-1]["mag"] - fb[-1]["mag"]
            if fs:
                prevm = fd[-1]["mag"] if fd else (fb[-1]["mag"] if fb else base)
                d_s = fs[-1]["mag"] - prevm
        out["per_sub"].append(dict(
            s=s, chunk=top[0]["chunk"], rem=top[0]["rem"], npass=len(top),
            D_sub=m_end - m_top, D_orient=d_ori, D_wheel=d_whe, D_fixup=d_fix,
            D_fx_bounce=d_b, D_fx_damp=d_d, D_fx_slide=d_s,
            nfx=nfx, r9ec=end[-1]["r9ec"], key0=end[-1]["key0"]))
        last_end = m_end
    out["D_tail"] = one["snap"]["mag"] - last_end
    return out, None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--sink", required=True)
    ap.add_argument("--port", required=True)
    ap.add_argument("--release", type=int, default=1)
    ap.add_argument("--lo", type=int, default=222)
    ap.add_argument("--hi", type=int, default=250)
    ap.add_argument("--csv")
    a = ap.parse_args()

    frames, bad = load_sink(a.sink)
    port = load_port(a.port)
    pby = {r["frame"]: r for r in port}
    print(f"sink frames {len(frames)}  unparsed lines {bad}  motion_diag matching lines "
          f"{len(port)}")

    # ---------------- CV
    cv = dict(frames=len(frames), miss={t: 0 for t in ONCE_TAGS}, dup={t: 0 for t in ONCE_TAGS},
              orphan_top=0, orphan_end=0, nsub_hist={}, bad_lines=bad)
    for f, fr in frames.items():
        for t in ONCE_TAGS:
            n = len(fr["once"].get(t, []))
            if n == 0:
                cv["miss"][t] += 1
            elif n > 1:
                cv["dup"][t] += 1
        for s, t in fr["sub"].items():
            if "sub_top" in t and "sub_end" not in t:
                cv["orphan_top"] += 1
            if "sub_end" in t and "sub_top" not in t:
                cv["orphan_end"] += 1

    rows = {}
    reject = {}
    for f, fr in frames.items():
        sp, why = split_frame(fr)
        if sp is None:
            reject[why] = reject.get(why, 0) + 1
            continue
        sp["frame"] = f
        sp["d"] = f - a.release
        p = pby.get(f)
        sp["md_s_post"] = p["s_post"] if p else float("nan")
        sp["md_s_mid"] = p["s_mid"] if p else float("nan")
        sp["gnd"] = p["gnd"] if p else float("nan")
        sp["ctrl_xz"] = p["ctrl_xz"] if p else float("nan")
        rows[f] = sp
        cv["nsub_hist"][sp["nsub"]] = cv["nsub_hist"].get(sp["nsub"], 0) + 1
    print("CV:", cv)
    print("rejected frames:", reject)

    sel = [r for r in rows.values() if a.lo <= r["d"] <= a.hi]
    sel.sort(key=lambda r: r["d"])

    # ---------------- KA1 / KA2 / KA3
    ka1 = [r for r in rows.values() if r["m_w1"] == r["r9e4_w1"]]
    ka1f = len(ka1) / len(rows) if rows else 0.0
    def relerr(x, y):
        d = max(abs(x), abs(y), 1e-9)
        return abs(x - y) / d
    ka2v = []
    for r in rows.values():
        tot = r["D_clamp6"] + r["D_a6b"] + r["D_damp"] + r["D_gap"] + r["D_tail"] + \
              sum(p["D_sub"] for p in r["per_sub"])
        ka2v.append(relerr(tot, r["T_post_probe"]))
    ka2f = sum(1 for x in ka2v if x <= KA2_REL) / len(ka2v) if ka2v else 0.0
    ka3v = [relerr(r["m_snap"], r["md_s_post"]) for r in rows.values()
            if math.isfinite(r["md_s_post"])]
    ka3f = sum(1 for x in ka3v if x <= KA3_REL) / len(ka3v) if ka3v else 0.0
    print(f"KA1 exact m(w1)==r9e4: {len(ka1)}/{len(rows)} = {ka1f:.4%}  "
          f"{'PASS' if ka1f >= KA1_FRAC else 'FAIL'}")
    print(f"KA2 telescoping sum: {ka2f:.4%} within {KA2_REL:g} (worst {max(ka2v) if ka2v else float('nan'):.3e})  "
          f"{'PASS' if ka2f >= KA2_FRAC else 'FAIL'}")
    print(f"KA3 snap == motion_diag |vel|: {ka3f:.4%} within {KA3_REL:g} "
          f"(worst {max(ka3v) if ka3v else float('nan'):.3e})  "
          f"{'PASS' if ka3f >= KA3_FRAC else 'FAIL'}")

    # ---------------- EV
    if not sel:
        print("EV FAIL: no frames in window")
        return
    gf = sum(1 for r in sel if abs(r["gnd"] - 4.0) < 1e-6) / len(sel)
    cf = sum(1 for r in sel if r["ctrl_xz"] > 0) / len(sel)
    msp = med([r["md_s_post"] for r in sel])
    evok = (len(sel) >= EV_N and gf >= EV_GND and cf >= EV_CTRL
            and abs(msp - EV_SPEED_REF) <= EV_SPEED_TOL * EV_SPEED_REF)
    print(f"EV d={a.lo}..{a.hi}: n={len(sel)} gnd4={gf:.2%} ctrl>0={cf:.2%} "
          f"median speed={msp:.1f} (ref {EV_SPEED_REF}) median frame={med([r['frame'] for r in sel]):.0f}  "
          f"{'PASS' if evok else 'FAIL'}")

    # ---------------- the split
    Tp = med([r["md_s_post"] - r["md_s_mid"] for r in sel])
    Tp_probe = med([r["T_post_probe"] for r in sel])
    print(f"\nT_post (motion_diag, the budget's own definition) median = {Tp:+.5f}")
    print(f"T_post (probe, m(snap)-m(w1))              median = {Tp_probe:+.5f}")
    print(f"substeps per frame in-window: "
          f"{ {k: sum(1 for r in sel if r['nsub'] == k) for k in sorted({r['nsub'] for r in sel})} }")
    print(f"fixups per frame in-window: median {med([r['fx_total'] for r in sel]):.1f} "
          f"total {sum(r['fx_total'] for r in sel)}")

    terms = [
        ("D_clamp6", "0x004687f0..0x0046897b", [r["D_clamp6"] for r in sel]),
        ("D_a6b", "0x00468980", [r["D_a6b"] for r in sel]),
        ("D_damp", "0x00470948 (+0x9f0==2)", [r["D_damp"] for r in sel]),
        ("D_gap", "A4 exit -> loop top", [r["D_gap"] for r in sel]),
        ("D_tail", "last sub_end -> render tick", [r["D_tail"] for r in sel]),
    ]
    maxsub = max(r["nsub"] for r in sel)
    for s in range(maxsub):
        vals = [p["D_sub"] for r in sel for p in r["per_sub"] if p["s"] == s]
        terms.append((f"D_sub[{s}]", f"substep {s}", vals))
    print(f"\n{'term':<14} {'site':<30} {'n':>5} {'median':>14} {'share of T_post':>16}")
    for name, site, vals in terms:
        m = med(vals)
        sh = (m / Tp) if (Tp and math.isfinite(m)) else float("nan")
        print(f"{name:<14} {site:<30} {len(vals):>5} {m:>+14.6f} {sh:>15.2%}")

    # per-substep legs
    print(f"\n{'sub':>4} {'n':>5} {'chunk':>10} {'nfx':>5} {'D_orient':>12} {'D_wheel':>12} "
          f"{'D_fixup':>12} {'D_bounce':>12} {'D_damp':>12} {'D_slide':>12}")
    for s in range(maxsub):
        ps = [p for r in sel for p in r["per_sub"] if p["s"] == s]
        if not ps:
            continue
        def M(k):
            vv = [p[k] for p in ps if math.isfinite(p[k])]
            return med(vv)
        print(f"{s:>4} {len(ps):>5} {med([p['chunk'] for p in ps]):>10.6f} "
              f"{sum(p['nfx'] for p in ps):>5} {M('D_orient'):>+12.6f} {M('D_wheel'):>+12.6f} "
              f"{M('D_fixup'):>+12.6f} {M('D_fx_bounce'):>+12.6f} {M('D_fx_damp'):>+12.6f} "
              f"{M('D_fx_slide'):>+12.6f}")

    # ---------------- decision rule, PREREG 3
    s2plus = 0.0
    for s in range(2, maxsub):
        vals = [p["D_sub"] for r in sel for p in r["per_sub"] if p["s"] == s]
        if vals:
            s2plus += med(vals)
    cands = [(name, med(vals)) for name, _, vals in terms]
    cands = [(n, v) for n, v in cands if math.isfinite(v)]
    cands.sort(key=lambda kv: -abs(kv[1]))
    top_name, top_val = cands[0]
    print(f"\nDECISION (PREREG 3)")
    print(f"  largest single producer: {top_name} median {top_val:+.6f} "
          f"share {top_val / Tp:.2%}" if Tp else "")
    print(f"  S2plus (substeps s >= 2): {s2plus:+.6f}  share {s2plus / Tp:.2%}" if Tp else "")
    r1 = abs(top_val) >= CARRIER_SHARE * abs(Tp)
    r2 = abs(s2plus) >= CARRIER_SHARE * abs(Tp)
    print(f"  rule 1 (single producer >= {CARRIER_SHARE:.0%}): {'FIRES' if r1 else 'no'} -> {top_name}")
    print(f"  rule 2 (substep budget >= {CARRIER_SHARE:.0%}):  {'FIRES' if r2 else 'no'}")
    if r2:
        print("  CARRIER = THE SUBSTEP BUDGET (U-9160)")
    elif r1:
        print(f"  CARRIER = {top_name}")
    else:
        print("  NO CARRIER DOMINATES -> report the split and STOP (PREREG 3 rule 4)")

    if a.csv:
        import csv as _csv
        with open(a.csv, "w", newline="") as fh:
            w = _csv.writer(fh)
            w.writerow(["d", "frame", "nsub", "fx_total", "md_s_mid", "md_s_post", "T_post",
                        "T_post_probe", "D_clamp6", "D_a6b", "D_damp", "D_gap", "D_tail",
                        "r9f0", "r9e0", "gnd", "ctrl_xz"] +
                       [f"D_sub{s}" for s in range(maxsub)])
            for r in sorted(rows.values(), key=lambda x: x["d"]):
                ds = {p["s"]: p["D_sub"] for p in r["per_sub"]}
                w.writerow([r["d"], r["frame"], r["nsub"], r["fx_total"], r["md_s_mid"],
                            r["md_s_post"], r["md_s_post"] - r["md_s_mid"], r["T_post_probe"],
                            r["D_clamp6"], r["D_a6b"], r["D_damp"], r["D_gap"], r["D_tail"],
                            r["r9f0"], r["r9e0"], r["gnd"], r["ctrl_xz"]] +
                           [ds.get(s, "") for s in range(maxsub)])
        print("\ncsv ->", a.csv)


if __name__ == "__main__":
    main()
