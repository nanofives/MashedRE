"""rhooks: the tracker transaction of this lane (re-classify skill rules: every UNCERTAINTIES row has a path to resolution,
the CHANGELOG entry goes immediately below the whole-line <!-- ENTRIES --> marker, nothing is rewritten except the two rows
this lane resolves). No function moves on the C0..C4 ladder: hooks.csv, STUBS.md, DEFERRED.md are untouched.

    py -3.12 verify\\rhooks_20261016\\apply_trackers.py [--dry-run]

Idempotent: refuses to run twice (the U-9726 row is the marker). UNCERTAINTIES.md is CRLF, CHANGELOG.md is LF; both are read
and written as bytes so no line ending changes.
"""
import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
UNC = ROOT / "UNCERTAINTIES.md"
CHG = ROOT / "re" / "analysis" / "CHANGELOG.md"
R = "verify/rhooks_20261016/RESULT_rhooks.md"
S = "mashedmod/src/mashed_re/Render/TrackHookSlots.cpp"

ROWS = [
    ("U-9726", "structural",
     "hook-slot state that changes after the race starts, on the tracks `" + S + "` covers",
     "The port draws each track's hook-slot state as it is at race reset. The original changes it later: Sands slot 1 "
     "`FUN_004507b0` advances the bridge stages (`_DAT_00684c40`, from `FUN_00407a40(0)`) and flips switch entries 0..3 "
     "(`FUN_00472560`), and draws the Debris clones while their timers `DAT_00684b84` / `DAT_00684be4` are above 0 "
     "(`FUN_00450480`, `FUN_004505a0`); Warzone switches craters off (`FUN_00472560(k,0)` at `0x0045260b`), moves the boat "
     "(`FUN_00451760` / `FUN_00452130`, `BoatAnim.anm`) and draws chunk copies whose flag is set (`FUN_00452310` / "
     "`FUN_00452490`); Highway draws the Jet when `DAT_005f9748` is set (its clump slot is read at `0x0044f707..0x0044f738`, "
     "between slot 1's entry `0x0044eeb0` and slot 11's `0x0044f7d0`). No row of the 30 capfix frames is attributed to one "
     "of them (the original draws BridgeWhole, every crater, the boat at the standalone's pose, no Jet). Section 8 of `" + R + "`.",
     "the triggers and timers of each change, and which race rows a capture would need to see them.",
     "decode Sands slot 1 and `FUN_00407a40`, Warzone `FUN_004525xx` (caller of `0x0045260b`) and slot 1 `0x00452b20`, port, "
     "capture a later race row on Sands / Warzone and score it with `score_tracks.py`.",
     "none (no row on the capfix frames)"),
    ("U-9725", "semantic",
     "SetClumpAnim clock in the standalone (`TrackRenderer::ClumpAnimTick`, called from `CopterTick`)",
     "The ported Dump train is drawn 0.0571 ahead in x on all three capfix frames (735, 775, 810); the train covers 2.28 "
     "over the 40 rows between frames, so the lead is the motion of one tick. The tick runs on the copters' units "
     "(`FUN_0040fc00`: `FUN_00426ab0` at `0x0040fc94`, the copter tick at `0x0040fcb9`, no branch between). Cite C28 in `" + R + "`.",
     "whether the standalone runs one extra update before the first rendered race frame, or the capture's row-to-frame "
     "mapping differs for this clock.",
     "count the original's `FUN_00426700` calls before the row-735 draw (Frida, explicit PID) against the standalone's "
     "`ClumpAnimTick` count before its row-735 frame; the copters' C14 rows share the clock.",
     "none (Dump, 3 MOVED rows of 0.057)"),
    ("U-9724", "structural",
     "`D3d9Render/TrackRenderer.cpp:1419-1421` (every `*.TXD` of the track piz decoded and searched)",
     "dump.piz holds DUMP.TXD and CITY.TXD; COURSE.LUA names `Texture_Dictionary_Filename(\"dump.txd\")`. Train.dff's roof "
     "material `trainroof` exists only in CITY.TXD. The original draws the roof's 32 triangles untextured (Dump 735, the "
     "A-side draw at the roof's AABB, cited C03); the standalone, which resolves a material across every TXD of the piz, "
     "draws it with `trainroof` (cite C30 in `" + R + "`).",
     "which dictionaries the original binds for a track's clumps and RWP objects (only the Texture_Dictionary_Filename one, "
     "or others under conditions not decoded); why the standalone was made to search all of them (the code comment says "
     "props reference secondary dictionaries).",
     "decode the `Texture_Dictionary_Filename` handler and the texture lookup the DFF stream read uses during "
     "`Course::CreateFromDescription` (`0x00479330`), then list every material of every track that resolves only outside "
     "the named TXD.",
     "none beyond the row"),
    ("U-9723", "structural",
     "Highway traffic: the 80-entry vehicle array `0x00890060` (stride 0xf8), slots 0 / 1 / 2 of record \"highway\" "
     "(`0x0044f970`, `0x0044eeb0`, `0x0044e470`)",
     "Slot 2 `FUN_0044e470` draws each vehicle that passes a frustum test (`FUN_004c1b40`) as an LOD atomic plus its wheel and "
     "brake-light atomics, and the Jet (clump 43) inside the same loop when `DAT_005f9748` is set. The standalone no longer "
     "draws the 45 excluded traffic clumps at the origin (rhooks) and draws no traffic: one MISSING traffic3 row per frame "
     "(Highway 853 / 893 / 928, 14 to 25 original draws). Cite C22 in `" + R + "`.",
     "the vehicle record layout, the lane / path data, the spawn and step rules of slots 0 and 1 (12846-byte decompilation "
     "of `0x0044eeb0`), and whether the traffic collides with the cars (a sim effect).",
     "decode slots 0 and 1 and the record, port the motion, compare the traffic3 draws per frame; if vehicles collide, "
     "measure D2 / D3 with the G-CTRL recipe.",
     "Highway D1 breadth (3 rows)"),
    ("U-9722", "structural",
     "Roundabout crane (clump 1 crane.dff, in the RpWorld), slot 1 `FUN_0044d3f0` -> `FUN_0044cb00`; scenario functions "
     "`RoundaboutStartCrane` / `RoundaboutStopCrane` (registered by slot 12 `0x0044ca70`)",
     "The original draws the crane's comp2 / girders / rustycable atomics 4 to 5 units off the standalone's in z on frame "
     "1091 and does not draw two of its meshes (comp2 468 triangles, rustycable 16) that the standalone draws. "
     "Cite C22 in `" + R + "`.",
     "what `FUN_0044cb00` writes to the crane's frames and which atomics it hides, and the scenario sequence step that starts it.",
     "decode `FUN_0044cb00` and the RoundaboutStartCrane function, port the frame poses and the RENDER bits, re-score "
     "Roundabout 1091.",
     "Roundabout D1 breadth (5 rows)"),
    ("U-9721", "structural",
     "Roundabout railcars: `FUN_0044c8b0` (slot 0) clones clump 2 (railcar.dff) three times and adds each to the RpWorld; "
     "`FUN_0044c4f0` puts each on the spline at course + 0x100b8 (parameter `DAT_005f9724[i]`) and registers it as a scene "
     "physics body (`FUN_0047fad0`, `FUN_0047cdc0(.., 100.0)`); slot 1 `FUN_0044c920` steps them (pairwise `FUN_0044c740`, "
     "per car `FUN_0044c5f0`)",
     "The standalone no longer draws the railcar base clump at the origin (rhooks) and draws no railcar: comp2 716 + gravel2 "
     "MISSING rows on all three Roundabout frames (the original's three cars sit still across them, e.g. AABB minimum "
     "(-18.17, -0.44, -18.12)). The spline path uses RpSpline (`FUN_005459c0`, `FUN_004728f0` arc length); the standalone "
     "has no RpSpline port and no scene-object physics. Cite C22 in `" + R + "`.",
     "the spline asset (which COURSE.LUA command loads course + 0x100b8), `FUN_0044c5f0`'s motion law, and whether the "
     "railcars collide with the cars (a sim effect).",
     "decode `FUN_005459c0` and `FUN_0044c5f0`, place the three cars at their reset pose, re-score Roundabout; port the "
     "motion with its contacts and measure D2 / D3 with the G-CTRL recipe.",
     "Roundabout D1 breadth (6 rows)"),
]

RESOLVED = {
    "U-9713": ("| U-9713 | ~~structural~~ **RESOLVED 2026-10-16** (rhooks: hook-slot draw model ported) | per-track hook-table "
               "slot functions (table `0x005f33f8`) for Warzone, Highway, Roundabout, Sands, Neustein; `" + S + "`, "
               "`D3d9Render/TrackRenderer.cpp` | Decoded and ported, default ON (`MASHED_NO_HOOKSLOTS=1` opts out): an "
               "excluded clump of a covered track is drawn only where the track's slots or the switch table "
               "`FUN_004725c0` draw it; Mts copy groups (`FUN_00479f30`), the three RpLODAtomic hooks (`FUN_004758d0`, "
               "select `FUN_00557a60`), the slots' cull / zwrite states, slot-10 groups after the copters. The answers the "
               "row asked for: Warzone groups OakTrunk x OakTrees_1 / OakTrees_2 and CrossTree x Crosstrees (slot 10), "
               "Fence_2 x fence_2 (slot 2), craters 9..16 as switch entries ON; Sands draws BridgeWhole alone (switch entry "
               "0 ON, 18 / 19 / Jump OFF). Scoring of the ten capfix tracks: C22 rows 50 -> 14, matched draws 5578 -> 6405 "
               "of 7691 (with U-9716 / U-9717), G-CTRL unchanged (`" + R + "`). What C22 still holds is filed as U-9721 "
               "(railcars), U-9722 (crane), U-9723 (Highway traffic), U-9726 (state after the race starts). | none | none | none |"),
    "U-9717": ("| U-9717 | ~~structural~~ **RESOLVED 2026-10-16** (rhooks: SetClumpAnim ported) | Dump `Train.dff` "
               "(`SetClumpAnim( LINEAR , 0 , 3 )`, `local LINEAR = 1`) | Ported, default ON (`MASHED_NO_CLUMPANIM=1` opts "
               "out): records of `FUN_0047ad40` (+0x70 clump, +0x71 anim, +0x74 time, +0x78 type), tick `FUN_00426700` on "
               "the copters' units, sampler `FUN_004b47e0` -> `FUN_00532b80` -> `FUN_00532a60` for type 1. The train is drawn "
               "on all three Dump frames, 0.0571 ahead in x (one tick, U-9725); its roof is textured where the original's "
               "is not (U-9724). G-CTRL unchanged (`" + R + "`). | none | none | none |"),
}

APPEND = {
    "U-9719": " [rhooks] The 0 -> 1 write is `FUN_00451820` @ 0x00451820 (case 0 of `FUN_00451d80`): per car, the car "
              "matrix (`FUN_0046d4a0`) times the inverse of the object matrix `[obj+0x94]` (`FUN_004c4dc0` invert, "
              "`FUN_004c52f0` transform, combine 2); if the transformed x lies between `[obj+0x20]` and `[obj+0x14]` and z "
              "between `[obj+0x28]` and `[obj+0x1c]`, `[obj+0x48] = 1` (it also calls `FUN_0046be10(&DAT_005f9788, car, 0, 0)` "
              "for every car). The object is slot 0's `FUN_00451760(&DAT_00684d18, clump 1 Boat.dff, anim table +0x105b0)` "
              "pontoon: an RpHAnim hierarchy (`FUN_004b4790`, `FUN_00533ec0` update, time `+0xc8`), drawn by slot 2 "
              "(`FUN_00452130`, `0x00452d68`), reset by slots 6 / 7 (`FUN_00451cc0`) and by the callback `0x00452170` "
              "(Tower / Deck object matrices back to `0x00684de8` / `0x00684c58`, `FUN_0047d180`). Slot 0 hides the Tower "
              "(label 1000) and Deck (label 2000) objects' RENDER bit (`FUN_0047d100(.., 0)`). A car leaving the box gets the "
              "object's movement `[obj+0xa8..0xb0]` (`FUN_0046bf50`) and a wheel test (`FUN_0041f1e0`, `FUN_00481750`, "
              "`FUN_00480720`): porting the producer needs that car coupling and the Deck physics body, which the standalone "
              "does not have (`" + R + "` section 8).",
    "U-9716": " [rhooks] The race-reset rule is decoded and ported (default ON, `MASHED_NO_SCENARIOVIS=1`): `FUN_00471cf0` "
              "(from `FUN_004111c0`) sets the objects of every scenario in a Set_Sequence list to physics membership "
              "(`FUN_0047d080`) and RENDER bit (`FUN_0047d100`) = (scenario == the list's first); hidden objects cast no "
              "shadow (`FUN_00481a30` tests `atomic+2 & 4`). Roundabout Sign02 / interactiveHut and Warzone interactiveHut "
              "extras are gone (C26 6 -> 3). Left: Roundabout Trucktyre AABB 0.0115 off (2 rows), Neustein Post_1: the "
              "original draws none of its 49 records on the three frames, no scenario holds it and creation "
              "(`FUN_00480340` -> `FUN_0047ff70`) has no cap; cause not decoded (`" + R + "`).",
    "U-9701": " [rhooks] The leaf sprites are an RpPTank atomic: `FUN_0047c330` (`FUN_00534b60(0x800, 0x87, 0)`, texture "
              "from the course TXD), `FUN_0047c560` adds one sprite per leaf Mts record around each tree (size from the "
              "record axes times the tree's), the per-frame update `FUN_0047cbb0` (Sands slot 1, Warzone / Neustein "
              "slot 1), the draw `FUN_0047cd30` (the atomic's render callback) from slot 10. Not ported (`" + R + "`).",
    "U-9710": " [rhooks] A scenario object hidden at race reset casts no shadow (`FUN_00481a30` requires the atomic RENDER "
              "bit); the port now passes `casts = false` for them, which matched two Warzone RWObjShad rows (`" + R + "`).",
    "U-9718": " [rhooks] (b) is resolved: the Neustein mesh at the origin was the Trunk base clump 3, which the original "
              "never draws (its copies are slot-2 Mts groups `0x0044af47` / `0x0044af53` / `0x0044af65`); C29 Neustein "
              "rows 3 -> 0 (`" + R + "`).",
}

CHG_ENTRY = ("2026-10-16  rhooks (work/rhooks): U-9713 and U-9717 RESOLVED. Per-track hook-slot draw model "
             "(Render/TrackHookSlots.cpp: Roundabout, Sands, Warzone, Highway, Neustein: Mts copy groups FUN_00479f30, LOD "
             "hooks FUN_004758d0 / FUN_00557a60, switch table FUN_004725c0, slot render states), SetClumpAnim "
             "(FUN_0047ad40 / FUN_00426700 / FUN_004b47e0) and the race-reset scenario visibility (FUN_00471cf0), all "
             "default ON with opt-outs. Ten capfix tracks: 979 -> 879 rows, 0 RED, matched draws 5578 -> 6405 of 7691, "
             "triangles 86.6 -> 88.2 %; G-CTRL 2b4d47bb / 356821c8 / f7a155e1 unchanged. New U-9721 .. U-9726; U-9701 / "
             "U-9710 / U-9716 / U-9718 / U-9719 extended. No function moved on the ladder.  " + R + "\n")


def main() -> int:
    ap = argparse.ArgumentParser(); ap.add_argument("--dry-run", action="store_true"); a = ap.parse_args()
    u = UNC.read_bytes().decode("utf-8")
    if "| U-9726 |" in u:
        print("U-9726 already present: the transaction was applied, nothing to do"); return 1
    eol = "\r\n"
    sep = "|----|------|-------|-----------|------------------|--------------------|--------|" + eol
    i = u.index(sep) + len(sep)
    block = "".join("| " + " | ".join(r) + " |" + eol for r in ROWS)
    u = u[:i] + block + u[i:]
    for uid, new in RESOLVED.items():
        j = u.index(f"| {uid} |")
        k = u.index(eol, j)
        u = u[:j] + new + u[k:]
    for uid, txt in APPEND.items():
        j = u.index(f"| {uid} |")
        k = u.index(eol, j)
        cols = u[j:k].split(" | ")
        if len(cols) != 7:
            print(f"{uid}: row has {len(cols)} columns, not appended"); return 2
        cols[3] = cols[3] + txt
        u = u[:j] + " | ".join(cols) + u[k:]
    c = CHG.read_bytes().decode("utf-8")
    m = "\n<!-- ENTRIES -->\n"
    p = c.index(m) + len(m)
    c = c[:p] + CHG_ENTRY + c[p:]
    if a.dry_run:
        print("dry run: would write", len(u), len(c)); return 0
    UNC.write_bytes(u.encode("utf-8"))
    CHG.write_bytes(c.encode("utf-8"))
    print(f"applied: {len(ROWS)} rows, {len(RESOLVED)} resolved, {len(APPEND)} extended, 1 CHANGELOG entry")
    return 0


if __name__ == "__main__":
    sys.exit(main())
