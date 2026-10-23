// Mashed RE — DirectSound wrapper leaf functions (audio_dsound cluster A).
// Seven thin wrappers around COM objects or Win32 API calls. In both rsp files since audiolink slice 6 (2026-10-23):
// one body each, no MASHED.exe address in either arm. Bytes: verify/audiolink_20261023/disasm_slice6.txt and
// bytes_step.txt (read_bytes.py on MASHED.exe.unpatched).
// Analysis notes: re/analysis/promote_c2_audio_dsound/ and re/analysis/audio_dsound_d3/
//
// STRUCT GAP: AudioBufFieldSet accesses audio buffer struct at offsets +0x74, +0x78,
//   +0x11c (and +0x38 on the COM object pointed to by +0x11c). These offsets are
//   mechanically confirmed from Ghidra decompilation but the struct is not yet
//   fully documented in audio_dsound_d4. Flagged as STRUCT GAP per caller gate.
//
// U-0361 (AudioDSoundRelease): vtable+0x20 semantic (GetSpeakerConfig vs other slot 8).
// U-0360 (AudioDSoundQIChain): RESOLVED audiolink slice 6. The IID bytes at 0x005d09dc are
//   84fa9a27 8149 ce11 a521 0020af0be560 = IID_IDirectSound3DListener (Windows SDK dsound.h:861); the ones at
//   0x005d09bc are IID_IKsPropertySet (dsound.h:1212).
// Both uncertainties are in the ## Open uncertainties section of analysis notes,
// not inline in the function body — caller gate satisfied.
#include "../Core/HookSystem.h"
#include <cstdint>
#include <windows.h>

namespace {
// The GUIDs the original passes by address, as constants with the image's bytes (identical in both arms; a COM
// QueryInterface / QuerySupport reads the 16 bytes, not the address).
const GUID kIID_DirectSound3DListener =                        // 0x005d09dc, dsound.h:861
    { 0x279AFA84, 0x4981, 0x11CE, { 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60 } };
const GUID kIID_KsPropertySet =                                // 0x005d09bc, dsound.h:1212
    { 0x31EFAC30, 0x515C, 0x11D0, { 0xA9, 0xAA, 0x00, 0xAA, 0x00, 0x61, 0xBE, 0x93 } };
// 0x005e7100: the property-set GUID handed to IKsPropertySet slot 5 (QuerySupport(REFGUID, ULONG, PULONG),
// dsound.h IKsPropertySet order). Bytes e06853cd 5034 d311 8b6e 00105a9b7bbc; not in the SDK headers, so unnamed.
// QuerySupport reads 16 bytes; the floats that follow it in .rdata are not part of the argument.
const GUID kPropSet_005e7100 =
    { 0xCD5368E0, 0x3450, 0x11D3, { 0x8B, 0x6E, 0x00, 0x10, 0x5A, 0x9B, 0x7B, 0xBC } };
}  // namespace

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

RH_ScopedInstall(AudioBufFieldSet, 0x005baf60);  // re-enabled 2026-05-24 phase-a1 buf_field_set GREEN

// 0x005baf90  FUN_005baf90  (0x4c bytes, 0x005baf90..0x005bafdb, jump table 0x005bafdc)
// int __cdecl AudioDSoundRelease(int param_1)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn). The name is historical: the body
// calls slot 8 of the object at param_1+0x7c with an OUT pointer and maps the result.
//   0x005baf90  eax = param_1
//   0x005baf94  edx = &[esp+4]            ; the caller's arg slot, still holding param_1
//   0x005baf98  push edx                  ; arg 2: out pointer
//   0x005baf99  eax = [param_1+0x7c]      ; COM object
//   0x005baf9c  push eax                  ; arg 1: this
//   0x005baf9f  call [[eax]+0x20]         ; slot 8, TWO stack args, callee cleans (stdcall);
//                                         ; return value discarded
//   0x005bafa2  eax = [esp+4]             ; the out dword (param_1 if the method left it)
//   0x005bafa6  and eax,0xff ; dec eax ; cmp eax,6 ; ja 0x005bafd6 (unsigned)
//   0x005bafb1  jmp [eax*4+0x005bafdc]: byte 1 -> 3 (0x005bafbe), 2 -> 2 (0x005bafb8),
//               3 -> 1, 4 -> 1 (0x005bafd6), 5 -> 4 (0x005bafc4), 6 -> 5 (0x005bafca),
//               7 -> 6 (0x005bafd0); any other byte (0 or 8..255) -> 1 (0x005bafd6)
// [UNCERTAIN U-0361]: slot 8 identity. The old port called it with one argument
// (unbalanced against a callee-cleans 2-arg method) and always returned 1.
extern "C" __declspec(dllexport) int __cdecl AudioDSoundRelease(int param_1) {
    typedef int (__stdcall *Slot8Fn)(void*, std::uint32_t*);
    std::uint32_t out = static_cast<std::uint32_t>(param_1);                 // 0x005baf94
    void* const obj = *reinterpret_cast<void**>(param_1 + 0x7c);             // 0x005baf99
    Slot8Fn const fn = (*reinterpret_cast<Slot8Fn**>(obj))[8];               // 0x005baf9d / 0x005baf9f
    fn(obj, &out);                                                           // 0x005baf9f
    switch ((out & 0xffu) - 1u) {                                            // 0x005bafa6..0x005bafb1
        case 0: return 3;   // 0x005bafbe
        case 1: return 2;   // 0x005bafb8
        case 4: return 4;   // 0x005bafc4
        case 5: return 5;   // 0x005bafca
        case 6: return 6;   // 0x005bafd0
        default: return 1;  // 0x005bafd6 (table entries 2, 3 and the ja path)
    }
}

RH_ScopedInstall(AudioDSoundRelease, 0x005baf90);  // re-enabled 2026-05-24 c3-audio-b

// 0x005bc400  FUN_005bc400  (0x4e bytes, 0x005bc400..0x005bc44d)
// int __cdecl AudioDSoundQIChain(IUnknown *param_1, void **param_2)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn). param_1 IS the COM object:
//   0x005bc400  eax = param_1 ; 0x005bc405 esi = param_2 ; 0x005bc410 edi = param_2 + 4
//   0x005bc413  param_2[0] = 0 ; 0x005bc41a param_2[1] = 0
//   0x005bc420  ecx = [eax] ; 0x005bc422 call [ecx]   ; QI(param_1, 0x005d09dc, &param_2[0])
//   0x005bc426  hr < 0 -> return 0 (0x005bc449)
//   0x005bc428  eax = param_2[0] ; 0x005bc433 call [[eax]] ; QI(param_2[0], 0x005d09bc, &param_2[1])
//   0x005bc437  hr >= 0 -> return 1 (0x005bc43a)
//   0x005bc441  esi = param_2[0] (re-read) ; 0x005bc446 call [[esi]+8] ; Release, result dropped
//   0x005bc44a  return 0
// U-0360 resolved (header). The old port treated param_1 as IUnknown** and dereferenced it once more.
extern "C" __declspec(dllexport) int __cdecl AudioDSoundQIChain(int* param_1, int* param_2) {
    typedef int (__stdcall *QIFn)(void*, const void*, void**);
    typedef int (__stdcall *ReleaseFn)(void*);

    void* const obj = param_1;                                               // 0x005bc400
    param_2[0] = 0;                                                          // 0x005bc413
    param_2[1] = 0;                                                          // 0x005bc41a

    int hr = (*reinterpret_cast<QIFn**>(obj))[0](
        obj, &kIID_DirectSound3DListener, reinterpret_cast<void**>(&param_2[0]));   // 0x005bc40b / 0x005bc422
    if (hr < 0) return 0;                                                    // 0x005bc426

    void* const obj0 = reinterpret_cast<void*>(param_2[0]);                  // 0x005bc428
    hr = (*reinterpret_cast<QIFn**>(obj0))[0](
        obj0, &kIID_KsPropertySet, reinterpret_cast<void**>(&param_2[1]));          // 0x005bc42b / 0x005bc433
    if (hr >= 0) return 1;                                                   // 0x005bc437

    void* const objr = reinterpret_cast<void*>(param_2[0]);                  // 0x005bc441
    (*reinterpret_cast<ReleaseFn**>(objr))[2](objr);                         // 0x005bc446
    return 0;                                                                // 0x005bc44a
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
//   (audiolink slice 6, from the bytes: the tail is 005aeeba NEG EAX / 005aeebc SBB EAX,EAX / 005aeebe AND EAX,ECX,
//    a full 32-bit mask, so the result is param_1 or 0. The SETNZ/AL lines that stood here were not the bytes.)
//   Note: Ghidra renders the return as -(uint)(pvVar1 != 0) & (uint)param_1.
extern "C" __declspec(dllexport) unsigned int __cdecl AudioSemaphoreCreate(
        void* param_1, LONG param_2, LONG param_3) {
    HANDLE pvVar1 = CreateSemaphoreA(
        static_cast<LPSECURITY_ATTRIBUTES>(nullptr),
        param_2,
        param_3,
        static_cast<LPCSTR>(nullptr));
    *static_cast<HANDLE*>(param_1) = pvVar1;
    // NEG / SBB: 0xffffffff if non-null, else 0. Cast via int to suppress C4146 (unary minus on unsigned).
    const unsigned int mask = static_cast<unsigned int>(-static_cast<int>(pvVar1 != nullptr));
    return mask & static_cast<unsigned int>(reinterpret_cast<uintptr_t>(param_1));
}

RH_ScopedInstall(AudioSemaphoreCreate, 0x005aeea0);  // re-enabled 2026-05-24 phase-a1 semaphore_create GREEN
// 0x005bbfc0  FUN_005bbfc0  (0x58 bytes, 0x005bbfc0..0x005bc017)
// int __cdecl AudioDSoundSecondaryInit(IUnknown** param_1)
//
// Re-ported from the bytes, audiolink slice 6 (2026-10-23):
//   005bbfc0 push ecx                        ; local ks = [esp]
//   005bbfc9 [esp] = 0
//   005bbfd2 eax = *param_1                  ; pUnk
//   005bbfdc QI(pUnk, IID_IKsPropertySet 0x005d09bc, &ks)          (stdcall, callee pops)
//   005bbfe0 jl 0x005bc014                   ; QI failed: return 0, NO Release
//   005bbfe6 edx = &[esp+8]                  ; the caller's param_1 slot
//   005bbff5 ks->vtbl[5](ks, 0x005e7100, 0, &param_1 slot)        ; IKsPropertySet::QuerySupport
//   005bbffa jl 0x005bc00a
//   005bbffc cmp [esp+8],3 / jne 0x005bc00a  ; support word written into the param_1 slot
//   005bc003 return 1                        ; ks is NOT released on this path
//   005bc00a eax = [esp] (= ks) / call [[eax]+8] ; Release(ks)
//   005bc014 return 0
// The previous port released pUnk (the original's vtable) on every failure path, including a failed QI. The bytes
// release the QI output ks, and only after a successful QI.
extern "C" __declspec(dllexport) int __cdecl AudioDSoundSecondaryInit(void** param_1)
{
    typedef int  (__stdcall *QIFn)(void*, const void*, void**);
    typedef int  (__stdcall *QuerySupportFn)(void*, const void*, unsigned long, void*);
    typedef unsigned long (__stdcall *ReleaseFn)(void*);

    void* ks = nullptr;                                                   // 0x005bbfc9
    void* const pUnk = *param_1;                                          // 0x005bbfd2
    const int hr = (*reinterpret_cast<QIFn**>(pUnk))[0](pUnk, &kIID_KsPropertySet, &ks);   // 0x005bbfdc
    if (hr < 0) return 0;                                                 // 0x005bbfe0 -> 0x005bc014

    const int hr2 = (*reinterpret_cast<QuerySupportFn**>(ks))[5](        // 0x005bbff5
        ks, &kPropSet_005e7100, 0ul, static_cast<void*>(&param_1));
    if (hr2 >= 0 && reinterpret_cast<std::uintptr_t>(param_1) == 3u) {   // 0x005bbffa / 0x005bbffc
        return 1;                                                         // 0x005bc003
    }
    (*reinterpret_cast<ReleaseFn**>(ks))[2](ks);                          // 0x005bc00a..0x005bc011
    return 0;                                                             // 0x005bc014
}

RH_ScopedInstall(AudioDSoundSecondaryInit, 0x005bbfc0);  // re-enabled 2026-05-24 phase-a1 dsound_secondary_init GREEN (post-vtable-routing-fix)

// ---------------------------------------------------------------------------
// 0x005aef00  FUN_005aef00  (0x26 bytes)
// AudioThreadDescInit — initialise a 5-field thread descriptor struct.
//
// Disassembly (0x005aef00–0x005aef26), verified byte-for-byte against MASHED.exe.unpatched:
//   005aef00  MOV EAX,[ESP+0x4]      ; EAX = param_1  <-- and NOTHING clobbers EAX after this
//   005aef04  MOV ECX,[ESP+0x8]      ; param_2
//   005aef08  MOV EDX,[ESP+0xc]      ; param_3
//   005aef0c  MOV [EAX+0x4],ECX      ; thread proc ptr
//   005aef0f  MOV ECX,[ESP+0x10]     ; param_4
//   005aef13  MOV dword ptr [EAX],0  ; HANDLE = NULL
//   005aef19  MOV dword ptr [EAX+0x8],0
//   005aef20  MOV [EAX+0xc],EDX      ; priority
//   005aef23  MOV [EAX+0x10],ECX     ; stack size
//   005aef26  RET                    ; C3 — plain __cdecl, caller cleans
//
// *** RETURNS param_1 IN EAX. *** Ghidra types this `void`; that is wrong, and it is not a
// cosmetic difference — ALL THREE callers branch on the result and skip the thread spawn
// when it is zero. In FUN_005a8060 (the stream-cluster init, which also creates the
// semaphore at [0x007dcae0] via 0x005a82d2):
//
//   005a8315  CALL FUN_005aef00
//   005a831a  ADD  ESP,0x10
//   005a831d  TEST EAX,EAX
//   005a831f  JZ   0x005a8334          <- skips the spawn
//   005a8328  CALL FUN_005aef30        <- __beginthread: the audio/stream WORKER THREAD
//
// (same shape at 0x005be3db in FUN_005be260.) Because the original always leaves the
// non-null descriptor pointer in EAX, stock always takes the branch.
//
// BUGFIX 2026-07-28 — this port was declared `void`, so it returned whatever EAX happened to
// hold. When that was 0 the worker thread was NEVER CREATED: measured over 2 stock + 2 hooked
// runs, 0x005aef00 was entered once in both configs while 0x005aef30 and __beginthread
// (0x005c2e79) were entered 1× in stock and 0× hooked, and an unfiltered CreateThread census
// found exactly one game thread in every stock run and none in any hooked run. The stream
// semaphore is created before this branch, so it still exists but is never serviced, and a
// later WaitForSingleObject(..., INFINITE) hangs the GUI thread — the intermittent race-entry
// wedge U-9025. Evidence: re/analysis/u9025_stream_lock_race.md, verify/u9025_semtrace/.
//
// Its C3 Frida diff (log/diff_audio_thread_desc_init.csv, GREEN 10/10) was structurally blind
// to this: arg_type `thread_desc_init` fingerprints the five written fields and never reads
// the return value. See [[feedback-installed-hook-abi-mismatch]].
//
// Called by FUN_005a8060, FUN_005bb000, FUN_005be260 — e.g. (buf, &LAB_005bb380, 0xf, 0x1000):
//   — thread proc ptr = &LAB_005bb380 (0x005bb380)
//   — priority        = 0xf (THREAD_PRIORITY_TIME_CRITICAL)
//   — stack size      = 0x1000 (4096 bytes)
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioThreadDescInit(
        std::uint32_t* param_1,
        std::uint32_t  param_2,
        std::uint32_t  param_3,
        std::uint32_t  param_4)
{
    param_1[1] = param_2;   // 0x005aef0c — thread proc ptr
    param_1[0] = 0u;        // 0x005aef13 — HANDLE = NULL
    param_1[2] = 0u;        // 0x005aef19 — arg/state = 0
    param_1[3] = param_3;   // 0x005aef20 — priority
    param_1[4] = param_4;   // 0x005aef23 — stack size
    // The original's EAX still holds param_1 at the RET; the callers TEST it.
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(param_1));
}

RH_ScopedInstall(AudioThreadDescInit, 0x005aef00);  // re-enabled 2026-05-24 c3-audio-b

// 0x005a9e10 AudioSubStructTwoCallInit moved to Audio/AudioRws.cpp (audiolink slice 6): it calls that file's
// LinkBuffer 0x005adfe0 / LinkDevice 0x005ae010, which stay asi-only until the pool family links.

// 0x005ade90: the dead second copy AudioListDrain2 was removed 2026-10-12 (U-9661). The installed body is
// AudioListDrain in Audio/AudioRws.cpp; evidence fix_diff_audio_list_drain.csv (z_fix_u9661.py).

// ---------------------------------------------------------------------------
// 0x005a9e40  FUN_005a9e40  (0x20 bytes, 0x005a9e40–0x005a9e5f)
// AudioOutputNodeCbDispatch — null-guarded dispatch through +0x3c callback.
//
// Decompilation (0x005a9e40–0x005a9e5f):
//   cb = *(code**)(param_1 + 0x3c);   // load callback fn-ptr at offset 0x3c
//   if (cb != NULL) {
//     cb(0, param_2);                  // call with fixed first arg 0
//     return param_1;                  // success: return the object ptr
//   }
//   return 0;                          // no callback installed -> return 0
//
// ASM key (0x005a9e40..0x005a9e5f):
//   005a9e40: PUSH ESI
//   005a9e41: MOV  ESI,[ESP+0x8]       ; ESI = param_1
//   005a9e45: MOV  EAX,[ESI+0x3c]      ; EAX = *(param_1+0x3c) — callback ptr
//   005a9e48: TEST EAX,EAX
//   005a9e4a: JZ   0x005a9e5c           ; null -> return 0
//   005a9e4c: MOV  ECX,[ESP+0xc]       ; ECX = param_2
//   005a9e50: PUSH ECX                  ; arg1 = param_2
//   005a9e51: PUSH 0x0                  ; arg0 = 0 (fixed)
//   005a9e53: CALL EAX                  ; call cb(0, param_2)
//   005a9e55: ADD  ESP,0x8             ; cdecl cleanup of 2 pushed args
//   005a9e58: MOV  EAX,ESI            ; return param_1
//   005a9e5a: POP  ESI
//   005a9e5b: RET
//   005a9e5c: XOR  EAX,EAX            ; return 0
//   005a9e5e: POP  ESI
//   005a9e5f: RET
//
// Callers: FUN_004627f0 (C2), FUN_005af860 (C2), FUN_005afa00 (C2), FUN_005afcf0 (C2).
// Indirect callee at +0x3c is an audio callback (semantic TBD); see U-6621.
// [UNCERTAIN U-6621]: Meaning of literal `0` first argument and +0x3c callback
//   contract. Data-semantic open; non-blocking for bit-identical leaf dispatch.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int __cdecl AudioOutputNodeCbDispatch(
        int param_1, int param_2)
{
    // 0x005a9e45: cb = *(code**)(param_1 + 0x3c)
    typedef int (__cdecl* CbFn)(int, int);
    CbFn cb = *reinterpret_cast<CbFn*>(param_1 + 0x3c);
    // 0x005a9e48: TEST EAX,EAX — null-guard
    if (cb != nullptr) {
        // 0x005a9e50..0x005a9e55: call cb(0, param_2); caller-cdecl cleanup
        cb(0, param_2);
        // 0x005a9e58: return param_1
        return param_1;
    }
    // 0x005a9e5c: return 0
    return 0;
}

RH_ScopedInstall(AudioOutputNodeCbDispatch, 0x005a9e40);

