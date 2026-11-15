"""Summarize the --peek fslist rows of run_slotlist.sh logs.
usage: py -3.12 aggregate_slotlist.py <log> [<log> ...]
Per log: every [peek] row's list order (table addr, n, getter, device prefix), the pizfs table's
slot rows (addr/+0x18/+0x1c/+0x20/+0x28/+0x38), the three u32 globals, and whether any row differs
from the first row of the same log."""
import json, re, sys

def rows(path):
    out = []
    for line in open(path, encoding="utf-8", errors="replace"):
        m = re.search(r"\[peek\] (.*?)\s*(\{.*\})\s*$", line)
        if not m:
            continue
        try:
            out.append((m.group(1).strip(), json.loads(m.group(2))))
        except Exception as e:
            out.append((m.group(1).strip(), {"PARSE_ERR": str(e)}))
    return out

def clean(s):
    return "".join(ch for ch in (s or "") if ch.isprintable() and ord(ch) < 128)[:6]

for path in sys.argv[1:]:
    rs = rows(path)
    print(f"== {path}: {len(rs)} peek rows")
    first = None
    for tag, d in rs:
        fl = d.get("fslist")
        if not isinstance(fl, dict):
            print(f"  {tag}: fslist={fl!r} keys={list(d)}")
            continue
        order = [(t["t"], t["n"], t["get"], clean(t["dev"])) for t in fl["tabs"]]
        pz = [t for t in fl["tabs"] if t["get"] == "0x4b6630"]
        pzslots = pz[0]["files"] if pz else None
        w32 = {t["t"]: sorted(set(f.split("/", 1)[1] for f in t["files"])) for t in fl["tabs"] if t["get"] == "0x550c30"}
        g = {k: v for k, v in d.items() if k != "fslist"}
        sig = (fl["head"], fl["max"], fl["count"], fl["deflt"], tuple(order), tuple(pzslots or ()), json.dumps(w32, sort_keys=True), json.dumps(g, sort_keys=True))
        same = "" if first is None else (" SAME-AS-FIRST" if sig == first else " DIFFERS-FROM-FIRST")
        if first is None:
            first = sig
        print(f"  {tag}: head={fl['head']} max={fl['max']} count={fl['count']} deflt={fl['deflt']}{same}")
        if same != " SAME-AS-FIRST":
            for o in order:
                print("    table", o)
            print("    pizfs slots addr/+18/+1c/+20/+28/+38:", pzslots)
            print("    win32 slot (+18/+1c/+20/+28/+38) sets per table:", w32)
            print("    globals:", g)
