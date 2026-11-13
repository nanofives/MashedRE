"""Variant sweep for 0x004cb4c0. usage: py -3.12 gen.py <variants.py> [show-best]"""
import pathlib, sys, itertools, re
WT = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-RWT-S1B")
sys.path.insert(0, str(WT / "re/tools/match"))
import match
SRC = (WT / "re/match/s20/004cb4c0.c").read_text()
head, rest = SRC.split("// MATCH: 0x004cb4c0 FUN_004cb4c0\n", 1)
head = re.sub(r"^// FLAGS:.*\n", "", head)
HERE = pathlib.Path(__file__).parent
ns = {}
exec(open(sys.argv[1]).read(), ns)
variants = ns["VARIANTS"]          # name -> full function text (after the MATCH line)
combos = ns.get("COMBOS", [("ddk", "/O2 /Ob1 /G5 /Oy-")])
pre = ns.get("PRE", "")
res = []
for name, body in variants.items():
    for tc, fl in combos:
        p = HERE / f"v_{name}.c"
        p.write_text(f"// TOOLCHAIN {tc}\n" + head + pre + "// MATCH: 0x004cb4c0 FUN_004cb4c0\n" + body)
        try:
            r = match.compare(p, fl, quiet=True, toolchain=tc, bytes_mode=True)[0]
        except SystemExit as e:
            print(f"{name:20} {tc:7} {fl:24} FAIL {str(e)[:200]}"); continue
        print(f"{name:20} {tc:7} {fl:24} {100*r['acc']:5.1f} regs {100*r['regs']:5.1f} ops {100*r['ops']:5.1f} "
              f"{r['n_mine']}/{r['n_orig']} badbytes {len(r['bytes_bad'])}", flush=True)
        res.append((r["acc"], name, tc, fl, p))
res.sort(reverse=True)
if len(sys.argv) > 2 and res:
    acc, name, tc, fl, p = res[0]
    p.write_text(p.read_text())
    match.compare(p, fl, show=True, toolchain=tc, bytes_mode=True)
