#!/usr/bin/env python3
"""Offline model of FUN_004538b0, MORTAR's projectile integrator, checked against a capture.

MORTAR's own contact site (`0x453789`, inside `FUN_00453730`) probes
`pos -> pos + delta` once per airborne projectile per frame, so its call count
and its segment both come out of this integrator. The `--puhook-mortar` channel
records the record's PRE and POST state on every update, which makes the
integrator falsifiable field by field, per frame -- not just as a call count.

The routine, from the decompilation of FUN_004538b0 (0x004538b0..0x00453b7b) and
the tick's own pool loop (0x00453c30..0x00453c51), both against
original/MASHED.exe.unpatched:

  agen = age * _DAT_005ce424 (0.769231 = 1/1.3);  rec[0x14] = agen
  if agen > _DAT_005cc574 (2.0):  expire (FUN_00453210) and stop
  if homing:
      tgt = FUN_0046d4a0(owner)                 # the target's frame position
      d  = (aim.x + tgt.x - pos.x,  0.0,  aim.z + tgt.z - pos.z)
      len = |d| ; normalise d
      s  = len * agen * _DAT_005ce420 (0.38)
      vel = d * s                               # NOTE: vel.y = d.y * s = 0
      if agen > _DAT_005ce41c (0.58): homing = 0
  h = (age - _DAT_005cd230 (1.3)) * _DAT_005ce418 (-2.2)   if agen >= _DAT_005cd074 (1.25)
      sin(agen * _DAT_005ce2f4 (pi))                       otherwise
  age += dt
  prev = pos
  pos.x += vel.x ; pos.z += vel.z               # X and Z integrate by velocity
  pos.y  = h * _DAT_005cc348 (1.5) + baseY      # Y is an ABSOLUTE arc, not integrated
  delta  = pos - prev

The Y rule is the reason a mortar lobs: the arc is a function of age alone, and
the velocity's Y component is never used.

Usage:
  py -3.12 re/tools/mortar_model.py verify/d3_contact_20260928b/m2.msd
"""
import argparse
import csv
import math
import struct
import sys

AGE_RATE  = 0.769230783   # _DAT_005ce424
EXPIRE    = 2.0           # _DAT_005cc574
HOME_S    = 0.379999995   # _DAT_005ce420
HOME_OFF  = 0.579999983   # _DAT_005ce41c
ARC_SPLIT = 1.25          # _DAT_005cd074
ARC_T0    = 1.29999995    # _DAT_005cd230
ARC_SLOPE = -2.20000005   # _DAT_005ce418
PI        = 3.14159274    # _DAT_005ce2f4
ARC_H     = 1.5           # _DAT_005cc348


def f32(h):
    if h is None or not str(h).startswith("0x"):
        return None
    return struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]


def r32(x):
    """Round a Python double to float32, the width the game computes in."""
    return struct.unpack("<f", struct.pack("<f", x))[0]


def step(r, dt, detonated):
    """Predict the POST record from the PRE record. Returns None if not comparable.

    `detonated` is FUN_00453730's MEASURED verdict for this update, taken from the
    contact capture -- it is an INPUT, exactly as it is in the replay, because the
    world query behind it is. FUN_004538b0's whole body sits inside
    `if (FUN_00453730() == 0)`, so a detonation leaves the record untouched: PRE
    == POST, age included.
    """
    if detonated:
        return "detonated"
    pos = [f32(r["pre_p" + a]) for a in "xyz"]
    vel = [f32(r["pre_v" + a]) for a in "xyz"]
    age = f32(r["pre_age"])
    homing = int(r["pre_homing"])
    baseY = f32(r["baseY"])
    aim = [f32(r["aim_x"]), f32(r["aim_y"]), f32(r["aim_z"])]
    tgt = [f32(r["tgt_x"]), f32(r["tgt_y"]), f32(r["tgt_z"])]
    if None in pos or None in vel or age is None or baseY is None:
        return None

    agen = r32(age * AGE_RATE)
    if agen > EXPIRE:
        return "expired"

    if homing:
        if None in tgt:
            return None                      # homing frame with no recorded target
        d = [r32((aim[0] + tgt[0]) - pos[0]), 0.0,
             r32((aim[2] + tgt[2]) - pos[2])]
        L = r32(math.sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]))
        if L != 0.0:
            d = [r32(d[0] / L), r32(d[1] / L), r32(d[2] / L)]
        s = r32(r32(L * agen) * HOME_S)
        vel = [r32(d[0] * s), r32(d[1] * s), r32(d[2] * s)]
        if agen > HOME_OFF:
            homing = 0

    if agen >= ARC_SPLIT:
        h = r32(r32(age - ARC_T0) * ARC_SLOPE)
    else:
        h = r32(math.sin(r32(agen * PI)))

    prev = list(pos)
    npos = [r32(vel[0] + pos[0]),
            r32(r32(h * ARC_H) + baseY),
            r32(vel[2] + pos[2])]
    delta = [r32(npos[0] - prev[0]), r32(npos[1] - prev[1]), r32(npos[2] - prev[2])]
    return {"p": npos, "v": vel, "d": delta,
            "age": r32(age + dt), "agen": agen, "homing": homing}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base")
    ap.add_argument("--tol", type=float, default=1e-4,
                    help="absolute tolerance per float field")
    ap.add_argument("--show", type=int, default=6)
    a = ap.parse_args()

    try:
        rows = list(csv.DictReader(open(a.base + ".pumortar.csv")))
    except IOError:
        print("no %s.pumortar.csv -- capture was taken without --puhook-mortar" % a.base)
        return 2
    if not rows:
        print("no mortar rows -- was the capture taken with --puhook-mortar?")
        return 2

    # dt, from the dispatcher capture (exact bits).
    dt = None
    for r in csv.DictReader(open(a.base + ".puhook.csv")):
        d = r.get("dt", "")
        if d.startswith("0x"):
            dt = struct.unpack("<f", struct.pack("<I", int(d, 16)))[0]
            break
    if dt is None:
        print("no dt in the puhook capture")
        return 2

    # FUN_00453730's measured verdict per update, from the contact capture.
    # Within a frame both the pool loop and the capture run in pool order, so the
    # i-th 0x453789 row of a frame belongs to the i-th update of that frame. The
    # gate 0x4537bb only runs when the query returned non-zero, so it gets its own
    # per-frame queue. Detonation = query != 0 AND gate == 0 (0x00453730's body).
    from collections import defaultdict
    qByFrame, gByFrame = defaultdict(list), defaultdict(list)
    for r in csv.DictReader(open(a.base + ".pucontact.csv")):
        if r["ret_addr"] == "0x453789":
            qByFrame[r["frame"]].append(int(r["ret"]))
        elif r["ret_addr"] == "0x4537bb":
            gByFrame[r["frame"]].append(int(r["ret"]))
    qSeen, gSeen = defaultdict(int), defaultdict(int)

    fields = [("p", "post_p", "xyz"), ("v", "post_v", "xyz"), ("d", "post_d", "xyz")]
    ok = bad = skip = expired = detonated = 0
    per_field = {}
    wrong = []
    for r in rows:
        fr = r["frame"]
        q = qByFrame[fr][qSeen[fr]] if qSeen[fr] < len(qByFrame[fr]) else 0
        qSeen[fr] += 1
        det = False
        if q != 0:
            g = gByFrame[fr][gSeen[fr]] if gSeen[fr] < len(gByFrame[fr]) else 1
            gSeen[fr] += 1
            det = (g == 0)
        pred = step(r, dt, det)
        if pred is None:
            skip += 1
            continue
        if pred == "expired":
            expired += 1
            continue
        if pred == "detonated":
            # the record must be untouched; check that rather than skip it
            detonated += 1
            same = all(r["pre_" + k] == r["post_" + k] for k in
                       ("px", "py", "pz", "vx", "vy", "vz", "age", "homing"))
            if same:
                ok += 1
            else:
                bad += 1
                wrong.append((r["frame"], r["rec"], r["pre_homing"],
                              [("detonated-but-changed", 0, 0, 1)]))
            continue
        errs = []
        for key, pre, axes in fields:
            for i, ax in enumerate(axes):
                got = f32(r[pre + ax])
                if got is None:
                    continue
                e = abs(got - pred[key][i])
                if e > a.tol:
                    errs.append(("%s%s" % (key, ax), pred[key][i], got, e))
        for nm, k in (("age", "post_age"), ("agen", "post_agen")):
            got = f32(r[k])
            if got is not None and abs(got - pred[nm]) > a.tol:
                errs.append((nm, pred[nm], got, abs(got - pred[nm])))
        if int(r["post_homing"]) != pred["homing"]:
            errs.append(("homing", pred["homing"], int(r["post_homing"]), 1))
        if errs:
            bad += 1
            for e in errs:
                per_field[e[0]] = per_field.get(e[0], 0) + 1
            wrong.append((r["frame"], r["rec"], r["pre_homing"], errs))
        else:
            ok += 1

    print("mortar updates        %d" % len(rows))
    print("  homing frames       %d" % sum(1 for r in rows if int(r["pre_homing"])))
    print("  past expiry         %d  (the routine returns before integrating)" % expired)
    print("  detonated           %d  (FUN_00453730 != 0; PRE must equal POST)" % detonated)
    print("  not comparable      %d" % skip)
    print()
    print("INTEGRATION  match=%d  mismatch=%d  (tol %g)" % (ok, bad, a.tol))
    if per_field:
        print("  mismatching fields: %s"
              % ", ".join("%s:%d" % kv for kv in sorted(per_field.items())))
    print("VERDICT: %s" % ("CLEAN" if bad == 0 else "DIVERGES"))
    for w in wrong[: a.show]:
        print("  frame %s rec %s homing=%s" % (w[0], w[1], w[2]))
        for nm, want, got, e in w[3]:
            print("     %-6s predicted %.7g  capture %.7g  err %.3g" % (nm, want, got, e))
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
