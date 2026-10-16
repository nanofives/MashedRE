"""U-9683 scorer (PREREG_u9683.md section 3). Read-only on its inputs. Run from this directory:
    py -3.12 score_u9683.py [OFF_NAME ON_NAME]        (defaults M_OFF M_ON)
Inputs: <arm>.rdtrace.tsv (producer P / AI-entry T rows), <arm>.c30.tsv (consumer trace), and the original's
../d3_u9219_20261013/o_rd3.msd.{rd,c30}.tsv (driving window W as score_u9219.py defines it)."""
import collections
import struct
import sys
from pathlib import Path

D = Path(__file__).resolve().parent
O = D.parent / "d3_u9219_20261013"
hexf = lambda h: struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]


def load(path):
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip()]
    ix = {k: i for i, k in enumerate(rows[0])}
    return ix, rows[1:]


# ---------------------------------------------------------------- G-TOOK
def took(name, expect_site, expect_lag):
    ix, rows = load(D / (name + ".rdtrace.tsv"))
    sites = collections.Counter(r[ix["site"]] for r in rows if r[ix["kind"]] == "P")
    lastP = None
    lag = collections.Counter(); eq = tot = 0
    nT = nP = 0
    for r in rows:
        if r[ix["kind"]] == "P":
            lastP = r; nP += 1
        else:
            nT += 1
            if lastP is None:
                continue
            tot += 1
            lag[int(r[ix["ord"]]) - int(lastP[ix["ord"]])] += 1
            eq += r[ix["rd"]] == lastP[ix["rd"]]
    other = "A" if expect_site == "U" else "U"
    ok_lag = lag[expect_lag] >= 0.99 * max(tot, 1)
    ok_site = sites[other] == 0 and sites[expect_site] > 0
    ok_eq = eq >= 0.999 * max(tot, 1)
    print("  %s: P rows %d (sites %s), T rows %d; lag(T.ord - lastP.ord) %s; array==lastP %d/%d"
          % (name, nP, dict(sites), nT, dict(sorted(lag.items())), eq, tot))
    print("    -> expect site %s lag %d: site %s, lag %s (%.2f %%), array-consistent %s -> %s"
          % (expect_site, expect_lag, "OK" if ok_site else "FAIL", "OK" if ok_lag else "FAIL",
             100.0 * lag[expect_lag] / max(tot, 1), "OK" if ok_eq else "FAIL",
             "PASS" if (ok_site and ok_lag and ok_eq) else "FAIL"))
    return ok_site and ok_lag and ok_eq


# ---------------------------------------------------------------- original reference
def orig_window():
    ix, rows = load(O / "o_rd3.msd.rd.tsv")
    w, on = [], False
    for r in rows:
        ba8c = int(r[ix["ba8c"]])
        st = [int(c.split(":")[2], 16) for c in r[ix["cars"]].split(";")]
        if not on:
            if ba8c == 6 and not any(st): on = True
            else: continue
        if ba8c != 6 or any(st): break
        w.append(int(r[ix["clk"]]))
    return w[0], w[-1]


def orig_calls():
    lo, hi = orig_window()
    ix, rows = load(O / "o_rd3.msd.c30.tsv")
    per = collections.defaultdict(list)
    for r in rows:
        if lo <= int(r[ix["frame"]]) <= hi:
            per[int(r[ix["v"]])].append(r)
    return ix, per


def exe_calls(name):
    ix, rows = load(D / (name + ".c30.tsv"))
    per = collections.defaultdict(list)
    for r in rows:
        if r[ix["ba8c"]] == "6":
            per[int(r[ix["v"]])].append(r)
    return ix, per


def zeros(rows, ix): return sum(int(r[ix["refdist"]], 16) == 0 for r in rows)
def ret2(rows, ix): return sum(r[ix["ret"]] == "2" for r in rows)


def first_held(rows, ix):
    i = next((i for i, r in enumerate(rows) if r[ix["held"]] != "0"), None)
    return (i, rows[i][ix["frame"]]) if i is not None else (None, None)


def main():
    off, on = (sys.argv[1], sys.argv[2]) if len(sys.argv) == 3 else ("M_OFF", "M_ON")
    print("== G-TOOK (producer site and the lag the AI tick sees)")
    t_off = took(off, "A", 1); t_on = took(on, "A", 1)   # u9685: both arms run on the u9683 default order (producer after the AI block)
    print("  G-TOOK %s" % ("PASS" if (t_off and t_on) else "FAIL"))

    oix, oper = orig_calls()
    nW = {v: len(oper[v]) for v in (1, 2, 3)}
    print("== original o_rd3 window: consumer calls %s, refdist 0 %s, ret 2 %s" % (
        dict(nW), {v: zeros(oper[v], oix) for v in (1, 2, 3)}, {v: ret2(oper[v], oix) for v in (1, 2, 3)}))
    print("   first held != 0 (call, clock): car 2 %s" % (first_held(oper[2], oix),))

    arm = {}
    for name in (off, on):
        ix, per = exe_calls(name); arm[name] = (ix, per)

    print("== G-REFDIST (refdist-0 consumer calls in the first nW driving calls per car; pass |exe-orig| <= 3)")
    for name in (off, on):
        ix, per = arm[name]
        z = {v: zeros(per[v][:nW[v]], ix) for v in (1, 2, 3)}
        oz = {v: zeros(oper[v], oix) for v in (1, 2, 3)}
        ok = {v: abs(z[v] - oz[v]) <= 3 for v in (1, 2, 3)}
        print("  %s: calls %s refdist0 %s vs orig %s -> %s %s" % (
            name, {v: min(len(per[v]), nW[v]) for v in (1, 2, 3)}, z, oz,
            "PASS" if all(ok.values()) else "FAIL", ok))

    print("== G-REFDIST-VAL (report: driving-call-aligned consumer refdist within 0.01 + 0.08*orig)")
    fr = {}
    for name in (off, on):
        ix, per = arm[name]
        within = n = 0; zero_agree = 0
        per_car = {}
        for v in (1, 2, 3):
            m = min(len(per[v]), nW[v]); w = 0
            for i in range(m):
                o = hexf(oper[v][i][oix["refdist"]]); e = hexf(per[v][i][ix["refdist"]])
                w += abs(e - o) <= 0.01 + 0.08 * abs(o)
                zero_agree += (o == 0.0) == (e == 0.0)
            per_car[v] = (w, m); within += w; n += m
        fr[name] = 100.0 * within / max(n, 1)
        print("  %s: within %d/%d (%.2f %%) per car %s; zero-ness agrees on %d/%d" % (
            name, within, n, fr[name], per_car, zero_agree, n))
    print("  ON - OFF = %+.2f points" % (fr[on] - fr[off]))

    print("== G-MODE7 / U-9685 (returns of 2 in the first nW driving calls; whole run in brackets)")
    for name in (off, on):
        ix, per = arm[name]
        win = {v: ret2(per[v][:nW[v]], ix) for v in (1, 2, 3)}
        whole = {v: ret2(per[v], ix) for v in (1, 2, 3)}
        print("  %s: in-window %s  whole run %s  (original window %s)" % (
            name, win, whole, {v: ret2(oper[v], oix) for v in (1, 2, 3)}))
        print("    car 2 first held != 0 (call, clock): %s (original %s)" % (first_held(per[2], ix), first_held(oper[2], oix)))
        for i, r in enumerate(per[2][:nW[2]]):
            if r[ix["ret"]] == "2":
                print("    car 2 ret 2 at driving call %d clock %s held %s refdist %s (orig same call: refdist %s ret %s)" % (
                    i, r[ix["frame"]], r[ix["held"]], r[ix["refdist"]],
                    oper[2][i][oix["refdist"]] if i < len(oper[2]) else "-", oper[2][i][oix["ret"]] if i < len(oper[2]) else "-"))
    print("  U-9685 %s" % ("CLOSES" if ret2(arm[on][1][2][:nW[2]], arm[on][0]) == 0 else "does NOT close (the 2nd arm keeps car-2 in-window returns of 2)"))

    print("== car 2 refdist, driving calls 12..20 (hex bits): orig / %s / %s" % (off, on))
    for i in range(12, 21):
        print("  call %2d clock %s: %s / %s / %s" % (
            i, oper[2][i][oix["frame"]] if i < len(oper[2]) else "-",
            oper[2][i][oix["refdist"]] if i < len(oper[2]) else "-",
            arm[off][1][2][i][arm[off][0]["refdist"]], arm[on][1][2][i][arm[on][0]["refdist"]]))


main()
