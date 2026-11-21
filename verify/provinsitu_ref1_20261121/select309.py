# PROV-INSITU-REF1: the sweep list. t2_nonc4.tsv (log/orch/audit_20261121, copied here) rows with a source
# (best_pct != None), classed as audit xtab.py does; keep game code, RW-Physics, RenderWare core.
import csv, collections
from pathlib import Path
H = Path(__file__).resolve().parent
def cls(s):
    s = s.lower()
    if "lua" in s or "qhull" in s or "crt" in s or "libpng" in s or "zlib" in s: return "library"
    if "renderware-physics" in s: return "RW-Physics"
    if "renderware" in s: return "RW core"
    if s == "": return "no-row"
    return "game"
R = list(csv.DictReader(open(H / "t2_nonc4.tsv", encoding="utf-8"), delimiter="\t"))
keep = [r for r in R if r["best_pct"] != "None" and cls(r["subsystem"]) in ("game", "RW-Physics", "RW core")]
print(collections.Counter(cls(r["subsystem"]) for r in keep), len(keep))
with open(H / "sweep309.tsv", "w", newline="") as f:
    w = csv.writer(f, delimiter="\t"); w.writerow(["rva", "name", "level", "class", "subsystem", "t2_calls", "best_pct", "insitu_1111"])
    for r in keep:
        w.writerow([r["rva"].lower(), r["name"], r["level"], cls(r["subsystem"]), r["subsystem"], r["t2_calls"], r["best_pct"], r["insitu"]])
(H / "sweep309_rvas.txt").write_text(",".join(r["rva"].lower() for r in keep))
