// RWC-CORE-RUN (L-124 (C) step 2): CRT entry points the RW core installs or calls, bound to the exe CRT for the
// MASHED_RWC_CORE knob path. EXE ONLY. The original's targets are MSVC 7.0 libcmt bodies (identities from the
// RwStringFunctions slot order RwGlobals +0xc4..+0x104 that 0x004d8570 fills, and from the RwMemoryFunctions slots
// +0x108..+0x114 that 0x004cce20 fills). [UNCERTAIN] value identity with the DDK libcmt bodies (L-120 (b) is not
// available on this path yet): this row only brings the engine up; no replay is measured on it.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
// rva 0x004a2b60 sprintf
int RwCoreCrt_sprintf(char* b, const char* f, ...) { va_list ap; va_start(ap, f); int n = vsprintf(b, f, ap); va_end(ap); return n; }
// rva 0x004a42c5 vsprintf
int RwCoreCrt_vsprintf(char* b, const char* f, va_list ap) { return vsprintf(b, f, ap); }
// rva 0x004ac560 strcpy
char* RwCoreCrt_strcpy(char* d, const char* s) { return strcpy(d, s); }
// rva 0x004a34b0 strncpy
char* RwCoreCrt_strncpy(char* d, const char* s, size_t n) { return strncpy(d, s, n); }
// rva 0x004ac570 strcat
char* RwCoreCrt_strcat(char* d, const char* s) { return strcat(d, s); }
// rva 0x005c2300 strncat
char* RwCoreCrt_strncat(char* d, const char* s, size_t n) { return strncat(d, s, n); }
// rva 0x004a3860 strstr (RwGlobals +0xe4)
const char* RwCoreCrt_strstr(const char* a, const char* b) { return strstr(a, b); }
// rva 0x004af340 strcmp
int RwCoreCrt_strcmp(const char* a, const char* b) { return strcmp(a, b); }
// rva 0x004a3a90 strncmp (RwGlobals +0xec)
int RwCoreCrt_strncmp(const char* a, const char* b, size_t n) { return strncmp(a, b, n); }
// rva 0x004a9410 strlen
size_t RwCoreCrt_strlen(const char* s) { return strlen(s); }
// rva 0x004a3ac9 strtok
char* RwCoreCrt_strtok(char* s, const char* d) { return strtok(s, d); }
// rva 0x005c22bd sscanf
int RwCoreCrt_sscanf(const char* s, const char* f, ...) { va_list ap; va_start(ap, f); int n = vsscanf(s, f, ap); va_end(ap); return n; }
// rva 0x005aead0 realloc (RwGlobals +0x110)
void* RwCoreCrt_realloc(void* p, size_t n) { return realloc(p, n); }
// rva 0x004a2d30 pow
double RwCoreCrt_pow(double x, double y) { return pow(x, y); }
// rva 0x005c24e0 qsort (RWC-PLUGIN-ATTACH)
void RwCoreCrt_qsort(void* b, size_t n, size_t w, int (__cdecl* c)(const void*, const void*)) { qsort(b, n, w, c); }
} // extern "C"
#endif // MASHED_STANDALONE
