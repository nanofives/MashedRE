"""r10: tables for RESULT_r10.md from the scored JSON of both samples.

    py -3.12 report_tables.py > tables_r10.md

Sample A = capfix captures (verify/capfix_20261016), sample B = the independent re-capture (bt/ , bt_rerun/).
diff0_ = first scoring, unmodified union (cites_r10.json); diff1_ = v2; diff2_ = v3 (final).
"""
import glob, json, importlib.util
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("d3", HERE.parent.parent / "re" / "tools" / "drawlist3d.py")
d3 = importlib.util.module_from_spec(spec); spec.loader.exec_module(d3)
TR = ["01_Egypt", "02_Neustein", "04_Highway", "05_Sands", "06_SuperG", "07_Roundabout", "08_Storm", "09_Forest", "10_Dump", "11_Warzone"]


def frames(root, prefix, trk):
    return [json.loads(Path(f).read_text()) for f in sorted(glob.glob(str(root / trk / f"{prefix}[0-9]*.json")))]


def tri_matched(txt):
    import re
    m = re.search(r"matched \d+ draws \((\d+) tri\)", txt)
    return int(m.group(1)) if m else 0


def table(root, label, first_prefix):
    print(f"### {label}\n")
    print("| track | frames | orig draws | matched draws | orig tri | matched tri | rows | cited | red (first run) | red (final) |")
    print("|---|---:|---|---|---|---|---:|---:|---:|---:|")
    tot = defaultdict(int)
    for t in TR:
        F = frames(root, "diff2_", t)
        F0 = frames(root, first_prefix, t)
        txts = [Path(f).with_suffix(".txt").read_text() for f in sorted(glob.glob(str(root / t / "diff2_[0-9]*.json")))]
        na = [j["na"] for j in F]; m = [j["matched"] for j in F]; ta = [j["tri_a"] for j in F]
        mt = [tri_matched(x) for x in txts]
        rows = sum(len(j["rows"]) for j in F); red = sum(1 for j in F for r in j["rows"] if "cite" not in r)
        red0 = sum(1 for j in F0 for r in j["rows"] if "cite" not in r)
        print(f"| {t[3:]} | {len(F)} | {' / '.join(map(str, na))} | {' / '.join(map(str, m))} | {' / '.join(map(str, ta))} | "
              f"{' / '.join(f'{100 * a / b:.1f}%' for a, b in zip(mt, ta))} | {rows} | {rows - red} | {red0} | {red} |")
        for k, v in (("frames", len(F)), ("rows", rows), ("red", red), ("red0", red0)):
            tot[k] += v
    print(f"| **all** | {tot['frames']} | | | | | {tot['rows']} | {tot['rows'] - tot['red']} | {tot['red0']} | {tot['red']} |\n")


def matrix(root, label):
    print(f"### {label}: rows per cite id and track (final cites, v3)\n")
    cnt = defaultdict(lambda: defaultdict(int))
    for t in TR:
        for j in frames(root, "diff2_", t):
            for r in j["rows"]:
                cnt[r.get("cite", "RED")][t[3:]] += 1
    names = [t[3:] for t in TR]
    print("| cite | " + " | ".join(n[:5] for n in names) + " | total |")
    print("|---|" + "---:|" * (len(names) + 1))
    for c in sorted(cnt):
        print(f"| {c} | " + " | ".join(str(cnt[c].get(n, "")) for n in names) + f" | {sum(cnt[c].values())} |")
    print()


table(HERE, "Sample A (capfix captures)", "diff0_")
matrix(HERE, "Sample A")
table(HERE / "bt_rerun", "Sample B (independent re-capture, scored once with v2 then v3)", "diff1_")
matrix(HERE / "bt_rerun", "Sample B")
