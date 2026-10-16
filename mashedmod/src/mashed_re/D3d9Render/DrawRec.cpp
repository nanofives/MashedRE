// [d1r1 2026-10-16] Per-draw D3D9 recorder. See DrawRec.h and verify/d1r1_20261016/PREREG_d1r1.md.
//
// The record schema matches re/frida/camera_probe.py --drawrec (the original-side twin), so
// re/tools/drawlist_diff.py can read both:
//   {"e":"d","k":slot,"pt":primType,"n":[counts],"o":[offsets],"tex":name|"-"|"#WxH:fmt",
//    "td":"#WxH:fmt","rs":[12 render states],"tss":[3],"w":[16]|null,"fvf":u,"vs":0|1,"st":stride,
//    "src":"rw"|"d3d"}
// slot = IDirect3DDevice9 vtable index: 81 DrawPrimitive, 82 DrawIndexedPrimitive,
// 83 DrawPrimitiveUP, 84 DrawIndexedPrimitiveUP.
#include "DrawRec.h"

#include <windows.h>
#include <d3d9.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

// librw-side hooks (deps/librw/src/d3d/d3d9render.cpp). librw draws every mesh through
// defaultRenderCB_Shader with vertex/pixel shaders, so the device's fixed-function world matrix
// is not the mesh's, and a material without a texture leaves stage 0 holding the previous
// texture (the setTexture call is skipped). The hooks hand over what librw is drawing.
extern void (*rw_drawrec_pre)(const char* texname, const float* ltm16);
extern void (*rw_drawrec_post)(void);

namespace mashed_re {
namespace DrawRec {
namespace {

using DP_t    = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRIMITIVETYPE, UINT, UINT);
using DIP_t   = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT,
                                            UINT, UINT);
using DPUP_t  = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRIMITIVETYPE, UINT, const void*,
                                            UINT);
using DIPUP_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRIMITIVETYPE, UINT, UINT, UINT,
                                            const void*, D3DFORMAT, const void*, UINT);

DP_t    s_dp = nullptr;
DIP_t   s_dip = nullptr;
DPUP_t  s_dpup = nullptr;
DIPUP_t s_dipup = nullptr;
bool    s_installed = false;

std::unordered_map<const void*, std::string> s_names;
std::vector<std::string> s_events;

// librw override for the draw in flight
bool        s_ov = false;
std::string s_ov_tex;
float       s_ov_w[16];

// The same render states, in the same order, as camera_probe.py RS_LIST:
// ZENABLE ZWRITEENABLE ZFUNC ALPHATESTENABLE ALPHAREF ALPHAFUNC ALPHABLENDENABLE SRCBLEND
// DESTBLEND CULLMODE FOGENABLE LIGHTING
const D3DRENDERSTATETYPE kRs[12] = {
    D3DRS_ZENABLE, D3DRS_ZWRITEENABLE, D3DRS_ZFUNC, D3DRS_ALPHATESTENABLE, D3DRS_ALPHAREF,
    D3DRS_ALPHAFUNC, D3DRS_ALPHABLENDENABLE, D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_CULLMODE,
    D3DRS_FOGENABLE, D3DRS_LIGHTING};
// camera_probe.py TSS_LIST: stage0 COLOROP, stage0 ALPHAOP, stage1 COLOROP
const DWORD kTss[3][2] = {{0, D3DTSS_COLOROP}, {0, D3DTSS_ALPHAOP}, {1, D3DTSS_COLOROP}};

void JsonStr(std::string& o, const char* s) {
    o += '"';
    for (const char* p = s; *p; ++p) {
        const unsigned char c = static_cast<unsigned char>(*p);
        if (c == '"' || c == '\\') { o += '\\'; o += static_cast<char>(c); }
        else if (c < 0x20 || c >= 0x7f) { char b[8]; std::snprintf(b, sizeof(b), "\\u%04x", c); o += b; }
        else o += static_cast<char>(c);
    }
    o += '"';
}

std::string TexDesc(IDirect3DBaseTexture9* t) {
    if (t->GetType() != D3DRTYPE_TEXTURE) {
        char b[32]; std::snprintf(b, sizeof(b), "#type%d", static_cast<int>(t->GetType()));
        return b;
    }
    D3DSURFACE_DESC d{};
    static_cast<IDirect3DTexture9*>(t)->GetLevelDesc(0, &d);
    char b[48];
    std::snprintf(b, sizeof(b), "#%ux%u:%u", d.Width, d.Height, static_cast<unsigned>(d.Format));
    return b;
}

// The draw call's own arguments, for the geometry read-back.
struct Call {
    int pt = 0;
    INT base = 0;              // DIP BaseVertexIndex
    UINT start = 0;            // DP StartVertex / DIP StartIndex
    UINT pc = 0;               // primitive count
    const void* up_v = nullptr;     // UP vertex data
    const void* up_i = nullptr;     // DIPUP index data
    D3DFORMAT up_fmt = D3DFMT_INDEX16;
    UINT up_stride = 0;
};

UINT NIdx(int pt, UINT pc) { return pt == 4 ? 3 * pc : (pt == 5 || pt == 6) ? pc + 2 : 0; }

void Tris(int pt, const std::vector<UINT>& idx, UINT pc, std::vector<UINT>& out3) {
    for (UINT k = 0; k < pc; ++k) {
        UINT a, b, c;
        if (pt == 4)      { a = idx[3 * k]; b = idx[3 * k + 1]; c = idx[3 * k + 2]; }
        else if (pt == 5) { a = idx[k]; b = idx[k + 1]; c = idx[k + 2]; }
        else              { a = idx[0]; b = idx[k + 1]; c = idx[k + 2]; }
        out3.push_back(a); out3.push_back(b); out3.push_back(c);
    }
}

// [d1r1 amendment 1] non-degenerate triangle count + world AABB of the vertices those
// triangles use: the same quantities camera_probe.py's geomOf reads on the original side.
// Positions are the first three floats of each vertex (FVF XYZ / librw's declarations).
void Geometry(IDirect3DDevice9* dev, int slot, const Call& c, const float* w, DWORD fvf,
              std::string& r) {
    if (c.pt < 4 || c.pt > 6) return;
    const bool screen = (fvf & 0xe) == D3DFVF_XYZRHW;
    const UINT n = NIdx(c.pt, c.pc);
    std::vector<UINT> idx;
    bool by_pos = false;                       // non-indexed: degenerate = repeated position
    const std::uint8_t* vbase = nullptr;
    UINT stride = 0;
    IDirect3DVertexBuffer9* vb = nullptr;
    IDirect3DIndexBuffer9* ib = nullptr;
    INT base = 0;
    if (slot == 82) {
        dev->GetIndices(&ib);
        if (!ib) return;
        D3DINDEXBUFFER_DESC d{};
        ib->GetDesc(&d);
        const UINT isz = d.Format == D3DFMT_INDEX32 ? 4 : 2;
        void* p = nullptr;
        if (FAILED(ib->Lock(c.start * isz, n * isz, &p, D3DLOCK_READONLY)) || !p) { ib->Release(); return; }
        for (UINT k = 0; k < n; ++k)
            idx.push_back(isz == 4 ? static_cast<const UINT*>(p)[k] : static_cast<const WORD*>(p)[k]);
        ib->Unlock();
        ib->Release();
        base = c.base;
    } else if (slot == 84) {
        const UINT isz = c.up_fmt == D3DFMT_INDEX32 ? 4 : 2;
        for (UINT k = 0; k < n; ++k)
            idx.push_back(isz == 4 ? static_cast<const UINT*>(c.up_i)[k] : static_cast<const WORD*>(c.up_i)[k]);
    } else {
        for (UINT k = 0; k < n; ++k) idx.push_back((slot == 81 ? c.start : 0) + k);
        by_pos = true;
    }
    if (slot == 81 || slot == 82) {
        UINT off = 0;
        dev->GetStreamSource(0, &vb, &off, &stride);
        if (!vb) return;
        void* p = nullptr;
        if (FAILED(vb->Lock(0, 0, &p, D3DLOCK_READONLY)) || !p) { vb->Release(); return; }
        vbase = static_cast<const std::uint8_t*>(p) + off;
    } else {
        vbase = static_cast<const std::uint8_t*>(c.up_v);
        stride = c.up_stride;
    }
    auto P = [&](UINT v) { return reinterpret_cast<const float*>(vbase + static_cast<std::size_t>(v + base) * stride); };
    // [d1r1] vertex alpha, as camera_probe.py's bbox(): D3DCOLOR offset from the FVF, or from
    // the bound declaration's stream-0 COLOR0 element (librw's shader path)
    int coff = -1;
    if (fvf & D3DFVF_DIFFUSE) {
        coff = (screen ? 16 : 12) + ((fvf & D3DFVF_NORMAL) ? 12 : 0);
    } else if (fvf == 0) {
        IDirect3DVertexDeclaration9* decl = nullptr;
        dev->GetVertexDeclaration(&decl);
        if (decl) {
            D3DVERTEXELEMENT9 el[MAXD3DDECLLENGTH + 1]; UINT ne = MAXD3DDECLLENGTH + 1;
            if (SUCCEEDED(decl->GetDeclaration(el, &ne)))
                for (UINT i = 0; i < ne && el[i].Stream != 0xFF; ++i)
                    if (el[i].Stream == 0 && el[i].Usage == D3DDECLUSAGE_COLOR &&
                        el[i].UsageIndex == 0 && el[i].Type == D3DDECLTYPE_D3DCOLOR)
                        coff = el[i].Offset;
            decl->Release();
        }
    }
    std::vector<UINT> t3;
    Tris(c.pt, idx, c.pc, t3);
    int tri = 0;
    float mn[3] = {1e30f, 1e30f, 1e30f}, mx[3] = {-1e30f, -1e30f, -1e30f};
    std::vector<UINT> used;
    for (std::size_t k = 0; k + 2 < t3.size(); k += 3) {
        const UINT a = t3[k], b = t3[k + 1], cc = t3[k + 2];
        bool degen;
        if (by_pos) {
            auto eq = [&](UINT u, UINT v) { const float* x = P(u); const float* y = P(v);
                return x[0] == y[0] && x[1] == y[1] && x[2] == y[2]; };
            degen = eq(a, b) || eq(b, cc) || eq(a, cc);
        } else {
            degen = (a == b || b == cc || a == cc);
        }
        if (degen) continue;
        ++tri;
        used.push_back(a); used.push_back(b); used.push_back(cc);
        for (UINT v : {a, b, cc}) {
            const float* q = P(v);
            float o[3] = {q[0], q[1], q[2]};
            if (!screen && w) {
                o[0] = q[0] * w[0] + q[1] * w[4] + q[2] * w[8] + w[12];
                o[1] = q[0] * w[1] + q[1] * w[5] + q[2] * w[9] + w[13];
                o[2] = q[0] * w[2] + q[1] * w[6] + q[2] * w[10] + w[14];
            }
            for (int i = 0; i < 3; ++i) { if (o[i] < mn[i]) mn[i] = o[i]; if (o[i] > mx[i]) mx[i] = o[i]; }
        }
    }
    if (coff >= 0 && !used.empty()) {
        std::sort(used.begin(), used.end());
        used.erase(std::unique(used.begin(), used.end()), used.end());
        int va = 0;
        for (UINT v : used)
            if (vbase[static_cast<std::size_t>(v + base) * stride + coff + 3] < 255) ++va;
        r += ",\"va\":" + std::to_string(va) + ",\"vn\":" + std::to_string(used.size());
    }
    if (vb) { vb->Unlock(); vb->Release(); }
    r += ",\"tri\":" + std::to_string(tri);
    if (tri > 0) {
        char b[256];
        std::snprintf(b, sizeof(b), ",\"bb\":[%.6g,%.6g,%.6g,%.6g,%.6g,%.6g]",
                      mn[0], mn[1], mn[2], mx[0], mx[1], mx[2]);
        r += b;
    }
    if (screen) r += ",\"screen\":1";
}

void Record(IDirect3DDevice9* dev, int slot, const Call& call, std::initializer_list<unsigned> n,
            std::initializer_list<int> o, int up_stride) {
    const int pt = call.pt;
    std::string r = "{\"e\":\"d\",\"k\":" + std::to_string(slot) + ",\"pt\":" + std::to_string(pt);
    r += ",\"n\":[";
    bool first = true;
    for (unsigned v : n) { if (!first) r += ','; r += std::to_string(v); first = false; }
    r += "],\"o\":[";
    first = true;
    for (int v : o) { if (!first) r += ','; r += std::to_string(v); first = false; }
    r += "]";

    IDirect3DBaseTexture9* t = nullptr;
    dev->GetTexture(0, &t);
    std::string td = t ? TexDesc(t) : "-";
    std::string tex;
    if (s_ov) {
        tex = s_ov_tex.empty() ? "-" : s_ov_tex;
    } else if (!t) {
        tex = "-";
    } else {
        auto it = s_names.find(t);
        tex = (it != s_names.end()) ? it->second : td;
    }
    if (t) t->Release();
    r += ",\"tex\":"; JsonStr(r, tex.c_str());
    r += ",\"td\":"; JsonStr(r, td.c_str());

    r += ",\"rs\":[";
    for (int i = 0; i < 12; ++i) {
        DWORD v = 0xdeadbeef;
        const HRESULT hr = dev->GetRenderState(kRs[i], &v);
        if (i) r += ',';
        r += (hr == D3D_OK) ? std::to_string(v) : std::string("-1");
    }
    r += "],\"tss\":[";
    for (int i = 0; i < 3; ++i) {
        DWORD v = 0xdeadbeef;
        const HRESULT hr = dev->GetTextureStageState(kTss[i][0],
                                                     static_cast<D3DTEXTURESTAGESTATETYPE>(kTss[i][1]), &v);
        if (i) r += ',';
        r += (hr == D3D_OK) ? std::to_string(v) : std::string("-1");
    }
    r += "],\"w\":[";
    float w[16];
    if (s_ov) {
        std::memcpy(w, s_ov_w, sizeof(w));
    } else {
        D3DMATRIX m{};
        dev->GetTransform(D3DTS_WORLD, &m);
        std::memcpy(w, &m, sizeof(w));
    }
    for (int i = 0; i < 16; ++i) {
        char b[32]; std::snprintf(b, sizeof(b), "%.9g", w[i]);
        if (i) r += ',';
        r += b;
    }
    DWORD fvf = 0;
    dev->GetFVF(&fvf);
    IDirect3DVertexShader9* vs = nullptr;
    dev->GetVertexShader(&vs);
    const int has_vs = vs ? 1 : 0;
    if (vs) vs->Release();
    unsigned st = static_cast<unsigned>(up_stride);
    if (up_stride < 0) {
        IDirect3DVertexBuffer9* vb = nullptr; UINT off = 0, stride = 0;
        dev->GetStreamSource(0, &vb, &off, &stride);
        if (vb) vb->Release();
        st = stride;
    }
    // [d1r1 amendment 1] stage-1 texture (the original's world meshes are two-stage)
    std::string tex1 = "-";
    {
        IDirect3DBaseTexture9* t1 = nullptr;
        dev->GetTexture(1, &t1);
        if (t1) {
            auto it = s_names.find(t1);
            tex1 = (it != s_names.end()) ? it->second : TexDesc(t1);
            t1->Release();
        }
    }
    r += "],\"fvf\":" + std::to_string(fvf) + ",\"vs\":" + std::to_string(has_vs) +
         ",\"st\":" + std::to_string(st) + ",\"src\":\"" + (s_ov ? "rw" : "d3d") + "\"";
    r += ",\"tex1\":"; JsonStr(r, tex1.c_str());
    Geometry(dev, slot, call, w, fvf, r);
    r += "}";
    s_events.push_back(std::move(r));
}

// Armed only for the frames around a replay shot (ArmForRow); a zero-primitive call rasterises
// nothing and is only counted (librw issues one per empty material mesh).
bool s_armed = false;
int  s_empty = 0;

HRESULT STDMETHODCALLTYPE Rec_DP(IDirect3DDevice9* d, D3DPRIMITIVETYPE pt, UINT sv, UINT pc) {
    if (s_armed && pc == 0) ++s_empty;
    else if (s_armed) {
        Call c; c.pt = pt; c.start = sv; c.pc = pc;
        Record(d, 81, c, {pc}, {static_cast<int>(sv)}, -1);
    }
    return s_dp(d, pt, sv, pc);
}
HRESULT STDMETHODCALLTYPE Rec_DIP(IDirect3DDevice9* d, D3DPRIMITIVETYPE pt, INT bv, UINT mi, UINT nv,
                                  UINT si, UINT pc) {
    if (s_armed && pc == 0) ++s_empty;
    else if (s_armed) {
        Call c; c.pt = pt; c.base = bv; c.start = si; c.pc = pc;
        Record(d, 82, c, {nv, pc}, {bv, static_cast<int>(mi), static_cast<int>(si)}, -1);
    }
    return s_dip(d, pt, bv, mi, nv, si, pc);
}
HRESULT STDMETHODCALLTYPE Rec_DPUP(IDirect3DDevice9* d, D3DPRIMITIVETYPE pt, UINT pc, const void* v,
                                   UINT st) {
    if (s_armed && pc == 0) ++s_empty;
    else if (s_armed) {
        Call c; c.pt = pt; c.pc = pc; c.up_v = v; c.up_stride = st;
        Record(d, 83, c, {pc}, {}, static_cast<int>(st));
    }
    return s_dpup(d, pt, pc, v, st);
}
HRESULT STDMETHODCALLTYPE Rec_DIPUP(IDirect3DDevice9* d, D3DPRIMITIVETYPE pt, UINT mi, UINT nv, UINT pc,
                                    const void* ix, D3DFORMAT fmt, const void* v, UINT st) {
    if (s_armed && pc == 0) ++s_empty;
    else if (s_armed) {
        Call c; c.pt = pt; c.pc = pc; c.up_v = v; c.up_i = ix; c.up_fmt = fmt; c.up_stride = st;
        Record(d, 84, c, {nv, pc}, {static_cast<int>(mi)}, static_cast<int>(st));
    }
    return s_dipup(d, pt, mi, nv, pc, ix, fmt, v, st);
}

void LibrwPre(const char* tex, const float* ltm) {
    s_ov = true;
    s_ov_tex = tex ? tex : "";
    // rw::Matrix = right,flags, up,pad, at,pad, pos,pad -> the D3DMATRIX the original's
    // RwD3D9SetTransformWorld builds (_14 = _24 = _34 = 0, _44 = 1)
    for (int i = 0; i < 16; ++i) s_ov_w[i] = ltm[i];
    s_ov_w[3] = s_ov_w[7] = s_ov_w[11] = 0.f;
    s_ov_w[15] = 1.f;
}
void LibrwPost() { s_ov = false; }

void Patch(void** vt, int slot, void* thunk, void** save) {
    DWORD old = 0, tmp = 0;
    if (!VirtualProtect(&vt[slot], sizeof(void*), PAGE_READWRITE, &old)) return;
    *save = vt[slot];
    vt[slot] = thunk;
    VirtualProtect(&vt[slot], sizeof(void*), old, &tmp);
}

}  // namespace

bool Enabled() {
    static const bool on = GetEnvironmentVariableA("MASHED_DBG_D3DREC", nullptr, 0) > 0;
    return on;
}

void NameTexture(const void* d3dtex, const char* name) {
    if (!Enabled() || !d3dtex || !name) return;
    s_names[d3dtex] = name;
}

void Install(IDirect3DDevice9* dev) {
    if (!Enabled() || s_installed || !dev) return;
    void** vt = *reinterpret_cast<void***>(dev);
    Patch(vt, 81, reinterpret_cast<void*>(&Rec_DP), reinterpret_cast<void**>(&s_dp));
    Patch(vt, 82, reinterpret_cast<void*>(&Rec_DIP), reinterpret_cast<void**>(&s_dip));
    Patch(vt, 83, reinterpret_cast<void*>(&Rec_DPUP), reinterpret_cast<void**>(&s_dpup));
    Patch(vt, 84, reinterpret_cast<void*>(&Rec_DIPUP), reinterpret_cast<void**>(&s_dipup));
    rw_drawrec_pre = &LibrwPre;
    rw_drawrec_post = &LibrwPost;
    s_installed = (s_dp && s_dip && s_dpup && s_dipup);
}

void BeginFrame(int prev_row) {
    if (!s_installed) return;
    s_events.clear();
    s_ov = false;
    s_empty = 0;
    // MASHED_CAM_REPLAY_SHOTS (the shot list exe_main.cpp dumps at): arm when the previous
    // frame consumed a row within 3 of a pending shot, so this frame's draws are kept if it
    // turns out to be the shot frame. Unarmed frames record nothing.
    static std::vector<int> s_shots = [] {
        std::vector<int> v; char b[512] = {};
        if (GetEnvironmentVariableA("MASHED_CAM_REPLAY_SHOTS", b, sizeof(b)) > 0)
            for (char* p = std::strtok(b, ","); p; p = std::strtok(nullptr, ","))
                v.push_back(std::atoi(p));
        return v; }();
    s_armed = false;
    for (int k : s_shots)
        if (prev_row >= k - 3 && prev_row <= k) s_armed = true;
}

bool Write(const char* path, int row, int actual_row) {
    if (!s_installed || !path) return false;
    std::FILE* f = std::fopen(path, "wb");
    if (!f) return false;
    std::fprintf(f, "{\"side\":\"stand\",\"row\":%d,\"actual_row\":%d,\"armed\":%d,\"empty\":%d,"
                    "\"events\":[", row, actual_row, static_cast<int>(s_armed), s_empty);
    for (std::size_t i = 0; i < s_events.size(); ++i)
        std::fprintf(f, "%s%s", i ? ",\n" : "\n", s_events[i].c_str());
    std::fprintf(f, "\n]}\n");
    std::fclose(f);
    return true;
}

}  // namespace DrawRec
}  // namespace mashed_re
