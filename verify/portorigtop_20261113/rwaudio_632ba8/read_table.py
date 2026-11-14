"""PORT-TOP-RWAUDIO-TABLE-632BA8: read the 16 {init, shutdown} pairs at 0x00632ba8 from MASHED.exe.unpatched and from
mashed_image.bin (the IMG-2 image), and print both side by side.
usage: py -3.12 verify/portorigtop_20261113/rwaudio_632ba8/read_table.py <MASHED.exe.unpatched> <mashed_image.bin>"""
import struct, sys
import pefile

TABLE, N = 0x00632ba8, 32


def from_pe(path):
    pe = pefile.PE(path, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    return [pe.get_dword_at_rva(TABLE - base + 4 * i) for i in range(N)]


def from_image(path):
    # mashed_image.bin v1, validated and split by re/tools/image/mashed_image.py parse() (the IMG-1 reader).
    sys.path.insert(0, 're/tools/image')
    import mashed_image
    _, ranges = mashed_image.parse(open(path, 'rb').read())
    for _name, va, data, _fnv in ranges:
        if va <= TABLE and TABLE + 4 * N <= va + len(data):
            o = TABLE - va
            return list(struct.unpack_from('<%dI' % N, data, o))
    return None


def main():
    pe = from_pe(sys.argv[1])
    img = from_image(sys.argv[2]) if len(sys.argv) > 2 else None
    print('slot        file        image       pair')
    for i in range(N):
        print('0x%08x  0x%08x  %s  %s' % (TABLE + 4 * i, pe[i], ('0x%08x' % img[i]) if img else '-', 'init' if i % 2 == 0 else 'shutdown'))
    if img is not None:
        print('image equals file at all %d slots: %s' % (N, img == pe))


if __name__ == '__main__':
    main()
