"""Summarise a camera_probe.py --matfx capture (PREREG_rtex section 5 amendment values).

    py -3.12 matfx_report.py <trace.csv.matfx.json> [<trace.csv>]

Per snapshot: the globals, and per car the env coefficient, its distance-fade inputs
(squared distance from rec+0x258 to the rec+0x29c object's frame position, FUN_0041f120) and
lerp input (rec+0x284 and its 8-sample window rec+0x200), the coefficient FUN_00420290 would
compute from them, and per part the effect / dual / env texture names. With the trace, the
camera position of the same row is printed next to the rec+0x29c position.
"""
import csv
import json
import re
import sys


def clean(s):
    return re.split(r"[^\x20-\x7e]", s or "")[0] if isinstance(s, str) else s


snaps = json.load(open(sys.argv[1]))
cam = {}
if len(sys.argv) > 2:
    for i, r in enumerate(csv.DictReader(open(sys.argv[2]))):
        cam[i] = (float(r["px"]), float(r["py"]), float(r["pz"]))
for sn in snaps:
    if "s" not in sn:
        print("row", sn.get("row"), "ERR", sn.get("err"))
        continue
    g = sn["s"]
    gl = {k: v for k, v in g.items() if k != "cars"}
    for k in ("dirt_90", "dirt_94"):
        if isinstance(gl.get(k), dict):
            gl[k] = dict(gl[k], name=clean(gl[k].get("name")))
    print(f"row {sn['row']} ba8c {sn['ba8c']} {gl}")
    for i, c in enumerate(g["cars"]):
        p, q = c.get("pos258"), c.get("p29c_pos")
        d2 = sum((a - b) ** 2 for a, b in zip(p, q)) if p and q else None
        lo, hi = g["gloss_35c"], g["gloss_360"]
        f = c["f284"]
        pred = None
        if d2 is not None:
            x = d2 if d2 < 100.0 else 100.0
            x = 0.0 if x < 0.0 else x
            pred = ((hi - lo) * f + lo) * (1.0 - x * 0.01)
        env = c.get("env_tex")
        print(f"  car{i} coef {c['coef']:.6f} pred {pred if pred is None else round(pred, 6)} "
              f"d2 {None if d2 is None else round(d2, 3)} f284 {f:.4f} f288 {c.get('f288')} "
              f"win {[round(v, 3) for v in c.get('win200', [])]} env {clean(env.get('name')) if isinstance(env, dict) else env} "
              f"envfa {env.get('fa') if isinstance(env, dict) else None} i290 {c['i290']} flags {c['flags']:#x}"
              + (f" cam {cam.get(sn['row'])} p29c {q}" if i == 0 else ""))
    if sn is snaps[-1] or sn["row"] == 900:
        c = g["cars"][0]
        for part in c["parts"]:
            ms = []
            for m in part["mats"]:
                s = clean(m.get("tex"))
                if "dual0" in m:
                    t = m["dual0"]["tex"]
                    s += f" DUAL[{clean(t.get('name'))} fa={t.get('fa'):#x} {t.get('w')}x{t.get('h')} d{t.get('d')} cf={t.get('cf'):#x} {m['dual0']['src']}/{m['dual0']['dst']}]" if isinstance(t, dict) else f" DUAL[{t}]"
                if "env0" in m:
                    s += f" ENV[{clean(m['env0']['tex'])} {m['env0']['coef']:.4f} fba {m['env0']['fba']}]"
                ms.append(s)
            print(f"    p{part['p']:3d} g{part['gflags']:#06x} r{part['rflags']} {ms}")
