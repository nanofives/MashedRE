# PROV-INSITU-REF1 NOT-WIRED leg: direct call targets of each NOT-WIRED function in the ORIGINAL bytes
# (MASHED.exe.unpatched, capstone over the Ghidra extent), so a symbol resolved from a libid row can be checked against
# the address the original body actually calls.   py -3.12 notwired_calls.py
import sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match as M  # noqa: E402
import capstone

RVAS = ["0x005b8c40", "0x00564310", "0x005c2f84", "0x0055b080", "0x004e9e40", "0x004e99b0", "0x005185a0",
        "0x00574920", "0x00574a20", "0x00487280"]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for r in RVAS:
    va = int(r, 16)
    size = M.ghidra_size(va)
    code = M.original_bytes(va, size)
    if code is None:
        raise SystemExit("match.orig_bytes missing; see match.py for the original-bytes reader")
    tg = []
    for ins in md.disasm(code, va):
        if ins.mnemonic == "call":
            tg.append(f"{ins.address:#010x}->{ins.op_str}")
    print(r, size, " ".join(tg))
