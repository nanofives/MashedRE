"""Per round (sub-state 6 windows from a --continue-rounds log): the player's (car 0) grid position, progress
rank among the 4 cars at the round's first sampled call, and its max speed in the round, from G-AILEAF rows.
Usage: round_census.py <scenario_launch log> <rows.tsv>"""
import re, struct, sys
ev = [(int(m.group(2)), int(m.group(3))) for m in re.finditer(r"sub-state (\S+) -> (\d+)  tick=\d+ sdframe=(\d+)", open(sys.argv[1], encoding="utf-8", errors="replace").read()) for _ in [0] if True for m in [m]]
ev = [(int(s), int(f)) for s, f in re.findall(r"sub-state \S+ -> (\d+)  tick=\d+ sdframe=(\d+)", open(sys.argv[1], encoding="utf-8", errors="replace").read())]
wins = []
for i, (s, f) in enumerate(ev):
    if s == 6:
        end = next((f2 for s2, f2 in ev[i + 1:]), 10 ** 9)
        wins.append((f, end))
rows = []
for ln in open(sys.argv[2]):
    k = ln.rstrip("\n").split("\t")
    if len(k) < 16 or k[0] != "570": continue
    c = [struct.unpack_from("<ffffffIiiiffffii", bytes.fromhex(k[7]), i * 64) for i in range(4)]
    rows.append((int(k[15]), c))
for r, (a, b) in enumerate(wins, 1):
    rr = [x for x in rows if a - 120 <= x[0] < b]
    if not rr: print(f"round {r}: frames {a}..{b} no rows"); continue
    f0, c = rr[0]
    prog = [c[i][10] for i in range(4)]
    mx = max(x[1][0][2] for x in rr if x[0] >= a)
    print(f"round {r}: driving frames {a}..{b} ({b - a}) | grid (x,z,prog) " +
          " ".join(f"c{i}=({c[i][0]:.1f},{c[i][1]:.1f},{prog[i]:.2f})" for i in range(4)) +
          f" | player max speed {mx:.0f} (sampled)")
