# Write an edited copy of the reference gamesave.bin: only cells of the 13x12 int32
# table at file offset 0x24A40 (= live 0x007f0a40) change. Usage: src dst row:col=val ...
import struct, sys
src, dst, *cells = sys.argv[1:]
d = bytearray(open(src, 'rb').read())
assert len(d) == 0x24FA0 and struct.unpack_from('<I', d, 0)[0] == 0xDEADBEEF
for c in cells:
    rc, v = c.split('='); r, k = map(int, rc.split(':'))
    struct.pack_into('<i', d, 0x24A40 + r * 0x30 + k * 4, int(v))
open(dst, 'wb').write(d)
for r in range(13):
    print(r, struct.unpack_from('<12i', d, 0x24A40 + r * 0x30))
