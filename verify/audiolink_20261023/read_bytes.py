# audiolink byte-read step: dump image bytes at fixed VAs from original/MASHED.exe.unpatched (the anchor), and say
# whether each VA is backed by file data or lies past its section's raw size (BSS: zero at load, written at runtime).
# Usage: py -3.12 read_bytes.py VA:LEN [VA:LEN ...]
import struct, sys
from pathlib import Path
EXE = Path(__file__).resolve().parents[2].parent.parent / 'original' / 'MASHED.exe.unpatched'
if not EXE.exists(): EXE = Path(r'C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched')
d = EXE.read_bytes()
pe = struct.unpack_from('<I', d, 0x3c)[0]; n = struct.unpack_from('<H', d, pe + 6)[0]; osz = struct.unpack_from('<H', d, pe + 20)[0]
base = struct.unpack_from('<I', d, pe + 52)[0]; s0 = pe + 24 + osz
secs = []
for i in range(n):
    s = s0 + i * 40; nm = d[s:s + 8].rstrip(b'\0').decode(); vs, va, rs, ro = struct.unpack_from('<IIII', d, s + 8)
    secs.append((nm, va, vs, rs, ro))
for arg in sys.argv[1:]:
    v, ln = arg.split(':'); v = int(v, 16); ln = int(ln, 0); rva = v - base
    sec = next((x for x in secs if x[1] <= rva < x[1] + max(x[2], x[3])), None)
    if not sec: print(f'{v:#010x}: not in any section'); continue
    nm, va, vs, rs, ro = sec; o = rva - va
    if o >= rs: print(f'{v:#010x} [{nm}] BSS (offset {o:#x} >= raw size {rs:#x}): zero at load'); continue
    b = d[ro + o: ro + o + min(ln, rs - o)]
    words = ' '.join(f'{struct.unpack_from("<I", b, k)[0]:08x}' for k in range(0, len(b) - 3, 4))
    print(f'{v:#010x} [{nm}] raw {b.hex()}')
    print(f'            dwords {words}')
