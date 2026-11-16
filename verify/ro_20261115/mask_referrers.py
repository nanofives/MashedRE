"""RO-REF-CONTROL-NOISE: list the absolute-operand referrers of one dword, with the hooks.csv subsystem of each referring
function at the current head and at a given git revision. Same scan and attribution as re/tools/gen_replay_masks.py
(lines 72-87: capstone over xref.EXE .text, X86_OP_MEM with base 0, owner() over the Ghidra facts ranges).

py -3.12 verify/ro_20261115/mask_referrers.py 0x00914700 --facts <ghidra_funcfacts.jsonl> [--rev e116c666b]
"""
import argparse, bisect, csv, io, json, pathlib, subprocess, sys

import capstone
import pefile

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import xref  # noqa: E402
import gen_replay_masks as g  # noqa: E402


def subsystems(text):
    fam = {}
    for r in csv.DictReader(l for l in io.StringIO(text) if not l.startswith("#")):
        try:
            fam[int(r["rva"], 16)] = r["subsystem"] or "?"
        except (ValueError, KeyError, TypeError):
            pass
    return fam


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("va")
    ap.add_argument("--facts", default=str(xref.FACTS))
    ap.add_argument("--rev", default="")
    a = ap.parse_args()
    target = int(a.va, 16) & ~3
    cur = subsystems((ROOT / "hooks.csv").read_text(encoding="utf-8"))
    old = subsystems(subprocess.run(["git", "-C", str(ROOT), "show", f"{a.rev}:hooks.csv"], capture_output=True,
                                    text=True, encoding="utf-8").stdout) if a.rev else {}
    funcs = []
    for line in open(a.facts):
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
    print(f"exe {xref.EXE}\nfacts {a.facts}\ntarget dword 0x{target:08x}")
    for ins in md.disasm(text.get_data(), base + text.VirtualAddress):
        if ins.id == 0:
            continue
        for op in ins.operands:
            if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0:
                d = op.mem.disp & 0xffffffff
                if g.LO <= d < g.HI and (d & ~3) == target:
                    f = owner(ins.address)
                    sc = cur.get(f, "no-hooks-row") if f else "no-function"
                    so = (old.get(f, "no-hooks-row") if f else "no-function") if a.rev else "-"
                    print(f"0x{ins.address:08x}  {ins.mnemonic} {ins.op_str:40s} fn={'0x%08x' % f if f else None}  "
                          f"head={sc!r} family={g.bound_name(sc)}  {a.rev or 'rev'}={so!r} family={g.bound_name(so) if a.rev else '-'}")


if __name__ == "__main__":
    main()
