# KV port-prep pack (2026-07-24)

**Scope & method.** Targets and sizes are from `re/analysis/b5e/island_vtable_targets.tsv`; group definitions and dispatch/install semantics from `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` ┬º2ÔÇô┬º3 (+ ┬º5 K13). hooks.csv status is the literal current row (`hooks.csv`); xtwin from `re/console/match/xbuild_match_v2.csv`; plate paths verified by Grep of `re/analysis/`. Per the tsv, `size_kind=defined` = a Ghidra-defined function (byte-accurate size); `size_kind=span-est` = Ghidra-**undefined**, size is a span-estimate upper bound ÔÇö these need a Ghidra `create-function` pass before porting (`B5e ┬º2`, "~69 Ghidra-undefined"). No xtwin in the whole set is `scaffold=ok-asc`; the three that exist are all `ambiguous` ÔåÆ **candidate-grade only** (per CLAUDE.md xtwin rule). "called?" = `island_called_slot` from the tsv (1 = the solver actually dispatches this slot; 0 = installed-but-not-reached-by-island).

Legend for cells: `?` = value absent in sources. Plate/xtwin `ÔÇö` = none found.

---

## KV1 ÔÇö scene callbacks (installed by scene ctor `FUN_0055f800`; dep: after K13, which is DONE)

Install semantics (`B5e ┬º2.3`, sites `0x55fd8e/9c/a6/b5/bf`): slot `+0x404 = 0x56b310` always; `+0xf4`/`+0xf8` = `0x56a450`/`0x56adb0` when `scene+0x58==2`, else `0x569140`/`0x5697f0`. Callback ABI frames (`p11` ├ù1, `p13` ├ù2 call-sites) were disasm-decoded in K13 and are fully specified in `re/analysis/b5e/K13_PORT_RECON_2026-07-18.md` (`B5e ┬º5`, sites `0x560a9c/0x560c5a/0x560d90`). Callees `0x56aae0`/`0x56ac40` named in `B5e ┬º3` KV1 row.

| RVA | approx size (tsv) | hooks.csv status | plate | xtwin (tier/scaffold) | prereq/order notes |
|---|---|---|---|---|---|
| 0x0056b310 | 1168 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x404` slot, installed unconditionally; `island_called=1`. **Discovery-first** (undefined). |
| 0x0056a450 | 835 (defined) | C1 mapped | `re/analysis/bucket_00565d50/0x0056a450.md` | ÔÇö | `+0xf4` when scene+0x58==2; `called=1` |
| 0x0056adb0 | 1369 (defined) | C1 mapped | `re/analysis/bucket_00565d50/0x0056adb0.md` | ÔÇö | `+0xf8` when scene+0x58==2; `called=1` |
| 0x00569140 | 1712 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0xf4` else-branch; `called=1`. **Discovery-first**. |
| 0x005697f0 | 3168 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0xf8` else-branch; `called=1`. **Discovery-first**. |
| 0x0056aae0 | ? (not in tsv) | C1 mapped | `re/analysis/bucket_00565d50/0x0056aae0.md` | ÔÇö | callee (`B5e ┬º3` KV1); size not stated |
| 0x0056ac40 | ? (not in tsv) | C1 mapped | `re/analysis/bucket_00565d50/0x0056ac40.md` | ÔÇö | callee (`B5e ┬º3` KV1); size not stated |

---

## KV2 ÔÇö body object table `0x0062403c` (installed by body-creator `FUN_0057c300` @`0x0057c31a`; dep: any)

`B5e ┬º2.2`: slot `+0x1c` is runtime-overridden from its static value `0x57c5a0` to `0x57c2b0` by registrar `FUN_0057c270` @`0x0057c31a`/plugin offset `DAT_007dc8d8`. Resolves the `FUN_0055a1f0`/`0055e050`/`00561390` `obj[0]` dispatches (ÔåÆ `0x57c3a0`/`0x57c3f0`/`0x57c590`/`0x57c2b0`). `rwpOBJTYPERAGDOLL` @`0x632b68` is never installed (0 refs) ÔÇö no port.

| RVA | approx size (tsv) | hooks.csv status | plate | xtwin (tier/scaffold) | prereq/order notes |
|---|---|---|---|---|---|
| 0x0057c3a0 | 80 (span-est) | *(no row)* | ÔÇö | ÔÇö | slot `+0x10`; `called=1`. **Discovery-first**. |
| 0x0057c3f0 | 48 (span-est) | *(no row)* | ÔÇö | ÔÇö | slot `+0x14`; `called=1`. **Discovery-first**. |
| 0x0057c590 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | slot `+0x18`; `called=1`. **Discovery-first**. |
| 0x0057c5a0 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | slot `+0x1c` **static**; `called=1`. Overridden at runtime ÔåÆ also port 0x57c2b0. **Discovery-first**. |
| 0x0057c2b0 | ? (not in tsv) | C1 mapped | `re/analysis/bucket_0057bf30/0x0057c2b0.md` | `0x000b6d00` mnem `byte-full` **candidate-grade** | runtime override target for `+0x1c`; 0x54-byte struct, refcount@+0x50 (plate) |
| 0x0057c270 | ? (registrar) | C1 new | `re/analysis/rw_engine_init_d2_cont1_b/0057c270.md` | `0x000b6db0` `mnem`/`ambiguous` **candidate-grade** | registrar; writes fn-ptr `0x57c2b0` to `PTR_LAB_00624058`; `DAT_007dc8d8` |

---

## KV3 ÔÇö volume-descriptor slot fns (7 live tables; Grid `0x5e54c0` & Aggregate `0x5e5900` are dead, 0 refs, no port ÔÇö `B5e ┬º2.1`; dep: any)

Shared leaves reused across tables (port **once**): **`0x004d8560`** (`+0x04` in Sphere/Box/Cylinder/Capsule/Triangle/Null + Sphere `+0x1c`) ÔÇö already **C3** (`re/analysis/timer_d3_cont2/0x004d8560.md`; "returns literal 1"); **`0x005c9d00`** (2-byte `XOR EAX,EAX;RET`, Null ├ù6 + Cylinder/Trilist slots) ÔÇö **C2** `GetRaceEndTrigger`, and its row carries a live caveat: *the 5-byte JMP overruns 3 bytes past the 2-byte body ÔåÆ must use a trampoline / 2-byte patch* (hooks.csv line 423); **`0x0057b9a0`** (16 B, `+0x24` in Sphere/Capsule/Triangle/Null); **`0x00562a10`** (16 B, Trilist+Grid `+0x14`).

### KV3 ┬À Sphere `0x5e4f50`
| RVA | size (tsv) | hooks.csv | plate | xtwin | notes |
|---|---|---|---|---|---|
| 0x004d8560 | 5 (defined) | **C3** impl | `re/analysis/timer_d3_cont2/0x004d8560.md` | `0x000baad0` `prop-weak`/`ambiguous` candidate | shared `+0x04`,`+0x1c`; already done |
| 0x0055c560 | 32 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x08`; `called=1`; discovery-first |
| 0x0055c580 | 80 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x0c`; `called=0` |
| 0x0055c5d0 | 160 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x10`; `called=1`; discovery-first |
| 0x0055c670 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x14`; `called=1` |
| 0x0055c680 | 64 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x18`; `called=1` |
| 0x0055c6c0 | 32 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x20`; `called=1` |
| 0x0055c6e0 | 304 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x28`; `called=0` |
| 0x0057b9a0 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | shared `+0x24`; `called=1` |

### KV3 ┬À Box `0x5e4fe0` (install site `0x0055c815` in `FUN_0055c810`, `B5e ┬º2.1`)
| RVA | size (tsv) | hooks.csv | plate | xtwin | notes |
|---|---|---|---|---|---|
| 0x004d8560 | 5 (defined) | **C3** impl | (shared, see above) | candidate | `+0x04`; `called=0` |
| 0x0055c840 | 80 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x08`; `called=1` |
| 0x0055c890 | 864 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x0c`; `called=0` |
| 0x0055cbf0 | 336 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x10`; `called=1` |
| 0x0055cd40 | 129 (defined) | C1 mapped | `re/analysis/bucket_00557fb0/0x0055cd40.md` | ÔÇö | `+0x14`; `called=1` |
| 0x0055cdd0 | 992 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x18`; `called=1` |
| 0x0055d1b0 | 192 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x1c`; `called=1` |
| 0x0055d270 | 112 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x24`; `called=1` |
| 0x0055d2e0 | 80 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x20`; `called=1` |
| 0x0055d330 | 352 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x28`; `called=0` |

### KV3 ┬À Cylinder `0x5e51c8`
| RVA | size (tsv) | hooks.csv | plate | xtwin | notes |
|---|---|---|---|---|---|
| 0x004d8560 | 5 (defined) | **C3** impl | (shared) | candidate | `+0x04`; `called=0` |
| 0x0055e470 | 48 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x08`; `called=1` |
| 0x0055e4a0 | 112 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x0c`; `called=0` |
| 0x0055e510 | 416 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x10`; `called=1` |
| 0x0055e6b0 | 320 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x14`; `called=1` |
| 0x0055e7f0 | 928 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x18`; `called=1` |
| 0x0055eb90 | 48 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x24`; `called=1` |
| 0x0055ebc0 | 96 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x20`; `called=1` |
| 0x0055ec20 | **3040** (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x28`; `called=0`; largest KV3 leaf |
| 0x005c9d00 | 2 (defined) | **C2** impl | `re/analysis/game_state_d2/0x005c9d00.md` | ÔÇö | shared `+0x1c`; `called=1`; trampoline caveat |

### KV3 ┬À Trilist `0x5e52a8`
| RVA | size (tsv) | hooks.csv | plate | xtwin | notes |
|---|---|---|---|---|---|
| 0x00562640 | 32 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x04`; `called=0` |
| 0x00562660 | 269 (defined) | C1 mapped | `re/analysis/bucket_00554010/0x00562660.md` | ÔÇö | `+0x08`; `called=1` |
| 0x00562770 | 304 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x0c`; `called=0` |
| 0x005628a0 | 368 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x10`; `called=1` |
| 0x00562a10 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | shared `+0x14` (Trilist+Grid); `called=1` |
| 0x00562a20 | 64 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x24`; `called=1` |
| 0x00562a60 | 1392 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x20`; `called=1` |
| 0x005c9d00 | 2 (defined) | **C2** impl | (shared) | ÔÇö | `+0x18`,`+0x1c` (called=1), `+0x28` (called=0) |

### KV3 ┬À Capsule `0x5e5338`
| RVA | size (tsv) | hooks.csv | plate | xtwin | notes |
|---|---|---|---|---|---|
| 0x004d8560 | 5 (defined) | **C3** impl | (shared) | candidate | `+0x04`; `called=0` |
| 0x00563000 | 32 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x08`; `called=1` |
| 0x00563020 | 112 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x0c`; `called=0` |
| 0x00563090 | 176 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x10`; `called=1` |
| 0x00563140 | 64 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x14`; `called=1` |
| 0x00563180 | 240 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x18`; `called=1` |
| 0x00563270 | 48 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x1c`; `called=1` |
| 0x005632a0 | 48 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x20`; `called=1` |
| 0x005632d0 | 272 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x28`; `called=0` |
| 0x0057b9a0 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | shared `+0x24`; `called=1` |

### KV3 ┬À Triangle `0x5e5db0` (installed island-internally by `FUN_00575fe0` @`0x00576344`, `B5e ┬º2.1`)
| RVA | size (tsv) | hooks.csv | plate | xtwin | notes |
|---|---|---|---|---|---|
| 0x004d8560 | 5 (defined) | **C3** impl | (shared) | candidate | `+0x04`; `called=0` |
| 0x0057ae30 | 347 (defined) | C1 mapped | `re/analysis/bucket_00565cd0/0x0057ae30.md` | ÔÇö | `+0x08`; `called=1` |
| 0x0057af90 | 112 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x0c`; `called=0` |
| 0x0057b000 | 832 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x10`; `called=1` |
| 0x0057b340 | 224 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x14`; `called=1` |
| 0x0057b420 | 816 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x18`; `called=1` |
| 0x0057b750 | 592 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x1c`; `called=1` |
| 0x0057b9a0 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | shared `+0x24`; `called=1` |
| 0x0057b9b0 | 224 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x20`; `called=1` |
| 0x0057ba90 | 208 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x28`; `called=0` |

### KV3 ┬À Null `0x5e5e50`
| RVA | size (tsv) | hooks.csv | plate | xtwin | notes |
|---|---|---|---|---|---|
| 0x004d8560 | 5 (defined) | **C3** impl | (shared) | candidate | `+0x04`; `called=0` |
| 0x005c9d00 | 2 (defined) | **C2** impl | (shared) | ÔÇö | `+0x08/+0x10/+0x14/+0x18/+0x1c/+0x20` (called=1), `+0x28` (called=0); trampoline caveat |
| 0x0057b9a0 | 16 (span-est) | *(no row)* | ÔÇö | ÔÇö | shared `+0x24`; `called=1` |
| 0x0057c1d0 | 64 (span-est) | *(no row)* | ÔÇö | ÔÇö | `+0x0c`; `called=0` |

---

## Suggested port order + session batching (6 sessions)

All KV1ÔÇôKV3 are **not** needed for per-cluster `.asi` A/B (ported island code reads the real tables by absolute address; dispatch lands in original code) ÔÇö they are the **lane-end standalone-truth** surface (`B5e ┬º3`, lines 127-129). No KV group has a forward dependency except KV1 (dep "after K13", which is DONE); KV2/KV3 are "any". Sizing targets the K-cluster session norm (~2.5ÔÇô7 KB / session per `B5e ┬º4-5`). Within each session, do the `island_called=1` slots first (they carry lane-end correctness; `called=0` slots are installed-but-unreached and can slip to a tail pass if a session overruns).

1. **Batch A ÔÇö shared leaves + KV2 + Null (~0.7 KB new).** Confirm/reuse `0x004d8560` (C3, done) and `0x005c9d00` (C2 ÔÇö apply the trampoline/2-byte-patch fix from its hooks.csv caveat), port `0x0057b9a0` + `0x00562a10`, then the KV2 body table (`0x57c3a0/f0/590/5a0` + override `0x57c2b0` + registrar `0x57c270`) and the tiny Null table (`0x57c1d0`). *Rationale: unblocks every shared stub the other tables reuse; smallest surface; KV2 already part-plated.*
2. **Batch B ÔÇö KV1 scene callbacks (~8.2 KB, 5 install targets + 2 callees).** `0x56b310`, `0x56a450`/`0x56adb0`, `0x569140`/`0x5697f0`, callees `0x56aae0`/`0x56ac40`. *Rationale: dep (K13) satisfied; the 3 callback ABI frames are already disasm-decoded in `K13_PORT_RECON_2026-07-18.md`; self-contained scene-ctor family. Largest single batch ÔÇö if it overruns, split at the `scene+0x58==2` branch (a450/adb0 vs 569140/5697f0).*
3. **Batch C ÔÇö Sphere + Capsule (~1.7 KB, ~15 new fns).** *Rationale: the two smallest volume tables; both reuse the Batch-A shared leaves.*
4. **Batch D ÔÇö Trilist + Box (~5.6 KB).** *Rationale: mid-size pair; Trilist `00562660` and Box `0055cd40` are already Ghidra-defined, reducing discovery load.*
5. **Batch E ÔÇö Cylinder (~5.0 KB, incl. the 3,040 B `0x0055ec20`).** *Rationale: dominated by one large `called=0` leaf ÔÇö isolate so its discovery+port doesn't crowd out another table.*
6. **Batch F ÔÇö Triangle (~3.4 KB).** *Rationale: installed island-internally (`FUN_00575fe0`); anchor `0x0057ae30` is already defined+plated; natural standalone unit.*

---

## Open gaps ÔÇö no hooks.csv row, no plate, no trustworthy xtwin ÔåÆ **Ghidra discovery first**

Every one below is `size_kind=span-est` in the tsv (Ghidra-**undefined**; size is an upper-bound estimate) and has no per-RVA analysis plate and no `ok-asc` twin ÔÇö so each needs a Ghidra `create-function` + decomp pass before it can be transcribed:

- **KV1 (3):** `0x00569140`, `0x005697f0`, `0x0056b310`
- **KV2 (4):** `0x0057c3a0`, `0x0057c3f0`, `0x0057c590`, `0x0057c5a0`
- **KV3 Sphere (7):** `0x0055c560`, `0x0055c580`, `0x0055c5d0`, `0x0055c670`, `0x0055c680`, `0x0055c6c0`, `0x0055c6e0`
- **KV3 Box (7):** `0x0055c840`, `0x0055c890`, `0x0055cbf0`, `0x0055cdd0`, `0x0055d1b0`, `0x0055d270`, `0x0055d2e0`, `0x0055d330` *(8)*
- **KV3 Cylinder (8):** `0x0055e470`, `0x0055e4a0`, `0x0055e510`, `0x0055e6b0`, `0x0055e7f0`, `0x0055eb90`, `0x0055ebc0`, `0x0055ec20`
- **KV3 Trilist (5):** `0x00562640`, `0x00562770`, `0x005628a0`, `0x00562a10`, `0x00562a20`, `0x00562a60` *(6)*
- **KV3 Capsule (8):** `0x00563000`, `0x00563020`, `0x00563090`, `0x00563140`, `0x00563180`, `0x00563270`, `0x005632a0`, `0x005632d0`
- **KV3 Triangle (7):** `0x0057af90`, `0x0057b000`, `0x0057b340`, `0x0057b420`, `0x0057b750`, `0x0057b9b0`, `0x0057ba90`
- **KV3 shared / Null (2):** `0x0057b9a0` (shared `+0x24`, 4 tables), `0x0057c1d0` (Null `+0x0c`)

**Already-covered (not gaps), for contrast:** `0x004d8560` (C3, plated), `0x005c9d00` (C2, plated ÔÇö needs the trampoline fix), `0x0055cd40` / `0x00562660` / `0x0057ae30` / `0x0056a450` / `0x0056adb0` / `0x0056aae0` / `0x0056ac40` (C1, plated), `0x0057c2b0` / `0x0057c270` (C1, plated; candidate-grade twins only).

**xtwin note:** only three targets have any Xbox twin ÔÇö `0x004d8560`ÔåÆ`0x000baad0` (prop-weak/ambiguous), `0x0057c270`ÔåÆ`0x000b6db0` (mnem/ambiguous), `0x0057c2b0`ÔåÆ`0x000b6d00` (byte-full/ambiguous). All are `ambiguous`, none `ok-asc`, so per the CLAUDE.md rule none is a trustworthy reading aid; confirm before relying on any pair.
