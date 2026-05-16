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

RH_ScopedInstall(AudioBufFieldSet, 0x005baf60);

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

RH_ScopedInstall(AudioDSoundRelease, 0x005baf90);

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

RH_ScopedInstall(AudioDSoundQIChain, 0x005bc400);

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

RH_ScopedInstall(AudioSemaphoreCreate, 0x005aeea0);
