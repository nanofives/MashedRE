#!/usr/bin/env python3
"""field_trace.py - extract a per-frame trajectory of named vehicle-record fields
from ONE MSD1 capture (re/frida/scenario_launch.py --statediff-out).

Purpose (D2 / A8 steer-sign): read the physical steer-angle output and the
resulting heading so the ORIGINAL's steer-sign convention can be measured.
Relates the front-wheel steer angle (+0x1a8, written by A4 FUN_00470670) to the
change in VELOCITY heading atan2(vel_z, vel_x) -- the like-for-like quantity for
the standalone's real-physics car_yaw (TrackRenderer.cpp:2553, io.yaw = velocity
heading; position advances along it at 2560-2562).

*** MEASURED 2026-08-24 -- +0x1a8/+0x26c ARE UNOBSERVABLE BY THIS METHOD. ***
Do not build an A8 verdict on them. In verify/a8_steer_20260823/orig_steerR.msd
both offsets are EXACTLY 0.0 in all 2335 frames (0 nonzero, max|v| = 0), while
315 of 832 dword slots in the record do vary. The offsets are NOT wrong -- the
derivation in VehiclePhysicsRun.h:26-51 is RVA-cited (wheelN steer =
+0x16c + N*0xC4 + 0x3c => w0=+0x1a8, w1=+0x26c). They are unreadable because A4
ZEROES ALL FOUR SLOTS AT EVERY ENTRY (0x004706c1..d3, ported verbatim at
VehicleControl.cpp:86) and A5 FUN_0046ddb0 Phase 0 consumes them inside the same
physics step (VehiclePhysicsRun.h:42-44). They are per-frame scratch, so a
capture that samples the record at a frame boundary reads 0 by construction.
CONSEQUENCE: the standalone side would ALSO read 0, the diff would be
bit-identical, and A8 would report a FALSE GREEN -- the same class as the iter17
0x00482900 incident in the orchestrator ledger. Measure the steer-sign law from
PERSISTENT record state instead: yaw rate +0x9c0 (A6a's body output), velocity
+0x9b0..b8, forward axis +0x9d4/+0x9dc.

NOTE the docstring below previously cited scenario_launch.py:133-149 for the two
steer offsets. That block documents +0x928/+0x958/+0x9b0..b8/+0x9c0/+0x9d4/
+0x9dc/+0x9e0/+0x9e4/+0xb20 and contains NEITHER +0x1a8 NOR +0x26c. Corrected
citation: VehiclePhysicsRun.h:26-51.

Fields (byte offsets into the 0xd04 record; velocity/heading/speed/grounded
citations in scenario_launch.py:133-149, steer slots in VehiclePhysicsRun.h:35-36):
  +0x1a8 steerAng0 (A4 out; ZERO AT SAMPLE TIME -- see above)  +0x26c steerAng1
  +0x9b0/+0x9b4/+0x9b8 velocity x/y/z                   +0x9c0 yaw rate (angvel.y)
  +0x9d4/+0x9dc forward axis x/z                        +0x9e0 grounded (4.0=all)
  +0x9e4 scalar speed                                    +0xbf4 countdown anchor

No re-execution, read-only. Prints a compact trajectory + a sign summary.
"""
import argparse
import math
import struct
import sys

MAGIC = b"MSD1"

F = {  # name -> (offset, kind)  kind: 'f' float, 'i' int
    "steerAng0": (0x1a8, "f"),
    "steerAng1": (0x26c, "f"),
    "velx":      (0x9b0, "f"),
    "vely":      (0x9b4, "f"),
    "velz":      (0x9b8, "f"),
    "yawrate":   (0x9c0, "f"),
    "fwdx":      (0x9d4, "f"),
    "fwdz":      (0x9dc, "f"),
    "grounded":  (0x9e0, "f"),
    "speed":     (0x9e4, "f"),
    "anchor":    (0xbf4, "i"),
}


def load(path):
    frames = {}
    with open(path, "rb") as f:
        hdr = f.read(16)
        if len(hdr) != 16 or hdr[:4] != MAGIC:
            sys.exit(f"{path}: not an MSD1 capture")
        rec_size, base_va, _ = struct.unpack_from("<III", hdr, 4)
        while True:
            fh = f.read(4)
            if len(fh) < 4:
                break
            (idx,) = struct.unpack("<I", fh)
            payload = f.read(rec_size)
            if len(payload) < rec_size:
                break
            frames[idx] = payload
    return rec_size, base_va, frames


def val(payload, off, kind):
    raw = payload[off:off + 4]
    if kind == "f":
        return struct.unpack("<f", raw)[0]
    return struct.unpack("<i", raw)[0]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("msd")
    ap.add_argument("--every", type=int, default=10, help="print every Nth frame (default 10)")
    ap.add_argument("--min-speed", type=float, default=1.0,
                    help="driving window = frames with |speed| >= this (default 1.0)")
    args = ap.parse_args()

    rec_size, base_va, frames = load(args.msd)
    order = sorted(frames)
    if not order:
        sys.exit("no frames")
    print(f"# {args.msd}  rec_size={rec_size:#x} base_va={base_va:#x} frames={len(order)} "
          f"[{order[0]}..{order[-1]}]")

    # velocity heading, computed identically to the standalone: atan2(z, x)
    def velH(p):
        vx, vz = val(p, 0x9b0, "f"), val(p, 0x9b8, "f")
        return math.atan2(vz, vx) if (vx * vx + vz * vz) > 1e-6 else None

    def fwdH(p):
        fx, fz = val(p, 0x9d4, "f"), val(p, 0x9dc, "f")
        return math.atan2(fz, fx) if (fx * fx + fz * fz) > 1e-6 else None

    print(f"{'frame':>7} {'anchor':>7} {'speed':>10} {'steerAng0':>10} {'steerAng1':>10} "
          f"{'yawrate':>10} {'velH':>9} {'fwdH':>9} {'grnd':>5}")
    drive = []
    for i in order:
        p = frames[i]
        sp = val(p, 0x9e4, "f")
        vh, fh = velH(p), fwdH(p)
        if i % args.every == 0 or abs(sp) >= args.min_speed:
            if i % args.every == 0:
                print(f"{i:>7} {val(p,0xbf4,'i'):>7} {sp:>10.3f} "
                      f"{val(p,0x1a8,'f'):>10.4f} {val(p,0x26c,'f'):>10.4f} "
                      f"{val(p,0x9c0,'f'):>10.5f} "
                      f"{('%.4f'%vh) if vh is not None else '   n/a':>9} "
                      f"{('%.4f'%fh) if fh is not None else '   n/a':>9} "
                      f"{val(p,0x9e0,'f'):>5.1f}")
        if abs(sp) >= args.min_speed and vh is not None:
            drive.append((i, sp, val(p, 0x1a8, "f"), val(p, 0x26c, "f"),
                          val(p, 0x9c0, "f"), vh, fh))

    print(f"\n# driving window (|speed|>={args.min_speed}): {len(drive)} frames")
    if len(drive) < 2:
        print("# too few driving frames to judge a heading change")
        return

    def unwrap(seq):
        out = [seq[0]]
        for a in seq[1:]:
            d = a - out[-1]
            while d > math.pi:
                d -= 2 * math.pi
            while d < -math.pi:
                d += 2 * math.pi
            out.append(out[-1] + d)
        return out

    def sgn(x):
        return "+" if x > 1e-6 else ("-" if x < -1e-6 else "0")

    # A heading delta is only defined WITHIN one active round. The stock car
    # ramps unbounded then resets to ~0 at each round boundary (measured:
    # D2_REALPHYS_REMEASURE_2026-08-21.md "there is no terminal velocity"), and
    # those resets are excluded from `drive` by the min-speed filter -- so
    # unwrapping the surviving frames as one series joins across the gaps and
    # invents rotation. orig_steerR.msd reported delta -13.0160 rad that way,
    # which is an unwrap artifact, not a turn. Segment on frame-index gaps.
    rounds = []
    cur = [drive[0]]
    for prev, d in zip(drive, drive[1:]):
        if d[0] - prev[0] > 1:
            rounds.append(cur)
            cur = [d]
        else:
            cur.append(d)
    rounds.append(cur)

    sa0 = [d[2] for d in drive]
    yr = [d[4] for d in drive]
    mean_sa0 = sum(sa0) / len(sa0)
    mean_yr = sum(yr) / len(yr)
    sa0_nonzero = sum(1 for v in sa0 if v != 0.0)

    print(f"# steerAng0: mean {mean_sa0:+.4f}  sign {sgn(mean_sa0)}  "
          f"nonzero {sa0_nonzero}/{len(sa0)}")
    print(f"# yawrate  : mean {mean_yr:+.6f} sign {sgn(mean_yr)}")
    print(f"# rounds (contiguous driving segments): {len(rounds)}")
    for n, seg in enumerate(rounds):
        if len(seg) < 2:
            print(f"#   round {n}: frames {seg[0][0]}..{seg[0][0]}  (1 frame, no delta)")
            continue
        vu = unwrap([d[5] for d in seg])
        fu = unwrap([d[6] for d in seg if d[6] is not None])
        d_v = vu[-1] - vu[0]
        d_f = (fu[-1] - fu[0]) if len(fu) >= 2 else float("nan")
        syr = sum(d[4] for d in seg) / len(seg)
        print(f"#   round {n}: frames {seg[0][0]:>5}..{seg[-1][0]:<5} "
              f"n={len(seg):>4}  velH {vu[0]:+.4f} -> {vu[-1]:+.4f} "
              f"delta {d_v:+.4f} sign {sgn(d_v)}  fwdH delta {d_f:+.4f}  "
              f"yawrate mean {syr:+.6f} sign {sgn(syr)}")

    if sa0_nonzero == 0:
        print("\n# NO LAW EMITTED: steerAng0 (+0x1a8) is identically 0 in every")
        print("#   driving frame, so no steer->heading law can be derived from it.")
        print("#   This is EXPECTED, not a capture failure: A4 zeroes +0x1a8/+0x26c")
        print("#   at every entry (0x004706c1..d3; VehicleControl.cpp:86) and A5")
        print("#   FUN_0046ddb0 Phase 0 consumes them inside the same physics step")
        print("#   (VehiclePhysicsRun.h:42-44), so the slots are per-frame scratch")
        print("#   and read 0 at any frame-boundary sample. Both sides of an A8 diff")
        print("#   would read 0 -> bit-identical -> FALSE GREEN (iter17 0x00482900")
        print("#   class). Derive the law from yawrate +0x9c0 vs the injected steer")
        print("#   byte instead, per round, using the per-round deltas above.")
    else:
        print(f"\n# LAW: steerAng0 sign {sgn(mean_sa0)}  ->  per-round velH signs "
              f"{[sgn(unwrap([d[5] for d in s])[-1] - unwrap([d[5] for d in s])[0]) for s in rounds if len(s) >= 2]}  "
              f"(yawrate sign {sgn(mean_yr)})")


if __name__ == "__main__":
    main()
