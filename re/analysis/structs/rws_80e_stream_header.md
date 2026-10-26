# 0x80E stream header (inside a 0x80D RWS bank) and the exe wave tables

Step D4-WSJ-80E, 2026-10-26. Static only: Ghidra decomp of `MASHED.exe` (pool slot, `-readOnly`) and the bytes of
every 0x80D bank. Evidence and per-bank tables: `verify/wsj80e_20261026/RESULT_D4-WSJ-80E.md`.
Tool: `re/tools/rws_80e.py` replays the exe's walk on a file and checks it.

## Who reads it

| RVA | What it does with the 0x80E chunk (mechanical) |
|---|---|
| `FUN_005b1260` | Stream open. Sets stream dword `0x20` = `0x80d` (`0x005b1467`), opens the file, the state machine is `FUN_005b15e0`. |
| `FUN_005b15e0` | State machine. On chunk `0x80d` it searches for child `0x80e` (`0x005b1812` `PUSH 0x80e`). On `0x80e` (`0x005b17e9`) it reads `0x14` bytes into stream dwords `0x47..0x4b` (PREFIX). With no UUID/name filter set (stream dwords `0x4c`,`0x4d` = 0) it allocates and reads PREFIX`+0x00` bytes (BODY) into stream dword `0x46` and calls `FUN_005b8200(BODY)`. After chunk `0x80f` (`0x005b1c44`) it reads `*(BODY+0x20)` as the block size for its ring buffers, and `*(BODY+0x10)` into stream dwords `0x4f`/`0x50`. |
| `FUN_005b8200` | In-place fix-up of BODY. Each pointer-sized field in the file is a present flag. Non-zero means the item follows. The fix-up overwrites the flag with the item's address and consumes the bytes in the order below. |
| `FUN_005aca80` | Size of one format descriptor: `0x1c`, `0x2c` when fmt`+0x04` != 0, plus fmt`+0x14` when fmt`+0x10` != 0. |
| `FUN_005acaa0` | Copies the 7-dword format descriptor and, when fmt`+0x04` != 0, replaces it with the codec entry returned by `FUN_005acd60` (16-byte compare against the 9 entries at `PTR_DAT_00633674`). |
| `FUN_005a6190` | `DAT_00914700` = max(`0x10`, arg). This is the alignment `A` in the walk. The walk lands exactly on PREFIX`+0x00` with `A = 0x10` in all 31 banks. |
| `FUN_005b30e0` / `FUN_005b3020` | Seek and block bound. Read record A `+0x0c` (per-B dword), `+0x18`, `+0x1c`, record B `+0x14` (fmt), `+0x1a` (u16), `+0x20`, fmt `+0x0d`, fmt `+0x18` bit 2, BODY `+0x20`, `+0x24`. |
| `FUN_005a86a0` | Sub-sound select. Copies a 4-dword selector. Selector `+0x04` = 1 copies a name (strncpy `0x80`), = 2 copies a 16-byte UUID, other values copy neither. Then `FUN_005b0ca0` sets command bit `0x08`. |

## Layout

PREFIX (chunk data `+0x00`, `0x14` bytes; stream dwords `0x47..0x4b`):

| off | value in the files | reader |
|---|---|---|
| +0x00 | BODY size (`0x1acc` character banks, `0x108` cdaudio) | `FUN_005b15e0` reads this many bytes |
| +0x04 | `0x14` | raw |
| +0x08 | `0x10` | `FUN_005b15e0`: non-zero means a 16-byte UUID for the filter compare |
| +0x0c | `0x24` | raw |
| +0x10 | bank-name length without NUL (`red` 3, `cdaudio` 7) | `FUN_005b15e0`: name length for the filter compare |

BODY (chunk data `+0x14`). Fixed part, 11 dwords (`FUN_005b8200`: cursor starts at `param_1 + 0xb`):

| off | meaning in the walk | character banks | cdaudio |
|---|---|---|---|
| +0x00 | UUID present | non-zero | non-zero |
| +0x04 | name present | non-zero | non-zero |
| +0x08 | raw | 0 | 0 |
| +0x0c | nA, record-A count | 98 | 1 |
| +0x10 | record-A array (present flag in file) | non-zero | non-zero |
| +0x14 | nB, record-B count | 1 | 1 |
| +0x18 | record-B array | non-zero | non-zero |
| +0x1c | raw | `0x800` | `0x800` |
| +0x20 | block size (`FUN_005b15e0` after `0x80f`) | `0x2800` | `0x4800` |
| +0x24 | added to the seek offset in `FUN_005b30e0` | `0x2000` | `0x800` |
| +0x28 | raw | 0 | 0 |

Variable part, in `FUN_005b8200` order (`A` = 0x10, name advance = `(len + A) & ~(A-1)`):

1. BODY UUID, 16 bytes (if `+0x00`).
2. BODY name, aligned (if `+0x04`).
3. Record-A array, nA x `0x20`.
4. For each A with `+0x0c` != 0: nB dwords.
5. For each A with `+0x00` != 0: 16-byte UUID.
6. For each A with `+0x04` != 0: name, aligned.
7. For each A with `+0x14` != 0: A`+0x10` x `0x14` record C.
8. Record-C UUIDs, then record-C names.
9. Record-B array, nB x `0x28`.
10. For each B with `+0x14` != 0: format descriptor, advance `(size - 1 + A) & ~(A-1)`.
11. Record-B UUIDs, then record-B names.

Record A, stride `0x20` (the sub-sound record):

| off | field | readers |
|---|---|---|
| +0x00 | UUID present | `FUN_005b8200` |
| +0x04 | name present | `FUN_005b8200` |
| +0x08 | raw (0 in every file) | none found |
| +0x0c | per-B dword array, nB entries | `FUN_005b3020` (per-B value, mod B`+0x20`) |
| +0x10 | nC (0 in every file) | `FUN_005b8200` |
| +0x14 | record-C array (0 in every file) | `FUN_005b8200` |
| +0x18 | byte length in the 0x80F data | `FUN_005b30e0` (`/ BODY+0x20`) |
| +0x1c | byte offset in the 0x80F data | `FUN_005b30e0` (seek = BODY`+0x20` x block + A`+0x1c` + BODY`+0x24`) |

Record B, stride `0x28`: `+0x00` UUID present, `+0x04` name present, `+0x0c` raw (64 character banks, 7 cdaudio),
`+0x10` raw (`0x2800` / `0x4800`), `+0x14` format descriptor, `+0x18` u16 and `+0x1a` u16 (36/36 character banks,
4/4 cdaudio), `+0x1c` raw (228 / 2304), `+0x20` dword (8208 / 18432). In every file `+0x1c` = `+0x20` / (`+0x1a` x fmt`+0x0d`).
`FUN_005b8200` lines at `0x005b8200` body: when the codec UUID equals `DAT_005e6454` or `DAT_005e64b4` and fmt`+0x0d` == 2,
it writes B`+0x0c` = `0x40`, B`+0x1a` = `0x24`, B`+0x18` = 4. Every file's codec UUID is `936538ef11b62d43957fa71ade44227a`
= the bytes at `DAT_005e64b4`, so cdaudio (fmt`+0x0d` = 2) is rewritten at load. The character banks (fmt`+0x0d` = 1) are not.

Format descriptor (`0x1c` + 16-byte codec UUID = `0x2c`): `+0x00` dword (44100, and 11028 in english bluejay/melon/pink/red/shadow),
`+0x04` codec present, `+0x08` dword (= record A0's per-B value), `+0x0c` u8 = 4, `+0x0d` u8 (1 character banks, 2 cdaudio),
`+0x10` extra present (0), `+0x14` extra size, `+0x18` u8 flags, `+0x19` u8. [UNCERTAIN] which reader consumes fmt `+0x00`:
no reader was traced in this step.

## Facts from the bytes

- All 31 0x80D banks (cdaudio + 6 characters x 5 languages): the walk ends exactly at PREFIX`+0x00`, and the A records
  tile the 0x80F data exactly (A0 offset 0, each offset = previous offset + length, last end = 0x80F size).
- The 30 character banks have 98 A records with the same names modulo the 2-letter prefix: `1_1_1_PP_1..13`,
  `1_2_1_PP_1..25`, `1_3_1_PP_1..25`, `1_4_1_PP_1..35`. `FUN_0045cd30` compares a group count against `0xd`, `0x19`, `0x23`.
- `cdaudio.rws` has **one** A record (`Segment0`, offset 0, length 8,460,288 = the whole 0x80F). There is no per-track
  sub-sound inside cdaudio to select.
- There is **no voice-count field** in the 0x80E header, A, B or C records.

## Where the voice counts are: exe `.data`, not the bank

`FUN_0045d460` (permdict) and `FUN_00462950` (course) read static, initialized wave descriptor tables:

| table | base | stride | rows | read by |
|---|---|---|---|---|
| permdict | `0x006044f0` | `0x94` | 43 (`DAT_0068fc88` = wave count of the loaded bank) | `FUN_0045d460` |
| course | `PTR 0x0060c820[track id]` | `0x90` | wave count of the course bank (`DAT_00690478`) | `FUN_00462950` (`DAT_0088e670` = row base, `0x00462a44`) |

Row fields: `+0x00` name (C string, `0x80` bytes, looked up in the bank by `FUN_005a7aa0`), `+0x80` voice base
(written at load: `piVar4[-2] = iVar2` in `FUN_0045d460`, `*(row+0x80) = uVar7` in `FUN_00462950`), `+0x84` voice count,
`+0x88` flag (0: clear voice `+0x34` bit 2 and `FUN_005a6dc0(v,8,1,0)`, else set it and `FUN_005a6dc0(v,8,1,1)`),
`+0x8c` dword written to voice `+0x74` and passed to `FUN_005a6dc0(v,3,1,value)`. Permdict rows have a `+0x90` dword
that `FUN_0045d460` does not read. The track id is record `+0x10` of the 13 track records at `0x005f33f8` (stride `0x48`,
`FUN_0041e980`). Full tables: `verify/wsj80e_20261026/wave_tables.tsv`, `track_course_tables.tsv`.
