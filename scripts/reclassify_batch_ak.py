"""Central re-classify for batch_ak (vehicle + ai).

- 123 plated RVAs C1->C2 (66 vehicle + 57 ai), file pointer -> bucket plate.
- 14 RW/RW-Physics library-skips (s3): subsystem -> third-party-library[...], KEEP C1.
- 004826d0 note correction (FUN_00482140 is collision-mesh strip builder, not gravity flag).
- Mint U-IDs from U-7500 for every bare [UNCERTAIN] marker in the 123 plates;
  replace inline marker with [UNCERTAIN U-NNNN]. (Extract statement text BEFORE
  replacing the marker — the bug that bit batch_aj.)
- Append CHANGELOG.

Run: py -3.12 scripts/reclassify_batch_ak.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260603-0116"
DATE = "2026-06-02"
APPLY = "--apply" in sys.argv
U_START = 7500

manifest = json.loads((ROOT / "sweep_manifest.json").read_text(encoding="utf-8"))

# rva -> (plate path rel, subsystem) ; ordered by bucket then address for deterministic U-IDs
plate_of: dict[str, str] = {}
sub_of: dict[str, str] = {}
ordered: list[str] = []
for b in manifest["buckets"]:
    bdir = ROOT / "re" / "analysis" / b["bucket"]
    sub = "vehicle" if "vehicle" in b["bucket"] else "ai"
    for e in sorted(b["rvas"], key=lambda e: e["rva"]):
        rva = e["rva"][2:].lower()
        for cand in (bdir / f"0x{rva}.md", bdir / f"{rva}.md"):
            if cand.exists():
                plate_of[rva] = str(cand.relative_to(ROOT)).replace("\\", "/")
                break
        else:
            sys.exit(f"FATAL no plate for {rva}")
        sub_of[rva] = sub
        ordered.append(rva)

assert len(ordered) == 123, f"expected 123, got {len(ordered)}"

LIB_SKIP = {
    "004c0b30": "third-party-library[renderware]", "004c1040": "third-party-library[renderware]",
    "004c39b0": "third-party-library[renderware]", "004e69a0": "third-party-library[renderware]",
    "004e6ab0": "third-party-library[renderware]", "004e7e30": "third-party-library[renderware]",
    "00559c40": "third-party-library[RenderWare-Physics-3.7]", "0055ae70": "third-party-library[RenderWare-Physics-3.7]",
    "0055b940": "third-party-library[RenderWare-Physics-3.7]", "0055bab0": "third-party-library[RenderWare-Physics-3.7]",
    "0055c4a0": "third-party-library[RenderWare-Physics-3.7]", "0057c220": "third-party-library[RenderWare-Physics-3.7]",
    "0057c300": "third-party-library[RenderWare-Physics-3.7]", "0057c500": "third-party-library[RenderWare-Physics-3.7]",
}
CORRECTION = {
    "004826d0": " CORRECTION: FUN_00482140 is the collision-mesh strip builder returning RpAtomic, NOT a gravity-flag setter",
}
NOTE_SUFFIX = f" | C1->C2 batch_ak {SID}"

def classify(text: str) -> str:
    t = text.lower()
    if any(k in t for k in ("calling convention", "unaff_", "in_eax", "__fastcall", "register",
                            " edi", " esi", " eax", "ecx", "edx")):
        return "calling-convention"
    if any(k in t for k in ("offset", "field", "struct", "layout", "+0x", "record", "stride", "vtable", "+0x")):
        return "structural"
    if any(k in t for k in ("enum", "flag", "tag", "value", "meaning", "constant", " id ", "mask", "bit")):
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
            # extract statement: this line + continuation until blank / next bullet / heading / table
            buf = [ln.strip()]
            for nxt in lines[i+1:]:
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
edits = {"promoted": 0, "libskip": 0, "corrected": 0}
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
            note = NOTE_SUFFIX + CORRECTION.get(rva, "")
            if rva in CORRECTION: edits["corrected"] += 1
            fields[8] = (fields[8] or "") + note
            edits["promoted"] += 1
        return f
    edit_row(rva, _mk(rva))
for rva, lib in LIB_SKIP.items():
    def _mk(rva, lib):
        def f(fields):
            fields[2] = lib  # keep C1
            while len(fields) < 9: fields.append("")
            fields[8] = (fields[8] or "") + (
                f" | reclass-OUT batch_ak s3 {SID}: vendored RW/RW-Physics primitive mislabeled vehicle, kept-C1 (not hand-plated)")
            edits["libskip"] += 1
        return f
    edit_row(rva, _mk(rva, lib))

# ===================== CHANGELOG =====================
clog = (f"{DATE}  re-classify batch_ak  C1->C2 x123 (vehicle 66, ai 57) + 14 RW/RW-Physics reclass-OUT kept-C1 "
    f"(6 renderware 004c0b30/004c1040/004c39b0/004e69a0/004e6ab0/004e7e30 + 8 RenderWare-Physics-3.7 "
    f"00559c40/0055ae70/0055b940/0055bab0/0055c4a0/0057c220/0057c300/0057c500) + {len(unc_rows)} U-rows "
    f"U-{U_START}..U-{U_START+len(unc_rows)-1} (non-blocking) + 004826d0 note correction. Evidence: 6 bucket "
    f"plates + {SID}. vehicle + ai C1 -> 0 (gameplay 792 + render 458 remain).")

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
