"""D4-WSB-C3-PORTED anti-island check (C2->C3 caller/callee halves) from the anchored binary.
callees: direct call/jmp rel32 targets inside [rva, next hooks.csv rva) (capstone);
callers: every E8/E9 rel32 in .text whose target is the rva, mapped to the containing hooks.csv row.
Confidence of each from hooks.csv.  py -3.12 -I antiisland.py <hooks.csv>"""
import bisect, csv, sys, pefile, capstone
H = sys.argv[1]
rows = {r[0]: r for r in csv.reader(open(H, encoding="utf-8", errors="replace")) if r and len(r[0]) == 8 and r[0].isalnum()}
rv = sorted(int(k, 16) for k in rows)
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
txt = [s for s in pe.sections if s.Name.rstrip(b"\0") == b".text"][0]
tb = 0x400000 + txt.VirtualAddress; td = txt.get_data()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
R = "0055b750 0055bd70 0055bd80 0055c2d0 00564310 005667c0 0056f0a0 0056f350 00574ad0 00575120 005751f0 005752b0 00575560 00576880 00577be0 00577cb0 00578b20 00578bd0 00578cb0 00578ff0".split()
def owner(a):
    i = bisect.bisect_right(rv, a) - 1
    return "%08x" % rv[i] if i >= 0 else "?"
def c(a): r = rows.get(a); return r[3] if r else "none"
callers = {r: set() for r in R}
for i in range(len(td) - 5):
    if td[i] in (0xE8, 0xE9):
        t = (tb + i + 5 + int.from_bytes(td[i+1:i+5], "little", signed=True)) & 0xffffffff
        k = "%08x" % t
        if k in callers: callers[k].add(owner(tb + i))
print("rva\tcallers\tcallees\tcaller_c2plus\tcallee_c2plus_or_leaf")
for r in R:
    a = int(r, 16); end = rv[bisect.bisect_right(rv, a)]
    data = pe.get_data(a - 0x400000, end - a); ce = set(); ind = 0
    for ins in md.disasm(data, a):
        if ins.mnemonic in ("call", "jmp"):
            if ins.op_str.startswith("0x"):
                t = int(ins.op_str, 16)
                if not (a <= t < end): ce.add("%08x" % t)
            elif ins.mnemonic == "call" or "[" in ins.op_str: ind += 1
    cl = sorted(x for x in callers[r] if x != r)
    clc = [f"{x}:{c(x)}" for x in cl]; cec = [f"{x}:{c(x)}" for x in sorted(ce)]
    okc = any(c(x) in ("C2", "C3", "C4") for x in cl)
    oke = (not ce) or any(c(x) in ("C2", "C3", "C4") for x in ce)
    print(f"{r}\t{' '.join(clc) or '-'}\t{' '.join(cec) or ('LEAF' if not ind else 'indirect-only x%d' % ind)}\t{okc}\t{oke}")
