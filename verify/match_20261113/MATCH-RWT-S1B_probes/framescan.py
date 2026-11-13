"""List functions in an address range that open with push ebp / mov ebp, esp and report whether they touch ebx.
usage: py -3.12 framescan.py <lo> <hi>   (function starts and sizes from re/console/pc_funcs.csv via match.py)"""
import sys, pathlib, csv
WT = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-RWT-S1B")
sys.path.insert(0, str(WT / "re/tools/match"))
import capstone, match
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
rows = list(csv.reader(open(WT / "re/console/pc_funcs.csv")))
hdr = rows[0]
starts = []
for r in rows[1:]:
    try:
        a = int(r[0], 16)
    except ValueError:
        continue
    if lo <= a < hi:
        starts.append(a)
for a in sorted(set(starts)):
    n = match.ghidra_size(a)
    if not n:
        continue
    b = match.original_bytes(a, n)
    if b[:3] != b"\x55\x8b\xec":
        continue
    ins = list(md.disasm(b, a))
    ebx = [i for i in ins if "ebx" in i.op_str or "bl" in i.op_str.split() or "bx" in i.op_str.split()]
    pro = " ; ".join(f"{i.mnemonic} {i.op_str}" for i in ins[2:5])
    print(f"0x{a:08x} size {n:5d} ebx_refs {len(ebx):3d}  {pro}")
