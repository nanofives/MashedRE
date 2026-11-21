# Static caller scan for the L-180 (b) own-argument-slot rule (row WS-OWNSLOT-CALLERSCAN, run 55).
#
# For a body that stores into its own incoming parameter slots (entry esp + k), the store counts as
# frame for the write-set leg only when no caller reads that slot after the call returns.
# This tool, on the anchored original/MASHED.exe.unpatched:
#   1. finds every direct CALL/JMP rel32 to the body (each hit confirmed by capstone at that address and
#      by a linear disassembly of the containing function from re/analysis/function_inventory.csv),
#   2. finds every absolute-dword occurrence of the body's VA in the image (address taken = indirect or
#      unknown caller -> the slot stays caller stack),
#   3. for each direct call site, walks the caller forward from the return address over the control-flow
#      graph, tracking esp relative to esp at the call, and reports the first read of any slot byte while
#      it is still at or above esp. A path ends clean when every slot byte is below esp, or overwritten,
#      or at ret. Anything the walk cannot resolve (indirect jmp, esp copied, address of the slot taken,
#      a call made while slot bytes are still above esp, unknown callee stack cleanup) is UNRESOLVED,
#      which counts as "caller may read" (fails the leg).
#   A `pop r32` over a slot byte is a read; it is reported as POPREAD with the popped register's liveness
#   on that path (dead = written before any read on every path walked).
#
# Usage: py -3.12 verify/wsownslot_20261121/ownslot_callerscan.py <out.json>
import csv
import json
import struct
import sys
from pathlib import Path

import capstone
from capstone import x86

ROOT = Path(__file__).resolve().parents[2]
EXE = Path(r"C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched")
INV = ROOT / "re" / "analysis" / "function_inventory.csv"

# rva -> list of (entry-esp offset, size) from esc_detail_ref1groups.json (PROV-INSITU-REF1-WS2)
BODIES = {
    0x00463640: [(0x4, 4)],
    0x00420290: [(0x4, 4)],
    0x00420870: [(0x4, 4)],
    0x00407e20: [(0x10, 4), (0x14, 4)],
    0x00470670: [(0xC, 4)],
    0x0047BB10: [(0x4, 4)],
    0x00450B10: [(0x1C, 4)],
    0x005C2300: [(0xC, 4)],
    0x0046B540: [(0x4, 4)],
    0x00441B30: [(0x8, 4)],
    0x0041A400: [(0x4, 4)],
}

data = EXE.read_bytes()
pe = struct.unpack_from("<I", data, 0x3C)[0]
nsec = struct.unpack_from("<H", data, pe + 6)[0]
opt = struct.unpack_from("<H", data, pe + 20)[0]
BASE = struct.unpack_from("<I", data, pe + 24 + 28)[0]
SECS = []
for i in range(nsec):
    s = pe + 24 + opt + i * 40
    name = data[s:s + 8].rstrip(b"\0").decode()
    vsz, va, rsz, raw = struct.unpack_from("<IIII", data, s + 8)
    SECS.append((name, BASE + va, max(vsz, rsz), raw, rsz))


def va2off(v):
    for n, va, sz, raw, rsz in SECS:
        if va <= v < va + rsz:
            return raw + v - va
    return None


def off2va(o):
    for n, va, sz, raw, rsz in SECS:
        if raw <= o < raw + rsz:
            return va + o - raw, n
    return None, None


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True

FUNCS = []
with open(INV, newline="") as f:
    for r in csv.DictReader(f):
        FUNCS.append((int(r["rva"], 16), int(r["body_size"]), r["name"]))
# Two caller bodies missing from function_inventory.csv (call sites 0x0040ff8c and 0x00428888 fell outside
# every inventoried body). Starts: 0x0040feb0 follows the jump table of FUN_0040fc00 (0x0040fe70..0x0040fead,
# then 4x 0x90); 0x00428790 is a direct CALL target in .text. Each ends at the next inventoried start.
FUNCS.append((0x0040FEB0, 0x004102F0 - 0x0040FEB0, "FUN_0040feb0 (not in inventory)"))
FUNCS.append((0x00428790, 0x004288A0 - 0x00428790, "FUN_00428790 (not in inventory)"))
FUNCS.sort()
# Inventory body_size can stop short of the last instructions (FUN_0046baa0: size 569 ends at 0x0046bcd9,
# its `pop esi; ret` sit at 0x0046bcda/0x0046bcdb). Bound every body by the next inventoried start instead.
FUNCS = [(s, (FUNCS[k + 1][0] - s) if k + 1 < len(FUNCS) else n, nm) for k, (s, n, nm) in enumerate(FUNCS)]


def func_of(v):
    lo, hi = 0, len(FUNCS)
    while lo < hi:
        m = (lo + hi) // 2
        if FUNCS[m][0] <= v:
            lo = m + 1
        else:
            hi = m
    if lo == 0:
        return None
    s, n, name = FUNCS[lo - 1]
    return (s, n, name) if s <= v < s + n else None


_insn_cache = {}


def insn_at(v):
    if v in _insn_cache:
        return _insn_cache[v]
    o = va2off(v)
    i = next(md.disasm(data[o:o + 16], v), None) if o is not None else None
    _insn_cache[v] = i
    return i


def linear_addrs(start, size):
    o = va2off(start)
    out = set()
    for i in md.disasm(data[o:o + size], start):
        out.add(i.address)
    return out


_ret_cache = {}


def callee_pop(target):
    """bytes the callee pops (ret imm16) if every ret in its linear body agrees, else None."""
    if target in _ret_cache:
        return _ret_cache[target]
    fo = func_of(target)
    res = None
    if fo and fo[0] == target:
        o = va2off(target)
        rets = set()
        for i in md.disasm(data[o:o + fo[1]], target):
            if i.id == x86.X86_INS_RET:
                rets.add(i.operands[0].imm if i.operands else 0)
        if len(rets) == 1:
            res = rets.pop()
    _ret_cache[target] = res
    return res


def ebp_minus_esp_at(func_start, site):
    """esp-at-site minus ebp (bytes), from a CFG depth walk of the caller. None if no frame or inconsistent."""
    # depth = entry_esp - esp (positive grows downward). ebp set by `mov ebp, esp` after which ebp depth fixed.
    work = [(func_start, 0, None)]
    seen = {}
    fo = func_of(func_start)
    end = fo[0] + fo[1]
    found = set()
    steps = 0
    while work and steps < 200000:
        a, dep, ebpd = work.pop()
        while True:
            steps += 1
            if not (func_start <= a < end):
                break
            key = a
            if key in seen:
                if seen[key] != (dep, ebpd):
                    return None
                break
            seen[key] = (dep, ebpd)
            i = insn_at(a)
            if i is None:
                break
            if a == site:
                if ebpd is None:
                    return None
                found.add(dep - ebpd)
            nd = dep
            iid = i.id
            ops = i.operands
            if iid == x86.X86_INS_PUSH:
                nd += 4
            elif iid == x86.X86_INS_POP:
                nd -= 4
                if ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_EBP:
                    ebpd = None
            elif iid in (x86.X86_INS_SUB, x86.X86_INS_ADD) and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP:
                if ops[1].type != x86.X86_OP_IMM:
                    return None
                nd += ops[1].imm if iid == x86.X86_INS_SUB else -ops[1].imm
            elif iid == x86.X86_INS_MOV and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_EBP \
                    and ops[1].type == x86.X86_OP_REG and ops[1].reg == x86.X86_REG_ESP:
                ebpd = dep
            elif iid == x86.X86_INS_MOV and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP:
                if ebpd is None:
                    return None
                nd = ebpd
            elif iid == x86.X86_INS_LEAVE:
                if ebpd is None:
                    return None
                nd = ebpd - 4
                ebpd = None
            elif iid == x86.X86_INS_CALL:
                if ops[0].type == x86.X86_OP_IMM:
                    p = callee_pop(ops[0].imm)
                    if p is None:
                        return None
                    nd -= p
                else:
                    return None
            elif iid == x86.X86_INS_RET:
                break
            if iid == x86.X86_INS_JMP:
                if ops[0].type == x86.X86_OP_IMM:
                    a = ops[0].imm
                    dep = nd
                    continue
                break
            if x86.X86_GRP_JUMP in i.groups and iid != x86.X86_INS_JMP:
                if ops[0].type == x86.X86_OP_IMM:
                    work.append((ops[0].imm, nd, ebpd))
            a = i.address + i.size
            dep = nd
    if len(found) == 1:
        return found.pop()
    return None


def mem_range(op, d, e):
    """address range of a memory operand relative to esp_at_call; None if not esp/ebp based; 'X' if unresolved."""
    m = op.mem
    if m.base == x86.X86_REG_ESP:
        if m.index != 0:
            return "X"
        return d + m.disp
    if m.base == x86.X86_REG_EBP:
        if e is None:
            return "XE"
        if m.index != 0:
            return "X"
        return e + m.disp
    if m.index in (x86.X86_REG_ESP, x86.X86_REG_EBP):
        return "X"
    return None


REG32 = {x86.X86_REG_EAX: "eax", x86.X86_REG_ECX: "ecx", x86.X86_REG_EDX: "edx", x86.X86_REG_EBX: "ebx",
         x86.X86_REG_ESI: "esi", x86.X86_REG_EDI: "edi", x86.X86_REG_EBP: "ebp"}
SUBREGS = {
    "eax": {x86.X86_REG_EAX, x86.X86_REG_AX, x86.X86_REG_AL, x86.X86_REG_AH},
    "ecx": {x86.X86_REG_ECX, x86.X86_REG_CX, x86.X86_REG_CL, x86.X86_REG_CH},
    "edx": {x86.X86_REG_EDX, x86.X86_REG_DX, x86.X86_REG_DL, x86.X86_REG_DH},
    "ebx": {x86.X86_REG_EBX, x86.X86_REG_BX, x86.X86_REG_BL, x86.X86_REG_BH},
    "esi": {x86.X86_REG_ESI, x86.X86_REG_SI}, "edi": {x86.X86_REG_EDI, x86.X86_REG_DI},
    "ebp": {x86.X86_REG_EBP, x86.X86_REG_BP},
}


def reg_dead_after(start, reg, fstart, fend, limit=4000):
    """True if on every path from start the register is fully written before any read (call clobbers eax/ecx/edx)."""
    regs = SUBREGS[reg]
    work = [start]
    seen = set()
    steps = 0
    while work:
        a = work.pop()
        while True:
            steps += 1
            if steps > limit or not (fstart <= a < fend):
                return False
            if a in seen:
                break
            seen.add(a)
            i = insn_at(a)
            if i is None:
                return False
            rr, ww = i.regs_access()
            if regs & set(rr):
                return False
            if i.id == x86.X86_INS_CALL:
                if reg in ("eax", "ecx", "edx"):
                    break
                # callee-saved register survives the call
            if i.id == x86.X86_INS_RET:
                if reg == "eax":
                    return False
                break
            full = {x86.X86_REG_EAX: "eax", x86.X86_REG_ECX: "ecx", x86.X86_REG_EDX: "edx",
                    x86.X86_REG_EBX: "ebx", x86.X86_REG_ESI: "esi", x86.X86_REG_EDI: "edi", x86.X86_REG_EBP: "ebp"}
            if any(full.get(w) == reg for w in ww):
                break
            ops = i.operands
            if i.id == x86.X86_INS_JMP:
                if ops[0].type == x86.X86_OP_IMM:
                    a = ops[0].imm
                    continue
                return False
            if x86.X86_GRP_JUMP in i.groups:
                if ops[0].type == x86.X86_OP_IMM:
                    work.append(ops[0].imm)
                else:
                    return False
            a = i.address + i.size
    return True


def reachable(fstart):
    """instruction addresses reachable from fstart over direct jumps/branches inside the body bounds."""
    fo = func_of(fstart)
    end = fo[0] + fo[1]
    seen = set()
    work = [fstart]
    while work:
        a = work.pop()
        while fstart <= a < end and a not in seen:
            i = insn_at(a)
            if i is None:
                break
            seen.add(a)
            if i.id in (x86.X86_INS_RET,):
                break
            if i.id == x86.X86_INS_JMP:
                if i.operands[0].type == x86.X86_OP_IMM:
                    a = i.operands[0].imm
                    continue
                break
            if x86.X86_GRP_JUMP in i.groups and i.operands[0].type == x86.X86_OP_IMM:
                work.append(i.operands[0].imm)
            a = i.address + i.size
    return seen


_reach_cache = {}


def arg_reach(target):
    """highest entry-relative byte (exclusive) at or above entry+4 the callee touches through esp/ebp.
    4 = touches no argument. None = cannot bound it (address of an argument taken, esp copied,
    unknown stack depth at an esp-relative access, indirect jump)."""
    if target in _reach_cache:
        return _reach_cache[target]
    fo = func_of(target)
    if not fo or fo[0] != target:
        _reach_cache[target] = None
        return None
    end = fo[0] + fo[1]
    work = [(target, 0, None)]
    seen = {}
    best = 4
    res = "ok"
    while work and res == "ok":
        a, dep, ebpd = work.pop()
        while res == "ok":
            if not (target <= a < end):
                res = None
                break
            if a in seen:
                if seen[a] != (dep, ebpd):
                    # depth differs at a merge: only safe if no later esp access; treat as unbounded
                    res = None
                break
            seen[a] = (dep, ebpd)
            i = insn_at(a)
            if i is None:
                res = None
                break
            iid = i.id
            ops = i.operands
            for op in ops:
                if op.type == x86.X86_OP_MEM:
                    m = op.mem
                    if m.base == x86.X86_REG_ESP or m.base == x86.X86_REG_EBP:
                        if m.index != 0:
                            res = None
                            break
                        if m.base == x86.X86_REG_ESP:
                            if dep is None:
                                res = None
                                break
                            off = m.disp - dep
                        else:
                            if ebpd is None:
                                res = None
                                break
                            off = m.disp - ebpd
                        if iid == x86.X86_INS_LEA:
                            if off >= 4:
                                res = None
                                break
                            continue
                        if off + op.size > 4:
                            best = max(best, off + op.size)
                    elif m.index in (x86.X86_REG_ESP, x86.X86_REG_EBP):
                        res = None
                        break
                elif op.type == x86.X86_OP_REG and op.reg == x86.X86_REG_ESP and iid == x86.X86_INS_MOV and op is ops[1] \
                        and not (ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_EBP):
                    res = None
                    break
            if res != "ok":
                break
            nd = dep
            if iid == x86.X86_INS_PUSH:
                nd = None if dep is None else dep + 4
            elif iid == x86.X86_INS_POP:
                nd = None if dep is None else dep - 4
                if ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_EBP:
                    ebpd = None
            elif iid in (x86.X86_INS_SUB, x86.X86_INS_ADD) and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP:
                if ops[1].type != x86.X86_OP_IMM or dep is None:
                    nd = None
                else:
                    nd = dep + ops[1].imm if iid == x86.X86_INS_SUB else dep - ops[1].imm
            elif iid == x86.X86_INS_AND and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP:
                nd = None
            elif iid == x86.X86_INS_MOV and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_EBP:
                ebpd = dep if (ops[1].type == x86.X86_OP_REG and ops[1].reg == x86.X86_REG_ESP) else None
            elif iid == x86.X86_INS_MOV and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP:
                nd = ebpd if (ops[1].type == x86.X86_OP_REG and ops[1].reg == x86.X86_REG_EBP) else None
            elif iid == x86.X86_INS_LEAVE:
                nd = None if ebpd is None else ebpd - 4
                ebpd = None
            elif iid == x86.X86_INS_CALL:
                if ops[0].type == x86.X86_OP_IMM:
                    p = callee_pop(ops[0].imm)
                    nd = None if (p is None or dep is None) else dep - p
                else:
                    nd = None
            elif iid == x86.X86_INS_RET:
                break
            if iid == x86.X86_INS_JMP:
                if ops[0].type == x86.X86_OP_IMM:
                    a, dep = ops[0].imm, nd
                    if not (target <= a < end):
                        res = None  # tail jump: the jumped-to body sees the same arguments
                    continue
                res = None
                break
            if x86.X86_GRP_JUMP in i.groups:
                if ops[0].type == x86.X86_OP_IMM:
                    work.append((ops[0].imm, nd, ebpd))
                else:
                    res = None
                    break
            a, dep = i.address + i.size, nd
    _reach_cache[target] = best if res == "ok" else None
    return _reach_cache[target]


def scan_site(site, slot_lo, slot_n, popped):
    """walk the caller from the return address. slot bytes are esp_at_call + slot_lo .. +slot_n."""
    fo = func_of(site)
    fstart, fend = fo[0], fo[0] + fo[1]
    e0 = ebp_minus_esp_at(fstart, site)  # esp_at_call - ebp  -> ebp = esp_at_call - e0
    e = (-e0) if e0 is not None else None  # ebp relative to esp_at_call
    events = []
    start_d = popped
    work = [(site + insn_at(site).size, start_d, frozenset(range(slot_lo, slot_lo + slot_n)), e)]
    seen = set()
    steps = 0
    while work:
        a, d, live, eb = work.pop()
        while True:
            steps += 1
            if steps > 100000:
                events.append(("UNRESOLVED", a, "step limit"))
                return events
            live = frozenset(b for b in live if b >= d)  # bytes below esp are dead
            if not live:
                break
            if not (fstart <= a < fend):
                events.append(("UNRESOLVED", a, "left caller bounds"))
                break
            key = (a, d, live, eb)
            if key in seen:
                break
            seen.add(key)
            i = insn_at(a)
            if i is None:
                events.append(("UNRESOLVED", a, "undecodable"))
                break
            txt = f"{i.address:#010x} {i.mnemonic} {i.op_str}"
            iid = i.id
            ops = i.operands
            rr, ww = i.regs_access()
            # esp/ebp copied out (address of frame escapes)
            if iid not in (x86.X86_INS_PUSH, x86.X86_INS_POP, x86.X86_INS_CALL, x86.X86_INS_RET) and \
                    not (iid in (x86.X86_INS_ADD, x86.X86_INS_SUB) and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP):
                for op in ops:
                    if op.type == x86.X86_OP_REG and op.reg == x86.X86_REG_ESP and iid != x86.X86_INS_MOV:
                        events.append(("UNRESOLVED", a, "esp used as value: " + txt))
                        live = frozenset()
                    if iid == x86.X86_INS_MOV and op.type == x86.X86_OP_REG and op.reg == x86.X86_REG_ESP and op is ops[1]:
                        events.append(("UNRESOLVED", a, "esp copied: " + txt))
                        live = frozenset()
                if not live:
                    break
            nd = d
            kill = set()
            # memory operands
            for op in ops:
                if op.type != x86.X86_OP_MEM:
                    continue
                r = mem_range(op, d, eb)
                if r is None:
                    continue
                if isinstance(r, str):
                    events.append(("UNRESOLVED", a, ("ebp frame unknown: " if r == "XE" else "indexed frame access: ") + txt))
                    live = frozenset()
                    break
                rng = set(range(r, r + op.size))
                if not (rng & live):
                    continue
                if iid == x86.X86_INS_LEA:
                    events.append(("UNRESOLVED", a, "address of slot taken: " + txt))
                    live = frozenset()
                    break
                if op.access & capstone.CS_AC_READ:
                    events.append(("READ", a, txt))
                    live = frozenset()
                    break
                kill |= rng
            if not live:
                break
            live = live - kill
            if iid == x86.X86_INS_PUSH:
                nd = d - 4
                live = live - set(range(nd, nd + 4))
            elif iid == x86.X86_INS_POP:
                rng = set(range(d, d + 4))
                if rng & live:
                    reg = REG32.get(ops[0].reg) if ops[0].type == x86.X86_OP_REG else None
                    dead = reg is not None and reg_dead_after(i.address + i.size, reg, fstart, fend)
                    events.append(("POPREAD_DEAD" if dead else "POPREAD_LIVE", a, txt))
                    if not dead:
                        break
                nd = d + 4
                if ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_EBP:
                    eb = None
            elif iid in (x86.X86_INS_ADD, x86.X86_INS_SUB) and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP:
                if ops[1].type != x86.X86_OP_IMM:
                    events.append(("UNRESOLVED", a, "esp adjusted by register: " + txt))
                    break
                nd = d + ops[1].imm if iid == x86.X86_INS_ADD else d - ops[1].imm
            elif iid == x86.X86_INS_MOV and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_ESP:
                if eb is None or not (ops[1].type == x86.X86_OP_REG and ops[1].reg == x86.X86_REG_EBP):
                    events.append(("UNRESOLVED", a, "esp loaded: " + txt))
                    break
                nd = eb
            elif iid == x86.X86_INS_MOV and ops[0].type == x86.X86_OP_REG and ops[0].reg == x86.X86_REG_EBP:
                if ops[1].type == x86.X86_OP_REG and ops[1].reg == x86.X86_REG_ESP:
                    eb = d
                else:
                    eb = None
            elif iid == x86.X86_INS_LEAVE:
                if eb is None:
                    events.append(("UNRESOLVED", a, "leave with unknown ebp"))
                    break
                nd = eb + 4
                eb = None
            elif iid == x86.X86_INS_CALL:
                still = frozenset(b for b in live if b >= d)
                if still:
                    # the callee sees slot byte b at entry-relative offset b - d + 4
                    if ops[0].type != x86.X86_OP_IMM:
                        events.append(("UNRESOLVED", a, "indirect call while slot bytes above esp: " + txt))
                        break
                    reach = arg_reach(ops[0].imm)
                    lo = min(still) - d + 4
                    if reach is None:
                        events.append(("UNRESOLVED", a, "call while slot bytes above esp, callee arg reach unknown: " + txt))
                        break
                    if reach > lo:
                        events.append(("READ", a, f"callee may read slot (slot at entry+{lo:#x}, callee touches up to entry+{reach:#x}): " + txt))
                        break
                    events.append(("PASSED_OVER", a, f"slot at callee entry+{lo:#x}, callee touches below entry+{reach:#x}: " + txt))
            elif iid == x86.X86_INS_RET:
                events.append(("RET_CLEAN", a, txt))
                break
            if iid == x86.X86_INS_CALL:
                if ops[0].type == x86.X86_OP_IMM:
                    p = callee_pop(ops[0].imm)
                    if p is None:
                        events.append(("UNRESOLVED", a, "callee cleanup unknown: " + txt))
                        break
                    nd = d + p
                else:
                    events.append(("UNRESOLVED", a, "indirect call: " + txt))
                    break
            if iid == x86.X86_INS_JMP:
                if ops[0].type == x86.X86_OP_IMM:
                    a, d = ops[0].imm, nd
                    continue
                events.append(("UNRESOLVED", a, "indirect jmp: " + txt))
                break
            if x86.X86_GRP_JUMP in i.groups:
                if ops[0].type == x86.X86_OP_IMM:
                    work.append((ops[0].imm, nd, live, eb))
                else:
                    events.append(("UNRESOLVED", a, "indirect branch: " + txt))
                    break
            a, d = i.address + i.size, nd
            live = frozenset(b for b in live if b >= d)
            if not live:
                events.append(("RELEASED", a, txt))
                break
    return events


_fpu_refs = None


def float_straddle(v, sec):
    """An unaligned hit outside .text is not a pointer when both 4-aligned dwords it spans are read by
    x87 memory operands ([disp32], no base/index) somewhere in .text. Returns the evidence string or None."""
    global _fpu_refs
    if sec == ".text" or v % 4 == 0:
        return None
    if _fpu_refs is None:
        _fpu_refs = {}
        t = [x for x in SECS if x[0] == ".text"][0]
        tb = data[t[3]:t[3] + t[4]]
        for k in range(len(tb) - 6):
            if tb[k] in (0xD8, 0xD9, 0xDC, 0xDD) and (tb[k + 1] & 0xC7) == 0x05:
                i = insn_at(t[1] + k)
                if i is None or i.size != 6 or not i.operands:
                    continue
                for op in i.operands:
                    if op.type == x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                        _fpu_refs.setdefault(op.mem.disp & 0xFFFFFFFF, []).append(f"{i.address:#010x} {i.mnemonic} {i.op_str}")
    lo = v & ~3
    hi = lo + 4
    if lo in _fpu_refs and hi in _fpu_refs:
        f = lambda a: struct.unpack_from("<I", data, va2off(a))[0]
        return (f"spans x87 constants {lo:#010x}={f(lo):#010x} ({'; '.join(_fpu_refs[lo][:2])}) and "
                f"{hi:#010x}={f(hi):#010x} ({'; '.join(_fpu_refs[hi][:2])}); not a pointer")
    return None


def main():
    text = [s for s in SECS if s[0] == ".text"][0]
    tstart, tsize, traw = text[1], text[4], text[3]
    tb = data[traw:traw + tsize]
    out = {"exe": "original/MASHED.exe.unpatched", "bodies": {}}
    for body, slots in BODIES.items():
        rec = {"slots": [f"+{k:#x}/{n}" for k, n in slots], "body_pop": callee_pop(body),
               "direct": [], "tail_jumps": [], "dword_refs": [], "verdict": None}
        # direct rel32 call/jmp
        for op in (0xE8, 0xE9):
            idx = 0
            while True:
                idx = tb.find(bytes([op]), idx)
                if idx < 0 or idx + 5 > len(tb):
                    break
                v = tstart + idx
                tgt = (v + 5 + struct.unpack_from("<i", tb, idx + 1)[0]) & 0xFFFFFFFF
                if tgt == body:
                    i = insn_at(v)
                    fo = func_of(v)
                    ok = i is not None and i.size == 5 and fo is not None and (v in linear_addrs(fo[0], fo[1]) or v in reachable(fo[0]))
                    if ok:
                        (rec["direct"] if op == 0xE8 else rec["tail_jumps"]).append({"site": f"{v:#010x}", "caller": f"{fo[0]:#010x} {fo[2]}"})
                    elif i is not None and i.size == 5 and i.operands and i.operands[0].type == x86.X86_OP_IMM:
                        (rec["direct"] if op == 0xE8 else rec["tail_jumps"]).append({"site": f"{v:#010x}", "caller": "NOT IN A LINEAR FUNCTION BODY", "unconfirmed": True})
                idx += 1
        # absolute dword refs
        pat = struct.pack("<I", body)
        idx = 0
        while True:
            idx = data.find(pat, idx)
            if idx < 0:
                break
            v, sec = off2va(idx)
            st = float_straddle(v, sec) if v else None
            if st:
                rec.setdefault("straddles", []).append(f"{v:#010x} {sec}: {st}")
            else:
                rec["dword_refs"].append(f"{v:#010x} {sec}" if v else f"file+{idx:#x}")
            idx += 1
        sites_ok = True
        for c in rec["direct"]:
            if c.get("unconfirmed"):
                c["result"] = "UNRESOLVED (site not in a linear function body)"
                sites_ok = False
                continue
            site = int(c["site"], 16)
            per = []
            for k, n in slots:
                pop = rec["body_pop"]
                if pop is None:
                    per.append({"slot": f"+{k:#x}", "events": [["UNRESOLVED", c["site"], "body ret imm unknown"]]})
                    continue
                ev = scan_site(site, k - 4, n, pop)
                per.append({"slot": f"+{k:#x}", "events": [[t, f"{a:#010x}", s] for t, a, s in ev]})
            c["slots"] = per
            bad = [e for p in per for e in p["events"] if e[0] in ("READ", "UNRESOLVED", "POPREAD_LIVE")]
            c["result"] = "NO READ" if not bad else ("READ" if any(e[0] in ("READ", "POPREAD_LIVE") for e in bad) else "UNRESOLVED")
            if bad:
                sites_ok = False
        if rec["dword_refs"] or rec["tail_jumps"]:
            rec["verdict"] = "FAIL: indirect or unknown caller (address taken)" if rec["dword_refs"] else "FAIL: tail jump (caller of the jumper unknown)"
        elif not rec["direct"]:
            rec["verdict"] = "FAIL: no static caller found"
        elif sites_ok:
            rec["verdict"] = "PASS: no caller reads the slot after the call"
        else:
            rec["verdict"] = "FAIL: a caller reads the slot or a site is unresolved"
        out["bodies"][f"{body:#010x}"] = rec
        print(f"{body:#010x} pop={rec['body_pop']} direct={len(rec['direct'])} jmp={len(rec['tail_jumps'])} refs={len(rec['dword_refs'])} -> {rec['verdict']}")
        for c in rec["direct"]:
            print("   ", c["site"], c["caller"], c.get("result"))
            for p in c.get("slots", []):
                for ev in p["events"]:
                    if ev[0] != "RELEASED" or True:
                        print("       ", p["slot"], *ev)
    Path(sys.argv[1]).write_text(json.dumps(out, indent=1))


if __name__ == "__main__":
    main()
