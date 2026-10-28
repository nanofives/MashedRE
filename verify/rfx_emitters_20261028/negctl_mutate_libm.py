import sys
src, dst = sys.argv[1], sys.argv[2]
s = open(src, encoding="utf-8").read()
a = "            SinCos24(F(t.rot[i]), &s, &c);"
assert a in s
s = s.replace(a, "            s = std::sin(F(t.rot[i])); c = std::cos(F(t.rot[i]));")
s = s.replace("#include <cstring>", "#include <cstring>\n#include <cmath>")
hdr = src.replace(chr(92), "/").rsplit("/", 1)[0] + "/RfxEmitter.h"
s = s.replace('#include "RfxEmitter.h"', '#include "' + hdr + '"')
open(dst, "w", encoding="utf-8").write(s)
