"""D4-AUDIO-VOLTODS: dump the three original pages the sweep maps at their own addresses.
usage: py -3.12 extract_pages.py <MASHED.exe.unpatched> <out.bin>
Writes 3 x (u32 va, 0x1000 bytes) for 0x005ae000 (.text, holds 0x005aeb90), 0x005cc000 and 0x005cd000 (.rdata, hold
0x005cc990 and 0x005cd0b8). Also prints the function bytes and both constants (consts.txt)."""
import struct, sys

d = open(sys.argv[1], 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]
n = struct.unpack_from('<H', d, pe + 6)[0]
opt = struct.unpack_from('<H', d, pe + 20)[0]
base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
secs = []
for i in range(n):
    s = pe + 24 + opt + 40 * i
    vs, va, rs, rp = struct.unpack_from('<IIII', d, s + 8)
    secs.append((d[s:s + 8].rstrip(b'\0').decode(), va, vs, rs, rp, struct.unpack_from('<I', d, s + 36)[0]))


def read(va, size):
    out = bytearray(size)
    for i in range(size):
        rva = va + i - base
        for name, sva, vs, rs, rp, ch in secs:
            if sva <= rva < sva + max(vs, rs):
                if rva - sva < rs:
                    out[i] = d[rp + rva - sva]
                break
    return bytes(out)


def sec_of(va):
    rva = va - base
    for name, sva, vs, rs, rp, ch in secs:
        if sva <= rva < sva + max(vs, rs):
            return f'{name} characteristics={ch:#010x}'
    return '?'


with open(sys.argv[2], 'wb') as f:
    for page in (0x005ae000, 0x005cc000, 0x005cd000):
        f.write(struct.pack('<I', page) + read(page, 0x1000))
print('fn 0x005aeb90 97 bytes:', read(0x005aeb90, 97).hex(' '), '|', sec_of(0x005aeb90))
for va in (0x005cc990, 0x005cd0b8):
    b = read(va, 4)
    print(f'{va:#010x} = {b[::-1].hex()} = {struct.unpack("<f", b)[0]!r} | {sec_of(va)}')
