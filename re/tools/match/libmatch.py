"""Library-object identity: compare object-file functions against MASHED.exe at hooks.csv RVAs.

A function is IDENTICAL at va when every byte of the object's function body (to the next function symbol, trailing
int3 fill trimmed) equals the image bytes, except 4-byte relocation sites. A relocation whose target lies inside the
same function (switch tables, local jumps written as relocations) is not masked: the image must hold
va + (target - function offset) + stored addend (DIR32) or the matching pc-relative value (REL32). Relocations to other
symbols are masked (MASHED.exe has no base relocations to check them against), but the address each one implies is
collected, and a candidate whose implied addresses disagree with the majority map is flagged.

Adapted from SimGolf re/tools/match.py + log/roadmap/S10/7_tools/libscore.py (same masking rule, byte level).

  py -3.12 re/tools/match/libmatch.py --objs <dir|lib|obj ...> --subsystem <hooks.csv subsystem prefix> [--tsv out]
"""
import argparse
import csv
import pathlib
import struct
import sys
from collections import Counter, defaultdict

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import coffpe  # noqa: E402

try:
    import capstone
    _CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
except ImportError:  # pragma: no cover
    _CS = None


def load_objs(paths):
    objs = []

    def add(data, name):
        try:
            objs.append(coffpe.Obj(data, name))
        except (ValueError, struct.error, IndexError) as e:  # /GL objects and import members are not plain COFF i386
            pass

    for p in paths:
        p = pathlib.Path(p)
        if p.is_dir():
            for q in sorted(p.glob("*.obj")):
                add(q.read_bytes(), q.name)
        elif p.suffix.lower() == ".lib":
            for name, data in coffpe.archive_members(p.read_bytes()):
                if len(data) > 20 and struct.unpack_from("<H", data, 0)[0] == 0x14c:
                    add(data, f"{p.name}:{name}")
        else:
            add(p.read_bytes(), p.name)
    return objs


def compare_at(img, obj, fn, va, want_refs=False):
    """(identical, first_diff_offset or None, n_masked, n_soft_bad[, refs]). soft_bad = intra-object relocations to
    another function of the same section whose implied address disagrees (reported, not counted as a byte difference).
    refs = [(key, implied target va)] for every masked relocation: key is the external symbol name, or
    (object, section) for a symbol the object defines in another section (implied va = that section's base)."""
    sec = obj.sections[fn["sec"] - 1]
    body = sec["data"][fn["off"]:fn["off"] + fn["size"]]
    orig = img.read(va, len(body))
    mask = set()
    soft_bad = 0
    hard_bad = []
    refs = []
    lo, hi = fn["off"], fn["off"] + fn["size"]
    for site, si, ty in sec["relocs"]:
        if not (lo <= site < hi) or site + 4 > hi:
            continue
        rel = site - lo
        mask.update(range(rel, rel + 4))
        sym = obj.symbols.get(si)
        if sym is None or ty not in (coffpe.REL_DIR32, coffpe.REL_REL32):
            continue
        addend = struct.unpack_from("<i", body, rel)[0]
        got = struct.unpack_from("<I", orig, rel)[0]
        if sym["sec"] != fn["sec"]:
            if want_refs:
                tgt = (got - addend) if ty == coffpe.REL_DIR32 else (va + rel + 4 + got - addend)
                tgt &= 0xffffffff
                if sym["sec"] > 0 and sym["cls"] != 2:
                    refs.append(((obj.name, sym["sec"]), (tgt - sym["value"]) & 0xffffffff))
                else:
                    refs.append((sym["name"], tgt))
            continue
        tgt_off = sym["value"] + addend if ty == coffpe.REL_DIR32 else sym["value"]
        if ty == coffpe.REL_DIR32:
            want = (va - lo + tgt_off) & 0xffffffff
        else:
            want = (va - lo + sym["value"] + addend - (va + rel + 4)) & 0xffffffff
        inside = lo <= tgt_off < fn["raw_end"]
        if got != want:
            if inside:
                hard_bad.append(rel)
            else:
                soft_bad += 1
    first = None
    for i in range(len(body)):
        if i not in mask and body[i] != orig[i]:
            first = i
            break
    if hard_bad:
        first = min([first] + hard_bad) if first is not None else min(hard_bad)
    if want_refs:
        return first is None, first, len(mask), soft_bad, refs
    return first is None, first, len(mask), soft_bad


def reason(img, obj, fn, va, first):
    """One-line description of the first differing instruction (masked operand bytes ignored)."""
    if _CS is None or first is None:
        return ""
    sec = obj.sections[fn["sec"] - 1]
    body = sec["data"][fn["off"]:fn["off"] + fn["size"]]
    orig = img.read(va, len(body) + 16)
    a = list(_CS.disasm(body, 0))
    b = list(_CS.disasm(orig, 0))
    ia = next((x for x in a if x.address <= first < x.address + x.size), None)
    ib = next((x for x in b if x.address <= first < x.address + x.size), None)
    fa = f"{ia.mnemonic} {ia.op_str}".strip() if ia else "?"
    fb = f"{ib.mnemonic} {ib.op_str}".strip() if ib else "?"
    same_boundary = ia is not None and ib is not None and ia.address == ib.address
    tag = "same-insn-start" if same_boundary else "insn-misaligned"
    return f"+0x{first:x} lib[{fa}] orig[{fb}] ({tag})"


def _anchor(body, mask):
    """Longest unmasked run (offset, bytes)."""
    best = (0, b"")
    run_start = 0
    for i in range(len(body) + 1):
        if i == len(body) or i in mask:
            if i - run_start > len(best[1]):
                best = (run_start, body[run_start:i])
            run_start = i + 1
    return best


def search(img, obj, fn, limit=8):
    """Every image va where fn is identical (anchored search over .text)."""
    sec = obj.sections[fn["sec"] - 1]
    body = sec["data"][fn["off"]:fn["off"] + fn["size"]]
    if len(body) < 6:
        return []
    mask = set()
    for site, si, ty in sec["relocs"]:
        if fn["off"] <= site < fn["off"] + fn["size"]:
            mask.update(range(site - fn["off"], site - fn["off"] + 4))
    aoff, anc = _anchor(body, mask)
    if len(anc) < 5:
        return []
    out, start = [], 0
    text = img.text
    while len(out) < limit:
        k = text.find(anc, start)
        if k < 0:
            break
        start = k + 1
        va = img.text_va + k - aoff
        if va < img.text_va:
            continue
        if compare_at(img, obj, fn, va)[0]:
            out.append(va)
    return out


def hooks_rows(prefixes, hooks=None):
    hooks = hooks or coffpe.ROOT / "hooks.csv"
    rows = []
    with open(hooks, encoding="utf-8") as f:
        for r in csv.DictReader(f):
            if any(r["subsystem"] == p or r["subsystem"].startswith(p) for p in prefixes):
                try:
                    rows.append((int(r["rva"], 16), r["name"], r["subsystem"], r["confidence"]))
                except ValueError:
                    pass
    return sorted(rows)


def _key(o, f):
    return (o.name, f["name"], f["off"])


def row_extents(hooks=None, cap=0x2000):
    """va -> bytes to the next hooks.csv row of any subsystem (capped)."""
    hooks = hooks or coffpe.ROOT / "hooks.csv"
    vas = []
    with open(hooks, encoding="utf-8") as f:
        for r in csv.DictReader(f):
            try:
                vas.append(int(r["rva"], 16))
            except ValueError:
                pass
    vas = sorted(set(vas))
    return {a: min(b - a, cap) for a, b in zip(vas, vas[1:])}


_MN = {}


def mnemonics(data, tag):
    if tag not in _MN:
        _MN[tag] = [i.mnemonic for i in _CS.disasm(data, 0)] if _CS else []
    return _MN[tag]


def classify(img, obj, fn, va, first):
    """Residual category of a differing pair, from the first differing instruction pair."""
    sec = obj.sections[fn["sec"] - 1]
    body = sec["data"][fn["off"]:fn["off"] + fn["size"]]
    orig = img.read(va, len(body) + 32)
    a = {x.address: x for x in _CS.disasm(body, 0)}
    b = {x.address: x for x in _CS.disasm(orig, 0)}
    ia = next((x for x in a.values() if x.address <= first < x.address + x.size), None)
    ib = next((x for x in b.values() if x.address <= first < x.address + x.size), None)
    if ia is None or ib is None:
        return "extent (function ends earlier/later)"
    ma, mb = ia.mnemonic, ib.mnemonic
    rmw = ("add", "sub", "or", "and", "xor", "inc", "dec", "neg", "not", "shl", "sar", "shr")
    if ma in ("int3", "nop") or mb in ("int3", "nop"):
        return "extent (function ends earlier/later)"
    if ma == mb and ma.startswith("j"):
        return "branch displacement (earlier size difference)"
    if ma == mb and ia.op_str.split(",")[0] == ib.op_str.split(",")[0]:
        return "operand/constant (same instruction, different immediate or displacement)"
    if ma == mb:
        return "register allocation (same instruction, different registers)"
    if (ma in rmw and "ptr" in ia.op_str.split(",")[0]) or (mb in rmw and "ptr" in ib.op_str.split(",")[0]):
        return "read-modify-write memory op split or fused"
    if ma.startswith("f") or mb.startswith("f"):
        return "x87 sequence (fld/fstp vs integer move, or FPU op order)"
    if ma in ("push", "pop") or mb in ("push", "pop") or "esp" in ia.op_str or "esp" in ib.op_str:
        return "prologue / stack-frame or argument-load order"
    return "instruction selection or scheduling"


def pair_rows(img, fns, rows, res, used, extents, min_sim=0.6, window=0x800):
    import difflib
    fmap = {(o.name, f["name"]): (o, f) for o, f in fns}
    anchors = defaultdict(list)
    for va, r in res.items():
        anchors[r["obj"]].append(va)
    todo = [va for va, *_ in rows if va not in res]
    pool = [(o, f) for o, f in fns if _key(o, f) not in used]
    cand = []
    for va in todo:
        ext = extents.get(va, 0x400)
        orig = img.read(va, ext)
        mo = mnemonics(orig, ("img", va, ext))
        near = {on for on, vl in anchors.items() if any(abs(v - va) <= window for v in vl)}
        for o, f in pool:
            if near and o.name not in near and o.name in anchors:
                continue
            if not (0.5 <= f["size"] / max(ext, 1) <= 2.0):
                continue
            sec = o.sections[f["sec"] - 1]
            mf = mnemonics(sec["data"][f["off"]:f["off"] + f["size"]], ("obj", o.name, f["name"], f["off"]))
            sm = difflib.SequenceMatcher(None, mf, mo[:len(mf) + 8], autojunk=False)
            if sm.real_quick_ratio() < min_sim or sm.quick_ratio() < min_sim:
                continue
            ratio = sm.ratio()
            if ratio >= min_sim:
                cand.append((ratio, va, o, f))
    for ratio, va, o, f in sorted(cand, key=lambda t: -t[0]):
        if va in res or _key(o, f) in used:
            continue
        ok, first, nm, soft = compare_at(img, o, f, va)
        if ok:
            res[va] = {"status": "identical", "obj": o.name, "func": f["name"], "size": f["size"], "first": "",
                       "why": f"paired by similarity {ratio:.2f}"}
        else:
            res[va] = {"status": "differs", "obj": o.name, "func": f["name"], "size": f["size"],
                       "first": f"0x{first:x}", "category": classify(img, o, f, va, first),
                       "why": f"mnemonic similarity {ratio:.2f}; " + reason(img, o, f, va, first)}
        used.add(_key(o, f))


def run(img, objs, rows, do_search=True, extents=None, pair=True):
    """Returns (res, extra, fns, used). res[va] = dict(status identical|identical-ambiguous|differs, obj, func, size,
    first, why); extra = identical object functions found at addresses that are not rows."""
    fns = [(o, f) for o in objs for f in o.functions() if f["size"] > 0]
    rowset = {r[0]: r for r in rows}
    # 1. every identical candidate at every row
    cands = defaultdict(list)
    for va in rowset:
        for o, f in fns:
            r = compare_at(img, o, f, va, True)
            if r[0]:
                cands[va].append((o, f, r))
    # 2. votes from unambiguous rows: object-section base (va - offset) and external symbol addresses
    base_votes, ref_votes = defaultdict(Counter), defaultdict(Counter)
    for va, cl in cands.items():
        if len({(o.name, f["name"]) for o, f, _ in cl}) == 1:
            o, f, r = cl[0]
            base_votes[(o.name, f["sec"])][va - f["off"]] += 1
            for k, t in r[4]:
                ref_votes[k][t] += 1

    def score(o, f, r, va):
        s = 0
        bv = base_votes.get((o.name, f["sec"]))
        if bv and (va - f["off"]) in bv:
            s += 2 * bv[va - f["off"]]
        for k, t in r[4]:
            rv = ref_votes.get(k)
            if rv:
                s += 1 if rv.most_common(1)[0][0] == t else -1
        return s

    res, used = {}, set()
    for va, cl in cands.items():
        ranked = sorted(cl, key=lambda c: (score(c[0], c[1], c[2], va), c[1]["size"]), reverse=True)
        o, f, r = ranked[0]
        top = score(o, f, r, va)
        ties = sorted({c[1]["name"] for c in ranked if score(c[0], c[1], c[2], va) == top and c[1]["size"] == f["size"]})
        bad = [k for k, t in r[4] if k in ref_votes and ref_votes[k].most_common(1)[0][0] != t]
        why = f"masked {r[2]} B"
        if r[3]:
            why += f"; {r[3]} intra-obj reloc implied addr differs"
        if bad:
            why += f"; {len(bad)} ext ref(s) disagree with majority map"
        status = "identical"
        if len(ties) > 1:
            status = "identical-ambiguous"
            why += "; same bytes as " + ",".join(t for t in ties if t != f["name"])
        res[va] = {"status": status, "obj": o.name, "func": f["name"], "size": f["size"], "first": "", "why": why}
        used.add(_key(o, f))
    # 3. differing functions: pair each remaining row with the unused object function whose instruction-mnemonic
    #    sequence is most similar (difflib ratio >= min_sim), searched among objects with an identical anchor within
    #    0x800 bytes of the row (all objects when none is). Greedy, best ratio first.
    if pair:
        pair_rows(img, fns, rows, res, used, extents or {})
    # 4. search the image for remaining identical functions (not at a listed RVA)
    extra = []
    if do_search:
        for o, f in fns:
            if _key(o, f) in used:
                continue
            if f["size"] < 16:      # short bodies match by chance anywhere
                continue
            for va in search(img, o, f):
                if va in rowset and va not in res:
                    res[va] = {"status": "identical", "obj": o.name, "func": f["name"], "size": f["size"],
                               "first": "", "why": "found by search"}
                    used.add(_key(o, f))
                elif va not in rowset:
                    extra.append((va, o.name, f["name"], f["size"]))
    return res, extra, fns, used


def counts(res, rows):
    n_id = sum(1 for r in res.values() if r["status"].startswith("identical"))
    n_amb = sum(1 for r in res.values() if r["status"] == "identical-ambiguous")
    n_df = sum(1 for r in res.values() if r["status"] == "differs")
    return n_id, n_amb, n_df, len(rows) - n_id - n_df


def write_tsv(path, rows, res, extra, libname=""):
    with open(path, "w", encoding="utf-8", newline="") as fo:
        fo.write("rva\thooks_name\tsubsystem\tconfidence\tstatus\tlibrary_object\tlib_function\tlib_size\t"
                 "first_diff\tcategory\tnote\n")
        for va, name, sub, conf in rows:
            r = res.get(va)
            if r:
                fo.write(f"0x{va:08x}\t{name}\t{sub}\t{conf}\t{r['status']}\t{libname}{r['obj']}\t{r['func']}\t"
                         f"{r['size']}\t{r['first']}\t{r.get('category', '')}\t{r['why']}\n")
            else:
                fo.write(f"0x{va:08x}\t{name}\t{sub}\t{conf}\tnot-compared\t\t\t\t\t\tno library function paired\n")
        for va, oname, fname, size in sorted(extra):
            fo.write(f"0x{va:08x}\t(not in hooks.csv)\t\t\tidentical-unlisted\t{libname}{oname}\t{fname}\t{size}\t\t\t\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--objs", nargs="+", required=True)
    ap.add_argument("--subsystem", nargs="+", required=True)
    ap.add_argument("--tsv")
    ap.add_argument("--label", default="")
    ap.add_argument("--no-search", action="store_true")
    a = ap.parse_args()
    img = coffpe.Image()
    objs = load_objs(a.objs)
    rows = hooks_rows(a.subsystem)
    res, extra, fns, used = run(img, objs, rows, not a.no_search, row_extents())
    n_id, n_amb, n_df, n_nc = counts(res, rows)
    print(f"{a.label} rows {len(rows)} identical {n_id} (ambiguous {n_amb}) differs {n_df} not-compared {n_nc} "
          f"obj-functions {len(fns)} unused {len(fns) - len(used)} identical-at-unlisted {len(extra)}")
    if a.tsv:
        write_tsv(a.tsv, rows, res, extra)
    return res


if __name__ == "__main__":
    main()
