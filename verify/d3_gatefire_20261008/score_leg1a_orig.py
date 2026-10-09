"""Score G-ORIG (PREREG_LEG1A.md section 4) from o_leg1a.csv. Pure reads."""
import csv
D = r"C:\Users\maria\Desktop\Proyectos\Mashed\verify\d3_gatefire_20261008"
R = list(csv.DictReader(open(D + r"\o_leg1a.csv")))
def iv(r, k):
    v = r.get(k); return None if v in (None, "") else int(float(v))
def fv(r, k):
    v = r.get(k); return None if v in (None, "") else float(v)
ss = [iv(r, "substate") for r in R]
print("samples", len(R), "substate sequence (run-length):")
seq = []
for s in ss:
    if not seq or seq[-1][0] != s: seq.append([s, 1])
    else: seq[-1][1] += 1
print("  ", seq)
p3 = [r for r in R if iv(r, "substate") == 3]
ins = [iv(r, "in%d" % k) for r in p3 for k in range(8)]
clk3 = [iv(r, "clk_0ff4") for r in p3 if iv(r, "clk_0ff4") is not None]
len3 = (max(clk3) - min(clk3)) / 50 if clk3 else 0
print("G-NOPRESS: phase-3 samples %d, nonzero input bytes %d, phase-3 span %.0f frames by clk/50 -> %s"
      % (len(p3), sum(1 for x in ins if x), len3, "PASS" if p3 and not any(ins) and len3 >= 600 else "FAIL"))
def show(label, r):
    print("  %s ss=%s clk=%s n_d584=%s t_d588=%s dt_100c=%s ea64=%s e9fc=%s 0fd0=%s" % (
        label, r["substate"], r["clk_0ff4"], r["n_d584"], r["t_d588"], r["dt_100c"], r["team_ea64"],
        r["submode"], r["rule_0fd0"]))
    print("     c2194=%s c2198=%s" % ([r["c2194_%d" % v] for v in range(4)], [r["c2198_%d" % v] for v in range(4)]))
    print("     rec(i64,f50,f54,f58,i70)=%s" % [tuple(r["r%d_%s" % (k, f)] for f in ("i64", "f50", "f54", "f58", "i70")) for k in range(4)])
# transitions 3 -> (4) -> 5 and 5 -> 6
idx = [i for i in range(1, len(R)) if ss[i] != ss[i - 1]]
for i in idx:
    if ss[i - 1] in (3, 4, 5) and ss[i] in (4, 5, 6):
        print("transition %s -> %s at sample %d:" % (ss[i - 1], ss[i], i))
        for j in range(max(0, i - 2), min(len(R), i + 3)):
            show("[%d]" % j, R[j])
ea = {}
for r in R:
    ea.setdefault(iv(r, "substate"), set()).add(r["team_ea64"])
print("team_ea64 values per sub-state:", {k: sorted(v) for k, v in ea.items()})
st = {}
for r in R:
    if iv(r, "substate") in (3, 5, 6): st.setdefault(r["dt_100c"], 0); st[r["dt_100c"]] += 1
print("dt_100c values over sub-states 3/5/6:", st)
