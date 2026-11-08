"""libmatch.py restricted to the MATCH-S17 slice rows (54 CRT-shaped functions filed boot/util/render).
    py -3.12 D:/MashedWT/MATCH-S17_data/tools/libmatch_slice.py --objs <libcmt.lib> --subsystem x --tsv out"""
import sys, csv
sys.path.insert(0, "re/tools/match")
import libmatch
S = [int(l, 16) for l in open("D:/MashedWT/MATCH-S17_data/slice_rvas.txt")]
H = {}
for r in csv.DictReader(open("hooks.csv", encoding="utf-8")):
    try: a = int(r["rva"], 16)
    except ValueError: continue
    a = a + 0x400000 if a < 0x400000 else a
    H.setdefault(a, r)
libmatch.hooks_rows = lambda prefixes, hooks=None: [(a, H[a]["name"], H[a]["subsystem"], H[a]["confidence"]) for a in S]
_re = libmatch.row_extents
def row_extents(hooks=None, cap=0x2000):
    vas = sorted(set(a + 0x400000 if a < 0x400000 else a for a in (int(r["rva"], 16) for r in csv.DictReader(open("hooks.csv", encoding="utf-8")) if not r["rva"].startswith("#"))))
    return {a: min(b - a, cap) for a, b in zip(vas, vas[1:])}
libmatch.row_extents = row_extents
libmatch.main()
