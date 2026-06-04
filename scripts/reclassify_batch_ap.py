"""Central re-classify for batch_ap (gameplay campaign FINAL batch 5/5, last-168 0x562460..0x57ae30).

The batch_ap header's dominant-library_skip prediction held COMPLETELY:
- 0 plated RVAs (all six sessions ALL-SKIP, rvas=NONE — nothing promoted, no U-/S- mints;
  reserved ranges U-8900..9199 / S-7000..7199 untouched).
- 168 library_skip reclass-OUT to third-party-library[RenderWare-Physics-3.7], kept C1
  (NOT promoted, NOT hand-plated): the entire remaining gameplay-C1 band is the vendored
  RW-Physics module (world-collision import / convex-hull / loose-octree broad-phase /
  rigid-body math / island+solver core / CCD / narrow-phase GJK-SAT tail).
- qhull watch (s6): NEGATIVE — none qh_*-shaped; qhull island still starts 0x0057c5b0.
- Tag flags:
    * 00562460/00562500 (s1) — RpWorld API boundary (RpWorldForAllWorldSectors driver +
      sector callback); tag could arguably be plain renderware; either way vendored, kept C1
      (same flag shape as ao_s3's 005592f0).
- This CLOSES the gameplay C1->C2 campaign: gameplay C1 168 -> 0.

Run: py -3.12 scripts/reclassify_batch_ap.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260603-2132"
DATE = "2026-06-03"
APPLY = "--apply" in sys.argv

S1 = ["00562460", "00562500", "00562560", "00562580", "005625e0", "00562600",
      "00562660", "00562ed0", "00562fd0", "00563810", "00563840", "00563940",
      "00563b00", "00563c80", "00563de0", "00563df0", "00563e70", "00563f60",
      "00564040", "00564100", "00564130", "00564190", "005641b0", "00564310",
      "005645b0", "005646c0", "00564c80", "00565120"]
S2 = ["00565160", "005651b0", "00565200", "00565260", "00565550", "00565570",
      "00565590", "005655a0", "00565780", "005659f0", "00565ab0", "00565bc0",
      "00565c20", "00565cd0", "00565d50", "00565ef0", "00565fa0", "00566050",
      "00566200", "005663a0", "00566420", "005665a0", "005667c0", "00566830",
      "005668e0", "00566aa0", "00566b40", "00566c10"]
S3 = ["00566ca0", "00566d50", "00566ea0", "005670a0", "005672c0", "00567350",
      "005675d0", "00567630", "005676f0", "00567c00", "00567c40", "00567c60",
      "00567f00", "005684c0", "00568560", "005685f0", "005687a0", "005687d0",
      "00568860", "00568990", "00568c40", "00568c80", "00568cf0", "00568d20",
      "00568dd0", "00568fd0", "00569690", "00569e90"]
S4 = ["0056a250", "0056a450", "0056a7a0", "0056aae0", "0056ac40", "0056adb0",
      "0056b7a0", "0056b9d0", "0056ba30", "0056bb30", "0056bb80", "0056bce0",
      "0056bdf0", "0056be80", "0056c0a0", "0056c310", "0056c580", "0056c8e0",
      "0056caa0", "0056cf90", "0056d070", "0056d350", "0056d3f0", "0056dd40",
      "0056e680", "0056ed60", "0056ef30", "0056efc0"]
S5 = ["0056f020", "0056f0a0", "0056f1f0", "0056f350", "0056fad0", "0056fb90",
      "0056fea0", "00570090", "005729a0", "005735f0", "00573670", "00573890",
      "005739d0", "00574230", "005742c0", "005748a0", "00574920", "005749b0",
      "00574a20", "00574ac0", "00574ad0", "00575120", "005751f0", "005752b0",
      "00575560", "005757d0", "00575880", "00575b60"]
S6 = ["00575c60", "00575fe0", "00576640", "00576880", "00577be0", "00577cb0",
      "00577ec0", "005784a0", "00578610", "00578b20", "00578bd0", "00578cb0",
      "00578d90", "00578e50", "00578ec0", "00578f20", "00578ff0", "00579b50",
      "00579c00", "00579d50", "00579e50", "00579ee0", "0057a250", "0057a660",
      "0057a9a0", "0057adb0", "0057ae20", "0057ae30"]

CLUSTER = {
    "s1": "world-collision import + convex-hull support + loose-octree broad-phase",
    "s2": "octree node tail + rigid-body math kernel (inertia/Jacobi-eigen/AABB) + poly roots",
    "s3": "ray-TOI primitives + island construction (union-find) + contact hash + PGS rows",
    "s4": "constraint-solver core (SAT/GJK axis, LDLt, PGS scalar+SSE twins, quat integrate)",
    "s5": "solver/CCD/narrow-phase dispatch + mass-property core (no qhull shapes)",
    "s6": "narrow-phase tail (pair emit + contact clip + SAT + manifold + GJK + CA-TOI); qhull watch NEGATIVE",
}

LIBSKIP: dict[str, str] = {}   # rva -> session
for sess, lst in (("s1", S1), ("s2", S2), ("s3", S3), ("s4", S4), ("s5", S5), ("s6", S6)):
    for r in lst:
        assert r not in LIBSKIP, f"duplicate {r}"
        LIBSKIP[r] = sess
assert len(LIBSKIP) == 168, f"expected 168 library_skip, got {len(LIBSKIP)}"

TAG = "RenderWare-Physics-3.7"
TAG_DOUBT = {
    "00562460": " [tag-doubt: RpWorldForAllWorldSectors import driver — RpWorld API boundary, "
                "renderware-vs-RW-Physics link unit UNCERTAIN, either way vendored]",
    "00562500": " [tag-doubt: RpWorldSectorCallBack thunk — RpWorld API boundary, "
                "renderware-vs-RW-Physics link unit UNCERTAIN, either way vendored]",
}

# ===================== hooks.csv edit (minimal churn) =====================
hooks_path = ROOT / "hooks.csv"
hk_lines = hooks_path.read_text(encoding="utf-8").split("\n")
def parse(line): return next(csv.reader([line]))
def serialize(fields):
    sio = io.StringIO(); csv.writer(sio, lineterminator="").writerow(fields); return sio.getvalue()
idx_by_rva = {}
for i, ln in enumerate(hk_lines):
    if not ln or ln.startswith("#") or ln.startswith("rva,"): continue
    f0 = ln.split(",", 1)[0].strip().lower()
    if f0: idx_by_rva.setdefault(f0, i)

edits = {"libskip": 0, "tag_doubt": 0, "drift_not_c1": 0, "drift_not_gameplay": 0}
drift = []
for rva, sess in LIBSKIP.items():
    i = idx_by_rva.get(rva)
    if i is None: sys.exit(f"FATAL {rva} not in hooks.csv")
    fields = parse(hk_lines[i])
    if fields[3] != "C1":
        edits["drift_not_c1"] += 1; drift.append(f"{rva}:{fields[3]}")
    if fields[2] != "gameplay":
        edits["drift_not_gameplay"] += 1; drift.append(f"{rva}:subsys={fields[2]}")
    fields[2] = f"third-party-library[{TAG}]"
    while len(fields) < 9: fields.append("")
    note = (f" | reclass-OUT batch_ap {sess} {SID}: vendored {TAG} primitive "
            f"({CLUSTER[sess]}) mislabeled gameplay, kept-C1 (not hand-plated); "
            f"see project_qhull_rwphysics_island")
    if rva in TAG_DOUBT:
        note += TAG_DOUBT[rva]; edits["tag_doubt"] += 1
    fields[8] = (fields[8] or "") + note
    hk_lines[i] = serialize(fields)
    edits["libskip"] += 1

# ===================== CHANGELOG =====================
clog = (f"{DATE}  re-classify batch_ap  reclass-OUT x168 (gameplay campaign 5/5 FINAL, last-168 "
    f"0x562460..0x57ae30): ALL six sessions ALL-SKIP — entire band is the vendored "
    f"RenderWare-Physics-3.7 module (s1 world-import/hull/octree, s2 octree-tail/rigid-body-math, "
    f"s3 ray-TOI/island/contact-hash, s4 solver core SAT/GJK/LDLt/PGS, s5 solver/CCD/mass-property, "
    f"s6 narrow-phase tail). 0 plated, 0 promoted, 0 U-/S- mints (U-8900..9199/S-7000..7199 "
    f"untouched). qhull watch s6 NEGATIVE (island still 0x0057c5b0). Tag-doubt: 00562460/00562500 "
    f"(RpWorld API boundary). Evidence: ap_s1..s6 fragments + {SID}. "
    f"gameplay C1 168 -> 0 — GAMEPLAY C1->C2 CAMPAIGN CLOSED (al 792->636, am ->480, an ->324, "
    f"ao ->168, ap ->0). C2 unchanged at 4386.")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"hooks.csv edits: {edits}")
if drift: print(f"DRIFT: {drift}")
if APPLY:
    hooks_path.write_text("\n".join(hk_lines), encoding="utf-8")
    c = (ROOT / "re/analysis/CHANGELOG.md").read_text(encoding="utf-8")
    if not c.endswith("\n"): c += "\n"
    (ROOT / "re/analysis/CHANGELOG.md").write_text(c + clog + "\n", encoding="utf-8")
    print("\nWROTE hooks.csv, CHANGELOG.md")
else:
    print("\nDRY RUN — nothing written.")
