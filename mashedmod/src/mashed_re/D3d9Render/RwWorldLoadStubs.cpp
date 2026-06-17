// Mashed RE — WS-E2: inert stub seam for the BSP->RpWorld loader (RwWorldLoad.cpp).
//
// The loader is a verbatim transcription of FUN_004e99b0/9e40/ea220; the RW
// stream/error/plugin/instance helpers it calls are runtime-bound RW-engine code.
// Until they are bound (to real RW-stream readers or the standalone Rws/Txd path —
// WS-E-DRIVER / B-full), these inert stubs make it compile + link. A stubbed
// FindChunk returns 0, so RwWorldLoad_StreamRead bails immediately (returns null)
// — the loader is INERT in the standalone; the spike world draw keeps shipping.
// Each cites the real RVA. Rwl_Alloc uses malloc so the (eventual) real readers can
// build the world without the RW heap; Rwl_WorldInstance returns success (device
// instancing is the B-full submit, deferred).
#include "RwWorldLoad.h"
#include <cstdint>
#include <cstdlib>

namespace mashed_re {
namespace D3d9Render {

// Rwl_StreamFindChunk / Rwl_StreamRead / Rwl_StreamReadReal are now REAL ports in
// RwWorldStream.cpp (WS-E-DEVICE stream half) — removed here to avoid duplicate symbols.
unsigned Rwl_ErrFmt(unsigned, unsigned)            { return 0; }          // FUN_004d7ff0
void  Rwl_ErrEmit(void*)                           {}                     // FUN_004d8480
void  Rwl_PluginCtor(void*, void*)                 {}                     // FUN_004d8000
int   Rwl_ReadExtensions(void*, void*, void*)      { return 1; }          // FUN_004e1b60
void  Rwl_BinMeshAttach(void*, int, void*, int)    {}                     // FUN_004e1c90
void* Rwl_Alloc(unsigned size, unsigned)           { return std::malloc(size); } // (**(RwGlobals+0x108))
void  Rwl_WorldSubInit(void*, unsigned)            {}                     // FUN_004e5280
void  Rwl_WorldSubInit2(void*)                     {}                     // FUN_004e51e0
void  Rwl_WorldSetRenderCB(void*, int)             {}                     // FUN_004e5820
void  Rwl_MaterialInit(void*, int)                 {}                     // FUN_004e8090
int   Rwl_MatListRead(void*, void*)                { return 1; }          // FUN_004f3e90
void  Rwl_WorldDestroy(void* w)                    { std::free(w); }      // RpWorldDestroy
int   Rwl_WorldInstance(void*)                     { return 1; }          // RpWorldInstance (device — deferred)
void  Rwl_FrameDestroy()                           {}                     // RwFrameDestroy
void  Rwl_FrameDestroyHierarchy(void*)             {}                     // RwFrameDestroyHierarchy

void* g_rpWorldPluginDesc  = nullptr;   // &DAT_0061864c
void* g_rpSectorPluginDesc = nullptr;   // &DAT_006189e4
int   g_binMeshWorldA = 0,  g_binMeshWorldB = 0;    // DAT_007d7278 / DAT_007d7274
int   g_binMeshSectorA = 0, g_binMeshSectorB = 0;   // DAT_007d727c / DAT_007d7270

} // namespace D3d9Render
} // namespace mashed_re
