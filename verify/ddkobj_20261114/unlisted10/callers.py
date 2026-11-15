"""Static caller scan on original/MASHED.exe.unpatched (DDKOBJ-UNLISTED-10).

usage: py -3.12 callers.py <exe> <target_hex> [<after_site_hex> ...]
Lists every E8/E9 rel32 to target in executable sections and every absolute dword equal to target,
then disassembles 12 instructions after each given call site (capstone).
"""
import struct
import sys

import capstone


def sections(data):
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    nsec = struct.unpack_from('<H', data, pe + 6)[0]
    optsz = struct.unpack_from('<H', data, pe + 20)[0]
    base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
    out = []
    off = pe + 24 + optsz
    for i in range(nsec):
        name = data[off:off + 8].rstrip(b'\0').decode()
        vsz, va, rsz, raw = struct.unpack_from('<IIII', data, off + 8)
        ch = struct.unpack_from('<I', data, off + 36)[0]
        out.append((name, base + va, vsz, raw, rsz, ch))
        off += 40
    return base, out


def va2off(secs, va):
    for name, sva, vsz, raw, rsz, ch in secs:
        if sva <= va < sva + max(vsz, rsz):
            return raw + (va - sva)
    return None


def main():
    data = open(sys.argv[1], 'rb').read()
    target = int(sys.argv[2], 16)
    base, secs = sections(data)
    print(f'target 0x{target:08x}')
    for name, sva, vsz, raw, rsz, ch in secs:
        blob = data[raw:raw + rsz]
        if ch & 0x20000000:
            for i in range(len(blob) - 5):
                if blob[i] in (0xE8, 0xE9):
                    rel = struct.unpack_from('<i', blob, i + 1)[0]
                    if (sva + i + 5 + rel) & 0xffffffff == target:
                        print(f'  rel32 {"call" if blob[i] == 0xE8 else "jmp"} at 0x{sva + i:08x} ({name})')
        pat = struct.pack('<I', target)
        j = blob.find(pat)
        while j >= 0:
            print(f'  abs dword at 0x{sva + j:08x} ({name})')
            j = blob.find(pat, j + 1)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for s in sys.argv[3:]:
        site = int(s, 16)
        off = va2off(secs, site)
        print(f'after 0x{site:08x}:')
        n = 0
        for ins in md.disasm(data[off:off + 120], site):
            print(f'    0x{ins.address:08x}  {ins.mnemonic} {ins.op_str}')
            n += 1
            if n >= 14:
                break


main()
