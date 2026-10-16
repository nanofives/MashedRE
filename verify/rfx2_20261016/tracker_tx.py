"""rfx2 tracker transaction (re-classify): UNCERTAINTIES.md U-9703 / U-9704 / U-9705 narrowed, U-9710 /
U-9711 filed after U-9705; re/analysis/CHANGELOG.md entries prepended below the ENTRIES marker.
No hooks.csv change. Asserts every replacement matches exactly once; scans for ESC bytes after.
"""
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
U = WT / "UNCERTAINTIES.md"
C = WT / "re" / "analysis" / "CHANGELOG.md"
EV = "verify/rfx2_20261016/RESULT_rfx2.md"

U9705 = ("| U-9705 | structural **NARROWED 2026-10-16 (rfx2): RWObjShad, Headlight, lensflare-fs8 ported, default ON** | "
         "C04 / C09 draws rfx did not port (`verify/rfx_20261016/RESULT_rfx.md` section 6; `" + EV + "` section 4) | "
         "Ported by rfx2: RWObjShad (`FUN_00481a30` / `FUN_00487280` / `FUN_00487d70`, opt-out `MASHED_OBJ_SHADOW=0`), "
         "Headlight (`FUN_00412f30` / `FUN_00412cf0` / `FUN_004128f0` / `FUN_00413a40`, `MASHED_HEADLIGHT=0`), lensflare-fs8 "
         "(lists from `FUN_00487e70`, batches `FUN_00488e70` / `FUN_004883b0`, `MASHED_LENSFLARE=0`); their stand-ins are "
         "U-9710 / U-9711. Still not drawn: flatshad (`FUN_0048e7b0` over the per-type particle buffers `0x00722250..0x007228b0`, "
         "written by the particle simulation `FUN_0048cf90`) and VehicleIcons (`FUN_00413cb0` per car x 5 icon slots, fade "
         "`DAT_0063bc60` moved by 2 dt, slot-0 flag from `FUN_00442c80`: `DAT_008989b0[car] > [0x5cc9b8]` in mode 6; batch "
         "`DAT_0063bd50`, flush `FUN_00413f50`). | flatshad: the particle simulation (C05). VehicleIcons: what writes "
         "`DAT_008989b0` and the other icon slots' `FUN_00413c70` callers; the matched-pose trace carries neither. | "
         "flatshad with the C05 particle port. VehicleIcons: decode the `DAT_008989b0` writer and the `FUN_00413c70` callers, "
         "sample them with `camera_probe.py --fx-out`, replay them as instrument inputs. | 5 C04 / C09 rows on the 9 d1r1 "
         "frames (flatshad Arctic 3, VehicleIcons Arctic 1126 + City 836) |")

U9704 = ("| U-9704 | structural **NARROWED 2026-10-16 (rfx2): non-team panels ported, default ON** | in-race HUD `FUN_0040dfc0` "
         "(`LibRw/RwRaceFx.cpp` RaceFx_RenderHud) | Ported: states 5/6; the team panels `FUN_0041c0c0` (rfx); the non-team "
         "panels `FUN_0041b630` -> `FUN_0041b340` with `FUN_0041b450` (PANEL0-3.DFF), `FUN_0041adb0`, `FUN_0041ae20`, "
         "`FUN_0041af50` (rfx2: GREEN 3/3 on the Team-Play-off reference `log/rfx2/nt_training`, opt-out "
         "`MASHED_HUD_NONTEAM=0`); the start lights `FUN_0041db80` / `FUN_0041d930`. Not ported: the per-mode HUDs "
         "`FUN_0041e850` (type 2), `FUN_0041ded0` / `FUN_0041c300` / `FUN_0041a3e0` (types 3-5 by `DAT_007f0fd0`), "
         "`FUN_00403160` (type 0xb) and the standings branch (state 7; the standalone's 2D overlay). With `MASHED_SUBSTATE` "
         "off the start-light clock `DAT_0063d588` adds `FUN_0040fc00`'s step `(float)units * [0x005cc948]` (0x0040fc57) but "
         "is zeroed 113 ticks before the stand-in GO, not at the state 3 -> 4 transition of `FUN_004102f0` (0x00410387), "
         "which exists only under `MASHED_SUBSTATE`; the HUD state comes from the stand-in countdown (3 / 5 / 6). In the "
         "default dev route the non-team panels activate no record: the standalone leaves `DAT_008a94d0` "
         "(`FUN_0040e340`) at 0. | Captures of the per-mode HUDs; the race state machine on the default path; the "
         "standalone's participant count where `FUN_0040e340` reads it. | Per-mode HUDs: one `camera_probe.py --hud-out "
         "--drawrec` reference per mode (rfx2's `--teamplay` input poke is the recipe). Clock: retire the adapter when "
         "`MASHED_SUBSTATE` becomes the default. Panels: write the participant count to `DAT_008a94d0`, then measure the "
         "default route. | C08 rows on non-default-mode scenarios |")

U9710 = ("| U-9710 | structural | object-shadow (RWObjShad) stand-ins (`LibRw/RwRaceFx.cpp` ObjShadowMatrix / DrawObjShadows; "
         "TrackRenderer RWP inputs) | (a) the world query: the original's `FUN_00538c80` BSP walk stops at the FIRST hit its "
         "sector order finds (`FUN_00487180` returns 0); the port takes the nearest hit over the flat collision soup, "
         "two-sided. (b) `RwMatrixInvert`'s general path `FUN_004c4eb0` is a cofactor inverse here. (c) the "
         "`FUN_0045c110` material codes 0xffNN3cff / 0xffNNffff read the runtime table `DAT_00691508` (`FUN_00472550`); "
         "only 0xff010101 / 0xffff0080 block a shadow here. (d) `FUN_00481a30` runs in the tick after the director "
         "(`FUN_0040fc00`); the port uses this frame's render camera position with the previous render's frustum planes "
         "(Arctic 1051 draws 7 shadows only with the previous planes). (e) RWP cylinders / spheres (`FUN_0047f940` / "
         "`FUN_0047fe00`) keep their index slot but cast no shadow (their primitives' fatness default, `FUN_0055c380`, is "
         "not read; Warzone has one). (f) the `DAT_007f0fd0 == 5` branch (0x00481ab3), `FUN_00484cf0` / `FUN_004818d0` and "
         "the per-frame recompute of a moving object are not modelled (the standalone's RWP objects are static). | (a) "
         "whether any object's ray meets two triangles; (c) `DAT_00691508` per track; (d) the planes `FUN_004c1b40` reads in "
         "the tick, live. | Sample `0x006c3670` / `0x006cad50` per object on every track (`camera_probe.py --fx-out`) and "
         "compare with the port's matrices; decode `FUN_00538d60`'s sector order if a two-hit object shows up. | none on the "
         "9 d1r1 frames (RWObjShad GREEN 7/7) |")

U9711 = ("| U-9711 | structural | Headlight and lensflare-fs8 stand-ins (`LibRw/RwRaceFx.cpp` DrawHeadlights / LensBuild; "
         "TrackRenderer light list) | (a) the Headlight fade `DAT_005f2a4c` is taken from the car clump root instead of the "
         "vehicle record +0x30 (`FUN_0046d4a0`); both give -0.05 when the car is 2+ units from the camera. (b) the camera "
         "view matrix (camera + 0x20) is rebuilt from the LTM by the relation measured on the original (x' = -0.5 * "
         "recipViewWindow.x * x + 0.5 z, y likewise, z' = z; 5.7e-6 on City); RW's camera sync is not transcribed. (c) the "
         "brake byte `0x007f103c + ctrl*0x4c + 1` comes only from the matched-pose instrument (carlod's brake rows); on "
         "City 801 the original draws two brake-light sprites with no brake plate on any car, so one MISSING / EXTRA pair "
         "stays (cite C09b). (d) the two lensflare batches are built once at the third Render; the original's build tick "
         "(the first `FUN_00488e70` with `DAT_007030b0` set) is not decoded, the timing is inferred from Arctic 1051's "
         "AABBs (the bob light holds one tick's k). (e) only light types 2 / 3 / 7 / 0xc (the ones City and Arctic create) "
         "are ported; the per-light frame path (+0x2c: `FUN_004c3d90`, k times the frame at . camera at) is unused there. | "
         "(a) the vehicle record position per car per frame; (c) the brake byte per controller per frame; (d) when "
         "`FUN_004881d0` runs. | Add the vehicle record positions and brake bytes to the trace (`camera_probe.py`); decode "
         "`FUN_004881d0`'s caller. | City 801 Headlight pair (C09b) |")

OLD_9703 = ("Outside the d1r1 replay the beam's body frame is the car instance matrix, without the librwrest body pose "
            "(`BuildCarPoses`, frame of part 0x13).")
NEW_9703 = ("Outside the d1r1 replay the frame the beam / light parts hang off is (rfx2) X * pose(part 0x13, `FUN_0041f4f0` "
            "via BuildCarPoses) * Y * root (CarModel body_X / body_Y; `MASHED_FX_BODYPOSE=0` keeps the instance matrix); "
            "under the replay that chain differs from the trace's F{i} by median 0, max 0.054 (the standalone's own "
            "suspension pose, `MASHED_DBG_FXBODY`).")

ENTRIES = [
    "2026-10-16  rfx2 (work/rfx2)  C04 / C08 / C09 rows 24 -> 7 on the 9 d1r1 frames (HUD replay, carlod brake rows), 0 RED: "
    "RWObjShad 7/7, lensflare-fs8 6/6, Headlight 5/6 GREEN, default ON with opt-outs; non-team panels FUN_0041b630 GREEN 3/3 "
    "on a Team-Play-off reference (camera_probe --teamplay 0), default ON; G-CTRL 716cb2fd / 2a2afda1 / ba90ed88 base and "
    "final; no-op 9/9  " + EV,
    "2026-10-16  U-9711  filed  Headlight / lensflare-fs8 stand-ins (fade input, measured view matrix, brake byte from the "
    "instrument: City 801 pair, lensflare build tick) (rfx2)  " + EV,
    "2026-10-16  U-9710  filed  RWObjShad stand-ins (nearest soup hit for the BSP first hit, inverse, material codes, tick "
    "camera, cylinders / spheres, static objects) (rfx2)  " + EV,
    "2026-10-16  U-9705  narrowed  RWObjShad, Headlight, lensflare-fs8 ported default ON; flatshad (C05) and VehicleIcons "
    "(DAT_008989b0, fade history) remain, 5 rows (rfx2)  " + EV,
    "2026-10-16  U-9704  narrowed  non-team panels ported default ON (GREEN 3/3, Team-Play-off reference); clock step is "
    "FUN_0040fc00's, its zero point needs MASHED_SUBSTATE; per-mode HUDs open (rfx2)  " + EV,
    "2026-10-16  U-9703  narrowed  beam / light body frame from the librwrest pose outside the replay; liveness stand-in and "
    "0x10 / 0x400 clears remain (rfx2)  " + EV,
]


def main() -> int:
    raw = U.read_bytes().decode("utf-8")
    lines = raw.split("\r\n")
    idx = {k: [i for i, ln in enumerate(lines) if ln.startswith(f"| {k} |")] for k in ("U-9703", "U-9704", "U-9705",
                                                                                        "U-9710", "U-9711")}
    assert len(idx["U-9703"]) == 1 and len(idx["U-9704"]) == 1 and len(idx["U-9705"]) == 1, idx
    assert not idx["U-9710"] and not idx["U-9711"], "ids already used"
    l3 = lines[idx["U-9703"][0]]
    assert l3.count(OLD_9703) == 1, "U-9703 sentence not found once"
    l3 = l3.replace(OLD_9703, NEW_9703)
    assert l3.startswith("| U-9703 | semantic | ")
    l3 = l3.replace("| U-9703 | semantic | ",
                    "| U-9703 | semantic **NARROWED 2026-10-16 (rfx2): body frame from the librwrest pose** | ", 1)
    lines[idx["U-9703"][0]] = l3
    lines[idx["U-9704"][0]] = U9704
    lines[idx["U-9705"][0]] = U9705
    at = idx["U-9705"][0] + 1
    lines[at:at] = [U9710, U9711]
    out = "\r\n".join(lines)
    assert "\x1b" not in out
    U.write_bytes(out.encode("utf-8"))
    craw = C.read_bytes().decode("utf-8")
    marker = "<!-- ENTRIES -->"
    assert craw.count(marker + "\n") == 1, "marker line not found once (LF file)"
    craw = craw.replace(marker + "\n", marker + "\n" + "\n".join(ENTRIES) + "\n", 1)
    assert "\x1b" not in craw
    C.write_bytes(craw.encode("utf-8"))
    print("UNCERTAINTIES: U-9703 / U-9704 / U-9705 narrowed, U-9710 / U-9711 filed; CHANGELOG +", len(ENTRIES))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
