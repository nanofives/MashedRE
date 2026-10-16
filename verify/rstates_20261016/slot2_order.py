#!/usr/bin/env python3
"""rstates F6: the after-world clump order of every track, read from the original's bytes.

The per-track hook table (0x005f33f8, 13 records of 0x48 bytes, count at 0x005f37a0) keys each
record by Course_Id at +0x10. Its +0x1c slot is the after-world render function (TRAINING
0x00451460, City 0x0044b8f0). This walks that function linearly, following direct `call`s into
the same track's code range (0x00448000..0x0045c000), and lists every RpClumpRender (0x004e6680)
whose argument is a course clump slot `[reg + 0x10110 + 4*i]`, in code order. Anything else that
reaches 0x004e6680 is printed as OTHER so the list is never silently partial.

    py -3.12 slot2_order.py [--json out.json]
"""
import json
import struct
import sys
from pathlib import Path

import capstone

EXE = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
CLUMP_RENDER = 0x004E6680
SLOT_BASE = 0x10110


def load():
    d = EXE.read_bytes()
    e = struct.unpack_from("<I", d, 0x3C)[0]
    n = struct.unpack_from("<H", d, e + 6)[0]
    so = e + 24 + struct.unpack_from("<H", d, e + 20)[0]
    secs = [struct.unpack_from("<8sIIII", d, so + 40 * i) for i in range(n)]

    def off(va):
        r = va - 0x400000
        for _nm, vs, sva, rs, ro in secs:
            if sva <= r < sva + max(vs, rs):
                return ro + r - sva
        raise KeyError(hex(va))
    return d, off


def walk(d, off, fn, depth=0, seen=None):
    """Linear walk of fn until its first ret; returns [(kind, value, at)]."""
    seen = seen if seen is not None else set()
    if fn in seen or depth > 2:
        return []
    seen.add(fn)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    code = d[off(fn):off(fn) + 0x600]
    out, regs = [], {}
    for ins in md.disasm(code, fn):
        ops = ins.op_str
        if ins.mnemonic == "mov" and "dword ptr [" in ops and ops.split(",")[0].strip() in (
                "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp"):
            dst = ops.split(",")[0].strip()
            src = ops.split(",", 1)[1]
            regs[dst] = None
            if "+ 0x101" in src:
                tail = src.split("+ ")[-1].rstrip("] ")
                if "*" in src:
                    regs[dst] = ("INDEXED", src.strip())          # a loop over slots: not a fixed list
                else:
                    v = int(tail, 16)
                    if SLOT_BASE <= v < SLOT_BASE + 0x40 * 4 and (v - SLOT_BASE) % 4 == 0:
                        regs[dst] = (v - SLOT_BASE) // 4
        elif ins.mnemonic == "push" and ops in regs:
            last_push = regs.get(ops)
            regs["__push"] = last_push
        elif ins.mnemonic == "call" and ops.startswith("0x"):
            tgt = int(ops, 16)
            if tgt == CLUMP_RENDER:
                v = regs.get("__push")
                if isinstance(v, int):
                    out.append(("CLUMP", v, ins.address))
                else:
                    out.append(("OTHER", v, ins.address))
            elif 0x00448000 <= tgt < 0x0045C000:
                out.extend(walk(d, off, tgt, depth + 1, seen))
            regs["__push"] = None
        elif ins.mnemonic in ("ret", "jmp") and depth == 0 and ins.mnemonic == "ret":
            break
        elif ins.mnemonic == "ret":
            break
    return out


def main():
    d, off = load()
    cnt = struct.unpack_from("<i", d, off(0x005F37A0))[0]
    res = {}
    for i in range(cnt):
        rec = 0x005F33F8 + i * 0x48
        o = off(rec)
        name = d[o:o + 0x10].split(b"\0")[0].decode()
        course = struct.unpack_from("<i", d, o + 0x10)[0]
        fn = struct.unpack_from("<I", d, o + 0x1C)[0]
        seq = walk(d, off, fn) if fn else []
        res[name] = {"course_id": course, "slot2": hex(fn), "clumps": [v for k, v, _ in seq if k == "CLUMP"],
                     "other": [hex(a) + (f" {v[1]}" if isinstance(v, tuple) else "")
                               for k, v, a in seq if k == "OTHER"],
                     "sites": [hex(a) for _, _, a in seq]}
        print(f"{name:9s} course={course:2d} slot2={fn:#010x} clumps={res[name]['clumps']} "
              f"other={res[name]['other']}")
    if "--json" in sys.argv:
        Path(sys.argv[sys.argv.index("--json") + 1]).write_text(json.dumps(res, indent=1))


if __name__ == "__main__":
    main()
