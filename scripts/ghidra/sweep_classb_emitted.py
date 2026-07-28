# sweep_classb_emitted.py — CLASS B detector, run over the EMITTED .asi.
#
# The 0x00448700 hang was invisible in the C source and obvious in the machine code:
#
#   mov eax, 0x64          ; loop counter placed in EAX
#   push <arg>
#   call <naked thunk>     ; MSVC read the thunk's visible __asm, saw no write to EAX,
#   add esp, 4             ; and concluded EAX survives the call
#   sub eax, 1             ; <- actually the CALLEE'S RETURN VALUE
#   jne  <back>            ; -> never terminates when the callee returns 0
#
# So we look for the shape directly: a CALL, then a read-modify-write of EAX that feeds a
# BACKWARD conditional branch, with no write to EAX in between. A normal "use the return
# value" sequence does not loop back onto its own EAX initialisation, which makes this
# specific and quiet.
#
# READ-ONLY. Needs the built .asi + its .map for symbolisation.
import os, re, sys

try:
    import capstone, pefile
except ImportError:
    sys.exit("needs capstone + pefile")

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ASI = os.path.join(ROOT, "original", "mashed_re_dev.asi")
MAP = os.path.join(ROOT, "mashedmod", "build", "mashed_re_dev.map")
PREF = 0x10000000   # .asi preferred base; the exe map is rebased below

EAX_WRITE = re.compile(r"^(mov|movzx|movsx|lea|xor|or|and|pop|imul|mul|div|idiv|cdq|xchg|setz|setne|sbb|adc|neg|not|shl|shr|sar|rol|ror|cmpxchg)$")
EAX_RMW = re.compile(r"^(sub|dec|add|inc)$")


def load_map():
    rows = []
    if not os.path.exists(MAP):
        return rows
    pat = re.compile(r"\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s+(f\s+)?(\S+)?")
    for line in open(MAP, encoding="latin1"):
        m = pat.match(line)
        if not m:
            continue
        va = int(m.group(4), 16)
        if va < PREF:
            continue
        rows.append((va - PREF, m.group(3), m.group(6) or ""))
    rows.sort()
    return rows


def nearest(rows, rva):
    prev = None
    for a, n, o in rows:
        if a <= rva:
            prev = (a, n, o)
        else:
            break
    return prev


def scan(insns):
    """Return the suspect (call, branch) pairs in a decoded instruction list."""
    hits = []
    for k, i in enumerate(insns):
        if i.mnemonic != "call":
            continue
        for j in range(k + 1, min(k + 8, len(insns))):
            n = insns[j]
            op = n.op_str.replace(" ", "")
            mn = n.mnemonic
            if mn.startswith("j"):
                try:
                    tgt = int(n.op_str, 16)
                except ValueError:
                    break
                if mn != "jmp" and tgt <= i.address:
                    hits.append((i, n, insns[max(0, k - 4):k + 1]))
                break
            if EAX_RMW.match(mn) and op.startswith("eax,"):
                continue
            if mn in ("cmp", "test") and "eax" in op:
                continue
            if EAX_WRITE.match(mn) and op.startswith("eax,"):
                break
            if mn in ("add", "sub") and op.startswith("esp,"):
                continue
            if mn in ("push", "pop", "nop", "mov") and "eax" not in op:
                continue
            if "eax" in op:
                continue
            break
    real = []
    for call_i, br, pre in hits:
        seg = [x for x in insns if call_i.address < x.address < br.address]
        if not any(EAX_RMW.match(x.mnemonic) and x.op_str.replace(" ", "").startswith("eax,")
                   for x in seg):
            continue
        real.append((call_i, br, pre))
    return real


def selftest():
    """Validate the detector against the ACTUAL bytes of the 0x00448700 hang.

    A zero-finding sweep is worthless unless the detector is known to fire on the defect
    it is looking for. These are the bytes MSVC emitted before the fix (asi+0xc5c0).
    """
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    buggy = bytes.fromhex("b864000000" "68e07f8900" "e8e1ffffff" "83c404" "83e801" "75ee")
    fixed = bytes.fromhex("56" "be64000000" "68e07f8900" "ff1538ce0910" "83c404" "4e" "75ef")
    for name, blob, expect in (("PRE-FIX (known defect)", buggy, True),
                               ("POST-FIX (naked, ESI)", fixed, False)):
        insns = list(md.disasm(blob, 0x1000c5c0))
        hits = scan(insns)
        ok = bool(hits) == expect
        print("  %-24s -> %d hit(s)  expected %s  %s"
              % (name, len(hits), "HIT" if expect else "clean", "PASS" if ok else "**FAIL**"))
        if not ok:
            return False
    return True


def main():
    if "--selftest" in sys.argv:
        print("detector self-test:")
        sys.exit(0 if selftest() else 1)
    target = ASI
    if "--target" in sys.argv:
        which = sys.argv[sys.argv.index("--target") + 1]
        if which == "exe":
            target = os.path.join(ROOT, "mashedmod", "build", "mashed_re.exe")
        elif which != "asi":
            target = which
    global MAP, PREF
    if target.endswith("mashed_re.exe"):
        MAP = os.path.join(ROOT, "mashedmod", "build", "mashed_re.map")
        PREF = 0x00010000        # /BASE:0x10000 for the standalone (see map header)
    pe = pefile.PE(target, fast_load=True)
    pe.parse_data_directories()
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    text = None
    for s in pe.sections:
        if s.Name.rstrip(b"\0").startswith(b".text"):
            text = (s.VirtualAddress, s.Misc_VirtualSize)
            break
    if not text:
        sys.exit("no .text")
    start, size = text
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    # RESYNC LOOP — capstone's disasm() STOPS at the first undecodable byte, and .text
    # contains jump tables and alignment padding. A single pass silently truncated this
    # scan (23,901 instructions for an 845 KB image), which made an earlier "0 findings"
    # result meaningless. Skip a byte and continue until the section is consumed.
    insns = []
    off = 0
    while off < size:
        decoded = 0
        for i in md.disasm(img[start + off:start + size], base + start + off):
            insns.append(i)
            decoded = i.address - (base + start + off) + i.size
        off += decoded if decoded else 1
    rows = load_map()

    by_addr = {i.address: k for k, i in enumerate(insns)}
    hits = []
    for k, i in enumerate(insns):
        if i.mnemonic != "call":
            continue
        # walk forward from the call
        for j in range(k + 1, min(k + 8, len(insns))):
            n = insns[j]
            op = n.op_str.replace(" ", "")
            mn = n.mnemonic
            if mn.startswith("j"):
                # conditional branch: is it BACKWARD past our call?
                try:
                    tgt = int(n.op_str, 16)
                except ValueError:
                    break
                if mn != "jmp" and tgt <= i.address:
                    hits.append((i, n, insns[max(0, k - 4):k + 1]))
                break
            if EAX_RMW.match(mn) and op.startswith("eax,"):
                continue                      # the read-modify-write we are looking for
            if mn in ("cmp", "test") and "eax" in op:
                continue
            if EAX_WRITE.match(mn) and op.startswith("eax,"):
                break                         # EAX redefined -> normal return-value use
            if mn in ("add", "sub") and op.startswith("esp,"):
                continue                      # cdecl cleanup
            if mn in ("push", "pop", "nop", "mov") and "eax" not in op:
                continue
            if "eax" in op:
                continue
            break

    # The structural shape alone is NOT sufficient: a busy-wait such as
    #     call [GetTickCount] / sub eax,[g_last] / cmp eax,0x258 / jb back
    # has the identical shape but is CORRECT, because the call itself produces EAX each
    # iteration. The true defect needs the callee to be one whose return value the C source
    # did not expect — i.e. one of OUR void/tail-transfer naked thunks. So join against the
    # stage-1 candidate list and resolve each call target through the .map.
    inv_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hooks_inventory.json")
    classb_syms = set()
    if os.path.exists(inv_path):
        import json
        inv = json.load(open(inv_path))
        for c in inv.get("classb_candidates", []):
            classb_syms.add(c["sym"])
            classb_syms.add("_" + c["sym"])          # MSVC __cdecl decoration
            classb_syms.add("?" + c["sym"])          # C++ mangling prefix

    def target_sym(ins):
        if not ins.op_str.startswith("0x"):
            return None                              # indirect (IAT or data slot)
        try:
            tgt = int(ins.op_str, 16) - base
        except ValueError:
            return None
        s = nearest(rows, tgt)
        return s[1] if s else None

    real, shape_only = [], []
    for call_i, br, pre in hits:
        seg = [x for x in insns if call_i.address < x.address < br.address]
        if not any(EAX_RMW.match(x.mnemonic) and x.op_str.replace(" ", "").startswith("eax,") for x in seg):
            continue
        sym = target_sym(call_i)
        hit = (call_i, br, pre, sym)
        if sym and any(sym == c or sym.lstrip("_?").startswith(c.lstrip("_?")) for c in classb_syms):
            real.append(hit)
        else:
            shape_only.append(hit)

    print("CLASS B scan over %s" % os.path.relpath(target, ROOT).replace("\\", "/"))
    print("instructions scanned : %d" % len(insns))
    print("suspect sites        : %d\n" % len(real))
    for call_i, br, pre in real:
        rva = call_i.address - base
        sym = nearest(rows, rva)
        where = "%s+0x%x" % (sym[1], rva - sym[0]) if sym else "?"
        obj = sym[2] if sym else ""
        print("+0x%05x  in %-38s [%s]  -> callee %s" % (rva, where, obj, sym))
        for p in pre[:-1]:
            print("      %-6s %s" % (p.mnemonic, p.op_str))
        print("   >  %-6s %s" % (call_i.mnemonic, call_i.op_str))
        seg = [x for x in insns if call_i.address < x.address <= br.address]
        for x in seg[:6]:
            print("      %-6s %s" % (x.mnemonic, x.op_str))
        print()


if __name__ == "__main__":
    main()
