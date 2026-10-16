# D1-residue R3: compare two run_races.py output trees (baseline exe vs post-deletion exe).
#
# Per track:
#   - every race1/*.bmp: SHA-256 equal, else differing-pixel counts (any channel / >16)
#   - drawstream3d.json: per frame, per category; categories present on only one side
#     are listed (the deleted D3D9 draws' world/props/cars tallies)
#   - librw_race.txt: the "ok: scene built" line and the registered-model count
#
# Usage: py -3.12 compare_races.py <base_dir> <post_dir> <report.md>
import hashlib, json, sys
from pathlib import Path

import numpy as np
from PIL import Image


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def pix(pa, pb):
    a = np.asarray(Image.open(pa).convert("RGB"), dtype=np.int16)
    b = np.asarray(Image.open(pb).convert("RGB"), dtype=np.int16)
    if a.shape != b.shape:
        return None
    d = np.abs(a - b).max(axis=2)
    return int((d > 0).sum()), int((d > 16).sum()), a.shape[0] * a.shape[1]


def ds3d(p):
    try:
        return json.loads(p.read_text())
    except Exception as e:
        return {"__error__": str(e)}


def libline(d):
    p = d / "librw_race.txt"
    if not p.exists():
        return None, 0
    lines = p.read_text(errors="replace").splitlines()
    scene = next((l for l in lines if l.startswith("ok: scene built")), None)
    models = sum(1 for l in lines if " registered:" in l)
    return scene, models


def main():
    base, post, out = Path(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3])
    rows, md = [], []
    tot_bmp = same_bmp = 0
    ds_mismatch = []
    ds_only = {}
    scene_mismatch = []
    for bd in sorted(p for p in base.iterdir() if p.is_dir()):
        pd = post / bd.name
        if not pd.is_dir():
            md.append(f"- {bd.name}: MISSING in post")
            continue
        bshots = {p.name: p for p in (bd / "race1").glob("*.bmp")}
        pshots = {p.name: p for p in (pd / "race1").glob("*.bmp")}
        line = [bd.name]
        for name in sorted(set(bshots) | set(pshots)):
            tot_bmp += 1
            if name not in bshots or name not in pshots:
                line.append(f"{name}:ONE-SIDED")
                continue
            if sha(bshots[name]) == sha(pshots[name]):
                same_bmp += 1
                line.append(f"{name}:SAME")
            else:
                r = pix(bshots[name], pshots[name])
                line.append(f"{name}:DIFF{r}")
        # drawstream3d
        jb, jp = ds3d(bd / "drawstream3d.json"), ds3d(pd / "drawstream3d.json")
        for fr in sorted(set(jb) | set(jp)):
            cb, cp = jb.get(fr, {}), jp.get(fr, {})
            for cat in sorted((set(cb) | set(cp)) - {"mode"}):
                if cat in cb and cat in cp:
                    if cb[cat] != cp[cat]:
                        ds_mismatch.append((bd.name, fr, cat, cb[cat], cp[cat]))
                elif cat in cb:
                    ds_only.setdefault(("base-only", cat), []).append((bd.name, fr, cb[cat]))
                else:
                    ds_only.setdefault(("post-only", cat), []).append((bd.name, fr, cp[cat]))
        sb, mb = libline(bd)
        sp, mp = libline(pd)
        if sb != sp or mb != mp:
            scene_mismatch.append((bd.name, sb, sp, mb, mp))
        rows.append(line)
    md.insert(0, "# D1-residue R3: baseline vs post-deletion race captures\n")
    md.append(f"## BMPs\n\n{same_bmp}/{tot_bmp} byte-identical (SHA-256).\n")
    for r in rows:
        md.append("- " + "  ".join(r))
    md.append("\n## drawstream3d (MASHED_DBG_DRAWSTREAM3D=60:62)\n")
    md.append(f"Categories present on both sides with DIFFERENT numbers: {len(ds_mismatch)}")
    for m in ds_mismatch[:20]:
        md.append(f"- {m}")
    for (side, cat), v in sorted(ds_only.items()):
        vals = {json.dumps(x[2], sort_keys=True) for x in v}
        md.append(f"- {side} category `{cat}`: {len(v)} frame-records over "
                  f"{len({x[0] for x in v})} tracks, distinct values: {sorted(vals)}")
    md.append("\n## librw scene line / registered model count\n")
    md.append(f"Mismatching tracks: {len(scene_mismatch)}")
    for m in scene_mismatch:
        md.append(f"- {m}")
    out.write_text("\n".join(md) + "\n", encoding="utf-8")
    print("\n".join(md[:3]))
    print(f"BMP identical {same_bmp}/{tot_bmp}; ds3d differing numbers {len(ds_mismatch)}; "
          f"one-sided categories {sorted(set(k for k in ds_only))}; scene mismatches {len(scene_mismatch)}")


if __name__ == "__main__":
    main()
