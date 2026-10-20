import json, math
ROWS = [("13_Training", "log/d1r1/orig_training", 959),
        ("07_Roundabout", "verify/capfix_20261016/07_Roundabout", 1091),
        ("07_Roundabout", "verify/capfix_20261016/07_Roundabout", 1131)]
def wheels(p):
    return [e for e in json.load(open(p))["events"]
            if e.get("e") == "d" and e.get("tex") == "Advantage" and e.get("tri") == 96]
def yaw(w): return math.degrees(math.atan2(w[2], -w[0]))
def bbmove(a, b): return max(abs(x - y) for x, y in zip(a["bb"], b["bb"]))
for d, cap, k in ROWS:
    A = wheels(f"{cap}/orig_row_{k:05d}.draws.json")
    for tag, root in (("trin", "D:/MashedWT/wcsflip_data"), ("def ", "D:/MashedWT/f5flip_data")):
        B = wheels(f"{root}/tracks_def/{d}/stand/d1r2/stand_row_{k:05d}.draws.json")
        worst = []
        for b in B:
            a = min(A, key=lambda a: sum((a["w"][12 + i] - b["w"][12 + i]) ** 2 for i in range(3)))
            dp = max(abs(a["w"][12 + i] - b["w"][12 + i]) for i in range(3))
            dy = (yaw(b["w"]) - yaw(a["w"]) + 180) % 360 - 180
            worst.append((bbmove(a, b), dp, dy, b["w"][12:15]))
        worst.sort(reverse=True)
        print(f"{d} {k} {tag} wheels A={len(A)} B={len(B)}  max dpos over all wheels {max(w[1] for w in worst):.4f}")
        for bm, dp, dy, p in worst[:4]:
            print(f"    bbmove={bm:.4f} dpos={dp:.4f} dsteer={dy:7.2f} deg at {[round(x, 2) for x in p]}")
