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
