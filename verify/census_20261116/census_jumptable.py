# T2-RACETICK-CENSUS: pc_funcs entries whose 5-byte counter patch overlaps a jump table (data in .text).
# A counter there rewrites table words: 0x0040e430 (switchdataD_0040e430, the table of FUN_0040e3a0's
# jmp [eax*4+0x40e430] at 0x0040e3a9) turned case 0 into 0x????cfe9 (low byte = the E9 opcode), the race-tick AV
# of c7..c10 and r2..r4.
# Tables: every "jmp dword ptr [reg*4 + imm32]" and "mov reg, dword ptr [reg*4 + imm32]" in .text (capstone linear
# sweep), extended over consecutive dwords that point into .text; byte index tables "movzx reg, byte ptr [reg + imm32]"
# with imm32 in .text are extended to the next dword table or pc_funcs entry, capped at 256 bytes.
#   py -3.12 verify/census_20261116/census_jumptable.py <exe> <in list> <out list> <out tsv>
import csv, re, sys
import capstone, pefile

exe, lin, lout, tsv = sys.argv[1:5]
pe = pefile.PE(exe, fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
tx = next(s for s in pe.sections if s.Name.rstrip(b"\0") == b".text")
t0, t1 = base + tx.VirtualAddress, base + tx.VirtualAddress + tx.Misc_VirtualSize
data = tx.get_data()[:tx.Misc_VirtualSize]
rd = lambda va: int.from_bytes(data[va - t0:va - t0 + 4], "little")
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.skipdata = True
dw, bt = {}, {}
pat_dw = re.compile(r"dword ptr \[(?:e\w\w)?\s*\+?\s*e\w\w\*4 \+ (0x[0-9a-f]+)\]")
pat_bt = re.compile(r"byte ptr \[e\w\w \+ (0x[0-9a-f]+)\]")
for i in md.disasm(data, t0):
    if i.mnemonic in ("jmp", "mov"):
        m = pat_dw.search(i.op_str)
        if m and t0 <= int(m.group(1), 16) < t1:
            dw.setdefault(int(m.group(1), 16), i.address)
    if i.mnemonic == "movzx":
        m = pat_bt.search(i.op_str)
        if m and t0 <= int(m.group(1), 16) < t1:
            bt.setdefault(int(m.group(1), 16), i.address)
ents = sorted(int(r["entry"], 16) for r in csv.DictReader(open("re/console/pc_funcs.csv")))
spans = []
for b, site in dw.items():
    e = b
    while e + 4 <= t1 and t0 <= rd(e) < t1:
        e += 4
    if e > b:
        spans.append((b, e, "dword", site))
dws = sorted(s[0] for s in spans)
for b, site in bt.items():
    nxt = min([x for x in dws if x > b] + [x for x in ents if x > b] + [b + 256])
    spans.append((b, nxt, "byte", site))
lst = open(lin).read().split()
hit = []
for e in lst:
    v = int(e, 16)
    for b, en, k, site in spans:
        if v < en and v + 5 > b:
            hit.append((e, b, en, k, site)); break
hs = {h[0] for h in hit}
open(lout, "w").write("\n".join(x for x in lst if x not in hs) + "\n")
with open(tsv, "w") as f:
    f.write("rva\treason\ttable\ttable_end\tkind\tindexing_site\n")
    for e, b, en, k, site in hit:
        f.write("%s\tpatch window overlaps a jump table\t0x%08x\t0x%08x\t%s\t0x%08x\n" % (e, b, en, k, site))
print("tables: %d dword, %d byte; list %d -> %d (excluded %d)" % (len([s for s in spans if s[2] == "dword"]), len(bt), len(lst), len(lst) - len(hs), len(hs)))
for h in hit:
    print("  %s table 0x%08x-0x%08x %s site 0x%08x" % h)
