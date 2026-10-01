# P1 + P2 checker for the pickup placement fix (PREREG_STAGE1.md).
#
# P1  the port's pickup list == the ORIGINAL's live-read pool, element-wise in
#     pool order, with BIT-IDENTICAL float32 positions.
# P2  the port KEEPS and DROPS the same POWERUPS_GOLD.DFF atomics as
#     FUN_00458e00 @0x00458e00 (cap 25; dedupe |d|^2 < 0.00100000005
#     = _DAT_005cc558 @0x005cc558; reject type 0x15 in the normal-race arm).
#
# Inputs
#   --log    the standalone's mashed_re.log from the run (PUPLACE/PUDROP lines)
#   --orig   the original's <bmp>.pickuprecs.json from re/frida/pickup_pos_probe2.py
#   --piz    the track piz, for the independent reference reader
#
# The original's record layout, as read by the probe (probe base 0x0068b1a0 is
# +8 from the true record base 0x0068b198, so raw[k] is true offset +(4k+8)):
#   raw[4]  +0x18 respawn   raw[6]  +0x20 state   raw[7]  +0x24 type
#   raw[9..11]  +0x2c..0x34 position     raw[12..14] +0x38..0x40 anchor
import argparse
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import powerups_gold_dump as P  # noqa: E402

DEDUPE_EPS = struct.unpack("<f", struct.pack("<I", 0x3A83126F))[0]
CAP = 25


def f32(u):
    return struct.unpack("<f", struct.pack("<I", u & 0xFFFFFFFF))[0]


def bits(x):
    return struct.unpack("<I", struct.pack("<f", x))[0]


def read_log(path):
    """-> (placed, dropped, meta). placed: [(i, type, respawn, (x,y,z))]."""
    placed, dropped, meta = [], [], {}
    for ln in Path(path).read_text(errors="replace").splitlines():
        t = ln.split()
        if not t:
            continue
        if t[0] == "PUPLACE":
            d = dict(kv.split("=", 1) for kv in t[1:] if "=" in kv)
            pos = tuple(f32(int(b, 16)) for b in d["bits"].split(","))
            placed.append((int(d["i"]), int(d["type"]), float(d["respawn"]), pos))
        elif t[0] == "PUDROP":
            d = dict(kv.split("=", 1) for kv in t[1:] if "=" in kv)
            dropped.append((int(d["atomic"]), int(d["type"]), d["reason"]))
        elif t[0] == "R4" and len(t) > 1 and t[1] == "pickups:":
            meta["r4"] = " ".join(t[2:])
    return placed, dropped, meta


def read_orig(path):
    samples = json.load(open(path))
    s0 = samples[0]
    for si, s in enumerate(samples):
        if s["count"] != s0["count"] or \
           [r["raw"][7] for r in s["recs"]] != [r["raw"][7] for r in s0["recs"]] or \
           [r["raw"][9:15] for r in s["recs"]] != [r["raw"][9:15] for r in s0["recs"]]:
            sys.exit(f"{path}: sample {si} disagrees with sample 0")
    out = []
    for r in s0["recs"][:s0["count"]]:
        raw = r["raw"]
        out.append((raw[7], f32(raw[4]),
                    tuple(f32(x) for x in raw[9:12]),
                    tuple(f32(x) for x in raw[12:15])))
    return out, len(samples)


def reference_filter(piz):
    """Independent implementation of FUN_00458e00's normal-race arm over the
    DFF atomics in the order the original enumerates them (measured: reverse of
    the file's atomic order). -> (kept, dropped)."""
    rows, warn = P.markers(piz)
    if rows is None:
        return None, warn
    kept, dropped = [], []
    for r in reversed(rows):
        if len(kept) > CAP - 1:                       # 0x18 < count
            dropped.append((r["atomic"], r["type"], "cap"))
            continue
        dup = False
        for k in kept:
            d = sum((k["local"][i] - r["local"][i]) ** 2 for i in range(3))
            if d < DEDUPE_EPS:
                dup = True
                break
        if dup:
            dropped.append((r["atomic"], r["type"], "dedupe"))
            continue
        if r["type"] == 0x15:
            dropped.append((r["atomic"], r["type"], "blank"))
            continue
        kept.append(r)
    return kept, dropped


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--log", required=True)
    ap.add_argument("--orig")
    ap.add_argument("--piz", required=True)
    a = ap.parse_args()

    placed, dropped, meta = read_log(a.log)
    kept_ref, dropped_ref = reference_filter(a.piz)
    print(f"log: {a.log}")
    print(f"  R4 line        : {meta.get('r4', '(absent)')}")
    print(f"  port placed    : {len(placed)}")
    print(f"  port dropped   : {len(dropped)}")
    print(f"  reference keeps: {len(kept_ref)}  drops: {len(dropped_ref)}")

    # ---- P2 vs the reference reader -------------------------------------
    p2_ref = True
    if len(placed) != len(kept_ref):
        p2_ref = False
    else:
        for (i, t, rs, pos), r in zip(placed, kept_ref):
            if t != r["type"] or rs != r["respawn"] or \
               any(bits(pos[k]) != bits(r["local"][k]) for k in range(3)):
                p2_ref = False
                break
    drops_port = sorted((d[0], d[2]) for d in dropped)
    drops_ref = sorted((d[0], d[2]) for d in dropped_ref)
    if drops_port != drops_ref:
        p2_ref = False
        only_p = set(drops_port) - set(drops_ref)
        only_r = set(drops_ref) - set(drops_port)
        print(f"  DROP SET DIFF  port-only={sorted(only_p)[:8]} "
              f"ref-only={sorted(only_r)[:8]}")
    print(f"  P2 (port vs reference reader): {'MATCH' if p2_ref else 'DIFFER'}")

    # ---- P1 vs the live original ----------------------------------------
    if not a.orig:
        print("  P1: no --orig given (no live read for this track)")
        return 0 if p2_ref else 1
    orig, nsamp = read_orig(a.orig)
    print(f"orig: {a.orig}   count={len(orig)}  samples={nsamp} (all agreeing)")
    ok = len(orig) == len(placed)
    if not ok:
        print(f"  COUNT MISMATCH port={len(placed)} orig={len(orig)}")
    else:
        for n, ((i, t, rs, pos), (ot, ors, opos, oanc)) in enumerate(
                zip(placed, orig)):
            same = (t == ot and bits(rs) == bits(ors)
                    and all(bits(pos[k]) == bits(opos[k]) for k in range(3)))
            ok &= same
            print(f"  [{n}] port type={t:2d} resp={rs:.0f} "
                  f"({pos[0]:.6f},{pos[1]:.6f},{pos[2]:.6f})  "
                  f"orig type={ot:2d} resp={ors:.0f} "
                  f"({opos[0]:.6f},{opos[1]:.6f},{opos[2]:.6f})  "
                  f"{'EXACT' if same else 'DIFFER'}")
    # order-insensitive fallback, so a pure permutation is reported as such
    if not ok:
        kp = sorted((t, bits(p[0]), bits(p[1]), bits(p[2])) for _i, t, _r, p in placed)
        ko = sorted((t, bits(p[0]), bits(p[1]), bits(p[2])) for t, _r, p, _a in orig)
        if kp == ko:
            print("  SET MATCHES but ORDER DIFFERS -> P1 PARTIAL")
    print(f"  P1 (port vs live original, bit-identical): "
          f"{'PASS' if ok else 'FAIL'}")
    return 0 if (ok and p2_ref) else 1


if __name__ == "__main__":
    sys.exit(main())
