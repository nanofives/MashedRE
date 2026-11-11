"""Which code family references each dword of MASHED.exe's writable image (REPLAY-ORACLE-DESIGN, 2026-11-11).

Every .text instruction with an absolute memory operand (base register none; an index register allowed, the
displacement is then the array base) inside 0x005ea000..0x00915008 is attributed to its containing function
(Ghidra facts, re/tools/xref.py cache), and the function to its hooks.csv subsystem. A family is the subsystem
with the bracketed library tag kept (third-party-library[renderware], [RenderWare-Physics-3.7], [msvc-crt-tail]
...). Output: per 4 KB page of the writable image, the families that reference it and how many distinct
addresses each references; then per family the page count. Static and incomplete by construction: a global
reached only through a pointer register (struct base in a register) is not seen.

    py -3.12 verify/replayoracle_20261111/data_owner_map.py > verify/replayoracle_20261111/data_owner_map.tsv
"""
import bisect, csv, json, pathlib, sys
from collections import defaultdict

import capstone
import pefile

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import xref  # noqa: E402

LO, HI = 0x005ea000, 0x00915008


def main():
    fam = {}
    for r in csv.DictReader(l for l in open(ROOT / "hooks.csv", encoding="utf-8") if not l.startswith("#")):
        try:
            fam[int(r["rva"], 16)] = r["subsystem"] or "?"
        except (ValueError, KeyError, TypeError):
            pass
    funcs = []
    for line in open(xref.FACTS):
        r = json.loads(line)
        if "entry" in r:
            for lo, hi in r["ranges"]:
                funcs.append((int(lo, 16), int(hi, 16), int(r["entry"], 16)))
    funcs.sort()
    starts = [f[0] for f in funcs]

    def owner(va):
        i = bisect.bisect_right(starts, va) - 1
        while i >= 0:
            if funcs[i][0] <= va <= funcs[i][1]:
                return funcs[i][2]
            if va - funcs[i][0] > 0x40000:
                break
            i -= 1
        return None

    pe = pefile.PE(str(xref.EXE))
    base = pe.OPTIONAL_HEADER.ImageBase
    text = next(s for s in pe.sections if s.Name.rstrip(b"\0") == b".text")
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    md.skipdata = True
    page = defaultdict(lambda: defaultdict(set))   # page -> family -> {addr}
    for ins in md.disasm(text.get_data(), base + text.VirtualAddress):
        if ins.id == 0:
            continue
        for op in ins.operands:
            if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0:
                a = op.mem.disp & 0xffffffff
                if LO <= a < HI:
                    f = owner(ins.address)
                    page[a & ~0xfff][fam.get(f, "no-hooks-row") if f else "no-function"].add(a)
    allfam = defaultdict(int)
    print("page\tfamilies (distinct referenced addresses)")
    for p in range(LO & ~0xfff, HI, 0x1000):
        d = page.get(p)
        if not d:
            print(f"0x{p:08x}\t(none: no absolute reference)")
            continue
        for k in d:
            allfam[k] += 1
        print(f"0x{p:08x}\t" + "; ".join(f"{k} {len(v)}" for k, v in sorted(d.items(), key=lambda kv: -len(kv[1]))))
    print("\n# family\tpages referenced")
    for k, v in sorted(allfam.items(), key=lambda kv: -kv[1]):
        print(f"{k}\t{v}")


if __name__ == "__main__":
    main()
