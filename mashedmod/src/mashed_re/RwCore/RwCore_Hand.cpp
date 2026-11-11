// RWC-CORE-RUN (L-124 (C) step 2): hand bodies for RW-band RVAs of the engine bring-up closure that have no re/match
// source, written from the anchored disassembly (MASHED.exe.unpatched). EXE ONLY, MASHED_RWC_CORE knob path only.
// Provisional (L-126 (6)): listed in verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv, never C4 by this status.
// The generator (verify/rwccorerun_20261111/gen_rwcore_tus.py) binds an RVA here by the "// HAND 0x<rva> <symbol>" line.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <intrin.h>
#include "../Compat/D3d9Wrap.h"

namespace {
// The pushf / xor 0x200000 / popf CPUID-support probe the four checks below start with (e.g. 0x004dcf9b..0x004dcfbd).
// Every CPU this exe runs on has CPUID, so the probe's result is 1; kept as a function for the citation.
inline bool HasCpuid() { return true; }
inline unsigned CpuidEdx(unsigned leaf) { int r[4]; __cpuid(r, static_cast<int>(leaf)); return static_cast<unsigned>(r[3]); }
inline unsigned CpuidEax(unsigned leaf) { int r[4]; __cpuid(r, static_cast<int>(leaf)); return static_cast<unsigned>(r[0]); }
inline std::uint32_t& U32(std::uint32_t va) { return *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(va)); }
inline std::uint32_t RwG() { return U32(0x007d3ff8u); }   // RwGlobals* (DAT_007d3ff8)
}

extern "C" {

// HAND 0x004dd140 RwCoreHand_Direct3DCreate9
// rva 0x004dd140: `jmp [0x005cc2c4]`, the Direct3DCreate9 import thunk (re/match/rwl01/004dd140.c, 100%, reproduces the
// thunk shape with a stand-in import name). The RW driver 0x004c7a70 calls it as __stdcall (re/match/rwd01/004c7a70.c:140)
// with 0x1f. Here the import is the RWC-DEVICE wrapper (Compat/D3d9Wrap.cpp): the real IDirect3D9 inside the wrapper,
// whose CreateDevice forces the windowed 640x480 device the d3d9 shim forces for MASHED.exe.
void* __stdcall RwCoreHand_Direct3DCreate9(unsigned sdkVersion) {
    return MashedD3D9Wrap_Direct3DCreate9(sdkVersion);
}

// HAND 0x004ccef0 RwCoreHand_004ccef0
// rva 0x004ccef0: push [esp+4]; call 0x004a45fb (CRT malloc); add esp,4; ret. RwGlobals +0x108 (0x004cce20 default).
void* RwCoreHand_004ccef0(std::size_t size, unsigned /*hint*/) { return std::malloc(size); }

// HAND 0x004ccf00 RwCoreHand_004ccf00
// rva 0x004ccf00: push [esp+8]; push [esp+4+4]; call 0x004a467e (CRT calloc); add esp,8; ret. RwGlobals +0x114.
void* RwCoreHand_004ccf00(std::size_t n, std::size_t size) { return std::calloc(n, size); }

// HAND 0x004c35b0 RwCoreHand_004c35b0
// rva 0x004c35b0: ecx = arg0; [esp+4] = *ecx; jmp [RwGlobals+0x108] (the malloc slot, remaining arguments unchanged).
int RwCoreHand_004c35b0(std::uint32_t* p, unsigned a1) {
    typedef int (*Fn)(std::uint32_t, unsigned);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(U32(RwG() + 0x108)))(*p, a1);
}

// HAND 0x004c35d0 RwCoreHand_004c35d0
// rva 0x004c35d0: call [RwGlobals+0x10c](arg1) (the free slot); return arg1.
int RwCoreHand_004c35d0(int /*a0*/, int a1) {
    typedef void (*Fn)(int);
    reinterpret_cast<Fn>(static_cast<std::uintptr_t>(U32(RwG() + 0x10c)))(a1);
    return a1;
}

// HAND 0x004cfdd0 RwCoreHand_004cfdd0
// rva 0x004cfdd0: *(DAT_00911ae4 + arg0 + 0x1c) = 0; return arg0 (eax = arg0 at 0x004cfdd0).
int RwCoreHand_004cfdd0(int a0) {
    U32(U32(0x00911ae4u) + static_cast<std::uint32_t>(a0) + 0x1c) = 0;
    return a0;
}

// HAND 0x004cfdf0 RwCoreHand_004cfdf0
// rva 0x004cfdf0: p = DAT_00911ae4 + arg0 + 0x1c; when *p: (*(*p)->vtbl[2])(*p) (IUnknown::Release, __stdcall this),
// *p = 0. Returns arg0.
int RwCoreHand_004cfdf0(int a0) {
    std::uint32_t* const p = &U32(U32(0x00911ae4u) + static_cast<std::uint32_t>(a0) + 0x1c);
    if (*p) {
        typedef unsigned long (__stdcall *Release)(std::uint32_t);
        const std::uint32_t vt = U32(*p);
        reinterpret_cast<Release>(static_cast<std::uintptr_t>(U32(vt + 8)))(*p);
        *p = 0;
    }
    return a0;
}

// HAND 0x004d7d70 RwCoreHand_004d7d70
// rva 0x004d7d70: q = *(arg0+0x38); when *(q+0x10): *q = *(q+4), *(q+0x10) = 0, *(q+0x14) = 0 (0x004d7d7f..0x004d7d8d);
// then return [RwGlobals+0x11c](arg1, arg0) (0x004d7d90..0x004d7d9c).
int RwCoreHand_004d7d70(std::uint32_t a0, std::uint32_t a1) {
    std::uint32_t q = U32(a0 + 0x38);
    if (U32(q + 0x10) != 0) {
        U32(q) = U32(q + 4);
        U32(U32(a0 + 0x38) + 0x10) = 0;
        U32(U32(a0 + 0x38) + 0x14) = 0;
    }
    typedef int (*Fn)(std::uint32_t, std::uint32_t);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(U32(RwG() + 0x11c)))(a1, a0);
}

// HAND 0x004dcf90 RwCoreHand_004dcf90
// rva 0x004dcf90: CPUID probe, then cpuid(1) (0x004dcfc2..0x004dcfcd), returns (edx >> 0x17) & 1 (0x004dcfd5).
int RwCoreHand_004dcf90(void) { return HasCpuid() ? static_cast<int>((CpuidEdx(1) >> 0x17) & 1) : 0; }

// HAND 0x004dcff0 RwCoreHand_004dcff0
// rva 0x004dcff0: CPUID probe, cpuid(1) (0x004dd022..0x004dd02d), returns (edx & 0x6000000) != 0 (0x004dd035..0x004dd03e).
int RwCoreHand_004dcff0(void) { return HasCpuid() ? ((CpuidEdx(1) & 0x6000000u) != 0) : 0; }

// HAND 0x004dd050 RwCoreHand_004dd050
// rva 0x004dd050: CPUID probe, cpuid(1) (0x004dd08d), returns (edx >> 0x1a) & 1 (0x004dd095).
int RwCoreHand_004dd050(void) { return HasCpuid() ? static_cast<int>((CpuidEdx(1) >> 0x1a) & 1) : 0; }

// HAND 0x004dd0b0 RwCoreHand_004dd0b0
// rva 0x004dd0b0: CPUID probe, cpuid(0x80000000) eax > 0x80000000 (0x004dd0e3..0x004dd0f5), then cpuid(0x80000001)
// (0x004dd106..0x004dd111), returns (edx & 0x80000000) != 0 (0x004dd116..0x004dd122). 0 on either failed test.
int RwCoreHand_004dd0b0(void) {
    if (!HasCpuid() || !(CpuidEax(0x80000000u) > 0x80000000u)) return 0;
    return (CpuidEdx(0x80000001u) & 0x80000000u) != 0;
}

// HAND 0x004cca80 RwCoreHand_004cca80
// rva 0x004cca80 (0x004cca80..0x004ccb9d), installed at 0x007d3fe0 by RwEngineInit 0x004c32b0 when initFlags bit 0 is
// clear. Argument 0 = a record r: +0 entry size, +4 entries per block, +8 bitmap bytes, +0xc alignment, +0x10 / +0x14
// block list head (next, prev). Walks the blocks from r+0x10 (0x004cca8b): per bitmap byte (block + 8 + i), 0xff skips
// 8 entries (0x004ccb94); else the first clear bit, highest first (0x80 >> bit, 0x004ccacc), counted against the
// entries left in the block, is set and the entry address returned:
//   ((align + block + bitmapBytes + 7) & ~(align - 1)) + (bit + i * 8) * entrySize      (0x004ccae8..0x004ccb08)
// With none free: a new block of entriesPerBlock * entrySize + align + bitmapBytes + 7 bytes from [RwGlobals+0x108]
// (size, argument 1) (0x004ccb2c..0x004ccb48), NULL -> 0; bitmap zeroed (rep stos, 0x004ccb61), linked at the head
// (0x004ccb6a..0x004ccb7a), first bit set (byte 0x80, 0x004ccb7c), entry 0 returned (0x004ccb80..0x004ccb8c).
std::uint32_t RwCoreHand_004cca80(std::uint32_t r, std::uint32_t hint) {
    const std::uint32_t head = r + 0x10;
    const std::uint32_t nbytes = U32(r + 8);
    std::uint32_t found = 0;
    for (std::uint32_t blk = U32(r + 0x10); blk != head && !found; blk = U32(blk)) {
        std::uint32_t left = U32(r + 4);
        for (std::uint32_t i = 0; i < nbytes && !found; ++i) {
            std::uint8_t* const bm = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(blk + 8 + i));
            if (*bm == 0xff) { left -= 8; continue; }
            for (std::uint32_t bit = 0; bit < 8; ++bit, --left) {
                if (left == 0) break;
                const std::uint8_t mask = static_cast<std::uint8_t>(0x80u >> bit);
                if (!(*bm & mask)) {
                    *bm = static_cast<std::uint8_t>(*bm | mask);
                    const std::uint32_t align = U32(r + 0xc);
                    found = ((align + blk + nbytes + 7) & ~(align - 1)) + (bit + i * 8) * U32(r);
                    break;
                }
            }
        }
    }
    if (found) return found;
    typedef std::uint32_t (*Malloc)(std::uint32_t, std::uint32_t);
    const std::uint32_t size = U32(r + 4) * U32(r) + U32(r + 0xc) + nbytes + 7;
    const std::uint32_t blk = reinterpret_cast<Malloc>(static_cast<std::uintptr_t>(U32(RwG() + 0x108)))(size, hint);
    if (!blk) return 0;
    std::memset(reinterpret_cast<void*>(static_cast<std::uintptr_t>(blk + 8)), 0, nbytes);
    U32(blk + 4) = head;
    U32(blk) = U32(head);
    U32(U32(head) + 4) = blk;
    U32(head) = blk;
    *reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(blk + 8)) = 0x80;
    const std::uint32_t align = U32(r + 0xc);
    return (blk + align + nbytes + 7) & ~(align - 1);
}

} // extern "C"
#endif // MASHED_STANDALONE
