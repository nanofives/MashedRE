#!/usr/bin/env python3
"""U-9194 RETRACTION: what record +0x10 actually is, and the denominator G-ARM should
have used.

Evidence doc: verify/d3_arm_20261005/RESULT_ARMRETRACT.md.

WHY THIS EXISTS. verify/d3_arm_20261005/RESULT_ARM.md reported that the original takes
FUN_0046e9e0's non-zero omega arm on "2558 of 3623 frames (70.60%)" while the port takes
the zero arm on 100% of substeps, and filed that as a structural defect (U-9194). The
figure is arithmetically right and materially misleading: its denominator is ALL frames,
and car 1 drives for only ~220 of them before being eliminated. Restricted to frames where
the car's POSITION CHANGES, +0x10 == 0 on 222 of 222 -- the same arm the port takes.

The reusable lesson, and the reason this is a tool and not a one-off: a flag that is
"not constant" over a capture is not therefore a per-frame fork. Count its TRANSITIONS and
score it over the population the claim is about, not over the file.

Usage:
  py -3.12 re/tools/ai_armregime.py <msd> [<msd> ...] [--offset 0x10] [--car-label N]
           [--min-frac 0.99] [--transition-diff] [--json]
"""
import argparse
import json
import statistics
import struct
import sys

# record offsets. Grepped against re/analysis/**/0046e9e0.md before use:
# +0x10 is the omega-arm gate (the plate's ESI[4]) and is given NO semantic name --
# no plate assigns it one. +0x958/+0x960 are the record's world X/Z (memory
# msd-world-position-is-the-0x928-matrix-row). +0x9e4 is the linear speed magnitude.
OFF_ARM_DEFAULT = 0x10
OFF_X, OFF_Z, OFF_SPEED = 0x958, 0x960, 0x9e4


def load(path):
    b = open(path, "rb").read()
    assert b[:4] == b"MSD1", "%s: not MSD1" % path
    rec, base, _ = struct.unpack_from("<III", b, 4)
    frames, payloads, off = [], [], 16
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from("<I", b, off)
        frames.append(fi)
        payloads.append(b[off + 4: off + 4 + rec])
        off += 4 + rec
    return frames, payloads, rec, base


def views(u):
    i, = struct.unpack("<i", struct.pack("<I", u))
    f, = struct.unpack("<f", struct.pack("<I", u))
    return {"u32": "0x%08x" % u, "i32": i, "f32": f}


def analyse(path, off_arm, min_frac, want_diff):
    frames, P, rec, base = load(path)
    n = len(P)

    def f32(i, o): return struct.unpack_from("<f", P[i], o)[0]
    def u32(i, o): return struct.unpack_from("<I", P[i], o)[0]

    arm = [u32(i, off_arm) for i in range(n)]

    # runs, so "not constant" cannot be mistaken for "per-frame fork"
    runs, cur, ln = [], arm[0], 1
    for a in arm[1:]:
        if a == cur:
            ln += 1
        else:
            runs.append({"value": views(cur), "length": ln})
            cur, ln = a, 1
    runs.append({"value": views(cur), "length": ln})

    moving = [i for i in range(n - 1)
              if (f32(i + 1, OFF_X) - f32(i, OFF_X)) ** 2 +
                 (f32(i + 1, OFF_Z) - f32(i, OFF_Z)) ** 2 > 0.0]
    movz = sum(1 for i in moving if arm[i] == 0)
    allz = sum(1 for a in arm if a == 0)
    spd = [i for i in range(n) if f32(i, OFF_SPEED) != 0.0]

    out = {"msd": path, "frames": n, "rec_size": "0x%x" % rec,
           "base_va": "0x%08x" % base,
           "frame_idx_range": [frames[0], frames[-1]],
           "frame_idx_contiguous": frames == list(range(frames[0], frames[-1] + 1)),
           "arm_offset": "+0x%x" % off_arm,
           "n_transitions": len(runs) - 1, "runs": runs,
           "all_frames_zero": allz, "all_frames_frac_zero": allz / n,
           "n_moving": len(moving),
           "moving_zero": movz,
           "moving_frac_zero": (movz / len(moving)) if moving else None,
           "moving_frac_required": min_frac,
           "n_nonzero_speed": len(spd),
           "nonzero_speed_range": [min(spd), max(spd)] if spd else None,
           "distance_travelled": sum(
               ((f32(i + 1, OFF_X) - f32(i, OFF_X)) ** 2 +
                (f32(i + 1, OFF_Z) - f32(i, OFF_Z)) ** 2) ** 0.5
               for i in range(n - 1)),
           "pass": bool(moving) and movz >= min_frac * len(moving)}

    if want_diff and len(runs) - 1 == 1:
        k = runs[0]["length"]            # first frame of the second run
        changed = []
        for o in range(0, rec - 3, 4):
            a, c = u32(k - 1, o), u32(k, o)
            if a != c:
                changed.append({"offset": "+0x%03x" % o,
                                "from": views(a), "to": views(c)})
        out["transition_at_frame_idx"] = [frames[k - 1], frames[k]]
        out["transition_changed_dwords"] = len(changed)
        out["transition_total_dwords"] = rec // 4
        out["transition_fields"] = changed[:40]
    return out


def report(o):
    print("%s  frames %d  rec %s  base %s  frame_idx %d..%d%s"
          % (o["msd"], o["frames"], o["rec_size"], o["base_va"],
             o["frame_idx_range"][0], o["frame_idx_range"][1],
             "" if o["frame_idx_contiguous"] else "  (GAPS PRESENT)"))
    print("  %s transitions: %d   runs: %s"
          % (o["arm_offset"], o["n_transitions"],
             ", ".join("%s x%d" % (r["value"]["i32"], r["length"]) for r in o["runs"][:6])))
    print("  THE WRONG DENOMINATOR  -- all frames:    == 0 on %d of %d = %.4f%%"
          % (o["all_frames_zero"], o["frames"], 100.0 * o["all_frames_frac_zero"]))
    if o["n_moving"]:
        print("  THE RIGHT DENOMINATOR -- MOVING frames: == 0 on %d of %d = %.4f%%  "
              "(need >= %.1f%%: %s)"
              % (o["moving_zero"], o["n_moving"], 100.0 * o["moving_frac_zero"],
                 100.0 * o["moving_frac_required"], "PASS" if o["pass"] else "FAIL"))
    else:
        print("  no moving frames in this capture")
    print("  distance travelled %.3f; non-zero-speed frames %d%s"
          % (o["distance_travelled"], o["n_nonzero_speed"],
             (" in %d..%d" % tuple(o["nonzero_speed_range"])) if o["nonzero_speed_range"] else ""))
    if "transition_changed_dwords" in o:
        print("  single transition at frame_idx %d -> %d changes %d of %d record dwords"
              % (o["transition_at_frame_idx"][0], o["transition_at_frame_idx"][1],
                 o["transition_changed_dwords"], o["transition_total_dwords"]))
        for c in o["transition_fields"][:12]:
            print("      %s  %s -> %s   i %d -> %d   f %g -> %g"
                  % (c["offset"], c["from"]["u32"], c["to"]["u32"],
                     c["from"]["i32"], c["to"]["i32"], c["from"]["f32"], c["to"]["f32"]))


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("msd", nargs="+")
    ap.add_argument("--offset", default=hex(OFF_ARM_DEFAULT))
    ap.add_argument("--min-frac", type=float, default=0.99,
                    help="required share of MOVING frames with the field == 0")
    ap.add_argument("--transition-diff", action="store_true",
                    help="when there is exactly one transition, diff the record across it")
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args(argv)
    off = int(a.offset, 0)
    outs = [analyse(p, off, a.min_frac, a.transition_diff) for p in a.msd]
    if a.json:
        print(json.dumps(outs, indent=1))
    else:
        for o in outs:
            report(o)
            print()
        ok = [o for o in outs if o["pass"]]
        print("%d of %d captures PASS (field == 0 on >= %.1f%% of moving frames)"
              % (len(ok), len(outs), 100.0 * a.min_frac))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
