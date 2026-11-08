"""C4R-DXLIB-TAG11 (2026-11-08): re-run the C4R-DXLIB-NEWROWS row add (verify/c4rdxlibfu_20261107/newrows.py checks
+ apply.py section 3 row format) against the regenerated re/console/pc_funcs.csv.

  py -3.12 verify/c4rdxlibghidrafn_20261108/rows_d3dx9.py [--apply]

Checks per identical-unlisted address of verify/c4rdxlib_20261107/d3dx_sum2003.tsv with no hooks.csv row (unchanged
from newrows.py): one library function, one object, a sub-tag from the object's `identical` d3dx9 rows (refused when
the object maps to no tag or to several: those rows wait for a user decision), a pc_funcs entry at the address.
Size rule, one extension: pc_funcs body == symbol size as before, OR body < symbol size and every byte of
[entry+body, entry+symbol size) is alignment fill (0x90 / 0xCC / mov r,r / lea r,[r+0], the CreateLibFunctions.java
isFill rule). MASM `align` puts that fill inside the object symbol; Ghidra's body stops at the last instruction.
Precedent: the amdmath rows 0x005130c0 (body 245, symbol 256) and 0x005131c0 (247 / 256) are already C4 by libid.
Name rule as apply.py short_name, plus a plain cdecl `_name` -> name and a two-scope `?f@B@A@@` -> A::B::f.
Line-based on hooks.csv (rows appended), CHANGELOG lines below <!-- ENTRIES -->. Idempotent. Dry run by default.
"""
import csv, collections, io, pathlib, re, sys
W = pathlib.Path(__file__).resolve().parents[2]
HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(W / "re/tools/match"))
import coffpe  # noqa: E402
TSV = "verify/c4rdxlib_20261107/d3dx_sum2003.tsv"
DX = "DirectX 9.0b SDK Summer 2003 d3dx9.lib member"
DATE = "2026-11-08"
RESULT = "verify/c4rdxlibtag11_20261108/RESULT_C4R-DXLIB-TAG11.md"
PSGP = "third-party-library[d3dx9-psgp]"
WANT = {0x4fc6f5,0x4fc75e,0x4fc793,0x4fc81e,0x4fc87d,0x50fc7c,0x50fcfe,0x50fd5d,0x50fe8f,0x50ff00,0x50ffc8}
MARKER = "<!-- ENTRIES -->"


def is_fill(b):
    i, n = 0, len(b)
    while i < n:
        c = b[i]
        if c in (0x90, 0xCC):
            i += 1
            continue
        if i + 1 >= n:
            return False
        m = b[i + 1]
        mod, reg, rm = m >> 6, (m >> 3) & 7, m & 7
        if c == 0x8B and mod == 3 and reg == rm:
            i += 2
            continue
        if c == 0x8D and mod in (1, 2) and reg == rm:
            j = i + 2
            if rm == 4:
                if j >= n or b[j] != 0x24:
                    return False
                j += 1
            d = 1 if mod == 1 else 4
            if j + d > n or any(b[j:j + d]):
                return False
            i = j + d
            continue
        return False
    return True


def short_name(sym):
    m = re.fullmatch(r"\?([A-Za-z_]\w*)@(?:([A-Za-z_]\w*)@)?@.*", sym)
    if m:
        return f"{m.group(2)}::{m.group(1)}" if m.group(2) else m.group(1)
    m = re.fullmatch(r"\?([A-Za-z_]\w*)@([A-Za-z_]\w*)@([A-Za-z_]\w*)@@.*", sym)  # MSVC: innermost scope first
    if m:
        return f"{m.group(3)}::{m.group(2)}::{m.group(1)}"
    m = re.fullmatch(r"_([A-Za-z_]\w*)@\d+", sym)
    if m:
        return m.group(1)
    m = re.fullmatch(r"_([A-Za-z_]\w*)", sym)
    if m:
        return m.group(1)
    return None


def dump(r):
    b = io.StringIO()
    csv.writer(b, lineterminator="").writerow(r)
    return b.getvalue()


def main():
    do = "--apply" in sys.argv
    img = coffpe.Image()
    raw = open(W / "hooks.csv", encoding="utf-8", newline="").read()
    eol = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(eol)
    trail = lines[-1] == ""
    if trail:
        lines.pop()
    hi = next(i for i, l in enumerate(lines) if l.startswith("rva,"))
    hdr = next(csv.reader([lines[hi]]))
    col = {n: k for k, n in enumerate(hdr)}
    have = {l.split(",", 1)[0].lower() for l in lines[hi + 1:]}
    pf = {int(r["entry"], 16): r for r in csv.DictReader(open(W / "re/console/pc_funcs.csv", encoding="utf-8"))}
    T = list(csv.DictReader(open(W / TSV, encoding="utf-8"), delimiter="\t"))
    tag = collections.defaultdict(collections.Counter)
    for r in T:
        if r["subsystem"].startswith("third-party-library[d3dx9") and r["library_object"] and r["status"] == "identical":
            tag[r["library_object"]][r["subsystem"]] += 1
    by = collections.defaultdict(list)
    for r in T:
        if r["status"] == "identical-unlisted":
            by[r["rva"]].append(r)
    cand, rows = [], []
    for rva in sorted(by, key=lambda s: int(s, 16)):
        k = f"{int(rva, 16):08x}"
        if int(rva, 16) not in WANT:
            continue
        if k in have:
            continue
        a = int(rva, 16)
        ents = by[rva]
        names = sorted({e["lib_function"] for e in ents})
        objs = sorted({e["library_object"] for e in ents})
        size = max(int(e["lib_size"]) for e in ents)
        p = pf.get(a)
        why, t, fill = [], "", 0
        if len(names) != 1:
            why.append(f"ambiguous: {len(names)} library functions")
        if len(objs) != 1:
            why.append(f"{len(objs)} objects")
        if len(objs) == 1:
            c = tag.get(objs[0])
            if objs[0].endswith(("d3dxmathx86.obj", "x3d_vec4.obj")):
                t = PSGP
            elif not c:
                why.append(f"no sub-tag: object {objs[0]} matched by no d3dx9 row")
            elif len(c) > 1:
                why.append(f"sub-tag ambiguous: {objs[0]} -> {'/'.join(sorted(c))}")
            else:
                t = next(iter(c))
        if p is None:
            inside = [q for e, q in pf.items() if e < a < int(q["body_end"], 16)]
            why.append("no pc_funcs entry at rva" + (f" (inside {inside[0]['entry']} {inside[0]['name']})" if inside
                                                     else " (in no pc_funcs body)"))
        else:
            bs = int(p["body_size"])
            if bs > size:
                why.append(f"size: pc_funcs body {bs} B > symbol {size} B (identity covers only the first {size} B)")
            elif bs < size:
                if is_fill(img.read(a + bs, size - bs)):
                    fill = size - bs
                else:
                    why.append(f"size: pc_funcs body {bs} < lib {size}, tail not alignment fill")
        nm = short_name(names[0]) if len(names) == 1 else None
        if len(names) == 1 and nm is None:
            why.append(f"no name rule for {names[0]}")
        cand.append([f"0x{k}", names[0] if len(names) == 1 else "|".join(names[:3]),
                     objs[0] if len(objs) == 1 else "|".join(objs), str(size), p["body_size"] if p else "",
                     str(fill) if fill else "", p["name"] if p else "", t, "add" if not why else "skip", "; ".join(why)])
        if why:
            continue
        r = [""] * len(hdr)
        r[col["rva"]] = k
        r[col["name"]] = nm
        r[col["subsystem"]] = t
        r[col["confidence"]] = "C1"
        r[col["status"]] = "new"
        fl = f" + {fill} B MASM alignment fill" if fill else ""
        r[col["notes"]] = (f"C1 by libid: identical {objs[0]} {names[0]} ({size} B; {DX}; {TSV}); function boundary "
                           f"re/console/pc_funcs.csv {p['name']} {p['body_size']} B{fl} (function created in master "
                           f"C4R-DXLIB-TAG11 {DATE}); sub-tag third-party-library[d3dx9-psgp] by decision Mariano 2026-11-08 (d3dx9-psgp); symbol {names[0]}; row added "
                           f"C4R-DXLIB-TAG11 {DATE}; not C4: pending c4_reclass batch")
        if "port" in col:
            r[col["port"]] = "none"
        rows.append((k, nm, t, dump(r)))
    with open(HERE / "candidates.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["rva", "lib_function", "library_object", "lib_size", "pc_body_size", "tail_fill", "pc_name",
                    "subsystem", "verdict", "why"])
        w.writerows(cand)
    print("addresses", len(cand), dict(collections.Counter(c[8] for c in cand)))
    print("added by tag", dict(collections.Counter(x[2] for x in rows)))
    for c in cand:
        if c[8] != "add":
            print("  skip", c[0], c[1][:50], "|", c[9])
    if not do:
        print("dry run: nothing written")
        return
    with open(HERE / "added.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("rva\troute\tevidence\n")
        for k, _, _, _ in rows:
            f.write(f"0x{k}\tlibid\t{TSV}\n")
    lines += [x[3] for x in rows]
    open(W / "hooks.csv", "w", encoding="utf-8", newline="").write(eol.join(lines) + (eol if trail else ""))
    ct = open(W / "re/analysis/CHANGELOG.md", encoding="utf-8", newline="").read()
    ceol = "\r\n" if "\r\n" in ct else "\n"
    cl = ct.split(ceol)
    mi = next(i for i, l in enumerate(cl) if l.strip() == MARKER)
    cl[mi + 1:mi + 1] = [f"{DATE}  {k}  {n}  (none)->C1  added by libid (d3dx9 9.0b Summer 2003, {t}) evidence: {RESULT}"
                         for k, n, t, _ in rows]
    open(W / "re/analysis/CHANGELOG.md", "w", encoding="utf-8", newline="").write(ceol.join(cl))
    print("written", len(rows))


if __name__ == "__main__":
    main()
