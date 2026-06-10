// Mashed RE — the shared race camera + camera-coupled elimination rule.
//
// VERBATIM PORT of the original's race-camera director and round-end check:
//   0x00446520  FUN_00446520(cam, force_reset)   race branch (camera director)
//   0x00442a60  FUN_00442a60                     per-player distance array
//   0x0040e180  FUN_0040e180                     most-separated pair finder
//   0x00441820  FUN_00441820                     per-node camera direction
//   0x00410d10  FUN_00410d10                     elimination / round-end core
// Blueprint with all constants + disassembly evidence:
//   re/analysis/race_camera/race_camera.md
// Ground truth: re/analysis/race_camera/camera_trace.csv (live original).
//
// Per-node camera angles load from Common/LED.piz LE<Course_Id>.LED
// (12-byte header + 384 × f32 triplets (elev°, azim°, height), stride 0xC —
// format verified against the live DAT_0063a5f0 table 2026-06-10).
//
// ADAPTERS (documented, non-verbatim): the original reads car state through
// getters (FUN_0046d4a0 pos, FUN_0046cb30 vel, FUN_0046c7b0 alive,
// FUN_0046cbb0 dead-flag+timer, FUN_00408a50/0x00408ad0 progress); the host
// fills the same quantities from standalone race state in RaceCamCar.
// Modes 4/5/7/8/9/10 (DM/spectator variants) are NOT ported — the standalone
// round runs the standard path (mode 0; matches the live probe).
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Race {

struct RaceCamCar {
    float pos[3];        // vehicle +0x30/+0x34/+0x38 (FUN_0046d4a0)
    float vel[3];        // FUN_0046cb30 world velocity
    bool  active;        // IsCarSlotActive (0x0040e370, C3-ported)
    bool  alive;         // FUN_0046c7b0 == 1
    bool  dead_flag;     // FUN_0046cbb0 out1 (dying/exploded)
    float dead_ms;       // FUN_0046cbb0 out2 (ms since death)
    float path_prog;     // 0x00408a50: camera-path progress = gate idx + frac
    float race_pct;      // 0x00408ad0: race progress percent 0..100
};

struct RaceCamNode {     // gate-ribbon node (DAT_00663658 stride 0x4c)
    float dir[3];        // +0x00 unit race direction (FUN_00426cc0)
    float c0[3];         // corner j=0 (FUN_00426d00(n,0))
    float c3[3];         // corner j=3 (FUN_00426d00(n,3))
};

class RaceCamera {
public:
    // LE<course_id>.LED from Common/LED.piz. Missing file/entry => all
    // entries unset (elev = -1) which selects the -25°-tilt fallback path
    // exactly like the original (FUN_00441820 default branch).
    bool LoadLed(const char* led_piz_path, int course_id);

    void SetNodes(const RaceCamNode* nodes, int count);
    void Reset() { primed_ = false; }   // next Update runs the snap path

    // FUN_00446520 race branch. track_type: Course_Id (City==26==0x1a gets
    // the special pitch/zoom law). dt_blend = DAT_007f100c (1/60 live).
    // time_ticks = DAT_007f1030 equivalent (sway phase; ~3.0 MHz live).
    // jitter_amp = DAT_007f0fc8 (0.0 live). force_reset = param_2.
    void Update(const RaceCamCar cars[4], int track_type, float dt_blend,
                std::uint32_t time_ticks, float jitter_amp, bool force_reset,
                bool overhead = false);

    // FUN_00410d10 standard-path core. Call AFTER Update() each tick.
    // Returns victim index (car to eliminate) or -1. The 10.0 saturation
    // compare is exact equality, as in the original (fcomp at 0x00410ee3).
    int EliminationCheck(const RaceCamCar cars[4]) const;

    // FUN_0040e180 verbatim: most-separated (3D) pair among active+alive+
    // not-dying cars.
    static void MostSeparatedPair(const RaceCamCar cars[4], int* a, int* b);

    const float* pos() const { return pos_out_; }
    const float* target() const { return tgt_out_; }
    float required_zoom() const { return required_zoom_; }   // cam[0x268]
    float view_window() const { return view_window_; }       // cam[0x16]
    bool  has_led() const { return led_loaded_; }
    int   node_count() const { return node_count_; }

private:
    // FUN_00441820: per-node camera direction + height.
    void NodeDir(int node, float out_dir[3], float* out_h) const;
    // path sample at progress (0x00446f91..0x00447081 block, banker's round)
    void PathSample(float prog, float out_dir[3], float* out_h) const;

    static constexpr int kMaxNodes = 384;      // LED file capacity
    struct LedEntry { float elev, azim, height; };
    LedEntry led_[kMaxNodes];
    bool led_loaded_ = false;

    const RaceCamNode* nodes_ = nullptr;
    int  node_count_ = 0;

    // camera struct state (offsets cite 0x00897fe0 fields)
    float smooth_pos_[3] = {};   // +0x964 [0x259..0x25b]
    float spring_pos_[3] = {};   // +0x970 [0x25c..0x25e]
    float smooth_tgt_[3] = {};   // +0x97c [0x25f..0x261]
    float spring_tgt_[3] = {};   // +0x988 [0x262..0x264]
    int   pair_a_ = -1;          // +0x994 [0x265]
    int   pair_b_ = -1;          // +0x998 [0x266]
    float pair_blend_ = 0.f;     // +0x99c [0x267]
    float required_zoom_ = 0.f;  // +0x9a0 [0x268] — elimination metric
    bool  primed_ = false;       // forces the snap path on first frame

    float pos_out_[3] = {};      // +0x40 (and [1..3])
    float tgt_out_[3] = {};      // +0x4c (and [4..6])
    float view_window_ = 0.6f;   // +0x58
    std::uint32_t prng_ = 0x12345678;  // jitter PRNG state (adapter; the
                                       // original uses FUN_00534870)
};

}  // namespace Race
}  // namespace mashed_re
