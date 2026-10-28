# copy of verify/wsbbvh_20261026/scan_stores.py, hooks.csv path and RVA list changed (D4-WSB-C3-PORTED)
"""Non-stack memory stores in each original body: [rva, next hooks.csv rva). capstone write-access
memory operands whose base is not ESP, plus string stores (stos/movs) reported separately."""
import csv, bisect, pefile, capstone
from capstone import x86_const as X
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
H = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-WSB-C3-PORTED\hooks.csv"
rv = sorted({int(r[0], 16) for r in csv.reader(open(H, encoding="utf-8", errors="replace")) if r and len(r[0]) == 8 and r[0].isalnum()})
R = "0055b750 0055bd70 0055bd80 0055c2d0 00564310 005667c0 0056f0a0 0056f350 00574ad0 00575120 005751f0 005752b0 00575560 00576880 00577be0 00577cb0 00578b20 00578bd0 00578cb0 00578ff0".split()
for r in R:
    a = int(r, 16); end = rv[bisect.bisect_right(rv, a)]
    data = pe.get_data(a - 0x400000, end - a)
    st, strs, n = [], [], 0
    for ins in md.disasm(data, a):
        n += 1
        if ins.mnemonic.startswith(("rep", "stos", "movs")) and ("stos" in ins.mnemonic or "movs" in ins.mnemonic):
            strs.append("%08x %s" % (ins.address, ins.mnemonic)); continue
        for op in ins.operands:
            x87w = ins.mnemonic in ('fstp', 'fst', 'fistp', 'fist', 'fisttp', 'fbstp', 'fnstcw', 'fnstenv', 'fnsave')
            if op.type == X.X86_OP_MEM and ((op.access & capstone.CS_AC_WRITE) or x87w) and op.mem.base != X.X86_REG_ESP:
                st.append("%08x %s %s" % (ins.address, ins.mnemonic, ins.op_str))
    print("%s size=%d insns=%d stores=%d str=%d  %s %s" % (r, end - a, n, len(st), len(strs), st[:3], strs[:2]))
