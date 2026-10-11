"""G-RG-GRID: original round-start grid positions (frozen countdown rows, o_rounds7b) vs the exe's Stage-A
formation at the same restart node (MASHED_ROUND_GRID_CHECK dump), matched as SETS (best permutation).
Usage: grid_compare.py <o_rounds7b.log> <o_rounds7b.msd.aileaf.tsv> <grid_check.txt>"""
import itertools, re, struct, sys
log = open(sys.argv[1], encoding="utf-8", errors="replace").read()
ev = [(int(s), int(f), int(nd)) for s, f, nd in re.findall(r"sub-state \S+ -> (\d+)  tick=\d+ sdframe=(\d+) node_63ba80=(-?\d+)", log)]
rows = []
for ln in open(sys.argv[2]):
    k = ln.rstrip("\n").split("\t")
    if len(k) < 16 or k[0] != "570": continue
    c = [struct.unpack_from("<ffffffIiiiffffii", bytes.fromhex(k[7]), i * 64) for i in range(4)]
    rows.append((int(k[15]), [(c[i][0], c[i][1]) for i in range(4)], max(c[i][2] for i in range(4))))
exe = {}
for ln in open(sys.argv[3]):
    m = re.match(r"node (\d+) .*", ln)
    if m:
        exe[int(m.group(1))] = [tuple(map(float, p)) for p in re.findall(r"s\d=\(([-0-9.e]+),([-0-9.e]+)\)", ln)]
worst_all = 0.0
for i, (s, f, nd) in enumerate(ev):
    if s != 5: continue                     # countdown start: grid placed, cars frozen until 6
    end = next((f2 for s2, f2, _ in ev[i + 1:]), None)
    # the AI cars move during sub-state 5 (Branch A launch), so take the first row with ALL four speeds 0
    cd = [r for r in rows if f - 150 <= r[0] < (end or 10 ** 9) and r[2] < 1.0]
    if not cd or nd not in exe: print(f"countdown at {f}: node {nd} no all-stationary row / no exe formation"); continue
    orig = cd[-1][1]
    best = min(itertools.permutations(range(4)), key=lambda p: max(max(abs(orig[a][0] - exe[nd][p[a]][0]), abs(orig[a][1] - exe[nd][p[a]][1])) for a in range(4)))
    err = max(max(abs(orig[a][0] - exe[nd][best[a]][0]), abs(orig[a][1] - exe[nd][best[a]][1])) for a in range(4))
    worst_all = max(worst_all, err)
    print(f"node {nd:2d} (countdown frame {f}): max |dx|,|dz| = {err:.4f}  {'PASS' if err <= 0.02 else 'FAIL'}  orig car -> exe slot {best}")
print(f"G-RG-GRID worst = {worst_all:.4f} (tolerance 0.02)")
