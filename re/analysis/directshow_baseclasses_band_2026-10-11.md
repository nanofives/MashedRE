# DirectShow BaseClasses band, 0x0049c5d0..0x004a2680 (2026-10-11)

**Second ruling, same day:** the band extends to `0x004a2b5f`, adding the 21 rows `0x004a26b0..0x004a2b50` (see the last section). The 7 prior-session C3s stay at C3.

**Ruling (Mariano, 2026-10-11, relayed by the parent session):** the code range
`0x0049c5d0..0x004a2680` is statically linked DirectShow BaseClasses. It is library-skip,
following the round-184 libpng/zlib precedent. Its rows move to the subsystem
`third-party-library[directshow-baseclasses]`.

This note records the byte evidence behind the ruling and the per-row check behind each
relabel. Per-row data: `re/analysis/directshow_baseclasses_band_2026-10-11.tsv`.
Generator: `re/tools/dshow_band_check.py`, run from a worktree root. It reads
`original/MASHED.exe.unpatched` via `../../original`.

## Band evidence (all addresses in `original\MASHED.exe.unpatched`)

| What | Where |
|---|---|
| returns `0x00040237` (the documented value of `VFW_S_STATE_INTERMEDIATE`) | `0x0049e178` |
| returns `0x80004003` (`E_POINTER`) | `0x0049e149`, `0x0049ef19`, `0x0049f959` |
| returns `0x80040224` (a `VFW_E_` code) | `0x0049e67c` |
| returns `0x8004022b` (a `VFW_E_` code) | `0x0049e7bd` |
| returns `0x8007000e` (`E_OUTOFMEMORY`) after `CoTaskMemAlloc` | `0x0049cf20`, call at `0x0049cf14` (IAT `0x005cc2d4`) |
| `timeBeginPeriod` | `0x0049cb63` (IAT `0x005cc2ac`) |
| `timeGetTime` | `0x0049d083` (IAT `0x005cc2b4`) |
| 16-byte compare against `{56A868B2-0AD4-11CE-B03A-0020AF0BA770}` / `{36B73880-C2C8-11CF-8B46-00805F6CEF60}` (the published `IID_IMediaPosition` / `IID_IMediaSeeking` values) | `repe cmpsd` at `0x0049c5e4` / `0x0049c5f6`, constants at `0x005d0bbc` / `0x005d0afc` |
| further GUID constants of the `56A868xx-0AD4-11CE-B03A-0020AF0BA770` and `36B7388x-C2C8-11CF-8B46-00805F6CEF60` families, plus `{00000000-0000-0000-C000-000000000046}` and `{0000010C-0000-0000-C000-000000000046}` | `0x005d0b5c`, `0x005d0b6c`, `0x005d0acc`, `0x005d0b9c`, `0x005d0bac`, `0x005d0b1c`, `0x005d0b3c`, `0x005d1124`, `0x005d1134` |

No string in the image names a BaseClasses file or class. No Xbox twin and no
`platform_candidates.csv` row covers the range. The ruling rests on the constants and
imports above.

## Per-row check (138 hooks.csv rows in the range)

Each row was checked against the bytes before it was moved. A row would NOT fit if either
of these held:

1. **It calls first-party game code.** That means a `call`/`jmp rel32` to a function
   outside the range whose hooks.csv row is neither `third-party-library[...]` nor a CRT
   helper named `_`/`FID_`.
2. **It references game-owned data.** That means a data address that first-party code
   outside the range also references, where the shared address is not a constant.

Results:

- **Test 1: 0 rows call first-party code.** Outside calls go only to the CRT
  (`__alldiv` `0x004a4170`, `__allmul` `0x004a4220`, `__aulldvrm` `0x004a4da0`, CRT free
  `0x004a3bda`), the library bands, and imports.
- **Test 2: 19 rows share a data address with code outside the range, and every one is a
  constant, not game state.**
  - The GUIDs listed above. These are also referenced by the RenderWare audio DirectShow
    code at `0x005bxxxx` and by the game's derived renderer at `0x004944c0` / `0x00494c80`.
  - The vtable/function-table starts `0x005cf9d0`, `0x005cfd30`, `0x005d0e74`,
    `0x005d0e88`. In-range constructor/destructor pairs (`0x0049ec10` / `0x0049ebc0`, called
    only from `0x00493c00` / `0x00494ac0` and `0x00493c60` / `0x00494b20`) write them, and
    so do the game's derived constructors.
  - The immediate `0x00989680` (10,000,000), which is not an address.
  - The 8 zero bytes at `0x005d1008`, also read by code at `0x0051c220`, `0x0051dca0`
    and `0x005270e0`.
- **References INTO the range from outside are expected for a base-class library and do
  not disqualify a row.**
  - The game's derived classes (constructors `0x00493c00`, `0x00493c60`, `0x00494ac0`,
    `0x00494b20`, code `0x00493fd0`, `0x00494c80`) call base methods and install vtables
    whose slots point at inherited base methods.
  - The adjustor thunks at `0x00494250..0x004942a8` (`sub dword [esp+4],0xd4/0xd8;
    jmp 0x0049de60/70/80`) jump into the range.
  - The SEH funclets at `0x005ca2f0..0x005ca680` (band `msvc-seh-funclets`) unwind
    in-range frames.

**Verdict: all 138 rows fit. None is left as it was.**

Before the move: 85 `particle`, 46 `third-party-library[msvc-crt-main]`, 6 `util`,
1 `video`. The 46 `msvc-crt-main` rows (`0x004a0000..0x004a2680`) came from a band-level
FidDB classification of `0x004a0000..0x004b4000`. By the test above they belong to this
cluster. Example: `0x004a1100` sits in slot 1 of the cluster's vtables and is reached
from them.

### Prior-session C3 rows in the range (confidence NOT changed; ruled: stay C3)

These follow the round-184 precedent: a prior session's in-band C3s are flagged for user
review, not reverted. Their subsystem moves and their confidence stays.

- `0x0049c690` ComField14Get
- `0x0049c800` Zero49c800
- `0x0049da90`
- `0x0049f180` ComRefRelease14
- `0x0049f2b0` ComRefRelease10
- `0x0049fa40`
- `0x004a1790` ComReleaseThunk (util)

**Decided (second ruling):** they stay at C3 as library rows.

## The band continues past the ruled end (MOVED by the second ruling)

`0x004a26b0`..`0x004a2b50` (21 rows, all labelled `third-party-library[msvc-crt-main]`)
have no code caller outside `0x0049c5d0..0x004a2b5f` other than the SEH funclets
`0x005ca580`, `0x005ca5c8` and `0x005ca680`, and no data reference. In the other direction,
`0x004a23e0` (in range) is called from `0x004a26b0`. The first function with a game-side caller is `sprintf` at `0x004a2b60`,
which is called from `0x00401000`, `0x00401630` and others. The same applies to
`__security_check_cookie` at `0x004a2be9`.

This places the BaseClasses / CRT boundary at `0x004a2b60`, not `0x004a2680`. Those 21 rows
were first left as they were, pending a ruling. The second ruling moved them. The re-run check (`re/tools/dshow_band_check.py`, now defaulting to `HI = 0x004a2b5f`; the TSV covers all 159 rows) finds that none calls first-party code. The only shared data is the 16 zero bytes at `0x005d1144` (`0x004a2940`, `0x004a2980`; also read at `0x005bbdb0`). Their imports are `DeleteCriticalSection` (`0x004a26b0`), `CoTaskMemAlloc` (`0x004a2a30`) and `CoTaskMemFree` (`0x004a2aa0`, `0x004a2b30`). All 21 moved and none was left as it was.

The lower edge is clean. The bytes immediately before `0x0049c5d0` are a data table, and
the nearest function below it is `0x0049b300` (DirectInput8Create).
