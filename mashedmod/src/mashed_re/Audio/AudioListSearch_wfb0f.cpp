// Mashed RE — Audio list search (wf_b0f68acd-63f-44, 2026-06-26)
// FUN_005aa8a0: scan DAT_007dccf0 audio list for the first node whose key field
// (at offset -4 from the node's "next" pointer, i.e. *(puVar2-1)) equals param_1
// AND whose field at +0x1c from node-1 (i.e. *(puVar2-1+0x1c) = *(puVar2+0x18))
// has bit 0x4 set.  Returns that field value on match, 0 if not found.
//
// Calling convention: cdecl, 1 arg, returns uint32.
// List walker: FUN_005aaac0 (C2, sentinel-list iterator at 0x005aaac0).
// Inline callback: LAB_005aa8d0 (data-referenced; reimplemented below).
// Ref: re/analysis/bucket_audio_005a7b60_005ab620/005aa8a0.md

#include "../Core/HookSystem.h"
#include <cstdint>

// Context struct for the inline callback.
// Mirrors the 2-dword stack block {local_8=key, local_4=result} that the
// original function allocates and passes by pointer to FUN_005aaac0.
struct AudioSearchCtx {
    uint32_t key;    // search key = param_1 (copied into local_8 at 0x005aa8b1)
    uint32_t result; // output slot = local_4 (zeroed at 0x005aa8b5; written by callback)
};

// Reimplementation of LAB_005aa8d0 — the inline callback passed to FUN_005aaac0.
// Called for each node as: cb(puVar2 - 1, &ctx)
// where puVar2 is the undefined4* "next" field pointer for the current node.
// So arg1 = puVar2 - 1 = 4 bytes before the "next" field = the node's key slot.
//
// Disassembly citations (all offsets in original/MASHED.exe):
//   0x005aa8d0  MOV EAX,[ESP+4]       ; arg1 = node_m1
//   0x005aa8d4  MOV ECX,[ESP+8]       ; arg2 = ctx
//   0x005aa8d9  MOV EDX,[EAX]         ; EDX = node_m1[0] = node key field
//   0x005aa8db  MOV ESI,[ECX]         ; ESI = ctx->key
//   0x005aa8dd  CMP EDX,ESI           ; compare
//   0x005aa8e0  JNZ 0x005aa8ef        ; mismatch -> return 1
//   0x005aa8e2  MOV EAX,[EAX+0x1c]   ; EAX = node_m1[7] = result field
//   0x005aa8e5  TEST AL,0x4           ; bit 2 check
//   0x005aa8e7  JZ 0x005aa8ef         ; bit unset -> return 1
//   0x005aa8e9  MOV [ECX+4],EAX       ; ctx->result = field value
//   0x005aa8ec  XOR EAX,EAX           ; return 0 (stop)
//   0x005aa8ef  MOV EAX,1             ; return 1 (continue)
static int __cdecl AudioListSearchCb(uint32_t* node_m1, AudioSearchCtx* ctx) {
    uint32_t node_key = node_m1[0];    // *(puVar2-1); matches 0x005aa8d9
    if (node_key == ctx->key) {        // CMP at 0x005aa8dd
        uint32_t field = node_m1[7];   // *(puVar2-1+0x1c); matches 0x005aa8e2
        if (field & 0x4u) {            // TEST AL,0x4 at 0x005aa8e5
            ctx->result = field;       // [ECX+4]=EAX at 0x005aa8e9
            return 0;                  // XOR EAX,EAX; RET at 0x005aa8ec/ee
        }
    }
    return 1;                          // MOV EAX,1; RET at 0x005aa8ef/f4
}

// FUN_005aaac0 type: sentinel-list walker
// void __cdecl FUN_005aaac0(code* callback, void* ctx)
typedef void (__cdecl *AudioListWalkFn)(void* cb, void* ctx);

// 0x005aa8a0  FUN_005aa8a0  (42 bytes, cdecl)
// uint32_t FUN_005aa8a0(uint32_t param_1)
//
// Disassembly citations:
//   0x005aa8a0  SUB ESP,0x8           ; allocate {local_8, local_4}
//   0x005aa8a3  MOV EAX,[ESP+0xc]     ; EAX = param_1
//   0x005aa8a7  LEA ECX,[ESP]         ; ECX = &local_8 (= &ctx)
//   0x005aa8ab  PUSH ECX              ; arg2 for FUN_005aaac0
//   0x005aa8ac  PUSH 0x5aa8d0         ; arg1 = callback pointer
//   0x005aa8b1  MOV [ESP+0x8],EAX     ; local_8 = param_1 (= ctx.key)
//   0x005aa8b5  MOV [ESP+0xc],0x0     ; local_4 = 0   (= ctx.result)
//   0x005aa8bd  CALL 0x005aaac0       ; FUN_005aaac0(callback, &ctx)
//   0x005aa8c2  MOV EAX,[ESP+0xc]     ; EAX = local_4 = ctx.result
//   0x005aa8c6  ADD ESP,0x10          ; clean locals + pushed args (cdecl)
//   0x005aa8c9  RET
extern "C" __declspec(dllexport) uint32_t __cdecl AudioListSearch(uint32_t param_1) {
    AudioSearchCtx ctx = { param_1, 0u };
    auto walk = reinterpret_cast<AudioListWalkFn>(0x005aaac0u);
    walk(reinterpret_cast<void*>(&AudioListSearchCb), &ctx);
    return ctx.result;
}

RH_ScopedInstall(AudioListSearch, 0x005aa8a0);  // wfb0f 2026-06-26
