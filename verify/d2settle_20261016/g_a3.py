"""G-A3 (PREREG_d2settle.md section 2): the exe's MASHED_A3_LOG line for slot 0 against the ORIGINAL's
o_p0a.msd frame-1 record, 26 fields, bit-exact. usage: py -3.12 g_a3.py <a3.txt> [<msd>]"""
import struct, sys
a3 = sys.argv[1]
msd = sys.argv[2] if len(sys.argv) > 2 else r'C:\Users\maria\Desktop\Proyectos\Mashed\verify\d3_car2pickup_20261013\o_p0a.msd'
b = open(msd, 'rb').read()
rec, base, _ = struct.unpack_from('<III', b, 4)
o = 16 + (4 + rec) * 1                     # frame 1 (frame 0 is the pre-spawn record)
R = b[o + 4:o + 4 + rec]
line = next(l for l in open(a3) if l.startswith('slot=0 '))
kv = dict(t.split('=') for t in line.split()[1:])
ok = bad = 0
for k, v in kv.items():
    off = int(k, 16)
    orig = struct.unpack_from('<I', R, off)[0]
    exe = int(v, 16)
    if orig == exe:
        ok += 1
    else:
        bad += 1
        print(f'  MISMATCH +0x{off:x}: exe {exe:08x} orig {orig:08x}')
print(f'G-A3 {a3}: {ok}/{ok + bad} bit-exact -> {"PASS" if bad == 0 and ok == 26 else "FAIL"}')
