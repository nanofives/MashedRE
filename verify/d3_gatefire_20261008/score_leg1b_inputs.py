"""D-11073 leg 1b step 2 (PREREG_LEG1B.md section 2): summarize the standalone input dump
(sa_l1b_inputs.csv) and the original poll (o_leg1b.csv). Read-only."""
import csv, sys
from collections import Counter
def rows(p): return list(csv.DictReader(open(p)))
S = rows(sys.argv[1] if len(sys.argv) > 1 else "sa_l1b_inputs.csv")
O = rows(sys.argv[2] if len(sys.argv) > 2 else "o_leg1b.csv")
def rng(rs, c):
    v = [r[c] for r in rs if r.get(c, "") != ""]
    return Counter(v).most_common(4)
print("=== STANDALONE (%d rows) ===" % len(S))
for c in ["ba8c","e9fc","ea64","fd0","f1018","f1014","cd29b8","ba78","d7e4","d584","d588","step100c"] + \
         ["r%d_i64" % k for k in range(4)]:
    print("  %-10s %s" % (c, rng(S, c)))
cd = [r for r in S if float(r["countdown"]) > 0]
print("  countdown>0 rows: %d (first %s last %s)" % (len(cd), cd[0]["frame"] if cd else None, cd[-1]["frame"] if cd else None))
for v in range(4):
    mis = sum(1 for r in S if r["bf4g%d" % v] != r["bf4r%d" % v] or r["bf8g%d" % v] != r["bf8r%d" % v])
    print("  v%d alive %s e470 %s cd %s acc(countdown) %s  bf4 %s bf8 %s  getter!=record rows %d" % (
        v, rng(S, "alive%d" % v), rng(S, "e470_%d" % v), rng(S, "cd%d" % v),
        Counter(r["acc%d" % v] for r in cd).most_common(3),
        rng(S, "bf4r%d" % v)[:3], rng(S, "bf8r%d" % v)[:3], mis))
    p = [(r["px%d" % v], r["py%d" % v], r["pz%d" % v]) for r in cd]
    print("     countdown pos distinct %d first %s last %s" % (len(set(p)), p[0] if p else None, p[-1] if p else None))
# transition: first rows after countdown ends
post = [r for r in S if float(r["countdown"]) <= 0][:3]
for r in post:
    print("  post-GO frame %s bf4 %s bf8 %s" % (r["frame"], [r["bf4r%d" % v] for v in range(4)], [r["bf8r%d" % v] for v in range(4)]))
print("\n=== ORIGINAL (%d rows) ===" % len(O))
seq = []
for r in O:
    s = r["substate"]
    if not seq or seq[-1][0] != s: seq.append([s, 0])
    seq[-1][1] += 1
print("  substate runs", seq)
p5 = [r for r in O if r["substate"] == "5"]
p6 = [r for r in O if r["substate"] == "6"]
for c in ["f1018","f1014","ba78","d7e4","d7e4_40","g91bc","ged6c","submode","cd_29b8"]:
    print("  %-8s all %s | p5 %s" % (c, rng(O, c), rng(p5, c)))
for v in range(4):
    print("  v%d cd4f8 %s  acc p5 %s" % (v, rng(O, "cd4f8_%d" % v), [r["acc_%d" % v] for r in p5][-12:]))
    print("     c2194 p5 tail %s  p6 head %s" % ([r["c2194_%d" % v] for r in p5][-4:], [r["c2194_%d" % v] for r in p6][:3]))
    print("     a0a04 p5-last %s p6 head %s | a0a08 p5-last %s p6 head %s" % (
        p5[-1]["a0a04_%d" % v] if p5 else None, [r["a0a04_%d" % v] for r in p6][:2],
        p5[-1]["a0a08_%d" % v] if p5 else None, [r["a0a08_%d" % v] for r in p6][:2]))
    pp = [(r["px_%d" % v], r["py_%d" % v], r["pz_%d" % v]) for r in p5]
    print("     p5 pos distinct %d first %s last %s" % (len(set(pp)), pp[0] if pp else None, pp[-1] if pp else None))
print("  p5 n_d584 first/last %s %s  t_d588 last %s" % (p5[0]["n_d584"], p5[-1]["n_d584"], p5[-1]["t_d588"]) if p5 else "")
