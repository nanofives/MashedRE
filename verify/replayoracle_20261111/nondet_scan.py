"""Static census of nondeterminism sources in MASHED.exe (REPLAY-ORACLE-DESIGN, 2026-11-11).

For a fixed list of imports (clock, time, threads, sleeps, waits, input, random), every instruction in
.text whose memory operand is that import's IAT slot is listed with its containing function (Ghidra facts
via re/tools/xref.py's cache). Then the callers of the game-side wrappers that matter for replay (CRT rand,
the RenderWare ring RNG, the QPC wrappers, the control resolver).

    py -3.12 verify/replayoracle_20261111/nondet_scan.py > verify/replayoracle_20261111/nondet_scan.txt

Reads original/MASHED.exe.unpatched (the SHA-256 anchor) only. Addresses are VAs (base 0x00400000).
"""
import json, pathlib, sys
from collections import defaultdict

import capstone
import pefile

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import xref  # noqa: E402

EXE = xref.MAIN / "original" / "MASHED.exe.unpatched"
IMPORTS = [
    "QueryPerformanceCounter", "QueryPerformanceFrequency", "GetTickCount", "timeGetTime", "timeBeginPeriod",
    "GetSystemTime", "GetSystemTimeAsFileTime", "GetLocalTime", "time", "_time64", "clock",
    "CreateThread", "_beginthread", "_beginthreadex", "ExitThread", "SetThreadPriority",
    "Sleep", "SleepEx", "WaitForSingleObject", "WaitForMultipleObjects", "WaitMessage", "MsgWaitForMultipleObjects",
    "CreateEventA", "SetEvent", "ResetEvent", "EnterCriticalSection", "InitializeCriticalSection",
    "PeekMessageA", "GetMessageA", "GetAsyncKeyState", "GetKeyState", "GetKeyboardState", "GetCursorPos",
    "DirectInput8Create", "joyGetPosEx", "joyGetDevCapsA", "XInputGetState",
    "GetCurrentProcessId", "GetCurrentThreadId", "GetOverlappedResult", "ReadFile", "CreateFileA",
    "HeapAlloc", "HeapCreate", "VirtualAlloc", "GlobalMemoryStatus",
]


def main():
    pe = pefile.PE(str(EXE))
    base = pe.OPTIONAL_HEADER.ImageBase
    slots = {}
    for e in pe.DIRECTORY_ENTRY_IMPORT:
        for imp in e.imports:
            if imp.name:
                slots[imp.address] = (e.dll.decode(), imp.name.decode())
    want = {a: v for a, v in slots.items() if v[1] in IMPORTS}
    print("# IAT slots in scope")
    for a, (dll, n) in sorted(want.items(), key=lambda kv: kv[1][1]):
        print(f"  0x{a:08x}  {dll}!{n}")
    missing = sorted(set(IMPORTS) - {v[1] for v in want.values()})
    print("# not imported:", ", ".join(missing))

    funcs = []
    for line in open(xref.FACTS):
        r = json.loads(line)
        if "entry" not in r:
            continue
        for lo, hi in r["ranges"]:
            funcs.append((int(lo, 16), int(hi, 16), int(r["entry"], 16), r["name"]))
    funcs.sort()

    def owner(va):
        import bisect
        i = bisect.bisect_right(funcs, (va, 0xffffffff, 0, "")) - 1
        while i >= 0 and funcs[i][0] <= va:
            if va <= funcs[i][1]:
                return f"0x{funcs[i][2]:08x} {funcs[i][3]}"
            i -= 1
        return "(no function)"

    text = next(s for s in pe.sections if s.Name.rstrip(b"\0") == b".text")
    code = text.get_data()
    tva = base + text.VirtualAddress
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    md.skipdata = True
    refs = defaultdict(list)
    calls_to = defaultdict(list)   # direct call targets for the wrapper list
    WRAP = {0x005c229b: "CRT _rand", 0x00534870: "RW ring RNG FUN_00534870", 0x004950b0: "FUN_004950b0 QPC->3MHz",
            0x005ab040: "FUN_005ab040 audio clock (QPC or timeGetTime)", 0x00497310: "FUN_00497310 control resolver",
            0x00493390: "FUN_00493390 frame timer", 0x004a8828: "__getptd"}
    gref = defaultdict(list)
    for ins in md.disasm(code, tva):
        if ins.id == 0:
            continue
        for op in ins.operands:
            if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                a = op.mem.disp & 0xffffffff
                if a in want:
                    refs[a].append((ins.address, f"{ins.mnemonic} {ins.op_str}"))
            if op.type == capstone.x86.X86_OP_IMM and ins.mnemonic in ("call", "jmp"):
                t = op.imm & 0xffffffff
                if t in WRAP:
                    calls_to[t].append(ins.address)
            if op.type == capstone.x86.X86_OP_IMM and ins.mnemonic in ("push", "mov"):
                t = op.imm & 0xffffffff
                if t in WRAP:
                    calls_to[t].append(ins.address)   # address taken (callback / table)
        for op in ins.operands:
            if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                a = op.mem.disp & 0xffffffff
                if a in CLOCKG:
                    gref[a].append((ins.address, f"{ins.mnemonic} {ins.op_str}"))
    print("\n# references to each IAT slot (instruction -> containing function)")
    for a, (dll, n) in sorted(want.items(), key=lambda kv: kv[1][1]):
        rs = refs.get(a, [])
        print(f"\n## {n} (IAT 0x{a:08x}): {len(rs)} reference(s)")
        for va, txt in rs:
            print(f"  0x{va:08x}  {txt:40s} in {owner(va)}")
    print("\n# direct references to the game-side wrappers")
    for t, name in WRAP.items():
        rs = calls_to.get(t, [])
        print(f"\n## {name} 0x{t:08x}: {len(rs)} site(s)")
        for va in rs:
            print(f"  0x{va:08x}  in {owner(va)}")

    print("\n# every absolute reference to the clock-derived globals (written from FUN_004950b0 / FUN_005ab040 values)")
    for a, name in CLOCKG.items():
        rs = gref.get(a, [])
        print(f"\n## 0x{a:08x} {name}: {len(rs)} reference(s)")
        for va, txt in rs:
            print(f"  0x{va:08x}  {txt:44s} in {owner(va)}")


# Globals written from a clock value (writers cited in RESULT / DESIGN): FUN_00493390 (re/match/s15/00493390.c),
# FUN_004926c0 (re/match/s15/004926c0.c), FUN_00492e90 (re/match/s15/00492e90.c), FUN_00492d30, FUN_005a6ea0.
CLOCKG = {
    0x007f0ffc: "frame delta (FUN_00493390)", 0x00771984: "drift (FUN_00493390)", 0x007f1010: "now*3.33e-7 float (FUN_00493390)",
    0x007f1030: "last sample (FUN_00493390)", 0x007f102c: "1 s mark (FUN_00493390)", 0x007f1024: "per-second counter",
    0x007f1028: "per-second counter latch", 0x00771994: "60-frame accumulator (FUN_004926c0)",
    0x0077197c: "60-frame average (FUN_004926c0)", 0x007719a0: "ms now (FUN_00492e90)", 0x00771988: "float s (FUN_00492e90)",
    0x00771998: "ms base (FUN_00492e90)", 0x0077199c: "1 s mark ms (FUN_00492e90)", 0x00771990: "fps latch (FUN_00492e90)",
    0x007719a4: "fps counter (FUN_00492e90)", 0x007719a8: "tick duration (FUN_00492d30)",
    0x007dca50: "audio clock lo (FUN_005a6ea0)", 0x007dca54: "audio clock hi (FUN_005a6ea0)",
}


if __name__ == "__main__":
    main()
