"""PROV-DIVERGED-18 (2026-11-11): order of the absolute x87 memory operands, DDK object against the original.

The defect class behind four of the PROV-INSITU DIVERGED bodies (0x00448220, 0x0046e9e0, 0x00468d80, and the
constant half of 0x00479100's neighbours): cl 13.00.9176 /O2 reassociates a float product or sum written as one
expression, so the constants and globals are multiplied in another order and the x87 result rounds differently.
match.py does not see it (it compares instructions with relocated fields blanked), relocheck.py only sees it when the
code before it has the same length (it pairs fields by offset).

This lists, in code order, every x87 instruction with an absolute memory operand, as the VALUE read (the dword or
qword at that address in MASHED.exe.unpatched for the original; the __real@ literal or the original's word at a
DAT_ address for ours), and reports where the two sequences differ (difflib opcodes). A difference is a lead, not a
verdict: the two bodies may legitimately order independent statements differently.

    py -3.12 re/tools/prov/x87_operand_order.py re/match/s11/0046e9e0.c [...]
"""
import difflib, os, pathlib, re, struct, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match  # noqa: E402
import equiv  # noqa: E402
import capstone  # noqa: E402
from capstone import x86 as X  # noqa: E402

MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True


def listing(src, flags):
    tc = match.TOOLCHAINS[match.DEFAULT_TOOLCHAIN]
    tmp = pathlib.Path(tempfile.mkdtemp(prefix="x87o_"))
    env = {"PATH": str(tc["bin"]), "INCLUDE": "", "LIB": "", "SystemRoot": os.environ.get("SystemRoot", r"C:\Windows"),
           "TEMP": str(tmp), "TMP": str(tmp)}
    asm = tmp / "a.asm"
    cmd = [str(tc["bin"] / "cl.exe"), "/nologo", "/c", "/Gy", *tc["extra"], *flags.split(), "/Fo" + str(tmp / "a.obj"),
           "/Fa" + str(asm), "/FAs", "/Fd" + str(tmp) + os.sep, "/Tc" + str(src)]
    subprocess.run(cmd, capture_output=True, text=True, env=env)
    return asm.read_text(errors="replace")


def word(addr, size):
    b = equiv.image().bytes(addr, size)
    return None if b is None else int.from_bytes(b, "little")


def ours(src, fn):
    txt = match.read_source(src) if hasattr(match, "read_source") else src.read_text(errors="replace")
    flags = "/O2 /GS /Ob1"
    m = re.search(r"//\s*FLAGS:\s*(.*)", txt)
    if m:
        flags = m.group(1).strip()
    asm = listing(src, flags)
    body = re.search(rf"^{re.escape(fn)}\s+PROC NEAR(.*?)^{re.escape(fn)}\s+ENDP", asm, re.S | re.M)
    if not body:
        raise SystemExit(f"{src}: {fn} not in the listing")
    seq = []
    for ln in body.group(1).splitlines():
        ln = ln.split(";")[0].strip()
        mm = re.match(r"(f\w+)\s+(DWORD|QWORD) PTR (_DAT_([0-9a-f]{8})|__real@([0-9a-f]+))(?:\+(\d+))?\b(?!\$)", ln)
        if not mm or "[" in ln.split("PTR", 1)[1].split(mm.group(3))[0]:
            continue
        size = 4 if mm.group(2) == "DWORD" else 8
        if mm.group(4):
            v = word(int(mm.group(4), 16) + int(mm.group(6) or 0), size)
            tag = f"0x{mm.group(4)}"
        else:
            v = int(mm.group(5), 16)
            tag = "lit"
        seq.append((mm.group(1), size, v, tag))
    return seq


def orig(rva):
    size = match.function_table().get(rva) or match.ghidra_size(rva)
    code = equiv.image().bytes(rva, size)
    seq = []
    for c in MD.disasm(code, rva):
        if not c.mnemonic.startswith("f"):
            continue
        for op in c.operands:
            if op.type == X.X86_OP_MEM and not op.mem.base and not op.mem.index and op.mem.disp:
                a = op.mem.disp & 0xffffffff
                seq.append((c.mnemonic, op.size, word(a, op.size), f"{a:#010x}@{c.address:#010x}"))
    return seq


def main():
    for s in sys.argv[1:]:
        src = pathlib.Path(s)
        rva = int(re.search(r"([0-9a-f]{8})\.c$", s).group(1), 16)
        a, b = ours(src, f"_FUN_{rva:08x}"), orig(rva)
        ka = [(m, v) for m, _, v, _ in a]
        kb = [(m, v) for m, _, v, _ in b]
        ops = [o for o in difflib.SequenceMatcher(a=ka, b=kb, autojunk=False).get_opcodes() if o[0] != "equal"]
        print(f"{rva:#010x}  ours {len(a)}  orig {len(b)}  differing runs {len(ops)}")
        for tag, i1, i2, j1, j2 in ops[:12]:
            fa = " ".join(f"{m}:{v:08x}({t})" for m, _, v, t in a[i1:i2]) or "-"
            fb = " ".join(f"{m}:{v:08x}({t})" for m, _, v, t in b[j1:j2]) or "-"
            print(f"   {tag:7s} ours {fa}\n           orig {fb}")


if __name__ == "__main__":
    main()
