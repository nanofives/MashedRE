"""D-11072 P3 G-P3-COUNT + G-P3-SPEEDTAIL + G-P3-INPUTS scorer (PREREG_P3.md section 3). Read-only.

usage: py -3.12 score_p3.py <port step.csv with ret14a70,tg3_los> <port tgt3 trace csv> <orig aistep.csv> [--n 220]
Window per car: the first N calls from the first c4 != 0 (ai_ctrl_window.py's definition).
Rule per car per kind: PASS if |port - orig| <= max(5, 0.5 * orig).
"""
import csv, sys, collections


def window(rows, n):
    i0 = next((i for i, x in enumerate(rows) if int(x["c4"]) != 0), None)
    return (None, []) if i0 is None else (i0, rows[i0:i0 + n])


def counts(path, n):
    rows = list(csv.DictReader(open(path, newline="")))
    out = {}
    for v in sorted({int(r["v"]) for r in rows}):
        r = [x for x in rows if int(x["v"]) == v]
        i0, w = window(r, n)
        c = collections.Counter(x["ret14a70"] for x in w)
        los = collections.Counter(x.get("tg3_los", x.get("ret16060")) for x in w if x["ret14a70"] == "1")
        out[v] = dict(i0=i0, start=w[0]["frame"] if w else None, n=len(w),
                      r1=c["1"], r2=c["2"], r0=c["0"], rneg=c["-1"], los1=los.get("1", 0))
    return out


def main(a):
    n = int(a[a.index("--n") + 1]) if "--n" in a else 220
    port, trace, orig = a[0], a[1], a[2]
    P, O = counts(port, n), counts(orig, n)
    print("# G-P3-COUNT  window = first %d calls from the first c4 != 0" % n)
    print("car | orig ==1 / ==2 | port ==1 (LOS) / ==2 | not-evaluated | verdict ==1, ==2")
    allok = True
    for v in sorted(set(P) | set(O)):
        p, o = P.get(v, {}), O.get(v, {})
        res = []
        for k in ("r1", "r2"):
            ok = abs(p.get(k, 0) - o.get(k, 0)) <= max(5, 0.5 * o.get(k, 0))
            allok &= ok
            res.append("PASS" if ok else "FAIL")
        print(f"{v} | {o.get('r1')} / {o.get('r2')} | {p.get('r1')} ({p.get('los1')}) / {p.get('r2')} | "
              f"{p.get('rneg')} | {res[0]}, {res[1]}   (port start frame {p.get('start')}, orig {o.get('start')})")
    print("G-P3-COUNT", "PASS" if allok else "FAIL")

    t = list(csv.DictReader(open(trace, newline="")))
    per = collections.defaultdict(list)
    for r in t:
        per[int(r["v"])].append(r)
    print("# G-P3-SPEEDTAIL (ret with +0x9e4 vs ret with |+0x9b0|) and G-P3-INPUTS")
    for v in sorted(per):
        rows = per[v]
        flips = sum(1 for r in rows if r["ret"] != r["ret_alt"])
        first = sum(1 for r in rows[:n] if r["ret"] != r["ret_alt"])
        pr = [float(r["prog"]) for r in rows]
        sp = [float(r["sp"]) for r in rows]
        dsp = [abs(float(r["sp"]) - float(r["sp_now"])) for r in rows]
        d6 = collections.Counter(r["d6d8"] for r in rows)
        print(f"v{v}: calls {len(rows)}  flips whole {flips}  flips first {n} calls {first}  "
              f"prog [{min(pr):.4f}, {max(pr):.4f}]  d6d8 {dict(d6)}  speed max {max(sp):.1f} "
              f"(>0 on {sum(1 for s in sp if s > 0)})  median|sp - |vel||={sorted(dsp)[len(dsp)//2]:.4g} max {max(dsp):.4g}")
    last = t[-1]
    print(f"own `at` row != record +0x9d4: {last['at_mis']} of {last['at_calls']} TG_Fwd calls")


if __name__ == "__main__":
    main(sys.argv[1:])
