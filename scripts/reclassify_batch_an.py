"""Central re-classify for batch_an (gameplay campaign batch 3/~5, next-156 0x456040..0x47f380).

- 156 plated RVAs C1->C2 (all were gameplay/C1 at session start), file pointer -> bucket plate.
- Subsystem reclass (worker-observed, evidence in the an_s1..s6 fragments + plates):
    * an_s1: ALL 26 stay gameplay (weapon/effect entity-pool family G/H/I/J: spawn/lifetime/
      raycast/state-machine/teardown; draw-side helpers are render-ADJACENT but bucket logic is
      gameplay).
    * an_s2: ALL 26 stay gameplay (pickup-crate pool I + homing-projectile pool J + hit-spark
      grid; game-side per-frame tick/glue that DRIVES render/audio primitives, per am_s2 precedent).
    * an_s3: 3 audio (0045da90/daf0/dce0 — pure RWS-audio emitter param/position sync via
      FUN_005a89d0 / FUN_005a6d60; no game-state mutation). 23 stay gameplay (incl. 0045c030
      borderline render/util kept gameplay — sole caller effect-spawner FUN_00487280).
    * an_s4: 5 audio (00462760/004656e0/00465f40/00466000/00466100 — per-entity 3D positional-
      sound slot mgmt; callee band 0x005a6xxx/0x005a8xxx RWS audio) + 3 render (004670a0/004671d0/
      004672d0 — alt-camera/render-object triple, caller FUN_00467110 already render). 18 stay
      gameplay (per-vehicle physics record accessors + collision/trigger).
    * an_s5: 6 render (00472b10/00473220/004733b0/004744a0 RwIm2D quads + 004729b0/00472ad0 RW
      3D-line draw; callers frontend-render FUN_0042c010/0042c8d30 + logo FUN_00473ee0). 20 stay
      gameplay (accumulation table + event-impulse + math-leaves + script-bytecode handlers).
    * an_s6: 4 camera (0047c1f0/c230/c270/c2d0 — camera-path node-array containment, managed by
      FUN_0047c160 already camera-C2) + 2 hud (0047d640/0047def0 — on-screen event-marker overlay
      builders pushing into 256-entry display ring DAT_007e9de0, caller FUN_0047e9c0) + 1 render
      (0047cdc0 squared-distance LOD setter, 3 callers incl render-C2 FUN_00412050; weakly held).
      19 stay gameplay (convex-hull collision narrow-phase + scenery-actor pool mgmt/destructors).
  Total reclass = 10 render + 8 audio + 4 camera + 2 hud = 24; 132 stay gameplay. ALL 156 go
  C1->C2, so gameplay C1 drops by the full 156 regardless of label.
- Boundary flags (kept gameplay, annotated): 0045c030 (render/util look-at matrix builder),
  0045a530 (audio-leaning event wrapper), 00461e90 (color-constant surface classifier, uncertain),
  0046bfc0 (vehicle collision/impulse solver, 0 direct callers = indirect/vtable), 00471780
  (stale 'camera-anim' master comment — body is a rigid-body impulse applier; reconcile, do NOT
  carry the camera-anim label forward). 0047cdc0 reclassed render but weakly held (1-line helper).
- NO library_skip (every RVA is genuine game code), NO function_create (all have function objects).
- Mint U-IDs from U-8300 for every bare [UNCERTAIN] marker in the 156 plates; replace inline
  marker with [UNCERTAIN U-NNNN]. (Extract statement text BEFORE replacing the marker.)
- Append CHANGELOG.

Run: py -3.12 scripts/reclassify_batch_an.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260603-1259"
DATE = "2026-06-03"
APPLY = "--apply" in sys.argv
U_START = 8300

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
AUDIO_S3  = {"0045da90", "0045daf0", "0045dce0"}
AUDIO_S4  = {"00462760", "004656e0", "00465f40", "00466000", "00466100"}
RENDER_S4 = {"004670a0", "004671d0", "004672d0"}
RENDER_S5 = {"00472b10", "00473220", "004733b0", "004744a0", "004729b0", "00472ad0"}
RENDER_S6 = {"0047cdc0"}
CAMERA_S6 = {"0047c1f0", "0047c230", "0047c270", "0047c2d0"}
HUD_S6    = {"0047d640", "0047def0"}

RECLASS = {}
for r in AUDIO_S3 | AUDIO_S4:        RECLASS[r] = "audio"
for r in RENDER_S4 | RENDER_S5 | RENDER_S6: RECLASS[r] = "render"
for r in CAMERA_S6:                  RECLASS[r] = "camera"
for r in HUD_S6:                     RECLASS[r] = "hud"
assert len(RECLASS) == 24, f"expected 24 reclass, got {len(RECLASS)}"
assert sum(1 for v in RECLASS.values() if v == "render") == 10
assert sum(1 for v in RECLASS.values() if v == "audio")  == 8
assert sum(1 for v in RECLASS.values() if v == "camera") == 4
assert sum(1 for v in RECLASS.values() if v == "hud")    == 2

BOUNDARY = {
    "0045c030": " [boundary render/util-vs-gameplay: look-at matrix builder (normalize x2 + "
                "orthonormalize + concat); kept gameplay (sole caller = effect-spawner FUN_00487280)]",
    "0045a530": " [boundary audio-vs-gameplay: thin event-descriptor wrapper over FUN_00484cf0; "
                "kept gameplay per an_s2 (sibling of FUN_0044bbc0/FUN_0044c490)]",
    "00461e90": " [boundary gameplay-vs-util: switch (selector,key)->(code,code); key constants "
                "decode as 0xAARRGGBB colors (possible surface/material classifier, UNCERTAIN); kept gameplay]",
    "0046bfc0": " [note: vehicle collision/impulse solver (AABB contact + velocity integration via "
                "FUN_004c3ac0 Vec3Magnitude); 0 direct callers = indirect/vtable dispatch (UNCERTAIN)]",
    "00471780": " [stale-name flag: 2026-05-13 master comment labels param_1 'camera-anim entry base "
                "pointer' but body is a rigid-body impulse applier (force pipe FUN_0055b650); kept "
                "gameplay, do NOT carry the camera-anim label forward]",
    "0047cdc0": " [weak-hold: reclassed render but 1-line squared-distance LOD helper; keep-gameplay "
                "acceptable. 3 callers incl render-C2 FUN_00412050]",
}
RECLASS_NOTE = {
    "render": f" | reclass gameplay->render batch_an {SID}: RW draw primitive / render-object / "
              f"scene-graph helper (RwIm2D quad / RW 3D-line / alt-camera triple / LOD setter); "
              f"callers already-C2 render/frontend",
    "audio":  f" | reclass gameplay->audio batch_an {SID}: RWS-audio emitter/channel position+param "
              f"sync (FUN_005a6d60/FUN_005a89d0/FUN_005a86a0 band 0x005a6xxx/0x005a8xxx); callers in audio region",
    "camera": f" | reclass gameplay->camera batch_an s6 {SID}: camera-path node-array containment test "
              f"(DAT_006c2fe8/0x006c2fa8/0x006c27a8); managed by FUN_0047c160 (already camera-C2)",
    "hud":    f" | reclass gameplay->hud batch_an s6 {SID}: on-screen event-marker overlay builder "
              f"(pushes RGBA+geometry draw records into 256-entry display ring DAT_007e9de0; caller FUN_0047e9c0)",
}
NOTE_SUFFIX = f" | C1->C2 batch_an {SID}"


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
edits = {"promoted": 0, "reclass_render": 0, "reclass_audio": 0, "reclass_camera": 0,
         "reclass_hud": 0, "boundary": 0, "drift_not_c1": 0}
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
clog = (f"{DATE}  re-classify batch_an  C1->C2 x156 (gameplay campaign 3/~5, next-156 "
    f"0x456040..0x47f380): 132 stay gameplay + 24 subsystem-reclass (10 gameplay->render = "
    f"3 s4 alt-camera/render-object triple + 6 s5 RwIm2D-quad/RW-3D-line draw + 1 s6 LOD setter; "
    f"8 gameplay->audio = 3 s3 + 5 s4 RWS-audio emitter/channel position+param sync; "
    f"4 gameplay->camera s6 camera-path node containment FUN_0047c160; 2 gameplay->hud s6 "
    f"event-marker overlay builders DAT_007e9de0). 0045c030/0045a530/00461e90/0046bfc0/00471780/"
    f"0047cdc0 boundary-flagged. NO library_skip, NO function_create (all 156 genuine game fns "
    f"w/ function objects). +{len(unc_rows)} U-rows U-{U_START}..U-{U_START+len(unc_rows)-1} "
    f"(non-blocking). Evidence: 6 bucket plates + {SID}. gameplay C1 480 -> 324.")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"hooks.csv edits: {edits}")
if drift: print(f"DRIFT (not-C1 before flip): {drift}")
print(f"U-rows minted: {len(unc_rows)}  (U-{U_START}..U-{U_START+len(unc_rows)-1})")
print(f"plate marker replacements: {plate_marker_edits}")
print(f"plates modified: {len(plate_new_text)}")
if len(unc_rows) > 300:
    print(f"WARNING: U-row count {len(unc_rows)} exceeds reserved range U-8300..U-8599 (300)")
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
