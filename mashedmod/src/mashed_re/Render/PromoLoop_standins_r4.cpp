// Mashed RE: stand-in promotion round 4 (plan item #3 step 2, 2026-10-11).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Two RenderWare pipeline builders (caller 0x00496e40 PCVS_Init) that differ only in the fragment-node getter
// (0x004eb3c0 / 0x004eb9d0; the other three differing instructions are rel8 targets of the same branches). Every callee
// is called at its original address; the pipeline allocations go through the RwGlobals slots (heap stand-in).
//
//   +0x02  call 0x004d4170            edi = pipe            test / je -> return 0
//   +0x0d  call 0x004d4dd0 (edi)      esi = locked          test / je -> destroy
//   +0x1c  call <node getter>         eax = node
//   +0x27  call 0x004d4f90 (esi, 0, eax, 0)
//   +0x2d  call 0x004d4380 (eax)      test / je -> destroy
//   +0x39  return edi
//   destroy: call 0x004d41e0 (edi), return 0
//   All calls cdecl, caller-cleaned (add esp, 4 after 0x004d4dd0; add esp, 0x14 after 0x004d4380 for both calls).

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {
using Fn0 = void*(__cdecl*)();
using Fn1 = void*(__cdecl*)(void*);
using Fn4 = void*(__cdecl*)(void*, void*, void*, void*);

void* PipelineWithFragment(std::uintptr_t nodeGetter) {
    void* pipe = reinterpret_cast<Fn0>(0x004d4170u)();
    if (!pipe) return nullptr;
    void* locked = reinterpret_cast<Fn1>(0x004d4dd0u)(pipe);
    if (locked) {
        void* node = reinterpret_cast<Fn0>(nodeGetter)();
        void* r = reinterpret_cast<Fn4>(0x004d4f90u)(locked, nullptr, node, nullptr);
        if (reinterpret_cast<Fn1>(0x004d4380u)(r)) return pipe;
    }
    reinterpret_cast<Fn1>(0x004d41e0u)(pipe);
    return nullptr;
}
}  // namespace

// 0x00496d60  PipelineFragment4eb3c0Create (FUN_00496d60, 76 B)
// 0x00496d62 0x004d4170 / 0x00496d6b je 0x00496da7 / 0x00496d6e 0x004d4dd0 / 0x00496d7a je 0x00496d9e /
// 0x00496d7c 0x004eb3c0 / 0x00496d87 0x004d4f90 / 0x00496d8d 0x004d4380 / 0x00496d97 je 0x00496d9e / 0x00496d99 ret pipe /
// 0x00496d9f 0x004d41e0 / 0x00496da8 xor eax, eax
// 0x00496d60
extern "C" __declspec(dllexport) void* __cdecl PipelineFragment4eb3c0Create() { return PipelineWithFragment(0x004eb3c0u); }
RH_ScopedInstall(PipelineFragment4eb3c0Create, 0x00496d60);

// 0x00496db0  PipelineFragment4eb9d0Create (FUN_00496db0, 76 B): as 0x00496d60 with node getter 0x004eb9d0 (0x00496dcc).
// 0x00496db0
extern "C" __declspec(dllexport) void* __cdecl PipelineFragment4eb9d0Create() { return PipelineWithFragment(0x004eb9d0u); }
RH_ScopedInstall(PipelineFragment4eb9d0Create, 0x00496db0);
