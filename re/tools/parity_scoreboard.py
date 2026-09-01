#!/usr/bin/env python3
"""parity_scoreboard.py -- committed, append-only per-area parity history.

Parse-only by design (the fork chosen 2026-08-31): captures + diffs are run by
hand each round using re/parity/recipes.toml; this tool ingests a diff tool's
stdout, extracts the verdict/metric, and appends one row to a committed
scoreboard so "did round N improve area X" is a diff, not a re-read of dated
handoff prose. It NEVER launches MASHED, Frida, or a diff -- zero machine binding.

Scoreboard: re/parity/scoreboard.tsv (append-only, git-committed).
  columns: round  date  area  recipe  kind  verdict  metric  mean  pct_over  note

Verdict semantics:
  drawlist  GREEN | RED       (from `VERDICT: GREEN/RED (match=.. mismatch=..)`)
  imgdiff   measured | FAIL   (imgdiff only fails when --fail-mean was passed)
  either    BLOCKED           (recipe has a non-empty blocker in recipes.toml)

Usage:
  py -3.12 re/tools/parity_scoreboard.py record --recipe render.race_first_frame --round 1 --from out.txt
  py -3.12 re/tools/parity_scoreboard.py record --recipe render.race_first_frame_arctic --round 1 --blocked
  py -3.12 re/tools/parity_scoreboard.py show --area render
  py -3.12 re/tools/parity_scoreboard.py show            # all areas, latest per recipe
"""
import argparse
import csv
import datetime as _dt
import re
import sys
from pathlib import Path

try:
    import tomllib  # py3.11+
except ModuleNotFoundError:  # pragma: no cover
    tomllib = None

ROOT = Path(__file__).resolve().parents[2]
RECIPES = ROOT / "re" / "parity" / "recipes.toml"
BOARD = ROOT / "re" / "parity" / "scoreboard.tsv"
COLUMNS = ["round", "date", "area", "recipe", "kind", "verdict", "metric", "mean", "pct_over", "note"]

RE_DRAWLIST = re.compile(
    r"VERDICT:\s+(GREEN|RED)\s+\(match=(\d+)\s+mismatch=(\d+)\s+missing=(\d+)\s+extra=(\d+)\)")
RE_IMG_MEAN = re.compile(r"\(all\s+([\d.]+)\)")
RE_IMG_OVER = re.compile(r"over threshold\s+\d+:\s+\d+\s+\(([\d.]+)%\)")
RE_IMG_FAIL = re.compile(r"^FAIL:\s+mean", re.MULTILINE)


def _flatten(d: dict, prefix: str = "") -> dict:
    """TOML [a.b] parses as nested tables; flatten back to 'a.b' keys.
    A leaf recipe is any table carrying an 'area' field."""
    out = {}
    for k, v in d.items():
        key = f"{prefix}{k}"
        if isinstance(v, dict) and "area" in v:
            out[key] = v
        elif isinstance(v, dict):
            out.update(_flatten(v, key + "."))
    return out


def load_recipes() -> dict:
    if tomllib is None or not RECIPES.exists():
        return {}
    with RECIPES.open("rb") as fh:
        return _flatten(tomllib.load(fh))


def parse_output(text: str, kind: str) -> dict:
    """Return {verdict, metric, mean, pct_over} extracted from a diff stdout."""
    if kind == "drawlist":
        m = RE_DRAWLIST.search(text)
        if not m:
            raise ValueError("no drawlist VERDICT line found in output "
                             "(expected 'VERDICT: GREEN/RED (match=.. mismatch=..)')")
        verdict, match, mism, miss, extra = m.group(1), *map(int, m.groups()[1:])
        total = match + mism + miss + extra
        return {"verdict": verdict, "metric": f"{match}/{total}", "mean": "", "pct_over": ""}
    if kind == "imgdiff":
        mm = RE_IMG_MEAN.search(text)
        if not mm:
            raise ValueError("no imgdiff mean line found (expected '(all X.XX)')")
        mean = mm.group(1)
        over = RE_IMG_OVER.search(text)
        pct = over.group(1) if over else ""
        verdict = "FAIL" if RE_IMG_FAIL.search(text) else "measured"
        return {"verdict": verdict, "metric": f"mean={mean}", "mean": mean, "pct_over": pct}
    raise ValueError(f"unknown kind {kind!r}")


def read_board() -> list:
    if not BOARD.exists():
        return []
    with BOARD.open(encoding="utf-8", newline="") as fh:
        return list(csv.DictReader(fh, delimiter="\t"))


def write_board(rows: list) -> None:
    BOARD.parent.mkdir(parents=True, exist_ok=True)
    with BOARD.open("w", encoding="utf-8", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=COLUMNS, delimiter="\t")
        w.writeheader()
        w.writerows(rows)


def cmd_record(args) -> int:
    recipes = load_recipes()
    rec = recipes.get(args.recipe)
    if rec is None:
        print(f"FATAL: recipe {args.recipe!r} not in {RECIPES}", file=sys.stderr)
        print(f"  known: {', '.join(sorted(recipes))}", file=sys.stderr)
        return 2
    area = rec.get("area", "")
    kind = rec.get("kind", "")
    blocker = rec.get("blocker", "")

    if args.blocked or blocker:
        parsed = {"verdict": "BLOCKED", "metric": "", "mean": "", "pct_over": ""}
        note = args.note or (blocker[:120] if blocker else "marked blocked")
    else:
        if not args.from_file:
            print("FATAL: need --from <diff stdout file> unless --blocked", file=sys.stderr)
            return 2
        text = Path(args.from_file).read_text(encoding="utf-8", errors="replace")
        try:
            parsed = parse_output(text, kind)
        except ValueError as e:
            print(f"FATAL: {e}", file=sys.stderr)
            return 2
        note = args.note or ""

    row = {
        "round": str(args.round), "date": _dt.date.today().isoformat(),
        "area": area, "recipe": args.recipe, "kind": kind,
        "verdict": parsed["verdict"], "metric": parsed["metric"],
        "mean": parsed["mean"], "pct_over": parsed["pct_over"], "note": note,
    }
    rows = read_board()
    rows.append(row)
    write_board(rows)
    print(f"recorded  round {args.round}  {args.recipe}  -> {parsed['verdict']} {parsed['metric']}")

    # delta vs the previous row for this recipe
    prev = [r for r in rows[:-1] if r["recipe"] == args.recipe]
    if prev:
        p = prev[-1]
        print(f"  prev (round {p['round']}): {p['verdict']} {p['metric']}"
              f"{'  <- CHANGED' if p['verdict'] != row['verdict'] or p['metric'] != row['metric'] else '  (no change -> parity flat)'}")
    return 0


def cmd_show(args) -> int:
    rows = read_board()
    if not rows:
        print("scoreboard empty (no rounds recorded yet)")
        return 0
    if args.area:
        rows = [r for r in rows if r["area"] == args.area]
    # latest per recipe, plus its predecessor for delta
    by_recipe = {}
    for r in rows:
        by_recipe.setdefault(r["recipe"], []).append(r)
    print(f"{'recipe':<34} {'rnd':<4} {'verdict':<9} {'metric':<14} {'delta':<20}")
    for recipe in sorted(by_recipe):
        hist = by_recipe[recipe]
        cur = hist[-1]
        delta = ""
        if len(hist) > 1:
            p = hist[-2]
            delta = f"was {p['verdict']} {p['metric']} (r{p['round']})" if (
                p["verdict"] != cur["verdict"] or p["metric"] != cur["metric"]) else "flat"
        print(f"{recipe:<34} {cur['round']:<4} {cur['verdict']:<9} {cur['metric']:<14} {delta:<20}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("record")
    r.add_argument("--recipe", required=True)
    r.add_argument("--round", required=True)
    r.add_argument("--from", dest="from_file", default="")
    r.add_argument("--blocked", action="store_true", help="record BLOCKED (recipe has a blocker or capture impossible this round)")
    r.add_argument("--note", default="")
    r.set_defaults(func=cmd_record)
    s = sub.add_parser("show")
    s.add_argument("--area", default="")
    s.set_defaults(func=cmd_show)
    args = ap.parse_args()
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
