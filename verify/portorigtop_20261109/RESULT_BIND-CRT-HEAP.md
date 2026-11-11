# RESULT BIND-CRT-HEAP (2026-11-11)

Verdict: **GREEN.** The original's four heap entry points are bound together on the MASHED_ORIG_TOP path to the exe CRT
heap (`Match/PortTop_CrtHeap.cpp`, exe-only, `// RVA-LINT: ORIGTOP-ONLY`). Runnable C4-by-match rows: **1,063 ->
1,064 (+1)**, as predicted.

## Identity and binding

| rva | function | identity | exe binding |
|---|---|---|---|
| 0x004a45fb | `_malloc` | libid malloc.obj identical-unlisted. Code: `_nh_malloc(size, [_newmode 0x00773c34])` | `MashedCrt_malloc_004a45fb` -> `std::malloc` |
| 0x004a460d | `_free` | libid free.obj identical-unlisted (the SBH-aware free) | `MashedCrt_free_004a460d` -> `std::free` |
| 0x004a3bda | `operator delete` | libid delete.obj identical: `jmp 0x004a460d` | `MashedCrt_delete_004a3bda` -> `std::free` |
| 0x004a3c1c | `operator new` | libid identical-ambiguous: the same 14 bytes as `_iswupper` (_wctype.obj) and `??2@YAPAXI@Z`. Settled from the code: `push 1; push [esp+8]; call 0x004a45cf`, and 0x004a45cf is `__nh_malloc`, the callee of `_malloc`. `_iswupper` would call `_iswctype` there. | `MashedCrt_new_004a3c1c` -> `std::malloc` |

**Failure semantics are kept.**
- The original never installs a new handler. `_pnhHeap` 0x00773c30 is read only by `__callnewh` 0x004aaf72 and written
  nowhere (xref).
- It never sets `_newmode`. 0x00773c34 is read only by `_malloc` / `_calloc` / `_realloc`.
- Both are zero-initialized, so malloc and new return NULL on failure without throwing. new therefore binds to
  `std::malloc`, not to the exe's throwing `operator new`.
- Size 0 gets a unique block on both CRTs. `free(NULL)` is a no-op on both.

## One owner: the exe CRT heap

- **librw** allocates through `rw::Engine::memfuncs = defaultMemfuncs` = `malloc_h` / `realloc_h` / `free`
  (`deps/librw/src/engine.cpp:170`, `Engine::init(nullptr)` in LibRw/RwBridge.cpp:140), which is the exe CRT heap.
- **RwGlobals** +0x108 / +0x10c / +0x110 (LibRw/RwApiBind.cpp `RwGlobalsMalloc` / `Free` / `Realloc`) route to those
  memfuncs. So a block allocated by matched code and freed through RwGlobals, or the reverse, stays on one heap.
- **RwGlobals freelist blocks** (+0x118, aligned, offset from the raw pointer) are returned only through +0x11c, never
  through `free`.
- **Hand code** in the exe uses the same CRT. The exe links one static CRT.
- **Seeds** (SEED_CANDIDATES.tsv) are static `.data` from the image. They hold no heap pointer.
- The original's own heap does not exist in the standalone, so after this binding no knob-path allocation can reach a
  second heap.

## Gates

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `.asi` not deployed. mashed_re.exe sha256 `c15357c3cb44fda85f5cb36c975c222f70cd5aa246b6905178613b0baadaa4a2` |
| knob boots, RWMOD off / on / on | rc 0, `WinMain 0x00492370 returned 0 after 3000 frames` in all three |
| 12 captures, RWMOD off / on | 8/8 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12` |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new |
| BINDINGS | `update_bindings_bindcrtheap.py` through `bindings_rows.apply`: 4 rows, kind `crt` (516 -> 520) |
| port column | regenerated, 0 rows changed (the four stay `none` like the other `crt` bindings) |
| runnable re-measure | 1,064 (`origtop_size.py`; SIZE.tsv updated) |

No caller of these four is compiled into the exe yet, so the TU is inert on both paths until a row binds them by
`#define`. STDIO is next and needs them for its buffers.
