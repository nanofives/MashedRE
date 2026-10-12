# negctl fix U-9663: 0x004971b0 ControllerConfigLoad_j5 (`controller_config_load_j5`), result (2026-10-12)

Branch `fix/u9663` (base 999000af). Built with `MASHED_NO_DEPLOY=1`; the shared `original\mashed_re_dev.asi` was
never touched. Nothing under `original\` was created, opened for write or modified (`ls original/contcfg*` is empty
before and after). No path2, no tracker edits, no re-classify.

**Verdict: GREEN.** The port body (`Input/DInput.cpp`, as left by the bootio re-port) needs no change. The fixture
now reaches the file-present branch on both arms, and the evidence set is complete for path1. **This row can reach
C3 again**: what is left is the parent's path2 (`run_verify_hook.py controller_config_load_j5`) and re-classify.

## What the old fixture got wrong

Registry entry: `eax_implicit_int`, slots 0..9, `crash_equal_ok`. The formatter 0x00497190 writes the RELATIVE name
`contcfg%d.bin` (format 0x005d000c, `push 0x5d000c` at 0x00497191; buffer 0x007730e4, 0x00497196 / 0x004971a3), and
no such file exists in the game CWD (`original\`). So `call 0x4a4541` (0x004971cd) returned NULL on every slot and
both arms took `je 0x49720f` (0x004971d9) to the "Unable to load" exit, `xor eax,eax` (0x0049721f). The read path
(`call 0x4a49cf` _fread at 0x004971ed, `call 0x4a4368` _fclose at 0x004971f3) that the bootio re-port switched to the
game CRT never ran, so the row's evidence measured nothing that changed.

## Fixture design (`re/frida/negctl_fixtures/z_fix_u9663.py`)

- arg_type `scalars_to_scattered_globals`, signature `uint32(uint32)`, `eax_arg: 0` (both arms through
  `mov eax,[esp+4]; jmp target`; the target takes the slot in EAX, `mov esi,eax` at 0x004971b3), `freeze_main`,
  `fold_ret`.
- `stub_at` replaces the formatter 0x00497190 on BOTH arms (0 stack args, reached by `call 0x497190` at 0x004971b5
  with the slot in EAX) with a callback returning `scratch + EAX*0x80` (new `ret_scratch` option, below). Each test
  seeds four absolute paths at scratch + k*0x80, one per slot, all under `%TEMP%\mashed_u9663\`, so a port that hands
  the formatter the wrong EAX opens a different file and reads RED. EAX above 3 would return NULL; no vector uses it.
- Everything after the formatter is real game code on both arms: 0x004a4541 (on the patched reference the
  `fix_fopen` JMP into the cave 0x0050f6c4, which the port also reaches since it calls 0x004a4541 by VA; real FILE*
  values are above 0xffff, so the cave's `cmp eax,0x10000` never fires), the game _fread into
  `0x007e95c0 + slot*0x200` (`shl esi,9` 0x004971e1, `add esi,0x7e95c0` 0x004971e6, count `push 0x200` 0x004971dc,
  size `push 1` 0x004971e4), and the game _fclose.
- Observed: the whole 4-slot table `0x007e95c0..0x007e9dc0` (0x800 bytes, pre-filled 0xA5 per arm, restored after),
  FNV-folded with the return (1 at 0x00497208, 0 at 0x0049721f).
- Files (written by the module at import, idempotent): `cc_valid0..3.bin`, 0x200-byte records in the
  `re/analysis/structs/contcfg_record.md` layout (+0x000 = 6, +0x004 name, +0x108 13 binding dwords, +0x13c type
  1/1/2/0, +0x140 joypad index, distinct tail per slot); `cc_long.bin` 0x2c0 bytes (record 2 + 0xc0 extra);
  `cc_short.bin` 0x51 bytes; `cc_empty.bin` 0 bytes; `cc_absent.bin` is deleted if present.
- Branches: the target has two, FILE* NULL or not (`test ebx,ebx; je` 0x004971d7/0x004971d9). It does not check the
  _fread count, so short and empty files return 1 with a partial or no copy. The record checks (+0x000 == 6, name
  strcmp) are in the caller FUN_00498510 (0x00498694 onward), not in this function, so "bad content" has no branch
  here to test.
- 14 vectors: valid on slots 0..3; long (slot 2); short (slot 1); empty (slot 3, and slot 0 with valid neighbours);
  absent (slots 0 and 3); slot 1 with only a neighbour's path valid; slot 2 with its own path absent; slot 3 short with
  valid neighbours; slot 2 reading slot 0's record. Slots stay in 0..3 because the table is 0x800 bytes
  (FUN_00498510 zero-fills 0x800 at 0x007e95c0).

## Harness change (minimal, additive)

`re/frida/diff_template.js`, `scalars_to_scattered_globals` `stub_at`: an entry may carry
`ret_scratch {off, eax_stride, eax_max}`; its callback then returns `scratch + off + EAX*eax_stride` (EAX from
`this.context` of the replaced call), or NULL when EAX > eax_max. Only entries with `ret_scratch` and no recorder
take the new branch, so the existing `stub_at` users (`fps_discretise` with a recorder, the settings `bare` stubs,
`fsopen_safe` on another handler) are unchanged. `run_diff.py` needed no change: `stub_at` dicts pass through and
`eax_arg` / `scratch_size` / `freeze_main` were already forwarded. The handler's `// MECHANISM:` line and
`re/frida/ARG_TYPES.md` were not regenerated.

## Evidence (`re/analysis/negctl/reearn_20261011/evidence/`)

| file | result |
|---|---|
| `fix_diff_controller_config_load_j5.csv` | GREEN 14/14, 10 distinct fingerprints |
| `fix_diff_controller_config_load_j5_selfbaseline.csv` | GREEN 14/14 |
| `fix_negctl_u9663.tsv` (= `runs/fix_u9663/negctl_20261012_122036.tsv`) | real GREEN 0/14, baseline GREEN 0/14, null RED 11/14, ret RED 14/14, field_last RED 9/14 (flip 0x007e9bbf), field_none CTRL-GREEN, field_first BLIND (flip 0x007739f8) |
| `fix_witness_hostcrt_controller_config_load_j5.csv` | RED 11/14 |

Fingerprint structure matches the bytes: the long file equals the valid slot-2 record (only 0x200 read); the two
empty-file vectors agree (ret 1, table untouched) and differ from the three absent-file vectors (ret 0, table
untouched); the cross-slot vectors differ from the same-slot ones. null RED is 11/14 because the three absent-file
vectors already expect "return 0, no writes".

field_first BLIND: the first byte the reimpl arm changed is 0x007739f8, written by the game CRT during
open/read/close and outside the observed table. It lies 0x10 below DAT_00773a08, the CRT lock storage block start
(`re/analysis/boot_crt_exit/0x004a774d.md`). [UNCERTAIN] which CRT field 0x007739f8 is; not observed because CRT
stream state is shared with every other open in the process.

Witness (not a fix, the port was GREEN): the pre-bootio host-CRT calls restored alone (`fread`/`fclose` of the DLL in
place of `s_GameFread` 0x004a49cf / `s_GameFclose` 0x004a4368, FsopenSafe still the game's), RED 11/14, `access
violation accessing 0x14` in the reimpl arm on every vector that opens a file; the three absent-file vectors stay
GREEN (NULL branch, untouched by the defect). After the witness the source was restored from a byte copy with a fresh
mtime, the build log showed `Input\DInput.cpp (source newer)`, and both diff CSVs above were re-taken on that build.

## Port notes

- No port defect. The body matches 0x004971b0..0x00497222 call for call, with stride 0x200 (`kCtrlCfgStride`).
- Not measured: the three log calls. The original calls 0x00496400 (`call` at 0x004971c2, 0x004971fe, 0x00497215),
  which the `disable_log` boot patch neutralises on the reference; the port calls its own `ConfigLogDebug` export
  (C4, the body installed at 0x00496400), which returns early when the log handle is NULL. Nothing in the fixture
  observes the log.
- The port's header comment line "valid -> 0x004971ef CALL _fread(&DAT_007e95c0 + EAX*0x80 ...)" still says
  `*0x80` while the code (and 0x004971e1 `shl esi,9`) use 0x200. Comment-only; left untouched.

## Callers / callees (hooks.csv)

- Caller: FUN_00498510 C2 (sole call site 0x00498705, `re/tools/callsites.py`).
- Callees: FUN_00497190 C2 (stubbed by the fixture on both arms), ConfigLogDebug 0x00496400 C4, FsopenSafe
  0x004a4541 C2 (stays C2 per U-9662), FUN_004a44e5 __fsopen C2, FUN_004a49cf _fread C2, FUN_004a4368 _fclose C2.

## New U-rows

None required. Optional: the formatter 0x00497190 is outside this fixture's measurement (stubbed); if it is ever
ported, it needs its own fixture.

## Summary

U-9663 is GREEN. The old fixture never left the NULL-FILE* exit because the relative `contcfg%d.bin` name has no file
in `original\`. The new fixture stubs the formatter 0x00497190 on both arms to return an absolute `%TEMP%` path chosen
by EAX (one small additive `ret_scratch` option in diff_template.js) and lets the game's own open, read and close run.
Result: 14 vectors across both branches, GREEN 14/14 with 10 distinct fingerprints, self-baseline GREEN 14/14. Negctl:
null, ret and field_last RED, field_none CTRL-GREEN. Restoring the pre-bootio host-CRT fread/fclose reads RED 11/14,
so the fixture measures the branch the bootio re-port changed. No port change was needed and nothing under
`original\` was written. The row can go back to C3 once the parent runs path2 and re-classify.
