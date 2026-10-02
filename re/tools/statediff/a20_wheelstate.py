#!/usr/bin/env python3
"""a20_wheelstate.py - D2 attempt 20, STEP 2: the per-wheel state-machine comparison.

Registered in verify/d2_wheelstate_20261002/PREREG_STEP2.md BEFORE this file was run.

U-9179 asks which branch of 0x0046f6c0's per-wheel 3-state machine leaves two wheels at
state 0 on the PORT. STEP 1 (RESULT_STEP1.md) found NO transcription defect and showed that
both remaining arms are driven by the same upstream quantity: whether the classifier
0x0046cc40 filled that wheel's record this call. The state machine's complete input set is
three per-wheel fields, and this tool reads them on BOTH sides:

  PORT      MASHED_D2SINK + MASHED_D2SINK_SM -> `wcs_ent` (entry snapshot, 4 lines/call)
                                                `wcs_sm`  (the transition, 4 lines/call)
                                                `wcs_cnt` (bVar16 / states / bVar4 / iVar8)
  ORIGINAL  scenario_launch.py --wheelstate-probe -> <out>.msd.wheelstate.csv
            (entry hooks only; a CONSECUTIVE PAIR of rows is one call's input/output record)

THE REPLAY is the transcribed rule verbatim (RESULT_STEP1.md sections 2-4), integer states,
no tolerance, no fitted constant:

  bVar4  = (#{w: state[w]==1 and fv[w] >= kState2Lo} != 0)
           and (#{w: state[w]==1 and fv[w] <= kStateCmp} == 0)
  per w:  state != 0 -> 0 if ((fv > kState2Lo and bVar4) or key == -1) else 1
          state == 0 -> 2 if (kSpring2 < fv <= 0.0) else 0
  bVar16 = #{w: state'[w] == 1}
  if bVar16 == 4: sel = argmax |fv| with 0x0046fae6..0x0046fbb2's tie-break; state'[sel]=0
  if bVar16 == 3: the single w with state'[w] != 1 becomes 1   (0x0046fc09..0x0046fd29)

Gates KA-P / KA-O / CV / EV and the decision rule are PREREG sections 2 and 3.

Usage:
  py -3.12 re/tools/statediff/a20_wheelstate.py
      --sink  verify/.../p_sm/sink.log  --port-md verify/.../p_sm/motion_diag.log
      --orig-csv verify/.../orig_ws1.msd.wheelstate.csv --orig-msd verify/.../orig_ws1.msd
      [--port-release 1] [--lo 222] [--hi 250] [--ctl-lo 0] [--ctl-hi 100]
      [--csv out.csv]
"""
import argparse
import collections
import csv
import math
import re
import statistics
import struct
import sys

# constants, byte-read from MASHED.exe.unpatched (RESULT_STEP1.md sections 2-3)
K_STATE2LO = 0.019999999552965164   # [0x005ce18c]
K_STATECMP = -0.004999999888241291  # [0x005ceaa0]
K_SPRING2 = -2.0                    # [0x005cc34c]
K_ZERO = 0.0                        # [0x005d757c]

# PREREG 2
KAP_STATE_FRAC = 0.99
KAP_SM_FRAC = 0.995
KAO_FRAC = 0.90
EV_N = 25
EV_SPEED_REF = 633.2
EV_SPEED_TOL = 0.25
# PREREG 3
RULE_WHEELS = 1.0

SINK = re.compile(
    r"f=(?P<f>-?\d+) tag=(?P<tag>\w+) sub=(?P<sub>-?\d+) pass=(?P<pass>-?\d+) "
    r"v=\([^)]*\) mag=(?P<mag>\S+) r9e4=(?P<r9e4>\S+) "
    r"r9e0=(?P<r9e0>\S+) r9f0=(?P<r9f0>-?\d+) r9ec=(?P<r9ec>-?\d+) key0=(?P<key0>-?\d+) "
    r"chunk=(?P<chunk>\S+) rem=(?P<rem>\S+) fx=(?P<fx>-?\d+)"
    r"(?: w=(?P<w>-?\d+) a=(?P<a>\S+) b=(?P<b>\S+) c=(?P<c>\S+) d=(?P<d>\S+))?")
RE_SP = re.compile(r"\bsp=([-+0-9.eE]+)")


def med(xs):
    return statistics.median(xs) if xs else float("nan")


# --------------------------------------------------------------------------- replay
def bvar4(states, fvs):
    """0x0046f827..0x0046f8f1."""
    hi = sum(1 for w in range(4) if states[w] == 1 and fvs[w] >= K_STATE2LO)
    lo = sum(1 for w in range(4) if states[w] == 1 and fvs[w] <= K_STATECMP)
    return (hi != 0) and (lo == 0)


def state_machine(states, fvs, keys):
    """0x0046f8f3..0x0046fa20. -> (out_states, arms)"""
    b4 = bvar4(states, fvs)
    out, arms = [0, 0, 0, 0], []
    for w in range(4):
        s, fv, k = states[w], fvs[w], keys[w]
        if s != 0:
            if fv > K_STATE2LO and b4:
                out[w] = 0
                arms.append("A-demote-bVar4")
            elif k == -1:
                out[w] = 0
                arms.append("A-demote-key")
            else:
                out[w] = 1
                arms.append("A-hold")
        else:
            if K_SPRING2 < fv <= K_ZERO:
                out[w] = 2
                arms.append("B-latch")
            else:
                out[w] = 0
                arms.append("B-stay")
    return out, arms, b4


def tail(states, fvs):
    """0x0046fa26..0x0046fd29: combine, the 4-wheel drop, the bVar16==3 promotion.
    -> (states_after_tail, bVar16_the_gate_sees)"""
    st = list(states)
    n = sum(1 for w in range(4) if st[w] == 1)
    if n == 4:
        a = [abs(fvs[w]) for w in range(4)]
        sel = 1 if a[0] < a[1] else 0
        mx = max(a[0], a[1])
        if mx < a[2]:
            sel, mx = 2, a[2]
        if mx < a[3]:
            sel = 3
        st[sel] = 0
        n = 3
    if n == 3:
        for w in range(4):
            if st[w] != 1:
                st[w] = 1
                break
    return st, n


# --------------------------------------------------------------------------- loaders
def load_sink(path):
    """-> {(f, sub, pass): {'ent': [4 rows], 'sm': [4 rows], 'cnt': row, 'in': row}}, bad"""
    calls, bad = {}, 0
    with open(path, "r", errors="replace") as fh:
        for ln in fh:
            m = SINK.match(ln)
            if not m:
                bad += 1
                continue
            tag = m.group("tag")
            if tag not in ("wcs_ent", "wcs_sm", "wcs_cnt", "wcs_in"):
                continue
            key = (int(m.group("f")), int(m.group("sub")), int(m.group("pass")))
            c = calls.setdefault(key, dict(ent={}, sm={}, cnt=None, inn=None))
            if tag == "wcs_in":
                c["inn"] = dict(mag=float(m.group("mag")), r9e4=float(m.group("r9e4")))
            elif tag == "wcs_cnt":
                c["cnt"] = dict(bVar16=float(m.group("a")), states=float(m.group("b")),
                                bVar4=float(m.group("c")), iVar8=float(m.group("d")))
            elif tag == "wcs_ent":
                c["ent"][int(m.group("w"))] = dict(state=int(float(m.group("a"))),
                                                   fv=float(m.group("b")),
                                                   key=int(float(m.group("c"))))
            else:
                c["sm"][int(m.group("w"))] = dict(sin=int(float(m.group("a"))),
                                                  fv=float(m.group("b")),
                                                  key=int(float(m.group("c"))),
                                                  sout=int(float(m.group("d"))))
    return calls, bad


def load_md_speed(path):
    sp = {}
    with open(path, "r", errors="replace") as fh:
        i = -1
        for ln in fh:
            m = RE_SP.search(ln)
            if not m:
                continue
            i += 1
            sp[i] = float(m.group(1))
    return sp


def load_orig_csv(path):
    rows = []
    with open(path, newline="") as fh:
        for r in csv.DictReader(fh):
            rows.append(dict(
                seq=int(r["seq"]), frame=int(r["frame"]),
                edi=int(r["edi_is_rec"]), speed=float(r["speed"]),
                gnd=float(r["gnd"]), bf8=int(r["bf8"]),
                states=[int(r["w%d_state" % w]) for w in range(4)],
                fvs=[float(r["w%d_fv" % w]) for w in range(4)],
                keys=[int(r["w%d_key" % w]) for w in range(4)]))
    return rows


def msd_release(path):
    b = open(path, "rb").read()
    assert b[:4] == b"MSD1", "not MSD1"
    rec, = struct.unpack_from("<I", b, 4)
    off, R, n = 16, None, 0
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from("<I", b, off)
        if R is None and struct.unpack_from("<i", b, off + 4 + 0xbf8)[0] != 0:
            R = fi
        off += 4 + rec
        n += 1
    return R, n


# --------------------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--sink", required=True)
    ap.add_argument("--port-md", required=True)
    ap.add_argument("--orig-csv", required=True)
    ap.add_argument("--orig-msd", required=True)
    ap.add_argument("--port-release", type=int, default=1)
    ap.add_argument("--lo", type=int, default=222)
    ap.add_argument("--hi", type=int, default=250)
    ap.add_argument("--ctl-lo", type=int, default=0)
    ap.add_argument("--ctl-hi", type=int, default=100)
    ap.add_argument("--csv")
    a = ap.parse_args()

    print("a20_wheelstate  U-9179  state machine 0x0046f8f3..0x0046fa20")
    print("  PREREG verify/d2_wheelstate_20261002/PREREG_STEP2.md (committed unrun)")

    # ---------------- PORT
    calls, bad = load_sink(a.sink)
    sp = load_md_speed(a.port_md)
    pc = []
    for (f, sub, ps), c in sorted(calls.items()):
        if len(c["ent"]) != 4 or len(c["sm"]) != 4:
            continue
        pc.append(dict(f=f, sub=sub, ps=ps, d=f - a.port_release,
                       ent=[c["ent"][w] for w in range(4)],
                       sm=[c["sm"][w] for w in range(4)], cnt=c["cnt"],
                       sp=sp.get(f, float("nan"))))
    print("\n--- GATE CV (PORT) ---")
    print("  sink %s: %d parsed calls, %d unparsed lines, %d complete calls"
          % (a.sink, len(calls), bad, len(pc)))
    hsub = collections.Counter(len(set(x["sub"] for x in pc if x["f"] == f))
                               for f in set(x["f"] for x in pc))
    print("  substeps/frame histogram: %s" % dict(sorted(hsub.items())))
    print("  frames with a complete call: %d" % len(set(x["f"] for x in pc)))

    # ---------------- ORIGINAL
    orows = load_orig_csv(a.orig_csv)
    R, nfr = msd_release(a.orig_msd)
    print("\n--- GATE CV (ORIG) ---")
    print("  csv %s: %d rows, %d dropped by edi filter"
          % (a.orig_csv, len(orows), sum(1 for r in orows if r["edi"] == 0)))
    print("  msd %s: %d frames, release first +0xbf8 != 0 at frame %s"
          % (a.orig_msd, nfr, R))
    if R is None:
        raise SystemExit("ORIG: no release marker -- STOP")
    orows = [r for r in orows if r["edi"] == 1]
    for r in orows:
        r["d"] = r["frame"] - R
    ho = collections.Counter(collections.Counter(r["frame"] for r in orows).values())
    print("  solver calls/frame histogram: %s" % dict(sorted(ho.items())))

    # ---------------- GATE KA-P : the replay, validated on the PORT
    nst = nok = nsm = nsmok = 0
    for i in range(1, len(pc)):
        p, q = pc[i - 1], pc[i]
        st_in = [p["ent"][w]["state"] for w in range(4)]
        fvs = [q["ent"][w]["fv"] for w in range(4)]
        keys = [q["ent"][w]["key"] for w in range(4)]
        out, _, _ = state_machine(st_in, fvs, keys)
        post, _ = tail(out, fvs)
        nst += 1
        if post == [q["ent"][w]["state"] for w in range(4)]:
            nok += 1
        # the pre-drop state' against the directly measured wcs_sm of the SAME call
        st_in2 = [p["sm"][w]["sin"] for w in range(4)] if False else st_in
        out2, _, _ = state_machine(st_in, [p["sm"][w]["fv"] for w in range(4)],
                                   [p["sm"][w]["key"] for w in range(4)])
        nsm += 1
        if out2 == [p["sm"][w]["sout"] for w in range(4)]:
            nsmok += 1
    fr_st = nok / nst if nst else 0.0
    fr_sm = nsmok / nsm if nsm else 0.0
    print("\n--- GATE KA-P (replay validated against the PORT's measured state machine) ---")
    print("  entry-pair states reproduced : %d/%d = %.4f %%  (bar %.1f %%)  %s"
          % (nok, nst, 100 * fr_st, 100 * KAP_STATE_FRAC,
             "PASS" if fr_st >= KAP_STATE_FRAC else "FAIL"))
    print("  pre-drop state' == wcs_sm    : %d/%d = %.4f %%  (bar %.1f %%)  %s"
          % (nsmok, nsm, 100 * fr_sm, 100 * KAP_SM_FRAC,
             "PASS" if fr_sm >= KAP_SM_FRAC else "FAIL"))

    # ---------------- GATE KA-O : the replay reproduces the ORIGINAL's own states
    no = nookay = 0
    omiss = collections.Counter()
    for i in range(1, len(orows)):
        p, q = orows[i - 1], orows[i]
        out, _, _ = state_machine(p["states"], q["fvs"], q["keys"])
        post, _ = tail(out, q["fvs"])
        no += 1
        if post == q["states"]:
            nookay += 1
        else:
            omiss[(tuple(p["states"]), tuple(post), tuple(q["states"]))] += 1
    fr_o = nookay / no if no else 0.0
    print("\n--- GATE KA-O (replay reproduces the ORIGINAL's own states) ---")
    print("  %d/%d = %.4f %%  (bar %.1f %%)  %s"
          % (nookay, no, 100 * fr_o, 100 * KAO_FRAC,
             "PASS" if fr_o >= KAO_FRAC else "FAIL"))
    for k, v in omiss.most_common(6):
        print("    miss in=%s predicted=%s actual=%s  x%d" % (k[0], k[1], k[2], v))

    # ---------------- the windows
    out_rows = []
    for name, lo, hi in (("CARRIER", a.lo, a.hi), ("LAUNCH-CONTROL", a.ctl_lo, a.ctl_hi)):
        print("\n=== WINDOW %s  d = %d..%d  (L = 0) ===" % (name, lo, hi))
        # PORT, per frame: the state machine's own inputs, measured directly
        pf = collections.defaultdict(list)
        for x in pc:
            if lo <= x["d"] <= hi:
                pf[x["d"]].append(x)
        pK, pF, pS1, pSp, pFr = [], [], [], [], []
        parms = collections.Counter()
        pdrift_states = collections.Counter()
        for d in sorted(pf):
            for x in pf[d]:
                keys = [x["sm"][w]["key"] for w in range(4)]
                fvs = [x["sm"][w]["fv"] for w in range(4)]
                sout = [x["sm"][w]["sout"] for w in range(4)]
                pK.append(sum(1 for k in keys if k != -1))
                pF.append(sum(1 for v in fvs if K_SPRING2 < v <= K_ZERO))
                pS1.append(sum(1 for s in sout if s == 1))
                st_in = [x["sm"][w]["sin"] for w in range(4)]
                _, arms, _ = state_machine(st_in, fvs, keys)
                for w in range(4):
                    parms[arms[w]] += 1
                pdrift_states[tuple(sout)] += 1
            pSp.append(pf[d][0]["sp"])
            pFr.append(d + a.port_release)
        # ORIG, per solver call
        oK, oF, oS1, oSp, oFr = [], [], [], [], []
        oarms = collections.Counter()
        ostates = collections.Counter()
        od = collections.defaultdict(list)
        for r in orows:
            if lo <= r["d"] <= hi:
                od[r["d"]].append(r)
        for i in range(1, len(orows)):
            p, q = orows[i - 1], orows[i]
            if not (lo <= p["d"] <= hi):
                continue
            out, arms, _ = state_machine(p["states"], q["fvs"], q["keys"])
            oK.append(sum(1 for k in q["keys"] if k != -1))
            oF.append(sum(1 for v in q["fvs"] if K_SPRING2 < v <= K_ZERO))
            oS1.append(sum(1 for s in out if s == 1))
            for w in range(4):
                oarms[arms[w]] += 1
            ostates[tuple(out)] += 1
        for d in sorted(od):
            oSp.append(od[d][0]["speed"])
            oFr.append(d + R)

        print("  n (solver calls)         ORIG %4d          PORT %4d" % (len(oK), len(pK)))
        print("  n (frames)               ORIG %4d          PORT %4d" % (len(oSp), len(pSp)))
        print("  median speed             ORIG %9.2f     PORT %9.2f" % (med(oSp), med(pSp)))
        print("  median frame index       ORIG %9.1f     PORT %9.1f" % (med(oFr), med(pFr)))
        print("  K  median (key != -1)    ORIG %9.3f     PORT %9.3f   |delta| %.3f"
              % (med(oK), med(pK), abs(med(oK) - med(pK))))
        print("  F  median (-2 < fv <= 0) ORIG %9.3f     PORT %9.3f   |delta| %.3f"
              % (med(oF), med(pF), abs(med(oF) - med(pF))))
        print("  S1 median (stateOut==1)  ORIG %9.3f     PORT %9.3f   |delta| %.3f"
              % (med(oS1), med(pS1), abs(med(oS1) - med(pS1))))
        print("  ORIG arms: %s" % dict(oarms))
        print("  PORT arms: %s" % dict(parms))
        print("  ORIG stateOut words: %s" % dict(ostates.most_common(6)))
        print("  PORT stateOut words: %s" % dict(pdrift_states.most_common(6)))
        if name == "CARRIER":
            evn = len(pSp) >= EV_N and len(oSp) >= EV_N
            evs = abs(med(pSp) - EV_SPEED_REF) <= EV_SPEED_TOL * EV_SPEED_REF
            print("  GATE EV: n >= %d %s ; port median speed within %.0f %% of %.1f %s"
                  % (EV_N, "PASS" if evn else "FAIL", 100 * EV_SPEED_TOL,
                     EV_SPEED_REF, "PASS" if evs else "FAIL"))
            dK = abs(med(oK) - med(pK))
            dF = abs(med(oF) - med(pF))
            dS = abs(med(oS1) - med(pS1))
            print("\n  --- DECISION RULE (PREREG section 3) ---")
            if dK >= RULE_WHEELS:
                print("  RULE 1 FIRES on K: the diverging term is the CLASSIFIER's "
                      "per-wheel yield (key, +0x1ec, written by 0x0046cc40 only on a new "
                      "contact). |delta| = %.3f wheels." % dK)
            elif dF >= RULE_WHEELS:
                print("  RULE 1 FIRES on F: the diverging term is fv (+0x194) = the "
                      "classifier's depth. |delta| = %.3f wheels." % dF)
            elif dS >= RULE_WHEELS:
                print("  RULE 2: inputs agree, outputs differ -- CONTRADICTS STEP 1. STOP.")
            else:
                print("  RULE 3: K, F and S1 all agree within %.1f wheel -- U-9179 is NOT "
                      "reproduced on this arm. Report named and unfixed." % RULE_WHEELS)
        for d in sorted(set(list(pf) + list(od))):
            out_rows.append(dict(window=name, d=d,
                                 orig_n=len(od.get(d, [])), port_n=len(pf.get(d, []))))
    if a.csv:
        with open(a.csv, "w", newline="") as fh:
            wr = csv.DictWriter(fh, fieldnames=["window", "d", "orig_n", "port_n"])
            wr.writeheader()
            wr.writerows(out_rows)
        print("\n  csv -> %s" % a.csv)


if __name__ == "__main__":
    sys.exit(main())
