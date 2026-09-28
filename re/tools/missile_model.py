#!/usr/bin/env python3
"""Offline model of the MISSILE tick's contact half, checked against a capture.

The tick is FUN_00455c90 (0x00455c90). Ghidra's auto-analysis never defined it,
so it was read with the `--create` mode on re/tools/decomp_pc.py (a TRANSIENT
definition against a -readOnly pool clone -- discarded on exit, not a master
write). Everything below is from that decompilation plus disassembly of
original/MASHED.exe.unpatched.

Per LIVE projectile record, per frame:

  flight step: (+0x54 == -1 && +0x58 == -1) ? FUN_00455610 : FUN_004556f0
  age += DAT_007f100c
  if (age > _DAT_005cc31c (3.0)):  FUN_00455910 terminal, done for this record
  if ((DAT_007f101c & 0x80000001) == 0):            # EVEN FRAMES ONLY
      r = |delta|^2 * _DAT_005cc348 (1.5) + 0.05
      n = FUN_004b4d10(world, {pos, r})             # a SPHERE query, RA 0x455de0
      if (n != 0 and FUN_0045c350(...) == 0): FUN_00455910()   # RA 0x455df9/0x455e07
  n = FUN_004b4cd0(world, pos -> pos - (0, 3.0, 0)) # RA 0x455e59, EVERY frame
  if (n == 0): bias = -0.0005                       # 0xba03126f
  else:        bias = (t*3.0 - 0.4) * 2.5 * -0.5 ;  if (bias < -0.05) bias = -0.05

`bias` is the record's +0x28, the ground-follow term added to delta.y. It is a
pure function of the query's verdict, so the port owns it completely even though
the port does NOT own the flight integration.

THREE PREDICTIONS, none of which the capture's schedule decides for us:
  A  the per-frame count of 0x455de0 rows == live records passing parity AND age
  B  the per-frame count of 0x455e59 rows == live records passing age
  C  a record whose ground query MISSED must carry bias == -0.0005 exactly, and
     one that HIT must carry a bias whose recovered t lies in [0, 1]

Prediction C needs no recorded `t`: bias = 0.5 - 3.75*t, clamped below at -0.05,
so t = (0.5 - bias) / 3.75 and the clamp shows up as exactly -0.05.

Tick ORDER matters for C: the loop starts at EBP = 0x688620 and DECREMENTS, so it
walks record index 4 first and 0 last. The capture emits rows in index order, so
the contact rows of a frame zip against the live records in DESCENDING index.

Usage:
  py -3.12 re/tools/missile_model.py verify/d3_contact_20260928b/s1.msd
"""
import argparse
import csv
import struct
import sys
from collections import defaultdict

AGE_LIMIT = 3.0             # _DAT_005cc31c 0x40400000
RAD_K     = 1.5             # _DAT_005cc348 0x3fc00000
RAD_BASE  = 0.05
G_A       = 0.400000006     # _DAT_005ccac0
G_B       = 2.5             # _DAT_005cd088
G_C       = -0.5            # _DAT_005cd50c
G_CLAMP   = -0.0500000007   # _DAT_005cd054  (and the value written on the clamp)
MISS      = -0.000500000024 # 0xba03126f

SPHERE = "0x455de0"
GATE   = "0x455df9"
GROUND = "0x455e59"
TERM   = ("0x455d37", "0x455e07")


def f32(h):
    if h is None or not str(h).startswith("0x"):
        return None
    return struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base")
    ap.add_argument("--show", type=int, default=6)
    a = ap.parse_args()

    try:
        rows = list(csv.DictReader(open(a.base + ".pumissile.csv")))
    except IOError:
        print("no %s.pumissile.csv -- capture taken without --puhook-missile" % a.base)
        return 2
    if not rows:
        print("no missile rows")
        return 2

    dt = None
    for r in csv.DictReader(open(a.base + ".puhook.csv")):
        d = r.get("dt", "")
        if d.startswith("0x"):
            dt = struct.unpack("<f", struct.pack("<I", int(d, 16)))[0]
            break
    if dt is None:
        print("no dt in the puhook capture")
        return 2

    sphereBy, groundBy, termBy = defaultdict(list), defaultdict(list), defaultdict(int)
    for r in csv.DictReader(open(a.base + ".pucontact.csv")):
        ra, fr = r["ret_addr"], r["frame"]
        if ra == SPHERE:
            sphereBy[fr].append(int(r["ret"]))
        elif ra == GROUND:
            groundBy[fr].append(int(r["ret"]))
        elif ra in TERM:
            termBy[fr] += 1

    byFrame = defaultdict(list)
    for r in rows:
        byFrame[r["frame"]].append(r)

    okA = badA = okB = badB = okC = badC = 0
    wrongA, wrongB, wrongC = [], [], []
    evenFrames = 0

    for fr, recs in byFrame.items():
        fc = int(recs[0]["framectr"])
        even = (fc & 0x80000001) == 0
        if even:
            evenFrames += 1
        # age gate, using the PRE age plus this frame's dt, per the tick's order
        alive = [r for r in recs if (f32(r["pre_age"]) + dt) <= AGE_LIMIT]
        # A: the sphere query
        predA = len(alive) if even else 0
        gotA = len(sphereBy[fr])
        if predA == gotA:
            okA += 1
        else:
            badA += 1
            wrongA.append((fr, fc, even, predA, gotA, len(recs)))
        # B: the ground query
        predB = len(alive)
        gotB = len(groundBy[fr])
        if predB == gotB:
            okB += 1
        else:
            badB += 1
            wrongB.append((fr, fc, predB, gotB, len(recs)))
        # C: the bias sentinel. Tick order is DESCENDING record index.
        aliveDesc = sorted(alive, key=lambda r: -int(r["rec"]))
        if len(aliveDesc) == len(groundBy[fr]):
            for r, n in zip(aliveDesc, groundBy[fr]):
                bias = f32(r["post_bias"])
                if bias is None:
                    continue
                if n == 0:
                    good = abs(bias - MISS) < 1e-9
                    why = "miss must be exactly -0.0005"
                else:
                    t = (bias - G_A * G_B * G_C) / (AGE_LIMIT * G_B * G_C)
                    good = (abs(bias - G_CLAMP) < 1e-9) or (-0.06 <= bias <= 0.51)
                    why = "hit bias out of range (recovered t=%.4f)" % t
                if good:
                    okC += 1
                else:
                    badC += 1
                    wrongC.append((fr, r["rec"], n, bias, why))

    print("missile rows (live x frame)   %d over %d frames" % (len(rows), len(byFrame)))
    print("  even frames (parity gate)   %d of %d" % (evenFrames, len(byFrame)))
    print("  0x455de0 sphere rows        %d" % sum(len(v) for v in sphereBy.values()))
    print("  0x455e59 ground rows        %d" % sum(len(v) for v in groundBy.values()))
    print("  terminal rows               %d" % sum(termBy.values()))
    print()
    print("A  sphere-query count   frames match=%d mismatch=%d" % (okA, badA))
    print("B  ground-query count   frames match=%d mismatch=%d" % (okB, badB))
    print("C  bias sentinel        records match=%d mismatch=%d" % (okC, badC))
    bad = badA + badB + badC
    print("VERDICT: %s" % ("CLEAN" if bad == 0 else "DIVERGES"))
    for w in wrongA[: a.show]:
        print("  A frame %s ctr=%d even=%s predicted %d got %d (live rows %d)" % w)
    for w in wrongB[: a.show]:
        print("  B frame %s ctr=%d predicted %d got %d (live rows %d)" % w)
    for w in wrongC[: a.show]:
        print("  C frame %s rec %s ret=%d bias=%.6g  %s" % w)
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
