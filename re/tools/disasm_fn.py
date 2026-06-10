# Disassemble a VA range of the anchored MASHED.exe.unpatched with capstone.
# Output is deterministic byte-level evidence for verbatim ports (NO-GUESSING:
# every cited instruction comes from the anchored binary at its VA).
#
# Usage: py -3.12 re/tools/disasm_fn.py 0x00446520 0x00448213 [outfile]
import struct, sys
from pathlib import Path
import capstone

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "original" / "MASHED.exe.unpatched"


def parse_pe(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    sec0 = pe + 24 + opt_size
    secs = []
    for i in range(nsec):
        s = sec0 + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode()
        vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
        secs.append((name, vaddr, vsz, roff, rsz))
    return base, secs


def main():
    start = int(sys.argv[1], 16)
    end = int(sys.argv[2], 16)
    out = Path(sys.argv[3]) if len(sys.argv) > 3 else None
    data = EXE.read_bytes()
    base, secs = parse_pe(data)
    off = None
    for name, vaddr, vsz, roff, rsz in secs:
        rva = start - base
        if vaddr <= rva < vaddr + max(vsz, rsz):
            off = roff + (rva - vaddr)
            break
    if off is None:
        print("start VA not in image"); return 1
    code = data[off:off + (end - start)]
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    lines = []
    for ins in md.disasm(code, start):
        lines.append(f"0x{ins.address:08x}: {ins.bytes.hex():24s} {ins.mnemonic} {ins.op_str}")
    text = "\n".join(lines) + "\n"
    if out:
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(text)
        print(f"{len(lines)} instructions -> {out}")
    else:
        print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
