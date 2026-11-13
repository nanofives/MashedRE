"""Find 'mov rA, [m1]' directly followed by 'cmp rA, [m2]' in the original, map to re/match sources."""
import sys, pathlib, re, bisect
WT = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-RWT-S1B")
sys.path.insert(0, str(WT / "re/tools/match"))
import coffpe, capstone
img = coffpe.Image()
lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
srcs = {}
for p in (WT / "re/match").rglob("*.c"):
    for m in re.finditer(r"//\s*MATCH:\s*0x([0-9a-fA-F]+)", p.read_text(errors="replace")):
        srcs.setdefault(int(m.group(1), 16), []).append(p)
starts = sorted(srcs)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
code = img.read(lo, hi - lo)
prev = None
hits = 0
for ins in md.disasm(code, lo):
    if prev and prev.mnemonic == "mov" and ins.mnemonic == "cmp":
        po = prev.op_str.split(", ", 1); io = ins.op_str.split(", ", 1)
        if len(po) == 2 and len(io) == 2 and po[0] == io[0] and "[" in po[1] and "[" in io[1] and "dword" in io[1]:
            i = bisect.bisect_right(starts, ins.address) - 1
            f = starts[i] if i >= 0 else None
            src = srcs.get(f, [None])[0] if f is not None and ins.address - f < 0x400 else None
            print(f"0x{prev.address:08x} {prev.mnemonic} {prev.op_str} ; {ins.mnemonic} {ins.op_str}  fn=0x{f:08x} {src.relative_to(WT) if src else '-'}")
            hits += 1
    prev = ins
print("hits", hits)
