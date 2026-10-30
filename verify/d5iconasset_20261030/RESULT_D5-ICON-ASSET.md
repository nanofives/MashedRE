# RESULT D5-ICON-ASSET (L-64 (b), Mariano 2026-10-30)

**Verdict: GREEN.** The loading-disc texture RWTEXDICTIONARY/404 is now a project asset embedded in
`mashed_re.exe`. The exe no longer opens `original\MASHED.exe`, and the LAN stage no longer ships it.
The disc draws byte-identical to the current build, G-CTRL and B hold, and a scratch stage has no MASHED.exe in its
files or its manifest.

Branch `orch/D5-ICON-ASSET`, base `orch/integration` @ 9d3e2421c. No overlap with what integration took in after
that (145895e24): none of the three edited files changed there.

## 1. Change

| File | What |
|---|---|
| `re/tools/extract_loadicon_asset.py` (new) | One-time extractor. Walks the PE resource directory by hand (no pefile) and copies RWTEXDICTIONARY/404 unchanged. `--check` re-reads it through the Win32 loader (LoadLibraryExW AS_DATAFILE + FindResourceW, the call the old exe path made) and compares |
| `mashedmod/assets/loadicon_404.txd` (new, 33932 B) | The asset: SHA-256 `2A4E74031AE49060BCAFCB76EB137FC1C48C7B63B86A088115E483B13D7AF0B2` |
| `mashedmod/assets/mashed_re.rc` (new) | `404 RWTEXDICTIONARY "loadicon_404.txd"`, LANGUAGE 9,2 (0x0809, en-GB, the same as in MASHED.exe) |
| `mashedmod/build.bat` | `rc /fo build\mashed_re.res assets\mashed_re.rc` before the exe link, `.res` added to the exe link line. The .asi is not affected |
| `mashedmod/src/mashed_re/exe_main.cpp:8564-8627` | `LoadLoadIconFromExe` becomes `LoadLoadIconResource`: `GetModuleHandleA(nullptr)` in place of `LoadLibraryExA("original\\MASHED.exe", AS_DATAFILE)`, `FreeLibrary` dropped. The FindResource / header / decode / upload body is untouched. Call site `:9660` renamed |
| `scripts/lan_release/stage_lan_release.py` | R7 gone: the `MASHED.exe.unpatched` / `MASHED.exe` staging block (old `:105-108`) and its docstring row. The docstring now says the release carries no original binary and that the updater prunes a MASHED.exe left by an earlier release (`mashed_update.ps1:146-156` deletes files under `original\` that the manifest no longer lists) |
| `verify/d5iconasset_20261030/cap_splash.py` (new) | Arm runner for the captures: MASHED_DBG_BBDUMP=<frame>, kills only its own PID |

`publish.sh:46` archives `mashedmod` whole, so the asset and the .rc reach the LAN build with no publish change.
No `.ps1` was edited.

## 2. Asset provenance

Extractor output (`--check`), source `original/MASHED.exe.unpatched` (SHA-256 `BDCAE093...EFD3C0E`, the anchor):

```
resource RWTEXDICTIONARY/404 lang 2057 at rva 0x0058a9a0, 33932 bytes, sha256 2A4E7403...7AF0B2
header  w=256 h=128 depth=8 stride=256
check   Win32 LoadLibraryExW/FindResourceW read is byte-identical
```

The patched `original/MASHED.exe` gives the same 33932 bytes (same SHA-256). The ten boot patches do not reach the
resource section. Re-reading the resource out of the built `mashed_re.exe` with the same tool gives the same SHA-256
at rva 0x00fa8080, so the embed is byte-exact.

Binary scan of the two exes for `mashed.exe` (any case, ASCII and UTF-16): the baseline has 2 hits
(`original\MASHED.exe` at 0x202015, `loadicon: LoadLibraryEx(MASHED.exe` at 0x202039), the new exe has 0.
`LoadLibraryExA` is no longer imported (count 1 to 0).

## 3. The disc draws the same (imgdiff)

Baseline = this worktree's build before the code edit (exe_main.cpp still reading MASHED.exe; it already carried the
.res, which is data its code never looked up). New = the build with the edit. Splash backbuffer at frontend frame N
(`MASHED_DBG_BBDUMP=N`), private root `D:/MashedWT/D5-ICON-ASSET_root`. "noexe" = every `MASHED.exe*` moved out of
that root for the arm, then put back.

| Arm | loadicon log line | BMP SHA-256 (8) | vs baseline same frame |
|---|---|---|---|
| base_full_f30 | decode+upload OK | 796911d9 | (ref) |
| new_full_f30 | decode+upload OK | 796911d9 | identical, 0 px |
| new_noexe_f30 | decode+upload OK | 796911d9 | identical, 0 px |
| stage_f30 (the scratch stage itself, section 5) | decode+upload OK | 796911d9 | identical, 0 px |
| base_full_f45 | decode+upload OK | 5c824c02 | (ref) |
| new_full_f45 | decode+upload OK | 5c824c02 | identical, 0 px |
| new_noexe_f45_r2, _r3 | decode+upload OK | 5c824c02 x2 | identical, 0 px |
| base_noexe_f30 (control) | `LoadLibraryEx(MASHED.exe) failed` | bd2543ba | 1849 px (0.60%), all in grid column 6 rows 0-1 = the disc |

The control shows the capture sees the disc: the old exe without MASHED.exe loses exactly the disc cells and
nothing else. The frame 30 vs frame 45 pair (2164 px, same cells) shows the disc's spin phase is in the frame too.
`disc_crops.png` puts the 140x140 crop at x 500-640, y 20-160 of seven arms side by side (order base_full_f30,
new_full_f30, new_noexe_f30, base_noexe_f30, base_full_f45, new_full_f45, new_noexe_f45_r2). Full table:
`imgdiff.txt`.

One arm was a re-run: the first new_noexe_f45 came out on the title screen (86% of pixels differ) because the splash
hands off on wall clock, `elapsed > 1500u` at `exe_main.cpp:5091`, and that boot took longer to reach frame 45. Its
loadicon line was `decode+upload OK`. Both re-runs (r2, r3) are byte-identical to the baseline. That is a timing
artefact of the capture method, not of the change.

## 4. Gates (new exe, merge_train.json refs)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe `5c87f4bc`, under test-slot and
mashed_lock, root `D:/MashedWT/D5-ICON-ASSET_root` (MASHED.exe present, as in the train's root).

| Gate | Got | Ref | |
|---|---|---|---|
| G-CTRL step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | hold |
| B K0 / K1 / K26 | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | 1d5bf4f9 x3 | hold |

`scripts/lint_rva_bodies.py`: 16 findings, all allowlisted, NEW=0. The full set (D2 x3, 13 tracks) was not run
here. Per the row, the merge train runs it as the -Full pass.

## 5. LAN stage without MASHED.exe (scratch, never published)

`stage_lan_release.py --app mashedmod/build --install <main>/original --out <scratchpad>/stage`: 204 files, 425.6 MB.
Files named `MASHED.exe*` in the stage: 0. Manifest entries containing `mashed.exe`: 0. The only exe/dll/asi in the
stage or the manifest is `mashed_re.exe`. Before this change the stage carried `original/MASHED.exe` (2,846,720 B).

The staged exe was then booted from the stage folder (`MASHED_ROOT` = the stage, nothing else on the path). It played
the four staged boot movies (empire, supersonic, renderware, intro, 89 s total), logged `loadicon: 256x128
decode+upload OK`, and its frame-30 splash is byte-identical to base_full_f30 (796911d9). That is the release layout
drawing the disc with no MASHED.exe anywhere. Nothing was published, deployed or pushed.

## 6. Raw data (D:/MashedWT/D5-ICON-ASSET_data, not committed)

`raw_sha256.txt` has the full SHA-256 of every cited raw file: the 10 capture BMPs, both exes
(`base/mashed_re.exe`, `new/mashed_re.exe`), the six gate files and the re-extracted `new_embedded_404.txd`. Logs:
`build_base.log`, `build_new.log`, `gates_run.log`, `stage.log`, `imgdiff.txt`.

## 7. For the lead

- No `.ps1` edits. No tracker change: no RVA moved on the C ladder (this is release plumbing, not a port).
- `CLAUDE.md` "LAN release" paragraph still says the stage includes "`MASHED.exe` (read as data only, the
  `.unpatched` anchor copy)". It is now out of date. I did not edit it (outside this step's write set). One line for
  whoever owns that text.
- New build input: `rc.exe` from the Windows SDK, which `vcvars32.bat` already puts on PATH (SDK 10.0.26100 here).
  A machine without the SDK would fail at the new rc step with `[ERROR] exe resource compile failed`.
- The comments at `exe_main.cpp:777-779` and `:5096` still describe where the original keeps the texture
  (MASHED.exe's resource). That is still true of the original, so they stay.
