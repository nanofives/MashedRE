#!/usr/bin/env python3
"""Find instructions that touch a struct field displacement, e.g. `[edi + 0x188]`.

The `search_pcode for writes to X+0xNN` step several uncertainty rows ask for.
findconst.py cannot answer those: a field offset is encoded as a ModRM displacement,
not as a 4-byte immediate, and `grep 0x188` over decompiler text also misses the
dword-index form (`arr[0x62]` off a base). This sweeps .text instruction by
instruction and matches on the decoded operand instead.

  py -3.12 re/tools/findoffset.py 0x188 0x18c 0x190
  py -3.12 re/tools/findoffset.py --writes 0x23      # only stores to the field
  py -3.12 re/tools/findoffset.py --base edi 0x188   # pin the base register

CAVEATS, both of which matter when citing a hit:

1. This is a LINEAR sweep, not a per-function walk. x86 self-syncs quickly, but a
   hit landing inside a jump table or a mid-instruction offset is possible. Confirm
   any cited hit with disasm_fn.py over the containing function.
2. A displacement is NOT unique to one struct. `[eax + 8]` occurs in thousands of
   unrelated places. Small offsets are only usable with --base, and even then the
   base register tells you nothing about the type. Large/odd offsets (0x188) are
   the ones where a bare displacement search is close to decisive.
"""
import argparse
import struct
from pathlib import Path

import capstone

ROOT = Path(__file__).resolve().parents[2]      # re/tools -> re -> repo root
EXE = ROOT / "original" / "MASHED.exe.unpatched"


def parse_pe(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        s = pe + 24 + opt + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode(errors="replace")
        vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
        secs.append((name, vaddr, vsz, roff, rsz))
    return base, secs


def main():
    ap = argparse.ArgumentParser(add_help=False)
    ap.add_argument("offsets", nargs="+")
    ap.add_argument("--base", default=None, help="only hits with this base register")
    ap.add_argument("--writes", action="store_true", help="only stores INTO the field")
    ap.add_argument("--lea", action="store_true", help="include lea (address-taken)")
    ap.add_argument("-h", "--help", action="store_true")
    a = ap.parse_args()
    if a.help:
        raise SystemExit(__doc__)

    wanted = {int(x, 16) for x in a.offsets}
    data = EXE.read_bytes()
    base, secs = parse_pe(data)
    text = next(s for s in secs if s[0] == ".text")
    _, vaddr, _, roff, rsz = text
    span = data[roff:roff + rsz]

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True

    # capstone's disasm() generator STOPS at the first byte it cannot decode, and
    # .text here contains plenty (jump tables, alignment padding, data islands).
    # Without this resync the sweep silently covered only .text up to ~0x408890 and
    # reported 0 hits for offsets that do exist. Always resync; never trust a bare
    # generator sweep to have reached the end of the section.
    def sweep():
        off = 0
        n = len(span)
        while off < n:
            last = off
            for ins in md.disasm(span[off:], base + vaddr + off):
                yield ins
                last = off + (ins.address - (base + vaddr + off)) + ins.size
            off = max(last, off + 1)

    found = {o: [] for o in wanted}
    for ins in sweep():
        # lea computes an address without touching memory; it is the address-taken
        # signal, so it is opt-in rather than lumped in with real accesses.
        if ins.mnemonic == "lea" and not a.lea:
            continue
        for op in ins.operands:
            if op.type != capstone.x86.X86_OP_MEM:
                continue
            m = op.mem
            if m.disp not in wanted or m.base == 0:
                continue
            breg = ins.reg_name(m.base)
            if a.base and breg != a.base:
                continue
            # Use capstone's own access flags, not operand position: `fstp
            # [esi+0xe4]` is a single-operand STORE and a positional heuristic
            # calls it a read, which would invert a "who writes this field" answer.
            is_write = bool(op.access & capstone.CS_AC_WRITE)
            if a.writes and not is_write:
                continue
            found[m.disp].append(
                (ins.address, breg, "W" if is_write else "r",
                 f"{ins.mnemonic} {ins.op_str}"))

    for o in sorted(wanted):
        hits = found[o]
        print(f"=== +0x{o:x} : {len(hits)} access(es) in .text"
              + (f" with base {a.base}" if a.base else "") + " ===")
        for addr, breg, kind, txt in hits:
            print(f"  {kind}  0x{addr:08x}  [{breg}]  {txt}")
        print()


if __name__ == "__main__":
    main()
