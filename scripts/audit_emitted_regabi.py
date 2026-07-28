# audit_emitted_regabi.py — MEASURE (not predict) whether our emitted hook bodies clobber
# a register the original preserves.
#
# Why: Sweep A (scripts/ghidra/sweep_rega_*.py) proves the ORIGINAL never writes ECX/EDX and
# that an unhooked caller holds a live value there across the CALL. It does NOT show what our
# replacement actually emits — "MSVC will probably use ECX as scratch" is a prediction. A
# prediction is not evidence (NO-GUESSING, CLAUDE.md). This disassembles the real bytes in
# mashedmod/build/mashed_re_dev.asi, by EXPORT NAME, and reports the registers each body
# actually writes.
#
# CAPSTONE RESYNC (hard-won, do not remove): capstone.disasm() STOPS at the first undecodable
# byte and returns silently short — .text is full of jump tables and padding. A single pass
# truncated an earlier sweep from 204,135 instructions to 23,901. Every linear scan here
# advances one byte and retries on a decode failure.
#
# SELF-TEST (mandatory, --self-test): a detector that finds nothing is worthless unless it is
# first shown to find a known instance. `Zero4944b0` (PromoLoop_sessionB.cpp:794) is naked asm
# that provably writes EAX and no more; `IsMultiplayerMode` is plain C++ over a multi-way
# compare. The self-test asserts the walker reaches a RET for both and reports a non-empty
# write set, so a silent truncation cannot masquerade as "no clobber found".
import argparse, sys
import capstone, pefile

ASI = r"C:\Users\maria\Desktop\Proyectos\Mashed\mashedmod\build\mashed_re_dev.asi"

# Sweep A's nine Class-A findings: export name -> register the ORIGINAL preserves.
NINE = [
    ("UtilFloat63b910Get",        "ecx", "0x0040dc80"),
    ("GatedSwitch636ad0",         "ecx", "0x0041f360"),
    ("TrackLoaderFloatGet",       "ecx", "0x00426e00"),
    ("IsMultiplayerMode",         "ecx", "0x00430760"),
    ("ClearTable471530",          "edx", "0x00471530"),
    ("ParticleEmitter_SetScalar", "ecx", "0x00476a30"),
    ("IntroSplashVtableSlot6",    "ecx", "0x004c1a00"),
    ("Mark4d5480",                "edx", "0x004d5480"),
    ("RwpWorldSolverHandle",      "edx", "0x0055deb0"),
]

SUB = {
    "ecx": {"ecx", "cx", "cl", "ch"},
    "edx": {"edx", "dx", "dl", "dh"},
}


def load():
    pe = pefile.PE(ASI, fast_load=False)
    base = pe.OPTIONAL_HEADER.ImageBase
    exports = {}
    d = getattr(pe, "DIRECTORY_ENTRY_EXPORT", None)
    if d:
        for e in d.symbols:
            if e.name:
                exports[e.name.decode()] = e.address  # RVA
    data = pe.get_memory_mapped_image()
    return pe, base, exports, data


def walk(data, rva, limit=4096):
    """Linear-sweep a body from `rva` until RET / unconditional tail JMP at depth 0.

    Returns (instructions, terminator, truncated). Advances one byte on a decode failure
    instead of stopping — see CAPSTONE RESYNC above.
    """
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    out = []
    off = rva
    end = rva + limit
    while off < end:
        chunk = data[off:off + 16]
        if not chunk:
            break
        insns = list(md.disasm(chunk, off, count=1))
        if not insns:
            off += 1  # resync
            continue
        i = insns[0]
        out.append(i)
        if i.mnemonic == "ret":
            return out, "ret", False
        if i.mnemonic == "jmp":
            # A tail JMP ends the body just as a RET does.
            return out, "jmp", False
        off += i.size
    return out, None, True


def writes(insns):
    """Registers actually WRITTEN, from capstone's regs_access (not a mnemonic guess).

    A `call` is treated as writing the full caller-saved set: the callee may clobber
    EAX/ECX/EDX regardless of what our own body's instructions touch.
    """
    w = set()
    has_call = False
    for i in insns:
        if i.mnemonic == "call":
            has_call = True
            w |= {"eax", "ecx", "edx"}
            continue
        try:
            _, wr = i.regs_access()
        except capstone.CsError:
            continue
        for r in wr:
            w.add(i.reg_name(r))
    return w, has_call


def clobbers(w, reg):
    return bool(w & SUB[reg])


def audit(name, reg, rva_orig, exports, data):
    if name not in exports:
        return dict(name=name, reg=reg, rva=rva_orig, verdict="NOT-EXPORTED")
    insns, term, trunc = walk(data, exports[name])
    w, has_call = writes(insns)
    if trunc:
        return dict(name=name, reg=reg, rva=rva_orig, verdict="TRUNCATED",
                    n=len(insns), note="no terminator within limit - result unusable")
    if term == "jmp" and not clobbers(w, reg):
        # The walker does NOT follow the jump, so everything past it is unexamined. A
        # "clean" verdict here would be a claim about code we never looked at. Two of the
        # nine ended this way: GatedSwitch636ad0 (a switch jump-table dispatch -- both arms
        # were dumped by hand and are `mov eax,1/ret` and `xor eax,eax/ret`, no ECX/EDX)
        # and IntroSplashVtableSlot6 (a genuine tail call into game code, unknowable here).
        return dict(name=name, reg=reg, rva=rva_orig, verdict="INCONCLUSIVE",
                    n=len(insns), term=term, call=has_call, wrote="-",
                    allw="ends in jmp; targets not followed - resolve by hand")
    return dict(name=name, reg=reg, rva=rva_orig,
                verdict="CLOBBERS" if clobbers(w, reg) else "clean",
                n=len(insns), term=term, call=has_call,
                wrote=",".join(sorted(w & SUB[reg])) or "-",
                allw=",".join(sorted(x for x in w if len(x) <= 3)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--self-test", action="store_true")
    args = ap.parse_args()

    pe, base, exports, data = load()
    print("asi=%s exports=%d" % (ASI, len(exports)))

    if args.self_test:
        print("\n== self-test: the walker must reach a terminator and report writes ==")
        bad = 0
        for probe in ("Zero4944b0", "IsMultiplayerMode"):
            if probe not in exports:
                print("  %-22s NOT EXPORTED - self-test cannot run" % probe); bad += 1; continue
            insns, term, trunc = walk(data, exports[probe])
            w, _ = writes(insns)
            ok = (not trunc) and term is not None and len(w) > 0
            print("  %-22s insns=%-4d term=%-4s writes={%s}  %s"
                  % (probe, len(insns), term, ",".join(sorted(w)), "OK" if ok else "FAIL"))
            if not ok:
                bad += 1
        if bad:
            print("SELF-TEST FAILED - a zero-finding audit below would be meaningless")
            return 1
        print("self-test OK")

    print("\n== emitted-body audit: does OUR code write the register the ORIGINAL preserves? ==")
    print("%-26s %-4s %-11s %-10s %-5s %-5s %-6s %s"
          % ("export", "reg", "orig-rva", "verdict", "insn", "term", "call?", "wrote"))
    n_clob = 0
    for name, reg, rva in NINE:
        r = audit(name, reg, rva, exports, data)
        if r["verdict"] == "CLOBBERS":
            n_clob += 1
        print("%-26s %-4s %-11s %-10s %-5s %-5s %-6s %s"
              % (r["name"], r["reg"], r["rva"], r["verdict"], r.get("n", "-"),
                 r.get("term", "-"), r.get("call", "-"), r.get("wrote", "-")))
    print("\n%d/%d emitted bodies actually clobber the preserved register." % (n_clob, len(NINE)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
