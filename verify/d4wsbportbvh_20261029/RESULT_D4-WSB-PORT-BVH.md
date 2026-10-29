# RESULT D4-WSB-PORT-BVH: GREEN on 20 of 25 rows (C2->C3), 4 ported rows stay C2, 1 row not ported

PREREG `PREREG_D4-WSB-PORT-BVH.md` committed alone at `10d767ce6`, amendment 1 alone at `5fc765c3b`, amendment 2
alone at `37d9ee83f`, each before the runs it governs. Branch `orch/D4-WSB-PORT-BVH` off `orch/integration` @ `4ee1823e1`.
Input: the 25 `D4-WSB-PORT-BVH` rows of `verify/d4wsb_mappeddisp_20261028/disposition.tsv`.

## Per-row outcome

| row | before | after | port | shadow A/B (census scenario, hook installed alone) | heap pages o/n per sample |
|---|---|---|---|---|---|
| `0x0055f450` | C2 | **C3** | `Lane2/L2_0055f450.cpp` (hand: EAX return, raw arg 4) | CLEAN n=1, `uninstalled=a1` | 12/12 |
| `0x0055f480` | C2 | **C3** | `Lane2/L2_0055f480.cpp` | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x0055f520` | C2 | **C3** | `Lane2/L2_0055f520.cpp` | CLEAN n=2, `uninstalled=53` | 0/0 x2 |
| `0x0055f670` | C2 | **C3** | `Lane2/L2_0055f670.cpp` | CLEAN n=1, `uninstalled=56` | 0/0 |
| `0x0055f800` | C2 | **C3** | `Lane2/L2_0055f800.cpp` (hand, amendment 1) | CLEAN n=1, `uninstalled=53` (amendment-1 build) | 9/9 |
| `0x0055fe10` | C2 | **C3** | `Lane2/L2_0055fe10.cpp` | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x0055fe30` | C2 | **C3** | `Lane2/L2_0055fe30.cpp` | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x0055fe40` | C2 | **C3** | `Lane2/L2_0055fe40.cpp` | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x00562520` | C2 | C2 (impl) | `Lane2/L2_00562520.cpp` (hand: EAX return) | DIVERGENT n=1 ndiff=1 (ret + heap `+0x710`) | 2/2 |
| `0x00562580` | C2 | **C3** | `Lane2/L2_00562580.cpp` | CLEAN n=1, `uninstalled=8b` | 0/0 |
| `0x005625e0` | C2 | **C3** | `Lane2/L2_005625e0.cpp` | CLEAN n=1, `uninstalled=8b` | 0/0 |
| `0x00562600` | C2 | **C3** | `Lane2/L2_00562600.cpp` (hand: callee thunk return) | CLEAN n=1, `uninstalled=56` | 6/6 |
| `0x00562fd0` | C2 | **C3** | `Lane2/L2_00562fd0.cpp` | CLEAN n=2, `uninstalled=56` | 1/1 x2 |
| `0x00563810` | C2 | **C3** | `Lane2/L2_00563810.cpp` | CLEAN n=4, `uninstalled=8b` | 7/7 x4 |
| `0x00563840` | C2 | C2 | **not ported** (exe body exists, see below) | no arm | |
| `0x00563940` | C2 | C2 (impl) | `Lane2/L2_00563940.cpp` (hand, amendment 1) | DIVERGENT 4/4, 1-ULP float values (amendment-1 build) | 4/4 x4 |
| `0x00563de0` | C2 | **C3** | `Lane2/L2_00563de0.cpp` | CLEAN n=1, `uninstalled=8b` | 0/0 |
| `0x00563df0` | C2 | **C3** | `Lane2/L2_00563df0.cpp` (hand: EAX return) | CLEAN n=1, `uninstalled=8b` | 4/4 |
| `0x00564100` | C2 | **C3** | `Lane2/L2_00564100.cpp` (R2 rewrite) | CLEAN n=22, `uninstalled=8b` | 1/1 x13, 2/2 x3, 3/3 x2, 4/4, 7/7, 11/11, 34/34 |
| `0x00564130` | C2 | C2 (impl) | `Lane2/L2_00564130.cpp` (hand: EAX return) | DIVERGENT 10/24 (14 CLEAN), every divergent sample ret=1 | 2..12 |
| `0x005645b0` | C2 | **C3** | `Lane2/L2_005645b0.cpp` | CLEAN n=1, `uninstalled=8b` | 5/5 |
| `0x00565570` | C2 | **C3** | `Lane2/L2_00565570.cpp` | CLEAN n=24, `uninstalled=8b` | 1/1 x24 |
| `0x00565590` | C2 | **C3** | `Lane2/L2_00565590.cpp` | CLEAN n=1, `uninstalled=8b` | 1/1 |
| `0x005655a0` | C2 | **C3** | `Lane2/L2_005655a0.cpp` (amendment 1) | CLEAN n=24, `uninstalled=83` (amendment-1 build) | 0/0 x24 |
| `0x00565c20` | C2 | C2 (impl) | `Lane2/L2_00565c20.cpp` (amendments 1 + 2) | DIVERGENT 3/24, 1-ULP float values (amendment-2 build) | 0/0 x19, 1/1 x5 |

Every CLEAN row: ndiff 0 on every sample, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`, 0 NOISY / UNTRACKED / OTHER-EXC
lines. Verdicts from `re/tools/shadow_ab_report.py` (`shadow_summary.tsv`, final). The race_load rows sample 1-2 calls per
boot because the census scenario loads the race once. No C4 is claimed: these rows have no race diff, and 6 CLEAN rows compared
0 heap pages (`re/CONFIDENCE.md` write-set leg, L-38/L-40/L-54).

## The 4 ported rows that stay C2

- `0x00562520` (slot `+0x114` allocator) and `0x00564130` (slot `+0x108` allocator): every divergent sample has a return
  mismatch, so the allocator handed out a different block on the second run. `Core/ShadowTrack.h:37` documents this class.
  `0x00564130` is CLEAN on 14 of 24 samples. I hand-read `0x00562520` against the port (`0x00562520..0x0056255b`): same
  args to the slot call, same three stores. A C3 here needs an arm that holds the allocator result fixed (proposed below).
- `0x00563940` and `0x00565c20`: 1-ULP float differences (`438505a4` vs `438505a5`, `3c32a98e` vs `3c32a98f`). The TUs build
  under SSE2 and the original keeps an x87 chain. The PREREG left an x87 trial to a later amendment (`mashedmod/x87_tus.txt`
  is a measured list), so none was run.

## What was built, and the defects the arms found

- Pipeline: `decomp_pc.py --port` (pool0) -> `prep_decomp.py` (R1: 6 function-address stores, R2: CONCAT byte fill) ->
  `decomp2port.py --emit-dir Lane2 --apply` (25/25 emitted, commit `fe65b14b4` unedited). 24 TUs, `asi_sources.rsp` only,
  `L2_` names install only via `MASHED_HOOK_ONLY` / `MASHED_HOOK_LANE2` (`Core/HookSystem.cpp:244-253`). No exe source change.
- Hand fixes before arm S (commit `59087341a`, cites in each TU): 5 rows Ghidra types void return EAX that callers read
  (`scan_eax_use.txt`); `0x0055f450` arg 4 was a numeric int->float conversion, now raw bits; `0x00563940` `SQRT` -> `sqrtf`;
  race_load rows sample under `kPhaseAny`.
- Amendment 1 (`1f09a4b77`): **generator hazard.** `decomp2port.py` types `.rdata` float constants (`0x005cc56c` = 0.1f,
  `0x005cc320` = 1.0f, `0x005cc32c` = 0.5f) as `const unsigned int`, so float arithmetic on them used the bit pattern as an
  integer. Found on `0x0055f800` (orig `3dcccccd`, port `4e773333`). A scan of all `Lane2/L2_*.cpp` finds only this step's 4 TUs.
- Amendment 2 (`4c0f9cec1`): **generator hazard.** `0x00565c20` passes the address of a 3-float stack array to `0x004c3ac0`
  (`lea eax,[esp+4]` at `0x00565c44`). Ghidra shows three scalars, and `&local_c` read two non-contiguous locals (radius orig
  `3f0069c0`, port `58d07606`). A scan for `(&local_` in all Lane-2 TUs finds no other site.
- Both hazards are generator defects. `re/tools/decomp2port.py` was not changed in this step. Proposed for the harness backlog:
  type a `.rdata` `undefined4` as float when the body uses it in float arithmetic, and refuse `(&local_x)` when the decomp
  declares `local_x` as a scalar.

Final build (`MASHED_NO_DEPLOY=1`, `4c0f9cec1`): `mashed_re.exe` `f026b40b315f958489709262d487b5fd196106527c33b6135acffe126dbffbb4`,
`mashed_re_dev.asi` `18f7605026e8e9d73811ca1b49023f853fd068b15ce8e638a5c4143691ec42d3` (private root only,
`original/mashed_re_dev.asi` not touched). The exe hash moved on each rebuild with no exe-source change (`13d3a74b`, `558ca866`,
`f026b40b`), the same non-reproducible link D4-WSB-PORT-SOLV recorded. The gates below carry the exe check.

## `0x00563840`: not ported (carry-forward)

Exe-only `Collision/QhullBridge.cpp` already holds a body for this RVA (`CollHull_Alloc`, default allocator `malloc`, plus a NULL
check the original lacks). A Lane-2 TU makes a new CROSS-TARGET dual copy, and the rva-lint failed the build
(`[rva-lint] ERROR CROSS-TARGET 0x00563840`). `re/tools/dual_copy_allowlist.txt` says adding a line is a decision, so I added none.
Options for the lead: (a) leave it C2 until D4's dual-copy burn-down consolidates `CollHull_Alloc` into one shared TU, (b) a decision
row to allowlist a Lane-2 A/B copy. Recommendation: (a).

## Default gates on the final build

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + machine lock,
`A4_ROOT=D:/MashedWT/D4-WSB-PORT-BVH_root`. All six equal `re/orchestrator/merge_train.json`:

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `5943c5f1` | `5943c5f1` |
| H_DEF | gates.csv | `306186d4` | `306186d4` |
| H_DEF | ptrace.log | `52052890` | `52052890` |
| B_DEF_K0 | step.csv | `3a03670c` | `3a03670c` |
| B_DEF_K1 | step.csv | `9fcdbd3a` | `9fcdbd3a` |
| B_DEF_K26 | step.csv | `53778664` | `53778664` |

## Run log

- Boot check first (no hooks, track 0, mode 10, 4 cars, hold 15): race running, 0 MASHED Event-1000 events, endpoint
  `OK Philips FTV (NVIDIA High Definition Audio)` before and after.
- Endpoint present before and after every one of the 29 shadow windows (24 first pass, 4 amendment-1, 1 amendment-2). 0 crash
  events in any window, no fault at `0x005baf48`, no U-9677 signature.
- First pass: `0x005655a0` `error: could not attach` (Frida, PID 18088 exited on its own, nothing ran), covered by the amendment-1
  re-run. Superseded first-pass verdicts: `0x0055f800` DIVERGENT, `0x00563940` DIVERGENT, `0x005655a0` NO_LOG, `0x00565c20`
  DIVERGENT 1/24 (`shadow_summary_pass1.tsv`). Superseded amendment-1 verdict: `0x00565c20` DIVERGENT 24/24 (`shadow_summary_a1.tsv`).

## Tracker transaction (re-classify)

- `hooks.csv`: 20 rows C2 mapped -> **C3 impl**, 4 rows C2 mapped -> C2 impl, each with `file` = the port,
  `scenario` `shadow_tracked:track0-mode10-cars4`, `frida_diff` `shadow_ab:verify/d4wsbportbvh_20261029/shadow_summary.tsv`
  and a dated note. `exe_file` unchanged (empty, no exe copy exists). `0x00563840` unchanged.
- C3 gate: port under `mashedmod/src/mashed_re/`, clean build, `RH_ScopedInstall`, runtime-toggleable (`L2_` opt-in),
  callers all at C2+ (`decomp_pc.py --callers`, e.g. `0x0055dca0`, `0x0047f840`, `0x0047f4c0`, `0x0057c670`, `0x005595d0`),
  callees C2+ or none (callee `0x004c3ac0` C4).
- U-9572 (`0x0055f450`), U-9573 (`0x0055f800`), U-9576 (`0x00563810`) carried open on the row notes. Their Blocks cell reads
  "the C3 hand-read must close or carry these". Hand-read here: the slot calls are caller-cleaned (`add esp,0x18` at
  `0x0055f47a`, `add esp,4` at `0x00563834`, `add esp,0x18` at `0x0055fc57` for the two `0x00564100` calls), and the
  `0x00563810` join at `0x00563837` is reached at depth 0 from both paths. The indirect targets (`+0x108`, `+0x10c`) are still
  not determined statically, so the rows stay open. No new U or S rows.
- `re/analysis/CHANGELOG.md`: 24 lines below `<!-- ENTRIES -->`.

## Next

- Allocator rows `0x00562520` / `0x00564130`: a shadow arm that pins the slot `+0x108` / `+0x114` result for both runs
  (or a region compare on the returned block only), then re-classify.
- Float rows `0x00563940` / `0x00565c20`: an x87 trial (`x87_tus.txt` entry under a PREREG) and a re-run.
- The two generator hazards above, as `decomp2port.py` checks (harness backlog).

## Files

Committed: `PREREG_D4-WSB-PORT-BVH.md`, `RESULT_D4-WSB-PORT-BVH.md`, `shadow_summary.tsv` (final), `shadow_summary_pass1.tsv`,
`shadow_summary_a1.tsv`, `prep_decomp.py`, `scan_eax_use.py` + `scan_eax_use.txt` (copy of `verify/wsbbvh_20261026/scan_eax_use.py`),
`disasm_lin.py` (copy of `verify/wsbbvh_20261027/disasm_lin.py`), `run_shadow7.py`, `drive_pb.py`.

Raw (not committed), `D:/MashedWT/D4-WSB-PORT-BVH_data/` (final logs; superseded runs kept as `<rva>_pass1/`, `00565c20_a1/`):

| file | SHA-256 |
|---|---|
| `0055f450/shadow_ab.log` | `57ea0487c831202b802c3835de7eab8247fdb11b7bbcbc45891cd86de5209770` |
| `0055f480/shadow_ab.log` | `cde6acd2ea2b622dbbf0ac27f594f780c9d8f3f339d0b36ed0301d55519e5de1` |
| `0055f520/shadow_ab.log` | `a8373e99112572f3d2dc3f15312b8869034e0edb8916ba4ec2e1dc88199b9d09` |
| `0055f670/shadow_ab.log` | `bd5c31c57550452eb363d8152fc716289d92a881fc663073bbcc0d6bc75ba210` |
| `0055f800/shadow_ab.log` | `aeb5501e785ebcfcd249e3009104938eb86f7ca5c394f515fcc862d56be86502` |
| `0055fe10/shadow_ab.log` | `8c7d67fa08f0dd60f694d7f5cb1a35fcc92c879e2486c66b51343eb57503bdc4` |
| `0055fe30/shadow_ab.log` | `c2ce850896a9e779545a161221a3fed52500f4ca7311470c9c414d7005a67a37` |
| `0055fe40/shadow_ab.log` | `87b98a62ec377a1d24a2daef2bb50c197748f521ba50df874d469dec0fb36150` |
| `00562520/shadow_ab.log` | `31815ce6a710f1db3695406a3d25a705226d70615b48690f9ddd605da6849bd4` |
| `00562580/shadow_ab.log` | `634a8cca5fa518599f2badf5f1ffeb2b80de96d41be2a5d1745fd0480e66d59d` |
| `005625e0/shadow_ab.log` | `f75e827992baee43a4c441ef4bd961e5b3f3e038795771a5a354fc0eda9bcb77` |
| `00562600/shadow_ab.log` | `eb07ac5c31777bbb9064e8b90d486285e464243f018a7727ac460f7c80dad895` |
| `00562fd0/shadow_ab.log` | `f7d295fef5a539cedbe06c848eae43862f7201c482cedc36a4a2bd33188b8e55` |
| `00563810/shadow_ab.log` | `08b4542ea5d6d1a0ce438c9b35cbac1f72f9654146e9083cd47e0a444355617c` |
| `00563940/shadow_ab.log` | `4d597852d145f5ea9acb967608f954fbafa64838a4b7e98825d42b0036c66f3d` |
| `00563de0/shadow_ab.log` | `9a0049c1acb48d4b35bf9df6384b19b22bf21d82337f4ace37a9500543e60376` |
| `00563df0/shadow_ab.log` | `5823a522abc3276b8788d1868a699dcd030f260f5dd24d79a72cf20c76f00f0b` |
| `00564100/shadow_ab.log` | `f653f7a892cfc40c75ad23929ab2993fed008123052967e819030bd6be580d8f` |
| `00564130/shadow_ab.log` | `5853746dd41a8b4d8903f2e08741bd489ddc7007902191c3e88f8f1b3d1af855` |
| `005645b0/shadow_ab.log` | `db188fafda9f5c1e96dc0eedc3810fdc87f8d3b2942b33de0ab9b6a694e72425` |
| `00565570/shadow_ab.log` | `85e6c26e31febd58fc9784ec8b9ab268b4b0ecc115ff470f8c3ee5badcd31dce` |
| `00565590/shadow_ab.log` | `f0ee6813873bab620f7f86495f188c6c7a940ebfadd735c2b345bcc9a3ad3e52` |
| `005655a0/shadow_ab.log` | `06e970b5a155969d68db8113907a39290cb977fc2225c1dfae9430d4a6e75b7c` |
| `00565c20/shadow_ab.log` | `0b264816e4b1f5a79cb7315e6e4f21c545b0d3280ab505a0896df55e547fc984` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `306186d44ce7193b93b53a12e1658b871e90bb276480c2add7d8c9f7c08e6a2e` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |

Game PIDs spawned (only these killed, by PID, or self-exited): boot check 6664; first pass 19672, 9576, 4328, 12552, 12800,
16344, 19944, 13796, 13736, 15912, 10868, 8104, 10536, 5000, 16156, 20700, 8584, 7200, 13340, 21216, 17072, 11428, 18088
(no-attach, self-exited), 10604; amendment 1 4180, 15808, 5368, 21752; amendment 2 2800; gates (standalone exe, self-exited)
run under lock pid 1700. None alive at the end. No watcher left running.
