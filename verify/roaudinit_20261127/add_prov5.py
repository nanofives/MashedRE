# RO-AUDIOINIT-45D460-REACH-5: PROVISIONAL.tsv rows (class P3, NOT-PROVEN on equiv.py tool commit 1ed7ceb1a) and the audio
# profile's prov_bind / alias lines (state/audio/rules.tsv; the 0x00551460 alias line, owned by RACE-VFS-COURSELUA, untouched).
from pathlib import Path
CRLF = chr(13) + chr(10)
T = chr(9)
D = Path("D:/MashedWT/RO-AUDIOINIT-45D460-REACH-5_data")
RES = "verify/roaudinit_20261127/RESULT_RO-AUDIOINIT-45D460-REACH-5.md"
eq = {}
for line in (D / "equiv2.txt").read_text().splitlines():
    p = line.split(T)
    if len(p) >= 3 and p[0].startswith("0x"):
        eq[p[0]] = (p[1], p[2][:300] + " (equiv.py tool commit 1ed7ceb1a)")
rows = [
    ("0x005b8910", "FUN_005b8910", "re/match/s27/005b8910.c", "71.0",
     "new source (RO-AUDIOINIT-45D460-REACH-4 draft from the Ghidra decompilation) checked against the listing 0x005b8910..0x005b8bda by "
     "RO-AUDIOINIT-45D460-REACH-5: call args of 0x005c6bb0 (0x005b8954..0x005b895f), 0x005b9460 (0x005b897b..0x005b8995), 0x005aeb90(1.0f) "
     "(0x005b89c0), list insert at 0x005b8b89..0x005b8ba9, both 0x005b8c40 call sites. The equiv first difference is the operand order of "
     "one compare (eq 0,x against eq x,0). Class P3 (conditional on the reading, no replay yet)"),
    ("0x005b8c40", "FUN_005b8c40", "re/match/s27/005b8c40.c", "26.2",
     "source rewritten by RO-AUDIOINIT-45D460-REACH-5 from the listing 0x005b8c40..0x005b92cb (the MATCH-S27 h1 source declared the record a "
     "float and dropped callee arguments): __stdcall vtable calls, truncating float-to-int at 0x005b8e12 / 0x005b8f09 / 0x005b9009, 32-bit "
     "divides at 0x005b910e / 0x005b9112, three property-set buffers (0x24 / 4 / 0x34 B). The equiv first difference is the early-return "
     "store of 0 at +0x88, which our build takes from the +0x84 register (0 on that path). Class P3 (reading, no replay)"),
    ("0x005c6f50", "FUN_005c6f50", "re/match/s28/005c6f50.c", "91.1",
     "new source by RO-AUDIOINIT-45D460-REACH-5 from the listing 0x005c6f50..0x005c7065. The equiv first difference is the stack word "
     "0x005ab040 (no parameters) would see as an argument: the original keeps the fild qword temp there. Class P3 (reading, no replay)"),
    ("0x005a65d0", "FUN_005a65d0", "re/match/s25/005a65d0.c", "84.6",
     "existing source (hooks.csv C2) checked against the listing 0x005a65d0..0x005a663f by RO-AUDIOINIT-45D460-REACH-5: rep stosd 0x19 dwords "
     "at +0x28, call 0x005a7420(param_1), field stores, +0x34 = *param_3 & 0x18. Class P3 (reading, no replay)"),
    ("0x004ccef0", "FUN_004ccef0", "re/match/s20/004ccef0.c", "0.0",
     "new source by RO-AUDIOINIT-45D460-REACH-5 from the listing 0x004ccef0..0x004ccefd (call 0x004a45fb with the size, return its value). "
     "The DDK and rw9044 builds emit a tail jmp (5 B) for every flag set tried; the equiv first difference is eax at the call, which the "
     "original loads with the argument and 0x004a45fb does not read as input. The hand body RwCoreHand_004ccef0 row above stays. "
     "Class P3 (reading, no replay)"),
]
p = Path("re/match/PROVISIONAL.tsv")
s = p.read_bytes().decode("utf-8")
assert s.endswith(CRLF)
add = ""
for rva, name, src, pct, diag in rows:
    assert (rva + T + name + T + src) not in s, rva
    v, first = eq[rva]
    assert v == "NOT-PROVEN", (rva, v)
    add += T.join([rva, name, src, "/O2 /Ob1 /G5", pct, v, first, "not run", diag, "not run", "provisional", RES]) + CRLF
p.write_bytes((s + add).encode("utf-8"))
r = Path("re/tools/bind/state/audio/rules.tsv")
s = r.read_bytes().decode("utf-8")
nl = CRLF if CRLF in s else chr(10)
if not s.endswith(nl):
    s += nl
lines = [
    "# RO-AUDIOINIT-45D460-REACH-5: the RO-AUDIOINIT-DEPTH window (0x005b8910 enter to the 0x00604604 store): 0x005b8910 (stop of",
    "# REACH-4), its callee 0x005b8c40, the stored code pointers 0x005c6f50 (0x005c6cc0) and 0x005a65d0 (0x005a6340), RwGlobals+0x108",
    "# 0x004ccef0 (0x005aea60). Sources checked against the listings, equiv NOT-PROVEN, PROVISIONAL.tsv class P3",
    "prov_bind" + T + "005b8910",
    "prov_bind" + T + "005b8c40",
    "prov_bind" + T + "005c6f50",
    "prov_bind" + T + "005a65d0",
    "prov_bind" + T + "004ccef0",
    "# RO-AUDIOINIT-45D460-REACH-5: CRT pow (0x004a2d30, C4 libid) called by 0x005b8c40 at 0x005b91c2: the exe CRT pow body",
    "alias" + T + "004a2d30" + T + "RwCoreCrt_pow" + T + "RwCore/RwCore_Crt.cpp",
]
r.write_bytes((s + nl.join(lines) + nl).encode("utf-8"))
print("ok", len(rows), "rows")
