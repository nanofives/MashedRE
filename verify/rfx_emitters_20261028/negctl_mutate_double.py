import sys
src, dst = sys.argv[1], sys.argv[2]
s = open(src, encoding="utf-8").read()
for ax in ("x", "y", "z"):
    k = {"x": 0, "y": 1, "z": 2}[ax]
    a = "const float r0%s = (ns * u[%d]) + (c * r[%d]);" % (ax, k, k)
    b = "const float r0%s = (float)(((double)ns * u[%d]) + ((double)c * r[%d]));" % (ax, k, k)
    assert a in s, a
    s = s.replace(a, b)
    a = "const float u0%s = (s * r[%d]) + (c * u[%d]);" % (ax, k, k)
    b = "const float u0%s = (float)(((double)s * r[%d]) + ((double)c * u[%d]));" % (ax, k, k)
    assert a in s, a
    s = s.replace(a, b)
hdr = src.replace(chr(92), "/").rsplit("/", 1)[0] + "/RfxEmitter.h"
s = s.replace('#include "RfxEmitter.h"', '#include "' + hdr + '"')
open(dst, "w", encoding="utf-8").write(s)
