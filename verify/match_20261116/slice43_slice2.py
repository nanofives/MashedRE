"""Score slice 2 of 0x0043dfd0: original instructions in [LO, HI) matched by match.py's alignment.
usage: py -3.12 slice43.py <src> [--dump <file>]"""
import sys, pathlib, capstone
sys.path.insert(0, str(pathlib.Path.cwd() / "re" / "tools" / "match"))
import match as M

LO, HI = 0x0043e8cc, 0x0043f775
src = pathlib.Path(sys.argv[1])
text = src.read_text()
flags = M.file_flags(text)
obj = M.compile_obj(src, flags, M.file_lang(text), M.file_toolchain(text))
secs, syms = M.coff(obj)
acc, regs, ops, a, b, full, sm = M.score_function(0x0043dfd0, secs, syms, "_FUN_0043dfd0")
# original addresses for b
p = M.pe()
orig = M.original_bytes(0x43dfd0, M.ghidra_size(0x43dfd0))
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
addrs = [i.address for i in md.disasm(orig, 0x43dfd0)][:len(b)]
inslice = [k for k, ad in enumerate(addrs) if LO <= ad < HI]
k0, k1 = inslice[0], inslice[-1] + 1
matched = set()
for bl in sm.get_matching_blocks():
    for j in range(bl.size):
        matched.add(bl.b + j)
n = k1 - k0
m = sum(1 for k in range(k0, k1) if k in matched)
print(f"whole {acc*100:.1f}% regs {regs*100:.1f}% ops {ops*100:.1f}%  slice2 {m}/{n} = {m/n*100:.1f}% orig idx {k0}..{k1}")
if len(sys.argv) > 3 and sys.argv[2] == "--dump":
    with open(sys.argv[3], "w") as f:
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if j2 <= k0 or j1 >= k1:
                continue
            f.write(f"== {tag} mine[{i1}:{i2}] orig[{j1}:{j2}] @{addrs[j1] if j1 < len(addrs) else 0:08x}\n")
            if tag != "equal":
                for x in a[i1:i2]:
                    f.write(f"  M {x[0]}\n")
                for jj in range(j1, j2):
                    f.write(f"  O {addrs[jj]:08x} {b[jj][0]}\n")
