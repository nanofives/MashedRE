# RESULT BIND-CRT-498510 (2026-11-13, roadmap run 43): GREEN

Follow-up to BIND-CRT-STDIO-CALLERS (`verify/portorigtop_20261112/RESULT_BIND-CRT-STDIO-CALLERS.md`, "Next"). The second
fopen caller, rva 0x00498510 (controller defaults + `contcfg%d.bin` load), now runs on the `MASHED_ORIG_TOP` knob path
from its source `re/match/s16/00498510.c` in place of the `PortTop_Missing_00498510` stub. Top-down row (WORKER_RULES
"Row kinds"): every new TU is `// RVA-LINT: ORIGTOP-ONLY` under `MASHED_STANDALONE`, no file the default path executes
changed. **Self-test: 12/12 PASS.** Base: `orch/integration` @ `a6636cab2`.

## 1. L-126 decision for 0x00498510: PROVISIONAL (not admitted)

| check | result |
|---|---|
| `py -3.12 re/tools/match/equiv.py re/match/s16/00498510.c --only 0x00498510` (equiv.py @ `a9ca480f1`, default mode, no `--relax-icall`) | `0x00498510 NOT-PROVEN esp-unknown: esp = (and32((esp0+-0x4),0xfffffff8)+-0x22c)` |
| match.py (source header) | best 95.3%, 11 differing bytes, all from one instruction placed differently in the record restore copy (`lea esi / mov edi,ebx / mov ecx,0x80` vs `mov ecx,0x80` first) |

The prover cannot model the `/GS` frame's `and esp,-8` alignment, so the verdict is NOT-PROVEN and not a conditional one:
no caller scan applies, no `--relax-icall`, no x87. Per L-126 the source is listed in `re/match/PROVISIONAL.tsv` (status
`provisional`, knob path only) and bound. Not admitted, not C4 (L-126, L-128).

## 2. What is bound

| rva | function | source | TU | was on the knob path |
|---|---|---|---|---|
| 0x00498510 | controller defaults + contcfg load | `re/match/s16/00498510.c` 95.3%, **provisional** | `Match/PortTop_00498510.cpp` (`Orig_00498510`) | `PortTop_Missing_00498510` (logged, did nothing). Deleted |
| 0x004971b0 | contcfg%d.bin load (file static) | same file, 100% | same TU | not reached |
| 0x00497190 | contcfg%d.bin name (file static) | same file, 100% | same TU | not reached |
| 0x00495790 | joypad count getter | `re/match/s16/00495790.c` 100% | `Match/PortTop_00495790.cpp` (`Orig_00495790`) | none in the exe |
| 0x00495830 | joypad name strcpy | `re/match/s16/00495830.c` 100% | `Match/PortTop_00495830.cpp` (`Orig_00495830`) | none in the exe |

Caller: 0x00495150 (`Match/PortTop_00495150.cpp`) now defines `FUN_00498510` as `Orig_00498510`.

Callee bindings (in `PortTop_00498510.cpp`): sprintf 0x004a2b60 -> `PortTop_LogSprintf`; 0x00496400 -> `Orig_00496400`
(matched log writer); fopen / fread / fclose 0x004a4541 / 0x004a49cf / 0x004a4368 -> `MashedCrt_*` (read mode opens the
`origtop_rw` overlay copy when present, else `original/<name>`); 0x00499710 / 0x00499720 -> `Global7e9584Get` /
`GetInputHinst` (matched, exe); 0x004b6520 -> `ZeroFillWrapper`; LoadStringA / DialogBoxParamA -> user32;
0x00498360 (dialog proc, no source) -> `PortTop_Missing_00498360` (logs, returns 0).

`.rdata` seed: `Match/PortTop_RdataSeed.inc` regenerated with `verify/porttop_20261108/gen_rdata_seed.py` over the
previous 12 sources plus `re/match/s16/00498510.c`. Diff = 5 added rows only: 0x005cf010 `rb`, 0x005d000c `contcfg%d.bin`,
0x005d001c / 0x005d0044 / 0x005d0064 (the three log formats).

rva-lint: 4 new `ORIGTOP-ONLY CROSS-TARGET` pairs allowlisted (0x00495790, 0x00495830, 0x00497190, 0x004971b0, each
against its asi-only `Match/MatchS16_*.cpp`), then `OK -- no NEW duplicate bodies (237 known)`.

### Knob-path facts the original does differently (not fixed here)

- **LoadStringA strings.** The exe carries no string table, so LoadStringA(0x007e9580, 0x82a + lang) and (0x834 + lang)
  load nothing. The self-test shows the call writes an empty string (sentinels at 0x007730b4 / 0x007730f8 became `""`),
  so the keyboard record's name is `""`. The original loads its own resource strings there. Nothing on the knob path reads
  0x007730b4 / 0x007730f8 / the record names yet.
- **Dialog.** 0x00495150 passes 0x006147c0 as `show`. The reference carries `skip_controller_dialog` (0x004951aa,
  `show` = 0). On the exe 0x006147c0 is 0 in all four boots (no `0x00498360` line). With `show` = 1 the self-test shows
  DialogBoxParamA returns (template 0x67 absent) without reaching the proc.
- **Joypads.** 0x00772fac stays 0 on the knob path (0x00495ee0 is still a missing stub), so the boots exercise only the
  keyboard record (0) and the three none records. The joypad branch is covered by the self-test.

## 3. Checks

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, .asi not deployed. mashed_re.exe sha256 `6a3d52f2e5aaf207fe86423a8818c7b8f515f7814112f7306fd74f1a3d683eac` |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, sha256 `212c9affd2db3e23da02f232a6c6a1b2e7e136dad4be3c600fd8331aff356b65` |
| `ddkobj.py check` | IDENTICAL 1192/1219, DIFFERENT 12, not-in-map 15, units linked 1167/1443, refusals 4 (wrapper-redefines-globals, none in this row). Integration head (INTEGRATION_LOG 2026-11-13 11:19): 1188/1214, DIFFERENT 11, units 1164/1440. New IDENTICAL: 0x00497190, 0x004971b0, 0x00495790, 0x00495830 (every 100% function of this row). New DIFFERENT: **0x00498510** only, 11 bytes from +0x23d = the documented 95.3% residue |
| **self-test** `verify/portorigtop_20261113/run_contcfg_selftest.bat` (compiles `PortTop_00498510.cpp`, `PortTop_00495790.cpp`, `PortTop_00495830.cpp`, the four log TUs and `PortTop_CrtStdio.cpp` unchanged into a test exe with the original data range mapped) | **12/12 PASS** (`contcfg_selftest_output.txt`). Covers: pointer table 0x00773098 and 0x00773118 = table[lang]; keyboard record (f0 6, type 2, 13 binds); none records; name buffer 0x007730e4; 2 joypads (type 1, joy, name from 0x00771eb4 + i*0x448, 9 binds); overlay contcfg kept when f0 and name equal; defaults restored on f0 or name mismatch; read from `original/` when no overlay copy; no write; `show` = 1 returns without the proc |
| rva-lint | OK, 237 known, 0 new |
| captures / menu walk / G-CTRL / B | not run: top-down row, no menu code changed. The merge train runs the default gates |

Note on the object build: `ddkobj.py` skips a unit only when every RVA in it is provisional. This unit also holds the
two 100% statics, so the DDK object is linked and 0x00498510 runs as the DDK compile of the 95.3% source (the one
instruction-order residue) in the object build, and as the VS2022 compile in the default build.

## 4. Knob boots (`MASHED_ORIG_TOP=1`, `MASHED_DET_FRAMES=3000`, private root `D:/MashedWT/BIND-CRT-498510_root`, test-slot outside, `mashed_lock.py run --label BIND-CRT-498510` inside)

| arm | exe | WinMain | overlay `mashed.log` contcfg lines | stub line |
|---|---|---|---|---|
| def_on_rw0 | default | returned 0 after 3000 frames | Attempting + Unable x4 (contcfg0..3) | 0 |
| ddk_on_rw0 | object | returned 0 after 3000 frames | Attempting + Unable x4 | 0 |
| def_on_rw0_cfg0 (planted `origtop_rw/contcfg0.bin`: f0 6, name `""`, type 2) | default | returned 0 after 3000 frames | contcfg0 **Loaded**, contcfg1..3 Unable | 0 |
| ddk_on_rw0_cfg0 (same file) | object | returned 0 after 3000 frames | contcfg0 **Loaded**, contcfg1..3 Unable | 0 |

- No FATAL or exception line in any boot log. The `rc=` line was dropped by the script's output filter (`tail -6`);
  all four exited inside the 300 s timeout with WinMain's return line.
- The default and object builds write the same `mashed.log` below the date header on both arms (diff empty).
- `original/contcfg*` absent after all four boots (no write, the reads only).

Raw files (not committed), sha256:
```
c19bdb98255e38dd51c8df776f992a98b6082240d54eb17c6db2cddc8dc4b3c5  D:/MashedWT/BIND-CRT-498510_data/r43/boots/def_on_rw0.log
4a77c6488a38264541e7ca0549369e3e060961b8b7875e251288cae1fcf0e340  D:/MashedWT/BIND-CRT-498510_data/r43/boots/def_on_rw0.mashed.log
1d5ff9728a978d7520b8b16e5bc97156948762246cebcf4c80060c3478f559f7  D:/MashedWT/BIND-CRT-498510_data/r43/boots/ddk_on_rw0.log
4d85159f09760c12893b9b16b6b8681d2ba69289885dbfb30b77f663e75c03eb  D:/MashedWT/BIND-CRT-498510_data/r43/boots/ddk_on_rw0.mashed.log
3427eb5540d2c0472d85a39084ce3b47c9ce454a38cd3f47b2a650621aba2b4e  D:/MashedWT/BIND-CRT-498510_data/r43/boots/def_on_rw0_cfg0.log
7012b41d24221739da93725b375ea11f482cfaf7c477498ca7d6fad8e1e4d55f  D:/MashedWT/BIND-CRT-498510_data/r43/boots/def_on_rw0_cfg0.mashed.log
4889ef346f3d07d289d9d7b970b58c60fcbf66df030508ee41e6dc5c5c265aac  D:/MashedWT/BIND-CRT-498510_data/r43/boots/ddk_on_rw0_cfg0.log
7dcde420f8a2d82df96ee0c79aafae0a4a5fc4db2db1ecead416d27cc9d49593  D:/MashedWT/BIND-CRT-498510_data/r43/boots/ddk_on_rw0_cfg0.mashed.log
b4cc0761a2a9996cd66d88c768935c9fede5f71efcb083eadd2e3351aa46d7c3  D:/MashedWT/BIND-CRT-498510_data/r43/boots.sh
357acab1975249a4db382f7d4f7475b487af775ab54ec5aa5410c82e2b4a8301  D:/MashedWT/BIND-CRT-498510_data/r43/boots.out
```
Build logs and `BYTECHECK.tsv`: `D:/MashedWT/BIND-CRT-498510_data/`.

No promotions and no tracker edit: 0x00498510 is provisional (never C4 by that status), and the other four functions'
C4 status is a match-route matter, unchanged by this row.

## Next

- 0x00498510 to C4 or admission: the residue is one instruction order; a 100% source (or an equiv.py that models the
  `and esp,-8` frame) would move it out of PROVISIONAL.tsv.
- LoadStringA on the knob path: the original's controller string resources (0x82a / 0x834 + language) are not in the exe.
  Carrying them needs an asset decision (embed in `mashed_re.rc` or read from data).
- The video-config cluster (`Match/PortSave01_Config.cpp`) still binds its CRT callees to `SettingsCfg*` (BIND-CRT-STDIO "Next").
