# RESULT D5-DEFER-D11083 (static, 2026-10-30)

Verdict: GREEN as a census. All 10 previously unresolved call sites and both table refs are now attributed to a function, each cited by RVA. **D-11083 stays OPEN.** One member, `0x0057ae30`, has a static path from an executed function: `0x0055bbf0` (census race_load=1) calls class slot +0xc at `0x0055bc07`. That slot holds `0x0057af90`, which calls `0x0057ae30` at `0x0057af9e`. The class table `0x005e5db0` is installed by executed `0x00575fe0` at `0x00576344`. The other 8 members have no executed static path. Gates: not run (static, no build, no game run, no test slot).

## Method
- `re/tools/decomp_pc.py --create --callees --callers --xrefs` on a read-only pool clone (Mashed_pool0, then a second slot). The owner functions were defined transiently (`created_transiently: true`) and nothing was persisted. Then `decomp_pc.py --datarefs` on the class-table addresses.
- A byte-level cross-check on `original/MASHED.exe.unpatched` (pefile + capstone). It scanned every dword of the mapped image for pointers into each class table, and found every `mov [reg+0x5c], imm` writer. Disassembling every Ghidra function range plus the 8 transient owners gave 502,382 instructions. It also found every indirect call through `[[obj+0x5c]+N]`.
- Executed status comes from `re/coverage/reach.tsv` (2026-10-11 census). Columns are menu/race/race_load/race_run.
- **Census coverage limit:** reach.tsv only has rows for functions Ghidra defined on 2026-10-11. The 8 transient owners have no row, so their status is **not measured**, which is not the same as not executed. Their status below is taken from their only referrer, a class-table slot, and from that slot's dispatchers.

## Class tables (`.rdata`, each preceded by its RenderWare-Physics source path string)
| Table base (type word) | Path string | Static refs to the base | Installer |
|---|---|---|---|
| `0x005e5db0` (4) | `/volume/src/RwpTriangle.c#1 $` (at `0x005e5d90`) | 1: `0x00576344` `MOV [EBX+0x5c],0x5e5db0` in `0x00575fe0` | `0x00575fe0`, reach 0/1/0/1 (executed, race_run) |
| `0x005e5900` (8) | `/volume/src/RwpAggregate.c#1 $` (at `0x005e58e0`) | 0 (Ghidra datarefs ref_total 0, byte scan 0) | none found |
| `0x005e54c0` (0xa) | `/volume/src/RwpGrid.c#3 $` (at `0x005e54a0`) | 0 | none found |
| `0x005e7968` (7) | `.../src/RwpConvex.c#1 $` (`src/` at `0x005e7950`) | 0 | none found |

The only `mov [reg+0x5c], imm` that names a class table in the whole image is the one at `0x00576344`. `0x0055bbf0` and `0x0055bde0` read the class pointer as `param_1[0x17]` (+0x5c).
[UNCERTAIN] Code could still form a table base arithmetically, which a byte scan cannot see. No such computation was found. Evidence that would settle it: a runtime watch of `[obj+0x5c]` values.

## Call-site census (all 9 members)
| Member | Referrer (site) | Owner function | Owner census status | How the owner is reached |
|---|---|---|---|---|
| `0x00573890` | `0x00563b17` | `0x00563b00` | 0/0/0/0 NOT-EXEC | `0x00563b00` is called from transient `0x005c51a0` (`0x005c51b4`) and transient `0x005c5730` (`0x005c5742`), which are Convex slots `0x005e7974` and `0x005e798c`. That table has no static ref. |
| `0x005739d0` | `0x005738a7` | `0x00573890` | 0/0/0/0 (member) | as above |
| `0x00574230` | DATA `0x005e5924` | Aggregate slot (+0x24 from `0x005e5900`) | n/a | the table has no static ref |
| `0x005742c0` | `0x00566a15` | transient `0x005669e0` | not in census | Grid slot `0x005e54cc`, table has no static ref |
| `0x005742c0` | `0x0057419b` | transient `0x00574170` | not in census | Aggregate slot `0x005e590c`, table has no static ref |
| `0x005748a0` | `0x00574767`, `0x0057480d` | `0x005742c0` | 0/0/0/0 (member) | as above |
| `0x00578ec0` | `0x00574118` | transient `0x00574110` | not in census | Aggregate slot `0x005e5904` |
| `0x00578f20` | `0x0057422b` (tail `JMP`) | transient `0x00574220` | not in census | Aggregate slot `0x005e5914` |
| `0x0057ae30` | DATA `0x005e5db8` | Triangle slot +0x8 | n/a | No reader of slot +0x8 found (see the dispatcher scan) |
| `0x0057ae30` | `0x0057af9e` (unconditional, first call in the body) | transient `0x0057af90` | not in census | **Triangle slot +0xc `0x005e5dbc`. It is dispatched by `0x0055bbf0` at `0x0055bc07` (`CALL [EAX+0xc]`, unconditional), census 0/1/1/0, EXECUTED.** |
| `0x0057bf30` | `0x0057bd22`, `0x0057bd40`, `0x0057bd5e` | transient `0x0057ba90` (1171 B) | not in census | Triangle slot +0x28 `0x005e5dd8`. It is dispatched by `0x0055bde0` at `0x0055bf0b` (`CALL [EDX+0x28]`), census 0/0/0/0 NOT-EXEC. |

Side finding for D-11084: the 6 unresolved callers of `0x0057c0d0` (`0x57bbcc`..`0x57bcae`) are inside transient `0x0057ba90`, whose callee list includes `FUN_0057c0d0`. That puts them on Triangle slot +0x28, which `0x0055bde0` dispatches (NOT-EXEC).

## Dispatcher scan: indirect calls through `[[obj+0x5c]+N]` (capstone, all functions)
+0xc `0x0055bc07` in `0x0055bbf0` (0/1/1/0) · +0x10 `0x0055bdca` in `0x0055bd80` (0/1/1/1) · +0x14 `0x0055c073` in `0x0055c000` (0/1/1/0), and `0x0055c3f7`/`0x0055c457` in `0x0055c3e0` (NOT-EXEC) · +0x18 `0x0055c2a8` in `0x0055c230` (0/1/0/1) · +0x1c `0x00578797`/`0x005787bd` in `0x00578610` (0/1/0/1) · +0x20 `0x0055c363` in `0x0055c2d0` (0/1/0/1) · +0x24 `0x005614e8` in `0x00561390` (0/1/1/1), and `0x005729ba`/`0x005729d7` in `0x005729a0` (NOT-EXEC) · +0x28 `0x0055bf0b` in `0x0055bde0` (NOT-EXEC). The scan found no +0x4 and no +0x8 dispatcher.
[UNCERTAIN] The scan is a pattern match (load `[X+0x5c]`, then a call through that register within 80 instructions), so a dispatcher that passes the class pointer some other way would be missed.

## Reading of the open caller (no runtime claim)
The static path `0x0055bbf0` -> `0x0057af90` -> `0x0057ae30` exists, and both ends of the table pairing (installer `0x00575fe0`, dispatcher `0x0055bbf0`) executed in the census. Even so, `0x0057ae30` was NOT-EXEC in 3/3 runs. The census shows `0x0055bbf0` only in race_load (race_run 0) and `0x00575fe0` only in race_run (race_load 0). That is consistent with the two never meeting on a Triangle object in the measured scenario. This is a census reading, not proof of the runtime pairing. The row's condition 1, a wider census, is still the way to promote or close it.

## Tracker change
`DEFERRED.md` D-11083 Why cell rewritten with `re/tools/deferred_close.py rewrite` to name the caller, with one CHANGELOG line. The row stays Active. No close.

## Raw data (D:/MashedWT/D5-DEFER-D11083_data, not committed)
| File | SHA-256 |
|---|---|
| owners_decomp.json (decomp_pc --create, 18 addresses) | 19962277f145fc86f23420ce06bd5c25a62612984b2fdd4e260e66827f55fc43 |
| dispatch_decomp.json (0x55bbf0, 0x55bde0, 0x57af90, 0x57ba90, 0x575fe0, 0x4d8560) | b2ad4adf3a18b213f5e2558eb7897b28198ab6c9ad4ea562f2cf33f7eeb8d3ae |
| table_datarefs.json (decomp_pc --datarefs, 10 table addresses) | 9849b80bfc10e3d7a97a32c0887b36fcdc36d05c51b1141612182843e9051dba |
| class_tables.txt (table dumps + pointer scan) | 0ec40ebef00c9bbd09f70f11d0bccd05a174e4c71214ec20a3d559a69342b37a |
| slot_dispatch_scan.txt | 39c2c9d0939432ea8f1cbcf9fb5c96f69fbbf5645b104e3d0ec682219e18ce71 |
| gap.py, tbl.py, t2.py, t3.py, t4b.py, t5.py (scan scripts) | not hashed, reproducibility only |

## Next
D-11083 needs the wider census (row condition 1). The cheapest discriminating probe is a runtime watch, during race_load, on `param_1+0x5c` (the class pointer) at entry to `0x0055bbf0`. If it ever equals `0x005e5db0`, then `0x0057ae30` executes. A second option: add the 8 transient owners to the census function list, since reach.tsv cannot see them today.
