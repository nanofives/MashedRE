# D3 bulk port, Phase 0 plan (2026-10-11)

User decision 2026-10-11: port in bulk, switch on in small call-chain groups. This file is the Phase 0
deliverable. No port code was written. Phase 1 starts only after the user's OK.

## 1. Verdict

The D3 bulk lane is thin. 359 C2 rows execute in the Training race, but only 2 D3 groups clear the
30% yield bar today: the event queue (G-EVQ) and the AI-step leaves (G-AILEAF). Every other D3 group
reads state the exe does not hold at the original VA, so it is blocked on a producer first. Most
executed unported rows (about 200) are render / particle / audio / CRT and sit outside D3.

## 2. Method and evidence

| step | tool / file | result |
|---|---|---|
| static candidates | `tools/cg.py` (capstone direct calls over `re/analysis/function_inventory.csv`) + plate `callees_depth1`, `tools/cand.py` | 1,686 functions reachable from 10 race roots; 1,195 C1/C2; 601 after dropping third-party (qhull, RW physics, CRT) -> `candidates.tsv` |
| execution census | new `scenario_launch.py --firsthit-probe` (entry-only, per-phase cap 16, self-detach at the phase-3 cap) | 2 batches (`fh_a`, `fh_b`), D3 argv `--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --hold 60`, 0 attach errors, 3,623 / 3,625 frames |
| controls (both batches) | zero: 0x004847d0, 0x0047cd50, 0x00458d00 | 0 / 0 / 0 in both batches |
| | live: 0x00416250, 0x0045bba0, 0x004111c0 | 16 / 16 / 16 in both batches |
| merge | `tools/merge.py`, `tools/bodies*.py` | **359 executed** (all C2; **0 C1 executed**), 242 not -> `executed.tsv`, `not_executed.tsv` |
| port status | `scripts/rva_body_scan.py` + `re/tools/naked_specs/*.json` + hooks.csv `exe_file` | exe body 43, exe naked 16, exe_file column only 4, .asi-only 8, none 288 |
| call-chain groups | `tools/groups.py`, `tools/members.py` | `groups.tsv` |
| inputs (static) | `decomp_pc.py --port --callers` on 49 group heads -> `inputs_static.json` | globals per function, R/W from the decomp text |
| inputs (live) | `tools/peek.py` (ReadProcessMemory only, no hooks): standalone default env (131 samples) vs original D3 argv (63 phase-3 samples) -> `peek_sa.json`, `peek_orig.json`, `inputs_live.json` | 170 addresses: NOWRITER (original non-zero, exe always 0) 132, both 5, zero 30, exe-only 3 |
| producers | `decomp_pc.py --datarefs` -> `drefs_evq.json` | 0x007f1030 written at 0x0040fe24 (FUN_0040fc00); 0x0089a4c0 at 0x00414208 (FUN_00414180) and 0x00417ca0 (FUN_004177b0) |

Executed unported rows by subsystem (`none` + `.asi-only`): render 84, util 58, gameplay 33, particle 26,
boot/CRT 21, audio 17, vehicle 19, ai 10, frontend 8, hud 5, powerups 4, camera 3, physics 3, other 5.

### 2.1 Integration facts found in Phase 0 (apply to every group)

1. **Car records are not at the original VA in the exe.** The exe keeps them at `g_vehicleArrayBase`
   (`Vehicle/ForceIntegratorStubs.cpp:39`). `PtrCompute881ec8` (`Ai/AiLeaderTimer.cpp:112`) is the
   rebind. Any body that reads `0x008815a0 + v*0xd04 + off` by absolute address reads the blank pad.
   Each such operand needs a spec `patch` or `consts` rebind. This is why the peek shows every
   `0x008815a0..0x008822a3` field as NOWRITER: those rows mean "not at the VA", not "no producer".
2. **The original's race loop has no exe body.** 0x004111c0 -> 0x0040fc00 -> {0x0040ce00,
   0x00425a40, 0x0040da50, ...} are all C2 `none`. The exe step is hand-written
   (`Ai/AiStandalone.cpp`, `D3d9Render/TrackRenderer.cpp`). So each group's "exe call site" is a new
   call placed in that hand-written order, cited against the original call address. It is not a
   rebinding of an existing host.
3. **Several AI-step members already exist as C++ stand-ins**, invisible to the body scan:
   - 0x00443dc0 and 0x00443440: `AiStandalone.cpp:272`, `:489`
   - 0x00415220: `:708`
   - 0x00414570 and 0x00415880 are stubbed on purpose (`:914-917`): the original returns 0 on
     6,696 / 6,696 calls (`verify/d3_modes37_20261009/PREREG_P3.md:44`).
   
   Every `none` row must be grepped by RVA before it is called unported.

### 2.2 Limits of this census

- Static reach follows direct `call`/tail-`jmp` only. Indirect calls (vtables, the Lua glue, registry
  callbacks such as 0x00486630) are missed.
- C3/C4 rows and third-party rows were not probed.
- The census covers one scenario: mode 10, 4 cars, 60 s, round 1. Code that runs in later rounds or
  other modes is invisible.
- Counts are capped at 16 per phase. The `hits_per_ticks` column is ticks per 16 calls, not a rate.
- The peek reads 4 dwords per address. For per-car tables it uses the known strides (0x74, 0xd04, 0xb4),
  otherwise 4 consecutive dwords. Field-level coverage of larger structs is [UNCERTAIN].

## 3. Ranked groups

Yield = expected fraction of members that pass replay AND run integrated without a new producer
outside the group.

### Rank 1: G-EVQ (vehicle interaction event queue). Expected yield 60-70%. SCHEDULE.

**Entry and call site**
- Writer 0x004252c0 (1,889 B) and drainer 0x00422ba0 (1,071 B). Both are called from 0x00425a40
  (the per-frame world update, which has no exe body).
- The exe call site is the hand-written step, at the order 0x00425a40 uses. Phase 1 reads that order
  from the disassembly.

**Members**
- 0x004252c0 and 0x00422ba0.
- The handlers: 0x00417370 (44 B, RNG draw -> `0x0089a4fc + v*0x74`), 0x004219f0 -> 0x004219a0 ->
  0x00421980, and 0x0047cea0.
- All 7 run in the race: 16 hits each, except the handlers at 1..10.

**Callees**
- Exe-ported: 0x0046cbb0 C4, 0x0046cbe0 C3, 0x004922e0 C3, 0x004c3ac0 C4, 0x0040e470 (naked),
  0x004a2c48 RNG (exe body), 0x00407a40 C4, 0x004233e0 C3, 0x0046d510 C3.
- 0 race hits, so these become counted no-ops with a witness: 0x0044e0a0 (type 10), 0x00480720 and
  0x00480d60 (type 3 high impact).

**Inputs**

| input | exe status | producer plan |
|---|---|---|
| ring `0x007e9de0..0x007eca1c` | NOWRITER; the original's write head took 31 distinct values in 63 phase-3 samples | inside the group |
| pair tables `0x008995b0..0x008995f4` (stride 0x4e) | NOWRITER | inside the group |
| frame stamp `0x007f1030` | NOWRITER | **producer leg P0**: one store from the exe step counter, cited to 0x0040fe24 |
| `0x007f0ff8` (timer stamped into the pair table) | NOWRITER | P0 as well [UNCERTAIN: its writer is not yet looked up] |
| car records | rebind (§2.1 item 1) | rebind |
| `0x007f1038`, `0x007f103c` | | zeroed by memset FUN_004030d0 at 0x00403122; no other direct writer [UNCERTAIN: computed-base writers] |

**Replay**
- A new `--evq-probe`, on the `--cb-probe` pattern: entry and leave on both functions, ring and pair
  tables per call.
- Plus a Unicorn grid over event types 3/4/6/8/9/10, like `pu_cb_unicorn.py`, for arm coverage.
- Negative control: 2 corrupted rows.

**What it touches**
- Criterion (b): yes. The original varies the AI slot field `+0x3c` (`0x0089a4fc`) on cars 1..3
  (5-7 distinct values in 63 phase-3 samples), and the exe holds 0.
- The type-9 path draws the RNG, so ring phase moves. The PREREG must predict "(b) and the G-CTRL
  hashes change with the knob ON".

**D2 watch:** handlers 3/4 read contact impacts but write only the pair tables and the spinout state. If
Phase 1 finds a write to `+0x4a4` or to the contact collector, that is a D2 REOPEN CANDIDATE.

### Rank 2: G-AILEAF (verbatim targeting-chain leaves). Expected yield 60% for promotion, about 0% behaviour. SCHEDULE.

**Members**
- 0x00414570 (821 B)
- 0x00415880 (1,137 B)
- 0x0046d570 (228 B, called from the exe-body 0x00417640 and from 0x0040ca00)

**Call site:** the existing stubs in `AiStandalone.cpp:914-917` and the 0x00417640 port.

**Inputs**
- `0x007f0fd0` (rule): 0 on both sides on Training.
- `0x0089a4c0 + v*0x74`: NOWRITER at the VA. The original writers FUN_00414180 and FUN_004177b0 have
  exe ports, which write private state.
- `0x005f2fc0`, `0x0089a368`, `0x0089a374`: NOWRITER. `0x0089a368` is the difficulty roll, and the exe
  holds the regime privately.
- Car records: rebind.

**Prediction:** the original returns 0 on all 6,696 calls, so the port must return 0 on Training. That
is no behaviour change, and the memory rule "predict a port's output" is satisfied in advance.
- Risk: 0x00415880 calls the RNG 0x004a2c48. If the exe reaches that draw where the stub did not,
  ring phase shifts. Phase 1 counts the draws on the original first (count-it-before-a-witness).

**Value:** removes 2 stubs from the targeting chain. These are pure-CPU leaves, so a shared-TU body
plus `run_diff` / `run_verify_hook` gives a C3 path (fix-briefs-carry-a-promotion-leg).

### Not scheduled (yield under 30% today, or excluded)

| group | entry -> members | why not now | first producer to port |
|---|---|---|---|
| G-STEP | 0x00424eb0 -> 0x00408b00, 0x00409290, 0x0046c7d0 (+0x004093b0), 0x004704c0 (+0x0047ea60, 0x0046d400) | camera matrix `0x006146f0..0x006146fc`, `0x00613134`, `0x00803324`, `0x007f0fec`: NOWRITER. 0x004704c0 calls VehicleWheelContactSolve 0x0046f6c0 (D2-adjacent) | camera matrix block writer (Phase 1 datarefs) |
| G-CARREG | 0x0040da50 -> 0x00420870 family (0x0041f3d0/4f0/590/770/880, 0x00420340 -> 0x00420290 -> 0x0041f120), 0x0040d8f0 -> 0x0041f290 | 17+ inputs in the player/camera block `0x0063d7ec..0x0063dc7c` and `0x006146f0..` are NOWRITER | player-block writer |
| G-PU | 0x0045bba0 children: 0x0045a190 (pool J tick), 0x0041f060, 0x0045ba10, 0x0045c010 -> 0x0045bfa0, 0x00476880 | the powerups third is MET (risk to a met gate); pool J `0x0068b9b8..` has no writer (it needs a pool-J spawn, as M2a did); 0x0041f060 reads `0x0063da38` NOWRITER | pool-J spawn |
| G-RESULT | 0x0040d470 -> 0x00448220 (2,184 B), 0x0040e4b0 | modes third MET; 10 NOWRITER inputs in 0x00448220; 0x0040e4b0 reads slot table `0x005f2770` (leg C, inert) | none until a mode gate needs it |
| G-SUBSTEP | 0x00470c70 -> 0x00467350, 0x004709a0, 0x0046e9e0, 0x004809e0 | **D2 watch list** (substep loop 0x00470c70). D2 REOPEN CANDIDATE, report only | n/a |
| 0x00426c90 | 23 B, host 0x004103a0 | runs only under `MASHED_SUBSTATE`, which is OFF by user decision (A) | n/a |
| out of D3 | 0x0040ce00 world-object dispatcher (81), 0x00426ab0 render (12), 0x004222d0 (15), 0x0040bd80 (20), CRT / printf chain | render / particle / audio / boot, about 200 rows | user decision: separate lane |

## 4. Phase 1 proposal (one session, 2 groups)

1. **P0 producer** (D3 bulk G-EVQ prerequisite)
   - Write the `0x007f1030` frame stamp (and `0x007f0ff8`) from the exe step at the 0x0040fe24 order.
   - This is a default-OFF sub-knob of the G-EVQ knob.
   - Gate: G-CTRL `ae67b13a / c8317ce2 / 31db9384` with the knob OFF.
2. **G-EVQ**
   - Write `re/tools/naked_specs/evq.json` and generate the bodies.
   - Bind callees: verbatim / exe host / counted no-op with a witness.
   - Rebind the car records. Add exe-safe `.rdata` copies for `0x005cc568`, `0x005cd098`, `0x005cd0ec`
     and the rest of the plate's constants.
   - PcScope PC_24, reading the control word with mask 0 first (U-9216).
   - Add `--evq-probe` (live) and a Unicorn grid (synthetic), each with a negative control.
   - Knob `MASHED_EVQ`, default OFF.
   - Before trusting the seed, run the rel32 caller scan on 0x004252c0 and 0x00422ba0 for follow-up
     writes (memory caller-overwrites-a-verbatim-default).
3. **G-AILEAF**
   - Shared-TU bodies at the RVAs, `RH_ScopedInstall`, `run_diff` / `run_verify_hook`.
   - Knob `MASHED_AILEAF`, default OFF. Count the original's RNG draws first.

Phase 2 (a later session, one group at a time): PREREG before enabling, then G-CTRL, `run_h4` arms,
(b) per regime, `collateral.py --mode paired` against the same-arm repeat floor, then RESULT and
re-classify. Exe-only bodies stay below C3.

## 5. Open questions for the user

**User decision 2026-10-11:** (1) approved, Phase 1 = P0 + G-EVQ + G-AILEAF. (2) not answered; the non-D3 lane stays out of scope until asked. (3) commit the probe with this plan.

1. Approve G-EVQ + G-AILEAF for Phase 1? The third slot is empty, because no other D3 group clears
   30% without porting a producer first.
2. Open a separate non-D3 bulk lane for the about 200 executed render / particle / audio rows (D1
   residue or D4), or leave it out of scope?
3. The `--firsthit-probe` addition to `re/frida/scenario_launch.py` is uncommitted. Commit it with this
   plan?

## 6. Files

- `PLAN.md` (this file)
- Census: `candidates.tsv`, `batch_{a,b}.txt`, `fh_{a,b}.log`, `fh_{a,b}.msd.firsthit.tsv`,
  `executed.tsv`, `not_executed.tsv`, `groups.tsv`
- Inputs: `inputs_static.json`, `peek_addrs.json`, `peek_sa.json`, `peek_orig.json`,
  `peek_orig_launch.log`, `inputs_live.json`, `drefs_evq.json`
- `tools/`: the Phase 0 scripts. They read intermediate files from the session scratchpad (`cg.json`,
  `port.json`). Re-run `cg.py` first to rebuild them.
- `fh_{a,b}.msd` are gitignored (`verify/**/*.msd`).
