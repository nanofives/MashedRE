// PORT-OT-IMAGE-FMT (MASHED_ORIG_TOP path, knob MASHED_ORIG_TOP_RWMOD): RwImageRegisterImageFormat 0x004ce790 and the
// two read functions 0x00493600 registers with it. EXE ONLY (exe_sources.rsp, not asi_sources.rsp).
// RVA-LINT: ORIGTOP-ONLY
//
// rva 0x004ce790: re/match/s20/004ce790.c (C4 match, 328 B byte-identical) compiled as Orig_004ce790. It runs on the RwGlobals
// record the knob path installs (LibRw/RwApiBind.cpp RegisterRwModules): slots +0xd0 / +0xe8 / +0xf8 / +0xfc / +0x118 and
// the image module at *(0x007d4628) = 0x170 (+0x218 freelist header 0x007d4604, +0x21c the node list). librw keeps its
// own format list (rw::Image::registerFileFormat, filled with tga / bmp / png at rw::Engine::init, deps/librw/src/
// engine.cpp:320-322). This is the RW 3.x-layout list that the matched 0x004ce8e0 (re/match/s20/004ce8e0.c) walks.
//
// rva 0x004935a0 / 0x004935d0: hand ports from the disassembly (original/MASHED.exe.unpatched). The matched C sources
// (re/match/s15/004935a0.c, 004935d0.c) declare the functions void and call 0x00496400 with two arguments through a
// one-parameter prototype, which does not compile as C++. The disassembly also shows the value they return.

#ifdef MASHED_STANDALONE

#define MATCH_PORT 1

// ---- bindings of rva 0x004ce790 ----
#define FUN_004ce790 Orig_004ce790

extern "C" {
#include "../../../../re/match/s20/004ce790.c"

void Orig_00496400(const char* fmt, ...);                          // 0x00496400 matched, Match/PortTop_00496400.cpp
int MashedRw_TexDictionarySetCurrent_004c5c80(int dict);           // LibRw/RwApiBind.cpp
void* MashedRw_RtBMPImageRead_0052cb00(const char* name);          // LibRw/RwApiBind.cpp -> rw::readBMP
void* MashedRw_RtPNGImageRead_00514330(const char* name);          // LibRw/RwApiBind.cpp -> rw::readPNG

// Disassembly: at 0x004935a1 esi = [esp+8] (the name). At 0x004935a5 push esi, at 0x004935a6 push 0x5cf734
// ("Reading texture %s\n"), at 0x004935ab call 0x00496400. At 0x004935b0 push 0, at 0x004935b2 call 0x004c5c80.
// At 0x004935b7 push esi, at 0x004935b8 call 0x0052cb00. At 0x004935bd add esp,0x10 / pop esi / ret: eax is not written
// after the last call, so the result of 0x0052cb00 is returned.
// 0x004935a0 PortTop_ImgRead_004935a0 (the "bmp" read function)
void* PortTop_ImgRead_004935a0(const char* name) {
    Orig_00496400("Reading texture %s\n", name);
    MashedRw_TexDictionarySetCurrent_004c5c80(0);
    return MashedRw_RtBMPImageRead_0052cb00(name);
}

// Disassembly: the same instructions, at 0x004935db call 0x00496400, at 0x004935e2 call 0x004c5c80, at 0x004935e8
// call 0x00514330, at 0x004935ed add esp,0x10 / pop esi / ret.
// 0x004935d0 PortTop_ImgRead_004935d0 (the "png" read function)
void* PortTop_ImgRead_004935d0(const char* name) {
    Orig_00496400("Reading texture %s\n", name);
    MashedRw_TexDictionarySetCurrent_004c5c80(0);
    return MashedRw_RtPNGImageRead_00514330(name);
}
} // extern "C"

#endif // MASHED_STANDALONE
