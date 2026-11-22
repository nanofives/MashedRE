"""For each function changed between Lua 4.0 and 4.0.1, compile both trees with the recorded flags and score the
game's Lua-range function starts by mnemonic similarity; print the best start and the two scores side by side."""
import sys, pathlib, tempfile, difflib
TOOLS = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\LUA-VENDOR\re\tools\match")
sys.path.insert(0, str(TOOLS))
import coffpe, libmatch, period_cc, capstone  # noqa

MAIN = period_cc.MAIN
TREES = {"4.0": MAIN / "tools/_download/lua/src_4.0/lua", "4.0.1": MAIN / "tools/_download/lua/src_4.0.1/lua-4.0.1"}
FILES = ["lapi.c", "ldo.c", "lgc.c", "lstring.c", "lvm.c", "lparser.c"]
WANT = ["_lua_newuserdata", "_luaS_newudata", "_protectedparser", "_lua_dofile", "_parse_file", "_luaC_checkGC",
        "_luaC_collectgarbage", "_traceexec", "_retstat", "_luaV_execute", "_statement"]
FL = "/Og /Oi /Ot /Oy /Ob1 /GF /G5"
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
img = coffpe.Image()
rows = libmatch.hooks_rows(["third-party-library[lua-4.0]"])
starts = sorted(r[0] for r in rows)


def mn(data, va):
    return [i.mnemonic + " " + i.op_str.split(",")[0] if i.mnemonic in ("push", "call", "jmp") else i.mnemonic
            for i in md.disasm(data, va)]


objs = {}
for tag, base in TREES.items():
    out = pathlib.Path(tempfile.mkdtemp(prefix="luadisc_"))
    errs = period_cc.compile_all([base / "src" / f for f in FILES], out, FL, [base / "include", base / "src"])
    for e in errs:
        print(e)
    objs[tag] = libmatch.load_objs([out])
gm = {}
for s in starts:
    gm[s] = [i.mnemonic for i in md.disasm(img.read(s, 0x400), s)]
for name in WANT:
    line = [name]
    for tag in TREES:
        fn = next(((o, f) for o in objs[tag] for f in o.functions() if f["name"] == name), None)
        if not fn:
            line.append(f"{tag}: absent")
            continue
        o, f = fn
        body = o.sections[f["sec"] - 1]["data"][f["off"]:f["off"] + f["size"]]
        m = [i.mnemonic for i in md.disasm(body, 0)]
        best = max(((difflib.SequenceMatcher(None, m, gm[s][:len(m) + 8]).ratio(), s) for s in starts))
        ident = libmatch.search(img, o, f)
        line.append(f"{tag}: size {f['size']} best 0x{best[1]:08x} ratio {best[0]:.3f} identical_at {[hex(x) for x in ident]}")
    print(" | ".join(line), flush=True)
