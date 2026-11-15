"""Compare summary() before (equiv_old.py = the row-2 commit) and after (working tree) on a bounded sample: the
direct-call trees of 0x005adc10 and 0x005ae650 (depth <= 3) plus 300 pc_funcs.csv entries drawn with seed 49.
One line per function, flushed."""
import sys, importlib, random, struct
M = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\EQUIV-INDIRECT-SCAN-COMPLETENESS\re\tools\match"
S = r"C:\Users\maria\AppData\Local\Temp\claude\2279e0b7f1f09563\3298e614-25b6-4a81-a325-be0942aee896\scratchpad"
sys.path.insert(0, M)
sys.path.insert(0, S)
new = importlib.import_module("equiv")
old = importlib.import_module("equiv_old")
sys.setrecursionlimit(20000)
ents = sorted(int(l.split(",")[0], 16) for l in open(M + r"\..\..\..\re\console\pc_funcs.csv").read().splitlines()[1:]
              if l.startswith("0x"))
img = new.image()


def callees(f):
    sz = new.match.ghidra_size(f) or 0
    out = set()
    for cs in new._MD.disasm(img.bytes(f, sz) or b"", f):
        if cs.mnemonic == "call" and cs.operands[0].type == 2:      # X86_OP_IMM
            out.add(new.resolve_thunk(cs.operands[0].imm))
    return out


tree, todo = set(), [(0x005adc10, 0), (0x005ae650, 0)]
while todo:
    f, d = todo.pop()
    if f in tree or d > 3:
        continue
    tree.add(f)
    todo += [(c, d + 1) for c in callees(f)]
rnd = random.Random(49).sample(ents, 300)
sample = sorted(tree) + sorted(set(rnd) - tree)
print(f"sample: {len(tree)} call-tree functions + {len(set(rnd) - tree)} random = {len(sample)}", flush=True)
ch = 0
for i, e in enumerate(sample, 1):
    try:
        a, b = old.summary(e), new.summary(e)
    except Exception as ex:
        print(f"[{i}/{len(sample)}] 0x{e:08x} ERROR {ex!r}"[:200], flush=True)
        continue
    diff = a["livein"] != b["livein"] or a["N"] != b["N"] or a["P"] != b["P"]
    ch += diff
    print(f"[{i}/{len(sample)}] 0x{e:08x} {'CHANGED' if diff else 'same'}"
          + (f" livein {sorted(a['livein'])} -> {sorted(b['livein'])} N {a['N']} -> {b['N']} P {a['P']} -> {b['P']}"
             if diff else ""), flush=True)
print(f"{len(sample)} functions, {ch} changed", flush=True)
