// PORT-TOP-02: 0x00499ba0 window create (re/match/s16/00499ba0.c, MATCH-S16 h6), compiled verbatim for the
// MASHED_ORIG_TOP knob path (see PortTop_00492370.cpp). EXE ONLY. No <windows.h> here: the source carries its own
// Win32 typedefs and imports.
//   rva 0x00499820 window procedure -> matched, PortTop_00499820.cpp
//   rva 0x00498bc0 / 0x00498bd0 render width / height -> stand-in: the exe resolution (kWidth/kHeight). The exe body
//     VideoGetRenderWidth reads DAT_00616028, zero in the exe; the reference returns 640x480 through the
//     fix_camera_res patch.
//   rva 0x00499890 OS-version log -> matched, PortTop_00499890.cpp
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00499ba0 Orig_00499ba0
#define FUN_00499820 Orig_00499820
#define FUN_00498bc0 PortTop_Width
#define FUN_00498bd0 PortTop_Height
#define FUN_00499890 Orig_00499890
extern "C" {
#include "../../../../re/match/s16/00499ba0.c"
}
#endif // MASHED_STANDALONE
