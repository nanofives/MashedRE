"""C4R-AC80-EXTENT (2026-11-07): byte-for-byte proof that Ghidra's body of 0x0040ac80 = the 80 B MATCH symbol + the
four HIDDEN handler symbols of re/match/s02/0040ac80.c + inter-function padding.

Inputs: ghidra_extent.txt (FuncRangesPC.java on Mashed_pool0, read-only: body ranges, instructions, owner of every byte
0x0040abb0..0x0040acdf), the source compiled exactly as match.py does (XP DDK, the file's // FLAGS), and the original
bytes of original/MASHED.exe.unpatched.

For every member the obj symbol's bytes (code + tables, trailing 0xcc/0x90 kept and listed) are laid at the member's
RVA. Every byte that is not a relocation site must equal the original. Every relocation site is resolved from the obj
(MGLOBAL address + addend, FUN_<va8> name, or a label of the same section = member RVA + label offset) and must equal
the original operand (DIR32: the stored dword, REL32: site + 4 + disp). Bytes of the window that no member covers must
be outside Ghidra's body and be padding.

    py -3.12 verify/c4rac80extent_20261107/extent_proof.py  > verify/c4rac80extent_20261107/extent_proof.txt
"""
import pathlib, re, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match  # noqa: E402

SRC = ROOT / "re" / "match" / "s02" / "0040ac80.c"
ENTRY = 0x0040AC80
RE_G = re.compile(r"MGLOBALA?\(\s*[^,]+?,\s*(\w+)\s*,\s*(?:[^,]+,\s*)?(0x[0-9a-fA-F]+)\s*\)")
REL_DIR32, REL_REL32 = 0x06, 0x14


def ghidra():
    ranges, owner = [], {}
    for ln in (HERE / "ghidra_extent.txt").read_text().splitlines():
        p = ln.split()
        if p[0] == "range":
            ranges.append((int(p[1], 16), int(p[2], 16)))
        elif p[0] == "byte":
            owner[int(p[1], 16)] = p[4].split("=")[1]
    return ranges, owner


def main():
    text = SRC.read_text()
    gl = {n: int(a, 16) for n, a in RE_G.findall(text)}
    members = [(a, s) for a, s in match.parse_annotations(text)]
    members += [(int(a, 16), s) for a, s, _ in re.findall(r"//\s*HIDDEN:\s*(0x[0-9a-fA-F]+)\s+(\S+)\s+(\d+)", text)]
    fl = match.file_flags(text) or match.DEFAULT_FLAGS
    secs, syms = match.coff(match.compile_obj(SRC, fl, match.file_lang(text), match.file_toolchain(text)))
    by_index = {s["index"]: s for s in syms}
    print(f"source {SRC.relative_to(ROOT).as_posix()} flags {fl} toolchain {match.file_toolchain(text)}")
    print(f"original {match.EXE}")
    cover = {}  # va -> member
    total_bad = 0
    for rva, sym in sorted(members):
        fn = [x for x in syms if x["sec"] > 0 and x["type"] == 0x20]
        s = ([x for x in fn if x["name"] == sym] or [x for x in fn if x["name"] == "_" + sym])[0]
        sec = secs[s["sec"] - 1]
        start = s["value"]
        later = sorted(x["value"] for x in syms if x["sec"] == s["sec"] and x["type"] == 0x20 and x["value"] > start)
        end = later[0] if later else len(sec["data"])
        mine = sec["data"][start:end]
        orig = match.original_bytes(rva, len(mine))
        rel = {}
        for off, idx, typ in sec["relocs"]:
            if start <= off < end:
                rel[off - start] = (by_index[idx], typ)
        masked = {k + j for k in rel for j in range(4)}
        bad = [i for i in range(len(mine)) if i not in masked and mine[i] != orig[i]]
        rlines = []
        for site, (t, typ) in sorted(rel.items()):
            stored = struct.unpack_from("<i", mine, site)[0]
            name = t["name"].lstrip("_")
            if t["sec"] == s["sec"]:
                exp, how = rva + (t["value"] - start) + (stored if typ == REL_DIR32 else 0), "same-section label"
            elif name in gl:
                exp, how = gl[name] + (stored if typ == REL_DIR32 else 0), "MGLOBAL"
            elif re.fullmatch(r"FUN_[0-9a-fA-F]{8}", name):
                exp, how = int(name[-8:], 16) + (stored if typ == REL_DIR32 else 0), "FUN_ name"
            else:
                exp, how = None, "UNRESOLVED"
            if typ == REL_DIR32:
                theirs = struct.unpack_from("<I", orig, site)[0]
            elif typ == REL_REL32:
                theirs = (rva + site + 4 + struct.unpack_from("<i", orig, site)[0]) & 0xFFFFFFFF
            else:
                theirs, how = None, f"type {typ:#x}"
            ok = exp is not None and theirs is not None and (exp & 0xFFFFFFFF) == theirs
            if not ok:
                bad.append(site)
            rlines.append(f"    reloc 0x{rva + site:08x} {'DIR32' if typ == REL_DIR32 else 'REL32'} -> {t['name']} "
                          f"({how}) ours 0x{(exp or 0) & 0xFFFFFFFF:08x} orig 0x{(theirs or 0):08x} {'OK' if ok else 'BAD'}")
        trail = len(mine)
        while trail and mine[trail - 1] in (0xCC, 0x90):
            trail -= 1
        sym = s["name"]
        print(f"member {sym} sec {s['sec']} +0x{start:x} @0x{rva:08x}: obj symbol {len(mine)} B -> 0x{rva:08x}..0x{rva + len(mine) - 1:08x} "
              f"(trailing pad {len(mine) - trail} B), relocs {len(rel)}, differing bytes {len(bad)}")
        print(f"    obj  {mine.hex()}")
        print(f"    orig {orig.hex()}")
        for l in rlines:
            print(l)
        total_bad += len(bad)
        for i in range(len(mine)):
            cover[rva + i] = sym
    ranges, owner = ghidra()
    body = {a for lo, hi in ranges for a in range(lo, hi + 1)}
    lo, hi = min(body), ENTRY + 0x50 - 1
    print(f"ghidra body: {len(body)} B in {len(ranges)} ranges: "
          + ", ".join(f"0x{a:08x}..0x{b:08x} ({b - a + 1})" for a, b in ranges))
    uncovered_body = sorted(a for a in body if a not in cover)
    print(f"ghidra body bytes outside every member symbol: {len(uncovered_body)}")
    gaps, cur = [], None
    for a in range(lo, hi + 1):
        if a in cover:
            cur = None
            continue
        b = match.original_bytes(a, 1)[0]
        if cur is None:
            cur = [a, a, []]
            gaps.append(cur)
        cur[1] = a
        cur[2].append(b)
    nonpad = 0
    for a, b, v in gaps:
        inbody = [x for x in range(a, b + 1) if x in body]
        pad = all(x == 0x90 for x in v)
        nonpad += 0 if pad else 1
        print(f"gap 0x{a:08x}..0x{b:08x} ({b - a + 1} B) bytes {bytes(v).hex()} "
              f"{'pad 0x90' if pad else 'NOT PAD'}; in ghidra body {len(inbody)}; owner "
              f"{','.join(sorted({owner.get(x, '?') for x in range(a, b + 1)}))}")
    for a in range(lo, hi + 1):
        if a in body and owner.get(a) not in ("0040ac80",):
            print(f"owner mismatch 0x{a:08x} {owner.get(a)}")
    span = hi - lo + 1
    print(f"window 0x{lo:08x}..0x{hi:08x} = {span} B = members {len(cover)} B + gaps "
          f"{sum(b - a + 1 for a, b, _ in gaps)} B")
    member_extra = sorted(a for a in cover if a not in body)
    print(f"member bytes outside the ghidra body: {len(member_extra)} "
          f"(0x{member_extra[0]:08x}..0x{member_extra[-1]:08x})" if member_extra else "member bytes outside the ghidra body: 0")
    ok = total_bad == 0 and not uncovered_body and nonpad == 0
    print(f"VERDICT {'PROVEN' if ok else 'NOT PROVEN'}: differing bytes {total_bad}, body bytes uncovered "
          f"{len(uncovered_body)}, non-pad gaps {nonpad}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
