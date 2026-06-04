"""Central re-classify for batch_aq (render campaign batch 1/~2, first-156 0x401690..0x4e7fa0).

The batch_aq header's RW-core prediction held almost completely:
- 3 plated RVAs C1->C2 (all aq_s1, all stay render — the three application-page RVAs the
  header flagged as most-likely-genuine): 00401690 mesh-AABB writer, 004492b0 SkyDomeRender
  (master function_create FUN_004492b0 done during sweep-20260603-2220, resolves the
  S-1760 no-fn-object hold), 00490500 per-frame effect-ring update.
- 153 library_skip reclass-OUT to third-party-library[renderware], kept C1 (NOT promoted,
  NOT hand-plated): rwID module open/close pairs, V3d-matrix family, dynamic-VB suballocator,
  rwd3d9 raster/image conversion, RwHeap, CSL pipelines, plugin registry/stream codec,
  RpLight/RpWorld/RpAtomic/RpClump core modules.
- Flags:
    * 004dd140 — Ghidra-named Direct3DCreate9 IAT thunk; tagged renderware as driver-band,
      a d3d9-import tag may be preferable (aq_s2).
    * 004e4450/004e45b0 — existing RwFrameAddChild/RwFrameRemoveChild names may belong to
      RpWorldAddClump/RpWorldRemoveClump (aq_s4 evidence: calls RpClumpForAllAtomics/
      ForAllLights on the child; DAT_007d7174 is RpClumpRegisterPlugin's offset). Naming
      question only; master rename deferred.
- U-mints from U-8900 for every bare [UNCERTAIN] in the 3 plates (dedup identical
  normalized statements to one shared U-ID; statement extracted BEFORE replacing marker).
- Pool poisonings recorded this batch: pool3 (aq_s2), pool10 (aq_s4), pool12 (aq_s5) —
  REAL LockExceptions at the FLAT location. pool11/13/14 worked as fallbacks.

Run: py -3.12 scripts/reclassify_batch_aq.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260603-2220"
DATE = "2026-06-03"
APPLY = "--apply" in sys.argv
U_START = 8900

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

assert len(ordered) == 3, f"expected 3 plated, got {len(ordered)}"
assert set(ordered) == {"00401690", "004492b0", "00490500"}

# ---- library_skip reclass-OUT (kept C1), tag=renderware throughout ----
S1 = ["004c1940", "004c1980", "004c3e20", "004c3e90", "004c5e00", "004c5f60",
      "004c78a0", "004c78e0", "004cbca0", "004cbd00", "004cd810", "004cd850",
      "004cd900", "004cdb60", "004d9000", "004d9030", "004d9040", "004dc300",
      "004dc370", "004dc410", "004dc4a0", "004dc530", "004dc5b0"]
S2 = ["004dc750", "004dc8a0", "004dc8e0", "004dc970", "004dc9e0", "004dcb80",
      "004dcbb0", "004dcd30", "004dcd50", "004dd0b0", "004dd140", "004dd150",
      "004ddcb0", "004ddd60", "004ddfb0", "004de1b0", "004de390", "004de800",
      "004dee20", "004dee40", "004df120", "004df750", "004df870", "004df8a0",
      "004e08b0", "004e0920"]
S3 = ["004e1290", "004e12a0", "004e12b0", "004e13f0", "004e14a0", "004e15f0",
      "004e1670", "004e1710", "004e1780", "004e17e0", "004e18f0", "004e1960",
      "004e1990", "004e19f0", "004e1a90", "004e1ac0", "004e1b00", "004e1b30",
      "004e1b60", "004e1c90", "004e1ce0", "004e1d20", "004e1e70", "004e2030",
      "004e2090", "004e2420"]
S4 = ["004e2470", "004e2500", "004e2530", "004e2650", "004e2930", "004e2b60",
      "004e2bf0", "004e2fa0", "004e2ff0", "004e3160", "004e33c0", "004e3410",
      "004e3470", "004e4130", "004e41b0", "004e42a0", "004e42d0", "004e4300",
      "004e4380", "004e4450", "004e45b0", "004e47b0", "004e4810", "004e4860",
      "004e48e0", "004e4900"]
S5 = ["004e4b90", "004e4bd0", "004e4c00", "004e4c30", "004e4dd0", "004e4ec0",
      "004e4f70", "004e5020", "004e5190", "004e51e0", "004e5280", "004e5300",
      "004e55d0", "004e5660", "004e5680", "004e5700", "004e5820", "004e5840",
      "004e5850", "004e5bf0", "004e5c30", "004e5c70", "004e5cd0", "004e5d00",
      "004e5d30", "004e5ef0"]
S6 = ["004e5f30", "004e5f90", "004e5fc0", "004e6100", "004e62f0", "004e6470",
      "004e65c0", "004e6650", "004e6760", "004e67b0", "004e6880", "004e6e00",
      "004e6fb0", "004e7010", "004e7060", "004e7420", "004e7cc0", "004e7d40",
      "004e7d70", "004e7da0", "004e7dd0", "004e7df0", "004e7e10", "004e7e50",
      "004e7e90", "004e7fa0"]

CLUSTER = {
    "s1": "rwID module open/close pairs + V3d-matrix transform family + dynamic-VB suballocator",
    "s2": "rwd3d9 driver raster/image-conversion + dynamic-VB/IB module + CPU detect",
    "s3": "RwHeap allocator + CSL/D3D9 instance-pipeline ctor + plugin-registry stream codec + pipeline connect/validate",
    "s4": "pipeline-cluster/registry + arena allocator + plugin registration + RwFrame/RpWorld add-remove",
    "s5": "RpLight module + RwFrame destroy pair + RpWorld core module",
    "s6": "RpAtomic/RpClump lifecycle/stream/plugin tail (6 Ghidra-named Rp* fns)",
}

LIBSKIP: dict[str, str] = {}
for sess, lst in (("s1", S1), ("s2", S2), ("s3", S3), ("s4", S4), ("s5", S5), ("s6", S6)):
    for r in lst:
        assert r not in LIBSKIP, f"duplicate {r}"
        LIBSKIP[r] = sess
assert len(LIBSKIP) == 153, f"expected 153 library_skip, got {len(LIBSKIP)}"
assert not set(LIBSKIP) & set(ordered), "library_skip overlaps plated"
assert len(LIBSKIP) + len(ordered) == 156

FLAGS = {
    "004dd140": " [tag-doubt aq_s2: Ghidra-named Direct3DCreate9 IAT thunk to d3d9.dll; "
                "tagged renderware as driver-band, a d3d9-import tag may be preferable]",
    "004e4450": " [naming-doubt aq_s4: existing RwFrameAddChild name may belong to "
                "RpWorldAddClump (calls RpClumpForAllAtomics/ForAllLights on child; "
                "DAT_007d7174 = RpClumpRegisterPlugin offset); master rename deferred]",
    "004e45b0": " [naming-doubt aq_s4: existing RwFrameRemoveChild name may belong to "
                "RpWorldRemoveClump (same evidence as 004e4450); master rename deferred]",
}
PLATED_NOTE = {
    "004492b0": " | SkyDomeRender; master function_create FUN_004492b0 (body 0x004492b0..0x0044942a) "
                "done in sweep " + SID + ", resolves S-1760 no-fn-object hold; dispatched only via "
                "fn-ptr slot 0x005f34ac (U-1768 stands)",
}
NOTE_SUFFIX = f" | C1->C2 batch_aq {SID}"


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
                    f"Mechanical transcription complete; data/struct semantics not resolved this pass | "
                    f"Cross-reference writers/callers of the cited globals/offsets; struct-extract pass "
                    f"at C2->C3 | none |")
            uid_locs.setdefault(uidstr, []).append(f"0x{rva}")
            lines[i] = ln.replace("[UNCERTAIN]", f"[UNCERTAIN {uidstr}]", 1)
            changed = True
            plate_marker_edits += 1
    if changed:
        plate_new_text[rva] = "\n".join(lines)

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
edits = {"promoted": 0, "libskip": 0, "flags": 0, "drift_not_c1": 0}
drift = []
def edit_row(rva, fn):
    i = idx_by_rva.get(rva)
    if i is None: sys.exit(f"FATAL {rva} not in hooks.csv")
    fields = parse(hk_lines[i]); fn(fields); hk_lines[i] = serialize(fields)

for rva in ordered:
    def _mk(rva):
        def f(fields):
            if fields[3] != "C1":
                edits["drift_not_c1"] += 1; drift.append(f"{rva}:{fields[3]}")
            fields[3] = "C2"
            if fields[4] in ("new", "", "C1", "stub"): fields[4] = "mapped"
            fields[5] = plate_of[rva]
            while len(fields) < 9: fields.append("")
            note = NOTE_SUFFIX + PLATED_NOTE.get(rva, "")
            fields[8] = (fields[8] or "") + note
            edits["promoted"] += 1
        return f
    edit_row(rva, _mk(rva))

for rva, sess in LIBSKIP.items():
    def _mk(rva, sess):
        def f(fields):
            if fields[3] != "C1":
                edits["drift_not_c1"] += 1; drift.append(f"{rva}:{fields[3]} (libskip)")
            fields[2] = "third-party-library[renderware]"
            while len(fields) < 9: fields.append("")
            note = (f" | reclass-OUT batch_aq {sess} {SID}: vendored renderware primitive "
                    f"({CLUSTER[sess]}) mislabeled render, kept-C1 (not hand-plated)")
            if rva in FLAGS:
                note += FLAGS[rva]; edits["flags"] += 1
            fields[8] = (fields[8] or "") + note
            edits["libskip"] += 1
        return f
    edit_row(rva, _mk(rva, sess))

# ===================== CHANGELOG =====================
clog = (f"{DATE}  re-classify batch_aq  C1->C2 x3 + reclass-OUT x153 (render campaign 1/~2, "
    f"first-156 0x401690..0x4e7fa0, RW-core band): 3 stay render (00401690 mesh-AABB writer, "
    f"004492b0 SkyDomeRender w/ master function_create resolving S-1760, 00490500 effect-ring "
    f"update — the header's three application-page watch RVAs, all genuine). 153 library_skip "
    f"kept-C1 reclass-OUT to third-party-library[renderware]: rwID module open/close + V3d-matrix "
    f"+ dynamic-VB + rwd3d9 raster/image + RwHeap + CSL pipelines + plugin registry/stream + "
    f"RpLight/RpWorld/RpAtomic/RpClump cores. Flags: 004dd140 Direct3DCreate9 IAT thunk tag-doubt; "
    f"004e4450/004e45b0 RwFrame*-vs-RpWorld*Clump naming-doubt. +{len(unc_rows)} U-rows "
    f"U-{U_START}..U-{U_START+len(unc_rows)-1}. Pools 3/10/12 newly POISONED (real LockExceptions); "
    f"11/13/14 clean fallbacks. Evidence: 1 bucket plate dir + aq_s1..s6 fragments + {SID}. "
    f"render C1 458 -> 302; C2 4386 -> 4389.")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"hooks.csv edits: {edits}")
if drift: print(f"DRIFT (not-C1 before flip): {drift}")
print(f"U-rows minted: {len(unc_rows)}  (U-{U_START}..U-{U_START+len(unc_rows)-1})")
print(f"shared (deduped) U-IDs: { {u: len(l) for u, l in shared.items()} }")
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
