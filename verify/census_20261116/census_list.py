# T2-RACEPATH-CENSUS: the --prov-count list. Every entry of re/console/pc_funcs.csv gets a native call counter
# (detorig.js provInstall, Interceptor + CModule) unless Interceptor's patch would break the code:
#   - the instructions Interceptor relocates (whole instructions from the entry until >= 5 bytes) must stay inside the
#     function's extent (entry .. body_end) and contain no ret / jmp / int3 before 5 bytes
#   - no direct branch or call anywhere in .text, and no jump-table entry, targets a byte
#     strictly inside that window
# Excluded entries are written with their reason; the census reports them as "not instrumented".
#   py -3.12 verify/census_20261116/census_list.py <MASHED.exe> <out_list.txt> <out_excluded.tsv>
import csv, sys
from pathlib import Path
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_JUMP, CS_GRP_CALL
from capstone.x86 import X86_OP_IMM

WT = Path(__file__).resolve().parent.parent.parent
exe, out_list, out_excl = sys.argv[1], Path(sys.argv[2]), Path(sys.argv[3])
pe = pefile.PE(exe, fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
img = pe.get_memory_mapped_image()
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True

funcs = []
with open(WT / "re/console/pc_funcs.csv", newline="") as f:
    for r in csv.DictReader(f):
        funcs.append((int(r["entry"], 16), int(r["body_end"], 16), r["name"]))

# direct branch / call targets over every function body (linear sweep per body)
targets = set()
for e, end, _ in funcs:
    code = img[e - base:end - base + 1]
    for ins in md.disasm(code, e):
        if ins.group(CS_GRP_JUMP) or ins.group(CS_GRP_CALL):
            for op in ins.operands:
                if op.type == X86_OP_IMM:
                    targets.add(op.imm & 0xffffffff)

# jump tables (the image has no base relocations): every `jmp dword [reg*4 + table]` reads its table while the
# entries point inside the same function (entry .. body_end)
from capstone.x86 import X86_OP_MEM
for e, end, _ in funcs:
    code = img[e - base:end - base + 1]
    for ins in md.disasm(code, e):
        if ins.mnemonic != "jmp" or len(ins.operands) != 1 or ins.operands[0].type != X86_OP_MEM:
            continue
        m = ins.operands[0].mem
        if m.scale != 4 or m.base != 0 or not (base <= (m.disp & 0xffffffff) < base + len(img)):
            continue
        t = (m.disp & 0xffffffff) - base
        while 0 <= t <= len(img) - 4:
            v = int.from_bytes(img[t:t + 4], "little")
            if not (e <= v <= end): break
            targets.add(v); t += 4
keep, excl = [], []
for e, end, name in funcs:
    code = img[e - base:e - base + 32]
    n, why = 0, ""
    for ins in md.disasm(code, e):
        if n < 5 and ins.mnemonic in ("ret", "retn", "jmp", "int3") and n + ins.size < 5:
            why = f"{ins.mnemonic} at +{n} before 5 bytes"; break
        n += ins.size
        if n >= 5: break
    if not why and n < 5:
        why = "window not decodable"
    if not why and e + n - 1 > end:
        why = f"window {n} B past body_end 0x{end:08x}"
    if not why:
        inner = sorted(t for t in targets if e < t < e + n)
        if inner:
            why = "branch target inside window: " + ",".join(f"0x{t:08x}" for t in inner)
    if why:
        excl.append((f"0x{e:08x}", name, why))
    else:
        keep.append(f"0x{e:08x}")
out_list.write_text("\n".join(keep) + "\n")
with open(out_excl, "w", newline="") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "name", "reason"]); w.writerows(excl)
print(f"functions {len(funcs)} counted {len(keep)} excluded {len(excl)}")
