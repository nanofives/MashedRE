# Asset-side GROUND TRUTH for CAR_GRAY_CHASSIS acceptance G1.
#
# Independently of the C++ loader, parse every vehicle DFF and split its
# atomics by the SAME predicate the fix uses -- `(geometry.flags & 0x84) == 0`,
# i.e. neither rpGEOMETRYTEXTURED (0x04) nor rpGEOMETRYTEXTURED2 (0x80).
# Reports, per DFF: total atomics, the kept (textured) count, the dropped
# (untextured) count, and the dropped set split into multi-triangle HULLS and
# one-triangle LOCATORS.
#
# G1 is scored by comparing these numbers against what the running standalone
# logs at load time ("R5 car atomics:" / "R6 livery atomics:"). If the binary's
# dropped count differs from this table for any DFF, G1 fails.
#
# Uses re/tools/dff_dump.py's parser (the same one that produced the note's M2
# and M4 tables) so the census is a re-run, not a second implementation.
#
# Usage: py -3.12 verify/car_gray_fix_20260930/dff_atomic_census.py [--liveries]
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "tools"))

import dff_dump                                        # noqa: E402
import piz_extract                                     # noqa: E402

VEH = ROOT / "original" / "TOASTART" / "VEHICLES"

CARS = [
    ("Advantag.piz", "ADVANTAGE"), ("Atmos.piz", "ATMOS"),
    ("BamBam.piz", "BAMBAM"),      ("Bullet.piz", "BULLET"),
    ("Cooler.piz", "COOLER"),      ("Creeper.piz", "CREEPER"),
    ("Crusader.piz", "CRUSADER"),  ("Formula.piz", "FORMULA"),
    ("Kustom.piz", "KUSTOM"),      ("Shorty.piz", "SHORTY"),
    ("Shuriken.piz", "SHURIKEN"),  ("Sputter.piz", "SPUTTER"),
    ("Stallion.piz", "STALLION"),
]


def census(blob):
    """-> (n_atomics, kept, dropped, hulls, locators, tri_histogram)

    HULL vs LOCATOR is split on the triangle count, and the cut is read off
    the data rather than assumed: across all 13 vehicles the untextured
    atomics fall into two disjoint bands, 1-2 triangles (the corner/roof
    locator quads) and >=28 triangles (the four car-sized shells). Nothing
    lands between 3 and 27, so the cut at 10 is unambiguous. The histogram is
    printed alongside so the gap is visible instead of asserted.
    """
    _frames, geos, atomics = dff_dump.parse_clump(blob)
    kept = dropped = hulls = locators = 0
    hist = {}
    for _fi, gi in atomics:
        g = geos[gi]
        if (g["flags"] & 0x84) == 0:
            dropped += 1
            hist[g["ntris"]] = hist.get(g["ntris"], 0) + 1
            if g["ntris"] >= 10:
                hulls += 1
            else:
                locators += 1
        else:
            kept += 1
    return len(atomics), kept, dropped, hulls, locators, hist


def main():
    liveries = "--liveries" in sys.argv
    sfx = [0, 1, 2, 3] if liveries else [0]
    print(f"{'piz':<14}{'dff':<18}{'atoms':>6}{'kept':>6}{'drop':>6}"
          f"{'hull':>6}{'loc':>6}")
    for pizname, base in CARS:
        data, _v, _a2, entries, _m = piz_extract.read_archive(VEH / pizname)
        names = {n.upper(): (o, ln) for (n, o, ln, _i) in entries}
        for s in sfx:
            want = f"{base}{s}.DFF"
            e = names.get(want)
            if e is None:
                continue
            n, k, d, h, l, hist = census(data[e[0]:e[0] + e[1]])
            hs = " ".join(f"{t}x{c}" for t, c in sorted(hist.items()))
            print(f"{pizname:<14}{want:<18}{n:>6}{k:>6}{d:>6}{h:>6}{l:>6}"
                  f"   tris[{hs}]")
    return 0


if __name__ == "__main__":
    sys.exit(main())
