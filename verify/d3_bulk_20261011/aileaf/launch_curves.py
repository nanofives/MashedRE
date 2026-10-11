"""Launch curves, original vs exe: per car, speed (record +0x9e4) at k frames after the launch frame L
(L = first frame any AI car's speed > 50). Original: G-AILEAF probe rows (sdframe column, all 4 cars per row).
Exe: run_h4 step.csv (frame, v, rec_9e4 for AI cars) + ptrace.log (player f=, v9e4=).
Usage: launch_curves.py orig <rows.tsv> | exe <step.csv> <ptrace.log>"""
import csv, re, struct, sys, collections
KS = (0, 5, 10, 20, 40, 60, 90, 120, 180)
def report(tag, sp):          # sp[car][frame] = speed
    frames = sorted({f for c in sp for f in sp[c]})
    L = next((f for f in frames if any(sp[c].get(f, 0) > 50 for c in (1, 2, 3))), None)
    print(f"{tag}: launch frame L={L}")
    if L is None: return
    for c in sorted(sp):
        row = []
        for k in KS:
            f = L + k
            near = min(sp[c], key=lambda x: abs(x - f)) if sp[c] else None
            row.append("%5.0f" % sp[c][near] if near is not None and abs(near - f) <= 3 else "    -")
        print(f"   car{c} " + " ".join(row) + "   (k=" + ",".join(map(str, KS)) + ")")
if sys.argv[1] == "orig":
    sp = collections.defaultdict(dict)
    for ln in open(sys.argv[2]):
        k = ln.rstrip("\n").split("\t")
        if len(k) < 16 or k[0] != "570": continue
        f = int(k[15]); cb = bytes.fromhex(k[7])
        for c in range(4):
            sp[c][f] = struct.unpack_from("<f", cb, c * 64 + 8)[0]
    report(sys.argv[2].split("/")[-1], sp)
else:
    sp = collections.defaultdict(dict)
    for r in csv.DictReader(open(sys.argv[2])):
        sp[int(r["v"])][int(r["frame"])] = float(r["rec_9e4"])
    for ln in open(sys.argv[3]):
        m = re.match(r"f=(\d+) .*?v9e4=([-0-9.e]+)", ln)
        if m: sp[0][int(m.group(1))] = float(m.group(2))
    report(sys.argv[2].split("/")[-1], sp)
