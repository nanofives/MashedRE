// Mashed RE — Boot/VideoCfgFirstRun.cpp (EXE-ONLY): the original's first-run videocfg.bin default write
// (lane wsfvcfg, 2026-10-26, L-28 (2)). Knob MASHED_VIDEOCFG_WRITE, default ON since L-29 (a)
// (D4-FLIP-L29-L30), =0 opts out.
//
//   0x00499400  VideoSettingsDispatcher  first-run branch only (param_1 == 0, skip_selector forces it, 0x004994e3)
//   0x00498c00  VideoModeTableInit       subsystem 0's name and mode list (the only ones the branch reads)
//   0x004c7a70  RW D3D9 system fn        cases 0x10 / 5 / 6 / 0xe, the mode list builder 0x004c90a0
//   0x00498e40  default mode finder
//   0x00498ea0  settings snapshot into the 0x200-byte buffer (0x00773208), FUN_00498a00 the mode label
//
// The original writes the file before its device exists (FUN_004951f0 -> FUN_00499400), so this runs from InitD3D9
// between Direct3DCreate9 and CreateDevice, on the standalone's own IDirect3D9. The bytes depend on the adapter.
// The write goes through ConfigLoad / ConfigSave (Match/PortSave01_Config.cpp, re/match/s16): "videocfg.bin" in the exe's cwd.
//
// Departure: the original copies the adapter name into a 0x50-byte table entry per subsystem (0x00498c00), so a name of
// 80 or more characters runs into the next entry. The name is read straight from the adapter here.
//
// Read from the disassembly of original/MASHED.exe.unpatched; PREREG verify/wsfvcfg_20261026/PREREG_D4-WSF-VIDEOCFG.md.
// Anchor: MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (on .unpatched)
#include <d3d9.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

extern "C" int __cdecl ConfigLoad(void);   // Match/PortSave01_Config.cpp, 0x00498950 (re/match/s16)
extern "C" int __cdecl ConfigSave(void);   // Match/PortSave01_Config.cpp, 0x004989b0 (re/match/s16)
void* ConfigSettingsBuf();   // Save/SettingsConfig.cpp, the 0x00773208 buffer

namespace {

void Log(const char* m) {
    if (std::FILE* f = std::fopen("log/mashed_re.log", "a")) {
        std::fprintf(f, "[videocfg] %s\n", m); std::fclose(f);
    }
}

// RwVideoMode as case 6 fills it (0x18 bytes).
struct VideoMode { std::int32_t width, height, depth; std::uint32_t flags, refresh, format; };

// Mode list entry of 0x004c90a0 (0x14 bytes): D3DDISPLAYMODE + flags.
struct Entry { D3DDISPLAYMODE m; std::uint32_t flags; };

const D3DFORMAT kFormats[3] = {                                  // 0x005d8b80
    static_cast<D3DFORMAT>(0x17), static_cast<D3DFORMAT>(0x16), static_cast<D3DFORMAT>(0x23) };

// 0x004c90a0 (reached through case 0x10, 0x004c8040..0x004c80c5, on adapter 0)
std::vector<Entry> BuildModeList(IDirect3D9* d3d, UINT adapter) {
    UINT total = 0;
    for (D3DFORMAT f : kFormats) total += d3d->GetAdapterModeCount(adapter, f);       // case 0x10, vtable +0x18
    std::vector<Entry> e(total + 1);                                                   // (n * 5 + 5) * 4 bytes
    std::memset(e.data(), 0, e.size() * sizeof(Entry));
    d3d->GetAdapterDisplayMode(adapter, &e[0].m);                                      // vtable +0x20
    int n;
    switch (static_cast<int>(e[0].m.Format)) {
    case 0x15: case 0x16: case 0x17: case 0x18: case 0x19: case 0x23:
        e[0].flags = 0; n = 1; break;
    default:
        n = 0;
    }
    for (D3DFORMAT f : kFormats) {
        const int cnt = static_cast<int>(d3d->GetAdapterModeCount(adapter, f));        // vtable +0x18
        for (int j = 0; j < cnt; ++j) {
            Entry& cur = e[n];
            d3d->EnumAdapterModes(adapter, f, j, &cur.m);                              // vtable +0x1c
            bool dup = false;
            if (1 < n) {
                for (int i = 1; i < n; ++i) {
                    if (e[i].m.Width == cur.m.Width && e[i].m.Height == cur.m.Height &&
                        e[i].m.Format == cur.m.Format) {
                        if (e[i].m.RefreshRate < cur.m.RefreshRate) e[i].m.RefreshRate = cur.m.RefreshRate;
                        dup = true;
                        break;
                    }
                }
            }
            if (!dup) { cur.flags = 1; ++n; }
        }
    }
    e.resize(n);
    return e;
}

// case 6 (0x004c7a70)
VideoMode ModeInfo(const Entry& x) {
    VideoMode v;
    v.width = static_cast<std::int32_t>(x.m.Width);
    v.height = static_cast<std::int32_t>(x.m.Height);
    switch (static_cast<int>(x.m.Format)) {
    case 0x15: case 0x16: case 0x23: v.depth = 0x20; break;
    case 0x17: case 0x18: case 0x19: v.depth = 0x10; break;
    default: v.depth = 0;
    }
    v.flags = x.flags;
    v.refresh = x.m.RefreshRate;
    switch (static_cast<int>(x.m.Format)) {
    case 0x15: case 0x23: v.format = 0x500; break;
    case 0x16: v.format = 0x600; break;
    case 0x17: v.format = 0x200; break;
    case 0x18: v.format = 0xa00; break;
    case 0x19: v.format = 0x100; break;
    default: v.format = 0;
    }
    return v;
}

// 0x00498e40
int DefaultModeFind(const std::vector<VideoMode>& modes) {
    const int n = static_cast<int>(modes.size());
    for (int i = 0; i < n; ++i) {
        const VideoMode& m = modes[i];
        if (m.flags == 0) continue;                                   // 0x00498e63..0x00498e70
        if (m.width < 0x280) continue;                                // 0x00498e74
        if (m.width < 0x1e0) continue;                                // 0x00498e7c, width again (edx = [ecx])
        return i;                                                     // 0x00498e90
    }
    return 0;
}

// 0x00498a00 (EBX = the mode, 0x00498ef7)
void ModeLabel(char* out, const VideoMode& m) {
    char tag[0x84] = {};
    if ((m.flags & 1) == 0) std::strcat(tag, "[Window]");             // 0x005d01f4
    else                    std::strcat(tag, "[Fullscreen]");         // 0x005d0200
    if (m.flags & 2)     std::strcat(tag, "[Interlaced]");            // 0x005d01e4
    if (m.flags & 4)     std::strcat(tag, "[Flicker Free Interlaced]");          // 0x005d01c8
    if (m.flags & 0x100) std::strcat(tag, "[Fullscreen antialiasing mode 0]");   // 0x005d01a4
    if (m.flags & 0x200) std::strcat(tag, "[Fullscreen antialiasing mode 1]");   // 0x005d0180
    std::sprintf(out, "%d x %d x %d %s", m.width, m.height, m.depth, tag);      // 0x005d0170, FUN_004a2b60
}

void Put32(unsigned char* b, int off, std::uint32_t v) { std::memcpy(b + off, &v, 4); }

}  // namespace

// 0x00499400, first-run branch. Returns 1 when it wrote the file.
int VideoCfgFirstRunWrite(IDirect3D9* d3d) {
    static const bool s_on = [] { const char* e = std::getenv("MASHED_VIDEOCFG_WRITE");
                                  return !(e && e[0] == '0'); }();
    if (!s_on || !d3d) return 0;

    // 0x00498c00 for subsystem 0: name (case 0xe, 0x004c7ffb..0x004c802b) and modes (case 0x10 / 5 / 6).
    D3DADAPTER_IDENTIFIER9 id;
    std::memset(&id, 0, sizeof id);
    if (FAILED(d3d->GetAdapterIdentifier(0, 0, &id))) {               // vtable +0x14
        Log("GetAdapterIdentifier(0) failed, no write");
        return 0;
    }
    std::vector<VideoMode> modes;
    for (const Entry& x : BuildModeList(d3d, 0)) modes.push_back(ModeInfo(x));

    if (ConfigLoad()) {                                            // 0x00499555..0x0049955c
        Log("videocfg.bin present, no write");
        return 0;
    }
    const int subsys = 0;                                              // 0x00499576
    const int mode = DefaultModeFind(modes);                           // 0x0049957c..0x00499581
    if (modes.empty()) { Log("empty mode list, no write"); return 0; }

    // 0x00498ea0
    unsigned char* buf = static_cast<unsigned char*>(ConfigSettingsBuf());
    const VideoMode& m = modes[mode];
    Put32(buf, 0x0, 5);                                                // 0x00498ecb
    {   // rwstrcpy [RwGlobals+0xcc] (0x00498edb) into 0x0077320c, bounded by the buffer end
        const std::size_t room = 0x200 - 0x4 - 1;
        std::size_t len = std::strlen(id.Description);
        if (len > room) len = room;
        std::memcpy(buf + 0x4, id.Description, len);
        buf[0x4 + len] = 0;
    }
    char label[0x100];
    ModeLabel(label, m);                                               // 0x00498eff -> 0x0077330c
    {
        const std::size_t room = 0x200 - 0x104 - 1;
        std::size_t len = std::strlen(label);
        if (len > room) len = room;
        std::memcpy(buf + 0x104, label, len);
        buf[0x104 + len] = 0;
    }
    Put32(buf, 0x184, subsys);                                         // 0x00498ebc -> 0x0077338c
    Put32(buf, 0x188, mode);                                           // 0x00498ed5 -> 0x00773390
    Put32(buf, 0x18c, m.width);                                        // 0x00498f21 -> 0x00773394
    Put32(buf, 0x190, m.height);                                       // 0x00498f2e -> 0x00773398
    Put32(buf, 0x194, m.depth);                                        // 0x00498f3b -> 0x0077339c
    Put32(buf, 0x198, m.flags & 1);                                    // 0x00498f4b..0x00498f4f -> 0x007733a0

    const int ok = ConfigSave();                                   // 0x0049958b
    char line[256];
    std::snprintf(line, sizeof line, "first-run write ret=%d modes=%d pick=%d '%s' '%s'", ok,
                  static_cast<int>(modes.size()), mode, id.Description, label);
    Log(line);
    return ok;
}
