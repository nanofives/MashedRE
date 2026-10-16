"""carlod tracker transaction (re-classify skill). Run once from the worktree root:

    py -3.12 verify/carlod_20261016/tracker_tx.py

- UNCERTAINTIES.md: U-9693 narrowed (both car passes default ON), U-9079 consumer ported, U-9697 (c) / (g)
  annotated, U-9698 filed after U-9697.
- hooks.csv (cp1252): 004b5240 demoted C3 -> C2 (its body lacked the clear arm; re-diff needed), note added to
  the 00420870 row (standalone transcription, C-level unchanged).
- re/analysis/CHANGELOG.md: entries inserted below the exact `<!-- ENTRIES -->` line.
Every replacement asserts it matched exactly once.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DATE = "2026-10-16"
R = "verify/carlod_20261016/RESULT_carlod.md"


def sub_row(text, rid, new_row):
    pat = re.compile(r"(?m)^\| " + re.escape(rid) + r" \|[^\r\n]*")
    hits = pat.findall(text)
    assert len(hits) == 1, (rid, len(hits))
    return pat.sub(lambda m: new_row, text, count=1)


def replace_once(text, old, new):
    assert text.count(old) == 1, (old[:60], text.count(old))
    return text.replace(old, new, 1)


# ---- UNCERTAINTIES.md --------------------------------------------------------------------
up = ROOT / "UNCERTAINTIES.md"
u = open(up, encoding="utf-8", newline="").read()

u = sub_row(u, "U-9693",
    "| U-9693 | structural **NARROWED 2026-10-16 (rtex, then carlod): dirt pass and env pass ported, both default ON** | "
    "car MatFX: dirt (DUAL, `FUN_0040cea0` -> `FUN_0041ecc0`) and env-map gloss (`FUN_00420420`, "
    "`FUN_00420340` -> `FUN_00420290`, render `FUN_005412d0`) | "
    "Dirt (rtex): dirt.piz PNGs named by `FUN_00404e00` / `FUN_00404e20` over the `FUN_00404830` tables, bound by part "
    "code; `MASHED_CAR_DIRT=0` opt-out; 8/8 C17 dirt TEX1 rows gone. Env: ported by rtex, held opt-in because the "
    "standalone drew every car LOD. carlod ported the per-part visibility (`FUN_00420870`, U-9079) and the glass "
    "pass (`FUN_00420050`): with them the env pass passes PREREG_carlod rules 1-6 and is default ON "
    "(`MASHED_CAR_ENV=0` opt-out; 14 C17 rows gone on the 7 frames that had them, 7 STATE CarGloss rows replace "
    "the vanished MISSING rows, no other new row; pixels against the F-VIS arm +0.37 at most). The env "
    "coefficient is computed against the PREVIOUS row's camera (live value of 112 records reproduced exactly, "
    "`verify/carlod_20261016/evidence/env_lag_live.txt`), and Arctic's collision colours were unparsed (shade 1.0 "
    "against the original's ~0.25) until `Track/TrackWorld.cpp` read the world header's format flags. `" + R + "`. | "
    "The residual inputs of the env pass are U-9697; the visibility inputs are U-9698. | "
    "Nothing for the draw-list rows; see U-9697 / U-9698 for the pixel-level and input gaps. | "
    "none (C17 rows cited, none left on the 9 frames beyond the replaced STATE rows) |")

u = sub_row(u, "U-9079",
    "| U-9079 | ~~structural~~ **RESOLVED 2026-10-16 (rtex); consumer ported 2026-10-16 (carlod)**: the slot is DFF data, "
    "nothing writes it at runtime | "
    "atomic->slot index source for the standings widget -- Finding 20; `FUN_004b5190` @`0x004b5190` | "
    "`FUN_004b5190` reads `*(atomic+0x18)`, which is the GEOMETRY (`RpAtomic` +0x18; `FUN_00420420` uses the same "
    "pointer as the geometry), not the frame: `FUN_00543d70(geo, 0)` returns user-data array 0 of the plugin block at "
    "`geo + DAT_007dc634` (stride 0x10) and `FUN_00543df0(arr, 0)` its element 0 (data at +0xc). Every vehicle and "
    "ENDPOINTPANEL.DFF geometry carries rpUSERDATA (chunk 0x11F) `0.tv_part_id` (int); the frame / atomic extensions "
    "the old row checked are empty, the geometry's are not. Live witness: 71/71 car record slots (`camera_probe.py "
    "--matfx`, TRAINING) hold the atomic whose geometry's `tv_part_id` equals the slot; the 7 flag mismatches are "
    "`FUN_00420420`'s own `FUN_004b52c0` edits. `verify/rtex_20261016/evidence/partid_check_training.txt`, "
    "`userdata_endpointpanel.txt`. Consumer (carlod): `FUN_00420870` @ `0x00420870` sets each part atomic's "
    "rpATOMICRENDER bit through `FUN_004b5240` by part code from the record's state +0x298 and flags +0x294, and "
    "`FUN_00420050` @ `0x00420050` then renders the glass atomics (0xe/0xf at state 0, 0x2f/0x30 at state 1) through "
    "their own callbacks; ported (`LibRw/RwCarFx.cpp` `CarPartVisible` / `CarGlassPart`, default ON, "
    "`MASHED_CAR_VIS=0` opt-out), 112 live car records reproduced, the 27 C02 rows gone. `" + R + "`. | -- | -- | "
    "nothing (parser: `Track/DffModel.cpp` `DffBatch::part_id`) |")

u = replace_once(u, "(c) the shade sample `FUN_00479100` runs on",
                 "(c) [carlod: Arctic's COLLISIONS.BSP colours were unparsed, shade 1.0 against ~0.25, fixed in "
                 "`Track/TrackWorld.cpp`; the rest stands] the shade sample `FUN_00479100` runs on")
u = replace_once(u, "keep the single-stage draw (`evidence/matfx_census.txt`). |",
                 "keep the single-stage draw (`evidence/matfx_census.txt`); carlod: no cited frame contains an Atmos or "
                 "Storm car (`verify/carlod_20261016/evidence/car_textures.txt`), so (g) stays unported. |")

new_row = (
    "| U-9698 | semantic | standalone inputs of `FUN_00420870`'s per-part visibility: record state S (+0x298) and flags F "
    "(+0x294) (`LibRw/RwCarFx.cpp` `CarLodState` / `CarVisFlags`) | "
    "(a) F bits 0x10 / 0x20 (`FUN_0041ede0`) and 0x400 (`FUN_0041ef80`) are always 0: the standalone has no damage or "
    "wreck state, so the parts they select (4..0xd, 0x25..0x2e, 0x65..0x67, 0x17, 0x32) show only their undamaged "
    "variants; (b) F & 0x800 is the controller's brake byte `*(0x007f103c + ctrl*0x4c + 1)` (`FUN_0040da50` -> "
    "`FUN_0041eeb0`'s fourth argument): the default AI cars produce no such byte and the player's `io.input[5]` is not "
    "wired, so part 0x11 (the 4-triangle brake-light plate) is never lit in the default build; the matched-pose arms "
    "take it from `MASHED_CAR_BRAKE_ROWS`, derived from the original's own draw lists, which is circular for that part "
    "(`evidence/brake_from_draws.txt`: Arctic 1091 slot 3, 1126 slot 1, City 836 slots 1 and 2); (c) S = 0 for slot 0 "
    "when `FUN_0042f6a0` returns 2 or `DAT_007f0fd0` is 5, 9 or 10 (`0x004469aa`..`0x004469ae`) is not modelled; "
    "(d) the timing of S against the camera and the cached part-0x13 position (`FUN_00446520` runs its state loop "
    "before its own camera update; the cache is written by `FUN_00420870`): all 16 (camera row, car row) lags predict "
    "the 36 observed car-frames, so S uses this frame's values (`evidence/state_lag_test.txt`). | "
    "A live read of rec+0x298 / +0x294 per row on a braking and a damaged car; the call order of `FUN_0040da50` and "
    "`FUN_00446520` in the main loop. | `camera_probe.py --matfx` already reads the flags and the render bytes; add "
    "+0x298, run a braking / colliding scenario; decompile the main loop; wire the player's `[5]` byte "
    "(`TrackRenderer.cpp` physics step) and the AI tick's brake once u9683's tick order lands. | "
    "none (F-VIS default ON; part 0x11 and the damage variants are not in any cited row once the brake rows are supplied) |")
idx = u.index("| U-9697 |")
line_end = u.index("\n", idx)
eol = "\r\n" if u[line_end - 1] == "\r" else "\n"
u = u[:line_end + 1] + new_row + eol + u[line_end + 1:]
assert u.count("| U-9698 |") == 1
open(up, "w", encoding="utf-8", newline="").write(u)

# ---- hooks.csv -------------------------------------------------------------------------
hp = ROOT / "hooks.csv"
h = open(hp, encoding="cp1252", newline="").read()
m = re.search(r"(?m)^(004b5240,[^\r\n]*)", h)
assert m
row = m.group(1)
assert ",C3,impl," in row and "green-earlywindow-r115" in row
nrow = row.replace(",C3,impl,", ",C2,impl,", 1).replace("green-earlywindow-r115", "stale-r115-no-clear-arm", 1)
add = (" | carlod " + DATE + " C3->C2: Or4b5240 lacked the clear arm (`je 0x4b5256` -> `and cl,0xfb; mov [eax+2],cl`, "
       "evidence/dis_004b5240.txt); the r115 vector never had flag=0 with the bit set. Body fixed from the disassembly, "
       "needs a diff-original run with that vector before C3")
nrow = (nrow[:-2] + add + '",') if nrow.endswith('",') else ((nrow[:-1] + add + '"') if nrow.endswith('"') else (nrow + add))
h = h.replace(row, nrow, 1)
m = re.search(r"(?m)^(00420870,[^\r\n]*)", h)
assert m
row = m.group(1)
add = (" | carlod " + DATE + ": its visibility switch (0x00420a33..0x00420b16) is transcribed for the standalone "
       "(LibRw/RwCarFx.cpp CarPartVisible, handler addresses cited; 112 live car records reproduced) and FUN_00420050's "
       "glass pass after it (CarGlassPart); not hooked, C-level unchanged")
nrow = (row[:-2] + add + '",') if row.endswith('",') else ((row[:-1] + add + ",") if row.endswith(",") else row + add)
h = h.replace(row, nrow, 1)
open(hp, "w", encoding="cp1252", newline="").write(h)

# ---- CHANGELOG -------------------------------------------------------------------------
cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
c = open(cp, encoding="utf-8", newline="").read()
lines = c.split("\n")
k = [i for i, ln in enumerate(lines) if ln.strip() == "<!-- ENTRIES -->"]
assert len(k) == 1
entries = [
    f"{DATE}  U-9698  filed  standalone inputs of the car part visibility (damage / wreck / brake flags, slot-0 state modes, S timing)  {R}",
    f"{DATE}  U-9697  ANNOTATED  (c) Arctic collision colours parsed (TrackWorld header flags), (g) Atmos / Storm not in any cited frame  {R}",
    f"{DATE}  U-9693  NARROWED  car dirt and env passes both default ON; env coefficient uses the previous row's camera (112 live records exact); Arctic shade fixed  {R}",
    f"{DATE}  U-9079  CONSUMER PORTED  FUN_00420870 per-part RENDER bits by part code (112 live car records reproduced) + FUN_00420050 glass pass; 27 C02 rows gone  {R}",
    f"{DATE}  00420870  C2=C2  visibility switch transcribed for the standalone (LibRw/RwCarFx.cpp), glass pass of FUN_00420050 added; not hooked  {R}",
    f"{DATE}  004b5240  C3->C2  Or4b5240 lacked the clear arm (and cl,0xfb at 0x004b5256); body fixed, the r115 vector never had flag=0 with the bit set; needs a re-diff  verify/carlod_20261016/evidence/dis_004b5240.txt",
]
lines[k[0] + 1:k[0] + 1] = entries
open(cp, "w", encoding="utf-8", newline="").write("\n".join(lines))
print("tracker transaction applied")
