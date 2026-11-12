"""Dynamic cross-check for equiv.py verdicts (EQUIV-CHECK): run both bodies in Unicorn from identical random
machine states and compare what a caller can observe.

    py -3.12 re/tools/match/equiv_dyn.py <src.c> --only 0x<rva> [--runs 32]
    py -3.12 re/tools/match/equiv_dyn.py --tsv verify/equivcheck_20261111/measure_v2.tsv [--runs 32] [--out <tsv>]
    ... [--seeded] [--seed N]   seeded inputs (EQUIV-ADMIT-1): sparse pages (one dword in 8 nonzero: a small count, a
                                heap-page pointer or a hash word), the per-RVA builders of equiv_seeds.py, and stubs
                                that return EAX = 0 one call in four. For rows whose uniform-random runs never end
                                (circular lists, loops on a callee result). The comparison is unchanged.
A `call [IAT slot]` is stubbed with the import's own pop count (from its __imp_ decoration), so __stdcall imports
leave the stack balanced (EQUIV-ADMIT-1; before, 0x005a8890 ended every run 20 bytes low and inconclusive).

What one run does
* One memory layout for both runs: the whole original image at its base, our compiled section at 0x20000000 (every
  relocation patched to the original's address, as equiv.py resolves it), a stack at 0x0E000000, and every other page
  mapped on first touch with pseudo-random bytes derived from its address. So every load sees the same bytes in
  both runs unless one body wrote them.
* Registers and the 16 stack arguments are random (half of them pointers into the random heap).
* Execution leaving the function body (a call or a tail jump) is stubbed: the stub records (target, the callee's
  stack arguments per equiv.summary, ECX, EDX), returns EAX = a hash of that record, pops the callee's argument
  bytes, and resumes at the return address. Callees have no side effects in this model. A call through a pointer
  pops the count A6 pinned for that site (equiv.pin_pops, the same count the proof used) and records P/4 arguments
  (EQUIV-A6-5BE9F0-DYN); an unpinned one pops what the target's summary says (0 outside the image).
* Observables compared: the stub records in order, the final value of every non-stack byte written, EBX ESI EDI
  EBP ESP, EAX/ECX/EDX (except registers the verdict lists as callers-ignore), the x87 registers and status word.

What it cannot cover: callee side effects (memory a real callee would write and the body later reads), aliasing
between stack slots and incoming pointers (A1; random pointers never hit the stack), paths that random inputs do
not reach, x87 precision-control effects of real callees, and timing. A run with a leg over 3 s or 200,000
instructions between stubs, or more than 300 calls, is "inconclusive", not a pass.
"""
from __future__ import annotations

import argparse
import hashlib
import pathlib
import random
import struct
import sys

from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_WRITE, \
    UC_HOOK_MEM_READ_UNMAPPED, UC_HOOK_MEM_WRITE_UNMAPPED, UC_HOOK_MEM_FETCH_UNMAPPED, UC_PROT_ALL
from unicorn import x86_const as R

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import importlib, os  # noqa: E402
equiv = importlib.import_module(os.environ.get("EQUIV_MODULE", "equiv"))  # noqa: E402
import match  # noqa: E402
import equiv_seeds  # noqa: E402

M32 = 0xFFFFFFFF
OURS_BASE = 0x20000000
DATA_BASE = 0x21000000
EXT_BASE = 0x30000000
STACK_LO, STACK_HI = 0x0E000000, 0x0E100000
ESP0 = 0x0E0F0000
SENTINEL = 0x0DFF0000
HEAP = 0x50000000
PAGE = 0x1000
MAXI = 200000
REGS = [("eax", R.UC_X86_REG_EAX), ("ecx", R.UC_X86_REG_ECX), ("edx", R.UC_X86_REG_EDX), ("ebx", R.UC_X86_REG_EBX),
        ("esi", R.UC_X86_REG_ESI), ("edi", R.UC_X86_REG_EDI), ("ebp", R.UC_X86_REG_EBP)]
FPREGS = [getattr(R, f"UC_X86_REG_FP{i}") for i in range(8)]


SEEDED = False   # --seeded: sparse pages (EQUIV-ADMIT-1), so list walks and loop bounds read from memory terminate


def page_bytes(base):
    if not SEEDED:
        return hashlib.blake2b(struct.pack("<I", base), digest_size=64).digest() * (PAGE // 64)
    # Seeded-input mode: a page is zero except one dword in 8, which is a small count (0..7), a pointer to the start of
    # another heap page, or a hash word, chosen from a hash of the page address. NULL next pointers and small counts
    # end the list walks and loops that make the uniform-random picture inconclusive.
    out = bytearray(PAGE)
    h = hashlib.blake2b(struct.pack("<I", base), digest_size=64).digest()
    k = 0
    for off in range(0, PAGE, 32):
        if k + 4 > len(h):
            h = hashlib.blake2b(h, digest_size=64).digest()
            k = 0
        sel, w = h[k], struct.unpack_from("<I", h, k)[0]
        k += 4
        if sel < 96:
            v = w & 7
        elif sel < 160:
            v = HEAP + (w % 0x100) * PAGE
        elif sel < 192:
            v = w
        else:
            v = 0
        struct.pack_into("<I", out, off, v)
    return bytes(out)


class Layout:
    """The shared memory picture: image + our patched section(s) + stack + lazily mapped random pages."""

    def __init__(self, secs, syms, res, ours_sec):
        self.img = equiv.image()
        self.secs, self.syms, self.res = secs, syms, res
        self.ours_sec = ours_sec
        self.sec_base = {ours_sec: OURS_BASE}
        self.ext = {}
        self.patched = self.patch()

    def base_of(self, secidx):
        if secidx not in self.sec_base:
            self.sec_base[secidx] = DATA_BASE + 0x10000 * len(self.sec_base)
        return self.sec_base[secidx]

    def target(self, symrec, field):
        name = symrec.get("name", "?")
        sec = symrec.get("sec", 0)
        a = self.res.sym_address(name) if sec != self.ours_sec else None
        if a is not None and name.startswith("__imp_"):
            m = __import__("re").search(r"@(\d+)$", name)
            self.res.import_pop[(a + field) & M32] = int(m.group(1)) if m else 0
        if a is not None:
            # a name that carries the original's address (FUN_/DAT_/MGLOBAL/symmap) goes to the original, as in
            # equiv.py, even when this object defines a body of that name
            return (a + field) & M32
        if sec > 0:
            key = (sec, symrec["value"] + field)
            if sec != self.ours_sec and key in self.res.odata_map:
                return self.res.odata_map[key]
            return (self.base_of(sec) + symrec["value"] + field) & M32
        a = self.res.sym_address(name)
        if a is not None:
            return (a + field) & M32
        if name not in self.ext:
            self.ext[name] = EXT_BASE + 0x10 * len(self.ext)
        return (self.ext[name] + field) & M32

    def patch(self):
        out = {}
        for idx in list(self.sec_base):
            pass
        todo = [self.ours_sec]
        done = set()
        while todo:
            idx = todo.pop()
            if idx in done:
                continue
            done.add(idx)
            sec = self.secs[idx - 1]
            data = bytearray(sec["data"])
            base = self.base_of(idx)
            by_index = {s["index"]: s for s in self.syms}
            for off, si, typ in sec["relocs"]:
                t = by_index.get(si, {})
                if t.get("sec", 0) > 0 and t["sec"] not in done:
                    todo.append(t["sec"])
                field = struct.unpack_from("<i", data, off)[0]
                tgt = self.target(t, field)
                if typ == 0x06:
                    struct.pack_into("<I", data, off, tgt)
                elif typ == 0x14:
                    struct.pack_into("<i", data, off, ((tgt - (base + off + 4)) + (1 << 31)) % (1 << 32) - (1 << 31))
            out[idx] = bytes(data)
        return out

    def build(self, uc):
        img = self.img
        size = (len(img.data) + PAGE - 1) & ~(PAGE - 1)
        uc.mem_map(img.base, size, UC_PROT_ALL)
        uc.mem_write(img.base, bytes(img.data))
        for idx, data in self.patched.items():
            b = self.base_of(idx)
            uc.mem_map(b, (len(data) + PAGE) & ~(PAGE - 1), UC_PROT_ALL)
            uc.mem_write(b, data)
        uc.mem_map(STACK_LO, STACK_HI - STACK_LO, UC_PROT_ALL)
        uc.mem_map(SENTINEL, PAGE, UC_PROT_ALL)
        uc.mem_write(SENTINEL, b"\xf4" * 16)


class Body:
    def __init__(self, entry, lo, hi, pins=None):
        self.entry, self.lo, self.hi = entry, lo, hi
        self.pins = pins or {}      # return address of a call through a pointer -> its A6 pop count (EQUIV-A6-5BE9F0-DYN)


def run_one(layout, body, state, max_ins=MAXI):
    """One execution. Returns the observable record or ('inconclusive', why)."""
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    layout.build(uc)
    stack, regs = state[0], state[1]
    seed = state[2] if len(state) > 2 else None
    uc.mem_write(STACK_LO, stack)
    uc.mem_write(ESP0, struct.pack("<I", SENTINEL))
    if seed:
        for addr, data in seed.get("mem", ()):
            seeded_write(uc, addr, data)
    for (name, rid), v in zip(REGS, regs):
        uc.reg_write(rid, v)
    uc.reg_write(R.UC_X86_REG_ESP, ESP0)
    writes = {}
    events = []
    pending = []
    count = [0]

    def on_unmapped(uc_, access, addr, size, value, _):
        if access == 21 or access == 18 or access == 20:      # fetch kinds differ between versions: handled below
            pass
        base = addr & ~(PAGE - 1)
        try:
            uc_.mem_map(base, PAGE, UC_PROT_ALL)
            uc_.mem_write(base, page_bytes(base))
        except UcError:
            return False
        if size and (addr + size - 1) & ~(PAGE - 1) != base:
            nb = base + PAGE
            try:
                uc_.mem_map(nb, PAGE, UC_PROT_ALL)
                uc_.mem_write(nb, page_bytes(nb))
            except UcError:
                pass
        return True

    def on_write(uc_, access, addr, size, value, _):
        if STACK_LO <= addr < STACK_HI:
            return
        for i in range(size):
            writes[addr + i] = (value >> (8 * i)) & 0xFF

    def on_code(uc_, addr, size, _):
        # hooked only outside the body: reaching here is a call or a jump out of it
        pending.append(("out", addr))
        uc_.emu_stop()

    uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED | UC_HOOK_MEM_WRITE_UNMAPPED, on_unmapped)
    uc.hook_add(UC_HOOK_MEM_WRITE, on_write)
    if body.lo > 0:
        uc.hook_add(UC_HOOK_CODE, on_code, None, 0, body.lo - 1)
    uc.hook_add(UC_HOOK_CODE, on_code, None, body.hi, 0xFFFFFFFF)
    pc = body.entry
    fault = None
    while True:
        pending.clear()
        try:
            uc.emu_start(pc, 0, 3_000_000, max_ins)      # 3 s or 200,000 instructions per leg
        except UcError as e:
            if not pending:
                eip = uc.reg_read(R.UC_X86_REG_EIP)
                if "FETCH_UNMAPPED" in str(e) and not (body.lo <= eip < body.hi):
                    # a call through a pointer into unmapped memory: stub it like any other external call
                    pending.append(("out", eip))
                else:
                    fault = (str(e), count[0], eip)
                    break
        if not pending:
            return ("inconclusive", f"instruction limit at eip 0x{uc.reg_read(R.UC_X86_REG_EIP):08x}")
        kind, addr = pending[0]
        if kind == "limit":
            return ("inconclusive", "instruction limit")
        if addr == SENTINEL:
            break
        # a call or tail jump out of the body: stub it
        esp = uc.reg_read(R.UC_X86_REG_ESP)
        ret = struct.unpack("<I", uc.mem_read(esp, 4))[0]
        tgt = equiv.resolve_thunk(addr) if equiv.image().in_image(addr) else addr
        name = next((n for n, v in layout.ext.items() if v == addr), None)
        livein = ()
        # a `call dword ptr [IAT slot]` (FF 15 <slot>) reaches the slot's file value, not a function: take the pop
        # count from the import's decoration (equiv.Resolver.import_pop) and name the record by the import, so a
        # __stdcall import does not leave the stack unbalanced (EQUIV-ADMIT-1: 0x005a8890's WaitForSingleObject /
        # ReleaseSemaphore left ESP 20 bytes low and every run ended inconclusive)
        imp = None
        try:
            cb = bytes(uc.mem_read((ret - 6) & M32, 6))
            if cb[0] == 0xFF and cb[1] == 0x15:
                slot = struct.unpack_from("<I", cb, 2)[0]
                if slot in getattr(layout.res, "import_pop", {}):
                    imp = slot
        except UcError:
            pass
        if imp is None and ret in body.pins:
            # a call through a pointer whose pop count A6 pinned from the body's own stack equations (equiv.pin_pops):
            # the stub pops what the proof assumed. Before, a pointer target outside the image popped 0, so a
            # __stdcall method call left ESP 4 bytes low per call, and a body that reads an argument after the call
            # read the saved registers instead (0x005be9f0: 2 false DIFFERS from push order alone)
            P = body.pins[ret]
            N = P // 4
        elif imp is not None:
            P = layout.res.import_pop[imp]
            N = P // 4
            name = "imp:" + "!".join(equiv.image().iat.get(imp, ("?", hex(imp))))
        elif tgt < OURS_BASE:
            s = equiv.summary(tgt)
            P = s["P"] if s["P"] is not None else (equiv.ret_pop(tgt) or 0)
            N = s["N"] if s["N"] is not None else 0
            livein = tuple(sorted(s["livein"])) if s.get("known") else ()
        else:
            P, N = 0, 0
        args = tuple(struct.unpack("<I", uc.mem_read(esp + 4 + 4 * i, 4))[0] for i in range(N))
        rid = dict(REGS)
        rec = (name or tgt, args, tuple((r, uc.reg_read(rid[r])) for r in livein))
        events.append(rec)
        h = hashlib.blake2b(repr(rec).encode(), digest_size=12).digest()
        uc.reg_write(R.UC_X86_REG_EAX, 0 if SEEDED and h[0] % 4 == 0 else struct.unpack_from("<I", h, 0)[0])
        if seed and seed.get("effect"):
            for addr, data in seed["effect"](tgt, args, h):
                seeded_write(uc, addr, data)
        uc.reg_write(R.UC_X86_REG_ECX, struct.unpack_from("<I", h, 4)[0])
        uc.reg_write(R.UC_X86_REG_EDX, struct.unpack_from("<I", h, 8)[0])
        uc.reg_write(R.UC_X86_REG_ESP, esp + 4 + P)
        if ret == SENTINEL:
            break
        pc = ret
        if len(events) > 300:
            return ("inconclusive", "too many calls")
    out = {
        "events": events,
        "writes": writes,
        "fault": fault[0] if fault else None,
        "regs": {n: uc.reg_read(rid) for n, rid in REGS},
        "esp": uc.reg_read(R.UC_X86_REG_ESP),
        "fp": tuple(bytes(uc.reg_read(r)[1].to_bytes(2, "little") + uc.reg_read(r)[0].to_bytes(8, "little"))
                    if isinstance(uc.reg_read(r), tuple) else uc.reg_read(r) for r in FPREGS),
        "fpsw": uc.reg_read(R.UC_X86_REG_FPSW),
        "count": count[0],
    }
    return out


def seeded_write(uc, addr, data):
    """Write seed bytes, mapping (with the page filler) any page not mapped yet."""
    for base in range(addr & ~(PAGE - 1), addr + len(data), PAGE):
        try:
            uc.mem_map(base, PAGE, UC_PROT_ALL)
            uc.mem_write(base, page_bytes(base))
        except UcError:
            pass
    uc.mem_write(addr, bytes(data))


def seeded_state(rng, rva):
    """random_state, then the RVA's seed (equiv_seeds.SEEDS) overrides the stack arguments and registers it names."""
    stack, regs = random_state(rng)
    fn = equiv_seeds.SEEDS.get(rva) if SEEDED else None
    if fn is None:
        return stack, regs
    sd = fn(rng)
    st = bytearray(stack)
    for i, v in enumerate(sd.get("args", ()), 1):
        struct.pack_into("<I", st, ESP0 - STACK_LO + 4 * i, v & M32)
    regs = list(regs)
    for k, v in sd.get("regs", {}).items():
        regs[[n for n, _ in REGS].index(k)] = v & M32
    return bytes(st), regs, sd


def random_state(rng):
    stack = bytearray(rng.getrandbits(8) for _ in range(STACK_HI - STACK_LO)) if False else \
        bytearray(hashlib.blake2b(struct.pack("<Q", rng.getrandbits(64)), digest_size=64).digest() *
                  ((STACK_HI - STACK_LO) // 64))
    def val():
        r = rng.random()
        if r < 0.5:
            return HEAP + rng.randrange(0, 0x100000) * 4
        if r < 0.7:
            return rng.randrange(0, 64)
        if r < 0.85:
            return struct.unpack("<I", struct.pack("<f", rng.uniform(-100, 100)))[0]
        return rng.getrandbits(32)
    off = ESP0 - STACK_LO
    for i in range(1, 17):
        struct.pack_into("<I", stack, off + 4 * i, val())
    regs = [val() for _ in REGS]
    return bytes(stack), regs


def compare(a, b, ignore=()):
    if isinstance(a, tuple) or isinstance(b, tuple):
        return "inconclusive"
    for k in ("events", "fault", "esp", "fp", "fpsw"):
        if a[k] != b[k]:
            return f"differs: {k}: {str(a[k])[:120]} | {str(b[k])[:120]}"
    if a["writes"] != b["writes"]:
        ks = sorted(set(a["writes"]) ^ set(b["writes"]) | {k for k in a["writes"] if a["writes"].get(k) != b["writes"].get(k)})
        return f"differs: memory at 0x{ks[0]:08x} ({len(ks)} bytes)"
    for n, _ in REGS:
        if n in ignore:
            continue
        if a["regs"][n] != b["regs"][n]:
            return f"differs: {n} 0x{a['regs'][n]:08x} | 0x{b['regs'][n]:08x}"
    return "same"


def check(layout, bodyA, bodyB, runs=32, seed=1, ignore=(), rva=None):
    rng = random.Random(seed)
    tally = {"same": 0, "inconclusive": 0, "differs": 0}
    first = None
    for _ in range(runs):
        st = seeded_state(rng, rva)
        ra = run_one(layout, bodyA, st)
        rb = run_one(layout, bodyB, st)
        c = compare(ra, rb, ignore)
        k = c.split(":")[0]
        tally[k] += 1
        if k == "differs" and first is None:
            first = c
    return tally, first


def bodies_for(src, rva):
    # callee summaries are order-independent (equiv.summary), so the stubs see the facts the proof compared
    text = pathlib.Path(src).read_text()
    obj = match.compile_obj(pathlib.Path(src), match.file_flags(text) or match.DEFAULT_FLAGS, match.file_lang(text),
                            match.file_toolchain(text))
    secs, syms = equiv.coff_full(obj)
    sym = dict((a, s) for a, s in match.parse_annotations(text))[rva]
    res = equiv.Resolver(text, secs, syms, rva)
    A = equiv.ObjSide("ours", secs, syms, sym, rva, res)
    size = match.ghidra_size(rva)
    B = equiv.ImageSide("orig", rva, size)
    try:
        equiv.verify_odata(text, A, B, res)
    except Exception:
        pass
    layout = Layout(secs, syms, res, A.secidx)
    t = match.function_table()
    later = [e for e in t if e > rva]
    hi = min(later) if later else rva + size
    # A6 pop counts of the calls through a pointer, keyed by the call's return address in the Unicorn layout (ours:
    # section offset + OURS_BASE). Only uniquely pinned sites: a free one keeps the old stub (pop from the target)
    ctx = equiv.Ctx(A, B, False, res)
    pins = {}
    for side, base in ((A, OURS_BASE), (B, 0)):
        pp = {}
        try:
            for pc, P in ctx.pins(side).get("pins", {}).items():
                pp[base + pc + side.decode(pc).size] = P
        except Exception:  # noqa: BLE001  (no pins: the stub falls back to the target's pop)
            pp = {}
        pins[side.name] = pp
    bA = Body(OURS_BASE + A.start - 0, OURS_BASE + A.start, OURS_BASE + A.end, pins["ours"])
    bB = Body(rva, rva, max(hi, rva + size), pins["orig"])
    # self-recursion in ours resolves to the original's entry: treat that entry as inside ours too
    return layout, bA, bB


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src", nargs="?")
    ap.add_argument("--only", default="")
    ap.add_argument("--tsv", default="")
    ap.add_argument("--runs", type=int, default=32)
    ap.add_argument("--out", default="")
    ap.add_argument("--verdicts", default="EQUIV,EQUIV-IF-CALLERS-IGNORE")
    ap.add_argument("--shard", default="0/1")
    ap.add_argument("--seeded", action="store_true", help="sparse seeded memory pages (see page_bytes)")
    ap.add_argument("--seed", type=int, default=1)
    a = ap.parse_args()
    global SEEDED
    SEEDED = a.seeded
    rows = []
    want = set(a.verdicts.split(","))
    si, sn = (int(x) for x in a.shard.split("/"))
    if a.tsv:
        for k, ln in enumerate(pathlib.Path(a.tsv).read_text().splitlines()[1:]):
            f = ln.split("\t")
            if f[7] in want and k % sn == si:
                rows.append((f[1], int(f[0], 16), tuple(x for x in f[9].split(",") if x)))
    else:
        rows.append((a.src, int(a.only, 16), ()))
    out = ["rva\tsource\tverdict_regs_ignored\tsame\tinconclusive\tdiffers\tfirst_difference"]
    for i, (src, rva, ign) in enumerate(rows, 1):
        try:
            layout, bA, bB = bodies_for(src, rva)
            tally, first = check(layout, bA, bB, a.runs, seed=a.seed, ignore=ign, rva=rva)
            status = "DIFFERS" if tally["differs"] else ("SAME" if tally["same"] else "INCONCLUSIVE")
        except BaseException as e:  # noqa: BLE001  (a failed compile raises SystemExit)
            tally, first, status = {"same": 0, "inconclusive": 0, "differs": 0}, f"error: {str(e).splitlines()[0] if str(e) else type(e).__name__}"[:200], "ERROR"
        print(f"[{i}/{len(rows)}] 0x{rva:08x} {status} same={tally['same']} inc={tally['inconclusive']} "
              f"diff={tally['differs']} {first or ''}", flush=True)
        out.append(f"0x{rva:08x}\t{src}\t{','.join(ign)}\t{tally['same']}\t{tally['inconclusive']}\t{tally['differs']}\t{first or ''}")
    if a.out:
        pathlib.Path(a.out).write_text("\n".join(out) + "\n")


if __name__ == "__main__":
    main()
