"""G-RG-NODE (original side): at each round end (sub-state 6 -> 7, --continue-rounds log with node_63ba80), the last
G-AILEAF row before it: the last car with alive word 1 and trunc(its progress), against the logged DAT_0063ba80.
FUN_00410860 0x00410890..0x004108c5. Usage: node_check.py <log> <rows.tsv>"""
import re, struct, sys
ev = [(int(s), int(f), int(nd)) for s, f, nd in re.findall(r"sub-state \S+ -> (\d+)  tick=\d+ sdframe=(\d+) node_63ba80=(-?\d+)", open(sys.argv[1], encoding="utf-8", errors="replace").read())]
rows = []
for ln in open(sys.argv[2]):
    k = ln.rstrip("\n").split("\t")
    if len(k) < 16 or k[0] != "570": continue
    c = [struct.unpack_from("<ffffffIiiiffffii", bytes.fromhex(k[7]), i * 64) for i in range(4)]
    rows.append((int(k[15]), c))
ok = n = 0
for s, f, nd in ev:
    if s not in (7, 11): continue
    before = [r for r in rows if r[0] < f]
    if not before: continue
    fr, c = before[-1]
    alive = [i for i in range(4) if c[i][6] == 1]
    surv = alive[-1] if alive else 0
    pred = int(c[surv][10])
    n += 1; ok += pred == nd
    print(f"round end at sdframe {f}: last row {fr}, alive {alive}, survivor {surv} prog {c[surv][10]:.4f} -> trunc {pred} | logged node {nd} {'OK' if pred == nd else 'DIFF'}")
print(f"G-RG-NODE {ok}/{n}")
