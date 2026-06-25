// Mashed RE — camera-path node predicates (any-active-node tests).
//
// Both functions iterate the camera-path node arrays and, for each ACTIVE node
// (sub-item count != 0), invoke the original inner per-node predicate with a
// NON-STANDARD register convention (EAX = node sub-item base, EBX = sub-item
// count, param_1 forwarded on the stack; cdecl: caller cleans param_1). The
// inner predicates stay ORIGINAL (we only replicate the OUTER scan), so the
// 0/1 result is bit-identical:
//   0x0047c230  FUN_0047c230 (C2) — all sub-items state == 2
//   0x0047c1f0  FUN_0047c1f0 (C2) — all sub-items state != 0
//
// Refs: re/analysis/bucket_gameplay_0047ba20_0047f380/{0047c270,0047c2d0,
//       0047c230,0047c1f0}.md  (outer scan disasm verified verbatim).
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// Camera-path node arrays (cited at 0x0047c270 / 0x0047c2d0):
constexpr std::uintptr_t kNodeCount_6c2fe8   = 0x006c2fe8u;  // int   node count
constexpr std::uintptr_t kSubCountArr_6c2fa8 = 0x006c2fa8u;  // int[] sub-item count, stride 4
constexpr std::uintptr_t kNodeRecArr_6c27a8  = 0x006c27a8u;  // node records, stride 0x80
constexpr std::uint32_t  kNodeStride         = 0x80u;

constexpr std::uintptr_t kInnerAllEq2_47c230 = 0x0047c230u;
constexpr std::uintptr_t kInnerAllNz_47c1f0  = 0x0047c1f0u;

// Invoke an original inner predicate that reads EAX = sub-item base,
// EBX = sub-item count, and param_1 on the stack (cdecl — caller cleans).
// Preserves EBX for our caller; returns the predicate's EAX result.
//   0x0047c29b  push ebp ; mov eax,edi ; call 0x47c230 ; add esp,4   (original)
__declspec(naked) int __cdecl InvokeInner(void* /*base*/, int /*count*/,
                                          int /*param_1*/, void* /*target*/) {
    __asm {
        push ebx
        mov  eax, [esp+8]    // base    (arg0)
        mov  ebx, [esp+12]   // count   (arg1)
        mov  ecx, [esp+16]   // param_1 (arg2)
        mov  edx, [esp+20]   // target  (arg3)
        push ecx             // stack arg param_1
        call edx             // inner(EAX=base, EBX=count, [esp+4]=param_1)
        add  esp, 4          // pop param_1 (cdecl)
        pop  ebx
        ret
    }
}

// Shared outer scan: "does ANY active node satisfy the inner predicate?"
// Verbatim control flow of 0x0047c270 / 0x0047c2d0 (count reloaded each iter).
inline std::uint32_t AnyActiveNode(int param_1, std::uintptr_t innerTarget) {
    int count = *reinterpret_cast<volatile int*>(kNodeCount_6c2fe8);
    if (count <= 0) return 0u;
    for (int i = 0; i < count; ++i) {
        int sub = reinterpret_cast<const int*>(kSubCountArr_6c2fa8)[i];
        if (sub != 0) {
            void* nodeBase =
                reinterpret_cast<void*>(kNodeRecArr_6c27a8 + i * kNodeStride);
            if (InvokeInner(nodeBase, sub, param_1,
                            reinterpret_cast<void*>(innerTarget)) != 0)
                return 1u;
        }
        count = *reinterpret_cast<volatile int*>(kNodeCount_6c2fe8);  // reload
    }
    return 0u;
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x0047c270  CameraPathAllNodesEq2 — any active node whose sub-items are all == 2.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl CameraPathAllNodesEq2(int param_1) {
    return AnyActiveNode(param_1, kInnerAllEq2_47c230);
}
RH_ScopedInstall(CameraPathAllNodesEq2, 0x0047c270);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0047c2d0  CameraPathAnyNodeNonzero — any active node whose sub-items are all != 0.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl CameraPathAnyNodeNonzero(int param_1) {
    return AnyActiveNode(param_1, kInnerAllNz_47c1f0);
}
RH_ScopedInstall(CameraPathAnyNodeNonzero, 0x0047c2d0);
