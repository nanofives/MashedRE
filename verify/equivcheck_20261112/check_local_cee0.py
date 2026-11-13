"""EQUIV-FU-48E820: ddkobj check for the local helper copy of 0x0048cee0 that the s15 DDK object carries.

`ddkobj.py check` compares only each unit's MATCH definitions. The s15 unit (re/match/s15/0048e820.c) also carries
a static FUN_0048cee0, which ddkobj keeps as a local helper (`local_helpers` in ddkobj_build.json). This applies the
same masking main_check uses (relocations masked, __ddkabs_ DIR32 compared exactly) to that symbol's section and the
original 0x0048cee0. usage: py -3.12 verify/equivcheck_20261112/check_local_cee0.py [--out mashedmod/build/ddkobj]
"""
import json, pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re/tools/ddkobj"))
sys.path.insert(0, str(ROOT / "re/tools/match"))
import pefile                       # noqa: E402
import coffedit as CE               # noqa: E402
import match as M                   # noqa: E402
from ddkobj import read_map         # noqa: E402

out = ROOT / (sys.argv[sys.argv.index("--out") + 1] if "--out" in sys.argv else "mashedmod/build/ddkobj")
RVA, NAME = 0x0048cee0, "_FUN_0048cee0"
rep = json.loads((out / "ddkobj_build.json").read_text())
u = next(x for x in rep["units"] if x["src"] == "re/match/s15/0048e820.c")
assert u["status"] == "ok" and NAME in u.get("local_helpers", []), (u["status"], u.get("local_helpers"))
co = CE.Coff.load(u["obj"])
sym = next(s for s in co.syms if s.name == NAME and s.sec > 0)
sec = co.section(sym.sec)
n = len(sec.data)
syms, _ = read_map(out / "mashed_re_ddk.map")
va = syms[(NAME, pathlib.Path(u["obj"]).name.lower())]
pe = pefile.PE(str(out / "mashed_re_ddk.exe"), fast_load=True)
img, base = pe.get_memory_mapped_image(), pe.OPTIONAL_HEADER.ImageBase
ope = M.pe()
oimg, obase = ope.get_memory_mapped_image(), ope.OPTIONAL_HEADER.ImageBase
got, want = img[va - base:va - base + n], oimg[RVA - obase:RVA - obase + n]
mask, exact = bytearray(n), 0
for off, si, typ in sec.relocs:
    t = co.slots[si]
    if t is not None and t.name.startswith("__ddkabs_") and typ == CE.REL_I386_DIR32:
        exact += 1
        continue
    for k in range(off, min(off + 4, n)):
        mask[k] = 1
diff = [k for k in range(n) if not mask[k] and got[k] != want[k]]
print(f"0x{RVA:08x}\t{NAME}\texe_va=0x{va:08x}\tsize={n}\trelocs_masked={len(sec.relocs) - exact}\t"
      f"abs_data_refs_exact={exact}\tdiff_bytes={len(diff)}\t{'IDENTICAL' if not diff else 'DIFFERENT +0x%x' % diff[0]}")
sys.exit(1 if diff else 0)
