#!/usr/bin/env py -3.12
"""semantic_screen.py — third screen: is this function SAFE and DETERMINISTIC
to force-call, according to what its own analysis plate says it does?

Screen 1 (prescreen_batch.py)  "does the scenario call it"
Screen 2 (shape_screen.py)     "is it mechanically callable"  — indirect
                               dispatch, destructive IMPORTS, register args
Screen 3 (this)                "is calling it twice harmless and repeatable"

Screen 2 catches KERNEL32!CloseHandle because the import table names it. It
cannot catch `FUN_004b7480(DAT_006bf1e0)` — a plain direct call that happens to
RELEASE the live DirectInput device. Measured 2026-07-29 on the six smallest
`direct` menu-reachable candidates, only two survived this screen:

  0x0047b880  FUN_004b7480(dev); dev = 0      teardown — releases the device
  0x0047b860  dev = FUN_004b7330(0); ...      initialiser — overwrites it
  0x004039c0  DAT_00636c00 = load(Bomb DFF)   asset load — allocates, overwrites
  0x00495110  DAT_007f1030 = timer()          NON-DETERMINISTIC: the two sides
                                              of an A/B read the clock at
                                              different instants, so a correct
                                              port reads RED

That last one is not a safety problem but a diffability one, and it is the
subtler trap: nothing about the disassembly looks dangerous.

The evidence is the analysis plate, which usually says so outright — 0x0047b880's
reads "the teardown counterpart to FUN_0047b860".

Usage:
  py -3.12 scripts/semantic_screen.py --from <shape tsv> [--verdict direct]
"""
import glob
import io
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

# Each pattern is a REASON not to force-call, matched against the plate text.
HAZARDS = [
    ("teardown",      r"\bteardown\b|\bshutdown\b|\brelease[sd]?\b|\bdestro|\bfree[sd]?\b|"
                      r"\bcleanup\b|\bclose[sd]?\b|\bunload"),
    ("initialiser",   r"\binit(ialis|ializ)?(e|es|ed|ation)?\b|\bcreate[sd]?\b|\ballocat|"
                      r"\bconstruct|\bregister[sd]?\b"),
    ("asset-load",    r"\bload(s|ed|er|ing)?\b|\.dff|\.txd|\.piz|\.rws|\bstream(s|ed)?\b"),
    ("non-determ",    r"\btimer\b|\bclock\b|\brandom\b|\brng\b|\bframe.count|\btick count|"
                      r"\bQueryPerformance|\bGetTickCount"),
    ("writes-global", r"DAT_[0-9a-f]{6,8}\s*=|=\s*FUN_[0-9a-f]{6,8}\(.*\).*;.*DAT_"),
]

_plates = None


def disasm_hazards(rva):
    """Hazards the PLATE may not mention but the bytes do.

    Found by reading disassembly for the seven candidates this screen had just
    called SAFE — 0x00496940 does `call 0x4c77c0` (with 0x84 pushed as a size)
    then `mov [0x0077307c], eax`, i.e. it ALLOCATES AND PUBLISHES an object, and
    its plate never says "init" or "create". A keyword screen over prose cannot
    see that; the store-the-return-into-a-global pattern is unambiguous.
    """
    import capstone, pefile
    global _pe
    try:
        _pe
    except NameError:
        _pe = pefile.PE(str(ROOT / "original" / "MASHED.exe.unpatched"), fast_load=True)
    base = _pe.OPTIONAL_HEADER.ImageBase
    data = _pe.get_memory_mapped_image()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    va = int(rva, 16)
    out, prev_call = [], False
    for i in md.disasm(data[va - base:va - base + 0x80], va):
        if (prev_call and i.mnemonic == "mov"
                and re.match(r"dword ptr \[0x[0-9a-f]+\], eax", i.op_str)):
            out.append("publishes-call-result")     # obj = make(); global = obj
        # A store through a REGISTER base is a write into live state, and the
        # register usually came from a global or an argument. 0x004e4320 ends
        # `mov [esi+0xc], eax` with esi = DAT_007d716c + param_2 — it mutates the
        # game, and every prose-level and import-level check rated it SAFE.
        # FALSE-POSITIVE WARNING, cleared by reading, not by the tool: a store
        # through a pointer that came straight from an ARGUMENT is an out-param
        # and is harmless — the harness owns that buffer. 0x0046cc10 does
        # `mov [ecx], eax` with ecx = param_1 and is fine; 0x004e4320 does
        # `mov [esi+0xc], eax` with esi = DAT_007d716c + param_2 and mutates the
        # game. The two are indistinguishable here, so this flags and a human
        # clears it.
        if (i.mnemonic == "mov"
                and re.match(r"dword ptr \[e(ax|cx|dx|bx|si|di)(\s*\+[^\]]+)?\], ", i.op_str)):
            out.append("stores-through-pointer")
        prev_call = (i.mnemonic == "call")
    # NOTE: no break on the first `ret`. An early-out returns before the body's
    # real work — 0x0046cc10 rets at 0x0046cc1b and stores at 0x0046cc31 — and
    # this function made the SAME truncation mistake shape_screen.py already had
    # to be fixed for. Third time this pattern has bitten in one session.
    return out


def plate_for(rva):
    global _plates
    if _plates is None:
        _plates = {}
        for p in glob.glob(str(ROOT / "re/analysis/**/*.md"), recursive=True):
            b = os.path.basename(p).lower().replace("0x", "").replace(".md", "").replace("fun_", "")
            if len(b) == 8 and all(c in "0123456789abcdef" for c in b):
                _plates.setdefault(b, []).append(p)
    key = rva.lower().replace("0x", "")
    for p in _plates.get(key, []):
        try:
            return p, io.open(p, encoding="utf-8", errors="replace").read()
        except OSError:
            pass
    return None, ""


def screen(rva):
    path, txt = plate_for(rva)
    if not txt:
        return {"rva": rva, "verdict": "NO-PLATE", "hits": [], "plate": ""}
    body = txt
    if "## Mechanical description" in txt:
        body = txt.split("## Mechanical description", 1)[1].split("## Constants")[0]
    hits = [name for name, pat in HAZARDS if re.search(pat, body, re.I)]
    hits += disasm_hazards(rva)
    # INHERIT hazards from direct callees. A wrapper is exactly as unsafe as what
    # it wraps, and the wrapper's own plate usually does not repeat it:
    # 0x0045d430 reads clean, then calls 0x005a60e0 — whose plate says teardown —
    # and clears the gate global so a second call is a no-op. Force-calling it
    # tears down once and then measures nothing.
    for c in re.findall(r"FUN_([0-9a-f]{8})", body):
        _, ctxt = plate_for(c)
        if not ctxt:
            continue
        cbody = (ctxt.split("## Mechanical description", 1)[1].split("## Constants")[0]
                 if "## Mechanical description" in ctxt else ctxt)
        for name, pat in HAZARDS:
            if name != "writes-global" and re.search(pat, cbody, re.I):
                hits.append(f"callee:{name}")
    # writes-global alone is fine — most STATE getters do — it only matters
    # alongside another hazard, so it never decides on its own.
    deciding = [h for h in hits if h != "writes-global"]
    return {"rva": rva, "verdict": ("SAFE" if not deciding else "+".join(deciding)),
            "hits": hits, "plate": path or ""}


def main():
    src = sys.argv[sys.argv.index("--from") + 1]
    want = sys.argv[sys.argv.index("--verdict") + 1] if "--verdict" in sys.argv else None
    rows = []
    for line in io.open(ROOT / src, encoding="utf-8"):
        if not line.startswith("0x"): continue
        f = line.rstrip("\n").split("\t")
        if want and len(f) > 1 and f[1] != want: continue
        rows.append((f[0], f[2] if len(f) > 2 else "", f[3] if len(f) > 3 else ""))
    res = [(screen(r), sz, sub) for r, sz, sub in rows]
    res.sort(key=lambda t: (t[0]["verdict"] != "SAFE", int(t[1] or 0)))
    print(f"{'rva':12s} {'semantic':22s} {'sz':>4s} {'sub':10s} plate")
    for r, sz, sub in res:
        print(f"{r['rva']:12s} {r['verdict'][:22]:22s} {sz:>4s} {sub:10s} "
              f"{os.path.relpath(r['plate'], ROOT) if r['plate'] else '-'}")
    safe = [r for r, _, _ in res if r["verdict"] == "SAFE"]
    print(f"\nSAFE {len(safe)}/{len(res)}  -> " + ",".join(s["rva"] for s in safe))


if __name__ == "__main__":
    main()
