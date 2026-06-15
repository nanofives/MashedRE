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

// ===== round 96 (more per-batch state setters) =====

// 0x00476c10  particle — base=*(*(p1+4)+DAT_007dc57c); base[0xd8/dc/e0/e4]=p2[0..3]; base[0x40]|=0x80000
extern "C" __declspec(dllexport) void __cdecl Batch476c10(std::uint32_t* p1, std::uint32_t* p2) {
    if (!p2) p2=(std::uint32_t*)0x00613294u;
    char* b=(char*)*(std::uint32_t*)(*(std::uint32_t*)((char*)p1+4)+*(std::uint32_t*)0x007dc57cu);
    *(std::uint32_t*)(b+0xd8)=p2[0]; *(std::uint32_t*)(b+0xdc)=p2[1];
    *(std::uint32_t*)(b+0xe0)=p2[2]; *(std::uint32_t*)(b+0xe4)=p2[3];
    *(std::uint32_t*)(b+0x40)|=0x80000u;
}
RH_ScopedInstall(Batch476c10, 0x00476c10);

// 0x00476c60  particle — base=...; copy 8 dwords p2->base+0xd8; base[0x40]|=0x100000
extern "C" __declspec(dllexport) void __cdecl Batch476c60(std::uint32_t* p1, std::uint32_t* p2) {
    if (!p2) p2=(std::uint32_t*)0x006132a4u;
    char* base=(char*)*(std::uint32_t*)(*(std::uint32_t*)((char*)p1+4)+*(std::uint32_t*)0x007dc57cu);
    std::uint32_t* d=(std::uint32_t*)(base+0xd8);
    for (int k=0;k<8;k++) d[k]=p2[k];
    *(std::uint32_t*)(base+0x40)|=0x100000u;
}
RH_ScopedInstall(Batch476c60, 0x00476c60);

// 0x00476cb0  particle — base=...; base[0xa4]=p2; base[0xa8]=p3; base[0x40]|=0x10000000
extern "C" __declspec(dllexport) void __cdecl Batch476cb0(std::uint32_t* p1, std::uint32_t p2, std::uint32_t p3) {
    char* b=(char*)*(std::uint32_t*)(*(std::uint32_t*)((char*)p1+4)+*(std::uint32_t*)0x007dc57cu);
    *(std::uint32_t*)(b+0xa4)=p2; *(std::uint32_t*)(b+0xa8)=p3; *(std::uint32_t*)(b+0x40)|=0x10000000u;
}
RH_ScopedInstall(Batch476cb0, 0x00476cb0);

// ===== round 97 =====

// 0x0047ce40  particle — int fn(key): if(key<=0) return -1; for i<200: if(*(int*)(0x6c6b90+i*4)==key) return i; return -1
extern "C" __declspec(dllexport) int __cdecl Search47ce40(int key) {
    if (key <= 0) return -1;
    for (int i = 0; i < 200; i++)
        if (reinterpret_cast<int*>(0x006c6b90u)[i] == key) return i;
    return -1;
}
RH_ScopedInstall(Search47ce40, 0x0047ce40);

// 0x0048f260  particle — strided clear: for p in [0x76d980,0x7706d0) step 0x488: *(u32*)p=0  (one dword/record)
extern "C" __declspec(dllexport) void __cdecl Clear48f260(void) {
    for (std::uint32_t p = 0x0076d980u; p < 0x007706d0u; p += 0x488u)
        *reinterpret_cast<std::uint32_t*>(p) = 0;
}
RH_ScopedInstall(Clear48f260, 0x0048f260);

// 0x00491bd0  particle — zero vertex buffer *(0x869ca0): for i in [0,0x4000) step 0x20: *(u32*)(*(0x869ca0)+i)=0
extern "C" __declspec(dllexport) void __cdecl ClearVB491bd0(void) {
    std::uint32_t base = *reinterpret_cast<std::uint32_t*>(0x00869ca0u);
    for (std::uint32_t i = 0; i < 0x4000u; i += 0x20u)
        *reinterpret_cast<std::uint32_t*>(base + i) = 0;
}
RH_ScopedInstall(ClearVB491bd0, 0x00491bd0);

// ===== round 98 =====

// 0x00497000  render — byte-verified: build 16-dword projection matrix at out from camera struct p2
//   out[0]=p2[0x70]; out[5]=p2[0x74]; out[10]=fv=p2[0x84]/(p2[0x84]-p2[0x80]); out[0xb]=-(fv*p2[0x80]);
//   out[0xe]=1.0f; rest 0
extern "C" __declspec(dllexport) void __cdecl Proj497000(std::uint32_t* out, char* p2) {
    out[0]=*reinterpret_cast<std::uint32_t*>(p2+0x70);
    out[1]=0; out[2]=0; out[3]=0; out[4]=0;
    out[5]=*reinterpret_cast<std::uint32_t*>(p2+0x74);
    out[6]=0; out[7]=0; out[8]=0; out[9]=0;
    float fv = *reinterpret_cast<float*>(p2+0x84) / (*reinterpret_cast<float*>(p2+0x84) - *reinterpret_cast<float*>(p2+0x80));
    *reinterpret_cast<float*>(out+10) = fv;
    float f1 = *reinterpret_cast<float*>(p2+0x80);
    out[0xc]=0; out[0xd]=0;
    *reinterpret_cast<std::uint32_t*>(out+0xe)=0x3f800000u; out[0xf]=0;
    *reinterpret_cast<float*>(out+0xb) = -(fv*f1);
}
RH_ScopedInstall(Proj497000, 0x00497000);

// ===== round 99 =====

// 0x0051ca60  util — byte-verified: void fn(p,v): p[0]=v>>24; p[1]=v>>16; p[2]=v>>8; p[3]=v  (store big-endian u32)
extern "C" __declspec(dllexport) void __cdecl StoreBE51ca60(std::uint8_t* p, std::uint32_t v) {
    p[0]=(std::uint8_t)(v>>24); p[1]=(std::uint8_t)(v>>16); p[2]=(std::uint8_t)(v>>8); p[3]=(std::uint8_t)v;
}
RH_ScopedInstall(StoreBE51ca60, 0x0051ca60);

// 0x00523110  util — byte-verified: int fn(p): return ((p[0]*0x100+p[1])*0x100+p[2])*0x100+p[3]  (load big-endian u32)
extern "C" __declspec(dllexport) int __cdecl LoadBE523110(std::uint8_t* p) {
    return (int)(((((std::uint32_t)p[0]*0x100u + p[1])*0x100u + p[2])*0x100u) + p[3]);
}
RH_ScopedInstall(LoadBE523110, 0x00523110);

// ===== round 100 =====

// 0x005a6190  audio — byte-verified: uint fn(v): if(v<0x10){ DAT_00914700=0x10; return 0x10; } DAT_00914700=v; return v
extern "C" __declspec(dllexport) unsigned int __cdecl Clamp5a6190(unsigned int v) {
    if (v < 0x10u) { *reinterpret_cast<std::uint32_t*>(0x00914700u) = 0x10u; return 0x10u; }
    *reinterpret_cast<std::uint32_t*>(0x00914700u) = v; return v;
}
RH_ScopedInstall(Clamp5a6190, 0x005a6190);

// ===== round 101 =====

// 0x004b6700  render — byte-verified: MOV dword[0x007d3e54],0; RET  (single const store)
extern "C" __declspec(dllexport) void __cdecl Zero4b6700(void) {
    *reinterpret_cast<std::uint32_t*>(0x007d3e54u) = 0u;
}
RH_ScopedInstall(Zero4b6700, 0x004b6700);

// 0x004a2bf7  boot — byte-verified: store 6 code-pointers into globals 0x006160d8..0x006160ec
//   [d8]=0x4a5d92(EAX); [dc]=0x4a5a0c; [e0]=0x4a5a71; [e4]=0x4a59d0; [e8]=0x4a5a57; [ec]=0x4a5d92(EAX)
extern "C" __declspec(dllexport) void __cdecl Tbl4a2bf7(void) {
    *reinterpret_cast<std::uint32_t*>(0x006160d8u) = 0x004a5d92u;
    *reinterpret_cast<std::uint32_t*>(0x006160dcu) = 0x004a5a0cu;
    *reinterpret_cast<std::uint32_t*>(0x006160e0u) = 0x004a5a71u;
    *reinterpret_cast<std::uint32_t*>(0x006160e4u) = 0x004a59d0u;
    *reinterpret_cast<std::uint32_t*>(0x006160e8u) = 0x004a5a57u;
    *reinterpret_cast<std::uint32_t*>(0x006160ecu) = 0x004a5d92u;
}
RH_ScopedInstall(Tbl4a2bf7, 0x004a2bf7);

// 0x0045c810  util — byte-verified: INC dword[0x0068d54c]; RET  (increment global counter)
extern "C" __declspec(dllexport) void __cdecl Inc45c810(void) {
    ++*reinterpret_cast<std::uint32_t*>(0x0068d54cu);
}
RH_ScopedInstall(Inc45c810, 0x0045c810);

// ===== round 102 =====
// indexed_global_field_read family: return *(*(base_glob) + *(idx_glob) + off)

// 0x004c5850  render — byte-verified: EAX=*[0x7d3ff8]; ECX=*[0x7d4054]; return *(ECX+EAX+0x20)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get4c5850(void) {
    std::uint32_t idx  = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    std::uint32_t base = *reinterpret_cast<std::uint32_t*>(0x007d4054u);
    return *reinterpret_cast<std::uint32_t*>(base + idx + 0x20u);
}
RH_ScopedInstall(Get4c5850, 0x004c5850);

// 0x004c5ca0  render — byte-verified: EAX=*[0x7d3ff8]; ECX=*[0x7d4054]; return *(ECX+EAX+0x10)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get4c5ca0(void) {
    std::uint32_t idx  = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    std::uint32_t base = *reinterpret_cast<std::uint32_t*>(0x007d4054u);
    return *reinterpret_cast<std::uint32_t*>(base + idx + 0x10u);
}
RH_ScopedInstall(Get4c5ca0, 0x004c5ca0);

// indexed_global_field_write family: *(*(base_glob)+*(idx_glob)+off)=v

// 0x004c5830  render — byte-verified: ECX=*[0x7d3ff8]; EDX=*[0x7d4054]; EAX=[ESP+4];
//   *(EDX+ECX+0x20)=EAX; return 1
extern "C" __declspec(dllexport) int __cdecl Set4c5830(std::uint32_t v) {
    std::uint32_t idx  = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    std::uint32_t base = *reinterpret_cast<std::uint32_t*>(0x007d4054u);
    *reinterpret_cast<std::uint32_t*>(base + idx + 0x20u) = v;
    return 1;
}
RH_ScopedInstall(Set4c5830, 0x004c5830);

// 0x004c5c80  render — byte-verified: ECX=*[0x7d3ff8]; EDX=*[0x7d4054]; EAX=[ESP+4];
//   *(EDX+ECX+0x10)=EAX; RET  (EAX left = v)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Set4c5c80(std::uint32_t v) {
    std::uint32_t idx  = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    std::uint32_t base = *reinterpret_cast<std::uint32_t*>(0x007d4054u);
    *reinterpret_cast<std::uint32_t*>(base + idx + 0x10u) = v;
    return v;
}
RH_ScopedInstall(Set4c5c80, 0x004c5c80);

// ===== round 103 =====
// thiscall_struct_from_table family: __thiscall(this) reads global table indexed by
// this[idx_off], derives into this fields. Declared __fastcall (this in ECX) so the
// ABI matches the original for both early_window (Frida thiscall) and auto-install.
// Export resolves under the decorated name @Name@4 (set in hooks_registry).

// 0x0041b770  render thiscall — byte-verified: i=this[0x164]; rec=&tbl_0x5f334c[i*3];
//   this[0x150]=rec[0]; this[0x154]=rec[1]; this[0x158]=rec[2]
extern "C" __declspec(dllexport) void __fastcall Copy41b770(char* self) {
    std::uint32_t i = *reinterpret_cast<std::uint32_t*>(self + 0x164);
    const std::uint32_t* rec = reinterpret_cast<const std::uint32_t*>(0x005f334cu + i * 12u);
    std::uint32_t* dst = reinterpret_cast<std::uint32_t*>(self + 0x150);
    dst[0] = rec[0]; dst[1] = rec[1]; dst[2] = rec[2];
}
RH_ScopedInstall(Copy41b770, 0x0041b770);

// 0x0041ae20  render thiscall — byte-verified: i=this[0x64]; rec=&tbl_0x5f3304[i*3];
//   this[0x50]=rec[0]; this[0x54]=rec[1]; this[0x58]=rec[2]; this[0x70]=0;
//   this[0x50] = (float)this[0x50] + (float)this[0x50]  (FLD;FADD ST0,ST0;FSTP — exact 2x)
extern "C" __declspec(dllexport) void __fastcall Copy41ae20(char* self) {
    std::uint32_t i = *reinterpret_cast<std::uint32_t*>(self + 0x64);
    const std::uint32_t* rec = reinterpret_cast<const std::uint32_t*>(0x005f3304u + i * 12u);
    std::uint32_t* dst = reinterpret_cast<std::uint32_t*>(self + 0x50);
    dst[0] = rec[0]; dst[1] = rec[1]; dst[2] = rec[2];
    *reinterpret_cast<std::uint32_t*>(self + 0x70) = 0;
    float* f = reinterpret_cast<float*>(self + 0x50);
    *f = *f + *f;
}
RH_ScopedInstall(Copy41ae20, 0x0041ae20);

// ===== round 104 =====

// 0x00413fe0  ai — byte-verified: state reset. [0x89a36c]=0; then for
//   eax=0x89a4f0; eax<0x89a6c0; eax+=0x74, edx(start 0x8032d4)+=0x14:
//   zero 12 fields at eax{-0x2c,-0x28,-0x24,-0x20,-4,0,+4,+0x18,+0x2c,+0x30,+0x34,+0x38};
//   [edx]=0x3e8 (1000). 4 records.
extern "C" __declspec(dllexport) void __cdecl Reset413fe0(void) {
    *reinterpret_cast<std::uint32_t*>(0x0089a36cu) = 0;
    std::uint32_t edx = 0x008032d4u;
    for (std::uint32_t eax = 0x0089a4f0u; eax < 0x0089a6c0u; eax += 0x74u, edx += 0x14u) {
        *reinterpret_cast<std::uint32_t*>(eax - 0x2cu) = 0;
        *reinterpret_cast<std::uint32_t*>(eax - 0x28u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax - 0x24u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax - 0x20u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax - 0x04u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax + 0x00u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax + 0x04u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax + 0x18u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax + 0x2cu) = 0;
        *reinterpret_cast<std::uint32_t*>(eax + 0x30u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax + 0x34u) = 0;
        *reinterpret_cast<std::uint32_t*>(eax + 0x38u) = 0;
        *reinterpret_cast<std::uint32_t*>(edx) = 0x3e8u;
    }
}
RH_ScopedInstall(Reset413fe0, 0x00413fe0);

// ===== round 106 =====
// 0x00484a50  world-objects — byte-verified register-conv (EAX=container, ECX=item)
//   cross-link insert. Verbatim naked __asm port (bit-identical by construction).
//   Guards on [eax+0x1b4] & [eax+0x190]<0x64; stores ecx into eax[count]; tracks
//   [ecx+0x10] into [eax+0x1ac]/[eax+0x1b0]; registers eax into ecx[0x24+cnt2]; ret 1/0.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Insert484a50(void) {
    __asm {
        mov edx, dword ptr [eax + 0x1b4]
        test edx, edx
        jne L_ret0
        mov edx, dword ptr [eax + 0x190]
        cmp edx, 0x64
        jl L_ins
    L_ret0:
        xor eax, eax
        ret
    L_ins:
        mov dword ptr [eax + edx*4], ecx
        inc dword ptr [eax + 0x190]
        mov edx, dword ptr [eax + 0x1b0]
        test edx, edx
        jne L_b
        mov edx, dword ptr [eax + 0x1ac]
        test edx, edx
        jne L_c
        mov edx, dword ptr [ecx + 0x10]
        mov dword ptr [eax + 0x1ac], edx
    L_c:
        mov edx, dword ptr [eax + 0x1ac]
        cmp edx, dword ptr [ecx + 0x10]
        je L_b
        mov dword ptr [eax + 0x1b0], 1
        mov edx, dword ptr [ecx + 0x10]
        mov dword ptr [eax + 0x1ac], edx
    L_b:
        mov edx, dword ptr [ecx + 0x64]
        mov dword ptr [ecx + edx*4 + 0x24], eax
        inc dword ptr [ecx + 0x64]
        mov eax, 1
        ret
    }
}
RH_ScopedInstall(Insert484a50, 0x00484a50);

// ===== round 107 =====
// ECX-input (thiscall) const-field setters — verbatim naked-asm ports.

// 0x004944b0  util — byte-verified: mov eax,ecx; mov dword[eax],0; ret  (*this = 0; return this)
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Zero4944b0(void) {
    __asm {
        mov eax, ecx
        mov dword ptr [eax], 0
        ret
    }
}
RH_ScopedInstall(Zero4944b0, 0x004944b0);

// 0x0049c800  particle — byte-verified: mov dword[ecx+0x68],0; ret  (this->field68 = 0)
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Zero49c800(void) {
    __asm {
        mov dword ptr [ecx + 0x68], 0
        ret
    }
}
RH_ScopedInstall(Zero49c800, 0x0049c800);

// ===== round 108 =====
// 0x00418a30  vehicle — byte-verified EAX-input entity init from per-type table.
//   idx=[eax+0x50]; [eax]=tbl_0x5f32b0[idx*8]; [eax+4]=tbl_0x5f32b4[idx*8];
//   [eax+0x64]=-1; zero [eax+{0x34,0x38,0x3c,0x40,0x44,0x4c}]. Verbatim naked-asm.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Init418a30(void) {
    __asm {
        mov ecx, dword ptr [eax + 0x50]
        mov edx, dword ptr [ecx*8 + 0x5f32b0]
        mov dword ptr [eax], edx
        mov ecx, dword ptr [ecx*8 + 0x5f32b4]
        mov dword ptr [eax + 4], ecx
        xor ecx, ecx
        mov dword ptr [eax + 0x34], ecx
        mov dword ptr [eax + 0x64], 0xffffffff
        mov dword ptr [eax + 0x38], ecx
        mov dword ptr [eax + 0x3c], ecx
        mov dword ptr [eax + 0x40], ecx
        mov dword ptr [eax + 0x44], ecx
        mov dword ptr [eax + 0x4c], ecx
        ret
    }
}
RH_ScopedInstall(Init418a30, 0x00418a30);

// ===== round 109 =====
// EAX-input bitmask-builder twins: count=[eax+0xc]; field=0x800000<<(count-1); then
// for i<count: field |= 1<<( tbl_0x7f1a1c[[eax+i*4]<<4] + i*6 ). Verbatim naked-asm.

// 0x0041b720  render — field at [eax+0x168]
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Bits41b720(void) {
    __asm {
        push edi
        mov edi, dword ptr [eax + 0xc]
        lea ecx, [edi - 1]
        mov edx, 0x800000
        shl edx, cl
        mov dword ptr [eax + 0x168], edx
        xor edx, edx
        test edi, edi
        jle L7b_done
        push ebx
        push esi
        xor esi, esi
        mov edi, edi
    L7b_loop:
        mov ecx, dword ptr [eax + edx*4]
        shl ecx, 4
        mov ecx, dword ptr [ecx + 0x7f1a1c]
        add ecx, esi
        mov ebx, 1
        shl ebx, cl
        mov ecx, dword ptr [eax + 0x168]
        add esi, 6
        or ecx, ebx
        inc edx
        cmp edx, edi
        mov dword ptr [eax + 0x168], ecx
        jl L7b_loop
        pop esi
        pop ebx
    L7b_done:
        pop edi
        ret
    }
}
RH_ScopedInstall(Bits41b720, 0x0041b720);

// 0x0041cdb0  render — field at [eax+0x15c]
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Bits41cdb0(void) {
    __asm {
        push edi
        mov edi, dword ptr [eax + 0xc]
        lea ecx, [edi - 1]
        mov edx, 0x800000
        shl edx, cl
        mov dword ptr [eax + 0x15c], edx
        xor edx, edx
        test edi, edi
        jle Lcd_done
        push ebx
        push esi
        xor esi, esi
        mov edi, edi
    Lcd_loop:
        mov ecx, dword ptr [eax + edx*4]
        shl ecx, 4
        mov ecx, dword ptr [ecx + 0x7f1a1c]
        add ecx, esi
        mov ebx, 1
        shl ebx, cl
        mov ecx, dword ptr [eax + 0x15c]
        add esi, 6
        or ecx, ebx
        inc edx
        cmp edx, edi
        mov dword ptr [eax + 0x15c], ecx
        jl Lcd_loop
        pop esi
        pop ebx
    Lcd_done:
        pop edi
        ret
    }
}
RH_ScopedInstall(Bits41cdb0, 0x0041cdb0);

// ===== round 110 =====
// 0x004840f0  vehicle — byte-verified reg-conv (EAX=ptr, EDX=idx): registers the ptr
//   in tblc40[idx]=0x6cec40, counts leading non-zero dwords of *ptr into
//   tbl840[idx]=0x6ce840. Verbatim naked-asm (incl. push/pop ebx balance).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Count4840f0(void) {
    __asm {
        lea ecx, [edx*4 + 0x6ce840]
        mov dword ptr [edx*4 + 0x6cec40], eax
        mov dword ptr [ecx], 0
        mov dl, byte ptr [eax + 3]
        push ebx
        or dl, byte ptr [eax + 2]
        or dl, byte ptr [eax + 1]
        or dl, byte ptr [eax]
        je L40_done
    L40_loop:
        mov ebx, dword ptr [ecx]
        inc ebx
        add eax, 4
        mov dword ptr [ecx], ebx
        mov bl, byte ptr [eax + 2]
        mov dl, byte ptr [eax + 3]
        or dl, bl
        or dl, byte ptr [eax + 1]
        or dl, byte ptr [eax]
        jne L40_loop
    L40_done:
        pop ebx
        ret
    }
}
RH_ScopedInstall(Count4840f0, 0x004840f0);

// ===== round 111 =====
// cdecl buffer ops (state-independent: memset / memcpy from abs static data).

// 0x00478cb0  render — byte-verified: void fn(p): rep stosd 0xab8 dwords of 0 at p
extern "C" __declspec(dllexport) void __cdecl Memset478cb0(std::uint32_t* p) {
    for (unsigned i = 0; i < 0xab8u; i++) p[i] = 0;
}
RH_ScopedInstall(Memset478cb0, 0x00478cb0);

// 0x004b65c0  render — byte-verified: void fn(dst): if(dst) rep movsd 0x82b dwords
//   from 0x8ab7e0 to dst
extern "C" __declspec(dllexport) void __cdecl Copy4b65c0(std::uint32_t* dst) {
    if (dst) {
        const std::uint32_t* s = reinterpret_cast<const std::uint32_t*>(0x008ab7e0u);
        for (unsigned i = 0; i < 0x82bu; i++) dst[i] = s[i];
    }
}
RH_ScopedInstall(Copy4b65c0, 0x004b65c0);

// ===== round 112 =====
// 0x00449880  particle — byte-verified: void fn(): f=(float)*0x5f96d0 + (float)*0x5cc320
//   (FLD;FADD;FSTP->float32); fill 0x683ec8[0..0xff] (0x100 dwords) with bits(f).
extern "C" __declspec(dllexport) void __cdecl Fill449880(void) {
    float f = *reinterpret_cast<float*>(0x005f96d0u) + *reinterpret_cast<float*>(0x005cc320u);
    std::uint32_t fb = *reinterpret_cast<std::uint32_t*>(&f);
    std::uint32_t* t = reinterpret_cast<std::uint32_t*>(0x00683ec8u);
    for (unsigned i = 0; i < 0x100u; i++) t[i] = fb;
}
RH_ScopedInstall(Fill449880, 0x00449880);

// ===== round 113b (per-side-convention fix of the reverted 113) =====
// 0x0042ac50  frontend — byte-verified reg-conv scalar compute (orig: EAX=a idx,
//   ECX=c val -> layout coord). Reimpl is plain __cdecl(a,c): verified via per-side
//   convention (orig reg-trampoline vs reimpl cdecl). NOT RH_ScopedInstall'd — the
//   install ABI (EAX/ECX) differs from cdecl; a standalone port would read its own
//   args. C3 evidence = formula match under force-call.
//   even(a): 0xf0 - ((c - (c>>31)) >>a 1) - (((a-1)>>1)*c);  odd(a): 0xf0 - ((a-1)>>1)*c
extern "C" __declspec(dllexport) std::uint32_t __cdecl Calc42ac50(std::uint32_t a, std::uint32_t c) {
    std::uint32_t term = ((a - 1u) >> 1) * c;
    if (a & 1u) return 0xf0u - term;
    int ic = static_cast<int>(c);
    int d = (ic - (ic >> 31)) >> 1;
    return static_cast<std::uint32_t>(0xf0 - d) - term;
}

// ===== round 114 =====
// 0x0041e150  gameplay — byte-verified reg+stack conv (EAX=struct ptr, [esp+4]=out):
//   *out = ((p[0]*0x3c + p[1]) * 0x64) + p[2]  (time pack: min*60+sec, *100 + cs).
//   Verbatim naked-asm; installable (ABI matches original EAX+stack).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Time41e150(void) {
    __asm {
        mov ecx, dword ptr [eax]
        mov edx, dword ptr [eax + 4]
        imul ecx, ecx, 0x3c
        add ecx, edx
        mov edx, dword ptr [eax + 8]
        imul ecx, ecx, 0x64
        add ecx, edx
        mov edx, dword ptr [esp + 4]
        mov dword ptr [edx], ecx
        ret
    }
}
RH_ScopedInstall(Time41e150, 0x0041e150);

// ===== round 115 =====
// cdecl(ptr,...) struct-field RMW (pure arg-deref; deref_struct_set verified).

// 0x004c45f0 — void fn(p): *(u32*)(p+0xc) &= 0xfffdfffc
extern "C" __declspec(dllexport) void __cdecl And4c45f0(std::uint8_t* p) {
    *reinterpret_cast<std::uint32_t*>(p + 0xc) &= 0xfffdfffcu;
}
RH_ScopedInstall(And4c45f0, 0x004c45f0);

// 0x004b52c0 — void fn(p,val,flag): e=*(u32*)(p+8); if(flag) e|=val; *(u32*)(p+8)=e
extern "C" __declspec(dllexport) void __cdecl Or4b52c0(std::uint8_t* p, std::uint32_t val, std::uint32_t flag) {
    std::uint32_t e = *reinterpret_cast<std::uint32_t*>(p + 8);
    if (flag) e |= val;
    *reinterpret_cast<std::uint32_t*>(p + 8) = e;
}
RH_ScopedInstall(Or4b52c0, 0x004b52c0);

// 0x004b5240 — void fn(p,flag): cl=p[2]; if(flag) p[2]=cl|4
extern "C" __declspec(dllexport) void __cdecl Or4b5240(std::uint8_t* p, std::uint32_t flag) {
    std::uint8_t cl = p[2];
    if (flag) p[2] = static_cast<std::uint8_t>(cl | 4);
}
RH_ScopedInstall(Or4b5240, 0x004b5240);

// ===== round 116 =====

// 0x00489290 — byte-verified: void fn(): for(eax=0x705a10;;){ eax-=0x68;
//   *(u32*)(eax-4)=0; *(u32*)eax=1; if(eax==0x703170) break; }  (100-record reset)
extern "C" __declspec(dllexport) void __cdecl Reset489290(void) {
    std::uint32_t eax = 0x705a10u;
    do {
        eax -= 0x68u;
        *reinterpret_cast<std::uint32_t*>(eax - 4) = 0;
        *reinterpret_cast<std::uint32_t*>(eax) = 1;
    } while (eax != 0x703170u);
}
RH_ScopedInstall(Reset489290, 0x00489290);

// 0x004b7020 — byte-verified: uint fn(a, idx): if(idx==-1) return 0x6172f8;
//   else return *(u32*)(0x5d8818 + idx*4)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Tbl4b7020(std::uint32_t a, std::uint32_t idx) {
    (void)a;
    if (idx == 0xffffffffu) return 0x006172f8u;
    return *reinterpret_cast<std::uint32_t*>(0x005d8818u + idx * 4u);
}
RH_ScopedInstall(Tbl4b7020, 0x004b7020);

// 0x004b9540 — byte-verified: int fn(a, b): if(a>=6) return 1;
//   else return (int)(signed char)*(0x5d8880 + a*15 + b)
extern "C" __declspec(dllexport) int __cdecl Tbl4b9540(std::uint32_t a, std::uint32_t b) {
    if (a >= 6u) return 1;
    return *reinterpret_cast<signed char*>(0x005d8880u + a * 15u + b);
}
RH_ScopedInstall(Tbl4b9540, 0x004b9540);

// ===== round 117 =====
// 0x00489da0 — byte-verified: void fn(idx): ptr=*(u32*)(0x7067fc + idx*4);
//   for(0x200 recs of 0x20B at ptr){ *(u32*)ptr=0; *(u32*)(ptr+4)=0; ptr+=0x20; }
extern "C" __declspec(dllexport) void __cdecl Zero489da0(std::uint32_t idx) {
    std::uint8_t* p = *reinterpret_cast<std::uint8_t**>(0x007067fcu + idx * 4u);
    for (unsigned i = 0; i < 0x200u; i++) {
        *reinterpret_cast<std::uint32_t*>(p) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 4) = 0;
        p += 0x20u;
    }
}
RH_ScopedInstall(Zero489da0, 0x00489da0);

// ===== round 118 =====
// 0x004d54d0 — byte-verified: void fn(idx, out): *out = *(u32*)(0x7d57f8 + idx*8)
extern "C" __declspec(dllexport) void __cdecl Get4d54d0(std::uint32_t idx, std::uint32_t* out) {
    *out = *reinterpret_cast<std::uint32_t*>(0x007d57f8u + idx * 8u);
}
RH_ScopedInstall(Get4d54d0, 0x004d54d0);

// ===== round 119 =====
// 0x004893b0 — byte-verified: void fn(p): p[0xc]++; base=p[0]; idx=p[8];
//   *(u32*)(base + idx*0x30 - 0x14) = 1   (nested: p[0] is a base ptr)
extern "C" __declspec(dllexport) void __cdecl Inc4893b0(std::uint8_t* p) {
    *reinterpret_cast<std::uint32_t*>(p + 0xc) += 1;
    std::uint8_t* base = *reinterpret_cast<std::uint8_t**>(p);
    std::uint32_t idx = *reinterpret_cast<std::uint32_t*>(p + 8);
    *reinterpret_cast<std::uint32_t*>(base + idx * 0x30u - 0x14u) = 1;
}
RH_ScopedInstall(Inc4893b0, 0x004893b0);

// ===== round 120 =====
// 0x0046be10 — byte-verified: uint fn(arg0,a1,a2,a3): if(a1>=0x10) return 0;
//   idx=a1*0x341+a2; off=idx*4; *(u32*)(0x88219c+off)=*(u32*)arg0;
//   *(u32*)(0x88221c+off)=a3; return off
extern "C" __declspec(dllexport) std::uint32_t __cdecl Write46be10(std::uint8_t* arg0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) {
    if (a1 >= 0x10u) return 0;
    std::uint32_t off = (a1 * 0x341u + a2) * 4u;
    *reinterpret_cast<std::uint32_t*>(0x0088219cu + off) = *reinterpret_cast<std::uint32_t*>(arg0);
    *reinterpret_cast<std::uint32_t*>(0x0088221cu + off) = a3;
    return off;
}
RH_ScopedInstall(Write46be10, 0x0046be10);

// ===== round 121 =====
// 0x0041a4a0 — byte-verified: void fn(idx, src): if(src) memcpy(0x63c630 + idx*0xc4,
//   src, 0x10 dwords)  (esi=src=[esp+8] after pushes, edi=idx=[esp+4])
extern "C" __declspec(dllexport) void __cdecl Copy41a4a0(std::uint32_t idx, std::uint32_t* src) {
    if (src) {
        std::uint32_t* d = reinterpret_cast<std::uint32_t*>(0x0063c630u + idx * 0xc4u);
        for (unsigned i = 0; i < 0x10u; i++) d[i] = src[i];
    }
}
RH_ScopedInstall(Copy41a4a0, 0x0041a4a0);

// ===== round 122 =====
// 0x005ae550 — byte-verified: void fn(list, key): doubly-linked-list unlink.
//   node=list[8]; walk node=node[0] until node+0xc==key (or node[0]==list[0xc]=ret);
//   prev=node[4]; if(prev!=sentinel) prev[0]=node[0];
//   next=node[0]; if(next!=sentinel) next[4]=node[4].
extern "C" __declspec(dllexport) void __cdecl Unlink5ae550(std::uint8_t* list, std::uint8_t* key) {
    std::uint8_t* sentinel = *reinterpret_cast<std::uint8_t**>(list + 0xc);
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(list + 8);
    for (;;) {
        if (node + 0xc == key) break;
        node = *reinterpret_cast<std::uint8_t**>(node);
        if (node == sentinel) return;
    }
    std::uint8_t* prev = *reinterpret_cast<std::uint8_t**>(node + 4);
    if (prev != sentinel) *reinterpret_cast<std::uint8_t**>(prev) = *reinterpret_cast<std::uint8_t**>(node);
    std::uint8_t* next = *reinterpret_cast<std::uint8_t**>(node);
    if (next != sentinel) *reinterpret_cast<std::uint8_t**>(next + 4) = *reinterpret_cast<std::uint8_t**>(node + 4);
}
RH_ScopedInstall(Unlink5ae550, 0x005ae550);

// ===== round 123 =====
// 0x005aa030 — byte-verified: u32 fn(p, key): circular list head=p[0x10], sentinel=p+0x10,
//   node[0]=next, object=node-0x18; is-in-list predicate -> returns p if some object's
//   addr==key, else 0. (eax holds p throughout; the found path never reassigns it.)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Search5aa030(std::uint8_t* p, std::uint8_t* key) {
    std::uint8_t* sentinel = p + 0x10;
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(p + 0x10);
    if (node == sentinel) return 0;
    do {
        std::uint8_t* obj = node - 0x18;
        std::uint8_t* next = *reinterpret_cast<std::uint8_t**>(node);
        if (obj == key) return reinterpret_cast<std::uint32_t>(p);
        node = next;
    } while (node != sentinel);
    return 0;
}
RH_ScopedInstall(Search5aa030, 0x005aa030);

// ===== round 124 =====
// 0x005af700 — byte-verified: u32 fn(p, cont, idx): DLL get Nth. count=cont[8];
//   if(idx<count/2) walk fwd from p[0x20] idx times via node[0]; else walk bwd from
//   p[0x24] (count-1-idx) times via node[4]; return node-0x2c.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get5af700(std::uint8_t* p, std::uint8_t* cont, std::uint32_t idx) {
    std::uint32_t count = *reinterpret_cast<std::uint32_t*>(cont + 8);
    std::uint8_t* node;
    if (idx < (count >> 1)) {
        node = *reinterpret_cast<std::uint8_t**>(p + 0x20);
        for (std::uint32_t i = 0; i < idx; i++) node = *reinterpret_cast<std::uint8_t**>(node);
    } else {
        node = *reinterpret_cast<std::uint8_t**>(p + 0x24);
        std::uint32_t steps = count - 1 - idx;
        for (std::uint32_t i = 0; i < steps; i++) node = *reinterpret_cast<std::uint8_t**>(node + 4);
    }
    return reinterpret_cast<std::uint32_t>(node - 0x2c);
}
RH_ScopedInstall(Get5af700, 0x005af700);

// ===== round 105 =====

// 0x004773f0  render — byte-verified EAX-implicit (this in EAX) struct init.
//   Verbatim asm port (bit-identical by construction). Zeroes 14 dwords, sets bytes
//   [0x40..0x43]=0xff, [0x28]/[0x14]/[0]=1.0f, [0xc]=([0xc]|0x20003).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Init4773f0(void) {
    __asm {
        xor ecx, ecx
        mov dl, 0xff
        mov dword ptr [eax + 0x48], ecx
        mov dword ptr [eax + 0x4c], ecx
        mov dword ptr [eax + 0x50], ecx
        mov dword ptr [eax + 0x54], ecx
        mov dword ptr [eax + 0x58], ecx
        mov dword ptr [eax + 0x10], ecx
        mov dword ptr [eax + 8], ecx
        mov dword ptr [eax + 4], ecx
        mov dword ptr [eax + 0x24], ecx
        mov dword ptr [eax + 0x20], ecx
        mov dword ptr [eax + 0x18], ecx
        mov dword ptr [eax + 0x38], ecx
        mov dword ptr [eax + 0x34], ecx
        mov dword ptr [eax + 0x30], ecx
        mov ecx, dword ptr [eax + 0xc]
        mov byte ptr [eax + 0x40], dl
        mov byte ptr [eax + 0x41], dl
        mov byte ptr [eax + 0x42], dl
        mov byte ptr [eax + 0x43], dl
        mov edx, 0x3f800000
        or ecx, 0x20003
        mov dword ptr [eax + 0x28], edx
        mov dword ptr [eax + 0x14], edx
        mov dword ptr [eax], edx
        mov dword ptr [eax + 0xc], ecx
        ret
    }
}
RH_ScopedInstall(Init4773f0, 0x004773f0);

// ===== round 125 =====
// 0x004c75c0 — byte-verified two-level indexed global read (RW texture-catalog region):
//   mov eax,[0x7d40a8]      ; base = *(u32*)0x7d40a8
//   mov ecx,[0x7d3ff8]      ; idx  = *(u32*)0x7d3ff8
//   mov edx,[eax+ecx+0x28]  ; edx  = *(u32*)(base+idx+0x28)
//   lea eax,[eax+edx*4]     ; eax  = base + edx*4
//   mov eax,[eax+ecx]       ; return *(u32*)(base+edx*4+idx)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get4c75c0(void) {
    std::uint8_t* base = *reinterpret_cast<std::uint8_t**>(0x007d40a8);
    std::uint32_t idx  = *reinterpret_cast<std::uint32_t*>(0x007d3ff8);
    std::uint32_t edx  = *reinterpret_cast<std::uint32_t*>(base + idx + 0x28);
    return *reinterpret_cast<std::uint32_t*>(base + edx * 4 + idx);
}
RH_ScopedInstall(Get4c75c0, 0x004c75c0);

// ===== round 126 =====
// 0x00485340 — byte-verified bounded indexed-array getter:
//   mov eax,[esp+4]          ; i
//   cmp eax,[0x6e71c4]       ; bound
//   ja  fail                 ; unsigned: i > bound -> return 0
//   mov ecx,[0x6e71cc]       ; container ptr
//   mov edx,[ecx+0x10]       ; arr = container[0x10]
//   mov eax,[edx+eax*4]      ; return arr[i]
//   ret
// fail: xor eax,eax; ret
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get485340(std::uint32_t i) {
    if (i > *reinterpret_cast<std::uint32_t*>(0x006e71c4)) return 0;
    std::uint8_t* cont = *reinterpret_cast<std::uint8_t**>(0x006e71cc);
    std::uint8_t* arr  = *reinterpret_cast<std::uint8_t**>(cont + 0x10);
    return *reinterpret_cast<std::uint32_t*>(arr + i * 4);
}
RH_ScopedInstall(Get485340, 0x00485340);

// ===== round 127 =====
// 0x004d5480 — byte-verified mark-dirty setter (absolute tables):
//   mov eax,[esp+4]                ; i
//   mov ecx,[esp+8]                ; v
//   cmp [eax*8+0x7d57f8],ecx       ; if (tbl[i*2] == v)
//   je  ret                        ;   no change -> return
//   mov [eax*8+0x7d57f8],ecx       ; tbl[i*2] = v
//   mov ecx,[eax*8+0x7d57fc]       ; flag = tbl[i*2+1]
//   test ecx,ecx / jne ret         ; if (already dirty) return
//   mov ecx,[0x7d6c14]             ; count = *0x7d6c14
//   mov [eax*8+0x7d57fc],1         ; tbl[i*2+1] = 1
//   mov [ecx*4+0x7d5168],eax       ; dirtylist[count] = i
//   inc ecx / mov [0x7d6c14],ecx   ; count++
extern "C" __declspec(dllexport) void __cdecl Mark4d5480(std::uint32_t i, std::uint32_t v) {
    std::uint32_t* tbl = reinterpret_cast<std::uint32_t*>(0x007d57f8);
    if (tbl[i * 2] == v) return;
    tbl[i * 2] = v;
    if (tbl[i * 2 + 1] != 0) return;
    std::uint32_t count = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
    tbl[i * 2 + 1] = 1;
    reinterpret_cast<std::uint32_t*>(0x007d5168)[count] = i;
    *reinterpret_cast<std::uint32_t*>(0x007d6c14) = count + 1;
}
RH_ScopedInstall(Mark4d5480, 0x004d5480);

// ===== round 128 =====
// 0x0045baa0 — byte-verified ESI-keyed linear search:
//   edx=[0x5f9bd8] (count); if(count<=0) return 0; eax=0x5f9998 (base);
//   loop: if(esi==[eax]) return eax; eax+=0x40; while(++i<count); return 0.
//   ESI = search key (register arg). Reimpl is __cdecl(key); compares the returned ptr.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Search45baa0(std::uint32_t key) {
    int count = *reinterpret_cast<int*>(0x005f9bd8);
    if (count <= 0) return 0;
    std::uint8_t* p = reinterpret_cast<std::uint8_t*>(0x005f9998);
    for (int i = 0; i < count; i++) {
        if (*reinterpret_cast<std::uint32_t*>(p) == key) return reinterpret_cast<std::uint32_t>(p);
        p += 0x40;
    }
    return 0;
}
RH_ScopedInstall(Search45baa0, 0x0045baa0);

// 0x0041f330 — byte-verified ESI-keyed linear search into a PARALLEL array:
//   edx=[0x5f5fe0] (count); eax=0 (index); if(count<=0) return 0; ecx=0x5f3828 (base);
//   loop: if(esi==[ecx]) goto found; eax++; ecx+=0x84; while(eax<count); return 0;
//   found: return 0x5f37a8 + eax*0x84.   ESI = search key. Reimpl __cdecl(key).
extern "C" __declspec(dllexport) std::uint32_t __cdecl Search41f330(std::uint32_t key) {
    int count = *reinterpret_cast<int*>(0x005f5fe0);
    if (count <= 0) return 0;
    std::uint8_t* p = reinterpret_cast<std::uint8_t*>(0x005f3828);
    for (int i = 0; i < count; i++) {
        if (*reinterpret_cast<std::uint32_t*>(p) == key)
            return static_cast<std::uint32_t>(0x005f37a8 + i * 0x84);
        p += 0x84;
    }
    return 0;
}
RH_ScopedInstall(Search41f330, 0x0041f330);

// 0x0048ebc0 — byte-verified indexed-global signed division with high clamp:
//   ecx=[esp+4]*0x488; eax=8; cdq; idiv [ecx+0x76d994]; if(eax>=8) eax=8; return eax.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Div48ebc0(std::uint32_t arg) {
    int d = *reinterpret_cast<int*>(0x0076d994 + arg * 0x488);
    int q = 8 / d;
    if (q >= 8) q = 8;
    return static_cast<std::uint32_t>(q);
}
RH_ScopedInstall(Div48ebc0, 0x0048ebc0);

// ===== round 129 ===== (deferred-renderstate setters, same subsystem as r127)
// 0x004d6c40 — byte-verified: if(arg==*0x7d6bf0) return 1; *0x7d5890=tbl_0x5d8c64[arg];
//   if(*0x7d5894==0){ c=*0x7d6c14; *0x7d5894=1; queue_0x7d5168[c]=0x13; *0x7d6c14=c+1; }
//   *0x7d6bf0=arg; return 1.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Set4d6c40(std::uint32_t arg) {
    if (arg == *reinterpret_cast<std::uint32_t*>(0x007d6bf0)) return 1;
    *reinterpret_cast<std::uint32_t*>(0x007d5890) = *reinterpret_cast<std::uint32_t*>(0x005d8c64 + arg * 4);
    if (*reinterpret_cast<std::uint32_t*>(0x007d5894) == 0) {
        std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
        *reinterpret_cast<std::uint32_t*>(0x007d5894) = 1;
        reinterpret_cast<std::uint32_t*>(0x007d5168)[c] = 0x13;
        *reinterpret_cast<std::uint32_t*>(0x007d6c14) = c + 1;
    }
    *reinterpret_cast<std::uint32_t*>(0x007d6bf0) = arg;
    return 1;
}
RH_ScopedInstall(Set4d6c40, 0x004d6c40);

// 0x004d6c90 — twin of 0x4d6c40: prev 0x7d6bf4, dest 0x7d5898, flag 0x7d589c, queued 0x14.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Set4d6c90(std::uint32_t arg) {
    if (arg == *reinterpret_cast<std::uint32_t*>(0x007d6bf4)) return 1;
    *reinterpret_cast<std::uint32_t*>(0x007d5898) = *reinterpret_cast<std::uint32_t*>(0x005d8c64 + arg * 4);
    if (*reinterpret_cast<std::uint32_t*>(0x007d589c) == 0) {
        std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
        *reinterpret_cast<std::uint32_t*>(0x007d589c) = 1;
        reinterpret_cast<std::uint32_t*>(0x007d5168)[c] = 0x14;
        *reinterpret_cast<std::uint32_t*>(0x007d6c14) = c + 1;
    }
    *reinterpret_cast<std::uint32_t*>(0x007d6bf4) = arg;
    return 1;
}
RH_ScopedInstall(Set4d6c90, 0x004d6c90);

// 0x004d54f0 — byte-verified 3-arg deferred-renderstate setter (indexed by a,b):
//   off=((a*33+b)<<3); slot=0x7d4720+off; if(slot[0]==v) return; slot[0]=v;
//   if(slot[4]!=0) return; slot[4]=1; c=*0x7d6c18; q_0x7d62a8[c*2]=a; q[c*2+1]=b; *0x7d6c18=c+1.
extern "C" __declspec(dllexport) void __cdecl Set4d54f0(std::uint32_t a, std::uint32_t b, std::uint32_t v) {
    std::uint32_t off = (a * 33 + b) << 3;
    std::uint8_t* slot = reinterpret_cast<std::uint8_t*>(0x007d4720 + off);
    if (*reinterpret_cast<std::uint32_t*>(slot) == v) return;
    *reinterpret_cast<std::uint32_t*>(slot) = v;
    if (*reinterpret_cast<std::uint32_t*>(slot + 4) != 0) return;
    *reinterpret_cast<std::uint32_t*>(slot + 4) = 1;
    std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c18);
    reinterpret_cast<std::uint32_t*>(0x007d62a8)[c * 2] = a;
    reinterpret_cast<std::uint32_t*>(0x007d62a8)[c * 2 + 1] = b;
    *reinterpret_cast<std::uint32_t*>(0x007d6c18) = c + 1;
}
RH_ScopedInstall(Set4d54f0, 0x004d54f0);

// ===== round 130 ===== (float vec3 lerp — verbatim x87 naked port, bit-identical)
// 0x004b4650 — out = a + t*(b-a) per component. void fn(out*[esp+4], a*[esp+8],
//   b*[esp+0xc], float t[esp+0x10]). A C float reimpl would round differently than the
//   original's 80-bit x87 chain, so this is transcribed instruction-for-instruction.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Lerp4b4650(void) {
    __asm {
        mov eax, [esp+4]
        mov ecx, [esp+8]
        mov edx, [esp+0Ch]
        fld   dword ptr [edx]
        fsub  dword ptr [ecx]
        fstp  dword ptr [eax]
        fld   dword ptr [edx+4]
        fsub  dword ptr [ecx+4]
        fstp  dword ptr [eax+4]
        fld   dword ptr [edx+8]
        fsub  dword ptr [ecx+8]
        fstp  dword ptr [eax+8]
        fld   dword ptr [esp+10h]
        fmul  dword ptr [eax]
        fst   dword ptr [eax]
        fld   dword ptr [esp+10h]
        fmul  dword ptr [eax+4]
        fst   dword ptr [eax+4]
        fld   dword ptr [esp+10h]
        fmul  dword ptr [eax+8]
        fst   dword ptr [esp+10h]
        fstp  dword ptr [eax+8]
        fxch  st(1)
        fadd  dword ptr [ecx]
        fstp  dword ptr [eax]
        fadd  dword ptr [ecx+4]
        fstp  dword ptr [eax+4]
        fld   dword ptr [esp+10h]
        fadd  dword ptr [ecx+8]
        fstp  dword ptr [eax+8]
        ret
    }
}
RH_ScopedInstall(Lerp4b4650, 0x004b4650);

// ===== round 131 ===== (dot-product + cosine-clamp twins — verbatim x87 naked ports)
// 0x004726b0 — float fn(a*, b*): d = a.y*b.y + a.x*b.x (2D dot); clamp d to [-1.0, 1.0]
//   (consts 0x5cc320=1.0, 0x5cc33c=-1.0); return ST0. Verbatim x87 transcription.
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl Dot4726b0(void) {
    __asm {
        mov  eax, [esp+4]
        fld  dword ptr [eax+4]
        mov  ecx, [esp+8]
        fmul dword ptr [ecx+4]
        fld  dword ptr [eax]
        fmul dword ptr [ecx]
        faddp st(1), st(0)
        fcom dword ptr ds:[05CC33Ch]
        fnstsw ax
        test ah, 5
        jp   L1
        fstp st(0)
        fld  dword ptr ds:[05CC33Ch]
        ret
    L1:
        fcom dword ptr ds:[05CC320h]
        fnstsw ax
        test ah, 41h
        jne  L2
        fstp st(0)
        fld  dword ptr ds:[05CC320h]
        ret
    L2:
        ret
    }
}
RH_ScopedInstall(Dot4726b0, 0x004726b0);

// 0x004726f0 — float fn(a*, b*): d = a.y*b.y + a.x*b.x + a.z*b.z (3D dot); clamp to
//   [-1.0, 1.0]; return ST0. Verbatim x87 transcription (twin of 0x4726b0).
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl Dot4726f0(void) {
    __asm {
        mov  eax, [esp+4]
        fld  dword ptr [eax+4]
        mov  ecx, [esp+8]
        fmul dword ptr [ecx+4]
        fld  dword ptr [eax]
        fmul dword ptr [ecx]
        faddp st(1), st(0)
        fld  dword ptr [eax+8]
        fmul dword ptr [ecx+8]
        faddp st(1), st(0)
        fcom dword ptr ds:[05CC33Ch]
        fnstsw ax
        test ah, 5
        jp   M1
        fstp st(0)
        fld  dword ptr ds:[05CC33Ch]
        ret
    M1:
        fcom dword ptr ds:[05CC320h]
        fnstsw ax
        test ah, 41h
        jne  M2
        fstp st(0)
        fld  dword ptr ds:[05CC320h]
        ret
    M2:
        ret
    }
}
RH_ScopedInstall(Dot4726f0, 0x004726f0);

// ===== round 132 ===== (6-plane inside/frustum test — verbatim x87 naked port)
// 0x004cbb70 — u32 fn(obj*[esp+4], point*[esp+8]): for 6 planes at obj+0x94 stride 0x14
//   (x@0,y@4,z@8,w@0xc), compute dot(plane.xyz, point.xyz) - plane.w and compare to
//   -point.w; return 1 if every plane passes, else 0. -point.w is stashed at [esp+8]
//   (scratch over the obj arg slot after the push esi). Verbatim x87 transcription.
extern "C" __declspec(dllexport) __declspec(naked) std::uint32_t __cdecl Inside4cbb70(void) {
    __asm {
        mov  edx, [esp+8]
        mov  eax, [esp+4]
        push esi
        mov  esi, 6
        fld  dword ptr [edx+0Ch]
        fchs
        fstp dword ptr [esp+8]
        lea  ecx, [eax+94h]
    PL:
        fld  dword ptr [ecx+4]
        fmul dword ptr [edx+4]
        fld  dword ptr [ecx]
        fmul dword ptr [edx]
        faddp st(1), st(0)
        fld  dword ptr [ecx+8]
        fmul dword ptr [edx+8]
        faddp st(1), st(0)
        fsub dword ptr [ecx+0Ch]
        fcomp dword ptr [esp+8]
        fnstsw ax
        and  eax, 4100h
        je   OUTSIDE
        add  ecx, 14h
        dec  esi
        jne  PL
        mov  eax, 1
        pop  esi
        ret
    OUTSIDE:
        xor  eax, eax
        pop  esi
        ret
    }
}
RH_ScopedInstall(Inside4cbb70, 0x004cbb70);

// ===== round 133 ===== (value -> 3-component split; EAX=v in, EDI=out)
// 0x0041e170 — byte-verified: a=v/6000; out[0]=a; rem1=v-a*6000; b=rem1/100; out[1]=b;
//   out[2]=v-(a*60+b)*100. The original uses magic-multiply reciprocals (0x57619f1>>39,
//   0x51eb851f>>37) which equal signed integer division, so plain C `/` is bit-identical.
extern "C" __declspec(dllexport) void __cdecl Split41e170(std::uint32_t v, int* out) {
    int vi = static_cast<int>(v);
    int a = vi / 6000;
    out[0] = a;
    int rem1 = vi - a * 6000;
    int b = rem1 / 100;
    out[1] = b;
    out[2] = vi - (a * 60 + b) * 100;
}
RH_ScopedInstall(Split41e170, 0x0041e170);

// ===== round 134 ===== (bounded 2D-grid multi-out getter)
// 0x004957a0 — u32 fn(i, j, out1, out2, out3): if(i>=*0x772fac || j>=*0x771e80) return 0;
//   idx=i*0x89+j; if(out1){out1[0]=tbl150[idx*8]; out1[1]=tbl154[idx*8];}
//   if(out2){out2[0]=tbl1d0[idx*8]; out2[1]=tbl1d4[idx*8];} idx3=i*0x112+j;
//   if(out3) out3[0]=tbl290[idx3*4]; return 1.
//   NB: original reads out2/out3 via post-`push esi/edi` [esp+0x18]/[esp+0x1c] which map
//   to pre-push +0x10/+0x14 -> they are args 4 and 5, not 6 and 7.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Grid4957a0(
        std::uint32_t i, std::uint32_t j, int* out1, int* out2, int* out3) {
    if (static_cast<int>(i) >= *reinterpret_cast<int*>(0x00772fac)) return 0;
    if (static_cast<int>(j) >= *reinterpret_cast<int*>(0x00771e80)) return 0;
    int idx = static_cast<int>(i) * 0x89 + static_cast<int>(j);
    if (out1) {
        out1[0] = *reinterpret_cast<int*>(0x00772150 + idx * 8);
        out1[1] = *reinterpret_cast<int*>(0x00772154 + idx * 8);
    }
    if (out2) {
        out2[0] = *reinterpret_cast<int*>(0x007721d0 + idx * 8);
        out2[1] = *reinterpret_cast<int*>(0x007721d4 + idx * 8);
    }
    int idx3 = static_cast<int>(i) * 0x112 + static_cast<int>(j);
    if (out3) out3[0] = *reinterpret_cast<int*>(0x00772290 + idx3 * 4);
    return 1;
}
RH_ScopedInstall(Grid4957a0, 0x004957a0);

// [REVERTED 2026-06-15] round 135 FUN_00528e30 (0x00528e30) is in the statically-linked
// libpng/zlib band (0x516000-0x529fff, right after zcalloc/zcfree) -> library-skip per user
// ruling; demoted C3->C2. Was a deterministic struct constructor reimpl.
// (0x0046cbb0 CarStatePairGet was ALREADY C3 since round 25 / PromoLoop_round25.cpp —
//  status-precheck miss; duplicate reverted to avoid a double RH_ScopedInstall.)

// ===== round 136 ===== (doubly-linked-list remove + count decrement)
// 0x005b35e0 — void fn(list, node): a pure-read search loop (converges either way, skipped
//   for an empty list[4]==list+4) then: list[0]--; A=node[0x24]; B=node[0x20]; *A=B;
//   *(B+4)=A  (intrusive DLL unlink via node's own [0x20]/[0x24] link pointers).
extern "C" __declspec(dllexport) void __cdecl Remove5b35e0(std::uint8_t* list, std::uint8_t* node) {
    std::uint8_t* sentinel = list + 4;
    std::uint8_t* p = *reinterpret_cast<std::uint8_t**>(list + 4);
    while (p != sentinel) {
        if (p - 0x20 == node) break;
        p = *reinterpret_cast<std::uint8_t**>(p);
    }
    int cnt = *reinterpret_cast<int*>(list);
    *reinterpret_cast<int*>(list) = cnt - 1;
    std::uint8_t* A = *reinterpret_cast<std::uint8_t**>(node + 0x24);
    std::uint8_t* B = *reinterpret_cast<std::uint8_t**>(node + 0x20);
    *reinterpret_cast<std::uint8_t**>(A) = B;
    *reinterpret_cast<std::uint8_t**>(B + 4) = A;
}
RH_ScopedInstall(Remove5b35e0, 0x005b35e0);

// ===== round 137 ===== (doubly-linked-list insert-at-head)
// 0x004c5bc0 — void fn(list, node): if(node[4]) { *(node[0xc])=node[8]; *(node[8]+4)=node[0xc]; }
//   node[4]=list; head=list+8; old=*head; node[0xc]=head; node[8]=old; *(old+4)=&node[8];
//   *head=&node[8].  (intrusive list; links reference the &node[8] next-field address.)
extern "C" __declspec(dllexport) void __cdecl Insert4c5bc0(std::uint8_t* list, std::uint8_t* node) {
    if (*reinterpret_cast<std::uint32_t*>(node + 4) != 0) {
        *reinterpret_cast<std::uint8_t**>(*reinterpret_cast<std::uint8_t**>(node + 0xc)) = *reinterpret_cast<std::uint8_t**>(node + 8);
        *reinterpret_cast<std::uint8_t**>(*reinterpret_cast<std::uint8_t**>(node + 8) + 4) = *reinterpret_cast<std::uint8_t**>(node + 0xc);
    }
    *reinterpret_cast<std::uint8_t**>(node + 4) = list;
    std::uint8_t* head = list + 8;
    std::uint8_t* old = *reinterpret_cast<std::uint8_t**>(head);
    *reinterpret_cast<std::uint8_t**>(node + 0xc) = head;
    *reinterpret_cast<std::uint8_t**>(node + 8) = old;
    *reinterpret_cast<std::uint8_t**>(old + 4) = node + 8;
    *reinterpret_cast<std::uint8_t**>(head) = node + 8;
}
RH_ScopedInstall(Insert4c5bc0, 0x004c5bc0);

// ===== round 138 ===== (4-entry global ptr-table match predicate)
// 0x0045c510 — u32 fn(arg1, arg2): for idx in 0..3: e=*(int**)(0x88f680 + idx*4);
//   if(e && e[0xc]==1 && e[0x28]==arg1 && arg2[4]==idx) return 1; return 0.
//   (orig loads arg2 then arg1 across two pushes -> arg1 first stack arg, arg2 second.)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Match45c510(std::uint32_t arg1, std::uint8_t* arg2) {
    for (int idx = 0; idx < 4; idx++) {
        std::uint8_t* e = *reinterpret_cast<std::uint8_t**>(0x0088f680 + idx * 4);
        if (e && *reinterpret_cast<int*>(e + 0xc) == 1
              && *reinterpret_cast<std::uint32_t*>(e + 0x28) == arg1
              && *reinterpret_cast<int*>(arg2 + 4) == idx)
            return 1;
    }
    return 0;
}
RH_ScopedInstall(Match45c510, 0x0045c510);

// ===== round 139 ===== (global-record clear + conditional return)
// 0x004e4350 — u32 fn(arg1, arg2): rec = *(u8**)0x7d716c + arg2; if(rec[0xc] != 0)
//   { rec[0xc]=0; rec[8]=0; return arg1; } return 0.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Clear4e4350(std::uint32_t arg1, std::uint32_t arg2) {
    std::uint8_t* rec = *reinterpret_cast<std::uint8_t**>(0x007d716c) + arg2;
    if (*reinterpret_cast<std::uint32_t*>(rec + 0xc) != 0) {
        *reinterpret_cast<std::uint32_t*>(rec + 0xc) = 0;
        *reinterpret_cast<std::uint32_t*>(rec + 8) = 0;
        return arg1;
    }
    return 0;
}
RH_ScopedInstall(Clear4e4350, 0x004e4350);

// ===== round 140 ===== (abs-array scan -> dirty flag)
// 0x0042c150 — void fn(): for p=0x67ea14; p<0x67ea44; p+=0x18: count nonzero among
//   [p-4],[p],[p+4],[p+8],[p+0xc],[p+0x10]; if(count!=0) *(u32*)0x67eab4 = 0xff.
extern "C" __declspec(dllexport) void __cdecl Scan42c150(void) {
    int cnt = 0;
    for (std::uint8_t* p = reinterpret_cast<std::uint8_t*>(0x0067ea14);
         reinterpret_cast<std::uintptr_t>(p) < 0x0067ea44; p += 0x18) {
        if (*reinterpret_cast<int*>(p - 4)) cnt++;
        if (*reinterpret_cast<int*>(p)) cnt++;
        if (*reinterpret_cast<int*>(p + 4)) cnt++;
        if (*reinterpret_cast<int*>(p + 8)) cnt++;
        if (*reinterpret_cast<int*>(p + 0xc)) cnt++;
        if (*reinterpret_cast<int*>(p + 0x10)) cnt++;
    }
    if (cnt != 0) *reinterpret_cast<std::uint32_t*>(0x0067eab4) = 0xff;
}
RH_ScopedInstall(Scan42c150, 0x0042c150);

// ===== round 141 ===== (global 2-level list search)
// 0x004850b0 — int fn(key): g=*(u8**)0x6e71cc; node=*(u8**)(g+4); if(node==0) return -1;
//   loop: e=*(u8**)node; if(e && e[8]==key) return e[0xc]; node=*(u8**)(node+8); if(node) loop;
//   return -1.
extern "C" __declspec(dllexport) int __cdecl Search4850b0(std::uint32_t key) {
    std::uint8_t* g = *reinterpret_cast<std::uint8_t**>(0x006e71cc);
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(g + 4);
    while (node) {
        std::uint8_t* e = *reinterpret_cast<std::uint8_t**>(node);
        if (e && *reinterpret_cast<std::uint32_t*>(e + 8) == key)
            return *reinterpret_cast<int*>(e + 0xc);
        node = *reinterpret_cast<std::uint8_t**>(node + 8);
    }
    return -1;
}
RH_ScopedInstall(Search4850b0, 0x004850b0);

// ===== round 142 ===== (4-branch pure pointer getter)
// 0x005aa9c0 — u32 fn(arg): c=arg[0x20]; if(c){ p=arg[0]; f=p[0x40]&0xfffffff;
//   return (arg[0x1c]&2) ? f+c+4 : f+c; } else return (arg[0x1c]&2) ? arg+0x2c : arg+0x28.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get5aa9c0(std::uint8_t* arg) {
    std::uint32_t c = *reinterpret_cast<std::uint32_t*>(arg + 0x20);
    if (c != 0) {
        std::uint8_t* p = *reinterpret_cast<std::uint8_t**>(arg);
        std::uint32_t f = *reinterpret_cast<std::uint32_t*>(p + 0x40) & 0x0fffffff;
        if (*reinterpret_cast<std::uint8_t*>(arg + 0x1c) & 2) return f + c + 4;
        return f + c;
    }
    if (*reinterpret_cast<std::uint8_t*>(arg + 0x1c) & 2) return reinterpret_cast<std::uint32_t>(arg + 0x2c);
    return reinterpret_cast<std::uint32_t>(arg + 0x28);
}
RH_ScopedInstall(Get5aa9c0, 0x005aa9c0);

// ===== round 143 ===== (global DLL insert-at-head + flag-bit clear)
// 0x005a7420 — u32 fn(arg): node=arg+0x28; node[4]=0; node[0]=0; old=*0x7dca24;
//   node[4]=0x7dca24; node[0]=old; old[4]=node; *0x7dca24=node; node[0xc]&=~1; return 1.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Insert5a7420(std::uint8_t* arg) {
    std::uint8_t* node = arg + 0x28;
    *reinterpret_cast<std::uint32_t*>(node + 4) = 0;
    *reinterpret_cast<std::uint32_t*>(node) = 0;
    std::uint8_t* old = *reinterpret_cast<std::uint8_t**>(0x007dca24);
    *reinterpret_cast<std::uint32_t*>(node + 4) = 0x007dca24;
    *reinterpret_cast<std::uint8_t**>(node) = old;
    *reinterpret_cast<std::uint8_t**>(old + 4) = node;
    *reinterpret_cast<std::uint8_t**>(0x007dca24) = node;
    std::uint32_t f = *reinterpret_cast<std::uint32_t*>(node + 0xc);
    *reinterpret_cast<std::uint32_t*>(node + 0xc) = f & 0xfffffffe;
    return 1;
}
RH_ScopedInstall(Insert5a7420, 0x005a7420);

// ===== round 144 ===== (global-field-offset struct clear + conditional copy)
// 0x00558140 — u32 fn(arg): V=*0x913274; entry=*(u8**)(arg+V); if(entry==0) return 0;
//   if(entry[0]==0) return arg; if(entry[4]) arg[0x48]=entry[4]; entry[4]=0; entry[0]=0; return arg.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Clear558140(std::uint8_t* arg) {
    std::uint32_t V = *reinterpret_cast<std::uint32_t*>(0x00913274);
    std::uint8_t* entry = *reinterpret_cast<std::uint8_t**>(arg + V);
    if (entry == 0) return 0;
    if (*reinterpret_cast<std::uint32_t*>(entry) == 0) return reinterpret_cast<std::uint32_t>(arg);
    std::uint32_t e4 = *reinterpret_cast<std::uint32_t*>(entry + 4);
    if (e4 != 0) *reinterpret_cast<std::uint32_t*>(arg + 0x48) = e4;
    *reinterpret_cast<std::uint32_t*>(entry + 4) = 0;
    *reinterpret_cast<std::uint32_t*>(entry) = 0;
    return reinterpret_cast<std::uint32_t>(arg);
}
RH_ScopedInstall(Clear558140, 0x00558140);

// ===== round 145 ===== (4-state dispatch with list-unlink)
// 0x005b0ec0 — void fn(p): state=p[0x48]; state1: if(p[0x14]){ *(p[0x18])=p[0x14];
//   *(p[0x14]+4)=p[0x18]; p[0x18]=0; p[0x14]=0; } p[0x50]=3; state2: p[0x50]=6;
//   state3: p[0x50]=5; else (no change).
extern "C" __declspec(dllexport) void __cdecl StateAdv5b0ec0(std::uint8_t* p) {
    int state = *reinterpret_cast<int*>(p + 0x48);
    if (state == 1) {
        std::uint8_t* a = *reinterpret_cast<std::uint8_t**>(p + 0x14);
        if (a) {
            std::uint8_t* b = *reinterpret_cast<std::uint8_t**>(p + 0x18);
            *reinterpret_cast<std::uint8_t**>(b) = a;
            *reinterpret_cast<std::uint8_t**>(a + 4) = b;
            *reinterpret_cast<std::uint32_t*>(p + 0x18) = 0;
            *reinterpret_cast<std::uint32_t*>(p + 0x14) = 0;
        }
        *reinterpret_cast<std::uint32_t*>(p + 0x50) = 3;
    } else if (state == 2) {
        *reinterpret_cast<std::uint32_t*>(p + 0x50) = 6;
    } else if (state == 3) {
        *reinterpret_cast<std::uint32_t*>(p + 0x50) = 5;
    }
}
RH_ScopedInstall(StateAdv5b0ec0, 0x005b0ec0);

// ===== round 146 ===== (byte-field modular counter)
// 0x005b11d0 — void fn(p): a = (u8)p[0] + 1; if(a >= (u8)p[3]) a -= (u8)p[3];
//   p[0] = (u8)a; p[1] = (u8)(p[1] - 1).  (cmp is on full 32-bit a vs p[3].)
extern "C" __declspec(dllexport) void __cdecl Counter5b11d0(std::uint8_t* p) {
    std::uint32_t a = static_cast<std::uint32_t>(p[0]) + 1;
    std::uint32_t lim = p[3];
    if (a >= lim) a -= lim;
    p[0] = static_cast<std::uint8_t>(a);
    p[1] = static_cast<std::uint8_t>(p[1] - 1);
}
RH_ScopedInstall(Counter5b11d0, 0x005b11d0);

// ===== round 147 ===== (arg-or-default memcpy to absolute dest)
// 0x00476a10 — void fn(src): if(src==0) src=0x692558; memcpy(0x6924e8, src, 16 dwords).
extern "C" __declspec(dllexport) void __cdecl Copy476a10(std::uint8_t* src) {
    if (src == 0) src = reinterpret_cast<std::uint8_t*>(0x00692558);
    std::uint32_t* d = reinterpret_cast<std::uint32_t*>(0x006924e8);
    std::uint32_t* s = reinterpret_cast<std::uint32_t*>(src);
    for (int i = 0; i < 16; i++) d[i] = s[i];
}
RH_ScopedInstall(Copy476a10, 0x00476a10);

// ===== round 148 ===== (byte-indexed table bit-clear)
// 0x005b1180 — void fn(p): if(p[1]!=p[3]){ off=(u8)p[1]+(u8)p[0]; if(off>=(u8)p[3]) off-=p[3];
//   p[1]++; ptr=*(u8**)(p+4) + off*0x14; } else ptr=0; *(u32*)ptr &= ~8.  (else-branch derefs
//   null exactly as the original; only the main path is exercised.)
extern "C" __declspec(dllexport) void __cdecl Tbl5b1180(std::uint8_t* p) {
    std::uint32_t* ptr;
    if (p[1] != p[3]) {
        std::uint32_t off = static_cast<std::uint32_t>(p[1]) + static_cast<std::uint32_t>(p[0]);
        if (off >= p[3]) off -= p[3];
        p[1] = static_cast<std::uint8_t>(p[1] + 1);
        std::uint8_t* tbl = *reinterpret_cast<std::uint8_t**>(p + 4);
        ptr = reinterpret_cast<std::uint32_t*>(tbl + off * 0x14);
    } else {
        ptr = reinterpret_cast<std::uint32_t*>(0);
    }
    *ptr &= 0xfffffff7;
}
RH_ScopedInstall(Tbl5b1180, 0x005b1180);

// [REVERTED 2026-06-15] round 149 FUN_00517200 (0x00517200) is in the statically-linked
// libpng/zlib band (0x516000-0x529fff) -> library-skip per user ruling; demoted C3->C2.
// Was a bounded backward search over a 5-byte-stride table reimpl.

// ===== round 150 ===== (circular-list search by key field)
// 0x005b0b60 — u32 fn(list, key): node=*(u8**)list; while(node!=list){ if(*(u32*)(node-0x44)
//   ==key) return node-0x4c; node=*(u8**)node; } return 0.  (circular list, sentinel=list.)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Search5b0b60(std::uint8_t* list, std::uint32_t key) {
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(list);
    while (node != list) {
        if (*reinterpret_cast<std::uint32_t*>(node - 0x44) == key)
            return reinterpret_cast<std::uint32_t>(node - 0x4c);
        node = *reinterpret_cast<std::uint8_t**>(node);
    }
    return 0;
}
RH_ScopedInstall(Search5b0b60, 0x005b0b60);

// ===== round 151 ===== (global-field-offset struct set — companion of r144 0x558140)
// 0x00558100 — u32 fn(arg): V=*0x913274; entry=*(u8**)(arg+V); if(entry==0) return 0;
//   if(entry[0]!=0) return arg; entry[4]=arg[0x48]; arg[0x48]=0x557b70; entry[0]=1; return arg.
//   (the original's 0x4e5f90 store is dead: its guard `0x557b70 != 0` is always true.)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Set558100(std::uint8_t* arg) {
    std::uint32_t V = *reinterpret_cast<std::uint32_t*>(0x00913274);
    std::uint8_t* entry = *reinterpret_cast<std::uint8_t**>(arg + V);
    if (entry == 0) return 0;
    if (*reinterpret_cast<std::uint32_t*>(entry) != 0) return reinterpret_cast<std::uint32_t>(arg);
    *reinterpret_cast<std::uint32_t*>(entry + 4) = *reinterpret_cast<std::uint32_t*>(arg + 0x48);
    *reinterpret_cast<std::uint32_t*>(arg + 0x48) = 0x557b70;
    *reinterpret_cast<std::uint32_t*>(entry) = 1;
    return reinterpret_cast<std::uint32_t>(arg);
}
RH_ScopedInstall(Set558100, 0x00558100);

// ===== round 152 ===== (EAX-dest struct init: 16-dword copy + field sets)
// 0x00477450 — void fn(EAX=dest, [esp+4]=src, [esp+8]=arg2, [esp+0xc]=arg3, [esp+0x10]=arg4):
//   dest[0x54]=0; dest[0x48]=0; dest[0x4c]=arg3; dest[0x58]=1; dest[0x50]=arg4;
//   memcpy(dest, src, 16 dwords); dest[0x40]=*arg2.  (dest in EAX; reimpl takes it as arg0.)
extern "C" __declspec(dllexport) void __cdecl Init477450(
        std::uint8_t* dest, std::uint8_t* src, std::uint32_t* arg2, std::uint32_t arg3, std::uint32_t arg4) {
    *reinterpret_cast<std::uint32_t*>(dest + 0x54) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x48) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x4c) = arg3;
    *reinterpret_cast<std::uint32_t*>(dest + 0x58) = 1;
    *reinterpret_cast<std::uint32_t*>(dest + 0x50) = arg4;
    for (int i = 0; i < 16; i++) reinterpret_cast<std::uint32_t*>(dest)[i] = reinterpret_cast<std::uint32_t*>(src)[i];
    *reinterpret_cast<std::uint32_t*>(dest + 0x40) = *arg2;
}
RH_ScopedInstall(Init477450, 0x00477450);

// ===== round 153 ===== (struct-table div/mod compute + out-param)
// 0x005b2fd0 — u32 fn(arg1, arg2, arg3, arg4, arg5):
//   div = *(u32*)(*(u8**)(arg1+0x18) + arg4*0x28 + 0x20);
//   q = arg2 / div (unsigned); rem = arg2 % div; *arg5 = rem;
//   return *(u32*)(*(u8**)(arg1+0x10) + arg3*0x20 + 0x1c) + *(u32*)(arg1+0x20)*q + rem.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Calc5b2fd0(
        std::uint8_t* arg1, std::uint32_t arg2, std::uint32_t arg3, std::uint32_t arg4, std::uint32_t* arg5) {
    std::uint32_t div = *reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint8_t**>(arg1 + 0x18) + arg4 * 0x28 + 0x20);
    std::uint32_t q = arg2 / div;
    std::uint32_t rem = arg2 % div;
    *arg5 = rem;
    std::uint32_t base = *reinterpret_cast<std::uint32_t*>(*reinterpret_cast<std::uint8_t**>(arg1 + 0x10) + arg3 * 0x20 + 0x1c);
    return base + *reinterpret_cast<std::uint32_t*>(arg1 + 0x20) * q + rem;
}
RH_ScopedInstall(Calc5b2fd0, 0x005b2fd0);

// ===== round 154 ===== (ring-buffer copy to linear dest)
// 0x005ab980 — void fn(arg): esi=arg[0xc]-*0x7dd610; cnt=*0x7dd614-esi; if(cnt>=arg[0x14])
//   cnt=arg[0x14]; memcpy(arg[0x18], 0x7dce08+esi, cnt); arg[0x18]+=cnt; arg[0x14]-=cnt.
extern "C" __declspec(dllexport) void __cdecl Ring5ab980(std::uint8_t* arg) {
    std::uint32_t esi = *reinterpret_cast<std::uint32_t*>(arg + 0xc) - *reinterpret_cast<std::uint32_t*>(0x007dd610);
    std::uint32_t cnt = *reinterpret_cast<std::uint32_t*>(0x007dd614) - esi;
    std::uint32_t lim = *reinterpret_cast<std::uint32_t*>(arg + 0x14);
    if (cnt >= lim) cnt = lim;
    std::uint8_t* src = reinterpret_cast<std::uint8_t*>(0x007dce08 + esi);
    std::uint8_t* dst = *reinterpret_cast<std::uint8_t**>(arg + 0x18);
    for (std::uint32_t i = 0; i < cnt; i++) dst[i] = src[i];
    *reinterpret_cast<std::uint32_t*>(arg + 0x18) = reinterpret_cast<std::uint32_t>(dst) + cnt;
    *reinterpret_cast<std::uint32_t*>(arg + 0x14) = lim - cnt;
}
RH_ScopedInstall(Ring5ab980, 0x005ab980);

// ===== round 155 ===== (integer Newton-method sqrt, stdcall)
// 0x0049da90 — int __stdcall fn(n): if(n>0x40000000) return 0x8000; x=1; if(n>1) do{x+=x;}
//   while(x*x<n); if(n==0) return 0; then 3 Newton iters x=(x*x+n)/(2*x) (with x<0 overflow
//   guards after iters 1 and 2); return x. (stdcall orig; reimpl __cdecl, compared by value.)
extern "C" __declspec(dllexport) int __cdecl Sqrt49da90(std::uint32_t narg) {
    int n = static_cast<int>(narg);
    if (n > 0x40000000) return 0x8000;
    int x = 1;
    if (n > 1) { do { x += x; } while (x * x < n); }
    if (n == 0) return 0;
    x = (x * x + n) / (2 * x);
    if (x < 0) return x;
    x = (x * x + n) / (2 * x);
    if (x < 0) return x;
    x = (x * x + n) / (2 * x);
    return x;
}
RH_ScopedInstall(Sqrt49da90, 0x0049da90);

// ===== round 156 ===== (3-arg struct init with nested sub-object)
// 0x00485a00 — void fn(a, b, dest): dest[0]=b; dest[4]=a[4]; dest[8/0x10/0x18/0x1c/0x20/0x24/
//   0x28]=0; dest[0xc]=1.0f; dest[0x14]=0.01f; sub=dest[0x60]; sub[0x3c]=0; sub[0x40]=0;
//   sub[0x44]=0; sub[0x38]=1; sub[0x48]=0; sub[0x50]=1.
extern "C" __declspec(dllexport) void __cdecl Init485a00(std::uint8_t* a, std::uint32_t b, std::uint8_t* dest) {
    std::uint32_t a4 = *reinterpret_cast<std::uint32_t*>(a + 4);
    *reinterpret_cast<std::uint32_t*>(dest) = b;
    *reinterpret_cast<std::uint32_t*>(dest + 4) = a4;
    *reinterpret_cast<std::uint32_t*>(dest + 8) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x10) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x18) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x1c) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x20) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x24) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0x28) = 0;
    *reinterpret_cast<std::uint32_t*>(dest + 0xc) = 0x3f800000;
    *reinterpret_cast<std::uint32_t*>(dest + 0x14) = 0x3c23d70a;
    std::uint8_t* sub = *reinterpret_cast<std::uint8_t**>(dest + 0x60);
    *reinterpret_cast<std::uint32_t*>(sub + 0x3c) = 0;
    *reinterpret_cast<std::uint32_t*>(sub + 0x40) = 0;
    *reinterpret_cast<std::uint32_t*>(sub + 0x44) = 0;
    *reinterpret_cast<std::uint32_t*>(sub + 0x38) = 1;
    *reinterpret_cast<std::uint32_t*>(sub + 0x48) = 0;
    *reinterpret_cast<std::uint32_t*>(sub + 0x50) = 1;
}
RH_ScopedInstall(Init485a00, 0x00485a00);

// ===== round 157 ===== (2-way flag-branch struct compute)
// 0x005b93a0 — void fn(p, arg2): if(p[0x94][0x50] & 8){ sub=p[0x11c]; sub[0x88]=0;
//   sub[0x8c]=arg2; } else { s=p[0x84]; val=((u32)s[0x38]>>3)*(u32)s[0x39]*arg2;
//   p[0x8c]=val; p[0x90]=val; p[0x88]=arg2; p[0x28] |= 0x400; }
extern "C" __declspec(dllexport) void __cdecl Calc5b93a0(std::uint8_t* p, std::uint32_t arg2) {
    std::uint8_t* f = *reinterpret_cast<std::uint8_t**>(p + 0x94);
    if (*reinterpret_cast<std::uint8_t*>(f + 0x50) & 8) {
        std::uint8_t* sub = *reinterpret_cast<std::uint8_t**>(p + 0x11c);
        *reinterpret_cast<std::uint32_t*>(sub + 0x88) = 0;
        *reinterpret_cast<std::uint32_t*>(sub + 0x8c) = arg2;
    } else {
        std::uint8_t* s = *reinterpret_cast<std::uint8_t**>(p + 0x84);
        std::uint32_t val = (static_cast<std::uint32_t>(s[0x38]) >> 3) * static_cast<std::uint32_t>(s[0x39]) * arg2;
        *reinterpret_cast<std::uint32_t*>(p + 0x8c) = val;
        *reinterpret_cast<std::uint32_t*>(p + 0x90) = val;
        *reinterpret_cast<std::uint32_t*>(p + 0x88) = arg2;
        *reinterpret_cast<std::uint32_t*>(p + 0x28) |= 0x400;
    }
}
RH_ScopedInstall(Calc5b93a0, 0x005b93a0);

// ===== round 158 ===== (strided record-array zeroer + record-index field)
// 0x00484c90 — void fn(): for p=0x6dccbc; p<0x6e70cc; p+=0x8c (idx 0,1,2,..):
//   *(u16*)(p+0x1c)=idx; zero [p-4],[p],[p+4],[p+8],[p+0xc],[p+0x10],[p+0x18]; zero 9 dwords
//   at [p+0x64]. Then *0x6e70d8=0; *0x6e70cc=0.
extern "C" __declspec(dllexport) void __cdecl Zero484c90(void) {
    int idx = 0;
    for (std::uint8_t* p = reinterpret_cast<std::uint8_t*>(0x006dccbc);
         reinterpret_cast<std::uintptr_t>(p) < 0x006e70cc; p += 0x8c) {
        *reinterpret_cast<std::uint16_t*>(p + 0x1c) = static_cast<std::uint16_t>(idx);
        *reinterpret_cast<std::uint32_t*>(p - 4) = 0;
        *reinterpret_cast<std::uint32_t*>(p) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 4) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 8) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 0xc) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 0x10) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 0x18) = 0;
        for (int k = 0; k < 9; k++) reinterpret_cast<std::uint32_t*>(p + 0x64)[k] = 0;
        idx++;
    }
    *reinterpret_cast<std::uint32_t*>(0x006e70d8) = 0;
    *reinterpret_cast<std::uint32_t*>(0x006e70cc) = 0;
}
RH_ScopedInstall(Zero484c90, 0x00484c90);

// ===== round 159 ===== (paired vec3 array fill: copy src into arr1, zero arr2)
// 0x004899c0 — void fn(p, src): count=p[0xc]; if(!count) return; arr1=p[0]; arr2=p[4];
//   for i in [0,count): *(vec3*)(arr1+i*12) = *(vec3*)src; *(vec3*)(arr2+i*12) = {0,0,0}.
extern "C" __declspec(dllexport) void __cdecl Fill4899c0(std::uint8_t* p, std::uint8_t* src) {
    int count = *reinterpret_cast<int*>(p + 0xc);
    if (count == 0) return;
    std::uint8_t* arr1 = *reinterpret_cast<std::uint8_t**>(p);
    std::uint8_t* arr2 = *reinterpret_cast<std::uint8_t**>(p + 4);
    for (int i = 0; i < count; i++) {
        std::uint32_t* d1 = reinterpret_cast<std::uint32_t*>(arr1 + i * 12);
        d1[0] = *reinterpret_cast<std::uint32_t*>(src);
        d1[1] = *reinterpret_cast<std::uint32_t*>(src + 4);
        d1[2] = *reinterpret_cast<std::uint32_t*>(src + 8);
        std::uint32_t* d2 = reinterpret_cast<std::uint32_t*>(arr2 + i * 12);
        d2[0] = 0; d2[1] = 0; d2[2] = 0;
    }
}
RH_ScopedInstall(Fill4899c0, 0x004899c0);

// ===== round 160 ===== (bounded abs-table state setter)
// 0x00458f20 — u32 fn(i, arg2): if((int)i<0 || i>=0x19) return 0; rec=0x68b198+i*0x50;
//   if(arg2==0){ rec[0x20]=3; rec[0x1c]=0; } else if(rec[0x20]==3){ rec[0x20]=1; rec[0x1c]=0; }
//   else { rec[0x1c]=0; } return 1.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Set458f20(std::uint32_t i, std::uint32_t arg2) {
    if (static_cast<int>(i) < 0 || i >= 0x19) return 0;
    std::uint8_t* rec = reinterpret_cast<std::uint8_t*>(0x0068b198 + i * 0x50);
    if (arg2 == 0) { *reinterpret_cast<std::uint32_t*>(rec + 0x20) = 3; *reinterpret_cast<std::uint32_t*>(rec + 0x1c) = 0; }
    else if (*reinterpret_cast<std::uint32_t*>(rec + 0x20) == 3) { *reinterpret_cast<std::uint32_t*>(rec + 0x20) = 1; *reinterpret_cast<std::uint32_t*>(rec + 0x1c) = 0; }
    else { *reinterpret_cast<std::uint32_t*>(rec + 0x1c) = 0; }
    return 1;
}
RH_ScopedInstall(Set458f20, 0x00458f20);

// ===== round 161 ===== (ESI/EDX field-match predicate)
// 0x0047bc90 — u32 fn(ESI=s, EDX=e): a=s[0x10]; c=e[0x10]; if(a==c || a==e[0x14]){
//   b=s[0x14]; return (b==c || b==e[0x14]) ? 1 : 0; } return 0.  (reimpl takes s,e on stack.)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Match47bc90(std::uint8_t* s, std::uint8_t* e) {
    std::uint32_t a = *reinterpret_cast<std::uint32_t*>(s + 0x10);
    std::uint32_t c = *reinterpret_cast<std::uint32_t*>(e + 0x10);
    std::uint32_t e14 = *reinterpret_cast<std::uint32_t*>(e + 0x14);
    if (a == c || a == e14) {
        std::uint32_t b = *reinterpret_cast<std::uint32_t*>(s + 0x14);
        if (b == c || b == e14) return 1;
        return 0;
    }
    return 0;
}
RH_ScopedInstall(Match47bc90, 0x0047bc90);

// ===== round 162 ===== (EDX/EBX/EDI register-arg array search)
// 0x0042ad90 — u32 fn(EDX=arr, EBX=key, EDI=n): if(arr==0) return -1; walk arr[ecx] (term
//   0xff070000); count matches==key in esi; when esi==n at a match, return arr[ecx+1]; else -1.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Find42ad90(std::uint8_t* arr, std::uint32_t key, std::uint32_t n) {
    if (arr == 0) return 0xffffffff;
    std::uint32_t* a = reinterpret_cast<std::uint32_t*>(arr);
    std::uint32_t ecx = 0, esi = 0, eax = a[0];
    if (eax == 0xff070000) return 0xffffffff;
    for (;;) {
        if (eax == key) { if (n == esi) return a[ecx + 1]; esi++; }
        eax = a[ecx + 1]; ecx++;
        if (eax == 0xff070000) break;
    }
    return 0xffffffff;
}
RH_ScopedInstall(Find42ad90, 0x0042ad90);

// ===== round 163 ===== (EBX/EDI register-arg search, arr from globals)
// 0x0042add0 — u32 fn(EBX=key, EDI=n): idx=*0x67e9f8; arr=*(u8**)(idx*0x40 + 0x67ed38);
//   same walk as 0x42ad90 (find (n+1)-th key, return following element; else -1).
extern "C" __declspec(dllexport) std::uint32_t __cdecl Find42add0(std::uint32_t key, std::uint32_t n) {
    std::uint32_t idx = *reinterpret_cast<std::uint32_t*>(0x0067e9f8);
    std::uint8_t* arr = *reinterpret_cast<std::uint8_t**>(idx * 0x40 + 0x0067ed38);
    std::uint32_t* a = reinterpret_cast<std::uint32_t*>(arr);
    std::uint32_t ecx = 0, esi = 0, eax = a[0];
    if (eax == 0xff070000) return 0xffffffff;
    for (;;) {
        if (eax == key) { if (n == esi) return a[ecx + 1]; esi++; }
        eax = a[ecx + 1]; ecx++;
        if (eax == 0xff070000) break;
    }
    return 0xffffffff;
}
RH_ScopedInstall(Find42add0, 0x0042add0);

// ===== round 164 ===== (RainColorInit: strided BGRA-swizzle buffer fill)
// 0x00491070 — void fn(): p = *0x771530 + 0x1d; for(o=0;o<4;o++) for(i=0;i<0xe0;i++){
//   p[-1]=*0x616030; p[0]=*0x616032; p[1]=*0x616031; p[2]=*0x616033; p+=0x20; }
extern "C" __declspec(dllexport) void __cdecl Rain491070(void) {
    std::uint8_t* p = *reinterpret_cast<std::uint8_t**>(0x00771530) + 0x1d;
    for (int o = 0; o < 4; o++) {
        for (int i = 0; i < 0xe0; i++) {
            p[-1] = *reinterpret_cast<std::uint8_t*>(0x00616030);
            p[0]  = *reinterpret_cast<std::uint8_t*>(0x00616032);
            p[1]  = *reinterpret_cast<std::uint8_t*>(0x00616031);
            p[2]  = *reinterpret_cast<std::uint8_t*>(0x00616033);
            p += 0x20;
        }
    }
}
RH_ScopedInstall(Rain491070, 0x00491070);

// ===== round 165 ===== (bitmap first-free-slot allocator)
// 0x00477b60 — u32 fn(): scan bitmap 0x6bf198 for first clear bit idx<0x100; if none return 0;
//   rec=0x693198+idx*0x2c0; rec[0x2b0]=0; rec[0x2b8]=0; rec[0x2b4]=1; rec[0x2bc]=30.0f
//   (0x41f00000); set bit idx in the bitmap; return idx+1.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Alloc477b60(void) {
    int idx = 0;
    while (idx < 0x100) {
        std::uint8_t bit = static_cast<std::uint8_t>(1 << (idx & 7));
        std::uint8_t b = *reinterpret_cast<std::uint8_t*>(0x006bf198 + (idx >> 3));
        if (!(b & bit)) {
            std::uint8_t* rec = reinterpret_cast<std::uint8_t*>(0x00693198 + idx * 0x2c0);
            *reinterpret_cast<std::uint32_t*>(rec + 0x2b0) = 0;
            *reinterpret_cast<std::uint32_t*>(rec + 0x2b8) = 0;
            *reinterpret_cast<std::uint32_t*>(rec + 0x2b4) = 1;
            *reinterpret_cast<std::uint32_t*>(rec + 0x2bc) = 0x41f00000;
            *reinterpret_cast<std::uint8_t*>(0x006bf198 + (idx >> 3)) |= bit;
            return idx + 1;
        }
        idx++;
    }
    return 0;
}
RH_ScopedInstall(Alloc477b60, 0x00477b60);

// ===== round 166 ===== (state dispatch + intrusive-list insert)
// 0x005b0f90 — void fn(p, _, state_src): state=*state_src; sub=p[0x20];
//   if(state==1){ if(sub[0x28]!=3) sub[0x28]=8; } else if(state==3){ if(sub[0x28]!=5) sub[0x28]=4; }
//   sub[0x20]=state; old=p[0x14]; if(old){ *(p[0x18])=old; *(old+4)=p[0x18]; }
//   node=p[0x24]+0xc; nx=*node; p[0x18]=node; p[0x14]=nx; *(nx+4)=&p[0x14]; *node=&p[0x14].
extern "C" __declspec(dllexport) void __cdecl StateInsert5b0f90(std::uint8_t* p, std::uint32_t /*unused*/, std::uint8_t* state_src) {
    std::uint32_t state = *reinterpret_cast<std::uint32_t*>(state_src);
    std::uint8_t* sub = *reinterpret_cast<std::uint8_t**>(p + 0x20);
    if (state == 1) { if (*reinterpret_cast<std::uint32_t*>(sub + 0x28) != 3) *reinterpret_cast<std::uint32_t*>(sub + 0x28) = 8; }
    else if (state == 3) { if (*reinterpret_cast<std::uint32_t*>(sub + 0x28) != 5) *reinterpret_cast<std::uint32_t*>(sub + 0x28) = 4; }
    *reinterpret_cast<std::uint32_t*>(sub + 0x20) = state;
    std::uint8_t* old = *reinterpret_cast<std::uint8_t**>(p + 0x14);
    if (old) {
        *reinterpret_cast<std::uint8_t**>(*reinterpret_cast<std::uint8_t**>(p + 0x18)) = old;
        *reinterpret_cast<std::uint8_t**>(old + 4) = *reinterpret_cast<std::uint8_t**>(p + 0x18);
    }
    std::uint8_t* node = *reinterpret_cast<std::uint8_t**>(p + 0x24) + 0xc;
    std::uint8_t* nx = *reinterpret_cast<std::uint8_t**>(node);
    *reinterpret_cast<std::uint8_t**>(p + 0x18) = node;
    *reinterpret_cast<std::uint8_t**>(p + 0x14) = nx;
    *reinterpret_cast<std::uint8_t**>(nx + 4) = p + 0x14;
    *reinterpret_cast<std::uint8_t**>(node) = p + 0x14;
}
RH_ScopedInstall(StateInsert5b0f90, 0x005b0f90);

// ===== round 167 ===== (multi-deref global setter)
// 0x00476b30 — void fn(p1, p2): val=*(p2?p2:0x613290); g=*0x7dc57c; e=p1[4]; obj=*(e+g);
//   obj[0xc4]=val; n=*(*(e[0x18])[0x20]); if(n) n[4]=val; obj[0x40] |= 0x2000.
extern "C" __declspec(dllexport) void __cdecl Set476b30(std::uint8_t* p1, std::uint8_t* p2) {
    std::uint32_t val = *reinterpret_cast<std::uint32_t*>(p2 ? p2 : reinterpret_cast<std::uint8_t*>(0x00613290));
    std::uint32_t g = *reinterpret_cast<std::uint32_t*>(0x007dc57c);
    std::uint8_t* e = *reinterpret_cast<std::uint8_t**>(p1 + 4);
    std::uint8_t* obj = *reinterpret_cast<std::uint8_t**>(e + g);
    *reinterpret_cast<std::uint32_t*>(obj + 0xc4) = val;
    std::uint8_t* x = *reinterpret_cast<std::uint8_t**>(e + 0x18);
    std::uint8_t* y = *reinterpret_cast<std::uint8_t**>(x + 0x20);
    std::uint8_t* n = *reinterpret_cast<std::uint8_t**>(y);
    if (n != 0) {
        std::uint8_t* obj2 = *reinterpret_cast<std::uint8_t**>(e + g);
        *reinterpret_cast<std::uint32_t*>(n + 4) = *reinterpret_cast<std::uint32_t*>(obj2 + 0xc4);
    }
    std::uint8_t* obj3 = *reinterpret_cast<std::uint8_t**>(e + g);
    *reinterpret_cast<std::uint32_t*>(obj3 + 0x40) |= 0x2000;
}
RH_ScopedInstall(Set476b30, 0x00476b30);

// ===== round 168 ===== (list-node const init: store *arg2 + float consts)
// 0x00482730 — void fn(p, arg2): s=p[0x18]; count=s[0x24]; if(count<=0) return; arr=s[0x20];
//   for(i=0;i<count;i++){ node=arr[i]; node[4]=*arg2; node[0xc]=1.0f; node[0x10]=1.0f; node[0x14]=0.5f; }
extern "C" __declspec(dllexport) void __cdecl Init482730(std::uint8_t* p, std::uint32_t* arg2) {
    std::uint8_t* s = *reinterpret_cast<std::uint8_t**>(p + 0x18);
    int count = *reinterpret_cast<int*>(s + 0x24);
    if (count <= 0) return;
    std::uint8_t** arr = *reinterpret_cast<std::uint8_t***>(s + 0x20);
    std::uint32_t v = *arg2;
    for (int i = 0; i < count; i++) {
        std::uint8_t* node = arr[i];
        *reinterpret_cast<std::uint32_t*>(node + 4) = v;
        *reinterpret_cast<std::uint32_t*>(node + 0xc) = 0x3f800000;
        *reinterpret_cast<std::uint32_t*>(node + 0x10) = 0x3f800000;
        *reinterpret_cast<std::uint32_t*>(node + 0x14) = 0x3f000000;
    }
}
RH_ScopedInstall(Init482730, 0x00482730);

// ===== round 169 ===== (bounded struct-of-arrays push)
// 0x004893d0 — void fn(p, arg2, arg3): top=p[8]; cap=p[4]; if(top>=cap){ p[8]=cap; return; }
//   buf=p[0]; off=top*0x30; rec=buf+off+4; rec[0..8]=arg2 vec3; buf[off+0x1c]=0;
//   buf[off+0..3]=arg3[0..3]; p[8]=top+1; p[0x54]=1.
extern "C" __declspec(dllexport) void __cdecl Push4893d0(std::uint8_t* p, std::uint8_t* arg2, std::uint8_t* arg3) {
    int top = *reinterpret_cast<int*>(p + 8);
    int cap = *reinterpret_cast<int*>(p + 4);
    if (top >= cap) { *reinterpret_cast<int*>(p + 8) = cap; return; }
    std::uint8_t* buf = *reinterpret_cast<std::uint8_t**>(p);
    int off = top * 0x30;
    std::uint8_t* rec = buf + off + 4;
    *reinterpret_cast<std::uint32_t*>(rec) = *reinterpret_cast<std::uint32_t*>(arg2);
    *reinterpret_cast<std::uint32_t*>(rec + 4) = *reinterpret_cast<std::uint32_t*>(arg2 + 4);
    *reinterpret_cast<std::uint32_t*>(rec + 8) = *reinterpret_cast<std::uint32_t*>(arg2 + 8);
    *reinterpret_cast<std::uint32_t*>(buf + off + 0x1c) = 0;
    buf[off + 0] = arg3[0];
    buf[off + 1] = arg3[1];
    buf[off + 2] = arg3[2];
    buf[off + 3] = arg3[3];
    *reinterpret_cast<int*>(p + 8) = top + 1;
    *reinterpret_cast<int*>(p + 0x54) = 1;
}
RH_ScopedInstall(Push4893d0, 0x004893d0);

// ===== round 170 ===== (trie walk; tail-recursion reimplemented as a loop)
// 0x004daa00 — u32 fn(node, key, depth): while(depth!=0){ depth--; idx=key&0xf; key>>=4;
//   node=*(u8**)(node+idx*4+0x1c); } return (node & 0xffffff00) | node[0x18].
//   (orig: mov eax,node; mov al,node[0x18] -> byte in AL, upper 24 bits = node ptr.)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Trie4daa00(std::uint8_t* node, std::uint32_t key, std::uint32_t depth) {
    while (depth != 0) {
        depth--;
        std::uint32_t idx = key & 0xf;
        key >>= 4;
        node = *reinterpret_cast<std::uint8_t**>(node + idx * 4 + 0x1c);
    }
    std::uint32_t r = reinterpret_cast<std::uint32_t>(node);
    return (r & 0xffffff00u) | *reinterpret_cast<std::uint8_t*>(node + 0x18);
}
RH_ScopedInstall(Trie4daa00, 0x004daa00);

// ===== round 171 ===== (struct delta-init + fcomp flag — verbatim naked port)
// 0x00557110 — u32 fn(out,a,b,c,d): out[0x10]=d[0]; out[0x14]=d[4]; out[0x28]=c[0]-d[0];
//   out[0x2c]=c[4]-d[4]; out[0x40]=a[0]; out[0x44]=a[4]; out[0x58]=b[0]-a[0];
//   out[0x5c]=b[4]-a[4]; if any of the 4 deltas is unordered vs [0x5d757c] -> out[0x64]|=2
//   else out[0x64]&=~2; return out. Single fsubs -> verbatim x87 (straight-line, no juggling).
extern "C" __declspec(dllexport) __declspec(naked) std::uint32_t __cdecl Delta557110(void) {
    __asm {
        mov  eax, [esp+0x14]
        mov  ecx, [esp+4]
        mov  edx, [eax]
        mov  [ecx+0x10], edx
        mov  edx, [eax+4]
        mov  [ecx+0x14], edx
        mov  edx, [esp+0x10]
        fld  dword ptr [edx]
        fsub dword ptr [eax]
        fstp dword ptr [ecx+0x28]
        fld  dword ptr [edx+4]
        fsub dword ptr [eax+4]
        mov  eax, [esp+8]
        fstp dword ptr [ecx+0x2c]
        mov  edx, [eax]
        mov  [ecx+0x40], edx
        mov  edx, [eax+4]
        mov  [ecx+0x44], edx
        mov  edx, [esp+0xc]
        fld  dword ptr [edx]
        fsub dword ptr [eax]
        fstp dword ptr [ecx+0x58]
        fld  dword ptr [edx+4]
        fsub dword ptr [eax+4]
        fstp dword ptr [esp+0x14]
        fld  dword ptr [ecx+0x28]
        fcomp dword ptr ds:[05D757Ch]
        mov  eax, [esp+0x14]
        mov  [ecx+0x5c], eax
        fnstsw ax
        test ah, 44h
        jp   L_SET
        fld  dword ptr [ecx+0x2c]
        fcomp dword ptr ds:[05D757Ch]
        fnstsw ax
        test ah, 44h
        jp   L_SET
        fld  dword ptr [ecx+0x58]
        fcomp dword ptr ds:[05D757Ch]
        fnstsw ax
        test ah, 44h
        jp   L_SET
        fld  dword ptr [esp+0x14]
        fcomp dword ptr ds:[05D757Ch]
        fnstsw ax
        test ah, 44h
        jp   L_SET
        mov  eax, [ecx+0x64]
        and  al, 0FDh
        mov  [ecx+0x64], eax
        mov  eax, ecx
        ret
    L_SET:
        mov  eax, [ecx+0x64]
        or   al, 2
        mov  [ecx+0x64], eax
        mov  eax, ecx
        ret
    }
}
RH_ScopedInstall(Delta557110, 0x00557110);

// 0x005b6b00  FUN_005b6b00 (audio backend, integer two-table accumulate+clamp)
// void f(uint32 a1, int32* p2, int32* p3):
//   idx = *p3;
//   d   = ((int16)tblA[0x634498][idx] * (2*(a1&7)+1)) >> 3   (imul then logical shr)
//   *p2 = clamp(*p2 + ((a1&8) ? -d : +d), -32768, 32767)
//   *p3 = clamp(*p3 + (int16)tblB[0x634478][a1],           0,    88)
// tblA/tblB are absolute .rdata signed-word tables in the loaded image; both
// sides read them identically. VERBATIM naked port (exact imul/shr/movsx/clamps).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Accum5b6b00(void)
{
    __asm {
        mov  edx, dword ptr [esp+0x0c]      // edx = p3 (arg3)
        push ebx
        mov  ebx, dword ptr [esp+8]         // ebx = a1 (arg1)
        push esi
        mov  eax, dword ptr [edx]           // eax = *p3 = idx
        mov  ecx, ebx
        and  ecx, 7
        movsx eax, word ptr [eax*2+0634498h]
        lea  ecx, [ecx+ecx+1]               // 2*(a1&7)+1
        imul eax, ecx
        mov  ecx, dword ptr [esp+0x10]      // ecx = p2 (arg2)
        shr  eax, 3                         // logical shift
        mov  esi, dword ptr [ecx]           // esi = *p2
        test bl, 8
        je   L_ADD
        sub  esi, eax
        jmp  L_STORE
    L_ADD:
        add  esi, eax
    L_STORE:
        mov  eax, esi
        mov  dword ptr [ecx], esi
        cmp  eax, 0FFFF8000h
        jge  L_HI
        mov  dword ptr [ecx], 0FFFF8000h
        jmp  L_SECOND
    L_HI:
        cmp  eax, 07FFFh
        jle  L_SECOND
        mov  dword ptr [ecx], 07FFFh
    L_SECOND:
        movsx eax, word ptr [ebx*2+0634478h]
        mov  ecx, dword ptr [edx]           // ecx = *p3
        pop  esi
        add  ecx, eax
        pop  ebx
        mov  dword ptr [edx], ecx
        mov  eax, ecx
        jns  L_HI2
        mov  dword ptr [edx], 0
        ret
    L_HI2:
        cmp  eax, 058h
        jle  L_RET
        mov  dword ptr [edx], 058h
    L_RET:
        ret
    }
}
RH_ScopedInstall(Accum5b6b00, 0x005b6b00);

// 0x00420de0  FUN_00420de0 (vehicle, __fastcall float accumulate + min-clamp)
// void __fastcall f(ECX=idx, EDX=base, [esp+4]=val):
//   st0 = val + base[idx]            (80-bit sum)
//   base[idx] = (float32)st0         (fst keeps st0)
//   if (st0 > 50.0)  base[idx] = 50.0f    (fcomp vs .rdata 0x5cd120 == 50.0f)
// The compare is on the 80-bit sum (fst stored the rounded f32 but st0 retains
// the unrounded value), so a plain-C `float v = ...; if (v>50.f)` would diverge
// at the boundary. Reimpl is a VERBATIM naked x87 port. Exported __cdecl reading
// stack args (idx,base,val) — only the COMPUTATION must match, not the ABI; the
// early_window harness drives the original via a fastcall trampoline.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Accum420de0(void)
{
    __asm {
        mov  edx, dword ptr [esp+8]         // base (arg2)
        mov  ecx, dword ptr [esp+4]         // idx  (arg1)
        fld  dword ptr [esp+0x0c]           // val  (arg3)
        fadd dword ptr [edx+ecx*4]
        fst  dword ptr [edx+ecx*4]
        fcomp dword ptr ds:[05CD120h]
        fnstsw ax
        test ah, 41h
        jne  L_AC_DONE
        mov  dword ptr [edx+ecx*4], 042480000h   // 50.0f
    L_AC_DONE:
        ret
    }
}
RH_ScopedInstall(Accum420de0, 0x00420de0);

// 0x004938e0  FUN_004938e0 (util, linked-list walk-to-self then global-offset write)
// void f(int* p, uint32 value):
//   node = *p;
//   while (node != *node) node = *node;       // walk until a self-pointing node
//   *(uint32*)(*(uint32*)0x911ae4 + node) = value;   // global base + node
// global 0x911ae4 is .bss (zero at process start) so the store lands on the
// terminal node's first dword. Pure integer/pointer; VERBATIM naked port.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Walk4938e0(void)
{
    __asm {
        mov  eax, dword ptr [esp+4]         // p
        mov  ecx, dword ptr [eax]           // *p
    L_WK_LOOP:
        mov  eax, ecx
        mov  ecx, dword ptr [eax]
        cmp  eax, ecx
        jne  L_WK_LOOP
        mov  ecx, dword ptr [esp+8]         // value
        mov  edx, dword ptr ds:[0911AE4h]   // global base
        mov  dword ptr [edx+eax], ecx
        ret
    }
}
RH_ScopedInstall(Walk4938e0, 0x004938e0);

// 0x004223f0  FUN_004223f0 (gameplay, integer noise-hash -> float; register args EAX=a, ECX=b)
// float f(EAX a, ECX b):
//   if (a) b &= a;
//   x = (b<<13) ^ b;
//   h = ((((x*x)*0x3d73 + 0xc0ae5) * x) - 0x2df722f3) & 0x7fffffff;
//   return (float)((double80)fild(h) [+ .rdata 0x5cc94c if h<0, dead] * .rdata 0x5cd314);
// h is always >=0 after the &0x7fffffff so the fadd branch is dead, kept for fidelity.
// Exported __cdecl(a,b)->float reading stack args + a scratch slot; EXACT integer +
// fild/fmul of the same .rdata consts. VERBATIM naked port (no fld-st(N) juggling).
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl Hash4223f0(void)
{
    __asm {
        push ecx                          // scratch slot; now [esp+8]=a, [esp+0x0c]=b
        mov  eax, dword ptr [esp+8]        // a
        mov  ecx, dword ptr [esp+0x0c]     // b
        test eax, eax
        je   L_HS1
        and  ecx, eax
    L_HS1:
        mov  eax, ecx
        shl  eax, 0x0d
        xor  eax, ecx
        mov  ecx, eax
        imul ecx, eax
        imul ecx, ecx, 0x3d73
        add  ecx, 0x0c0ae5
        imul ecx, eax
        sub  ecx, 0x2df722f3
        and  ecx, 0x7fffffff
        test ecx, ecx
        mov  dword ptr [esp], ecx
        fild dword ptr [esp]
        jge  L_HS2
        fadd dword ptr ds:[05CC94Ch]
    L_HS2:
        fmul dword ptr ds:[05CD314h]
        pop  ecx
        ret
    }
}
RH_ScopedInstall(Hash4223f0, 0x004223f0);

// 0x005ae170  FUN_005ae170 (audio, bounded case-insensitive compare; custom inline-ASCII toupper)
// int f(char* s1, char* s2, int n): for up to n chars, fold each to a CUSTOM upper
// then compare. ASYMMETRIC toupper: s1 folds 0x5b-0x60 via &0xdf (cmp 0x41 jl-skip),
// s2 leaves them (cmp 0x41 jg-skip) -> e.g. '`'(0x60): s1->0x40, s2->0x60. Returns 0
// if all n folded chars equal, else toupper(s1[i]) - toupper(s2[i]) (signed). A naive
// symmetric C toupper would diverge on 0x5b-0x60, so this is a VERBATIM naked port.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Cmp5ae170(void)
{
    __asm {
        mov  edx, dword ptr [esp+4]         // s1
        push esi
        mov  esi, dword ptr [esp+0x0c]      // s2
        push edi
        mov  edi, dword ptr [esp+0x14]      // n
    L_CC_LOOP:
        mov  al, byte ptr [edx]
        cmp  al, 0x61
        jge  L_CC_S1Z
        cmp  al, 0x41
        jl   L_CC_S1D
    L_CC_S1Z:
        cmp  al, 0x7a
        jle  L_CC_S1U
        cmp  al, 0x5a
        jg   L_CC_S1D
    L_CC_S1U:
        and  al, 0xdf
    L_CC_S1D:
        mov  cl, al
        mov  al, byte ptr [esi]
        cmp  al, 0x61
        jge  L_CC_S2Z
        cmp  al, 0x41
        jg   L_CC_S2D
    L_CC_S2Z:
        cmp  al, 0x7a
        jle  L_CC_S2U
        cmp  al, 0x5a
        jg   L_CC_S2D
    L_CC_S2U:
        and  al, 0xdf
    L_CC_S2D:
        cmp  cl, al
        jne  L_CC_DIFF
        inc  edx
        inc  esi
        dec  edi
        jne  L_CC_LOOP
        pop  edi
        xor  eax, eax
        pop  esi
        ret
    L_CC_DIFF:
        movsx edx, al
        movsx eax, cl
        pop  edi
        sub  eax, edx
        pop  esi
        ret
    }
}
RH_ScopedInstall(Cmp5ae170, 0x005ae170);

// 0x0048a630  FUN_0048a630 (ai, sphere-vs-AABB overlap predicate)
// int f(box* arg1=EDX, sphere* arg2=ECX):  box = {min.xyz @ 0,4,8; max.xyz @ 0xc,0x10,0x14}
//   sphere = {center.xyz @ 0,4,8; radius @ 0xc}. Block 1: if center strictly inside the
//   box on all 3 axes -> return 1. Else block 2: test the box expanded by radius; return 1
//   if center within, else 0. All straight-line fld/fcomp (NO fld st(N) juggling, no globals)
//   -> VERBATIM naked port is bit-identical.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Aabb48a630(void)
{
    __asm {
        mov  ecx, dword ptr [esp+8]         // arg2 sphere
        mov  edx, dword ptr [esp+4]         // arg1 box
        fld   dword ptr [ecx]
        fcomp dword ptr [edx+0x0c]
        fnstsw ax
        test ah, 1
        jne  L_AB_B2
        fld   dword ptr [ecx]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 0x41
        jp   L_AB_B2
        fld   dword ptr [ecx+4]
        fcomp dword ptr [edx+0x10]
        fnstsw ax
        test ah, 1
        jne  L_AB_B2
        fld   dword ptr [ecx+4]
        fcomp dword ptr [edx+4]
        fnstsw ax
        test ah, 0x41
        jp   L_AB_B2
        fld   dword ptr [ecx+8]
        fcomp dword ptr [edx+0x14]
        fnstsw ax
        test ah, 1
        jne  L_AB_B2
        fld   dword ptr [ecx+8]
        fcomp dword ptr [edx+8]
        fnstsw ax
        test ah, 0x41
        jnp  L_AB_TRUE
    L_AB_B2:
        fld   dword ptr [ecx+0x0c]
        fadd  dword ptr [ecx]
        fcomp dword ptr [edx+0x0c]
        fnstsw ax
        test ah, 1
        jne  L_AB_FALSE
        fld   dword ptr [ecx]
        fsub  dword ptr [ecx+0x0c]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 0x41
        jp   L_AB_FALSE
        fld   dword ptr [ecx+0x0c]
        fadd  dword ptr [ecx+4]
        fcomp dword ptr [edx+0x10]
        fnstsw ax
        test ah, 1
        jne  L_AB_FALSE
        fld   dword ptr [ecx+4]
        fsub  dword ptr [ecx+0x0c]
        fcomp dword ptr [edx+4]
        fnstsw ax
        test ah, 0x41
        jp   L_AB_FALSE
        fld   dword ptr [ecx+0x0c]
        fadd  dword ptr [ecx+8]
        fcomp dword ptr [edx+0x14]
        fnstsw ax
        test ah, 1
        jne  L_AB_FALSE
        fld   dword ptr [ecx+8]
        fsub  dword ptr [ecx+0x0c]
        fcomp dword ptr [edx+8]
        fnstsw ax
        test ah, 0x41
        jp   L_AB_FALSE
    L_AB_TRUE:
        mov  eax, 1
        ret
    L_AB_FALSE:
        xor  eax, eax
        ret
    }
}
RH_ScopedInstall(Aabb48a630, 0x0048a630);

// 0x004c5c00  FUN_004c5c00 (frontend, circular-list case-insensitive string search)
// void* f(list* arg1, char* query): walk the circular list at arg1[8] (sentinel = &arg1[8]);
// each list node has next at node[0] and a key string at node+8 (object = node-8). Compare
// the key vs query case-insensitively (only 'a'-'z' folded via +0xe0 == -0x20 mod 256, both
// sides). Return the object (node-8) on a full match, else 0. Pure pointer/byte walk (no
// globals, no float) -> VERBATIM naked port.
extern "C" __declspec(dllexport) __declspec(naked) void* __cdecl Search4c5c00(void)
{
    __asm {
        mov  eax, dword ptr [esp+4]         // arg1
        push ebx
        add  eax, 8                          // sentinel = &arg1[8]
        push ebp
        push esi
        push edi
        mov  ebx, dword ptr [eax]            // head = arg1[8]
        mov  dword ptr [esp+0x14], eax       // save sentinel (clobbers own arg1 slot)
        cmp  ebx, eax
        je   L_SR_NF
        mov  ebp, dword ptr [esp+0x18]       // query (arg2)
    L_SR_LOOP:
        lea  eax, [ebx-8]                     // object = node-8
        lea  ecx, [eax+0x10]                  // key string = object+0x10 = node+8
        test ecx, ecx
        je   L_SR_NEXT
        mov  esi, ecx
        mov  edi, ebp
        mov  cl, byte ptr [esi]
        test cl, cl
        je   L_SR_END
    L_SR_INNER:
        mov  dl, byte ptr [edi]
        test dl, dl
        je   L_SR_END
        cmp  cl, 0x61
        jl   L_SR_DL
        cmp  cl, 0x7a
        jg   L_SR_DL
        add  cl, 0xe0
    L_SR_DL:
        cmp  dl, 0x61
        jl   L_SR_CMP
        cmp  dl, 0x7a
        jg   L_SR_CMP
        add  dl, 0xe0
    L_SR_CMP:
        cmp  cl, dl
        jne  L_SR_NEXT
        mov  cl, byte ptr [esi+1]
        inc  esi
        inc  edi
        test cl, cl
        jne  L_SR_INNER
    L_SR_END:
        mov  cl, byte ptr [esi]
        mov  dl, byte ptr [edi]
        cmp  cl, dl
        je   L_SR_RET
    L_SR_NEXT:
        mov  ebx, dword ptr [ebx]
        mov  eax, dword ptr [esp+0x14]
        cmp  ebx, eax
        jne  L_SR_LOOP
    L_SR_NF:
        xor  eax, eax
    L_SR_RET:
        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        ret
    }
}
RH_ScopedInstall(Search4c5c00, 0x004c5c00);

// [REVERTED 2026-06-15] round 179 FUN_005172f0 (0x005172f0) is in the statically-linked
// libpng/zlib band (0x516000-0x529fff) -> library-skip per user ruling; demoted C3->C2.
// Was a 4-byte -> printable/[hex] string formatter reimpl.

// 0x00482860  FUN_00482860 (vehicle, pool / circular-freelist initializer)
// void f(pool* arg1): N = arg1[0x16c]; zero a (N+2)*0x24-byte buffer at arg1[0x14];
//   arg1[0x18] = buffer; build a circular freelist of (N+1) nodes (stride 0x24) linked via
//   node[0x1c], head at arg1[0x1c]=buffer, tail at arg1[0x20], tail[0x1c]=buffer (wrap);
//   arg1[0x168]=arg1[0x170]=arg1[0x194]=0; arg1[0x198]=1; arg1[0x1c]=buffer. Pure
//   pointer/integer (rep stosd zero-fill + pointer chain) -> VERBATIM naked port.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Pool482860(void)
{
    __asm {
        mov  edx, dword ptr [esp+4]          // pool
        mov  eax, dword ptr [edx+0x16c]       // N
        add  eax, 2
        lea  ecx, [eax+eax*8]
        shl  ecx, 2                            // (N+2)*0x24 bytes
        push esi
        mov  esi, ecx
        shr  ecx, 2                            // dword count
        push edi
        mov  edi, dword ptr [edx+0x14]         // buffer
        xor  eax, eax
        mov  dword ptr [edx+0x18], edi
        rep  stosd
        mov  ecx, esi
        and  ecx, 3
        rep  stosb
        mov  eax, dword ptr [edx+0x18]
        lea  ecx, [eax+0x24]
        mov  dword ptr [edx+0x1c], eax
        mov  dword ptr [eax+0x1c], ecx
        mov  eax, dword ptr [edx+0x16c]
        xor  esi, esi
        xor  ecx, ecx
        cmp  eax, esi
        jle  L_PL_DONE
    L_PL_LOOP:
        mov  eax, dword ptr [edx+0x1c]
        lea  edi, [eax+0x24]
        mov  dword ptr [eax+0x1c], edi
        mov  eax, dword ptr [edx+0x1c]
        mov  eax, dword ptr [eax+0x1c]
        mov  dword ptr [edx+0x1c], eax
        mov  eax, dword ptr [edx+0x16c]
        inc  ecx
        cmp  ecx, eax
        jl   L_PL_LOOP
    L_PL_DONE:
        mov  eax, dword ptr [edx+0x1c]
        mov  ecx, dword ptr [edx+0x18]
        mov  dword ptr [edx+0x20], eax
        mov  dword ptr [eax+0x1c], ecx
        mov  eax, dword ptr [edx+0x18]
        pop  edi
        mov  dword ptr [edx+0x168], esi
        mov  dword ptr [edx+0x170], esi
        mov  dword ptr [edx+0x194], esi
        mov  dword ptr [edx+0x198], 1
        mov  dword ptr [edx+0x1c], eax
        pop  esi
        ret
    }
}
RH_ScopedInstall(Pool482860, 0x00482860);

// 0x004ceaf0  FUN_004ceaf0 (render, bitmap/texture blit: palette + per-row copy)
// int f(dst* arg1=EAX, src* arg2=EDX):  dst{[4]=channels,[8]=rows,[0xc]=width_bits,
//   [0x10]=dst_stride,[0x14]=dst_pixels,[0x18]=dst_palette}; src{[0xc]=pal_bits,
//   [0x10]=src_stride,[0x14]=src_pixels,[0x18]=src_palette}.
//   Block A: if dst_palette && src_palette && pal_bits<=8 copy (2^pal_bits)*4 bytes.
//   Block B: bpr=((width_bits+7)>>3)*channels; for rows: copy bpr bytes src row->dst row,
//   advancing src by src_stride and dst by dst_stride. Always returns 1. Uses a push-ecx
//   scratch slot + push ebp inside the loop. Pure rep movs (no float/global) -> VERBATIM naked.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Blit4ceaf0(void)
{
    __asm {
        push ecx
        mov  eax, dword ptr [esp+8]          // dst (arg1)
        mov  edx, dword ptr [esp+0x0c]       // src (arg2)
        push ebx
        push esi
        push edi
        mov  edi, dword ptr [eax+0x18]       // dst_palette
        test edi, edi
        je   L_TX_B
        mov  esi, dword ptr [edx+0x18]       // src_palette
        test esi, esi
        je   L_TX_B
        mov  ecx, dword ptr [edx+0x0c]       // pal_bits
        cmp  ecx, 8
        jg   L_TX_B
        mov  ebx, 1
        shl  ebx, cl
        shl  ebx, 2                           // (2^pal_bits)*4
        mov  ecx, ebx
        shr  ecx, 2
        rep  movsd
        mov  ecx, ebx
        and  ecx, 3
        rep  movsb
    L_TX_B:
        mov  ecx, dword ptr [eax+0x0c]       // width_bits
        mov  esi, dword ptr [eax+8]          // rows
        add  ecx, 7
        mov  ebx, dword ptr [edx+0x14]       // src_pixels
        sar  ecx, 3
        imul ecx, dword ptr [eax+4]          // * channels = bytes/row
        mov  edx, dword ptr [eax+0x14]       // dst_pixels
        mov  dword ptr [esp+0x0c], ecx       // save bytes/row (scratch slot)
        test esi, esi
        mov  dword ptr [esp+0x14], 0         // i = 0 (reuse arg1 slot)
        jle  L_TX_END
        push ebp
        jmp  L_TX_BODY
    L_TX_LOOP:
        mov  ecx, dword ptr [esp+0x10]       // bytes/row
    L_TX_BODY:
        mov  ebp, ecx
        mov  esi, ebx                         // src row
        mov  edi, edx                         // dst row
        shr  ecx, 2
        rep  movsd
        mov  ecx, ebp
        and  ecx, 3
        rep  movsb
        mov  ecx, dword ptr [eax+0x10]       // dst_stride
        mov  esi, dword ptr [eax+8]          // rows (reload for cmp)
        add  edx, ecx                         // dst += dst_stride
        mov  ecx, dword ptr [esp+0x1c]       // arg2 (preserved slot)
        mov  ebp, dword ptr [ecx+0x10]       // src_stride
        mov  ecx, dword ptr [esp+0x18]       // i
        add  ebx, ebp                         // src += src_stride
        inc  ecx
        cmp  ecx, esi
        mov  dword ptr [esp+0x18], ecx
        jl   L_TX_LOOP
        pop  ebp
    L_TX_END:
        pop  edi
        pop  esi
        mov  eax, 1
        pop  ebx
        pop  ecx
        ret
    }
}
RH_ScopedInstall(Blit4ceaf0, 0x004ceaf0);

// 0x005b0cf0  FUN_005b0cf0 (audio, record-array filter + conditional update)
// void f(arg1, arg2, arg3, arg4, arg5, arg6): derive rowLo/rowHi from arg3 (==-1 -> [0,arg2[8]),
//   else exact arg3) and colLo/colHi from arg5 (==-1 -> [0,4), else exact arg5). For each of
//   arg2[4] records (stride 0x10 from arg1+0x18; fields A@+0,B@+4,C@+8,D@+0xc): if (A&0x7fffffff)
//   in [rowLo,rowHi) and C in [colLo,colHi) and (B==arg4 || arg4<0): D=arg6 and set A's sign bit.
//   Finally *arg1 |= 0x10. Uses 2 stack locals + push ebp. Pure int/pointer -> VERBATIM naked.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl RecUpd5b0cf0(void)
{
    __asm {
        sub  esp, 8
        push ebx
        push esi
        push edi
        mov  edi, dword ptr [esp+0x1c]       // arg2
        xor  esi, esi
        xor  ebx, ebx
        mov  eax, dword ptr [edi+8]          // arg2[8]
        mov  dword ptr [esp+0x0c], esi       // local_A = 0
        mov  dword ptr [esp+0x1c], eax       // hi_bound = arg2[8]
        mov  eax, dword ptr [esp+0x20]       // arg3
        cmp  eax, -1
        mov  dword ptr [esp+0x10], 4         // local_B = 4
        je   L_RS_ARG5
        mov  dword ptr [esp+0x0c], eax       // local_A = arg3
        inc  eax
        mov  dword ptr [esp+0x1c], eax       // hi_bound = arg3+1
    L_RS_ARG5:
        mov  eax, dword ptr [esp+0x28]       // arg5
        cmp  eax, -1
        je   L_RS_CHK
        mov  ebx, eax                         // ebx = arg5
        inc  eax
        mov  dword ptr [esp+0x10], eax       // local_B = arg5+1
    L_RS_CHK:
        cmp  dword ptr [edi+4], esi          // arg2[4] vs 0
        jbe  L_RS_END
        mov  ecx, dword ptr [esp+0x18]       // arg1
        push ebp
        mov  ebp, dword ptr [esp+0x28]       // arg4
        add  ecx, 0x20
    L_RS_LOOP:
        mov  eax, dword ptr [ecx-8]
        test eax, eax
        jns  L_RS_NONNEG
        and  eax, 0x7fffffff
    L_RS_NONNEG:
        cmp  eax, dword ptr [esp+0x10]       // local_A
        jl   L_RS_NEXT
        cmp  eax, dword ptr [esp+0x20]       // hi_bound
        jge  L_RS_NEXT
        mov  edx, dword ptr [ecx]
        cmp  edx, ebx
        jl   L_RS_NEXT
        cmp  edx, dword ptr [esp+0x14]       // local_B
        jge  L_RS_NEXT
        cmp  dword ptr [ecx-4], ebp          // field_B vs arg4
        je   L_RS_UPD
        test ebp, ebp
        jge  L_RS_NEXT
    L_RS_UPD:
        mov  edx, dword ptr [esp+0x30]       // arg6
        or   eax, 0x80000000
        mov  dword ptr [ecx+4], edx          // field_D = arg6
        mov  dword ptr [ecx-8], eax          // field_A |= sign
    L_RS_NEXT:
        mov  eax, dword ptr [edi+4]          // arg2[4]
        inc  esi
        add  ecx, 0x10
        cmp  esi, eax
        jb   L_RS_LOOP
        pop  ebp
    L_RS_END:
        mov  eax, dword ptr [esp+0x18]       // arg1
        pop  edi
        pop  esi
        pop  ebx
        or   dword ptr [eax], 0x10
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(RecUpd5b0cf0, 0x005b0cf0);

// [REVERTED 2026-06-15] FUN_00520990 (0x00520990) was promoted in round 183 but is a
// libpng/zlib memset wrapper (statically-linked library band 0x516000-0x529fff). Per the
// user's library-skip ruling it is NOT a first-party promotion; demoted back to C2.

// 0x005ae4c0  FUN_005ae4c0 (audio, aligned first-fit block allocator over an embedded list)
// void* f(heap* arg1, uint32 size, uint32 align): walk blocks at arg1[8] via block[0] (which
// doubles as block-end) until arg1[0xc] (sentinel). For each block: free = block[0] - block[8]
// (used) - block - 0xc; if free >= size+0xc, align the cur top (block[8]+block) up to `align`
// (aligned=((cur+align+0x17)&~(align-1))-0xc) and, if it fits before block-end and >= cur+0xc,
// splice a new block there (new.prev=block, new.next=old-next, fix neighbor links), new.used=
// size, return new+0xc. Else next block; none -> 0. Pure int/pointer -> VERBATIM naked port.
extern "C" __declspec(dllexport) __declspec(naked) void* __cdecl Alloc5ae4c0(void)
{
    __asm {
        mov  eax, dword ptr [esp+4]          // heap (arg1)
        mov  ecx, dword ptr [esp+8]          // size (arg2)
        push ebx
        push ebp
        mov  edx, dword ptr [eax+8]          // first block
        mov  ebp, dword ptr [esp+0x14]       // align (arg3)
        push esi
        push edi
        lea  ebx, [ecx+0x0c]                  // size + 0xc
    L_AL_LOOP:
        mov  esi, dword ptr [edx]            // block.next/end
        mov  ecx, dword ptr [edx+8]          // block.used
        mov  eax, esi
        sub  eax, ecx
        sub  eax, edx
        sub  eax, 0x0c                        // free
        cmp  eax, ebx
        jb   L_AL_NEXT
        lea  edi, [ecx+edx]                   // cur top = used + block
        lea  eax, [ebp-1]
        not  eax                              // ~(align-1)
        lea  ecx, [edi+ebp+0x17]
        and  ecx, eax                         // align up
        sub  ecx, 0x0c                        // aligned new-block addr
        lea  eax, [ebx+ecx]
        cmp  eax, esi
        ja   L_AL_NEXT
        add  edi, 0x0c
        cmp  ecx, edi
        jae  L_AL_ALLOC
    L_AL_NEXT:
        mov  ecx, dword ptr [esp+0x14]       // arg1 (heap)
        mov  edx, esi
        cmp  edx, dword ptr [ecx+0x0c]       // next == sentinel?
        jne  L_AL_LOOP
    L_AL_FAIL:
        pop  edi
        pop  esi
        pop  ebp
        xor  eax, eax
        pop  ebx
        ret
    L_AL_ALLOC:
        test ecx, ecx
        je   L_AL_FAIL
        mov  eax, dword ptr [edx]            // old next
        mov  dword ptr [ecx+4], edx          // new.prev = block
        mov  edx, dword ptr [esp+0x14]       // arg1
        mov  dword ptr [ecx], eax            // new.next = old next
        cmp  eax, dword ptr [edx+0x0c]       // old next == sentinel?
        je   L_AL_S1
        mov  dword ptr [eax+4], ecx          // old_next.prev = new
    L_AL_S1:
        mov  eax, dword ptr [ecx+4]          // new.prev (= block)
        mov  esi, dword ptr [edx+0x0c]       // sentinel
        cmp  eax, esi
        je   L_AL_S2
        mov  dword ptr [eax], ecx            // prev.next = new
    L_AL_S2:
        mov  edx, dword ptr [esp+0x18]       // size (arg2)
        pop  edi
        pop  esi
        pop  ebp
        mov  dword ptr [ecx+8], edx          // new.used = size
        lea  eax, [ecx+0x0c]                  // return new+0xc
        pop  ebx
        ret
    }
}
RH_ScopedInstall(Alloc5ae4c0, 0x005ae4c0);

// 0x00458f80  FUN_00458f80 (gameplay, NEAR-LEAF: init loop over a C3 state-setter)
// void f(int arg1): for i in 0..0x18 call Set458f20(i, arg1) (callee 0x00458f20 is C3; it
// writes the absolute record table 0x68b198 stride 0x50). Verbatim naked port; the `call
// rel32` to the callee is reproduced as `mov eax,0x458f20; call eax` (absolute) so the .asi
// reimpl targets the same original callee address.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Init458f80(void)
{
    __asm {
        push esi
        mov  esi, dword ptr [esp+8]          // arg1
        xor  edx, edx
    L_NLI_LOOP:
        push esi
        push edx
        mov  eax, 0458F20h                    // callee 0x00458f20 (absolute)
        call eax
        add  esp, 8
        inc  edx
        cmp  edx, 0x19
        jl   L_NLI_LOOP
        pop  esi
        ret
    }
}
RH_ScopedInstall(Init458f80, 0x00458f80);

// 0x0042c1f0  FUN_0042c1f0 (gameplay, NEAR-LEAF: count nonzero callee results -> all-zero?)
// int f(void): edi=0; for esi=0..5 { if (FUN_00496900(esi)) edi++; } return (edi==0)?1:0.
// Callee 0x00496900 (C3, tail-jmp to 0x497450) returns table[esi*0x200+0x7e96fc]!=0 for esi<=3
// else 0. So this returns 1 iff table[0..3] are all zero. Verbatim naked port; `call rel32`
// to the callee -> `mov eax,0x496900; call eax` (callee unhooked at suspended-spawn).
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Count42c1f0(void)
{
    __asm {
        push esi
        push edi
        xor  edi, edi
        xor  esi, esi
    L_NLC_LOOP:
        push esi
        mov  eax, 0496900h                    // callee 0x00496900 (absolute)
        call eax
        add  esp, 4
        test eax, eax
        je   L_NLC_SKIP
        inc  edi
    L_NLC_SKIP:
        inc  esi
        cmp  esi, 6
        jl   L_NLC_LOOP
        xor  eax, eax
        test edi, edi
        pop  edi
        sete al
        pop  esi
        ret
    }
}
RH_ScopedInstall(Count42c1f0, 0x0042c1f0);

// 0x005aa1e0  FUN_005aa1e0 (audio, NEAR-LEAF: logical-NOT of a 16-byte memcmp)
// int f(arg1_unused, arg2, arg3): a=*arg2, b=arg3; return !FUN_005adf30(a,b). Callee 0x5adf30
// (C3) returns 0 if a==b or the 16 bytes at a/b match, else +/-1. So this returns 1 iff the
// 16 bytes at *arg2 and arg3 are equal. Verbatim naked port; `call rel32`->`mov eax,abs;call eax`.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Cmp5aa1e0(void)
{
    __asm {
        mov  ecx, dword ptr [esp+8]          // arg2 (ptr-to-ptr)
        mov  eax, dword ptr [esp+0x0c]       // arg3 (ptr)
        push eax
        mov  edx, dword ptr [ecx]            // *arg2
        push edx
        mov  eax, 05ADF30h                    // callee 0x005adf30 (absolute)
        call eax
        add  esp, 8
        neg  eax
        sbb  eax, eax
        inc  eax
        ret
    }
}
RH_ScopedInstall(Cmp5aa1e0, 0x005aa1e0);

// 0x0041a9b0  FUN_0041a9b0 (vehicle, NEAR-LEAF: write arg[0..1] into an abs table via a C3 setter)
// void f(int* arg): for i=0,1 call GhostSlotSet63c6f0(i, arg[i]) (callee 0x41a8b0 C3 writes
// *(int*)(i*0xc4 + 0x63c6f0) = value). Verbatim naked port; `call rel32`->`mov eax,abs;call eax`.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Ghost41a9b0(void)
{
    __asm {
        push esi
        push edi
        mov  edi, dword ptr [esp+0x0c]       // arg (int*)
        xor  esi, esi
    L_NLA_LOOP:
        mov  eax, dword ptr [edi+esi*4]
        push eax
        push esi
        mov  eax, 041A8B0h                    // callee 0x0041a8b0 (absolute)
        call eax
        add  esp, 8
        inc  esi
        cmp  esi, 2
        jl   L_NLA_LOOP
        pop  edi
        pop  esi
        ret
    }
}
RH_ScopedInstall(Ghost41a9b0, 0x0041a9b0);

// 0x00408590  FUN_00408590 (gameplay, NEAR-LEAF: plane dot-product + counter)
// void f(arg1, arg2): idx=arg1[0x20]; rec=FUN_00426cc0(idx) (C3 pure: idx*0x4c+0x663658);
// dp = arg2[0x20]*rec[0] + arg2[0x24]*rec[4] + arg2[0x28]*rec[8]; arg1[0xac]=dp;
// if (dp < [0x5d757c]) arg1[0xa8]++ else arg1[0xa8]=0. Straight x87 (no st(N) juggling).
// Verbatim naked port; the 3 `call rel32` to the pure callee -> `mov eax,0x426cc0; call eax`.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Dot408590(void)
{
    __asm {
        push esi
        mov  esi, dword ptr [esp+8]          // arg1
        mov  eax, dword ptr [esi+0x20]       // idx
        push edi
        push eax
        mov  eax, 0426CC0h
        call eax
        fld  dword ptr [eax+4]
        mov  edi, dword ptr [esp+0x14]       // arg2
        mov  ecx, dword ptr [esi+0x20]
        fmul dword ptr [edi+0x24]
        push ecx
        fstp dword ptr [esp+0x14]
        mov  eax, 0426CC0h
        call eax
        fld  dword ptr [edi+0x20]
        mov  edx, dword ptr [esi+0x20]
        fmul dword ptr [eax]
        push edx
        fadd dword ptr [esp+0x18]
        fstp dword ptr [esp+0x18]
        mov  eax, 0426CC0h
        call eax
        fld  dword ptr [eax+8]
        fmul dword ptr [edi+0x28]
        add  esp, 0x0c
        fadd dword ptr [esp+0x0c]
        fst  dword ptr [esi+0xac]
        fcomp dword ptr ds:[05D757Ch]
        fnstsw ax
        test ah, 5
        jp   L_DOT_RESET
        mov  eax, dword ptr [esi+0xa8]
        inc  eax
        pop  edi
        mov  dword ptr [esi+0xa8], eax
        pop  esi
        ret
    L_DOT_RESET:
        pop  edi
        mov  dword ptr [esi+0xa8], 0
        pop  esi
        ret
    }
}
RH_ScopedInstall(Dot408590, 0x00408590);

// 0x00413fa0  FUN_00413fa0 (hud, NEAR-LEAF: combine two pure global getters)
// int f(void): a=FUN_00430790()(=*0x67f17c); s=a*3; b=FUN_0042f6a0()(=*0x67e9fc);
//   return s + (b==4 ? 1 : b==5 ? 2 : 0). Both callees C3 pure global getters. Verbatim
//   naked port; `call rel32`->`mov eax,abs;call eax`.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Calc413fa0(void)
{
    __asm {
        push esi
        mov  eax, 0430790h
        call eax
        lea  esi, [eax+eax*2]
        mov  eax, 042F6A0h
        call eax
        xor  ecx, ecx
        cmp  eax, 4
        jne  L_C4_1
        mov  eax, 1
        add  eax, esi
        pop  esi
        ret
    L_C4_1:
        cmp  eax, 5
        jne  L_C4_2
        mov  eax, 2
        add  eax, esi
        pop  esi
        ret
    L_C4_2:
        lea  eax, [ecx+esi]
        pop  esi
        ret
    }
}
RH_ScopedInstall(Calc413fa0, 0x00413fa0);

// [SKIPPED 2026-06-15] FUN_0049a730 (0x0049a730) is a clean near-leaf (!FUN_00426c00()) but
// has NO static caller (no direct call, no address-as-data ref) -> cannot satisfy the
// caller-C2+ gate, so NOT promoted (left C2).

// 0x0040b6e0  FUN_0040b6e0 (gameplay, NEAR-LEAF: conditional global accumulate)
// void f(int arg): if (FUN_0042f6a0()(=GetRaceSubMode,*0x67e9fc) != 2) *(int*)0x63b8ec += arg.
// Callee C3 pure getter. Verbatim naked port; `call rel32`->`mov eax,abs;call eax`.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Add40b6e0(void)
{
    __asm {
        mov  eax, 042F6A0h
        call eax
        cmp  eax, 2
        je   L_ADD_END
        mov  ecx, dword ptr ds:[063B8ECh]
        mov  eax, dword ptr [esp+4]
        add  ecx, eax
        mov  dword ptr ds:[063B8ECh], ecx
    L_ADD_END:
        ret
    }
}
RH_ScopedInstall(Add40b6e0, 0x0040b6e0);

// 0x0045c7f0  FUN_0045c7f0 (gameplay, NEAR-LEAF: getter-compare then tail-call getter)
// int f(void): if (GetRenderSubMode()(=*0x63ba8c) == *0x88fbc0) return that; else tail-call
// Flag63b908Get()(=*0x63b908). Both callees C3 pure getters. Verbatim naked port; `call rel32`
// -> `mov eax,abs;call eax`; tail `jmp rel32` -> `mov eax,abs; jmp eax`.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Sub45c7f0(void)
{
    __asm {
        mov  eax, 040E350h
        call eax
        cmp  eax, dword ptr ds:[088FBC0h]
        je   L_SC7_RET
        mov  eax, 040E450h
        jmp  eax
    L_SC7_RET:
        ret
    }
}
RH_ScopedInstall(Sub45c7f0, 0x0045c7f0);

// 0x00430250  FUN_00430250 (frontend, NEAR-LEAF: pointer-array search via C3 getter)
// int f(int key, int gate): if(gate==0) return 0; arr=FUN_0040d430()(=*0x5f2770, 0xd ptrs);
//   for i in 0..0xc: if(*arr[i]==key) return i*0x14+0x7f0cb0; return 0. Callee C3 getter.
//   Verbatim naked port; `call rel32`->`mov eax,abs;call eax`.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Search430250(void)
{
    __asm {
        mov  eax, dword ptr [esp+8]          // gate
        test eax, eax
        jne  L_SR2_GO
        xor  eax, eax
        ret
    L_SR2_GO:
        mov  eax, 040D430h
        call eax
        mov  edx, dword ptr [esp+4]          // key
        push esi
        xor  ecx, ecx
    L_SR2_LOOP:
        mov  esi, dword ptr [eax+ecx*4]
        cmp  dword ptr [esi], edx
        je   L_SR2_FOUND
        inc  ecx
        cmp  ecx, 0x0d
        jl   L_SR2_LOOP
    L_SR2_FOUND:
        cmp  ecx, 0x0d
        pop  esi
        je   L_SR2_NF
        lea  eax, [ecx+ecx*4]
        lea  eax, [eax*4+07F0CB0h]
        ret
    L_SR2_NF:
        xor  eax, eax
        ret
    }
}
RH_ScopedInstall(Search430250, 0x00430250);

// 0x00412130  FUN_00412130 (gameplay, NEAR-LEAF: state->value map via GetRaceSubMode + jump table)
// int f(void): sub = GetRaceSubMode_0x42f6a0() (=*0x67e9fc); if(sub==2) return 3; if(sub<2 ||
//   sub>5) return 0; t = *0x7f0fd0 - 4; if((unsigned)t>6) return 0; return jt[t] where the
//   original's jump table @0x41216c maps {1,0,0,0,1,1,6}. Callee C3 pure getter. Clean C reimpl
//   (calls the original getter via fn-ptr; both sides hit the real getter at suspended-spawn).
extern "C" __declspec(dllexport) int __cdecl Calc412130(void)
{
    int sub = ((int(__cdecl*)(void))0x0042f6a0)();
    if (sub == 2) return 3;
    if (sub < 2 || sub > 5) return 0;
    unsigned t = *reinterpret_cast<unsigned*>(0x007f0fd0) - 4u;
    if (t > 6u) return 0;
    static const int jt[7] = { 1, 0, 0, 0, 1, 1, 6 };
    return jt[t];
}
RH_ScopedInstall(Calc412130, 0x00412130);

// 0x00429b30  FUN_00429b30 (gameplay, NEAR-LEAF: one-shot float init via two C3 getters)
// void f(void): if (GetRaceSubMode()(*0x67e9fc)==2) return; if (*0x8991b0 != 0) return;
//   v=(short)FUN_0042b8c0()(*0x67ea56); *0x8991b0=0x3f; *0x8991b4 = -([0x5cd6c8] / (float)v).
// Both callees C3 pure getters. Straight x87 (fild/fdivr/fchs). Verbatim naked port; calls ->
// `mov eax,abs; call eax`.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Init429b30(void)
{
    __asm {
        push ecx
        mov  eax, 042F6A0h
        call eax
        cmp  eax, 2
        je   L_IN9_END
        mov  eax, dword ptr ds:[08991B0h]
        test eax, eax
        jne  L_IN9_END
        mov  eax, 042B8C0h
        call eax
        movsx eax, ax
        mov  dword ptr [esp], eax
        mov  dword ptr ds:[08991B0h], 03Fh
        fild dword ptr [esp]
        fdivr dword ptr ds:[05CD6C8h]
        fchs
        fstp dword ptr ds:[08991B4h]
    L_IN9_END:
        pop  ecx
        ret
    }
}
RH_ScopedInstall(Init429b30, 0x00429b30);

// 0x00429b70  FUN_00429b70 (frontend, NEAR-LEAF: sibling of 0x429b30, submode in {3,4,5} gate)
// void f(void): if (GetRaceSubMode() in {3,4,5} && *0x8991b0==0) { v=(short)FUN_0042b8c0();
//   *0x8991b0=0xeb; *0x8991b8=0; *0x8991b4 = -([0x5cd6c8] / (float)v); }. Both callees C3
//   getters. Straight x87. Verbatim naked port; calls -> `mov eax,abs; call eax`.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Init429b70(void)
{
    __asm {
        push ecx
        mov  eax, 042F6A0h
        call eax
        cmp  eax, 3
        je   L_IN7_C
        mov  eax, 042F6A0h
        call eax
        cmp  eax, 4
        je   L_IN7_C
        mov  eax, 042F6A0h
        call eax
        cmp  eax, 5
        jne  L_IN7_END
    L_IN7_C:
        mov  eax, dword ptr ds:[08991B0h]
        test eax, eax
        jne  L_IN7_END
        mov  eax, 042B8C0h
        call eax
        movsx eax, ax
        mov  dword ptr [esp], eax
        mov  dword ptr ds:[08991B0h], 0EBh
        mov  dword ptr ds:[08991B8h], 0
        fild dword ptr [esp]
        fdivr dword ptr ds:[05CD6C8h]
        fchs
        fstp dword ptr ds:[08991B4h]
    L_IN7_END:
        pop  ecx
        ret
    }
}
RH_ScopedInstall(Init429b70, 0x00429b70);

// 0x00472560  FUN_00472560 (gameplay, NEAR-LEAF: record + table builder via C3 setter)
// void f(_, arg2, _, arg4): rec=arg2*0x10+0x691500; flag=(arg4==0)?1:0; for esi=0..3:
//   FUN_0046be10(rec, esi, arg2, flag); FUN_0046be10(rec+4, esi, arg2+0xa, arg4); rec[8]=arg4.
// Callee 0x46be10 C3 writes *(0x88219c + (idx*0x341+base)*4) = *p1. Verbatim naked port; the
// two `call rel32` -> `mov eax,0x46be10; call eax`.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Build472560(void)
{
    __asm {
        push ebx
        mov  ebx, dword ptr [esp+8]          // arg2
        push ebp
        mov  ebp, dword ptr [esp+0x10]       // arg4
        push esi
        push edi
        mov  edi, ebx
        shl  edi, 4
        xor  eax, eax
        xor  esi, esi
        add  edi, 0x691500
        test ebp, ebp
        sete al
        mov  dword ptr [esp+0x14], eax
    L_BLD_LOOP:
        mov  ecx, dword ptr [esp+0x14]
        push ecx
        push ebx
        push esi
        push edi
        mov  eax, 046BE10h
        call eax
        push ebp
        lea  eax, [ebx+0x0a]
        push eax
        lea  eax, [edi+4]
        push esi
        push eax
        mov  eax, 046BE10h
        call eax
        add  esp, 0x20
        inc  esi
        cmp  esi, 4
        jl   L_BLD_LOOP
        mov  dword ptr [edi+8], ebp
        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        ret
    }
}
RH_ScopedInstall(Build472560, 0x00472560);

// 0x004725f0  FUN_004725f0 (gameplay, NEAR-LEAF: no-arg record-range initializer)
// void f(void): for edi=0..9 (esi=0x69150c; esi+=0x10): FUN_0046be10(esi-0xc, edi, edi, 0);
//   FUN_0046be10(esi-8, edi, edi+0xa, 0); [esi-4]=0; [esi]=0. Callee 0x46be10 C3 table setter
//   (writes *(0x88219c+(idx*0x341+base)*4)=*p1). Verbatim naked port; calls -> mov eax,abs;call eax.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl InitRange4725f0(void)
{
    __asm {
        push esi
        push edi
        xor  edi, edi
        mov  esi, 069150Ch
    L_IRG_LOOP:
        push 0
        push edi
        lea  eax, [esi-0x0c]
        push edi
        push eax
        mov  dword ptr [esi-4], 0
        mov  dword ptr [esi], 0
        mov  eax, 046BE10h
        call eax
        push 0
        lea  ecx, [edi+0x0a]
        push ecx
        lea  edx, [esi-8]
        push edi
        push edx
        mov  eax, 046BE10h
        call eax
        add  esp, 0x20
        add  esi, 0x10
        inc  edi
        cmp  esi, 06915ACh
        jl   L_IRG_LOOP
        pop  edi
        pop  esi
        ret
    }
}
RH_ScopedInstall(InitRange4725f0, 0x004725f0);

// 0x004215c0  FUN_004215c0 (frontend, NEAR-LEAF: fastcall float-accumulate wrapper)
// void f(arg1, float arg2, arg3): base = 0x63e4b8 + arg1*0x24; calls __fastcall FUN_00420de0
// (ecx=arg3 idx, edx=base, [esp+4]=arg2 val) which does base[arg3] = min(arg2+base[arg3], 50.0).
// Callee C3. Verbatim naked port; `call rel32` -> `mov eax,0x420de0; call eax` (callee is
// __fastcall: ecx/edx set before the call exactly as the original).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Accum4215c0(void)
{
    __asm {
        mov  eax, dword ptr [esp+8]          // arg2 (val)
        mov  ecx, dword ptr [esp+0x0c]       // arg3 (idx) -> ecx for fastcall
        push eax
        mov  eax, dword ptr [esp+8]          // arg1
        lea  edx, [eax+eax*8]
        lea  edx, [edx*4+063E4B8h]           // base = 0x63e4b8 + arg1*0x24
        mov  eax, 0420DE0h
        call eax
        pop  ecx
        ret
    }
}
RH_ScopedInstall(Accum4215c0, 0x004215c0);

// 0x0041b520  FUN_0041b520 (gameplay, NEAR-LEAF: per-record update loop over 4 records)
// void f(void): for (ecx=0x63c8d0; ecx<0x63caa0; ecx+=0x74) FUN_0041ae20(ecx). Callee 0x41ae20
// (__fastcall, C3, preserves ecx) copies table[0x5f3304+rec[0x64]*0xc] vec3 -> rec[0x50..0x58],
// doubles rec[0x50] (float), zeros rec[0x70]. Verbatim naked port; `call rel32`->`mov eax,abs;call eax`.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Update41b520(void)
{
    __asm {
        mov  ecx, 063C8D0h
    L_UPD_LOOP:
        mov  eax, 041AE20h
        call eax
        add  ecx, 0x74
        cmp  ecx, 063CAA0h
        jl   L_UPD_LOOP
        ret
    }
}
RH_ScopedInstall(Update41b520, 0x0041b520);

// 0x00462760  FUN_00462760 (gameplay, NEAR-LEAF: predicate over a 4-entry pointer array)
// int f(void): for each p in array 0x69045c[0..3]: if (p==0 || GetD70(p)==0 || GetD70(p)==2 ||
//   GetD6c(p)==2) return 1; return 0. Callees 0x5a89b0(=p[0xd70]), 0x5a89a0(=p[0xd6c]) C3 getters.
// Verbatim naked port; `call rel32` -> `mov eax,abs; call eax`.
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Pred462760(void)
{
    __asm {
        push esi
        mov  esi, 069045Ch
    L_PRD_LOOP:
        mov  eax, dword ptr [esi]
        test eax, eax
        je   L_PRD_RET1
        push eax
        mov  eax, 05A89B0h
        call eax
        add  esp, 4
        test eax, eax
        je   L_PRD_RET1
        mov  eax, dword ptr [esi]
        push eax
        mov  eax, 05A89B0h
        call eax
        add  esp, 4
        cmp  eax, 2
        je   L_PRD_RET1
        mov  ecx, dword ptr [esi]
        push ecx
        mov  eax, 05A89A0h
        call eax
        add  esp, 4
        cmp  eax, 2
        je   L_PRD_RET1
        add  esi, 4
        cmp  esi, 069046Ch
        jl   L_PRD_LOOP
        xor  eax, eax
        pop  esi
        ret
    L_PRD_RET1:
        mov  eax, 1
        pop  esi
        ret
    }
}
RH_ScopedInstall(Pred462760, 0x00462760);

// 0x0040bb30  FUN_0040bb30 (boot, NEAR-LEAF: global-list string-search wrapper)
// void* f(query): return FUN_004c5c00(*0x63b8f8, query). Callee 0x4c5c00 (C3) = circular-list
// case-insensitive search -> matched object or 0. Verbatim naked port; call -> mov eax,abs;call eax.
extern "C" __declspec(dllexport) __declspec(naked) void* __cdecl Search40bb30(void)
{
    __asm {
        mov  eax, dword ptr [esp+4]          // query
        mov  ecx, dword ptr ds:[063B8F8h]    // list = *0x63b8f8
        push eax
        push ecx
        mov  eax, 04C5C00h
        call eax
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(Search40bb30, 0x0040bb30);
