"""Whole-symbol byte compare with same-section relocations resolved by type (C4R-NEARMISS, 2026-11-07).

re/tools/match/match.py byte_compare() checks every relocation that targets the function's own section as an
absolute address: ours = target + stored addend, theirs = the original's raw dword - addr. That is right for
DIR32 (jump tables) and wrong for REL32 (0x14): a self-recursive `call` (E8 rel32) stores a displacement, so
the check always fails on its first byte. This script repeats byte_compare() with REL32 resolved as
    theirs = site + 4 + signed(rel32)      (target offset from the function start in the original)
    ours   = symbol offset + stored addend (target offset in our COMDAT; addend 0 for MSVC REL32)
and prints, per row: n bytes, bad bytes under match.py's rule, bad bytes under the typed rule, and every
same-section relocation with its type and target symbol.

    py -3.12 verify/c4rnearmiss_20261107/selfcall_bytecheck.py <src.c> 0x<addr> [...]
"""
import pathlib, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match  # noqa: E402

REL_DIR32, REL_REL32 = 0x06, 0x14


def check(src: pathlib.Path, addr: int):
    text = src.read_text()
    sym = dict(match.parse_annotations(text))[addr]
    fl = match.file_flags(text) or match.DEFAULT_FLAGS
    secs, syms = match.coff(match.compile_obj(src, fl, match.file_lang(text), match.file_toolchain(text)))
    n, bad_tool = match.byte_compare(addr, secs, syms, sym)
    mine, relocs, full, _ = match.function_from_obj(secs, syms, sym)
    while mine and mine[-1] in (0xCC, 0x90):
        mine = mine[:-1]
    orig = match.original_bytes(addr, len(mine))
    s = [x for x in syms if x["sec"] > 0 and x["type"] == 0x20 and x["name"] == full][0]
    start = s["value"]
    by_index = {x["index"]: x for x in syms}
    sec = secs[s["sec"] - 1]
    masked = {r + k for r in relocs for k in range(4)}
    bad = [i for i in range(len(mine)) if i not in masked and mine[i] != orig[i]]
    rows = []
    for off, symidx, rtype in sec["relocs"]:
        t = by_index.get(symidx, {})
        site = off - start
        if not (0 <= site < len(mine)) or t.get("sec") != s["sec"]:
            continue
        addend = struct.unpack_from("<i", mine, site)[0]
        ours = t["value"] - start + addend
        raw = struct.unpack_from("<i", orig, site)[0]
        if rtype == REL_REL32:
            theirs = site + 4 + raw
        elif rtype == REL_DIR32:
            theirs = (raw & 0xFFFFFFFF) - addr
        else:
            theirs = None
        ok = theirs == ours
        rows.append((site, rtype, t.get("name"), ours, theirs, ok))
        if not ok:
            bad.append(site)
    return full, fl, n, sorted(bad_tool), sorted(bad), rows


def main():
    src = pathlib.Path(sys.argv[1])
    for a in sys.argv[2:]:
        full, fl, n, bt, bf, rows = check(src, int(a, 16))
        verdict = "BYTE-IDENTICAL" if not bf else f"{len(bf)} bad"
        print(f"{a}\t{full}\tflags={fl}\tbytes={n}\tmatch_py_bad={len(bt)}\ttyped_bad={len(bf)}\t{verdict}")
        for site, rtype, name, ours, theirs, ok in rows:
            print(f"   +0x{site:x} type=0x{rtype:02x} -> {name} ours_off=0x{ours:x} orig_off="
                  f"{'n/a' if theirs is None else hex(theirs)} {'OK' if ok else 'BAD'}")


if __name__ == "__main__":
    main()
