"""d1r1 instrument gates (PREREG_d1r1.md section 2) over every adjudicated frame.

    py -3.12 check_gates.py <log/d1r1 dir> [track ...]

Original side, per orig_row_<K>.draws.json:
  I-COVER  recorder draws in the frame == the shim's draw_calls in orig_row_<K>.bmp.draw3d.json
  I-FRAME  no draw between the Present before D(K) and D(K)
  I-NAME   >= 95% of textured draws resolve to a name (stage 0)
Standalone side, per stand_row_<K>.draws.json:
  I-SYNC   actual_row == K, recorder armed, and the librw camera frame for row K equals the
           trace's M_* to 1e-5 (librw_dump.csv readback; cam_dump.csv src == 4)
  I-NAME   >= 95% of textured draws resolve to a name
"""
import csv
import json
import sys
from pathlib import Path

M12 = [f"{a}{b}" for a in ("r", "u", "a", "p") for b in ("x", "y", "z")]


def orig_gates(d: Path):
    out = []
    for f in sorted(d.glob("orig_row_*.draws.json")):
        j = json.loads(f.read_text())
        k, ev = j["row"], j["events"]
        iD = next(i for i, e in enumerate(ev) if e.get("e") == "D" and e.get("row") == k)
        lo = max((i for i, e in enumerate(ev[:iD]) if e.get("e") == "P"), default=0)
        hi = next((i for i in range(iD + 1, len(ev)) if ev[i].get("e") == "P"), len(ev))
        fr = [e for e in ev[lo + 1:hi] if e.get("e") == "d"]
        pre = sum(1 for e in ev[lo + 1:iD] if e.get("e") == "d")
        shim = json.loads((d / f"orig_row_{k:05d}.bmp.draw3d.json").read_text())
        tex = [e for e in fr if e.get("tex") != "-"]
        named = [e for e in tex if not str(e.get("tex", "")).startswith("#")]
        geo = sum(1 for e in fr if "bb" in e or (e["n"][-1] == 0))
        unn = sorted({e["tex"] for e in tex if str(e["tex"]).startswith("#")})
        out.append({
            "row": k, "draws": len(fr), "shim": shim["draw_calls"],
            "I-COVER": len(fr) == shim["draw_calls"], "pre": pre, "I-FRAME": pre == 0,
            "named": f"{len(named)}/{len(tex)}", "I-NAME": len(named) >= 0.95 * len(tex),
            "unnamed": unn, "geometry": f"{geo}/{len(fr)}",
            "gerr": sum(1 for e in fr if "gerr" in e)})
    return out


def stand_gates(d: Path, trace: Path):
    out = []
    rows = list(csv.DictReader(open(trace)))
    sd = d / "d1r2"
    lib = list(csv.DictReader(open(d / "librw_dump.csv"))) if (d / "librw_dump.csv").exists() else []
    cam = list(csv.DictReader(open(d / "cam_dump.csv"))) if (d / "cam_dump.csv").exists() else []
    for f in sorted(sd.glob("stand_row_*.draws.json")):
        j = json.loads(f.read_text())
        k = j["row"]
        ev = [e for e in j["events"] if e.get("e") == "d"]
        tex = [e for e in ev if e.get("tex") != "-"]
        named = [e for e in tex if not str(e.get("tex", "")).startswith("#")]
        # camera: the cam_dump row(s) whose replay row == K
        cr = [c for c in cam if c.get("row") == str(k)]
        src = sorted({c.get("src") for c in cr})
        dmax = None
        if cr:
            c = cr[-1]
            t = rows[k]
            want = [float(t[f"M_{m}"]) for m in ("px", "py", "pz", "rx", "ry", "rz", "ux", "uy", "uz",
                                                   "ax", "ay", "az")]
            got = [float(c[x]) for x in ("ex", "ey", "ez", "rx", "ry", "rz", "ux", "uy", "uz",
                                          "ax", "ay", "az")]
            dmax = max(abs(a - b) for a, b in zip(want, got))
        out.append({
            "row": k, "actual_row": j.get("actual_row"), "armed": j.get("armed"),
            "draws": len(ev), "empty": j.get("empty"),
            "cam_src": src, "cam_max_abs_dev": dmax,
            "I-SYNC": j.get("actual_row") == k and j.get("armed") == 1 and src == ["4"]
                      and dmax is not None and dmax <= 1e-5,
            "named": f"{len(named)}/{len(tex)}", "I-NAME": len(named) >= 0.95 * len(tex),
            "unnamed": sorted({e["tex"] for e in tex if str(e["tex"]).startswith("#")})})
    return out, len(lib)


def main() -> int:
    base = Path(sys.argv[1])
    tracks = sys.argv[2:] or ["training", "arctic", "city"]
    ok = True
    for t in tracks:
        od = base / f"orig_{t}"
        for g in orig_gates(od):
            print(f"orig {t:9} {json.dumps(g)}")
            ok &= g["I-COVER"] and g["I-FRAME"] and g["I-NAME"]
        sd = od / "stand"
        if sd.exists():
            sg, nl = stand_gates(sd, od / "trace.csv")
            for g in sg:
                print(f"stand {t:8} {json.dumps(g)}")
                ok &= g["I-SYNC"] and g["I-NAME"]
    print("INSTRUMENT GATES:", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
