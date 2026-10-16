"""One-off: add Frontend\\KvFind.cpp to the host navsm test build and drop KvFind.cpp's HookSystem include (it uses no hook macro)."""
import pathlib

root = pathlib.Path(__file__).resolve().parent.parent.parent
p = root / "re" / "analysis" / "standalone_menu_sm" / "harvest" / "build_navsm_test.bat"
b = p.read_bytes()
nl = b"\r\n" if b"\r\n" in b else b"\n"
old = b"   ..\\..\\..\\..\\mashedmod\\src\\mashed_re\\Frontend\\MenuNavSM.cpp ^"
assert b.count(old) == 1
p.write_bytes(b.replace(old, old + nl + b"   ..\\..\\..\\..\\mashedmod\\src\\mashed_re\\Frontend\\KvFind.cpp ^"))
print("bat updated")

q = root / "mashedmod" / "src" / "mashed_re" / "Frontend" / "KvFind.cpp"
c = q.read_bytes()
nl2 = b"\r\n" if b"\r\n" in c else b"\n"
inc = b'#include "../Core/HookSystem.h"' + nl2 + nl2
assert c.count(inc) == 1, c.count(inc)
q.write_bytes(c.replace(inc, b""))
print("KvFind include dropped")
