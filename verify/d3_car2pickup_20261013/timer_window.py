#!/usr/bin/env python3
"""Print +0xaec, velocity +0x9b0..b8, speed +0x9e4 and position +0x958/+0x960 from 2 frames before the first nonzero
+0xaec to 14 frames after, for one MSD1 capture. Usage: py -3.12 timer_window.py <x.msd>"""
import os, struct, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as M
fr = list(M.parse(sys.argv[1]))
i = next(k for k, (_, p) in enumerate(fr) if struct.unpack_from('<i', p, 0xaec)[0] != 0)
for k in range(max(0, i - 2), min(len(fr), i + 14)):
    f, p = fr[k]
    t = struct.unpack_from('<i', p, 0xaec)[0]
    v = struct.unpack_from('<fff', p, 0x9b0)
    print('%5d aec=%5d v=(%9.3f,%9.3f,%9.3f) spd=%9.3f pos=(%.5f,%.5f)' % (
        f, t, *v, struct.unpack_from('<f', p, 0x9e4)[0], struct.unpack_from('<f', p, 0x958)[0], struct.unpack_from('<f', p, 0x960)[0]))
