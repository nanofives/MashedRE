// Mashed RE - Audio memory primitives (aligned alloc/free, buffer endian utilities).
// All five functions are pure transforms or thin trampolines — no global side
// effects beyond the underlying allocator; safe to A/B-diff via Frida.
//
// Source RVAs and asm annotations are cited per function below.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x005aea10  FUN_005aea10  (42 bytes)
// Allocates (size + 4) bytes via the vtable trampoline at 0x005aea00, then
// 4-byte-aligns the returned pointer and stores the raw base 4 bytes before
// the aligned pointer (so AudioAlignedFree can recover it).
//
// ASM key:
//   005aea10: MOV ECX,[ESP+4]      ; size
//   005aea18: ADD ECX,4            ; size+4
//   005aea1d: CALL 005aea00        ; raw = RawAlloc(size+4, tag)
//   005aea2b: LEA EAX,[ECX+4]     ; raw+4
//   005aea2e: SHR EAX,2; SHL EAX,2 ; (raw+4) & ~3  (4-byte alignment)
//   005aea34: MOV [EAX-4],ECX     ; *(aligned-4) = raw
//   005aea37: RET
extern "C" __declspec(dllexport) void* __cdecl AudioAlignedAlloc(int size, int tag) {
    typedef void* (__cdecl *AllocFn)(int, int);
    auto* const rawAlloc = reinterpret_cast<AllocFn>(0x005aea00u);
    auto* const raw = static_cast<char*>(rawAlloc(size + 4, tag));
    if (!raw) return nullptr;
    const auto r = reinterpret_cast<uintptr_t>(raw);
    const auto aligned = (r + 4u) & ~static_cast<uintptr_t>(3u);
    *reinterpret_cast<uintptr_t*>(aligned - 4) = r;
    return reinterpret_cast<void*>(aligned);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(AudioAlignedAlloc, 0x005aea10);

// 0x005aea40  FUN_005aea40  (15 bytes)
// Recovers the raw allocation base from *(ptr - 4), replaces the stack
// argument, and tail-calls the vtable dealloc trampoline at 0x004522d0.
//
// ASM key:
//   005aea40: MOV EAX,[ESP+4]   ; aligned_ptr
//   005aea44: MOV ECX,[EAX-4]   ; raw = *(aligned_ptr - 4)
//   005aea47: MOV [ESP+4],ECX   ; replace stack arg
//   005aea4b: JMP 004522d0      ; tail-call RawFree(raw)
extern "C" __declspec(dllexport) void __cdecl AudioAlignedFree(void* ptr) {
    typedef void (__cdecl *FreeFn)(void*);
    auto* const rawFree = reinterpret_cast<FreeFn>(0x004522d0u);
    void* const raw = *reinterpret_cast<void**>(static_cast<char*>(ptr) - 4);
    rawFree(raw);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(AudioAlignedFree, 0x005aea40);

// 0x005aec00  FUN_005aec00  (42 bytes)
// Reverses all bytes in buf in-place using two pointers from opposite ends.
// No-op when len < 2 (len>>1 == 0).
//
// ASM key:
//   005aec09: SHR EDI,1       ; half = len >> 1
//   005aec0b: JZ exit          ; no-op if < 2 bytes
//   005aec13: LEA ECX,[ESI+ECX-1] ; hi = buf + len - 1
//   005aec17: MOV BL,[ECX]    ; tmp = *hi
//   005aec1c: MOV [EAX+ESI],BL ; buf[lo] = tmp
//   005aec1f: MOV [ECX],DL    ; *hi = buf[lo]
//   005aec25: JC loop          ; while lo < len/2
extern "C" __declspec(dllexport) void __cdecl AudioByteReverse(char* buf, uint32_t len) {
    const uint32_t half = len >> 1;
    if (half == 0) return;
    char* hi = buf + len - 1;
    for (uint32_t lo = 0; lo < half; ++lo, --hi) {
        const char tmp = *hi;
        *hi = buf[lo];
        buf[lo] = tmp;
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(AudioByteReverse, 0x005aec00);

// 0x005aee20  FUN_005aee20  (27 bytes)
// Scans bits 0..31 of param; returns the index of the lowest set bit (0..31),
// or 32 if no bit is set.  Return value lives in AL (caller reads as byte).
//
// ASM key:
//   005aee24: XOR AL,AL          ; bit = 0
//   005aee27: MOV ESI,1          ; [loop] mask = 1
//   005aee2e: SHL ESI,CL         ; mask <<= bit
//   005aee30: TEST EDX,ESI       ; param & mask
//   005aee32: JNZ exit            ; found
//   005aee34: INC AL; CMP AL,0x20; JC loop
//   005aee3a: RET                 ; return AL (bit index or 32)
extern "C" __declspec(dllexport) unsigned char __cdecl AudioBitScanForward(unsigned int param) {
    unsigned char bit = 0;
    do {
        if (param & (1u << bit)) break;
        ++bit;
    } while (bit < 32u);
    return bit;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(AudioBitScanForward, 0x005aee20);

// 0x005aec30  FUN_005aec30  (103 bytes)
// In-place endian swap of a buffer by element width.
//   width == 4: bswap each uint32  {b0,b1,b2,b3} -> {b3,b2,b1,b0}
//   width == 2: swap bytes of each uint16  {b0,b1} -> {b1,b0}
//   other:      no-op
//
// ASM dispatch at 005aec30:
//   SUB EAX,2; JZ (width==2 path)
//   SUB EAX,2; JNZ (no-op if neither 2 nor 4)
//   ; fall-through: width == 4 path
extern "C" __declspec(dllexport) void __cdecl AudioByteSwapBuffer(
        uint8_t* buf, uint32_t len, int width) {
    if (width == 4) {
        uint32_t count = len >> 2;
        if (count == 0) return;
        auto* p = reinterpret_cast<uint32_t*>(buf);
        do {
            const uint32_t v = *p;
            *p++ = ((v & 0xFFu) << 24)
                 | (((v >>  8) & 0xFFu) << 16)
                 | (((v >> 16) & 0xFFu) <<  8)
                 |  (v >> 24);
            --count;
        } while (count > 0);
    } else if (width == 2) {
        uint32_t count = len >> 1;
        if (count == 0) return;
        uint8_t* p = buf;
        do {
            const uint8_t b0 = p[0], b1 = p[1];
            p[0] = b1;
            p[1] = b0;
            p += 2;
            --count;
        } while (count > 0);
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(AudioByteSwapBuffer, 0x005aec30);
