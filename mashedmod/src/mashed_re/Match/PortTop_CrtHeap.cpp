// BIND-CRT-HEAP (L-111 step 2, split of PORT-OT-BIND-CRT): the original's four heap entry points, bound together on the
// MASHED_ORIG_TOP path to the exe CRT heap, so every allocation on the knob path lives on one heap.
//   _malloc          rva 004a45fb  libid malloc.obj identical-unlisted: _nh_malloc(size, _newmode)
//   _free            rva 004a460d  libid free.obj identical-unlisted (SBH-aware free)
//   operator delete  rva 004a3bda  libid delete.obj identical: jmp _free
//   operator new     rva 004a3c1c  libid identical-ambiguous (same 14 bytes as _iswupper in _wctype.obj and as
//                                  ??2@YAPAXI@Z); settled from the code: it calls 0x004a45cf = __nh_malloc, the callee
//                                  of _malloc (_iswupper would call _iswctype), so it is operator new = _nh_malloc(size, 1)
// Failure semantics: the original never installs a new handler (_pnhHeap 0x00773c30 is read only by __callnewh
// 0x004aaf72, written nowhere) and never sets _newmode (0x00773c34 is read only by malloc / calloc / realloc), both
// zero-initialised: malloc and new return NULL on failure, no exception. So new binds to std::malloc, not to the exe's
// throwing operator new. Size 0 gives a unique block on both CRTs.
// One owner: the exe CRT heap. librw allocates through rw::Engine::memfuncs = defaultMemfuncs (malloc_h / realloc_h /
// free, deps/librw/src/engine.cpp:170, Engine::init(nullptr)), and RwGlobals +0x108 / +0x10c / +0x110 route to those, so
// memory crosses between matched code, RwGlobals and librw on the same heap. The RwGlobals freelist blocks (+0x118) are
// returned only through +0x11c. Seeds are static .data (no heap pointers).
// EXE-ONLY: the .asi runs inside MASHED.exe and calls the originals directly.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdlib>
extern "C" {
void* __cdecl MashedCrt_malloc_004a45fb(unsigned int n) { return std::malloc(n); }
void __cdecl MashedCrt_free_004a460d(void* p) { std::free(p); }
void __cdecl MashedCrt_delete_004a3bda(void* p) { std::free(p); }
void* __cdecl MashedCrt_new_004a3c1c(unsigned int n) { return std::malloc(n); }
}  // extern "C"
#endif  // MASHED_STANDALONE
