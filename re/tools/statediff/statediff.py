#!/usr/bin/env python3
"""statediff.py - diff two MSD1 state captures (see FORMAT.md).

Usage:
  py -3.12 re\\tools\\statediff\\statediff.py A.msd B.msd [--json out.json] [--max-fields N]

Reports the first divergent frame and, per dword-aligned field, the first
frame at which that field diverged plus both sides' raw values
(hex / i32 / f32). Interpretation is deliberately not attempted.
"""
import argparse
import json
import struct
import sys

MAGIC = b"MSD1"


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
                print(f"{path}: truncated record at frame {idx} (ignored)", file=sys.stderr)
                break
            frames[idx] = payload
    return rec_size, base_va, frames


def fmt_val(raw):
    """raw: 4 bytes (or fewer at tail)."""
    h = raw.hex()
    if len(raw) == 4:
        (i,) = struct.unpack("<i", raw)
        (fl,) = struct.unpack("<f", raw)
        return {"hex": h, "i32": i, "f32": None if fl != fl else round(fl, 6),
                "f32_raw": repr(fl)}
    return {"hex": h}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a")
    ap.add_argument("b")
    ap.add_argument("--json", help="write machine-readable result here")
    ap.add_argument("--max-fields", type=int, default=64)
    ap.add_argument("--mask", help="noise-floor json from a prior (stock-vs-stock) run; "
                                   "its diverging_fields offsets are excluded here")
    ap.add_argument("--until", type=int, default=None,
                    help="only compare frames with frame_idx < N (chaotic scenarios have a "
                         "bounded deterministic window; diff only inside it)")
    ap.add_argument("--shift-b", type=int, default=0,
                    help="add N to B's frame indices before aligning (corrects a cross-boot "
                         "offset in the race-GO countdown start relative to the phase-3 anchor)")
    ap.add_argument("--anchor-nonzero", type=lambda v: int(v, 0), default=None,
                    help="re-anchor BOTH captures at the first frame where the dword at this "
                         "record offset becomes nonzero (e.g. 0xbf4 = race-GO countdown start), "
                         "dropping earlier frames. Use when an in-race event clock floats "
                         "relative to the phase-3 capture anchor.")
    args = ap.parse_args()

    masked = set()
    if args.mask:
        with open(args.mask) as f:
            masked = {e["offset"] for e in json.load(f)["diverging_fields"]}

    rec_a, va_a, fa = load(args.a)
    rec_b, va_b, fb = load(args.b)
    if args.shift_b:
        fb = {idx + args.shift_b: p for idx, p in fb.items()}
    if args.anchor_nonzero is not None:
        off = args.anchor_nonzero
        def rebase(frames, name):
            for idx in sorted(frames):
                if frames[idx][off:off + 4] != b"\x00\x00\x00\x00":
                    print(f"{name}: anchor +{off:#x} nonzero at frame {idx}; rebased to 0")
                    return {i - idx: p for i, p in frames.items() if i >= idx}
            sys.exit(f"{name}: dword +{off:#x} never becomes nonzero — cannot anchor")
        fa = rebase(fa, args.a)
        fb = rebase(fb, args.b)
    if rec_a != rec_b:
        sys.exit(f"rec_size mismatch: {rec_a:#x} vs {rec_b:#x}")
    rec = rec_a

    common = sorted(set(fa) & set(fb))
    if args.until is not None:
        common = [i for i in common if i < args.until]
    if not common:
        sys.exit("no common frame indices between captures")

    first_div = None
    field_first = {}  # dword offset -> first divergent frame
    for idx in common:
        a, b = fa[idx], fb[idx]
        if a == b:
            continue
        for off in range(0, rec, 4):
            if off in masked:
                continue
            if a[off:off + 4] != b[off:off + 4]:
                if first_div is None:
                    first_div = idx
                if off not in field_first:
                    field_first[off] = idx

    result = {
        "a": args.a, "b": args.b,
        "rec_size": rec, "base_va_a": va_a, "base_va_b": va_b,
        "frames_a": len(fa), "frames_b": len(fb),
        "common_frames": len(common),
        "frame_range": [common[0], common[-1]],
        "first_divergent_frame": first_div,
        "masked_offsets": sorted(masked),
        "diverging_fields": [],
    }

    if first_div is None:
        note = f" ({len(masked)} noise-masked dwords excluded)" if masked else ""
        print(f"GREEN: {len(common)} common frames "
              f"[{common[0]}..{common[-1]}], zero divergence{note}")
    else:
        print(f"RED: first divergent frame = {first_div} "
              f"({len(field_first)} dword fields diverge over the run)")
        for off in sorted(field_first)[: args.max_fields]:
            f0 = field_first[off]
            va = fa[f0][off:off + 4]
            vb = fb[f0][off:off + 4]
            entry = {"offset": off, "offset_hex": f"+{off:#05x}",
                     "first_frame": f0,
                     "a": fmt_val(va), "b": fmt_val(vb)}
            result["diverging_fields"].append(entry)
            print(f"  +{off:#05x}  frame {f0:>6}  "
                  f"A={va.hex()} B={vb.hex()}  "
                  f"(f32 A={entry['a'].get('f32_raw')} B={entry['b'].get('f32_raw')})")
        if len(field_first) > args.max_fields:
            print(f"  ... {len(field_first) - args.max_fields} more fields "
                  f"(raise --max-fields)")

    if args.json:
        with open(args.json, "w") as f:
            json.dump(result, f, indent=1)
        print(f"json -> {args.json}")

    sys.exit(0 if first_div is None else 2)


if __name__ == "__main__":
    main()
