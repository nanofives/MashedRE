// Mashed RE — D3D9 deferred render-/texture-/sampler-state cache (cluster core).
// Reimpl-first cluster module (workstream reimpl/d3d9-state-cache, 2026-06-25).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Shared struct contract: re/analysis/d3d9_state_cache_struct.md (every global +
// stride + vtable offset cited there). This file reimplements the cache-core
// functions that read/write those documented globals.
//
// The subsystem is a deferred state cache in front of IDirect3DDevice9*
// (DAT_007d4110). Three sub-caches:
//   * Render-state (RS): unified desired/dirty array DAT_007d57f8 (8B/state:
//     [+0]=desired,[+4]=dirty), applied DAT_007d54b0 (4B/state), dirty-queue
//     DAT_007d5168 (count DAT_007d6c14). Flushed via SetRenderState (vtbl+0xe4).
//   * Texture-stage-state (TSS): desired/dirty DAT_007d4720 (8B/slot, slot=
//     stage*0x21+state), applied DAT_007d5e88 (4B), pair-queue DAT_007d62a8
//     (count DAT_007d6c18). Flushed via SetTextureStageState (vtbl+0x10c).
//   * Sampler-state: DIRECT (not deferred) — DAT_007d4f60 value cache
//     (idx=state+sampler*0xe); writes device immediately via SetSamplerState
//     (vtbl+0x114).
//
// Functions in this file:
//   0x004d6910  D3d9State_Get          — 30-case input-cache getter (rosetta)
//   0x004d7ac0  D3d9State_FogBlendToggle — fog-blend enable/disable (queue-only)
//   0x004d5570  D3d9State_SetSampler   — direct SetSamplerState w/ value cache
//   0x004d6b90  D3d9State_SetStage0Filter — stage-0 min/mag/mip filter setter
//   0x004d53b0  D3d9State_Flush        — drain RS + TSS dirty queues to device
//
// Already-C3 cluster members reimplemented elsewhere (do NOT redo): the
// queue-mark setters Mark4d5480/Set4d54f0/Set4d6c40/Set4d6c90/Set4d6Cx and the
// RS getter Get4d54d0 — see Render\RenderStateSettersA.cpp / RenderStateSettersB.cpp
// and Util\PromoLoop_sessionB.cpp.

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {

// ---------------------------------------------------------------------------
// Device vtable helpers. DAT_007d4110 = IDirect3DDevice9*; *device = its vtable.
// COM methods are __stdcall with `this` as the implicit first arg. The original
// re-reads DAT_007d4110 and re-derefs the vtable at each call site, so these
// helpers do the same (no caching) to stay structurally faithful.
// ---------------------------------------------------------------------------
typedef long(__stdcall* D3DCall2)(void*, unsigned, unsigned);
typedef long(__stdcall* D3DCall3)(void*, unsigned, unsigned, unsigned);

inline void Dev_SetRenderState(unsigned state, unsigned value) {        // vtbl+0xe4
    void* dev = *reinterpret_cast<void**>(0x007d4110u);
    void** vt = *reinterpret_cast<void***>(dev);
    reinterpret_cast<D3DCall2>(vt[0xe4 / 4])(dev, state, value);
}
inline void Dev_SetTextureStageState(unsigned stage, unsigned type, unsigned value) { // vtbl+0x10c
    void* dev = *reinterpret_cast<void**>(0x007d4110u);
    void** vt = *reinterpret_cast<void***>(dev);
    reinterpret_cast<D3DCall3>(vt[0x10c / 4])(dev, stage, type, value);
}
inline void Dev_SetSamplerState(unsigned sampler, unsigned type, unsigned value) {    // vtbl+0x114
    void* dev = *reinterpret_cast<void**>(0x007d4110u);
    void** vt = *reinterpret_cast<void***>(dev);
    reinterpret_cast<D3DCall3>(vt[0x114 / 4])(dev, sampler, type, value);
}

inline std::uint32_t G(std::uintptr_t a) { return *reinterpret_cast<std::uint32_t*>(a); }
inline void SetG(std::uintptr_t a, std::uint32_t v) { *reinterpret_cast<std::uint32_t*>(a) = v; }

// Shared RS dirty-queue base + write index.
inline std::uint32_t* RsQueue()  { return reinterpret_cast<std::uint32_t*>(0x007d5168u); }
inline std::uint32_t* RsCount()  { return reinterpret_cast<std::uint32_t*>(0x007d6c14u); }

} // namespace

// ---------------------------------------------------------------------------
// D3d9State_Get — 0x004d6910  (516 B)
//
// undefined4 FUN_004d6910(int enum, int *out)
// Jump-table dispatch (enum 1..0x1e). Pure read of a per-control input-cache
// global into *out; returns 1 on a recognized enum, 0 otherwise.
//   0x004d6910  8b 44 24 04   mov eax,[esp+4]   ; enum
//   0x004d6914  48            dec eax
//   0x004d6915  83 f8 1d      cmp eax,0x1d
//   0x004d6918  77 73         ja  default(0x4d698d: xor eax,eax; ret)
//   0x004d691a  ff 24 85 18 6b 4d 00  jmp [eax*4 + 0x4d6b18]   ; jump table
// Case 2 special: if DAT_007d6b34 == DAT_007d6b38 -> *out=that, ret 1; else
//   *out=0, ret 0 (0x4d6983: mov [eax],0; 0x4d698d xor eax,eax; ret).
// Enums 5/0x12/0x13 fall to default (no *out write, ret 0).
// Enum 0x11 reads DAT_007d6b24 via fld/fstp (float copy) — bit-identical to a
//   dword copy for all non-SNaN values (real render-state values are never SNaN).
// Pure leaf (no callees). Caller FUN_004d07b0 (C2). Full enum->global map +
// case-2 / float-case notes: re/analysis/d3d9_state_cache_struct.md §5.
// ---------------------------------------------------------------------------

// 0x004d6910
extern "C" __declspec(dllexport) std::uint32_t __cdecl
D3d9State_Get(std::uint32_t state_enum, std::uint32_t* out)
{
    switch (state_enum) {
    case 1:    *out = G(0x007d6b30u); return 1;
    case 2:
        if (G(0x007d6b34u) == G(0x007d6b38u)) { *out = G(0x007d6b34u); return 1; }
        *out = 0; return 0;
    case 3:    *out = G(0x007d6b34u); return 1;
    case 4:    *out = G(0x007d6b38u); return 1;
    case 6:    *out = G(0x007d6aecu); return 1;
    case 7:    *out = G(0x007d6b2cu); return 1;
    case 8:    *out = G(0x007d6ae8u); return 1;
    case 9:    *out = G(0x007d6b3cu); return 1;
    case 0xa:  *out = G(0x007d6bf0u); return 1;
    case 0xb:  *out = G(0x007d6bf4u); return 1;
    case 0xc:  *out = G(0x007d6b10u); return 1;
    case 0xd:  *out = G(0x007d6b40u); return 1;
    case 0xe:  *out = G(0x007d6b1cu); return 1;
    case 0xf:  *out = G(0x007d6b28u); return 1;
    case 0x10: *out = G(0x007d6b20u); return 1;
    case 0x11: *out = G(0x007d6b24u); return 1;   // float in orig (fld/fstp); bit-identical for non-SNaN
    case 0x14: *out = G(0x007d6b18u); return 1;
    case 0x15: *out = G(0x007d6af0u); return 1;
    case 0x16: *out = G(0x007d6af4u); return 1;
    case 0x17: *out = G(0x007d6af8u); return 1;
    case 0x18: *out = G(0x007d6afcu); return 1;
    case 0x19: *out = G(0x007d6b00u); return 1;
    case 0x1a: *out = G(0x007d6b04u); return 1;
    case 0x1b: *out = G(0x007d6b08u); return 1;
    case 0x1c: *out = G(0x007d6b0cu); return 1;
    case 0x1d: *out = G(0x007d6bf8u); return 1;
    case 0x1e: *out = G(0x007d58b8u); return 1;
    default:   return 0;                          // enums 5/0x12/0x13/unknown: no write
    }
}

RH_ScopedInstall(D3d9State_Get, 0x004d6910);

// ---------------------------------------------------------------------------
// D3d9State_FogBlendToggle — 0x004d7ac0  (252 B)
//
// void FUN_004d7ac0(int enable). Pure queue-only (NO device call). Toggles the
// fog-blend input cache DAT_007d6aec and, gated by the alpha-blend cache
// DAT_007d6ae8, marks RS opcode 7 (store DAT_007d5830/dirty DAT_007d5834) and
// always marks RS opcode 0x17 (store DAT_007d58b0/dirty DAT_007d58b4).
// Byte-verified (0x004d7ac0..0x004d7bbb):
//   enable!=0 & DAT_007d6aec==0 path:
//     if (DAT_007d6ae8==0) { DAT_007d5830=1; if(!DAT_007d5834){q[c]=7;c++;DAT_007d5834=1;} }
//     DAT_007d58b0=4; if(!DAT_007d58b4){q[c]=0x17;c++;DAT_007d58b4=1;} DAT_007d6aec=1;
//   enable==0 & DAT_007d6aec!=0 path (mirror): stores 0 then 8, marks 7 then 0x17,
//     DAT_007d6aec=0.
// Cache-HIT (enable matches current DAT_007d6aec polarity) writes nothing.
// NOTE the orig sets DAT_007d5830 (op7 store) on EVERY taken branch but only
//   pushes opcode 7 when DAT_007d6ae8==0 AND DAT_007d5834==0 (see disasm: the
//   `mov [0x7d5830],eax` precedes the dirty-flag test). Reproduced exactly below.
// Caller FUN_004d7480 case 6 (in-module). Contract §2.
// ---------------------------------------------------------------------------

// 0x004d7ac0
extern "C" __declspec(dllexport) void __cdecl
D3d9State_FogBlendToggle(std::uint32_t enable)
{
    if (enable == 0) {
        if (G(0x007d6aecu) != 0) {
            if (G(0x007d6ae8u) == 0) {
                SetG(0x007d5830u, 0);                       // op7 store = 0
                if (G(0x007d5834u) == 0) {
                    std::uint32_t c = *RsCount();
                    SetG(0x007d5834u, 1);
                    RsQueue()[c] = 7; *RsCount() = c + 1;
                }
            }
            SetG(0x007d58b0u, 8);                           // op0x17 store = 8
            if (G(0x007d58b4u) == 0) {
                std::uint32_t c = *RsCount();
                SetG(0x007d58b4u, 1);
                RsQueue()[c] = 0x17; *RsCount() = c + 1;
            }
            SetG(0x007d6aecu, 0);
        }
    } else if (G(0x007d6aecu) == 0) {
        if (G(0x007d6ae8u) == 0) {
            SetG(0x007d5830u, 1);                           // op7 store = 1
            if (G(0x007d5834u) == 0) {
                std::uint32_t c = *RsCount();
                SetG(0x007d5834u, 1);
                RsQueue()[c] = 7; *RsCount() = c + 1;
            }
        }
        SetG(0x007d58b0u, 4);                               // op0x17 store = 4
        if (G(0x007d58b4u) == 0) {
            std::uint32_t c = *RsCount();
            SetG(0x007d58b4u, 1);
            RsQueue()[c] = 0x17; *RsCount() = c + 1;
        }
        SetG(0x007d6aecu, 1);
    }
}

RH_ScopedInstall(D3d9State_FogBlendToggle, 0x004d7ac0);

// ---------------------------------------------------------------------------
// D3d9State_SetSampler — 0x004d5570  (62 B)
//
// void FUN_004d5570(int sampler, int state, int value). DIRECT sampler-state
// write with a value cache (NOT deferred). idx = state + sampler*0xe.
//   0x004d557a  lea eax,[ecx*8]; sub eax,ecx        ; sampler*7
//   0x004d5583  lea edx,[esi+eax*2]                 ; state + sampler*0xe
//   0x004d558a  cmp [edx*4+0x7d4f60], value ; je ret ; value-cache change-detect
//   0x004d5593  mov [edx*4+0x7d4f60], value          ; update cache
//   0x004d55a6  call [device->vtbl+0x114](dev,sampler,state,value)  ; SetSamplerState
// Callers FUN_004d5bc0/FUN_004d6200 (in-module). Contract §4.
// ---------------------------------------------------------------------------

// 0x004d5570
extern "C" __declspec(dllexport) void __cdecl
D3d9State_SetSampler(std::uint32_t sampler, std::uint32_t state, std::uint32_t value)
{
    std::uint32_t idx = state + sampler * 0xeu;
    std::uint32_t* cache = reinterpret_cast<std::uint32_t*>(0x007d4f60u);
    if (cache[idx] != value) {
        cache[idx] = value;
        Dev_SetSamplerState(sampler, state, value);
    }
}

RH_ScopedInstall(D3d9State_SetSampler, 0x004d5570);

// ---------------------------------------------------------------------------
// D3d9State_SetStage0Filter — 0x004d6b90  (175 B)
//
// undefined4 FUN_004d6b90(int filter_idx). Stage-0 min/mag/mip filter setter.
// One-time aniso clamp + table-mapped filter push, all to sampler 0.
//   0x004d6b90  cmp [0x7d6b44],1; jle ...            ; SIGNED: if((int)>1)
//                 [0x7d6b44]=1; [0x7d4f88]=1; SetSamplerState(0,0xa,1)  ; aniso clamp
//   0x004d6bc5  if (filter_idx != [0x7d6b3c]) {       ; filter-index cache
//                 edi = [0x5d8cac + filter_idx*8]      ; mag value
//                 esi = [0x5d8ca8 + filter_idx*8]      ; min/mip value
//                 [0x7d6b3c]=filter_idx;
//                 [0x7d4f78]=esi; SetSamplerState(0,6,esi)   ; D3DSAMP_MIPFILTER
//                 [0x7d4f74]=esi; SetSamplerState(0,5,esi)   ; D3DSAMP_MINFILTER
//                 if ([0x7d4f7c] != edi) { [0x7d4f7c]=edi; SetSamplerState(0,7,edi) } ; MAGFILTER
//               }
//   return 1.
// Tables 0x005d8ca8 (min, stride 8) / 0x005d8cac (mag, =base+4). Caller
// FUN_004d7480 case 9 (in-module). Contract §4.
// ---------------------------------------------------------------------------

// 0x004d6b90
extern "C" __declspec(dllexport) std::uint32_t __cdecl
D3d9State_SetStage0Filter(std::uint32_t filter_idx)
{
    if (static_cast<std::int32_t>(G(0x007d6b44u)) > 1) {   // SIGNED cmp/jle
        SetG(0x007d6b44u, 1);
        SetG(0x007d4f88u, 1);
        Dev_SetSamplerState(0, 0xa, 1);
    }
    if (filter_idx != G(0x007d6b3cu)) {
        std::uint32_t mag = *reinterpret_cast<std::uint32_t*>(0x005d8cacu + filter_idx * 8u);
        std::uint32_t mn  = *reinterpret_cast<std::uint32_t*>(0x005d8ca8u + filter_idx * 8u);
        SetG(0x007d6b3cu, filter_idx);
        SetG(0x007d4f78u, mn); Dev_SetSamplerState(0, 6, mn);
        SetG(0x007d4f74u, mn); Dev_SetSamplerState(0, 5, mn);
        if (G(0x007d4f7cu) != mag) {
            SetG(0x007d4f7cu, mag);
            Dev_SetSamplerState(0, 7, mag);
        }
    }
    return 1;
}

RH_ScopedInstall(D3d9State_SetStage0Filter, 0x004d6b90);

// ---------------------------------------------------------------------------
// D3d9State_Flush — 0x004d53b0  (194 B)
//
// void FUN_004d53b0(void). Drains both deferred queues to the device.
//   RS path (if DAT_007d6c14 != 0): for each queued state index i in
//     DAT_007d5168[0..count): desired=DAT_007d57f8[i*2]; applied=DAT_007d54b0[i];
//     clear dirty DAT_007d57fc[i*2]=0; if(applied!=desired){applied=desired;
//     SetRenderState(i,desired)}.  Then DAT_007d6c14=0.
//   TSS path (if DAT_007d6c18 != 0): for each queued pair in DAT_007d62a8:
//     stage=[k*2]; state=[k*2+1]; slot=stage*0x21+state;
//     desired=DAT_007d4720[slot*2]; applied=DAT_007d5e88[slot];
//     clear dirty DAT_007d4724[slot*2]=0; if(applied!=desired){applied=desired;
//     SetTextureStageState(stage,state,desired)}.  Then DAT_007d6c18=0.
// Byte-verified 0x004d53b0..0x004d5471 (queue/array indexing, vtbl+0xe4/+0x10c).
// Callers FUN_004d5bc0/6200 (in-module) + FUN_004db770/00499d90/004cb2f0 (C2).
// Contract §2/§3.
// ---------------------------------------------------------------------------

// 0x004d53b0
extern "C" __declspec(dllexport) void __cdecl
D3d9State_Flush(void)
{
    // RS path.
    std::uint32_t rsCount = G(0x007d6c14u);
    if (rsCount != 0) {
        std::uint32_t* queue   = reinterpret_cast<std::uint32_t*>(0x007d5168u);
        std::uint32_t* desired = reinterpret_cast<std::uint32_t*>(0x007d57f8u); // [i*2]=val,[i*2+1]=dirty
        std::uint32_t* applied = reinterpret_cast<std::uint32_t*>(0x007d54b0u); // [i]=applied
        for (std::uint32_t k = 0; k < rsCount; ++k) {
            std::uint32_t i = queue[k];
            std::uint32_t want = desired[i * 2];
            std::uint32_t have = applied[i];
            desired[i * 2 + 1] = 0;                  // clear dirty
            if (have != want) {
                applied[i] = want;
                Dev_SetRenderState(i, want);
            }
            rsCount = G(0x007d6c14u);                // orig re-reads count each iter
        }
        SetG(0x007d6c14u, 0);
    }
    // TSS path.
    std::uint32_t tssCount = G(0x007d6c18u);
    if (tssCount != 0) {
        std::uint32_t* pq      = reinterpret_cast<std::uint32_t*>(0x007d62a8u); // [k*2]=stage,[k*2+1]=state
        std::uint32_t* desired = reinterpret_cast<std::uint32_t*>(0x007d4720u); // [slot*2]=val,[slot*2+1]=dirty
        std::uint32_t* applied = reinterpret_cast<std::uint32_t*>(0x007d5e88u); // [slot]=applied
        for (std::uint32_t k = 0; k < tssCount; ++k) {
            std::uint32_t stage = pq[k * 2];
            std::uint32_t state = pq[k * 2 + 1];
            std::uint32_t slot  = stage * 0x21u + state;
            std::uint32_t want = desired[slot * 2];
            std::uint32_t have = applied[slot];
            desired[slot * 2 + 1] = 0;               // clear dirty
            if (have != want) {
                applied[slot] = want;
                Dev_SetTextureStageState(stage, state, want);
            }
            tssCount = G(0x007d6c18u);               // orig re-reads count each iter
        }
        SetG(0x007d6c18u, 0);
    }
}

RH_ScopedInstall(D3d9State_Flush, 0x004d53b0);
