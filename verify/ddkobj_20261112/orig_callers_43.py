"""DDKOBJ-42AD90: original-side direct callers of the 43 DDKOBJ-REGCONV statics. Scans MASHED.exe.unpatched .text for
E8/E9 rel32 to each RVA and names the containing function from hooks.csv (nearest row at or below the site), with its
subsystem. Output TSV: rva, site, kind, caller_rva, caller_name, subsystem.

    py -3.12 verify/ddkobj_20261112/orig_callers_43.py <callers_tsv_in REGCONV_callers.tsv> <out_tsv>
"""
import bisect, csv, struct, sys, pathlib
import pefile
W = pathlib.Path(__file__).resolve().parents[2]
pe = pefile.PE(str(W.parents[1] / "original" / "MASHED.exe.unpatched") if (W.parents[1] / "original").exists()
               else r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
base = pe.OPTIONAL_HEADER.ImageBase
img = pe.get_memory_mapped_image()
t = [s for s in pe.sections if s.Name.startswith(b".text")][0]
lo = t.VirtualAddress
data = img[lo:lo + t.Misc_VirtualSize]
rvas = [int(l.split("\t")[0], 16) for l in open(sys.argv[1]) if l.startswith("0x")]
want = set(rvas)
hooks = {}
for r in csv.DictReader(open(W / "hooks.csv", encoding="utf-8", errors="replace")):
    try:
        hooks[int(r["rva"], 16)] = (r["name"], r["subsystem"])
    except ValueError:
        pass
hk = sorted(hooks)
sites = {r: [] for r in rvas}
for off in range(len(data) - 5):
    op = data[off]
    if op in (0xE8, 0xE9):
        tgt = (base + lo + off + 5 + struct.unpack_from("<i", data, off + 1)[0]) & 0xFFFFFFFF
        if tgt in want:
            s = base + lo + off
            i = bisect.bisect_right(hk, s) - 1
            c = hk[i] if i >= 0 else 0
            sites[tgt].append((f"0x{s:08x}", "call" if op == 0xE8 else "jmp", f"0x{c:08x}") + hooks.get(c, ("?", "?")))
with open(sys.argv[2], "w") as f:
    f.write("rva\tsite\tkind\tcaller_rva\tcaller_name\tsubsystem\n")
    for r in rvas:
        for s in sites[r] or [("", "none", "", "", "")]:
            f.write(f"0x{r:08x}\t" + "\t".join(s) + "\n")
print("statics", len(rvas), "sites", sum(len(v) for v in sites.values()))
