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

I-NAME rule for unnamed rasters (dflip 2026-10-17, user decision on U-9736): the recorders name a texture with no
dictionary name "#WxH:<D3DFORMAT>" (DrawRec.cpp TexDesc; the original-side recorder the same). Such a name counts as
named on a row when the SAME name is drawn on the other side of that row (original vs standalone): the car-shadow
render textures (#256x256:21 / #64x64:21, FUN_00420050) are drawn under that name by both. A "#..." name drawn on
one side only stays unnamed, so a texture the standalone fails to resolve still fails the gate.
"""
import csv
import json
import re
import sys
from pathlib import Path

RT_NAME = re.compile(r"^#\d+x\d+:\d+$")


def _orig_frame(j):
    """The original side's draws of row j["row"]: between the Present before D(K) and the next Present."""
    k, ev = j["row"], j["events"]
    iD = next(i for i, e in enumerate(ev) if e.get("e") == "D" and e.get("row") == k)
    lo = max((i for i, e in enumerate(ev[:iD]) if e.get("e") == "P"), default=0)
    hi = next((i for i in range(iD + 1, len(ev)) if ev[i].get("e") == "P"), len(ev))
    return [e for e in ev[lo + 1:hi] if e.get("e") == "d"], sum(1 for e in ev[lo + 1:iD] if e.get("e") == "d")


def peer_names(f: Path, orig: bool):
    """Texture names drawn on the other side of a row (empty set when that side has no record)."""
    if f is None or not f.exists():
        return set()
    j = json.loads(f.read_text())
    ev = _orig_frame(j)[0] if orig else [e for e in j["events"] if e.get("e") == "d"]
    return {str(e.get("tex", "")) for e in ev}


def is_named(name, peer):
    n = str(name)
    return not n.startswith("#") or (RT_NAME.match(n) is not None and n in peer)

M12 = [f"{a}{b}" for a in ("r", "u", "a", "p") for b in ("x", "y", "z")]


def orig_gates(d: Path, peer_dir: Path = None):
    """peer_dir: the standalone's d1r2 directory (stand_row_<K>.draws.json), for the I-NAME render-texture rule."""
    out = []
    for f in sorted(d.glob("orig_row_*.draws.json")):
        j = json.loads(f.read_text())
        k = j["row"]
        fr, pre = _orig_frame(j)
        peer = peer_names(peer_dir / f"stand_row_{k:05d}.draws.json" if peer_dir else None, orig=False)
        shim = json.loads((d / f"orig_row_{k:05d}.bmp.draw3d.json").read_text())
        tex = [e for e in fr if e.get("tex") != "-"]
        named = [e for e in tex if is_named(e.get("tex", ""), peer)]
        geo = sum(1 for e in fr if "bb" in e or (e["n"][-1] == 0))
        unn = sorted({e["tex"] for e in tex if not is_named(e["tex"], peer)})
        out.append({
            "row": k, "draws": len(fr), "shim": shim["draw_calls"],
            "I-COVER": len(fr) == shim["draw_calls"], "pre": pre, "I-FRAME": pre == 0,
            "named": f"{len(named)}/{len(tex)}", "I-NAME": len(named) >= 0.95 * len(tex),
            "unnamed": unn, "geometry": f"{geo}/{len(fr)}",
            "gerr": sum(1 for e in fr if "gerr" in e)})
    return out


def stand_gates(d: Path, trace: Path, peer_dir: Path = None):
    """peer_dir: the original capture directory (orig_row_<K>.draws.json), for the I-NAME render-texture rule."""
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
        peer = peer_names(peer_dir / f"orig_row_{k:05d}.draws.json" if peer_dir else None, orig=True)
        named = [e for e in tex if is_named(e.get("tex", ""), peer)]
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
            "unnamed": sorted({e["tex"] for e in tex if not is_named(e["tex"], peer)})})
    return out, len(lib)


def main() -> int:
    base = Path(sys.argv[1])
    tracks = sys.argv[2:] or ["training", "arctic", "city"]
    ok = True
    for t in tracks:
        od = base / f"orig_{t}"
        for g in orig_gates(od, od / "stand" / "d1r2"):
            print(f"orig {t:9} {json.dumps(g)}")
            ok &= g["I-COVER"] and g["I-FRAME"] and g["I-NAME"]
        sd = od / "stand"
        if sd.exists():
            sg, nl = stand_gates(sd, od / "trace.csv", od)
            for g in sg:
                print(f"stand {t:8} {json.dumps(g)}")
                ok &= g["I-SYNC"] and g["I-NAME"]
    print("INSTRUMENT GATES:", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
