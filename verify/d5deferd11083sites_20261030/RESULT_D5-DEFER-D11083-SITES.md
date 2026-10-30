# RESULT D5-DEFER-D11083-SITES (static, 2026-10-30)

Verdict: GREEN. All 16 call sites and both DATA refs now have an owner function, each cited by RVA.
Neither group is unreachable, so no close is proposed.
- **D-11083 stays OPEN.** Two of its members have a static path from an executed function:
  - `0x0057ae30` through Triangle slot +0xc (`0x0055bbf0`, executed). Already known from D5-DEFER-D11083.
  - `0x0057bf30` through Triangle slot +0x28. **New:** dispatcher `0x0055bde0` is called from `0x00459620` at `0x00459fae`, and `0x00459620` is called from `0x004568d0` at `0x004569c5`. `0x004568d0` is executed (census race=1, race_run=1).
- **D-11084 (part `0x0057c0d0`) stays OPEN.** Its 6 sites sit in the same owner `0x0057ba90` (Triangle +0x28), so they reach the same executed `0x004568d0`.

Gates: not run (static only, no build, no game run, no test slot).

## Method
- `re/tools/decomp_pc.py --create --callers --xrefs --callees` ran on read-only pool slot `Mashed_pool0` (released after each run, and no `Mashed_pool0.lock` remains). Nothing was persisted. The project was not the master.
  - Run 1 created functions at the 16 site addresses. Each came back `exact_entry: false` and started at the site itself. For example `FUN_0057bd22` (523 B) and `thunk_FUN_00578f20` at `0x0057422b`. Run 1 therefore cannot find owners: `--create` at a mid-function address makes a stub that starts there.
  - Run 2 created functions at the 8 candidate owner entries. Each entry has exactly one ref, a DATA ref from a class-table slot, and its body covers its sites (table below).
  - Run 3 collected callers and xrefs for the 9 members plus the dispatchers.
  - Run 4 was an upward walk from `0x0055bde0` (`upwalk.py`), batched per level, which stops at census-executed functions.
- Byte-level checks used `original/MASHED.exe.unpatched` (anchor file) with pefile and capstone:
  - the 4 bytes before each owner entry
  - straight-line disassembly from each entry to its site, listing every `j*` / `ret` / `loop` that comes before the site
  - an unaligned scan of every 4-byte window in the mapped image for values inside each class-table window
  - a `.reloc` check
- Executed status comes from `re/coverage/reach.tsv` (2026-10-11 census). Columns: reached_menu / race / race_load / race_run.

## Owner functions (run 2, transient)
| Owner entry | Body end (entry+size) | Only ref to entry | 4 bytes before entry | Sites covered |
|---|---|---|---|---|
| `0x0057ba90` | `0x0057bf23` (1171 B) | DATA `0x005e5dd8` = Triangle `0x005e5db0` +0x28 | `90 90 90 90` | `0x57bbcc 0x57bbe4 0x57bbfc 0x57bc86 0x57bc9a 0x57bcae 0x57bd22 0x57bd40 0x57bd5e` |
| `0x0057af90` | `0x0057affc` (108 B) | DATA `0x005e5dbc` = Triangle +0xc | `90 90 90 90` | `0x57af9e` |
| `0x00574220` | `0x00574230` (16 B) | DATA `0x005e5914` = Aggregate `0x005e5900` +0x14 | `90 90 90 90` | `0x57422b` (`jmp 0x578f20`) |
| `0x00574110` | `0x00574126` (22 B) | DATA `0x005e5904` = Aggregate +0x4 | `90 90 90 90` | `0x574118` |
| `0x005669e0` | `0x00566a58` (120 B) | DATA `0x005e54cc` = Grid `0x005e54c0` +0xc | `90 90 90 90` | `0x566a15` |
| `0x00574170` | `0x005741d2` (98 B) | DATA `0x005e590c` = Aggregate +0xc | `0c c3 90 90` (`ret` + pad) | `0x57419b` |
| `0x005c51a0` | `0x005c51e8` (72 B) | DATA `0x005e7974` = Convex `0x005e7968` +0xc | `90 90 90 90` | `0x5c51b4` |
| `0x005c5730` | `0x005c574f` (31 B) | DATA `0x005e798c` = Convex +0x24 | `90 90 90 90` | `0x5c5742` |

The two DATA refs are table slots:
- `0x005e5924` is Aggregate +0x24 and holds `0x00574230`.
- `0x005e5db8` is Triangle +0x8 and holds `0x0057ae30`.

Table dwords are in `ptrscan.txt`.

Run 2 agrees with D5-DEFER-D11083 on all 8 owners. That result assigned them through a gap scan, and here each one is confirmed by its slot ref and by the padding before its entry.

## Per-site table
"Owner census" is the owner's own reach.tsv row. The 8 transient owners have no row because reach.tsv only lists functions Ghidra defined on 2026-10-11.
- "Unconditional" means straight-line disassembly from the entry reaches the site with no branch or `ret` before it.
- For those owners, the member's census row NOT-EXEC 3/3 implies the owner was not entered in the 3 census runs. Entering the owner runs the call.

| Site | Group / member | Owner | Path to site | Owner entered in census | Static chain reaches an executed function |
|---|---|---|---|---|---|
| `0x57bd22` | D-11083 `0x0057bf30` | `0x0057ba90` | 16 branches before (first `jne` `0x57bae3`, `ret` `0x57bb1a`) | [UNCERTAIN] not measured (no reach.tsv row, conditional site) | **yes**: `0x004568d0` (0/1/0/1) -> `0x00459620` @`0x004569c5` -> `0x0055bde0` @`0x00459fae` -> `CALL [EDX+0x28]` @`0x0055bf0b` -> Triangle +0x28 `0x0057ba90` |
| `0x57bd40` | D-11083 `0x0057bf30` | `0x0057ba90` | 16 before | [UNCERTAIN] as above | yes, same chain |
| `0x57bd5e` | D-11083 `0x0057bf30` | `0x0057ba90` | 16 before | [UNCERTAIN] as above | yes, same chain |
| `0x57af9e` | D-11083 `0x0057ae30` | `0x0057af90` | unconditional (0 before) | **no** (`0x0057ae30` 0/0/0/0, 0/3 runs) | **yes**: `0x0055bbf0` (0/1/1/0) `CALL [EAX+0xc]` @`0x0055bc07` -> Triangle +0xc `0x0057af90` (table installed by executed `0x00575fe0` @`0x00576344`) |
| DATA `0x5e5db8` | D-11083 `0x0057ae30` | Triangle slot +0x8 | n/a | n/a | [UNCERTAIN] no +0x8 dispatcher was found (D5-DEFER-D11083 dispatcher scan). Missing evidence: a reader of `[[obj+0x5c]+8]` |
| `0x57422b` | D-11083 `0x00578f20` | `0x00574220` | unconditional tail `jmp` | **no** (`0x00578f20` 0/0/0/0) | **no static path found**. Aggregate table: see "Unreferenced tables" |
| `0x574118` | D-11083 `0x00578ec0` | `0x00574110` | unconditional | **no** (`0x00578ec0` 0/0/0/0) | no static path found (Aggregate) |
| `0x566a15` | D-11083 `0x005742c0` | `0x005669e0` | 1 before (`jp 0x566a23` @`0x5669fb`) | [UNCERTAIN] not measured | no static path found (Grid) |
| `0x57419b` | D-11083 `0x005742c0` | `0x00574170` | 1 before (`jp 0x5741a6` @`0x574188`) | [UNCERTAIN] not measured | no static path found (Aggregate) |
| DATA `0x5e5924` | D-11083 `0x00574230` | Aggregate slot +0x24 | n/a | n/a | no static path found (Aggregate). +0x24 dispatchers exist (`0x00561390` executed, `0x005729a0`), but no object is statically given the Aggregate table |
| `0x5c51b4` | D-11083 `0x00563b00` (then `0x00573890`, `0x005739d0`) | `0x005c51a0` | 1 before (`jne 0x5c51bc` @`0x5c51b1`) | [UNCERTAIN] not measured | no static path found (Convex) |
| `0x5c5742` | D-11083 `0x00563b00` | `0x005c5730` | 1 before (`jne 0x5c574a` @`0x5c573f`) | [UNCERTAIN] not measured | no static path found (Convex) |
| `0x57bbcc` | D-11084 `0x0057c0d0` | `0x0057ba90` | 7 before | [UNCERTAIN] not measured | **yes**, same chain as `0x57bd22` |
| `0x57bbe4` | D-11084 `0x0057c0d0` | `0x0057ba90` | 8 before | [UNCERTAIN] | yes |
| `0x57bbfc` | D-11084 `0x0057c0d0` | `0x0057ba90` | 9 before | [UNCERTAIN] | yes |
| `0x57bc86` | D-11084 `0x0057c0d0` | `0x0057ba90` | 12 before | [UNCERTAIN] | yes |
| `0x57bc9a` | D-11084 `0x0057c0d0` | `0x0057ba90` | 13 before | [UNCERTAIN] | yes |
| `0x57bcae` | D-11084 `0x0057c0d0` | `0x0057ba90` | 14 before | [UNCERTAIN] | yes |

Member-to-member edges inside owned functions, all unconditional from the entry:
- `0x00563b00` calls `0x00573890` at `0x563b17`.
- `0x00573890` calls `0x005739d0` at `0x5738a7`.

`0x005748a0` is called only from `0x005742c0`, at `0x574767` and `0x57480d`.

## Upward walk from `0x0055bde0` (run 4, `upwalk_55bde0.txt`)
| Callee | Ref | Owner | Census |
|---|---|---|---|
| `0x0055bde0` | UNCONDITIONAL_CALL `0x00459fae` | `0x00459620` | 0/0/0/0 |
| `0x0055bde0` | UNCONDITIONAL_CALL `0x00481875` | `0x00481810` | 0/0/0/0 |
| `0x00459620` | UNCONDITIONAL_CALL `0x004569c5` | `0x004568d0` | **0/1/0/1, runs_race 3/3, EXECUTED** |
| `0x00459620` | UNCONDITIONAL_CALL `0x00455c29` | `0x00455b50` | 0/0/0/0 |
| `0x00459620` | UNCONDITIONAL_CALL `0x00453bd9` | no Ghidra function | not walked (an executed path was already found) |
| `0x00481810` | UNCONDITIONAL_CALL `0x00455a5c` | no Ghidra function | not walked |
| `0x00455b50` | UNCONDITIONAL_CALL `0x00455cc7` | no Ghidra function | not walked |

So the D5-DEFER-D11083 statement that the +0x28 dispatcher `0x0055bde0` is NOT-EXEC holds for `0x0055bde0` itself, but its caller chain does reach an executed function. A static caller from the executed image exists for both `0x0057bf30` and `0x0057c0d0`.

## Unreferenced tables (Aggregate `0x005e5900`, Grid `0x005e54c0`, Convex `0x005e7968`)
An unaligned 4-byte scan of the whole mapped image (`ptrscan.txt`) checked each window from the path string through the table end:
- **Grid** `0x005e5490..0x005e5540`: 0 hits.
- **Convex** `0x005e7940..0x005e79f0`: 0 hits.
- **Aggregate** `0x005e58d0..0x005e5980`: 2 hits, both past the table's terminating 0 at `0x005e592c`.
  - `push 0x5e5948` at `0x574925`
  - `push 0x5e5958` at `0x5749b5`
  - Both targets hold `0xffffffff` followed by code addresses (`0x5e594c` = `0x0057497e`, `0x5e595c` = `0x005749fd`), so they are not the class table.
- Positive control, Triangle: the scan finds the known installer immediate `0x576347` -> `0x5e5db0`.

[UNCERTAIN] for the Aggregate / Grid / Convex members (`0x00578f20`, `0x00578ec0`, `0x005742c0`, `0x00574230`, `0x00563b00`, `0x00573890`, `0x005739d0`, `0x005748a0`):
- No address of these tables appears anywhere in the image. A table base could still be formed by arithmetic.
- The `.reloc` check cannot rule that out. The image has no relocations (`Characteristics 0x10f`, `IMAGE_FILE_RELOCS_STRIPPED`, 0 HIGHLOW entries), so there is no relocation table to read absolute references from.
- Missing evidence: a runtime watch of `[obj+0x5c]` values, or a full writer list for `[reg+0x5c]` with register sources. The D5-DEFER-D11083 scan covered immediate writers only.

These 8 members still do not close the group, because `0x0057ae30` and `0x0057bf30` sit in the same row.

## Verdict per DEFERRED group
- **D-11083: STAYS OPEN.** Its close condition is "a static caller walk proves it has no caller in the executed image", and that is false:
  - `0x0057ae30` is reached from `0x0055bbf0` (executed, slot +0xc).
  - `0x0057bf30` is reached from `0x004568d0` (executed), through `0x00459620`, `0x0055bde0` and slot +0x28.
  - The other 7 members have no static path found ([UNCERTAIN], see above).
  - Condition 1, a wider census, is still the way to move the row.
- **D-11084, part `0x0057c0d0`: STAYS OPEN.** All 6 sites are in `0x0057ba90`, which is reached statically from executed `0x004568d0`. The other member, `0x0057c370`, already had executed callers (D5-DEFER-CALLERWALK: `0x0047f290` <- `0x004074a0`, race_load=1).
- **deferred_close.py lines: none.** Neither group is proven unreachable.

## Proposed tracker text (PROPOSAL only, nothing run)
Optional Why-cell addition, to be applied through `re/tools/deferred_close.py rewrite` by the lead if accepted:
- D-11083: "Static census 2026-10-30 (verify/d5deferd11083sites_20261030/RESULT_D5-DEFER-D11083-SITES.md): 0x0057bf30 also has an executed-image caller, 0x004568d0 -> 0x00459620 @0x004569c5 -> 0x0055bde0 @0x00459fae -> slot +0x28 0x0057ba90."
- D-11084: "0x0057c0d0's 6 sites are in 0x0057ba90 (Triangle slot +0x28), reached from executed 0x004568d0 through 0x00459620 and 0x0055bde0."

## Next
The cheapest probe that tells the Triangle members apart is a runtime watch on `[param_1+0x5c]` in race_run:
- at entry to `0x0055bde0` (reached only through `0x00459620` from `0x004568d0` @`0x004569c5`, or from `0x00481810`)
- at entry to `0x0055bbf0`

If either one sees `0x005e5db0`, the census scenario simply misses the branch. Otherwise the wider census (row condition 1) is still needed.

## Raw data (D:/MashedWT/D5-DEFER-D11083-SITES_data, not committed)
| File | SHA-256 |
|---|---|
| sites_owners.json (run 1, --create at 16 sites) | 24839189bcf7bd5fe23113abdab1360fd0a7c8569c6cbd2b34d290c6cbae1379 |
| owner_entries.json (run 2, 8 owner entries) | 57175b1a9917adc4b6b93a757881de59aa58117f987e60195a503990f3f417b8 |
| members_xrefs.json (run 3, members + dispatchers) | be0e6381f9326bc432a2cfbd01846bf0c618e86e31b1819c89db4f7ca0800876 |
| up_l1.json / up_l2.json (run 4 levels) | ad237e4792be26bd8656975953829f842da0500aa079e69b3ade9f9991b68cb9 / 9d4902ac83ccd3b4692e0061d3ebd3b68d705076570849c539fd53b92fb33b6a |
| upwalk_edges.json / upwalk_55bde0.txt | 24d8160b0249455a7f5f74dafadba9a81b393070843f96b5aae861e16c51ef0e / 42b2e391f7ee0c9baeec3102c0396eb0ffa927328bbe882c3953702c3df8178f |
| relocscan.txt (sections, .reloc, straight-line branch lists) | 0c1e4c325aca5c0e349fd587467293e9715ee627515a411bfd34bb9027e7801c |
| ptrscan.txt (unaligned pointer scan + table dwords) | cc1f133d428cbf8d6b1754988e075dfbebc6c56ddafce85481d8b0c629581652 |
| insn.txt (instructions at scan hits) | 4d2f8c13abe83ef4281dcdc7482262ad97d8ca811602b4bc7cde4054c897e92b |
| relocscan.py, ptrscan.py, insn.py, upwalk.py | scripts, reproducibility only |
