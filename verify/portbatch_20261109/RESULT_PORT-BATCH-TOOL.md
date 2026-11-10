# RESULT PORT-BATCH-TOOL (D4): closed-set tool, first batch, gates, bisect driver

Branch `orch/PORT-BATCH-TOOL` off orch/integration `25a6fe1c5`. Commits: PREREG `a4be556f2` (alone, before any arm),
tool + swap `6f837644e`, bisect driver fix `3fbdcc417`, this RESULT. Verdict **DECISION**: the swap is in, default
OFF, and switch ON leaves every gate unchanged. The flip is Mariano's call. There is a second decision, on how the
mass route handles in-place swaps, because most of the port surface sits there.

## 1. The tool

`re/tools/port_closed_sets.py [--build-dir <dir>] [--out <dir>] [--sub <s> [--all]]`. It reads these inputs:
- `hooks.csv`
- the `// MATCH:` bodies of `re/match/**`
- `re/console/pc_funcs.csv` (function entries)
- `verify/porttop_20261108/BINDINGS.tsv`
- every `#define FUN_<rva> <Name>` of the exe TUs
- `Compat/PortStateSeed.inc` (L-99 seed table)
- `re/tools/dual_copy_allowlist.txt`
- the linked exe and its map, through the graph and name resolution of `scripts/backfill_port.py` and `scripts/rva_body_scan.py` (reused, not copied)

Method:
- **Candidates:** C4 rows with a matched body. The library must be in the port count (game code and RW-Physics 3.7;
  CRT, libpng, zlib, RW core, Lua, qhull, d3dx9 and DirectShow are out under L-67, L-70, L-97 and L-98). The exe must
  not already compile that matched source. Result: 2,158 rows in 2,074 units.
- **Unit = matched file.** `#include` compiles every body in a file, so all of them must link.
- **Callee classes.** A callee is `set` (another candidate) or `bound`: an exe C-linkage, non-static symbol for its
  body, an exe `#define` to such a name, a BINDINGS row, or one of the 6 thunks. RW core bound by name counts as
  `rw` and is allowed (L-98). Anything else is `open`: C2/C3 code, unbound RW core, CRT or Lua, or a callee only a
  stand-in binds.
- **Fixpoint:** remove every unit with an open callee and repeat. 1,196 units / 1,231 rows survive.
  `closed/closed_removed.tsv` lists the 878 removed units with their open callees.
- **Groups:** one per root, as the root's downward closure, plus one per unreached top unit. 913 groups. Closures
  overlap.
- **Reach:** the linked exe's graph (rel32 calls + base relocations) is walked from `_WinMain@16` with
  `_Orig_00492370@16` as a barrier (the default path) and from `_Orig_00492370@16` (the MASHED_ORIG_TOP path). A
  member is a root in one of two shapes:
  - **inplace:** the exe already runs a hand body for it on that path.
  - **rebind:** a reached, exe-compiled matched body calls it through a forwarder or stand-in, and the member has no
    exe body.
- **Blockers per member:**
  - DUP (exe body)
  - CROSS (asi-only body)
  - ALLOW (allowlisted)
  - DUPFILE (the file also holds an already-compiled body)
  - SEED (a nonzero file-backed .rdata/.data address the seed does not hold)
  - RWGLOBALS (reads `DAT_007d3ff8`, which is 0 in the exe pad)
  - NONC4 (a non-C4 body that the file compiles with the set)
  - INDIRECT (calls through a pointer it reads; a note only)

  A group is **lint-clean** when no member has DUP, CROSS, ALLOW, DUPFILE or RWGLOBALS. It is seed-clean when no
  member has SEED.

Outputs, committed under `verify/portbatch_20261109/closed/` (311 KB):
- `closed_<subsystem>.tsv`: columns group, rows, reached, blockers, head, head_shape, n_def, n_top, lint_clean,
  seed_clean, rvas
- `closed_members.tsv`
- `closed_removed.tsv`
- `closed_summary.tsv`

All are computed on base exe `32dee813` (orch/integration `25a6fe1c5` built in this worktree).

## 2. Per-subsystem closed-set table

Columns:
- "set": the union of the closures of the groups that path reaches.
- "lint-clean": the part a default-OFF runtime switch can carry without a second body.
- Blocker counts are member hits summed over that subsystem's default-reached groups. Closures overlap, so a member
  can count more than once.

| subsystem | closed rows | groups | default-reached set | of which lint-clean | top-reached set | of which lint-clean | blockers in the default set |
|---|---|---|---|---|---|---|---|
| ai | 39 | 31 | 16 | 0 | 16 | 0 | CROSS 2, DUP 19, NONC4 2, RWGLOBALS 5 |
| audio | 229 | 157 | 0 | 0 | 0 | 0 | - |
| boot | 34 | 25 | 0 | 0 | 22 | 6 | - |
| camera | 3 | 3 | 0 | 0 | 0 | 0 | - |
| debug-overlay | 1 | 1 | 0 | 0 | 0 | 0 | - |
| frontend | 75 | 40 | 19 | 0 | 26 | 0 | CROSS 4, DUP 11, DUPFILE 13, INDIRECT 3, RWGLOBALS 4 |
| gameplay | 170 | 116 | 25 | 0 | 36 | 1 | CROSS 5, DUP 15, SEED 1 |
| hud | 47 | 22 | 23 | **4** | 25 | 4 | CROSS 12, DUP 4, INDIRECT 3, NONC4 1, RWGLOBALS 4, SEED 1 |
| input | 18 | 5 | 0 | 0 | 12 | 0 | - |
| particle | 22 | 19 | 0 | 0 | 5 | 1 | - |
| physics | 1 | 1 | 0 | 0 | 0 | 0 | - |
| powerups | 4 | 3 | 3 | 0 | 3 | 0 | DUP 2, INDIRECT 2, SEED 2 |
| render | 357 | 260 | 16 | 0 | 52 | 0 | CROSS 4, DUP 7, NONC4 2, RWGLOBALS 7, SEED 1 |
| save | 12 | 6 | 0 | 0 | 0 | 0 | - |
| sky | 4 | 4 | 0 | 0 | 0 | 0 | - |
| smplfzx | 18 | 7 | 0 | 0 | 0 | 0 | - |
| RenderWare-Physics-3.7 | 100 | 75 | 0 | 0 | 0 | 0 | - |
| track | 5 | 5 | 0 | 0 | 0 | 0 | - |
| unknown | 7 | 6 | 0 | 0 | 0 | 0 | - |
| util | 120 | 94 | 14 | 0 | 23 | 0 | DUP 13, RWGLOBALS 2 |
| vehicle | 54 | 31 | 12 | 1 | 14 | 1 | CROSS 1, DUP 11, RWGLOBALS 3 |
| video | 1 | 1 | 0 | 0 | 0 | 0 | - |
| world-objects | 1 | 1 | 0 | 0 | 0 | 0 | - |

What the table says:
- **The default path reaches 128 closed rows in 8 subsystems: ai, frontend, gameplay, hud, powerups, render, util,
  vehicle.** Almost all of them are rooted at *in-place* hand bodies: the exe already runs a hand port for the head.
- A default-OFF runtime switch over an in-place head needs the hand body and the matched body side by side, which is
  DUP-IN-TARGET under `scripts/lint_rva_bodies.py`. Only 5 of the 128 are lint-clean: hud 4 and vehicle 1
  (`0x00496900`). On the MASHED_ORIG_TOP path the clean sets are boot 6, hud 4, gameplay 1 and particle 1, all
  rooted at PortTop stand-ins.
- The main open callees, by number of units removed:
  - the Lua API, out of the count (L-97) but called by game code: `lua_gettop` `0x004b6fc0` blocks 62 units,
    `lua_tonumber` `0x004b7090` 49, `0x004b70d0` 21
  - unbound RW core: `0x004e7e30` 26, `0x004c15c0` 19
  - `RwErrSlotWrite` `0x004d8480` (C3) 22
  - `0x004770a0` (C2) 18
  - `RwpBodyTableLookup` `0x0057c210` 18

  By class: C2 game code in 277 units, a removed in-set callee in 219, RW core 187, no hooks row 74, Lua 68, CRT 69.
- CROSS (461 member hits overall) comes from the old asi-only hook bodies (`Util/PromoLoop_*.cpp` and similar). An
  exe-only matched body beside one of them is a new CROSS-TARGET finding.

**Pick: hud.** It has the largest lint-clean set the default path reaches: 4 rows, 2 heads, and no other
subsystem has more than 1. In-place sets (gameplay 25, hud 23, frontend 19) are larger but cannot meet
"default-OFF switch + clean lint" (section 6).

## 3. The swap (commit `6f837644e`)

| head | closure | matched source | wiring point | port before -> after |
|---|---|---|---|---|
| `0x0041d870` | + static `0x0041d410` | `re/match/s04/0041d410.c` | `MatchS02_0040dfc0_Call0041d870` (`Match/MatchS02_0040dfc0.cpp`, exe block) | none -> called (both rows) |
| `0x0041e850` | + static `0x0041e630` | `re/match/s04/0041e850.c` | `MatchS02_0040dfc0_Call0041e850` (same file) | none -> called (both rows) |

How it is wired:
- **Before:** both wiring points were no-op `{}` forwarders, which the matched body of HudIngameDispatch `0x0040dfc0`
  calls (`re/match/s02/0040dfc0.c`, reached from LibRw::RaceFx_RenderHud).
- **After:** each forwarder is
  `{ PortBatch_Run("HUD", 0x<head>u, &PortBatch_<head>); }`.
  - `Match/PortBatch_Switch.cpp` reads `MASHED_BATCH_HUD` (default OFF) and `MASHED_BATCH_HUD_ROWS` (head list, used
    by the bisect).
  - The body runs only when the switch is on.
  - With the switch OFF nothing is logged.
  - The address argument is deliberate. A direct call compiled to a conditional-jump tail call that
    `backfill_port.py` does not follow, and the first build derived `0x0041e850` as `body`. The address is a
    relocation, which the graph does follow.
- **Bodies:** `Match/PortBatch_HUD_0041d410.cpp` and `Match/PortBatch_HUD_0041e630.cpp`, generated by
  `re/tools/port_batch_gen.py --sub hud`. The generator also adds them to `exe_sources.rsp` and `x87_tus.txt`,
  rewires the `void(void) {}` forwarders, and writes `batch_HUD.tsv`. Any other forwarder shape stops the generator
  without writing anything (exit 2).
- **Run harness:** `verify/savelink_20261023/run_arms.py` gains stack `BATCH`, whose knobs are passed in
  `A4_BATCH_KNOBS`.

**Excluded:** `0x00556e40` (hud, top path only, PortTop stand-in `PortTop_StandIn04C_00556e40`). It calls
`(*(DAT_007d3ff8 + 0x11c))(...)`, and RwGlobals is 0 in the exe. The tool now flags it RWGLOBALS. The first pass of
the tool counted it as clean before that blocker existed.

Build and lint:
- Both targets build. Exe `e871d3da307381f33e6991e48f30b5a347c181e5fb78146b813a410dd80305cb`, asi
  `87be777bf438f78337cb9f8c1ee5dfc30003424a62368d247927fbb8259d4c17`.
- rva-lint: `1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0`, the same as the base build. The allowlist is
  unchanged.

Port column:
- `backfill_exe_file.py` then `backfill_port.py`: 4 rows none -> called. No row of this set loses `called`.
- **Drift that predates this branch:** on this tree `backfill_port.py` also derives `0x00418860` (Ai/AiStandalone.cpp)
  called -> body (`unresolved`, no map symbol). AiStandalone.cpp last changed in `d6ed23348` PORT-AISPLINE-DUALCOPY,
  this branch does not touch it, and the build has no /GL. I left that row at its committed `called`, so this
  branch's hooks.csv diff is the 4 rows only. The next tracker pass needs to re-derive it.
- The hooks.csv `file` column was not touched (re-classify's job).

## 4. Gates (switch OFF and ON)

All passes ran inside `test-slot` + `mashed_lock.py run` with exe `e871d3da`, using
`py -3.12 re/tools/port_batch_bisect.py --tag HUD --heads 0x0041d870,0x0041e850`. Roots:
`D:/MashedWT/PORT-BATCH-TOOL_root` (gates) and `_rootK` (nav), copies of the PORT-TOP-04G roots. The PORT-TOP-04E
settings baseline was restored before each nav run.

**Run 2 (evidence, label prefix `r2_`, order OFF, ON, OFF, ON):**

| pass | G-CTRL | B (K0/K1/K26) | 12 nav captures vs 04E list |
|---|---|---|---|
| OFF | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON (`MASHED_BATCH_HUD=1`) | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| OFF repeat | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON repeat | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |

- Every pass equals the `merge_train.json` refs. All arms exited 0 and self-exited, and every nav run ended
  `NAV_DEMO phase=28 done ok=1`. Bisect: not needed (responsible: none). Table: `bisect_HUD.tsv`.
- **The gates do not execute this set.** No log of any ON pass, in either run, holds a `[PORT-BATCH]` line. The switch
  writes one on its first query, so `PortBatch_On` was never called. HudIngameDispatch does not reach either
  forwarder in the TRAINING G-CTRL race, the B races, or the menu walk.
- The sources also show the bodies would do nothing on the default path today:
  - `0x0041d870` calls `obj[k]->m48` only where the `DAT_0063d298` record has `on[k] != 0`.
  - `0x0041e850` calls `0x0041e630` only when `DAT_0063d610.n1d0 != 0`.
  - Both addresses are .bss. The only exe-compiled writer of `0x0063d298`, `0x0041d6e0` (`Match/MatchS04_0041cd20.cpp`),
    is port `body`, so no exe path reaches it. No exe TU outside the MASHED_ORIG_TOP path names `0x0063d610`.
- The PREREG's [UNCERTAIN] is answered: neither forwarder runs in the gate scenarios.

**Run 1 (first attempt, order OFF, ON, then bisect), kept for the harness finding:**
- G-CTRL and B were at the refs on every pass.
- The nav captures were not:
  - OFF matched 1/12 of the 04E list.
  - ON matched 12/12.
  - The bisect pass with head `0x0041e850` alone matched 1/12, with a third set of hashes.
- With the switch never queried, the difference cannot come from the switch. It is a capture flake: in
  deterministic mode `MpegVideoTexture::Update` freezes the frontend.mpg backdrop after its first upload
  (D3d9Render/MpegVideoTexture.cpp:231-264), and which frame that is depends on how fast the machine gets there.
  Run 1 ran while the test-slot memory gate showed 18-19 GB committed. Captures 02-12 differ in x 0..510, the
  backdrop; walk_01 has no backdrop and stayed stable.
- With a single OFF pass the first driver sent this to the bisect and named both heads "responsible". That
  attribution is void. Commit `3fbdcc417` makes the driver gate OFF, ON, OFF, ON and bisect only a difference that
  repeats on both sides. A side that does not repeat is reported UNSTABLE.
- The run-1 table is kept at `D:/MashedWT/PORT-BATCH-TOOL_data/bisect_HUD_run1.tsv` (not committed).

## 5. Bisect driver

`re/tools/port_batch_bisect.py --tag <SUB> --heads <list> [--data] [--root] [--rootk] [--no-nav] [--label-prefix]`:
- **One pass:** run_arms H/B with stack DEF (OFF) or BATCH (`A4_BATCH_KNOBS`), then the nav-demo walk
  (`verify/porttop_20261108/knob_boot.py`). The pass is hashed the merge-train way and compared with the 04E list.
- **Order:** OFF, ON, OFF, ON. An unstable side stops the run.
- **When ON moves:** the heads are split in halves through `MASHED_BATCH_<SUB>_ROWS`, recursing to single heads. A
  half that moves only together with the other is reported as one unit. The responsible heads are excluded and the
  rest is gated ON again.
- **Output:** `<data>/passes.tsv` and `verify/portbatch_20261109/bisect_<SUB>.tsv`.
- **Tested so far:** only the no-movement path ran here. Run 1 also exercised the halves split and the recursion, on
  spurious input.

## 6. Decisions

**D1, the flip (this step).** Flip `MASHED_BATCH_HUD` to default ON? The hashes do not change:
G-CTRL OFF = ON = `8284be46/67931834/31fe043e`, B OFF = ON = `1d5bf4f9` x3, nav 12/12 on both sides.

Options:
- **(a)** Flip now, with opt-out `MASHED_NO_BATCH_HUD=1`.
- **(b)** Keep OFF until a writer of `DAT_0063d298` / `DAT_0063d610` is ported, so a gate actually runs the bodies.
- **(c)** Drop the switch and keep the bodies as plain wiring.

Recommendation: **(b)**. The port column already counts the 4 rows as `called` with the switch OFF (static
reachability, see backfill_port's docstring). The gates cannot see the set today, so a flip now buys no measured
parity.

**D2, the mass route (for the lead and Mariano).** 123 of the 128 default-reached closed rows are behind in-place
heads. Under the current lint, a default-OFF runtime switch cannot carry them.

Options:
- **(a)** PORT-SAVE-01 shape: no runtime switch. The matched body replaces the hand body. The batch is gated as one
  unit. A moved gate is bisected by rebuilding with halves of the heads swapped (a compile-time selection the
  generator can emit). This needs a rule that build-time bisects are allowed in place of a knob for hash-neutral
  batches.
- **(b)** Keep the runtime switch and allowlist one DUP-IN-TARGET line per in-place head for the switch window, with
  the lines removed at the flip. The rule says "adding a line is a decision", so this is that decision.
- **(c)** Only the lint-clean sets: 5 rows on the default path, 12 under MASHED_ORIG_TOP.

Recommendation: **(a)**. It keeps the one-body rule and still bisects.

Whatever is chosen, the largest single unlocks are a Lua binding for the game's Lua callers (L-97 keeps Lua out of
the count but does not bind it) and the asi-only CROSS bodies (461 member hits).

## Evidence (raw data in `D:/MashedWT/PORT-BATCH-TOOL_data`, not committed)

| file | SHA-256 |
|---|---|
| r2_01_off/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| r2_01_off/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| r2_01_off/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| r2_02_on_all/arms/H_BATCH.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| r2_02_on_all/arms/H_BATCH.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| r2_02_on_all/arms/H_BATCH.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| r2_02_on_all/arms/B_BATCH_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| r2_02_on_all/arms/H_BATCH.log (no `[PORT-BATCH]` line) | 282c91f9c092391b7c210fcce18fdb642f19e69257f742af816a89c10bb2727e |
| r2_01_off/navdemo.log | 23e87f29de2e1d45bbbd237faa9b412a61ff7985f299403b4f32eb10fb3b72f1 |
| r2_02_on_all/navdemo.log | 4f81c5ca54d4300f9a9ac97ffe457c7046c32289a3c9ee6a7bfc09a58cb34675 |
| passes.tsv (run 2, all 4 passes incl. nav hashes) | f622d54cf26293b96b64a7e3c4b3064f20868e3183c08cfe35ca478354e01aca |
| bisect_run2.log | 09a30ea0df0a8419909437196a718d17357861883a1f907975654e054bf64646 |
| passes_run1.tsv | 92fc81b0c5e19156e8e399a76d3f290d861dfe00b5a3f2a14fb6d67a4b539301 |
| bisect_run.log (run 1) | 492308d954f628da05274da196fb930c75fd8496be0c4e2960ce537a7796aafb |
| 01_off/navdemo.log (run 1, OFF 1/12) | bfa55527a05ee6c20eb0032206eff98bef628177fc74b79a884172725757570a |
| 04_bis_41e850/navdemo.log (run 1) | cc4446f08459f681629e608cf952be799ba91923e8af7f6066633b871200ed16 |
| build_swap2.log (Build OK, rva-lint NEW=0) | 3f2374c0f3aa99a71f60989c81ddc7d76364559df49efce1f328a3bdaae56add |

The B K1 and K26 step files and the remaining nav BMPs are in the same pass folders. The hashes of all four passes
are in `passes.tsv`.
