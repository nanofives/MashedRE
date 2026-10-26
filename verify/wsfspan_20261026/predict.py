"""Offline prediction for D4-WSF-SAVE-SPANINIT: emulate the new first-run rebuild
(InitSpanDefaults + BuildSpanFromProgress overlay + BuildImage) and diff it against the
original's first-run file O_0. Usage: py -3.12 predict.py <O_0.bin> [<before.bin>]"""
import struct, sys

SIZE, SPAN, LEN, CNT = 0x24FA0, 0x24A40, 0x520, 0x24EFC


def init_span():
    s = bytearray(LEN)
    put = lambda o, v: struct.pack_into('<I', s, o, v)
    put(0x4C4, 0x3F333333); put(0x4C8, 0x3F333333); put(0x4C0, 0x3F333333); put(0x4D0, 2)
    for i in range(8):
        s[0x514 + i] = 1
    for r in range(13):
        for k in range(5):
            put(0x270 + r * 0x14 + 4 * k, 0xFFFFFFFF)
        put(0x374 + r * 4, 9); put(0x3A8 + r * 4, 0x3B); put(0x3DC + r * 4, 0x3F7D70A4)
        for c in range(12):
            s[0x410 + r * 12 + c] = 1
        row = r * 0x30
        put(row + 0x04, 1 if r == 0 else 0); put(row + 0x0C, 2 if r < 4 else 0)
        put(row + 0x10, 2); put(row + 0x2C, 1 if r == 0 else 0)
    return s


def first_run_image():
    s = init_span()
    unlock = [1] + [0] * 12          # Campaign_LoadProgress: g_progUnlock[0] = 1, no file
    for r in range(13):
        struct.pack_into('<I', s, r * 0x30 + 0x10, 2 if unlock[r] else 0)
        struct.pack_into('<I', s, r * 0x30 + 0x14, 0)
    struct.pack_into('<I', s, 0x4EC, 1 if any(unlock) else 0)
    img = bytearray(SIZE)
    img[SPAN:SPAN + LEN] = s
    struct.pack_into('<I', img, CNT, 0)
    struct.pack_into('<I', img, 0, 0xDEADBEEF)
    return bytes(img)


def regions(d):
    out = {}
    for i in d:
        k = ('magic' if i < 4 else 'block' if i < SPAN else 'table' if i < 0x24D10 else
             'state' if CNT <= i < CNT + 4 else 'gather' if 0x24F54 <= i < 0x24F60 else
             'span_rest' if i < 0x24F60 else 'tail')
        out[k] = out.get(k, 0) + 1
    return out


def main():
    o = open(sys.argv[1], 'rb').read()
    p = first_run_image()
    d = [i for i in range(SIZE) if o[i] != p[i]]
    print(f'predicted new first-run vs O_0: {len(d)} bytes differ {regions(d)}')
    with open(sys.argv[3] if len(sys.argv) > 3 else 'predicted_first_run.bin', 'wb') as f:
        f.write(p)
    if len(sys.argv) > 2:
        b = open(sys.argv[2], 'rb').read()
        d = [i for i in range(SIZE) if o[i] != b[i]]
        print(f'before ({sys.argv[2]}) vs O_0: {len(d)} bytes differ {regions(d)}')


if __name__ == '__main__':
    main()
