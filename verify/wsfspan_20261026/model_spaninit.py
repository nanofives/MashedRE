"""Model FUN_004924f0's writes into the 0x007f0a40 championship span (file 0x24a40..0x24f5f)
and compare against a gamesave file. Usage: py -3.12 model_spaninit.py <file.bin> [<file2.bin>]

Every write cited from the decomp of FUN_004924f0 (re/tools/decomp_pc.py, 2026-10-26).
The span is memory 0x007f0a40..0x007f0f5f; file offset = 0x24a40 + (va - 0x007f0a40).
The gather bytes (file 0x24f54..0x24f5f) are the low byte of [0x007f105c + i*0x4c] (0x00404ef0).
"""
import struct, sys

SPAN_VA, SPAN_OFF, SPAN_LEN = 0x007F0A40, 0x24A40, 0x520


def model():
    bytes_ = {}  # va -> byte, only the VAs FUN_004924f0 writes (BSS elsewhere stays 0)

    def w(va, v):  # dword store
        for k in range(4):
            bytes_[va + k] = (v >> (8 * k)) & 0xFF
    # zero-fill 0xdce9 dwords at 0x007f0f60 (outside the span)
    for va in (0x7F0F2C, 0x7F0F30, 0x7F0F34, 0x7F0F38, 0x7F0F3C, 0x7F0FEC, 0x7F0FD0):
        w(va, 0)
    w(0x7F0F04, 0x3F333333)  # FUN_00431ae0
    w(0x7F0F08, 0x3F333333)  # FUN_00431af0
    w(0x7F0F00, 0x3F333333)  # FUN_00431b00
    w(0x7F0F10, 2)           # FUN_00431b10 (0x00431b12)
    va = 0x7F105C
    while va < 0x7F12BC:     # *puVar1 = 1, stride 0x13 dwords
        w(va, 1)
        va += 0x4C
    for r in range(13):      # puVar1 0x7f0cb0 + r*0x14 while < 0x7f0db4
        p1 = 0x7F0CB0 + r * 0x14
        for k in range(5):
            w(p1 + 4 * k, 0xFFFFFFFF)
        p5 = 0x7F0DE8 + r * 4
        w(p5 - 0x34, 9)
        w(p5, 0x3B)
        w(p5 + 0x34, 0x3F7D70A4)
        base = 0x7F0A40 + r * 0x30
        for c in range(12):
            bytes_[0x7F0E50 + c + r * 12] = 1   # BYTE store, mov byte ptr [eax+esi+0x7f0e50],1 0x004925a3
            if c == 1 or c == 11:
                w(base + 4 * c, 1 if p1 < 0x7F0CC4 else 0)
            elif c == 3:
                w(base + 4 * c, (((1 if p1 > 0x7F0CFF else 0) - 1) & 2))
            elif c == 4:
                w(base + 4 * c, 2)
            else:
                w(base + 4 * c, 0)
    return bytes_


def span_bytes(mem):
    b = bytearray(SPAN_LEN)
    for va, v in mem.items():
        if SPAN_VA <= va < SPAN_VA + SPAN_LEN:
            b[va - SPAN_VA] = v
    # gather 0x24f54..0x24f5f (span +0x514): byte i = low byte of [0x007f105c + i*0x4c]
    for i in range(12):
        b[0x514 + i] = mem.get(0x7F105C + i * 0x4C, 0)
    return bytes(b)


def main():
    m = span_bytes(model())
    for path in sys.argv[1:]:
        data = open(path, 'rb').read()
        f = data[SPAN_OFF:SPAN_OFF + SPAN_LEN]
        diffs = [i for i in range(SPAN_LEN) if f[i] != m[i]]
        print(f'{path}: span bytes differing from the FUN_004924f0 model: {len(diffs)}')
        for i in diffs:
            print(f'  file 0x{SPAN_OFF + i:05x} (va 0x{SPAN_VA + i:08x}) file {f[i]:3d} model {m[i]:3d}')


if __name__ == '__main__':
    main()
