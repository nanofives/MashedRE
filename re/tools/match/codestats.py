"""Code-shape statistics over MASHED.exe game functions (hooks.csv subsystem not third-party-library):
frame-pointer prologues, SEH/EH frames, /GS cookie, inc/dec vs add/sub 1 (P4 /G7 avoids inc/dec), etc.
    py -3.12 re/tools/match/codestats.py"""
import collections, csv, pathlib, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import capstone, match
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = False
game = {}
with open(match.ROOT / "hooks.csv", newline="") as f:
    for r in csv.reader(f):
        if not r or r[0].startswith("#") or r[0] == "rva": continue
        try: va = int(r[0], 16)
        except ValueError: continue
        game[va] = r[2] if len(r) > 2 else ""
data = match.pe().__data__
cookie = (0xBB40E64E).to_bytes(4, "little")
print("cookie 0xBB40E64E present in image:", data.find(cookie) >= 0)
c = collections.Counter(); bysub = collections.Counter()
for va, sub in game.items():
    if sub.startswith("third-party") or sub == "": continue
    n = match.ghidra_size(va)
    if not n: continue
    b = match.original_bytes(va, n)
    c["funcs"] += 1
    if b[:3] == b"\x55\x8b\xec": c["prologue push ebp;mov ebp,esp"] += 1
    if b"\x64\xa1\x00\x00\x00\x00" in b: c["fs:[0] (SEH/EH frame)"] += 1
    if b[:2] == b"\x6a\xff": c["starts push -1 (EH frame)"] += 1
    if b"\x83\xe4\xf8" in b or b"\x83\xe4\xf0" in b: c["and esp,-8/-16 (aligned frame)"] += 1
    for ins in md.disasm(b, va):
        m, o = ins.mnemonic, ins.op_str
        if m in ("inc", "dec"): c[m] += 1
        elif m in ("add", "sub") and o.endswith(", 1") and "ptr" not in o: c[f"{m} r,1"] += 1
        elif m in ("fistp", "fist"): c["fistp"] += 1
        elif m == "call": c["call"] += 1
        elif m.startswith("cmov"): c["cmov"] += 1
        elif m.startswith("fcomi") or m.startswith("fucomi"): c["fcomi"] += 1
        elif m in ("movss", "addss", "mulss"): c["sse scalar"] += 1
        elif m == "fnstsw": c["fnstsw"] += 1
        elif m == "lea" and "+ 1]" in o: c["lea r,[r+1]"] += 1
for k, v in c.most_common(): print(f"{v:8}  {k}")
