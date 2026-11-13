# RESULT RWC-D3DX9-LINK (2026-11-13, L-142)

**Verdict: GREEN (design only).** No code, no build, no game run, no gate. Text only, so no object build, ddkobj check or
knob boot was needed. Branch `orch/RWC-D3DX9-LINK` from `orch/integration` @ 145dec4d0.

## 1. The one executed row: 0x00514330 is RtPNGImageRead, not d3dx9

- hooks.csv row 0x00514330 is `FUN_00514330`, `third-party-library[d3dx9-backward-extension]`, C2. The note is a band label
  only ("library_residue band=d3dx9-backward-extension span=0x005115aa..0x00516a71", C0 band classification, no body read).
- The body, as recorded in `mashedmod/src/mashed_re/LibRw/RwApiBind.cpp:839-841`: calls 0x004cc230(2, 1, name) at
  0x0051434b (the RwStream open, read mode), then 0x005173f0 with the string "1.0.12" (string at 0x00620e1c). "1.0.12" is
  the libpng version the image carries (hooks.csv rows `third-party-library[libpng-1.0.12]`, e.g. 0x0051f350, 0x00520850,
  0x00520960). The sibling writer 0x00513f90 does the same with 0x00514d00 and the same string.
- libmatch against the Summer 2003 d3dx9.lib paired no function: `verify/c4rdxlib_20261107/d3dx_sum2003.tsv` row 372
  `not-compared ... no library function paired`, `verdicts.tsv` `nopair`, the batch refused it (`dryrun_output.txt` line 371).
- Verdict: **not d3dx9.** It is the RenderWare Rt PNG reader (RtPNGImageRead, bound as `MashedRw_RtPNGImageRead_00514330`)
  over libpng 1.0.12. The d3dx label is the address-band relabel. Refile by class P2 (L-127, identity evidence). RESULT_RW-SIZING
  already marks the label [UNCERTAIN].
- RW-SIZING says 5 more executed d3dx-labelled rows are called from RtPNG / libpng code. Same band mislabel expected.
  [UNCERTAIN] their identities: not read here.
- Consequence: once refiled, the closure has 0 executed d3dx9 rows, consistent with RW-SIZING "no executed RW function calls
  d3dx (0 direct edges)".

## 2. Where the library is

- `C:\Users\maria\Desktop\Proyectos\Mashed\tools\dxsdk9\sum2003\Lib\d3dx9.lib` (main checkout, gitignored per L-94, absent
  from worktrees, so use the absolute path). DirectX 9.0b SDK Summer 2003, 5,534,754 B, SHA-256 `9496d4ce...6099ed56`
  (`tools/dxsdk9/SHA256SUMS.txt`), 228 i386 COFF members, 6,025 functions. It is a static library: RW-SIZING's `d3dx_link_test`
  linked it into a VS2022 /MT exe and ran it (`0.877583 -0.479426 0.267261`), with
  `d3dx9.lib d3d9.lib user32.lib gdi32.lib advapi32.lib ole32.lib winmm.lib legacy_stdio_definitions.lib`.
- Summer 2003 is the right release: 219 unique identical rows against 31 for Summer 2004 (C4R-DXLIB section 1), and
  `D3D_SDK_VERSION` 31 at 0x004c7e02 is 9.0b.

## 3. Closure rows and objects

The 330 rows (RW_CLOSURE_REVIEW section 1) are 294 labelled d3dx9 plus 36 with no census row between d3dx9 neighbours.
libmatch (`d3dx_sum2003.tsv`) over the 397 d3dx9-subsystem hooks.csv rows:

| status | rows | meaning for the link |
|---|---|---|
| identical, one candidate | 219 | the member's function is the original's bytes (C4 libid) |
| identical, ambiguous | 26 | same bytes as several functions (sse_/sse2_ twins, `??_G` destructors) |
| differs | 121 | nearest member known (mnemonic similarity 0.9 and up), bytes differ by constant, register or displacement |
| not-compared | 31 | no library function paired (includes 0x00514330) |

The 219 identical rows sit in **32 members** (d3dxmath.obj 89, ctokenize.obj 20, cassembler.obj 20, d3dxmathsse.obj 8,
cnode.obj 8, x3d_quat/x3d_matx/x3d_vec3 7 each, cpreprocessor.obj 6, ccommentblock.obj 5, d3dxmathx86.obj 4, cfile.obj 4,
and 20 more). Every row with a candidate (identical, ambiguous, differs) touches **57 members**: that is the link set. The
other 171 members stay out. The 31 not-compared rows have no member and need a read each.

## 4. How the object build links them

1. **Completeness needs no link.** The linker pulls a member only for an undefined external. No wrapper or ORIG_TOP row
   references a `_D3DX*` or `c_D3DX*` name (0 executed edges), and /OPT:REF would drop them anyway. The closure disposition
   is "declared library boundary (L-142): vendor object, not matched, not run".
2. **First consumer.** When a row calls d3dx9 (for example the 19 `jmp [pfn]` dispatch stubs 0x004ecafb
   `_D3DXVec2Transform@12`, 0x004ed6ba `_D3DXMatrixMultiply@12`, 0x0050f1bf `_x3d_D3DXVec3TransformCoord@12`), bind by the
   lib's own exported name. In `re/tools/ddkobj/ddkobj.py` the variant link response is the default `link.rsp` plus the DDK
   objects and `ddk_abs.obj` (docstring step 4): add `/LIBPATH:<main>\tools\dxsdk9\sum2003\Lib d3dx9.lib` after the object
   list. `mashedmod/build.bat` (line 267) and the default build stay untouched. The existing `/NODEFAULTLIB:libc.lib` stays,
   and the link test showed the lib links under /MT, so no new CRT conflict is expected.
3. **Global data.** Lib members own their `.data`/`.bss`. A row that reads them at original addresses needs the
   `__ddkabs_` absolute-symbol path. The non-executed rows do not, so it is out of scope until one runs.
4. **ddkobj check.** It byte-checks substituted matched sources. Vendor members are not substituted units, so they add no
   check cases. Their identity claim is libid (219 rows already C4). The 121 differs rows carry no byte claim: the boundary
   claim is same source, other flags or point release, safe because none run.

## 5. Open items for the lead

- Refile 0x00514330 to the RenderWare (rtpng) side (P2) and re-census: d3dx9 closure 330 to 329, executed 0. The
  re-classify leg is a tracker write, not done here.
- Check the 5 other executed d3dx-labelled rows named in RW-SIZING before the closure count is final.
- The 36 unlabelled and 31 not-compared rows (0x004fbe1b, 0x004fd0d3, 0x004fd6a2, ...) need hooks.csv rows (159 addresses
  have none, C4R-DXLIB section 4) so libid can cover them.
- No wiring was committed: no row calls d3dx9, so a link line with no consumer changes nothing and cannot be verified. The
  one-line `ddkobj.py` edit in section 4 item 2 belongs to the first row that needs it.
