"""One-shot central re-classify for batch_aj (C1->C2 for 145 plated RVAs +
3 RW-Physics/qhull library reclass-OUT + particle->render + U-ID minting).

Edits only the target rows in hooks.csv (minimal churn), replaces inline
[UNCERTAIN] markers in the 19 plates with filed U-IDs, appends 17 new U-rows
to UNCERTAINTIES.md, and appends CHANGELOG lines. Prints an ASCII summary.

Run: py -3.12 scripts/reclassify_batch_aj.py [--apply]
Without --apply it is a dry run (prints the plan, writes nothing).
"""
from __future__ import annotations
import csv, io, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260602-1945"
DATE = "2026-06-02"
APPLY = "--apply" in sys.argv

manifest = json.loads((ROOT / "sweep_manifest.json").read_text(encoding="utf-8"))

# rva (bare, lower) -> plate path (relative, forward slash)
plate_of: dict[str, str] = {}
rva_subsystem_bucket: dict[str, str] = {}
for b in manifest["buckets"]:
    bdir = ROOT / "re" / "analysis" / b["bucket"]
    for e in b["rvas"]:
        rva = e["rva"][2:].lower()
        for cand in (bdir / f"0x{rva}.md", bdir / f"{rva}.md"):
            if cand.exists():
                plate_of[rva] = str(cand.relative_to(ROOT)).replace("\\", "/")
                break
        else:
            sys.exit(f"FATAL no plate for {rva} in {b['bucket']}")

PLATED = list(plate_of.keys())
assert len(PLATED) == 145, f"expected 145 plated, got {len(PLATED)}"

PARTICLE_TO_RENDER = {"004b4010","004b4050","004b4080","004b40c0","004b40f0",
                      "004b4120","004b4130","004b4140"}
RENAME = {"00479030": "FUN_00479030"}  # master already has FUN_ (2026-05-18 create-fn)
BOUNDARY_NOTE = {
    "00462500": "[boundary track-vs-audio: kept track, sits in audio cluster]",
    "00462510": "[boundary track-vs-audio: kept track, sits in audio cluster]",
    "0047b9b0": "[boundary physics-vs-script-glue: kept physics per hooks.csv]",
}
LIB_SKIP = {
    "0055dc70": "third-party-library[RenderWare-Physics-3.7]",
    "00562520": "third-party-library[RenderWare-Physics-3.7]",
    "0057c4b0": "third-party-library[qhull-2002.1]",
}

# 17 new-mint uncertainty markers: rva -> type
UNC_TYPE = {
  "004b7200":"structural","004b7ff0":"data","00478cb0":"structural",
  "0047f940":"structural","0047fc40":"semantic","0047fe00":"structural",
  "0047ff70":"semantic","004850e0":"structural","004852e0":"semantic",
  "004853f0":"semantic","00485460":"semantic","004854e0":"data",
  "004858c0":"data","00485a00":"structural","00485a70":"structural",
  "00485b30":"structural","00485c20":"data",
}
# 2 hud markers reference pre-existing U-IDs (function_create resolved boundary)
HUD_EXISTING = {"00554940":"U-1067", "00555910":"U-5649"}

NOTE_SUFFIX = f" | C1->C2 batch_aj {SID}"

def bullet_text(path: Path) -> str:
    lines = path.read_text(encoding="utf-8").split("\n")
    for i, l in enumerate(lines):
        if "[UNCERTAIN]" in l:
            buf = [l.strip()]
            for nxt in lines[i+1:]:
                s = nxt.strip()
                if not s or s.startswith("- ") or s.startswith("#") or s.startswith("|"):
                    break
                buf.append(s)
            txt = " ".join(buf)
            txt = re.sub(r"^-\s*\[UNCERTAIN\]\s*", "", txt)
            txt = re.sub(r"\s+", " ", txt).strip()
            return txt
    return ""

# ---- assign U-IDs to the 17 (deterministic order = address order) ----
new_unc = sorted(UNC_TYPE.keys())
uid_of = {rva: f"U-{7400+i}" for i, rva in enumerate(new_unc)}

# ===================== hooks.csv edit (minimal churn) =====================
hooks_path = ROOT / "hooks.csv"
hk_lines = hooks_path.read_text(encoding="utf-8").split("\n")

def parse(line): return next(csv.reader([line]))
def serialize(fields):
    sio = io.StringIO(); csv.writer(sio, lineterminator="").writerow(fields)
    return sio.getvalue()

edits = {"promoted":0,"render":0,"renamed":0,"libskip":0,"boundary":0}
idx_by_rva: dict[str, int] = {}
for i, ln in enumerate(hk_lines):
    if not ln or ln.startswith("#") or ln.startswith("rva,"):
        continue
    f0 = ln.split(",", 1)[0].strip().lower()
    if f0:
        idx_by_rva.setdefault(f0, i)

def edit_row(rva, fn):
    i = idx_by_rva.get(rva)
    if i is None:
        sys.exit(f"FATAL rva {rva} not in hooks.csv")
    fields = parse(hk_lines[i])
    fn(fields)
    hk_lines[i] = serialize(fields)

for rva in PLATED:
    def _mk(rva):
        def f(fields):
            # cols: 0 rva,1 name,2 subsystem,3 conf,4 status,5 file,6 scenario,7 frida,8 notes
            fields[3] = "C2"
            if fields[4] in ("new", "", "C1"):
                fields[4] = "mapped"
            fields[5] = plate_of[rva]
            if rva in PARTICLE_TO_RENDER:
                fields[2] = "render"; edits["render"] += 1
            if rva in RENAME:
                fields[1] = RENAME[rva]; edits["renamed"] += 1
            note = NOTE_SUFFIX
            if rva in BOUNDARY_NOTE:
                note += " " + BOUNDARY_NOTE[rva]; edits["boundary"] += 1
            while len(fields) < 9: fields.append("")
            fields[8] = (fields[8] or "") + note
            edits["promoted"] += 1
        return f
    edit_row(rva, _mk(rva))

for rva, lib in LIB_SKIP.items():
    def _mk(rva, lib):
        def f(fields):
            fields[2] = lib            # subsystem -> third-party-library
            # confidence stays C1 (vendored: library-tag drain, not hand-plate)
            while len(fields) < 9: fields.append("")
            fields[8] = (fields[8] or "") + (
                f" | reclass-OUT batch_aj s4 {SID}: vendored callee of FUN_0047f4c0, kept-C1 (not hand-plated); see project_qhull_rwphysics_island")
            edits["libskip"] += 1
        return f
    edit_row(rva, _mk(rva, lib))

# ===================== plate inline marker replacement =====================
plate_marker_edits = 0
for rva in new_unc:
    p = ROOT / plate_of[rva]
    t = p.read_text(encoding="utf-8")
    nt = t.replace("[UNCERTAIN]", f"[UNCERTAIN {uid_of[rva]}]", 1)
    if nt != t:
        plate_marker_edits += 1
        if APPLY: p.write_text(nt, encoding="utf-8")
for rva, uid in HUD_EXISTING.items():
    p = ROOT / plate_of[rva]
    t = p.read_text(encoding="utf-8")
    nt = t.replace("[UNCERTAIN]", f"[UNCERTAIN {uid}]", 1)
    if nt != t:
        plate_marker_edits += 1
        if APPLY: p.write_text(nt, encoding="utf-8")

# ===================== UNCERTAINTIES.md append =====================
unc_path = ROOT / "UNCERTAINTIES.md"
unc_rows = []
for rva in new_unc:
    stmt = bullet_text(ROOT / plate_of[rva])
    where = f"0x00{rva} (FUN_00{rva})" if not rva.startswith("00") else f"0x{rva} (FUN_{rva})"
    where = f"0x{rva} (FUN_{rva})"
    row = (f"| {uid_of[rva]} | {UNC_TYPE[rva]} | {where} | {stmt} | "
           f"Mechanical transcription complete; data/struct/calling-convention semantics not resolved this pass | "
           f"Cross-reference writers/callers of the cited globals/offsets; struct-extract pass at C2->C3 | none |")
    unc_rows.append(row)

# ===================== CHANGELOG append =====================
clog_path = ROOT / "re" / "analysis" / "CHANGELOG.md"
clog_line = (f"{DATE}  re-classify batch_aj  C1->C2 x145 (input 41, track 26, smplfzx 14, "
    f"physics 13, powerups 10, camera 9, particle->render 8, sky 7, world-objects 6, "
    f"debug-overlay 5, video 4, hud 2) + 3 RW-Physics/qhull reclass-OUT kept-C1 (0055dc70,00562520,0057c4b0) "
    f"+ rename LAB->FUN 00479030 + 2 master function_create (00554940,00555910 resolves U-1067/U-5649 boundary) "
    f"+ 17 U-rows U-7400..U-7416 (data/struct/semantic, non-blocking). "
    f"Boundary-flags surfaced: 00462500/00462510 (track-vs-audio), 0047b9b0 (physics-vs-script). "
    f"Evidence: 6 bucket plates + {SID}. input/track/physics/smplfzx/powerups/camera/particle/sky/"
    f"world-objects/debug-overlay/video/hud C1 -> 0.")

# ---- write ----
try:
    sys.stdout.reconfigure(encoding="utf-8")
except Exception:
    pass
print(f"APPLY={APPLY}")
print(f"hooks.csv edits: {edits}")
print(f"plate marker replacements: {plate_marker_edits} (17 new U-IDs U-7400..U-{7400+len(new_unc)-1} + 2 hud->U-1067/U-5649)")
print(f"UNCERTAINTIES.md new rows: {len(unc_rows)}")
print("U-ID assignment:")
for rva in new_unc:
    print(f"  {uid_of[rva]}  {UNC_TYPE[rva]:11s} {rva}")
if APPLY:
    hooks_path.write_text("\n".join(hk_lines), encoding="utf-8")
    u = unc_path.read_text(encoding="utf-8")
    if not u.endswith("\n"): u += "\n"
    unc_path.write_text(u + "\n".join(unc_rows) + "\n", encoding="utf-8")
    c = clog_path.read_text(encoding="utf-8")
    if not c.endswith("\n"): c += "\n"
    clog_path.write_text(c + clog_line + "\n", encoding="utf-8")
    print("\nWROTE hooks.csv, UNCERTAINTIES.md, CHANGELOG.md, 19 plates")
else:
    print("\nDRY RUN — nothing written. Re-run with --apply.")
