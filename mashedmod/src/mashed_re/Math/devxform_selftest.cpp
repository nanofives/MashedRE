// Mashed RE — WS-A-DEVXFORM self-test for RwV3dTransformPointsCPU.
// Build: cl /nologo /EHsc devxform_selftest.cpp RwV3dTransformPointsCPU.cpp
#include <cstdio>
#include <cmath>
namespace mashed_re { namespace Math {
    void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m);
} }
static bool near(float a, float b) { return std::fabs(a - b) < 1e-5f; }
int main() {
    using mashed_re::Math::RwV3dTransformPointsCPU;
    // 90deg about Z: right=(0,1,0) up=(-1,0,0) at=(0,0,1) pos=0  -> (1,0,0) maps to (0,1,0)
    float mz[16] = { 0,1,0,0,  -1,0,0,0,  0,0,1,0,  0,0,0,1 };
    float in[3] = {1,0,0}, out[3];
    RwV3dTransformPointsCPU(out, in, 1, mz);
    bool r1 = near(out[0],0) && near(out[1],1) && near(out[2],0);
    // translation: pos=(5,6,7), identity rot -> (1,2,3) maps to (6,8,10)
    float mt[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 5,6,7,1};
    float in2[3] = {1,2,3}, o2[3];
    RwV3dTransformPointsCPU(o2, in2, 1, mt);
    bool r2 = near(o2[0],6) && near(o2[1],8) && near(o2[2],10);
    std::printf("rot90Z (1,0,0)->(%.3f,%.3f,%.3f) [r1=%d]  trans->(%.1f,%.1f,%.1f) [r2=%d]  %s\n",
        out[0],out[1],out[2], r1, o2[0],o2[1],o2[2], r2, (r1&&r2)?"PASS":"FAIL");
    return (r1&&r2)?0:1;
}
