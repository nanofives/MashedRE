#!/usr/bin/env python3
"""Offline model of FUN_00459620's target-ACQUISITION loop, checked against a capture.

Criterion (c) for MORTAR / GUN / MISSILE turns on ONE branch inside the routine
those three share:

    if (local_104 == 0)  ->  the vertical fallback probe, query site 0x459c19
    else                 ->  the locked-target path, which skips it

`local_104` is the count of acquisition candidates. Both query sites are reached
on every call, but 0x459c19 only on the `== 0` side, so "did the candidate loop
find anything" is directly observable in the contact capture as "did 0x459c19
fire on this call". That makes the loop falsifiable without porting it first.

This script predicts the branch from the `--puhook-aim` channel's recorded inputs
and diffs the prediction against the `.pucontact.csv` for the same run.

The loop, from the decompilation of FUN_00459620 (0x00459620) and the
disassembly of its prologue, both against original/MASHED.exe.unpatched:

  for car i = 3, 2, 1, 0:                     # `MOV EBX,4` @0x004596ab, DEC then test
      if i == param_1: skip                   # `CMP EAX,EBX` @0x004596cd
      if FUN_0040e370(i) == 0: skip           # car-active flag
      d = (car[i].x - origin.x, 0.0, car[i].z - origin.z)   # NOTE: y is forced to 0
      dist = |d|                              # FUN_004c3ac0
      if |d|^2 > _DAT_005cc990 (1e-5): normalise d          # FUN_004c39b0
      ang = acos(dot(d, at)) * _DAT_005cc98c (57.29578, i.e. 180/pi)
      if ang < param_4 and dist < param_3: it is a candidate

`at` is rows 8..10 of the matrix FUN_0041f220 copies into the aim record's +0x38
buffer, i.e. the RwMatrix `at` row at +0x20.

The second candidate list (FUN_004075a0 / FUN_004075b0, records at 0x00639d90
stride 0x3b dwords, count at 0x0063a5d0) is walked too and appends to the same
array. It is modelled as empty and the `list_n` column says whether that held:
every row of the 2026-09-28b capture has list_n == 0.

Usage:
  py -3.12 re/tools/aim_model.py verify/d3_contact_20260928b/m1.msd
"""
import argparse
import csv
import math
import struct
import sys
from collections import Counter

RAD2DEG = 57.2957802          # _DAT_005cc98c
NORM_EPS = 9.99999975e-06     # _DAT_005cc990
FALLBACK_SITE = 0x459c19      # the `local_104 == 0` query site
LOS_SITE = 0x459d54           # the unconditional line-of-sight query site


def f32(h):
    """A recorded float, from its exact bits. Never parse these as decimals."""
    if not h or not h.startswith("0x"):
        return None
    return struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]


def predict(row):
    """Return (n_candidates, per-car detail) for one acquisition call."""
    slot = int(row["slot"])
    o = [f32(row[k]) for k in ("ox", "oy", "oz")]
    rng = f32(row["range"])
    cone = f32(row["cone"])
    at = [f32(row[k]) for k in ("at_x", "at_y", "at_z")]
    if None in o or None in at or rng is None or cone is None:
        return None, []
    n, detail = 0, []
    for i in (3, 2, 1, 0):
        if i == slot:
            continue
        if int(row["c%d_act" % i]) == 0:
            continue
        c = [f32(row["c%d_%s" % (i, a)]) for a in "xyz"]
        if None in c:
            continue
        d = [c[0] - o[0], 0.0, c[2] - o[2]]
        dist = math.sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2])
        lsq = d[0] * d[0] + d[1] * d[1] + d[2] * d[2]
        v = list(d)
        if lsq > NORM_EPS:
            L = math.sqrt(lsq)
            v = [d[0] / L, d[1] / L, d[2] / L]
        dot = v[0] * at[0] + v[1] * at[1] + v[2] * at[2]
        ang = math.acos(max(-1.0, min(1.0, dot))) * RAD2DEG
        hit = ang < cone and dist < rng
        detail.append((i, dist, ang, hit))
        if hit:
            n += 1
    return n, detail


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base", help="capture base, e.g. verify/.../m1.msd")
    ap.add_argument("--show", type=int, default=0, help="print N mismatching calls")
    a = ap.parse_args()

    aim = list(csv.DictReader(open(a.base + ".puaim.csv")))
    if not aim:
        print("no aim rows -- was the capture taken with --puhook-aim?")
        return 2

    # which calls the fallback site actually fired on
    fired, los = set(), set()
    for r in csv.DictReader(open(a.base + ".pucontact.csv")):
        ra = int(r["ret_addr"], 16)
        if ra == FALLBACK_SITE:
            fired.add(int(r["call"]))
        elif ra == LOS_SITE:
            los.add(int(r["call"]))

    print("aim calls              %d" % len(aim))
    print("list_n values          %s" % dict(Counter(r["list_n"] for r in aim)))
    print("0x459c19 fired on      %d calls" % len(fired))
    print("0x459d54 fired on      %d calls" % len(los))

    ok = bad = skipped = 0
    wrong = []
    for r in aim:
        n, detail = predict(r)
        if n is None:
            skipped += 1
            continue
        call = int(r["call"])
        # the routine only reaches its query sites at all on calls the capture saw
        if call not in los:
            skipped += 1
            continue
        want_fallback = call in fired
        got_fallback = (n == 0)
        if want_fallback == got_fallback:
            ok += 1
        else:
            bad += 1
            wrong.append((call, n, want_fallback, detail))

    print()
    print("BRANCH PREDICTION   match=%d  mismatch=%d  (not comparable: %d)"
          % (ok, bad, skipped))
    print("VERDICT: %s" % ("CLEAN" if bad == 0 else "DIVERGES"))
    for w in wrong[: a.show]:
        call, n, want, detail = w
        print("  call %d  predicted n=%d (fallback=%s) but capture fallback=%s"
              % (call, n, n == 0, want))
        for i, dist, ang, hit in detail:
            print("     car %d dist=%.4f ang=%.4f hit=%s" % (i, dist, ang, hit))
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
