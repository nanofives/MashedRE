# hudburst_to_drawlist.py — convert a race_hud_burst.py free-run capture into the
# per-frame label form that re/tools/drawlist_diff.py expects.
#
# race_hud_burst.py --free-run collects into ONE bucket and tags every draw with
# its true frame index (the d3d9 shim's Present counter, see
# re/analysis/race_hud_capture_20260902.md Finding 5). drawlist_diff.py instead
# wants {"<label>": [records...]} with one label per frame, matching the
# standalone's MASHED_DBG_DRAWSTREAM output ({"f<N>": [...]}). This regroups.
#
# Side note on schema: the original-side records carry no "s" field (the Frida
# capture cannot see the mirrored blend/texture state that the standalone bridge
# records), which drawlist_diff.py already tolerates (s defaults None). That
# also means --exclude-tex only ever filters side B.
#
# Usage:
#   py -3.12 re/tools/hudburst_to_drawlist.py log/race_hud_burst.json \
#       -o log/race_hud_frames.json [--frames N] [--prefix f]
import argparse
import json
import sys
from collections import defaultdict
from pathlib import Path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("-o", "--out", required=True)
    ap.add_argument("--frames", type=int, default=None,
                    help="keep only the first N distinct frames")
    ap.add_argument("--prefix", default="f")
    args = ap.parse_args()

    data = json.loads(Path(args.src).read_text(encoding="utf-8"))

    by_frame = defaultdict(list)
    untagged = 0
    for label, rows in data.items():
        for r in rows:
            f = r.get("f", -1)
            if f is None or f < 0:
                untagged += 1
                continue
            rec = {"v": r["v"]}
            if r.get("r"):
                rec["r"] = r["r"]
            by_frame[f].append(rec)

    if untagged:
        print(f"WARNING: {untagged} draws had no valid frame tag (f=-1) and were "
              f"dropped. That means the shim's Present counter was not resolved "
              f"during capture — the capture is not frame-accurate.",
              file=sys.stderr)
    if not by_frame:
        print("ERROR: no frame-tagged draws at all; nothing to convert.",
              file=sys.stderr)
        return 2

    frames = sorted(by_frame)
    if args.frames:
        frames = frames[:args.frames]

    out = {f"{args.prefix}{n}": by_frame[n] for n in frames}
    Path(args.out).write_text(json.dumps(out, indent=1))

    counts = [len(by_frame[n]) for n in frames]
    print(f"-> {args.out}")
    print(f"   {len(frames)} frames, {sum(counts)} draws, "
          f"per-frame min/max {min(counts)}/{max(counts)}"
          + ("  (uniform)" if min(counts) == max(counts) else
             "  (NON-UNIFORM — frame boundaries may be wrong)"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
