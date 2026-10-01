# Evaluate the PRE-REGISTERED rules in verify/sea_fix_20260930/PREREG.md.
# Thresholds are the ones fixed in PREREG.md before the source edit; they are
# hard-coded here rather than recomputed, so a later re-run cannot drift them.
#
#   py -3.12 verify/sea_fix_20260930/check_rules.py 1     # instance list
#   py -3.12 verify/sea_fix_20260930/check_rules.py 2a    # texture-detail boxes
#   py -3.12 verify/sea_fix_20260930/check_rules.py 3     # other-12-tracks diff
#   py -3.12 verify/sea_fix_20260930/check_rules.py 4     # car/prop controls
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from PIL import Image
import boxstats

ROOT = Path(__file__).resolve().parent.parent.parent
D = Path(__file__).resolve().parent

# Rule 2a, PREREG.md: (pose, box) -> (min post g, min post g/L)
R2A = {
    ("s8", "ROAD_UC"): (1.3590, 0.03138),
    ("s8", "ROAD_MC"): (0.5481, 0.05700),
    ("s8", "ROAD_LC"): (0.2841, 0.07462),
    ("s14", "ROAD_FG"): (0.3993, 0.04612),
    ("s14", "ROAD_MID"): (1.2552, 0.03736),
    ("s14", "ROAD_R"): (0.5292, 0.06428),
}
# Rule 1: the original's grid, from SEA_LEVEL_2026-09-29.md 4.3.
GRID = sorted((-150.0 + 60.0 * ix, -4.1, -150.0 + 60.0 * iz)
              for ix in range(5) for iz in range(5))

AREAS = ["Arctic", "Egypt", "City", "Forest", "Highway", "Neustein", "Storm",
         "SuperG", "Warzone", "rouabout", "sands", "dump", "training"]


def shot(base, idx):
    return D / base / f"{idx:02d}_{AREAS[idx]}" / "race1" / "01_grid.bmp"


def logf(base, idx):
    return D / base / f"{idx:02d}_{AREAS[idx]}" / "mashed_re.log"


def parse_sea_log(path):
    txt = Path(path).read_text(errors="replace")
    head = re.findall(r"SEA-TILE course_id=(\d+) clump=(\d+) dff=(\S+) "
                      r"instances=(\d+) grid=(\S+) origin=(\S+) step=(\S+) y=(\S+)", txt)
    inst = [(float(a), float(b), float(c)) for a, b, c in
            re.findall(r"SEA-TILE\[\d+\] pos=\(([-\d.]+), ([-\d.]+), ([-\d.]+)\)", txt)]
    off = re.findall(r"SEA-TILE-OFF course_id=(\d+) clump=(\d+) dff=(\S+) "
                     r"instances=1 \(identity\) sea_tile_instances=(\d+)", txt)
    return head, inst, off


def rule1():
    ok = True
    print("== Rule 1: exactly 25 sea instances at the original's positions ==")
    head, inst, off = parse_sea_log(logf("post_all", 0))
    print(f"Arctic (MASHED_TRACK_SEL=0)  header lines: {head}")
    print(f"  logged instances: n={len(inst)}")
    for i, p in enumerate(inst):
        print(f"    [{i:02d}] ({p[0]:.6f}, {p[1]:.6f}, {p[2]:.6f})")
    c1 = len(inst) == 25
    c2 = sorted(inst) == [(round(a, 6), round(b, 6), round(c, 6)) for a, b, c in GRID]
    xs = sorted({round(p[0], 3) for p in inst})
    zs = sorted({round(p[2], 3) for p in inst})
    ys = sorted({round(p[1], 4) for p in inst})
    print(f"  X set {xs}\n  Z set {zs}\n  Y set {ys}")
    print(f"  (1) count==25          : {'PASS' if c1 else 'FAIL'}")
    print(f"  (2) set==original grid : {'PASS' if c2 else 'FAIL'}")
    print(f"  (3) all 25 listed above: {'PASS' if len(inst) == 25 else 'FAIL'}")
    # PREREG.md rule 1(4), verbatim: "on a non-Arctic track
    # (MASHED_TRACK_SEL=12, training) the branch does not fire: 0 sea-tile
    # instances logged." That is the criterion -- count only.
    h2, i2, o2 = parse_sea_log(logf("post_all", 12))
    c4 = len(i2) == 0 and len(h2) == 0
    print(f"  training (SEL=12) sea-tile header lines={len(h2)} instances={len(i2)}")
    print(f"  (4) branch off non-Arctic: {'PASS' if c4 else 'FAIL'}")
    # Supporting, NOT part of the criterion: training's absent SEA-TILE-OFF line
    # is a silent negative (its COURSE.LUA declares no Clump_Filename at all, so
    # the loop body never runs). The OFF control is therefore demonstrated LIVE
    # on the other non-Arctic tracks that do declare a clump 2
    # (memory `absent-log-proves-nothing-run-a-control`).
    print("  [supporting, not gating] SEA-TILE-OFF control across all 13 tracks:")
    for i in range(13):
        p = logf("post_all", i)
        if not p.exists():
            continue
        h, inst, off = parse_sea_log(p)
        print(f"    {i:2d} {AREAS[i]:<9} sea_instances={len(inst):<3} "
              f"SEA-TILE-OFF lines={len(off)}")
    ok = c1 and c2 and c4
    print(f"RULE 1: {'PASS' if ok else 'FAIL'}")
    return ok


def rule2a():
    print("== Rule 2a: texture-detail recovery in the 6 gating ROAD boxes ==")
    refs = {"s8": ROOT / "verify/arctic_ref/sea_search/s8/a.bmp",
            "s14": ROOT / "verify/arctic_ref/sea_search/s14/a.bmp"}
    ok = True
    for pose in ("s8", "s14"):
        Yo = boxstats.luma(Image.open(refs[pose]))
        Yp = boxstats.luma(Image.open(D / f"pre_{pose}/00_Arctic/race1/01_grid.bmp"))
        Yq = boxstats.luma(Image.open(D / f"post_{pose}/00_Arctic/race1/01_grid.bmp"))
        A = Image.open(D / f"pre_{pose}/00_Arctic/race1/01_grid.bmp").convert("RGB").load()
        B = Image.open(D / f"post_{pose}/00_Arctic/race1/01_grid.bmp").convert("RGB").load()
        for name, box in boxstats.BOXES[pose].items():
            if not name.startswith("ROAD"):
                continue
            so = boxstats.box_stats(Yo, box)
            sp = boxstats.box_stats(Yp, box)
            sq = boxstats.box_stats(Yq, box)
            x0, y0, x1, y1 = box
            over = sum(1 for y in range(y0, y1) for x in range(x0, x1)
                       if max(abs(A[x, y][c] - B[x, y][c]) for c in range(3)) > 16)
            npx = (x1 - x0) * (y1 - y0)
            gate = R2A.get((pose, name))
            tag = "NON-GATING" if (pose, name) in boxstats.NON_GATING else ""
            v = ""
            if gate:
                c_a = sq["g"] >= gate[0]
                c_b = sq["g_over_L"] >= gate[1]
                v = "PASS" if (c_a and c_b) else "FAIL"
                ok = ok and c_a and c_b
                tag = (f"need g>={gate[0]:.4f} ({'ok' if c_a else 'NO'}) "
                       f"g/L>={gate[1]:.5f} ({'ok' if c_b else 'NO'})")
            print(f"{pose:>4} {name:<9} n={so['n']:>6}  "
                  f"orig g={so['g']:.4f} g/L={so['g_over_L']:.5f} | "
                  f"pre g={sp['g']:.4f} g/L={sp['g_over_L']:.5f} | "
                  f"post g={sq['g']:.4f} g/L={sq['g_over_L']:.5f} L={sq['L']:.2f} | "
                  f"pre->post diff {over}/{npx} ({100.0*over/npx:.1f}%)  {tag} {v}")
    print(f"RULE 2a: {'PASS' if ok else 'FAIL'}")
    return ok


def framediff(pa, pb):
    A = Image.open(pa).convert("RGB")
    B = Image.open(pb).convert("RGB")
    a, b = A.load(), B.load()
    w, h = A.size
    over = 0
    tot = 0
    for y in range(h):
        for x in range(w):
            d = [abs(a[x, y][c] - b[x, y][c]) for c in range(3)]
            tot += sum(d)
            if max(d) > 16:
                over += 1
    return over, w * h, tot / (w * h * 3.0)


def rule3():
    print("== Rule 3: the tiling is Arctic-only ==")
    print("-- prerequisite determinism control (pre-fix vs pre-fix repeat) --")
    ctl = None
    for i in (12, 0):
        p1, p2 = shot("pre_all", i), shot("pre_ctl", i)
        if p1.exists() and p2.exists():
            o, n, m = framediff(p1, p2)
            print(f"  ctrl track {i:2d} {AREAS[i]:<9} over16={o}/{n}  mean={m:.3f}  "
                  f"{'PASS' if o == 0 and m == 0.0 else 'FAIL'}")
            ctl = (o == 0 and m == 0.0) if ctl is None else (ctl and o == 0 and m == 0.0)
    if not ctl:
        print("RULE 3: VOID -- determinism control did not pass")
        return False
    ok = True
    for i in range(1, 13):
        o, n, m = framediff(shot("pre_all", i), shot("post_all", i))
        v = (o == 0 and m == 0.0)
        ok = ok and v
        print(f"  track {i:2d} {AREAS[i]:<9} over16={o}/{n}  mean={m:.3f}  "
              f"{'PASS' if v else 'FAIL'}")
    print(f"RULE 3: {'PASS' if ok else 'FAIL'}")
    return ok


def rule4():
    print("== Rule 4: Arctic cars and props unchanged ==")
    ok = True
    for pose, names in (("s8", ("CAR_A", "CAR_B", "CAR_C")),
                        ("s14", ("CTRL_BLDG", "CTRL_PIPE"))):
        A = Image.open(D / f"pre_{pose}/00_Arctic/race1/01_grid.bmp").convert("RGB").load()
        B = Image.open(D / f"post_{pose}/00_Arctic/race1/01_grid.bmp").convert("RGB").load()
        for nm in names:
            x0, y0, x1, y1 = boxstats.BOXES[pose][nm]
            over = sum(1 for y in range(y0, y1) for x in range(x0, x1)
                       if max(abs(A[x, y][c] - B[x, y][c]) for c in range(3)) > 16)
            npx = (x1 - x0) * (y1 - y0)
            ok = ok and over == 0
            print(f"  {pose:>4} {nm:<10} n={npx:>6} over16={over}  "
                  f"{'PASS' if over == 0 else 'FAIL'}")
    print("-- gate check: post-fix MASHED_NO_SEA_TILE=1 vs pre-fix, whole frame --")
    g = True
    for pose in ("s8", "s14"):
        p = D / f"post_nosea_{pose}/00_Arctic/race1/01_grid.bmp"
        if not p.exists():
            print(f"  {pose}: MISSING {p}")
            g = False
            continue
        o, n, m = framediff(D / f"pre_{pose}/00_Arctic/race1/01_grid.bmp", p)
        v = (o == 0 and m == 0.0)
        g = g and v
        print(f"  {pose} over16={o}/{n} mean={m:.3f}  {'PASS' if v else 'FAIL'}")
    ok = ok and g
    print(f"RULE 4: {'PASS' if ok else 'FAIL'}")
    return ok


if __name__ == "__main__":
    fns = {"1": rule1, "2a": rule2a, "3": rule3, "4": rule4}
    sel = sys.argv[1:] or list(fns)
    res = {k: fns[k]() for k in sel}
    print("\n=== summary ===")
    for k, v in res.items():
        print(f"rule {k}: {'PASS' if v else 'FAIL'}")
