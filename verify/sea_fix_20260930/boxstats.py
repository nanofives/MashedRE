# Fixed-screen-box statistics for the Arctic sea-tile acceptance.
#
# WHY NOT A COLOUR CLASS. Two earlier children of this lane lost an acceptance
# rule by declaring the measured region with a colour predicate: Arctic's tinted
# night lighting moves every achromatic/luma threshold, so the "sea class" and
# the "car class" stopped selecting the same surfaces between arms and the rule
# became unfalsifiable (memory `achromatic-selector-breaks-on-tinted-light`).
# Here every region is a FIXED PIXEL RECTANGLE in the 640x480 backbuffer, chosen
# once, before the fix exists, and identical for the original arm, the pre-fix
# arm and the post-fix arm.
#
# The discriminating statistic is TEXTURE DETAIL, not colour:
#   g    = mean |Sobel gradient| of luma over the box  (road is textured: snow
#          streaks, painted lines, falling snow. A flat water sheet is smooth.)
#   L    = mean luma over the box
#   g/L  = relative gradient -- g scales with brightness, so the ratio is the
#          brightness-invariant form. Arctic's two arms differ in overall
#          exposure, so g/L is the primary statistic and g is reported beside it.
#   n    = pixel count in the box (so every figure carries its n).
#
# Usage:
#   py -3.12 verify/sea_fix_20260930/boxstats.py BOXSET img [img ...]
#   py -3.12 verify/sea_fix_20260930/boxstats.py --diff BOXSET imgA imgB
#
# BOXSET is a key in BOXES below. --diff prints, per box, the count and percent
# of pixels whose max-channel abs difference exceeds 16 (imgdiff's threshold).
import json
import sys

from PIL import Image

# --- The pre-registered boxes. x0,y0,x1,y1 in 640x480 backbuffer pixels,
# half-open. Chosen from the ORIGINAL captures only (never from a fix arm).
BOXES = {
    # Arctic at the s8 original pose (verify/arctic_ref/sea_search/s8).
    # Camera looks down ~62 deg; the whole frame is ground. ROAD* boxes sit on
    # ground the original shows as textured road and the pre-fix port covers
    # with the mis-placed sea sheet. They avoid all four cars and the right-edge
    # EMPIRE banner prop.
    "s8": {
        # NON-GATING (see NON_GATING below): measured and reported, but excluded
        # from rule 2a because the ORIGINAL is itself near-featureless here
        # (g 0.8229 vs the pre-fix port's 0.6421, a 1.28x separation), so the
        # box cannot discriminate road from water in either direction. Dropped
        # from the gate on the baseline numbers alone, before the fix existed.
        "ROAD_UL": (40, 30, 180, 130),
        "ROAD_UC": (215, 20, 345, 110),
        "ROAD_MC": (255, 215, 375, 315),
        "ROAD_LC": (200, 400, 340, 475),
        # CAR_* are tight boxes strictly INSIDE a car silhouette in the PORT
        # arm (verified against the pre-fix capture, boxes_port_s8.png). Cars
        # draw over the sea in both arms, so these are the rule-(iv) controls:
        # moving the sea 4.1 m down must not touch a single pixel here.
        "CAR_A": (195, 140, 245, 185),
        "CAR_B": (395, 120, 445, 165),
        "CAR_C": (375, 360, 430, 420),
    },
    # Arctic at the s14 original pose (verify/arctic_ref/sea_search/s14).
    # Camera looks down ~34 deg down the road. ROAD* on the road surface;
    # CTRL_BLDG is the lit building facade at screen left and CTRL_PIPE the
    # silo/pipe cluster at upper right -- prop geometry well above the water
    # plane, the rule-(iv) prop controls.
    "s14": {
        "ROAD_FG": (250, 390, 400, 478),
        "ROAD_MID": (400, 250, 520, 350),
        "ROAD_R": (420, 385, 540, 470),
        "CTRL_BLDG": (25, 60, 120, 195),
        "CTRL_PIPE": (455, 25, 535, 115),
    },
}

# Boxes measured and printed but NOT part of any pass criterion.
NON_GATING = {("s8", "ROAD_UL")}


def luma(im):
    px = im.convert("RGB").load()
    w, h = im.size
    return [[0.299 * px[x, y][0] + 0.587 * px[x, y][1] + 0.114 * px[x, y][2]
             for x in range(w)] for y in range(h)]


def box_stats(Y, box):
    x0, y0, x1, y1 = box
    tot = 0.0
    gtot = 0.0
    n = 0
    ng = 0
    for y in range(y0, y1):
        for x in range(x0, x1):
            tot += Y[y][x]
            n += 1
            if 0 < x < len(Y[0]) - 1 and 0 < y < len(Y) - 1:
                gx = (Y[y - 1][x + 1] + 2 * Y[y][x + 1] + Y[y + 1][x + 1]
                      - Y[y - 1][x - 1] - 2 * Y[y][x - 1] - Y[y + 1][x - 1])
                gy = (Y[y + 1][x - 1] + 2 * Y[y + 1][x] + Y[y + 1][x + 1]
                      - Y[y - 1][x - 1] - 2 * Y[y - 1][x] - Y[y - 1][x + 1])
                gtot += (gx * gx + gy * gy) ** 0.5 / 8.0
                ng += 1
    L = tot / n if n else 0.0
    g = gtot / ng if ng else 0.0
    return {"n": n, "L": round(L, 3), "g": round(g, 4),
            "g_over_L": round(g / L, 5) if L > 1e-6 else None}


def main():
    a = sys.argv[1:]
    if a and a[0] == "--diff":
        bs = BOXES[a[1]]
        A = Image.open(a[2]).convert("RGB").load()
        B = Image.open(a[3]).convert("RGB").load()
        out = {}
        for name, (x0, y0, x1, y1) in bs.items():
            over = 0
            n = 0
            for y in range(y0, y1):
                for x in range(x0, x1):
                    d = max(abs(A[x, y][c] - B[x, y][c]) for c in range(3))
                    n += 1
                    if d > 16:
                        over += 1
            out[name] = {"n": n, "over16": over,
                         "pct": round(100.0 * over / n, 2) if n else None}
        print(json.dumps(out, indent=1))
        return 0
    bs = BOXES[a[0]]
    for path in a[1:]:
        Y = luma(Image.open(path))
        print(path)
        print(json.dumps({k: box_stats(Y, v) for k, v in bs.items()}, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
