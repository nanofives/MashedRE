"""Central re-classify for batch_al (gameplay campaign batch 1/~5, lowest-156).

- 156 plated RVAs C1->C2 (all were gameplay/C1 at session start), file pointer -> bucket plate.
- Subsystem reclass (worker-observed, evidence in the al_s4/al_s6 fragments + plates):
    * al_s4: 6 hud (VehicleIcons sprite-batch) + 13 render (wheel-trail-decal / drift-mark / particle).
    * al_s6: 26 render (4 ParticleEmitter classes + ghost-vehicle visual tint).
    * The other 111 stay gameplay. ALL 156 still go C1->C2, so gameplay C1 drops by 156 regardless.
- 004173e0: kept render but boundary-flagged (worker: ambiguous render-decal vs gameplay-collision grid).
- NO library_skip (every RVA is genuine game code), NO function_create (all have function objects).
- Mint U-IDs from U-7700 for every bare [UNCERTAIN] marker in the 156 plates; replace inline
  marker with [UNCERTAIN U-NNNN]. (Extract statement text BEFORE replacing the marker.)
- Append CHANGELOG.

Run: py -3.12 scripts/reclassify_batch_al.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260603-0334"
DATE = "2026-06-03"
APPLY = "--apply" in sys.argv
U_START = 7700

manifest = json.loads((ROOT / "sweep_manifest.json").read_text(encoding="utf-8"))

# rva -> plate path (rel) ; ordered by bucket then address for deterministic U-IDs
plate_of: dict[str, str] = {}
ordered: list[str] = []
for b in manifest["buckets"]:
    bdir = ROOT / "re" / "analysis" / b["bucket"]
    for e in sorted(b["rvas"], key=lambda e: e["rva"]):
        rva = e["rva"][2:].lower()
        for cand in (bdir / f"0x{rva}.md", bdir / f"{rva}.md"):
            if cand.exists():
                plate_of[rva] = str(cand.relative_to(ROOT)).replace("\\", "/")
                break
        else:
            sys.exit(f"FATAL no plate for {rva}")
        ordered.append(rva)

assert len(ordered) == 156, f"expected 156, got {len(ordered)}"

# ---- subsystem reclass (worker-observed) ----
HUD = {"004128f0", "00413b80", "00413bb0", "00413bc0", "00413cb0", "00413f50"}
RENDER_S4 = {"00412100", "00412190", "00412620", "00412880", "00412e30", "004173e0",
             "00418a00", "00418a70", "00418aa0", "00418bd0", "00418db0", "00418e30", "00418e50"}
RENDER_S6 = {"0041a980", "0041a9d0", "0041aac0", "0041ac60", "0041ad60", "0041adb0",
             "0041ae20", "0041ae60", "0041af00", "0041af50", "0041b440", "0041b520",
             "0041b690", "0041b720", "0041b770", "0041b7a0", "0041beb0", "0041c320",
             "0041c380", "0041c410", "0041cb00", "0041cd20", "0041cdb0", "0041ce00",
             "0041d6d0", "0041d910"}
RECLASS = {}
for r in HUD:       RECLASS[r] = "hud"
for r in RENDER_S4: RECLASS[r] = "render"
for r in RENDER_S6: RECLASS[r] = "render"
assert len(RECLASS) == 45, f"expected 45 reclass, got {len(RECLASS)}"

BOUNDARY = {
    "004173e0": " [boundary render-decal vs gameplay-collision: kept render per al_s4, two-level world-grid byte lookup &DAT_007f1a9c->&DAT_007f9a9c]",
}
RECLASS_NOTE = {
    "hud":    f" | reclass gameplay->hud batch_al s4 {SID}: VehicleIcons sprite-batch &DAT_0063bd50 (icon vertex buf &DAT_00828320)",
    "render": f" | reclass gameplay->render batch_al {SID}: RW clump/atomic/material effect emitter (trail-decal/drift-mark/ParticleEmitter); callers already-C2 hud+boot",
}
NOTE_SUFFIX = f" | C1->C2 batch_al {SID}"


def classify(text: str) -> str:
    t = text.lower()
    if any(k in t for k in ("calling convention", "unaff_", "in_eax", "in_ecx", "__fastcall",
                            "register", " edi", " esi", " eax", " ecx", " edx")):
        return "calling-convention"
    if any(k in t for k in ("offset", "field", "struct", "layout", "+0x", "record", "stride",
                            "vtable", "atomic", "clump")):
        return "structural"
    if any(k in t for k in ("enum", "flag", "tag", "value", "meaning", "constant", " id ",
                            "mask", "bit", "code", "sentinel")):
        return "data"
    return "structural"


# ---- walk plates, extract markers (statement BEFORE replacing), assign U-IDs ----
unc_rows = []
plate_marker_edits = 0
plate_new_text: dict[str, str] = {}
uid = U_START
for rva in ordered:
    p = ROOT / plate_of[rva]
    lines = p.read_text(encoding="utf-8").split("\n")
    changed = False
    for i, ln in enumerate(lines):
        if "[UNCERTAIN]" in ln and "[UNCERTAIN U-" not in ln:
            buf = [ln.strip()]
            for nxt in lines[i + 1:]:
                s = nxt.strip()
                if not s or s.startswith("- ") or s.startswith("#") or s.startswith("|") or s.startswith("## "):
                    break
                buf.append(s)
            stmt = " ".join(buf)
            stmt = re.sub(r"^[-*]?\s*\[UNCERTAIN\]\s*", "", stmt)
            stmt = re.sub(r"\s+", " ", stmt).strip().replace("|", r"\|")
            uidstr = f"U-{uid}"
            where = f"0x{rva} (FUN_{rva})"
            unc_rows.append(
                f"| {uidstr} | {classify(stmt)} | {where} | {stmt} | "
                f"Mechanical transcription complete; data/struct/calling-convention semantics not resolved this pass | "
                f"Cross-reference writers/callers of the cited globals/offsets; struct-extract pass at C2->C3 | none |")
            lines[i] = ln.replace("[UNCERTAIN]", f"[UNCERTAIN {uidstr}]", 1)
            uid += 1
            changed = True
            plate_marker_edits += 1
    if changed:
        plate_new_text[rva] = "\n".join(lines)

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
edits = {"promoted": 0, "reclass_hud": 0, "reclass_render": 0, "boundary": 0}
def edit_row(rva, fn):
    i = idx_by_rva.get(rva)
    if i is None: sys.exit(f"FATAL {rva} not in hooks.csv")
    fields = parse(hk_lines[i]); fn(fields); hk_lines[i] = serialize(fields)

for rva in ordered:
    def _mk(rva):
        def f(fields):
            fields[3] = "C2"
            if fields[4] in ("new", "", "C1"): fields[4] = "mapped"
            fields[5] = plate_of[rva]
            while len(fields) < 9: fields.append("")
            note = NOTE_SUFFIX
            if rva in RECLASS:
                newsub = RECLASS[rva]
                fields[2] = newsub
                note += RECLASS_NOTE[newsub]
                edits["reclass_hud" if newsub == "hud" else "reclass_render"] += 1
            if rva in BOUNDARY:
                note += BOUNDARY[rva]; edits["boundary"] += 1
            fields[8] = (fields[8] or "") + note
            edits["promoted"] += 1
        return f
    edit_row(rva, _mk(rva))

# ===================== CHANGELOG =====================
clog = (f"{DATE}  re-classify batch_al  C1->C2 x156 (gameplay campaign 1/~5, lowest-156 "
    f"0x405400..0x41d910): 111 stay gameplay + 45 subsystem-reclass (6 gameplay->hud s4 "
    f"VehicleIcons sprite-batch; 39 gameplay->render = 13 s4 trail-decal/drift-mark/particle + "
    f"26 s6 ParticleEmitter classes + ghost tint). 004173e0 render boundary-flagged. "
    f"NO library_skip, NO function_create (all 156 genuine game fns w/ function objects). "
    f"+{len(unc_rows)} U-rows U-{U_START}..U-{U_START+len(unc_rows)-1} (non-blocking). "
    f"Evidence: 6 bucket plates + {SID}. gameplay C1 792 -> 636 (render 458 + remaining gameplay 636).")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"hooks.csv edits: {edits}")
print(f"U-rows minted: {len(unc_rows)}  (U-{U_START}..U-{U_START+len(unc_rows)-1})")
print(f"plate marker replacements: {plate_marker_edits}")
print(f"plates modified: {len(plate_new_text)}")
if APPLY:
    for rva, txt in plate_new_text.items():
        (ROOT / plate_of[rva]).write_text(txt, encoding="utf-8")
    hooks_path.write_text("\n".join(hk_lines), encoding="utf-8")
    u = (ROOT / "UNCERTAINTIES.md").read_text(encoding="utf-8")
    if not u.endswith("\n"): u += "\n"
    (ROOT / "UNCERTAINTIES.md").write_text(u + "\n".join(unc_rows) + "\n", encoding="utf-8")
    c = (ROOT / "re/analysis/CHANGELOG.md").read_text(encoding="utf-8")
    if not c.endswith("\n"): c += "\n"
    (ROOT / "re/analysis/CHANGELOG.md").write_text(c + clog + "\n", encoding="utf-8")
    print("\nWROTE hooks.csv, UNCERTAINTIES.md, CHANGELOG.md, plates")
else:
    print("\nDRY RUN — nothing written.")
