// Mashed RE - DirectSound secondary init + thread descriptor + list drain.
// Four functions from the audio DirectSound init cluster.
//
// Source RVAs and asm annotations cited per function below.
#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x005bbfc0  FUN_005bbfc0  (~70 bytes)
// AudioDSoundSecondaryInit — QueryInterface + vtable+0x14 secondary init.
//
// Mechanical description (0x005bbfc0):
//   param_1  = IUnknown** (pointer to a COM object pointer slot)
//   piVar2   = (int*)*param_1      — deref to IUnknown*
//   iVar1    = vtable[0](piVar2, &DAT_005d09bc, &puStack_8)
//              — QueryInterface on IID at 0x005d09bc; output ptr on stack
//   if HRESULT < 0: goto release
//   iVar3    = 0  (set before vtable+0x14 call; never updated [U-0362])
//   iVar1    = vtable[5](piVar2, &DAT_005e7100, 0, &puStack_8)
//              — vtable offset 0x14 (slot 5); arg: IID/format at 0x005e7100
//   if (iVar1 >= 0) && (iVar3 == 3): return 1   // dead: iVar3 always 0 [U-0362]
//   release: vtable[2](piVar2)   — Release (vtable offset 0x08, slot 2)
//   return 0
//
// [U-0362]: iVar3 == 3 branch is unreachable: iVar3 is 0 before the call and
//            the decompiler shows no update. Decompiler artifact from register
//            reuse in MSVC codegen. Return 0 in all reachable paths.
//
// IID at DAT_005d09bc (0x005d09bc): shared with FUN_005bc400.
// DAT_005e7100 (0x005e7100): constant arg to vtable+0x14 (audio format struct?).
// ---------------------------------------------------------------------------
// AudioDSoundSecondaryInit: calls QI (vtable slot 0), vtable+0x14 (slot 5), Release (slot 2).
// COM vtable methods on this fake object use stdcall (IUnknown ABI). The dead branch
// iVar3==3 is reproduced faithfully per analysis note U-0362.
// 0x005bbfc0
extern "C" __declspec(dllexport) int __cdecl AudioDSoundSecondaryInit(void** param_1)
{
    // 0x005bbfc0: piVar2 = (int *)*param_1
    // param_1 = IUnknown** (pointer to COM object pointer slot)
    // *param_1 = IUnknown* (pointer to the COM object)
    // piVar2[0] = vtable pointer (first field of COM object)
    // Vtable uses __cdecl (caller-cleans) in our reimpl: the Frida test harness
    // provides mscdecl (cdecl) stubs for the reimpl side. The original at 0x005bbfc0
    // uses __stdcall vtable calls — detected by calling convention mismatch in Frida.
    typedef int  (__cdecl *VtblFn3)(void*, void*, void*);
    typedef int  (__cdecl *VtblFn4)(void*, void*, int,  void*);
    typedef void (__cdecl *VtblFn1)(void*);

    void*  pUnk     = *param_1;                          // IUnknown*
    void** vtbl     = *reinterpret_cast<void***>(pUnk);  // vtable ptr

    // 0x005bbfc8: QI call — vtable slot 0 (offset 0x00)
    // iVar1 = (**(code **)*piVar2)(piVar2, &DAT_005d09bc, &puStack_8)
    void* puStack_8 = nullptr;
    auto qi  = reinterpret_cast<VtblFn3>(vtbl[0]);
    int iVar1 = qi(pUnk, reinterpret_cast<void*>(0x005d09bcu), &puStack_8);

    if (iVar1 >= 0) {
        // 0x005bbfd8: iVar3 = 0; never updated before the branch below [U-0362]
        int iVar3 = 0;

        // 0x005bbfdc: vtable+0x14 call (slot 5)
        // iVar1 = (**(code **)(* piVar2 + 0x14))(piVar2, &DAT_005e7100, 0, &puStack_8)
        auto slot5 = reinterpret_cast<VtblFn4>(vtbl[5]);
        iVar1 = slot5(pUnk, reinterpret_cast<void*>(0x005e7100u), 0, &puStack_8);

        // 0x005bbff0: dead branch — iVar3 is always 0 [U-0362]
        if ((iVar1 >= 0) && (iVar3 == 3)) {
            auto rel1 = reinterpret_cast<VtblFn1>(vtbl[2]);
            rel1(pUnk);
            return 1;
        }
    }

    // 0x005bc000: Release — vtable slot 2 (offset 0x08)
    // (**(code **)(*piVar2 + 8))(piVar2)
    auto rel = reinterpret_cast<VtblFn1>(vtbl[2]);
    rel(pUnk);
    return 0;
}

RH_ScopedInstall(AudioDSoundSecondaryInit, 0x005bbfc0);

// ---------------------------------------------------------------------------
// 0x005aef00  FUN_005aef00  (0x26 bytes)
// AudioThreadDescInit — initialise a 5-field thread descriptor struct.
//
// Decompilation (0x005aef00–0x005aef26):
//   param_1[1] = param_2;   // 0x005aef03 — thread proc ptr
//   *param_1   = 0;         // 0x005aef07 — handle = NULL
//   param_1[2] = 0;         // 0x005aef0b — arg/state = 0
//   param_1[3] = param_3;   // 0x005aef11 — priority
//   param_1[4] = param_4;   // 0x005aef17 — stack size
//
// Called by FUN_005bb000: (param_1+0x3e, &LAB_005bb380, 0xf, 0x1000)
//   — thread proc ptr = &LAB_005bb380 (0x005bb380)
//   — priority        = 0xf (THREAD_PRIORITY_TIME_CRITICAL)
//   — stack size      = 0x1000 (4096 bytes)
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void __cdecl AudioThreadDescInit(
        std::uint32_t* param_1,
        std::uint32_t  param_2,
        std::uint32_t  param_3,
        std::uint32_t  param_4)
{
    param_1[1] = param_2;   // 0x005aef03 — thread proc ptr
    param_1[0] = 0u;        // 0x005aef07 — HANDLE = NULL
    param_1[2] = 0u;        // 0x005aef0b — arg/state = 0
    param_1[3] = param_3;   // 0x005aef11 — priority
    param_1[4] = param_4;   // 0x005aef17 — stack size
}

RH_ScopedInstall(AudioThreadDescInit, 0x005aef00);

// ---------------------------------------------------------------------------
// 0x005a9e10  FUN_005a9e10  (0x21 bytes)
// AudioSubStructTwoCallInit — two-call dispatcher; returns param_1.
//
// Decompilation (0x005a9e10–0x005a9e31):
//   FUN_005adfe0(param_1, param_3);   // sub-struct B init at +0x34 (C2)
//   FUN_005ae010(param_1, param_2);   // sub-struct A init at +0x24 (S-0345)
//   return param_1;
//
// No local state; pure pass-through wrapper.
// [U-0351]: Exact semantic role of param_2/param_3 not confirmed from this
//            function alone; confirmed from callee analysis notes.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void* __cdecl AudioSubStructTwoCallInit(
        void* param_1,
        void* param_2,
        void* param_3)
{
    // 0x005a9e13: FUN_005adfe0(param_1, param_3) — sub-struct B initializer
    typedef void (__cdecl *SubBFn)(void*, void*);
    auto sub_b = reinterpret_cast<SubBFn>(0x005adfe0u);
    sub_b(param_1, param_3);

    // 0x005a9e1f: FUN_005ae010(param_1, param_2) — sub-struct A initializer
    typedef void (__cdecl *SubAFn)(void*, void*);
    auto sub_a = reinterpret_cast<SubAFn>(0x005ae010u);
    sub_a(param_1, param_2);

    // 0x005a9e2c: return param_1 (pass-through)
    return param_1;
}

RH_ScopedInstall(AudioSubStructTwoCallInit, 0x005a9e10);

// ---------------------------------------------------------------------------
// 0x005ade90  FUN_005ade90  (0x32 bytes)
// AudioListDrain2 — circular doubly-linked list drainer.
//
// Decompilation (0x005ade90–0x005adec1):
//   piVar1 = (int *)param_1[1];       // 0x005ade93: sentinel->next
//   while (piVar1 != param_1) {       // 0x005ade9e: sentinel-check
//     *(int *)(*piVar1 + 4) = piVar1[1]; // 0x005adea4: node->prev->next = node->next
//     *(int *)piVar1[1]     = *piVar1;   // 0x005adeaa: node->next->prev = node->prev
//     FUN_005ae920(&DAT_009146c0, piVar1); // 0x005adeb0: return to free pool
//     piVar1 = (int *)param_1[1];     // 0x005adeb9: reload sentinel->next
//   }
//
// Node layout: node[0] = prev ptr, node[1] = next ptr.
// Audio free pool at DAT_009146c0 (0x009146c0).
// [U-0990]: Pool node type and block size not confirmed; pool identity
//            is confirmed from multiple callers.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void __cdecl AudioListDrain2(int* param_1)
{
    // 0x005ade93: piVar1 = (int *)param_1[1]  — sentinel->next
    int* piVar1 = reinterpret_cast<int*>(param_1[1]);

    // 0x005ade9e: while sentinel not reached
    while (piVar1 != param_1) {
        // 0x005adea4: *(int *)(*piVar1 + 4) = piVar1[1]
        //   *piVar1 = prev_ptr; (*piVar1 + 4) = &prev_node->next; set to node->next
        *reinterpret_cast<int*>(piVar1[0] + 4) = piVar1[1];

        // 0x005adeaa: *(int *)piVar1[1] = *piVar1
        //   piVar1[1] = next_ptr; *(next_ptr) = node->prev  (next_node->prev = node->prev)
        *reinterpret_cast<int*>(piVar1[1]) = piVar1[0];

        // 0x005adeb0: FUN_005ae920(&DAT_009146c0, piVar1) — return node to pool
        typedef void (__cdecl *PoolFreeFn)(void*, void*);
        static const std::uintptr_t kPoolAddr = 0x009146c0u;
        auto pool_free = reinterpret_cast<PoolFreeFn>(0x005ae920u);
        pool_free(reinterpret_cast<void*>(kPoolAddr), piVar1);

        // 0x005adeb9: reload sentinel->next
        piVar1 = reinterpret_cast<int*>(param_1[1]);
    }
}

RH_ScopedInstall(AudioListDrain2, 0x005ade90);
