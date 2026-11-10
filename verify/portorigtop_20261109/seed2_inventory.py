"""PORT-OT-SEED-2: extend the L-99 seed inventory with the ORIG_TOP rows' unseeded initialised data.

  py -3.12 verify/portorigtop_20261109/seed2_inventory.py [--data D:/MashedWT/PORT-OT-MEASURE_data] [--exclude a,b,..]

Input: <data>/ot_rows.json (ot_measure.py lists). For every address a runnable ORIG_TOP row reads that holds nonzero
bytes in MASHED.exe's .data / .rdata and that neither seed covers (Compat/PortStateSeed.inc, PortTop_RdataSeed.inc):
  * reads of one address at two sizes collapse to the larger; ranges that touch or overlap each other or an existing
    inventory entry merge into one union entry (the generator refuses overlaps);
  * an entry any of whose aligned dwords is a FUNCTION ENTRY of the original (re/console/pc_funcs.csv) is NOT seeded:
    in the exe that address is exe code, so the original value would be a wild code pointer. Listed as
    `code-pointer` (needs a binding, not a seed);
  * a 4-byte entry whose value points into the original's .rdata / .data at a NUL-terminated printable string also
    seeds that string (kind string), so the pointer leads to its bytes; other data-pointer targets are listed;
  * kind = string when the glob is s_* and the bytes end on NUL, else scalar/array.
Writes the merged inventory over verify/portstate_20261108/SEED_CANDIDATES.tsv (new rows carry exe=origtop) and
verify/portorigtop_20261109/SEED2_ADDED.tsv (what was added, merged, skipped and why). Then run
`py -3.12 re/tools/gen_portstate_seed.py` and `--check`.
"""
from __future__ import annotations

import csv
import json
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(ROOT / "re" / "tools"))
import ot_measure as M        # noqa: E402
import port_closed_sets as P  # noqa: E402

INV = ROOT / "verify" / "portstate_20261108" / "SEED_CANDIDATES.tsv"
TEXT_LO, TEXT_HI = 0x00401000, 0x005cb000


def main(argv):
    data = argv[argv.index("--data") + 1] if "--data" in argv else "D:/MashedWT/PORT-OT-MEASURE_data"
    excl = set()
    if "--exclude" in argv:
        excl = {int(x, 16) for x in argv[argv.index("--exclude") + 1].split(",") if x}
    rows = json.load(open(pathlib.Path(data) / "ot_rows.json"))
    rng = M.seed_ranges()
    pe = P.PE()
    funcs = P.load_funcs()

    def fbytes(a, n):
        for name, va, vs, rs, pr in pe.secs:
            if va <= a < va + max(vs, rs) and a - va + n <= rs:
                return name, pe.raw[pr + a - va: pr + a - va + n]
        return None, None

    need = {}
    for k, v in rows.items():
        for x in v.get("seed", []):
            a = int(x["addr"], 16)
            if M.covered(rng, a, x["size"]):
                continue
            e = need.setdefault(a, {"glob": x["name"], "size": 0, "rows": set()})
            e["size"] = max(e["size"], x["size"])
            e["rows"].add(k)
    log = ["addr\tglob\tsize\taction\tdetail\trows"]
    keep = {}
    for a in sorted(need):
        e = need[a]
        if a in excl:
            log.append("0x%08x\t%s\t%d\texcluded\t--exclude (bisect)\t%s" % (a, e["glob"], e["size"], " ".join(sorted(e["rows"]))))
            continue
        sec, b = fbytes(a, e["size"])
        if b is None:
            log.append("0x%08x\t%s\t%d\tskipped\tno file bytes\t%s" % (a, e["glob"], e["size"], " ".join(sorted(e["rows"]))))
            continue
        dw = [int.from_bytes(b[i:i + 4], "little") for i in range(0, len(b) - 3, 4)] if a % 4 == 0 else []
        code = [d for d in dw if TEXT_LO <= d < TEXT_HI and d in funcs]
        if code or e["glob"].startswith(("PTR_FUN_", "PTR_LAB_", "PTR_thunk_")):
            log.append("0x%08x\t%s\t%d\tcode-pointer\tholds original function entries %s: needs a binding, not a seed\t%s" % (
                a, e["glob"], e["size"], " ".join("0x%08x" % d for d in code[:6]) or "(PTR_ to code)", " ".join(sorted(e["rows"]))))
            continue
        keep[a] = {"glob": e["glob"], "size": e["size"], "sec": sec, "rows": set(e["rows"]), "src": "row"}
        if e["size"] == 4 and dw:
            t = dw[0]
            tsec, _ = fbytes(t, 1)
            if tsec in (".rdata", ".data") and not M.covered(rng, t, 1):
                s, _b = fbytes(t, 256)
                end = _b.find(b"\0") if _b else -1
                if end > 0 and all(32 <= c < 127 or c in (9, 10, 13) for c in _b[:end]):
                    keep.setdefault(t, {"glob": "s_ptrtarget_%08x" % t, "size": end + 1, "sec": tsec,
                                        "rows": set(e["rows"]), "src": "pointer target of 0x%08x" % a})
                    log.append("0x%08x\t%s\t%d\tadded+target\tpoints at string 0x%08x (%d B), seeded too\t%s" % (
                        a, e["glob"], e["size"], t, end + 1, " ".join(sorted(e["rows"]))))
                    continue
                if tsec in (".rdata", ".data"):
                    log.append("0x%08x\t%s\t%d\tadded\tdata pointer to 0x%08x (%s, not a string: target not seeded)\t%s" % (
                        a, e["glob"], e["size"], t, tsec, " ".join(sorted(e["rows"]))))
                    continue
        log.append("0x%08x\t%s\t%d\tadded\t%s\t%s" % (a, e["glob"], e["size"], sec, " ".join(sorted(e["rows"]))))

    # merge with the existing inventory
    inv = list(csv.DictReader(open(INV, encoding="utf-8"), delimiter="\t"))
    fields = list(inv[0].keys())
    ent = []
    for r in inv:
        ent.append({"a": int(r["addr"], 16), "n": int(r["size"]), "row": r, "new": False})
    for a, e in keep.items():
        _sec, b = fbytes(a, e["size"])
        kind = "string" if e["glob"].startswith("s_") and b[-1:] == b"\0" else "scalar/array"
        ent.append({"a": a, "n": e["size"], "new": True,
                    "row": {"addr": "0x%08x" % a, "glob": e["glob"], "section": e["sec"], "kind": kind,
                            "size": str(e["size"]), "exe": "origtop", "n_rows": str(len(e["rows"])),
                            "rows": " ".join(sorted(e["rows"]))}})
    ent.sort(key=lambda x: x["a"])
    merged = []
    for x in ent:
        if merged and x["a"] < merged[-1]["a"] + merged[-1]["n"]:
            m = merged[-1]
            end = max(m["a"] + m["n"], x["a"] + x["n"])
            m["n"] = end - m["a"]
            m["row"] = dict(m["row"])
            m["row"]["size"] = str(m["n"])
            m["row"]["kind"] = "scalar/array"
            m["row"]["glob"] = m["row"]["glob"] + "+" + x["row"]["glob"]
            rr = set(m["row"]["rows"].split()) | set(x["row"]["rows"].split())
            m["row"]["rows"] = " ".join(sorted(rr))
            m["row"]["n_rows"] = str(len(rr))
            m["new"] = m["new"] or x["new"]
            log.append("0x%08x\t%s\t%d\tmerged\tunion with 0x%08x -> %d B\t" % (x["a"], x["row"]["glob"], x["n"], m["a"], m["n"]))
        else:
            merged.append(dict(x))
    with open(INV, "w", encoding="utf-8", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=fields, delimiter="\t", lineterminator="\n")
        w.writeheader()
        for x in merged:
            w.writerow({k: x["row"].get(k, "") for k in fields})
    (HERE / "SEED2_ADDED.tsv").write_text("\n".join(log) + "\n", encoding="utf-8")
    acts = {}
    for l in log[1:]:
        acts[l.split("\t")[3]] = acts.get(l.split("\t")[3], 0) + 1
    print("unseeded addresses:", len(need), "actions:", acts, "inventory rows:", len(inv), "->", len(merged))


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
