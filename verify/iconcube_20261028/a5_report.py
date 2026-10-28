r"""D5-ICONCUBE-GLOW arm A5: the PUGlow draws of the standalone (knob ON and OFF) next to the original's captures.

    py -3.12 verify\iconcube_20261028\a5_report.py --on <run13 out dir, knob ON> --off <run13 out dir, default>

For every track row run13.py replays, lists the original capture's PUGlow draw events (verify/capfix_20261016/<track>/
or log/d1r1/orig_*/, the same captures run13.py diffs against) and the standalone's, from the per-draw records
(MASHED_DBG_D3DREC) run13.py leaves in <out>/<track>/stand/d1r2/stand_row_<row>.draws.json.
Fields: n = [vertices, primitives], so k = n[0] / 4 particles; rs = the recorder's render-state vector; bb = world box.
Pass (PREREG section 2, arm A5 names the Training rows 884 / 924 / 959): every Training ON row has >= 1 PUGlow draw
with n = [4k, 2k], k >= 1, and every Training OFF row has none. The other 12 tracks are reported, not scored.
"""
import argparse
import importlib.util
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
spec = importlib.util.spec_from_file_location("run13", WT / "verify" / "rsky_20261016" / "run13.py")
r13 = importlib.util.module_from_spec(spec)
sys.argv, _argv = [sys.argv[0]], sys.argv
spec.loader.exec_module(r13)
sys.argv = _argv


def glows(p):
    if not p.exists():
        return None
    d = json.loads(p.read_text())
    return [e for e in d.get("events", []) if e.get("e") == "d" and str(e.get("tex", "")).lower() == "puglow"]


def fmt(evs):
    if evs is None:
        return "(no record)"
    if not evs:
        return "-"
    return "; ".join(f"k={e['n'][0] // 4} n={e['n']} rs={e.get('rs')} bb=[{', '.join(f'{v:.1f}' for v in e.get('bb', []))}]"
                     for e in evs)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--on", required=True)
    ap.add_argument("--off", required=True)
    a = ap.parse_args()
    on, off = Path(a.on), Path(a.off)
    bad = 0
    rows = 0
    for t in sorted(k for k in r13.TRACKS if k < 20):
        d, view, cap, _, _ = r13.TRACKS[t]
        for row in r13.shots_of(t):
            rows += 1
            o = glows(cap / f"orig_row_{row:05d}.draws.json")
            s_on = glows(on / d / "stand" / "d1r2" / f"stand_row_{row:05d}.draws.json")
            s_off = glows(off / d / "stand" / "d1r2" / f"stand_row_{row:05d}.draws.json")
            ok_on = bool(s_on) and all(e["n"][0] % 4 == 0 and e["n"][0] >= 4 and e["n"][1] * 2 == e["n"][0] for e in s_on)
            ok_off = s_off == []
            if t == 13:
                bad += (not ok_on) + (not ok_off)
            print(f"{d:16s} {row:5d}  orig: {fmt(o)}")
            print(f"{'':16s} {'':5s}  ON  : {fmt(s_on)}  {'ok' if ok_on else ('FAIL' if t == 13 else 'no')}")
            print(f"{'':16s} {'':5s}  OFF : {fmt(s_off)}  {'ok' if ok_off else ('FAIL' if t == 13 else 'no')}")
    print(f"rows {rows}  failing Training checks {bad}")
    print("A5", "PASS" if bad == 0 and rows else "FAIL")
    return 0 if bad == 0 and rows else 1


if __name__ == "__main__":
    sys.exit(main())
