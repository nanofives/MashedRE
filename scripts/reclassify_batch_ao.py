"""Central re-classify for batch_ao (gameplay campaign batch 4/~5, next-156 0x47f450..0x562380).

RW/RW-Physics-heavy slice — the batch_ao header's high-library_skip prediction held:
- 27 plated RVAs C1->C2 (s1=14, s2=10, s3=3), file pointer -> bucket plate.
- 129 library_skip reclass-OUT to third-party-library[<tag>], kept C1 (NOT promoted,
  NOT hand-plated): 33 renderware + 96 RenderWare-Physics-3.7.
- Subsystem reclass among plated (worker-observed, evidence in ao_s1..s3 fragments + plates):
    * ao_s1: ALL 14 stay gameplay (world teardown / scenery-actor spawn / contact-sample ring
      + the 0x30-byte keyframe-track family deepening bucket_00489450).
    * ao_s2: ALL 10 reclass gameplay->hud (font-vector 2D text/path module 0x00554010..0x005572c0:
      FGDC20.RWF binary vector-font loader chain + PostScript-operator text loader + stroke
      renderer; 2026-05-19 plates in bucket_00554010 already said font-vector; old batch-y
      "collision-bvh-octree" hooks.csv note refuted).
    * ao_s3: ALL 3 stay gameplay (00558100/00558140 callback-swap glue on game .bss DAT_00913274;
      00558b40 segment-vs-sphere hit test — game-side utility math, zero engine-region callers).
- Boundary flags:
    * 00558b40 — origin doubt (library-shaped math but no library statics/callers; kept gameplay).
    * the 10 hud rows carry a module-level vendor doubt (possible vendored Rt2d-family 2D toolkit:
      PostScript operator strings, alloc hints 0x30190/0x301a1; no Library tag / FidDB name —
      plated per the in-doubt rule; single shared U-ID, see below).
- U-mints from U-8600 for every bare [UNCERTAIN] in the 27 plates; the s2 recurring module-level
  marker is minted ONCE and the same U-ID is reused across plates (worker: "do not multi-mint").
  Statement text extracted BEFORE replacing the marker.
- Append CHANGELOG.

Run: py -3.12 scripts/reclassify_batch_ao.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260603-2210"
DATE = "2026-06-03"
APPLY = "--apply" in sys.argv
U_START = 8600

manifest = json.loads((ROOT / "sweep_manifest.json").read_text(encoding="utf-8"))

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

assert len(ordered) == 27, f"expected 27 plated, got {len(ordered)}"

# ---- subsystem reclass among plated (worker-observed) ----
HUD_S2 = {"00554010", "00554150", "00554200", "00554390", "00555830",
          "00556780", "00556e40", "00557110", "005572b0", "005572c0"}
RECLASS = {r: "hud" for r in HUD_S2}
assert len(RECLASS) == 10

# ---- library_skip reclass-OUT (kept C1) ----
RW_S1 = ["004c0790", "004c0870", "004c0910", "004c0a60", "004c0d70", "004c0de0",
         "004c1210", "004c15c0", "004c4220", "004d8bd0", "004e43b0", "004e4440"]
RW_S2 = ["004e4800", "004e4d90", "004e6710", "004e68a0", "004e6920", "004e6d00",
         "004e6d80", "004e6f80", "004e6fe0", "004e8e90", "004e8ea0", "00534b60",
         "00534d00", "00557ec0", "00557fb0", "00558030"]
RW_S3 = ["00558180", "00558400", "00558550", "005586f0", "00558860"]
RWP_S3 = ["00558c50", "00558c80", "00558ca0", "00558cb0", "00558d30", "00559040",
          "005592f0", "00559560", "005595d0", "00559b50", "00559ba0", "00559bc0",
          "00559be0", "00559c00", "00559c20", "00559ee0", "0055a140", "0055a1f0"]
RWP_S4 = ["0055a9a0", "0055ab30", "0055abb0", "0055ad20", "0055ade0", "0055ae50",
          "0055af40", "0055af60", "0055af70", "0055af80", "0055afc0", "0055b030",
          "0055b060", "0055b080", "0055b4a0", "0055b750", "0055b800", "0055b860",
          "0055b930", "0055ba90", "0055bac0", "0055bad0", "0055bae0", "0055bb70",
          "0055bbf0", "0055bd70"]
RWP_S5 = ["0055bd80", "0055bde0", "0055c000", "0055c0f0", "0055c230", "0055c2d0",
          "0055c380", "0055c3e0", "0055c490", "0055c540", "0055cd40", "0055dca0",
          "0055dd60", "0055ddd0", "0055de20", "0055de60", "0055deb0", "0055ded0",
          "0055df90", "0055dff0", "0055e050", "0055e190", "0055e200", "0055e2d0",
          "0055e300", "0055e330"]
RWP_S6 = ["0055e440", "0055f450", "0055f480", "0055f520", "0055f670", "0055f800",
          "0055fdd0", "0055fe10", "0055fe30", "0055fe40", "0055fe50", "0055fea0",
          "0055ff70", "0055ff90", "005601f0", "00560260", "00561040", "00561280",
          "00561390", "00561c50", "00561e60", "00561e80", "00561ea0", "00561ec0",
          "00562010", "00562380"]

LIBSKIP: dict[str, tuple[str, str]] = {}   # rva -> (tag, session)
for r in RW_S1:  LIBSKIP[r] = ("renderware", "s1")
for r in RW_S2:  LIBSKIP[r] = ("renderware", "s2")
for r in RW_S3:  LIBSKIP[r] = ("renderware", "s3")
for r in RWP_S3: LIBSKIP[r] = ("RenderWare-Physics-3.7", "s3")
for r in RWP_S4: LIBSKIP[r] = ("RenderWare-Physics-3.7", "s4")
for r in RWP_S5: LIBSKIP[r] = ("RenderWare-Physics-3.7", "s5")
for r in RWP_S6: LIBSKIP[r] = ("RenderWare-Physics-3.7", "s6")
assert len(LIBSKIP) == 129, f"expected 129 library_skip, got {len(LIBSKIP)}"
assert sum(1 for t, _ in LIBSKIP.values() if t == "renderware") == 33
assert sum(1 for t, _ in LIBSKIP.values() if t == "RenderWare-Physics-3.7") == 96
assert not set(LIBSKIP) & set(ordered), "library_skip overlaps plated"
assert len(LIBSKIP) + len(ordered) == 156

BOUNDARY = {
    "00558b40": " [boundary library-vs-gameplay: segment-vs-sphere test, library-shaped math but "
                "ZERO engine-region callers and no library statics in body (callers FUN_004197e0/"
                "FUN_00459620/FUN_0045b390 all gameplay); kept gameplay per ao_s3]",
    "00558180": " [tag-doubt: RtCharset-shaped Im2D debug-text toolkit; renderware-vs-RW-Physics "
                "link unit UNCERTAIN, either way vendored]",
    "005592f0": " [tag-doubt: frustum-corner lerp on camera-shaped struct; RW-Physics-vs-renderware "
                "UNCERTAIN, either way vendored]",
}
HUD_VENDOR_NOTE = (" [module-vendor-doubt: 0x00553xxx-0x00557xxx font-vector module may be a "
                   "vendored RenderWare-family 2D toolkit (PostScript ops, alloc hints "
                   "0x30190/0x301a1); no Library tag / FidDB name -> plated per in-doubt rule; "
                   "if a calibration confirms an Rt2d-family band, reclass-OUT all 10 in one txn]")

RECLASS_NOTE = {
    "hud": f" | reclass gameplay->hud batch_ao s2 {SID}: font-vector 2D text/path module "
           f"(FGDC20.RWF binary vector-font loader 00554390 caller FUN_00427880 + PostScript-"
           f"operator text loader 00556780 + stroke renderer 005572c0 via FUN_004cd070 triplet); "
           f"old batch-y 'collision-bvh-octree' note refuted by bodies",
}
NOTE_SUFFIX = f" | C1->C2 batch_ao {SID}"


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
# Dedup: identical normalized statement text across plates -> ONE U-ID reused
# (honors ao_s2's "recurring [UNCERTAIN], do not multi-mint").
unc_rows = []
plate_marker_edits = 0
plate_new_text: dict[str, str] = {}
uid = U_START
stmt_uid: dict[str, str] = {}
uid_locs: dict[str, list[str]] = {}
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
            key = re.sub(r"0x[0-9a-f]+", "<addr>", stmt.lower())[:200]
            if key in stmt_uid:
                uidstr = stmt_uid[key]
            else:
                uidstr = f"U-{uid}"
                stmt_uid[key] = uidstr
                uid += 1
                unc_rows.append(
                    f"| {uidstr} | {classify(stmt)} | __WHERE_{uidstr}__ | {stmt} | "
                    f"Mechanical transcription complete; data/struct/vendor semantics not resolved this pass | "
                    f"Cross-reference writers/callers of the cited globals/offsets; vendor-band calibration or "
                    f"struct-extract pass at C2->C3 | none |")
            uid_locs.setdefault(uidstr, []).append(f"0x{rva}")
            lines[i] = ln.replace("[UNCERTAIN]", f"[UNCERTAIN {uidstr}]", 1)
            changed = True
            plate_marker_edits += 1
    if changed:
        plate_new_text[rva] = "\n".join(lines)

# fill in WHERE with all locations sharing the U-ID
unc_rows = [re.sub(r"__WHERE_(U-\d+)__",
                   lambda m: ", ".join(uid_locs[m.group(1)]), row) for row in unc_rows]
shared = {u: locs for u, locs in uid_locs.items() if len(locs) > 1}

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
edits = {"promoted": 0, "reclass_hud": 0, "libskip_rw": 0, "libskip_rwp": 0,
         "boundary": 0, "drift_not_c1": 0}
drift = []
def edit_row(rva, fn):
    i = idx_by_rva.get(rva)
    if i is None: sys.exit(f"FATAL {rva} not in hooks.csv")
    fields = parse(hk_lines[i]); fn(fields); hk_lines[i] = serialize(fields)

for rva in ordered:
    def _mk(rva):
        def f(fields):
            if fields[3] != "C1":
                edits["drift_not_c1"] += 1
                drift.append(f"{rva}:{fields[3]}")
            fields[3] = "C2"
            if fields[4] in ("new", "", "C1"): fields[4] = "mapped"
            fields[5] = plate_of[rva]
            while len(fields) < 9: fields.append("")
            note = NOTE_SUFFIX
            if rva in RECLASS:
                fields[2] = RECLASS[rva]
                note += RECLASS_NOTE["hud"]
                edits["reclass_hud"] += 1
                note += HUD_VENDOR_NOTE
            if rva in BOUNDARY:
                note += BOUNDARY[rva]; edits["boundary"] += 1
            fields[8] = (fields[8] or "") + note
            edits["promoted"] += 1
        return f
    edit_row(rva, _mk(rva))

for rva, (tag, sess) in LIBSKIP.items():
    def _mk(rva, tag, sess):
        def f(fields):
            if fields[3] != "C1":
                edits["drift_not_c1"] += 1
                drift.append(f"{rva}:{fields[3]} (libskip)")
            fields[2] = f"third-party-library[{tag}]"
            while len(fields) < 9: fields.append("")
            note = (f" | reclass-OUT batch_ao {sess} {SID}: vendored {tag} primitive mislabeled "
                    f"gameplay, kept-C1 (not hand-plated); see project_qhull_rwphysics_island")
            if rva in BOUNDARY:
                note += BOUNDARY[rva]; edits["boundary"] += 1
            fields[8] = (fields[8] or "") + note
            edits["libskip_rwp" if tag.startswith("RenderWare-P") else "libskip_rw"] += 1
        return f
    edit_row(rva, _mk(rva, tag, sess))

# ===================== CHANGELOG =====================
clog = (f"{DATE}  re-classify batch_ao  C1->C2 x27 + reclass-OUT x129 (gameplay campaign 4/~5, "
    f"next-156 0x47f450..0x562380, RW/RW-Physics-heavy slice): 17 stay gameplay (s1 world-teardown/"
    f"keyframe-track family + s3 callback-swap/segment-vs-sphere) + 10 gameplay->hud s2 font-vector "
    f"module 0x00554010..0x005572c0 (FGDC20.RWF loader chain; module-vendor-doubt flagged). "
    f"129 library_skip kept-C1 reclass-OUT to third-party-library: 33 renderware (RwFrame core page "
    f"0x004c0xxx + RpClump/geometry page 0x004e4xxx-0x004e8xxx + PTank 00534b60/d00 + LOD-atomic "
    f"00557ec0/fb0/00558030 + RtCharset-shaped Im2D text 00558180..00558860) + 96 "
    f"RenderWare-Physics-3.7 (farfield/broad-phase + rigid-body/scene API + solver/CCD/sleep "
    f"0x00558c50..0x00562380). Boundary: 00558b40 (kept gameplay), 00558180/005592f0 (tag-doubt). "
    f"+{len(unc_rows)} U-rows U-{U_START}..U-{U_START+len(unc_rows)-1} (non-blocking; "
    f"{len(shared)} shared-dedup). NO function_create. Evidence: 3 bucket plates dirs + ao_s1..s6 "
    f"fragments + {SID}. gameplay C1 324 -> 168; C2 4359 -> 4386.")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"hooks.csv edits: {edits}")
if drift: print(f"DRIFT (not-C1 before flip): {drift}")
print(f"U-rows minted: {len(unc_rows)}  (U-{U_START}..U-{U_START+len(unc_rows)-1})")
print(f"shared (deduped) U-IDs: { {u: len(l) for u, l in shared.items()} }")
print(f"plate marker replacements: {plate_marker_edits}")
print(f"plates modified: {len(plate_new_text)}")
if len(unc_rows) > 300:
    print(f"WARNING: U-row count {len(unc_rows)} exceeds reserved range U-8600..U-8899 (300)")
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
