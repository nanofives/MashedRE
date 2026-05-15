// Mashed RE - Frontend sprite dispatch / frame thunk reimplementations.
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042fab0.md
//   re/analysis/promote_c2_frontend_menus/0x0042e590.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee: FUN_0040bb90 — sprite lookup variant B
// Analysis: re/analysis/frontend_promote_menus_b/0040bb90.md
// Status: C2 in hooks.csv. Called with a single void* (slot-specific pointer).
// ---------------------------------------------------------------------------
static auto* const s_FUN_0040bb90 =
    reinterpret_cast<void(__cdecl*)(void*)>(0x0040bb90);

// ---------------------------------------------------------------------------
// Callee: FUN_0040bb70 — sprite draw / string lookup
// Analysis: re/analysis/promote_c2_frontend_menus/0x0042e590.md (callee list)
// Status: C2 in hooks.csv. Called with transformed first arg.
// Original signature includes 9 params (first replaced by sprite ptr lookup).
// We call it through the original VA; only the first arg differs from caller.
// ---------------------------------------------------------------------------
static auto* const s_FUN_0040bb70 =
    reinterpret_cast<void(__cdecl*)(void*, float, float, float, float,
                                    std::uint32_t, int, int, int)>(0x0040bb70);

// ---------------------------------------------------------------------------
// SpriteSlotDispatch  --  0x0042fab0
//
// Original: FUN_0042fab0 (151 bytes, 0x0042fab0..0x0042fb46)
// Signature: void (int slot)   slot: 0–9; default: no-op.
//
// Assembly-confirmed: each case overwrites [ESP+4] with a slot-specific pointer
// from the table at 0x5cd838..0x5cd898 (slot 0 → 0x5cd898, stride ≈0xc),
// then JMPs to FUN_0040bb90. Decompiler collapsed all cases to identical
// FUN_0040bb90() calls, hiding the per-case pointer argument (U-3178a resolved).
//
// Slot → pointer table (cited from assembly at 0x0042fab0 body):
//   Slot 0: 0x005cd898
//   Slot 1: 0x005cd88c
//   Slot 2: 0x005cd880
//   Slot 3: 0x005cd878
//   Slot 4: 0x005cd870
//   Slot 5: 0x005cd864
//   Slot 6: 0x005cd858
//   Slot 7: 0x005cd850
//   Slot 8: 0x005cd844
//   Slot 9: 0x005cd838
// ref: re/analysis/promote_c2_frontend_menus/0x0042fab0.md
// ---------------------------------------------------------------------------

// Slot pointer table.  Entries are raw addresses of in-process global data
// that FUN_0040bb90 uses as a sprite descriptor (exact struct TBD at C4).
// All addresses cited from assembly per-case at 0x0042fab0..0x0042fb46.
static void* const kSlotPtrs[10] = {
    reinterpret_cast<void*>(0x005cd898u),   // slot 0  (0x0042fab0 case 0)
    reinterpret_cast<void*>(0x005cd88cu),   // slot 1  (0x0042fab0 case 1)
    reinterpret_cast<void*>(0x005cd880u),   // slot 2  (0x0042fab0 case 2)
    reinterpret_cast<void*>(0x005cd878u),   // slot 3  (0x0042fab0 case 3)
    reinterpret_cast<void*>(0x005cd870u),   // slot 4  (0x0042fab0 case 4)
    reinterpret_cast<void*>(0x005cd864u),   // slot 5  (0x0042fab0 case 5)
    reinterpret_cast<void*>(0x005cd858u),   // slot 6  (0x0042fab0 case 6)
    reinterpret_cast<void*>(0x005cd850u),   // slot 7  (0x0042fab0 case 7)
    reinterpret_cast<void*>(0x005cd844u),   // slot 8  (0x0042fab0 case 8)
    reinterpret_cast<void*>(0x005cd838u),   // slot 9  (0x0042fab0 case 9)
};

// 0x0042fab0
extern "C" __declspec(dllexport) void __cdecl SpriteSlotDispatch(int slot) {
    if (slot < 0 || slot > 9) {
        return;  // default case: fall-through (no-op), cited at 0x0042fb46
    }
    s_FUN_0040bb90(kSlotPtrs[slot]);
}

RH_ScopedInstall(SpriteSlotDispatch, 0x0042fab0);


// ---------------------------------------------------------------------------
// SpriteAnimFrameThunk  --  0x0042e590
//
// Original: FUN_0042e590 (28 bytes, 0x0042e590..0x0042e5ac)
// Signature: void (int sprite_slot, float x1, float y1, float x2, float y2,
//                  uint32_t color, int uv_param, int frame_idx, int flag)
//
// Assembly-confirmed (asm at 0x0042e590..0x0042e5a8):
//   EAX  = [ESP+4]              ; sprite_slot
//   ECX  = [0x0067f17c]         ; DAT_0067f17c (animation frame counter)
//   EDX  = EAX + ECX*2          ; index = sprite_slot + 2*frame
//   EAX  = [EDX*4 + 0x5f79d8]   ; sprite_ptr_table[index]
//   [ESP+4] = EAX               ; overwrite first arg
//   JMP  0x0040bb70             ; tail-call FUN_0040bb70 with new first arg
//
// Decompiler showed void FUN_0042e590(void) — completely wrong (U-2547 resolved).
// ref: re/analysis/promote_c2_frontend_menus/0x0042e590.md
// ---------------------------------------------------------------------------

// DAT_0067f17c: animation frame counter global.  (cited at 0x0042e594)
static constexpr std::uintptr_t kAnimFrame      = 0x0067f17cu;
// Sprite pointer table base: 0x005f79d8.  (cited at 0x0042e59d)
static constexpr std::uintptr_t kSpritePtrTable = 0x005f79d8u;

// 0x0042e590
extern "C" __declspec(dllexport) void __cdecl SpriteAnimFrameThunk(
    int sprite_slot,
    float x1, float y1, float x2, float y2,
    std::uint32_t color, int uv_param, int frame_idx, int flag)
{
    // Compute lookup index: sprite_slot + 2 * DAT_0067f17c.
    int anim_frame = *reinterpret_cast<int*>(kAnimFrame);
    int idx = sprite_slot + 2 * anim_frame;

    // Look up sprite pointer: sprite_ptr_table[idx]  (0x0042e59d)
    void* sprite_ptr = reinterpret_cast<void* const*>(kSpritePtrTable)[idx];

    // Tail-call FUN_0040bb70 with sprite_ptr replacing sprite_slot as first arg.
    s_FUN_0040bb70(sprite_ptr, x1, y1, x2, y2, color, uv_param, frame_idx, flag);
}

RH_ScopedInstall(SpriteAnimFrameThunk, 0x0042e590);
