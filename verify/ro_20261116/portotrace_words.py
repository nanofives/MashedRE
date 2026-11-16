# PORT-OT-RACE: copy of verify/ro_20261116/loadalign_words.py from orch/RO-EXE-ARM-LOADALIGN 01df8abf9 (that branch is not in orch/integration).
# RO-EXE-ARM-LOADALIGN: print chosen dwords from a reference checkpoint dump and an exe checkpoint dump
# (both images start at 0x005ea000, the layout detorig_firstdiv.py reads).
#   py -3.12 verify/ro_20261116/loadalign_words.py <ref ckpt.data.bin> <run ckpt.data.bin> <va,va,...>
import struct, sys
from pathlib import Path

BASE = 0x005EA000
a = Path(sys.argv[1]).read_bytes()
b = Path(sys.argv[2]).read_bytes()
for s in sys.argv[3].split(","):
    va = int(s, 16)
    o = va - BASE
    ra = struct.unpack_from("<I", a, o)[0]
    rb = struct.unpack_from("<I", b, o)[0]
    print(f"0x{va:08x}  ref 0x{ra:08x} ({ra if ra < 2**31 else ra - 2**32})  run 0x{rb:08x} ({rb if rb < 2**31 else rb - 2**32})  {'same' if ra == rb else 'DIFF'}")
