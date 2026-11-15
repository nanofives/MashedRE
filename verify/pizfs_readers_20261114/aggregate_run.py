# RO-PIZFS-READERS-RUN: aggregate the access-watch TSVs (re/frida/js/access_watch.js) into per-group reader tables.
# usage: py -3.12 aggregate_run.py <exe> <function_inventory.csv> <data_dir> <out_tsv> <run>[,<run>..]
# Guard-page rows: pc = the faulting instruction. Hardware rows (op pc_after): pc = the next instruction; the accessing
# instruction is found by a linear sweep from the containing function's start that ends exactly at pc (or, for a
# rep-prefixed string instruction still iterating, the instruction at pc itself).
import sys, csv, bisect, collections, pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

exe, inv, data, out, runs = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5].split(",")
pe = pefile.PE(exe); IB = pe.OPTIONAL_HEADER.ImageBase
img = pe.get_memory_mapped_image()
md = Cs(CS_ARCH_X86, CS_MODE_32)
fs = sorted((int(r["rva"], 16) + (0 if int(r["rva"], 16) >= IB else IB), int(r["body_size"] or 0))
            for r in csv.DictReader(open(inv)))
starts = [f[0] for f in fs]

def func_of(va):
    i = bisect.bisect_right(starts, va) - 1
    if i >= 0 and va < fs[i][0] + max(fs[i][1], 1): return "FUN_%08x" % fs[i][0], fs[i][0]
    return "outside-inventory", (fs[i][0] if i >= 0 else va - 0x40)

def insn_at(va):
    for i in md.disasm(img[va - IB: va - IB + 16], va): return i
    return None

def insn_before(va):
    _, st = func_of(va)
    a = st
    while a < va:
        i = insn_at(a)
        if i is None: return None
        if a + i.size == va: return i
        a += i.size
    return None

def describe(pc, op):
    if "!" in pc: return pc, "", "non-image"
    va = int(pc, 16)
    if op == "pc_after":
        b = insn_before(va)
        i = insn_at(va)
        if (b is None or "[" not in b.op_str) and i is not None and i.mnemonic.startswith("rep"):
            return pc, "%s %s" % (i.mnemonic, i.op_str), func_of(va)[0]
        i = b
        if i is None: return pc, "[UNCERTAIN] no anchored insn ends at pc", func_of(va)[0]
        return "0x%08x" % i.address, "%s %s" % (i.mnemonic, i.op_str), func_of(i.address)[0]
    i = insn_at(va)
    return pc, ("%s %s" % (i.mnemonic, i.op_str)) if i else "?", func_of(va)[0]

agg = collections.OrderedDict()
for run in runs:
    for row in list(csv.reader(open("%s/%s.tsv" % (data, run)), delimiter="\t"))[1:]:
        w, pc, op, addr, st, n = row
        k = (pc, op)
        e = agg.setdefault(k, {"n": 0, "win": set(), "addr": set(), "stk": collections.Counter(), "runs": set()})
        e["n"] += int(n); e["win"].add(w); e["runs"].add(run); e["stk"][st] += int(n)
        e["addr"].update(addr.split(","))
with open(out, "w", encoding="utf-8", newline="") as f:
    wr = csv.writer(f, delimiter="\t")
    wr.writerow(["pc", "insn", "function", "outside_004b6xxx", "op", "count", "windows", "runs", "addresses", "top_stack3"])
    for (pc, op), e in sorted(agg.items(), key=lambda kv: kv[0]):
        ipc, ins, fn = describe(pc, op)
        out6 = "yes" if not (0x004b6000 <= int(ipc, 16) < 0x004b7000) else "no"
        addrs = sorted(e["addr"]); a = ",".join(addrs[:6]) + (",..(%d)" % len(addrs) if len(addrs) > 6 else "")
        wr.writerow([ipc, ins, fn, out6, "read/write (hw)" if op == "pc_after" else op, e["n"],
                     "/".join(sorted(e["win"])), "/".join(sorted(e["runs"])), a,
                     " ; ".join("%s x%d" % (s or "-", c) for s, c in e["stk"].most_common(3))])
print("wrote", out, len(agg), "rows")
