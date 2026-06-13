// Mashed RE — promote-round session B (rounds 79-88), consolidated into one TU to stay
// under cmd.exe's ~8191-char command-line limit for the .asi cl invocation.
// Each function keeps its original RVA + byte-verification comment + RH_ScopedInstall.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>

// ===== round 79 =====
// Mashed RE — promote-round round 79 (frontier-tool validation batch: 3 display-independent leaves).
//
// FIRST batch surfaced by the new tooling pipeline:
//   scripts/promote_frontier.py  — graph-level leaf finder (capstone call-graph over
//     MASHED.exe.unpatched): C2 ∩ first-party ∩ zero-callee ∩ <260B ∩ C2+ caller.
//   scripts/promote_classify.py  — disasm-shape auto-classifier; tags each frontier
//     row AUTO (display-independent, emittable) / STATE (needs a booted game) / MANUAL.
// All three below are AUTO pure leaves diffed via early_window_leaf_diff.py (no booted
// game needed — the display is environmentally wedged this session).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)



// 0x00495520  input — byte-verified: A1 78 1E 77 00 C3
//   MOV EAX,[0x00771e78] ; RET   (read_global u32; direct caller FUN_00496040 C2)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get771e78(void) {
    return *reinterpret_cast<std::uint32_t*>(0x00771e78u);
}
RH_ScopedInstall(Get771e78, 0x00495520);

// 0x004842b0  debug-overlay — byte-verified: 33 C0 / A3.. x4 / C3
//   XOR EAX,EAX ; MOV [0x6cfc88],EAX ; MOV [0x6cf068],EAX ; MOV [0x6cf06c],EAX ;
//   MOV [0x6cfc8c],EAX ; RET   (4-global zero-clear; direct caller FUN_00484580 C2)
extern "C" __declspec(dllexport) void __cdecl Clear4842b0(void) {
    *reinterpret_cast<std::uint32_t*>(0x006cfc88u) = 0;   // MOV [0x6cfc88],EAX
    *reinterpret_cast<std::uint32_t*>(0x006cf068u) = 0;   // MOV [0x6cf068],EAX
    *reinterpret_cast<std::uint32_t*>(0x006cf06cu) = 0;   // MOV [0x6cf06c],EAX
    *reinterpret_cast<std::uint32_t*>(0x006cfc8cu) = 0;   // MOV [0x6cfc8c],EAX
}
RH_ScopedInstall(Clear4842b0, 0x004842b0);

// NOTE: 0x005c9d00 (XOR EAX,EAX; RET = GetRaceEndTrigger) was surfaced by the
// classifier as const_return 0 and diffs GREEN bit-identically — but it is DELIBERATELY
// NOT hooked here. Its body is 3 bytes; the 5-byte inline-JMP patch overwrites past the
// function boundary into the next function (confirmed crasher, demoted C3->C2 2026-05-22).
// early_window proves bit-identity (hook bypassed) but cannot catch the install-time
// corruption. Needs a 2-byte/trampoline patch mechanism before it can be a safe C3.

// ===== round 80 =====
// Mashed RE — promote-round round 80 (frontier/classifier batch: byte-getter + arg-field-clear).
//
// Surfaced by scripts/promote_frontier.py + promote_classify.py (extended recognizers:
// read_global_u8, ptr_fields_clear). Both display-independent; diffed via early_window_leaf_diff.py.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)



// 0x0042a9f0  render — byte-verified: A0 A8 EC 67 00 C3
//   MOV AL,[0x0067eca8] ; RET   (read_global u8; caller FUN_00492e90 C2)
extern "C" __declspec(dllexport) std::uint8_t __cdecl GetFadeAlpha(void) {
    return *reinterpret_cast<std::uint8_t*>(0x0067eca8u);
}
RH_ScopedInstall(GetFadeAlpha, 0x0042a9f0);

// 0x005be930  audio — byte-verified: 8B 44 24 04 33 C9 89 48 0C 89 48 10 C3
//   MOV EAX,[ESP+4] ; XOR ECX,ECX ; MOV [EAX+0xc],ECX ; MOV [EAX+0x10],ECX ; RET
//   -> fn(p): p->[0xc]=0, p->[0x10]=0, return p (eax=arg unchanged). callers 005be8c0/990/5c0c20 C2
extern "C" __declspec(dllexport) void* __cdecl AudioClear5be930(void* p) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x0c) = 0;   // MOV [EAX+0xc],ECX
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x10) = 0;   // MOV [EAX+0x10],ECX
    return p;                                                              // EAX = arg at RET
}
RH_ScopedInstall(AudioClear5be930, 0x005be930);

// ===== round 81 =====
// Mashed RE — promote-round round 81 (indexed table setter + ptr-struct field setters).
//
// Surfaced by promote_frontier.py; decompiled via Ghidra. Display-independent; diffed via
// early_window_leaf_diff.py (0046dd90 = existing indexed_table_set; 005bf7d0/005b0ca0 = NEW
// deref_struct_set handler, SWEEP-CRITICAL).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)



// 0x0046dd90  gameplay — byte-verified: 8B 44 24 04 8B 4C 24 08 69 C0 04 0D 00 00 89 88 F4 16 88 00 C3
//   MOV EAX,[ESP+4]=i ; MOV ECX,[ESP+8]=v ; IMUL EAX,EAX,0xd04 ; MOV [EAX+0x8816f4],ECX ; RET
//   -> *(u32*)(0x008816f4 + i*0xd04) = v   (per-vehicle 0xd04-stride field setter; caller 004177b0 C2)
extern "C" __declspec(dllexport) void __cdecl VehField8816f4Set(std::uint32_t i, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(0x008816f4u + i * 0xd04u) = v;
}
RH_ScopedInstall(VehField8816f4Set, 0x0046dd90);

// 0x005bf7d0  audio — byte-verified: 8B 44 24 04 8B 4C 24 08 8B 54 24 0C 89 88 44 01 00 00 89 90 48 01 00 00 C3
//   fn(p,a,b): *(p+0x144)=a ; *(p+0x148)=b ; RET   (audio object 2-callback setter; caller 005be260 C2)
extern "C" __declspec(dllexport) void __cdecl AudioCb5bf7d0Set(void* p, std::uint32_t a, std::uint32_t b) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x144) = a;   // MOV [EAX+0x144],ECX
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x148) = b;   // MOV [EAX+0x148],EDX
}
RH_ScopedInstall(AudioCb5bf7d0Set, 0x005bf7d0);

// 0x005b0ca0  audio — byte-verified: 8B 44 24 04 8B 4C 24 08 89 48 0C 8B 08 83 C9 08 89 08 C3
//   fn(p,v): *(p+0xc)=v ; *p |= 8 ; RET   (cmd-builder: set field + flag bit 0x8; caller 005a86a0 C2)
extern "C" __declspec(dllexport) void __cdecl CmdBuild5b0ca0Set(void* p, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x0c) = v;    // MOV [EAX+0xc],ECX
    *reinterpret_cast<std::uint32_t*>(p) |= 8u;                             // MOV ECX,[EAX];OR ECX,8;MOV [EAX],ECX
}
RH_ScopedInstall(CmdBuild5b0ca0Set, 0x005b0ca0);

// ===== round 82 =====
// Mashed RE — promote-round round 82 (more indexed/ptr-struct setters + 5-field clear).
//
// Ghidra-decompiled STATE leaves; display-independent; diffed via early_window_leaf_diff.py
// (indexed_table_set + deref_struct_set + ptr_fields_clear handlers).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)



// 0x005b0dc0  audio — byte-verified: 8B 44 24 04 8B 4C 24 08 89 48 04 8B 08 80 C9 80 89 08 C3
//   fn(p,v): *(p+4)=v ; *p |= 0x80 ; RET   (cmd-builder: field + flag bit 0x80; caller 005a8960 C2)
extern "C" __declspec(dllexport) void __cdecl CmdBuild5b0dc0Set(void* p, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x04) = v;    // MOV [EAX+4],ECX
    *reinterpret_cast<std::uint32_t*>(p) |= 0x80u;                          // MOV ECX,[EAX];OR CL,0x80;MOV [EAX],ECX
}
RH_ScopedInstall(CmdBuild5b0dc0Set, 0x005b0dc0);

// 0x005bde50  audio — byte-verified: 8B 44 24 04 33 C9 89 08 89 48 04 89 48 08 89 48 0C 89 48 10 C3
//   fn(p): p[0..4 dwords]=0 ; return p   (zero a 5-dword media-buffer descriptor; callers 005bd6f0/005bdad0 C2)
extern "C" __declspec(dllexport) void* __cdecl ClearDesc5bde50(void* p) {
    char* b = static_cast<char*>(p);
    *reinterpret_cast<std::uint32_t*>(b + 0x00) = 0;   // MOV [EAX],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x04) = 0;   // MOV [EAX+4],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x08) = 0;   // MOV [EAX+8],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x0c) = 0;   // MOV [EAX+0xc],ECX
    *reinterpret_cast<std::uint32_t*>(b + 0x10) = 0;   // MOV [EAX+0x10],ECX
    return p;                                          // EAX = arg at RET
}
RH_ScopedInstall(ClearDesc5bde50, 0x005bde50);

// 0x00477e00  render — byte-verified: 8B 44 24 04 8B 4C 24 08 69 C0 C0 02 00 00 89 88 8C 31 69 00 C3
//   fn(i,v): IMUL EAX,EAX,0x2c0 ; *(u32*)(0x0069318c + i*0x2c0) = v ; RET   (caller 0040d8f0 C2)
extern "C" __declspec(dllexport) void __cdecl Table69318cSet(std::uint32_t i, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(0x0069318cu + i * 0x2c0u) = v;
}
RH_ScopedInstall(Table69318cSet, 0x00477e00);

// ===== round 83 =====
// Mashed RE — promote-round round 83 (3-field struct setters + byte-OR RMW; deref_struct_set).
//
// 005173d0/005209d0 auto-surfaced by promote_classify.py's new deref_struct_set recognizer;
// 00518570 hand-authored (or ch,4 byte-OR form). Display-independent; early_window_leaf_diff.py.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E



// 0x005173d0  util — byte-verified: 8B 44 24 04 8B 4C 24 08 8B 54 24 0C 89 48 48 8B 4C 24 10 89 50 40 89 48 44 C3
//   fn(p,a,b,c): *(p+0x48)=a ; *(p+0x40)=b ; *(p+0x44)=c ; RET   (callers 00514d00/005173f0 C2)
extern "C" __declspec(dllexport) void __cdecl Set5173d0(void* p, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x48) = a;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x40) = b;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x44) = c;
}
RH_ScopedInstall(Set5173d0, 0x005173d0);

// 0x005209d0  util — byte-verified: ... 89 88 E8 01 00 00 ... 89 90 EC 01 00 00 89 88 F0 01 00 00 C3
//   fn(p,a,b,c): *(p+0x1e8)=a ; *(p+0x1ec)=b ; *(p+0x1f0)=c ; RET   (callers 00514d00/005173f0 C2)
extern "C" __declspec(dllexport) void __cdecl Set5209d0(void* p, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x1e8) = a;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x1ec) = b;
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x1f0) = c;
}
RH_ScopedInstall(Set5209d0, 0x005209d0);

// 0x00518570  util — byte-verified: 8B 44 24 04 8B 48 70 80 CD 04 89 48 70 C3
//   fn(p): MOV ECX,[EAX+0x70] ; OR CH,4 ; MOV [EAX+0x70],ECX ; RET  -> *(p+0x70) |= 0x400
extern "C" __declspec(dllexport) void __cdecl Rmw518570(void* p) {
    *reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 0x70) |= 0x400u;
}
RH_ScopedInstall(Rmw518570, 0x00518570);

// ===== round 85 =====
// Mashed RE — promote-round round 85 (conditional deref-get + table bool predicates).
//
// Display-independent; diffed via early_window_leaf_diff.py (NEW cond_deref_get +
// table_bool_predicate handlers). early_window works despite the env D3D9 wedge (it
// attaches pre-D3D9-init).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E



// 0x005c4d30  boot — byte-verified: 8B 44 24 04 8B 48 04 85 C9 74 03 8B 00 C3 33 C0 C3
//   fn(p): if (*(u32*)(p+4) != 0) return *(u32*)p ; else return 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl CondGet5c4d30(void* p) {
    if (*reinterpret_cast<std::uint32_t*>(static_cast<char*>(p) + 4) != 0)
        return *reinterpret_cast<std::uint32_t*>(p);
    return 0;
}
RH_ScopedInstall(CondGet5c4d30, 0x005c4d30);

// 0x0045bff0  gameplay — byte-verified: 8B 44 24 04 69 C0 B4 00 00 00 8B 90 88 FC 88 00 33 C9 85 D2 0F 94 C1 8B C1 C3
//   fn(i): return (*(u32*)(0x0088fc88 + i*0xb4) == 0) ? 1 : 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred45bff0(std::uint32_t i) {
    return (*reinterpret_cast<std::uint32_t*>(0x0088fc88u + i * 0xb4u) == 0) ? 1u : 0u;
}
RH_ScopedInstall(Pred45bff0, 0x0045bff0);

// 0x00497450  input — byte-verified: 8B 44 24 04 83 F8 03 7E 03 33 C0 C3 C1 E0 09 8B 90 FC 96 7E 00 33 C9 85 D2 0F 95 C1 8B C1 C3
//   CMP EAX,3 ; JLE predicate (i<=3) ; else fall-through XOR EAX,EAX;RET (i>3 -> 0)
//   fn(i): if ((int)i <= 3) return (*(u32*)(0x007e96fc + i*0x200) != 0) ? 1 : 0 ; return 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred497450(std::uint32_t i) {
    if (static_cast<std::int32_t>(i) <= 3)
        return (*reinterpret_cast<std::uint32_t*>(0x007e96fcu + i * 0x200u) != 0) ? 1u : 0u;
    return 0;
}
RH_ScopedInstall(Pred497450, 0x00497450);

// ===== round 86 =====
// Mashed RE — promote-round round 86 (global swap-return + 3-byte-globals + indexed float-square).
//
// Display-independent; diffed via early_window_leaf_diff.py (NEW global_swap,
// byte_args_to_globals, indexed_float_sq handlers). Works despite the env D3D9 wedge.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E



// 0x005a7b40  audio — byte-verified: 8B 4C 24 04 A1 BC CA 7D 00 89 0D BC CA 7D 00 C3
//   fn(v): old = *(u32*)0x007dcabc ; *(u32*)0x007dcabc = v ; return old  (audio-ctx swap; caller 005a7b60 C2)
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioCtxSwap5a7b40(std::uint32_t v) {
    std::uint32_t old = *reinterpret_cast<std::uint32_t*>(0x007dcabcu);
    *reinterpret_cast<std::uint32_t*>(0x007dcabcu) = v;
    return old;
}
RH_ScopedInstall(AudioCtxSwap5a7b40, 0x005a7b40);

// 0x004924c0  boot — byte-verified: 8A 44 24 04 8A 4C 24 08 8A 54 24 0C A2 B4 47 61 00 88 0D B5 47 61 00 88 15 B6 47 61 00 C3
//   fn(a,b,c): *(u8*)0x006147b4=a ; *(u8*)0x006147b5=b ; *(u8*)0x006147b6=c   (callers 00426810/00426e10 C2)
extern "C" __declspec(dllexport) void __cdecl Set6147b4Triple(unsigned char a, unsigned char b, unsigned char c) {
    *reinterpret_cast<unsigned char*>(0x006147b4u) = a;
    *reinterpret_cast<unsigned char*>(0x006147b5u) = b;
    *reinterpret_cast<unsigned char*>(0x006147b6u) = c;
}
RH_ScopedInstall(Set6147b4Triple, 0x004924c0);

// 0x0047cdc0  render — byte-verified: D9 44 24 08 8B 44 24 04 D8 4C 24 08 D9 1C 85 70 68 6C 00 C3
//   fn(i,f): FLD [esp+8]=f ; FMUL [esp+8]=f ; FSTP [eax*4+0x6c6870] -> *(float*)(0x006c6870 + i*4) = f*f
//   (store-squared-distance setter; callers 00407800/00412050/0044c4f0 C2)
extern "C" __declspec(dllexport) void __cdecl StoreDistSq47cdc0(std::uint32_t i, float f) {
    *reinterpret_cast<float*>(0x006c6870u + i * 4u) = f * f;
}
RH_ScopedInstall(StoreDistSq47cdc0, 0x0047cdc0);

// ===== round 87 =====
// Mashed RE — promote-round round 87 (double-deref vec3 getter + gated float-compare predicate).
//
// Display-independent; early_window_leaf_diff.py (NEW double_deref_vec3_get + global_float_predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E



// 0x0044dff0  gameplay — byte-verified: 8B 44 24 04 69 C0 F8 00 00 00 8B 88 80 00 89 00 8B 51 04 8B 44 24 08 83 C2 40 8B 0A 89 08 8B 4A 04 89 48 04 8B 52 08 89 50 08 C3
//   fn(i,out): rec=*(u32*)(0x00890080 + i*0xf8) ; t=*(u32*)(rec+4) ;
//              out[0]=*(u32*)(t+0x40) ; out[1]=*(u32*)(t+0x44) ; out[2]=*(u32*)(t+0x48)  (item world-pos; caller 00461650 C2)
extern "C" __declspec(dllexport) void __cdecl ItemWorldPos44dff0(std::uint32_t i, std::uint32_t* out) {
    std::uint32_t rec = *reinterpret_cast<std::uint32_t*>(0x00890080u + i * 0xf8u);
    std::uint32_t t = *reinterpret_cast<std::uint32_t*>(rec + 4);
    out[0] = *reinterpret_cast<std::uint32_t*>(t + 0x40);
    out[1] = *reinterpret_cast<std::uint32_t*>(t + 0x44);
    out[2] = *reinterpret_cast<std::uint32_t*>(t + 0x48);
}
RH_ScopedInstall(ItemWorldPos44dff0, 0x0044dff0);

// 0x00405430  gameplay — byte-verified: A1 78 9D 63 00 85 C0 74 15 A1 70 9D 63 00 D9 05 74 9D 63 00 D8 58 0C DF E0 F6 C4 41 75 03 33 C0 C3 B8 01 00 00 00 C3
//   fn(): if (*(int*)0x639d78 == 0) return 0 ; return (*(float*)0x639d74 <= *(float*)(*(u32*)0x639d70 + 0xc)) ? 1 : 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Pred405430(void) {
    if (*reinterpret_cast<std::int32_t*>(0x00639d78u) == 0) return 0;
    float thr = *reinterpret_cast<float*>(0x00639d74u);
    float val = *reinterpret_cast<float*>(*reinterpret_cast<std::uint32_t*>(0x00639d70u) + 0xc);
    return (thr <= val) ? 1u : 0u;
}
RH_ScopedInstall(Pred405430, 0x00405430);

// ===== round 88 =====
// Mashed RE — promote-round round 88 (double-deref ptr getter + float field RMW).
//
// Display-independent; early_window_leaf_diff.py (NEW double_deref_ptr_get + deref_float_field_rmw).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E



// 0x00407620  gameplay — byte-verified: 8B 44 24 08 69 C0 EC 00 00 00 8B 88 90 9D 63 00 8B 51 04 8B 44 24 04 83 C2 10 89 10 C3
//   fn(out,idx): rec=*(u32*)(0x00639d90 + idx*0xec) ; *out = *(u32*)(rec+4) + 0x10   (caller 004556f0 C2)
extern "C" __declspec(dllexport) void __cdecl Deref407620(std::uint32_t* out, std::uint32_t idx) {
    std::uint32_t rec = *reinterpret_cast<std::uint32_t*>(0x00639d90u + idx * 0xecu);
    *out = *reinterpret_cast<std::uint32_t*>(rec + 4) + 0x10;
}
RH_ScopedInstall(Deref407620, 0x00407620);

// 0x004058b0  gameplay — byte-verified: 8B 44 24 04 D9 40 5C D8 64 24 08 D9 58 5C C3
//   fn(p,f): FLD [p+0x5c] ; FSUB [esp+8]=f ; FSTP [p+0x5c]  ->  *(float*)(p+0x5c) -= f   (caller 004058c0 C2)
extern "C" __declspec(dllexport) void __cdecl FloatSub4058b0(void* p, float f) {
    *reinterpret_cast<float*>(static_cast<char*>(p) + 0x5c) -= f;
}
RH_ScopedInstall(FloatSub4058b0, 0x004058b0);


// ===== round 89 =====

// 0x004576b0  particle — byte-verified: 33 C0 8B 0C 85 00 A3 68 00 85 C9 75 25 40 83 F8 02 7C EF 83 F8 04 7D 17 C1 E0 04 05 54 A2 68 00 83 38 00 75 0D 83 C0 10 3D 94 A2 68 00 7C F1 33 C0 C3 B8 01 00 00 00 C3
//   "any active?" over 4 slots (0x68a300,0x68a304 then 0x68a274,0x68a284) -> return 1 if any nonzero
extern "C" __declspec(dllexport) std::uint32_t __cdecl AnyActive4576b0(void) {
    for (std::uint32_t i = 0; i < 2; i++)
        if (*reinterpret_cast<std::int32_t*>(0x0068a300u + i * 4u)) return 1u;
    for (std::uint32_t p = 0x0068a274u; p < 0x0068a294u; p += 0x10u)
        if (*reinterpret_cast<std::int32_t*>(p)) return 1u;
    return 0u;
}
RH_ScopedInstall(AnyActive4576b0, 0x004576b0);

// 0x004075b0  gameplay — byte-verified (70B): IMUL i,0xec; rec=*(0x639d90+i*0xec); t=*(rec+4);
//   out[0..2]=t[0x40/44/48]; out[3]=0x40000000(2.0f); return (*(0x639dc8+i*0xec)==0)?1:0
extern "C" __declspec(dllexport) std::uint32_t __cdecl ItemPosRet4075b0(std::uint32_t i, std::uint32_t* out) {
    std::uint32_t rec = *reinterpret_cast<std::uint32_t*>(0x00639d90u + i * 0xecu);
    std::uint32_t t = *reinterpret_cast<std::uint32_t*>(rec + 4);
    out[0] = *reinterpret_cast<std::uint32_t*>(t + 0x40);
    out[1] = *reinterpret_cast<std::uint32_t*>(t + 0x44);
    out[2] = *reinterpret_cast<std::uint32_t*>(t + 0x48);
    out[3] = 0x40000000u;
    return (*reinterpret_cast<std::uint32_t*>(0x00639dc8u + i * 0xecu) == 0) ? 1u : 0u;
}
RH_ScopedInstall(ItemPosRet4075b0, 0x004075b0);

// ===== round 90 =====

// 0x0045c4e0  gameplay — byte-verified: 8B 54 24 0C 56 33 C0 85 D2 7E 14 ... 39 31 74 0B 40 83 C1 28 3B C2 7C F4 83 C8 FF 5E C3
//   int fn(key, table*, count): for i in [0,count): if(table[i*10]==key) return i ; return -1  (caller 0045cb20/0045cbe0 C2)
extern "C" __declspec(dllexport) int __cdecl ArgSearch45c4e0(int key, int* table, int count) {
    for (int i = 0; i < count; i++)
        if (table[i * 10] == key) return i;
    return -1;
}
RH_ScopedInstall(ArgSearch45c4e0, 0x0045c4e0);

// 0x0045df70  gameplay — byte-verified (71B): steps global float DAT_006036c0 toward `target` by DAT_005cc9a4
//   if (g < target) g += step ; if (target < g) g -= step   (caller 0045dfc0 C2)
extern "C" __declspec(dllexport) void __cdecl FloatStep45df70(float target) {
    float* g = reinterpret_cast<float*>(0x006036c0u);
    float step = *reinterpret_cast<float*>(0x005cc9a4u);
    if (*g < target) *g += step;
    if (target < *g) *g -= step;
}
RH_ScopedInstall(FloatStep45df70, 0x0045df70);

// ===== round 91 (struct-const-init family) =====

// 0x005b0b90  audio — byte-verified: 8B 44 24 04 33 C9 89 08 C7 40 04 01.. 89 48 08 89 48 0C C7 40 10 ..3F 89 48 14 C3
//   fn(p): p[0]=0;p[1]=1;p[2]=0;p[3]=0;p[4]=1.0f;p[5]=0   (6-word descriptor default; caller 005af070 C2)
extern "C" __declspec(dllexport) void __cdecl Init5b0b90(std::uint32_t* p) {
    p[0]=0; p[1]=1; p[2]=0; p[3]=0; p[4]=0x3f800000u; p[5]=0;
}
RH_ScopedInstall(Init5b0b90, 0x005b0b90);

// 0x005b0f10  audio — byte-verified: fn(a,p=[esp+8]): p[0]=p[1]=-1;p[4]=0;p[2]=0;p[3]=1;p[5]=0;p[6]=0; return a
extern "C" __declspec(dllexport) std::uint32_t __cdecl Init5b0f10(std::uint32_t a, std::uint32_t* p) {
    p[0]=0xffffffffu; p[1]=0xffffffffu; p[4]=0; p[2]=0; p[3]=1; p[5]=0; p[6]=0;
    return a;
}
RH_ScopedInstall(Init5b0f10, 0x005b0f10);

// 0x005beb50  audio — byte-verified (69B): zero a CBasePin/CBaseFilter bookkeeping block + set vtable-ish ptr
//   p[0x10]=0; *(u16*)(p+0x18)=0; p[0x118/11c/120/124/128]=0; p[0x14]=&DAT_00635948; p[0x148/14c/150]=0
extern "C" __declspec(dllexport) void __cdecl Init5beb50(char* p) {
    *reinterpret_cast<std::uint32_t*>(p+0x10)=0;
    *reinterpret_cast<std::uint16_t*>(p+0x18)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x118)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x120)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x11c)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x124)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x128)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x14)=0x00635948u;
    *reinterpret_cast<std::uint32_t*>(p+0x148)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x14c)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x150)=0;
}
RH_ScopedInstall(Init5beb50, 0x005beb50);

// 0x005c9120  audio — byte-verified: p[0x1e8]=0;p[0x20c]=0;p[0x210]=0;p[0x224]=p[0x1ec]*2
extern "C" __declspec(dllexport) void __cdecl Init5c9120(char* p) {
    *reinterpret_cast<std::uint32_t*>(p+0x1e8)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x20c)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x210)=0;
    *reinterpret_cast<std::uint32_t*>(p+0x224)=*reinterpret_cast<std::uint32_t*>(p+0x1ec)*2u;
}
RH_ScopedInstall(Init5c9120, 0x005c9120);

// ===== round 92 =====

// 0x005b0c70  audio — byte-verified: fn(p,v): p[2]=v; p[3]=0; *p=(*p&0xfffffff7)|4; p[4]=1.0f; p[1]=1; p[5]=0
extern "C" __declspec(dllexport) void __cdecl Init5b0c70(std::uint32_t* p, std::uint32_t v) {
    p[2]=v; p[3]=0; p[0]=(p[0]&0xfffffff7u)|4u; p[4]=0x3f800000u; p[1]=1; p[5]=0;
}
RH_ScopedInstall(Init5b0c70, 0x005b0c70);

// ===== round 93 =====

// 0x0041ee50  gameplay — byte-verified: flag set/clear on *(0x63dc74 + i*0x2ac)
//   if(p2==0) clear 0x1000 else set 0x1000 ; if(p3) set 0x2000 else clear 0x2000
extern "C" __declspec(dllexport) void __cdecl Flag41ee50(std::uint32_t i, std::uint32_t p2, std::uint32_t p3) {
    std::uint32_t* f = reinterpret_cast<std::uint32_t*>(0x0063dc74u + i * 0x2acu);
    if (p2 == 0) *f &= 0xffffefffu; else *f |= 0x1000u;
    if (p3) *f |= 0x2000u; else *f &= 0xffffdfffu;
}
RH_ScopedInstall(Flag41ee50, 0x0041ee50);

// 0x00471530  particle — byte-verified: strided clear [0x690ab0,0x691500) step 0x84 (byte@-0x80 + dword@0) + [0x691484]=0
extern "C" __declspec(dllexport) void __cdecl ClearTable471530(void) {
    for (std::uint32_t p = 0x00690ab0u; p < 0x00691500u; p += 0x84u) {
        *reinterpret_cast<std::uint8_t*>(p - 0x80) = 0;
        *reinterpret_cast<std::uint32_t*>(p) = 0;
    }
    *reinterpret_cast<std::uint32_t*>(0x00691484u) = 0;
}
RH_ScopedInstall(ClearTable471530, 0x00471530);

// 0x0046d2e0  vehicle — byte-verified: u32 fn(i,wheel,out): if(i<0x10 && wheel<4){ *out=*(u32*)(0x881790+(i*0x11+wheel)*0xc4); return 1 } return 0
extern "C" __declspec(dllexport) std::uint32_t __cdecl WheelGet46d2e0(std::uint32_t i, std::uint32_t wheel, std::uint32_t* out) {
    if (i < 0x10u && wheel < 4u) {
        *out = *reinterpret_cast<std::uint32_t*>(0x00881790u + (i * 0x11u + wheel) * 0xc4u);
        return 1u;
    }
    return 0u;
}
RH_ScopedInstall(WheelGet46d2e0, 0x0046d2e0);

// ===== round 94 =====

// 0x00476a80  render — byte-verified: copy 8 dwords from p (null->&DAT_006132a4) to globals 0x692534..0x692550
extern "C" __declspec(dllexport) void __cdecl Copy476a80(std::uint32_t* p) {
    if (!p) p = reinterpret_cast<std::uint32_t*>(0x006132a4u);
    *reinterpret_cast<std::uint32_t*>(0x00692534u)=p[0];
    *reinterpret_cast<std::uint32_t*>(0x00692538u)=p[1];
    *reinterpret_cast<std::uint32_t*>(0x0069253cu)=p[2];
    *reinterpret_cast<std::uint32_t*>(0x00692540u)=p[3];
    *reinterpret_cast<std::uint32_t*>(0x00692544u)=p[4];
    *reinterpret_cast<std::uint32_t*>(0x00692548u)=p[5];
    *reinterpret_cast<std::uint32_t*>(0x0069254cu)=p[6];
    *reinterpret_cast<std::uint32_t*>(0x00692550u)=p[7];
}
RH_ScopedInstall(Copy476a80, 0x00476a80);

// 0x00474d80  render — byte-verified: void fn(p,set): if(p){ b=*(u8*)(p+2); set?b|=4:b&=~4; *(u8*)(p+2)=b; }
extern "C" __declspec(dllexport) void __cdecl ByteFlag474d80(char* p, std::uint32_t set) {
    if (p) {
        if (set) *reinterpret_cast<std::uint8_t*>(p+2) |= 4u;
        else     *reinterpret_cast<std::uint8_t*>(p+2) &= 0xfbu;
    }
}
RH_ScopedInstall(ByteFlag474d80, 0x00474d80);

// 0x0041f0d0  gameplay — byte-verified: void fn(i,out): if(out) *out = *(u32*)(0x63dc74 + i*0x2ac) & 0x400
extern "C" __declspec(dllexport) void __cdecl MaskGet41f0d0(std::uint32_t i, std::uint32_t* out) {
    if (out) *out = *reinterpret_cast<std::uint32_t*>(0x0063dc74u + i * 0x2acu) & 0x400u;
}
RH_ScopedInstall(MaskGet41f0d0, 0x0041f0d0);

// ===== round 95 =====

// 0x0047c0b0  render — byte-verified: zero 4 contiguous global tables 0x6bf468..0x6c2fec + DAT_006c2fec=0
extern "C" __declspec(dllexport) void __cdecl ClearColl47c0b0(void) {
    for (std::uint32_t* p=(std::uint32_t*)0x006bfca8u; p<(std::uint32_t*)(0x006bfca8u+0x560u*4u); ++p) *p=0;
    for (std::uint32_t* p=(std::uint32_t*)0x006bf468u; p<(std::uint32_t*)(0x006bf468u+0x200u*4u); ++p) *p=0;
    for (std::uint32_t* p=(std::uint32_t*)0x006bfc68u; p<(std::uint32_t*)(0x006bfc68u+0x10u*4u); ++p) *p=0;
    for (std::uint32_t* p=(std::uint32_t*)0x006c1228u; p<(std::uint32_t*)(0x006c1228u+0x771u*4u); ++p) *p=0;
    *(std::uint32_t*)0x006c2fecu=0;
}
RH_ScopedInstall(ClearColl47c0b0, 0x0047c0b0);

// 0x00476ae0  render — byte-verified: base=*(*(p1+4)+DAT_007dc57c); base[0xb8]=p2[0]; base[0xbc]=p2[1]; base[0x40]|=0x4000
extern "C" __declspec(dllexport) void __cdecl Batch476ae0(std::uint32_t* p1, std::uint32_t* p2) {
    if (!p2) p2=(std::uint32_t*)0x00613288u;
    char* b=(char*)*(std::uint32_t*)(*(std::uint32_t*)((char*)p1+4) + *(std::uint32_t*)0x007dc57cu);
    *(std::uint32_t*)(b+0xb8)=p2[0]; *(std::uint32_t*)(b+0xbc)=p2[1]; *(std::uint32_t*)(b+0x40)|=0x4000u;
}
RH_ScopedInstall(Batch476ae0, 0x00476ae0);

// 0x00476b90  render — byte-verified: base=...; copy 16 dwords p2->base+0xf8; base[0x40]|=0x8000
extern "C" __declspec(dllexport) void __cdecl Batch476b90(std::uint32_t* p1, std::uint32_t* p2) {
    if (!p2) p2=(std::uint32_t*)0x00692558u;
    char* base=(char*)*(std::uint32_t*)(*(std::uint32_t*)((char*)p1+4) + *(std::uint32_t*)0x007dc57cu);
    std::uint32_t* d=(std::uint32_t*)(base+0xf8);
    for (int k=0;k<16;k++) d[k]=p2[k];
    *(std::uint32_t*)(base+0x40)|=0x8000u;
}
RH_ScopedInstall(Batch476b90, 0x00476b90);

// 0x00476be0  render — byte-verified: base=...; base[0xc0]=v; base[0x40]|=0x20000
extern "C" __declspec(dllexport) void __cdecl Batch476be0(std::uint32_t* p1, std::uint32_t v) {
    char* b=(char*)*(std::uint32_t*)(*(std::uint32_t*)((char*)p1+4) + *(std::uint32_t*)0x007dc57cu);
    *(std::uint32_t*)(b+0xc0)=v; *(std::uint32_t*)(b+0x40)|=0x20000u;
}
RH_ScopedInstall(Batch476be0, 0x00476be0);
