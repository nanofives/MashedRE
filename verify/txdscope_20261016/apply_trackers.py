"""txdscope: the tracker transaction (re-classify rules: UNCERTAINTIES.md row, hooks.csv notes, CHANGELOG entries below the marker).

    py -3.12 verify\\txdscope_20261016\\apply_trackers.py [--dry]

Byte-level edits (UNCERTAINTIES.md and hooks.csv are CRLF, CHANGELOG.md is LF, each carries one NUL byte that must survive). Idempotent: a second
run finds its own marker and changes nothing. No confidence level moves: the hooks.csv edits are notes only (name corrections / roles).
"""
import csv
import io
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
DRY = "--dry" in sys.argv
TAG = "[txdscope 2026-10-16]"

# ---------------------------------------------------------------- UNCERTAINTIES.md
U_PATH = WT / "UNCERTAINTIES.md"
u = U_PATH.read_bytes()
if b"txdscope: lookup scoped" in u:
    print("UNCERTAINTIES.md already applied")
else:
    lines = u.split(b"\r\n")
    i24 = next(i for i, l in enumerate(lines) if l.startswith(b"| U-9724 |"))
    old = lines[i24].decode("utf-8")
    where = old.split(" | ")[2]
    resolved = (
        "| U-9724 | ~~structural~~ **RESOLVED 2026-10-16** (txdscope: lookup scoped to the named dictionary) | " + where +
        " | Decoded and ported, default ON (`MASHED_NO_TXDSCOPE=1` opts out). The original binds ONE dictionary for the whole track load: "
        "`Texture_Dictionary_Filename` (registered `0x00440c2e`, handler `0x0047a6f0`) is loaded by `FUN_0042a6b0` (`0x00479372`) and made current by "
        "`FUN_004c5c80` (`0x004793de`) before the world BSP, clumps, sky domes and Mts are read; `FUN_004c5cb0` (RwTextureRead) walks the current dictionary only "
        "(default find callback `0x004c7490`), then the read callback `0x004c61b0`, and returns 0 on a miss; no game code installs another callback. The standalone "
        "decodes only the named TXD of the track piz (`TrackRenderer::Load`), the car's own PC TXD (`RegisterCarModel`) and `FX.TXD` for the FX names (`SfxTexture`). "
        "Only Dump has a second TXD; `trainroof` (TRAIN.DFF) is the one name that resolved outside `dump.txd` and is now untextured as in the original; 36 of 39 draw files "
        "of the 13 tracks are byte-identical to the base, rows 1047 / 0 RED before and after, G-CTRL `d3b90ee0` / `80f01f35` / `56d62ae7` unchanged "
        "(`verify/txdscope_20261016/RESULT_txdscope.md`). Cite C30 retired, C30-TRAIN-ROOF-LEAD added (the roof now pairs and shows the train's U-9725 lead). | none | none | none |"
    )
    lines[i24] = resolved.encode("utf-8")
    new_rows = [
        "| U-9731 | semantic | `0x004c61b0`, the default RwTextureRead read callback (installed at `0x004c60e3`), reached through `FUN_004c6ba0` / `FUN_004c6750` | "
        "Not decoded in the txdscope lane: which file name or path it builds for a texture the current dictionary does not hold. For the 13 tracks it cannot supply a texture: "
        "no track piz holds a `.png` / `.bmp` / `.dds` / `.ico` entry (image entries exist only in Font36, Perm, Powerups, SFX, dirt), and Dump's `trainroof` is drawn untextured by the original. | "
        "the name -> path rule of `FUN_004c6750` / `FUN_004ce6c0` / `FUN_004ce970`. | decode those three; list which loose images the original can resolve through it. | none beyond the row |",
        "| U-9730 | structural | `FUN_00420420` (`0x004204c0` loads, `0x004204ce` makes current), `FUN_0041ec00` (frees), standalone `TrackRenderer::RegisterCarModel` | "
        "The original loads the vehicle dictionary only when `DAT_0063d7e8 == 0` and frees it when the live-vehicle counter `DAT_0063e498` is 1, so every car of a race is read against the "
        "dictionary of the FIRST car instantiated. Each car's own TXD holds `Glass`, `CarGloss`, `<Car>Small`, `<Car>`, so a second, different model would resolve only `Glass` / `CarGloss`. "
        "The standalone registers the TXD of each model. [UNCERTAIN] whether the original ever instantiates different models in one race (`FUN_0041f330` returns the table entry per slot). | "
        "an original capture of a race with two different cars: `DAT_0063d7e8`'s texture list and the second car's body draw texture names. | take that capture, then bind cars 2..N to the first car's dictionary if the draws show it. | multi-car race rows |",
        "| U-9729 | semantic | `hooks.csv` rows `004c5800` / `004c5820` (C4, notes say RwTexDictionarySetCurrent / GetCurrent), `re/analysis/texture_loader_d3_cont1/0x004c5800.md`, `0x004c5820.md`, `0x004c5830.md`, `0x004c5850.md` | "
        "The slot these two functions address, `DAT_007d4054 + 0x1c + DAT_007d3ff8`, is read by `FUN_004c6ba0` only at `0x004c6cb3` (with slot +0x20 at `0x004c6cb9`) to choose the value it passes on as `0x4` / `0x8004` / `0x9004`, "
        "and `FUN_00550130` / `FUN_004cf5a0` save, set and restore it around a texture read. The dictionary `FUN_004c5cb0` (RwTextureRead) adds new textures to and `0x004c7490` walks is slot +0x10, written by "
        "`FUN_004c5c80` and read by `FUN_004c5ca0`: those two are the SetCurrent / GetCurrent pair. The C4 evidence of 5800 / 5820 is about their bodies and stands; only the names move. | "
        "what slots +0x1c / +0x20 hold, beyond the mechanical reads above. | decode the consumers of the value `FUN_004c6ba0` builds, then rename `0x004c5800` / `0x004c5820` and the plates; `0x004c5c80` / `0x004c5ca0` get the SetCurrent / GetCurrent names. | none beyond the row |",
    ]
    ih = next(i for i, l in enumerate(lines) if l.startswith(b"|----|"))
    for r in reversed(new_rows):
        lines.insert(ih + 1, r.encode("utf-8"))
    out = b"\r\n".join(lines)
    if not DRY:
        U_PATH.write_bytes(out)
    print("UNCERTAINTIES.md: U-9724 resolved, U-9729 / U-9730 / U-9731 filed")

# ---------------------------------------------------------------- hooks.csv (notes only)
H_PATH = WT / "hooks.csv"
h = H_PATH.read_bytes()
NOTES = {
    "004c5800": TAG + " NAME CORRECTION: slot +0x1c is read by FUN_004c6ba0 (0x004c6cb3) as the value it passes on as 0x4/0x8004/0x9004 and saved/cleared/restored by FUN_00550130 and FUN_004cf5a0 around a texture read; it is not the dictionary RwTextureRead walks (slot +0x10, FUN_004c5c80/FUN_004c5ca0). Body evidence stands; U-9729",
    "004c5820": TAG + " NAME CORRECTION: reads slot +0x1c, the format-flag word (see 004c5800), not the current dictionary (FUN_004c5ca0 reads slot +0x10). Body evidence stands; U-9729",
    "004c5c80": TAG + " this is the RwTexDictionarySetCurrent: slot +0x10 is the dictionary FUN_004c5cb0 (RwTextureRead) links new textures into and the default find callback 0x004c7490 walks; CreateFromDescription calls it at 0x004793de, FUN_00420420 at 0x004204ce; U-9724 resolved, U-9729",
    "004c5ca0": TAG + " this is the RwTexDictionaryGetCurrent (reads slot +0x10); U-9729",
    "004c5cb0": TAG + " RwTextureRead(name, mask): find callback [+0x18] (default 0x004c7490: current dictionary only, case-insensitive) then read callback [+0x14] (default 0x004c61b0); a miss returns 0 (0x004c5d09 / 0x004c5d2f); a read result is linked into the slot +0x10 dictionary (0x004c5d4d); the only stores into slots +0x10/+0x14/+0x18 in the image are 0x004c5c90, 0x004c6057, 0x004c60d0, 0x004c60e3; U-9724 resolved, U-9731",
    "0047a6f0": TAG + " string is copied to the course description base +0x0; CreateFromDescription loads it with FUN_0042a6b0 (0x00479372) and makes it current (0x004793de); standalone parse_texdic_filename + TrackRenderer::Load scope; U-9724 resolved",
    "004cf5a0": TAG + " reads the name and mask strings (FUN_004d8810) and calls FUN_004c5cb0; returns 0 when that returns 0 (the material keeps no texture); U-9724",
}
if TAG.encode() in h:
    print("hooks.csv already applied")
else:
    lines = h.split(b"\r\n")
    done = []
    for i, l in enumerate(lines):
        key = l[:8].decode("latin1").lower()
        if key[:8] in NOTES and l[8:9] == b",":
            row = next(csv.reader(io.StringIO(l.decode("utf-8"))))
            buf = io.StringIO()
            csv.writer(buf, lineterminator="").writerow(row)
            if buf.getvalue().encode("utf-8") != l:
                print("ABORT: csv round-trip differs for", key)
                sys.exit(1)
            row[8] = (row[8] + " | " if row[8] else "") + NOTES[key[:8]]
            buf = io.StringIO()
            csv.writer(buf, lineterminator="").writerow(row)
            lines[i] = buf.getvalue().encode("utf-8")
            done.append(key)
    missing = set(NOTES) - set(done)
    if missing:
        print("ABORT: rows not found", missing)
        sys.exit(1)
    if not DRY:
        H_PATH.write_bytes(b"\r\n".join(lines))
    print("hooks.csv notes:", sorted(done))

# ---------------------------------------------------------------- CHANGELOG.md
C_PATH = WT / "re/analysis/CHANGELOG.md"
c = C_PATH.read_bytes()
if b"txdscope (work/txdscope)" in c:
    print("CHANGELOG already applied")
else:
    entries = [
        "2026-10-16  txdscope (work/txdscope): U-9724 RESOLVED. Texture lookup is now the original's: the dictionary Texture_Dictionary_Filename names for the track (0x0047a6f0 -> FUN_00479330 0x00479372 / 0x004793de), the car's own PC TXD (FUN_00420420 0x004204c0), FX.TXD for the FX names (FUN_0040bb30); FUN_004c5cb0 walks the current dictionary only. Default ON, MASHED_NO_TXDSCOPE=1 opts out. 13 tracks x 3 frames 1047 rows / 0 RED unchanged, 36 of 39 draw files byte-identical, Dump roof trainroof -> untextured, cite C30 retired + C30-TRAIN-ROOF-LEAD, G-CTRL d3b90ee0 / 80f01f35 / 56d62ae7 unchanged  verify/txdscope_20261016/RESULT_txdscope.md",
        "2026-10-16  U-9729  FILED  hooks.csv names of 004c5800 / 004c5820 (RwTexDictionarySetCurrent / GetCurrent) do not match the bytes: slot +0x1c is a format-flag word (FUN_004c6ba0 0x004c6cb3); the dictionary pair is 004c5c80 / 004c5ca0 (slot +0x10). C4 body evidence stands, notes corrected, no level moved  verify/txdscope_20261016/PREREG_txdscope.md",
        "2026-10-16  U-9730  FILED  the original keeps ONE vehicle dictionary per race (DAT_0063d7e8, FUN_00420420 / FUN_0041ec00); the standalone registers one per model; needs a multi-car original capture  verify/txdscope_20261016/RESULT_txdscope.md",
        "2026-10-16  U-9731  FILED  default RwTextureRead read callback 0x004c61b0 not decoded; moot for the 13 tracks (no image entries in any track piz)  verify/txdscope_20261016/RESULT_txdscope.md",
    ]
    lines = c.split(b"\n")
    im = next(i for i, l in enumerate(lines) if l.strip() == b"<!-- ENTRIES -->")
    for e in reversed(entries):
        lines.insert(im + 1, e.encode("utf-8"))
    if not DRY:
        C_PATH.write_bytes(b"\n".join(lines))
    print("CHANGELOG.md: 4 entries prepended below the marker")
