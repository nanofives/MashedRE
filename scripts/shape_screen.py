#!/usr/bin/env py -3.12
"""shape_screen.py — can a synthetic A/B safely call this function?

The exercise pre-screen (prescreen_batch.py) answers "does the scenario CALL
this RVA". It does not answer "can we force-call it in a diff", and those are
very different questions. 2026-07-29: the eight smallest race-gated audio
candidates looked like an easy batch on size alone, and turned out to contain

  0x005b8080  call dword ptr [0x005cc088]   -> KERNEL32!CloseHandle(*(p+0xc))
  0x005aeed0  call dword ptr [0x005cc090]   -> KERNEL32!WaitForSingleObject(h,0)

Force-calling the first CLOSES A LIVE KERNEL HANDLE, twice per test vector
(once for the original, once for the reimpl). The second is a zero-timeout wait,
which ACQUIRES when signalled. Neither is a diff; both corrupt the running game.
Most of the rest were indirect dispatches through pointers that only exist in
live objects.

So: screen the SHAPE from disassembly before authoring anything. No boots, no
Ghidra — capstone over the pinned binary plus the PE import table.

Verdicts
  destructive     an imported call on the deny-list (handles, files, sync
                  objects). NEVER batch.
  imported        some other imported call — read it before deciding.
  indirect        calls through a register or a non-absolute memory operand, so
                  the target only exists in live data. Needs captured live
                  arguments (capture_args); a seeded buffer jumps to garbage.
  reg_arg         reads a register at entry before writing it => non-standard
                  register-argument convention, or the address is not a function
                  start at all. Needs a naked shim and a hand-written signature.
  direct          only direct E8 calls (or none). The authorable shape.

Usage:
  py -3.12 scripts/shape_screen.py <rva>[,<rva>...]
  py -3.12 scripts/shape_screen.py --from <tsv> [--class race_gated] [--out <tsv>]
"""
import io
import sys
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "original" / "MASHED.exe.unpatched"

# Imported calls that MUTATE process state when called. A synthetic A/B calls the
# target twice per vector, so any of these corrupts the live game rather than
# measuring it.
DESTRUCTIVE = {
    "CloseHandle", "WaitForSingleObject", "WaitForMultipleObjects",
    "ReleaseSemaphore", "ReleaseMutex", "SetEvent", "ResetEvent",
    "TerminateThread", "ExitProcess", "ExitThread", "FreeLibrary",
    "DeleteFileA", "DeleteFileW", "WriteFile", "SetFilePointer",
    "HeapFree", "VirtualFree", "LocalFree", "GlobalFree",
    "EnterCriticalSection", "LeaveCriticalSection", "DeleteCriticalSection",
    "fclose", "fwrite", "free", "_close", "_unlink",
}

SCRATCH = {"eax", "ecx", "edx", "ebx", "esi", "edi"}
CALLEE_SAVED = {"ebx", "esi", "edi", "ebp"}   # a push of one of these is a prologue save
SUB8 = {"al": "eax", "ah": "eax", "ax": "eax", "cl": "ecx", "ch": "ecx", "cx": "ecx",
        "dl": "edx", "dh": "edx", "dx": "edx", "bl": "ebx", "bh": "ebx", "bx": "ebx",
        "si": "esi", "di": "edi"}


def _norm(tok):
    tok = tok.strip()
    return SUB8.get(tok, tok)


def load():
    pe = pefile.PE(str(EXE))
    base = pe.OPTIONAL_HEADER.ImageBase
    data = pe.get_memory_mapped_image()
    imports = {}
    for e in pe.DIRECTORY_ENTRY_IMPORT:
        dll = e.dll.decode()
        for imp in e.imports:
            imports[imp.address] = (dll, imp.name.decode() if imp.name else f"#{imp.ordinal}")
    return base, data, imports


def body_end(va, base, data, next_start, maxlen=0x400):
    """Where does this function's body end?

    NOT at the first `ret`. A function with an early-out returns from the guard
    and then continues — 0x0047d150 rets at 0x0047d161 and calls 0x0057c210 at
    0x0047d16a, eight bytes later. Breaking on the first ret truncated it to
    "leaf (no calls)" and would have mis-verdicted it as safely direct.

    Bound by the next known function start from hooks.csv when there is one,
    otherwise stop at ANY alignment padding (nop / int3) following a ret. "Any",
    not "a run of 3+": 0x005b0f40 ends at 0x005b0f4e with a SINGLE nop before the
    next function at 0x005b0f50, and requiring 3 bytes swallowed that neighbour
    and mis-attributed its call to 0x005b0f70. Both errors are one-sided in
    opposite directions — break-at-first-ret truncates (missing calls, a
    false "safe"), over-running merges (inventing calls). One nop after a ret is
    padding; a real early-out continues immediately (0x0047d150 rets at 0x47d161
    and resumes at 0x47d162 with no gap at all).
    """
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    limit = min(next_start, va + maxlen) if next_start else va + maxlen
    off, end, seen_ret, pad = va - base, va, False, 0
    for i in md.disasm(data[off:off + (limit - va)], va):
        if i.mnemonic in ("nop", "int3"):
            pad += i.size
            if seen_ret and pad >= 1:
                break
            continue
        pad = 0
        end = i.address + i.size
        if i.mnemonic in ("ret", "retn"):
            seen_ret = True
    return end


def screen(va, base, data, imports, next_start=None, maxlen=0x200):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    out = {"rva": f"0x{va:08x}", "size": 0, "direct": [], "indirect": [],
           "imported": [], "destructive": [], "reg_arg": []}
    written = set()
    off = va - base
    end = body_end(va, base, data, next_start)
    out["size"] = end - va
    # A `push <callee-saved>` is a SAVE if the same register is POPped in this
    # body. That is the exact rule; position is not — 0x004c1be0, 0x00495080 and
    # 0x005a9de0 all push esi/edi several instructions in, after loading their
    # stack arguments, and all three pop it back before returning. Judging by
    # "is this the first instruction" mis-flagged every one of them as taking a
    # register argument, and reg_arg is the verdict used to SKIP a candidate.
    popped = {_norm(j.op_str) for j in md.disasm(data[off:off + (end - va)], va)
              if j.mnemonic == "pop"}
    for i in md.disasm(data[off:off + (end - va)], va):
        op = i.op_str
        # --- register-argument detection: a scratch register READ before written.
        #
        # THREE things that look like an argument read and are not. All three
        # produced false positives on the first pass, and reg_arg was the verdict
        # being used to SKIP candidates, so each one cost real work:
        #
        #  1. `push esi` / `push ebx` / `push edi` at the top of a body is the
        #     PROLOGUE SAVING a callee-saved register, not reading a parameter.
        #     0x004c0ed0, 0x0048fef0, 0x005a9de0 and 0x0047a0f0 all begin exactly
        #     that way and were all mis-flagged.
        #  2. EAX after a `call` holds the RETURN VALUE. `mov [0x007f1030], eax`
        #     following a call is storing a result, not consuming an argument
        #     (0x00495110, 0x004039c0, 0x0048bbe0).
        #  3. `xor r,r` / `sub r,r` is the zeroing idiom. It nominally reads the
        #     register but depends on nothing (0x00534920).
        prologue_save = (i.mnemonic == "push" and _norm(op) in CALLEE_SAVED
                         and _norm(op) in popped)
        zero_idiom = (i.mnemonic in ("xor", "sub") and "," in op
                      and _norm(op.split(",")[0]) == _norm(op.split(",")[1]))
        if not prologue_save and not zero_idiom and (
                i.mnemonic in ("push", "test", "cmp") or (
                i.mnemonic in ("mov", "add", "or", "and", "sub", "xor") and "," in op)):
            parts = [p.strip() for p in op.split(",")]
            srcs = parts[1:] if len(parts) > 1 else parts[:1]
            for s in srcs:
                s = _norm(s)
                if s in SCRATCH and s not in written:
                    out["reg_arg"].append(f"{i.address:08x} {i.mnemonic} {op}")
        if prologue_save or zero_idiom:
            d = _norm(op if i.mnemonic == "push" else op.split(",")[0])
            if d in SCRATCH: written.add(d)
        if "," in op and i.mnemonic in ("mov", "lea", "xor", "pop", "add", "sub",
                                        "movzx", "movsx", "imul", "and", "or"):
            d = _norm(op.split(",")[0])
            if d in SCRATCH:
                written.add(d)
        elif i.mnemonic == "pop":
            d = _norm(op)
            if d in SCRATCH:
                written.add(d)
        # --- call classification
        if i.mnemonic == "call":
            written.update(("eax", "ecx", "edx"))   # defined/clobbered by the callee
            if op.startswith("0x"):
                out["direct"].append(op)
            elif op.startswith("dword ptr [0x"):
                addr = int(op[len("dword ptr ["):-1], 16)
                if addr in imports:
                    dll, name = imports[addr]
                    tag = f"{dll}!{name}"
                    (out["destructive"] if name in DESTRUCTIVE else out["imported"]).append(tag)
                else:
                    out["indirect"].append(f"[0x{addr:08x}]")
            else:
                out["indirect"].append(op)
    if out["destructive"]:   out["verdict"] = "destructive"
    elif out["imported"]:    out["verdict"] = "imported"
    elif out["indirect"]:    out["verdict"] = "indirect"
    elif out["reg_arg"]:     out["verdict"] = "reg_arg"
    else:                    out["verdict"] = "direct"
    return out


def main():
    base, data, imports = load()
    rvas = []
    if "--from" in sys.argv:
        src = sys.argv[sys.argv.index("--from") + 1]
        want = (sys.argv[sys.argv.index("--class") + 1] if "--class" in sys.argv else None)
        for line in io.open(ROOT / src, encoding="utf-8"):
            if not line.startswith("0x"): continue
            f = line.rstrip("\n").split("\t")
            if want and len(f) > 1 and f[1] != want: continue
            rvas.append((int(f[0], 16), f[3] if len(f) > 3 else ""))
    else:
        rvas = [(int(x, 16), "") for x in sys.argv[1].split(",")]

    # next known function start bounds each body (hooks.csv is the function map)
    import csv as _csv
    starts = sorted({int(r["rva"], 16) for r in
                     _csv.DictReader(io.open(ROOT / "hooks.csv", encoding="utf-8"))
                     if r["rva"] and not r["rva"].startswith("#")
                     and int(r["rva"], 16) >= 0x400000})
    import bisect
    def nxt(va):
        j = bisect.bisect_right(starts, va)
        return starts[j] if j < len(starts) else None
    res = [(screen(va, base, data, imports, nxt(va)), sub) for va, sub in rvas]
    order = {"direct": 0, "reg_arg": 1, "indirect": 2, "imported": 3, "destructive": 4}
    res.sort(key=lambda r: (order[r[0]["verdict"]], r[0]["size"]))
    print(f"{'rva':12s} {'verdict':12s} {'sz':>4s} {'sub':10s} detail")
    for r, sub in res:
        det = ""
        if r["destructive"]: det = "!! " + ",".join(sorted(set(r["destructive"])))
        elif r["imported"]:  det = ",".join(sorted(set(r["imported"])))
        elif r["indirect"]:  det = "via " + ",".join(sorted(set(r["indirect"]))[:3])
        elif r["reg_arg"]:   det = "entry-read: " + r["reg_arg"][0]
        elif r["direct"]:    det = "calls " + ",".join(r["direct"][:4])
        else:                det = "leaf (no calls)"
        print(f"{r['rva']:12s} {r['verdict']:12s} {r['size']:4d} {sub:10s} {det}")
    if "--out" in sys.argv:
        p = ROOT / sys.argv[sys.argv.index("--out") + 1]
        with io.open(p, "w", encoding="utf-8") as f:
            f.write("rva\tverdict\tsize\tsubsystem\tdetail\n")
            for r, sub in res:
                det = (",".join(sorted(set(r["destructive"] or r["imported"] or
                                           r["indirect"] or r["direct"]))) or "leaf")
                f.write(f"{r['rva']}\t{r['verdict']}\t{r['size']}\t{sub}\t{det}\n")
        print(f"\nwrote {p}")


if __name__ == "__main__":
    main()
