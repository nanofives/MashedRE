"""rfx: build hud_inputs.txt (MASHED_HUD_REPLAY, PREREG_rfx.md amendment 1) from the original-side
probe run `camera_probe.py --frame --early --hud-out` (re/frida/camera_probe.py hudglobals).

    py -3.12 make_hud_inputs.py <hud.json> <trace.csv> <out.txt> [--from 30 --to 105]

Takes every sample whose row lies between GO+from and GO+to (GO = first trace row with ba8c 6),
requires the mode / slot / score inputs to be identical across them, and writes them. The probe's
own panel records are printed as the check (never written).
"""
import csv
import json
import sys


def main() -> int:
    hud, trace, out = sys.argv[1], sys.argv[2], sys.argv[3]
    lo = int(sys.argv[sys.argv.index("--from") + 1]) if "--from" in sys.argv else 30
    hi = int(sys.argv[sys.argv.index("--to") + 1]) if "--to" in sys.argv else 105
    rows = list(csv.DictReader(open(trace)))
    go = next(i for i, r in enumerate(rows) if r["ba8c"] == "6")
    sm = [s for s in json.load(open(hud)) if go + lo <= s["row"] <= go + hi]
    if not sm:
        print("no samples in the window"); return 1
    key = lambda s: (s["ea64"], s["e9fc"], s["fd0"], s["n4d0"], tuple(s["scores"]),
                     tuple(tuple(x[:3]) for x in s["slots"]))
    keys = {key(s) for s in sm}
    print(f"GO row {go}; {len(sm)} samples in rows {go + lo}..{go + hi}; distinct inputs: {len(keys)}")
    if len(keys) != 1:
        for s in sm: print("  ", s["row"], key(s))
        return 1
    s = sm[0]
    with open(out, "w", newline="\n") as f:
        f.write(f"# measured on the original: {hud} rows {go + lo}..{go + hi} after GO row {go}\n")
        f.write(f"team_play {s['ea64']}\nround_type {s['e9fc']}\nmode_fd0 {s['fd0']}\nn4d0 {s['n4d0']}\n")
        f.write("score " + " ".join(str(v) for v in s["scores"]) + "\n")
        for i, sl in enumerate(s["slots"]):
            f.write(f"player {i} {sl[0]} {sl[1]} {sl[2]}\n")
    for p in s["panels"]:
        print(f"  check panel {p['idx']}: members {p['members'][:p['count']]} count {p['count']} "
              f"flags {p['flags']:#x} visible slots {[i for i, v in enumerate(p['vis']) if v]}")
    print(f"wrote {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
