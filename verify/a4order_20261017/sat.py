"""Offline evaluation of VehicleCarCarContact's phase-1 SAT (FUN_00469df0, port Collision/CarCarContacts.cpp:57-97) on
recorded inputs. A car = four hull vertices (+0xa28 / +0xa34 / +0xa40 / +0xa4c, dwords 0x28a..0x295) and its up row
(+0x9c8..+0x9d0, dwords 0x272..0x274). Prints, per direction, which vertices of the other car fall inside each half-plane, the
four signed tests, and the total count (0 = no contact). float32 arithmetic via numpy.
usage: py -3.12 sat.py  (inputs are set in main from the msd files of this lane)"""
import struct, sys
import numpy as np

f32 = np.float32


def rec(path, frame):
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1'
    n, _b, _ = struct.unpack_from('<III', b, 4)
    off = 16
    while off + 4 + n <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        if fi == frame:
            return b[off + 4: off + 4 + n]
        off += 4 + n
    raise KeyError(frame)


def car(p):
    hull = [tuple(f32(x) for x in struct.unpack_from('<3f', p, o)) for o in (0xa28, 0xa34, 0xa40, 0xa4c)]
    up = tuple(f32(x) for x in struct.unpack_from('<3f', p, 0x9c8))
    return hull, up


def tests(src, other):
    (d8, e0, cc), (f0, ec, e4), (e8, f4, f6), (dc, d0, d4) = src[0]
    f3, f4_, f5 = src[1]
    n0 = ((e4 - cc) * f4_ - (ec - e0) * f5, (f0 - d8) * f5 - (e4 - cc) * f3, (ec - e0) * f3 - (f0 - d8) * f4_)
    n1 = ((d4 - e4) * f4_ - (d0 - ec) * f5, (dc - f0) * f5 - (d4 - e4) * f3, (d0 - ec) * f3 - (dc - f0) * f4_)
    n2 = ((f6 - d4) * f4_ - (f4 - d0) * f5, (e8 - dc) * f5 - (f6 - d4) * f3, (f4 - d0) * f3 - (e8 - dc) * f4_)
    l148, l140 = e0 - f4, cc - f6
    n3 = (l140 * f4_ - l148 * f5, (d8 - e8) * f5 - l140 * f3, l148 * f3 - (d8 - e8) * f4_)
    planes = [((d8, e0, cc), n0), ((f0, ec, e4), n1), ((dc, d0, d4), n2), ((e8, f4, f6), n3)]
    out = []
    for v in other[0]:
        s = [f32((v[0] - a[0]) * n[0] + (v[1] - a[1]) * n[1] + n[2] * (v[2] - a[2])) for a, n in planes]
        out.append((all(x < 0 for x in s), s))
    return out


def report(tag, A, B):
    # VehicleCarCarContact(vehA = OTHER car j, vehB = SELF car i): uVar13 0 tests B's vertices in A's quad, 1 the reverse.
    tot = 0
    for name, s, o in (('B in A', A, B), ('A in B', B, A)):
        r = tests(s, o)
        tot += sum(x[0] for x in r)
        print('  %s %s: ' % (tag, name) + ' | '.join('%s %s' % ('IN ' if x[0] else 'out', ','.join('%+.4f' % y for y in x[1])) for x in r))
    print('  %s count = %d -> %s' % (tag, tot, 'CONTACT' if tot else 'no contact'))


if __name__ == '__main__':
    W = sys.argv[1] if len(sys.argv) > 1 else '.'
    player = car(rec(W + r'\o_pw1.msd', int(sys.argv[2]) if len(sys.argv) > 2 else 800))
    print('player hull', player[0], 'up', player[1])
    for fr in [int(x) for x in (sys.argv[3].split(',') if len(sys.argv) > 3 else ['835', '836', '837'])]:
        c2 = car(rec(W + r'\o_c2r1.msd', fr))
        print('car2 frame %d hull %s up %s' % (fr, c2[0], c2[1]))
        report('orig f%d' % fr, c2, player)
