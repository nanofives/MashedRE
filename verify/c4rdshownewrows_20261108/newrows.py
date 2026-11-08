"""C4R-DSHOW-NEWROWS (2026-11-08): add C1 rows for BaseClasses identical-unlisted addresses (line-based, appended).
  py -3.12 verify/c4rdshownewrows_20261108/newrows.py [--apply]
Mirrors verify/c4rdxlibfu_20261107/newrows.py + apply.py."""
import csv, io, pathlib, re, sys, collections
W = pathlib.Path(__file__).resolve().parents[2]
HERE = pathlib.Path(__file__).resolve().parent
TSV = "verify/c4rdshowbase_20261107/rel_ob1.tsv"
SUB = "third-party-library[directshow-baseclasses]"
DATE = "2026-11-08"
RESULT = "verify/c4rdshownewrows_20261108/RESULT_C4R-DSHOW-NEWROWS.md"
SRC = "DirectShow BaseClasses source (DirectX 9.0b SDK Summer 2003)"
CFG = "/Gz /MT /GX /O2 /Ob1 -D NDEBUG WIN32 _MBCS _LIB _WIN32_DCOM WINVER=0x400"

def short_name(sym):
    m = re.fullmatch(r"\?([A-Za-z_]\w*)@(?:([A-Za-z_]\w*)@)?@.*", sym)
    if m:
        return f"{m.group(2)}::{m.group(1)}" if m.group(2) else m.group(1)
    m = re.fullmatch(r"\?(\?[0-2G]|\?_G)([A-Za-z_]\w*)@@.*", sym)
    if m:
        c = m.group(2)
        return {"??0": f"{c}::{c}", "??1": f"{c}::~{c}", "??_G": f"{c}::`scalar deleting destructor'"}.get(m.group(1), None) or sym
    return sym

def short2(sym):
    m = re.match(r"\?(\?_G|\?0|\?1)([A-Za-z_]\w*)@@", sym)
    if m:
        p, c = m.groups()
        return {"?_G": f"{c}::`scalar deleting destructor'", "?0": f"{c}::{c}", "?1": f"{c}::~{c}"}[p]
    m = re.match(r"\?([A-Za-z_]\w*)@([A-Za-z_]\w*)@@", sym)
    if m:
        return f"{m.group(2)}::{m.group(1)}"
    m = re.match(r"\?([A-Za-z_]\w*)@@", sym)
    if m:
        return m.group(1)
    return sym

def main():
    do = "--apply" in sys.argv
    raw = open(W / "hooks.csv", encoding="utf-8", newline="").read()
    eol = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(eol)
    trail = lines[-1] == ""
    if trail: lines.pop()
    hdr = next(csv.reader([lines[0]]))
    col = {n: k for k, n in enumerate(hdr)}
    have = {l.split(",", 1)[0].lower() for l in lines[1:]}
    pf = {int(r["entry"], 16): r for r in csv.DictReader(open(W / "re/console/pc_funcs.csv", encoding="utf-8"))}
    by = collections.defaultdict(list)
    for r in csv.DictReader(open(W / TSV, encoding="utf-8"), delimiter="\t"):
        if r["status"] == "identical-unlisted":
            by[r["rva"]].append(r)
    rows = []; cand = []
    for rva in sorted(by, key=lambda s: int(s, 16)):
        k = f"{int(rva,16):08x}"
        ents = by[rva]
        names = sorted({e["lib_function"] for e in ents})
        objs = sorted({e["library_object"] for e in ents})
        size = max(int(e["lib_size"]) for e in ents)
        p = pf.get(int(rva, 16))
        why = []
        if k in have: why.append("hooks.csv row present")
        if len(names) != 1: why.append(f"ambiguous: {len(names)} library functions")
        if p is None: why.append("no pc_funcs boundary at rva")
        elif int(p["body_size"]) != size: why.append(f"size mismatch: pc_funcs {p['body_size']} B vs symbol {size} B")
        cand.append([rva, names[0] if len(names)==1 else "|".join(names[:3]), "|".join(objs), str(size),
                     p["body_size"] if p else "", "add" if not why else "skip", "; ".join(why)])
        if why: continue
        sym = names[0]; name = short2(sym)
        r = [""] * len(hdr)
        r[col["rva"]] = k; r[col["name"]] = name; r[col["subsystem"]] = SUB
        r[col["confidence"]] = "C1"; r[col["status"]] = "new"
        r[col["notes"]] = (f"C1 by libid: identical {objs[0]} {sym} ({size} B; {SRC}; {CFG}; {TSV}); function boundary "
                           f"re/console/pc_funcs.csv {p['name']} {p['body_size']} B; row added C4R-DSHOW-NEWROWS {DATE}; "
                           f"not C4: pending c4_reclass batch")
        r[col["port"]] = "none"
        b = io.StringIO(); csv.writer(b, lineterminator="").writerow(r)
        rows.append((k, name, b.getvalue()))
    with open(HERE / "candidates.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["rva","lib_function","library_object","lib_size","pc_body_size","verdict","why"]); w.writerows(cand)
    print("addresses", len(cand), collections.Counter(c[5] for c in cand))
    for c in cand:
        if c[5] != "add": print("  skip", c[0], c[1][:50], "|", c[6])
    if not do:
        print("dry run"); return
    with open(HERE / "added_batch.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("rva\troute\tevidence\n")
        for k,_,_ in rows: f.write(f"0x{k}\tlibid\t{TSV}\n")
    lines += [r[2] for r in rows]
    open(W / "hooks.csv", "w", encoding="utf-8", newline="").write(eol.join(lines) + (eol if trail else ""))
    ct = open(W / "re/analysis/CHANGELOG.md", encoding="utf-8", newline="").read()
    ceol = "\r\n" if "\r\n" in ct else "\n"
    cl = ct.split(ceol)
    mi = next(i for i,l in enumerate(cl) if l.strip() == "<!-- ENTRIES -->")
    cl[mi+1:mi+1] = [f"{DATE}  {k}  {n}  (none)->C1  added by libid (DirectShow BaseClasses DX 9.0b Summer 2003) evidence: {RESULT}" for k,n,_ in rows]
    open(W / "re/analysis/CHANGELOG.md", "w", encoding="utf-8", newline="").write(ceol.join(cl))
    print("written", len(rows))
main()
