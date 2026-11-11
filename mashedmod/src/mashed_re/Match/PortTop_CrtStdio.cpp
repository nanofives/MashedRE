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
// Write modes ("wb" 0x005d00d8 for the config / save writers, "wt" 0x005d0004 for the mashed.log boot log) return NULL
// and log one line: "original/" is the shared reference install and must not be written. Where those writes should go
// is open (RESULT_BIND-CRT-STDIO.md); the original's own boot log is already disabled by the disable_log patch.
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

extern "C" void PortTop_Fs_DiskPath(const char* name, char* path, unsigned size);

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
}  // namespace

extern "C" {

std::FILE* __cdecl MashedCrt_fopen_004a4541(const char* name, const char* mode) {
    if (!name || !mode) return nullptr;
    if (std::strchr(mode, 'w') || std::strchr(mode, 'a') || std::strchr(mode, '+')) {
        LogStdio("fopen(\"%s\", \"%s\") refused: write mode (original/ is the reference install), returns NULL", name, mode);
        return nullptr;
    }
    char path[400];
    PortTop_Fs_DiskPath(name, path, sizeof(path));
    std::FILE* f = std::fopen(path, mode);
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

int __cdecl MashedCrt_vsnprintf_004a3b84(char* buf, unsigned int count, const char* fmt, char* args) {
    return _vsnprintf(buf, count, fmt, reinterpret_cast<va_list>(args));
}

char* __cdecl MashedCrt_ctime64_004a43b9(const __int64* t) { return _ctime64(t); }

void __cdecl MashedCrt_tzset_004aa00c(void) {
    _tzset();
    long tz = 0, dst = 0;
    int day = 0;
    _get_timezone(&tz);
    _get_daylight(&day);
    _get_dstbias(&dst);
    *reinterpret_cast<long*>(0x00616670u) = tz;    // _timezone, read by the matched _ftime64 0x004a43d2
    *reinterpret_cast<int*>(0x00616674u) = day;    // _daylight
    *reinterpret_cast<long*>(0x00616678u) = dst;   // _dstbias
}

}  // extern "C"
#endif  // MASHED_STANDALONE
