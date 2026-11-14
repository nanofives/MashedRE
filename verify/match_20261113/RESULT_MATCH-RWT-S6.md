# MATCH-RWT-S6: 0x004ce970 byte-identical (GREEN)

Row kind: match-only (no build, no gates). Branch `orch/MATCH-RWT-S6` from `orch/integration` `ba39eeb6b`.

| RVA | source | before | after | bytes | relocheck |
|---|---|---|---|---|---|
| 0x004ce970 | `re/match/s20/004ce970.c` (header `/O2 /Ob1 /G5`, tc ddk) | 56.2% (MATCH-RWT-S4, semantically fixed body) | **100.0%** | 375 B, 0 differing unmasked bytes, ddk and rw9044 | OK, checked=22, unchecked=0 |

Source SHA-256 (working tree, LF): `6d25f8084e15fe9b933059605e40d13944cddb4656decfc4bd6b5406da5c086a`.
Plain C body. No inline asm, no naked function. The image destroy stays the S4 `IMAGEDESTROY` macro (same expansion
as the standalone destroy 0x004cdd00, without its `return 1`).

## Lever

The two failure tests are separate statements, each with its own copy of the failure body:

```c
mask = FUN_004ce390(param_2);
if (!mask) { IMAGEDESTROY(image); return 0; }
if (!FUN_004cde50(mask)) { IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0; }
if (!FUN_004cdf20(image, mask)) { IMAGEDESTROY(image); IMAGEDESTROY(mask); return 0; }
IMAGEDESTROY(mask);
```

inside the nested `if (image) { if (param_2 && *param_2) { ... } return image; } return 0;` form. Effects, each read
off the compiled listing against the original:

- **Register roles.** The duplicated failure body adds uses of `image`. c2 gives the heavier variable esi: image = esi
  (`mov esi,eax` at 0x004ce97d), mask = edi (0x004ce9a6). Diagnostic (`d1.txt`): removing the success-path mask
  destroy also flips image to esi, removing image destroys keeps it in edi. Every S4 form had one failure body and
  image in edi.
- **Block order.** c2 merges the two failure copies into one block that falls through after `jne 0x004cea99`
  (0x004ce9fd) and places the success path last, sharing `mov eax,esi` at 0x004ceae1 with the no-mask-name exit.
  With `make && apply` in one condition (S4 forms, T1 to T5 here) the success path came first.
- **Cross-jump.** With the original register roles and order the mask == NULL tail matches the failure path's mask
  freelist tail, and c2 emits `push esi; jmp 0x004cea7b` at 0x004ce9de. No scheduling lever was needed.

## Search (probe folder `verify/match_20261113/MATCH-RWT-S6_probes/`)

`exp.py` compiles a variants file in parallel with `match.compare` and prints score, byte count, the first-call
register and the jump-shape signature. `gen1.py` (`v2.txt`, `v2_ddk.log`, 140 variants) and `gen2.py` (`v3.txt`,
`v3_ddk.log`, 105 variants) cross outer form x mask-null form x try form.

| variant | form | score | bytes bad | image reg |
|---|---|---|---|---|
| `O1M1Td` (committed) | nested, `if (!mask)` first, separate make / apply tests, fall through to `return image` | **100.0** | 0 | esi |
| `O2M1Td` | early returns, same try form | **100.0** | 0 | esi |
| `O4M1Td` | no `return 0` after the outer `if`, `return image` at the end | 98.4 | 5 | esi |
| `O5M1Td` | `if (!image) return 0;` then nested mask test | 78.6 | 264 | esi |
| `O2M3Ta` | mask-null in `else`, nested make / apply with two failure copies | 66.4 | 264 | esi |
| `O1M1Tc` | make in `if`, apply nested, failure in both arms | 65.5 | 301 | esi |
| `O1M1T4` / `T5` | one `make && apply` condition, fall through | 49.6 | 276 | edi |
| `O1M1T1` | `if (!make \|\| !apply) { fail }` then success | 41.6 | 220 | edi |
| `O1M3Td` / `O1M5Td` | mask-null in `else` with the separate tests | 44.0 / 46.9 | 241 / 272 | esi |

`/Ob2` with a real `ImgDestroy` function in the TU (`v1.txt` h3 to h5): not inlined, 21.3 / 15.7. Not used.

## Checks (run on the branch)

```
py -3.12 re/tools/match/match.py --bytes re/match/s20/004ce970.c
  0x004ce970 _FUN_004ce970: 100.0% (regs 100.0%, ops 100.0%)  127/127 ins  flags=/O2 /Ob1 /G5 tc=ddk
  bytes: 375 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/match.py --bytes --toolchain rw9044 re/match/s20/004ce970.c
  0x004ce970 _FUN_004ce970: 100.0% (regs 100.0%, ops 100.0%)  127/127 ins  flags=/O2 /Ob1 /G5 tc=rw9044
  bytes: 375 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/s20/004ce970.c
  0x004ce970	OK	checked=22	unchecked=0	004ce970.c
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S6.tsv      (dry run, no --apply)
  [1/1] 0x004ce970 match PASS byte-identical 375 B, relocheck OK (22/0)
  batch: 1 pass, 0 refused, 0 skipped (already C4), 1 rows  [by route: match=1]
  dry run: nothing written.
```

VS2022 `cl /nologo /Zs /TP /W3 /DMASHED_STANDALONE /std:c++17` in `mashedmod/src/mashed_re/RwCore/` (after vcvars32),
exit 0 for `RwCore_004ce970.cpp` (includes the source) and `RwCore_004c6750.cpp` (calls it). No build run (match-only
row).

Behaviour is the S4 semantics unchanged: NULL image -> 0, no mask name -> image, NULL mask -> destroy image, 0, make or
apply fails -> destroy image then mask, 0 (0x004cea03 then 0x004cea48), success -> destroy mask, image.

Port note: the body runs on the RW core knob path as a listed provisional body
(`verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`, per the TU header). Moving it off that list after the merge is a
port-axis step, not done here.

Promotion TSV: `re/promotions/MATCH-RWT-S6.tsv` (1 row, route match), not applied on the branch.
