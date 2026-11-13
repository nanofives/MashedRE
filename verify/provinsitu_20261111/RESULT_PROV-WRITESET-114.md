# RESULT PROV-WRITESET-114 (run 38, 2026-11-12): dynamic write-set leg for the 114 register-store bodies

Verdict: **GREEN for the tool and the table, no promotion made.** All 114 bodies have a row. 69 are `covered`, 45 are
`escapes`, 0 are `not-run`. Input: the 114 IDENTICAL-FOR-2724 bodies of `verify/provinsitu_20261111/PROV_VERDICTS.tsv`
whose static write-set leg was `not-covered`.

## Method

The DDK blobs of PROV-INSITU run in the original behind inline JMPs (`re/tools/prov/provlink.py`, `detorig.py --prov`).
New here, in `re/frida/js/detorig.js` (`wsInstall`) and `re/frida/detorig.py` (`--prov-wlog N`):

- Each body's stub single-steps (TF) its first N calls (N = 10, fewer where noted). A native vectored handler (CModule,
  no JS per step) logs `eip` + 8 registers before every instruction of the body, steps over calls into code outside the
  blob (int3 at the return address, re-armed on return), and ends the frame at the body's own return. Only the main
  thread is logged (the frame state is global). Record = 10 dwords, 40 B.
- Offline, `re/tools/prov/prov_writeset.py` decodes the instruction at each logged `eip` from the linked blob (with the
  load-time fixups applied), computes the effective address of every memory write from the logged registers (rep movs /
  stos = `[edi, edi + ecx*size)`), and classifies it against the frame's `base_esp` (esp at body entry):

| class | meaning |
|---|---|
| frame | `base_esp - 4 MB <= EA < base_esp`: the body's own frame and deeper |
| image | inside `0x005ea000..0x00915008`, outside the masked blocks: the per-tick hash compares it |
| image-masked | inside a block two unmodified runs already disagree on (`prov_compare` allowed blocks, ref_a/ref_b both orders) |
| caller-stack | `base_esp <= EA < base_esp + 4 MB`: return slot, arguments, caller frame |
| blob | the body's own blob |
| other | heap, other modules, exe code and `.rdata` |

Push / call / enter stores are the frame by construction and are not counted.
**Verdict:** `covered` = every observed store is frame or image. `escapes` = at least one caller-stack, image-masked,
blob or other store. `not-run` = no call stepped. This is the same covered rule as the static leg
(`prov_table.py`), applied to observed targets. The call is the lead's: whether a caller-stack store (an out-param the
caller then reads into the image) counts as covered is not decided here, the columns let it be re-cut.

## Result (`WRITESET_TABLE.tsv`, one row per body)

| verdict | bodies |
|---|---|
| covered | 69 (13 with no store outside the frame in the stepped calls, 20 frame only, 36 with image stores) |
| escapes | 45 |
| not-run | 0 |

Escapes by class: caller-stack only 22, other only (heap or non-image static) 11, caller-stack + other 7, image-masked
only 3, image-masked + other 1, image-masked + caller-stack + other 1. No body stores into its own blob.
Stores observed over all bodies: 61,644 (frame 37,779, image 16,163, image-masked 57, caller-stack 2,237, other 5,408),
664,221 logged steps. `WRITESET_SITES.tsv` lists every store site (blob offset, class, count). `detail` in the table
lists up to 12 image targets and 6 outside targets per body.

Image-masked bodies (store into a block the references already disagree on): `0x004967e0`, `0x005a8890`, `0x005a9e60`,
`0x005aa560`, `0x005a6ea0`.

## Limits

- **Calls stepped are the first N per body, counted from install at the menu**, so a body called often before the warp
  spends part of N there. Bodies that ran fewer than 10 calls in the whole run are listed by `calls_stepped` in the
  table (17 rows, same set as the thin rows of PROV-INSITU, plus `0x004c4eb0` at 3).
- **`0x004c4eb0`**: with N = 10 the game died with `0x80000004` (single step) before the warp, in
  `ws_s_0x004c4eb0`, cause not found. N = 1 and N = 3 ran clean, the row uses N = 3 (384 records, 3 frames). So its
  write-set covers 3 calls.
- **Group runs**: 4 of the 6 groups of 19 completed (`ws_g1a`, `ws_g1b`, `ws_g2a`, `ws_g3a`). `ws_g2b` and `ws_g3b` died
  with a single-step exception at the menu, so their 38 bodies were run one per process (`ws_s_<rva>`); 2 of those
  needed one re-run after an attach failure. `ws_g3b` contains `0x004c4eb0`. `ws_g2b` died as a group with every member
  clean alone, so a combination of stepped bodies (nested frames) is the suspect, not found. The table rows are from
  clean runs only.
- Stores by other threads are not logged (6 other-thread steps seen in `ws_g3a`, cleared without logging).
- Frame/caller classification uses a 4 MB window around `base_esp`. A store far from both lands in `other`.
- Three harness faults fixed on the way (all in the committed code): the vectored handler must be `__stdcall`, the
  int3 `Eip` is the int3 address itself on Windows, CModule globals are not writable (state is allocated memory).
- The runs are original-side replays with TF stepping. Per-tick identity of these runs against the reference was not
  re-scored here: this leg observes stores, the IDENTICAL-FOR-2724 verdicts are PROV-INSITU's.
- Not run, by the top-down rule: builds, `ddkobj.py check`, G-CTRL, B. No file under `mashedmod/` changed.

## For the promotion step (not done here)

Per L-128 the 114 need this leg plus the other legs already recorded in PROV-INSITU. Suggested cut: `covered` (69) is
the set whose observed stores are all inside the compared image or the frame. The 22 caller-stack-only bodies need a
decision on out-params. The 23 with `other` or `image-masked` stores (45 escapes minus the 22 caller-stack-only) write outside what the per-tick hash sees, so by
the covered rule they stay where they are.

## Evidence

Committed (this folder): `WRITESET_TABLE.tsv`, `WRITESET_SITES.tsv`, `WRITESET_RAW_SHA256.tsv` (SHA-256 of each cited
raw `wlog.bin` and `prov_install.json`), `run_ws.sh`, `ws_batch.sh`, `ws_singles.sh`, `ws_retry.sh`, `ws_4c4eb0.sh`,
`ws_group{1,2,3}.txt`, `ws_group{1a,1b,2a,2b,3a,3b}.txt`.
Tools: `re/tools/prov/prov_writeset.py`, `re/frida/js/detorig.js` (`wsInstall`), `re/frida/detorig.py` (`--prov-wlog`).
Raw (not committed): `D:/MashedWT/PROV-WRITESET-114_data/` (`ws_g1a`, `ws_g1b`, `ws_g2a`, `ws_g3a`, `ws_s_<rva>` x 38,
`ws_4c4eb0_n3`, plus failed and test runs `t3*`, `ws_g1`..`ws_g3`, `ws_4c4eb0_n1`), each with `wlog.bin`, `prov_install.json`,
`meta.json`. Private root `D:/MashedWT/PROV-WRITESET-114_root`.
Reproduce a decode:
`py -3.12 re/tools/prov/prov_writeset.py --bodies verify/provinsitu_20261111/bodies_all.json --run <dir> ... --allowed-ref D:/MashedWT/PROV-INSITU_data/ref_a,D:/MashedWT/PROV-INSITU_data/ref_b --rvas <list> --out out.tsv`
