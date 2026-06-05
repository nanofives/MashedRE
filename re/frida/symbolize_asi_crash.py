#!/usr/bin/env python3
"""Symbolize a crash_eip.txt against the mashed_re_dev.asi link map.

The poll_attach_catch_crash.py harness records eip + register values + a raw
stack dump + the loaded-module table. This tool:
  1. finds the mashed_re_dev.asi load range from the crash's module table,
  2. converts eip and every .asi-resident stack return-address to a map RVA,
  3. resolves each to the nearest-preceding public symbol in the .map.

So `asi+0x13efc` becomes a real function name, letting us name the faulting
function and its caller chain without guessing. Requires the .map to match the
.asi that produced the crash (rebuild emits /MAP:build\mashed_re_dev.map).

Usage:  py -3.12 re/frida/symbolize_asi_crash.py [crash_eip.txt] [mashed_re_dev.map]
"""
import json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
CRASH = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "log" / "crash_eip.txt"
MAP   = Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "mashedmod" / "build" / "mashed_re_dev.map"

def load_map(path):
    base = 0x10000000
    syms = []
    with open(path, encoding="utf-8", errors="replace") as f:
        inpub = False
        for ln in f:
            if "Preferred load address is" in ln:
                base = int(ln.split()[-1], 16)
            if "Publics by Value" in ln:
                inpub = True; continue
            if not inpub:
                continue
            m = re.match(r"\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(.*)", ln)
            if m:
                rvabase = int(m.group(2), 16)
                if rvabase >= base:
                    syms.append((rvabase, m.group(1), m.group(3).strip()))
    syms.sort()
    return base, syms

def resolve(syms, base, rva):
    tgt = base + rva
    best = None
    for rb, nm, ob in syms:
        if rb <= tgt: best = (rb, nm, ob)
        else: break
    if best:
        return f"{best[1]} (+0x{tgt-best[0]:x}) [{best[2]}]"
    return "?"

def main():
    d = json.load(open(CRASH))
    mods = {m["name"]: (int(m["base"],16), int(m["end"],16))
            for m in d["modules"] if "base" in m and "end" in m}
    asi = mods.get("mashed_re_dev.asi")
    if not asi:
        sys.exit("mashed_re_dev.asi not in crash module table")
    alo, ahi = asi
    base, syms = load_map(MAP)
    print(f"asi load range: 0x{alo:x}..0x{ahi:x}   map base: 0x{base:x}   {len(syms)} publics")

    eip = int(d["eip"], 16)
    if alo <= eip < ahi:
        print(f"\nFAULT eip 0x{eip:x} = asi+0x{eip-alo:x} -> {resolve(syms, base, eip-alo)}")
    else:
        print(f"\nFAULT eip 0x{eip:x} NOT in .asi (module: "
              + next((n for n,(b,e) in mods.items() if b<=eip<e), '?') + ")")

    print("\n.asi-resident stack return-addresses (caller chain):")
    for s in d["stack"]:
        v = int(s["val"], 16)
        if alo <= v < ahi:
            print(f"  off{s['off']:>4}: asi+0x{v-alo:<6x} -> {resolve(syms, base, v-alo)}")

if __name__ == "__main__":
    main()
