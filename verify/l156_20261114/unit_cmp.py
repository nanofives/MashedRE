"""L156-REGCONV-EVIDENCE: unit-level vector compare of the VS2022 body against the original's bytes (Unicorn).

    py -3.12 verify/l156_20261114/unit_cmp.py [--runs 400] [--cw 0x037f] [--only 0x00427e00]

Why not equiv.py / equiv_dyn.py alone: both pair ONE function of our object with ONE original function and stub every
call. The question here crosses a call: the original unit passes its arguments to a private-convention helper in
registers (0x00427680 ESI/EDI, 0x004277a0 EAX/EBX, 0x00427840 EAX/ECX, 0x00458570 EAX/ESI, verify/ddkregconv7_20261113
RESULT section 1), and the VS2022 unit calls the exe's cdecl body of that helper. So this harness executes the unit
AND its helpers on both sides and stubs only what lies outside:

  original side: the original bytes of the unit plus the original helpers (ranges from re/console/pc_funcs.csv).
  ours side:     tu/unit_<rva>.cpp compiled by match.py's vc19 toolchain (cl 19.44, VS2022), all of its sections
                 (unit + Port_ helper bodies) at 0x20000000+, relocations patched as equiv_dyn.Layout does. For
                 0x00458920 the helper is the original 0x00458570 on both sides, reached through the exe's register
                 adapter (gen_tus.py ADAPTER_458570).

Memory layout, stubs and the observable set are equiv_dyn.py's (Layout, run model, page filler), with these changes:
  * MGLOBAL under C++ is a `static T &` whose slot the exe fills at dynamic init. A relocation to a DAT_ symbol defined
    in our object is that slot: it is mapped at its own address and pre-filled with the DAT_ address (equiv_dyn maps
    it to the original address itself, a double dereference the exe never does).
  * Stub records replace a stack-address argument by the bytes the callee reads there (short string up to its 0, 4,
    8 or 12 bytes per callee below), because the two compilers place locals at different stack offsets.
  * Stubs with outputs the units consume: 0x00427780 returns a length-prefixed short string (length 0..127, the
    size of the units' text[128]), RW slot +0xf4 (strlen) returns the real length of its argument, 0x0046d4a0 writes
    a heap pointer through its first argument, 0x005554d0 / 0x004c3ac0 / 0x004c39b0 return a float in ST0.
  * Inputs: arguments from per-unit generators (align 0..3, strings that fit the buffers, alpha around 0x80/0x17f,
    0x00458920's records with the gating fields hit), globals 0x0067d830..0x0067d83c, 0x007d3ff8, 0x008990e4,
    0x007f0fd0 seeded.
Observables compared: stub records in order, every non-stack byte written, ESP, EBX/ESI/EDI/EBP, fault, x87 TOP.
EAX/ECX/EDX are not compared: all five units are void.
"""
from __future__ import annotations

import argparse
import hashlib
import pathlib
import random
import struct
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent.parent
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import equiv  # noqa: E402
import equiv_dyn as dyn  # noqa: E402
import match  # noqa: E402
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_WRITE, \
    UC_HOOK_MEM_READ_UNMAPPED, UC_HOOK_MEM_WRITE_UNMAPPED, UC_PROT_ALL  # noqa: E402
from unicorn import x86_const as R  # noqa: E402

M32 = 0xFFFFFFFF
PAGE = 0x1000
SLOTS = 0x23000000
RWTAB = 0x52000000
FAKE_RS, FAKE_STRLEN = 0x53000000, 0x53000010
STRTAB = 0x54000000
STRARG = 0x55000000
RECS = 0x56000000
FSTUB = 0x57000000      # fld dword [FSTUB+0x100]; ret  (float-returning stubs)
FLOAT_RET = {0x005554d0, 0x004c3ac0, 0x004c39b0}
# __security_check_cookie (re/console/pc_funcs.csv; cmp ecx,[0x00616038] at 0x004a2be9): the original units are /GS
# builds, the vc19 compile here is /GS-, so this call is not recorded (it only aborts on a smashed cookie).
COOKIE_CHECK = 0x004a2be9
# --ignore-ecx-556ca0: 0x00556ca0 does not read ECX (0x00556ca0..0x00556cb0, disassembled) but tail-jumps through
# [DAT_0067d838 + 0x138], a target no static evidence pins (U-5674), so equiv.summary keeps ECX live-in.
IGNORE_LIVEIN = {}

HELPERS = {   # original helper entries executed on the original side
    0x00427e00: [0x00427680, 0x004277a0],
    0x00427f00: [0x00427680, 0x00427840],
    0x00427ff0: [0x00427680, 0x004277a0],
    0x00428140: [0x00427680, 0x004277a0],
    0x00458920: [0x00458570],
}
# per-callee: stack-pointer argument index -> how many bytes the callee reads ("s" = short string up to its 0)
PTRSPEC = {
    0x00556e90: {1: 4, 2: 4, 3: 4, 4: 4},
    0x00556ca0: {1: "s", 3: 8},
    0x005554d0: {1: "s"},
    0x004c3ac0: {0: 12},
    0x004c39b0: {0: 12, 1: 12},
    0x0046d4a0: {0: 0},
}


def fbits(f):
    return struct.unpack("<I", struct.pack("<f", f))[0]


def frange(addr):
    t = match.function_table()
    later = [e for e in t if e > addr]
    return (addr, min(later))


class UnitLayout(dyn.Layout):
    def __init__(self, *a, **k):
        self.slots = {}
        super().__init__(*a, **k)

    def target(self, symrec, field):
        name = symrec.get("name", "?")
        sec = symrec.get("sec", 0)
        if sec == 0 and name.startswith("_Port_"):
            # cl emits an undefined record (the declaration in namespace U) next to the defined one (namespace H<i>):
            # both name the same extern "C" function, so bind the reloc to the definition
            d = next((s for s in self.syms if s.get("name") == name and s.get("sec", 0) > 0), None)
            if d is not None:
                symrec, sec = d, d["sec"]
        u = equiv.undecorate(name)
        if sec > 0 and sec != self.ours_sec and u.startswith("DAT_"):
            key = (sec, symrec["value"])
            if key not in self.slots:
                self.slots[key] = (SLOTS + 4 * len(self.slots), int(u[4:12], 16))
            return (self.slots[key][0] + field) & M32
        return super().target(symrec, field)

    def build(self, uc):
        super().build(uc)
        uc.mem_map(SLOTS, PAGE, UC_PROT_ALL)
        for addr, val in self.slots.values():
            uc.mem_write(addr, struct.pack("<I", val))
        uc.mem_map(FSTUB, PAGE, UC_PROT_ALL)
        uc.mem_write(FSTUB, bytes([0xD9, 0x05]) + struct.pack("<I", FSTUB + 0x100) + b"\xC3")


def build_ours(rva, tu=None):
    src = pathlib.Path(tu) if tu else HERE / "tu" / f"unit_{rva:08x}.cpp"
    text = src.read_text()
    obj = match.compile_obj(src, match.file_flags(text), match.file_lang(text), match.file_toolchain(text))
    secs, syms = equiv.coff_full(obj)
    for s in syms:      # undefined _Port_ records (see UnitLayout.target) take the definition's section and offset
        if s.get("sec", 0) == 0 and s.get("name", "").startswith("_Port_"):
            d = next((x for x in syms if x.get("name") == s["name"] and x.get("sec", 0) > 0), None)
            if d is not None:
                s["sec"], s["value"] = d["sec"], d["value"]
    sym = dict(match.parse_annotations(text))[rva]
    res = equiv.Resolver(text, secs, syms, rva)
    A = equiv.ObjSide("ours", secs, syms, sym, rva, res)
    layout = UnitLayout(secs, syms, res, A.secidx)
    entry = dyn.OURS_BASE + A.start
    ours_ranges = [(b, b + len(d)) for i, d in layout.patched.items() for b in [layout.base_of(i)]]
    return layout, entry, ours_ranges


def seed_inputs(rng, rva):
    """(stack args, memory writes) for one vector."""
    mem = []
    rnd_f = lambda lo, hi: fbits(rng.uniform(lo, hi))  # noqa: E731
    mem.append((0x007d3ff8, struct.pack("<I", RWTAB)))
    tab = bytearray(0x100)
    struct.pack_into("<I", tab, 0x20, FAKE_RS)
    struct.pack_into("<I", tab, 0xf4, FAKE_STRLEN)
    mem.append((RWTAB, bytes(tab)))
    mem.append((0x0067d830, struct.pack("<ffII", rng.choice([1.0, 1.3333334, rng.uniform(0.5, 2.0)]),
                                       rng.choice([1.0, 0.75, rng.uniform(0.5, 2.0)]),
                                       rng.getrandbits(32), rng.getrandbits(32))))
    mem.append((0x008990e4, struct.pack("<I", rng.choice([0, 0, 1, rng.getrandbits(32)]))))
    align = rng.choice([0, 1, 2, 3, rng.getrandbits(32)])
    x, y = rnd_f(-50, 700), rnd_f(-50, 520)
    color = rng.getrandbits(32)
    scale = rnd_f(0.05, 4.0)
    if rva in (0x00427e00, 0x00427ff0, 0x00428140):
        args = [rng.getrandbits(16), x, y, color, scale, align]
        if rva == 0x00427ff0:
            args.append(rng.getrandbits(32))
        if rva == 0x00428140:
            args.append(rng.choice([rng.randrange(-16, 0x220), rng.choice([0x80, 0x81, 0xff, 0x100, 0x17f, 0x180])]))
    elif rva == 0x00427f00:
        n = rng.randrange(0, 500)
        s = bytes(rng.randrange(1, 256) for _ in range(n)) + b"\0"
        mem.append((STRARG, s))
        args = [STRARG, x, y, color, scale, align]
    else:
        p1, rec, p2 = RECS, RECS + 0x1000, RECS + 0x2000
        b1 = bytearray(0x40)
        struct.pack_into("<II", b1, 0x10, 7 if rng.random() < 0.85 else rng.getrandbits(32), rec)
        br = bytearray(0x80)
        struct.pack_into("<I", br, 0, RECS + 0x3000)
        struct.pack_into("<f", br, 0x18, rng.uniform(-100, 100))
        struct.pack_into("<I", br, 0x20, 1 if rng.random() < 0.85 else rng.getrandbits(32))
        struct.pack_into("<I", br, 0x24, rng.getrandbits(32))
        b2 = bytearray(0x40)
        struct.pack_into("<I", b2, 0x10, rng.choice([10, 10, 0xfeff, 0xff08, rng.getrandbits(32)] +
                                                   list(range(0xff00, 0xff08)) * 2))
        bx = bytearray(0x80)
        struct.pack_into("<I", bx, 4, RECS + 0x4000)
        by = [struct.pack("<f", rng.uniform(-50, 50)) for _ in range(0x20)]
        mem += [(p1, bytes(b1)), (rec, bytes(br)), (p2, bytes(b2)), (RECS + 0x3000, bytes(bx)),
                (RECS + 0x4000, b"".join(by)),
                (0x007f0fd0, struct.pack("<I", rng.choice([5, 5, 0, rng.getrandbits(32)])))]
        args = [p1, p2]
    return args, mem


def read_ptr_arg(uc, v, how):
    if how == 0:
        return ("out",)
    if how == "s":
        out = bytearray()
        for i in range(700):
            w = bytes(uc.mem_read(v + 2 * i, 2))
            out += w
            if w == b"\0\0":
                break
        return ("s", bytes(out))
    return ("b", bytes(uc.mem_read(v, how)))


def run_one(layout, entry, ranges, args, mem, cw):
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    layout.build(uc)
    rng_stack = hashlib.blake2b(b"stack", digest_size=64).digest() * ((dyn.STACK_HI - dyn.STACK_LO) // 64)
    uc.mem_write(dyn.STACK_LO, rng_stack)
    uc.mem_write(dyn.ESP0, struct.pack("<I", dyn.SENTINEL))
    for i, a in enumerate(args):
        uc.mem_write(dyn.ESP0 + 4 + 4 * i, struct.pack("<I", a & M32))
    for addr, data in mem:
        dyn.seeded_write(uc, addr, data)
    for (name, rid) in dyn.REGS:
        uc.reg_write(rid, 0x0BAD0000 | (rid & 0xff))
    uc.reg_write(R.UC_X86_REG_ESP, dyn.ESP0)
    uc.reg_write(R.UC_X86_REG_FPCW, cw)
    writes, events, pending = {}, [], []

    def allowed(a):
        return any(lo <= a < hi for lo, hi in ranges) or FSTUB <= a < FSTUB + 16

    def on_unmapped(uc_, access, addr, size, value, _):
        base = addr & ~(PAGE - 1)
        try:
            uc_.mem_map(base, PAGE, UC_PROT_ALL)
            uc_.mem_write(base, dyn.page_bytes(base))
        except UcError:
            return False
        if size and (addr + size - 1) & ~(PAGE - 1) != base:
            try:
                uc_.mem_map(base + PAGE, PAGE, UC_PROT_ALL)
                uc_.mem_write(base + PAGE, dyn.page_bytes(base + PAGE))
            except UcError:
                pass
        return True

    def on_write(uc_, access, addr, size, value, _):
        if dyn.STACK_LO <= addr < dyn.STACK_HI:
            return
        for i in range(size):
            writes[addr + i] = (value >> (8 * i)) & 0xFF

    def on_code(uc_, addr, size, _):
        if not allowed(addr):
            pending.append(addr)
            uc_.emu_stop()

    uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED | UC_HOOK_MEM_WRITE_UNMAPPED, on_unmapped)
    uc.hook_add(UC_HOOK_MEM_WRITE, on_write)
    uc.hook_add(UC_HOOK_CODE, on_code)
    pc, fault = entry, None
    while True:
        pending.clear()
        try:
            uc.emu_start(pc, 0, 5_000_000, 400000)
        except UcError as e:
            eip = uc.reg_read(R.UC_X86_REG_EIP)
            if not pending and "FETCH_UNMAPPED" in str(e) and not allowed(eip):
                pending.append(eip)
            elif not pending:
                fault = (str(e), eip if not allowed(eip) else "in-body")
                break
        if not pending:
            return ("inconclusive", "instruction limit")
        addr = pending[0]
        if addr == dyn.SENTINEL:
            break
        esp = uc.reg_read(R.UC_X86_REG_ESP)
        ret = struct.unpack("<I", uc.mem_read(esp, 4))[0]
        tgt = equiv.resolve_thunk(addr) if equiv.image().in_image(addr) else addr
        livein = ()
        if tgt == FAKE_RS:
            P, N, name = 0, 2, "rw+0x20"
        elif tgt == FAKE_STRLEN:
            P, N, name = 0, 1, "rw+0xf4"
        elif tgt < dyn.OURS_BASE:
            s = equiv.summary(tgt)
            P = s["P"] if s["P"] is not None else (equiv.ret_pop(tgt) or 0)
            N = s["N"] if s["N"] is not None else 0
            livein = tuple(sorted(s["livein"])) if s.get("known") else ()
            name = tgt
        else:
            P, N = 0, 0
            name = next((n for n, v in layout.ext.items() if v == addr), hex(addr))
        if tgt in PTRSPEC:
            N = max(N, max(PTRSPEC[tgt]) + 1)
        raw = [struct.unpack("<I", uc.mem_read(esp + 4 + 4 * i, 4))[0] for i in range(N)]
        norm = []
        for i, v in enumerate(raw):
            if dyn.STACK_LO <= v < dyn.STACK_HI:
                norm.append(("stk",) + read_ptr_arg(uc, v, PTRSPEC.get(tgt, {}).get(i, 4)))
            else:
                norm.append(v)
        rid = dict(dyn.REGS)
        lv = []
        for r in livein:
            if r in IGNORE_LIVEIN.get(tgt, ()):
                continue
            v = uc.reg_read(rid[r])
            lv.append((r, ("stk",) if dyn.STACK_LO <= v < dyn.STACK_HI else v))
        rec = (name, tuple(norm), tuple(lv))
        if tgt != COOKIE_CHECK:
            events.append(rec)
        h = hashlib.blake2b(repr(rec).encode(), digest_size=16).digest()
        w = struct.unpack_from("<I", h, 0)[0]
        eax = [0, 6, 10, 1][h[0] % 4] if h[1] % 2 == 0 else w
        if tgt == FAKE_STRLEN:
            s = bytes(uc.mem_read(raw[0], 600))
            eax = s.index(b"\0") if b"\0" in s else 600
        elif tgt == 0x00427780:
            n = h[2] % 128
            base = STRTAB + (h[3] % 16) * PAGE
            body = [n] + [[8, 9, 10, 11, 12, 13, 14, 0x41, 0x20, h[(5 + k) % 16] * 37 % 0x10000][h[(4 + k) % 16] % 10]
                          for k in range(n)]
            dyn.seeded_write(uc, base, struct.pack(f"<{len(body)}H", *[b & 0xffff for b in body]))
            eax = base
        elif tgt == 0x0046d4a0:
            uc.mem_write(raw[0], struct.pack("<I", dyn.HEAP + (w % 0x100) * PAGE))
        uc.reg_write(R.UC_X86_REG_EAX, eax)
        uc.reg_write(R.UC_X86_REG_ECX, struct.unpack_from("<I", h, 4)[0])
        uc.reg_write(R.UC_X86_REG_EDX, struct.unpack_from("<I", h, 8)[0])
        if tgt in FLOAT_RET:
            uc.mem_write(FSTUB + 0x100, struct.pack("<f", (w % 100000) / 1000.0 - 50.0))
            pc = FSTUB                      # fld; ret (cdecl: the caller pops the arguments)
            continue
        uc.reg_write(R.UC_X86_REG_ESP, esp + 4 + P)
        if ret == dyn.SENTINEL:
            break
        pc = ret
        if len(events) > 300:
            return ("inconclusive", "too many calls")
    rid = dict(dyn.REGS)
    return {"events": events, "writes": writes, "fault": fault,
            "regs": {n: uc.reg_read(rid[n]) for n in ("ebx", "esi", "edi", "ebp")},
            "esp": uc.reg_read(R.UC_X86_REG_ESP), "top": (uc.reg_read(R.UC_X86_REG_FPSW) >> 11) & 7}


def compare(a, b):
    if isinstance(a, tuple) or isinstance(b, tuple):
        return f"inconclusive: {a if isinstance(a, tuple) else b}"
    for k in ("fault", "esp", "top", "regs"):
        if a[k] != b[k]:
            return f"differs: {k}: {a[k]} | {b[k]}"
    if len(a["events"]) != len(b["events"]):
        return f"differs: call count {len(a['events'])} | {len(b['events'])}"
    for i, (x, y) in enumerate(zip(a["events"], b["events"])):
        if x != y:
            tname = hex(x[0]) if isinstance(x[0], int) else x[0]
            if x[0] != y[0] or len(x[1]) != len(y[1]):
                return f"differs: call {i}: target/arity {tname} | {y[0]}"
            for j, (p, q) in enumerate(zip(x[1], y[1])):
                if p != q:
                    fx = lambda v: (f"0x{v:08x} ({struct.unpack('<f', struct.pack('<I', v))[0]!r})"  # noqa: E731
                                    if isinstance(v, int) else (v[0], v[1], v[2].hex() if len(v) > 2 else ""))
                    return f"differs: call {i} to {tname} arg {j}: ours {fx(p)} | orig {fx(q)}"
            return f"differs: call {i} to {tname} live-in regs: {x[2]} | {y[2]}"
    if a["writes"] != b["writes"]:
        ks = sorted({k for k in set(a["writes"]) | set(b["writes"]) if a["writes"].get(k) != b["writes"].get(k)})
        return f"differs: memory at 0x{ks[0]:08x} ({len(ks)} bytes)"
    return "same"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--runs", type=int, default=400)
    ap.add_argument("--cw", default="0x037f")
    ap.add_argument("--only", default="")
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--ignore-ecx-556ca0", action="store_true")
    ap.add_argument("--tu", default="", help="negative control: compile this TU instead of tu/unit_<rva>.cpp (one --only)")
    a = ap.parse_args()
    if a.ignore_ecx_556ca0:
        IGNORE_LIVEIN[0x00556ca0] = {"ecx"}
    cw = int(a.cw, 16)
    rvas = [int(x, 16) for x in a.only.split(",")] if a.only else list(HELPERS)
    for rva in rvas:
        layout, entry, ours_ranges = build_ours(rva, a.tu or None)
        orig_ranges = [frange(rva)] + [frange(h) for h in HELPERS[rva]]
        if rva == 0x00458920:
            ours_ranges = ours_ranges + [frange(0x00458570)]
        rng = random.Random(a.seed)
        tally = {"same": 0, "differs": 0, "inconclusive": 0}
        first, ncalls = None, 0
        for _ in range(a.runs):
            args, mem = seed_inputs(rng, rva)
            ro = run_one(layout, entry, ours_ranges, args, mem, cw)
            rg = run_one(layout, rva, orig_ranges, args, mem, cw)
            c = compare(ro, rg)
            k = c.split(":")[0]
            tally[k] += 1
            if not isinstance(rg, tuple):
                ncalls += len(rg["events"])
            if k != "same" and first is None:
                first = (c, [hex(x) for x in args])
        print(f"0x{rva:08x}\tcw={a.cw}\tignore_ecx_556ca0={int(a.ignore_ecx_556ca0)}\truns={a.runs}\tsame={tally['same']}\tdiffers={tally['differs']}\t"
              f"inconclusive={tally['inconclusive']}\tcalls(orig)={ncalls}")
        if first:
            print(f"  first non-same: {first[0]}\n  vector args: {first[1]}")
        sys.stdout.flush()
    return 0


if __name__ == "__main__":
    sys.exit(main())
