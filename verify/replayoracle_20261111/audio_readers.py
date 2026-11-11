# RO-AUDIO-ISOLATION static census: every .text instruction of MASHED.exe.unpatched whose memory operand has a
# displacement in 0x007dc000..0x007de000 (any base / index, capstone linear sweep), the function containing it (Ghidra
# facts cache log/ghidra_funcfacts.jsonl) and that function's hooks.csv subsystem. Prints, per dword, the subsystems
# that reference it, and lists the dwords referenced by an audio function AND a non-audio function (bridges).
#   py -3.12 verify/replayoracle_20261111/audio_readers.py [--lo 0x007dc000 --hi 0x007de000] [--tsv OUT]
import argparse, bisect, csv, json, sys
from collections import defaultdict
from pathlib import Path
import capstone, pefile
from capstone import x86

HERE = Path(__file__).resolve()
ROOT = HERE.parents[2]
MAIN = next(p for p in (ROOT, *ROOT.parents) if (p / "original" / "MASHED.exe.unpatched").is_file())
ap = argparse.ArgumentParser()
ap.add_argument("--lo", default="0x007dc000"); ap.add_argument("--hi", default="0x007de000"); ap.add_argument("--tsv", default="")
a = ap.parse_args()
LO, HI = int(a.lo, 16), int(a.hi, 16)

funcs = []
for line in (MAIN / "log" / "ghidra_funcfacts.jsonl").read_text().splitlines():
    f = json.loads(line)
    if "entry" not in f: continue
    for lo, hi in f["ranges"]:
        funcs.append((int(lo, 16), int(hi, 16), int(f["entry"], 16)))
funcs.sort(); starts = [x[0] for x in funcs]
def func_of(va):
    i = bisect.bisect_right(starts, va) - 1
    return funcs[i][2] if i >= 0 and funcs[i][0] <= va <= funcs[i][1] else None
sub = {}
with open(ROOT / "hooks.csv", newline="", encoding="utf-8") as fh:
    for r in csv.DictReader(fh):
        try: sub[int(r["rva"], 16)] = r["subsystem"] or "-"
        except ValueError: pass

pe = pefile.PE(str(MAIN / "original" / "MASHED.exe.unpatched"))
txt = [s for s in pe.sections if s.Name.startswith(b".text")][0]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True; md.skipdata = True
refs = defaultdict(list)   # dword va -> [(insn va, func, subsystem, mnemonic)]
for ins in md.disasm(txt.get_data(), pe.OPTIONAL_HEADER.ImageBase + txt.VirtualAddress):
    if ins.id == 0: continue
    for op in ins.operands:
        if op.type == x86.X86_OP_MEM:
            d = op.mem.disp & 0xffffffff
            if LO <= d < HI:
                fn = func_of(ins.address)
                refs[d & ~3].append((ins.address, fn, sub.get(fn, "no-hooks-row") if fn else "no-function", ins.mnemonic))
rows = []
for va in sorted(refs):
    subs = sorted({s for _, _, s, _ in refs[va]})
    rows.append((va, subs, refs[va]))
bridges = [r for r in rows if "audio" in r[1] and any(s != "audio" for s in r[1])]
print(f"{len(rows)} dwords in 0x{LO:08x}..0x{HI:08x} referenced by displacement; {sum(len(r[2]) for r in rows)} instructions")
cnt = defaultdict(int)
for _, subs, _ in rows: cnt[" + ".join(subs)] += 1
for k, v in sorted(cnt.items(), key=lambda x: -x[1]): print(f"  {v:4d}  {k}")
print(f"bridges (audio and non-audio referencers): {len(bridges)}")
for va, subs, rr in bridges:
    print(f"  0x{va:08x}  {subs}")
    for ia, fn, s, m in rr:
        if s != "audio": print(f"      0x{ia:08x} {m:6s} in FUN_{fn or 0:08x} [{s}]")
if a.tsv:
    with open(a.tsv, "w", encoding="utf-8") as fh:
        fh.write("va\tsubsystems\tinsns\n")
        for va, subs, rr in rows:
            fh.write(f"0x{va:08x}\t{','.join(subs)}\t{' '.join(f'0x{ia:08x}' for ia, *_ in rr)}\n")
