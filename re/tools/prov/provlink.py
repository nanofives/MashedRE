"""PROV-INSITU (2026-11-11): link the XP DDK object of a match source into a position-independent blob that a Frida
agent loads into the ORIGINAL MASHED.exe and reaches through an inline JMP at the function's RVA.

This is the in-process form of "link the DDK object into a dev asi": the object is compiled exactly as match.py does
(re/tools/match/match.py compile_obj, the source's // FLAGS / LANG / TOOLCHAIN), and its relocations are resolved
against the original image instead of the exe:

  FUN_<va8> (any decoration)            -> 0x<va8>   (the ORIGINAL function, even when this object defines it:
                                                      each run tests one body per RVA, everything else stays original)
  MGLOBAL / MGLOBALA name               -> the address in the source
  DAT_<va8> / PTR_DAT_<va8> / _<va8>    -> 0x<va8>
  CRT helpers, __imp__ IAT slots        -> re/tools/match/equiv_symmap.tsv, then the original's import table
  absolute symbols (sec -1)             -> their value
  this object's own sections            -> copied into the blob (the function's COMDAT, __real@ literals, strings,
                                           switch tables, static helpers it calls), DIR32 fixed up at load time

Anything else is UNRESOLVED and the function is not wired (no guessing).

    py -3.12 re/tools/prov/provlink.py --rvas 0x004019d0,... | --tier 95 [--out bodies.json]

Output JSON: {"bodies": [{rva, source, pct, status, code (hex), abs (offsets to add the blob base to),
               ext (offset -> absolute target, rel32 to patch at load), entry, size, writes {...}}]}
"""
from __future__ import annotations
import argparse, csv, json, pathlib, re, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(HERE.parent / "match"))
import match as M  # noqa: E402

REL_DIR32, REL_REL32, REL_DIR32NB, REL_SECREL = 0x06, 0x14, 0x07, 0x0B
RE_G = re.compile(r"MGLOBALA?\(\s*[^,]+?,\s*(\w+)\s*,\s*(?:[^,;\n]+,\s*)?(0x[0-9a-fA-F]+)\s*\)")
RE_ADDRNAME = re.compile(r"^(?:FUN|DAT|PTR_DAT|PTR_FUN|LAB|thunk_FUN|PTR|s_\w*|u_\w*|switchD|caseD)_?([0-9a-fA-F]{8})$")
DATA_LO, DATA_HI = 0x005EA000, 0x00915008     # the per-tick compared range (detorig.js)


def undecorate(n: str) -> str:
    n = n.lstrip("_@")
    return n.split("@")[0]


_SYMMAP = None
_IMPORTS = None


def symmap():
    global _SYMMAP
    if _SYMMAP is None:
        _SYMMAP = {}
        for ln in (HERE.parent / "match" / "equiv_symmap.tsv").read_text().splitlines():
            if ln.startswith("#") or not ln.strip():
                continue
            f = ln.split("\t")
            _SYMMAP[f[0]] = int(f[1], 16)
    return _SYMMAP


def imports():
    """__imp__Name@N -> IAT slot VA, from the original's import table (names only, decoration dropped)."""
    global _IMPORTS
    if _IMPORTS is None:
        import pefile
        p = pefile.PE(str(M.EXE))
        _IMPORTS = {}
        for e in p.DIRECTORY_ENTRY_IMPORT:
            for i in e.imports:
                if i.name:
                    _IMPORTS[i.name.decode()] = i.address
    return _IMPORTS


def resolve_ext(name: str, gl: dict):
    """Absolute VA for an undefined (or deliberately external) symbol, or None."""
    if name in symmap():
        return symmap()[name]
    u = undecorate(name)
    if u in gl:
        return gl[u]
    m = re.fullmatch(r"(?:thunk_)?FUN_([0-9a-fA-F]{8})", u)
    if m:
        return int(m.group(1), 16)
    m = RE_ADDRNAME.match(u)
    if m:
        return int(m.group(1), 16)
    if name.startswith("__imp__"):
        base = name[len("__imp__"):].split("@")[0]
        return imports().get(base)
    return None


def link(src: pathlib.Path, rva: int):
    text = src.read_text(errors="replace")
    ann = dict(M.parse_annotations(text))
    if rva not in ann:
        return {"status": "NOANN"}
    want = ann[rva]
    gl = {name: int(a, 16) for name, a in RE_G.findall(text)}
    fl = M.file_flags(text) or M.DEFAULT_FLAGS
    try:
        obj = M.compile_obj(src, fl, M.file_lang(text), M.file_toolchain(text))
    except SystemExit as e:
        return {"status": "COMPILE-FAIL", "detail": str(e).splitlines()[-1][:200] if str(e) else ""}
    secs, syms = M.coff(obj)
    by_index = {s["index"]: s for s in syms}
    fsyms = [s for s in syms if s["sec"] > 0 and s["type"] == 0x20 and (s["name"] == want or undecorate(s["name"]) == undecorate(want))]
    if not fsyms:
        return {"status": "NOSYM", "detail": want}
    fs = fsyms[0]
    # the target function is defined by ITS section; any other symbol named FUN_<va8> is the original's
    target_sec = fs["sec"]
    layout, order, blob = {}, [], bytearray()
    unresolved, absfix, ext = set(), [], {}

    def place(si):
        if si in layout:
            return
        sec = secs[si - 1]
        while len(blob) % 16:
            blob.append(0xCC)
        layout[si] = len(blob)
        order.append(si)
        blob.extend(sec["data"] if sec["data"] else b"\0" * 0)
        for _, symidx, _ in sec["relocs"]:
            t = by_index.get(symidx, {})
            ts = t.get("sec", 0)
            if ts > 0 and not is_external_fun(t, si):
                place(ts)

    def is_external_fun(t, from_sec):
        """A defined FUN_<va8> other than the target is taken from the original (one body per RVA under test)."""
        u = undecorate(t.get("name", ""))
        return t.get("sec", 0) != target_sec and re.fullmatch(r"FUN_[0-9a-fA-F]{8}", u) is not None and t.get("type") == 0x20

    # .bss-style sections (no raw data) would need zero fill with their size: not expected in match sources
    place(target_sec)
    for si in order:
        sec = secs[si - 1]
        base = layout[si]
        for off, symidx, typ in sec["relocs"]:
            t = by_index.get(symidx, {})
            site = base + off
            stored = struct.unpack_from("<i", blob, site)[0]
            ts = t.get("sec", 0)
            if ts > 0 and not is_external_fun(t, si):
                tv = layout[ts] + t["value"]
                if typ == REL_DIR32:
                    struct.pack_into("<i", blob, site, tv + stored)
                    absfix.append(site)
                elif typ == REL_REL32:
                    struct.pack_into("<i", blob, site, tv + stored - (site + 4))
                else:
                    unresolved.add(f"{t.get('name')}(type {typ:#x})")
                continue
            if ts == -1:                       # absolute symbol
                va = t["value"]
            else:
                va = resolve_ext(t.get("name", "?"), gl) if ts == 0 else int(undecorate(t["name"])[-8:], 16)
            if va is None:
                unresolved.add(t.get("name", "?"))
                continue
            if typ == REL_DIR32:
                struct.pack_into("<I", blob, site, (va + stored) & 0xFFFFFFFF)
            elif typ == REL_REL32:
                ext[site] = (va + stored) & 0xFFFFFFFF
            else:
                unresolved.add(f"{t.get('name')}(type {typ:#x})")
    entry = layout[target_sec] + fs["value"]
    res = {"status": "UNRESOLVED" if unresolved else "OK", "unresolved": sorted(unresolved), "sym": fs["name"],
           "code": blob.hex(), "abs": absfix, "ext": {str(k): v for k, v in ext.items()}, "entry": entry,
           "size": len(blob), "flags": fl}
    if not unresolved:
        res["writes"] = write_set(bytes(blob), entry, layout[target_sec], secs[target_sec - 1], fs, syms, ext, absfix)
    return res


def write_set(blob: bytes, entry: int, secbase: int, sec, fs, syms, ext, absfix):
    """Static store list of the function body (capstone, our DDK code): frame (esp, or ebp after push ebp/mov ebp,esp),
    absolute (a VA: inside the compared image range or not), indirect (register base: pointee unknown statically)."""
    import capstone
    from capstone import x86
    later = sorted(x["value"] for x in syms if x["sec"] == fs["sec"] and x["type"] == 0x20 and x["value"] > fs["value"])
    end = secbase + (later[0] if later else len(sec["data"]))
    code = blob[entry:end]
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    frame_ebp = code[:3] == b"\x55\x8b\xec"
    out = {"frame": 0, "abs_in": [], "abs_out": [], "indirect": [], "calls": 0}
    absset = set(absfix)
    for ins in md.disasm(code, entry):
        if ins.mnemonic.startswith("call"):
            out["calls"] += 1
        for op in ins.operands:
            if op.type != x86.X86_OP_MEM or not (op.access & capstone.CS_AC_WRITE):
                continue
            base = ins.reg_name(op.mem.base) if op.mem.base else ""
            idx = ins.reg_name(op.mem.index) if op.mem.index else ""
            if base == "esp" or (base == "ebp" and frame_ebp):
                out["frame"] += 1
                continue
            if not base and not idx:
                va = op.mem.disp & 0xFFFFFFFF
                # a DIR32 into our own blob (a literal pool) is not a store target in practice; keep it listed
                tgt = "abs_in" if DATA_LO <= va < DATA_HI else "abs_out"
                out[tgt].append(f"{va:#010x}@{ins.address - entry:#x}")
                continue
            if not base and idx:
                va = op.mem.disp & 0xFFFFFFFF
                tgt = "abs_in" if DATA_LO <= va < DATA_HI else "abs_out"
                out[tgt].append(f"{va:#010x}[{idx}]@{ins.address - entry:#x}")
                continue
            out["indirect"].append(f"[{base}{'+' + idx if idx else ''}{op.mem.disp:+#x}]@{ins.address - entry:#x}")
    return out


def blocking_rows():
    bl = [l.split("\t")[0].strip().lower() for l in (ROOT / "verify/equivcheck_20261111/blocking_c0c3.tsv").read_text().splitlines() if l.startswith("0x")]
    meas = {r["rva"].lower(): r for r in csv.DictReader(open(ROOT / "verify/equivcheck_20261111/measure_v4.tsv"), delimiter="\t")}
    return bl, meas


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--rvas", default="")
    ap.add_argument("--tier", type=float, default=None, help="every blocking function with a source at >= this best_pct")
    ap.add_argument("--below", type=float, default=101.0)
    ap.add_argument("--out", default="")
    a = ap.parse_args()
    bl, meas = blocking_rows()
    if a.rvas:
        rvas = [x.strip().lower() for x in a.rvas.split(",") if x.strip()]
    else:
        rvas = [r for r in bl if r in meas and a.tier <= float(meas[r]["best_pct"] or 0) < a.below]
    bodies = []
    for r in rvas:
        m = meas.get(r)
        if not m:
            bodies.append({"rva": r, "status": "NOSOURCE"}); continue
        src = ROOT / m["source"]
        res = link(src, int(r, 16))
        res.update({"rva": r, "source": m["source"], "pct": float(m["best_pct"] or 0), "equiv": m["verdict"]})
        size = M.ghidra_size(int(r, 16))
        res["orig_size"] = size
        if res["status"] == "OK" and size is not None and size < 5:
            res["status"] = "TOO-SHORT"
        bodies.append(res)
        print(f"{r}\t{res['pct']:5.1f}\t{res['status']}\t{res.get('size', '')}\t{','.join(res.get('unresolved', []))[:150]}"
              + (f"\tw={len(res['writes']['abs_in'])}/{len(res['writes']['abs_out'])}/{len(res['writes']['indirect'])}" if res.get("writes") else ""),
              flush=True)
    if a.out:
        pathlib.Path(a.out).write_text(json.dumps({"bodies": bodies}, indent=0))
    from collections import Counter
    print(Counter(b["status"] for b in bodies))


if __name__ == "__main__":
    main()
