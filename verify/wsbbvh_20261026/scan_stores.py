"""Non-stack memory stores in each original body: [rva, next hooks.csv rva). capstone write-access
memory operands whose base is not ESP, plus string stores (stos/movs) reported separately."""
import csv, bisect, pefile, capstone
from capstone import x86_const as X
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
H = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-WSB-C4-BVH\hooks.csv"
rv = sorted({int(r[0], 16) for r in csv.reader(open(H, encoding="utf-8", errors="replace")) if r and len(r[0]) == 8 and r[0].isalnum()})
R = "0055fe50 0055fea0 0055ff70 0055ff90 005601f0 00560260 00561040 00561390 00561c50 00561e60 00561e80 00563e70 00563f60 00564040 005641b0 005646c0 00564c80 00565120 005651b0 00565200 00565260 00565550".split()
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
