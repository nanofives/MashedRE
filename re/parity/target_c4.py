# target_c4.py — turn a parity-sweep report into a C4-promotion targeting queue.
#
# The sweep already classified each RE emitter function's draw output as MATCHED
# vs MISMATCHED against the original across every scenario that exercised it. A
# function whose draws matched everywhere (and never mismatched) is a high-
# probability run_diff win — that is the whole point of the targeting layer:
# convert the historical ~8% blind C2->C3 diff yield into a pre-filtered list.
#
# HARD RULE (re/CONFIDENCE.md, feedback_no_overclaiming_c_levels): a sweep match
# grants NOTHING on the C-ladder. This script only decides WHICH functions get a
# run_diff/diff-original pass; that Frida diff (hook installed in the ORIGINAL,
# 0xE9 live, canonical scenario) remains the sole C3/C4 gate.
#
# Emitter names come from mashed_re.map (STANDALONE symbols), so the bridge to
# hooks.csv (keyed by MASHED RVA) is by SYMBOL NAME (ClassName::Method). Emitters
# that don't name-match a hooks.csv row are listed separately for manual triage
# (harden later by parsing RH_ScopedInstall(Method, 0xRVA) from the reimpl).
#
# Usage:  py -3.12 re/parity/target_c4.py <run-dir>
#                 [--hooks hooks.csv] [--out re/parity/C4_TARGETS.md]
import argparse
import csv
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
HOOKS_CSV = ROOT / "hooks.csv"
DEFAULT_OUT = ROOT / "re" / "parity" / "C4_TARGETS.md"


def norm_name(n):
    """Normalize a symbol for name-join: drop '+0x..' offset, lowercase, keep
    the trailing Class::Method (the most specific, most comparable part)."""
    if not n:
        return None
    n = n.split("+")[0].strip().lower()
    return n or None


def leaf(n):
    """Trailing method token, for a looser secondary match."""
    return n.split("::")[-1] if n else None


def load_hooks(path):
    """-> ({norm_name: row}, {leaf: [rows]}) where row = dict(rva,name,sub,conf,status)."""
    by_name, by_leaf = {}, {}
    with open(path, newline="", encoding="utf-8", errors="replace") as fh:
        for r in csv.reader(fh):
            if not r or r[0].startswith("#") or r[0] == "rva":
                continue
            if len(r) < 5:
                continue
            rva, name, sub, conf, status = r[0], r[1], r[2], r[3], r[4]
            row = {"rva": rva.strip().lower(), "name": name, "sub": sub,
                   "conf": conf.strip().upper(), "status": status}
            nn = norm_name(name)
            if nn:
                by_name.setdefault(nn, row)
            lf = leaf(nn)
            if lf:
                by_leaf.setdefault(lf, []).append(row)
    return by_name, by_leaf


def load_registry_rvas():
    """Set of MASHED RVAs (hex strings) that already have a run_diff test vector."""
    try:
        sys.path.insert(0, str(ROOT / "re" / "frida"))
        from hooks_registry import HOOKS
        out = set()
        for h in HOOKS.values():
            rva = h.get("rva")
            if isinstance(rva, int):
                out.add(f"{rva:08x}")
        return out
    except Exception:
        return set()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("run_dir")
    ap.add_argument("--hooks", default=str(HOOKS_CSV))
    ap.add_argument("--out", default=str(DEFAULT_OUT))
    args = ap.parse_args()

    report = json.loads((Path(args.run_dir) / "report.json").read_text())
    stats = report.get("emitter_stats", {})
    by_name, by_leaf = load_hooks(args.hooks)
    reg = load_registry_rvas()

    targets, fix_first, unmatched = [], [], []
    for re_off, e in stats.items():
        nn = norm_name(e.get("name"))
        hook = by_name.get(nn) if nn else None
        if hook is None and nn:
            cand = by_leaf.get(leaf(nn))
            hook = cand[0] if cand and len(cand) == 1 else None   # unambiguous only
        rec = {
            "re_off": re_off, "name": e.get("name"),
            "match": e.get("match", 0), "mismatch": e.get("mismatch", 0),
            "scenarios": e.get("scenarios", []), "phases": e.get("phases", []),
            "hook": hook,
        }
        if hook is None:
            unmatched.append(rec)
            continue
        if rec["mismatch"] > 0:
            fix_first.append(rec)
        elif rec["match"] > 0 and hook["conf"] in ("C2", "C3"):
            rec["in_registry"] = hook["rva"] in reg
            targets.append(rec)
        # C0/C1 (no reimpl to diff) and C4 (done) are dropped.

    # Rank: C3 (one step from C4) before C2; then by match count desc.
    targets.sort(key=lambda r: (r["hook"]["conf"] != "C3", -r["match"]))
    fix_first.sort(key=lambda r: -r["mismatch"])

    _write(args.out, report, targets, fix_first, unmatched)
    print(f"-> {args.out}")
    print(f"targets (clean-match, C2/C3): {len(targets)}   "
          f"fix-first (mismatched): {len(fix_first)}   "
          f"unmatched emitters: {len(unmatched)}")
    return 0


def _write(out, report, targets, fix_first, unmatched):
    s = report.get("summary", {})
    L = [f"# C4 targets — from parity run {report.get('ts','')}", "",
         f"Source: `{report.get('run','')}` "
         f"({s.get('green')}/{s.get('scenarios')} scenarios GREEN). ",
         "",
         "**A sweep match is a targeting signal, not C4 evidence.** Run "
         "`re/frida/run_diff.py <hook>` (hook installed in the original, "
         "canonical scenario) on these; only a GREEN there + `re-classify` "
         "promotes to C4.", "",
         "## Clean-match targets (C2/C3, zero sweep mismatches) — run_diff these",
         "",
         "| rank | MASHED rva | confidence | name | match | scenarios | registry | recommend |",
         "|---|---|---|---|---|---|---|---|"]
    for i, r in enumerate(targets, 1):
        h = r["hook"]
        recommend = "run_diff" if r.get("in_registry") else "add hooks_registry entry"
        L.append(f"| {i} | 0x{h['rva']} | {h['conf']} | {h['name']} | "
                 f"{r['match']} | {len(r['scenarios'])} | "
                 f"{'yes' if r.get('in_registry') else 'no'} | {recommend} |")
    if not targets:
        L.append("| — | — | — | (none this run) | — | — | — | — |")

    # PROMOTION_QUEUE-format block (paste into re/PROMOTION_QUEUE.md "Queued").
    reg_rvas = [f"0x{r['hook']['rva']}" for r in targets if r.get("in_registry")]
    L += ["", "## Paste-ready promotion-queue row (registry-ready targets only)",
          "", "```",
          (f"{report.get('ts','YYYY-MM-DD')}  sweep-target  "
           f"rvas={','.join(reg_rvas) if reg_rvas else '(none)'}  "
           f"evidence=re/parity/runs/{report.get('ts','')}/report.json  "
           f"note=clean-match in parity sweep; run_diff to confirm bit-identity "
           f"before C4 (sweep match != C4)"),
          "```"]

    L += ["", "## Fix-first (emitter mismatched in the sweep — make RE faithful, then re-sweep)",
          "", "| MASHED rva | confidence | name | mismatch | scenarios |",
          "|---|---|---|---|---|"]
    for r in fix_first:
        h = r["hook"]
        L.append(f"| 0x{h['rva']} | {h['conf']} | {h['name']} | "
                 f"{r['mismatch']} | {','.join(r['scenarios'])} |")
    if not fix_first:
        L.append("| — | — | (none) | — | — |")

    L += ["", "## Unmatched emitters (named in mashed_re.map, no hooks.csv name-join)",
          "", "_Triage manually — harden the bridge by parsing "
          "`RH_ScopedInstall(Method, 0xRVA)` from the reimpl source._", "",
          "| re offset | name | match | mismatch |", "|---|---|---|---|"]
    for r in sorted(unmatched, key=lambda r: -(r["match"] + r["mismatch"]))[:60]:
        L.append(f"| {r['re_off']} | {r['name'] or '?'} | "
                 f"{r['match']} | {r['mismatch']} |")

    Path(out).write_text("\n".join(L), encoding="utf-8")


if __name__ == "__main__":
    sys.exit(main())
