"""D3 AI — offline simulator of FUN_00416250's steer/accel/brake bands, plus the
speed/err counterfactual matrix.  re/analysis/D3_SPEED_GAP_2026-09-28.md.

Every input comes from a committed *.aistep.csv; nothing is launched. The point is
to separate the three links of the opponent throttle chain:

  AI ctrl bytes (ctrl[0],[1] steer, [4] accel, [5] brake)
        -> cook/descriptor (standalone: a straight copy, TrackRenderer.cpp:2944)
        -> ported D2 physics  -> rec+0x9e4 (speed)  -> back into the AI's steer band.

WHAT IS RECONSTRUCTED, AND HOW (no guessing; every constant cited in AiStandalone.cpp):
  err  — 0x004165cc writes [0x008032d8+v*0x14]=360 and 0x004165f7 writes
         [0x008032dc+v*0x14]=err on the err<180 branch; 0x004166db writes
         [...dc]=0 and 0x0041670c writes [...d8]=err on the err>180 branch.
         The standalone dump (TrackRenderer::AiStepDump) samples both AFTER the
         tick, so row i's own pair carries call i's err:
             hist_d8 == 360 -> low branch,  err = hist_dc
             hist_dc == 0   -> high branch, err = hist_d8
  X    — the x87 history value the brake rule at 0x00416818 compares with 20:
         low branch  `if (h < err) X = h`      (0x004165d6..0x004165e3)
         high branch `if (err < h) X = 360-h`  (0x004166e5..0x004166f8)
         h = the PRE-write value, i.e. row i-1's column.
  bands— magnitude m = err * speed * 0.0030034 (_DAT_005cd0e8, 0x00416648),
         *= curv*0.05 (_DAT_005cc9a0) when mode==0 && curv>20 (0x0041665c),
         clamped to 255 (_DAT_005cd04c, 0x0041667b), truncated by _ftol2
         (FUN_004a2c48), stored to ctrl[0] (0x00416697) or ctrl[1] (0x004167b1).
         ctrl[0]/[1] are ZEROED before each call — measured, not assumed:
         sa_d1 car 1 idx 63 has err=1.0194 > deadband and c0=4; idx 64 has
         err=0.9714 < deadband so no write fires, yet the byte reads 0.

VALIDATION GATE: the simulator must reproduce the standalone's OBSERVED c0/c1 and
c4/c5 on >= 95% of window calls before any counterfactual is believed.

Usage:
  py -3.12 re/tools/ai_band_sim.py --orig <p*.msd.aistep.csv ...> \
                                   --sa <sa*.csv> [--out <report.txt>] [--n 220]
"""
import csv, json, os, statistics, sys

N_DEFAULT = 220
# FUN_00416250 constants (AiStandalone.cpp:566..579, each with its _DAT_ address)
K_SCALE=0.0030034; K_EXTRA=0.05; K_CLAMP=255.0; K_SPLIT=180.0; K_WRAP=360.0
K_DEAD=1.0; K_359=359.0; K20=20.0; K_CTR=0.005; K_SETTLE=200
K_ERRLO=30.0; K_ERRHI=330.0; K_BRKDEL=15.0; K_BRKMIN=10.0; K_RATE1=2000.0

# D3_AI_RESIDUE_2026-09-27.md section 10.3 — the PURE regime-0 K=220 envelope
# (24 observations: the 8 non-flip original runs x cars 1..3). NOT a band change;
# this file only reads it.
TOL_PURE0 = {"c0_distinct":(13,35), "c1_distinct":(21,70), "steer_distinct":(33,96),
             "c0_median":(0,0), "c1_median":(0,0), "abs_steer_median":(7,23),
             "accel_distinct":(2,3), "accel_median":(255,255),
             "brake_distinct":(2,2), "brake_median":(0,0)}

def rnd(x):
    r = int(x)                      # FUN_004a2c48 = _ftol2, truncates toward zero
    return 0 if r < 0 else (255 if r > 255 else r)

def car_rows(path, v):
    rows = [r for r in csv.DictReader(open(path, newline="")) if int(r["v"]) == v]
    i0 = next((i for i, x in enumerate(rows) if int(x["c4"]) != 0), None)
    return rows, i0

def decode(rows, i):
    """(branch, err, h_prev) for call i, or None when neither branch wrote."""
    d8 = float(rows[i]["hist_d8"]); dc = float(rows[i]["hist_dc"])
    pd8 = float(rows[i-1]["hist_d8"]) if i > 0 else 360.0
    pdc = float(rows[i-1]["hist_dc"]) if i > 0 else 0.0
    if d8 == 360.0:            return ("lo", dc, pdc)
    if dc == 0.0 and d8 != 0.0: return ("hi", d8, pd8)
    return None

def simulate(rows, i0, speeds, n=N_DEFAULT, errseq=None, mode=0,
             modeseq=None, curvseq=None, rateseq=None):
    """modeseq — [D3 STEP 2 / G2-SIM] per-call committed behaviour mode
    (DAT_0089a52c + v*0x74, written 0x00416590), replacing the scalar `mode`.
    The shipping exe pins it to 0 at AiStandalone.cpp:844, so the `mode == 0`
    conjunct of the 0x0041665c curvature multiplier is always true there.
    curvseq — [D3 STEP 2, arm added after G2-STEER, see RESULT_STEP2.md] per-call
    `curv`, the multiplier's other input. Declared as an addition to the
    pre-registration; it changes no decision rule.
    rateseq — [D3 2026-10-03 STEP 2] per-call `rec_b0c` (+0xb0c), the input to the
    R_B0C brake rule at 0x004167eb. Declared in
    verify/d3_elim_20261003/PREREG_STEP2.md section 2.4 BEFORE it was run, in the
    same form and with the same status as curvseq: it substitutes a MEASURED
    sequence from the other side, adds no free parameter and changes no decision
    rule."""
    dirst = 0; lastf = 0; stored = 0; prev_b0c = None
    out = []
    for j in range(n):
        i = i0 + j
        d = decode(rows, i)
        if d is None: continue
        br, err, h = d
        if errseq is not None and errseq[j] is not None:
            br, err, h = errseq[j]
        curv = float(rows[i]["curv"]); speed = speeds[j]
        if curvseq is not None and curvseq[j] is not None:
            curv = curvseq[j]
        if modeseq is not None:
            mode = modeseq[j]
        frame = int(rows[i]["clk_0ff4"]); rate0 = float(rows[i]["rec_b0c"])
        if rateseq is not None and rateseq[j] is not None:
            rate0 = rateseq[j]
        c0 = 0; c1 = 0; X = 0.0
        if br == "lo":
            if h < err: X = h
            if err > K_DEAD:
                el = frame - lastf
                if dirst == 1 or el >= K_SETTLE:
                    m = err * speed * K_SCALE
                    if mode == 0 and curv > K20: m = m * (curv * K_EXTRA)
                    if not (m <= K_CLAMP): m = K_CLAMP
                    c0 = rnd(m); stored = int(m); dirst = 1; lastf = frame
                else:
                    c1 = rnd(float(K_SETTLE - el) * K_CTR * float(stored))
        else:
            if err < h: X = K_WRAP - h
            if err < K_359:
                el = frame - lastf
                if dirst == 2 or el >= K_SETTLE:
                    m = (K_WRAP - err) * speed * K_SCALE
                    if mode == 0 and curv > K20: m = m * (curv * K_EXTRA)
                    if not (m <= K_CLAMP): m = K_CLAMP
                    c1 = rnd(m); stored = int(m); dirst = 2; lastf = frame
                else:
                    c0 = rnd(float(K_SETTLE - el) * K_CTR * float(stored))
        c4 = 255; c5 = 0
        if prev_b0c is None: prev_b0c = rate0
        if rate0 - prev_b0c > K_BRKDEL and rate0 > K_BRKMIN: c4 = 0; c5 = 255
        if X > K20 and speed > K_RATE1:                      c4 = 0; c5 = 255
        if err < K_SPLIT and err > K_ERRLO: c4 = 255; c5 = 255; c0 = 255
        if err > K_SPLIT and err < K_ERRHI: c4 = 255; c5 = 255; c1 = 255
        prev_b0c = rate0
        out.append((c0, c1, c4, c5, X, err))
    return out

def bands(s):
    c0=[x[0] for x in s]; c1=[x[1] for x in s]; c4=[x[2] for x in s]; c5=[x[3] for x in s]
    st=[b-a for a,b in zip(c0,c1)]
    return {"c0_distinct":len(set(c0)), "c0_median":statistics.median(c0),
            "c1_distinct":len(set(c1)), "c1_median":statistics.median(c1),
            "steer_distinct":len(set(st)), "steer_median":statistics.median(st),
            "abs_steer_median":statistics.median([abs(x) for x in st]),
            "accel_distinct":len(set(c4)), "accel_median":statistics.median(c4),
            "brake_distinct":len(set(c5)), "brake_median":statistics.median(c5)}

def score(b, tol=TOL_PURE0):
    return [f"{k}={b[k]} not in [{lo},{hi}]" for k,(lo,hi) in tol.items() if not (lo<=b[k]<=hi)]

def main(argv):
    n = N_DEFAULT
    if "--n" in argv: n = int(argv[argv.index("--n")+1])
    def grab(flag):
        if flag not in argv: return []
        out=[]
        for a in argv[argv.index(flag)+1:]:
            if a.startswith("--"): break
            out.append(a)
        return out
    origs = grab("--orig"); sas = grab("--sa")
    outp = argv[argv.index("--out")+1] if "--out" in argv else None
    buf = []
    def P(s=""):
        buf.append(s); print(s)

    report = {"n": n, "orig": origs, "sa": sas}

    # ---- link 1: the ctrl bytes, both sides -------------------------------
    P("### LINK 1 — AI ctrl bytes over the %d-call window" % n)
    P("%-28s %-3s %-22s %-7s %-14s %-7s" % ("src","v","c4 set","c4=255","c5 set","c5=255"))
    l1 = {}
    for p in origs + sas:
        for v in (1,2,3):
            rows,i0 = car_rows(p,v)
            if i0 is None: continue
            w = rows[i0:i0+n]
            a=[int(x["c4"]) for x in w]; b=[int(x["c5"]) for x in w]
            P("%-28s %-3d %-22s %-7.3f %-14s %-7.3f" % (
                os.path.basename(p), v, sorted(set(a)), sum(q==255 for q in a)/len(a),
                sorted(set(b)), sum(q==255 for q in b)/len(b)))
            l1.setdefault(os.path.basename(p),{})[v]={"accel_set":sorted(set(a)),
                "accel255":sum(q==255 for q in a)/len(a),"brake_set":sorted(set(b)),
                "brake255":sum(q==255 for q in b)/len(b)}
    report["link1"]=l1

    # ---- link 3: physics under MATCHED ctrl -------------------------------
    P(); P("### LINK 3 — speed under MATCHED ctrl (c4=255 and c5=0 on the side quoted)")
    P("%-28s %-3s %-10s %-8s %-10s" % ("src","v","med(all)","n(full)","med(full)"))
    l3={}
    for p in origs + sas:
        for v in (1,2,3):
            rows,i0=car_rows(p,v)
            if i0 is None: continue
            w=rows[i0:i0+n]
            sp=[float(x["rec_9e4"]) for x in w]
            full=[s for s,x in zip(sp,w) if int(x["c4"])==255 and int(x["c5"])==0]
            P("%-28s %-3d %-10.0f %-8d %-10.0f" % (os.path.basename(p), v,
              statistics.median(sp), len(full), statistics.median(full) if full else -1))
            l3.setdefault(os.path.basename(p),{})[v]={"med_all":statistics.median(sp),
                "n_full":len(full),"med_full":statistics.median(full) if full else None}
    report["link3"]=l3

    if not sas:
        if outp: open(outp,"w").write("\n".join(buf)+"\n")
        return 0

    # ---- validation gate --------------------------------------------------
    sa = sas[0]
    P(); P("### VALIDATION GATE — simulator vs OBSERVED standalone (its own speed)")
    gate={}
    for v in (1,2,3):
        rows,i0=car_rows(sa,v)
        if i0 is None: continue
        sp=[float(rows[i0+j]["rec_9e4"]) for j in range(n)]
        s=simulate(rows,i0,sp,n)
        ok=sum(1 for j,x in enumerate(s)
               if (x[0],x[1],x[2],x[3])==(int(rows[i0+j]["c0"]),int(rows[i0+j]["c1"]),
                                          int(rows[i0+j]["c4"]),int(rows[i0+j]["c5"])))
        gate[v]=ok/len(s)
        P("  car %d: all four bytes exact %d/%d = %.3f  %s" % (
            v, ok, len(s), ok/len(s), "PASS" if ok/len(s)>=0.95 else "*** GATE FAILED ***"))
    report["gate"]=gate
    if min(gate.values()) < 0.95:
        P("  gate failed -> no counterfactual reported")
        if outp: open(outp,"w").write("\n".join(buf)+"\n")
        return 1

    # ---- counterfactual matrix -------------------------------------------
    otr={}
    for v in (1,2,3):
        ws=[]
        for t in origs:
            r,i=car_rows(t,v)
            if i is None: continue
            ws.append([float(r[i+j]["rec_9e4"]) for j in range(n)])
        if ws: otr[v]=[statistics.median(w[j] for w in ws) for j in range(n)]
    P(); P("### COUNTERFACTUAL — scored against the PURE regime-0 K=%d envelope" % n)
    P("  NOTE: err and speed are COUPLED in the live loop (a speed change moves the")
    P("  trajectory and therefore next call's err). Holding one fixed is a BOUND, not")
    P("  a prediction of what a rebuilt binary would do.")
    cf={}
    oerr_src = origs[0] if origs else None
    for v in (1,2,3):
        rows,i0=car_rows(sa,v)
        if i0 is None or v not in otr: continue
        own=[float(rows[i0+j]["rec_9e4"]) for j in range(n)]
        oerr=None
        if oerr_src:
            orows,oi0=car_rows(oerr_src,v)
            if oi0 is not None: oerr=[decode(orows,oi0+j) for j in range(n)]
        # [D3 STEP 2 / G2-SIM] the ORIGINAL's own per-call committed mode and curv,
        # taken from the same window index. oerr_src is the original capture.
        omode = ocurv = None
        if oerr_src:
            orows,oi0=car_rows(oerr_src,v)
            if oi0 is not None:
                omode=[int(orows[oi0+j]["ai_mode"]) for j in range(n)]
                ocurv=[float(orows[oi0+j]["curv"]) for j in range(n)]
        variants=[("own speed, own err",own,None,None,None),
                  ("ORIG speed, own err",otr[v],None,None,None)]
        if oerr: variants += [("own speed, ORIG err",own,oerr,None,None),
                              ("ORIG speed, ORIG err",otr[v],oerr,None,None)]
        if omode: variants += [("G2-SIM ORIG modeseq",own,None,omode,None)]
        if ocurv: variants += [("ADDED ORIG curvseq",own,None,None,ocurv),
                               ("ADDED ORIG mode+curv",own,None,omode,ocurv)]
        for lbl,spv,ev,mv,cv in variants:
            b=bands(simulate(rows,i0,spv,n,errseq=ev,modeseq=mv,curvseq=cv)); f=score(b)
            cf.setdefault(v,{})[lbl]={"bands":b,"fails":f}
            P("  car %d [%-21s] c0D=%3d c1D=%3d stD=%3d |st|Med=%5s -> %s" % (
                v,lbl,b["c0_distinct"],b["c1_distinct"],b["steer_distinct"],
                b["abs_steer_median"], "PASS" if not f else "FAIL "+"; ".join(f)))
    report["counterfactual"]=cf

    if outp:
        open(outp,"w").write("\n".join(buf)+"\n")
        json.dump(report, open(os.path.splitext(outp)[0]+".json","w"), indent=1, default=str)
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
