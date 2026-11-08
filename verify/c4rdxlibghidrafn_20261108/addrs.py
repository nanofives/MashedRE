"""C4R-DXLIB-GHIDRAFN (2026-11-08): build the list of library-identical addresses that need a Ghidra function, and
re-check the bytes at each one independently of the earlier TSVs.

  MSYS_NO_PATHCONV=1 py -3.12 verify/c4rdxlibghidrafn_20261108/addrs.py

Inputs:
  d3dx9   verify/c4rdxlibfu_20261107/candidates.tsv, refused rows whose reason is `in no pc_funcs body` (E, 55 single
          candidate) or `inside` another pc_funcs body (D), against tools/dxsdk9/sum2003/Lib/d3dx9.lib.
  dshow   verify/c4rdshownewrows_20261108/candidates.tsv, single-candidate rows refused only for `no pc_funcs boundary`
          (83), against a fresh build of the DX 9.0b BaseClasses source with rel_ob1 flags (/Gz /MT /GX /O2 /Ob1),
          the C4R-DSHOW-BASECLASSES recipe.
Per address: libmatch.compare_at for the named symbol (identical = every non-relocation byte equal, intra-function
relocations checked), the count of library functions identical at the address (uniqueness), and the pc_funcs.csv
bodies that overlap [va, va+size).
Outputs (beside this file): create.tsv (the creation list for CreateLibFunctions.java: va size), addrs.tsv (every
address with its checks), inside.tsv (the D labels with the split test).
"""
import csv, pathlib, sys, tempfile, collections
HERE = pathlib.Path(__file__).resolve().parent
W = HERE.parents[1]
sys.path.insert(0, str(W / "re/tools/match"))
import coffpe, libmatch, period_cc  # noqa: E402
import capstone  # noqa: E402

M = period_cc.MAIN
DX = M / "tools/dxsdk9/sum2003/Lib/d3dx9.lib"
BC = M / "tools/dxsdk9/sum2003/Samples/C++/DirectShow/BaseClasses"
INCS = [BC, W / "verify/c4rdshowbase_20261107/stubinc", M / "tools/dxsdk9/sum2003/Include",
        M / "tools/vc7ddk/inc/wxp", M / "tools/vc7ddk/inc/crt"]
REL = ["NDEBUG", "WIN32", "_MBCS", "_LIB", "_WIN32_DCOM", "WINVER=0x400"]
FLAGS = "/Gz /MT /GX /O2 /Ob1"


def pcfuncs():
    out = []
    for r in csv.DictReader(open(W / "re/console/pc_funcs.csv", encoding="utf-8")):
        e = int(r["entry"], 16)
        out.append((e, e + int(r["body_size"]), r["name"]))
    return sorted(out)


def overlaps(pf, lo, hi):
    return [f"{n}@0x{e:08x}+{hi_ - e}" for e, hi_, n in pf if e < hi and lo < hi_]


def check(img, objs, fns, lib, va, objname, sym):
    o = next((o for o in objs if o.name == objname), None)
    f = next((f for f in (o.functions() if o else []) if f["name"] == sym and f["size"] > 0), None)
    if f is None:
        return None
    ident, first, nmask, soft = libmatch.compare_at(img, o, f, va)
    n_id = sorted({(oo.name, ff["name"]) for oo, ff in fns if libmatch.compare_at(img, oo, ff, va)[0]})
    return {"lib": lib, "va": va, "obj": objname, "sym": sym, "size": f["size"], "identical": ident,
            "first_diff": "" if first is None else first, "masked": nmask, "soft_bad": soft,
            "n_identical_lib_fns": len(n_id)}


def main():
    img = coffpe.Image()
    pf = pcfuncs()
    rows = []
    # d3dx9
    dobjs = libmatch.load_objs([DX])
    dfns = [(o, f) for o in dobjs for f in o.functions() if f["size"] > 0]
    inside = []
    for r in csv.DictReader(open(W / "verify/c4rdxlibfu_20261107/candidates.tsv", encoding="utf-8"), delimiter="\t"):
        w = r["why"]
        if r["verdict"] == "add" or "ambiguous" in w:
            continue
        if "in no pc_funcs body" in w:
            c = check(img, dobjs, dfns, "d3dx9", int(r["rva"], 16), r["library_object"], r["lib_function"])
            c["src"] = "C4R-DXLIB-FU E"
            rows.append(c)
        elif "inside" in w and "no pc_funcs" in w:
            inside.append(r)
    # dshow
    out = pathlib.Path(tempfile.mkdtemp(prefix="dsb_ghidrafn_"))
    errs = period_cc.compile_all(sorted(BC.glob("*.cpp")), out, FLAGS, INCS, REL)
    for e in errs:
        print("  compile:", e[:200])
    bobjs = libmatch.load_objs([out])
    bfns = [(o, f) for o in bobjs for f in o.functions() if f["size"] > 0]
    print(f"d3dx9 objs {len(dobjs)} fns {len(dfns)}; BaseClasses rel_ob1 objs {len(bobjs)} fns {len(bfns)}", flush=True)
    for r in csv.DictReader(open(W / "verify/c4rdshownewrows_20261108/candidates.tsv", encoding="utf-8"), delimiter="\t"):
        if r["why"] != "no pc_funcs boundary at rva":
            continue
        c = check(img, bobjs, bfns, "dshow", int(r["rva"], 16), r["library_object"], r["lib_function"])
        c["src"] = "C4R-DSHOW-NEWROWS"
        rows.append(c)
    keep = []
    for c in rows:
        ov = overlaps(pf, c["va"], c["va"] + c["size"])
        c["pc_overlap"] = ";".join(ov)
        ok = c["identical"] and c["n_identical_lib_fns"] == 1 and not ov
        c["verdict"] = "create" if ok else "skip"
        c["why"] = "; ".join(x for x in [
            "" if c["identical"] else f"bytes differ at +{c['first_diff']}",
            "" if c["n_identical_lib_fns"] == 1 else f"{c['n_identical_lib_fns']} library functions identical",
            "" if not ov else "overlaps pc_funcs " + ";".join(ov)] if x)
        if ok:
            keep.append(c)
    cols = ["lib", "va", "size", "obj", "sym", "identical", "first_diff", "masked", "soft_bad", "n_identical_lib_fns",
            "pc_overlap", "verdict", "why", "src"]
    with open(HERE / "addrs.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(cols)
        for c in sorted(rows, key=lambda c: c["va"]):
            w.writerow([f"0x{c[k]:08x}" if k == "va" else c[k] for k in cols])
    with open(HERE / "create.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("# va size lib sym (from addrs.py)\n")
        for c in sorted(keep, key=lambda c: c["va"]):
            f.write(f"0x{c['va']:08x} {c['size']} {c['lib']} {c['sym']}\n")
    print("addresses", len(rows), collections.Counter((c["lib"], c["verdict"]) for c in rows))
    for c in rows:
        if c["verdict"] != "create":
            print("  skip", c["lib"], f"0x{c['va']:08x}", c["sym"][:40], "|", c["why"])
    # D: labels inside another pc_funcs body. Split test: the outer body's flow must end before the label (last
    # instruction before the label is ret/jmp, or only padding sits between), and no branch in [entry, label) may
    # target an address >= label inside the outer body.
    cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    with open(HERE / "inside.tsv", "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["va", "sym", "lib_size", "outer", "outer_entry", "outer_end", "last_insn_before", "falls_through",
                    "branches_into_or_past", "verdict"])
        for r in inside:
            va = int(r["rva"], 16)
            e, hi, n = next((e, hi, n) for e, hi, n in pf if e <= va < hi)
            code = img.read(e, hi - e)
            last, ft, into = "", True, []
            for i in cs.disasm(code, e):
                if i.address >= va:
                    break
                pad = i.mnemonic in ("nop", "int3") or (i.mnemonic == "lea" and i.op_str in (
                    "esi, [esi]", "edi, [edi]", "esi, [esi + 0]", "edi, [edi + 0]", "esp, [esp]"))
                if pad:
                    continue  # alignment fill: the flow verdict comes from the last real instruction
                last = f"0x{i.address:08x} {i.mnemonic} {i.op_str}"
                ft = i.mnemonic not in ("ret", "jmp")
                if i.mnemonic.startswith("j") or i.mnemonic in ("call", "loop"):
                    try:
                        t = int(i.op_str, 16)
                        if va <= t < hi:
                            into.append(f"0x{i.address:08x}->0x{t:08x}")
                    except ValueError:
                        pass
                if i.mnemonic in ("ret", "jmp"):
                    ft = False
            verdict = "split-proven" if not ft and not into else "no-split"
            w.writerow([r["rva"], r["lib_function"][:60], r["lib_size"], n, f"0x{e:08x}", f"0x{hi:08x}", last, ft,
                        " ".join(into[:6]) + (f" (+{len(into) - 6})" if len(into) > 6 else ""), verdict])
            print("  inside", r["rva"], r["lib_function"][:30], n, verdict, last, "into", len(into))


if __name__ == "__main__":
    main()
