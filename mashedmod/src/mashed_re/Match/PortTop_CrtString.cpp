// BIND-CRT-STRING (split of PORT-OT-BIND-CRT): the original's strchr / strrchr / wcsncpy bound to the exe CRT on the
// MASHED_ORIG_TOP path. Identity (verify/bindcrtstring_20261112/RESULT_BIND-CRT-STRING.md):
//   _strchr   rva 004a2fd0  libid strchr.obj DIFFERS on raw bytes only: the image spells the DDK's one `inc edx` as
//                           `add edx, 1` (+2 bytes, so every later branch displacement shifts). The normalized listings
//                           (inc/dec -> add/sub 1, branch targets by instruction index) are equal over the object's whole
//                           .text (found_bx tail at 0x004a2fc0 included), the next instruction (cmp) rewrites CF, and a
//                           4000-input unicorn run of both bodies gives the C-standard result on every input.
//   _strrchr  rva 004a3480  libid strrchr.obj DIFFERS the same way: 3 inc/dec in the DDK object are add/sub 1 in the
//                           image (+6 bytes). Normalized listings equal, CF rewritten before any read, 4000/4000 runs.
//   _wcsncpy  rva 004a3bdf  libid wcsncpy.obj identical (61 bytes, no relocations).
// All three are pure functions of their arguments (no CRT state, no locale), so the exe CRT computes the same result.
// strchr / strrchr search for (char)c, including c == 0 (pointer to the terminator), as the C standard requires.
// NOT bound here: 0x005a7b40 (filed msvc-crt as AudioCtxSwap5a7b40). Its 16 bytes equal both fpinit.obj ___setfflag
// and user.obj __set_inconsistency, its global 0x007dcabc is also read at 0x005a7855 / 0x005a7894 / 0x005a7907 /
// 0x005a7a19 / 0x005a7a2c, and its five callers are 0x005a7c9b..0x005a7e1a, all outside the CRT block. Identity open.
// EXE-ONLY: the .asi runs inside MASHED.exe and calls the originals directly.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstring>
#include <cwchar>
extern "C" {
// 0x004a2fd0
char* __cdecl MashedCrt_strchr_004a2fd0(const char* s, int c) { return const_cast<char*>(std::strchr(s, c)); }
// 0x004a3480
char* __cdecl MashedCrt_strrchr_004a3480(const char* s, int c) { return const_cast<char*>(std::strrchr(s, c)); }
// 0x004a3bdf
wchar_t* __cdecl MashedCrt_wcsncpy_004a3bdf(wchar_t* d, const wchar_t* s, unsigned int n) { return std::wcsncpy(d, s, n); }
}  // extern "C"
#endif  // MASHED_STANDALONE
