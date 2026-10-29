"""D3-SUBSTATE-ROW315 P4: first differing record between two port MSD1 dumps (MASHED_RECDUMP, slot 0).

Each dump: 'MSD1' + u32 rec_size + u32 0 + u32 0, then per race frame u32 frame_idx + rec_size bytes.
Alignment: the first record (in each file) whose velocity +0x9b0..+0x9b8 is nonzero is index 0 of that file.
Prints the aligned index of the first record that differs, the first 24 differing dword offsets on it, and their
float / int values on both sides. Usage: py -3.12 recdiff.py A.rec.msd B.rec.msd
"""
import struct
import sys


def load(p):
    b = open(p, 'rb').read()
    assert b[:4] == b'MSD1', p
    rs = struct.unpack_from('<I', b, 4)[0]
    out, o = [], 16
    while o + 4 + rs <= len(b):
        fi = struct.unpack_from('<I', b, o)[0]
        out.append((fi, b[o + 4:o + 4 + rs]))
        o += 4 + rs
    return rs, out


def first_moving(recs):
    for i, (_, r) in enumerate(recs):
        if any(struct.unpack_from('<3f', r, 0x9b0)):
            return i
    return None


def main(a, b):
    rsa, ra = load(a)
    rsb, rb = load(b)
    assert rsa == rsb
    ia, ib = first_moving(ra), first_moving(rb)
    print(f'A {a}: {len(ra)} records, first moving at record {ia} (frame_idx {ra[ia][0]})')
    print(f'B {b}: {len(rb)} records, first moving at record {ib} (frame_idx {rb[ib][0]})')
    pre_a, pre_b = ra[ia - 1][1] if ia else None, rb[ib - 1][1] if ib else None
    if pre_a is not None and pre_b is not None:
        print('record before first moving: ' + ('identical' if pre_a == pre_b else 'DIFFERS'))
    n = min(len(ra) - ia, len(rb) - ib)
    for k in range(n):
        x, y = ra[ia + k][1], rb[ib + k][1]
        if x != y:
            offs = [o for o in range(0, rsa, 4) if x[o:o + 4] != y[o:o + 4]]
            print(f'first differing aligned record k={k} (A frame_idx {ra[ia + k][0]}, B frame_idx {rb[ib + k][0]}),'
                  f' {len(offs)} dwords differ')
            for o in offs[:24]:
                fa, fb = struct.unpack_from('<f', x, o)[0], struct.unpack_from('<f', y, o)[0]
                ia_, ib_ = struct.unpack_from('<i', x, o)[0], struct.unpack_from('<i', y, o)[0]
                print(f'  +0x{o:03x}  A {fa:.9g} ({ia_:#010x})  B {fb:.9g} ({ib_:#010x})  dA-B {fa - fb:.3g}')
            return
    print(f'no difference on {n} aligned records')


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
