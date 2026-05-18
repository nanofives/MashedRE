---
rva: 0x004b6940
name: PizOpenAndParse
size_bytes: 447
confidence_target: C2 (scoping)
session: fun_004b6940_scoping-20260517
slot: Mashed_pool0
opened_in_slot: Mashed_pool0
session_date: 2026-05-17
---

# FUN_004b6940 — scoping pass

The function that gates past-menu runtime (track / vehicle / AI / render-frame canonical scenarios). Already C1-mapped via `re/analysis/piz_fsmanager_handler/REPORT.md`; this report is the **bug-class scoping pass** that proposes a path to resolution.

## Mechanical description (RVA-cited)

Function body: `0x004b6940..0x004b6aff` (447 bytes).

1. **Stack cookie setup** at 0x004b6940 — `local_4 = DAT_00616038 ^ unaff_retaddr`.
2. **Zero `&DAT_008ab7e0`** for 0x82B uints (8364 bytes) — clears the current-piz-name area.
3. **`_strncpy(&DAT_008ab7e0, param_1, 0x80)`** — copy incoming piz path (max 128 bytes).
4. **`__stricmp(&DAT_008ad8a0, param_1)`** — compare against previous piz name. If different:
   - `DAT_007d3e64 = 0`, `DAT_007d3e68 = 0` (entry counts reset)
   - **Clear 0x8000 uints (128 KB) at `&DAT_008ad9c0`** — directory blob reset.
5. Manual byte-loop `strcpy(DAT_008ad8a0, param_1)` — saves new piz name for next-call comparison.
6. **Progress callback** (if `DAT_007d3e5c != 0`): `(*DAT_007d3e5c)(&DAT_008ad8a0, &DAT_008ab6e0, DAT_007d3e60)` — **U-2908 still open** (writer unknown).
7. **`FUN_004b65e0()`** — preparation/cleanup call (not yet RE'd).
8. **`DAT_007d3e48 = 0`** — null the file handle.
9. **`iVar4 = FUN_004b6710()` (PizWin32Open)** — opens the piz on disk.
10. **If open succeeded**:
    - `DAT_007d3e4c = 1` (piz is open flag).
    - **`FUN_004b67e0(0, auStack_804, 0x800, auStack_808)`** — read first 2 KB into stack buffer.
    - Extract 4-DWORD header → `DAT_008ad9a0` / `DAT_008ad9a4` / `DAT_008ad9a8` / `DAT_008ad9ac`.
    - **Clear 0x1000 uints (16 KB) at `&DAT_0090dac0`** — directory table reset.
    - **Loop while `uVar8` (entry count = `auStack_804[2]`) != 0**:
      - `FUN_004b67e0(iVar4, auStack_804, 0x800, auStack_808)` — read 2 KB chunk.
      - `iVar4 += 0x800` — advance file offset.
      - Copy `(uVar8 < 0x10 ? uVar8 << 7 : 0x800)` bytes from `auStack_804` into `puVar7`.
      - `puVar7 += 0x200` uints (advance directory pointer by 2 KB).
      - `uVar8 -= (uVar2 >> 7)` — decrement by entries consumed (entry size = 0x80).
    - `DAT_007d3e58 = FUN_005507a0()`; `FUN_00550790(DAT_007d3e54)` — RW VFS detach.
    - return 1.
11. Else return 0.

## Bug class — diagnosis

The crash class is **`FILE_FLAG_NO_BUFFERING` incompatibility with modern 4 KB-sector storage**.

### Cited evidence

- **`FUN_004b6710` (PizWin32Open)** at `0x004b6710` calls `CreateFileA` with `dwFlagsAndAttributes = 0x60000000`:
  ```
  CreateFileA(path, 0x80000000 /*GENERIC_READ*/, 0, NULL, 3 /*OPEN_EXISTING*/,
              0x60000000, NULL)
  ```
  `0x60000000` decomposes to `FILE_FLAG_NO_BUFFERING (0x20000000) | FILE_FLAG_OVERLAPPED (0x40000000)`.

- **`FUN_004b67e0` (PizWin32Read)** at `0x004b67e0` issues reads of size **`0x800` bytes (2 KB)** into a **stack-allocated buffer** (`auStack_804` in `FUN_004b6940`).

### Why this crashes on modern hardware (per Microsoft docs)

`FILE_FLAG_NO_BUFFERING` requires:
1. **Read size** must be an integer multiple of the volume physical sector size.
2. **Buffer address** must be aligned to the volume physical sector size.
3. **File offset** must be aligned to the volume physical sector size.

On legacy 512-byte-sector drives, `0x800` (2048) and stack-aligned buffers satisfy all three constraints. On modern NVMe / Advanced Format drives with **4096-byte physical sectors**, `0x800` is *not* an integer multiple of 4096, and stack buffer addresses are not 4 KB-aligned. `ReadFile` returns `ERROR_INVALID_PARAMETER (0x57)`.

When the read fails:
- `FUN_004b67e0` does not check the return value of `ReadFile` — it goes straight into the `GetOverlappedResult` polling loop.
- `GetOverlappedResult` reports the same error.
- The polling loop terminates on `DVar4 == 0x26 (ERROR_HANDLE_EOF)` or `bVar1 (success)` — neither of which gets set on the size/alignment failure, so the loop **spins forever** OR exits with `param_4` (bytes-read out) uninitialized.
- The caller in `FUN_004b6940` proceeds with garbage in `auStack_804`, treats `auStack_804[2]` as the entry count, and the chunk-read loop may read 4 GB of garbage and overflow into adjacent memory before the process faults.

### Why the existing powerups bypass works

`scripts/patch_mashed_skip_powerups.py` NOPs the call-site that opens `powerups.piz` specifically. The first piz opened during boot (`font36.piz` or similar) apparently happens to align by accident (smaller size, lucky stack-alignment, or different file system path) — the second open from a cold-cache state hits the alignment trap. Skipping it lets boot continue, but **any later piz open (track / vehicle / AI assets) will hit the same trap**.

### Secondary issue — directory-table buffer overflow

`DAT_0090dac0` is a fixed 16 KB region. Each entry is 0x80 bytes, so this caps the table at **512 entries**. A piz file with > 512 entries would overflow the directory blob into adjacent globals. Track .piz files almost certainly exceed this. Today this is masked by the alignment crash hitting first, but a fix to the alignment issue may unmask it.

## Path to resolution

Three viable options. Recommended order:

### Option A (RECOMMENDED) — hook FUN_004b6710 + FUN_004b67e0 in PizOpenBypass-style

Hook both Win32 wrappers and reissue the I/O without `FILE_FLAG_NO_BUFFERING`. Keep `FILE_FLAG_OVERLAPPED` so the existing offset-via-OVERLAPPED semantics in `FUN_004b67e0` continue to work, or switch to synchronous `SetFilePointer` + `ReadFile`.

- **Surface**: 2 new hook functions, ~100 lines of C++ in `mashedmod/src/mashed_re/Compat/`.
- **Risk**: low — both functions are leaf-ish, well-understood, and we control the call surface entirely. Existing `PizOpenBypass.cpp` proves the hook pattern.
- **Verification**: cold boot, load a track, check `mashed.log` for piz-open lines; Frida-trace `FUN_004b6940` return values.

### Option B — binary patch the CreateFileA flags

NOP-patch the `0x60000000` immediate at the CreateFileA call site to `0x40000000` (drop `FILE_FLAG_NO_BUFFERING`, keep `FILE_FLAG_OVERLAPPED`).

- **Surface**: one 4-byte edit, idempotent script in `scripts/`.
- **Risk**: depends on whether the OS will return `ERROR_INVALID_PARAMETER` for offsets that aren't sector-aligned even without `NO_BUFFERING`. Per docs, no — without `NO_BUFFERING`, offsets are unrestricted. So this *should* be a working minimal fix.
- **Verification**: same as Option A. But this is a global change to the original binary; if anything else relies on `NO_BUFFERING` semantics (e.g., a cache-busting assumption), regressions are silent.

### Option C — force the "alternate path" (DAT_007d3e50 & 1)

The decomp shows a second branch in both `FUN_004b6710` and `FUN_004b67e0` that uses `_fseek` / `_fread` instead of `CreateFileA` / `ReadFile` — a CRT-stdio fallback path. Setting `DAT_007d3e50 |= 1` at runtime would route through this path.

- **Surface**: one byte to flip via Frida or one global write at boot.
- **Blocker**: `FUN_004a4541` (the alt-path opener) is **U-2907**, not yet reversed. Need to RE that function first to verify it actually opens piz files and doesn't introduce a different bug.
- **Risk**: medium — the alt-path may have been deprecated by Supersonic during development; could be buggy or incomplete.

## Recommended next session

**Pick Option A**. Author `mashedmod/src/mashed_re/Compat/PizWin32Bypass.cpp` with two hook functions (`PizWin32Open_Compat`, `PizWin32Read_Compat`), register via `RH_ScopedInstall(_, 0x004b6710)` and `RH_ScopedInstall(_, 0x004b67e0)`. Use synchronous `SetFilePointer` + `ReadFile` without flags. Wire into `mashed_re_dev.asi`, cold boot, attempt a track load.

Pre-emptively also bump `DAT_0090dac0` table size or relocate to a heap allocation **before** the first track load — the 512-entry cap will hit on track .piz otherwise. Defer the heap relocation behind the first-boot success of Option A.

## Open uncertainties (existing, unchanged)

| U-ID | What |
|---|---|
| U-2907 | `FUN_004a4541` — the alt-path opener; gates Option C |
| U-2908 | `DAT_007d3e5c` — progress callback writer (informational only; not on the crash path) |

## Estimated effort to unblock past-menu C4

- Option A authoring + diff verification: **1 hook-author session + 1 frida-diff session** (~3 hours human-attended).
- If directory-table overflow hits next: **+1 session** to allocate the directory table on the heap and adjust the loop's bound.
- Net: **2-3 sessions** to reach a state where track-load Frida tracing works, unblocking C4 promotion for vehicle/AI/track/audio/render-frame subsystems.

## Cross-references

- `re/analysis/piz_fsmanager_handler/REPORT.md` — structural C1 mapping (this report builds on it)
- `mashedmod/src/mashed_re/Compat/PizOpenBypass.cpp` — existing selective bypass at the outer `FUN_00495280` wrapper; pattern to mirror
- `memory/project_runtime_blocked.md` — prior project memory describing this as the gating function
- `scripts/patch_mashed_skip_powerups.py` — the boot-time NOP that masks the first occurrence

---

## Results (2026-05-17)

Option A implemented as `mashedmod/src/mashed_re/Compat/PizWin32Bypass.cpp`
on branch `feature/pizwin32-bypass`. Both hooks compile clean (no new
warnings) and export from `mashed_re_dev.asi`. Inline-JMP install succeeds
and the boot proceeds without crash through the first piz cycle.

### Files produced

- `mashedmod/src/mashed_re/Compat/PizWin32Bypass.cpp` — 295 lines, two hooks:
  - `PizWin32Open_Compat` — replaces `FUN_004b6710` (0x004b6710). Naked
    entry captures the path from EAX (register-passing convention used by
    the caller `FUN_004b6940` at 0x004b69ee `MOV EAX, ESI`) and forwards to
    a `__cdecl` impl. The impl drops `FILE_FLAG_NO_BUFFERING` from the
    `CreateFileA` flags (now `0x40000000` = `FILE_FLAG_OVERLAPPED` only).
    The alt-path branch (`DAT_007d3e50 & 1` → `FUN_004a4541(arg, 0x5cf010)`)
    is preserved verbatim. Mirrors the original's `GetLastError == 0 &&
    handle != INVALID_HANDLE_VALUE → return 1` contract.
  - `PizWin32Read_Compat` — replaces `FUN_004b67e0` (0x004b67e0). Plain
    `__cdecl(DWORD offset, void* buf, size_t size, LPDWORD outBytes)`.
    Mirrors the original 1:1: alt-path `_fseek` + `_fread` via cited
    trampoline RVAs 0x005c1d63 / 0x004a49cf, `OVERLAPPED` setup with
    `Offset = param_1`, `ReadFile` followed by `GetOverlappedResult` poll
    loop with progress callback dispatch. Exit conditions match:
    `ERROR_HANDLE_EOF (0x26)` or `BVar3 == TRUE`. Re-enter loop on
    `ERROR_IO_INCOMPLETE (0x3e4)` only.
- `mashedmod/build.bat` — appended `Compat\PizWin32Bypass.cpp` to the ASI
  link list.
- `re/frida/hooks_registry.py` — two `harness_limited` entries for the
  new hooks (`piz_win32_open_compat`, `piz_win32_read_compat`). Marked
  harness-limited because:
    1. `FUN_004b6710` uses register-passing (EAX), incompatible with the
       generic Frida `Interceptor` A/B harness's stack-arg call shape.
    2. Both hooks have global side effects (file handle and OVERLAPPED
       state in `DAT_007d3e48`) — running the synthetic harness against
       them would corrupt the live piz reader state.
  Standard C3 evidence is decompiler-cited 1:1 port + runtime-canonical
  smoke test (see "Runtime verification" below).
- `re/frida/test_pizwin32_install.py` — focused runtime test that loads
  the ASI with auto-hook OFF then manually patches in only the PizWin32
  hooks via Frida `Memory.protect` + 5-byte `E9 rel32`. Used to isolate
  these hooks from a pre-existing crash in the unrelated auto-hook
  bulk-install path (the same crash reproduces with the prior committed
  `original/mashed_re_dev.asi` build, so it is not caused by this change).

### Build

`cmd /c mashedmod\build.bat > log/build_pizwin32.txt 2>&1` → exit 0.
Both targets emitted (`mashed_re.exe`, `mashed_re_dev.asi`). Zero new
warnings; the 13 existing warnings (CrtCompilerSupport FS:0, CrtStartup
GetVersionExA, exe_main fopen) are pre-existing.

### Path-1 Frida diff: harness-limited (expected)

Both hooks are registered with `arg_type='harness_limited'` per the project
convention used by `crt_fast_error_exit`, `crt_stack_probe`,
`crt_seh_prolog`, and `crt_seh_epilog`. Bit-identical synthetic A/B diffing
is not possible — `FUN_004b6710` does not have a stack-args ABI and both
hooks mutate process-global file-handle state.

### Runtime verification — boot scenario

`py -3.12 re/frida/test_pizwin32_install.py` — log captured at
`log/runtime_test_pizwin32_isolated.txt`. Sequence:

1. Spawn `MASHED.exe` with `MASHED_RE_NO_AUTO_HOOK=1` (so the .asi's auto
   `InjectHooks` is disabled — avoids a pre-existing unrelated boot crash
   in the bulk-install path that also reproduces with the prior `.asi`
   build).
2. Attach Frida; arm exception handler.
3. Poll for LUT readiness (~1.4 s), then `Module.load` the .asi.
4. Manually patch `0x004b6710` and `0x004b67e0` with `E9` to our exports.
5. Watch for 45 s, ignoring `type === 'system'` (benign Windows-internal
   `RaiseException` from `KernelBase` for things like FindFirstFile-EOF).

Result: **survived 45 s, no access-violation**. `mashed.log` reaches:

    Setting up PIZ file system
        OK
    Opening piz file TOASTART\COMMON\FONT36.PIZ
        OK                                       ← OUR HOOK FIRED
    Reading texture MashedNEWLogo.png            ← FUN_004b67e0 firing per texture
    Reading texture bigE.bmp
    Reading texture proLogicII.bmp
    Reading texture DolbyDig.bmp
    Searching piz for fgdc20.txd
    Closing piz file TOASTART\COMMON\FONT36.PIZ  ← FUN_004b6940 returned 1
    SOUND CARD - MAX voices 32

The `Closing piz file` line is emitted by `FUN_004952f0` only when
`DAT_007d3e4c == 1`, which `FUN_004b6940` sets only on the success-return
path (the store at 0x004b6a14). So the downstream `FUN_004b6940(font36.piz)`
returned 1 with our hooks live — i.e. CreateFileA + 16 directory-blob reads
+ texture reads all succeeded against the dropped-NO_BUFFERING flags.

### Runtime verification — track scenario

Not executed in this session. Track load requires interactive menu
navigation (driver, vehicle select, track select, Start), which is not
automatable from the Frida-only harness. The boot scenario is the
strongest fully-automatable evidence currently available; track load
remains a follow-up canonical-scenario test under a human-driven run.

### Directory-table overflow check

Not triggered during the boot test — `FONT36.PIZ` is small enough that the
directory-blob loop in `FUN_004b6940` terminates well before approaching
the 512-entry / 16 KB cap at `DAT_0090dac0`. The hypothesised overflow on
track .piz files (the "Secondary issue" section above) remains open until
a track .piz can be opened in a live run with diagnostics.

### Open follow-ups

| ID | What | Path to close |
|---|---|---|
| FU-1 | Track-load canonical scenario | Human-driven run with hooks live, navigate to a track, capture screenshot to `verify/pizwin32_track_load.png` |
| FU-2 | Pre-existing auto-hook bulk-install crash | Separate triage session — crash reproduces with the prior committed `.asi` so unrelated to this change. EIP=0x001aeb49 (stack-borne); occurs during HardwareSetVideoMode mode enumeration. Not on the piz path. |
| FU-3 | Directory-table 512-entry cap on track .piz | Trace `FUN_004b6940`'s `uVar8` entry count on the first track load; if >512, relocate `DAT_0090dac0` to a heap allocation |
| U-2907 | `FUN_004a4541` alt-path opener | Still U-2907; not on the bypass critical path (we only enter the alt branch if `DAT_007d3e50 & 1`, which is never set in observed boots) |
