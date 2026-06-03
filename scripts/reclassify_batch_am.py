"""Central re-classify for batch_am (gameplay campaign batch 2/~5, next-156 0x41e140..0x455fe0).

- 156 plated RVAs C1->C2 (all were gameplay/C1 at session start), file pointer -> bucket plate.
- Subsystem reclass (worker-observed, evidence in the am_s1..s6 fragments + plates):
    * am_s1: 2 render (0041e4b0/0041ecc0 pure RpMaterial.color writes). 24 stay gameplay
      (incl. the 0041e8d0..0041ea70 singleton accessor family flagged frontend but KEPT
      gameplay pending object identity).
    * am_s2: ALL 26 stay gameplay (per-vehicle visual ORCHESTRATION glue; callers gameplay/
      util/boot, not render/particle).
    * am_s3: 25 render (debris-clump + smoke-particle effects; caller FUN_004219c0 already
      render). 004223f0 stays gameplay (xorshift PRNG-float leaf; worker flagged util-math,
      kept gameplay — util-math leaves not split this campaign).
    * am_s4: 20 render (RW effect-entity subsystem) + 5 audio (0044dfe0..0044e070 in-world-
      item accessors; sole caller FUN_00461650 = C2 audio dispatcher, pitch param FUN_005a6dc0).
      00422440 stays gameplay (cosine-ease lerp leaf; worker flagged util, kept gameplay).
    * am_s5: ALL 26 stay gameplay (projectile/depth-charge-effect + portal-trigger + scenery
      sub-object cluster; worker confirmed no reclass).
    * am_s6: 13 render (visual-frame / scene-graph mgmt) + 1 hud (00455b50 lock-on billboard
      reticle). 12 stay gameplay (weapon spawn/flight/AI/detonation entity logic).
  Total reclass = 60 render + 5 audio + 1 hud = 66; 90 stay gameplay. ALL 156 go C1->C2, so
  gameplay C1 drops by the full 156 regardless of label.
- Boundary flags (kept the indicated subsystem, annotated): 00422160 (render w/ gameplay
  event-0x21 branch), 0044df80 (render-visibility on shared in-world-item array, gameplay
  caller), 004223f0 + 00422440 (kept gameplay, worker flagged util-math).
- NO library_skip (every RVA is genuine game code), NO function_create (all have function objects).
- Mint U-IDs from U-8000 for every bare [UNCERTAIN] marker in the 156 plates; replace inline
  marker with [UNCERTAIN U-NNNN]. (Extract statement text BEFORE replacing the marker.)
- Append CHANGELOG.

Run: py -3.12 scripts/reclassify_batch_am.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260603-0427"
DATE = "2026-06-03"
APPLY = "--apply" in sys.argv
U_START = 8000

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
RENDER_S1 = {"0041e4b0", "0041ecc0"}
RENDER_S3 = {"00421100", "00421160", "004212b0", "00421560", "004215e0", "00421630",
             "00421690", "004216b0", "00421720", "00421750", "00421800", "004218b0",
             "004218d0", "00421930", "00421980", "00421a90", "00421ba0", "00421c10",
             "00421c50", "00421c80", "004220d0", "00422140", "00422160", "00422290",
             "004223b0"}
RENDER_S4 = {"00449ba0", "0044aaa0", "0044ab20", "0044b000", "0044bbc0", "0044bc30",
             "0044bd50", "0044be70", "0044bfa0", "0044c370", "0044c490", "0044c4f0",
             "0044c5f0", "0044c740", "0044c8b0", "0044c920", "0044caa0", "0044cb00",
             "0044d6e0", "0044df80"}
RENDER_S6 = {"00454130", "00454cd0", "00454bd0", "00454c00", "00454c10", "00454c60",
             "00454f80", "00454fa0", "00454ff0", "00455030", "00455060", "00455100",
             "00455fe0"}
AUDIO_S4 = {"0044dfe0", "0044dff0", "0044e020", "0044e050", "0044e070"}
HUD_S6 = {"00455b50"}

RECLASS = {}
for r in RENDER_S1 | RENDER_S3 | RENDER_S4 | RENDER_S6: RECLASS[r] = "render"
for r in AUDIO_S4: RECLASS[r] = "audio"
for r in HUD_S6:   RECLASS[r] = "hud"
assert len(RECLASS) == 66, f"expected 66 reclass, got {len(RECLASS)}"
assert sum(1 for v in RECLASS.values() if v == "render") == 60
assert sum(1 for v in RECLASS.values() if v == "audio") == 5
assert sum(1 for v in RECLASS.values() if v == "hud") == 1

BOUNDARY = {
    "00422160": " [boundary render-vs-gameplay: kept render per am_s3; render-bookkeeping dominant "
                "(visibility-bit clear + material-color zero FUN_004c1340) but inactive-lane branch "
                "submits gameplay event 0x21 via FUN_00465c10]",
    "0044df80": " [boundary render-vs-gameplay: shared &DAT_00890080 80-item array; kept render — body "
                "calls RW-visibility FUN_00557fb0/00558100/00558140; caller FUN_00410510 is gameplay race-state]",
    "004223f0": " [boundary gameplay-vs-util: xorshift+quadratic-hash PRNG-float leaf (_DAT_005cd314=2^-31); "
                "worker flagged util(math), kept gameplay (util-math leaves not split this campaign)]",
    "00422440": " [boundary gameplay-vs-util: cosine-ease lerp math leaf; worker flagged util(math), kept gameplay]",
}
RECLASS_NOTE = {
    "render": f" | reclass gameplay->render batch_am {SID}: RW clump/atomic/material effect entity "
              f"(debris-clump/smoke-particle/effect-entity/scene-graph visual mgmt); callers already-C2 render/boot/util",
    "audio":  f" | reclass gameplay->audio batch_am s4 {SID}: in-world-item &DAT_00890080 accessor; sole caller "
              f"FUN_00461650 (C2 proximity-SFX dispatcher), pitch param via FUN_005a6dc0",
    "hud":    f" | reclass gameplay->hud batch_am s6 {SID}: homing-missile lock-on billboard reticle (FUN_00459620 draw)",
}
NOTE_SUFFIX = f" | C1->C2 batch_am {SID}"


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
edits = {"promoted": 0, "reclass_render": 0, "reclass_audio": 0, "reclass_hud": 0, "boundary": 0}
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
                edits["reclass_" + newsub] += 1
            if rva in BOUNDARY:
                note += BOUNDARY[rva]; edits["boundary"] += 1
            fields[8] = (fields[8] or "") + note
            edits["promoted"] += 1
        return f
    edit_row(rva, _mk(rva))

# ===================== CHANGELOG =====================
clog = (f"{DATE}  re-classify batch_am  C1->C2 x156 (gameplay campaign 2/~5, next-156 "
    f"0x41e140..0x455fe0): 90 stay gameplay + 66 subsystem-reclass (60 gameplay->render = "
    f"2 s1 material-color + 25 s3 debris/smoke effects + 20 s4 effect-entity + 13 s6 scene-graph; "
    f"5 gameplay->audio s4 in-world-item accessors FUN_00461650; 1 gameplay->hud s6 lock-on reticle). "
    f"00422160/0044df80/004223f0/00422440 boundary-flagged. NO library_skip, NO function_create "
    f"(all 156 genuine game fns w/ function objects). +{len(unc_rows)} U-rows U-{U_START}..U-{U_START+len(unc_rows)-1} "
    f"(non-blocking). Evidence: 6 bucket plates + {SID}. gameplay C1 636 -> 480.")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"hooks.csv edits: {edits}")
print(f"U-rows minted: {len(unc_rows)}  (U-{U_START}..U-{U_START+len(unc_rows)-1})")
print(f"plate marker replacements: {plate_marker_edits}")
print(f"plates modified: {len(plate_new_text)}")
if len(unc_rows) > 300:
    print(f"WARNING: U-row count {len(unc_rows)} exceeds reserved range U-8000..U-8299 (300)")
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
