"""Merge per-library identity TSVs into one per-function table plus counts and residual categories.

  py -3.12 re/tools/match/summarize.py <out.tsv> lib=<label>:<tsv> [lib=...] [--uncompared <label>:<subsystem prefix>:<note>]
"""
import argparse
import collections
import csv
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import libmatch  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("libs", nargs="+", help="label:path.tsv")
    ap.add_argument("--uncompared", action="append", default=[], help="label|subsystem prefix|note")
    a = ap.parse_args()
    out_rows, extra_rows = [], []
    for spec in a.libs:
        label, path = spec.split(":", 1)
        with open(path, encoding="utf-8") as f:
            for r in csv.DictReader(f, delimiter="\t"):
                r = {"library": label, **r}
                # a pairing that differs at the first byte of a body under 32 bytes says nothing about which function
                # sits there: count it as not compared
                if r["status"] == "differs" and r["first_diff"] == "0x0" and int(r["lib_size"] or 0) < 32:
                    r.update(status="not-compared", category="", note="weak pairing dropped (body < 32 B, differs "
                             f"at +0x0; candidate was {r['lib_function']}); " + r["note"])
                    r.update(library_object="", lib_function="", lib_size="", first_diff="")
                (extra_rows if r["status"] == "identical-unlisted" else out_rows).append(r)
    for spec in a.uncompared:
        label, prefix, note = spec.split("|", 2)
        for va, name, sub, conf in libmatch.hooks_rows([prefix]):
            out_rows.append({"library": label, "rva": f"0x{va:08x}", "hooks_name": name, "subsystem": sub,
                             "confidence": conf, "status": "not-compared", "library_object": "", "lib_function": "",
                             "lib_size": "", "first_diff": "", "category": "", "note": note})
    cols = ["library", "rva", "hooks_name", "subsystem", "confidence", "status", "library_object", "lib_function",
            "lib_size", "first_diff", "category", "note"]
    with open(a.out, "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, cols, delimiter="\t", extrasaction="ignore", lineterminator="\n")
        w.writeheader()
        for r in out_rows + extra_rows:
            w.writerow(r)
    per = collections.defaultdict(collections.Counter)
    cat = collections.defaultdict(collections.Counter)
    for r in out_rows:
        st = "identical" if r["status"].startswith("identical") else r["status"]
        per[r["library"]][st] += 1
        if r["status"] == "identical-ambiguous":
            per[r["library"]]["of which ambiguous"] += 1
        if r["status"] == "differs":
            cat[r["library"]][r["category"] or "(uncategorised)"] += 1
    for r in extra_rows:
        per[r["library"]]["identical at an address outside this subsystem's rows"] += 1
    for lib, c in per.items():
        print(f"{lib}: rows {c['identical'] + c['differs'] + c['not-compared']} identical {c['identical']} "
              f"(ambiguous {c['of which ambiguous']}) differs {c['differs']} not-compared {c['not-compared']} "
              f"identical-elsewhere {c['identical at an address outside this subsystem' + chr(39) + 's rows']}")
        for k, v in cat[lib].most_common():
            print(f"    {v:4d}  {k}")


if __name__ == "__main__":
    main()
