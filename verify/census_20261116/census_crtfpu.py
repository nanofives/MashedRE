# T2-RACEPATH-CENSUS: CRT functions that use the x87 unit. Run c7 (--crash-fpmask) traps 30 times at 0x00478542, the
# first x87 instruction after FUN_004783f0's call of CRT::acos 0x004a3384 (0x0047853d), with FCW 0x004f
# (precision and underflow unmasked). acos calls __fload_withFB 0x004a70d5. The CRT x87 helpers save / restore the
# control word and branch on x87 status bits, and an Interceptor counter on them is the candidate cause. This lists
# every function whose hooks.csv subsystem is an msvc-crt library or whose name starts with CRT::, and whose body
# holds an x87 instruction, so the next census run can leave them uncounted (reported as not instrumented).
#   py -3.12 verify/census_20261116/census_crtfpu.py <MASHED.exe> <count_list_in> <count_list_out> <excluded.tsv>
import csv, sys
from pathlib import Path
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

WT = Path(__file__).resolve().parent.parent.parent
exe, lin, lout, excl = sys.argv[1:5]
pe = pefile.PE(exe, fast_load=True); img = pe.get_memory_mapped_image(); base = pe.OPTIONAL_HEADER.ImageBase
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True
F = {int(r["entry"], 16): int(r["body_end"], 16) for r in csv.DictReader(open(WT / "re/console/pc_funcs.csv"))}
H = {}
for r in csv.DictReader(open(WT / "hooks.csv", encoding="utf-8")):
    try: H[int(r["rva"], 16)] = (r["name"], r["subsystem"])
    except ValueError: pass
out = []
for e, end in F.items():
    nm, ss = H.get(e, ("", ""))
    if not ("msvc-crt" in ss or nm.startswith("CRT::")): continue
    fpu = next((i for i in md.disasm(img[e - base:end - base + 1], e) if i.mnemonic.startswith("f")), None)
    if fpu is not None:
        out.append((f"0x{e:08x}", nm, ss, f"x87 {fpu.mnemonic} at 0x{fpu.address:08x}"))
drop = {r[0] for r in out}
keep = [l.strip() for l in open(lin) if l.strip() and l.strip().lower() not in drop]
Path(lout).write_text("\n".join(keep) + "\n")
with open(excl, "w", newline="") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n"); w.writerow(["rva", "name", "subsystem", "reason"]); w.writerows(sorted(out))
print(f"CRT x87 functions {len(out)}; counted {len(keep)}")
