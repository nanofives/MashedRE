// Mashed RE — Track/KfInterpolate.h
// 0x00532b80 RpHAnimStdKeyFrameInterpolate: the one body, in both rsp files (lane kfdot, 2026-10-22).
#pragma once

namespace mashed_re {
namespace Track {

// On the keyframe fields (q x,y,z,w; t x,y,z): slerp + linear translation into oq / ot; bq is negated IN PLACE when
// the dot is negative. Callers: the Warzone boat's RtAnim interpolator (Track/WarzonePontoon.cpp) and, under
// MASHED_KF_INTERP, the Dump train's sampler (D3d9Render/TrackRenderer.cpp:LinearAnimMatrix, FUN_004b47e0).
void KfInterpolateFields(float* oq, float* ot, const float* aq, const float* at, float atime, float* bq, const float* bt,
                         float btime, float time);

}  // namespace Track
}  // namespace mashed_re
