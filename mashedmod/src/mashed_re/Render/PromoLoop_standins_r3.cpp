// Mashed RE: stand-in promotion round 3 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Two RenderWare object-assembly sequences that differ only in the geometry builder they call. Every callee is a C2
// function called at its original address; the body is the call order and the argument wiring. All allocations go
// through the RwGlobals memory slots, so the heap stand-in carves the whole object graph from its arena.
//
//   call <geometry builder>     ebx = geometry      (0x00402244 -> 0x00401fe0 / 0x00402594 -> 0x004022a0)
//   call 0x004e6d80             esi = clump         (alloc type 0x30010, three list sentinels)
//   call 0x004c0b30             [esi+4] = frame A   (RW alloc type 0x3000e via RwGlobals+0x118)
//   call 0x004e67b0             edi = atomic        (RpAtomicCreate)
//   call 0x004c0b30             ebp = frame B
//   call 0x004e7e30 (edi, ebp)                      atomic <- frame B
//   call 0x004e68a0 (edi, ebx, 0)                   atomic <- geometry
//   call 0x004e6f80 (esi, edi)                      clump list insert of the atomic
//   call 0x004c1040 ([esi+4], ebp)                  frame B reparented under frame A
//   call 0x004e8ea0 (ebx)                           geometry release (refcount drop)
//   add esp, 0x28 (all ten pushes cdecl, caller-cleaned); return esi (clump)

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {
using Fn0 = void*(__cdecl*)();
using Fn1 = void*(__cdecl*)(void*);
using Fn2 = void*(__cdecl*)(void*, void*);
using Fn3 = void*(__cdecl*)(void*, void*, std::uint32_t);

void* ClumpAssemble(std::uintptr_t geometryBuilder) {
    void* geom = reinterpret_cast<Fn0>(geometryBuilder)();
    std::uint8_t* clump = static_cast<std::uint8_t*>(reinterpret_cast<Fn0>(0x004e6d80u)());
    *reinterpret_cast<void**>(clump + 4) = reinterpret_cast<Fn0>(0x004c0b30u)();
    void* atomic = reinterpret_cast<Fn0>(0x004e67b0u)();
    void* frameB = reinterpret_cast<Fn0>(0x004c0b30u)();
    reinterpret_cast<Fn2>(0x004e7e30u)(atomic, frameB);
    reinterpret_cast<Fn3>(0x004e68a0u)(atomic, geom, 0);
    reinterpret_cast<Fn2>(0x004e6f80u)(clump, atomic);
    reinterpret_cast<Fn2>(0x004c1040u)(*reinterpret_cast<void**>(clump + 4), frameB);
    reinterpret_cast<Fn1>(0x004e8ea0u)(geom);
    return clump;
}
}  // namespace

// 0x00402240  ClumpGeom401fe0Create (FUN_00402240, 89 B; caller 0x004025f0). Geometry builder 0x00401fe0 (0x00402244).
// 0x00402244 call 0x00401fe0 / 0x0040224b 0x004e6d80 / 0x00402252 0x004c0b30 -> [esi+4] (0x00402257) /
// 0x0040225a 0x004e67b0 / 0x00402261 0x004c0b30 / 0x0040226a 0x004e7e30 / 0x00402273 0x004e68a0 /
// 0x0040227a 0x004e6f80 / 0x00402284 0x004c1040 / 0x0040228a 0x004e8ea0 / 0x00402293 mov eax, esi
// 0x00402240
extern "C" __declspec(dllexport) void* __cdecl ClumpGeom401fe0Create() { return ClumpAssemble(0x00401fe0u); }
RH_ScopedInstall(ClumpGeom401fe0Create, 0x00402240);

// 0x00402590  ClumpGeom4022a0Create (FUN_00402590, 89 B; caller 0x004025f0). Same sequence as 0x00402240 with the
// geometry builder 0x004022a0 (0x00402594); callee order and pushes byte-identical otherwise (0x0040259b..0x004025e8).
// 0x00402590
extern "C" __declspec(dllexport) void* __cdecl ClumpGeom4022a0Create() { return ClumpAssemble(0x004022a0u); }
RH_ScopedInstall(ClumpGeom4022a0Create, 0x00402590);
