# Heap-shift launcher for ORIGINAL MASHED.exe (map-aware v2).
#
# Root cause (this boot): MASHED's 2004 MSVC CRT __sbh init corrupts its own
# process heap when the loader places it at the low 0x30000 region -> 0xC0000005
# WRITE to 0x5477 in ntdll!RtlpHeap. Compat shims do NOT change this (proven).
# Frida (which gains control before ntdll creates the heap) shifts the heap high
# (0xdc0000) and survives heap init.
#
# This launcher: CreateProcess(SUSPENDED), enumerate the low address space with
# VirtualQueryEx, RESERVE every FREE region in [0x10000, 0x400000) so the loader
# cannot place the process heap low, then ResumeThread. Use --map to just dump the
# suspended memory map (diagnosis), --keep to leave a booted process running.
#
# Usage: py -3.12 scripts/launch_heapshift.py [--seconds N] [--map] [--keep] [--top HEX]
import os, sys, time, ctypes
from ctypes import wintypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
k32 = ctypes.WinDLL("kernel32", use_last_error=True)

class STARTUPINFO(ctypes.Structure):
    _fields_ = [("cb", wintypes.DWORD), ("lpReserved", wintypes.LPWSTR), ("lpDesktop", wintypes.LPWSTR),
                ("lpTitle", wintypes.LPWSTR), ("dwX", wintypes.DWORD), ("dwY", wintypes.DWORD),
                ("dwXSize", wintypes.DWORD), ("dwYSize", wintypes.DWORD), ("dwXCountChars", wintypes.DWORD),
                ("dwYCountChars", wintypes.DWORD), ("dwFillAttribute", wintypes.DWORD), ("dwFlags", wintypes.DWORD),
                ("wShowWindow", wintypes.WORD), ("cbReserved2", wintypes.WORD), ("lpReserved2", ctypes.c_void_p),
                ("hStdInput", wintypes.HANDLE), ("hStdOutput", wintypes.HANDLE), ("hStdError", wintypes.HANDLE)]

class PROCESS_INFORMATION(ctypes.Structure):
    _fields_ = [("hProcess", wintypes.HANDLE), ("hThread", wintypes.HANDLE),
                ("dwProcessId", wintypes.DWORD), ("dwThreadId", wintypes.DWORD)]

class MEMORY_BASIC_INFORMATION(ctypes.Structure):
    _fields_ = [("BaseAddress", ctypes.c_void_p), ("AllocationBase", ctypes.c_void_p),
                ("AllocationProtect", wintypes.DWORD), ("__a1", wintypes.DWORD),
                ("RegionSize", ctypes.c_size_t), ("State", wintypes.DWORD),
                ("Protect", wintypes.DWORD), ("Type", wintypes.DWORD), ("__a2", wintypes.DWORD)]

CREATE_SUSPENDED = 0x00000004
MEM_RESERVE = 0x00002000; MEM_COMMIT = 0x1000; MEM_FREE = 0x10000
PAGE_NOACCESS = 0x01; STILL_ACTIVE = 259

k32.VirtualAllocEx.restype = ctypes.c_void_p
k32.VirtualAllocEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_size_t, wintypes.DWORD, wintypes.DWORD]
k32.VirtualQueryEx.restype = ctypes.c_size_t
k32.VirtualQueryEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t]

STATE = {MEM_COMMIT: "COMMIT", MEM_RESERVE: "RESERVE", MEM_FREE: "free"}


def walk(hproc, lo, hi):
    """yield (base, size, state) for regions in [lo, hi)."""
    addr = lo
    mbi = MEMORY_BASIC_INFORMATION()
    while addr < hi:
        n = k32.VirtualQueryEx(hproc, ctypes.c_void_p(addr), ctypes.byref(mbi), ctypes.sizeof(mbi))
        if not n:
            break
        base = mbi.BaseAddress or 0
        size = mbi.RegionSize
        yield base, size, mbi.State
        addr = base + size


def main():
    seconds = int(sys.argv[sys.argv.index("--seconds") + 1]) if "--seconds" in sys.argv else 12
    top = int(sys.argv[sys.argv.index("--top") + 1], 16) if "--top" in sys.argv else 0x00400000
    keep = "--keep" in sys.argv
    maponly = "--map" in sys.argv

    si = STARTUPINFO(); si.cb = ctypes.sizeof(si)
    pi = PROCESS_INFORMATION()
    if not k32.CreateProcessW(str(EXE), None, None, None, False, CREATE_SUSPENDED,
                              None, str(ORIG), ctypes.byref(si), ctypes.byref(pi)):
        print("CreateProcess failed:", ctypes.get_last_error()); return 2
    print(f"  created MASHED pid={pi.dwProcessId} (suspended)")

    print("  --- low memory map at CREATE_SUSPENDED (0x0 .. 0x500000) ---")
    free_regions = []
    for base, size, state in walk(pi.hProcess, 0, 0x00500000):
        s = STATE.get(state, hex(state))
        if state != MEM_FREE or size >= 0x10000:   # don't spam tiny gaps
            print(f"    0x{base:08x}  size=0x{size:08x}  {s}")
        if state == MEM_FREE and base >= 0x10000 and base < top:
            free_regions.append((base, min(size, top - base)))
    q = MEMORY_BASIC_INFORMATION()
    k32.VirtualQueryEx(pi.hProcess, ctypes.c_void_p(0x30000), ctypes.byref(q), ctypes.sizeof(q))
    print(f"  >>> 0x30000 is: {STATE.get(q.State, hex(q.State))} (free => heap not yet created => shiftable)")

    if maponly:
        k32.TerminateProcess(pi.hProcess, 0); return 0

    reserved = 0; rbytes = 0
    for base, size in free_regions:
        r = k32.VirtualAllocEx(pi.hProcess, ctypes.c_void_p(base), size, MEM_RESERVE, PAGE_NOACCESS)
        if r:
            reserved += 1; rbytes += size
    print(f"  reserved {reserved} free low regions ({rbytes//1024} KB) below 0x{top:08x}")

    k32.ResumeThread(pi.hThread)
    print(f"  resumed; watching {seconds}s (heap-AV fires ~4s in)...")
    code = wintypes.DWORD(STILL_ACTIVE)
    deadline = time.time() + seconds; exited = False
    while time.time() < deadline:
        k32.GetExitCodeProcess(pi.hProcess, ctypes.byref(code))
        if code.value != STILL_ACTIVE:
            exited = True; break
        time.sleep(0.5)

    if exited:
        print(f"\n>>> EXITED  code=0x{code.value & 0xffffffff:08X}")
    else:
        print(f"\n>>> ALIVE after {seconds}s — BOOTED past heap init!")
        if keep:
            print(f"  left running: pid={pi.dwProcessId}")
        else:
            k32.TerminateProcess(pi.hProcess, 0); print("  (killed; --keep to leave running)")
    k32.CloseHandle(pi.hThread)
    if exited or not keep:
        k32.CloseHandle(pi.hProcess)
    return 1 if exited else 0


if __name__ == "__main__":
    sys.exit(main())
