# Next session — kickoff prompt

## => KICKOFF PROMPT - D3: U-D3-DRIVE, the last gate on D3, written 2026-09-28d, paste verbatim

```
Session goal: close U-D3-DRIVE, which is now the ONLY thing gating ROADMAP D3
(user decision 2026-09-28). Read ROADMAP.md section D3 "D3 closure state
2026-09-28" and re/analysis/D3_SPEED_GAP_2026-09-28.md section 6.3. Do NOT
re-derive them.

SETTLED 2026-09-28d - POWERUPS (c) IS DONE. Do not re-open it.
- The 181 s2 decision mismatches are fixed. The cause was NOT the MISSILE chain:
  the dispatcher FUN_0045bba0 reads a per-slot BOX STATE DAT_0068d1f0[slot] at
  0x0045bc6b, BEFORE the armed test at 0x0045bcab, and values 4/2/3
  short-circuit the whole per-slot pass (0x0045bc75 / 0x0045bc85+0x0045bc98 RA
  0x45bc9d / 0x0045bca5). Ported into Powerup/PowerupSystem.cpp.
- The 2026-09-28c kickoff's hypothesis (the aim record 0x006885d0 + slot*0x2c)
  was REFUTED: FUN_00455150 does consult it at 0x00455163 and the port already
  matched that verbatim. The original never reached the FIRE call.
- Measured over all 11 captures: s2 decision 181 -> 0, contact DIVERGES -> CLEAN
  (sweep 0x45bcd8 152-vs-185 -> 152/152); the other ten unchanged; g3 still on
  exactly its R_FLAME 2-query residue. Control MASHED_PU_FORCE=nobox reproduces
  181. Second witness b3 via the new scenario_launch.py --pu-box.
- Record: re/analysis/D3_BOX_STATE_2026-09-28.md, verify/d3_box_20260928/,
  commits 2e7a2b92 + be06d381. New tool: re/tools/pu_replay/sweep.ps1 replays
  every capture and prints both verdicts in one table -- use it as the powerups
  regression guard from now on, it is one command.

THE REMAINING GATE, D3 criterion (e): under byte-identical ctrl bytes the ported
physics accelerates AI cars differently -- +25.5% / +6.8% / +5.2% full-throttle
median gain (cars 1/2/3) and a slow launch (+182 vs +1542/+2053 over the first 11
calls, all wheels grounded). D2's gate was measured on the PLAYER car only. First
suspect: the gearbox pair +0x490 / +0x494. The criterion requires the original's
OWN run-to-run spread to be measured and WRITTEN INTO ROADMAP.md BEFORE the
post-fix capture -- do that first, it is the part that is easy to skip.

ALSO CARRIED, not this session: D3-R1 (AI car 1, due before D5, route = port
FUN_00414c30 + FUN_00484c70) and the D1 residue block.

STILL OPEN on powerups, small, listed so they are not lost (D3_BOX_STATE
section 11): the box state's own MEANING is [UNCERTAIN U-9139] (naming only,
blocks nothing); the box gate's `== 2` arm is ported but only witnessed with an
EMPTY slot -- forcing the box to 2 while the subject HOLDS a type is a one-line
change to --pu-box; every capture in the lane is track 0.

RULES: NO-GUESSING, cite RVAs, [UNCERTAIN] + the next command. Trackers only via
re-classify. Launch muted (MASHED_MUTE=1), MASHED_WIN_POS=left-bl, always
--poke-ctrl-slots. Track the PIDs you spawn and kill ONLY those. Commit cited
evidence after each step (git add -f for small files; never commit a .msd, they
are 22 MB). Do not push.
```

## => HISTORY: KICKOFF PROMPT - D3 powerups, the MORTAR->MISSILE decision defect, written 2026-09-28c, SUPERSEDED (FIXED 2026-09-28d, and its stated hypothesis was REFUTED - see D3_BOX_STATE_2026-09-28.md)

```
Session goal: fix the ONE open powerup defect. It is in the DECISION half
(criterion (b)), NOT the contact half -- criterion (c) is CLOSED. Read ROADMAP.md
section D3 (Powerups row) and re/analysis/D3_CONTACT_PORT_2026-09-28.md sections
4.4b and 8. Do NOT re-derive anything below.

SETTLED 2026-09-28c. Criterion (c) is DONE: 8 clean + the dispatcher sweep + the
shared target acquisition, 1 near-clean.
- CLEAN: OIL, P_MINE, SHOTGUN, DRUM, FLASH, GUN, MORTAR, MISSILE.
- NEAR-CLEAN: R_FLAME -- 2 queries of 546 over on g3 only, cause named
  (FUN_0045ac40, the per-group sort whose reference point is the viewport query
  FUN_004671d0). Do not re-open unless you are porting that sort.
- Five ported modules: Powerup/PowerupContact.cpp (every leaf + the replay
  injectors), PowerupAim.cpp (FUN_00459620, the target ACQUISITION routine
  MORTAR/GUN/MISSILE share), PowerupMortar.cpp, PowerupMissile.cpp.

THE DEFECT, measured and attributed:
- Capture verify/d3_contact_20260928b/s2.msd (plan 11,7,11,11) is the first to
  put a MISSILE pickup AFTER a MORTAR one. Its 9-type DECISION replay reports
  181 mismatches, ALL on the third activation; the first MISSILE and the MORTAR
  before it are both CLEAN.
- Shape: from t+1 the PORT fires and the ORIGINAL does not --
  fire_modes orig='' port='2', ammo orig=1 port=0, jet orig=0 port=1,
  life orig=0.0 port=0.483.
- It is NOT a regression. `re/tools/pu_replay/build_control.bat 5bb0d5e3` links
  the pre-R_FLAME PowerupEffects.cpp against everything else current and
  reproduces the SAME 181 mismatches. Do not spend time bisecting; that is done.
- Consequence, so you do not chase the wrong row: the port holds that slot armed
  longer, so the dispatcher sweep fires 33 EXTRA times (0x45bcd8, 152 vs 185),
  and that single row is the WHOLE of s2's CONTACT VERDICT: DIVERGES. Every
  MISSILE, MORTAR and AIM row on s2 is clean. s1 (plan 11,11,11,11, no MORTAR)
  is clean throughout. Fixing the decision defect should clear the sweep row too.

HYPOTHESIS TO TEST FIRST (stated as a hypothesis, not a finding): the original's
MISSILE FIRE consults the AIM record and the port's does not. The tick tests the
aim record's +0x1c before calling FUN_00455100 (0x00455cc7..0x00455cd4), and the
aim pool is 0x006885d0 stride 0x2c, 5 entries. Start:
    py -3.12 re/tools/decomp_pc.py 0x00455150 0x00455100 --create --slot <n>
NOTE the --create flag: it was added this session and TRANSIENTLY defines a
function Ghidra's auto-analysis missed, against a -readOnly pool clone. It is NOT
a master write. 0x00455c90 (the MISSILE tick) is one such function; use it freely.

HOW TO WORK (the discipline that found three real defects this week):
1. Falsify the decode OFFLINE first, before any C++, the way re/tools/
   aim_model.py, mortar_model.py and missile_model.py do. Each found a real bug
   at zero cost.
2. ADD A NON-DEGENERACY CONTROL and RUN IT (MASHED_AIM_FORCE,
   MASHED_MORTAR_FORCE, MASHED_MISSILE_FORCE are the precedents). MEASURED: on
   MORTAR all three controls left every contact COUNT clean while the drift moved
   from 4.8e-07 to 6.03. A count the harness schedules is not evidence.
3. Report an inert control as inert. MISSILE's `nolife` is inert on both captures
   because no projectile aged out; that gap is recorded, not papered over.
4. Regression guard after every change, all of it:
     9-type:  o3, o4, m1, m2, s1 CLEAN (s2 is the one you are fixing)
     contact: c2, c3, g2, g4, m1, m2, s1 CLEAN; g3 DIVERGES on exactly the
              R_FLAME 2-query residue; s2 on exactly the sweep row
     mashedmoduild.bat -> both targets

ALSO OPEN, smaller (note section 8): MISSILE's 3.0 s lifetime gate has no control
coverage -- a capture with a missile that TIMES OUT would close it (item 3c). All
new capture channels are track 0 only, and the `hit_t` column exists on ONE
capture (item 8).

CAPTURE RECIPE:
  py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0
    --poke-ctrl-slots --statediff-out verify/<dir>/<name>.msd --statediff-car 0
    --statediff-drive --statediff-puhook --puhook-contacts --puhook-aim
    --puhook-mortar --puhook-missile --pu-plan 11,7,11,11 --pu-warm 60 --hold 110
Launch muted (MASHED_MUTE=1), MASHED_WIN_POS=left-bl, always --poke-ctrl-slots.
Track the PIDs you spawn and kill ONLY those.

RULES: NO-GUESSING, cite RVAs, [UNCERTAIN] + the next command. Trackers only via
re-classify. Commit cited evidence after each step (git add -f for small files;
never commit a .msd, they are 22 MB). Do not push.
```

## => HISTORY: KICKOFF PROMPT - D3 powerups (c), MISSILE (the last type), written 2026-09-28b, SUPERSEDED (MISSILE is done)

```
Session goal: close ROADMAP v3 D3 powerups criterion (c) by porting MISSILE, the
ONE type still owing its own chain. Read ROADMAP.md section D3 (Powerups row) and
re/analysis/D3_CONTACT_PORT_2026-09-28.md sections 4.2d-4.2g and 7. Do NOT
re-derive any of it.

SETTLED 2026-09-28b - 8 of 9 types are done. Criterion (c) stands at
7 clean + the sweep + the shared acquisition / 1 near-clean / 1 partial:
- CLEAN: OIL, P_MINE, SHOTGUN, DRUM, FLASH (makes no contact call), GUN, MORTAR.
- NEAR-CLEAN: R_FLAME - exact on c3 and g4, 2 queries of 546 over on g3, cause
  named (the unported per-group sort FUN_0045ac40, whose reference point is the
  viewport query FUN_004671d0 the replay has no equivalent for). Do not re-open
  unless you are porting that sort.
- PARTIAL: MISSILE. That is this session.

THREE PORTED MODULES to copy the shape of, in increasing order of relevance:
- Powerup/PowerupContact.cpp - every contact leaf, with injectors for the replay.
- Powerup/PowerupAim.cpp     - FUN_00459620, the target ACQUISITION routine
  MORTAR/GUN/MISSILE share (it is NOT a projectile routine; the older notes are
  wrong and say so now). MISSILE's acquisition is ALREADY COVERED by it:
  FUN_00455b50 calls it at 0x00455c29 with 8.0 range / 30.0 cone, and m1 measures
  0x459c19 32/32 and 0x459d54 32/32. Do not re-port it.
- Powerup/PowerupMortar.cpp  - THE EXEMPLAR. Same shape as MISSILE: a pool found
  off the tick's own loop bounds, a per-frame integrator, a detonation test whose
  verdict is injected. Copy it.

WHAT MISSILE NEEDS (map in section 4.2g, all measured or disassembled):
- Its tick is 0x00455c90 and it is NOT a defined Ghidra function, so there is no
  decompilation - only disassembly. Do NOT ask for a Ghidra master write: MORTAR's
  pool was found off its tick's loop bounds the same way. First command:
      py -3.12 re/tools/disasm_va.py 0x455c90 0x400
  It walks TWO interleaved pools in one loop: aim records at 0x006885d0 stride
  0x2c (4 entries; MOV EDI,0x6886ac @0x00455c9a, SUB EDI,0x2c @0x00455ca9) and
  projectiles at stride 0x6c (MOV EBP,0x688620 @0x00455c9f, SUB EBP,0x6c
  @0x00455caf). Find the EBP loop's bound to get the projectile count.
- FIVE own sites, one of which (0x455df9) is missing from every earlier list:
      0x455cd9 -> 0x00455100 impact        m1 31/30 hits, m2 12/12
      0x455e59 -> 0x004b4cd0               m1 31/31,      m2 12/12
      0x455de0 -> 0x004b4d10               m1 15/2,       m2 6/0
      0x455df9 -> 0x0045c350 gate          m1 2/2
      0x455e07 -> 0x00455910 terminal      g2 1/1
- The chain's shape is already disassembled (0x00455de3..0x00455e07): query
  0x4b4d10; JE skip; gate 0x45c350; JNE skip (NON-ZERO REFUSES, same polarity as
  MORTAR's - use Contact::ConfirmGateAt, not SweepConfirm); then the terminal.
- One leaf PowerupContact does NOT have: 0x004b4d10. Second command:
      py -3.12 re/tools/decomp_pc.py 0x004b4d10 0x00455910 0x00455100 --slot 0

HOW TO MEASURE IT (this is the part that makes it evidence, not a compile):
1. Add a --puhook-missile channel to re/frida/scenario_launch.py, modelled on
   --puhook-mortar: one row per projectile per frame with the record's PRE and
   POST state. Both existing channels emit floats as RAW HEX DWORDS; keep that.
2. Falsify the decode OFFLINE first, the way re/tools/mortar_model.py and
   re/tools/aim_model.py do, BEFORE writing any C++. Both found real bugs this
   way at zero cost. The detonation/impact verdicts are INPUTS taken from the
   contact capture - the world queries behind them are.
3. Port, wire into re/tools/pu_replay (kSites + the injector arrays + the
   not-armed rule for a capture that lacks the channel), and measure.
4. ADD A NON-DEGENERACY CONTROL, like MASHED_AIM_FORCE and MASHED_MORTAR_FORCE.
   This is not optional: MEASURED on MORTAR, all three controls leave every
   contact COUNT clean while the drift moves from 4.8e-07 to 6.03. Counts alone
   pass a broken integrator. If your measurement is a drift, fold it into the
   verdict as pu_replay now does for MORTAR.
5. Regression guard after every change, all of it:
      o3, o4, m1, m2   -> pu_diff.py, the 9-type decision replay
      c2, c3, g2, g4, m1, m2 -> pu_replay contact, all CLEAN
      g3 -> DIVERGES on exactly the R_FLAME 2-query residue and NOTHING else
      mashedmoduild.bat -> both targets

CAPTURE RECIPE (the two from this session, adapt --pu-plan):
  py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0
    --poke-ctrl-slots --statediff-out verify/<dir>/<name>.msd --statediff-car 0
    --statediff-drive --statediff-puhook --puhook-contacts --puhook-aim
    --puhook-mortar --pu-plan 11,11,11 --pu-warm 60 --hold 110
Launch muted (MASHED_MUTE=1), MASHED_WIN_POS=left-bl, always --poke-ctrl-slots.
Track the PIDs you spawn and kill ONLY those.

RULES: NO-GUESSING, cite RVAs, [UNCERTAIN] + the next command. Trackers only via
re-classify. Commit cited evidence after each step (git add -f for small files;
never commit a .msd, they are 22 MB). Do not push.
```

## => HISTORY: KICKOFF PROMPT - D3 powerups (c), the four projectile types, written 2026-09-28, SUPERSEDED by the 2026-09-28b prompt above

```
Session goal: close ROADMAP v3 D3 powerups criterion (c) for the FIVE types still
blocked. Read ROADMAP.md section D3 (Powerups row) and
re/analysis/D3_CONTACT_PORT_2026-09-28.md. Do NOT re-derive them.

SETTLED 2026-09-28 (D3_CONTACT_PORT_2026-09-28.md - read it, do not re-measure):
- The contact CHAIN is no longer the blocker. Powerup/PowerupContact.cpp ports
  0x004b4cd0 (and its 0x004b4b20 sibling), 0x004b4650, 0x004b5080, 0x0045c110 and
  the sweep pair 0x004b4b60 / 0x0045c350, with the BSP walk FUN_00538c80 and the
  RpMaterial colour channel as stated stand-ins.
- (c) is CLEAN, per call site, for OIL (10/10/10/10), P_MINE (2/2/2/2), SHOTGUN
  (8/8 query, 7/7 basis), DRUM (83/83 query on TWO captures) and the dispatcher's
  armed sweep (280/309/207/309). FLASH makes no contact call. P_MINE went 78
  decision mismatches -> 0 on c2. DRUM is the worked PROJECTILE exemplar: pool of
  8 records (orig &DAT_00688020 stride 0x44), state machine 0/1/2/3/4/5, flight
  pos+=vel*dt, vel.y+=-5*dt, life gate 5.0s. Copy its shape.
- RESOLVED: the gate that refused 6 of P_MINE's 7 press edges is the WORLD QUERY
  (JE 0x00457caa), not the surface gate. 0x0045c110 is now instrumented and has
  never been observed refusing a power-up drop.
- The four that remain (R_FLAME, MORTAR, GUN, MISSILE) each put their contact call
  inside a per-frame PROJECTILE/PARTICLE update, decoded with RVAs in section 5.3.
- READ SECTION 5.2 BEFORE PLANNING. A .text call-site scan settles the attribution:
  FUN_00459620 has THREE callers (0x00453bd9 MORTAR tick, 0x00455c29 MISSILE,
  0x004569c5 GUN tick), so the four sites the 2026-09-27 note listed under GUN
  (0x459c19, 0x459d54, 0x459db5, + 0x459c3c it missed) belong to whichever of the
  three is in flight - they CANNOT be attributed to GUN. FUN_00453730 (MORTAR),
  FUN_00454350 (DRUM) and FUN_0045b390 (SHOTGUN) each have exactly one caller,
  which is why DRUM and SHOTGUN were the two closable today.
- MEASUREMENT NOTE, refined by DRUM: a projectile's query COUNT is (frames in the
  flying state until the hit or the life gate). The spawn frame comes from the
  replayed press edge and the hit frame from the injected verdict, so the count is
  reproducible WITHOUT faithful trajectory - as long as nothing else can end the
  flight. Check that per type before assuming it.
- ATTRIBUTION, third rule, learned on DRUM: a dropped projectile OUTLIVES its slot.
  MEASURED on g2, slot 0 held DRUM for calls 1181..1190 but its 0x45444f queries run
  1182..1242. pu_replay site mode 1 attributes those by call range only, and the
  loader CHECKS that no other slot held the type (status `contested` otherwise).
- Tooling, do not rebuild it:
    py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
       --poke-ctrl-slots --statediff-out <out>.msd --statediff-car 0 \
       --statediff-drive --statediff-puhook --puhook-contacts \
       --pu-plan 11,7,10,12 --pu-warm 60 --hold 110       # c2 recipe
       # c3/OIL/SHOTGUN recipe: --pu-plan 19,16,18,9,17
    py -3.12 re/tools/pu_contact_report.py <out>.msd --slot 0
    re\tools\pu_replay\build.bat  then
    MASHED_PU_STEPDUMP=<p>.csv MASHED_PU_CONTACTDUMP=<p>.pucontact.csv \
      re/tools/pu_replay/out/pu_replay.exe <orig>.msd.puhook.csv 0
    py -3.12 re/tools/pu_diff.py <orig>.msd.puhook.csv <p>.csv --slot 0
  pu_replay prints a per-call-site table (orig vs port) and a CONTACT VERDICT. Add
  a new type by adding its sites to kSites / kQuerySites / kGateSites, with the
  owning type code and the gatedBy site. Reference captures:
  verify/d3_contact_20260928/g2 (P_MINE), g3 (OIL), g4 (OIL + SHOTGUN).
- THREE attribution rules the tooling enforces, all learned by getting them wrong:
  key every count on (name, ret_addr); attribute the dispatcher sweep by slot via
  arg2 = slot_base + 0x80; and attribute a PER-TYPE site by the subject slot's
  code_pre on that call - a .pucontact.csv row carries no slot, and g2 has three
  0x457ca5 rows of which only two are slot 0's.
- A site whose RVA is armed in no Frida listener is marked not-armed and excluded,
  and the mark propagates to sites it gates. Do not "fix" a not-armed row.

DO, in this order:
1. R_FLAME (FUN_0045ae80, the TICK already ported, single caller so attributable
   today) - 25 sparks (5 owners x 5 groups x 5), ballistic: miss -> vel.y -=
   _DAT_005ce018(0.002); hit -> lerp 0x45aff3, p += normal*_DAT_005ce18c(0.02),
   zero the velocity, latch pfVar8[5]=1, basis 0x45b04c. Copy the DRUM shape.
2. FUN_00459620, the SHARED projectile routine (2727 B). One slice that unlocks
   MORTAR, MISSILE and GUN; nothing about those three is measurable until it lands.
3. MORTAR's own detonation test FUN_00453730 once (2) is in.
4. MISSILE - Ghidra has no function at 0x00455cd9; create it first, then decode.

DO NOT: flip Vehicle/ForceIntegratorStubs.cpp:39 to the vectors form without a D2
re-measure (U-9138). Trackers only via re-classify. Launch muted (MASHED_MUTE=1),
always --poke-ctrl-slots. Track and kill only your PIDs. Re-run the 9-type
regression replay (verify/d3_pu_20260926/o3,o4) after every change.
```

## => HISTORY: KICKOFF PROMPT - D3 powerups (c), the contact chain, written 2026-09-27, SUPERSEDED by the 2026-09-28 prompt above

```
Session goal: close ROADMAP v3 D3 powerups criterion (c) for the 7 types still blocked on
the contact chain. Read ROADMAP.md section D3 (Powerups row) and
re/analysis/D3_CONTACT_2026-09-27.md. Do NOT re-derive them.

SETTLED 2026-09-27 (D3_CONTACT_2026-09-27.md — read it, do not re-measure):
- The old "blocked on Collision/ContactStubs.cpp" label is REFUTED. Powerup/*.cpp has no
  call path into Collision/*.cpp, and 3 of the 4 callers of Rw_TransformPoints are dead.
  Both stubs are now BOUND to the real ports and bit-exact; that changed nothing live.
- The actual blocker is 8 unported C2 RVAs: 0x004b4b60, 0x0045c350, 0x004b4cd0,
  0x004b4d10, 0x004b4650, 0x004b5080, 0x00455910, 0x00455100 — plus the scaffold
  IPowerupBackend. None has an implementation in the powerup path.
- Criterion (c) contact half, MEASURED per type: FLASH clean (no contact call exists),
  P_MINE DIVERGES (cited mechanism, below), the other 7 blocked WITH a counted reference.
- Captures + tooling exist, do not rebuild them:
    py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
       --poke-ctrl-slots --statediff-out <out>.msd --statediff-car 0 --statediff-drive \
       --statediff-puhook --puhook-contacts --pu-plan 11,7,10,12 --pu-warm 60 --hold 110
    py -3.12 re/tools/pu_contact_report.py <out>.msd --slot 0
  --hold 110, NOT 45: the race sub-state reaches 6 only around dispatcher call ~860 and a
  45 s run ends at ~776 with an empty .puhook.csv (verify/d3_contact_20260927/c1 is that
  negative control). Reference captures: verify/d3_contact_20260927/c2 (MISSILE, MORTAR,
  DRUM, P_MINE) and c3 (OIL, R_FLAME, FLASH, GUN, SHOTGUN).
- TWO ATTRIBUTION RULES the report enforces, both learned by getting them wrong first:
  key every count on (name, ret_addr) — 0x0045c350 ran 18 times in c2 but only ONCE from
  the dispatcher — and attribute the dispatcher sweep by slot via arg2 = slot_base + 0x80,
  not by activation window.

DO, in this order:
1. Port OIL's ground placement first — cheapest, and its reference is exact: one
   0x004b4cd0 (from 0x4578d1) / 0x004b4650 (0x457932) / 0x004b5080 (0x45797f) triple per
   drop, 10 drops, all 10 succeeding. Acceptance = the same three counts in the port.
2. Fix P_MINE. The port drops on every press edge; the original gates each drop TWICE
   inside FUN_00457c10 (reached only from FUN_00457ef0 on fire mode 2, CMP [ESP+0xc],2 /
   JNE at 0x00457efb..0x00457f00): JE 0x00457e08 at 0x00457caa on CALL 0x004b4cd0, and
   JNE 0x00457e08 at 0x00457cfe on CALL 0x0045c110, with 0x00457e08 the bare epilogue and
   the +0x08 decrement (0x00457d29..0x00457d2d) after both. Measured: 7 press edges, 1
   success, 1 decrement. FIRST add 0x0045c110 to PU_CONTACT in scenario_launch.py and
   re-run c2 — which of the two gates refused the other 6 is the one open [UNCERTAIN].
3. Then MISSILE (0x00455100 every in-flight frame from 0x455cd9, 0x004b4d10 from
   0x455de0, terminal 0x00455910 from 0x455e07), then MORTAR/DRUM (2 placements each),
   R_FLAME (15), GUN (163 per-frame), SHOTGUN (8).
4. Port the dispatcher's armed sweep (0x0045bcc8..0x0045bd11). It fires rarely — once in
   2364 slot-passes across c2+c3 — but that once is what deactivated P_MINE at call 1229.

DO NOT: flip Vehicle/ForceIntegratorStubs.cpp:39 to the vectors form without a D2
re-measure (U-9138 — latent defect, the D2 gate was closed against current behaviour).
Trackers only via re-classify. Launch muted (MASHED_MUTE=1). Track and kill only your PIDs.
```

## => KICKOFF PROMPT - D3 AI (b), written 2026-09-27 after the residue session, paste verbatim

```
Session goal: close ROADMAP v3 D3 AI criterion (b). Read ROADMAP.md section D3 (pass
criteria + the (b) tolerance, commit eebaf511) and re/analysis/D3_AI_RESIDUE_2026-09-27.md.
Do NOT re-derive them. Do NOT change the tolerance bands.

SETTLED 2026-09-27 (D3_AI_RESIDUE_2026-09-27.md — read it, do not re-measure):
- (b) re-captured on the post-4ff428ad MASHED_ROUND route: IDENTICAL result. Cars 2/3
  pass all 10 bands, car 1 fails the same 2 (c0/c1 distinct 7/82 vs 13..37/17..70).
- CAUSE LOCALISED. DAT_0089a368 is 0 for the whole standalone window and 1 for 159 of
  the original's 220 calls. It is set by a ONE-SHOT 20% roll (FUN_004177b0 tail
  0x00417c43..0x00417c7a; row = __ftol(DAT_0089a360) = 2, band 0, table 0x005f30a0
  row 2 = [20,40,60,75,100]). flag 1 -> BankSwitch sets line type 2 -> a different
  spline bank -> curvature median 106 vs 10 -> the `mode==0 && curv>20` steer
  multiplier at 0x0041665c saturates the magnitude -> few distinct c0/c1. Also
  accel 255 -> 102 at 0x004169e0.
- REFUTED: (i) "the ported lookahead target is too close" — that was an artifact of
  own_x/own_z reading 0 at 0x008815a0+v*0xd04+0x30; with the record pointer taken from
  FUN_0046d4a0 the two sides agree to within 15% on the max. (ii) "the phase-8
  wall-march rejects the target" — march_n == 1 on 220/220 on BOTH sides.
- U-D3-AIRAND RESOLVED: FUN_00534990 seeds the ring from a hard-coded constant with no
  entropy, so FUN_00534870 is now ported VERBATIM (the LCG stand-in is gone). It does
  NOT close (b) — the call index cannot match, only the distribution.
- Both step dumps now carry the step INPUTS: look_x, look_z, curv, own_x, own_z,
  hist_d8, hist_dc, march_n, march_idx0 (+ standalone-only look_best/look_idx/look_blk).
  Compare with: py -3.12 re/tools/ai_step_compare.py <orig.aistep.csv> <sa.csv>
- Frida note that cost a run: Interceptor.attach is an ENTRY hook (it swaps [esp]), so
  it CANNOT be pointed mid-function. Probing 0x004165a5 and 0x0041657c each killed the
  game in 2 ticks; MASHED_AISTEP_LOCALS=0 is the control.
- MASHED_AI_DIFFFLAG=<n> seeds DAT_0089a368 at race reset. Diagnostic only, default OFF.

DO, in this order:
1. Decide (b) car 1 on evidence, not by moving a band. Either (a) port FUN_00414c30 so
   modes 3/7 exist — the original's car 1 is in mode 7 for 115 of its 220 window calls —
   which needs the world-object query FUN_00484c70 (stride 0x23 dwords) plus
   FUN_0041f030 / FUN_0048a630 / FUN_00414300 / FUN_00414490 / FUN_00442cc0; or (b)
   take 4 more ORIGINAL captures and establish whether the pooled envelope already
   spans both DAT_0089a368 regimes, then state plainly whether the standalone lands
   outside both. Whichever you pick, say so in the note before you run it.
   FUN_00416060 (LOS) is a 20-line port on top of the existing TileBlocked, but it
   cannot change a byte until a producer returns non-zero — do not land it alone.
2. Brake share 0.005 standalone vs 0.109-0.268 original, unchanged by the RNG port AND
   by MASHED_AI_DIFFFLAG=1. Rule 0x00416818 = (X > 20) && (rec+0x9e4 > 2000). X is
   already derivable from hist_d8/hist_dc in both CSVs — find which conjunct fails.
3. NEW, open: the standalone's opponents run 16-23% fast (rec_9e4 median 2818-2822 vs
   2299-2423). No (b) band tests it. Check it against the D2 physics capture first — it
   may be a physics residue, not AI.
4. FUN_00442a60 is a SPECTATOR-CAMERA routine (it picks a car pair with FUN_0040e180
   before filling 0x008989b0). Until FUN_0040e180 exists the array stays 0 and
   MORTAR/DRUM/P_MINE/R_FLAME/SHOTGUN cannot pass their fire gates.
5. MODES residues — unchanged, D3_MODES_2026-09-26.md section 6.

RULES (unchanged): cite RVAs, NO-GUESSING, [UNCERTAIN] + next command; always
--poke-ctrl-slots; launch muted; kill only your PIDs; trackers only via re-classify;
C4 needs a canonical run with the hook live; never dereference a pointer read out of
the image-pad.
```

## => D3 2026-09-27 — AI (b) still open, cause localised to DAT_0089a368

> Record: `re/analysis/D3_AI_RESIDUE_2026-09-27.md`. Evidence: `verify/d3_ai_20260927/`
> (`o4.msd.aistep.csv` original, `s6rng.csv` standalone, `s5flag.csv` the flag A/B).
> Two hypotheses refuted, U-D3-AIRAND resolved and `FUN_00534870` ported verbatim.

## => HISTORY: KICKOFF PROMPT - D3 continuation (written 2026-09-26 after the AI port; modes item updated 2026-09-26 after the modes session), superseded

```
Session goal: close ROADMAP v3 D3. Read the D3 STATUS block and pass criteria in
ROADMAP.md, then re/analysis/D3_AI_PORT_2026-09-26.md. Do NOT re-derive them.

SETTLED 2026-09-26 (D3_AI_PORT_2026-09-26.md):
- The ported AI tick FUN_00418860 runs every race frame and its ctrl bytes drive the
  opponents through the ported physics chain (StepCar, raw_steer). MASHED_AI_TICK is
  deleted. AI criteria (a), (c), (d) met.
- AI criterion (b), tolerance in ROADMAP (written before the capture, commit eebaf511;
  checker re/tools/ai_ctrl_window.py --check): cars 2 and 3 pass all 10 bands, car 1
  fails 2 (c0/c1 distinct 7/82 vs 13..37/17..70). NOT met.
- The 2026-09-14 bang-bang steer had a larger cause than the stubs: the AI clock
  DAT_007f1008/DAT_007f0ff4 was never advanced standalone. Fixed (Ai_AdvanceClock).
- Powerups criterion (b) met: slots 1, 2, 3 armed and fired in a 180 s race
  (verify/d3_ai_20260926/sa4_pu.csv). Only OIL was on offer, so only that branch of
  AiFireDecision (FUN_00415220) is observed live.
- The original is NOT deterministic run to run: 2 of 5 captures take the
  DAT_0089a368 == 1 accel x0.4 path.
- MODES (a)-(d) all met (re/analysis/D3_MODES_2026-09-26.md): live oracle covers all
  11 rules at 0 mismatches, APPEND fired (rules 4/7/8/9; rules 0-3/5/6/10 never
  append), G-G1 = FUN_0040b180 score seed (6, or 4) + FUN_00410510 target, ported,
  G-G2 = engine on by default on every route. Also REFUTED + fixed: the rule-10
  countdown IS re-seeded per round (FUN_004046a0 caught live).
- WATCH for AI (b): the MASHED_ROUND capture route used by sa2 ran the LEGACY
  collapse with scores seeded 0. It now runs the rule engine with scores seeded 6, so
  rounds and matches end at different times than in sa2. Re-capture the standalone
  before comparing against sa2-era numbers; the 220-call window itself is unchanged.

DO, in this order:
1. AI (b) car 1. Add the lookahead target (look[]) and the FUN_00443440 curvature to
   BOTH dumps (scenario_launch.py --statediff-aistep reads them at onLeave of
   FUN_00416250; the standalone AiStepDump) and compare the err / curvature
   distributions car by car. Candidates, none measured: the original's start launch
   (+0x9e4 0 -> 1164 in 6 frames), the stubbed targeting modes, the FUN_00534870 RNG
   stand-in. Re-check against the SAME tolerance; do not move the bands.
2. Brake fraction: standalone 0.5% vs original 11-30% of window calls (inside the bands,
   but a real gap). Same instrument as item 1.
3. Targeting chain (modes 1..10): FUN_00414570/15880/14a70/14c30/150e0/14f00/148b0/
   15020 + LOS FUN_00416060 + wall FUN_00415d00. The original reaches modes 3 and 7.
4. FUN_00442a60 (the 0x008989b0 reference distances). Until ported, MORTAR/DRUM/P_MINE/
   R_FLAME/SHOTGUN never pass their fire gates. Then run a track whose orbs are not all
   OIL and show those types fire.
5. MODES residues (none blocks the modes gate; D3_MODES_2026-09-26.md section 6):
   feed team play into the frontend route (SetTeamPlay is dev-route only, so a menu
   team game seeds 6 not 4); FUN_0040b420 decomp for the -1000 delta seed; rule-5
   counter poke ends the process when total != registered count (decomp FUN_00406ce0);
   motion0 exits of rules 8/9/10 never reached.

RULES (unchanged): cite RVAs, NO-GUESSING, [UNCERTAIN] + next command; always
--poke-ctrl-slots; launch muted; kill only your PIDs; trackers only via re-classify;
C4 needs a canonical run with the hook live; never dereference a pointer read out of
the image-pad.
```

## => D3 2026-09-26 — modes (a)-(d) met

> Record: `re/analysis/D3_MODES_2026-09-26.md`. Evidence: `verify/d3_modes_20260926/`.
> Harness: `scenario_launch.py --oracle` now records `seen` (per-rule entry-input ranges
> over every SegmentCheck call) and `seed10` (every FUN_004046a0 exit). Rule-10 runs need
> `--mode 3` (the countdown ticks only in modes 3/4/5). Rule-5 runs need a KTC_NewCopter
> track (Arctic `--track 3`) and a collect poke whose total equals the registrar's count.

## => D3 2026-09-26 — AI port landed; AI (b) 2 of 3 cars; powerups (b) met

> Gate table in ROADMAP section D3. Record: `re/analysis/D3_AI_PORT_2026-09-26.md`.
> Open [UNCERTAIN] items with next commands are in its section 6 (U-D3-AIRAND,
> U-D3-DIFF360, human slot state).

## => HISTORY: D3 continuation kickoff (written 2026-09-14, powerups updated 2026-09-26), superseded

```
Session goal: close ROADMAP v3 D3. The D3 step-1 audit and the AI + modes measurements
are DONE (2026-09-14). Read the D3 STATUS block in ROADMAP.md first, then the three
notes; do NOT re-derive any of it.

WHAT IS SETTLED (evidence in the tree, not in a ledger):
- re/analysis/D3_AUDIT_2026-09-14.md - what a clean-env race actually runs, per
  subsystem, plus every MASHED_* read in Ai/, Powerup/, Race/ and exe_main's mode block
  classified. Headline: NO flag reachable in a clean-env race is scaffold-selecting, so
  D3 step 5 is a port task, not a flag inversion. Section 6 carries corrections made
  after the measurements - read it, it amends section 1.3.
- re/analysis/D3_MODES_2026-09-14.md - modes are GREEN. The live rules oracle
  (scenario_launch.py --oracle) found 0 mismatches over 1290-3963 calls per function for
  rules 0/1/2 against FUN_00410d10 / FUN_00410510 / FUN_004177b0.
- re/analysis/D3_AI_TICK_WIRING_2026-09-14.md - AI is RED and the cause is LOCALISED.

POWERUPS MEASURED 2026-09-26 (re/analysis/D3_POWERUPS_2026-09-26.md, read it, do not
re-derive): scenario_launch.py --statediff-puhook captures the original's dispatcher
FUN_0045bba0 per frame; re/tools/pu_replay + re/tools/pu_diff.py replay those inputs
through the port TUs. All 9 types' decision traces are CLEAN, floats bit-exact (7 of 9
were wrong and are fixed). G-D1 is CLOSED (per-frame, 4 slots, live in the exe).
Criterion (b) is NOT met: AI slots can own but cannot fire, because the AI's fire decision
FUN_00415220 (writes ctrl[7], the byte the dispatcher reads) is stubbed in the ported AI
tick. Contact outcomes (MISSILE/MORTAR/DRUM/P_MINE impacts) stay BLOCKED on
Collision/ContactStubs.cpp.

THREE THINGS TO DO, in this order.

1. POWERUPS residue, small: (i) confirm AI orb ownership live -- no car, player included,
   collected an orb in a 60 s Training drive demo (D3_POWERUPS section 6 item 2 has the
   diagnostic to add); (ii) FUN_00415220 is part of the AI port below, and once it writes
   ctrl[7] re-run the standalone with MASHED_PU_STEPDUMP and check slots 1..3 FIRE rows.

2. THE AI PORT - FUN_00443300 and the FUN_00443dc0 curvature-walk/wall-march tail,
   plus FUN_00415220 (power-up fire decision, 8 MOV [EDI+7],1 sites from 0x0041536c).
   Note: the aistep CSV's c3 column is block+3, NOT the fire byte; fire is block+7
   (AiState.h corrected 2026-09-26).
   This is the named critical path and the measurement that named it is in
   D3_AI_TICK_WIRING section 4.1: the verbatim steer bands are FAITHFUL, their INPUT is
   not. With those two stubbed the bearing error sits in the bands' 30..180deg
   full-steer range, so the standalone commands 2-3 distinct steer values per car where
   the original commands 33-96. Port them, then re-run the diff:
     original:   py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4                    --car 0 --poke-ctrl-slots --statediff-out verify/<d>/o.msd                    --statediff-car 1 --statediff-aistep --hold 60
     standalone: py -3.12 re/tools/sa_capture.py verify/<d>/sa 8,18 MASHED_MUTE=1                    MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1                    MASHED_AI_TICK=1 MASHED_AI_STEPDUMP=verify/<d>/sa.csv
   Both CSVs share columns, so they diff directly. Baselines from 2026-09-14 are in
   verify/d3_ai_20260914/.
   Then: consume the ctrl bytes (the tick produces them today, the Option B motion model
   at TrackRenderer.cpp:2831-2960 still drives), and DELETE MASHED_AI_TICK - it is a
   default-OFF gate on a ported behaviour, which is exactly what D3 exists to remove.
   Smaller AI residues: the original-side accel distribution is [UNCERTAIN] because the
   capture race was degenerate (D3_AI_TICK_WIRING section 4.3 has the next command); and
   CarSlotStateSet is a guarded no-op standalone (section 2).

3. MODES blind spots, to turn GREEN into CLOSED (D3_MODES section 3): the finish-order
   APPEND branch never fired in any of five runs (ord.appends == 0, so the 3.0 threshold
   and the -1.0 slot sentinel are untested - --poke-lap 0:3 did NOT move it), and only 3
   of 11 rules are covered - run --rule 5 --poke-collect and --rule 10 --rule10-timer,
   which exist for those two pre-blocks. Plus G-G1 (RaceSession.cpp:118 hardcodes
   StartMatch(3) instead of deriving the round target from the rule) and G-G2
   (RaceSceneState.h:273 rule_engine_on_ defaults false).

RULES (unchanged):
- Cite RVAs for every original claim; NO-GUESSING; mark [UNCERTAIN] with a next command.
- ALWAYS pass --poke-ctrl-slots on any AI-behavioural capture. Without it the launcher
  leaves 0x007f1a14[0..3] at zeros and all four cars write controller 0's ctrl block
  (measured 2026-09-14). Physics captures that drive block 0 via the cook injector are
  unaffected.
- Never dereference a pointer READ OUT OF the standalone's image-pad: the pad is
  zero-filled, so a .data pointer constant the original loads is 0 there. That is what
  AV'd the first tick wiring (CarSlotStateSet, 0x005f2770).
- Track the PIDs you spawn; kill only those. Every launch muted.
- Trackers only via re-classify; C4 needs a canonical run with the hook live.
- Record findings as re/analysis/D3_*.md notes shaped like the A8 follow-ups
  (measured / refuted / open). Update re/NEXT_SESSION.md at the end.
```

## => D3 2026-09-14 — audit + AI + modes done; powerups NOT measured

> Gate table in ROADMAP section D3. **Modes GREEN** (oracle, 0 mismatches over 1290-3963 calls
> per fn, rules 0/1/2). **AI RED**, cause localised to the stubbed `FUN_00443300` /
> `FUN_00443dc0` curvature-walk tail. **Powerups OPEN** — never measured. Two reusable findings:
> `scenario_launch.py` left the AI output-slot table at zeros so all four cars shared one ctrl
> block (`--poke-ctrl-slots` fixes it), and a pointer read out of the standalone's image-pad is
> never safe to dereference (it AV'd the first tick wiring).

## => Roadmap reconciliation 2026-09-26 (docs only, no code)

> ROADMAP.md now gives D3 explicit **pass criteria per third** (§D3). Use them as the
> acceptance line and **write the AI tolerance into §D3 before taking the post-port
> capture**. D1 is split into CLOSED (default flip) plus **D1-residue R1-R3** (owed before
> D5). RE_MASTER_PLAN is frozen, and its user decisions are now DEC-2/4/6/7.

### Housekeeping backlog (small, not D3-gating; do these in a gap or a dedicated session)

- ROADMAP open decision #1: QoL strand tracker row + reclassify borderless (librw P5) as port work (via re-classify).
- ROADMAP open decision #2: re-measure the collision-FX skid thresholds now that real physics is the default (DUE since 2026-09-14).
- ROADMAP open decision #4: sweep tracker citations into untracked `log/` paths (commit or re-cite).
- Stale strings `RaceSession.cpp:85` ("gate-ribbon AI") and `:87` ("effects TODO"). Check whether they are still present, and fix them.
- `hooks.csv` retag `0045bba0` util→powerups when it is next touched.
- 182+ commits ahead of origin. Push when Mariano says so.


## => HISTORY: original D3 kickoff (superseded by the continuation prompt at the top), written 2026-09-14

```
Session goal: ROADMAP v3 D3. Gate: a clean-env race (NO MASHED_* variable set) where
opponents, powerups and mode rules are all the ported implementations, measured against
the original. Phase order: audit what the default build ALREADY runs -> measure each of the
three against the original -> port/wire only the named gaps -> invert any flag that still
selects a scaffold (a flag may only turn a ported behaviour OFF). Do not re-port what is
already verbatim; the WS ledger rows for WS-C/WS-D/WS-G in ROADMAP.md are STALE (2026-06-16)
and the code is ahead of them.

STATE YOU INHERIT (verified from the source 2026-09-14, not from the ledger):
- D1 (librw) closed 2026-08-19; D2 (physics) closed 2026-09-14: the ported RWP-3.7 chain
  with A4->A5->A6a before the substep loop is the default; MASHED_REAL_PHYSICS=0 and
  MASHED_A8_A4_FIRST=0 are A/B reverts only. Evidence + method (transcribe a law to Python,
  run it on the ORIGINAL's record fields, compare with what the original stored):
  re/analysis/data/A8_velocity_vector_motion_20260825.md follow-ups 25-30. Reuse the
  method; it found the D2 mechanism in one session after 24 follow-ups had not.
- WS-C AI: the opponent AI is ALREADY the default. Ai/AiStandalone.cpp ports the tick
  spine FUN_00418860 -> per-vehicle FUN_00418560 -> control step FUN_00416250 (+ mode
  tails FUN_00416a30/FUN_00417da0), steering FUN_00415e20, target seed FUN_004161e0,
  lookahead FUN_00443dc0, on the real AI<course>.AI line (Ai_BridgeLoad,
  TrackRenderer.cpp:1809-1818). MASHED_GATE_RIBBON_AI=1 is the scaffold REVERT. Residual
  A/B knobs: MASHED_AI_PUREPURSUIT, MASHED_AI_STEERFLIP, MASHED_AI_NAV (AiStandalone.cpp
  :439/:500/:513), MASHED_AI_DRIVES_PLAYER (TrackRenderer.cpp:2662). hooks.csv: 77 ai
  rows, FUN_00418860/FUN_00418560/FUN_004177b0 at C3. The header says "PENDING
  diff-original C4". Open residue in the file's ledger: [U-C-STEER-MAG] the ROUND(ST0)
  magnitude, [U-C-RATE0/1] the two rate floats FUN_0046d6a0/6d0 (speed substituted,
  rate1=0). RaceSession.cpp:85 still says "gate-ribbon AI" - stale text.
- WS-D powerups: dispatcher FUN_0045bba0 + lookup/activate/deactivate + the 9-entry type
  table (orig 0x005f9998, stride 0x40) are VERBATIM in Powerup/PowerupSystem.cpp; the 9
  per-type DECISION functions (ammo/cooldown/fire-mode/charge/jet) are verbatim in
  Powerup/PowerupEffects.cpp; every LEAF (SpawnMissile, SpawnMortar, DropHazard,
  HitscanForward, SpreadCone, FlameJet, BlindFlash, DropOilSlick, DEACT teardown) goes
  through IPowerupBackend, implemented by PowerupBackendImpl in TrackRenderer.cpp:3070+
  as STANDALONE reimplementations (renderer-side projectiles/FX), not ports of the
  original leaves (orig pools DAT_006883bc stride 0x6c / DAT_00684ea8 stride 0x110,
  RwFrameAddChild attach, FX FUN_00465e80/FUN_00465ca0, contacts FUN_004b4cd0 etc.).
  The orb economy (collect/respawn) is the PickupField scaffold, data-faithful to the 9
  codes. Map: re/analysis/structs/powerup_system.md (sections 8 input bridge, 9 gating).
  Blockers recorded there for verbatim leaves: Ghidra fn-split of 0x00453f60-0x0045be81,
  projectile-pool struct map, WS-B car<->projectile contacts. RaceSession.cpp:87 "effects
  TODO" is stale. hooks.csv: 0045bba0 tagged `util` C2 - retag to powerups when touched.
- WS-G modes: Race/RaceModes.cpp is a verbatim transcription of game-mode -> race-rule
  (WS-G1, re/analysis/game_mode_rules_REmap_20260616.md; cup table 0x005f65c8), and
  Race/RuleEngine.cpp transcribes the rule predicates (0x00405890, 0x004177b0 finish
  order, ...). exe_main.cpp:2236-2256 derives raceRule/raceMode from game length via
  RaceModes; MASHED_RACE_MODE / MASHED_LAPS / MASHED_GAME_LENGTH are dev overrides applied
  AFTER the real derivation (allowed). Ledger says "no work since 2026-06-16" - stale.
  What is NOT known: whether every mode's rule set (elimination zoom-sat, lap targets,
  points, cup progression) is the ported one at runtime, or a scaffold path in
  RaceSession/GameFlow (RaceSession.cpp:1 "scaffold impl", GameFlow.cpp:57 [SCAFFOLD]
  area grouping) still decides outcomes.

TASK, in order:
1. AUDIT (no Ghidra, no game): for each of AI / powerups / modes, list what runs in a
   clean-env race and which file decides it; list every MASHED_* read in Ai/, Powerup/,
   Race/, GameFlow and the mode block of exe_main.cpp, and classify each as
   revert-only (fine), scaffold-selecting (must be inverted or deleted), or dev-override
   (fine). Write it as re/analysis/D3_AUDIT_2026-09-14.md. Fix the three stale ledger
   strings in RaceSession.cpp (:85 :87) and ROADMAP's WS-C/D/G rows from the audit.
2. AI MEASUREMENT: original-side capture of an AI car's record with the existing
   statediff tool (`re/frida/scenario_launch.py --statediff-out ... --statediff-car N`,
   N = an opponent slot; the player capture recipe is in
   verify/a8_steer_20260824/orig_steerR.msd.provenance.json) on a normal race (no drive
   injector), and the standalone's AI on the same track/car; compare the CONTROL BYTES
   the AI writes ([0]/[1] steer, [4] accel, [5] brake - re/analysis/ai_ctrl_byte_map_
   RESOLVED_2026-06-16.md) per frame against track position, and the lap time. That is
   the AI's C4-shaped evidence and the D3 gate for opponents. If the bytes disagree,
   apply the D2 method: transcribe the control step to Python, run it on the original's
   AI record fields, find the input that differs.
3. POWERUPS: the leaves are standalone reimplementations by design until the recorded
   blockers land. For D3 the gate is BEHAVIOUR, not verbatim: for each of the 9 types,
   one original-side capture of a fire event (player fires; record + projectile pool
   fields) vs the standalone's, comparing the DECISION outcome (ammo decrement, cooldown,
   fire-mode transition - these are verbatim and must match exactly) and the leaf's
   observable (projectile spawned/hazard placed/flash applied, hit on the car ahead).
   Start with OIL and FLASH (fewest vehicle-field dependencies, per powerup_system.md
   section 9), then GUN/SHOTGUN (hitscan), then MISSILE/MORTAR/DRUM/P_MINE (need
   car<->projectile contact: if WS-B contacts are still stubs - Collision/ContactStubs.cpp
   still stubs Rw_TransformPoints (identity) and Rw_MatrixFromAxisAngle (no-op) for the
   CarWorld/CarCar solvers, found 2026-09-13 - record that as the blocker, do not fake it).
4. MODES: for each game mode the frontend can select, one clean-env race to its natural
   end on the standalone and the same on the original (scenario_launch --mode/--cars);
   compare the outcome fields the RuleEngine transcribes (0x0063a5d0/0x0063a5d4 counters,
   finish order 0x0089a870.., points, elimination) at race end. Any mode whose outcome is
   produced by a RaceSession/GameFlow scaffold path instead of RaceModes/RuleEngine is the
   port target; wire the transcribed rule and invert its selector.
5. Only after 2-4: invert or delete every scaffold-selecting flag found in step 1 (the
   ribbon AI hatch may stay as a revert). Re-run one clean-env race and record the gate
   table in ROADMAP section D3 the way section D2's CLOSED block does.

RULES:
- Cite RVAs for every original claim; NO-GUESSING; mark [UNCERTAIN] with a next command.
- Track the PIDs you spawn; kill only those (a8_run_port.py and scenario_launch.py do).
- Every launch muted (both launchers already set MASHED_MUTE=1).
- Trackers only via re-classify; C4 needs a canonical run with the hook live (AI rows
  are C3; a matching control-byte diff on a real race is the C4-shaped evidence).
- Record findings as re/analysis/D3_*.md notes with the same shape as the A8 follow-ups:
  what was measured, what was refuted, what is open. Update re/NEXT_SESSION.md at the end.
```
## => D2 CLOSED 2026-09-14 — ported physics is the default build's drive model

> `MASHED_REAL_PHYSICS=0` and `MASHED_A8_A4_FIRST=0` are the only remaining uses of those flags (A/B
> reverts). Clean-env held-lock run matches the original (thirtieth follow-up; ROADMAP §D2 CLOSED block).
> NEXT CANDIDATES, in order: (1) ROADMAP open decision #2 — re-measure the collision-FX skid thresholds
> now that `vel[]` is real (`TrackRenderer::EmitCarFx`, calibration caveat at TrackRenderer.cpp:4245);
> (2) D3 — default AI / powerups / modes (WS-C dispatcher FUN_00418860 family, WS-D FUN_0045bba0 +
> 9-entry table, WS-G mode rules); (3) delete the kinematic scaffold once the A/B is no longer needed.


## => D2 slip-angle: MECHANISM FOUND 2026-09-13 (twenty-sixth follow-up in the A8 data note)

> **The port ran A4/A5/A6a AFTER the substep loop; the original runs them BEFORE it** (step 3 vs step 5
> of FUN_00470c70). With `MASHED_A8_A4_FIRST=1` the port reproduces the original on the held-lock recipe:
> slip 0.192/0.263 vs 0.191/0.250 (fwd), 0.149/0.221 vs 0.147/0.205 (wheel axis), av.y 1.12/1.58 vs
> 1.14/1.46, the one-frame axis phase (+0.0425 vs +0.0446), and 12 reseeds and no spin-out vs 40 reseeds and 87 spike-window
> rows in the same-build control. Every A6a law was verified on both sides first (a8_wheelvel_orig.py,
> a8_angvel_orig.py, a8_angvel_port.py: 0.99 / 0.998 one-frame predictions).
> **DONE 2026-09-13:** A4-first is the DEFAULT (`MASHED_A8_A4_FIRST=0` reverts). Clean-env held-lock run
> matches: slip 0.192/0.263 vs 0.191/0.250, speed 1874 vs 1901 (twenty-seventh follow-up). Ramp regime:
> port ~10% ABOVE at full lock; old-order ramp control not obtainable (exits ~14 s in, no frames).
> DONE (twenty-eighth follow-up): original-side ramp capture taken with the port's schedule — the original
> crashes, respawns and wedges within 2 s of half steer; the port's collision scaffold keeps it driving. The
> ramp is a WORLD-level mismatch (D1/D3), not a physics one. scenario_launch.py now has
> `--statediff-steer-schedule` and magnitude steer bytes. NEXT is the owner's D2 decision on the held-lock
> evidence alone. Gentler two-sided ramp TRIED (twenty-ninth follow-up): three more original captures; the
> original's launch at full lock from standstill spins out at ~1.5 s and sometimes stays stopped until the
> steer is released (partial bytes exonerated: 33.87x0.75 = 25.37 deg lands exactly). Under wall-clock input
> timing no schedule repeats, so the port-side knob was NOT added. Prerequisite for any ramp comparison:
> frame-anchored injection on the original (steps keyed to the 0x004c1be0 tick counter), then the port knob.
> ~~NEXT: an ORIGINAL-side ramp capture (re/frida scenario capture, .msd) for a like-for-like ramp comparison;~~
> then the owner decides D2's close. Old text follows:
> ~~**OWNER CALL NEEDED:** make the original order the default (v3 default-build rule) and re-gate D2 on~~
> the standing SLIP metric, which now passes; then the D2 gate reduces to the ramp regime re-run.

## => D2 slip-angle session 1 done (superseded by the twenty-sixth) 2026-09-13 (twenty-fifth follow-up in the A8 data note)

> **Read section 6 of the twenty-fifth follow-up for the next measurement.** Settled: the orientation half
> MATCHES (both sides rotate the body from steer x grip, not from +0x9c0; the prompt below is STALE on that
> point - the "alignment block" no longer exists). The per-wheel force law matches on all four wheels. What
> differs: the A6a angular-velocity state +0x9c0 is 22-42% low in the port at equal body rotation, and the
> rotation-path wheel-point velocity term weighs half as much per unit av on the port. The 500-1000 "5x" is a
> regime mismatch and is withdrawn. Tools: a8_orient.py, a8_wheelfit.py, a8_run_port.py; capture
> verify/a8_orient_20260913/. Next: port Rw_MatrixFromAxisAngle + the wheel-point velocity to Python and
> evaluate it on the original record; then log block-#5 torque and #6 damping per frame on the port.

## (stale on the orientation point) KICKOFF PROMPT - D2 slip-angle session (written 2026-09-13)

```
Session goal: explain the A8 slip-angle deficit on ported physics (ROADMAP section D2,
"RULING 2026-08-26"). D2 stays gated on SLIP, not trajectory. Do not re-gate it. Do not
close it by inventing a mechanism. Phase = measure -> localise; port only if the
localisation names a specific line.

STATE YOU INHERIT (do not re-derive):
- Default build: librw is the default renderer (D1 closed 2026-08-19). Physics default is
  still the kinematic scaffold; MASHED_REAL_PHYSICS=1 selects the ported RWP-3.7 chain
  (Vehicle/VehiclePhysicsRun.cpp:203). All 83 zeroed physics constants are fixed
  (53e5c05d); the car steers (9cc41fa8); top speed ramps in the stock shape (8917e29c).
- On matched full-lock inputs EVERY trajectory quantity matches the original: turn radius
  within 3-11%, yaw rate within 4-10%, ramp-run speed within 1% (1778 vs 1760), summed
  per-wheel force magnitude and direction and the grip chain within 4-22%.
- Median slip angle is 1.36x-4.12x SHORT (worst at 500-1000 speed, ~5x; ~1.3x at
  1500-2600). Numbers: re/tools/statediff/a8_momentum.py header and
  re/analysis/data/A8_velocity_vector_motion_20260825.md follow-ups 19-24.
- EIGHT causes are ELIMINATED BY MEASUREMENT. Do not re-test them: (1) grip/clamp chain
  (l_60/ld4/le4/grip match), (2) per-wheel force magnitude, (3) force direction (lateral
  fraction within 2%), (4) the constants (5 wrong-bit literals fixed, sub-0.02% effect),
  (5) force->velocity application (velocity-turn momentum identity gives the same
  effective dt on both sides, confirmed on three regimes), (6) steer-regime mismatch
  (matching regimes changed nothing), (7) a tighter turn radius (radius matches),
  (8) the off-mesh/reseed rate as a fidelity signal (it is a GroundHeight
  collision-scaffold artifact; both sides report gnd=4.0 in every frame).
- ONE PARTIAL LEAD, not yet run to ground (twenty-second follow-up): a body-basis reseed
  ZEROES slip and it takes >12 frames to rebuild; ~29% of port driving frames sit in
  that window. It explains part of the 1000-2000 bands and NOTHING at 500-1000 or
  2000-2600. g_bodyBasisReseed is set only by VehiclePhysics_ResetOrientation
  (VehiclePhysicsRun.cpp:409-418), reached from spawn/grid/off-mesh recovery.
- THE UNTESTED HALF (a8_momentum.py says it in its own header): with the force->velocity
  half proven equal, "the remaining suspect is the orientation half (bodyH)". The port's
  slip is velH - io.yaw (VehiclePhysicsRun.cpp:775) and io.yaw comes from an ALIGNMENT
  block that steers yaw toward the velocity heading (VehiclePhysicsRun.cpp:582-589), i.e.
  the port's body heading is partly derived from velocity. The original's slip is between
  independently stored record fields: forward axis +0x9d4/+0x9dc vs velocity
  +0x9b0/+0x9b8 in the 0xd04 vehicle record (field_trace.py:65-70). A heading that is
  pulled toward the velocity direction cannot hold a large slip angle. This is a
  HYPOTHESIS, not a finding: it has not been measured.

TASK, in order:
1. Measure the orientation half per side, the way the momentum identity was measured for
   the velocity half: d(bodyH)/dt per frame vs the integrated angular velocity (port:
   av=(x,y,z) in motion_diag.log; original: the record's yaw-rate source, which you must
   locate). If the port's bodyH rotates at a rate the original's does not, or is clamped
   toward velH, that is the mechanism. Inputs already on disk, no game run needed:
   verify/a8_steer_20260824/orig_steerR.msd and
   verify/a8_velvec_20260825/cleanhold_motion.log (1097 samples, 50 reseeds) plus the
   ramp run in verify/a8_standalone_20260824/. Reducers:
   re/tools/statediff/a8_momentum.py, a8_radius.py (extend, do not fork).
2. In Ghidra (ghidra-pool skill, read-only slot), find the ORIGINAL's writer of the
   forward axis +0x9d4/+0x9dc. Offset reads are register-relative, so reference_to on the
   record base DAT_008815a0 is the wrong tool; start from FUN_0046b540's init (0x0046bb30
   wheel loop, WS-A1 note) and the A-series plates in re/analysis/ for the per-tick
   orientation integration, and state mechanically what rotates the body basis and from
   which quantity. Cite RVAs. If it is the angular-velocity integrator that B5c ported
   (Vehicle/RwpIntegrator.cpp), diff that path's INPUTS per frame, not its output.
3. Only then compare with VehiclePhysicsRun.cpp:582-589 and Vehicle/VehicleControl.cpp:155
   (orient passed as nullptr, "orient bound at A8" - binding it reaches
   Math/RwMatrixRotateInner.cpp:159-166 mode 1 through a function pointer that is
   currently nullptr; A8 must handle that).
4. Nail down or drop the weakest standing claim before building on anything: the
   held-lock run's 897-vs-1941 median speed gap is ATTRIBUTED to 50 RecoverOffMesh 0.5x
   halvings (TrackRenderer.cpp:1989) but the magnitude was never quantified.
5. If step 1 names a mechanism, port the fix behind an env A/B knob, re-run the held-lock
   recipe, reduce with a8_momentum.py, and report slip per speed band both sides. The
   acceptance bar is the RULING: slip within the same tolerance the other quantities
   already meet, on a run with the reseed contamination quantified.

RECIPE for a port-side capture (from verify/a8_velvec_20260825/PROVENANCE.txt):
  MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
  MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0 MASHED_DRIVE_HOLD=1 MASHED_WIN_POS=left-bl
  MASHED_MOTION_DIAG=1 MASHED_STEER_HOLD=1 MASHED_STEER_HOLD_AFTER=4 MASHED_MUTE=1
  Reduce: py -3.12 re/tools/statediff/a8_momentum.py <motion.log>
          verify/a8_steer_20260824/orig_steerR.msd --orig-steer-min 33.0 --port-steer-min 0.9
  Kill only the MASHED/mashed_re PID you spawned. Record the capture's git HEAD in
  PROVENANCE.txt. *.log is gitignored: git add -f, as the existing captures did.

RULES THAT BIT EARLIER A8 SESSIONS (all in the data note's "traps"):
- A quantity computed from our own formula is not a measurement (trap 3).
- age>=N / steer>=N filters are regime filters; report n per band, do not quote n<60
  bands as solid (trap 6).
- Re-measure on the CURRENT build before trusting any prior number (nineteenth
  follow-up: every prior figure was stale).
- Log both sides from record fields where possible; no Frida Interceptor on 0x00496530
  during phase 2 (hangs 8/30).
- Write findings into re/analysis/data/A8_velocity_vector_motion_20260825.md as the
  twenty-fifth follow-up, same shape: what was measured, what was refuted, what is open.
  Tracker moves only via re-classify.
```

## ⇒ CURRENT STATE (2026-09-12, uncertainty-drain + 8 promotion rounds) — READ THIS FIRST

Branch `race/first-frame-parity`, tree clean, **27 commits** this session. Zero worktrees,
zero Ghidra pool locks, zero stray processes. Anchor verified.

| measure | value | how to re-derive |
|---|---:|---|
| C4 / C3 / C2 / C1 | 184 / **1029** / 3865 / 821 | `Import-Csv hooks.csv \| Group-Object confidence` |
| gating uncertainties | **0** | `Blocks` cell exactly `C2->C3` or `C3`, Active section only |
| open uncertainty rows | 3,031 | rows `^\| *U-[0-9]+` between the Active and Resolved headers |
| promotion rounds run | 256 | ledger `rounds_run` |
| non-canonical column count | 131 | pre-existing baseline, unchanged all session |

## ⇒ THE TWO THINGS MOST LIKELY TO BE MISREAD

**1. "0 gating" does NOT mean "everything is answered."** 46 rows were gating this morning.
17 were **resolved on evidence**; **29 were DE-GATED** and are still open, carrying their
evidence and next command. The de-gate rests on `re/CONFIDENCE.md`'s own C2→C3 wording — a
field may be named *or* explicitly marked `[UNCERTAIN]` with the marker recorded — so a
properly recorded row is the rubric's sanctioned alternative to a name. Owner-approved.
The test applied to each: *would a byte-for-byte verbatim transcription be wrong without
this answer?* For all 29, no.

**2. If you hit a function you cannot transcribe, that is a NEW finding.** Open a new row;
do not assume an old one covers it.

## What landed

### Uncertainty drain: 46 → 0 gating
17 resolved with citations. The ones worth knowing:
- **U-4780** — takes **five** stack args, not three; `[E+0xc]` proven never addressed.
- **U-4700** — the three constants are DirectShow GUIDs (`MEDIATYPE_Video`,
  `MEDIASUBTYPE_RGB24`, `FORMAT_VideoInfo`), named from the **local Windows SDK**
  `uuids.h`, matched at `+0x00/+0x10/+0x2c` = `AM_MEDIA_TYPE` per `strmif.h`.
- **U-4713** — a full 4×4 carrying translation, not a normal matrix: exactly four `fchs`
  negate the whole first row *including* `-pos.x`.
- **U-4701** — `0x00494b65` **is not a function**; it is the `jne` target inside `0x00494b50`.
- **U-5654** — the `-3` is one uniform 3-pixel inset on both axes (float `3.0` at `0x005cc31c`).

**Three corrections to the existing record**, which matter more than the wins:
- Ghidra's *"could not recover jumptable at `0x0041dec0`"* is a **misclassification** — it is
  `jmp dword ptr [eax+0x48]`, a vtable tail call. That was the only genuine transcription
  gate among the 46 and it existed because nobody disassembled the address.
- **U-4313's fourth write site does not exist.** `0x0043f8d5 mov [ebx+8],edx` never lands on
  `0x007f1a1c`; EBX is never loaded with that constant in the range.
- **U-4583's `DAT_006668f8`** is a digit transposition of `DAT_007668f8`.

### 18 promotions across rounds 249–256
`RwEngineRegisterPlugin`, `DriverSystemDispatch`, `RwErrorModuleDtor`, four r252 leaves,
three r253, `Mat4x3InvertOrthonormal`, two r255 walkers, five r256 forwarders.
**One deliberate non-promotion:** `0x004f10e0` is GREEN 8/8 with path2 PASS and **stays C2** —
both callers are anonymous (`FUN_004e4300` C1; `FUN_004e41e0` has no hooks.csv row), so
promoting would be an island promotion. Evidence banked; unblock by raising either caller.

### Harness work (all SWEEP-CRITICAL)
- **`observe_bufs`** on `stub_dispatch_observe` (path1) — three arms of `0x004c2c90` all
  return 1 and differ only through an out-pointer, so return-only observation could not tell
  a correct port from a swapped one.
- **path2 buffer-arg support** — `bgra_encode` / `ptr_seed_observe` / `stub_dispatch_observe`
  added to the verify template, the missing CONFIG forwarding added to `run_verify_hook.py`,
  and an `orch-iter21` test-shape branch narrowed because it was **shadowing** the new
  handlers. Unblocked 15 registry entries; all 17 `arg_layout` entries re-run, no regressions.
- **`re/tools/caller_screen.py`** — applies the C2→C3 caller rule *before* any code is
  written. On the r256 slice: 227 promotable, 69 caller-blocked.

## ⇒ STANDING RULE LEARNED THE HARD WAY: a new arg_type has FOUR homes
`diff_template.js`, `verify_hook_install_template.js`, **and** the config builders in
`run_diff.py` **and** `run_verify_hook.py`. Both builders are **whitelists that drop unknown
keys silently**. Miss the template and path1 goes GREEN while path2 dies `bad argument
count`; miss the builder and the handler runs against an EMPTY config. This bit three
separate ways in one round. Also check `callFn`'s branch ORDER — a branch keyed off *test
shape* rather than arg_type will shadow later handlers (same lesson as U-9067).

## ⇒ AND: a name may not claim more than its comment does
A naming audit of this session's own work **withdrew or corrected 11 of 18 names**.
`RwFrameHeadSet` asserted a frame type and a head field, neither ever read.
`PizOpenDefaultMode` sat on a **particle** row. `RwRGBAToIntensityScaled` claimed a channel
order its own comment explicitly declined to claim. Grounded names were kept (a C4 or
named-library callee gives you one); otherwise `Fwd<callee>_<literal>` says enough.
**An export rename is not cosmetic — re-run both paths.** The rename script itself replaced
by dict order and substituted `RwPluginListDispatch` *inside* `RwPluginListDispatch3`; only
the re-run caught it. Sort replacement keys by descending length.

## PICK ONE

> **Options A and B were DONE 2026-09-12 (same day, later session).** U-9135: attestation NARROWED, not re-banded
> (13 render rows -> psgp on dispatcher/table evidence, 92 keep `render`, 66 hlsl rows -> psgp; new U-9136 for
> the hlsl band). U-9134: Lua ends at `0x004c0735`; 16 rows -> render, 10 -> unknown. 0 C-levels moved.
> Method that settled both: `reference_to`/`reference_from` per function, never the range. U-9136 (hlsl band) also DONE the same way: 5 -> psgp, 68 -> d3dx9-shader-compiler, range label retired. Remaining pick: **C**, or D2.

### A. **U-9135 — decide the PSGP band disposition.** (recommended, and it is a decision, not research)
103 rows are tagged `render` (first-party) while carrying a note asserting they are
statically-linked Microsoft PSGP. The attested range `0x004ec000..0x004fc9e0` holds 198 rows
that the per-row tag splits into **four contiguous, non-interleaved blocks** — render 25,
psgp 79, render 80, psgp 14 — and only 93 are tagged `d3dx9-psgp`, while 12 psgp-tagged rows
sit outside the range. The clean block structure is the evidence. Same shape as U-9134.
Either **narrow the attestation** to the two genuine psgp blocks, or **re-band** the 103 rows
under library-skip. Do not edit either field before deciding: both are range-assigned and the
wrong choice mislabels 100+ rows. Seven rows in the range are already C3 (five promoted this
session) — **all seven are in the render blocks**, so under the per-row tag they are fine.

### B. **U-9134 — audit the `0x004b4a80..0x004c4000` lua band.** Same class, one row proven
mis-banded (`004c0c20` dereferences RwGlobals). May be hiding reachable first-party work.

### C. **More promotion rounds.** The pool is deep and the loop is cheap now: no path2
friction since r252, and `caller_screen.py` removes ~30% of dead ends before authoring.

## Carried over, still needing YOUR call
- **`area/frontend` is the one unmerged branch**, deliberately (WIP `PanelSortInit` hook).
- **D-11069** — 4 duplicate-RVA rows need a `hooks.csv` schema change.
- **U-9087** — 4 C4 rows may need demotion; their install proof is a byte the original has.
- **`main` is level with HEAD**; nothing pushed to `origin` (182 commits ahead).

## Harness wishlist (measured, per the ledger's own rule)
- **`ptr_to` cannot express buf+OFFSET**, which blocks intrusive CIRCULAR lists whose
  sentinel is an interior address — `0x004c59c0` (`param_1+8`), `0x004d8280`/`0x004d8300`
  (`param_1+0x90`). NULL-terminated lists are unaffected. **Count the rows before spending a
  round on it.**
- Absolute-global seeding combined with `stub_at` — blocks `0x004c9f60` (spec in ledger L2c,
  including the `__stdcall` vtable-slot hazard at `0x004c9f85`).

## Standing rules that bit earlier sessions
- Shadow lane: single-boot verdicts are unreliable; require two boots.
- Races need `--cars 4 --hold 60`; a 1-car race never fires the contact solver.
- Never `git worktree remove --force` — use `py -3.12 scripts/diag.py wt-remove`.
- Kill only PIDs you spawned; never blanket-kill MASHED by name.
- A Python script that reads text and writes text **strips CRLF**. Read/write bytes.
  `git diff --stat` is the tell (3,240 lines "changed" = you did it).
- Do not put a literal `|` in an UNCERTAINTIES cell — it splits the row. Spell it `OR`.

## Ready-to-paste kickoff

> Resume the Mashed RE lane on `race/first-frame-parity`. Read `re/NEXT_SESSION.md` first —
> note that **gating uncertainties are 0 but 29 of the 46 were de-gated, not resolved**, and
> that **U-9135 is an open owner decision affecting 103 rows**. Then pick A (decide U-9135),
> B (audit the lua band, U-9134), or C (more promotion rounds — run
> `py -3.12 re/tools/decomp_pc.py --file rvas.txt --callers --json -o batch.json` then
> `py -3.12 re/tools/caller_screen.py batch.json` and author only from the PROMOTABLE list).
