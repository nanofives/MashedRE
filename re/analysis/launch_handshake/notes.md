# Session: launch_handshake-20260506

## Anchor outcome

**FOUND** — all five strategies ran; CLI and disc-check anchors converged.

Protection is **absent** inside MASHED.exe. The copy-protection lives entirely in
`launch.exe` (VB6 wrapper), confirmed by two routes:

1. MashedRunner's NoCD patches target `launch.exe`, not MASHED.exe.
2. Inside MASHED.exe, `GetLogicalDrives` + `GetDriveTypeA` are used only for
   **RenderWare file-system setup** (`FUN_004955d0` = `HardwareInstallFileSystem`);
   the return value of that call is **discarded** by its caller `FUN_00493710`.

The protection anchor is therefore confirmed absent inside MASHED.exe — the
success path is "no protection check needed in mashed_re.exe."

---

## HANDSHAKE_FN identification

| RVA | Ghidra name | Role | hooks.csv status |
|-----|-------------|------|-----------------|
| 0x00093900 | sub_00493900 | **CLI parser** — tokenizes lpCmdLine on space; sets 5 globals via `__stricmp` | C1 mapped (boot_app_init bucket) |
| 0x00099ba0 | sub_00499ba0 | **Window init** — copies lpCmdLine → DAT_00773818; CoInitialize; RegisterClassA("MASHED"); CreateWindowExA | C1 mapped (boot_app_init bucket) |
| 0x000955d0 | FUN_004955d0 | **HardwareInstallFileSystem** — RW FS setup; NOT protection (new this session) | → scribe via launch_handshake bucket |
| 0x00095780 | FUN_00495780 | Bool wrapper for FUN_004955d0; resolves U-0070 (new this session) | → scribe via launch_handshake bucket |

---

## CLI parameter strings (verbatim from decompiler — 0x00493900)

All comparisons are case-insensitive via `__stricmp`. Tokenization on ASCII 0x20 (space).

| Token (verbatim) | Effect | Global written |
|---|---|---|
| `-vs0` | video-skip off | `DAT_006147bc = 0`, `DAT_007719e0 = 0` |
| `-vs1` | video-skip on | `DAT_006147bc = 1`, `DAT_007719e0 = 1` |
| `-cs0` | controller-setup skip off | `DAT_006147c0 = 0`, `DAT_007719e4 = 0` |
| `-cs1` | controller-setup skip on | `DAT_006147c0 = 1`, `DAT_007719e4 = 1` |
| `-l0` | language 0 (English) | `DAT_007719e8 = 0` |
| `-l1` | language 1 (French) | `DAT_007719e8 = 1` |
| `-l2` | language 2 (German) | `DAT_007719e8 = 2` |
| `-l3` | language 3 (Spanish) | `DAT_007719e8 = 3` |
| `-l4` | language 4 (Italian) | `DAT_007719e8 = 4` |
| `-l5` | language 5 (unknown) | `DAT_007719e8 = 5` |

MashedRunner only passes `-L0` through `-L4`; the game additionally handles `-l5`.
`DAT_007719e8` is consumed by the localization init (`FUN_004274d0`, already mapped)
which copies it into `DAT_007f0f60`.

---

## Registry keys read (verbatim)

No game-specific registry keys exist in MASHED.exe. The only registry reads are:

| Key (verbatim) | Value | Purpose | RVA |
|---|---|---|---|
| `HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\ProductOptions` | `ProductType` | OS-version string logging | 0x00499890 |
| `HKEY_LOCAL_MACHINE\Software\Microsoft\Direct3D` | (via param) | D3D settings query | 0x004fbd78 |

`installpath`, `installstatus`, `language` — **absent** from MASHED.exe entirely.
These are written by `launch.exe` only; MASHED.exe never reads them.

---

## Mutex / IPC observed (verbatim)

- `CreateMutexA` / `CreateMutexW` — **not imported**
- `CreateFileMappingA` is imported and used in `FUN_0051039a` (memory-mapped file
  read utility), **not** for IPC or single-instance mutex.
- No shared memory handshake from launch.exe into MASHED.exe.

---

## Copy-protection signatures (verbatim)

| String | Found | Notes |
|---|---|---|
| "StarForce" | no | — |
| "SafeDisc" | no | — |
| "SecuROM" / "securom" | no | — |
| "SECDRV" | no | — |
| "cdcheck" | no | — |
| "please insert" | no | — |
| "DVD" | no | — |

`GetDriveTypeA` + `GetLogicalDrives` are imported but used only for RW FS setup
(see `FUN_004955d0`). `GetVolumeInformationA/W` — **not imported**.

---

## Window registration (from sub_00499ba0 — 0x00099ba0)

| Item | Verbatim value | Address |
|---|---|---|
| Window class | `"MASHED"` | 0x00499c13 (lpszClassName) |
| Window title | `"MASHED [D3D9] [Release]"` | 0x005d041c |
| Window style | `0xcf0000` | 0x00499c72 |
| WndProc | `FUN_00499820` | 0x00499bdf |

---

## U-0070 resolution

U-0070 asked: what is `thunk_FUN_00495780`'s real target?

Call chain confirmed via MCP decompile:
- `thunk_FUN_00495780` (0x004951e0, 4 bytes) → JMP `FUN_00495780` (0x00495780)
- `FUN_00495780` (0x00095780, 11 bytes) → calls `FUN_004955d0`, returns `result != 0`
- `FUN_004955d0` (0x000955d0, 418 bytes) = `HardwareInstallFileSystem`:
  - `GetCurrentDirectoryA` → uppercase via `CharUpperA`
  - RW FS APIs `FUN_00551190`(0x14, currentDir, PTR_DAT_005cfee0) → find current drive FS
  - `GetLogicalDrives` → iterate all drive bits
  - `GetDriveTypeA(drive)` → skip drives with type==0 or type==1
  - RW FS `FUN_00551190` again per-drive → set up CD/DVD drives in RW FS
  - Returns 1 if at least one drive found, 0 otherwise
- Caller `FUN_00493710` logs "Calling HardwareInstallFileSystem\n" then calls
  `thunk_FUN_00495780()` and **ignores the return value**.

U-0070 → **RESOLVED**: target is `FUN_004955d0` = `HardwareInstallFileSystem`.

---

## GREENFIELD CONTRACT

What `mashed_re.exe` must implement/emulate from launch.exe's contract to run
standalone:

### MUST implement
1. **CLI arg parsing**: accept `-vs0`, `-vs1`, `-cs0`, `-cs1`, `-l0` through `-l5`
   (case-insensitive). Write the 5 globals listed above.  
   Default values when no args given: `DAT_007719e8 = 0` (English, per global init).

2. **Window class "MASHED"** with `RegisterClassA` before `CreateWindowExA`.

### MUST NOT implement (absent from MASHED.exe)
- No disc check — protection is 100% in launch.exe; MASHED.exe runs without disc.
- No registry reads for installpath, installstatus, or language.
- No launch.exe-specific mutex or shared-memory IPC.

### OPTIONAL / runtime default
- Language default (no `-l` arg): `DAT_007719e8 = 0` (English). Language is
  consumed downstream by `FUN_004274d0` before the main game loop.
- Video/controller skip defaults (no `-vs`/`-cs` arg): globals remain zero (disabled).

---

## Strategies that converged

| Strategy | Converged? | Key evidence |
|---|---|---|
| CLI / argv | YES | `GetCommandLineA` → `entry` → `FUN_00493900`; full `__stricmp` parse |
| Registry | YES (negative) | `RegOpenKeyExA`/`RegOpenKeyA` found; all reads non-game-specific |
| Copy-protection signatures | YES (negative) | No protection strings; `GetVolumeInformationA/W` absent |
| IPC / mutex | YES (negative) | `CreateMutexA/W` not imported; `CreateFileMappingA` = file reader only |
| Entry-point scan | YES | `entry` → `FUN_00492370` → `FUN_00493900` call chain confirmed |

---

## Session metadata

- Slot: Mashed_pool15
- SHA-256 verified: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
- MCP failures: 1 (memory_read TypeError on session_id vs int — non-blocking)
- cap_count: 0 / 20 (work loop short; new functions: 2)
- IDs used: none (existing IDs resolved; no new S/U/D range consumed this session)
