# RESULT BIND-CRT-STDIO (2026-11-11): HANDOFF. Code and bindings committed, build and gates NOT run

Stopped at the PORT lead's wind-down request. The build was queued behind the merge train's lock when the row
stopped. It never started, and nothing was launched.

## Done (committed)

- `Match/PortTop_CrtStdio.cpp` (exe-only, `// RVA-LINT: ORIGTOP-ONLY`) plus 1 line in `exe_sources.rsp`. It binds 8
  functions to the exe CRT:

  | rva | function | identity |
  |---|---|---|
  | 0x004a4541 | fopen | libid fopen.obj, identical-unlisted |
  | 0x004a4368 | fclose | libid fclose.obj, identical |
  | 0x004a49cf | fread | libid ambiguous with _fwrite; code: calls `__fread_lk` |
  | 0x004a4b22 | fwrite | libid ambiguous with _fread; code: calls `__fwrite_lk` |
  | 0x004a4254 | fputs | libid differs (prologue order); code: lock, strlen, _stbuf, __fwrite_lk, _ftbuf, return 0 / -1 |
  | 0x004a3b84 | _vsnprintf | libid vsnprint.obj |
  | 0x004a43b9 | _ctime64 | libid matches x4 objects; code: localtime reads a 64-bit time |
  | 0x004aa00c | _tzset | libid tzset.obj |

- **FILE objects:** owned by the exe CRT. None of the 16 matched callers dereferences a FILE field.
- **Paths:** one rule, `PortTop_Fs_DiskPath`, factored out of `PortTop_RwStream04F.cpp` with no behaviour change to
  its two existing users.
- **fopen:** keeps the fix_fopen contract (NULL, never a garbage FILE*). **Write modes ("wb", "wt") return NULL and
  log one line**, so nothing writes into the shared `original/` install. Where those writes should go is a decision
  for whoever picks this up.
- **Time zone:** owned by the exe CRT. The `_tzset` binding runs the exe `_tzset` and mirrors `_timezone` /
  `_daylight` / `_dstbias` into 0x00616670/74/78, which the matched `_ftime64` 0x004a43d2 reads directly.
  0x00616670 is also seeded with the image value 0x7080. That is the original's own order: the static initializer
  first, then `__tzset` at runtime.
- **BINDINGS:** `update_bindings_bindcrtstdio.py` through `bindings_rows.apply`, 8 rows of kind `crt` (fputs's row
  changed from under-stand-in).
- **rva-lint:** OK.
- **Runnable re-measure:** 1,064 -> **1,074 (+10)**. +2 without fputs, and fputs alone adds +8. SIZE.tsv updated.

## Next step

1. Build with `MASHED_NO_DEPLOY=1`, then the knob boots (RWMOD off / on), 12 captures x2, walk_04, G-CTRL / B, and
   the port column. The scripts used for HEAP work as-is with the data dir changed.
2. Decide the write-mode policy: refuse, as now, or redirect beside the exe.
3. [UNCERTAIN] Whether the exe `_tzset` values equal the DDK `__tzset` values. Both read GetTimeZoneInformation when
   TZ is unset, but they were not compared.
