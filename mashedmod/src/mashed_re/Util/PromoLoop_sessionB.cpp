// Mashed RE — promote-round session B (rounds 79-88), consolidated into one TU to stay
// under cmd.exe's ~8191-char command-line limit for the .asi cl invocation.
// Each function keeps its original RVA + byte-verification comment + RH_ScopedInstall.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include "../Powerup/PowerupTypeSearch.h"

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



// MATCH-S16: the body of 0x00495520 (Get771e78) moved to Match/MatchS16_00495520.cpp,
// which compiles the byte-matched source re/match/s16/00495520.c.

// MATCH-S14: the body of 0x004842b0 (Clear4842b0) moved to Match/MatchS14_004842b0.cpp,
// which compiles the byte-matched source re/match/s14/004842b0.c.

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



// MATCH-S06: the body of 0x0042a9f0 (GetFadeAlpha) moved to Match/MatchS06_0042a9f0.cpp,
// which compiles the byte-matched source re/match/s06/0042a9f0.c.

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



// MATCH-S11: the body of 0x0046dd90 (VehField8816f4Set) moved to Match/MatchS11_0046dd90.cpp,
// which compiles the byte-matched source re/match/s11/0046dd90.c.

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

// MATCH-S12: the body of 0x00477e00 (Table69318cSet) moved to Match/MatchS12_00477e00.cpp,
// which compiles the byte-matched source re/match/s12/00477e00.c.

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

// MATCH-S10: the body of 0x0045bff0 (Pred45bff0) moved to Match/MatchS10_0045bff0.cpp,
// which compiles the byte-matched source re/match/s10/0045bff0.c.

// MATCH-PORTSTATE-GAMEFLOW-FU: the body of 0x00497450 (Pred497450) moved to Match/MatchGFFU_00497450.cpp
// (byte-matched re/match/gffu/00497450.c).

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

// MATCH-S15: the body of 0x004924c0 (Set6147b4Triple) moved to Match/MatchS15_004924c0.cpp,
// which compiles the byte-matched source re/match/s15/004924c0.c.

// MATCH-S13: the body of 0x0047cdc0 (StoreDistSq47cdc0) moved to Match/MatchS13_0047cdc0.cpp,
// which compiles the byte-matched source re/match/s13/0047cdc0.c.

// ===== round 87 =====
// Mashed RE — promote-round round 87 (double-deref vec3 getter + gated float-compare predicate).
//
// Display-independent; early_window_leaf_diff.py (NEW double_deref_vec3_get + global_float_predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E



// MATCH-S08: the body of 0x0044dff0 (ItemWorldPos44dff0) moved to Match/MatchS08_0044dff0.cpp,
// which compiles the byte-matched source re/match/s08/0044dff0.c.

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

// [moved] 0x004576b0 AnyActive4576b0 moved to Gameplay/PickupPredicates.cpp (D4-DC-INC-batch3, 2026-11-01): one body, both targets.

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

// MATCH-S10: the body of 0x0045c4e0 (ArgSearch45c4e0) moved to Match/MatchS10_0045c4e0.cpp,
// which compiles the byte-matched source re/match/s10/0045c4e0.c.

// MATCH-S10: the body of 0x0045df70 (FloatStep45df70) moved to Match/MatchS10_0045df70.cpp,
// which compiles the byte-matched source re/match/s10/0045df70.c.

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

// MATCH-S04: the body of 0x0041ee50 (Flag41ee50) moved to Match/MatchS04_0041ee50.cpp,
// which compiles the byte-matched source re/match/s04/0041ee50.c.

// MATCH-S11: the body of 0x00471530 (ClearTable471530) moved to Match/MatchS11_00471530.cpp,
// which compiles the byte-matched source re/match/s11/00471530.c.

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

// D4-DC-RFX-CORE: the Copy476a80 body moved to Render/RfxEmitter.cpp (one body for both targets).

// MATCH-S11: the body of 0x00474d80 (ByteFlag474d80) moved to Match/MatchS11_00474d80.cpp,
// which compiles the byte-matched source re/match/s11/00474d80.c.

// MATCH-S04: the body of 0x0041f0d0 (MaskGet41f0d0) moved to Match/MatchS04_0041f0d0.cpp,
// which compiles the byte-matched source re/match/s04/0041f0d0.c.

// ===== round 95 =====

// MATCH-S13: the body of 0x0047c0b0 (ClearColl47c0b0) moved to Match/MatchS13_0047c0b0.cpp,
// which compiles the byte-matched source re/match/s13/0047c0b0.c.

// D4-DC-RFX-CORE: the Batch476ae0 / Batch476b90 / Batch476be0 bodies moved to Render/RfxEmitter.cpp.

// ===== round 96 (more per-batch state setters) =====

// D4-DC-RFX-CORE: the Batch476c10 / Batch476c60 / Batch476cb0 bodies moved to Render/RfxEmitter.cpp.

// ===== round 97 =====

// MATCH-S13: the body of 0x0047ce40 (Search47ce40) moved to Match/MatchS13_0047ce40.cpp,
// which compiles the byte-matched source re/match/s13/0047ce40.c.

// MATCH-S15: the body of 0x0048f260 (Clear48f260) moved to Match/MatchS15_0048f260.cpp,
// which compiles the byte-matched source re/match/s15/0048f260.c.

// MATCH-S15: the body of 0x00491bd0 (ClearVB491bd0) moved to Match/MatchS15_00491bd0.cpp,
// which compiles the byte-matched source re/match/s15/00491bd0.c.

// ===== round 98 =====

// MATCH-S16: the body of 0x00497000 (Proj497000) moved to Match/MatchS16_00497000.cpp,
// which compiles the byte-matched source re/match/s16/00497000.c.

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
extern "C" __declspec(dllexport) void __cdecl Tbl4a2bf7(void);
static void __cdecl Tbl4a2bf7_impl(void) {
    *reinterpret_cast<std::uint32_t*>(0x006160d8u) = 0x004a5d92u;
    *reinterpret_cast<std::uint32_t*>(0x006160dcu) = 0x004a5a0cu;
    *reinterpret_cast<std::uint32_t*>(0x006160e0u) = 0x004a5a71u;
    *reinterpret_cast<std::uint32_t*>(0x006160e4u) = 0x004a59d0u;
    *reinterpret_cast<std::uint32_t*>(0x006160e8u) = 0x004a5a57u;
    *reinterpret_cast<std::uint32_t*>(0x006160ecu) = 0x004a5d92u;
}
// --- 0x004a2bf7 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
// LANE 3: page-level write tracking (Core/ShadowTrack.h) -- no region spec; every page
// the call writes plus the caller's 4 KB stack window is compared, pre-state restored
// between the two runs. Verifies EFFECTS; read the pages= detail on a DIVERGENT row.
extern "C" __declspec(dllexport) void __cdecl Tbl4a2bf7(void) {
    SHADOW_AB_COUNTER(ab, "Tbl4a2bf7", 0x004a2bf7u, ShadowAB::kPhaseRace);
    ShadowAB::RunTracked(ab, Tbl4a2bf7_impl, SHADOW_STACK_WINDOW());
}
RH_ScopedInstall(Tbl4a2bf7, 0x004a2bf7);

// MATCH-S10: the body of 0x0045c810 (Inc45c810) moved to Match/MatchS10_0045c810.cpp,
// which compiles the byte-matched source re/match/s10/0045c810.c.

// ===== round 102 =====
// indexed_global_field_read family: return *(*(base_glob) + *(idx_glob) + off)

// MATCH-S19: the body of 0x004c5850 (Get4c5850) moved to Match/MatchS19_004c5850.cpp,
// which compiles the byte-matched source re/match/s19/004c5850.c.

// MATCH-S19: the body of 0x004c5ca0 (Get4c5ca0) moved to Match/MatchS19_004c5ca0.cpp,
// which compiles the byte-matched source re/match/s19/004c5ca0.c.

// indexed_global_field_write family: *(*(base_glob)+*(idx_glob)+off)=v

// MATCH-S19: the body of 0x004c5830 (Set4c5830) moved to Match/MatchS19_004c5830.cpp,
// which compiles the byte-matched source re/match/s19/004c5830.c.

// MATCH-S19: the body of 0x004c5c80 (Set4c5c80) moved to Match/MatchS19_004c5c80.cpp,
// which compiles the byte-matched source re/match/s19/004c5c80.c.

// ===== round 103 =====
// thiscall_struct_from_table family: __thiscall(this) reads global table indexed by
// this[idx_off], derives into this fields. Declared __fastcall (this in ECX) so the
// ABI matches the original for both early_window (Frida thiscall) and auto-install.
// Export resolves under the decorated name @Name@4 (set in hooks_registry).

// MATCH-S04: the body of 0x0041b770 (Copy41b770) moved to Match/MatchS04_0041b770.cpp,
// which compiles the byte-matched source re/match/s04/0041b770.c.

// [MOVED 2026-10-09, D-11073 leg 1a] Copy41ae20 (FUN_0041ae20) and Update41b520
// (FUN_0041b520) -> Race/SubStateCallees.cpp (built into both targets).

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
// [moved] 0x00484a50 Insert484a50 moved to Race/WorldObjectLinks.cpp (D4-DC-INC-batch3, 2026-11-01): one body, both targets.

// ===== round 107 =====
// ECX-input (thiscall) const-field setters — verbatim naked-asm ports.

// MATCH-S16: the body of 0x004944b0 (Zero4944b0) moved to Match/MatchS16_004944b0.cpp,
// which compiles the byte-matched source re/match/s16/004944b0.c.

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

// MATCH-S04: the body of 0x0041b720 (Bits41b720) moved to Match/MatchS04_0041b720.cpp,
// which compiles the byte-matched source re/match/s04/0041b720.c.

// MATCH-S04: the body of 0x0041cdb0 (Bits41cdb0) moved to Match/MatchS04_0041cdb0.cpp,
// which compiles the byte-matched source re/match/s04/0041cdb0.c.

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

// MATCH-S12: the body of 0x00478cb0 (Memset478cb0) moved to Match/MatchS12_00478cb0.cpp,
// which compiles the byte-matched source re/match/s12/00478cb0.c.

// 0x004b65c0  render — byte-verified: void fn(dst): if(dst) rep movsd 0x82b dwords
//   from 0x8ab7e0 to dst
extern "C" __declspec(dllexport) void __cdecl Copy4b65c0(std::uint32_t* dst) {
    if (dst) {
        const std::uint32_t* s = reinterpret_cast<const std::uint32_t*>(0x008ab7e0u);
        for (unsigned i = 0; i < 0x82bu; i++) dst[i] = s[i];
    }
}
RH_ScopedInstall(Copy4b65c0, 0x004b65c0);

// MATCH-S08: the body of 0x00449880 (Fill449880) moved to Match/MatchS08_00449880.cpp,
// which compiles the byte-matched source re/match/s08/00449880.c.

// MATCH-S06: the body of 0x0042ac50 (Calc42ac50) moved to Match/MatchS06_0042ac50.cpp,
// which compiles the byte-matched source re/match/s06/0042ac50.c.

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

// MATCH-S19: the body of 0x004c45f0 (And4c45f0) moved to Match/MatchS19_004c45f0.cpp,
// which compiles the byte-matched source re/match/s19/004c45f0.c.

// 0x004b52c0 — void fn(p,val,flag): e=*(u32*)(p+8); if(flag) e|=val; *(u32*)(p+8)=e
extern "C" __declspec(dllexport) void __cdecl Or4b52c0(std::uint8_t* p, std::uint32_t val, std::uint32_t flag) {
    std::uint32_t e = *reinterpret_cast<std::uint32_t*>(p + 8);
    if (flag) e |= val;
    *reinterpret_cast<std::uint32_t*>(p + 8) = e;
}
RH_ScopedInstall(Or4b52c0, 0x004b52c0);

// 0x004b5240 — void fn(p,flag): cl=p[2]; if(flag) p[2]=cl|4 else p[2]=cl&0xfb
// [carlod 2026-10-16] the else arm was missing: `je 0x4b5256` (0x004b524d) -> `and cl,0xfb;
// mov [eax+2],cl` (0x004b5256..0x004b5259), verify/carlod_20261016/evidence/dis_004b5240.txt.
// FUN_00420870 @ 0x00420870 clears each hidden part's rpATOMICRENDER through it every frame.
extern "C" __declspec(dllexport) void __cdecl Or4b5240(std::uint8_t* p, std::uint32_t flag) {
    std::uint8_t cl = p[2];
    if (flag) p[2] = static_cast<std::uint8_t>(cl | 4);
    else      p[2] = static_cast<std::uint8_t>(cl & 0xfb);
}
RH_ScopedInstall(Or4b5240, 0x004b5240);

// ===== round 116 =====

// MATCH-S14: the body of 0x00489290 (Reset489290) moved to Match/MatchS14_00489290.cpp,
// which compiles the byte-matched source re/match/s14/00489290.c.

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

// MATCH-S14: the body of 0x00489da0 (Zero489da0) moved to Match/MatchS14_00489da0.cpp,
// which compiles the byte-matched source re/match/s14/00489da0.c.

// ===== round 118 =====
// 0x004d54d0 — byte-verified: void fn(idx, out): *out = *(u32*)(0x7d57f8 + idx*8)
extern "C" __declspec(dllexport) void __cdecl Get4d54d0(std::uint32_t idx, std::uint32_t* out) {
    *out = *reinterpret_cast<std::uint32_t*>(0x007d57f8u + idx * 8u);
}
RH_ScopedInstall(Get4d54d0, 0x004d54d0);

// MATCH-S14: the body of 0x004893b0 (Inc4893b0) moved to Match/MatchS14_004893b0.cpp,
// which compiles the byte-matched source re/match/s14/004893b0.c.

// ===== round 120 =====
// 0x0046be10 — byte-verified: uint fn(arg0,a1,a2,a3):
//   0x0046be14 cmp eax,0x10 ; jb  -> a1 >= 0x10 (unsigned): 0x0046be19 xor eax,eax ; ret
//   0x0046be20 imul eax,eax,0x341 ; 0x0046be2a add eax,a2 ; 0x0046be32 shl eax,2 -> off
//   0x0046be35 *(u32*)(0x88219c+off)=*(u32*)arg0 ; 0x0046be3b *(u32*)(0x88221c+off)=a3
//   0x0046be41 mov eax,1 ; ret   (re-ported 2026-10-12 reearn/fstate: was `return off`)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Write46be10(std::uint8_t* arg0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) {
    if (a1 >= 0x10u) return 0;                                                                        // 0x0046be14
    std::uint32_t off = (a1 * 0x341u + a2) * 4u;                                                      // 0x0046be20..0x0046be32
    *reinterpret_cast<std::uint32_t*>(0x0088219cu + off) = *reinterpret_cast<std::uint32_t*>(arg0);   // 0x0046be35
    *reinterpret_cast<std::uint32_t*>(0x0088221cu + off) = a3;                                        // 0x0046be3b
    return 1;                                                                                         // 0x0046be41
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

// MATCH-S12: the body of 0x004773f0 (Init4773f0) moved to Match/MatchS12_004773f0.cpp,
// which compiles the byte-matched source re/match/s12/004773f0.c.

// MATCH-S19: the body of 0x004c75c0 (Get4c75c0) moved to Match/MatchS19_004c75c0.cpp,
// which compiles the byte-matched source re/match/s19/004c75c0.c.

// MATCH-S14: the body of 0x00485340 (Get485340) moved to Match/MatchS14_00485340.cpp,
// which compiles the byte-matched source re/match/s14/00485340.c.

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
// PowerupTypeLookup (0x0045baa0): the body is Powerup/PowerupTypeSearch.cpp, shared with the exe (dcmix
// 2026-10-22). This export feeds it the original's table: base 0x005f9998, count [0x005f9bd8], stride 0x40.
// ESI = search key (register arg) on the original; this export is __cdecl(key) and returns the entry or 0.
extern "C" __declspec(dllexport) std::uint32_t __cdecl Search45baa0(std::uint32_t key) {
    return reinterpret_cast<std::uint32_t>(PowerupTypeSearch(
        key, reinterpret_cast<const std::uint8_t*>(0x005f9998), *reinterpret_cast<int*>(0x005f9bd8), 0x40));
}
// Register-ABI shim — this, NOT Search45baa0, is installed at 0x0045baa0.
// ABI FIX 2026-07-27 — direct sibling of the 0x0041f330 crasher. The original takes its key
// in ESI and nothing on the stack (0x0045bab1 `CMP ESI,dword ptr [EAX]`), so the plain
// __cdecl install read [esp+4] caller garbage, never matched, and returned 0
// (0x0045babd `XOR EAX,EAX`) — the same NULL-propagation failure mode.
// Register preservation: the original clobbers EAX/ECX/EDX (0x0045baa0 writes EDX,
// 0x0045baa6 writes ECX, 0x0045baac writes EAX) and preserves ESI/EBX/EDI/EBP — exactly what
// __cdecl gives the C body, so a plain marshalling shim suffices. Return stays in EAX.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Search45baa0_RegAbi(void) {
    __asm {
        push esi            // key (the original reads it from ESI)
        call Search45baa0
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(Search45baa0_RegAbi, 0x0045baa0);  // ABI fixed 2026-07-27 (was __cdecl)

// 0x0041f330 — byte-verified ESI-keyed linear search into a PARALLEL array:
//   edx=[0x5f5fe0] (count); eax=0 (index); if(count<=0) return 0; ecx=0x5f3828 (base);
//   loop: if(esi==[ecx]) goto found; eax++; ecx+=0x84; while(eax<count); return 0;
//   found: return 0x5f37a8 + eax*0x84.   ESI = search key. Reimpl __cdecl(key).
//
// ABI FIX 2026-07-27 — this was the ~71 s runtime AV, isolated by hook-index bisection
// (registry index 917 ALONE: 2/2 AV; the two neighbouring indices 918/919 alone: 0/2).
// The original takes its key in ESI and NOTHING on the stack (0x0041f341 `CMP [ECX],ESI`);
// the C body below was installed DIRECTLY, so the live hook read `key` from [esp+4] —
// caller stack garbage. The key then never matched, the function returned 0, and that NULL
// propagated downstream to `RpClumpForAllAtomics` (0x004e66d0), which does
// `MOV EAX,[EAX+8]` on it: AV 0xC0000005 at eip=0x004e66db with eax=0, t≈71 s.
// Same class as crashers #3/#5/#6 and 0x0047bc90 — note 2854245f called 0x0042add0 and
// 0x0047bc90 "the last two register-arg ABI suspects"; that was an OVERCLAIM, this is a third.
// Register preservation: the original clobbers EAX/ECX/EDX (0x0041f330 writes EDX,
// 0x0041f33c writes ECX) and preserves ESI/EBX/EDI/EBP — exactly what __cdecl gives the C
// body, so a plain marshalling shim suffices here (no EDX save/restore needed, unlike
// Match47bc90_RegAbi / Find42ad90_RegAbi).
// The C body KEEPS its stack ABI for re/frida/hooks_registry.py (arg_type esi_global_search
// sets ESI on the ORIGINAL side only); the INSTALLED hook is the naked shim below.
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
// Register-ABI shim — this, NOT Search41f330, is installed at 0x0041f330.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Search41f330_RegAbi(void) {
    __asm {
        push esi            // key (the original reads it from ESI)
        call Search41f330
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(Search41f330_RegAbi, 0x0041f330);  // ABI fixed 2026-07-27 (was __cdecl -> ~71 s AV)

// MATCH-S15: the body of 0x0048ebc0 (Div48ebc0) moved to Match/MatchS15_0048ebc0.cpp,
// which compiles the byte-matched source re/match/s15/0048ebc0.c.

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
// MATCH-S11: the body of 0x004726b0 (Dot4726b0) moved to Match/MatchS11_004726b0.cpp,
// which compiles the byte-matched source re/match/s11/004726b0.c.

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
// Register-ABI shim — this, NOT Split41e170, is installed at 0x0041e170.
// ABI FIX 2026-07-27 — same class. The original takes BOTH args in registers and nothing on
// the stack: EAX = input value (0x0041e171 `MOV ESI,EAX`) and EDI = out pointer
// (0x0041e199 `MOV [EDI],ECX`, 0x0041e1b2 `MOV [EDI+0x4],EDX`, 0x0041e1af `MOV [EDI+0x8],ESI`).
// Installed as plain __cdecl it read two caller stack-garbage words and wrote three dwords
// through the second one. Fixed on disassembly evidence, not on an observed crash.
// Register preservation: the original preserves ESI (0x0041e170 PUSH / 0x0041e1b5 POP), EDI,
// EBX and EBP, and clobbers EAX/ECX/EDX — matching __cdecl, so no extra saves are needed.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Split41e170_RegAbi(void) {
    __asm {
        push edi            // out  (2nd C param — __cdecl pushes right-to-left)
        push eax            // v    (1st C param)
        call Split41e170
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(Split41e170_RegAbi, 0x0041e170);  // ABI fixed 2026-07-27 (was __cdecl)

// MATCH-S16: the body of 0x004957a0 (Grid4957a0) moved to Match/MatchS16_004957a0.cpp,
// which compiles the byte-matched source re/match/s16/004957a0.c.

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

// MATCH-S19: the body of 0x004c5bc0 (Insert4c5bc0) moved to Match/MatchS19_004c5bc0.cpp,
// which compiles the byte-matched source re/match/s19/004c5bc0.c.

// ===== round 138 ===== (4-entry global ptr-table match predicate)
// MATCH-S10: the body of 0x0045c510 (Match45c510) moved to Match/MatchS10_0045c510.cpp,
// which compiles the byte-matched source re/match/s10/0045c510.c.

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

// MATCH-S07: the body of 0x0042c150 (Scan42c150) moved to Match/MatchS07_0042c150.cpp,
// which compiles the byte-matched source re/match/s07/0042c150.c.

// MATCH-S14: the body of 0x004850b0 (Search4850b0) moved to Match/MatchS14_004850b0.cpp,
// which compiles the byte-matched source re/match/s14/004850b0.c.

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
// D4-DC-RFX-CORE: the Copy476a10 body moved to Render/RfxEmitter.cpp (one body for both targets).

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

// MATCH-S12: the body of 0x00477450 (Init477450) moved to Match/MatchS12_00477450.cpp,
// which compiles the byte-matched source re/match/s12/00477450.c.
// Its register-ABI entry Init477450_RegAbi (the installed hook) moved there too.

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
// 0x005ab980 — fn(arg): esi=arg[0xc]-*0x7dd610; cnt=*0x7dd614-esi; if(cnt>=arg[0x14])
//   cnt=arg[0x14]; memcpy(arg[0x18], 0x7dce08+esi, cnt); arg[0x18]+=cnt; arg[0x14]-=cnt.
// IMPLICIT EAX RETURN (U-6701 resolved 2026-07-31): Ghidra types the original
// void, but it leaves cnt in EAX at RET (0x005ab9a4 MOV ECX,EAX path keeps EAX)
// and the single caller FUN_005ab710 consumes it: 0x005ab7f7 ADD EBP,EAX /
// 0x005ab7f9 ADD EDI,EAX (stream-cursor advance). The void port wedged the
// audio stream pump during track load (statediff bisection, registry idx 946).
extern "C" __declspec(dllexport) std::uint32_t __cdecl Ring5ab980(std::uint8_t* arg) {
    std::uint32_t esi = *reinterpret_cast<std::uint32_t*>(arg + 0xc) - *reinterpret_cast<std::uint32_t*>(0x007dd610);
    std::uint32_t cnt = *reinterpret_cast<std::uint32_t*>(0x007dd614) - esi;
    std::uint32_t lim = *reinterpret_cast<std::uint32_t*>(arg + 0x14);
    if (cnt >= lim) cnt = lim;
    std::uint8_t* src = reinterpret_cast<std::uint8_t*>(0x007dce08 + esi);
    std::uint8_t* dst = *reinterpret_cast<std::uint8_t**>(arg + 0x18);
    for (std::uint32_t i = 0; i < cnt; i++) dst[i] = src[i];
    *reinterpret_cast<std::uint32_t*>(arg + 0x18) = reinterpret_cast<std::uint32_t>(dst) + cnt;
    *reinterpret_cast<std::uint32_t*>(arg + 0x14) = lim - cnt;
    return cnt;
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

// MATCH-S14: the body of 0x00485a00 (Init485a00) moved to Match/MatchS14_00485a00.cpp,
// which compiles the byte-matched source re/match/s14/00485a00.c.

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
// [moved] 0x00484c90 Zero484c90 moved to Race/WorldObjectLinks.cpp (D4-DC-INC-batch3, 2026-11-01): one body, both targets.

// MATCH-S14: the body of 0x004899c0 (Fill4899c0) moved to Match/MatchS14_004899c0.cpp,
// which compiles the byte-matched source re/match/s14/004899c0.c.

// MATCH-S09: the body of 0x00458f20 (Set458f20) moved to Match/MatchS09_00458f20.cpp,
// which compiles the byte-matched source re/match/s09/00458f20.c.

// MATCH-S13: the body of 0x0047bc90 (Match47bc90) and its register-ABI entry Match47bc90_RegAbi
// moved to Match/MatchS13_0047bf70.cpp, which compiles the byte-matched source re/match/s13/0047bf70.c.

// ===== round 162 ===== (EDX/EBX/EDI register-arg array search)
// 0x0042ad90 — u32 fn(EDX=arr, EBX=key, EDI=n): if(arr==0) return -1; walk arr[ecx] (term
//   0xff070000); count matches==key in esi; when esi==n at a match, return arr[ecx+1]; else -1.
//
// BUGFIX 2026-07-26 — ABI MISMATCH; FIFTH confirmed boot-AV crasher (index 951;
// MASHED_HOOK_ONLY=0x0042AD90 alone: 3/3 AV 0xC0000005 @ 8.15-8.42 s).
// The original takes NOTHING on the stack: 0x0042ad90 `TEST EDX,EDX`, 0x0042ada6
// `CMP EAX,EBX`, 0x0042adaa `CMP EDI,ESI`. The game's callers therefore push nothing,
// and this __cdecl body read three words of caller stack garbage as (arr,key,n) and
// walked a wild pointer looking for the 0xff070000 sentinel.
// Same class as crasher #3 (0x0042ac00 MenuGroupCount) — see that commit.
//
// The C body below KEEPS its stack ABI because re/frida/hooks_registry.py drives the
// export `Find42ad90` with stack args (arg_type edx_ebx_edi_find sets the registers on
// the ORIGINAL side only). The INSTALLED hook is the naked marshalling shim below it.
// dualcopy4 (2026-10-16): the C body Find42ad90 (stack ABI, the export the registry key find_42ad90 drives) moved to
// Frontend/KvFind.cpp, compiled into BOTH targets, so the exe's menu-table lookup (Frontend/MenuNavSM.cpp:KvLookup) runs the same
// body the original-side evidence covers. The naked shim below still marshals EDX/EBX/EDI onto the stack and calls it.
extern "C" std::uint32_t __cdecl Find42ad90(std::uint8_t* arr, std::uint32_t key, std::uint32_t n);

// Register-ABI shim — this, NOT Find42ad90, is what gets installed at 0x0042ad90.
// Marshals the original's EDX/EBX/EDI args onto the stack. EBX/ESI/EDI are callee-saved
// under __cdecl so the C body preserves them exactly as the original does (the original
// only pushes/pops ESI, 0x0042ad9a / 0x0042adbe). Return value stays in EAX.
//
// EDX FIX 2026-07-27 — same defect class as 0x0047bc90 (which produced the ~71 s AV).
// The original PRESERVES EDX: it is the array base, addressed as [EDX + ECX*4 + 4] at
// 0x0042adaf and 0x0042adc0, and never written. The compiled C body does write it
// (`lea edx,[edi+4]` / `add edx,4`). Fixed on disassembly evidence, not on a crash.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Find42ad90_RegAbi(void) {
    __asm {
        push edx            // preserve EDX — the original never writes it
        push edi            // n
        push ebx            // key
        push edx            // arr
        call Find42ad90
        add  esp, 0Ch
        pop  edx
        ret
    }
}
RH_ScopedInstall(Find42ad90_RegAbi, 0x0042ad90);  // ABI fixed 2026-07-26 (was __cdecl -> live boot AV)

// ===== round 163 ===== (EBX/EDI register-arg search, arr from globals)
// 0x0042add0 — u32 fn(EBX=key, EDI=n): idx=*0x67e9f8; arr=*(u8**)(idx*0x40 + 0x67ed38);
//   same walk as 0x42ad90 (find (n+1)-th key, return following element; else -1).
//
// ABI FIX 2026-07-26 — sibling of crasher #5 (0x0042ad90), same class. The original takes
// BOTH args in registers and nothing on the stack: 0x0042adf0 `CMP EAX,EBX` (key) and
// 0x0042adf4 `CMP EDI,ESI` (n); the array comes from globals (0x0042add0 `MOV EAX,[0x67e9f8]`,
// 0x0042add5 `SHL EAX,0x6`, 0x0042add8 `MOV EDX,[EAX + 0x67ed38]`), not from an argument.
// Fixed on disassembly evidence, NOT because it crashed — this hook does not AV in isolation
// (the boot bisect never reached it). C body keeps its stack ABI for hooks_registry.py
// (arg_type ebx_edi_global_find); the INSTALLED hook is the naked shim below.
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
// Register-ABI shim — this, NOT Find42add0, is installed at 0x0042add0.
// The original only pushes/pops ESI (0x0042ade0 / 0x0042ae08), and EBX/ESI/EDI are
// callee-saved under __cdecl, so the shim preserves exactly what the original preserves.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Find42add0_RegAbi(void) {
    __asm {
        push edi            // n
        push ebx            // key
        call Find42add0
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(Find42add0_RegAbi, 0x0042add0);  // ABI fixed 2026-07-26 (was __cdecl)

// MATCH-S15: the body of 0x00491070 (Rain491070) moved to Match/MatchS15_00491070.cpp,
// which compiles the byte-matched source re/match/s15/00491070.c.

// MATCH-S12: the body of 0x00477b60 (Alloc477b60) moved to Match/MatchS12_00477b60.cpp,
// which compiles the byte-matched source re/match/s12/00477b60.c.

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
// D4-DC-RFX-CORE: the Set476b30 body moved to Render/RfxEmitter.cpp (one body for both targets).

// MATCH-S13: the body of 0x00482730 (Init482730) moved to Match/MatchS13_00482730.cpp,
// which compiles the byte-matched source re/match/s13/00482730.c.

// MATCH-S14: the body of 0x004893d0 (Push4893d0) moved to Match/MatchS14_004893d0.cpp,
// which compiles the byte-matched source re/match/s14/004893d0.c.

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

// MATCH-S05: the body of 0x00420de0 (Accum420de0) moved to Match/MatchS05_00420de0.cpp,
// which compiles the byte-matched source re/match/s05/00420de0.c.

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

// MATCH-S05: the body of 0x004223f0 (Hash4223f0) moved to Match/MatchS05_00422470.cpp,
// which compiles the byte-matched source re/match/s05/00422470.c.

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

// MATCH-S14: the body of 0x0048a630 (Aabb48a630) moved to Match/MatchS14_0048a630.cpp,
// which compiles the byte-matched source re/match/s14/0048a630.c.

// MATCH-S19: the body of 0x004c5c00 (Search4c5c00) moved to Match/MatchS19_004c5c00.cpp,
// which compiles the byte-matched source re/match/s19/004c5c00.c.

// [REVERTED 2026-06-15] round 179 FUN_005172f0 (0x005172f0) is in the statically-linked
// libpng/zlib band (0x516000-0x529fff) -> library-skip per user ruling; demoted C3->C2.
// Was a 4-byte -> printable/[hex] string formatter reimpl.

// MATCH-S13: the body of 0x00482860 (Pool482860) moved to Match/MatchS13_00482860.cpp,
// which compiles the byte-matched source re/match/s13/00482860.c.

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

// MATCH-S09: the body of 0x00458f80 (Init458f80) moved to Match/MatchS09_00458f80.cpp,
// which compiles the byte-matched source re/match/s09/00458f80.c.

// MATCH-S07: the body of 0x0042c1f0 (Count42c1f0) moved to Match/MatchS07_0042c1f0.cpp,
// which compiles the byte-matched source re/match/s07/0042c1f0.c.

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

// FUN_00408590 Dot408590: MOVED 2026-11-01 (D4-DC-INC-ai) to Race/PathProgDot.cpp, one body for both targets.

// FUN_00413fa0 Calc413fa0: MOVED 2026-11-02 (MATCH-T13-SEED-WRITER) to Match/MatchS03_00413fa0.cpp, the
// byte-matched re/match/s03/h2_00413fa0.c, one body for both targets.

// [SKIPPED 2026-06-15] FUN_0049a730 (0x0049a730) is a clean near-leaf (!FUN_00426c00()) but
// has NO static caller (no direct call, no address-as-data ref) -> cannot satisfy the
// caller-C2+ gate, so NOT promoted (left C2).

// MATCH-PORTSTATE-GAMEFLOW-FU: the body of 0x0040b6e0 (Add40b6e0) moved to Match/MatchS02_0040b6e0.cpp
// (byte-matched re/match/s02/0040b6e0.c).

// MATCH-S10: the body of 0x0045c7f0 (Sub45c7f0) moved to Match/MatchS10_0045c7f0.cpp,
// which compiles the byte-matched source re/match/s10/0045c7f0.c.

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

// MATCH-S06: the body of 0x00429b30 (Init429b30) moved to Match/MatchS06_00429b30.cpp,
// which compiles the byte-matched source re/match/s06/00429b30.c.

// MATCH-S06: the body of 0x00429b70 (Init429b70) moved to Match/MatchS06_00429b70.cpp,
// which compiles the byte-matched source re/match/s06/00429b70.c.

// MATCH-S11: the body of 0x00472560 (Build472560) moved to Match/MatchS11_00472560.cpp,
// which compiles the byte-matched source re/match/s11/00472560.c.

// MATCH-S11: the body of 0x004725f0 (InitRange4725f0) moved to Match/MatchS11_004725f0.cpp,
// which compiles the byte-matched source re/match/s11/004725f0.c.

// MATCH-S05: the body of 0x004215c0 (Accum4215c0) moved to Match/MatchS05_00420de0.cpp,
// which compiles the byte-matched source re/match/s05/00420de0.c.

// MATCH-S10: the body of 0x00462760 (Pred462760) moved to Match/MatchS10_00462760.cpp,
// which compiles the byte-matched source re/match/s10/00462760.c.

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

// [DEFERRED 2026-06-15] FUN_00423b00 (0x00423b00) gated 4-function orchestrator (3 C3 calls +
// C2 tail 0x423670). Reimpl drafted but the callees are deep multi-branch state machines whose
// output globals weren't located -> early_window gave a DEGENERATE all-zero gate-on/off snapshot
// (couldn't find an observe target that changes). Left C2; needs deeper per-callee analysis or
// the booted lane. Not promoted (no degenerate-GREEN).

// MATCH-S07: the body of 0x0042c280 (Init42c280) moved to Match/MatchS07_0042c280.cpp,
// which compiles the byte-matched source re/match/s07/0042c280.c.

// MATCH-S14: the body of 0x0048ade0 (ZeroFill48ade0) moved to Match/MatchS14_0048ade0.cpp,
// which compiles the byte-matched source re/match/s14/0048ade0.c.

// MATCH-S14: the body of 0x0048a830 (ZeroFill48a830) moved to Match/MatchS14_0048a830.cpp,
// which compiles the byte-matched source re/match/s14/0048a830.c.

// [REVERTED r209 2026-06-15] FUN_004c2dc0 reimpl drafted + GREEN, but its subsystem tag is
// third-party-library[lua-5.0] -> Lua library-skip (NOT first-party). Left C2. Lesson: the
// near-leaf scan must check the hooks.csv SUBSYSTEM tag and skip third-party-library[*]
// (lua/libpng/zlib/CRT), not just address bands.

// [SKIPPED r210 2026-06-15] 0x004b6520 ZeroFillWrapper is ALREADY reimplemented +
// installed in Util/TimerSlot.cpp:33 (RH_ScopedInstall(ZeroFillWrapper, 0x004b6520);
// std::memset(p1,0,p2), observably == original FUN_004b64e0(p1,0,p2)). The near-leaf
// scan re-surfaced an already-implemented function; a 2nd RH_ScopedInstall at the same
// RVA would be a duplicate install. C2->C3 promoted via the EXISTING ZeroFillWrapper
// export + a non-degenerate near_leaf_memset2 early_window diff (registry zero_fill_wrapper).

// MATCH-S05: the body of 0x00420d80 (IndexedFloatPairSum420d80) moved to Match/MatchS05_00420d80.cpp,
// which compiles the byte-matched source re/match/s05/00420d80.c.

// MATCH-S08: the body of 0x0044e070 (DoubleIndexedFloatMul44e070) moved to Match/MatchS08_0044e070.cpp,
// which compiles the byte-matched source re/match/s08/0044e070.c.

// 0x004cb740  FUN_004cb740 (render, StructTagEquals) — PURE LEAF (no calls)
// Tagged-union dword equality. Common: compare a/b at [0,4,8,c]; then tag=a[0]
// selects an extra field set (capstone-verified, all branches transcribed):
//   tag==1: {0x34,0x38,0x3c,0x4c}                          (0x4cb800)
//   tag==2: {0x34,0x38,0x3c,0x40,0x44,0x48,0x4c,0x60,0x64} (0x4cb7a8)
//   tag==3: {0x40,0x44,0x48}                               (0x4cb781)
//   else  : (none — common prefix suffices)                (jmp 0x4cb82c)
// Returns 1 if all compared dwords equal, else 0. Only the return is observed
// (integer logic) so bit-identity is exact.
extern "C" __declspec(dllexport) int __cdecl StructTagEquals4cb740(const void* a, const void* b)
{
    #define MRE_AE(off) (*reinterpret_cast<const int*>(static_cast<const char*>(a) + (off)) \
                      == *reinterpret_cast<const int*>(static_cast<const char*>(b) + (off)))
    if (!MRE_AE(0x00)) return 0;
    if (!MRE_AE(0x04)) return 0;
    if (!MRE_AE(0x08)) return 0;
    if (!MRE_AE(0x0c)) return 0;
    const int tag = *reinterpret_cast<const int*>(static_cast<const char*>(a) + 0x00);
    if (tag == 1) {
        if (!MRE_AE(0x34)) return 0;
        if (!MRE_AE(0x38)) return 0;
        if (!MRE_AE(0x3c)) return 0;
        return MRE_AE(0x4c) ? 1 : 0;
    } else if (tag == 2) {
        if (!MRE_AE(0x34)) return 0;
        if (!MRE_AE(0x38)) return 0;
        if (!MRE_AE(0x3c)) return 0;
        if (!MRE_AE(0x40)) return 0;
        if (!MRE_AE(0x44)) return 0;
        if (!MRE_AE(0x48)) return 0;
        if (!MRE_AE(0x4c)) return 0;
        if (!MRE_AE(0x60)) return 0;
        return MRE_AE(0x64) ? 1 : 0;
    } else if (tag == 3) {
        if (!MRE_AE(0x40)) return 0;
        if (!MRE_AE(0x44)) return 0;
        return MRE_AE(0x48) ? 1 : 0;
    }
    return 1;
    #undef MRE_AE
}
RH_ScopedInstall(StructTagEquals4cb740, 0x004cb740);

// MATCH-S11: the body of 0x0046bda0 (IndexedFloatAccum16_46bda0) moved to Match/MatchS11_0046bda0.cpp,
// which compiles the byte-matched source re/match/s11/0046bda0.c.

// 0x0046d7f0  FUN_0046d7f0 (ai, BoundedTableSignClamp) — MOVED 2026-10-01.
// The body now lives in Vehicle\LaunchRevCharge.cpp, which is in BOTH
// exe_sources.rsp and asi_sources.rsp. It had to move because the STANDALONE
// needs it: U-9174 resolved this function as the per-tick CHARGE half of the
// start-line launch mechanic whose release (0x0046d780, writer 0x0046d7a2) sets
// veh+0xbf8, and this TU is asi-only. ONE RVA, ONE BODY — do not re-add it here.

// 0x004d7100  FUN_004d7100 (render, EventEnqueueCascade) — PURE LEAF (no calls)
// void f(int param): one-shot guarded event-enqueue cascade over global flags +
// an event queue at 0x7d5168 indexed by counter 0x7d6c14. capstone-verified
// (both param!=0 and param==0 paths + shared tail at 0x4d71c5). eax=1, edx=0
// throughout the original. Integer/flag state; observed via seeded globals.
extern "C" __declspec(dllexport) void __cdecl EventEnqueueCascade4d7100(int param)
{
    #define MRE_G(a) (*reinterpret_cast<int*>(static_cast<unsigned>(a)))
    int* const queue = reinterpret_cast<int*>(0x007d5168u);
    if (param != 0) {
        if (MRE_G(0x007d6b10u) != 0) return;
        MRE_G(0x007d6b10u) = 1;
        if (MRE_G(0x007d6b14u) != 0) return;
        MRE_G(0x007d58d0u) = 1;
        if (MRE_G(0x007d58d4u) == 0) {
            int c = MRE_G(0x007d6c14u);
            MRE_G(0x007d58d4u) = 1;
            queue[c] = 0x1b;
            MRE_G(0x007d6c14u) = c + 1;
        }
        {
            int e = MRE_G(0x007d6bfcu);
            if (MRE_G(0x007d5870u) == e) return;
            MRE_G(0x007d5870u) = e;
        }
    } else {
        if (MRE_G(0x007d6b10u) == 0) return;
        MRE_G(0x007d6b10u) = 0;
        if (MRE_G(0x007d6b14u) != 0) return;
        MRE_G(0x007d58d0u) = 0;
        if (MRE_G(0x007d58d4u) == 0) {
            int c = MRE_G(0x007d6c14u);
            MRE_G(0x007d58d4u) = 1;          // original writes 1 here even on param==0
            queue[c] = 0x1b;
            MRE_G(0x007d6c14u) = c + 1;
        }
        if (MRE_G(0x007d5870u) == 0) return;
        MRE_G(0x007d5870u) = 0;
    }
    // shared tail (0x4d71c5):
    if (MRE_G(0x007d5874u) != 0) return;
    {
        int c = MRE_G(0x007d6c14u);
        MRE_G(0x007d5874u) = 1;
        queue[c] = 0xf;
        MRE_G(0x007d6c14u) = c + 1;
    }
    #undef MRE_G
}
RH_ScopedInstall(EventEnqueueCascade4d7100, 0x004d7100);

// 0x005b6a40  FUN_005b6a40 (audio, SuccApproxQuantize) — PURE LEAF (no calls)
// void f(int arg1, int* p2, int* p3): successive-approximation quantizer.
//   esi=(short)arg1; ecx=range=*(s16*)(0x634498 + (*p3)*2); edx=esi-*p2;
//   al=0; if(edx<0){al=8;edx=-edx;} bits 4/2/1 by edx>=ecx (ecx>>=1 each);
//   *p2 = (al&8)? esi-ecx+edx : ecx-edx+esi; clamp [-0x8000,0x7fff];
//   *p3 += *(s16*)(0x634478 + (al&0xff)*2); if(*p3<0) *p3=0.
// capstone-verified; pure integer -> bit-identity exact. ecx sar = signed >>.
extern "C" __declspec(dllexport) void __cdecl SuccApproxQuantize5b6a40(int arg1, int* p2, int* p3)
{
    const short esi = static_cast<short>(arg1);
    int ecx = *reinterpret_cast<const short*>(0x00634498u + static_cast<unsigned>(*p3) * 2u);
    int edx = static_cast<int>(esi) - *p2;
    int al = 0;
    if (edx < 0) { al = 8; edx = -edx; }
    if (edx >= ecx) { al |= 4; edx -= ecx; }
    ecx >>= 1;
    if (edx >= ecx) { al |= 2; edx -= ecx; }
    ecx >>= 1;
    if (edx >= ecx) { al |= 1; edx -= ecx; }
    ecx >>= 1;
    *p2 = (al & 8) ? (static_cast<int>(esi) - ecx + edx)
                   : (ecx - edx + static_cast<int>(esi));
    if (*p2 > 0x7fff) *p2 = 0x7fff;
    else if (*p2 < -0x8000) *p2 = -0x8000;
    const int d = *reinterpret_cast<const short*>(0x00634478u + static_cast<unsigned>(al & 0xff) * 2u);
    *p3 = *p3 + d;
    if (*p3 < 0) *p3 = 0;
}
RH_ScopedInstall(SuccApproxQuantize5b6a40, 0x005b6a40);

// 0x0040dcb0  FUN_0040dcb0 (gameplay, TimerStateMachine) — PURE LEAF (no calls)
// void f(void): multi-phase countdown state machine on globals. state=*0x63b90c
// (1/2/3); each phase decrements a float timer by a rate and, on crossing a
// threshold, advances the state. capstone-verified (consts: rate=700.0 @0x5ccd7c,
// thr -320 @0x5ccd78, thr 320 @0x5ccd74, thr 0.0 @0x5d757c). fcomp jp-idiom => the
// reset fires when timer <= threshold (finite). x87: comparison uses the 80-bit
// intermediate; verified with non-borderline vectors so the 32-bit float compare agrees.
extern "C" __declspec(dllexport) void __cdecl TimerStateMachine40dcb0(void)
{
    int* const pstate = reinterpret_cast<int*>(0x0063b90cu);
    float* const t1 = reinterpret_cast<float*>(0x0063b910u);
    float* const t2 = reinterpret_cast<float*>(0x0063b918u);
    const float dt   = *reinterpret_cast<const float*>(0x007f100cu);
    const float rate = *reinterpret_cast<const float*>(0x005ccd7cu);
    const int state = *pstate;
    if (state == 1) {
        float v = *t1 - (dt * rate);
        *t1 = v;
        if (v <= *reinterpret_cast<const float*>(0x005ccd74u)) {     // <= 320.0
            *reinterpret_cast<int*>(0x0063b910u) = 0x43a00000;       // 320.0
            *reinterpret_cast<int*>(0x0063b918u) = 0x40a00000;       // 5.0
            *pstate = 2;
        }
    } else if (state == 2) {
        float v = *t2 - dt;
        *t2 = v;
        if (v <= *reinterpret_cast<const float*>(0x005d757cu)) {     // <= 0.0
            *pstate = 3;
        }
    } else if (state == 3) {
        float v = *t1 - (dt * rate);
        *t1 = v;
        if (v <= *reinterpret_cast<const float*>(0x005ccd78u)) {     // <= -320.0
            *pstate = 0;
        }
    }
}
RH_ScopedInstall(TimerStateMachine40dcb0, 0x0040dcb0);

// MATCH-FIX-45CA30: the body of 0x0045ca30 (FloatClamp3Level45ca30) moved to Match/MatchS10_0045ca30.cpp,
// which compiles the byte-matched source re/match/s10/0045ca30.c (the old body here walked 6 of the 36 floats).

// D4-DC-RFX-CORE: the MultiArrayScatter476d00 body moved to Render/RfxEmitter.cpp (one body for both targets).

// MATCH-S19: the body of 0x004c0e50 (DllHeadInsert4c0e50) moved to Match/MatchS19_004c0e50.cpp,
// which compiles the byte-matched source re/match/s19/004c0e50.c.

// 0x00413c70 FUN_00413c70 (vehicle, Idx2RecordCondSet) — PURE LEAF (no calls)
// void f(int i, int j, int v): off = (j + 5*i)*12; *(int*)(0x63bc68+off) = v;
//   if (*(float*)(0x63bc60+off) == 0.0f) *(int*)(0x63bc60+off) = 0x3c23d70a (0.01f).
// capstone-verified (fcomp jp test ah,0x44 idiom: jp NOT taken only when ==0.0).
// The fld loads the PRE-store value of field A; field B is always set to v.
extern "C" __declspec(dllexport) void __cdecl Idx2RecordCondSet413c70(int i, int j, int v)
{
    const unsigned off = static_cast<unsigned>(j + 5 * i) * 12u;
    *reinterpret_cast<int*>(0x0063bc68u + off) = v;
    if (*reinterpret_cast<const float*>(0x0063bc60u + off) == 0.0f)
        *reinterpret_cast<int*>(0x0063bc60u + off) = 0x3c23d70a;
}
RH_ScopedInstall(Idx2RecordCondSet413c70, 0x00413c70);

// 0x005b0bb0 FUN_005b0bb0 (audio, QuadBufferBuild) — PURE LEAF (no calls)
// int f(void* out, uint maxsize, struct* rec): pass1 sums sub-counts (byte @P+0xd
// over rec[0x14] records, each P = *(arr + 0x14 + k*0x28), arr = rec[0x18]); if
// sum*4 > maxsize return 0. pass2: per (record ri, sub si) write a 64-byte block
// of 4x16 entries {ri, si, 0, 1.0f} at out+counter*64; return counter*4.
// capstone-verified. Integer + one 1.0f constant -> bit-identical.
extern "C" __declspec(dllexport) int __cdecl QuadBufferBuild5b0bb0(unsigned char* out, unsigned maxsize, unsigned char* rec)
{
    const int count = *reinterpret_cast<int*>(rec + 0x14);
    unsigned char* arr = *reinterpret_cast<unsigned char**>(rec + 0x18);
    int sum = 0;
    for (int k = 0; k < count; k++) {
        unsigned char* p = *reinterpret_cast<unsigned char**>(arr + 0x14 + k * 0x28);
        sum += p[0xd];
    }
    if (static_cast<unsigned>(sum * 4) > maxsize) return 0;
    int counter = 0;
    for (int ri = 0; ri < count; ri++) {
        unsigned char* p = *reinterpret_cast<unsigned char**>(arr + 0x14 + ri * 0x28);
        const int sub = p[0xd];
        for (int si = 0; si < sub; si++) {
            int* e = reinterpret_cast<int*>(out + counter * 64);
            for (int j = 0; j < 4; j++) {
                e[j * 4 + 0] = ri;
                e[j * 4 + 1] = si;
                e[j * 4 + 2] = 0;
                *reinterpret_cast<float*>(e + j * 4 + 3) = 1.0f;
            }
            counter++;
        }
    }
    return counter * 4;
}
RH_ScopedInstall(QuadBufferBuild5b0bb0, 0x005b0bb0);

// MATCH-S12: the body of 0x00475f30 (ScaledMulAdd475f30) moved to Match/MatchS12_00475f30.cpp,
// which compiles the byte-matched source re/match/s12/00475f30.c.

// 0x004d8c40 FUN_004d8c40 (frontend, DllMergeSwap) — PURE LEAF (no calls)
// void f(void): on table entry base = *0x911ad8 + *0x7d3ff8, splice circular list B
// (base+0x20) into list A (base+0x24), then swap the two header fields and clear
// base+8. VERBATIM naked copy of the original -> byte-identical. Internal jumps via
// MASM labels. Verified via the swap path (empty-B early-exit).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl DllMergeSwap4d8c40(void)
{
    __asm {
        mov   edx, dword ptr ds:[0911AD8h]
        mov   eax, dword ptr ds:[07D3FF8h]
        push  esi
        mov   ecx, dword ptr [edx+eax+24h]
        mov   eax, dword ptr [edx+eax+20h]
        mov   edx, dword ptr [eax]
        cmp   edx, eax
        je    L82
        cmp   dword ptr [ecx], ecx
        jne   L6d
        mov   dword ptr [ecx], edx
        mov   dword ptr [edx+4], ecx
        mov   edx, dword ptr [eax+4]
        mov   dword ptr [ecx+4], edx
        mov   dword ptr [edx], ecx
        jmp   L7d
    L6d:
        mov   esi, dword ptr [ecx+4]
        mov   dword ptr [esi], edx
        mov   dword ptr [edx+4], esi
        mov   edx, dword ptr [eax+4]
        mov   dword ptr [edx], ecx
        mov   dword ptr [ecx+4], edx
    L7d:
        mov   dword ptr [eax], eax
        mov   dword ptr [eax+4], eax
    L82:
        mov   edx, dword ptr ds:[0911AD8h]
        mov   esi, dword ptr ds:[07D3FF8h]
        mov   dword ptr [edx+esi+24h], eax
        mov   eax, dword ptr ds:[0911AD8h]
        mov   edx, dword ptr ds:[07D3FF8h]
        pop   esi
        mov   dword ptr [eax+edx+20h], ecx
        mov   eax, dword ptr ds:[07D3FF8h]
        mov   ecx, dword ptr ds:[0911AD8h]
        mov   dword ptr [ecx+eax+8], 0
        ret
    }
}
RH_ScopedInstall(DllMergeSwap4d8c40, 0x004d8c40);

// MATCH-S13: the body of 0x00483ca0 (FindNodeStructCopy483ca0) moved to Match/MatchS13_00483ca0.cpp,
// which compiles the byte-matched source re/match/s13/00483ca0.c.

// 0x005a7a60 FUN_005a7a60 (audio, NestedListSearch) — PURE LEAF (no calls)
// uint f(int key): walk the outer circular list at *0x7dca7c (sentinel 0x7dca7c);
// for each outer node, walk its inner list (head @node-0xc, sentinel node-0x10,
// link +4, payload ptr +8); if any inner payload[0xc]==key return key else 0.
// VERBATIM naked copy (byte-identical; MASM labels).
extern "C" __declspec(dllexport) __declspec(naked) unsigned __cdecl NestedListSearch5a7a60(void)
{
    __asm {
        mov   edx, dword ptr ds:[07DCA7Ch]
        push  esi
        cmp   edx, 7DCA7Ch
        push  edi
        je    Lnf
        mov   eax, dword ptr [esp+0Ch]
    Louter:
        mov   ecx, dword ptr [edx-0Ch]
        lea   esi, [edx-10h]
        cmp   ecx, esi
        je    Lnext
    Linner:
        mov   edi, dword ptr [ecx+8]
        cmp   dword ptr [edi+0Ch], eax
        je    Lret
        mov   ecx, dword ptr [ecx+4]
        cmp   ecx, esi
        jne   Linner
    Lnext:
        mov   edx, dword ptr [edx]
        cmp   edx, 7DCA7Ch
        jne   Louter
    Lnf:
        xor   eax, eax
    Lret:
        pop   edi
        pop   esi
        ret
    }
}
RH_ScopedInstall(NestedListSearch5a7a60, 0x005a7a60);

// 0x004cde50 FUN_004cde50 (render, PixelMaxAlpha) — PURE LEAF (no calls)
// int f(struct* s): per-pixel alpha = max(R,G,B). mode = s[0xc]:
//   mode 4|8: (1<<mode) pixels from s[0x18]+2.
//   mode 0x20: s[8] rows x s[4] cols from s[0x14]+2, base += s[0x10] per row.
//   else: no-op. Returns s. capstone-verified; integer max-of-3 per pixel
//   (pixel p: p[1] = max(p[-2],p[-1],p[0])), p += 4.
extern "C" __declspec(dllexport) int __cdecl PixelMaxAlpha4cde50(unsigned char* s)
{
    const int mode = *reinterpret_cast<int*>(s + 0xc);
    if (mode == 4 || mode == 8) {
        const int n = 1 << mode;
        unsigned char* p = *reinterpret_cast<unsigned char**>(s + 0x18) + 2;
        for (int j = 0; j < n; j++) {
            int c = p[-2];
            if (static_cast<int>(p[-1]) > c) c = p[-1];
            if (static_cast<int>(p[0])  > c) c = p[0];
            p[1] = static_cast<unsigned char>(c);
            p += 4;
        }
    } else if (mode == 0x20) {
        const int rows = *reinterpret_cast<int*>(s + 8);
        unsigned char* base = *reinterpret_cast<unsigned char**>(s + 0x14);
        for (int ri = 0; ri < rows; ri++) {
            const int cols = *reinterpret_cast<int*>(s + 4);
            unsigned char* p = base + 2;
            for (int ci = 0; ci < cols; ci++) {
                int c = p[-2];
                if (static_cast<int>(p[-1]) > c) c = p[-1];
                if (static_cast<int>(p[0])  > c) c = p[0];
                p[1] = static_cast<unsigned char>(c);
                p += 4;
            }
            base += *reinterpret_cast<int*>(s + 0x10);
        }
    }
    return reinterpret_cast<int>(s);
}
RH_ScopedInstall(PixelMaxAlpha4cde50, 0x004cde50);

// 0x004d8570 FUN_004d8570 (render, EngineRegisterFuncs) — PURE LEAF (no calls/branches)
// int f(void): register 17 function pointers into the RW engine struct (*0x7d3ff8)
// at offsets 0xc4..0x104; return 1. Straight-line stores, capstone-verified.
extern "C" __declspec(dllexport) int __cdecl EngineRegisterFuncs4d8570(void)
{
    static const struct { unsigned off; unsigned val; } regs[] = {
        {0xc4,0x4a2b60},{0xc8,0x4a42c5},{0xcc,0x4ac560},{0xd0,0x4a34b0},
        {0xd4,0x4ac570},{0xd8,0x5c2300},{0xdc,0x4d8750},{0xe0,0x4d8730},
        {0xe4,0x4a3860},{0xe8,0x4af340},{0xec,0x4a3a90},{0xf0,0x4d8680},
        {0xf4,0x4a9410},{0xf8,0x4d86d0},{0xfc,0x4d8700},{0x100,0x4a3ac9},
        {0x104,0x5c22bd},
    };
    for (int i = 0; i < 17; i++) {
        unsigned char* e = *reinterpret_cast<unsigned char**>(0x007d3ff8u);
        *reinterpret_cast<unsigned*>(e + regs[i].off) = regs[i].val;
    }
    return 1;
}
RH_ScopedInstall(EngineRegisterFuncs4d8570, 0x004d8570);

// 0x0041e4b0 FUN_0041e4b0 (gameplay, StructPropagate) — EAX-implicit struct ptr, no calls.
// void f(EAX=s): if (s[0x1b4] == s[0x1b8]) return; s[0x1b8] = s[0x1b4]; then for each
// of 12 sub-object offsets, write table[s[0x1b4]] (table @0x63d5f8) into a 4-level
// deref chain: p=s[OFF]; p=p[0x18]; p=p[0x20]; p=*p; p[4]=table[idx]. capstone-verified.
static void __cdecl F41e4b0_Body(unsigned char* s)
{
    const int idx = *reinterpret_cast<int*>(s + 0x1b4);
    if (idx == *reinterpret_cast<int*>(s + 0x1b8)) return;
    *reinterpret_cast<int*>(s + 0x1b8) = idx;
    static const unsigned offs[12] = {
        0x108, 0x10c, 0x110, 0x114, 0x118, 0x11c,
        0x120, 0x124, 0x128, 0x12c, 0x178, 0x174,
    };
    for (int k = 0; k < 12; k++) {
        const int val = *reinterpret_cast<int*>(
            0x0063d5f8u + static_cast<unsigned>(*reinterpret_cast<int*>(s + 0x1b4)) * 4u);
        unsigned char* p = *reinterpret_cast<unsigned char**>(s + offs[k]);
        p = *reinterpret_cast<unsigned char**>(p + 0x18);
        p = *reinterpret_cast<unsigned char**>(p + 0x20);
        p = *reinterpret_cast<unsigned char**>(p);
        *reinterpret_cast<int*>(p + 4) = val;
    }
}
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl StructPropagate41e4b0(void)
{
    __asm {
        push  ebp
        mov   ebp, esp
        push  eax
        call  F41e4b0_Body
        add   esp, 4
        mov   esp, ebp
        pop   ebp
        ret
    }
}
RH_ScopedInstall(StructPropagate41e4b0, 0x0041e4b0);

// MATCH-S15: the body of 0x0048f590 (ParticlePoolAlloc48f590) moved to Match/MatchS15_0048f590.cpp,
// which compiles the byte-matched source re/match/s15/0048f590.c.

// MATCH-S14: the body of 0x00486f90 (PoolArrayReset486f90) moved to Match/MatchS14_00486f90.cpp,
// which compiles the byte-matched source re/match/s14/00486f90.c.

// MATCH-S12: the body of 0x00477b40 (ZeroTwoRegions477b40) moved to Match/MatchS12_00477b40.cpp,
// which compiles the byte-matched source re/match/s12/00477b40.c.

// MATCH-S14: the body of 0x004893a0 (AdjThunk4893a0) moved to Match/MatchS14_004893a0.cpp,
// which compiles the byte-matched source re/match/s14/004893a0.c.

// [SKIPPED r239 2026-06-15] 0x004b6550 is a pure tail-jmp thunk (jmp 0x4b6700); its
// first byte is 0xE9, which the early_window NO_AUTO_HOOK safety check reads as an
// installed inline-JMP hook -> aborts. Pure-jmp thunks (b0==0xE9) are UNVERIFIABLE via
// this harness. Reimpl would be trivially correct but cannot earn a GREEN -> left C2.
// LESSON: skip near_leaf_frontier rows whose name starts with thunk_FUN_ (pure jmp).

// 0x004b4130 FUN_004b4130 (render, Thunk4b4130) — NEAR-LEAF adjustor thunk to C3 0x4b40c0.
// void f(p, out): p = p[0x18]; tail-call 0x4b40c0(p, out). 0x4b40c0(s, out): if(out && s[0x24]>0)
// copy s[0x24] dwords from *(s[0x20]) to out. So f copies (p[0x18])[0x24] dwords from
// *((p[0x18])[0x20]) into out. VERBATIM naked (b0=0x8b ok; rel32 jmp -> mov eax,abs; jmp eax).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Thunk4b4130(void)
{
    __asm {
        mov   ecx, dword ptr [esp+4]
        mov   edx, dword ptr [ecx+18h]
        mov   dword ptr [esp+4], edx
        mov   eax, 04B40C0h
        jmp   eax
    }
}
RH_ScopedInstall(Thunk4b4130, 0x004b4130);

// 0x004b52f0 FUN_004b52f0 (render, Thunk4b52f0) — NEAR-LEAF adjustor thunk to C3 0x4b52c0.
// uint f(p, a2, a3): p = p[0x18]; tail-call 0x4b52c0(p, a2, a3). 0x4b52c0(s,a2,a3): eax=s[8];
// if(a3!=0){ eax|=a2; s[8]=eax; } return eax. So f: s=p[0x18]; if(a3) s[8]|=a2; return s[8].
// VERBATIM naked (b0=0x8b; rel32 jmp -> mov eax,abs; jmp eax).
extern "C" __declspec(dllexport) __declspec(naked) unsigned __cdecl Thunk4b52f0(void)
{
    __asm {
        mov   edx, dword ptr [esp+4]
        mov   eax, dword ptr [edx+18h]
        mov   dword ptr [esp+4], eax
        mov   eax, 04B52C0h
        jmp   eax
    }
}
RH_ScopedInstall(Thunk4b52f0, 0x004b52f0);

// 0x005a7af0 FUN_005a7af0 (audio, Thunk5a7af0) — NEAR-LEAF adjustor thunk to C3 0x5aded0.
// int f(p): p += 0xc; tail-call AudioListNodeCount(p) = count nodes of the circular list
// headed at p (linked via +4, sentinel = head). So f(p) = count of list at (p+0xc).
// VERBATIM naked (b0=0x8b; rel32 jmp -> mov eax,abs; jmp eax).
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl Thunk5a7af0(void)
{
    __asm {
        mov   eax, dword ptr [esp+4]
        add   eax, 0Ch
        mov   dword ptr [esp+4], eax
        mov   eax, 05ADED0h
        jmp   eax
    }
}
RH_ScopedInstall(Thunk5a7af0, 0x005a7af0);

// 0x004058c0 FUN_004058c0 (gameplay, FloatSubThunk4058c0) — NEAR-LEAF adjustor thunk to C3 0x4058b0.
// void f(idx, float fval): p = 0x639d80 + idx*0xec; tail-call FloatSub4058b0(p, fval) =
// *(float*)(p+0x5c) -= fval. So f: *(float*)(0x639d80 + idx*0xec + 0x5c) -= fval.
// VERBATIM naked (b0=0x8b; rel32 jmp -> mov eax,abs; jmp eax).
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl FloatSubThunk4058c0(void)
{
    __asm {
        mov   ecx, dword ptr [esp+4]
        imul  ecx, ecx, 0ECh
        add   ecx, 0639D80h
        mov   dword ptr [esp+4], ecx
        mov   eax, 04058B0h
        jmp   eax
    }
}
RH_ScopedInstall(FloatSubThunk4058c0, 0x004058c0);

// 0x0047d100 FUN_0047d100 (physics, BoundedThunk47d100) — NEAR-LEAF bounds-checked adjustor thunk to C3 0x4b5240.
// int f(idx, a2): if(idx<0 || idx>=0xc8) return; s = *(int*)(0x6c71d8 + idx*4); if(!s) return;
// tail-call 0x4b5240(s, a2) -> if(a2!=0) s[2]|=4. VERBATIM naked (b0=0x8b).
extern "C" __declspec(dllexport) __declspec(naked) int __cdecl BoundedThunk47d100(void)
{
    __asm {
        mov   eax, dword ptr [esp+4]
        test  eax, eax
        jl    Lret47d
        cmp   eax, 0C8h
        jge   Lret47d
        mov   eax, dword ptr [eax*4 + 06C71D8h]
        test  eax, eax
        je    Lret47d
        mov   dword ptr [esp+4], eax
        mov   eax, 04B5240h
        jmp   eax
    Lret47d:
        ret
    }
}
RH_ScopedInstall(BoundedThunk47d100, 0x0047d100);

// 0x005c95b0 FUN_005c95b0 (audio, BitRangeFill5c95b0) — PURE LEAF bitfield range clear/set.
//   arg1 (pbuf, [esp+4])  : pointer to a (uint8_t*) buffer pointer; buf = *(uint8_t**)arg1
//                           (0x005c9628 mov ecx,[ebp]; ebp=[arg1] @ 0x005c95f2)
//   arg2 (startbit,[esp+8]): start bit index
//   arg3 (nbits,[esp+0xc]) : bit count (length in bits)
//   arg4 (fill,[esp+0x10]) : nonzero -> set covered bits to 1; zero -> clear to 0
// Sets bits [startbit, startbit+nbits) of the little-endian bit-array buf to (fill!=0),
// preserving bits outside the range. Decode:
//   fill       = (arg4 != 0)                  (0x005c95bb test eax,eax; 0x005c95c0 setne)
//   start_byte = startbit >> 3                (0x005c95d1 shr edx,3)
//   start_bit  = startbit & 7                 (0x005c95c5 and al,7)
//   end_byte   = (startbit+nbits) >> 3        (0x005c95d4 lea; 0x005c95d9 shr ebx,3)
//   end_bit    = (startbit+nbits) & 7         (0x005c95d7 add al,cl; 0x005c95dc and al,7)
//   if (start_byte > end_byte) return         (0x005c95df cmp edx,ebx; 0x005c95eb ja)
//   for byte i = start_byte..end_byte:        (0x005c9670 cmp esi,ebx; 0x005c9672 jbe)
//     a = (i==start_byte)? start_bit : 0      (0x005c95f7..0x005c960b)
//     b = (i==end_byte)?   end_bit   : 8      (0x005c95f9 mov cl,8; 0x005c960f..0x005c9617)
//     if (a==0 && b==8) buf[i] = fill?0xFF:0  (0x005c961b..0x005c962f; dec/not idiom)
//     else for bit c=a..b-1:                  (0x005c9648 cmp; 0x005c9669 jb)
//       buf[i] = (buf[i] & ~(1<<c)) | (fill<<c)  (0x005c9655 not bl,and; 0x005c9664 or)
// C reimpl (data-loop; bit-for-bit identical final buffer state).
extern "C" __declspec(dllexport) void __cdecl BitRangeFill5c95b0(
        unsigned char **pbuf, unsigned int startbit, unsigned int nbits, int fillval)
{
    unsigned char *buf = *pbuf;
    unsigned char fill = (fillval != 0) ? 1 : 0;
    unsigned int start_byte = startbit >> 3;
    unsigned int start_bit  = startbit & 7;
    unsigned int end_byte   = (startbit + nbits) >> 3;
    unsigned int end_bit    = (startbit + nbits) & 7;
    if (start_byte > end_byte)
        return;
    for (unsigned int i = start_byte; i <= end_byte; ++i) {
        unsigned int a = (i == start_byte) ? start_bit : 0u;
        unsigned int b = (i == end_byte)   ? end_bit   : 8u;
        if (a == 0u && b == 8u) {
            buf[i] = fill ? (unsigned char)0xFF : (unsigned char)0x00;
        } else {
            for (unsigned int c = a; c < b; ++c) {
                buf[i] = (unsigned char)((buf[i] & ~(1u << c)) | ((unsigned int)fill << c));
            }
        }
    }
}
RH_ScopedInstall(BitRangeFill5c95b0, 0x005c95b0);

// 0x00418a00 FUN_00418a00 (render, StructInit418a00) — NEAR-LEAF struct initializer.
// The original takes its pointer in ESI (register arg) and:
//   memset(p, 0, 0x6c)                     (0x00418a00 push 0x6c; push esi; call C3 ZeroFillWrapper 0x4b6520)
//   *(u32*)(p+0x08) = 0x3d4ccccd  (0.05f)  (0x00418a08 mov eax,0x3d4ccccd; 0x00418a0d mov [esi+8],eax)
//   *(u32*)(p+0x0c) = 0x3d4ccccd  (0.05f)  (0x00418a10 mov [esi+0xc],eax)
//   p[0x10]=p[0x11]=p[0x12]=p[0x13] = 0xFF (0x00418a13 mov al,0xff; 0x00418a18..0x00418a21)
// Behavioral C reimpl as plain __cdecl(p) (the diff drives the original via an ESI
// trampoline and the reimpl via a stack-arg pointer — same observable buffer writes).
// memset is inlined (0x6c is a multiple of 4); identical to ZeroFillWrapper(p,0,0x6c).
//
// BUGFIX 2026-07-26 — ABI MISMATCH; SIXTH confirmed boot-AV crasher (index 1023;
// MASHED_HOOK_ONLY=0x00418A00 alone: 3/3 AV 0xC0000005 @ 6.88-7.23 s). The original
// reads its pointer ONLY from ESI (0x00418a02 `PUSH ESI`, 0x00418a0d/0x00418a10/
// 0x00418a18.. `MOV [ESI+n]`) and the game's callers push nothing, so this __cdecl body
// took a caller stack-garbage word as `p` and wrote 0x6c bytes through it.
// The C body keeps its stack ABI for the Frida harness; the INSTALLED hook is the naked
// ESI shim below. Same class as crashers #3 (0x0042ac00) and #5 (0x0042ad90).
extern "C" __declspec(dllexport) void __cdecl StructInit418a00(unsigned char *p)
{
    for (unsigned int i = 0; i < 0x6c; i += 4)
        *(unsigned int *)(p + i) = 0u;
    *(unsigned int *)(p + 0x08) = 0x3d4ccccdu;
    *(unsigned int *)(p + 0x0c) = 0x3d4ccccdu;
    p[0x10] = 0xFF;
    p[0x11] = 0xFF;
    p[0x12] = 0xFF;
    p[0x13] = 0xFF;
}

// Register-ABI shim — this, NOT StructInit418a00, is installed at 0x00418a00.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl StructInit418a00_RegAbi(void) {
    __asm {
        push esi                // p (register arg in the original)
        call StructInit418a00
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(StructInit418a00_RegAbi, 0x00418a00);  // ABI fixed 2026-07-26 (was __cdecl -> live boot AV)

// MATCH-S05: the body of 0x00421060 (StructZero421060) moved to Match/MatchS05_00421060.cpp,
// which compiles the byte-matched source re/match/s05/00421060.c (the register-ABI shim
// StructZero421060_RegAbi and its RH_ScopedInstall moved with it).

// ===== round 248 (2026-06-23) — NEAR-LEAF lane: vehicle-slot-getter family =====
// Four C2 near-leaves whose ONLY callees are the pure bounds-checked static-table
// getters VehicleSlotGetter 0x0046c7b0 (C3) / CarStatePairGet 0x0046cbb0 (C3) /
// Pred45bff0 0x0045bff0 (C3). Each is a VERBATIM naked port: the original's
// `call rel32 <callee>` is replaced by `mov eax,<callee_abs>; call eax` (byte-identical
// behavior; BOTH orig+reimpl hit the SAME real C3 callee at its absolute RVA at
// suspended-spawn). Callees read static tables (DAT_008815a4 / DAT_00881f90 /
// DAT_0088fc88) which early_window seeds -> non-degenerate. Anchored to MASHED.exe
// SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

// 0x0040ba60 FUN_0040ba60 (gameplay) — Active4Slots404ba60.
//   void f(int* out): for i=0..3: out[i]=0; if(VehicleSlotGetter(i)==0) out[i]=1;
//   Disasm (0x0040ba60..0x0040ba93):
//     56               push esi
//     57               push edi
//     8b 7c 24 0c      mov  edi,[esp+0xc]        ; arg out
//     33 f6            xor  esi,esi
//     eb 06            jmp  L70
//   L70:
//     56               push esi
//     c7 04 b7 00..    mov  dword ptr [edi+esi*4],0
//     e8 33 0d 06 00   call 0x0046c7b0           ; VehicleSlotGetter(i)  [->mov eax,abs;call eax]
//     83 c4 04         add  esp,4
//     85 c0            test eax,eax
//     75 07            jnz  L8b
//     c7 04 b7 01..    mov  dword ptr [edi+esi*4],1
//   L8b:
//     46               inc  esi
//     83 fe 04         cmp  esi,4
//     7c df            jl   L70
//     5f 5e c3         pop edi; pop esi; ret
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Active4Slots40ba60(int * /*out*/)
{
    __asm {
        push  esi
        push  edi
        mov   edi, dword ptr [esp+0Ch]
        xor   esi, esi
        jmp   La70
    La70:
        push  esi
        mov   dword ptr [edi+esi*4], 0
        mov   eax, 046C7B0h
        call  eax
        add   esp, 4
        test  eax, eax
        jnz   La8b
        mov   dword ptr [edi+esi*4], 1
    La8b:
        inc   esi
        cmp   esi, 4
        jl    La70
        pop   edi
        pop   esi
        ret
    }
}
RH_ScopedInstall(Active4Slots40ba60, 0x0040ba60);

// 0x0040b9a0 FUN_0040b9a0 (gameplay) — MaxScoreFlags40b9a0.
//   void f(int* out,int mode): find max m over DAT_008a94e0[0..3] (when mode!=0 only
//   slots i with VehicleSlotGetter(i)!=0 are eligible); then out[i]=(DAT_008a94e0[i]==m)?1:0.
//   Disasm (0x0040b9a0..0x0040b9fe) verbatim; only callee call rewritten.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl MaxScoreFlags40b9a0(int * /*out*/, int /*mode*/)
{
    __asm {
        push  ebx
        mov   ebx, dword ptr [esp+0Ch]     ; arg mode  (1 push so far -> +0xc)
        push  esi
        push  edi
        xor   edi, edi
        xor   esi, esi
        jmp   Lb_b0
    Lb_b0:
        test  ebx, ebx
        jz    Lb_c1
        push  esi
        mov   eax, 046C7B0h
        call  eax
        add   esp, 4
        test  eax, eax
        jz    Lb_ce
    Lb_c1:
        mov   eax, dword ptr [esi*4 + 08A94E0h]
        cmp   eax, edi
        jle   Lb_ce
        mov   edi, eax
    Lb_ce:
        inc   esi
        cmp   esi, 4
        jl    Lb_b0
        mov   eax, dword ptr [esp+10h]      ; arg out (3 pushes -> +0x10)
        mov   ecx, 08A94E0h
        sub   ecx, eax
        mov   edx, 4
    Lb_e4:
        mov   dword ptr [eax], 0
        cmp   dword ptr [ecx+eax*1], edi
        jnz   Lb_f5
        mov   dword ptr [eax], 1
    Lb_f5:
        add   eax, 4
        dec   edx
        jnz   Lb_e4
        pop   edi
        pop   esi
        pop   ebx
        ret
    }
}
RH_ScopedInstall(MaxScoreFlags40b9a0, 0x0040b9a0);

// MATCH-S10: the body of 0x0045cab0 (SlotAliveState45cab0) moved to Match/MatchS10_0045cab0.cpp,
// which compiles the byte-matched source re/match/s10/0045cab0.c.

// [moved] 0x0045c330 PredNot45c330 moved to Gameplay/PickupPredicates.cpp (D4-DC-INC-batch3, 2026-11-01): one body, both targets.

// ===== round 165 ===== (2D fixed-point delta, round-toward-zero bias)
// 0x005c6b60 — first row released by the U-9023 skip-band correction (a448e116); it had
// been silently excluded from the frontier since 2026-06-15. Plate records kind: GAME /
// library_match: (none — game code), and it is a true leaf (0 callees -> leaf exemption).
// Caller 0x005c7330 is audio C2, satisfying the C3 caller gate.
//
// void/u32 fn(int* p, u32 x, u32 y), all three args on the stack (__cdecl):
//   0x005c6b60 MOV ECX,[ESP+4]            p
//   0x005c6b64 MOV EAX,[ESP+8]            x
//   0x005c6b7c MOV EAX,[ESP+0x10]         y  (after 0x005c6b6e PUSH ESI, = entry [ESP+0xc])
//   dx = (x<<8) - p[1]; CDQ/AND EDX,0x3f/ADD  => dx += (dx<0 ? 0x3f : 0)   0x005c6b68..0x005c6b75
//   dy = (y<<8) - p[2]; same bias                                          0x005c6b7c..0x005c6b89
//   SAR ESI,6 / SAR EAX,6                 dx >>= 6; dy >>= 6               0x005c6b8b/0x005c6b8e
//   p[3] = dx; p[4] = dy;                                                  0x005c6b91/0x005c6b94
//   p[0] = (dx || dy) ? 0x40 : 0;                                          0x005c6b97..0x005c6baa
// The +0x3f bias before an arithmetic >>6 is the standard round-toward-zero correction for
// a signed divide by 64 (0x3f = 64-1, added only when the value is negative).
//
// RETURN VALUE: declared u32, NOT void. At BOTH RET paths EAX equals the dword just stored
// to p[0] — 0x005c6ba0 `MOV [ECX],EAX` with EAX==0 (the zeroed dy), and 0x005c6ba8 with
// EAX==0x40. Declaring this void would leave EAX undefined for any caller that consumes it;
// that exact mistake was a confirmed boot-AV crasher at 0x004b40c0 (see memory
// feedback_installed_hook_abi_mismatch, "lost-return-value sub-class").
extern "C" __declspec(dllexport) std::uint32_t __cdecl Delta5c6b60(
        int* p, std::uint32_t x, std::uint32_t y) {
    int dx = static_cast<int>(x << 8) - p[1];
    if (dx < 0) dx += 0x3f;
    int dy = static_cast<int>(y << 8) - p[2];
    if (dy < 0) dy += 0x3f;
    dx >>= 6;
    dy >>= 6;
    p[3] = dx;
    p[4] = dy;
    const std::uint32_t flag = (dx != 0 || dy != 0) ? 0x40u : 0u;
    p[0] = static_cast<int>(flag);
    return flag;
}
RH_ScopedInstall(Delta5c6b60, 0x005c6b60);
