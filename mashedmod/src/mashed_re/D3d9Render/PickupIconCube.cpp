// Mashed RE — the pickup IconCube (D5-ICONCUBE-SPIN). See PickupIconCube.h.
//
// Pool 0x0068b198 (25 x 0x50), count 0x0068b9a8, the original-layout pool Gameplay/PickupTick.cpp
// runs FUN_00459000 on. Entry fields read here: +0x04 angle (deg), +0x0c axis, +0x20 state,
// +0x24 type, +0x38 position. FUN_00458a40 @ 0x00458a40 leaves the frame as
// RwFrameTranslate(+0x38, REPLACE) (0x00458a8e) then RwFrameRotate(+0x0c, +0x04, PRECONCAT)
// (0x00458aac), and IconCube_Submit rebuilds that matrix with the port's RwMatrixRotate. The port's
// PK_FrameRotate does not model the rows, and nothing in the sim reads them.
#include "PickupIconCube.h"

#include "../Gameplay/PickupTick.h"
#include "../LibRw/RwRaceSubmit.h"
#include "../Piz/PizReader.h"
#include "../Track/DffModel.h"
#include "../Txd/TxdDecoder.h"

#include <cstdlib>
#include <cstring>
#include <vector>

extern "C" void* __cdecl RwMatrixRotate(void* m, const float* axis, float deg, int mode);   // 0x004c4d20

namespace mashed_re {
namespace D3d9Render {
namespace {

constexpr std::uintptr_t kPool = 0x0068b198u, kCount = 0x0068b9a8u, kStride = 0x50u;
constexpr int kMax = 25;

// FUN_00458630 @ 0x00458630: type -> PowerUpIcons.txd name (FUN_004c5c00 on DAT_0068b9ac).
// Index kNoTex = the default arm (returns 0: RpMaterialSetTexture with no texture).
const char* const kNames[] = { "flamethrower", "mine", "mortar", "machinegun", "depthcharge", "missile",
                               "shotgun", "flare", "oil", "chaos", "airstrike", "" };
constexpr int kNoTex = 11;
int NameIndex(std::int32_t type) {
    switch (type) {
    case 2: case 0x10:          return 0;   // "flamethrower"
    case 6: case 0xc: case 0xd: return 1;   // 0x005ce594 "mine"
    case 7:                     return 2;   // "mortar"
    case 9:                     return 3;   // "machinegun"
    case 10:                    return 4;   // "depthcharge"
    case 0xb:                   return 5;   // "missile"
    case 0x11:                  return 6;   // "shotgun"
    case 0x12:                  return 7;   // "flare"
    case 0x13:                  return 8;   // 0x005ce4fc "oil"
    case 0x15:                  return 9;   // "chaos"
    case 0x16:                  return 10;  // "airstrike"
    default:                    return kNoTex;
    }
}

int s_handle[kNoTex + 1];
bool s_ready = false;

const float kDrawDist2 = 800.0f;   // [0x005ce5d4]
const float kZero      = 0.0f;     // [0x005d757c]

}  // namespace

bool IconCube_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_ICONCUBE");
        return s && s[0] && s[0] != '0';
    }();
    return s_on && Gameplay::PickupsOrig_Enabled();
}

void IconCube_OnTrackLoad(std::uint32_t ambient, std::FILE* log) {
    s_ready = false;
    if (!IconCube_Enabled()) return;

    // FUN_004587a0: per entry +0x08 = 0x3f800000, +0x0c = 0, +0x10 = 0x3f800000, +0x14 = 0.
    // +0x04 (the angle) is not written.
    for (int i = 0; i < kMax; ++i) {
        float* e = reinterpret_cast<float*>(kPool + i * kStride);
        e[2] = 1.0f;
        e[3] = 0.0f; e[4] = 1.0f; e[5] = 0.0f;
    }

    const char kRel[] = "original/TOASTART/Common/POWERUPS/Powerups.piz";
    Piz::Archive piz;
    bool ok = piz.Load(kRel);
    if (!ok) {
        if (const char* root = std::getenv("MASHED_ROOT")) {
            char path[512];
            std::snprintf(path, sizeof path, "%s/%s", root, kRel);
            ok = piz.Load(path);
        }
    }
    if (!ok) {
        if (log) std::fprintf(log, "  [iconcube] Powerups.piz load FAILED: %s\n", piz.last_error());
        return;
    }
    const std::uint8_t* dff = nullptr; std::uint32_t dffn = 0;
    std::vector<Txd::Dictionary> dicts(1);
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        std::uint32_t n = 0;
        if (_stricmp(piz.entry(i).name, "ICONCUBE.DFF") == 0) dff = piz.blob(i, &dffn);
        else if (_stricmp(piz.entry(i).name, "POWERUPICONS.TXD") == 0) {
            const std::uint8_t* b = piz.blob(i, &n);
            if (b) dicts[0].Decode(b, n);
        }
    }
    Track::DffModel base;
    if (!dff || !base.Parse(dff, dffn)) {
        if (log) std::fprintf(log, "  [iconcube] ICONCUBE.DFF missing or unparsed\n");
        return;
    }
    // FUN_00458dd0 sets the texture of the material FUN_004b4080 returns. The DFF carries one
    // material, so each skin is a copy of the model with that material's name replaced.
    int nok = 0;
    for (int k = 0; k <= kNoTex; ++k) {
        Track::DffModel m = base;
        for (auto& mat : m.materials) std::snprintf(mat.tex_name, sizeof(mat.tex_name), "%s", kNames[k]);
        std::snprintf(m.source_name, sizeof(m.source_name), "ICONCUBE.DFF");
        s_handle[k] = LibRw::RaceSubmit_RegisterModel(m, dicts.data(), dicts.size(), ambient);
        if (s_handle[k] >= 0) { LibRw::RaceSubmit_SetModelRwStates(s_handle[k]); ++nok; }
    }
    s_ready = nok > 0;
    if (log) std::fprintf(log, "  [iconcube] %d/%d skins registered (materials=%zu batches=%zu txd=%zu)\n",
                          nok, kNoTex + 1, base.materials.size(), base.batches.size(),
                          static_cast<std::size_t>(dicts[0].count()));
}

// FUN_00458b10 @ 0x00458b10, the cube loop (after the glow flush, under RwRenderStateSet(8,1)(6,1)).
void IconCube_Submit(const float eye[3], const float at[3]) {
    if (!s_ready || !IconCube_Enabled()) return;
    const float fwd[3] = { at[0] - eye[0], at[1] - eye[1], at[2] - eye[2] };
    const std::int32_t cnt = *reinterpret_cast<const std::int32_t*>(kCount);
    for (std::int32_t i = cnt - 1; i >= 0; --i) {   // DAT_0068b9a8 down to 1, entry n-1
        if (i >= kMax) continue;
        const unsigned char* e = reinterpret_cast<const unsigned char*>(kPool + i * kStride);
        if (*reinterpret_cast<const std::int32_t*>(e + 0x20) != 1) continue;
        const float* pos = reinterpret_cast<const float*>(e + 0x38);
        const float dx = pos[0] - eye[0], dy = pos[1] - eye[1], dz = pos[2] - eye[2];
        if (!(kZero < fwd[2] * dz + dx * fwd[0] + fwd[1] * dy)) continue;
        if (!(dz * dz + dx * dx + dy * dy < kDrawDist2)) continue;
        const int h = s_handle[NameIndex(*reinterpret_cast<const std::int32_t*>(e + 0x24))];
        if (h < 0) continue;
        // FUN_00458a40's frame: identity + translation (REPLACE), then the rotation PRECONCAT
        float m[16] = { 1.f, 0.f, 0.f, 0.f,  0.f, 1.f, 0.f, 0.f,  0.f, 0.f, 1.f, 0.f,
                        pos[0], pos[1], pos[2], 0.f };
        float angle;
        std::memcpy(&angle, e + 0x04, 4);
        RwMatrixRotate(m, reinterpret_cast<const float*>(e + 0x0c), angle, 1);
        m[3] = 0.f; m[7] = 0.f; m[11] = 0.f; m[15] = 1.f;   // RwMatrix flags/pads -> D3DMATRIX
        // ZWRITE 1: FUN_00458b10's RwRenderStateSet(8,1) after the glow flush; cull left to the RW states
        LibRw::RaceSubmit_AddInstanceStates(h, m, LibRw::kInstAfterFx, 0, 1);
    }
}

}  // namespace D3d9Render
}  // namespace mashed_re
