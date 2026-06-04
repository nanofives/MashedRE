"""Central re-classify for batch_ar (render campaign batch 2/2 FINAL, last-302 0x4e8030..0x5cb42b).

The batch_ar header's dominant-library_skip prediction held COMPLETELY:
- 0 plated RVAs (all six sessions ALL-SKIP, rvas=NONE — nothing promoted, no U-/S- mints;
  reserved ranges U-9200..9499 / S-7200..7399 untouched).
- 302 library_skip reclass-OUT to third-party-library[<tag>], kept C1. The entire remaining
  render-C1 band is vendored: RenderWare 3.x core/driver/toolkit + D3DX9 + RW-Physics fringe.
- This CLOSES the render C1->C2 campaign: render C1 302 -> 0. After ar, project-wide
  non-library C1 = 8 stragglers (util 3, unknown 2, boot/io/save 1 each).

TAG REFINEMENT (the one judgement call this pass):
- Workers s1/s2 tagged 21 RVAs with a FLAT `d3dx9`. The existing hooks.csv convention has
  specific d3dx9 sub-tags (d3dx9-psgp 93, d3dx9-shader-compiler 80, d3dx9-hlsl-shader-compiler,
  d3dx9-backward-extension, d3dx9-fx). To avoid creating a 6th inconsistent variant, the flat
  `d3dx9` is refined to the existing sub-tag the worker's OWN documented evidence points to:
    * d3dx9-psgp (12): the SSE/SSE2/3DNow!/femms matrix/vector/quaternion math kernels
      (004fcac3 + 0050570a/0050706f/00508772/0050a41e/0050b6ff/0050c897 [s1] +
       0050d089/0050e1dd/0050e8b7/0050ef6d/0050eff8 [s2]).
    * d3dx9-shader-compiler (9): the D3DXAssembleShader / "D3DX9 Shader Assembler" /
      ShaderCompiler:: family + the D3DDECLUSAGE vertex-decl parser
      (00500c48/005010f1/00501ce4/0050272a/00502e9e/005033f4/00503a3a/00504008 [s1] +
       005112c9 [s2]).
  This is a within-third-party-library documentation refinement (C1 unchanged); recorded in
  the note + CHANGELOG for transparency.

Flags:
- 004fcac3 — worker [UNCERTAIN] on symbol name (RwMatrixMultiply per batch_aa s3 note vs
  D3DXMatrixMultiply 4x4 shape); operand is a full 16-float 4x4 with zero RW module touches,
  so d3dx9-psgp is the better fit. Naming question only; library verdict identical.
- 00553c50/00553cf0/00553e00/00553e80/00553ef0 — Rt2d path bbox/node primitives, the library
  SUBSTRATE of the 0x00554010 font-vector module (ao_s2 reclassed gameplay->hud with
  module-vendor-doubt). s6's bodies resolve the doubt TOWARD vendor; tagged renderware.
- 005356f0 — Ghidra thunk to FUN_004e6920 (RW core destructor); body is RW core, tag renderware.

Run: py -3.12 scripts/reclassify_batch_ar.py [--apply]   (dry run without --apply)
"""
from __future__ import annotations
import csv, io, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SID = "sweep-20260604-0020"
DATE = "2026-06-04"
APPLY = "--apply" in sys.argv

# ---- per-session RVA lists with tags ----
S1_RW = ["004e8030", "004e8090", "004e80c0", "004e80f0", "004e8120", "004e8400",
         "004e8440", "004e8640", "004e8750", "004e8910", "004e8940", "004e89a0",
         "004e89e0", "004e8a10", "004e8c70", "004e8f50", "004e8f80", "004e8fb0",
         "004e8fd0", "004e9010", "004e9080", "004e9330", "004e9850", "004e98d0",
         "004e9910", "004e9950", "004e99b0", "004e9e40", "004ea220", "004ea9a0",
         "004eb3c0", "004eb9d0", "004eb9e0", "004eba40", "004ebb00", "004ebc30"]
S1_PSGP = ["004fcac3", "0050570a", "0050706f", "00508772", "0050a41e", "0050b6ff", "0050c897"]
S1_SHCC = ["00500c48", "005010f1", "00501ce4", "0050272a", "00502e9e", "005033f4", "00503a3a", "00504008"]

S2_PSGP = ["0050d089", "0050e1dd", "0050e8b7", "0050ef6d", "0050eff8"]
S2_SHCC = ["005112c9"]
S2_RW = ["0052d8e0", "0052df70", "0052e0e0", "0052e2c0", "0052e310", "0052e650",
         "0052ea70", "0052eb40", "0052eeb0", "0052fd60", "0052fdb0", "0052fe90",
         "0052ff00", "0052ffd0", "00530000", "00530010", "00530050", "00530090",
         "005300d0", "00530160", "005302c0", "00530650", "00530c00", "00532000",
         "005320b0", "005327d0", "00532a60", "00532b80", "005333a0", "005336d0",
         "00533d00", "00533ec0", "00534870", "00534a80", "00535330", "005356f0",
         "00535700", "00535910", "00538310", "00538600", "00538a80", "00538b70",
         "00538ba0", "00538bd0", "00538c80"]

S3_RW = ["00538d60", "00539900", "00539ec0", "0053a6c0", "0053a7a0", "0053b7b0",
         "0053bdf0", "0053c5d0", "0053c6e0", "0053ccc0", "0053cd80", "0053d040",
         "0053d060", "0053d090", "0053d0b0", "0053d200", "0053d400", "0053d460",
         "0053d5a0", "0053d690", "0053d6c0", "0053d820", "0053d920", "0053dba0",
         "0053de50", "0053dea0", "0053e430", "0053e5d0", "0053e690", "0053e6f0",
         "0053e730", "0053e760", "0053e790", "0053e7f0", "0053e830", "0053eaa0",
         "0053eca0", "0053f150", "0053fa20", "0053fb00", "0053fb70", "0053fba0",
         "0053fbb0", "0053fd30", "0053fea0", "0053fec0", "00540080", "00540100",
         "00540160", "005401d0"]

S4_RW = ["00540260", "005402d0", "00540340", "00540510", "005405c0", "005412d0",
         "00541b50", "00541d40", "005422c0", "00543710", "00543a40", "00543b10",
         "00543b20", "00543b30", "00543b90", "00543bb0", "00543d40", "00543d70",
         "00543da0", "00543dc0", "00543df0", "00543e00", "00543e10", "00543e30",
         "00543e50", "005449f0", "00544a70", "00544ad0", "00544bf0", "00544d20",
         "00545260", "005459c0", "00545d30", "00546320", "00546380", "00546530",
         "00546890", "00546bf0", "00546c50", "00546cb0", "00546d10", "00547230",
         "005492a0", "005493d0", "005493e0", "00549580", "005495a0", "005495b0",
         "00549610", "00549620"]

S5_RW = ["00549640", "00549900", "00549970", "005499f0", "00549a50", "00549a90",
         "00549aa0", "00549b20", "0054a120", "0054b340", "0054b630", "0054b6d0",
         "0054b740", "0054b8c0", "0054ba20", "0054bda0", "0054c400", "0054c4f0",
         "0054c870", "0054ceb0", "0054cfa0", "0054d090", "0054d4d0", "0054dbd0",
         "0054e520", "0054eb90", "0054f410", "0054f8d0", "0054fd60", "00550130",
         "00550350", "00550400", "005504d0", "00550520", "00550580", "00550670",
         "00550740", "00550750", "005507a0", "00550a20", "00550bd0", "00551090",
         "00551190", "005512b0", "00551330", "00551410", "00551460", "00551510",
         "00551550", "005515a0"]

S6_RW = ["00551840", "00551a50", "00551ad0", "00551c40", "00551ca0", "00551ce0",
         "00551cf0", "00551d40", "00551fd0", "00551fe0", "00552020", "00552230",
         "005522e0", "005524a0", "00552720", "00552890", "00552920", "00552fa0",
         "00553030", "005530f0", "005531f0", "005533d0", "00553590", "00553620",
         "005538a0", "00553c50", "00553cf0", "00553e00", "00553e80", "00553ef0",
         "005578a0", "005584c0", "00558df0", "00561ee0", "005cb404", "005cb42b"]
S6_RWP = ["0055ac00", "0055ac50", "0055ad30", "0055b650", "0057bf30", "0057c0d0",
          "0057c1b0", "0057c210", "0057c270", "0057c2b0", "0057c370", "0057c420",
          "0057c440", "0057c550"]

RW = "renderware"
PSGP = "d3dx9-psgp"
SHCC = "d3dx9-shader-compiler"
RWP = "RenderWare-Physics-3.7"

CLUSTER = {
    "s1": "RpGeometry/RpWorld stream+plugin tail + RW D3D9 driver instance path; "
          "D3DX9 PSGP math kernels + shader-assembler past the PSGP band end",
    "s2": "D3DX9 PSGP/shader-compiler residue + RtAnim interpolator + MatFX/RpSkin D3D9 CSL "
          "pipeline + PTank 0x12f + world-sector BVH plugin 0x12a",
    "s3": "two RW plugin modules: 0x11d collision toolkit + 0x120 material-effects toolkit",
    "s4": "MatFX D3D9 pipeline + RpUserData 0x11f + plugin-0x105 + RpSpline 0x102 + RpPatch 0x123",
    "s5": "RpPatch 0x123 toolkit + texdict stream readers + RtFS manager + plugin-0x135 registrar",
    "s6": "Rt2d 2D-graphics toolkit (anim-channel + vector-path, font-module substrate) + "
          "RW-Physics-3.7 fringe abutting the qhull island + RW-core residue",
}

LIBSKIP: dict[str, tuple[str, str]] = {}   # rva -> (tag, session)
for sess, lst, tag in (
        ("s1", S1_RW, RW), ("s1", S1_PSGP, PSGP), ("s1", S1_SHCC, SHCC),
        ("s2", S2_PSGP, PSGP), ("s2", S2_SHCC, SHCC), ("s2", S2_RW, RW),
        ("s3", S3_RW, RW), ("s4", S4_RW, RW), ("s5", S5_RW, RW),
        ("s6", S6_RW, RW), ("s6", S6_RWP, RWP)):
    for r in lst:
        assert r not in LIBSKIP, f"duplicate {r}"
        LIBSKIP[r] = (tag, sess)

assert len(LIBSKIP) == 302, f"expected 302 library_skip, got {len(LIBSKIP)}"
_by_tag = {}
for tag, _ in LIBSKIP.values():
    _by_tag[tag] = _by_tag.get(tag, 0) + 1
assert _by_tag == {RW: 267, PSGP: 12, SHCC: 9, RWP: 14}, f"tag tally off: {_by_tag}"

FLAGS = {
    "004fcac3": " [tag-doubt ar_s1: symbol-name RwMatrixMultiply (batch_aa s3) vs D3DXMatrixMultiply "
                "4x4 shape; full 16-float 4x4, zero RW module touches -> d3dx9-psgp; naming only]",
    "00553c50": " [Rt2d substrate of the 0x00554010 font-vector module; ao_s2 hud module-vendor-doubt "
                "resolves TOWARD vendor per ar_s6 bodies]",
    "00553cf0": " [Rt2d font-module substrate; see 00553c50]",
    "00553e00": " [Rt2d font-module substrate; see 00553c50]",
    "00553e80": " [Rt2d font-module substrate; see 00553c50]",
    "00553ef0": " [Rt2d font-module substrate; see 00553c50]",
    "005356f0": " [Ghidra thunk to FUN_004e6920 RW-core destructor; tag renderware]",
}
DOUBT_TAGS = {PSGP, SHCC}  # d3dx9 refined-from-flat note

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

edits = {"libskip": 0, "flags": 0, "d3dx9_refined": 0, "drift_not_c1": 0, "drift_not_render": 0}
drift = []
for rva, (tag, sess) in LIBSKIP.items():
    i = idx_by_rva.get(rva)
    if i is None: sys.exit(f"FATAL {rva} not in hooks.csv")
    fields = parse(hk_lines[i])
    if fields[3] != "C1":
        edits["drift_not_c1"] += 1; drift.append(f"{rva}:{fields[3]}")
    if fields[2] != "render":
        edits["drift_not_render"] += 1; drift.append(f"{rva}:subsys={fields[2]}")
    fields[2] = f"third-party-library[{tag}]"
    while len(fields) < 9: fields.append("")
    note = (f" | reclass-OUT batch_ar {sess} {SID}: vendored {tag} primitive "
            f"({CLUSTER[sess]}) mislabeled render, kept-C1 (not hand-plated); "
            f"see project_qhull_rwphysics_island")
    if tag in DOUBT_TAGS:
        note += f" [d3dx9 sub-tag {tag} refined from worker flat 'd3dx9' via documented evidence]"
        edits["d3dx9_refined"] += 1
    if rva in FLAGS:
        note += FLAGS[rva]; edits["flags"] += 1
    fields[8] = (fields[8] or "") + note
    hk_lines[i] = serialize(fields)
    edits["libskip"] += 1

# ===================== CHANGELOG =====================
clog = (f"{DATE}  re-classify batch_ar  reclass-OUT x302 (render campaign 2/2 FINAL, last-302 "
    f"0x4e8030..0x5cb42b): ALL six sessions ALL-SKIP — entire remaining render band is vendored "
    f"(267 renderware = RpGeometry/RpWorld/RW-D3D9-driver/RtAnim/MatFX/RpSkin/PTank/RpPatch/RtFS/"
    f"Rt2d toolkits + 6 generic RW; 12 d3dx9-psgp + 9 d3dx9-shader-compiler [refined from worker "
    f"flat 'd3dx9' via documented evidence]; 14 RenderWare-Physics-3.7 qhull-adjacent fringe). "
    f"0 plated, 0 promoted, 0 U-/S- mints (U-9200..9499/S-7200..7399 untouched). qhull watch s6 "
    f"NEGATIVE (no qh_*-shapes; island still 0x0057c5b0). Flags: 004fcac3 name-doubt; "
    f"00553c50..ef0 Rt2d font-module substrate (ao_s2 hud doubt -> vendor); 005356f0 RW-core thunk. "
    f"Evidence: ar_s1..s6 fragments + {SID}. render C1 302 -> 0 — RENDER C1->C2 CAMPAIGN CLOSED "
    f"(aq 458->302, ar ->0). C2 unchanged at 4389. Remaining non-library C1 = 8 stragglers "
    f"(util 3, unknown 2, boot/io/save 1).")

try: sys.stdout.reconfigure(encoding="utf-8")
except Exception: pass
print(f"APPLY={APPLY}")
print(f"tag tally: {_by_tag}")
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
