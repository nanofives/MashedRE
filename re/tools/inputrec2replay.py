#!/usr/bin/env python3
"""RO-INPUT-RECORD: convert a detorig.py --rec log (inputrec.json) into a replay.json input list, and compare two logs per tick.

  convert:  inputrec2replay.py convert <inputrec.json> --out <replay.json> [--base <replay.json>]
  compare:  inputrec2replay.py compare <inputrec_A.json> <inputrec_B.json> [--ticks N]

The log rows are [absolute tick, control, value]: the value FUN_00497310 (0x00497310) returned for player 0 whenever it
changed from the previous return for that control (detorig.js, cfg.rec). Rows become [tick - anchor, 0, control, value]
(DESIGN_REPLAY-ORACLE.md section 2). Per-tick state of a log = for every control, the last row at or before the tick.
"""
import argparse, json, sys


def load(p):
    d = json.load(open(p, encoding="utf-8"))
    if d.get("format") != "mashed-inputrec-1":
        raise SystemExit(f"{p}: not a mashed-inputrec-1 file")
    if d.get("overflow"):
        raise SystemExit(f"{p}: {d['overflow']} rows lost to log overflow")
    return d


def to_input(d):
    a = d["tick_anchor"]
    return [[t - a, 0, c, v] for t, c, v in d["rows"]]


def per_tick(rows, n):
    """rows [tick_rel, player, control, value] -> list of n dicts control->value (player 0)."""
    out, cur, i = [], {}, 0
    rows = sorted(rows, key=lambda r: r[0])
    for t in range(n):
        while i < len(rows) and rows[i][0] <= t:
            cur[rows[i][2]] = rows[i][3]; i += 1
        out.append({c: v for c, v in cur.items() if v})
    return out


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("convert"); c.add_argument("log"); c.add_argument("--out", required=True); c.add_argument("--base", default="")
    k = sub.add_parser("compare"); k.add_argument("a"); k.add_argument("b"); k.add_argument("--ticks", type=int, default=0)
    args = ap.parse_args()
    if args.cmd == "convert":
        d = load(args.log)
        base = json.load(open(args.base, encoding="utf-8")) if args.base else {"format": "mashed-replay-1"}
        base["input"] = to_input(d)
        json.dump(base, open(args.out, "w", encoding="utf-8"), indent=1)
        print(f"{len(base['input'])} input rows -> {args.out}")
        return 0
    a, b = load(args.a), load(args.b)
    ra, rb = to_input(a), to_input(b)
    n = args.ticks or (max([r[0] for r in ra + rb] + [0]) + 1)
    pa, pb = per_tick(ra, n), per_tick(rb, n)
    bad = [t for t in range(n) if pa[t] != pb[t]]
    print(f"rows A {len(ra)} B {len(rb)}, rows identical: {ra == rb}")
    print(f"per-tick compare over {n} ticks: {n - len(bad)} identical, {len(bad)} differ" + (f", first differing tick {bad[0]}" if bad else ""))
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
