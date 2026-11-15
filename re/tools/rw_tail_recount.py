"""Recount the RW executed tail (L-144, 176 functions) on the checked-out tree.

  py -3.12 re/tools/rw_tail_recount.py [--out <tsv>]

Set: the 176 RVAs of verify/match_20261112/RWT_TAIL_CENSUS.tsv (fixed membership defined by
MATCH-RWT-REFRESH: 163 ranked RWT rows + 12 hand bodies + 0x004c7a70). Membership is not recomputed.
Per RVA it reads hooks.csv (confidence, notes), re/match/ADMITTED.tsv, re/match/PROVISIONAL.tsv.
Resolved (L-144 strict) = hooks.csv confidence C4 (any route) or a row in ADMITTED.tsv.
Route: c4=match / c4=libid / c4=frida-replay / frida (C4, other) read from the hooks.csv notes.
Static, read-only.
"""
import argparse
import collections
import csv
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]


def tsv(path):
    lines = (l for l in open(path, encoding="utf-8", newline="") if not l.startswith("#"))
    return list(csv.DictReader(lines, delimiter="\t"))


def route(notes):
    n = notes.lower()
    if "c4=match" in n:
        return "match"
    if "c4=libid" in n:
        return "libid"
    if "c4=frida-replay" in n:
        return "frida-replay"
    return "c4-other"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out")
    a = ap.parse_args()
    members = [r["rva"].lower() for r in tsv(ROOT / "verify/match_20261112/RWT_TAIL_CENSUS.tsv")]
    hk = {}
    for r in csv.DictReader(open(ROOT / "hooks.csv", newline="", encoding="utf-8")):
        try:
            hk[int(r["rva"], 16)] = r
        except ValueError:
            pass
    adm = {int(r["rva"], 16) for r in tsv(ROOT / "re/match/ADMITTED.tsv")}
    prov = {int(r["rva"], 16): r["status"] for r in tsv(ROOT / "re/match/PROVISIONAL.tsv")}
    rows = []
    for k in members:
        v = int(k, 16)
        h = hk.get(v)
        conf = h["confidence"] if h else "NOROW"
        if conf == "C4":
            st, rt = "resolved", route(h["notes"])
        elif v in adm:
            st, rt = "resolved", "admitted"
        else:
            st, rt = "open", ""
        rows.append((k, conf, st, rt, prov.get(v, ""), h["port"] if h else ""))
    c = collections.Counter(r[2] for r in rows)
    rc = collections.Counter(r[3] for r in rows if r[3])
    head = subprocess.run(["git", "-C", str(ROOT), "rev-parse", "HEAD"], capture_output=True, text=True).stdout.strip()
    print("head", head)
    print("members", len(members), "unique", len(set(members)))
    print("resolved", c["resolved"], "open", c["open"])
    print("by route", dict(rc))
    print("open rvas:", " ".join(r[0] for r in rows if r[2] == "open"))
    if a.out:
        w = csv.writer(open(a.out, "w", newline=""), delimiter="\t")
        w.writerow(["rva", "hooks_conf", "state", "route", "provisional", "port"])
        w.writerows(rows)


if __name__ == "__main__":
    sys.exit(main())
