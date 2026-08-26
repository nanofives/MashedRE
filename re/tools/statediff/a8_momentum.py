#!/usr/bin/env python3
"""A8 slip-deficit: the VELOCITY-TURN MOMENTUM IDENTITY, measured per side.

WHY THIS EXISTS
---------------
A8 closed with every measured quantity in the vehicle chain agreeing between the
port and the original EXCEPT slip angle (2.76x short at speed 1000-1500, 1.29x at
2000-2600). Four candidate causes are eliminated by measurement: the grip/clamp
chain, the per-wheel force MAGNITUDE, the per-wheel force DIRECTION, and the
constants. See re/analysis/data/A8_velocity_vector_motion_20260825.md follow-ups
seventeen..twenty-one.

Note that "force matches AND slip differs" is, by itself, near-tautologically a
statement that the slip->force map differs. Restating it as a stiffness ratio adds
nothing. What has NOT been measured is the OTHER half of the loop: whether the
velocity heading rotates at the rate the measured force commands. That is a
PER-SIDE SELF-CONSISTENCY check between two measured quantities, so it cannot be
circular (A8 trap 3: a quantity computed from our own formula is not a measurement).

THE IDENTITY
------------
For a body of mass m moving with horizontal velocity v, a horizontal force F turns
the velocity vector at

    d(velH)/dt = F_lat / (m * |v|)          F_lat = component of F perpendicular to v

Only the PERPENDICULAR component enters, so this is immune to the longitudinal
drive force (+0xb14) and to gravity -- neither of which is included in the summed
per-wheel force, and both of which would pollute a raw dv-vs-F/m comparison.

We measure d(velH) per frame directly, predict it from the measured force, and
report the ratio as an EFFECTIVE dt per frame, per side, per speed band.

  - If both sides yield the SAME effective dt, velocity responds to force
    identically on both, the defect is NOT in the force->velocity application,
    and the remaining suspect is the orientation half (bodyH).
  - If the PORT's effective dt is LARGER, the port's velocity heading over-rotates
    for the force it is given -- i.e. there is an extra term aligning velocity to
    the body -- and that is a mechanism, not a restatement.

INPUTS (both already on disk; no game run, no Frida hook, no Ghidra)
--------------------------------------------------------------------
ORIGINAL: an MSD1 capture. Every field below is in the raw 0xd04 payload.
    velocity x/z      +0x9b0 / +0x9b8        (field_trace.py:65-67)
    forward axis x/z  +0x9d4 / +0x9dc        (field_trace.py:69-70)
    grounded          +0x9e0  (4.0 = all)    (field_trace.py:71)
    speed             +0x9e4                 (field_trace.py:72)
    steerAng0         +0x1a8                 (field_trace.py:63)
    per-wheel force   wheel base 0x1a4 + w*0xc4, +0x70/+0x74/+0x78
                      -> x at 0x214/0x2d8/0x39c/0x460, z at +8
                      (VehiclePhysicsRun.cpp:785-793)

PORT: a motion_diag.log written by MASHED_MOTION_DIAG=1
      (VehiclePhysicsRun.cpp:769-820). Fields used: ftot=[x,y,z], horiz, velH,
      bodyH, slip, sp, steer, gnd, reseed, av=(x,y,z).
      NOTE the port does NOT log a signed velocity Y (VehiclePhysicsRun.cpp:778-782
      logs no Y velocity component), so this tool is HORIZONTAL-PLANE ONLY on both
      sides. That is sound here: slip is defined in the plane on both sides, and the
      original's forward axis +0x9d8 is exactly 0.0 in all driving frames.

REGIME CONTROL (A8 trap 6: control for regime before comparing)
---------------------------------------------------------------
  - driving-only (speed >= --min-speed, default 500; below that the force ratio is
    known to be 2.0x and the band is not informative)
  - grounded only
  - consecutive frame indices only (a gap is not an integration step)
  - PORT: frames flagged reseed=1 are EXCLUDED. A reseed is a heading
    discontinuity, not an integration step; including them inflates yaw bands 2x.
  - both sides report their steer distribution so a steer mismatch is visible
    rather than silent.
  - SAMPLE COUNT IS PRINTED FIRST. A healthy port run is ~1100 motion lines; this
    recipe intermittently stalls in the frontend nav and yields 0 or ~400, and a
    short run has a different speed distribution whose bands are NOT comparable.

Read-only. Does not execute the game.
"""
import argparse
import math
import re
import struct
import sys

MAGIC = b"MSD1"

# --- original record offsets (citations in the module docstring) ---
OFF_VELX, OFF_VELZ = 0x9b0, 0x9b8
OFF_FWDX, OFF_FWDZ = 0x9d4, 0x9dc
OFF_GND, OFF_SPEED = 0x9e0, 0x9e4
OFF_STEER0 = 0x1a8
WHEEL_FX = (0x214, 0x2d8, 0x39c, 0x460)   # +0 x, +8 z

# MEASURED live on the original, seventeenth follow-up: mass 1000.0
MASS = 1000.0

# speed bands: the three the A8 note reports, plus the low band for context
BANDS = ((500, 1000), (1000, 1500), (1500, 2000), (2000, 2600), (2600, 5000))


def wrap(a):
    while a > math.pi:
        a -= 2.0 * math.pi
    while a < -math.pi:
        a += 2.0 * math.pi
    return a


# ---------------------------------------------------------------- original side

def load_msd(path):
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


def f32(payload, off):
    return struct.unpack_from("<f", payload, off)[0]


def samples_original(path):
    """-> list of dicts, one per frame, in frame order."""
    _, _, frames = load_msd(path)
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        vx, vz = f32(p, OFF_VELX), f32(p, OFF_VELZ)
        fx = sum(f32(p, b) for b in WHEEL_FX)
        fz = sum(f32(p, b + 8) for b in WHEEL_FX)
        fwdx, fwdz = f32(p, OFF_FWDX), f32(p, OFF_FWDZ)
        horiz = math.hypot(vx, vz)
        out.append(dict(
            idx=idx, vx=vx, vz=vz, fx=fx, fz=fz,
            horiz=horiz,
            velH=math.atan2(vz, vx) if horiz > 1e-3 else 0.0,
            bodyH=math.atan2(fwdz, fwdx),
            sp=f32(p, OFF_SPEED),
            gnd=f32(p, OFF_GND),
            steer=f32(p, OFF_STEER0),
            reseed=0,
        ))
    return out


# -------------------------------------------------------------------- port side

RE_FTOT = re.compile(r"ftot=\[([^\]]*)\]")
RE_KV = re.compile(r"\b(reseed|gnd|sp|horiz|velH|bodyH|slip|steer)=([-+0-9.eE]+)")


def samples_port(path):
    """-> list of dicts, one per motion_diag line, in file order."""
    out = []
    with open(path, "r", errors="replace") as fh:
        for lineno, line in enumerate(fh):
            if "ftot=[" not in line or "velH=" not in line:
                continue
            m = RE_FTOT.search(line)
            if not m:
                continue
            try:
                ft = [float(x) for x in m.group(1).split(",")]
            except ValueError:
                continue
            if len(ft) != 3:
                continue
            kv = {k: float(v) for k, v in RE_KV.findall(line)}
            if "velH" not in kv or "horiz" not in kv:
                continue
            out.append(dict(
                idx=lineno, vx=kv["horiz"] * math.cos(kv["velH"]),
                vz=kv["horiz"] * math.sin(kv["velH"]),
                fx=ft[0], fz=ft[2],
                horiz=kv["horiz"], velH=kv["velH"], bodyH=kv.get("bodyH", 0.0),
                sp=kv.get("sp", 0.0), gnd=kv.get("gnd", 0.0),
                steer=kv.get("steer", 0.0), reseed=int(kv.get("reseed", 0)),
            ))
    return out


# ------------------------------------------------------------------- the measure

def pairs(rows, min_speed, require_grounded, drop_reseed, steer_min):
    """Consecutive-frame pairs that survive the regime filter."""
    out = []
    for a, b in zip(rows, rows[1:]):
        if b["idx"] != a["idx"] + 1:
            continue                      # a gap is not an integration step
        if drop_reseed and b["reseed"]:
            continue                      # heading discontinuity, not integration
        if require_grounded and (a["gnd"] < 3.5 or b["gnd"] < 3.5):
            continue
        if a["horiz"] < min_speed or b["horiz"] < min_speed:
            continue
        # A8 trap 6: control for regime -- matched speed AND MATCHED STEER. The
        # original captures are a full-lock HELD turn; a port run whose steer
        # varies over its whole range is a different regime, and comparing the two
        # medians is an uncontrolled comparison. Units differ per side (the
        # original's +0x1a8 is a wheel angle in degrees, the port's `steer` is the
        # normalised io.steer input), so the threshold is supplied per side.
        if steer_min is not None and (a["steer"] < steer_min or b["steer"] < steer_min):
            continue
        out.append((a, b))
    return out


def measure(rows, label, min_speed, require_grounded, drop_reseed, steer_min=None):
    ps = pairs(rows, min_speed, require_grounded, drop_reseed, steer_min)
    print(f"\n=== {label} ===")
    print(f"  raw samples {len(rows)}   usable consecutive pairs {len(ps)}"
          f"   (min horiz speed {min_speed}, grounded={require_grounded},"
          f" drop_reseed={drop_reseed})")
    if not ps:
        print("  NO USABLE PAIRS -- check the sample count before drawing anything.")
        return {}

    drv = [r for r in rows if r["horiz"] >= min_speed
           and (steer_min is None or r["steer"] >= steer_min)]
    if drv:
        med = sorted(r["horiz"] for r in drv)[len(drv) // 2]
        st = sorted(r["steer"] for r in drv)
        print(f"  driving-only horiz median {med:.2f}   n={len(drv)}")
        print(f"  steer over driving frames: min {st[0]:+.3f}  "
              f"median {st[len(st)//2]:+.3f}  max {st[-1]:+.3f}")

    res = {}
    print(f"  {'band':<12} {'n':>5} {'dvelH/fr':>10} {'pred/s':>11} "
          f"{'eff dt':>9} {'slip':>8} {'|F|':>10} {'Flat/|F|':>9}")
    for lo, hi in BANDS:
        sel = [(a, b) for a, b in ps if lo <= a["horiz"] < hi]
        if len(sel) < 8:
            continue
        dvh, pred, slips, fmag, latfrac = [], [], [], [], []
        for a, b in sel:
            v = math.hypot(a["vx"], a["vz"])
            if v < 1e-3:
                continue
            # unit velocity, and the left-normal in the same atan2(z,x) convention
            ux, uz = a["vx"] / v, a["vz"] / v
            nx, nz = -uz, ux
            flat = a["fx"] * nx + a["fz"] * nz
            f = math.hypot(a["fx"], a["fz"])
            dvh.append(wrap(b["velH"] - a["velH"]))
            pred.append(flat / (MASS * v))          # rad per second
            slips.append(abs(wrap(a["velH"] - a["bodyH"])))
            fmag.append(f)
            latfrac.append(abs(flat) / f if f > 1e-6 else 0.0)
        if len(dvh) < 8:
            continue
        # per-sample ratio, then median -- NOT a ratio of sums. Dividing a total by a
        # span across a changing regime has manufactured a fake defect in this project
        # twice; segment and use per-sample statistics.
        rr = [d / p for d, p in zip(dvh, pred) if abs(p) > 1e-9]
        rr.sort()
        eff = rr[len(rr) // 2] if rr else float("nan")
        mdvh = sorted(dvh)[len(dvh) // 2]
        mpred = sorted(pred)[len(pred) // 2]
        mslip = sorted(slips)[len(slips) // 2]
        mf = sorted(fmag)[len(fmag) // 2]
        mlf = sorted(latfrac)[len(latfrac) // 2]
        print(f"  {lo}-{hi:<7} {len(dvh):>5} {mdvh:>10.5f} {mpred:>11.5f} "
              f"{eff:>9.5f} {mslip:>8.4f} {mf:>10.0f} {mlf:>9.4f}")
        res[(lo, hi)] = dict(n=len(dvh), dvelH=mdvh, pred=mpred, eff_dt=eff,
                             slip=mslip, fmag=mf, latfrac=mlf)
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--orig", help="original-side MSD1 capture (.msd)")
    ap.add_argument("--port", help="port-side motion_diag.log (MASHED_MOTION_DIAG=1)")
    ap.add_argument("--min-speed", type=float, default=500.0)
    ap.add_argument("--no-grounded-filter", action="store_true")
    ap.add_argument("--orig-steer-min", type=float, default=None,
                    help="keep original frames with steerAng0 (+0x1a8, DEGREES) >= this."
                         " The steerR capture is a held full-lock right at ~+33.9 deg.")
    ap.add_argument("--port-steer-min", type=float, default=None,
                    help="keep port frames with io.steer (NORMALISED -1..+1) >= this."
                         " Use ~0.9 to match a full-lock original capture.")
    args = ap.parse_args()
    if not args.orig and not args.port:
        ap.error("give --orig and/or --port")

    grounded = not args.no_grounded_filter
    o = p = {}
    if args.orig:
        o = measure(samples_original(args.orig), f"ORIGINAL  {args.orig}",
                    args.min_speed, grounded, False, args.orig_steer_min)
    if args.port:
        p = measure(samples_port(args.port), f"PORT      {args.port}",
                    args.min_speed, grounded, True, args.port_steer_min)

    if o and p:
        print("\n=== EFFECTIVE dt PER FRAME, side by side ===")
        print("  the momentum identity d(velH)/dt = F_lat/(m*|v|), solved for dt.")
        print(f"  {'band':<12} {'ORIG':>10} {'PORT':>10} {'port/orig':>11}"
              f"   {'slip orig':>10} {'slip port':>10} {'slip x':>7}")
        for band in BANDS:
            if band not in o or band not in p:
                continue
            a, b = o[band], p[band]
            r = b["eff_dt"] / a["eff_dt"] if abs(a["eff_dt"]) > 1e-12 else float("nan")
            sx = a["slip"] / b["slip"] if b["slip"] > 1e-9 else float("nan")
            print(f"  {band[0]}-{band[1]:<7} {a['eff_dt']:>10.5f} {b['eff_dt']:>10.5f}"
                  f" {r:>11.3f}   {a['slip']:>10.4f} {b['slip']:>10.4f} {sx:>7.2f}")
        print("\n  READ IT THIS WAY: port/orig ~= 1.0 means velocity responds to force")
        print("  identically on both sides -- the force->velocity application is NOT")
        print("  the defect, and the orientation half (bodyH) is what is left.")
        print("  port/orig > 1 means the port's velocity heading over-rotates for the")
        print("  force it is given, which IS a mechanism for a short slip angle.")


if __name__ == "__main__":
    main()
