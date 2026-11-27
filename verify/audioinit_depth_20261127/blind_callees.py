# RO-AUDIOINIT-DEPTH blind spot: direct call / jmp targets of the window functions (window_functions.tsv) that land on a
# pc_funcs.csv entry with no census counter (not in count_list8.txt). Capstone over original/MASHED.exe.unpatched,
# linear sweep of each function's pc_funcs extent. Indirect calls (call reg / call [mem]) are counted, not resolved.
#   py -3.12 verify/audioinit_depth_20261127/blind_callees.py <window_functions.tsv> <count_list.txt>
import csv, struct, sys
from pathlib import Path
import capstone
from capstone import x86

WT = Path(__file__).resolve().parent.parent.parent
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
pe = (MAIN / "original" / "MASHED.exe.unpatched").read_bytes()
e = struct.unpack_from("<I", pe, 0x3c)[0]
nsec, opt = struct.unpack_from("<H", pe, e + 6)[0], struct.unpack_from("<H", pe, e + 20)[0]
secs = [struct.unpack_from("<IIII", pe, e + 24 + opt + 40 * i + 8) for i in range(nsec)]
def rd(va, n):
    for vs, rva, rs, ro in secs:
        if 0x400000 + rva <= va < 0x400000 + rva + rs:
            o = ro + va - 0x400000 - rva; return pe[o:o + n]
    return b""
ext = {}
for r in csv.reader(open(WT / "re/console/pc_funcs.csv")):
    if r and r[0].startswith("0x"): ext["0x%08x" % int(r[0], 16)] = (int(r[0], 16), int(r[1], 16))
counted = set("0x%08x" % int(x, 16) for x in open(sys.argv[2]).read().split())
win = [r["rva"] for r in csv.DictReader(open(sys.argv[1]), delimiter="\t")]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
blind, indirect = {}, {}
for k in win:
    lo, hi = ext[k]
    for ins in md.disasm(rd(lo, hi - lo + 1), lo):
        if ins.id in (x86.X86_INS_CALL, x86.X86_INS_JMP):
            op = ins.operands[0]
            if op.type == x86.X86_OP_IMM:
                t = "0x%08x" % (op.imm & 0xffffffff)
                if t in ext and t not in counted and t != k: blind.setdefault(t, []).append(f"{k}@0x{ins.address:08x}")
            elif ins.id == x86.X86_INS_CALL:
                indirect[k] = indirect.get(k, 0) + 1
print("window functions", len(win))
print("direct targets with no counter:", len(blind))
for t, v in sorted(blind.items()): print(" ", t, ",".join(v))
print("indirect call sites:", sum(indirect.values()), "in", len(indirect), "functions:", ",".join(f"{k}x{n}" for k, n in sorted(indirect.items())))
