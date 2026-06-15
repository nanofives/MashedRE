// Mashed RE — scenario-attach leaf reimplementations (c3-batch-sa1 session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// These functions read arrays/objects that are ZERO at the main menu, so a
// menu-attach A/B gives a false-GREEN (INCONCLUSIVE-DEGENERATE). They are
// verified attached inside a LIVE Quick Battle race (scenario:'race' in
// hooks_registry.py) where the state is populated. See
// re/analysis/scenario_attach_lane.md and the probe snapshot
// re/analysis/scenario_attach_probe_2026-06-12.json.
//
// Every reimplementation is a verbatim transcription of the Ghidra
// decompilation (master Mashed.gpr, read read-only from Mashed_pool1 on
// 2026-06-15) and matches the original __cdecl frames so the inline-JMP
// redirect lines up. RVA + body range cited per function.
//
// INSTALLED + VERIFIED (void_write_observe, scenario:'race'):
//   0x0041c010  HudConstTableInitAndSweep  — copies 24-byte const block to
//               &DAT_005f334c, sweeps 2 records calling FUN_0041b770 (__fastcall),
//               resets DAT_0063cda0.  Observe 0x005f334c (= 0x3ee66666).
//   0x0041d930  HudSlideBillboardTick      — RW 2D billboard with time-driven
//               Y-lerp on DAT_0063d580; advances frame counter + time accumulator.
//               Observe 0x0063d584 (frame counter += 1).
//   0x00423480  AiFilenameBuild            — builds "AI%d.AI" for the current
//               track into DAT_00644110 (no disk IO).  Observe 0x00644110.
//
// AUTHORED BUT NOT INSTALLED — verification-blocked, queued for a future harness
// (faithful transcriptions, build-clean; same pattern as Frontend/MenuLeaves_af4):
//   0x0047c160  CameraPathNodeContainTest  — void f(int queryObj); derefs
//               *(queryObj+4). run_diff's void_write_observe passes no args and
//               int_scalar derefs an arbitrary int -> AV. Needs a NEW arg_type
//               that passes a valid in-race entity pointer (e.g. deref a static
//               vehicle-0 global). Queued.
//   0x00418860  AiPerFrameTick             — void f(void) AI dispatcher; all
//               side-effects are inside callees that write DYNAMIC heap state
//               (FUN_0040e480 writes *(PTR_PTR_005f2770 + idx*4 + 0x34)). No
//               static global observable exists for void_write_observe /
//               state_machine_observe (which only snapshot fixed addresses).
//               Needs a heap-snapshot harness. Queued.
//
// Analysis notes:
//   re/analysis/bucket_util_0040e4b0_0042f790/0x0041c010.md
//   re/analysis/bucket_util_0040e4b0_0042f790/0x0041d930.md
//   re/analysis/ai_path_following/0x00423480.md
//   re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0047c160.md
//   re/analysis/ai_path_following/0x00418860.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (RVAs into MASHED.exe)
// ---------------------------------------------------------------------------

// 0x0041b770  FUN_0041b770 — __fastcall(record*): copies a mode-indexed vec3 from
//             &DAT_005f334c into the record at +0x150. Receives the record pointer
//             in ECX (asm 0x0041c060: MOV ECX,ESI; CALL 0x0041b770). [C2]
static auto* const s_F0041b770 =
    reinterpret_cast<void(__fastcall*)(int)>(0x0041b770);

// 0x004c51a0  FUN_004c51a0 — RwMatrixTranslate(float* mat, float* vec, int combineOp). [C1]
static auto* const s_F004c51a0 =
    reinterpret_cast<void(__cdecl*)(float*, float*, int)>(0x004c51a0);

// 0x004c1480  FUN_004c1480 — applies a matrix to an RW object's frame, marks dirty.
//             (int obj, void* mat, int flag); return value unused here. [C1]
static auto* const s_F004c1480 =
    reinterpret_cast<int(__cdecl*)(int, void*, int)>(0x004c1480);

// 0x004c13e0  FUN_004c13e0 — (int obj, float* color, int flag). [C1]
static auto* const s_F004c13e0 =
    reinterpret_cast<void(__cdecl*)(int, float*, int)>(0x004c13e0);

// 0x00426c00  FUN_00426c00 — returns DAT_00644158 (current track index). [C1]
static auto* const s_F00426c00 =
    reinterpret_cast<int(__cdecl*)(void)>(0x00426c00);

// 0x004a2b60  FUN_004a2b60 — snprintf-style formatter (dst, fmt, ...). [C2]
static auto* const s_F004a2b60 =
    reinterpret_cast<int(__cdecl*)(char*, const char*, ...)>(0x004a2b60);

// Callees for the NOT-INSTALLED transcriptions below.
// 0x004c0ed0  FUN_004c0ed0 — resolve a transform/matrix from a handle.
static auto* const s_F004c0ed0 =
    reinterpret_cast<int(__cdecl*)(int)>(0x004c0ed0);
// 0x004c1b40  FUN_004c1b40 — containment/region test (query, nodeBound).
static auto* const s_F004c1b40 =
    reinterpret_cast<int(__cdecl*)(int, void*)>(0x004c1b40);
// 0x0047bb10  FUN_0047bb10 — per-node compute writing the two output cursors.
static auto* const s_F0047bb10 =
    reinterpret_cast<void(__cdecl*)(void*, int, void*, void*)>(0x0047bb10);
// 0x0042f6a0  FUN_0042f6a0 — game-mode/round-type getter (int). [C3]
static auto* const s_F0042f6a0 =
    reinterpret_cast<int(__cdecl*)(void)>(0x0042f6a0);
// 0x0046c7b0  FUN_0046c7b0 — entity alive/present flag getter (idx -> int). [C3]
static auto* const s_F0046c7b0 =
    reinterpret_cast<int(__cdecl*)(int)>(0x0046c7b0);
// 0x0040e480  FUN_0040e480 — entity state-setter (idx, state). [C3]
static auto* const s_F0040e480 =
    reinterpret_cast<void(__cdecl*)(int, int)>(0x0040e480);
// 0x004177b0  FUN_004177b0 — AI pre-tick (race-angle update + rubber-banding). [C2]
static auto* const s_F004177b0 =
    reinterpret_cast<void(__cdecl*)(void)>(0x004177b0);
// 0x0040e470  FUN_0040e470 — vehicle type getter (idx -> int). [C3]
static auto* const s_F0040e470 =
    reinterpret_cast<int(__cdecl*)(int)>(0x0040e470);
// 0x00443080  FUN_00443080 — mode flag getter (int). [C3]
static auto* const s_F00443080 =
    reinterpret_cast<int(__cdecl*)(void)>(0x00443080);
// 0x00418560  FUN_00418560 — per-vehicle AI step (idx, type). [C2]
static auto* const s_F00418560 =
    reinterpret_cast<void(__cdecl*)(int, int)>(0x00418560);

// ---------------------------------------------------------------------------
// Global addresses (cited from the decompilation)
// ---------------------------------------------------------------------------

// 0x005f334c  DAT_005f334c — 24-byte (two float-triple) const destination.
static const std::uintptr_t k_005f334c = 0x005f334cu;
// 0x0063cab8 / 0x0063cd90 / 0x16c — record table base / end / byte stride (2 records).
static const std::uintptr_t k_0063cab8 = 0x0063cab8u;
static const std::uintptr_t k_0063cd90 = 0x0063cd90u;
// 0x0063cda0  _DAT_0063cda0 — counter reset to 0.
static const std::uintptr_t k_0063cda0 = 0x0063cda0u;

// 0x0063d580  DAT_0063d580 — HUD RW object.
static const std::uintptr_t k_0063d580 = 0x0063d580u;
// 0x0063d584  DAT_0063d584 — frame counter (+= 1).
static const std::uintptr_t k_0063d584 = 0x0063d584u;
// 0x0063d588  DAT_0063d588 — time accumulator (drives Y-lerp; += _DAT_007f100c).
static const std::uintptr_t k_0063d588 = 0x0063d588u;
// Y-lerp + color-scale + frame-delta constants (.rdata floats).
static const std::uintptr_t k_005cd22c = 0x005cd22cu; // lerp time threshold (start)
static const std::uintptr_t k_005cc564 = 0x005cc564u; // lerp slope
static const std::uintptr_t k_005cd228 = 0x005cd228u; // lerp base offset
static const std::uintptr_t k_005cd118 = 0x005cd118u; // color scale
static const std::uintptr_t k_007f100c = 0x007f100cu; // per-frame scaled delta

// 0x0064410c  DAT_0064410c — path buffer base (4-byte prefix preserved).
static const std::uintptr_t k_0064410c = 0x0064410cu;
// 0x00644110  DAT_00644110 — filename-part start within the path buffer.
static const std::uintptr_t k_00644110 = 0x00644110u;

// 0x006c2fe8 / 0x006c1228 / 0x006c27a8 / 0x006c2fa8 — camera-path node table +
//             per-node output arrays (NOT-INSTALLED transcription).
static const std::uintptr_t k_006c2fe8 = 0x006c2fe8u; // node count
static const std::uintptr_t k_006c1228 = 0x006c1228u; // node records (stride 0x56 dwords)
static const std::uintptr_t k_006c27a8 = 0x006c27a8u; // per-node output A
static const std::uintptr_t k_006c2fa8 = 0x006c2fa8u; // per-node output B/flag

// 0x00801ca0 / 0x007f0fd0 — AI tick guards (NOT-INSTALLED transcription).
static const std::uintptr_t k_00801ca0 = 0x00801ca0u; // race-lines count guard (>3)
static const std::uintptr_t k_007f0fd0 = 0x007f0fd0u; // game mode selector

// ---------------------------------------------------------------------------
// HudConstTableInitAndSweep  --  0x0041c010
//
// Original: FUN_0041c010 (0x0041c010..0x0041c084)  void FUN_0041c010(void)
//
// Verbatim decompilation:
//   local_18 = {0x3ee66666,0x3ea8f5c3,0x3f800000, 0xbee66666,0x3ea8f5c3,0x3f800000};
//   copy 6 dwords local_18 -> &DAT_005f334c;
//   puVar3 = &DAT_0063cab8;
//   do { FUN_0041b770(); puVar3 += 0x16c; } while ((int)puVar3 < 0x63cd90);
//   _DAT_0063cda0 = 0;
//
// Disassembly (cited): 0x0041c053 MOVSD.REP (6 dwords -> 0x5f334c);
//   0x0041c060 MOV ECX,ESI; CALL 0x0041b770 — FUN_0041b770 takes the record
//   pointer in ECX (__fastcall). ADD ESI,0x16c; CMP ESI,0x63cd90; JL — 2 records.
//   0x0041c076 MOV [0x0063cda0],0.
//
// ref: re/analysis/bucket_util_0040e4b0_0042f790/0x0041c010.md
// ---------------------------------------------------------------------------

// 0x0041c010
extern "C" __declspec(dllexport) void __cdecl HudConstTableInitAndSweep(void)
{
    std::uint32_t local_18[6] = {
        0x3ee66666u, 0x3ea8f5c3u, 0x3f800000u,   // ( 0.45, 0.33, 1.0)
        0xbee66666u, 0x3ea8f5c3u, 0x3f800000u,   // (-0.45, 0.33, 1.0)
    };
    std::uint32_t* dst = reinterpret_cast<std::uint32_t*>(k_005f334c);
    std::uint32_t* src = local_18;
    for (int i = 6; i != 0; i--) {
        *dst = *src;
        src++;
        dst++;
    }
    // Sweep the two 0x16c-byte records, ECX = record pointer (__fastcall).
    unsigned char* cur = reinterpret_cast<unsigned char*>(k_0063cab8);
    do {
        s_F0041b770(reinterpret_cast<int>(cur));
        cur += 0x16c;
    } while (reinterpret_cast<std::uintptr_t>(cur) < k_0063cd90);
    *reinterpret_cast<std::int32_t*>(k_0063cda0) = 0;
}

RH_ScopedInstall(HudConstTableInitAndSweep, 0x0041c010);

// ---------------------------------------------------------------------------
// HudSlideBillboardTick  --  0x0041d930
//
// Original: FUN_0041d930 (0x0041d930..0x0041da7f)  void FUN_0041d930(void)
//
// Verbatim decompilation: builds an RW matrix (local_40, 16 dwords) + translation
// vec3 (local_4c) + color vec3 (local_58), with a time-driven Y offset:
//   if (_DAT_005cd22c < DAT_0063d588)
//     local_48 = (DAT_0063d588 - _DAT_005cd22c) * _DAT_005cc564 + _DAT_005cd228;
//   FUN_004c51a0(&local_40,&local_4c,0);      // RwMatrixTranslate
//   FUN_004c1480(DAT_0063d580,&local_40,0);   // apply to HUD frame
//   local_58/54/50 *= _DAT_005cd118;          // color scale
//   FUN_004c13e0(DAT_0063d580,&local_58,1);
//   DAT_0063d584 += 1;                         // frame counter   <- observable
//   DAT_0063d588 += _DAT_007f100c;             // time accumulator
//
// local_34 (mat[3], the RwMatrix flags field) is read UNINITIALIZED then OR'd
// with 0x20003 (asm 0x0041d943: MOV EAX,[ESP+0x24]; OR EAX,0x20003; MOV
// [ESP+0x24],EAX). FUN_004c51a0 re-ORs and masks it to (garbage & 0xfffdffff)|3,
// then FUN_004c52f0 copies all 16 dwords into the frame — the garbage high bits
// land only in the (rendering-irrelevant) flags word of DAT_0063d580, never in
// the observed frame counter. local_24/local_14/mat[15] are likewise left
// unwritten by the original; transcribed faithfully. Observable: DAT_0063d584.
//
// ref: re/analysis/bucket_util_0040e4b0_0042f790/0x0041d930.md
// ---------------------------------------------------------------------------

// 0x0041d930
extern "C" __declspec(dllexport) void __cdecl HudSlideBillboardTick(void)
{
    std::uint32_t mat[16];   // &local_40 — RW matrix (16 dwords)
    float trans[3];          // &local_4c — translation vec3
    float col[3];            // &local_58 — color vec3

    // local_34 (mat[3]) read uninitialized then OR 0x20003 (matches the asm).
    mat[3] = mat[3] | 0x20003u;
    trans[0] = 0.0f;                  // local_4c
    trans[1] = 0.35f;                 // local_48 default (0x3eb33333)
    trans[2] = 1.0f;                  // local_44
    col[0] = 1.0f; col[1] = 1.0f; col[2] = 1.0f;  // local_58/54/50
    mat[10] = 0x3f800000u;            // local_18
    mat[5]  = 0x3f800000u;            // local_2c
    mat[0]  = 0x3f800000u;            // local_40
    mat[4]  = 0;                      // local_30
    mat[6]  = 0;                      // local_28
    mat[2]  = 0;                      // local_38
    mat[1]  = 0;                      // local_3c
    mat[9]  = 0;                      // local_1c
    mat[8]  = 0;                      // local_20
    mat[12] = 0;                      // local_10
    mat[13] = 0;                      // local_c
    mat[14] = 0;                      // local_8
    // mat[7] (local_24), mat[11] (local_14), mat[15] (local_4): unwritten in the
    // original (uninitialized stack). FUN_004c51a0 does not write them either.

    const float thresh = *reinterpret_cast<float*>(k_005cd22c);
    const float acc     = *reinterpret_cast<float*>(k_0063d588);
    if (thresh < acc) {
        trans[1] = (acc - thresh) * *reinterpret_cast<float*>(k_005cc564)
                   + *reinterpret_cast<float*>(k_005cd228);
    }

    s_F004c51a0(reinterpret_cast<float*>(mat), trans, 0);
    s_F004c1480(*reinterpret_cast<std::int32_t*>(k_0063d580), mat, 0);

    const float cscale = *reinterpret_cast<float*>(k_005cd118);
    col[0] = col[0] * cscale;
    col[1] = col[1] * cscale;
    col[2] = col[2] * cscale;
    s_F004c13e0(*reinterpret_cast<std::int32_t*>(k_0063d580), col, 1);

    *reinterpret_cast<std::int32_t*>(k_0063d584) =
        *reinterpret_cast<std::int32_t*>(k_0063d584) + 1;
    *reinterpret_cast<float*>(k_0063d588) =
        *reinterpret_cast<float*>(k_0063d588) + *reinterpret_cast<float*>(k_007f100c);
}

RH_ScopedInstall(HudSlideBillboardTick, 0x0041d930);

// ---------------------------------------------------------------------------
// AiFilenameBuild  --  0x00423480
//
// Original: FUN_00423480 (0x00423480..0x00423532)  undefined1* FUN_00423480(void)
//
// Verbatim decompilation (NO disk IO — its file-IO siblings 0x00423540/0x004235b0
// are separate): zeroes two 32-byte stack buffers, formats "AI%d.AI" with the
// current track index into local_44, clears DAT_00644110 (copies the empty
// local_24 -> a single null byte), then appends local_44 at DAT_00644110:
//   n = FUN_00426c00();
//   FUN_004a2b60(&local_44,"AI%d.AI",n);
//   for (i=0; (cVar=local_24[i]); ) { DAT_00644110[i]=cVar; i++; }  // writes one 0
//   len = strlen(&local_44)+1;
//   d = &DAT_0064410c + 3; do { q=d+1; d++; } while (*q != 0);   // -> 0x00644110
//   memcpy(d, &local_44, len);                                   // rep movsd+movsb
//   return &DAT_00644110;
// The clear of DAT_00644110[0] plus the scan (which checks 0x00644110 first)
// always lands the filename at DAT_00644110, regardless of the 4-byte prefix at
// DAT_0064410c. Observable: DAT_00644110 (= "AI<track>.AI"; probe-confirmed
// 0x30324941 = "AI20" in a sample race).
//
// The /GS stack cookie wrapping this buffer-using function is an MSVC artifact
// (the compiler emits its own); it has no observable effect, so it is not
// hand-replicated.
//
// ref: re/analysis/ai_path_following/0x00423480.md
// ---------------------------------------------------------------------------

// 0x00423480
extern "C" __declspec(dllexport) void* __cdecl AiFilenameBuild(void)
{
    char local_24[32];   // empty source buffer (clears DAT_00644110)
    char local_44[32];   // receives "AI%d.AI"
    for (int z = 0; z < 32; z++) { local_24[z] = '\0'; local_44[z] = '\0'; }

    int n = s_F00426c00();
    s_F004a2b60(local_44, "AI%d.AI", n);

    // Copy empty local_24 to DAT_00644110 — writes a single null terminator.
    char* fileSlot = reinterpret_cast<char*>(k_00644110);
    int i = 0;
    char c;
    do {
        c = local_24[i];
        fileSlot[i] = c;
        i++;
    } while (c != '\0');

    // strlen(local_44) + 1 (includes the terminator).
    char* e = local_44;
    do {
        c = *e;
        e++;
    } while (c != '\0');
    std::uint32_t len = static_cast<std::uint32_t>(e - local_44);

    // strcat scan from DAT_0064410c+3 — exits at 0x00644110 (just cleared).
    char* d = reinterpret_cast<char*>(k_0064410c + 3);
    char* q;
    do {
        q = d + 1;
        d = d + 1;
    } while (*q != '\0');

    // memcpy(d, local_44, len) — rep movsd then movsb.
    char* s = local_44;
    for (std::uint32_t w = len >> 2; w != 0; w--) {
        *reinterpret_cast<std::uint32_t*>(d) = *reinterpret_cast<std::uint32_t*>(s);
        s += 4;
        d += 4;
    }
    for (std::uint32_t w = len & 3; w != 0; w--) {
        *d = *s;
        s++;
        d++;
    }
    return reinterpret_cast<void*>(k_00644110);
}

RH_ScopedInstall(AiFilenameBuild, 0x00423480);

// ---------------------------------------------------------------------------
// CameraPathNodeContainTest  --  0x0047c160   (NOT INSTALLED — see header)
//
// Original: FUN_0047c160 (0x0047c160..0x0047c1ec)  void FUN_0047c160(int param_1)
//
// Verbatim transcription:
//   iVar1 = FUN_004c0ed0(*(param_1+4));            // resolve transform from handle
//   if (0 < DAT_006c2fe8) {
//     puVar6=&DAT_006c1228; puVar4=&DAT_006c27a8; puVar5=&DAT_006c2fa8;
//     do {
//       *puVar5 = 0;
//       if (FUN_004c1b40(param_1, puVar6+0x4e) != 0)        // +0x4e dwords
//         FUN_0047bb10(puVar6, iVar1+0x30, puVar4, puVar5);
//       puVar5 += 1; puVar4 += 0x80; puVar6 += 0x56;        // dword / byte / dword
//     } while (++i < DAT_006c2fe8);
//   }
// puVar6/puVar5 are dword pointers (stride 0x56 dwords / 1 dword); puVar4 is a
// byte pointer (stride 0x80 bytes) per the decompiler types.
//
// Verification-blocked: param_1 is a query OBJECT pointer (immediately
// dereferenced at +4). run_diff's void_write_observe passes no args; int_scalar
// would deref an arbitrary int -> AV. A faithful in-race diff needs a NEW
// arg_type that supplies a valid entity pointer (e.g. deref a static vehicle-0
// global). NOTE: the batch prompt's "pass the int node index" hint contradicts
// the decompilation (param_1 is not a node index). Queued for harness work.
//
// ref: re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0047c160.md
// ---------------------------------------------------------------------------

// 0x0047c160
extern "C" __declspec(dllexport) void __cdecl CameraPathNodeContainTest(int param_1)
{
    int iVar1 = s_F004c0ed0(*reinterpret_cast<std::int32_t*>(param_1 + 4));
    int iVar3 = 0;
    if (0 < *reinterpret_cast<std::int32_t*>(k_006c2fe8)) {
        std::uint32_t* puVar6 = reinterpret_cast<std::uint32_t*>(k_006c1228); // node records
        unsigned char* puVar4 = reinterpret_cast<unsigned char*>(k_006c27a8); // output A (byte ptr)
        std::int32_t*  puVar5 = reinterpret_cast<std::int32_t*>(k_006c2fa8);   // output B/flag
        do {
            *puVar5 = 0;
            int iVar2 = s_F004c1b40(param_1, puVar6 + 0x4e);
            if (iVar2 != 0) {
                s_F0047bb10(puVar6, iVar1 + 0x30, puVar4, puVar5);
            }
            iVar3 = iVar3 + 1;
            puVar5 = puVar5 + 1;       // +1 dword
            puVar4 = puVar4 + 0x80;    // +0x80 bytes
            puVar6 = puVar6 + 0x56;    // +0x56 dwords
        } while (iVar3 < *reinterpret_cast<std::int32_t*>(k_006c2fe8));
    }
}

// NOT installed — verification-blocked (needs a valid-entity-pointer arg_type).
// RH_ScopedInstall(CameraPathNodeContainTest, 0x0047c160);

// ---------------------------------------------------------------------------
// AiPerFrameTick  --  0x00418860   (NOT INSTALLED — see header)
//
// Original: FUN_00418860 (0x00418860..0x00418977)  void FUN_00418860(void)
//
// Verbatim transcription of the per-frame AI dispatcher. Guards on
// DAT_00801ca0 > 3; gates a vehicle-1..3 state-set block on the game mode
// (FUN_0042f6a0 in {3,4,5,10}) or DAT_007f0fd0 in {4,9,8,10}; runs the AI
// pre-tick (FUN_004177b0); then steps each AI vehicle (FUN_00418560).
//
// Verification-blocked: a void(void) dispatcher with no direct global write.
// Its side-effects are entirely inside callees that write DYNAMIC heap state
// (e.g. FUN_0040e480 writes *(PTR_PTR_005f2770 + idx*4 + 0x34)). There is no
// static-address observable for void_write_observe, and state_machine_observe
// only snapshots fixed addresses. Needs a heap-snapshot A/B harness. Queued.
//
// ref: re/analysis/ai_path_following/0x00418860.md
// ---------------------------------------------------------------------------

// 0x00418860
extern "C" __declspec(dllexport) void __cdecl AiPerFrameTick(void)
{
    if (3 < *reinterpret_cast<std::int32_t*>(k_00801ca0)) {
        int iVar1 = s_F0042f6a0();
        if ((((iVar1 == 3) || (iVar1 = s_F0042f6a0(), iVar1 == 4)) ||
             (iVar1 = s_F0042f6a0(), iVar1 == 5)) ||
            (((iVar1 = s_F0042f6a0(), iVar1 == 10) ||
              (*reinterpret_cast<std::int32_t*>(k_007f0fd0) == 4)) ||
             ((*reinterpret_cast<std::int32_t*>(k_007f0fd0) == 9) ||
              ((*reinterpret_cast<std::int32_t*>(k_007f0fd0) == 8) ||
               (*reinterpret_cast<std::int32_t*>(k_007f0fd0) == 10))))) {
            if (s_F0046c7b0(1) == 1) { s_F0040e480(1, 2); }
            if (s_F0046c7b0(2) == 1) { s_F0040e480(2, 2); }
            if (s_F0046c7b0(3) == 1) { s_F0040e480(3, 2); }
        }
        s_F004177b0();
        int idx = 0;
        do {
            int iVar2 = s_F0040e470(idx);
            if (((iVar2 != 0) && (iVar2 != 1)) || (s_F00443080() == 1)) {
                if (s_F0046c7b0(idx) == 1) {
                    s_F00418560(idx, iVar2);
                }
                int iVar3 = s_F0042f6a0();
                if ((((iVar3 == 3) || (iVar3 = s_F0042f6a0(), iVar3 == 4)) ||
                     (iVar3 = s_F0042f6a0(), iVar3 == 5)) &&
                    (*reinterpret_cast<std::int32_t*>(k_007f0fd0) == 7)) {
                    s_F00418560(0, iVar2);
                }
            }
            idx = idx + 1;
        } while (idx < 4);
    }
}

// NOT installed — verification-blocked (needs a heap-snapshot harness).
// RH_ScopedInstall(AiPerFrameTick, 0x00418860);
