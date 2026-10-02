// D2SinkProbe.cpp — DIAGNOSTIC ONLY, default-OFF. See D2SinkProbe.h.
#include "D2SinkProbe.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace mashed_re {
namespace D2Sink {
namespace {

const char* Path() {
    static const char* const p = std::getenv("MASHED_D2SINK");
    return (p && p[0]) ? p : nullptr;
}

int   s_frame = -1;
long  s_lines = 0;
int   s_sub   = -1;
int   s_pass  = 0;
float s_chunk = 0.f;
float s_rem   = 0.f;
int   s_fx    = 0;
bool  s_active = false;

constexpr long kLineCap = 400000;

inline float Rf(const void* r, unsigned off) {
    return *reinterpret_cast<const float*>(reinterpret_cast<const char*>(r) + off);
}
inline int Ri(const void* r, unsigned off) {
    return *reinterpret_cast<const int*>(reinterpret_cast<const char*>(r) + off);
}

}  // namespace

bool Armed() { return Path() != nullptr; }

void Begin(int slot) {
    if (!Armed()) return;
    s_active = (slot == 0);
    if (!s_active) return;
    ++s_frame;
    s_fx = 0;
    s_sub = -1;
    s_pass = 0;
    s_chunk = 0.f;
    s_rem = 0.f;
}

void SetSubstep(int sub, int pass, float chunk, float rem) {
    s_sub = sub; s_pass = pass; s_chunk = chunk; s_rem = rem;
}

void ClearSubstep() { s_sub = -1; s_pass = 0; s_chunk = 0.f; s_rem = 0.f; }

void NoteFixup() { ++s_fx; }

void Mark(const char* tag, const void* rec, int sub, int pass, float chunk, float rem) {
    const char* const path = Path();
    if (!path || !s_active || s_lines >= kLineCap || !rec) return;
    if (sub >= 0) { s_sub = sub; s_pass = pass; s_chunk = chunk; s_rem = rem; }
    const float vx = Rf(rec, 0x9b0), vy = Rf(rec, 0x9b4), vz = Rf(rec, 0x9b8);
    const float mag = std::sqrt(vx * vx + vy * vy + vz * vz);
    std::FILE* f = std::fopen(path, "a");
    if (!f) return;
    std::fprintf(f,
        "f=%d tag=%s sub=%d pass=%d v=(%.9g,%.9g,%.9g) mag=%.9g r9e4=%.9g "
        "r9e0=%.9g r9f0=%d r9ec=%d key0=%d chunk=%.9g rem=%.9g fx=%d\n",
        s_frame, tag, s_sub, s_pass, vx, vy, vz, mag, Rf(rec, 0x9e4),
        Rf(rec, 0x9e0), Ri(rec, 0x9f0), Ri(rec, 0x9ec), Ri(rec, 0x4ac),
        s_chunk, s_rem, s_fx);
    std::fclose(f);
    ++s_lines;
}

void MarkWheel(const char* tag, const void* rec, int wheel,
               float a, float b, float c, float d) {
    const char* const path = Path();
    if (!path || !s_active || s_lines >= kLineCap || !rec) return;
    const float vx = Rf(rec, 0x9b0), vy = Rf(rec, 0x9b4), vz = Rf(rec, 0x9b8);
    const float mag = std::sqrt(vx * vx + vy * vy + vz * vz);
    std::FILE* f = std::fopen(path, "a");
    if (!f) return;
    std::fprintf(f,
        "f=%d tag=%s sub=%d pass=%d v=(%.9g,%.9g,%.9g) mag=%.9g r9e4=%.9g "
        "r9e0=%.9g r9f0=%d r9ec=%d key0=%d chunk=%.9g rem=%.9g fx=%d "
        "w=%d a=%.9g b=%.9g c=%.9g d=%.9g\n",
        s_frame, tag, s_sub, s_pass, vx, vy, vz, mag, Rf(rec, 0x9e4),
        Rf(rec, 0x9e0), Ri(rec, 0x9f0), Ri(rec, 0x9ec), Ri(rec, 0x4ac),
        s_chunk, s_rem, s_fx, wheel, a, b, c, d);
    std::fclose(f);
    ++s_lines;
}

}  // namespace D2Sink
}  // namespace mashed_re
