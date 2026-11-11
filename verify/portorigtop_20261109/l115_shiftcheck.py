"""L115-HUDINIT-PROOFS: label every differing unmasked byte of 0x005c4ad0 and 0x00555360 (re/match sources, recorded DDK)
against the original, and check the shift / permutation claims byte for byte. Read-only.

usage: py -3.12 verify/portorigtop_20261109/l115_shiftcheck.py
Prints one TSV table per function (off, addr, ours, orig, label, reason) and a summary line per function.
"""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match as M  # noqa: E402


def load(src):
    src = ROOT / src
    text = src.read_text()
    (addr, sym), = M.parse_annotations(text)
    fl = M.file_flags(text) or M.DEFAULT_FLAGS
    secs, syms = M.coff(M.compile_obj(src, fl, M.file_lang(text), M.file_toolchain(text)))
    n, bad = M.byte_compare(addr, secs, syms, sym)
    mine, relocs, _full, _local = M.function_from_obj(secs, syms, sym)
    mask = {r + k for r in relocs for k in range(4)}
    return addr, bytes(mine[:n]), M.original_bytes(addr, n), bad, mask


def eq_shift(mine, orig, mask, lo, hi, shift):
    """ours[k] == orig[k + shift] for every unmasked k in [lo, hi)."""
    return [k for k in range(lo, hi) if k not in mask and mine[k] != orig[k + shift]]


def table(addr, mine, orig, bad, label):
    rows, other = [], []
    for off in bad:
        lab, why = label(off)
        rows.append(f"+0x{off:x}\t0x{addr + off:08x}\t{mine[off]:02x}\t{orig[off]:02x}\t{lab}\t{why}")
        if lab == "other":
            other.append(addr + off)
    return rows, other


def f5c4ad0():
    addr, mine, orig, bad, mask = load("re/match/s28/005c4ad0.c")
    # window +0x87..+0x95 (15 B): ours  c7 44 24 04 01 00 00 00 | 8b 56 0c | 0f af 56 08
    #                              orig  8b 56 08 | c7 44 24 04 01 00 00 00 | 0f af 56 0c
    W0, W1 = 0x87, 0x96
    st = bytes.fromhex("c744240401000000")
    ours_ok = mine[W0:W1] == st + bytes.fromhex("8b560c") + bytes.fromhex("0faf5608")
    orig_ok = orig[W0:W1] == bytes.fromhex("8b5608") + st + bytes.fromhex("0faf560c")
    outside = [k for k in bad if not (W0 <= k < W1)]

    def label(off):
        if not (W0 <= off < W1):
            return "other", "outside the multiply window"
        if off == 0x95:
            return ("explained by class 1",
                    "imul m32 displacement: ours [esi+8], orig [esi+0xc] (the two multiplicands swapped)")
        return ("other",
                "the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store "
                "[esp+4] = 1 in ours and before it in the original: two independent instructions in the other order "
                "(not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8")
    rows, other = table(addr, mine, orig, bad, label)
    return addr, rows, other, {"window bytes as stated (ours)": ours_ok, "window bytes as stated (orig)": orig_ok,
                               "differing bytes outside the window": len(outside)}


def f555360():
    addr, mine, orig, bad, mask = load("re/match/s24/00555360.c")
    checks = {}
    # (1) first failed check: ours +0x2d `74 4b` je -> +0x7a (2 B), orig +0x2d `75 01` jne +0x30 / +0x2f `c3` ret (3 B)
    # (2) second: ours +0x59 `74 1f` je -> +0x7a, orig +0x5a `75 01` / +0x5c `c3`
    # (3) third: ours +0x78 `75 03` jne +0x7d / +0x7a `33 c0` xor eax,eax / +0x7c `c3`; orig +0x7a `75 01` / +0x7c `c3`
    checks["ours +0x2d = 74 4b (je +0x7a)"] = mine[0x2d:0x2f] == bytes.fromhex("744b") and 0x2f + 0x4b == 0x7a
    checks["orig +0x2d = 75 01 c3"] = orig[0x2d:0x30] == bytes.fromhex("7501c3")
    checks["ours +0x59 = 74 1f (je +0x7a)"] = mine[0x59:0x5b] == bytes.fromhex("741f") and 0x5b + 0x1f == 0x7a
    checks["orig +0x5a = 75 01 c3"] = orig[0x5a:0x5d] == bytes.fromhex("7501c3")
    checks["ours +0x78 = 75 03 33 c0 c3"] = mine[0x78:0x7d] == bytes.fromhex("750333c0c3")
    checks["orig +0x7a = 75 01 c3"] = orig[0x7a:0x7d] == bytes.fromhex("7501c3")
    # shifted runs: ours [0x2f, 0x59) == orig [0x30, 0x5a) ; ours [0x5b, 0x78) == orig [0x5d, 0x7a)
    s1 = eq_shift(mine, orig, mask, 0x2f, 0x59, 1)
    s2 = eq_shift(mine, orig, mask, 0x5b, 0x78, 2)
    checks["ours [+0x2f,+0x59) == orig shifted +1 (unmasked)"] = not s1
    checks["ours [+0x5b,+0x78) == orig shifted +2 (unmasked)"] = not s2
    # relocated operands in the shifted runs: our reloc site k maps to orig k+shift; the original's absolute there
    # names the same global as our symbol (checked by listing them)
    # (4) loop store pair: ours +0x107 `66 89 08` / +0x10a `66 89 70 fe`; orig +0x107 `66 89 70 fe` / +0x10b `66 89 08`
    checks["ours +0x107 = 668908 668970fe"] = mine[0x107:0x10e] == bytes.fromhex("668908668970fe")
    checks["orig +0x107 = 668970fe 668908"] = orig[0x107:0x10e] == bytes.fromhex("668970fe668908")

    def label(off):
        if off in (0x2d, 0x2e):
            return "explained by class 2", "first failed check: ours je +0x7a (shared xor/ret), orig jne +1 over its own ret"
        if 0x2f <= off < 0x59:
            return "explained by class 2", "shift +1 after the first check: ours[off] == orig[off+1]"
        if off in (0x59, 0x5a):
            return "explained by class 2", "second failed check: ours je +0x7a, orig jne +1 over its own ret"
        if 0x5b <= off < 0x78:
            return "explained by class 2", "shift +2 after the second check: ours[off] == orig[off+2]"
        if 0x78 <= off < 0x7d:
            return ("explained by class 2",
                    "third check: ours jne +3 over the shared xor eax,eax / ret, orig test/jne/ret shifted +2; both "
                    "realign at +0x7d")
        if 0x107 <= off < 0x10e:
            return "explained by class 3", "the two 16-bit stores [eax] / [eax-2] in the other order (same 7 bytes)"
        return "other", "outside the named regions"
    rows, other = table(addr, mine, orig, bad, label)
    # relocated operands inside the shifted runs, both sides, for the table
    rel = []
    for lo, hi, sh in ((0x2f, 0x59, 1), (0x5b, 0x78, 2)):
        for k in sorted(mask):
            if lo <= k < hi and (k - 1) not in mask:
                rel.append(f"ours +0x{k:x} -> orig 0x{addr + k + sh:08x} = 0x{int.from_bytes(orig[k + sh:k + sh + 4], 'little'):08x}")
    return addr, rows, other, checks, rel


def main():
    a, rows, other, checks = f5c4ad0()
    print(f"== 0x{a:08x} re/match/s28/005c4ad0.c")
    print("off\taddr\tours\torig\tlabel\treason")
    print("\n".join(rows))
    for k, v in checks.items():
        print(f"check: {k}: {v}")
    print("SUMMARY 0x005c4ad0:", "all explained" if not other else "other found at " + ", ".join(f"0x{x:08x}" for x in other))
    a, rows, other, checks, rel = f555360()
    print(f"\n== 0x{a:08x} re/match/s24/00555360.c")
    print("off\taddr\tours\torig\tlabel\treason")
    print("\n".join(rows))
    for k, v in checks.items():
        print(f"check: {k}: {v}")
    for r in rel:
        print("reloc in shifted run:", r)
    print("SUMMARY 0x00555360:", "all explained" if not other else "other found at " + ", ".join(f"0x{x:08x}" for x in other))


if __name__ == "__main__":
    main()
