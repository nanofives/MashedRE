# P3 checker for the pickup placement fix (PREREG_STAGE1.md rule P3).
#
# Builds the comparison mask from PROJECTED GEOMETRY ONLY. Each pickup region is
# the screen-space disc the ENGINE computed for that orb, from the live
# D3DTS_VIEW * D3DTS_PROJECTION and the live viewport, dumped beside the frame
# it drew (`<bmp>.pudump.txt`, written by PickupField::Render -> exe_main's
# capture path). There is no colour class and no hand-drawn rectangle here: the
# last three fix children each lost a rule to one of those.
#
# Three jobs, selected by sub-command:
#
#   diff   <preBMP> <preDUMP> <postBMP> <postDUMP> [--dilate 3] [--mask out.png]
#          -> differing pixels INSIDE and OUTSIDE the union of the two arms'
#             regions. P3 wants outside == 0, and inside > 0 (otherwise the
#             comparison is vacuous and P3 is UNMEASURABLE, not PASS).
#
#   xcheck <dump> --pose <12-float basis file or literal> [--tol 1.0]
#          -> independent Python projection of the same world centres through
#             the committed basis + published lens, vs the engine's own screen
#             centres. Max |d| must be <= tol. This is what proves the mask is
#             not stale.
#
#   whole  <bmpA> <bmpB>
#          -> differing pixels over the WHOLE frame (the instrumentation
#             control, and the two-boot identity check).
#
# Lens, matching TrackRenderer.cpp:5331-5343: fovy = 2*atan(0.45),
# aspect = 800/600, near = 0.1. MatPerspectiveFovLH (TrackRenderer.cpp:964):
# ys = 1/tan(fovy/2), xs = ys/aspect, w = eye-space z.
import argparse
import math
import sys
from pathlib import Path

from PIL import Image

K_VIEWWINDOW_Y = 0.45
ASPECT = 800.0 / 600.0


def load_dump(p):
    """-> dict(vp=(W,H), worldR, orbs=[dict(...)], active, total)."""
    out = {"orbs": [], "vp": None, "worldR": None}
    for ln in Path(p).read_text(errors="replace").splitlines():
        t = ln.split()
        if not t:
            continue
        if t[0] == "PUDUMP":
            for kv in t[1:]:
                k, _, v = kv.partition("=")
                if k == "vp":
                    w, _, h = v.partition("x")
                    out["vp"] = (int(w), int(h))
                elif k in ("worldR", "bound_r"):
                    out[k] = float(v)
                elif k in ("orbs", "active"):
                    # NOT out["orbs"] � that key holds the per-orb list.
                    out["n_" + k] = int(v)
        elif t[0] == "ORB":
            d = {"i": int(t[1])}
            for kv in t[2:]:
                k, _, v = kv.partition("=")
                if k == "w":
                    d["w"] = tuple(float(x) for x in v.split(","))
                elif k == "s":
                    d["s"] = tuple(float(x) for x in v.split(","))
                elif k in ("type", "active", "onscreen"):
                    d[k] = int(v)
                else:
                    d[k] = float(v)
            out["orbs"].append(d)
    if out["vp"] is None:
        sys.exit(f"{p}: no PUDUMP header line")
    return out


def discs(dump, dilate):
    """-> list of (cx, cy, r) screen discs for the on-screen, drawn orbs."""
    o = []
    for d in dump["orbs"]:
        if not d.get("onscreen", 0) or not d.get("active", 1):
            continue
        o.append((d["s"][0], d["s"][1], d["r"] + dilate))
    return o


def mask_from(discs_, W, H):
    m = bytearray(W * H)
    for cx, cy, r in discs_:
        r2 = r * r
        x0, x1 = max(0, int(cx - r) - 1), min(W - 1, int(cx + r) + 1)
        y0, y1 = max(0, int(cy - r) - 1), min(H - 1, int(cy + r) + 1)
        for y in range(y0, y1 + 1):
            dy = y - cy
            for x in range(x0, x1 + 1):
                dx = x - cx
                if dx * dx + dy * dy <= r2:
                    m[y * W + x] = 1
    return m


def px(img):
    im = Image.open(img).convert("RGB")
    return im.size, im.tobytes()


def cmd_whole(a):
    (W1, H1), b1 = px(a.A)
    (W2, H2), b2 = px(a.B)
    if (W1, H1) != (W2, H2):
        print(f"SIZE MISMATCH {W1}x{H1} vs {W2}x{H2}")
        return 2
    n = sum(1 for i in range(0, len(b1), 3) if b1[i:i+3] != b2[i:i+3])
    print(f"whole-frame differing pixels = {n}   ({W1}x{H1}, "
          f"{100.0*n/(W1*H1):.4f}%)")
    print(f"  A={a.A}\n  B={a.B}")
    return 0


def cmd_diff(a):
    (W, H), bp = px(a.preBMP)
    (W2, H2), bq = px(a.postBMP)
    if (W, H) != (W2, H2):
        print(f"SIZE MISMATCH {W}x{H} vs {W2}x{H2}")
        return 2
    dpre, dpost = load_dump(a.preDUMP), load_dump(a.postDUMP)
    for nm, d in (("pre", dpre), ("post", dpost)):
        if d["vp"] != (W, H):
            print(f"VIEWPORT MISMATCH {nm} dump {d['vp']} vs bmp {W}x{H}")
            return 2
        if d.get("n_active") != d.get("n_orbs"):
            print(f"COLLECTION OCCURRED in {nm}: active={d.get('n_active')} "
                  f"of {d.get('n_orbs')} - frame not comparable (P3 guard)")
            return 2
    ds = discs(dpre, a.dilate) + discs(dpost, a.dilate)
    m = mask_from(ds, W, H)
    inside = outside = 0
    for i in range(W * H):
        if bp[3*i:3*i+3] != bq[3*i:3*i+3]:
            if m[i]:
                inside += 1
            else:
                outside += 1
    area = sum(m)
    print(f"pre  orbs={dpre.get('n_orbs')} on-screen={len(discs(dpre,0))}")
    print(f"post orbs={dpost.get('n_orbs')} on-screen={len(discs(dpost,0))}")
    print(f"mask discs={len(ds)} dilate={a.dilate} area={area} px "
          f"({100.0*area/(W*H):.3f}% of frame)")
    print(f"differing pixels: INSIDE mask = {inside}   OUTSIDE mask = {outside}")
    if outside == 0 and inside > 0:
        print("P3 verdict: PASS  (all change confined to projected pickup regions)")
    elif outside == 0 and inside == 0:
        print("P3 verdict: UNMEASURABLE  (no pixel changed anywhere — vacuous)")
    else:
        print("P3 verdict: FAIL")
    if a.mask:
        im = Image.open(a.preBMP).convert("RGB")
        q = im.load()
        for i in range(W * H):
            x, y = i % W, i // W
            if bp[3*i:3*i+3] != bq[3*i:3*i+3]:
                q[x, y] = (255, 0, 0) if not m[i] else (0, 255, 0)
            elif m[i]:
                r, g, b = q[x, y]
                q[x, y] = (r // 2, g // 2, min(255, b // 2 + 90))
        im.save(a.mask)
        print(f"  overlay -> {a.mask}  (green = diff inside mask, red = diff OUTSIDE)")
    return 0


def cmd_xcheck(a):
    d = load_dump(a.dump)
    W, H = d["vp"]
    txt = a.pose
    p = Path(txt)
    if p.exists():
        txt = p.read_text()
    v = [float(x) for x in txt.replace("\n", ",").split(",") if x.strip()]
    if len(v) != 12:
        sys.exit(f"--pose needs 12 floats, got {len(v)}")
    P, R, U, A = v[0:3], v[3:6], v[6:9], v[9:12]
    ys = 1.0 / math.tan(0.5 * (2.0 * math.atan(K_VIEWWINDOW_Y)))
    xs = ys / ASPECT
    worst = -1.0
    worst_i = -1
    n = 0
    for o in d["orbs"]:
        if not o.get("onscreen", 0):
            continue
        w = o["w"]
        dx = [w[k] - P[k] for k in range(3)]
        # NEGATED right axis: MatViewFromBasis (TrackRenderer.cpp:950-955)
        # negates it, because RenderWare's camera space and D3D's disagree on
        # which way right points on screen (the 2026-08-16 mirror fix,
        # verify/d1_basis/RESULT.md). The basis in MASHED_CAM_POSE is RW's,
        # straight off the RwCamera frame, so it needs the same treatment.
        # Omitting this mirrors every x about W/2 and leaves ez and sy exact,
        # which is precisely the signature the first run of this check showed.
        ex = -sum(dx[k] * R[k] for k in range(3))
        ey = sum(dx[k] * U[k] for k in range(3))
        ez = sum(dx[k] * A[k] for k in range(3))
        if ez <= 1e-4:
            continue
        sx = (ex * xs / ez * 0.5 + 0.5) * W
        sy = (0.5 - ey * ys / ez * 0.5) * H
        dd = math.hypot(sx - o["s"][0], sy - o["s"][1])
        n += 1
        if dd > worst:
            worst, worst_i = dd, o["i"]
        print(f"  orb {o['i']:2d} engine=({o['s'][0]:7.2f},{o['s'][1]:7.2f}) "
              f"python=({sx:7.2f},{sy:7.2f})  |d|={dd:.3f} px   "
              f"ez engine={o.get('ez', float('nan')):.4f} python={ez:.4f}")
    if n == 0:
        print("xcheck: no on-screen orb in the dump — UNMEASURABLE")
        return 3
    print(f"xcheck n={n}  max |d| = {worst:.3f} px (orb {worst_i})  "
          f"tol = {a.tol} -> {'PASS' if worst <= a.tol else 'FAIL'}")
    return 0 if worst <= a.tol else 1


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    d = sub.add_parser("diff")
    d.add_argument("preBMP"); d.add_argument("preDUMP")
    d.add_argument("postBMP"); d.add_argument("postDUMP")
    d.add_argument("--dilate", type=float, default=3.0)
    d.add_argument("--mask", default=None)
    d.set_defaults(fn=cmd_diff)
    x = sub.add_parser("xcheck")
    x.add_argument("dump"); x.add_argument("--pose", required=True)
    x.add_argument("--tol", type=float, default=1.0)
    x.set_defaults(fn=cmd_xcheck)
    w = sub.add_parser("whole")
    w.add_argument("A"); w.add_argument("B")
    w.set_defaults(fn=cmd_whole)
    a = ap.parse_args()
    return a.fn(a)


if __name__ == "__main__":
    sys.exit(main())
