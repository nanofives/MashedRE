"""Three-axis scoreboard: Reach x Understanding x Verification per hooks.csv row (SimGolf scripts/scoreboard.py).

    py -3.12 scripts/scoreboard.py            # writes re/coverage/scoreboard.tsv, prints summary + top priorities
    py -3.12 scripts/scoreboard.py --top 40

Axes (0..3 each), computed from evidence on disk; nothing is hand-entered:

  Reach (R)          from re/coverage/reach.tsv (re/frida/coverage_census.py, one-shot INT3 on the ORIGINAL):
                     number of census phases the row was hit in, of menu / race_load / race_run.
                     "?" when no reach.tsv exists yet (treated as 0 for priority).
  Understanding (U)  hooks.csv confidence: C0=0, C1=1, C2=2, C3/C4=3.
  Verification (V)   0 no diff evidence
                     1 hooks.csv frida_diff names evidence but the row is below C3
                     2 C3 (path1 / shadow / replay GREEN, re/CONFIDENCE.md)
                     3 C4 (canonical scenario with the hook live)

Priority = R * ((3 - U) + (3 - V)): code the original runs that is neither understood nor verified first.
Unreached code (R = 0) never outranks reached code. Ranking aid only: it moves no C-level.
"""
from __future__ import annotations

import argparse
import csv
import pathlib
from collections import Counter

ROOT = pathlib.Path(__file__).resolve().parents[1]
C_TO_U = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 3}
C_TO_V = {"C3": 2, "C4": 3}
PHASES = ("reached_menu", "reached_race_load", "reached_race_run")


def reach(root: pathlib.Path) -> tuple[dict[str, dict], bool]:
    f = root / "re" / "coverage" / "reach.tsv"
    if not f.exists():
        return {}, False
    with open(f, newline="") as fh:
        return {r["rva"].lower(): r for r in csv.DictReader(fh, delimiter="\t")}, True


def build(root: pathlib.Path = ROOT) -> list[dict]:
    rch, have = reach(root)
    rows = []
    with open(root / "hooks.csv", newline="", encoding="utf-8-sig") as fh:
        for h in csv.DictReader(fh):
            rva = (h.get("rva") or "").strip().lower()
            if not rva or rva.startswith("#"):
                continue
            conf = h["confidence"].strip()
            u = C_TO_U.get(conf, 0)
            ev = (h.get("frida_diff") or "").strip()
            v = C_TO_V.get(conf, 1 if ev and ev.lower() != "na" else 0)
            r = rch.get(rva)
            phases = [p.removeprefix("reached_") for p in PHASES if r and r.get(p) == "1"]
            R = len(phases) if have else None
            rows.append({
                "rva": rva, "name": h["name"], "subsystem": h["subsystem"], "confidence": conf,
                "reach": "?" if R is None else R, "understanding": u, "verification": v,
                "priority": (R or 0) * ((3 - u) + (3 - v)), "phases": ",".join(phases),
            })
    rows.sort(key=lambda x: (-x["priority"], x["rva"]))
    return rows


def priorities(root: pathlib.Path = ROOT) -> dict[str, dict]:
    """rva -> scoreboard row; the hook consumed by re/analysis/plans/c3_filter_v4.py."""
    return {r["rva"]: r for r in build(root)}


def summary(rows: list[dict]) -> str:
    live = [r for r in rows if r["reach"] not in ("?", 0)]
    c = lambda key, rs: Counter(r[key] for r in rs)  # noqa: E731
    lines = [f"rows: {len(rows)}   reached: {len(live) if rows and rows[0]['reach'] != '?' else '?'}"]
    for axis in ("reach", "understanding", "verification"):
        lines.append(f"  {axis:13} " + "  ".join(f"{k}={v}" for k, v in
                                                 sorted(c(axis, rows).items(), key=lambda kv: str(kv[0]))))
    if live:
        und = sum(1 for r in live if r["understanding"] >= 3)
        ver = sum(1 for r in live if r["verification"] >= 2)
        lines.append(f"  reached code at C3+ (U=3): {und}/{len(live)} ({100 * und / len(live):.1f}%)   "
                     f"verified (V>=2): {ver}/{len(live)} ({100 * ver / len(live):.1f}%)")
        c2 = Counter(r["subsystem"] for r in live if r["confidence"] == "C2")
        lines.append(f"  reached C2 rows: {sum(c2.values())}, by subsystem:")
        lines += [f"    {n:5}  {s}" for s, n in c2.most_common()]
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--top", type=int, default=20)
    ap.add_argument("--root", type=pathlib.Path, default=ROOT)
    a = ap.parse_args()
    rows = build(a.root)
    out = a.root / "re" / "coverage" / "scoreboard.tsv"
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    print(summary(rows))
    print(f"\ntop {a.top} priorities (R*((3-U)+(3-V))):")
    for r in rows[:a.top]:
        print(f"  {r['rva']}  P={r['priority']:2}  R={r['reach']} U={r['understanding']} V={r['verification']}  "
              f"{r['confidence']}  {r['subsystem'][:24]:24}  {r['name'][:40]}  [{r['phases']}]")
    print(f"-> {out.relative_to(a.root)}")


if __name__ == "__main__":
    main()
