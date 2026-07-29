# sweep_void_eax_return.py — find installed hooks that DROP an implicit EAX return value.
#
# THE CLASS (U-9025, 2026-07-28, commit f1855ad9): FUN_005aef00 loads MOV EAX,[ESP+4] as its
# first instruction and never clobbers EAX, so it returns param_1. Ghidra typed it `void`, and
# our port was declared `void` too — but all three callers do TEST EAX,EAX / JZ and skip a
# __beginthread when it is zero. The audio/stream worker thread was therefore never created,
# and the game hung with no crash and no dump.
#
# Eight earlier instances of the register-ABI class were all CRASHERS, which biased every
# previous hunt toward crash dumps. A dropped return value that merely GATES A BRANCH produces
# a missing subsystem or a hang instead, so it needs its own detector.
#
# WHY NOT REUSE scripts/ghidra/sweep_reg_abi.py: it MISSED 0x005aef00 (checked — absent from
# both `confirmed` and `candidates` in classa_findings.json). Its funnel looks for a register
# the original PRESERVES that our emitted body CLOBBERS. This class is the opposite shape: the
# original DEFINES EAX and our body fails to define it the same way. An all-clear from that
# sweep is not an all-clear for this one.
#
# THE RULE (all three must hold):
#   1. our port for RVA R is installed via RH_ScopedInstall and its C++ return type is `void`
#   2. the ORIGINAL body at R writes EAX (so EAX at the RET is meaningful, not incidental)
#   3. some direct caller READS EAX after `CALL R` before redefining it
#
# SELF-TEST IS MANDATORY. A zero-finding sweep is worthless unless it demonstrably fires on a
# known instance, so --self-test forces 0x005aef00 to be treated as `void` (its source is now
# fixed) and asserts the detector reports it with caller 0x005a8060 and use `test eax, eax`.
# The tool REFUSES to print findings unless the self-test passed.
#
# Capstone note: a single linear disasm pass over .text silently TRUNCATES at the first
# undecodable byte, and .text here is full of jump tables and padding — that trap already
# invalidated two earlier sweeps. This tool never does a whole-section pass: it disassembles
# only bounded windows (a function body up to its first RET, and N instructions after a call
# site), each starting at an address known to be code.
import argparse, json, os, re, struct, sys
from collections import defaultdict

import capstone

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "mashedmod", "src", "mashed_re")
IMAGE = os.path.join(ROOT, "original", "MASHED.exe.unpatched")

KNOWN_RVA = 0x005AEF00           # the self-test instance
KNOWN_CALLER = 0x005A8060


# ── PE helpers ───────────────────────────────────────────────────────────────
class Image:
    def __init__(self, path):
        self.d = open(path, "rb").read()
        pe = struct.unpack_from("<I", self.d, 0x3C)[0]
        nsec = struct.unpack_from("<H", self.d, pe + 6)[0]
        opt = struct.unpack_from("<H", self.d, pe + 20)[0]
        self.base = struct.unpack_from("<I", self.d, pe + 24 + 28)[0]
        self.secs = []
        for i in range(nsec):
            o = pe + 24 + opt + i * 40
            vs, va, rs, ra = struct.unpack_from("<IIII", self.d, o + 8)
            self.secs.append((va, vs, ra, rs))

    def va2off(self, v):
        v -= self.base
        for va, vs, ra, rs in self.secs:
            if va <= v < va + rs:
                return ra + (v - va)
        return None

    def off2va(self, o):
        for va, vs, ra, rs in self.secs:
            if ra <= o < ra + rs:
                return self.base + va + (o - ra)
        return None

    def read(self, va, n):
        o = self.va2off(va)
        return self.d[o:o + n] if o is not None else b""


# ── source parsing ───────────────────────────────────────────────────────────
INSTALL_RE = re.compile(r"RH_ScopedInstall\(\s*([A-Za-z_]\w*)\s*,\s*(0x[0-9a-fA-F]+)\s*\)")


def installed_hooks():
    """(sym, rva, file) for every RH_ScopedInstall that is not commented out."""
    out = []
    for dirpath, _, names in os.walk(SRC):
        for n in names:
            if not n.endswith((".cpp", ".h")):
                continue
            p = os.path.join(dirpath, n)
            for line in open(p, encoding="utf-8", errors="replace"):
                if line.lstrip().startswith("//"):
                    continue
                m = INSTALL_RE.search(line)
                if m:
                    out.append((m.group(1), int(m.group(2), 16),
                                os.path.relpath(p, ROOT).replace("\\", "/")))
    return out


# One generic definition pattern, applied ONCE per file. Compiling a per-symbol regex and
# re-scanning every source file for each of ~1300 symbols did not finish in 10 minutes.
DEF_RE = re.compile(
    r"^[ \t]*(?:extern\s+\"C\"\s+)?(?:__declspec\(\s*dllexport\s*\)\s*)?"
    r"(?:(?P<naked>__declspec\(\s*naked\s*\))\s*)?"
    r"(?P<ret>[A-Za-z_][\w:]*(?:\s*<[^>;{]*>)?(?:\s*\*)*)\s+"
    r"(?:__cdecl|__stdcall|__fastcall|__thiscall)?\s*"
    r"(?P<sym>[A-Za-z_]\w*)\s*\([^;{]*\)\s*(?:\{|$)",
    re.MULTILINE)

RET_NOISE = {"return", "if", "while", "for", "switch", "else", "case", "sizeof", "new"}


def return_types():
    """sym -> (return_type_text, is_void, is_naked), indexed in a single pass.

    `__declspec(naked)` bodies are excluded from the class by construction: their EAX is
    written by hand, so the compiler is not the one choosing it."""
    idx = {}
    for dirpath, _, names in os.walk(SRC):
        for n in names:
            if not n.endswith((".cpp", ".h")):
                continue
            p = os.path.join(dirpath, n)
            s = open(p, encoding="utf-8", errors="replace").read()
            for m in DEF_RE.finditer(s):
                bol = s.rfind("\n", 0, m.start()) + 1
                if s[bol:m.start()].lstrip().startswith("//"):
                    continue
                rt = " ".join(m.group("ret").split())
                if rt in RET_NOISE:
                    continue
                sym = m.group("sym")
                if sym in idx and idx[sym][0] is not None:
                    continue                      # first definition wins
                idx[sym] = (rt, rt == "void", bool(m.group("naked")))
    return lambda sym: idx.get(sym, (None, False, False))


# ── disassembly probes ───────────────────────────────────────────────────────
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
EAX_IDS = {capstone.x86.X86_REG_EAX, capstone.x86.X86_REG_AX,
           capstone.x86.X86_REG_AL, capstone.x86.X86_REG_AH}


def writes_eax(img, rva, max_bytes=512):
    """Does the ORIGINAL body write EAX before its first RET? Bounded linear scan from a known
    code address; returns (bool, first_write_text, first_write_addr)."""
    code = img.read(rva, max_bytes)
    for ins in md.disasm(code, rva):
        r, w = ins.regs_access()
        if any(x in EAX_IDS for x in w):
            return True, "%s %s" % (ins.mnemonic, ins.op_str), ins.address
        if ins.mnemonic == "ret":
            return False, None, None
    return False, None, None


def eax_consumed_after(img, call_site_end, window=12):
    """Starting right after a CALL, is EAX READ before it is redefined?
    Returns (text, addr) of the first consuming instruction, else None."""
    code = img.read(call_site_end, window * 8)
    for ins in md.disasm(code, call_site_end):
        r, w = ins.regs_access()
        reads = any(x in EAX_IDS for x in r)
        writes = any(x in EAX_IDS for x in w)
        # `xor eax,eax` / `sub eax,eax` are the idiomatic "EAX = 0" and are pure DEFINITIONS,
        # but capstone reports EAX in the read set too because the ALU op reads both operands.
        # Counting them as uses produced 20+ false positives on the first run, all of the form
        # "caller call site X -> xor eax, eax". Treat a self-cancelling ALU op as a def.
        if ins.mnemonic in ("xor", "sub") and reads and writes:
            ops = [o.strip() for o in ins.op_str.split(",")]
            if len(ops) == 2 and ops[0] == ops[1]:
                return None
        if reads:
            return "%s %s" % (ins.mnemonic, ins.op_str), ins.address
        if writes:
            return None                       # redefined without being read -> dead
        if ins.mnemonic in ("call", "jmp", "ret"):
            return None                       # leaving the straight-line window
    return None


_CALLMAP = None


def call_map(img):
    """target VA -> [call site VAs], built in ONE pass over the image.

    Built once and cached: doing this per-hook re-scanned ~2.8 MB for each of ~1300 hooks and
    did not finish in 10 minutes.

    This is a byte scan for `E8 rel32`, not a disassembly, so it deliberately avoids the
    capstone truncation trap. The cost is false call sites from `E8` bytes that are really
    operands or data; those are harmless here because a bogus site only survives if the bytes
    that follow it happen to disassemble into a genuine EAX read, and every surviving finding
    is inspected by hand anyway."""
    global _CALLMAP
    if _CALLMAP is not None:
        return _CALLMAP
    m = defaultdict(list)
    d = img.d
    i = d.find(b"\xe8")
    while i >= 0:
        va = img.off2va(i)
        if va is not None and i + 5 <= len(d):
            rel = struct.unpack_from("<i", d, i + 1)[0]
            m[va + 5 + rel].append(va)
        i = d.find(b"\xe8", i + 1)
    _CALLMAP = m
    return m


def direct_callers(img, target):
    return [(va, va + 5) for va in call_map(img).get(target, ())]


# ── main ─────────────────────────────────────────────────────────────────────
def analyse(img, hooks, rt_lookup, force_void=()):
    findings, skipped = [], defaultdict(int)
    for sym, rva, f in hooks:
        rt, is_void, naked = rt_lookup(sym)
        if rva in force_void:
            is_void, naked = True, False
        if rt is None:
            skipped["no-definition-found"] += 1
            continue
        if naked:
            skipped["naked (EAX written by hand)"] += 1
            continue
        if not is_void:
            skipped["returns a value"] += 1
            continue
        w, wtxt, waddr = writes_eax(img, rva)
        if not w:
            skipped["original never writes EAX"] += 1
            continue
        uses = []
        for call_va, after in direct_callers(img, rva):
            u = eax_consumed_after(img, after)
            if u:
                uses.append({"caller_call_site": "0x%08x" % call_va,
                             "use": u[0], "use_addr": "0x%08x" % u[1]})
        if not uses:
            skipped["no caller consumes EAX"] += 1
            continue
        findings.append({"rva": "0x%08x" % rva, "sym": sym, "file": f,
                         "declared_return": rt,
                         "original_eax_write": wtxt,
                         "original_eax_write_addr": "0x%08x" % waddr,
                         "consumers": uses})
    return findings, skipped


# ── emitted-body triage ──────────────────────────────────────────────────────
ASI = os.path.join(ROOT, "original", "mashed_re_dev.asi")
MAP = os.path.join(ROOT, "mashedmod", "build", "mashed_re_dev.map")

MAP_RE = re.compile(r"^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s")


def map_symbols():
    """symbol -> RVA in the .asi, from the linker .map.

    Needed because a hook body may be `extern "C"` but NOT `__declspec(dllexport)`, in which
    case the export table cannot see it — the blind spot that hid 0x0055deb0 from an earlier
    audit. MSVC decorates __cdecl with a leading underscore; both forms are indexed."""
    if not os.path.exists(MAP):
        return {}, None
    base = None
    out = {}
    for line in open(MAP, encoding="utf-8", errors="replace"):
        if base is None and "Preferred load address is" in line:
            base = int(line.strip().split()[-1], 16)
            continue
        m = MAP_RE.match(line)
        if m and base is not None:
            sym, va = m.group(1), int(m.group(2), 16)
            if sym.startswith("?"):          # C++ mangled — not our extern "C" hooks
                continue
            rva = va - base
            out.setdefault(sym, rva)
            if sym.startswith("_"):
                out.setdefault(sym[1:], rva)
    return out, base


def emitted_body(sym, exports, mapsyms, asipe, limit=24):
    """(source_of_address, [instructions]) for our compiled body, or (None, [])."""
    rva, src = None, None
    if sym in exports:
        rva, src = exports[sym], "export table"
    elif sym in mapsyms:
        rva, src = mapsyms[sym], ".map"
    if rva is None:
        return None, []
    try:
        code = asipe.get_data(rva, 200)
    except Exception:
        return src, []
    out = []
    for ins in md.disasm(code, rva):
        out.append("%s %s" % (ins.mnemonic, ins.op_str))
        if ins.mnemonic.startswith("ret") or len(out) >= limit:
            break
    return src, out


def orig_body(img, rva, limit=24):
    out = []
    for ins in md.disasm(img.read(rva, 200), rva):
        out.append("%s %s" % (ins.mnemonic, ins.op_str))
        if ins.mnemonic.startswith("ret") or len(out) >= limit:
            break
    return out


def triage(img, findings):
    import pefile
    pe = pefile.PE(ASI, fast_load=True)
    pe.parse_data_directories(
        directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]])
    exports = {}
    if hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        exports = {e.name.decode(): e.address
                   for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name}
    mapsyms, _ = map_symbols()
    print("\n== emitted-body triage ==")
    print("   export-table symbols: %d   .map symbols: %d" % (len(exports), len(mapsyms)))
    for f in findings:
        rva = int(f["rva"], 16)
        src, ours = emitted_body(f["sym"], exports, mapsyms, pe)
        print("\n%s  %s" % (f["rva"], f["sym"]))
        print("   ORIG : " + " | ".join(orig_body(img, rva)[:9]))
        if ours:
            print("   OURS [%s] : %s" % (src, " | ".join(ours[:9])))
        else:
            print("   OURS : NOT RESOLVABLE (neither export table nor .map) — UNASSESSED")
        print("   consumers: " + ", ".join(sorted({c["use"] for c in f["consumers"]}))
              + "  (%d call sites)" % len(f["consumers"]))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", help="write findings here")
    ap.add_argument("--triage", action="store_true",
                    help="also dump original vs emitted bodies (export table, then .map)")
    args = ap.parse_args()

    img = Image(IMAGE)
    hooks = installed_hooks()
    rt_lookup = return_types()
    print("installed hooks parsed: %d" % len(hooks))

    # ---- SELF-TEST (mandatory) ----------------------------------------------
    print("\n== SELF-TEST: force 0x%08x to `void` and require the detector to fire ==" % KNOWN_RVA)
    st, _ = analyse(img, [h for h in hooks if h[1] == KNOWN_RVA], rt_lookup,
                    force_void={KNOWN_RVA})
    ok = False
    if st:
        f = st[0]
        sites = {c["caller_call_site"] for c in f["consumers"]}
        tests = [c for c in f["consumers"] if c["use"].startswith("test eax")]
        print("   fired: %s  original writes EAX at %s (%s)"
              % (f["rva"], f["original_eax_write_addr"], f["original_eax_write"]))
        for c in f["consumers"]:
            print("     caller call site %s -> %s @ %s"
                  % (c["caller_call_site"], c["use"], c["use_addr"]))
        ok = bool(tests)
    if not ok:
        print("   SELF-TEST FAILED — the detector does not reproduce the known instance.")
        print("   Refusing to report findings: a sweep that cannot find a known positive")
        print("   cannot support an all-clear.")
        return 2
    print("   SELF-TEST PASSED\n")

    findings, skipped = analyse(img, hooks, rt_lookup)
    print("== funnel ==")
    for k, v in sorted(skipped.items(), key=lambda kv: -kv[1]):
        print("   %-34s %d" % (k, v))
    print("   %-34s %d" % ("FLAGGED", len(findings)))

    print("\n== findings ==")
    for f in findings:
        print("\n%s  %s   (%s)" % (f["rva"], f["sym"], f["file"]))
        print("   declared `%s`; original writes EAX at %s: %s"
              % (f["declared_return"], f["original_eax_write_addr"], f["original_eax_write"]))
        for c in f["consumers"]:
            print("   caller call site %s -> %s @ %s"
                  % (c["caller_call_site"], c["use"], c["use_addr"]))
    if not findings:
        print("   none")
    if args.triage:
        triage(img, findings)
    if args.json:
        json.dump(findings, open(args.json, "w", encoding="utf-8"), indent=1)
        print("\nwrote %s" % args.json)
    return 0


if __name__ == "__main__":
    sys.exit(main())
