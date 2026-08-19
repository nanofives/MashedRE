# CLAUDE.md claim audit against the binary — 2026-08-18

Every factual claim in `CLAUDE.md`'s engine/anchor block, checked against
`original/MASHED.exe.unpatched` and `original/launch.exe` rather than against other
documents. Method: PE import table, embedded GUID bytes, string table, byte signatures.

**Three claims were wrong.** All three had been carried forward as received wisdom; none
had ever been checked against the artifact.

| Claim | Verdict | Evidence |
|---|---|---|
| `MASHED.exe` size 2,846,720 | **CONFIRMED** | exact |
| `MASHED.exe` SHA-256 `BDCAE093…` | **CONFIRMED** | exact |
| `launch.exe` size 978,944 | **CONFIRMED** | exact |
| `launch.exe` SHA-256 `694AA949…` | **WRONG** | actual `01506209E42C79A4E5BEDB43DCE9FB953F0CA628B26AF9FCD49EE2522DF78DA2` |
| "MCI for video" | **WRONG** | no `mciSendString`/`mciExecute` anywhere; it is DirectShow |
| "Lua 5.x" | **WRONG** (fixed earlier today, U-9042) | `$Lua: Lua 4.0 …` at `0x005d8790` |
| "MSVBVM60 for `launch.exe`" | **CONFIRMED** | `launch.exe` imports *only* `MSVBVM60.DLL`, linker 6.0 |
| "native PE32 i386 (MSVC-era)" | **CONFIRMED**, now precise | machine `0x014c`, linker **7.0** = MSVC .NET 2002 |
| "DirectX 9.0c" | **PARTIAL** | `d3d9.dll` + `Direct3DCreate9` confirm D3D9. The *`0c` runtime revision* is not determinable from the image — [UNCERTAIN] |
| "RenderWare 3.x" | **SUPPORTED** | `Core built at Mar 17 2004 07:14:02`, `rwFILTER*`/`rpMATFXEFFECT*` enum strings |

## The video stack is DirectShow, not MCI

`CLAUDE.md` said MCI. The image contains **no** `mciSendString` or `mciExecute`, and
`WINMM.dll`'s import is used for the `time*` timer family (`timeSetEvent`,
`timeBeginPeriod`, `timeGetTime`, `timeKillEvent`), not media control.

DirectShow is used through COM, which is why there is no `quartz.dll` import to notice —
the classes are created via `CoCreateInstance`, and `ole32.dll` **is** imported. The GUID
bytes are present verbatim in the image:

| GUID | present |
|---|---|
| `CLSID_FilterGraph` `e436ebb3-524f-11ce-9f53-0020af0ba770` | yes |
| `IID_IGraphBuilder` `56a868a9-0ad4-11ce-b03a-0020af0ba770` | yes |
| `IID_IMediaControl` `56a868b1-0ad4-11ce-b03a-0020af0ba770` | yes |

Corroborated by the error strings at `0x005cfae3`–`0x005cfc90`: *"Could not run the
DirectShow graph!"*, *"Could not add source filter to graph!"*, *"Could not render source
output pin!"*, plus the UTF-16 filter/pin names `TEXTURERENDERER`, `SOURCE`, `Output`.

This matters beyond pedantry: a port written against MCI would target the wrong API for
the five intro `.mpg` files and for `MpegVideoTexture::Update()`, which the D1 work already
found pulls frames from a live DirectShow graph.

## `launch.exe` — DOC-1 closed, and it is not tampering

Size matches exactly; the hash does not. Same-size-different-hash is the signature of an
in-place patch, and the obvious suspect is the NoCD patch that `re/prior_art/MashedRunner`
documents (`VersionMFL.cs:28-40`). **Tested directly, and it is NOT patched:**

| signature | at | result |
|---|---|---|
| unpatched `FF 90 0C 07 00 00 3B C3 7D 12` (messagebox call intact) | `0x0e8181` | **present** |
| unpatched `0F 85 54 02 00 00 8B 17` (conditional "do not run" jump) | `0x0e81a4` | **present** |
| patched `90 90 90 90 90 90 …` (NOPed) | — | absent |
| patched `EB 04 90 90 90 90 …` (jmp always) | — | absent |

Both original signatures are intact and neither replacement appears, so the file is
pristine. The recorded hash was simply never correct for this install. Anchor updated to
the measured value. `launch.exe` RVAs are not used by the port; this anchor only guards
against silent substitution.

## Imports, recorded because nothing else does

`MASHED.exe`: `ADVAPI32`, `DINPUT8`, `DSOUND`, `GDI32`, `KERNEL32`, `USER32`, `WINMM`,
`d3d9`, `ole32`. Note `DSOUND.dll` is imported directly (not via COM — `CLSID_DirectSound`
is absent), consistent with `Audio/AudioDSound.cpp` and its `0x005d09dc` IID deref.

`launch.exe`: `MSVBVM60.DLL` only.

## New facts recovered

- `MASHED.exe` build stamp **`Jun 14 2004 11:39:38`** (`0x005cd67c` / `0x005cd688`, printed
  by the `Build date : %s, %s` format at `0x005cd663`).
- RenderWare core **`Core built at Mar 17 2004 07:14:02`** (`0x005d8b41`) — a three-month
  gap, so the RW build predates the game build.
- Linker version **7.0** → MSVC .NET 2002.

## Naming veins — status after this sweep

| vein | yield | state |
|---|---|---|
| `"Calling <Name>"` log strings | 14 | **exhausted** (exactly 14 exist, all harvested) |
| `push fn; push name; call <registrar>` | **70** | **exhausted** — only 2 registrars exist (`0x0047b980` ×68, `0x004714f0` ×2) |
| `Usage : Fn(args)` strings | 6 | exhausted; all 6 are `Rain*`, already covered by the registrar vein, but they add **parameter names and arity** |
| static `{name, fn}` pointer tables | **0** | **empty.** 58 candidate pairs found in `.rdata`/`.data`, all false positives — the "code pointers" are 3-letter ASCII locale codes from the MSVC CRT table (`0x00414e45` = `"ENA\0"`). Consistent with Lua 4.0 registering by call, not by table. |
| assert / error strings | 1 | `RtFSManagerRegister failed` (`0x005ce6f5`). No `__FILE__`-style asserts exist. Subsystem tags only otherwise (`COURSES ERROR: …`). |

**Conclusion: the naming veins are exhausted.** The registrar vein was the large one and it
is fully mined. Further names would have to come from a different class of evidence
(the Xbox twin, or behavioural inference), not from strings in this image.


---

# Part 2 - boot-patch and runtime claims

Same method: check the claim against the artifact. Here the artifact is the **diff between
`original/MASHED.exe` and `original/MASHED.exe.unpatched`**, the only authoritative
statement of what is actually applied.

**111 differing bytes in 14 clusters, all in `.text`, file sizes identical.**
(Now **112 / 15** — `no_focus_pause` was adopted 2026-08-19, adding a single byte at
`0x004996d3`. Everything below describes the pre-adoption state.) Every cluster
maps to a documented patch address. **No undocumented modification exists on disk.**

| VA | bytes | old -> new | patch |
|---|---|---|---|
| `0x0040283f` | 5 | `e8 3c 24 09 00` -> `90 90 90 90 90` | `skip_movies` |
| `0x004951aa` | 5 | `a1 c0 47 61 00` -> `33 c0 90 90 90` | `skip_controller_dialog` |
| `0x004951f0` | 5 | `a1 bc 47 61 00` -> `33 c0 90 90 90` | `skip_selector` |
| `0x00495870` | 5 | `83 ec 6c a1 38` -> `e9 60 33 07 00` | `fix_joypad` (detour) |
| `0x004963e7` | 1 | `74` -> `eb` | `disable_log` |
| `0x00496400` | 1 | `81` -> `c3` | `disable_log` |
| `0x00496490` | 6 | `81 ec 14 01 00 00` -> `31 c0 c3 90 90 90` | `disable_log` |
| `0x00498bc0` | 4 | `a1 28 60 61` -> `b8 80 02 00` | `fix_camera_res` (= 640) |
| `0x00498bd0` | 4 | `a1 2c 60 61` -> `b8 e0 01 00` | `fix_camera_res` (= 480) |
| `0x00498dbc` | 2 | `75 44` -> `90 90` | `show_windowed` |
| `0x004a4541` | 5 | `6a 40 ff 74 24` -> `e9 7e b1 06 00` | `fix_fopen` (detour) |
| `0x00508bd5` | 39 | `cc...` -> trampoline | `fix_joypad` code cave |
| `0x0050f6c4` | 28 | `cc...` -> trampoline | `fix_fopen` code cave |
| `0x005bc750` | 3 | `83 ec 34` -> `33 c0 c3` | `skip_audio_com` |

## Claim by claim

| Claim | Verdict | Evidence |
|---|---|---|
| "Nine on-disk binary patches" | **CONFIRMED as a count** | exactly 9 distinct patches applied |
| ...but the bullet list under it names **ten** | **CATEGORY ERROR, fixed** | `skip_intro` is a **data** patch. Its script targets `0x00495350`, which is **not** in the diff; it replaces `.mpg` files, not bytes in the exe |
| `fix_camera_res` forces 640x480 | **CONFIRMED** | `mov eax, 0x280` (640), `mov eax, 0x1e0` (480) |
| `fix_fopen` at `FUN_004a4541` | **CONFIRMED** | patched at exactly `0x004a4541`; detour to a cave holding the relocated `6a 40 ff 74 24 0c ...` prologue |
| `fix_joypad` guards `FUN_00495870` | **CONFIRMED**, mechanism clarified | patched at exactly `0x00495870`, but it is a **detour** to `0x00508bd5`, not the inline guard the wording implies |
| `skip_movies` must keep the `push 0` at `0x40283d` | **CONFIRMED** | the call at `0x0040283f` is NOPed; `0x0040283d` is untouched |
| `skip_intro` replaces 5 `.mpg` | **CONFIRMED** | `intro`/`renderware`/`supersonic`/`empire`/`small` all exactly 167,414 bytes; `frontend.mpg` untouched at 38 MB |
| `skip_powerups` RETIRED, refuses to apply | **CONFIRMED** | script header: RETIRED PATCH - DO NOT APPLY (kept as un-applier / refusal guard) |
| Canonical `videocfg.bin` is 800x600 | **CONFIRMED** | byte-identical to the canonical; trailing dwords `0x320`/`0x258`/`0x20` = 800x600x32 |
| `MASHED_FPS_CAP` default 60 | **CONFIRMED** | `d3d9_shim.cpp:329` |
| "Apply the four binary patches" | **WRONG, fixed** | stale by five; the same file says nine, 75 lines earlier |

## The undocumented eleven

`scripts/` holds **21** `patch_mashed_*.py`. Nine are applied, `skip_intro` is the data
patch, and **eleven are neither documented in `CLAUDE.md` nor applied**:

`clean_exit`, `fix_movie_uaf`, `force_keyboard`, `heap_reserve`, `no_focus_pause`,
`skip_intro_logos`, `skip_teardown`, `unlock_all`, `unlock_restore`, `unlock_tracks`,
plus the retired `skip_powerups`.

Their being unapplied is proven by construction: all 14 diff clusters are consumed by the
nine, so nothing else has touched the binary. Several look useful (`no_focus_pause`,
`force_keyboard`, `unlock_tracks`) and a reader of `CLAUDE.md` would not know they exist.

## Incidental

`original/videocfg.bin` embeds this machine's GPU string (`NVIDIA GeForce RTX 5070 Ti`), so
the committed canonical was generated on this machine and is **not portable** to another
GPU. Not a defect, but worth knowing before treating it as universal.
