# RESULT — image check: two of my claims were wrong. `.data` is 90% BSS, and `DAT_005f29b8` is `0xff`.

Date 2026-10-08. Follows `RESULT_CLIPSRC.md` §5, chosen by **USER DECISION (Mariano, 2026-10-08)**:
"Confirm the image-zero values against `MASHED.exe.unpatched` before they get relied on."
**Static, read-only.** No build, nothing run. `original/` untouched (read as a file image only).
No C-level.

## 0. Verdict first — the check caught two errors

> **(1) `DAT_005f29b8` is FILE-BACKED with value `0x000000ff` (255), not 0.** I repeatedly wrote
> that it is "already known dead standalone" in a way that implied it had no initialised value.
> It does, and that makes it **transcribable** — the `0x005f2770` class, not the unreachable class.
>
> **(2) The three phase-3 globals are in `.data`'s UNINITIALISED tail** — no file bytes at all.
> They are zero at load **on both sides**. So the port is not missing an *initialiser* for them;
> it is missing the **runtime writer**. `RESULT_CLIPSRC.md`'s "all three globals are image-ZERO"
> is true but was framed as if the port could transcribe its way out. It cannot.

## 1. The section table, read properly

```
ImageBase=0x00400000  sections=6
  .text    VA=0x00401000 VSize=0x1c98e0 RawPtr=0x001000 RawSize=0x1ca000
  _rwcseg  VA=0x005cb000 VSize=0x000451 RawPtr=0x1cb000 RawSize=0x001000
  .rdata   VA=0x005cc000 VSize=0x01d044 RawPtr=0x1cc000 RawSize=0x01e000
  .data    VA=0x005ea000 VSize=0x32a704 RawPtr=0x1ea000 RawSize=0x04d000
  _rwdseg  VA=0x00915000 VSize=0x000008 RawPtr=0x237000 RawSize=0x001000
  .rsrc    VA=0x00916000 VSize=0x07eb18 RawPtr=0x238000 RawSize=0x07f000
```

**`.data` has `VSize = 0x32a704` (≈3.3 MB) but `RawSize = 0x4d000` (≈316 KB).** Only the first
~9.5% of `.data` is file-backed; everything past `RawPtr + 0x4d000` is BSS-style, zero-filled by
the loader with no bytes on disk.

**This generalises the `0x005f2770` finding.** "Is this address initialised in the image?" is a
**per-address question** answered by `offset_in_section < SizeOfRawData`, not by which section it
lands in. A flat `fileoff = VA − 0x400000` read — which is what `dump_rdata_floats.py` does and
what I used for the `0x005f2dd8` limit table — is only valid inside the file-backed part.

## 2. The four addresses

| address | section | status | value |
|---|---|---|---|
| `DAT_00639d70` | `.data` | off `0x4fd70` ≥ RawSize → **BSS, no file bytes** | zero at load, both sides |
| `DAT_00639d78` | `.data` | off `0x4fd78` ≥ RawSize → **BSS** | zero at load, both sides |
| `DAT_00897fe0` | `.data` | off `0x2adfe0` ≥ RawSize → **BSS** | zero at load, both sides |
| **`DAT_005f29b8`** | `.data` | **fileoff `0x1f29b8`, FILE-BACKED** | **`0x000000ff` = 255** |

`0xff` matches `FUN_004111c0` case 1 setting `DAT_005f29b8 = 0xff`
(`bucket_util_0040e4b0_0042f790/0x004111c0.md:25`) — the image ships the same value the race-init
path writes.

## 3. Corrections to earlier results

- **`RESULT_PHASE3.md` §2(b)** and **`RESULT_EXITGATES.md`**: I described `DAT_005f29b8` as dead
  standalone and implied the countdown "has no live value today" in the sense of being
  unobtainable. Wrong: it is file-backed `0xff` and transcribable exactly as `H3-CONSTS` handled
  the four floats and as the `0x005f2dd8` table was handled.
- **`RESULT_CLIPSRC.md` §0/§4**: "all three globals are image-ZERO … a faithful port of
  `FUN_004102f0` alone would produce no hold" — the **conclusion stands** (the port would exit
  immediately), but the **reason is sharper**: those globals are zero on the *original* too at
  load. The original's fly-in exists because `FUN_004053d0`/`FUN_00405400` **write** them at
  runtime. The port needs those writers; there is nothing to transcribe.

Both files carry pointers to this one.

## 4. What is NOT claimed

- Not that `dump_rdata_floats.py` is wrong — it was used only on `.rdata` and on the file-backed
  part of `.data` (`0x005f2dd8`, fileoff `0x1f2dd8`, well under `RawSize`). Those reads stand.
  The caution is about **extending** that method past the raw boundary.
- Not that every other `.data` address this session cited is file-backed. Only these four were
  checked. `0x005f2dd8` and `0x005cc*` were verified earlier by independent agreement with live
  Frida reads (`RESULT_LIMITTBL.md` §1, `RESULT_WITNESS.md:23`), which is stronger.
- No C-level. Nothing executed.

## 5. Next

1. Where a port needs an **initialised** value, check `offset < SizeOfRawData` first — and prefer
   corroboration by a live read, as the limit table and thresholds got.
2. The phase-3 clip handles cannot be shortcut. Sizing (b)/(c) in `SCOPE_SUBSTATE.md` must include
   the camera-path module's **writers**, confirming `RESULT_CLIPSRC.md` §4 for a better reason
   than it gave.
