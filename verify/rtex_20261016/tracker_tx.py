"""rtex tracker transaction (re-classify skill; no C-level change). Run once from the worktree root:

    py -3.12 verify/rtex_20261016/tracker_tx.py

- UNCERTAINTIES.md: U-9692 / U-9693 narrowed, U-9079 resolved, U-9697 filed after U-9696.
- hooks.csv (cp1252): file column of 00404e00 / 00404e20 / 0041f360 -> the shared TU; notes of
  those and of 00404830 extended.
- re/analysis/CHANGELOG.md: entries inserted below the exact `<!-- ENTRIES -->` line.
Every replacement asserts it matched exactly once.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DATE = "2026-10-16"
R = "verify/rtex_20261016/RESULT_rtex.md"


def sub_row(text, rid, new_row):
    pat = re.compile(r"(?m)^\| " + re.escape(rid) + r" \|[^\r\n]*")
    hits = pat.findall(text)
    assert len(hits) == 1, (rid, len(hits))
    return pat.sub(lambda m: new_row, text, count=1)


# ---- UNCERTAINTIES.md --------------------------------------------------------------------
up = ROOT / "UNCERTAINTIES.md"
u = open(up, encoding="utf-8", newline="").read()

u = sub_row(u, "U-9692",
    "| U-9692 | structural **NARROWED 2026-10-16 (rtex): TRAINING half ported, default ON; Arctic half open** | "
    "Arctic `WetRoad` (and `Ic?` materials): game render callback `FUN_0049a750` @ `0x0049a750` | "
    "TRAINING: the two-stage meshes are RenderWare MatFX DUAL (ROAD.DFF materials and GRAPH.BSP material 0, "
    "src 5 / dst 6, `newShadow` / `newShadow_Small`, two UV sets), drawn by `FUN_005405c0` @ `0x005405c0`; "
    "ported (librw P9 `matfxRender_Dual`, `MASHED_LIBRW_DUAL=0` opt-out); the 15 C16 rows are gone, 0 new rows. "
    "Arctic: `FUN_0049a750` walks the sector's instanced meshes and, for texture names `W?t` / `Ic?` "
    "(`bl = 0x65` at `0x0049a8c8`), binds vertex shader `[0x00773974]` / pixel shader `[0x0077397c]` "
    "(created at `0x0049aae0` from bytecode `0x005d0438` / `0x005d0518`) and stage-1 texture `[0x00773978]` "
    "(setter `0x0049a740`) with TTFF PROJECTED and CLAMP; not ported. `" + R + "`. | "
    "The two shaders' code and constants (`0x00497060`, `FUN_004cbb00` uploads at `0x0049a828..0x0049a866`) and "
    "the source of the 128x128 X8R8G8B8 texture at `[0x00773978]`. | "
    "Disassemble the vs/ps bytecode (D3DDisassemble), decompile `0x00497060` and the callers of `0x0049a740`. | "
    "none (C16 Arctic WetRoad, 3 rows) |")

u = sub_row(u, "U-9693",
    "| U-9693 | structural **NARROWED 2026-10-16 (rtex): dirt pass ported default ON; env pass ported OPT-IN** | "
    "car MatFX: dirt (DUAL, `FUN_0040cea0` -> `FUN_0041ecc0`) and env-map gloss (`FUN_00420420`, "
    "`FUN_00420340` -> `FUN_00420290`, render `FUN_005412d0`) | "
    "Dirt: dirt.piz PNGs named by `FUN_00404e00` / `FUN_00404e20` over the `FUN_00404830` tables, bound by part code "
    "(parts 0..3 / 0x21..0x24 wheel); ported, `MASHED_CAR_DIRT=0` opt-out; the C17 dirt TEX1 row is gone on 8/8 "
    "frames that had it, 0 new rows. Env: ported (`MASHED_CAR_ENV=1`) and its target rows go, but it fails "
    "PREREG_rtex rules 2 and 6: the standalone draws every car LOD (C02), so the second pass also runs on the "
    "duplicate glass atomics (uncited EXTRA CarGloss) and C21 ORDER rows appear; Arctic 1051 pixels +0.60. `" + R + "`. | "
    "Per-part car visibility (`FUN_00420870`'s `FUN_004b5240` render flags by part code and record +0x294 / "
    "+0x298) is not ported; the env pass's residual inputs are U-9697. | "
    "Port `FUN_00420870`'s part visibility switch (part codes are now parsed, see U-9079), then re-score F-ENV. | "
    "none (C17 env TEX1 + MISSING CarGloss stay cited) |")

u = sub_row(u, "U-9079",
    "| U-9079 | ~~structural~~ **RESOLVED 2026-10-16 (rtex)**: the slot is DFF data, nothing writes it at runtime | "
    "atomic->slot index source for the standings widget -- Finding 20; `FUN_004b5190` @`0x004b5190` | "
    "`FUN_004b5190` reads `*(atomic+0x18)`, which is the GEOMETRY (`RpAtomic` +0x18; `FUN_00420420` uses the same "
    "pointer as the geometry), not the frame: `FUN_00543d70(geo, 0)` returns user-data array 0 of the plugin block at "
    "`geo + DAT_007dc634` (stride 0x10) and `FUN_00543df0(arr, 0)` its element 0 (data at +0xc). Every vehicle and "
    "ENDPOINTPANEL.DFF geometry carries rpUSERDATA (chunk 0x11F) `0.tv_part_id` (int); the frame / atomic extensions "
    "the old row checked are empty, the geometry's are not. Live witness: 71/71 car record slots (`camera_probe.py "
    "--matfx`, TRAINING) hold the atomic whose geometry's `tv_part_id` equals the slot; the 7 flag mismatches are "
    "`FUN_00420420`'s own `FUN_004b52c0` edits. `verify/rtex_20261016/evidence/partid_check_training.txt`, "
    "`userdata_endpointpanel.txt`. | -- | -- | nothing (parser: `Track/DffModel.cpp` `DffBatch::part_id`) |")

new_row = (
    "| U-9697 | semantic | rtex residuals (`" + R + "`): car dirt / env passes and the MatFX env renderer port | "
    "(a) the dirt texture's 8->4 bit rule (truncation is used) and mip chain (none; cFormat 0x3 read live has no mip "
    "bit); (b) `FUN_005412d0`'s env-raster-with-alpha branch under `DAT_007dc600` (`0x005413da..`) is not ported "
    "(two-pass is drawn); (c) the shade sample `FUN_00479100` runs on the flat collision soup with Moller-Trumbore "
    "barycentrics instead of `FUN_00538c80`'s BSP walk and `FUN_00559040`'s weight matrix; (d) `DAT_007f101c` "
    "(single increment `0x004111c1`, zeroed `0x0040ff17`) is stood in by +1 per rendered race frame; (e) the env "
    "frame angle uses the standalone's car velocity, whose units against the original's +0x9b0 are not checked; "
    "(f) `DAT_00636ad0` / `DAT_00636ad4` are held at 0, the value read live, their writers `FUN_00401000` / "
    "`FUN_0040cda0` / `FUN_00499400` / `FUN_00479330` / `FUN_0049a0e0` are not ported. | "
    "Original-side readback of the dirt raster texels and of the window / angle per frame; the decomp of the "
    "named writers. | `camera_probe.py --matfx` already reads the window and angle; add a texel readback, "
    "decompile the writers. | none (pixels only; no draw-list row depends on them) |")
idx = u.index("| U-9696 |")
line_end = u.index("\n", idx)
eol = "\r\n" if u[line_end - 1] == "\r" else "\n"
u = u[:line_end + 1] + new_row + eol + u[line_end + 1:]
assert u.count("| U-9697 |") == 1
open(up, "w", encoding="utf-8", newline="").write(u)

# ---- hooks.csv -------------------------------------------------------------------------
hp = ROOT / "hooks.csv"
h = open(hp, encoding="cp1252", newline="").read()
NEWF = "mashedmod/src/mashed_re/Vehicle/CarMaterialTables.cpp"
for rva, old in (("00404e00", "Util/PromoLoop_round74.cpp"), ("00404e20", "Util/PromoLoop_round74.cpp"),
                 ("0041f360", "Util/PromoLoop_round66.cpp")):
    pat = re.compile(r"(?m)^(" + rva + r",[^\r\n]*?)mashedmod/src/mashed_re/" + re.escape(old) + r"([^\r\n]*)")
    assert len(pat.findall(h)) == 1, rva
    h = pat.sub(lambda m: m.group(1) + NEWF + m.group(2), h, count=1)
    # notes: append to the last column (rows may end with a quoted notes cell)
    rowpat = re.compile(r"(?m)^(" + rva + r",[^\r\n]*)")
    row = rowpat.search(h).group(1)
    add = (" | rtex " + DATE + ": body moved unchanged for the .asi to Vehicle/CarMaterialTables.cpp, compiled by "
           "both targets (MASHED_STANDALONE resolves the MASHED tables); the standalone car set-up calls it")
    if row.endswith('",'):
        nrow = row[:-2] + add + '",'
    elif row.endswith('"'):
        nrow = row[:-1] + add + '"'
    elif row.endswith(","):
        nrow = row[:-1] + add + ","
    else:
        nrow = row + add
    h = h.replace(row, nrow, 1)
row = re.search(r"(?m)^(00404830,[^\r\n]*)", h).group(1)
add = (" | rtex " + DATE + ": standalone data transcription of its stores (generated from the decomp, "
       "Vehicle/CarMaterialTables_dirt.inc, MASHED_STANDALONE only; not hooked, C-level unchanged)")
h = h.replace(row, (row[:-1] + add + ",") if row.endswith(",") else row + add, 1)
open(hp, "w", encoding="cp1252", newline="").write(h)

# ---- CHANGELOG -------------------------------------------------------------------------
cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
c = open(cp, encoding="utf-8", newline="").read()
lines = c.split("\n")
k = [i for i, ln in enumerate(lines) if ln.strip() == "<!-- ENTRIES -->"]
assert len(k) == 1
entries = [
    f"{DATE}  U-9697  filed  rtex residual uncertainties of the car dirt / env passes (pixels only)  {R}",
    f"{DATE}  U-9079  RESOLVED  part code = geometry rpUSERDATA 0.tv_part_id (FUN_004b5190 reads atomic+0x18 = geometry; FUN_00543d70 / FUN_00543df0); 71/71 live slots  verify/rtex_20261016/evidence/partid_check_training.txt",
    f"{DATE}  U-9693  NARROWED  car dirt (MatFX dual) ported default ON; env pass ported opt-in (fails rules 2/6 until car LOD selection, C02)  {R}",
    f"{DATE}  U-9692  NARROWED  TRAINING two-stage = MatFX DUAL, ported default ON (15 C16 rows gone, 0 new); Arctic FUN_0049a750 shader path open  {R}",
    f"{DATE}  00404e00/00404e20/0041f360  C3=C3  file moved to Vehicle/CarMaterialTables.cpp (shared by both targets; .asi code unchanged)  {R}",
]
lines[k[0] + 1:k[0] + 1] = entries
open(cp, "w", encoding="utf-8", newline="").write("\n".join(lines))
print("tracker transaction applied")
