"""r10: cites_r10_v3.json = cites_r10_v2.json + the three widenings made after scoring the independent re-capture
(bt_rerun) with v2 (4 RED rows in 4 of 30 frames, all instances of causes already grouped for the first sample):

  C25-CARLOD-MISSING  : no track restriction (Sands row 1174 Glass, same selection as Neustein / Warzone)
  C27-CAR-PART-OFFSET : Advantage MOVED by <= 0.0199 on any track (Dump 735 0.0123, SuperG 857 0.012)
  C04b-OBJSHAD        : RWObjShad EXTRA / MOVED on any track, U-9710 (Warzone 659 10 vs 6 triangles, Roundabout 1148 / 1183 0.0281)
  C29-RESIDUAL        : narrowed to the two rows that stay unmapped (Egypt 740 '-' EXTRA, Neustein '-' MOVED)
"""
import json
from pathlib import Path
HERE = Path(__file__).resolve().parent
c = json.loads((HERE / "cites_r10_v2.json").read_text())
out = []
for e in c:
    e = dict(e)
    if e["id"] == "C25-CARLOD-MISSING":
        e.pop("tracks", None)
        e["note"] = ("The original draws a car part that the standalone does not (AdvantageSmall 4 draws vs 3 on Neustein "
                     "row 1169, Glass 4 vs 2 on Warzone row 659, Glass 2 vs 0 on Sands row 1174 of the re-capture). The "
                     "standalone has no damage or wreck state and does not model every state input of FUN_00420870's "
                     "per-part visibility (S +0x298, F +0x294): U-9706 (a), (c).")
    if e["id"] == "C29-RESIDUAL":
        e["tex"] = r"^-$"
        e["tracks"] = ["Egypt", "Neustein"]
        e["note"] = ("Two rows with no mapped cause. Egypt row 740: one B-only 6-triangle ground patch with an unnamed "
                     "#128x64:21 texture that the standalone also draws, and matches, in rows 780 and 815. Neustein: one "
                     "thin untextured mesh drawn at the origin (AABB 0.07 x 2.5 x 0.09) where the original has it with AABB "
                     "minimum (7.70, 2.67, 9.23). U-9718.")
    out.append(e)
out += [
    dict(id="C27-CAR-PART-OFFSET", kind=["MOVED"], tex=r"^Advantage$", detail=r"moved by up to 0\.0[01]", status="OPEN",
         evidence="texcompare.py",
         note="Car body parts whose AABB differs from the original's by 0.01 to 0.02 (tolerance 0.01 + 1e-4 * extent), 2 to 4 "
              "parts per row, on cars whose frame LTM is the transplanted one. The part poses come from the standalone's own "
              "wheel / suspension state. U-9718."),
    dict(id="C04b-OBJSHAD", kind=["EXTRA", "MOVED"], tex=r"^RWObjShad$", status="MAPPED",
         evidence="U-9710",
         note="Object-shadow stand-in (LibRw/RwRaceFx.cpp ObjShadowMatrix / DrawObjShadows) against FUN_00481a30's shadows: the "
              "same triangle count with an AABB offset of 0.0281 (Roundabout rows 1148, 1183), or a different shape (Warzone "
              "row 659, 10 triangles vs the original's 6; Warzone has the track's one RWP cylinder). U-9710 (b), (d), (e)."),
]
(HERE / "cites_r10_v3.json").write_text(json.dumps(out, indent=1))
print(len(out), "entries")
