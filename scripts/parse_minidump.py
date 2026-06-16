# Minimal WER minidump parser (no deps) for MASHED boot-AV diagnosis.
#
# Usage: py -3.12 scripts/parse_minidump.py <path-to.dmp>
#        (defaults to the newest %LOCALAPPDATA%\CrashDumps\MASHED.exe.*.dmp)
#
# Extracts:
#   - ExceptionStream(6): exception code, faulting address, EIP.
#   - ModuleListStream(4): every loaded module name + base (reveals injected
#     AppCompat shim DLLs — AcLayers/AcXtrnal/acgenral/verifier — that turn a
#     "clean Layers key" into a heap-corrupting boot, the EMULATEHEAP/PCA class).
#   - the MASHED return-address chain scanned out of the faulting thread stack.
#
# Recipe per [[project-emulateheap-boot-av]]. MASHED image VAs map straight to
# Ghidra RVAs.
import os
import struct
import sys
import glob

STREAM_THREADLIST   = 3
STREAM_MODULELIST   = 4
STREAM_MEMORYLIST   = 5
STREAM_EXCEPTION    = 6
STREAM_MEMORY64LIST = 9


def _u32(b, o): return struct.unpack_from('<I', b, o)[0]
def _u64(b, o): return struct.unpack_from('<Q', b, o)[0]


def read_mdstring(b, rva):
    n = _u32(b, rva)  # length in BYTES of the UTF-16 buffer
    raw = b[rva + 4: rva + 4 + n]
    return raw.decode('utf-16-le', errors='replace')


def parse(path):
    with open(path, 'rb') as fh:
        b = fh.read()
    if b[:4] != b'MDMP':
        sys.exit(f"not a minidump: {path}")
    nstreams = _u32(b, 8)
    dir_rva  = _u32(b, 12)
    streams = {}
    for i in range(nstreams):
        off = dir_rva + i * 12
        stype = _u32(b, off)
        dsize = _u32(b, off + 4)
        srva  = _u32(b, off + 8)
        streams[stype] = (dsize, srva)

    print(f"== {os.path.basename(path)} ==")

    fault_eip = None
    fault_tid = None
    # ---- Exception ----
    if STREAM_EXCEPTION in streams:
        _, rva = streams[STREAM_EXCEPTION]
        tid = _u32(b, rva)
        exc = rva + 8  # skip ThreadId + alignment
        code = _u32(b, exc)
        flags = _u32(b, exc + 4)
        addr = _u64(b, exc + 16)
        nparm = _u32(b, exc + 24)
        params = [_u64(b, exc + 32 + 8 * k) for k in range(min(nparm, 15))]
        fault_tid = tid
        print(f"[exception] tid={tid} code=0x{code:08x} flags=0x{flags:x} "
              f"addr=0x{addr:016x} nparam={nparm} params={[hex(p) for p in params]}")
        # x86 CONTEXT: Eip @ 0xb8, Esp @ 0xc4, Ebp @ 0xb4
        ctx_size = _u32(b, exc + 32 + 15 * 8)
        ctx_rva  = _u32(b, exc + 32 + 15 * 8 + 4)
        if ctx_size >= 0xcc:
            eip = _u32(b, ctx_rva + 0xb8)
            esp = _u32(b, ctx_rva + 0xc4)
            ebp = _u32(b, ctx_rva + 0xb4)
            eax = _u32(b, ctx_rva + 0xb0)
            ecx = _u32(b, ctx_rva + 0xac)
            fault_eip = eip
            print(f"[context]   eip=0x{eip:08x} esp=0x{esp:08x} ebp=0x{ebp:08x} "
                  f"eax=0x{eax:08x} ecx=0x{ecx:08x}")

    # ---- Modules ----
    mashed_base = mashed_end = None
    shim_hits = []
    SHIM = ('aclayers', 'acxtrnal', 'acgenral', 'verifier', 'apphelp',
            'aceventlog', 'acspecfc')
    if STREAM_MODULELIST in streams:
        _, rva = streams[STREAM_MODULELIST]
        nmod = _u32(b, rva)
        print(f"[modules]   {nmod} loaded")
        for i in range(nmod):
            m = rva + 4 + i * 108
            base = _u64(b, m)
            size = _u32(b, m + 8)
            name_rva = _u32(b, m + 20)
            name = read_mdstring(b, name_rva)
            low = name.lower()
            if low.endswith('mashed.exe'):
                mashed_base, mashed_end = base, base + size
            if any(s in low for s in SHIM):
                shim_hits.append((name, base))
        print(f"[mashed]    base=0x{mashed_base:08x} end=0x{mashed_end:08x}"
              if mashed_base else "[mashed]    MASHED.exe not found in module list")
        if shim_hits:
            print("[SHIM DLLs LOADED] (AppCompat/verifier injection present):")
            for n, base in shim_hits:
                print(f"    0x{base:08x}  {n}")
        else:
            print("[shims]     none of AcLayers/AcXtrnal/acgenral/verifier/apphelp loaded")

    # ---- Faulting-thread stack: scan for MASHED return addresses ----
    if mashed_base and STREAM_THREADLIST in streams and fault_tid is not None:
        # find faulting thread's stack range from ThreadList
        stack_start = stack_size = None
        if STREAM_THREADLIST in streams:
            _, trva = streams[STREAM_THREADLIST]
            nthr = _u32(b, trva)
            for i in range(nthr):
                t = trva + 4 + i * 48
                tid = _u32(b, t)
                if tid == fault_tid:
                    # MINIDUMP_THREAD (48B): ThreadId0 SuspendCount4 Pri8 Pri12
                    # Teb16(8) Stack@24(MEMORY_DESCRIPTOR: Start8@24, DataSize4@32,
                    # Rva4@36) ThreadContext@40.
                    stack_start = _u64(b, t + 24)
                    sdsize = _u32(b, t + 32)
                    srva   = _u32(b, t + 36)
                    stack_bytes = b[srva: srva + sdsize]
                    print(f"[stack]     tid={tid} start=0x{stack_start:08x} size=0x{sdsize:x}")
                    chain = []
                    for o in range(0, len(stack_bytes) - 4, 4):
                        v = _u32(stack_bytes, o)
                        if mashed_base <= v < mashed_end:
                            chain.append(v)
                    # de-dup consecutive, print first ~25 distinct
                    seen = []
                    for v in chain:
                        if v not in seen:
                            seen.append(v)
                    print(f"[ret-chain] {len(seen)} distinct MASHED addrs on stack; first 25:")
                    print("    " + " ".join(f"0x{v:08x}" for v in seen[:25]))
                    break


if __name__ == '__main__':
    if len(sys.argv) > 1:
        target = sys.argv[1]
    else:
        cd = os.path.join(os.environ.get('LOCALAPPDATA', ''), 'CrashDumps')
        dumps = sorted(glob.glob(os.path.join(cd, 'MASHED.exe.*.dmp')),
                       key=os.path.getmtime)
        if not dumps:
            sys.exit("no MASHED dumps found")
        target = dumps[-1]
    parse(target)
