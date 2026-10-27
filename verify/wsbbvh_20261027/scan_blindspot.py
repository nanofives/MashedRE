"""Return-only blind spot across the RW-Physics bands (lead ask: where does it affect BROAD / SOLV).

  py -3.12 verify/wsbbvh_20261027/scan_blindspot.py > verify/wsbbvh_20261027/blindspot.tsv

For every hooks.csv row at C3 or C4 in BROAD 0x00558000-0x0055efff, BVH 0x0055f000-0x00565fff and
SOLV 0x00566000-0x0056ffff:
  wrapper = the ShadowAB template the port's shadow wrapper calls (Run = return only, RunTracked = page
            write tracking, RunRegion = one output region, none = no shadow wrapper found), read from
            mashedmod/src/mashed_re/**/*.cpp (`ShadowAB::<T>(ab, FUN_<rva>_impl`).
  stores    = write-access memory operands with a base other than ESP in the ORIGINAL body
            [rva, next hooks.csv rva) (capstone linear sweep, x87 fst*/fist* counted as writes, as in
            verify/wsbbvh_20261026/scan_stores.py), plus string stores (stos/movs) as str.
  blind     = YES when wrapper is Run and stores+str > 0: a return-only shadow A/B cannot see that
            body's out-memory, so its C3 rests on the return alone.
"""
import bisect, csv, re
from pathlib import Path
import pefile, capstone
from capstone import x86_const as X

WT = Path(__file__).resolve().parent.parent.parent
pe = pefile.PE(str(WT.parent.parent / "original" / "MASHED.exe.unpatched"), fast_load=True)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
rows = [r for r in csv.reader(open(WT / "hooks.csv", encoding="utf-8", errors="replace"))
        if r and len(r[0]) == 8 and re.fullmatch(r"[0-9a-fA-F]{8}", r[0])]
rv = sorted({int(r[0], 16) for r in rows})
BANDS = [("BROAD", 0x00558000, 0x0055f000), ("BVH", 0x0055f000, 0x00566000), ("SOLV", 0x00566000, 0x00570000)]
X87W = ("fstp", "fst", "fistp", "fist", "fisttp", "fbstp", "fnstcw", "fnstenv", "fnsave")

src = {}
for p in (WT / "mashedmod" / "src" / "mashed_re").rglob("*.cpp"):
    for m in re.finditer(r"ShadowAB::(Run|RunTracked|RunRegion)\(ab,\s*FUN_([0-9a-fA-F]{8})_impl", p.read_text(encoding="utf-8", errors="replace")):
        src.setdefault(m.group(2).lower(), (m.group(1), p.name))

print("band\trva\tconfidence\tstatus\twrapper\tfile\tstores\tstr\tblind\tfirst_store")
for r in sorted(rows, key=lambda r: int(r[0], 16)):
    a = int(r[0], 16)
    band = next((b for b, lo, hi in BANDS if lo <= a < hi), None)
    if band is None or r[3] not in ("C3", "C4"):
        continue
    i = bisect.bisect_right(rv, a); end = rv[i] if i < len(rv) else a + 0x100
    st, strs = [], []
    for ins in md.disasm(pe.get_data(a - 0x400000, end - a), a):
        if "stos" in ins.mnemonic or "movs" in ins.mnemonic:
            if not ins.mnemonic.startswith("movsx") and not ins.mnemonic.startswith("movss") and not ins.mnemonic.startswith("movsd x"):
                strs.append(ins.address); continue
        for op in ins.operands:
            if op.type == X.X86_OP_MEM and ((op.access & capstone.CS_AC_WRITE) or ins.mnemonic in X87W) \
                    and op.mem.base != X.X86_REG_ESP:
                st.append(ins.address)
    w, f = src.get(r[0].lower(), ("none", ""))
    blind = "YES" if w == "Run" and (st or strs) else "no"
    print(f"{band}\t0x{r[0].lower()}\t{r[3]}\t{r[4]}\t{w}\t{f}\t{len(st)}\t{len(strs)}\t{blind}\t"
          f"{('0x%08x' % min(st + strs)) if (st or strs) else ''}")
