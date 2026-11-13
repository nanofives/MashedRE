"""RWC-THUNK-BIND-AUDIT run 44: call-target check for the rebound RwCore bodies that ddkobj does not byte-check.

    py -3.12 verify/rwcthunk_20261113/callcheck.py <exe> <map> [<out.tsv>]

ddkobj skips provisional units (ddkobj.py:712-713, L-126 (6a)), so 0x004c5f60, 0x004cd900 and 0x004c32b0 have no
BYTECHECK row. This script checks the one thing the THUNK_BIND change alters in them: the direct call targets.
For every STUB row of THUNK_AUDIT.tsv whose source is compiled as RwCore_<own>, it disassembles
  - the original at each call site (original/MASHED.exe.unpatched, E8 at the cited address): the target RVA, and
  - the exe body _RwCore_<own> (map symbol to the next symbol): every E8 target, named from the map.
Row result: OK when the exe body calls _RwCore_<stub> exactly as many times as the original has sites, and never
_RwCore_<t> (the thunk's own target). Otherwise MISMATCH.
"""
import bisect
import csv
import re
import sys
from pathlib import Path

import capstone
import pefile

HERE = Path(__file__).resolve().parent
ORIG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")


def read_map(p):
    syms = {}
    for l in open(p, errors="replace"):
        m = re.match(r"\s*0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s", l)
        if m:
            syms[m.group(1)] = int(m.group(2), 16)
    return syms


def main():
    exe, mp = sys.argv[1], sys.argv[2]
    syms = read_map(mp)
    by_va = sorted((v, s) for s, v in syms.items())
    vas = [v for v, _ in by_va]
    pe = pefile.PE(exe, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    ope = pefile.PE(str(ORIG), fast_load=True)
    obase = ope.OPTIONAL_HEADER.ImageBase
    oimg = ope.get_memory_mapped_image()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    gen = HERE.parents[1] / "mashedmod" / "src" / "mashed_re" / "RwCore"
    rows = []
    for r in csv.DictReader(open(HERE / "THUNK_AUDIT.tsv"), delimiter="\t"):
        if r["verdict"] != "STUB":
            continue
        own = Path(r["source"]).stem
        sym = f"_RwCore_{own}"
        if sym not in syms or not (gen / f"RwCore_{own}.cpp").exists():
            continue
        t = r["name"].split("_")[-1]
        stub = r["stub_rva"]
        sites = [s.split("@")[1] for s in r["sites(own@callsite)"].split(";") if s.startswith(own + "@")]
        otgts = []
        for s in sites:
            a = int(s, 16)
            b = oimg[a - obase:a - obase + 5]
            otgts.append(f"{(a + 5 + int.from_bytes(b[1:5], 'little', signed=True)) & 0xffffffff:08x}" if b[0] == 0xE8 else "not-E8")
        va = syms[sym]
        i = bisect.bisect_right(vas, va)
        end = vas[i] if i < len(vas) else va + 0x1000
        code = img[va - base:end - base]
        calls = []
        for ins in md.disasm(bytes(code), va):
            if ins.mnemonic == "call" and ins.bytes[0] == 0xE8:
                tgt = int(ins.op_str, 16)
                j = bisect.bisect_right(vas, tgt) - 1
                calls.append(by_va[j][1] if j >= 0 and by_va[j][0] == tgt else f"0x{tgt:08x}")
        n_stub = calls.count(f"_RwCore_{stub}")
        n_t = calls.count(f"_RwCore_{t}")
        ok = all(o == stub for o in otgts) and n_stub == len(sites) and n_t == 0
        rows.append({"rva": f"0x{own}", "exe_symbol": sym, "exe_va": f"0x{va:08x}", "thunk": r["name"], "stub": f"0x{stub}",
                     "orig_sites": ";".join(sites), "orig_targets": ";".join(otgts),
                     "exe_calls_stub": n_stub, "exe_calls_thunk_target": n_t, "result": "OK" if ok else "MISMATCH"})
    w = sys.stdout if len(sys.argv) < 4 else open(sys.argv[3], "w", newline="", encoding="utf-8")
    cw = csv.DictWriter(w, fieldnames=list(rows[0].keys()), delimiter="\t", lineterminator="\n")
    cw.writeheader()
    cw.writerows(rows)
    if w is not sys.stdout:
        w.close()
    print(f"[callcheck] {exe}: {sum(r['result'] == 'OK' for r in rows)}/{len(rows)} OK", file=sys.stderr)


if __name__ == "__main__":
    main()
