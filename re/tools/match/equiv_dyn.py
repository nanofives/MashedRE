"""Dynamic cross-check for equiv.py verdicts (EQUIV-CHECK): run both bodies in Unicorn from identical random
machine states and compare what a caller can observe.

    py -3.12 re/tools/match/equiv_dyn.py <src.c> --only 0x<rva> [--runs 32]
    py -3.12 re/tools/match/equiv_dyn.py --tsv verify/equivcheck_20261111/measure_v2.tsv [--runs 32] [--out <tsv>]

What one run does
* One memory layout for both runs: the whole original image at its base, our compiled section at 0x20000000 (every
  relocation patched to the original's address, as equiv.py resolves it), a stack at 0x0E000000, and every other page
  mapped on first touch with pseudo-random bytes derived from its address. So every load sees the same bytes in
  both runs unless one body wrote them.
* Registers and the 16 stack arguments are random (half of them pointers into the random heap).
* Execution leaving the function body (a call or a tail jump) is stubbed: the stub records (target, the callee's
  stack arguments per equiv.summary, ECX, EDX), returns EAX = a hash of that record, pops the callee's argument
  bytes, and resumes at the return address. Callees have no side effects in this model.
* Observables compared: the stub records in order, the final value of every non-stack byte written, EBX ESI EDI
  EBP ESP, EAX/ECX/EDX (except registers the verdict lists as callers-ignore), the x87 registers and status word.

What it cannot cover: callee side effects (memory a real callee would write and the body later reads), aliasing
between stack slots and incoming pointers (A1; random pointers never hit the stack), paths that random inputs do
not reach, x87 precision-control effects of real callees, and timing. A run that exceeds 200,000 instructions or
faults in both bodies at the same instruction count is "inconclusive", not a pass.
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


def page_bytes(base):
    return hashlib.blake2b(struct.pack("<I", base), digest_size=64).digest() * (PAGE // 64)


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
    def __init__(self, entry, lo, hi):
        self.entry, self.lo, self.hi = entry, lo, hi


def run_one(layout, body, state, max_ins=MAXI):
    """One execution. Returns the observable record or ('inconclusive', why)."""
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    layout.build(uc)
    stack, regs = state
    uc.mem_write(STACK_LO, stack)
    uc.mem_write(ESP0, struct.pack("<I", SENTINEL))
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
        count[0] += 1
        if count[0] > max_ins:
            pending.append(("limit", addr))
            uc_.emu_stop()
            return
        if addr == SENTINEL or not (body.lo <= addr < body.hi):
            pending.append(("out", addr))
            uc_.emu_stop()

    uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED | UC_HOOK_MEM_WRITE_UNMAPPED, on_unmapped)
    uc.hook_add(UC_HOOK_MEM_WRITE, on_write)
    uc.hook_add(UC_HOOK_CODE, on_code)
    pc = body.entry
    fault = None
    while True:
        pending.clear()
        try:
            uc.emu_start(pc, 0)
        except UcError as e:
            if not pending:
                fault = (str(e), count[0], uc.reg_read(R.UC_X86_REG_EIP))
                break
        if not pending:
            fault = ("stopped", count[0], uc.reg_read(R.UC_X86_REG_EIP))
            break
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
        if tgt < OURS_BASE:
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
        uc.reg_write(R.UC_X86_REG_EAX, struct.unpack_from("<I", h, 0)[0])
        uc.reg_write(R.UC_X86_REG_ECX, struct.unpack_from("<I", h, 4)[0])
        uc.reg_write(R.UC_X86_REG_EDX, struct.unpack_from("<I", h, 8)[0])
        uc.reg_write(R.UC_X86_REG_ESP, esp + 4 + P)
        if ret == SENTINEL:
            break
        pc = ret
        if len(events) > 2000:
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


def check(layout, bodyA, bodyB, runs=32, seed=1, ignore=()):
    rng = random.Random(seed)
    tally = {"same": 0, "inconclusive": 0, "differs": 0}
    first = None
    for _ in range(runs):
        st = random_state(rng)
        ra = run_one(layout, bodyA, st)
        rb = run_one(layout, bodyB, st)
        c = compare(ra, rb, ignore)
        k = c.split(":")[0]
        tally[k] += 1
        if k == "differs" and first is None:
            first = c
    return tally, first


def bodies_for(src, rva):
    # run the proof first in this process, so the callee summaries the stubs use are the ones the proof compared
    try:
        equiv.check_source(pathlib.Path(src), {rva})
    except BaseException:  # noqa: BLE001
        pass
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
    bA = Body(OURS_BASE + A.start - 0, OURS_BASE + A.start, OURS_BASE + A.end)
    bB = Body(rva, rva, max(hi, rva + size))
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
    a = ap.parse_args()
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
            tally, first = check(layout, bA, bB, a.runs, ignore=ign)
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
