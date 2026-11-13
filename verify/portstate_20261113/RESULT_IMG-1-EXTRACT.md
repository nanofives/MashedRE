# RESULT IMG-1-EXTRACT: image extractor and offline image check (L-149 a, step 1)

Row `IMG-1-EXTRACT` (D4-IMG, run 44). Branch `orch/IMG-1-EXTRACT` from `orch/integration` @ `e47e10795`.
Worker cmwfa18ls1lzalh1nngyrpzmq, lead cmwf9zx6d1ljulh1nd88fk6k9. Spec: `verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md`
sections 1, 2 and 5 (IMG-1). Row kind: tool row. No build, no game run, no gates.

## Verdict: GREEN

`image_check.py` on the extracted file: format OK, **392 of 392** seed entries byte-equal to the image, 0 overlaps with
the IAT range 0x005cc000..0x005cc2e8, 188 of 188 CP_FIX slots hold their original target in the image, both B16
constants equal. Every safety refusal tested fires (below). Nothing in the real `original\` was written.

## Files

| File | Content |
|---|---|
| `re/tools/image/mashed_image.py` | format v1 (spec section 2): build, parse/validate, FNV-1a 32, expected section table, IAT range |
| `re/tools/image/extract_image.py` | `--exe <MASHED.exe.unpatched> --out <path> [--replace]`: anchor check, section-table check, writes the file |
| `re/tools/image/image_check.py` | `--image <path> [--src <mashed_re root>]`: format check, then the four seed tables against the image |
| `verify/portstate_20261113/img1_extract.txt` | extractor output |
| `verify/portstate_20261113/img1_check.txt` | image_check output |
| `verify/portstate_20261113/img1_negative.txt` | safety refusals and the untouched-`original\` check |
| `verify/portstate_20261113/img1_repeat.txt` | second extraction to a fresh path, same SHA-256 |

## The staged file (not committed: `original\` is gitignored)

```
D:\MashedWT\IMG-1-EXTRACT_root\original\mashed_image.bin  size 434400  sha256 81CC77758FFE93AC0B681EB1FED3D63912D25E7D059503AF6D4F871DEA9AFCD8
source  original\MASHED.exe.unpatched  size 2846720  sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (read only)
range .rdata   va 0x005cc000 len 0x01d044 (118852) fnv1a32 0xee647e46
range .data    va 0x005ea000 len 0x04d000 (315392) fnv1a32 0x935b1e30
range _rwdseg  va 0x00915000 len 0x000008 (8)      fnv1a32 0x9be17165
payload bytes 434252
```

434,400 = 0x2c header + 3 x 24 range records + 434,252 payload + 32 trailer. The output is deterministic (no
timestamp): a second extraction to a fresh path gave the same SHA-256 (`img1_repeat.txt`). Anyone can re-stage it with
`py -3.12 re/tools/image/extract_image.py --exe original/MASHED.exe.unpatched --out <root>/original/mashed_image.bin`.

## Self-check (verbatim, `img1_check.txt`)

```
image D:/MashedWT/IMG-1-EXTRACT_root/original/mashed_image.bin size 434400 source sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E format OK (magic, version, trailer sha256, 3 range fnv)
PortStateSeed.inc: entries 272 bytes 3231 equal 272 of 272 by range {'.rdata': 147, '.data': 125}
PortTop_RdataSeed.inc: entries 46 bytes 767 equal 46 of 46 by range {'.rdata': 46}
PortTop_DInputSeed.inc: entries 2 bytes 32 equal 2 of 2 by range {'.rdata': 2}
PortTopCodePtr.inc CP_SEED: entries 72 bytes 288 equal 72 of 72 by range {'.data': 52, '.rdata': 20}
PortTopCodePtr.inc CP_FIX: slots 188, image dword equals the original target 188 of 188
B16 const 0x005cd5a8 image 0x3acccccd expected 0x3acccccd equal True
B16 const 0x005cc560 image 0x3b088889 expected 0x3b088889 equal True
TOTAL seed entries equal 392 of 392, IAT overlaps 0 -> PASS
```

Count note: the plan's "387/387" counted `PortTop_RdataSeed.inc` at 41 entries. The tree at `e47e10795` has **46**
(767 bytes), so the total is 392. All 46 are equal. The check reads the tables live, so it follows future growth.

## Safety rules and their tests (`img1_negative.txt`)

| Rule | Test | Result |
|---|---|---|
| exe must be the anchor (SHA-256 and size) | `--exe original/MASHED.exe` (patched) | `REFUSED: exe is not the anchor`, exit 2, nothing written |
| section table must equal spec section 1 | checked in `mashed_image.build` | passes on the anchor (no negative input exists that has the anchor hash) |
| never overwrite an existing output | rerun onto the staged file | `REFUSED: ... exists`, exit 2 |
| never `--replace` inside a folder named `original` | `--replace` onto the staged file | `REFUSED: --replace of an existing file inside an "original" folder`, exit 2 |
| output may not be the input | `--out original/MASHED.exe.unpatched` | `REFUSED: output is the input file`, exit 1 |
| corrupt image detected | one payload byte flipped | `FORMAT FAIL: trailer sha256 mismatch`, exit 1 |
| real `original\` untouched | `ls original/mashed_image.bin`, `sha256sum original/MASHED.exe.unpatched` | no such file; anchor hash unchanged |

The exe is opened read-only (`open(exe, 'rb')`). A new file is created with exclusive create (`open(out, 'xb')`), so a
file that appears between the check and the write still is not overwritten.

## Not in this row (from the IMG-1 line of the plan)

- The `stage_lan_release.py` hook (place `original\mashed_image.bin` in the release stage and manifest) and the
  `build.bat` post-link step. Not done: the release would ship a 434 KB file that no loader reads until IMG-2, and
  `build.bat` edits belong with the loader row. Both are a few lines on top of `extract_image.py`.
- Loader, trap, Ghidra CP export: IMG-2, IMG-3, IMG-4.

## Next

IMG-2: loader behind `MASHED_PORTSTATE_IMAGE=1` on the `MASHED_ORIG_TOP` branch, reading the file in the order of
spec section 2 (`MASHED_IMAGE`, `<exe dir>`, `<root>\original\`), with the memory-side FNV witness `mem=5ecb7d52`.
The stage and build hooks can land with it.
