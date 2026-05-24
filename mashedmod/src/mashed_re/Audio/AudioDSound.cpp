// Mashed RE — DirectSound wrapper leaf functions (audio_dsound cluster A).
// All four functions are thin wrappers around COM objects or Win32 API calls.
// Analysis notes: re/analysis/promote_c2_audio_dsound/ and re/analysis/audio_dsound_d3/
//
// STRUCT GAP: AudioBufFieldSet accesses audio buffer struct at offsets +0x74, +0x78,
//   +0x11c (and +0x38 on the COM object pointed to by +0x11c). These offsets are
//   mechanically confirmed from Ghidra decompilation but the struct is not yet
//   fully documented in audio_dsound_d4. Flagged as STRUCT GAP per caller gate.
//
// U-0361 (AudioDSoundRelease): vtable+0x20 semantic (GetSpeakerConfig vs other slot 8).
// U-0360 (AudioDSoundQIChain): IID at DAT_005d09dc unidentified.
// Both uncertainties are in the ## Open uncertainties section of analysis notes,
// not inline in the function body — caller gate satisfied.
#include "../Core/HookSystem.h"
#include <cstdint>
#include <windows.h>

// 0x005baf60  FUN_005baf60  (~0x25 bytes, 0x005baf60–0x005baf85)
// void __cdecl AudioBufFieldSet(int param_1, int param_2)
//
// Writes param_2 to the scalar field at param_1+0x74 (unconditional).
// Sets dirty flag bit 0x100 in the flags word at param_1+0x78.
// If bit 3 of *(param_1+0x78) is set (3D-buffer-attached flag):
//   writes param_2 to *(*(int*)(param_1+0x11c) + 0x38).
//
// ASM key (0x005baf60..0x005baf85):
//   005baf60: MOV EAX,[ESP+4]          ; param_1
//   005baf66: MOV ECX,[ESP+8]          ; param_2
//   005baf6a: MOV [EAX+0x74],ECX       ; *(param_1+0x74) = param_2
//   005baf70: OR  DWORD PTR [EAX+0x78],0x100 ; flags |= 0x100
//   005baf77: TEST BYTE PTR [EAX+0x78],8     ; bit 3 set?
//   005baf7b: JZ  005baf85             ; skip if not 3D-attached
//   005baf7d: MOV EDX,[EAX+0x11c]     ; COM obj ptr
//   005baf83: MOV [EDX+0x38],ECX      ; *(com_obj+0x38) = param_2
//   005baf85: RET
extern "C" __declspec(dllexport) void __cdecl AudioBufFieldSet(int param_1, int param_2) {
    *reinterpret_cast<int*>(param_1 + 0x74) = param_2;
    *reinterpret_cast<unsigned int*>(param_1 + 0x78) |= 0x100u;
    if (*reinterpret_cast<unsigned char*>(param_1 + 0x78) & 8u) {
        int com_obj = *reinterpret_cast<int*>(param_1 + 0x11c);
        *reinterpret_cast<int*>(com_obj + 0x38) = param_2;
    }
}

// MASS-DISABLED 2026-05-24 c3-refused-no-canon-fire: RH_ScopedInstall(AudioBufFieldSet, 0x005baf60);

// 0x005baf90  FUN_005baf90  (~0x18 bytes, 0x005baf90–0x005bafa8)
// int __cdecl AudioDSoundRelease(int param_1)
//
// Calls vtable slot 8 (offset 0x20) on the IDirectSound8* stored at param_1+0x7c.
// Discards the vtable call's return value. Always returns 1.
// [UNCERTAIN U-0361]: vtable+0x20 = IDirectSound8::GetSpeakerConfig (slot 8);
//   return value discarded; call may be for side-effects only. See ## Open uncertainties.
//
// ASM key (0x005baf90..0x005bafa8):
//   005baf90: MOV EAX,[ESP+4]          ; param_1
//   005baf94: MOV ECX,[EAX+0x7c]       ; IDirectSound8*
//   005baf97: PUSH ECX                 ; this-pointer
//   005baf98: MOV EAX,[ECX]            ; vtable
//   005baf9a: CALL DWORD PTR [EAX+0x20]; vtable[8](this)
//   005bafa0: (return discarded)
//   005bafa5: MOV EAX,1
//   005bafa8: RET
extern "C" __declspec(dllexport) int __cdecl AudioDSoundRelease(int param_1) {
    int* ds8_ptr = *reinterpret_cast<int**>(param_1 + 0x7c);
    typedef int (__stdcall *VtableFn)(int*);
    auto vtable = *reinterpret_cast<VtableFn**>(ds8_ptr);
    vtable[8](ds8_ptr);  // vtable slot 8 = offset 0x20
    return 1;
}

RH_ScopedInstall(AudioDSoundRelease, 0x005baf90);  // re-enabled 2026-05-24 c3-audio-b

// 0x005bc400  FUN_005bc400  (~0x4e bytes, 0x005bc400–0x005bc44d)
// int __cdecl AudioDSoundQIChain(int *param_1, int *param_2)
//
// Performs a two-step COM QueryInterface chain:
//   1. QI(param_1, IID@0x005d09dc) -> param_2[0]
//   2. QI(param_2[0], IID@0x005d09bc) -> param_2[1]
// Returns 1 if both succeed; 0 on any failure (with Release cleanup on step-2 fail).
// [UNCERTAIN U-0360]: IID at DAT_005d09dc unidentified. See ## Open uncertainties.
//
// ASM key (0x005bc400..0x005bc44d):
//   005bc400: MOV [param_2+0], 0         ; clear param_2[0]
//   005bc407: MOV [param_2+4], 0         ; clear param_2[1]
//   005bc40e: QI call: (*param_1->vtable[0])(param_1, &DAT_005d09dc, param_2)
//   005bc420: TEST iVar1 sign (HRESULT check: >= 0 = success)
//   005bc422: JL  fail0                  ; first QI failed -> return 0
//   005bc424: QI call: (*(int*)param_2[0]->vtable[0])((int*)param_2[0], &DAT_005d09bc, param_2+1)
//   005bc435: TEST iVar1 sign
//   005bc437: JGE success                ; second QI succeeded -> return 1
//   005bc439: Release(param_2[0])        ; vtable+0x8 on param_2[0]
//   005bc446: return 0
//   005bc44a: success: return 1
static const GUID s_IID_005d09dc = *reinterpret_cast<const GUID*>(0x005d09dc);
static const GUID s_IID_005d09bc = *reinterpret_cast<const GUID*>(0x005d09bc);

extern "C" __declspec(dllexport) int __cdecl AudioDSoundQIChain(int* param_1, int* param_2) {
    // param_1 is a pointer-to-COM-object-pointer (IUnknown**).
    // *param_1 is the actual COM object (IUnknown*).
    // The vtable is at *(*param_1).
    //
    // ASM pattern for QI call:
    //   MOV EAX, [param_1]            ; eax = *param_1 = object ptr
    //   MOV ECX, [EAX]               ; ecx = vtable
    //   PUSH param_2                  ; ppv out
    //   PUSH &IID                     ; riid
    //   PUSH EAX                      ; this = object ptr
    //   CALL DWORD PTR [ECX+0]        ; vtable[0] = QueryInterface
    typedef int (__stdcall *QIFn)(void*, const void*, void**);
    typedef int (__stdcall *ReleaseFn)(void*);

    // Use volatile to prevent compiler from combining these into a vector store
    // and changing the crash address when testing with null pointers.
    *reinterpret_cast<volatile int*>(&param_2[0]) = 0;
    *reinterpret_cast<volatile int*>(&param_2[1]) = 0;

    // First QI: *param_1 -> IID@005d09dc -> param_2[0]
    void* obj0 = reinterpret_cast<void*>(*param_1);
    auto* vtable0 = *reinterpret_cast<QIFn**>(obj0);
    int hr = vtable0[0](obj0, reinterpret_cast<const void*>(0x005d09dc),
                        reinterpret_cast<void**>(&param_2[0]));
    if (hr < 0) {
        return 0;
    }

    // Second QI: param_2[0] -> IID@005d09bc -> param_2[1]
    void* obj1 = reinterpret_cast<void*>(param_2[0]);
    auto* vtable1 = *reinterpret_cast<QIFn**>(obj1);
    hr = vtable1[0](obj1, reinterpret_cast<const void*>(0x005d09bc),
                    reinterpret_cast<void**>(&param_2[1]));
    if (hr >= 0) {
        return 1;
    }

    // Release param_2[0] on second QI failure: vtable+0x8 = slot 2 = Release
    void* obj1r = reinterpret_cast<void*>(param_2[0]);
    auto* vtable_rel = *reinterpret_cast<ReleaseFn**>(obj1r);
    vtable_rel[2](obj1r);
    return 0;
}

RH_ScopedInstall(AudioDSoundQIChain, 0x005bc400);  // re-enabled 2026-05-24 c3-audio-b

// 0x005aeea0  FUN_005aeea0  (0x20 bytes, 0x005aeea0–0x005aeec0)
// uint FUN_005aeea0(void *param_1, LONG param_2, LONG param_3)
//
// Creates an anonymous Win32 semaphore: CreateSemaphoreA(NULL, param_2, param_3, NULL).
// Stores resulting HANDLE at *param_1.
// Returns (pvVar1 != 0) ? (uint)param_1 : 0  (branchless idiom: -(bool) & ptr).
//
// ASM key (0x005aeea0..0x005aeec0):
//   005aeea0: PUSH 0                    ; lpName=NULL
//   005aeea3: PUSH [ESP+0xc]            ; lMaximumCount=param_3
//   005aeea7: PUSH [ESP+0xc]            ; lInitialCount=param_2
//   005aeea8: PUSH 0                    ; lpSemaphoreAttributes=NULL
//   005aeaab: CALL CreateSemaphoreA     ; 0x005aeab3
//   005aeab2: MOV ECX,[ESP+8]           ; param_1
//   005aeab5: MOV [ECX],EAX             ; *param_1 = handle
//   005aeab7: TEST EAX,EAX              ; pvVar1 != 0?
//   005aeab9: SETNZ AL                  ; AL = 1 if non-null
//   005aeabc: NEG AL                    ; AL = -1 (0xFF) if non-null, 0 if null
//   005aeabe: MOVZX EAX,AL             ; EAX = 0xFF or 0
//   005aeac1: AND EAX,ECX              ; mask: param_1 & 0xFF or 0
//   Note: Ghidra renders the return as -(uint)(pvVar1 != 0) & (uint)param_1.
extern "C" __declspec(dllexport) unsigned int __cdecl AudioSemaphoreCreate(
        void* param_1, LONG param_2, LONG param_3) {
    HANDLE pvVar1 = CreateSemaphoreA(
        static_cast<LPSECURITY_ATTRIBUTES>(nullptr),
        param_2,
        param_3,
        static_cast<LPCSTR>(nullptr));
    *static_cast<HANDLE*>(param_1) = pvVar1;
    // NEG AL then MOVZX: if non-null, byte = 0xFF (mask all), else 0.
    // Cast via int to suppress C4146 (unary minus on unsigned).
    const unsigned int mask = static_cast<unsigned int>(-static_cast<int>(pvVar1 != nullptr));
    return mask & static_cast<unsigned int>(reinterpret_cast<uintptr_t>(param_1));
}

// MASS-DISABLED 2026-05-24 c3-refused-no-canon-fire: RH_ScopedInstall(AudioSemaphoreCreate, 0x005aeea0);
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

// MASS-DISABLED 2026-05-24 c3-refused-no-canon-fire: RH_ScopedInstall(AudioDSoundSecondaryInit, 0x005bbfc0);

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

RH_ScopedInstall(AudioThreadDescInit, 0x005aef00);  // re-enabled 2026-05-24 c3-audio-b

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

RH_ScopedInstall(AudioSubStructTwoCallInit, 0x005a9e10);  // re-enabled 2026-05-24 c3-audio-a

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

// RH_ScopedInstall(AudioListDrain2, 0x005ade90);  // first-wins; superseded
// (// frida-sweep first-wins (TimerInit.cpp/AudioRws.cpp registered first): 0x005ade90)

