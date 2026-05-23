// Mashed RE — Audio mixed-cluster C3 sweep (wave1-s6).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Candidates in this file:
//   0x005abfa0  AudioWaveLoad      — per-wave data loader (0x803 + 0x804 chunk reader)
//
// Deferred from this file:
//   0x005ac210  FUN_005ac210       — DEFERRED: [UNCERTAIN U-3030] param_3 semantics and
//                                    [UNCERTAIN U-3031] FUN_005aaac0 semantics are both
//                                    in the body. 16-callee factory with complex flag
//                                    dispatch; faithful authoring requires both resolved.
//                                    Re-pickup when U-3030 and U-3031 are closed.
//
// Analysis notes:
//   re/analysis/promote_c2_audio_rws/005abfa0.md
//   re/analysis/promote_c2_audio_rws/005ac210.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ─── Trampolines to original RVAs for callees not yet reimplemented ──────────

// 0x005ab380  FUN_005ab380 — read RWS chunk header
// Reads chunk header fields into the three locals pointed to by param_2.
// Ghidra layout at call sites: param_2 points to {local_14 (type), local_10 (size), local_c (version)}.
static int Orig_ReadChunkHdr(int stream, uint32_t* out_hdr3, uint32_t streaming_flag) {
    typedef int (__cdecl *Fn)(int, uint32_t*, uint32_t);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005ab380u))(
        stream, out_hdr3, streaming_flag);
}

// 0x005aea00  FUN_005aea00 — raw alloc (size, tag) -> void*
static void* Orig_RawAlloc(uint32_t size, uint32_t tag) {
    typedef void* (__cdecl *Fn)(uint32_t, uint32_t);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005aea00u))(size, tag);
}

// 0x004cbd30  FUN_004cbd30 — stream read: (stream, buf, size) -> bytes_read
static uint32_t Orig_StreamRead(int stream, void* buf, uint32_t size) {
    typedef uint32_t (__cdecl *Fn)(int, void*, uint32_t);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x004cbd30u))(stream, buf, size);
}

// 0x004522d0  FUN_004522d0 — heap free (vtable dealloc wrapper)
static void Orig_Free(void* ptr) {
    typedef void (__cdecl *Fn)(void*);
    reinterpret_cast<Fn>(static_cast<uintptr_t>(0x004522d0u))(ptr);
}

// 0x005ac210  FUN_005ac210 — wave_node factory [DEFERRED — see file header]
// Trampolined to original RVA so that AudioWaveLoad can be exercised.
static int Orig_WaveNodeFactory(void* fmt_buf, uint32_t* param_2,
                                int param_3, int* param_4, uint32_t param_5) {
    typedef int (__cdecl *Fn)(void*, uint32_t*, int, int*, uint32_t);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005ac210u))(
        fmt_buf, param_2, param_3, param_4, param_5);
}

// 0x005adf30  FUN_005adf30 — 16-byte format-key compare (AudioFmtKeyCompare, C3)
// Returns 0 on match, non-zero on mismatch.
static int Orig_FmtKeyCmp(const void* a, const void* b) {
    typedef int (__cdecl *Fn)(const void*, const void*);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005adf30u))(a, b);
}

// 0x005aea10  FUN_005aea10 — alloc PCM read buffer (size, tag) -> void*
static void* Orig_RawAlloc2(uint32_t size, uint32_t tag) {
    typedef void* (__cdecl *Fn)(uint32_t, uint32_t);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005aea10u))(size, tag);
}

// 0x005aec30  FUN_005aec30 — byte-swap PCM block (buf, size, bytes_per_sample)
static void Orig_Bswap(void* buf, uint32_t size, uint32_t bps) {
    typedef void (__cdecl *Fn)(void*, uint32_t, uint32_t);
    reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005aec30u))(buf, size, bps);
}

// 0x005abd30  FUN_005abd30 — feed PCM chunk to wave_node; returns non-zero on success
static int Orig_FeedPcm(int node, uint32_t offset, void* buf, uint32_t size) {
    typedef int (__cdecl *Fn)(int, uint32_t, void*, uint32_t);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005abd30u))(node, offset, buf, size);
}

// 0x005abf80  FUN_005abf80 — drain/pump after each PCM feed; returns 1 while still pumping
static int Orig_DrainPump(int node) {
    typedef int (__cdecl *Fn)(int);
    return reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005abf80u))(node);
}

// 0x005aea40  FUN_005aea40 — free PCM read buffer
static void Orig_FreePcmBuf(void* ptr) {
    typedef void (__cdecl *Fn)(void*);
    reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005aea40u))(ptr);
}

// 0x005abcf0  FUN_005abcf0 — destroy wave_node on 0x804 parse failure
static void Orig_DestroyNode(int node) {
    typedef void (__cdecl *Fn)(int);
    reinterpret_cast<Fn>(static_cast<uintptr_t>(0x005abcf0u))(node);
}

// ─── Constants ────────────────────────────────────────────────────────────────
// 0x007dcaa8  DAT_007dcaa8 — global pre-allocated fmt scratch buffer ptr
//             0 = not pre-allocated; non-zero = caller supplied a buffer
static constexpr uintptr_t kGlobalFmtBuf    = 0x007dcaa8u;
// 0x007dcaac  DAT_007dcaac — [UNCERTAIN U-3029] max streaming read size (or buffer bound)
//             Address cited in decomp at conditional: uVar4 < DAT_007dcaac + uVar11
static constexpr uintptr_t kGlobalStreamCap = 0x007dcaacu;
// 0x005e6414  DAT_005e6414 — known PCM format key #1
static constexpr uintptr_t kPcmFmtKey1      = 0x005e6414u;
// 0x005e6444  DAT_005e6444 — known PCM format key #2
static constexpr uintptr_t kPcmFmtKey2      = 0x005e6444u;
// 0x30806 — pool/alloc type tag (inlined at alloc call sites)
static constexpr uint32_t  kAllocTag        = 0x00030806u;

// RWS audio chunk type IDs (inlined in function body)
static constexpr uint32_t kChunkTypeFmt = 0x803u;  // format descriptor chunk
static constexpr uint32_t kChunkTypePcm = 0x804u;  // PCM data chunk

// ─── 0x005abfa0  AudioWaveLoad ───────────────────────────────────────────────
// Per-wave data loader: reads 0x803 (fmt) then 0x804 (PCM) RWS chunks from
// a stream, allocates + initialises a wave_node via WaveNodeFactory, feeds
// all PCM data in a chunked loop.
//
// param_1  = stream handle (passed directly to FUN_005ab380 / FUN_004cbd30)
// param_2  = audio context ptr (forwarded to WaveNodeFactory as param_2)
// param_3  = extra context arg (forwarded to WaveNodeFactory as param_3)
// param_4  = output format override ptr (forwarded to WaveNodeFactory as param_4)
// param_5  = flags: bit4 = streaming/byte-swap flag; bits 0..3 forwarded to factory
// Returns  = allocated wave_node ptr on success, 0 on any failure.
//
// Callee gate satisfied:
//   FUN_005adf30 (AudioFmtKeyCompare) — C3 in Audio/AudioRws.cpp
//
// U-3029 in body: DAT_007dcaac is the streaming buffer capacity; used as
//   cap on per-chunk read size when bVar2==false. Exact semantic is open,
//   but the address and its use in the conditional are cited from the decomp.
//
// ref: re/analysis/promote_c2_audio_rws/005abfa0.md

// Chunk header layout: FUN_005ab380 writes 3 consecutive uint32 locals.
// Ghidra names in the first call: local_14 (offset 0) = type,
//   local_10 (offset 4) = size/body_len, local_c (offset 8) = version.
// Second call (0x804): local_14 = type, local_c = chunk body size.
// Note: local_10 retains its value from the first call (fmt size) and is
//   reused as the per-iteration PCM buffer allocation size.
struct RwsHdr3 {
    uint32_t type;      // chunk type (e.g. 0x803, 0x804)
    uint32_t body_len;  // body byte count (local_10 in decomp)
    uint32_t version;   // chunk version / secondary size field (local_c)
};

// 0x005abfa0
extern "C" __declspec(dllexport)
int __cdecl AudioWaveLoad(int    param_1,
                          uint32_t* param_2,
                          int       param_3,
                          int*      param_4,
                          uint32_t  param_5)
{
    // streaming/byte-swap flag: bit4 of param_5 (uVar9 in decomp)
    const uint32_t uVar9 = (param_5 >> 4u) & 1u;

    // Snapshot the global pre-allocated fmt buffer at entry (iVar8 in decomp).
    // Used later to decide whether we own the PCM read buffer.
    const int iVar8_entry = *reinterpret_cast<const int*>(kGlobalFmtBuf);

    // ── Phase 1: read 0x803 chunk header ──────────────────────────────────
    RwsHdr3 hdr = {};
    const int iVar3_h1 = Orig_ReadChunkHdr(param_1, reinterpret_cast<uint32_t*>(&hdr), uVar9);

    if (iVar3_h1 == 0 || hdr.type != kChunkTypeFmt || hdr.version == 0u) {
        return 0;
    }
    // hdr.body_len (local_10) = fmt chunk body size; retained for Phase 3 buffer alloc.
    const uint32_t local_10 = hdr.body_len;

    // Resolve fmt buffer: reuse global if available, else allocate
    int iVar3_fmtbuf = *reinterpret_cast<const int*>(kGlobalFmtBuf);
    const bool fmt_allocated_here = (iVar3_fmtbuf == 0);
    if (fmt_allocated_here) {
        iVar3_fmtbuf = reinterpret_cast<int>(Orig_RawAlloc(local_10, kAllocTag));
        if (iVar3_fmtbuf == 0) {
            return 0;
        }
    }

    // Read fmt body (local_10 bytes)
    const uint32_t uVar4_read = Orig_StreamRead(
        param_1,
        reinterpret_cast<void*>(static_cast<uintptr_t>(iVar3_fmtbuf)),
        local_10);

    if (local_10 != uVar4_read) {
        if (fmt_allocated_here) {
            Orig_Free(reinterpret_cast<void*>(static_cast<uintptr_t>(iVar3_fmtbuf)));
        }
        return 0;
    }

    // Parse fmt buffer → wave_node via factory
    const int iVar5 = Orig_WaveNodeFactory(
        reinterpret_cast<void*>(static_cast<uintptr_t>(iVar3_fmtbuf)),
        param_2, param_3, param_4, param_5);

    // Free tmp fmt buffer if we allocated it (regardless of factory result)
    if (fmt_allocated_here) {
        Orig_Free(reinterpret_cast<void*>(static_cast<uintptr_t>(iVar3_fmtbuf)));
    }

    if (iVar5 == 0) {
        return 0;
    }

    // ── Phase 2: read 0x804 chunk header ──────────────────────────────────
    // Reuse hdr locals (same Ghidra locals local_14, local_10, local_c).
    // After this call: hdr.type = 0x804, hdr.version (local_c) = 0x804 body size.
    const int iVar3_h2 = Orig_ReadChunkHdr(param_1, reinterpret_cast<uint32_t*>(&hdr), uVar9);

    if (iVar3_h2 == 0 || hdr.type != kChunkTypePcm || hdr.version == 0u) {
        Orig_DestroyNode(iVar5);
        return 0;
    }
    // local_c after second call = total 0x804 body size (uVar4 in Phase 3 loop)
    const uint32_t pcm_total = hdr.version;  // uVar4 in decomp loop bound

    // ── PCM fmt detection (buffer strategy selection) ──────────────────────
    // Read src fmt key ptr and out fmt key ptr from wave_node
    //   wave_node+0x14 = ptr to src fmt sub-struct fmt-key
    //   wave_node+0x30 = ptr to out fmt sub-struct fmt-key
    const uintptr_t node_base = static_cast<uintptr_t>(iVar5);
    const uintptr_t src_key = *reinterpret_cast<const uintptr_t*>(node_base + 0x14u);
    const uintptr_t out_key = *reinterpret_cast<const uintptr_t*>(node_base + 0x30u);

    // iVar3: src == out (same format, no conversion needed)
    const int iVar3_same = Orig_FmtKeyCmp(
        reinterpret_cast<const void*>(src_key),
        reinterpret_cast<const void*>(out_key));

    // iVar6: OR of four checks — non-zero if src or out key matches either known PCM key
    int iVar6 = Orig_FmtKeyCmp(reinterpret_cast<const void*>(src_key),
                                reinterpret_cast<const void*>(kPcmFmtKey1));
    iVar6 |= Orig_FmtKeyCmp(reinterpret_cast<const void*>(src_key),
                             reinterpret_cast<const void*>(kPcmFmtKey2));
    iVar6 |= Orig_FmtKeyCmp(reinterpret_cast<const void*>(out_key),
                             reinterpret_cast<const void*>(kPcmFmtKey1));
    iVar6 |= Orig_FmtKeyCmp(reinterpret_cast<const void*>(out_key),
                             reinterpret_cast<const void*>(kPcmFmtKey2));

    // bVar1: at least one of (src==out, or src/out matches a known-PCM key)
    const bool bVar1 = ((iVar3_same == 0) || (iVar6 == 0));

    // Allocate separate PCM read buffer if:
    //   (hdr2 read ok AND known-PCM match) OR no global buf was available at entry
    // Otherwise, use the global scratch buffer (iVar8_entry) directly.
    bool bVar2 = false;
    void* local_1c;
    if ((iVar3_h2 != 0 && bVar1) || iVar8_entry == 0) {
        // alloc with local_10 (fmt chunk size, retained from Phase 1)
        local_1c = Orig_RawAlloc2(local_10, kAllocTag);
        bVar2 = true;
    } else {
        // Use the pre-allocated global fmt scratch buffer for PCM reads.
        // iVar8_entry holds the global buffer address (DAT_007dcaa8 at entry).
        local_1c = reinterpret_cast<void*>(static_cast<uintptr_t>(
            static_cast<uint32_t>(iVar8_entry)));
    }

    // ── Phase 3: chunked PCM read loop ────────────────────────────────────
    // uVar11: bytes fed so far; loop until uVar11 >= pcm_total (uVar4)
    uint32_t uVar11 = 0u;
    bool loop_ok    = true;

    do {
        // Default: read local_10 bytes per iteration (uVar10 = local_10)
        uint32_t uVar10 = local_10;

        if (!bVar2 && pcm_total < *reinterpret_cast<const uint32_t*>(kGlobalStreamCap) + uVar11) {
            // Cap to remaining: pcm_total - uVar11
            // [UNCERTAIN U-3029]: DAT_007dcaac — streaming buffer capacity;
            // when pcm_total < cap + uVar11, limit this iteration's read.
            uVar10 = pcm_total - uVar11;
        }

        // Read PCM chunk into local_1c buffer
        const uint32_t uVar7 = Orig_StreamRead(param_1, local_1c, uVar10);
        if (uVar10 != uVar7) {
            loop_ok = false;
            break;
        }

        // Byte-swap if streaming flag set and bits-per-sample > 8
        // wave_node+0x1c: bits-per-sample byte
        const uint8_t bps = *reinterpret_cast<const uint8_t*>(node_base + 0x1cu);
        if (uVar9 != 0u && bps > 8u) {
            // bps >> 3 = bytes-per-sample
            Orig_Bswap(local_1c, uVar10, static_cast<uint32_t>(bps >> 3u));
        }

        // Feed PCM chunk to wave_node
        const int iVar8_feed = Orig_FeedPcm(iVar5, uVar11, local_1c, uVar10);
        if (iVar8_feed == 0) {
            loop_ok = false;
            break;
        }

        // Drain pump after each successful feed (iVar8==1 means still pumping)
        int pump_result;
        do {
            pump_result = Orig_DrainPump(iVar5);
        } while (pump_result == 1);

        uVar11 += uVar10;
    } while (uVar11 < pcm_total);

    // Free PCM read buffer if we allocated it
    if (bVar2) {
        Orig_FreePcmBuf(local_1c);
    }

    // On loop failure, destroy the partially-constructed node
    if (!loop_ok) {
        Orig_DestroyNode(iVar5);
        return 0;
    }

    return iVar5;
}

RH_ScopedInstall(AudioWaveLoad, 0x005abfa0);
