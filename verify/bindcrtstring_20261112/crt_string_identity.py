"""BIND-CRT-STRING: code-level identity of the original's strchr / strrchr / wcsncpy against the DDK libcmt objects.

Two legs per function, both read-only on the anchored image (original/MASHED.exe.unpatched):
  1. normalized listing: disassemble the DDK object's whole .text section and the same span of the image, rewrite
     `inc r` -> `add r, 1` and `dec r` -> `sub r, 1`, map every branch target to an instruction index, and require the
     two listings to be equal. Each rewritten site must be followed by an instruction that writes CF before anything
     reads it (inc/dec leave CF, add/sub set it), checked mechanically.
  2. differential run: unicorn executes the image bytes and the DDK object bytes on the same inputs (random strings,
     every start alignment 0..3, c = 0, c > 0xff, absent c, last-byte hits) and compares the returned pointer with the
     C-standard result computed in Python.

  py -3.12 verify/bindcrtstring_20261112/crt_string_identity.py [--n 4000]
"""
import argparse
import pathlib
import random
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import coffpe  # noqa: E402
import capstone  # noqa: E402
from capstone import x86  # noqa: E402
import unicorn  # noqa: E402
from unicorn import x86_const as U  # noqa: E402

LIB = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\tools\vc7ddk\lib\wxp\i386\libcmt.lib")
CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
CS.detail = True

FUNCS = [  # (image va, object member suffix, symbol)
    (0x004a2fd0, "strchr.obj", "_strchr"),
    (0x004a3480, "strrchr.obj", "_strrchr"),
    (0x004a3bdf, "wcsncpy.obj", "_wcsncpy"),
]
CF_WRITERS = {"add", "sub", "cmp", "test", "and", "or", "xor", "neg", "shl", "shr", "sar", "scasb", "scasw", "scasd",
              "cmpsb", "cmpsw", "cmpsd", "repne scasb", "repe scasb", "repne scasw", "repe scasw"}
CF_READERS_PREFIX = ("jb", "jae", "jbe", "ja", "jc", "jnc", "adc", "sbb", "setb", "setae", "setc", "setnc", "cmovb",
                     "cmovae", "rcl", "rcr", "jnae", "jnb", "jna", "jnbe")


def member(name):
    for mname, data in coffpe.archive_members(LIB.read_bytes()):
        if mname.lower().endswith("\\" + name) or mname.lower().endswith("/" + name) or mname.lower() == name:
            return coffpe.Obj(data, mname)
    raise SystemExit(f"{name} not in {LIB}")


def norm(insns):
    idx = {i.address: k for k, i in enumerate(insns)}
    out, sites = [], []
    for k, i in enumerate(insns):
        m, op = i.mnemonic, i.op_str
        if m in ("inc", "dec") and len(i.operands) == 1 and i.operands[0].type == x86.X86_OP_REG:
            m, op = ("add" if m == "inc" else "sub"), op + ", 1"
            sites.append(k)
        if i.group(capstone.CS_GRP_JUMP) and i.operands and i.operands[0].type == x86.X86_OP_IMM:
            t = i.operands[0].imm
            op = f"@{idx[t]}" if t in idx else f"ext"
        out.append(f"{m} {op}")
    return out, sites


def cf_safe(insns, k):
    for j in range(k + 1, len(insns)):
        m = insns[j].mnemonic
        if m.startswith(CF_READERS_PREFIX):
            return False, f"{m} reads CF at +{j - k}"
        if m in CF_WRITERS:
            return True, m
        if insns[j].group(capstone.CS_GRP_JUMP) or m == "ret":
            return False, f"control flow ({m}) before a CF writer"
    return False, "end of listing"


def listing_leg(img, va, objname, sym):
    obj = member(objname)
    fn = [f for f in obj.functions() if f["name"] == sym][0]
    sec = obj.sections[fn["sec"] - 1]
    relocs = [r for r in sec["relocs"]]
    data = sec["data"].rstrip(b"\xcc\x90")
    base = va - fn["off"]
    a = list(CS.disasm(data, 0))
    # the image span is the same instruction count, read generously and cut by count
    b_all = list(CS.disasm(img.read(base, len(data) + 64), base))
    b = b_all[:len(a)]
    na, sa = norm(a)
    nb, sb = norm(b)
    # branch indices are computed per listing, so recompute nb's with the cut listing
    eq = na == nb
    first = next((k for k in range(min(len(na), len(nb))) if na[k] != nb[k]), None)
    raw_identical = data == img.read(base, len(data))
    # only sites where the two listings spell the instruction differently (inc/dec on one side, add/sub on the other)
    cf = [(k, *cf_safe(a if k in sa else b, k)) for k in sorted(set(sa) ^ set(sb))]
    return {
        "sym": sym, "obj": obj.name, "va": va, "span": (base, base + sum(i.size for i in b)), "lib_bytes": len(data),
        "img_bytes": sum(i.size for i in b), "relocs": len(relocs), "insns": len(a), "raw_identical": raw_identical,
        "normalized_equal": eq, "first_diff": first, "lib_inc_dec": len(sa), "img_inc_dec": len(sb),
        "cf_checks": cf, "lib_code": data, "lib_entry": fn["off"],
        "first_pair": (na[first], nb[first]) if first is not None else None,
    }


class Emu:
    CODE_IMG, CODE_LIB, DATA, STACK, STOP = 0x00400000, 0x10000000, 0x20000000, 0x30000000, 0x40000000

    def __init__(self, img, va, lib_code):
        self.mu = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
        page = va & ~0xfff
        self.mu.mem_map(page, 0x4000)
        self.mu.mem_write(page, img.read(page, 0x4000))
        self.mu.mem_map(self.CODE_LIB, 0x1000)
        self.mu.mem_write(self.CODE_LIB, lib_code)
        self.mu.mem_map(self.DATA, 0x10000)
        self.mu.mem_map(self.STACK, 0x10000)
        self.mu.mem_map(self.STOP, 0x1000)
        self.mu.mem_write(self.STOP, b"\xc3")

    def call(self, entry, args):
        esp = self.STACK + 0x8000
        for a in reversed(args):
            esp -= 4
            self.mu.mem_write(esp, struct.pack("<I", a & 0xffffffff))
        esp -= 4
        self.mu.mem_write(esp, struct.pack("<I", self.STOP))
        self.mu.reg_write(U.UC_X86_REG_ESP, esp)
        self.mu.reg_write(U.UC_X86_REG_EFLAGS, 0x202)
        for r in (U.UC_X86_REG_EBX, U.UC_X86_REG_ESI, U.UC_X86_REG_EDI, U.UC_X86_REG_EBP):
            self.mu.reg_write(r, 0x5a5a5a5a)
        self.mu.emu_start(entry, self.STOP, count=200000)
        regs = {r: self.mu.reg_read(r) for r in (U.UC_X86_REG_EBX, U.UC_X86_REG_ESI, U.UC_X86_REG_EDI)}
        assert all(v == 0x5a5a5a5a for v in regs.values()), "callee-saved register clobbered"
        assert self.mu.reg_read(U.UC_X86_REG_ESP) == esp + 4, "stack imbalance (cdecl)"
        return self.mu.reg_read(U.UC_X86_REG_EAX)


def rand_str(rng):
    n = rng.choice([0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 64, rng.randint(0, 200)])
    alpha = rng.choice([b"ab", b"abc/\\.", bytes(range(1, 256))])
    return bytes(rng.choice(alpha) for _ in range(n))


def diff_leg(img, leg, n, seed=1):
    rng = random.Random(seed)
    emu = Emu(img, leg["va"], leg["lib_code"])
    lib_entry = Emu.CODE_LIB + leg["lib_entry"]
    bad, runs = [], 0
    sym = leg["sym"]
    for t in range(n):
        if sym == "_wcsncpy":
            src = [rng.randint(1, 0xffff) for _ in range(rng.randint(0, 40))]
            cnt = rng.randint(0, 50)
            res = []
            for entry in (leg["va"], lib_entry):
                emu.mu.mem_write(Emu.DATA, b"\xee" * 0x400)
                emu.mu.mem_write(Emu.DATA + 0x800, b"".join(struct.pack("<H", w) for w in src) + b"\0\0")
                r = emu.call(entry, [Emu.DATA, Emu.DATA + 0x800, cnt])
                res.append((r, bytes(emu.mu.mem_read(Emu.DATA, 0x100))))
            exp = (src + [0] * cnt)[:cnt] if cnt > len(src) else src[:cnt]
            exp_b = b"".join(struct.pack("<H", w) for w in exp) + b"\xee" * (0x100 - 2 * len(exp))
            want = (Emu.DATA, exp_b)
            runs += 1
            if res[0] != want or res[1] != want:
                bad.append((t, src, cnt))
            continue
        s = rand_str(rng)
        align = t % 4
        c = rng.choice([0, 0x100 + rng.randint(0, 255), rng.randint(1, 255)] + ([s[rng.randrange(len(s))]] * 3 if s else []))
        addr = Emu.DATA + 0x100 + align
        emu.mu.mem_write(Emu.DATA, b"\x11" * 0x400)
        emu.mu.mem_write(addr, s + b"\0")
        ch = c & 0xff
        if sym == "_strchr":
            k = (s + b"\0").find(bytes([ch]))
        else:
            k = (s + b"\0").rfind(bytes([ch]))
        want = 0 if k < 0 else addr + k
        got_img = emu.call(leg["va"], [addr, c])
        got_lib = emu.call(lib_entry, [addr, c])
        runs += 1
        if got_img != want or got_lib != want:
            bad.append((t, s[:24], c, hex(want), hex(got_img), hex(got_lib)))
    return runs, bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=4000)
    a = ap.parse_args()
    img = coffpe.Image()
    rc = 0
    for va, objname, sym in FUNCS:
        leg = listing_leg(img, va, objname, sym)
        runs, bad = diff_leg(img, leg, a.n)
        cf_ok = all(ok for _, ok, _ in leg["cf_checks"])
        verdict = ("IDENTICAL" if leg["raw_identical"] else
                   "EQUIVALENT-INC-ADD" if leg["normalized_equal"] and cf_ok else "DIFFERENT")
        if bad or verdict == "DIFFERENT":
            rc = 1
        print(f"{sym} va=0x{va:08x} obj={leg['obj']} span=0x{leg['span'][0]:08x}..0x{leg['span'][1]:08x} "
              f"insns={leg['insns']} lib_bytes={leg['lib_bytes']} img_bytes={leg['img_bytes']} relocs={leg['relocs']}")
        print(f"  listing: raw_identical={leg['raw_identical']} normalized_equal={leg['normalized_equal']} "
              f"first_diff={leg['first_diff']} {leg['first_pair'] or ''} inc/dec lib={leg['lib_inc_dec']} "
              f"img={leg['img_inc_dec']}")
        for k, ok, why in leg["cf_checks"]:
            print(f"  cf insn#{k}: {'ok' if ok else 'FAIL'} ({why})")
        print(f"  differential: {runs} runs, {len(bad)} mismatches" + (f" first={bad[0]}" if bad else ""))
        print(f"  VERDICT {sym}: {verdict}{' + DIFF-RUN-CLEAN' if not bad else ' + DIFF-RUN-FAIL'}")
    return rc


if __name__ == "__main__":
    sys.exit(main())
