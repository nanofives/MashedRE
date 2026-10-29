r"""D5-ICONCUBE-SPIN: compare the IconCube draws of an original-side d1r1 draw record with a standalone one.

    py -3.12 cube_cmp.py <orig.draws.json> <stand.draws.json> [<stand2.draws.json> ...]

A cube draw = a "d" event with 12 triangles and a texture in FUN_00458630's name set (or "-" = untextured).
Events are split into frames at each "P" (present) event. Per frame it prints each cube draw: texture,
rs, translation w[12..14], theta = atan2(w[8], w[10]) in degrees (RotY(theta): at = (sin, 0, cos)), the
largest off-Y-rotation term, and the index of the cube draw among the frame's draws with the texture of
the draw just before it. Then the original-vs-standalone pairing by texture in order.
"""
import json
import math
import sys

NAMES = {"flamethrower", "mine", "mortar", "machinegun", "depthcharge", "missile", "shotgun", "flare",
         "oil", "chaos", "airstrike", "-"}


def frames(path):
    ev = json.load(open(path))["events"]
    out, cur = [], []
    for x in ev:
        if x.get("e") == "P":
            out.append(cur); cur = []
        elif x.get("e") == "d":
            cur.append(x)
    if cur:
        out.append(cur)
    return [f for f in out if f]


def cubes(fr):
    res = []
    for i, x in enumerate(fr):
        if x.get("tri") == 12 and x.get("tex") in NAMES and x.get("w"):
            w = x["w"]
            th = math.degrees(math.atan2(w[8], w[10]))
            off = max(abs(w[1]), abs(w[4]), abs(w[6]), abs(w[9]), abs(w[5] - 1.0))
            prev = fr[i - 1].get("tex") if i else None
            nxt = fr[i + 1].get("tex") if i + 1 < len(fr) else None
            res.append(dict(i=i, n=len(fr), tex=x["tex"], rs=x.get("rs"), t=w[12:15], th=th, off=off,
                            prev=prev, nxt=nxt))
    return res


def show(tag, path):
    fs = frames(path)
    print(f"{tag} {path}: {len(fs)} frame(s)")
    allc = []
    for k, fr in enumerate(fs):
        cs = cubes(fr)
        allc.append(cs)
        for c in cs:
            print(f"  f{k} #{c['i']}/{c['n']} {c['tex']:<8} rs={c['rs']} t=({c['t'][0]:.5f},{c['t'][1]:.5f},"
                  f"{c['t'][2]:.5f}) theta={c['th']:.4f} offY={c['off']:.2e} prev={c['prev']} next={c['nxt']}")
        if not cs:
            print(f"  f{k} no cube draws ({len(fr)} draws)")
    return allc


def main():
    o = show("ORIG", sys.argv[1])
    for p in sys.argv[2:]:
        s = show("STAND", p)
        for k in range(min(len(o), len(s))):
            ot = [c["tex"] for c in o[k]]; st = [c["tex"] for c in s[k]]
            print(f"  pair f{k}: orig {ot} stand {st} order {'SAME' if ot == st else 'DIFF'}")
            for a, b in zip(o[k], s[k]):
                dt = max(abs(a["t"][j] - b["t"][j]) for j in range(3))
                dth = (b["th"] - a["th"] + 180.0) % 360.0 - 180.0
                print(f"    {a['tex']:<8}/{b['tex']:<8} dpos={dt:.2e} dtheta={dth:+.4f} rs {'SAME' if a['rs'] == b['rs'] else 'DIFF ' + str(b['rs'])}")


if __name__ == "__main__":
    main()
