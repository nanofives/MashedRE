// BIND-CRT-STDIO (L-111 step 2, split of PORT-OT-BIND-CRT): the original's stdio / time CRT entry points on the
// MASHED_ORIG_TOP path, bound to the exe CRT. Identity per row (libid vs XP DDK libcmt.lib, settled from the code where
// libid is ambiguous):
//   fopen     rva 004a4541  libid identical-unlisted (fopen.obj / wfopen.obj); narrow: its callers pass char* names
//   fclose    rva 004a4368  libid fclose.obj identical
//   fread     rva 004a49cf  libid identical-ambiguous (same bytes as _fwrite); code: calls __fread_lk 0x004a48e6
//   fwrite    rva 004a4b22  libid identical-ambiguous (same bytes as _fread); code: calls __fwrite_lk 0x004a4a1b
//   _vsnprintf rva 004a3b84 libid vsnprint.obj identical-unlisted
//   _ctime64  rva 004a43b9  libid identical-unlisted (ctime / ctime64 / wctime / wctime64); code: localtime 0x004a9858
//                           reads a 64-bit time ([edi], [edi+4]), callers pass __int64* and take char*: _ctime64
//   _tzset    rva 004aa00c  libid tzset.obj identical
//   fputs     rva 004a4254  libid DIFFERS from fputs.obj (prologue / argument-load order, mnemonic similarity 0.82);
//                           code-level identity: __lock_file, _strlen 0x004a9410, __stbuf, __fwrite_lk 0x004a4a1b,
//                           __ftbuf, unlock, return (written == length) ? 0 : -1 (sete / dec at 0x004a42b1) = VC7 fputs.
//                           Its callers are the mashed.log writers 0x004963e0 / 0x00496400.
//
// FILE objects: one owner, the exe CRT. No matched source dereferences a FILE field (the 16 callers only pass the
// pointer back to these functions), so the exe's FILE* never meets the DDK _iobuf layout.
// Paths: one rule, PortTop_Fs_DiskPath (Match/PortTop_RwStream04F.cpp, "original/<name>"), shared with the RwStream
// and file-exists bindings. fopen keeps the fix_fopen boot-patch contract (NULL, never a garbage FILE*).
// Write-mode policy (BIND-CRT-STDIO, run 37): a write overlay. "original/" is the shared reference install and is never
// opened for writing. Every mode with 'w', 'a' or '+' ("wb" 0x005d00d8 for the contcfg%d.bin / videocfg.bin writers
// 0x00497230 / 0x004982a0 / 0x004989b0, "wt" 0x005d0004 for the mashed.log opener 0x00496490) opens
// "<overlay>/<name>" instead, where <overlay> is MASHED_ORIGTOP_WRITE_DIR or "origtop_rw" (relative to the cwd, i.e.
// beside original/, the directory the exe already writes videocfg.bin / mashed_re_gamesave.bin into). 'a' and '+' copy
// the original/ file into the overlay first when the overlay has none. Read modes open the overlay copy when it exists,
// else original/, so a written config reads back on the next boot. MASHED_ORIGTOP_WRITE_DIR=off restores the refusal
// (write modes return NULL). The overlay keeps the hand path's own files (videocfg.bin, mashed_re_*) separate.
// Time zone: one owner, the exe CRT. The matched _ftime64 (0x004a43d2, re/match/s17) calls _tzset and then reads
// _timezone at 0x00616670 directly; the DDK __tzset writes _timezone / _daylight / _dstbias at 0x00616670 / 74 / 78.
// The _tzset binding runs the exe _tzset and publishes those three into the original addresses. _ctime64 runs on the
// exe CRT's own state.
// EXE-ONLY: the .asi runs inside MASHED.exe and calls the originals directly.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <time.h>
#include <cstdlib>
#include <direct.h>
#include <io.h>

extern "C" void PortTop_Fs_DiskPath(const char* name, char* path, unsigned size);

// The original's _timezone / _daylight / _dstbias block. The self-test (verify/bindcrtstdio_20261111) points it at a buffer.
#ifndef MASHED_CRT_TZ_BASE
#define MASHED_CRT_TZ_BASE 0x00616670u
#endif

namespace {
void LogStdio(const char* fmt, const char* a, const char* b) {
    static int s_n = 0;
    if (s_n++ >= 16) return;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] [BIND-CRT-STDIO] ");
        std::fprintf(f, fmt, a, b);
        std::fputc('\n', f);
        std::fclose(f);
    }
}
// The write overlay root: MASHED_ORIGTOP_WRITE_DIR, else "origtop_rw"; "off" / "0" = none (write modes refused).
bool OverlayPath(const char* name, char* out, unsigned size) {
    const char* root = std::getenv("MASHED_ORIGTOP_WRITE_DIR");
    if (!root || !root[0]) root = "origtop_rw";
    if (!std::strcmp(root, "off") || !std::strcmp(root, "0")) return false;
    char rel[400];
    PortTop_Fs_DiskPath(name, rel, sizeof(rel));                        // "original/<name>", forward slashes
    std::snprintf(out, size, "%s/%s", root, rel + 9);                   // drop "original/"
    return true;
}

void MakeParentDirs(const char* file) {
    char d[512];
    std::snprintf(d, sizeof(d), "%s", file);
    for (char* c = d; *c; ++c) {
        if ((*c == '/' || *c == '\\') && c != d) {
            const char keep = *c;
            *c = '\0';
            _mkdir(d);
            *c = keep;
        }
    }
}

void CopyFileBytes(const char* from, const char* to) {
    std::FILE* in = std::fopen(from, "rb");
    if (!in) return;
    if (std::FILE* out = std::fopen(to, "wb")) {
        char buf[4096];
        std::size_t n;
        while ((n = std::fread(buf, 1, sizeof(buf), in)) > 0) std::fwrite(buf, 1, n, out);
        std::fclose(out);
    }
    std::fclose(in);
}
}  // namespace

extern "C" {

std::FILE* __cdecl MashedCrt_fopen_004a4541(const char* name, const char* mode) {
    if (!name || !mode) return nullptr;
    char path[400];
    PortTop_Fs_DiskPath(name, path, sizeof(path));                     // "original/<name>"
    char over[512];
    const bool haveOverlay = OverlayPath(name, over, sizeof(over));
    const bool write = std::strchr(mode, 'w') || std::strchr(mode, 'a') || std::strchr(mode, '+');
    std::FILE* f = nullptr;
    if (write) {
        if (!haveOverlay) {
            LogStdio("fopen(\"%s\", \"%s\") refused: write mode, MASHED_ORIGTOP_WRITE_DIR=off, returns NULL", name, mode);
            return nullptr;
        }
        MakeParentDirs(over);
        if ((std::strchr(mode, 'a') || std::strchr(mode, '+')) && _access(over, 0) != 0) CopyFileBytes(path, over);
        f = std::fopen(over, mode);
        LogStdio("fopen(\"%s\", \"%s\") write -> overlay", name, mode);
    } else {
        f = std::fopen(haveOverlay && _access(over, 0) == 0 ? over : path, mode);
    }
    if (reinterpret_cast<std::uintptr_t>(f) < 0x10000u) f = nullptr;  // fix_fopen contract: NULL, never garbage
    return f;
}

int __cdecl MashedCrt_fputs_004a4254(const char* str, std::FILE* f) { return f && str ? std::fputs(str, f) : EOF; }

int __cdecl MashedCrt_fclose_004a4368(std::FILE* f) { return f ? std::fclose(f) : EOF; }

unsigned int __cdecl MashedCrt_fread_004a49cf(void* buf, unsigned int size, unsigned int n, std::FILE* f) {
    return f ? static_cast<unsigned int>(std::fread(buf, size, n, f)) : 0u;
}

unsigned int __cdecl MashedCrt_fwrite_004a4b22(const void* buf, unsigned int size, unsigned int n, std::FILE* f) {
    return f ? static_cast<unsigned int>(std::fwrite(buf, size, n, f)) : 0u;
}

// The DDK _vsnprintf (VC7 output.c) prints three-digit exponents, rounds half away from zero and writes 1.#INF / -1.#IND.
// The UCRT default does none of these, so the call passes the UCRT's legacy option bits: LEGACY_VSPRINTF_NULL_TERMINATION
// (the _vsnprintf contract: -1 on truncation, no terminator), LEGACY_MSVCRT_COMPATIBILITY (inf / nan text) and
// LEGACY_THREE_DIGIT_EXPONENTS, without STANDARD_ROUNDING. verify/bindcrtstdio_20261111/pf_probe: 25 of 26 format cases
// equal the DDK libcmt output; the one left is %a, which the DDK does not implement.
int __cdecl MashedCrt_vsnprintf_004a3b84(char* buf, unsigned int count, const char* fmt, char* args) {
    return __stdio_common_vsprintf(_CRT_INTERNAL_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION |
                                       _CRT_INTERNAL_PRINTF_LEGACY_MSVCRT_COMPATIBILITY |
                                       _CRT_INTERNAL_PRINTF_LEGACY_THREE_DIGIT_EXPONENTS,
                                   buf, count, fmt, nullptr, reinterpret_cast<va_list>(args));
}

// The DDK asctime writes the day of the month as two digits ("Nov 09"), the UCRT pads it with a space ("Nov  9").
// Character 8 of "Www Mmm dd hh:mm:ss yyyy\n" is the tens digit. Same NULL for a negative time on both
// (verify/bindcrtstdio_20261111/edge_probe).
char* __cdecl MashedCrt_ctime64_004a43b9(const __int64* t) {
    char* s = _ctime64(t);
    if (s && s[8] == ' ') s[8] = '0';
    return s;
}

// _daylight: with TZ set the DDK __tzset stores the first character of the DST zone name (TZ=PST8PDT -> 0x50 'P'), the
// UCRT stores 1. With TZ unset both store the GetTimeZoneInformation flag. _timezone and _dstbias are equal on both
// (verify/bindcrtstdio_20261111/tz_probe, 7 TZ settings). The mirror writes the DDK value.
void __cdecl MashedCrt_tzset_004aa00c(void) {
    _tzset();
    long tz = 0, dst = 0;
    int day = 0;
    _get_timezone(&tz);
    _get_daylight(&day);
    _get_dstbias(&dst);
    const char* env = std::getenv("TZ");
    if (day && env && env[0]) {
        char name[64] = {};
        std::size_t n = 0;
        if (_get_tzname(&n, name, sizeof(name), 1) == 0) day = static_cast<unsigned char>(name[0]);
    }
    *reinterpret_cast<long*>(MASHED_CRT_TZ_BASE + 0u) = tz;    // 0x00616670 _timezone, read by the matched _ftime64 0x004a43d2
    *reinterpret_cast<int*>(MASHED_CRT_TZ_BASE + 4u) = day;    // 0x00616674 _daylight
    *reinterpret_cast<long*>(MASHED_CRT_TZ_BASE + 8u) = dst;   // 0x00616678 _dstbias
}

}  // extern "C"
#endif  // MASHED_STANDALONE
