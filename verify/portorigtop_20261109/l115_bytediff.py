"""L115-HUDINIT-PROOFS: list every differing unmasked byte of a matched source against the original, with the instruction
that holds it on each side. Uses re/tools/match/match.py's own compile and byte_compare (relocation sites masked).
Read-only.

usage: py -3.12 verify/portorigtop_20261109/l115_bytediff.py <source.c>
"""
import pathlib
import sys

import capstone

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match as M  # noqa: E402


def insns(code, base):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [(i.address - base, i.size, f"{i.mnemonic} {i.op_str}".strip(), i.bytes.hex()) for i in md.disasm(code, base)]


def at(lst, off):
    for o, n, txt, hx in lst:
        if o <= off < o + n:
            return o, n, txt, hx
    return None


def main():
    src = pathlib.Path(sys.argv[1])
    text = src.read_text()
    (addr, sym), = M.parse_annotations(text)
    fl = M.file_flags(text) or M.DEFAULT_FLAGS
    secs, syms = M.coff(M.compile_obj(src, fl, M.file_lang(text), M.file_toolchain(text)))
    n, bad = M.byte_compare(addr, secs, syms, sym)
    mine, relocs, full, local = M.function_from_obj(secs, syms, sym)
    mine = mine[:n]
    orig = M.original_bytes(addr, n)
    a, b = insns(bytes(mine), 0), insns(orig, addr)
    b0 = [(o - 0, s, t, h) for o, s, t, h in b]
    print(f"{src}  0x{addr:08x}  {n} B, {len(bad)} differing unmasked bytes, relocs masked at "
          + ",".join(f"+0x{r:x}" for r in sorted(relocs)))
    print("off\taddr\tours\torig\tours_insn(start)\torig_insn(start)")
    for off in bad:
        ia, ib = at(a, off), at(b0, off)
        fa = f"+0x{ia[0]:x} {ia[3]} {ia[2]}" if ia else "-"
        fb = f"0x{addr + ib[0]:08x} {ib[3]} {ib[2]}" if ib else "-"
        print(f"+0x{off:x}\t0x{addr + off:08x}\t{mine[off]:02x}\t{orig[off]:02x}\t{fa}\t{fb}")


if __name__ == "__main__":
    main()
