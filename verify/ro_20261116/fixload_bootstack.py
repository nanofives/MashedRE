# RO-REF-FIXED-LOAD: count boot clock calls (calllog_boot.bin, 64-byte records [N, kind, esp[0..13]]) whose stack scan holds
# the piz progress callback FUN_00429290's return 0x00429297 (its call to FUN_0045d3f0 -> FUN_005a60b0), and those holding
# 0x00428af4 (FUN_00428a30's FUN_004950b0 call). RESULT_RO-REF-FIXED-LOAD section 2.
#   py -3.12 verify/ro_20261116/fixload_bootstack.py RUN_DIR [RUN_DIR ...]
import struct, sys
from pathlib import Path

for d in sys.argv[1:]:
    b = (Path(d) / "calllog_boot.bin").read_bytes()
    a = c = both = 0
    for i in range(len(b) // 64):
        st = struct.unpack_from("<16I", b, i * 64)[2:]
        A, C = 0x00429297 in st, 0x00428af4 in st
        a += A; c += C; both += A and C
    print(f"{Path(d).name}: boot calls {len(b) // 64}, with 0x00429297 {a}, with 0x00428af4 {c}, both {both}")
