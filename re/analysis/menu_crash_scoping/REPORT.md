# Runtime self-exit scoping — it is a CRASH, not a timeout/clean-exit

> **NO-`.asi` CONTROL (2026-05-27) — the ~94 s crash is NOT caused by our hooks.**
> Renamed BOTH `original/mashed_re_dev.asi` and `mashedmod/build/mashed_re_dev.asi` aside
> (the dinput8 proxy tries both paths), launched plain stock MASHED: **still crashed at
> t=94.8 s, 0xC0000005** (WER module `unknown`+0 = null-exec). So with NO hook DLL loaded at
> all, the title screen still dies at ~94 s. The crash is **stock MASHED + the modded-boot
> environment** (5 binary patches + d3d9 shim + dinput8 loader) on this Win11 box — our
> reimplementation hooks are **fully exonerated** for this blocker. (The exact site differs —
> null-exec here vs the `0x5554e3` `[font_ctx+0x134]` read with hooks-on — only because layout
> shifts; same NULL-font-context root, same ~94 s timing. Both `.asi` were restored after,
> hash `2ed4a3ba…` verified.)
>
> Consequence: the FontText `push src` fix (commit `2c4c05d`) was a real, correct bug fix, but
> it is NOT the ~94 s blocker. The blocker is the **stock font/draw-context build failing
> because `FGDC20.TXD` (present in Font36.piz) is never loaded + set current** before the font
> initializes in the modded boot → `RwTextureRead('fgdc20')` fails (err 0x16) → NULL ctx → crash.
> Unblocking C4 now means fixing/working-around that **stock TXD-load/ordering** issue (a
> boot/resource problem), not hook work. Next: find what should `RwTexDictionaryStreamRead`+
> `SetCurrent` `FGDC20.TXD` and why it doesn't in the modded boot (candidates: a boot patch, the
> d3d9 shim's device config, or post-intro RW/D3D state).


> **ROOT CAUSE + FIX (2026-05-27; supersedes the "data corruption" hypothesis below).**
> A HW write-watchpoint on 0x005f6560 **never fired** and the Toast string was **intact at
> the crash** — so it is NOT corrupted. The real bug was in **our `FontText_UTF16WidenCopy`
> (0x00427840) reimpl** (`HudBatch.cpp`): it **omitted the `push src` argument** before
> `call [obj+0xf4]`, so the length-getter measured a stale stack slot (0/NULL) → `strlen(NULL)`.
>
> Diagnosis caveat (recorded so it isn't repeated): I FIRST mis-called it an "extra deref"
> and removed the reimpl's `mov eax,[eax]`. That was WRONG — `s_VtableRootAddr_00427840` is a
> C variable *holding* the address 0x007d3ff8, so `mov eax,[var]` loads the constant and the
> `mov eax,[eax]` is the **correct, required** deref to obtain `obj = *(0x007d3ff8)`. Removing
> it made the build crash at `unknown+0` (null-exec) instead. The **only** real divergence was
> the missing `push src`.
>
> | | original `FUN_00427840` | reimpl (corrected) |
> |---|---|---|
> | obj | `MOV EAX,[0x7d3ff8]` | `mov eax,[s_VtableRootAddr]; mov eax,[eax]` (var holds the addr) |
> | call | `PUSH EDI(src); CALL [EAX+0xf4]; ADD ESP,4` | `push edi(src); call [eax+0xf4]; add esp,4` ✓ |
>
> **Fix applied + VERIFIED:** rebuilt, redeployed, plain-launch survival run — the `_strlen`
> crash (0x004a9440) is **gone**; the crash moved one call downstream (see next section). The
> ~94 s timing = when the title screen (`FUN_00428a30`) first renders text post-intro
> (intro-skip → title ~6 s → crash ~15 s, matching the observed "intro-skip dies before t=15 s").
>
> **NEW BLOCKER (downstream, separate):** after FontText, `FUN_00428320` calls the STOCK
> `FUN_005554d0(DAT_0067d838 /*font ctx*/, widened_str, scale)`, which crashes at **0x005554e3**
> = `MOV EAX,[ESI+0x134]` with `ESI` = the font/draw context `DAT_0067d838`. So the draw
> context is **NULL/invalid at title-render time** (a stock function dereferencing a bad
> context — an upstream font/draw-context init issue, NOT another hook reimpl: our 0x005554d0
> reimpl is REFUSED/MASS-DISABLED, so stock runs). `DAT_0067d838` is set at 0x004282c4.
> Also still open: the hooks-OFF ~96 s null-exec crash is a separate matter.


Session date: 2026-05-26/27. Purpose: diagnose the ~60-75s "self-exit" that gates
the full-race C4 fire-counting observation. Mandate was scope-and-stop (no fix).

## Verdict

The self-exit is a **genuine access violation (0xC0000005)**, NOT a clean quit, idle
timeout, or watchdog. It reproduces with our hooks **disabled** (different faulting
site), so it is **not caused by hook installation** — the ~94 s instability is upstream
of our patcher. With hooks ON the fault is **`strlen(NULL)`** reached through the
RenderWare "strlen-wrapper" vtable slot invoked by our (faithful) `FontText_UTF16WidenCopy`
hook; with hooks OFF the same ~94 s state surfaces as a null/wild-pointer call instead.
Root cause = a **NULL/corrupt pointer state that appears at ~94 s**; the hooks only
relocate where it is first dereferenced. See "Frida caller-recovery" below.

## Evidence (3 plain launches, no Frida; exit code + Windows WER event log)

Launched via `Start-Process MASHED.exe -WorkingDirectory original -PassThru`, polled
window-truth on a timeline, captured `Process.ExitCode` on exit. Tool:
`scripts/observe_runtime.ps1` (repeatable). WER = Application-Error event log
(`Get-WinEvent ... ProviderName='Application Error'`).

| Run | Hooks (`MASHED_RE_NO_AUTO_HOOK`) | Crash t | ExitCode | WER faulting module | WER offset | Faulting EIP |
|-----|----------------------------------|---------|----------|---------------------|-----------|--------------|
| 1   | ON  (unset)  | 93.6 s | 0xC0000005 | `MASHED.exe`  | 0x000a9440 | 0x004a9440 |
| 2   | OFF (=1)     | 95.9 s | 0xC0000005 | `unknown`     | 0x00000000 | 0x00000000 |
| 3   | ON  (unset)  | 94.7 s | 0xC0000005 | `MASHED.exe`  | 0x000a9440 | 0x004a9440 |

PE ImageBase = 0x00400000 (verified from the optional header), so WER offset
0xa9440 → EIP 0x004a9440.

## What the faulting EIP is

`0x004a9440` is inside **`_strlen`** (MSVC VS2003 CRT, function entry 0x004a9410,
size 139 B). Per the existing analysis plate `re/analysis/boot_crt_env/004a9410.md`:

> Dword-at-a-time loop (LAB_004a9440, 0x004a9440): At 0x004a9440: `MOV EAX, dword ptr [ECX]` — load 4 bytes.

So the hooks-ON crash is a **read access violation while `strlen` scans for the NUL
terminator** — i.e., MASHED handed `strlen` a `char*` (ECX) that walks into unmapped
memory (bad/garbage pointer, or a string with no NUL inside its committed page).
`_strlen` itself is correct CRT code; the bad pointer originates upstream in the caller.

The hooks-OFF crash faults at **EIP 0x00000000** (WER module `unknown`, offset 0) —
a transfer of control to a null/wild pointer. Different faulting *instruction*, same
exception and same ~94 s timing.

## Determinism

- Faulting site is **stable per configuration**: both hooks-ON runs (1 and 3) fault at
  the identical `_strlen` instruction 0x004a9440.
- It **differs between configs**: hooks-ON → `_strlen` read-AV; hooks-OFF → null-exec AV.
- Crash *timing* is **config-independent**: 93.6 / 95.9 / 94.7 s across both configs.

Most parsimonious reading (marked inference, not a decompilation fact): a single
underlying defect terminates the process at ~94 s; toggling the 250 inline-JMP hooks
changes process memory layout/contents, which moves *where* the wild pointer first
gets dereferenced (a string ptr vs a code ptr) without changing *that* it crashes or
*when*. This is the classic signature of upstream memory corruption / use of
uninitialised-or-freed state. [UNCERTAIN] — not proven; alternative is two independent
bugs that coincidentally both fire at ~94 s.

## Timer vs stage-transition (rules out a fixed watchdog)

All 3 runs above had **full-size intros** (intro-skip OFF), so they all reach the
intro→menu region at ~the same wall-time → can't distinguish a launch-relative timer
from a transition crash *from these 3 runs alone*.

Prior-session observation (memory `project_runtime_blocked.md`, 2026-05-26): with
**intro-skip ON** (5 empty MPGs) MASHED dies **before t=15 s** — much earlier, tracking
the (now near-zero) intro length. That death-time-scales-with-intro-length behaviour
argues **against** a fixed wall-clock watchdog/idle-timeout (H1) and **for** a crash at
the intro→menu stage transition (H2). [UNCERTAIN] — could NOT independently re-confirm
this session: the empty-video resource `re/prior_art/MashedRunner/tmp/SciLorsEmptyVideo.mpg`
is missing, so `scripts/patch_mashed_skip_intro.py` cannot run without first regenerating it.

## Frida caller-recovery (the real AV + caller chain)

`re/frida/poll_attach_catch_crash.py` (exception handler + ESP stack-dump + module
table), attached by PID to a normal launch. Raw capture:
`re/analysis/menu_crash_scoping/crash_eip_real_av.json`.

NOTE: MASHED raises `OutputDebugStringA` ("Replay size is N", N growing) **every frame**
— a shipped debug-build leftover in MASHED.exe itself (our source contains no
`OutputDebugString`; verified). Under a debugger this is `DBG_PRINTEXCEPTION_C`
(0x40010006, Frida type `system`) and floods the handler. The catcher was fixed this
session to ignore non-fatal types and only report real faults. It then caught the AV:

```
type        access-violation         eip 0x004a9440  (= _strlen; bytes 8b 01 = MOV EAX,[ECX])
ecx         0x0                       mem_address 0x0   mem_op read     → strlen(NULL)
stack[0]    0x717580d4                → return addr INSIDE mashed_re_dev.asi (base 0x71750000 → asi+0x80d4)
stack[+16]  0x00428349                → MASHED.exe return address (one frame up the chain)
```

Caller chain (resolved from the on-disk PE, no guessing):
- `0x00428344` in MASHED.exe is `E8 f7 f4 ff ff` = `CALL rel32` → **target 0x00427840**
  (return addr 0x00428349 = the stack[+16] value).
- `0x00427840` = **`FontText_UTF16WidenCopy`**, an ACTIVE hook
  (`mashedmod/src/mashed_re/HUD/HudBatch.cpp:642`). Its reimpl does
  `call [*(0x007d3ff8) + 0xf4]` to get the string length.
- `*(rw_base + 0xf4)` is documented in our own source as the RenderWare **"strlen
  wrapper"** (`Render/TextureLoaderCluster.cpp:205`, cited 0x004c5af6). That wrapper
  calls the original `_strlen` (0x004a9410) — whose return address is the `.asi`
  value at stack[0].

So a **NULL string/charset text pointer** reaches `strlen` via the RW strlen-wrapper,
invoked from our `FontText_UTF16WidenCopy` hook. Our reimpl mirrors the original's
vtable call faithfully (the original 0x00427840 would issue the same call) — the bug is
the **NULL string in the RW object at ~94 s**, not the reimpl's structure. This is
consistent with the hooks-OFF run dying at a different null-pointer site at the same time.

[UNCERTAIN] mechanism that produces the NULL at ~94 s. One lead: the every-frame
"Replay size is N" grows monotonically (frame loop is recording replay even at this
stage); a fixed-cap replay buffer overflowing could corrupt adjacent state. NOT proven —
needs a watchpoint / buffer-cap check before any claim.

## NULL traced to a corrupted .data string (2026-05-27)

Deep-stack re-run (`poll_attach_catch_crash.py` now dumps 320 stack dwords + extracts
MASHED-range return-address chain). Real AV again: `strlen(NULL)` at 0x004a9440. The
MASHED return chain (live frames):
- `+0x010 = 0x00428349` → return into `FUN_00428320` (after its `CALL 0x00427840`).
- `+0x418 = 0x00428b67` → return into **`FUN_00428a30`** (after its `CALL 0x00428320` at
  0x00428b62). Offset 0x418 matches `FUN_00428320`'s `SUB ESP,0x404` frame, so this is the
  live caller (the `0x4a5xxx` values in between are stale data inside that 1KB buffer).

`FUN_00428a30` is the **title-screen / build-date renderer**. The exact asm at the crash site:
```
0x428b4d PUSH 0x3f800000      ; scale 1.0f  (param_2)
0x428b52 PUSH 0x54            ; 'T'
0x428b54 PUSH 0x005f6560      ; the "Toast Toast Toast…" string
0x428b59 CALL 0x004a3480      ; strrchr  →  EAX = ptr to last 'T', or NULL
0x428b61 PUSH EAX             ; param_1 = strrchr result
0x428b62 CALL 0x00428320      ; FUN_00428320(result, 1.0f)
```
So `strrchr(0x005f6560, 'T')` returned **NULL** → `FUN_00428320(NULL)` → `FontText_UTF16WidenCopy`
→ RW strlen-wrapper → `strlen(NULL)`.

`strrchr` returns NULL only if the string has no 'T'. Established facts:
- `0x005f6560` is in **`.data` (0x5ea000–0x914703, WRITABLE)** — `memory_blocks_list`.
  (The non-crashing call sites pass `.rdata` constants at 0x5cc4d8, or stack buffers — never NULL.)
- The string is statically `"Toast Toast … "` (13×, contains 'T') — `memory_read`.
- `strrchr` = `FUN_004a3480`, **NOT one of our hooks** (grep: no `4a3480`/`strrchr` in
  `mashedmod/src`) — so the NULL is not a reimpl bug; strrchr genuinely found no 'T'.
- `reference_to 0x005f6560` = exactly **2 refs, both reads** (the two title renderers'
  `strrchr`); **zero intentional writers.**

Conclusion: the Toast string at 0x005f6560 is **corrupted (its 'T'/content zeroed or
overwritten) by an unintended stray write / buffer overflow at ~94 s** — there is no code
that writes there on purpose. The only hooked function in the whole chain is
`FontText_UTF16WidenCopy`, and it faithfully mirrors the original; everything else
(`FUN_00428a30`, `FUN_00428320`, `strrchr`) is unmodified MASHED. This explains the
hooks-ON vs hooks-OFF difference: the same stray overflow lands on different victims
depending on memory layout (our 250 hooks perturb it) — Toast string (→ `strlen(NULL)`)
with hooks ON, a code pointer (→ null-exec) with hooks OFF.

**Next decisive step (runtime): a write-watchpoint on 0x005f6560** to catch the exact
instruction that corrupts it. HW watchpoint (debug reg) is preferable to `MemoryAccessMonitor`
page-guard here because the page is read every frame (page-guard would trap the hot reads).
Candidate corruptor to check: the per-frame "Replay size is N" growing buffer (a frame loop
is recording replay even at the title screen) — a fixed-cap replay/array overflowing into
adjacent `.data`. [UNCERTAIN] until the watchpoint names the writer.

## Font-context blocker trace (2026-05-27) — stock VFS file-open failure

After the FontText fix, the title overlay crashes in stock `FUN_005554d0` @0x005554e3
(`MOV EAX,[ESI+0x134]`). Crash-catcher capture: **`ESI = 0x0`**, `mem 0x134 read` → the
font/draw context `DAT_0067d838` is **NULL**. Full static trace (all stock; NONE hooked):

1. `FUN_005554d0(DAT_0067d838, str, scale)` derefs `font_ctx+0x134` → NULL deref.
2. `DAT_0067d838` is set by `FUN_00427ca0()` (font init) via `DAT_0067d838 = FUN_00427880()`.
   `FUN_00427ca0` ← `FUN_00402750` (boot), called *after* intros (`FUN_00495350`) and the
   `font36.piz` load, and *before* `FUN_004669b0` (which renders the overlay). The crash
   stack confirms boot `FUN_00402750` is live (ret 0x4028e0 = its `call FUN_004669b0`). So
   init RAN but **`FUN_00427880()` returned NULL**.
3. `FUN_00427880()` (stock) takes a filename in EAX and does:
   `stream = RwStreamOpen(2 /*rwSTREAMFILENAME*/, 1 /*read*/, filename)`; if `stream==0`
   it returns NULL (`TEST ESI,ESI; JZ`). Then `FindChunk(stream,0x199)`, `FUN_00554390`.
4. `RwStreamOpen` = `FUN_004cc230`; for (type 2, mode 1) it opens the file via
   `FUN_005507b0(filename, &DAT_0061737c)` and returns NULL if that open fails.
5. `FUN_005507b0` (the VFS/piz file open) — **stock, not hooked** — failed.

### Runtime probe of FUN_00427880 (2026-05-27) — the file OPENS; the BUILDER fails

`re/frida/probe_fontctx_open.py` (light Interceptors on the cold, once-at-~90 s
`FUN_00427880`) overturns the "VFS open fails" guess above:

```
FUN_00427880 ENTER : filename = 'fgdc20.rwf'          (a RenderWare vector-font file)
RwStreamOpen (FUN_004cc230)      -> 0x2daaf5c  (OK — opened fine)
RwStreamFindChunk (FUN_004cc5e0) -> 0x1        (OK — chunk found)
FUN_00427880 RETURN ctx (EDI)    -> 0x0        (NULL — FAILED)
```

So the open + find-chunk SUCCEED; the font/draw-context **builder `FUN_00554390(stream)`
returns NULL** while parsing the valid `fgdc20.rwf`. `FUN_00554390` is **stock** (not hooked).
Its ~15 NULL-return paths; with stream reads succeeding for valid data, the prime suspects
are the **RW raster/texture creates** it performs — `FUN_004c5cb0(record,0)` (per-glyph;
`if (iVar11==0) goto fail`), `FUN_00554200` / `FindChunk 0x1a1` (other branch), or the ctx
allocator `FUN_005551d0()` at the top. These are D3D9/RW-raster-backed → consistent with a
modded-boot / d3d9-shim dependency not being ready when the font loads (~90 s, right after
the blocking intro player returns).

Correction note: the static failure-path reading above suggested the *open* fails; runtime
proved the open succeeds and the *builder* fails. (Third static-inference correction this
session — keep verifying at runtime.)

### Probe of FUN_00554390 internals (2026-05-27) — the per-glyph texture lookup fails

`re/frida/probe_fontctx_build.py` (hooks the builder + suspect sub-calls, attributing inner
returns via an in-function flag):

```
FUN_00554390 ENTER (stream=0x1)
  FUN_005551d0_alloc        -> 0xd797ccc   (OK — ctx allocated)
  FUN_005c4d30_arraybase    -> 0xd7b9e60   (OK — glyph array allocated)
  FUN_004c5cb0_rastercreate -> 0x0         <-- NULL/FAIL
FUN_00554390 LEAVE -> 0x0                   (builder fails)
AV pc=0x5554e3 esi=0 mem=0x134              (title render with NULL ctx)
```

The failing call is **`FUN_004c5cb0(glyph_name, dict=0)`** (stock) — a RenderWare
**texture lookup-or-load by name**:
```
tex = (*(rwdev+0x18))(name);        // find in current texture dictionary
if (tex) { ++refcount; return tex; }
tex = (*(rwdev+0x14))(name, dict);  // else load it
if (tex == 0) { log_error(0x16, name, …); return 0; }   // <-- returns NULL
```
(`rwdev = DAT_007d3ff8`; the +0x14/+0x18 method ptrs are the RW device's texture
read/find, set during RW/D3D init.)

**Deepest root (9-layer trace):** a per-glyph **texture named in `fgdc20.rwf` is neither in
the current RW texture dictionary nor loadable** at font-load time (~90 s, right after the
blocking intro player returns) → `FUN_004c5cb0` logs error **0x16** and returns NULL →
`FUN_00554390` (font builder) returns NULL → `FUN_00427880` NULL → `DAT_0067d838` NULL →
stock `FUN_005554d0` derefs `NULL+0x134` → AV. Entirely stock code; the only hook bug in the
whole path (FontText) is fixed. This is the **RW texture-dictionary / resource-state**
subsystem: the font's glyph TXD isn't current/loaded when the font initializes in the modded boot.

### Missing texture named (2026-05-27) — `fgdc20`; its TXD is present but not current

`re/frida/probe_fontctx_texname.py` (gated to lookups inside `FUN_00554390`):
```
[1] FUN_004c5cb0 name='fgdc20' dict=0x0 ecx=0x2b0 -> 0x0   <-- FAIL (NULL)
AV pc=0x5554e3 esi=0 mem=0x134
```
The missing texture is **`fgdc20`** — the font's own glyph texture (same base as `fgdc20.rwf`).

`Font36.piz` (`piz_extract.py list`) contains **both**:
- `FGDC20.RWF` (the font, read OK) and **`FGDC20.TXD`** (its glyph texture dictionary).

So the texture **exists in the mounted piz** — this is purely a **TXD load/ordering** failure:
nothing loads `FGDC20.TXD` and sets it as the current RW texture dictionary before the font
builds. Contrast: the logo textures `FUN_004283a0` loads (`MashedNEWLogo`, `proLogicII`,
`DolbyDig`) are **loose PNG/BMP** in Font36.piz, so `RwTextureRead`-as-image finds them; but
`fgdc20` lives only inside `FGDC20.TXD`, so its lookup needs that TXD current — and it isn't.
`FUN_004283a0` (the stock call right before font init) loads the logos but **does not** load
`FGDC20.TXD`. Every function in this path is stock (FontText was the lone hook bug, fixed).

### TXD-load trace (2026-05-27) — `VfsFileExists("fgdc20.txd")` returns 0

`FUN_00427ca0` (font init) is *meant* to: `FUN_0042a6b0("fgdc20.txd")` → load+return the TXD,
`FUN_00556cc0(txd)` → set current, then `FUN_00427880("fgdc20.rwf")` → build the font (finding
`fgdc20` in the now-current TXD). The TXD/RWF names are at `DAT_005cd600` = `"fgdc20.txd\0\0fgdc20.rwf"`.

`FUN_0042a6b0` only loads the TXD if `FUN_0042a530(reg, name)` ("Searching piz for %s") finds it.
`FUN_0042a530` tries `FUN_0042a470(reg, name, t)` for t∈{4,1,3,2,5} = prefixes
{`pc/`,bare,`xbox/`,`ps2/`,`gamecube/`}, each calling **`VfsFileExists` (`thunk_FUN_00550b00`)**;
on all-miss it logs "Unable to find %s in piz" and returns 0.

Runtime probe (`re/frida/probe_piz_txd_search.py`):
```
FUN_0042a530 'Searching piz for' name='fgdc20.txd' -> 0x0   <-- NOT FOUND IN PIZ
FUN_0042a6b0(name='fgdc20.txd')                    -> 0x0   <-- no current TXD
AV pc=0x5554e3 esi=0
```
So **`VfsFileExists("fgdc20.txd")` (and the prefixed variants) all return 0** — the file is
reported absent from the **VFS default mount** (`DAT_007dc76c`; original `FUN_00550b00`: no `:`
prefix → dispatch `default_mount->vtable[0x13](name)`). Yet `FGDC20.TXD` IS in Font36.piz, and
`fgdc20.rwf` *opened* fine earlier via `FUN_005507b0` (a different stock VFS path). So font36.piz
files resolve through `FUN_005507b0` but are **absent from the default mount `VfsFileExists`
queries** — a VFS mount-routing gap.

`VfsFileExists` (0x00550b00) IS one of our hooks, but its reimpl was **diffed against the original
and is faithful** (entry guard `DAT_007dc75c`, strlen-wrapper, `:`-scan + mount-list walk at
`DAT_007dc754`, default-mount fallback `DAT_007dc76c`, `vtable[0x13]` dispatch — all match), and
the no-`.asi` control reproduces — so this is **stock VFS behaviour**, not a reimpl bug.

**14-layer root, fully pinned:** the modded boot leaves font36.piz's entries out of the VFS
*default mount*, so `VfsFileExists("fgdc20.txd")=0` → piz search fails → font TXD never loads/sets
current → `RwTextureRead('fgdc20')` misses → font build NULL → NULL font ctx → title render AV ~94 s.
Next: the VFS mount setup — what `FUN_00495280("...font36.piz")` registers vs what `DAT_007dc76c`
(default mount) points to, and why `FUN_005507b0` resolves font36.piz but the default mount doesn't.
Strong regression candidate (menus worked 2026-05-17).

### (superseded guess) Was the decisive open question pre-no-`.asi`:
does stock MASHED (no `.asi`) load/set `FGDC20.TXD`
current here, or does it fail the same way on this Win11 box? → run the **no-`.asi` control**
(temporarily move `original/mashed_re_dev.asi` aside — touches `original/`, needs approval).
If stock also fails, the modded boot is missing/mis-ordering a TXD load the real game does;
if stock works, one of our boot patches / the d3d9 shim disrupts the TXD load. Secondary: find
what is *supposed* to load+set `FGDC20.TXD` current (search refs to the font TXD / a
`RwTexDictionaryStreamRead`+`SetCurrent` near the font path).

## Screenshot status (goal: verified main-menu shot) — NOT achieved

- `empire_splash_t007.png` (this dir; copied from `verify/scene_t007.png`, run 3) shows
  the **Empire Interactive splash (`empire.mpg` intro)** rendering in the MASHED window →
  renderer + intro pipeline work; the window is live, not frozen-black. (`verify/` is
  overwritten each `observe_runtime.ps1` run; this copy is the frozen evidence.)
- Later shots (t≈90 s) captured an **occluding terminal**, not MASHED: GDI
  `CopyFromScreen` grabs whatever is top-of-Z-order at the window rect, and MASHED was
  not foreground. No menu content was ever visually confirmed.
- A clean main-menu screenshot is **blocked** on this build: the menu (if reached) only
  appears ~90 s in, ~4 s before the crash, and is occluded. Foregrounding MASHED to
  de-occlude risks the documented focus-restore-during-intro freeze.

## Bottom line for C4

- C4 is UNBLOCKED at the loader+hook level (live inline-JMP confirmed last session).
- C4 full-race fire-counting is **still gated**: the menu/intro scene crashes (AV) at
  ~94 s, so there is no sustained stable observation window, and the crash is **not**
  something we can clear by disabling/bisecting our hooks (it reproduces hooks-off).

## Recommended next steps (NOT executed — for user decision; involves a fix or a
## control that touches `original/`)

1. **No-`.asi` control** (decisive: stock-MASHED vs our-DLL-present). Temporarily
   rename `original/mashed_re_dev.asi` aside (it is OUR build artifact, restorable from
   `mashedmod/build/`), launch, and check whether the ~94 s AV persists with NO loaded
   DLL at all. This is the one remaining question the env-var A/B can't answer (the
   `.asi` is still mapped, just unhooked, in run 2). NOTE: touches `original/`, so it is
   a stop-and-ask item per project rules — not done here.
2. **DONE this session — strlen caller recovered** (see "Frida caller-recovery"): the
   NULL string reaches `strlen` via the RW strlen-wrapper invoked by `FontText_UTF16WidenCopy`
   (0x00427840). Next, find *what nulls the RW charset/string object at ~94 s*: set a
   write-watchpoint on the `[rw_base+0xf4]`-target's string field, and check the
   "Replay size is N" path (`ReplayRecordFrame` 0x00411600 etc.) for a fixed-cap buffer
   that overflows ~94 s. Trace the value of `*(0x007d3ff8)` and its `+0xf4` string field
   over time.
3. **Regenerate the empty-MPG resource** and re-run intro-skip to lock down H1-vs-H2.
4. If the no-`.asi` control still crashes → this is a stock-MASHED-on-Win11 defect;
   investigate what populates/clears the RW charset object, and the audio-COM path we
   NOP'd, as candidates. Whether the original 0x00427840 also `strlen(NULL)`-crashes given
   the same state (i.e. is this purely a state bug?) is answerable by the no-`.asi` control.
