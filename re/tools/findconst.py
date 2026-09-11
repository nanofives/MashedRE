#!/usr/bin/env python3
"""Find 32-bit immediates in the anchored MASHED.exe.unpatched, with instruction context.

The `search_constants` step several uncertainty rows ask for. Scans .text for the
little-endian 4-byte encoding of each value, then disassembles backwards from the hit so
the containing instruction is shown rather than a bare offset - a raw byte match is not
evidence that the value is an immediate operand.

  py -3.12 re/tools/findconst.py 0x30406
  py -3.12 re/tools/findconst.py --mask 0xffff0000 0x30000   # group by upper bits

Only .text is scanned: a match in .rdata/.data is a stored value, not an immediate, and
reporting those together is how a "constant is used at N sites" claim goes wrong.
"""
import struct
import sys
from pathlib import Path

import capstone

ROOT = Path(__file__).resolve().parents[2]      # re/tools -> re -> repo root
EXE = ROOT / "original" / "MASHED.exe.unpatched"


def sections(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    out = []
    for i in range(nsec):
        s = pe + 24 + opt + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode(errors="replace")
        vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
        out.append((name, vaddr, vsz, roff, rsz))
    return base, out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if not args:
        raise SystemExit(__doc__)
    values = [int(a, 16) for a in args]

    data = EXE.read_bytes()
    base, secs = sections(data)
    text = next((s for s in secs if s[0] == ".text"), None)
    if text is None:
        raise SystemExit("no .text section")
    _, vaddr, vsz, roff, rsz = text
    span = data[roff:roff + rsz]

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    for v in values:
        needle = struct.pack("<I", v)
        hits = []
        start = 0
        while True:
            i = span.find(needle, start)
            if i < 0:
                break
            hits.append(i)
            start = i + 1

        print(f"=== 0x{v:08x} : {len(hits)} raw match(es) in .text ===")
        for i in hits:
            va = base + vaddr + i
            # Disassemble from a little before the hit so the instruction that *contains*
            # the bytes is what gets printed. Try a few starts; keep the one whose
            # instruction actually spans the match.
            shown = None
            for back in range(1, 12):
                off = i - back
                if off < 0:
                    continue
                for ins in md.disasm(span[off:off + 24], base + vaddr + off):
                    if ins.address <= va < ins.address + ins.size:
                        shown = (ins.address, ins.mnemonic, ins.op_str, ins.size)
                    break
                if shown:
                    break
            if shown:
                a, mn, ops, sz = shown
                print(f"  0x{va:08x}  in  0x{a:08x}: {mn} {ops}")
            else:
                print(f"  0x{va:08x}  (no instruction spans it - stored data, not an operand)")
        print()


if __name__ == "__main__":
    main()
