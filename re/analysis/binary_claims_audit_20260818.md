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
