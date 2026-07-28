# sweep_rega_analyze.py — CLASS A stage 3: which plain-C hooks can break an unhooked caller?
#
# THE DEFECT (0x0045c640, 9 instances of this class to date):
#   The ORIGINAL touches only EAX. Compiled C treats EAX/ECX/EDX as free scratch. The
#   UNHOOKED caller keeps its loop index in EDX across the CALL, because the original never
#   wrote EDX:
#       0x0045c6b1 XOR EDX,EDX / PUSH EDX / CALL 0x0045c640 / MOV [EDX*4+0x88f0c0],0
#   Installing a C hook there destroys EDX and the caller writes through a wild index.
#
# Three filters, each removing a class of false positive:
#   1. The original must be a LEAF (no CALL). A non-leaf clobbers EAX/ECX/EDX by ABI
#      anyway, so no caller may rely on preservation across it.
#   2. The original must never write the register (explicitly or implicitly - MUL/DIV/CDQ
#      write EDX:EAX, string ops write ECX/ESI/EDI, etc).
#   3. At least one UNHOOKED caller must actually HOLD A LIVE VALUE in that register across
#      the call site: written before the CALL, read after it, with no intervening write.
#      Filter 3 is what turns a large candidate pool into a short actionable list.
#
# READ-ONLY, offline. Needs hooks_bodies.json (from sweep_rega_eval.py via ghidra_eval).
import json, os, struct, sys

try:
    import capstone
except ImportError:
    sys.exit("needs capstone")

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(ROOT, "original", "MASHED.exe.unpatched")
HERE = os.path.dirname(os.path.abspath(__file__))

SCRATCH = ("eax", "ecx", "edx")
SUB = {
    "eax": {"eax", "ax", "al", "ah"},
    "ecx": {"ecx", "cx", "cl", "ch"},
    "edx": {"edx", "dx", "dl", "dh"},
}
# instructions with implicit scratch writes
IMPLICIT = {
    "mul": ("eax", "edx"), "imul": ("eax", "edx"), "div": ("eax", "edx"),
    "idiv": ("eax", "edx"), "cdq": ("edx",), "cwd": ("edx",), "cbw": ("eax",),
    "cwde": ("eax",), "lodsb": ("eax",), "lodsd": ("eax",), "rep": ("ecx",),
    "repe": ("ecx",), "repne": ("ecx",), "loop": ("ecx",), "xlatb": ("eax",),
}
WRITES_DEST = {
    "mov", "movzx", "movsx", "lea", "xor", "or", "and", "add", "sub", "adc", "sbb",
    "inc", "dec", "neg", "not", "shl", "shr", "sar", "rol", "ror", "rcl", "rcr",
    "pop", "xchg", "imul", "setz", "setnz", "sete", "setne", "setl", "setg",
    "setle", "setge", "seta", "setb", "setae", "setbe", "cmov", "bswap", "btr", "bts",
}


def load_pe():
    data = open(EXE, "rb").read()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        s = pe + 24 + opt + i * 40
        vsz, va, rsz, ro = struct.unpack_from("<IIII", data, s + 8)
        secs.append((va, vsz, ro, rsz))
    return data, base, secs


def reader(data, base, secs):
    def read(va, n):
        r = va - base
        for v, vs, ro, rs in secs:
            if v <= r < v + max(vs, rs):
                off = ro + (r - v)
                return data[off:off + n]
        return b""
    return read


def base_reg(tok):
    for full, alts in SUB.items():
        if tok in alts:
            return full
    return None


def written_regs(ins):
    """Scratch registers this instruction writes."""
    out = set()
    mn = ins.mnemonic
    for k, regs in IMPLICIT.items():
        if mn.startswith(k):
            out.update(regs)
    if mn.startswith("cmov"):
        mn = "cmov"
    if mn in WRITES_DEST or mn.startswith("set"):
        dest = ins.op_str.split(",")[0].strip()
        b = base_reg(dest)
        if b:
            out.add(b)
    if mn == "xchg":
        for part in ins.op_str.split(","):
            b = base_reg(part.strip())
            if b:
                out.add(b)
    return out


def reads_reg(ins, reg):
    return any(a in ins.op_str for a in SUB[reg])


def main():
    bodies = json.load(open(os.path.join(HERE, "hooks_bodies.json")))
    data, base, secs = load_pe()
    read = reader(data, base, secs)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    # ---- filters 1 + 2: leaf originals that never write some scratch register ----
    cands = []
    for r in bodies:
        if r.get("status") != "OK" or r.get("naked"):
            continue
        start, end = r["start"], r["end"]
        n = end - start + 1
        if n <= 0 or n > 0x4000:
            continue
        blob = read(start, n)
        if not blob:
            continue
        ins = list(md.disasm(blob, start))
        if not ins:
            continue
        if any(i.mnemonic == "call" for i in ins):
            continue                                   # filter 1: non-leaf
        w = set()
        for i in ins:
            w |= written_regs(i)
        preserved = [g for g in SCRATCH if g not in w]  # filter 2
        if not preserved:
            continue
        if not r.get("callers_unhooked"):
            continue
        cands.append((r, preserved, len(ins)))

    # ---- filter 3: does an unhooked caller hold a live value in that register? ----
    confirmed = []
    for r, preserved, nins in cands:
        target = r["rva"]
        for caller in r["callers_unhooked"]:
            cb = next((b for b in bodies if b.get("start") == caller), None)
            # caller may not be in our hook list; disassemble a window from its entry
            cstart = caller
            cn = (cb["end"] - cb["start"] + 1) if cb and cb.get("end") else 0x600
            blob = read(cstart, min(cn, 0x2000))
            if not blob:
                continue
            cins = list(md.disasm(blob, cstart))
            for k, i in enumerate(cins):
                if i.mnemonic != "call" or not i.op_str.startswith("0x"):
                    continue
                try:
                    if int(i.op_str, 16) != target:
                        continue
                except ValueError:
                    continue
                for reg in preserved:
                    # written before the call (look back up to 8) ...
                    pre_write = any(reg in written_regs(cins[j]) for j in range(max(0, k - 8), k))
                    if not pre_write:
                        continue
                    # ... and read after it before being rewritten (look ahead up to 8)
                    for j in range(k + 1, min(k + 9, len(cins))):
                        nx = cins[j]
                        if reads_reg(nx, reg) and reg not in written_regs(nx):
                            confirmed.append({
                                "rva": target, "sym": r["sym"], "file": r["file"],
                                "reg": reg, "caller": caller,
                                "call_site": i.address, "use": "%s %s" % (nx.mnemonic, nx.op_str),
                                "use_addr": nx.address,
                            })
                            break
                        if reg in written_regs(nx):
                            break

    print("CLASS A sweep — plain-C hooks that may clobber a register the original preserves")
    print("  hooks analysed (plain C, resolved) : %d" % sum(
        1 for r in bodies if r.get("status") == "OK" and not r.get("naked")))
    print("  leaf + preserves a scratch reg + has an unhooked caller : %d" % len(cands))
    print("  CONFIRMED (unhooked caller holds a live value across the call) : %d\n" % len(confirmed))
    seen = set()
    for c in confirmed:
        key = (c["rva"], c["reg"], c["caller"])
        if key in seen:
            continue
        seen.add(key)
        print("  0x%08x %-28s clobbers %s   caller 0x%08x  call@0x%08x  uses: %s"
              % (c["rva"], c["sym"], c["reg"].upper(), c["caller"], c["call_site"], c["use"]))
        print("        %s" % c["file"])
    with open(os.path.join(HERE, "classa_findings.json"), "w") as f:
        json.dump({"candidates": [{"rva": r["rva"], "sym": r["sym"], "preserved": p}
                                  for r, p, _ in cands], "confirmed": confirmed}, f, indent=1)


if __name__ == "__main__":
    main()
