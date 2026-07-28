# sweep_const_width.py — audit float-constant reads in the ports against the ORIGINAL's
# actual x87 operand width.
#
# MOTIVATION (2026-07-28): AiSteeringAngleError (0x00415e20) read three constants through
# F32() that the original loads as QWORDs:
#     0x00415eba  dc0d 70c95c00   FMUL QWORD PTR [0x005cc970]   = 57.2958  (180/pi)
#     0x00415e85  dc15 d0d05c00   FCOM QWORD PTR [0x005cd0d0]   = -1.0
#     0x00415e9a  dc15 c8d05c00   FCOM QWORD PTR [0x005cd0c8]   = +1.0
# Reading a double through a float* silently yields the low half: 57.2958 -> 1.0842e-19,
# and the two acos-domain clamps became 0.0. The function returned a dead steering angle
# on every call, and that hook alone wedged a menu-navigated race.
#
# Neither register-ABI sweep can see this class, and a value-level Frida diff on the CALLER
# cannot either — both implementations are "just reading a global". The only ground truth is
# the operand width encoded in the instruction that references the address:
#     D8 /r  -> m32fp (dword)      DC /r  -> m64fp (qword)
#     D9 /r  -> fld/fst m32fp      DD /r  -> fld/fst m64fp
# capstone renders these as "dword ptr" / "qword ptr", so we can just read them off.
#
# READ-ONLY, offline. Self-test: --selftest audits the PRE-FIX AiTargeting.cpp out of git
# and asserts it flags exactly the three known constants.
import os, re, struct, subprocess, sys
from collections import defaultdict

try:
    import capstone
except ImportError:
    sys.exit("needs capstone")

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(ROOT, "original", "MASHED.exe.unpatched")
SRC = os.path.join(ROOT, "mashedmod", "src", "mashed_re")

# float reads at an absolute image address, in the shapes the ports actually use
PAT_F32 = [
    re.compile(r"\bF32\s*\(\s*(0x00[0-9a-fA-F]{6})u?\s*\)"),
    re.compile(r"reinterpret_cast\s*<\s*(?:const\s+)?float\s*\*\s*>\s*\(\s*(0x00[0-9a-fA-F]{6})u?\s*\)"),
    re.compile(r"\*\s*\(\s*(?:const\s+)?float\s*\*\s*\)\s*\(?\s*(0x00[0-9a-fA-F]{6})u?"),
]
PAT_F64 = [
    re.compile(r"\bF64\s*\(\s*(0x00[0-9a-fA-F]{6})u?\s*\)"),
    re.compile(r"reinterpret_cast\s*<\s*(?:const\s+)?double\s*\*\s*>\s*\(\s*(0x00[0-9a-fA-F]{6})u?\s*\)"),
]


def image_widths():
    """addr -> {'dword': n, 'qword': n} counted over every x87 memory reference."""
    data = open(EXE, "rb").read()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    text = None
    for i in range(nsec):
        s = pe + 24 + opt + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode(errors="replace")
        vsz, va, rsz, ro = struct.unpack_from("<IIII", data, s + 8)
        if name.startswith(".text"):
            text = (va, vsz, ro, rsz)
    va, vsz, ro, rsz = text
    blob = data[ro:ro + max(vsz, rsz)]
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    widths = defaultdict(lambda: defaultdict(int))
    sites = defaultdict(list)
    rx = re.compile(r"(dword|qword|tbyte|word) ptr \[(0x[0-9a-f]+)\]")
    # RESYNC LOOP — capstone's disasm() STOPS at the first undecodable byte, and .text is
    # full of jump tables and alignment padding. A single pass silently truncated the scan
    # (it stopped between 0x004055c9 and 0x00415e85, hiding two of the three constants the
    # self-test looks for). Skip one byte and continue until the section is consumed.
    start = base + va
    off, n = 0, len(blob)
    while off < n:
        decoded = 0
        for ins in md.disasm(blob[off:], start + off):
            decoded = ins.address - (start + off) + ins.size
            if not ins.mnemonic.startswith("f"):
                continue                   # x87 only: float semantics
            m = rx.search(ins.op_str)
            if not m:
                continue
            w, a = m.group(1), int(m.group(2), 16)
            widths[a][w] += 1
            if len(sites[a]) < 4:
                sites[a].append("0x%08x %s %s" % (ins.address, ins.mnemonic, ins.op_str))
        off += decoded if decoded else 1
    return widths, sites


def scan_sources(files=None, override=None):
    """-> list of (addr, kind, file, line). `override` maps path -> text (for --selftest)."""
    out = []
    targets = []
    if files:
        targets = files
    else:
        for dirpath, _, fs in os.walk(SRC):
            for fn in fs:
                if fn.endswith((".cpp", ".h", ".hpp")):
                    targets.append(os.path.join(dirpath, fn))
    for path in targets:
        rel = os.path.relpath(path, ROOT).replace("\\", "/")
        try:
            text = override[rel] if override and rel in override else open(
                path, encoding="utf-8", errors="replace").read()
        except Exception:
            continue
        lines = text.split("\n")
        for kind, pats in (("f32", PAT_F32), ("f64", PAT_F64)):
            for pat in pats:
                for m in pat.finditer(text):
                    ln = text.count("\n", 0, m.start()) + 1
                    out.append((int(m.group(1), 16), kind, rel, ln,
                                lines[ln - 1].strip()[:100] if ln - 1 < len(lines) else ""))
    return out


def audit(reads, widths):
    findings = []
    for addr, kind, rel, ln, snip in reads:
        w = widths.get(addr)
        if not w:
            continue                        # never touched by x87 in the original
        has_d, has_q = w.get("dword", 0), w.get("qword", 0)
        if kind == "f32" and has_q and not has_d:
            findings.append(("F32-READS-QWORD", addr, rel, ln, snip, has_d, has_q))
        elif kind == "f64" and has_d and not has_q:
            findings.append(("F64-READS-DWORD", addr, rel, ln, snip, has_d, has_q))
        elif kind == "f32" and has_q and has_d:
            findings.append(("MIXED", addr, rel, ln, snip, has_d, has_q))
    return findings


def main():
    widths, sites = image_widths()

    if "--selftest" in sys.argv:
        # audit the PRE-FIX file straight out of git; it must flag the three constants
        rel = "mashedmod/src/mashed_re/Ai/AiTargeting.cpp"
        try:
            old = subprocess.run(["git", "show", "HEAD:" + rel], cwd=ROOT,
                                 capture_output=True, text=True, encoding="utf-8").stdout
        except Exception as e:
            sys.exit("selftest needs git: %s" % e)
        if not old:
            sys.exit("selftest: could not read HEAD:%s" % rel)
        reads = scan_sources(files=[os.path.join(ROOT, rel)], override={rel: old})
        f = audit(reads, widths)
        got = sorted(set(x[1] for x in f if x[0] == "F32-READS-QWORD"))
        want = [0x005cc970, 0x005cd0c8, 0x005cd0d0]
        ok = got == want
        print("self-test against pre-fix HEAD:%s" % rel)
        print("  flagged: %s" % [hex(a) for a in got])
        print("  expected: %s" % [hex(a) for a in want])
        print("  %s" % ("PASS" if ok else "**FAIL**"))
        return 0 if ok else 1

    reads = scan_sources()
    findings = audit(reads, widths)
    by_kind = defaultdict(list)
    for f in findings:
        by_kind[f[0]].append(f)

    print("constant-width audit — port float reads vs the original's x87 operand width")
    print("  absolute float reads found in ports : %d" % len(reads))
    print("  addresses referenced by x87 in the original : %d\n" % len(widths))
    labels = {
        "F32-READS-QWORD": "BUG  port reads f32, original only ever loads QWORD (the 0x005cc970 class)",
        "F64-READS-DWORD": "BUG  port reads f64, original only ever loads DWORD",
        "MIXED": "check  original uses BOTH widths at this address",
    }
    for kind in ("F32-READS-QWORD", "F64-READS-DWORD", "MIXED"):
        rows = by_kind.get(kind, [])
        print("== %s : %d ==" % (labels[kind], len(rows)))
        seen = set()
        for _, addr, rel, ln, snip, hd, hq in sorted(rows, key=lambda r: (r[2], r[3])):
            print("  0x%08x  %s:%d   (orig dword=%d qword=%d)" % (addr, rel, ln, hd, hq))
            print("        %s" % snip)
            if addr not in seen:
                seen.add(addr)
                for s in sites.get(addr, [])[:2]:
                    print("        orig: %s" % s)
        print()


if __name__ == "__main__":
    sys.exit(main() or 0)
