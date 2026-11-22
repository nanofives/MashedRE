"""RACE-LOAD-LUA symbol pairing check (run 55).

For each Lua rva the race-load callers reach, compare the game's direct call targets (capstone over the anchored
MASHED.exe.unpatched) with the vendored library function's REL32 relocation targets (lua_4_0_ddk.lib), in order.
A game call target is named through the libmatch table of the same library (verify/luavendor_20261121/
bytecheck_lua_4_0_ddk_lib.tsv, lib_function column). The pairing holds when the two ordered callee lists are equal.

Usage: py -3.12 verify/racelua_20261121/pairing_check.py <rva>:<symbol> [...]
"""
import csv
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import coffpe  # noqa: E402
import capstone  # noqa: E402

LIB = ROOT / "mashedmod" / "deps" / "lua-4.0" / "lua_4_0_ddk.lib"
TABLE = ROOT / "verify" / "luavendor_20261121" / "bytecheck_lua_4_0_ddk_lib.tsv"


def load_lib():
    funcs = {}
    for mname, data in coffpe.archive_members(LIB.read_bytes()):
        if not mname.lower().endswith(".obj"):
            continue
        o = coffpe.Obj(data, mname)
        for f in o.functions():
            funcs.setdefault(f["name"], []).append((o, f))
    return funcs


def lib_callees(o, f):
    sec = o.sections[f["sec"] - 1]
    out = []
    for rva, si, ty in sorted(sec["relocs"]):
        if f["off"] <= rva < f["off"] + f["size"] and ty == coffpe.REL_REL32:
            out.append(o.symbols[si]["name"])
    return out


def game_extent(va, starts):
    """the game function runs to the next function start hooks.csv lists"""
    nxt = min(s for s in starts if s > va)
    return nxt - va


def game_callees(img, va, size, names):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    code = img.read(va, size)
    out = []
    for ins in md.disasm(code, va):
        if ins.mnemonic in ("call", "jmp") and ins.op_str.startswith("0x"):
            t = int(ins.op_str, 16)
            if va <= t < va + size:
                continue  # intra-function branch
            out.append((t, names.get(t, f"?0x{t:08x}")))
    return out


def main():
    names = {}
    with open(TABLE, newline="") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            if r["lib_function"]:
                names[int(r["rva"], 16)] = r["lib_function"]
    rows = {}
    with open(TABLE, newline="") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            rows[int(r["rva"], 16)] = r
    starts = set()
    with open(ROOT / "hooks.csv", newline="", encoding="utf-8", errors="replace") as fh:
        for r in csv.reader(fh):
            if r and len(r[0]) == 8 and not r[0].startswith("#"):
                try:
                    starts.add(int(r[0], 16))
                except ValueError:
                    pass
    img = coffpe.Image()
    funcs = load_lib()
    allok = True
    for arg in sys.argv[1:]:
        rva_s, sym = arg.split(":")
        rva = int(rva_s, 16)
        lsym = "_" + sym
        cands = funcs.get(lsym, [])
        r = rows.get(rva, {})
        print(f"== 0x{rva:08x} -> {sym}: libmatch status {r.get('status')} best {r.get('lib_function')} "
              f"({r.get('note','')[:60]})")
        if len(cands) != 1:
            print(f"   FAIL: {lsym} defined {len(cands)} times in {LIB.name}")
            allok = False
            continue
        o, f = cands[0]
        # game extent: the library size plus slack; branch targets inside it are intra-function
        lc = lib_callees(o, f)
        gsize = game_extent(rva, starts)
        gc = game_callees(img, rva, gsize, names)
        # an unnamed game callee takes the library name when its own callees equal that library function's
        lnames = [n for n in lc]
        if len(gc) == len(lnames):
            fixed = []
            for (t, n), ln in zip(gc, lnames):
                if n.startswith("?") and len(funcs.get(ln, [])) == 1:
                    lo, lf = funcs[ln][0]
                    sub_l = [x.lstrip("_") for x in lib_callees(lo, lf)]
                    sub_g = [x.lstrip("_") for _, x in game_callees(img, t, game_extent(t, starts), names)]
                    if sub_l == sub_g:
                        n = ln + " (by its callees " + ",".join(sub_g) + ")"
                fixed.append((t, n))
            gc = fixed
        gnames = [n.split(" (by")[0] for _, n in gc]
        same = [n.lstrip("_") for n in lc] == [n.lstrip("_") for n in gnames]
        print(f"   lib  {o.name}:{lsym} size {f['size']} callees {lc}")
        print(f"   game extent {gsize} (next hooks.csv start)")
        print(f"   game callees {[f'0x{t:08x}={n}' for t, n in gc]}")
        print(f"   pairing {'HOLDS' if same else 'DIFFERS'}")
        allok &= same
    print("ALL HOLD" if allok else "NOT ALL HOLD")


if __name__ == "__main__":
    main()
