# RESULT D4-WIP-PANELSORT (2026-11-06)

Verdict: **GREEN.** `PanelSortInit` (0x00420d00, `.asi` naked transcription) is bit-identical to the original on
path1 (5/5) and installs and runs through the inline JMP on path2 (2/2).

## What landed

- `8a7ea0538` (area-frontend r9 WIP checkpoint, marked INCOMPLETE) cherry-picked clean as `27ea4a66a`:
  `mashedmod/src/mashed_re/Boot/PanelSortInit.cpp` and one `asi_sources.rsp` line.
- `2a3b259fa` finishing commit: header and asm checked against the unpatched image (0x00420d00..0x00420d3a,
  9 instructions, same operands, one documented substitution `CALL rel32` to `mov eax, 0x004b6520 / call eax`, EAX
  dead across it), and the `panel_sort_init` entry in `re/frida/hooks_registry.py`.
- `.asi` only. The standalone is untouched, so no default gate can move.

## Test design (`hooks_registry.py` `panel_sort_init`)

`cache_setter_observe`, `void(void)`. Observed: all 36 dwords of the 0x90-byte block at `0x0063e4b8`. Each test
seeds all 36 with `pat + k*0x01010101` (none in 0..3), so a skipped zero-fill, a short fill, or a store at the wrong
offset leaves a seed value in the readback. Patterns path1: `0xDEADBEEF 0x11111111 0x80000000 0x7F7F7F7F 0xC0DEC0DE`.
Path2: `0xDEADBEEF 0x5A5A5A5A`.

Original readback (every test): the block zeroed, with dwords at `+0x18 / +0x3c / +0x60 / +0x84` (`0x0063e4d0 /
0x0063e4f4 / 0x0063e518 / 0x0063e53c`) = 0 / 1 / 2 / 3.

## Runs

Private root `D:\MashedWT\D4-WIP-PANELSORT_root` (robocopy of `original\`), `MASHED_ROOT` + `MASHED_DEV_ASI` +
`MASHED_VERIFY_ASI` pointing at that root's `mashed_re_dev.asi` (a copy of this worktree's build), one test slot,
each boot inside `mashed_lock run --label D4-WIP-PANELSORT`. One game at a time.

| run | result |
|---|---|
| `run_diff.py panel_sort_init` (path1) | GREEN 5/5 bit-identical (`evidence/diff_panel_sort_init.csv`) |
| `run_verify_hook.py panel_sort_init` (path2) | PASS: opcode E9, rel32 `0x5d1b0a2b` OK, reimpl interceptor fired 2/2, 2/2 calls returned, both readbacks equal the original's (`evidence/verify_hook_install_panel_sort_init.txt`) |

`.asi` identity (memory `shared-asi-overwritten-between-window-steps`): path2 reported `reimpl 0x5d5d1730 - base
0x5d5b0000 = 0x21730`, and the `PanelSortInit` export RVA in the `.asi` under test is `0x21730`. Not a foreign build.

## Hashes

| file | SHA-256 |
|---|---|
| `D:\MashedWT\D4-WIP-PANELSORT_root\original\mashed_re_dev.asi` (= `mashedmod/build/mashed_re_dev.asi` @ `2a3b259fa`) | `5bbc340a1ee24d27be2a45ba6a6c7e0faa58c18620aa71c662803970fb137d46` |
| `D:\MashedWT\D4-WIP-PANELSORT_root\original\MASHED.exe` (patched, ten-patch set) | `b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87` |
| `evidence/diff_panel_sort_init.csv` | `bdab3943ac72d19756b94b52ac7df560545a50a7980161c574904f1c6b3f06f9` |
| `evidence/verify_hook_install_panel_sort_init.txt` | `9f4faa0332f5df31d9d91d60cc5e922a869e97ac6180d332b3970d7208bdbbd0` |
| `evidence/run_diff_console.txt` | `a3e450ce5178a80a015164444ab7a22b1edbc21fe268cbacd3b66c0e104dc7d4` |

## Tracker

re-classify on this branch: `hooks.csv` 00420d00 C2 -> C3 (`PanelSortInit`, file `Boot/PanelSortInit.cpp`).
C3 gate: semantics stated (below), reimplementation in `mashedmod/src/mashed_re/Boot/PanelSortInit.cpp`, build clean,
`RH_ScopedInstall(PanelSortInit, 0x00420d00)`, caller `0x00402750` AppInitialiseOnBootup (call at `0x004029b7`) C4,
callee `0x004b6520` ZeroFillWrapper C3, the one attached structural row U-3226 resolved (section below). No C4 claim:
no canonical-scenario run with the JMP live was made.

## Expected gate effect

None. The standalone `mashed_re.exe` does not compile this TU, and the default `.asi` install set gains one
ordinary `RH_ScopedInstall` whose body is verified bit-identical above.

## U-3226 (structural): the block is a 4 x 0x24 array

Static scan of `MASHED.exe.unpatched` for every instruction whose operand falls in `0x0063e4b8..0x0063e547`
(`re/tools/disasm_va.py`, capstone). Every indexed reader uses element stride 0x24 from base `0x0063e4b8`:

| site | instructions |
|---|---|
| `0x004215c0` | `lea edx,[eax+eax*8]` (`0x004215cd`), `lea edx,[edx*4+0x63e4b8]` (`0x004215d0`), call `0x00420de0` |
| `0x004215e0` | `lea edx,[eax+eax*8]` (`0x004215e4`), `lea edx,[edx*4+0x63e4b8]` (`0x004215ee`) |
| `0x00420d80` | `lea eax,[eax+eax*8]` (`0x00420d84`) |
| `0x004215a0` | `mov esi,0x63e4b8` (`0x004215a1`), call `0x00420e00`, `add esi,0x24`, `cmp esi,0x63e548`, `jl` (`0x004215a6..0x004215b4`): 4 elements |
| `0x00420d40` | `mov esi,0x63e4c4` (`0x00420d42`), `lea eax,[esi-0xc]` = element base, `FUN_004b6520(base, 8)`, dwords `+0x8 / +0xc / +0x10 / +0x14` = 0, `add esi,0x24`, `cmp esi,0x63e554`, `jl` (`0x00420d50..0x00420d72`): 4 elements |
| `0x004212b0` | `mov ebx,0x63e4b8` / `mov edi,0x63e4bc` (`0x004212b8` / `0x004212bd`), `fld [edi]` / `fadd [ebx]` |

So `0x0063e4b8` is 4 elements of 0x24 bytes (0x90 total, the zero-fill size at `0x00420d05`), and `0x00420d00`
writes dword `+0x18` of element i to i (0..3). Field meanings beyond this are not claimed (mechanical only).
