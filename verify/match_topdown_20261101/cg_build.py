"""MATCH-TOPDOWN-MAP, step 1: build the two static call graphs this map is computed from.

Static only (no game, no Ghidra). Writes one JSON to the path given as argv[1].

Original side (MASHED.exe.unpatched, the SHA-256 anchor):
  nodes  every hooks.csv RVA (5969 rows, the project's function list), range = [rva, next rva)
  edges  'call'  E8 rel32 to a node start
         'tail'  E9 rel32 / Jcc rel32 to a node start outside the caller's own range
         'ptr'   imm32 operand (push/mov/cmp...) equal to a node start (function-pointer argument)
         'tab'   memory operand with an index register and a disp32 in a data section: the dwords at
                 disp32 that are node starts, read until the first one that is not (dispatch table)
         'imp'   call/jmp [IAT slot] -> "dll!name"
  Linear capstone sweep per node; an undecodable byte advances one byte. Indirect calls through
  registers or vtables held in heap objects are NOT edges (stated as a limit in the RESULT).

Port side (mashed_re.exe built from orch/integration 0e3d71a75, its /MAP):
  nodes  every map symbol (public + static), range = [addr, next symbol addr)
  edges  E8/E9 rel32 from code symbols, plus every .reloc site: containing symbol -> target symbol
  reach  BFS from _WinMain@16. The CRT initializer table (.CRT$XCU: the s_AutoReg_* registrations)
         is not a root, so an RVA reached only through the hook registry is not "called".
         Two reach sets: 'reach' (all edges) and 'reach_nothunk' (no edge out of
         StandaloneRvaThunks.obj, the by-address thunk table; MATCH-S01 did not count those).
"""
import bisect
import csv
import json
import re
import struct
import sys
from pathlib import Path

import capstone
import pefile
from capstone import x86

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
ORIG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
EXE = Path(sys.argv[2]) if len(sys.argv) > 2 else None
MAP = Path(sys.argv[3]) if len(sys.argv) > 3 else None


def norm(s):
    n = int(s, 16)
    return n + 0x400000 if n < 0x400000 else n


def pe_sections(pe):
    base = pe.OPTIONAL_HEADER.ImageBase
    out = []
    for s in pe.sections:
        nm = s.Name.rstrip(b"\0").decode(errors="replace")
        out.append((nm, base + s.VirtualAddress, base + s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData), s))
    return base, out


def imports(pe):
    out = {}
    if hasattr(pe, "DIRECTORY_ENTRY_IMPORT"):
        for e in pe.DIRECTORY_ENTRY_IMPORT:
            dll = e.dll.decode().lower()
            for i in e.imports:
                nm = i.name.decode() if i.name else f"#{i.ordinal}"
                out[i.address] = f"{dll}!{nm}"
    return out


def build_orig():
    rows = [x for x in csv.DictReader(open(WT / "hooks.csv", encoding="utf-8")) if not x["rva"].startswith("#")]
    meta = {}
    for x in rows:
        meta.setdefault(norm(x["rva"]), x)
    pe = pefile.PE(str(ORIG))
    base, secs = pe_sections(pe)
    text = [s for s in secs if s[0] == ".text"][0]
    data_secs = [s for s in secs if s[0] in (".rdata", ".data")]
    imp = imports(pe)
    mem = pe.get_memory_mapped_image()
    starts = sorted(a for a in meta if text[1] <= a < text[2])
    sset = set(starts)

    def rd(va, n):
        o = va - base
        return mem[o:o + n]

    def in_data(va):
        return any(lo <= va < hi for _n, lo, hi, _s in data_secs)

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    edges = {}
    for i, a in enumerate(starts):
        end = starts[i + 1] if i + 1 < len(starts) else text[2]
        end = min(end, a + 0x10000)
        code = rd(a, end - a)
        e = {}
        off = 0
        while off < len(code):
            got = False
            for ins in md.disasm(code[off:], a + off):
                got = True
                off = ins.address + ins.size - a
                mn = ins.mnemonic
                for op in ins.operands:
                    if op.type == x86.X86_OP_IMM:
                        t = op.imm & 0xFFFFFFFF
                        if mn == "call":
                            if t in sset and t != a:
                                e.setdefault(t, "call")
                        elif mn == "jmp" or mn.startswith("j"):
                            if t in sset and not (a <= t < end):
                                e.setdefault(t, "tail")
                        elif t in sset and t != a:
                            e.setdefault(t, "ptr")
                    elif op.type == x86.X86_OP_MEM:
                        d = op.mem.disp & 0xFFFFFFFF
                        if op.mem.base == 0 and op.mem.index == 0 and d in imp and mn in ("call", "jmp"):
                            e.setdefault(imp[d], "imp")
                        elif op.mem.index != 0 and in_data(d):
                            k = 0
                            while k < 512:
                                b = rd(d + 4 * k, 4)
                                if len(b) < 4:
                                    break
                                t = struct.unpack("<I", b)[0]
                                if t not in sset:
                                    break
                                if t != a:
                                    e.setdefault(t, "tab")
                                k += 1
                if off >= len(code):
                    break
            if not got:
                off += 1
        edges[a] = e
    nodes = {}
    for a in starts:
        x = meta[a]
        nodes[f"0x{a:08x}"] = {"name": x["name"], "sub": x["subsystem"], "conf": x["confidence"],
                               "file": x["file"], "exe_file": x.get("exe_file") or ""}
    return nodes, {f"0x{a:08x}": {(k if isinstance(k, str) else f"0x{k:08x}"): v for k, v in e.items()}
                   for a, e in edges.items()}


RE_MAPSYM = re.compile(r"^\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s+(f\s+)?(?:i\s+)?(\S+)\s*$")


def undecorate(sym):
    s = sym
    if s.startswith("?"):
        s = s[1:]
        if s.startswith("?"):
            return None  # operators / special names
        return s.split("@")[0]
    if s.startswith("_") or s.startswith("@"):
        s = s[1:]
    return s.split("@")[0]


def build_exe():
    syms = []
    for ln in open(MAP, encoding="utf-8", errors="replace"):
        m = RE_MAPSYM.match(ln)
        if not m:
            continue
        va = int(m.group(4), 16)
        if va == 0:
            continue
        syms.append((va, m.group(3), bool(m.group(5)), m.group(6)))
    syms.sort()
    # dedupe same address (COMDAT folding keeps several names on one body)
    addrs = sorted({s[0] for s in syms})
    by_addr = {}
    for s in syms:
        by_addr.setdefault(s[0], []).append(s)
    pe = pefile.PE(str(EXE))
    base, secs = pe_sections(pe)
    text = [s for s in secs if s[0] == ".text"][0]
    mem = pe.get_memory_mapped_image()

    def owner(va):
        i = bisect.bisect_right(addrs, va) - 1
        return addrs[i] if i >= 0 else None

    edges = {a: set() for a in addrs}
    # relocations
    if hasattr(pe, "DIRECTORY_ENTRY_BASERELOC"):
        for blk in pe.DIRECTORY_ENTRY_BASERELOC:
            for ent in blk.entries:
                if ent.type != 3:
                    continue
                site = base + ent.rva
                tgt = struct.unpack_from("<I", mem, ent.rva)[0]
                s, t = owner(site), owner(tgt)
                if s is not None and t is not None and s != t:
                    edges[s].add(t)
    # rel32 calls/jmps in code
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    code_addrs = [a for a in addrs if text[1] <= a < text[2]]
    for i, a in enumerate(code_addrs):
        end = code_addrs[i + 1] if i + 1 < len(code_addrs) else text[2]
        blob = mem[a - base:end - base]
        off = 0
        while off < len(blob):
            got = False
            for ins in md.disasm(blob[off:], a + off):
                got = True
                off = ins.address + ins.size - a
                if ins.size == 5 and ins.bytes[0] in (0xE8, 0xE9):
                    t = (ins.address + 5 + struct.unpack("<i", bytes(ins.bytes[1:5]))[0]) & 0xFFFFFFFF
                    o = owner(t)
                    if o is not None and o != a:
                        edges[a].add(o)
                if off >= len(blob):
                    break
            if not got:
                off += 1
    names = {a: [s[1] for s in by_addr[a]] for a in addrs}
    objs = {a: by_addr[a][0][3] for a in addrs}
    root = [a for a in addrs if "_WinMain@16" in names[a]][0]

    def bfs(skip_obj):
        seen = {root}
        st = [root]
        while st:
            a = st.pop()
            if skip_obj and skip_obj in objs[a] and a != root:
                continue
            for t in edges[a]:
                if t not in seen:
                    seen.add(t)
                    st.append(t)
        return seen

    reach = bfs(None)
    reach_nt = bfs("StandaloneRvaThunks")
    out = []
    for a in addrs:
        if not (text[1] <= a < text[2]):
            continue
        out.append({"va": f"0x{a:08x}", "names": names[a], "obj": objs[a],
                    "reach": a in reach, "reach_nothunk": a in reach_nt})
    return out, {f"0x{a:08x}": sorted(f"0x{t:08x}" for t in edges[a]) for a in code_addrs}


def main():
    res = {}
    nodes, oedges = build_orig()
    res["orig_nodes"] = nodes
    res["orig_edges"] = oedges
    if EXE:
        syms, xedges = build_exe()
        res["exe_syms"] = syms
        res["exe_edges"] = xedges
    json.dump(res, open(sys.argv[1], "w"), indent=0)
    print("orig nodes", len(nodes), "edges", sum(len(e) for e in oedges.values()))
    if EXE:
        print("exe code syms", len(syms), "reach", sum(s["reach"] for s in syms),
              "reach_nothunk", sum(s["reach_nothunk"] for s in syms))


if __name__ == "__main__":
    main()
