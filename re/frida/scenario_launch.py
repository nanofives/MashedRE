# SCENARIO LAUNCHER (2026-06-23) — warp the ORIGINAL MASHED.exe straight into a race,
# bypassing the menu, by writing the selection-state globals and poking the session-phase
# state machine. Build spec: investigation 2026-06-23 (Ghidra-verified).
#
# How it works (re/analysis/game_state/0x004929d0.md):
#   The session is a global state machine FUN_004929d0 switching on the byte DAT_00771968:
#     phase 1 = menu/lobby
#     phase 2 = LOAD TRACK + SPAWN CARS (calls FUN_0040d440 = Course::LoadCurrent, then ->3)
#     phase 3 = race running
#   There is NO callable StartRace(cfg). We set the globals the menu would have built, then
#   write DAT_00771968 = 2 and the engine's own loop loads the track + spawns every car.
#
# Increment 1 (this file): drop into a race on a chosen track and confirm phase->3 + a car
#   spawned. No warp / no input injection yet (those are increment 2/3).
#
# Spawn+attach (NEVER frida.spawn — perturbs boot layout, project_replay_deterministic_clock).
# Kills ONLY the pid it launches. No OS input injection.
#
# Usage:
#   py -3.12 re/frida/scenario_launch.py [--track 0] [--mode 10] [--players 1] [--fps 60] [--hold 20]
import os, sys, time, argparse, subprocess
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
# Worktrees do NOT junction original/ (WORKTREE-SYMLINK-WIPE); when run from a
# worktree, point MASHED_ROOT at the main checkout to find the game install.
GAME_ROOT = Path(os.environ.get("MASHED_ROOT", ROOT))
EXE  = GAME_ROOT / "original" / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG = 0x400000;
let M = null;
function modBase(){ if(!M) M = Process.findModuleByName('MASHED.exe'); return M ? M.base : null; }
function ga(addr){ const b = modBase(); return b ? b.add(addr - IMG) : null; }   // global VA -> live ptr

// --- selection-state globals (Ghidra-verified build spec 2026-06-23) ---
const PHASE      = 0x00771968;   // session-phase enum (U8): 1=menu 2=load+spawn 3=race
const TRACK_ENG  = 0x0063ba7c;   // engine track idx (FUN_0040d440 loads this)
const TRACK_MENU = 0x0067f17c;   // menu-side track idx (keep consistent)
const MODE       = 0x0067e9fc;   // game-mode 2..10 (10=QuickRace, 2=TimeTrial)
const RULE       = 0x007f0fd0;   // race-rule
const CAR_P0     = 0x0067ea98;   // player-0 car/character cursor
const DIFFICULTY = 0x0067ea7c;   // RaceConfig.difficulty
const POWERUPS   = 0x0067ea80;   // RaceConfig.powerUps
const SLOT0      = 0x007f1a14;   // per-slot car-index array (stride 0x10; -1=inactive)
const PCOUNT     = 0x008a94d0;   // player count 1..4
const TEAM       = 0x0067ea64;   // team-game flag
const CARREC     = 0x008815a0;   // player car record base (stride 0xd04)
const SPAWN_RVA  = 0x0046b540;   // VehicleSpawnInit (one-shot spawn confirm; fires once/car)
const ACTIVATE   = 0x0040e480;   // FUN_0040e480(slot,val) cdecl: writes the per-slot array the
                                 // spawn loop (FUN_004111c0 case DAT_0063ba8c==1) reads at
                                 // PTR_PTR_005f2770+0x34 to decide which slots get VehicleSpawnInit

let spawnFired = 0, spawnArmed = false;
function armSpawn(){ if(spawnArmed) return; spawnArmed = true;
  try { Interceptor.attach(ga(SPAWN_RVA), { onEnter(){ spawnFired++; } }); } catch(e){ send({kind:'err', msg:'armSpawn '+e}); } }

// In-process control injection (nav_agent.js method): override FUN_00497310's return for
// player-0 control `pressCtrl` to 0xff (pressed) while pressUntil is in the future. Control 4
// = confirm/accelerate -> used to skip the race-start intro, keep the race driving, and
// continue between rounds. No OS input injection.
const RES_RVA = 0x00497310;
let pressCtrl = -1, pressUntil = 0, inputArmed = false;
function armInput(){ if(inputArmed) return; inputArmed = true;
  try { Interceptor.attach(ga(RES_RVA), {
    onEnter(){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
    onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }
  }); } catch(e){ send({kind:'err', msg:'armInput '+e}); } }

// --- D1 SPIKE (2026-07-06): proxy-body step live-vs-bypassed ---------------
// Settles COLLISION_GATE_BRIEF_D1_2026-07.md Open-Unknown #1: is the RW-Physics
// proxy-body world (system 2) load-bearing for RENDERED car motion?
// Bypass = Interceptor.replace of VehiclePhysicsWorldStep 0x0047eb30
// (bool(void), globals-only; its own DAT_006ce274==0 guard path returns 0, so a
// constant-0 replacement is the "no physics world" path callers already
// tolerate — re/analysis/vehicle_promote_c2_b/0047eb30.md). Armed MID-race
// (after spawn) so world init + the frame-0x7b qhull hull build (FUN_0047d3c0,
// called from inside 0x0047eb30) complete normally; only steady-state stepping
// dies. Control runs arm a call counter instead (60/s — far under the 1000/s
// hot-path limit). Never both on one target (attach+replace conflict).
const PSTEP_RVA = 0x0047eb30;    // VehiclePhysicsWorldStep (C2)
const PWORLD    = 0x006ce274;    // physics world ptr (its guard global)
// IN-RACE DRIVE INJECTOR (ported from capture_player_dynamics.py — the proven
// path): the cook FUN_00496530 zeroes player p's descriptor block
// (0x007f1038 + p*0x4c) then rewrites it. Forcing the block ON LEAVE survives
// the cook. Armed only on demand (spike runs) — it clobbers real input with
// zeros when idle, so oracle runs must not arm it.
//
// STEER BYTES CORRECTED 2026-08-24 (D2/A8). This comment used to assert that A4
// FUN_00470670 reads "block[2]/[3]=steer", and the injector wrote only [2]/[3]
// (+[0xe]/[0xf]). That contradicts the RVA-cited byte map at
// VehiclePhysicsRun.h:67-73, which has [0]/[1]=STEER (sign A/B), [4]=accel,
// [5]=brake, and names this very cook as the writer of [0]/[1]. The port agrees:
// A4 reads input[0]/[1] (VehiclePhysicsRun.h:32-34, asm CMP [EBP],BL @0x470732
// and [EBP+1] @0x470754) and StepPlayer sets input[0]/[1] from io.steer
// (VehiclePhysicsRun.cpp:404-405). Accel is [4] in BOTH accounts — only steer
// disagreed.
//
// This is not a theoretical discrepancy: verify/a8_steer_20260823/orig_steerR.msd
// was captured with --statediff-steer +1 and the car DID NOT TURN (per-round velH
// deltas -0.0136 and -0.0166 rad over 199 and 279 frames, i.e. under one degree).
// Writing steer into bytes A4 never reads is exactly that symptom.
//
// FIX, deliberately minimal: also write [0]/[1]. The pre-existing [2]/[3] and
// [0xe]/[0xf] writes are KEPT so the accel-only baseline every prior statediff
// capture used is byte-unchanged — the only delta versus those runs is the new
// [0]/[1] steer write.
//
// [2]/[3]/[0xe]/[0xf] RESOLVED 2026-08-24 by reading FUN_00496530 in Ghidra
// (Mashed_pool13, read-only, anchor verified on MASHED.exe.unpatched). The cook
// makes FOUR conditional analog-axis reads, each writing a VALUE byte plus an
// ACTIVE-FLAG byte, and the pairing rule is flag(N) = N + 0x0c:
//
//   call FUN_00497310(player, arg)   flag byte      value byte
//     arg 0x09 @0x0049663c          [0x0c]=0xff    [0x00]=AL @0x0049664f
//     arg 0x0a @0x00496658          [0x0d]=0xff    [0x01]=AL @0x0049666b
//     arg 0x0b @0x00496674          [0x0e]=0xff    [0x02]=AL @0x00496687
//     arg 0x0c @0x00496690          [0x0f]=0xff    [0x03]=AL @0x004966a3
//
// So the four value bytes form TWO DIFFERENTIAL AXIS PAIRS, and the cook reduces
// each pair to a float in the device-type-2 branch (0x004966ca, gated on
// [ECX+0x13c]==2):
//   offset 0x14 = ([0x01] - [0x00]) * _DAT_005ceb90   (0x004966cf..0x00496701)
//   offset 0x18 = ([0x03] - [0x02]) * _DAT_005ceb90   (0x004966dd..0x00496711)
//
// [0]/[1] is therefore one axis pair and [2]/[3] is the OTHER — [2]/[3] are NOT
// a second steer channel, and [0xe]/[0xf] are merely the active flags for [2]/[3].
// Corroboration that [0]/[1] is the steer pair: 0x00496717 swaps [0]<->[1] when
// DAT_007f0f30 != 0, i.e. an invert-steering option, and the A8 measurement
// (verify/a8_steer_20260824) turned the car only once [0]/[1] were driven.
// [UNCERTAIN] which of the 0x14/0x18 floats is consumed as steer vs throttle
// downstream — not needed here, since A4 reads the raw bytes [0]/[1], not the floats.
const COOK_RVA = 0x00496530;
const BLK0 = 0x007f1038;
let gAccel = 0, gSteer = 0, cookArmed = false;
function armCook(){ if (cookArmed) return 'already on'; cookArmed = true;
  try { Interceptor.attach(ga(COOK_RVA), { onLeave(){ const b = ga(BLK0); if (!b) return;
    b.add(4).writeU8(gAccel ? 0xff : 0);
    // steer, per the RVA-cited map (A4 reads these two)
    b.add(0).writeU8(gSteer > 0 ? 0xff : 0);
    b.add(1).writeU8(gSteer < 0 ? 0xff : 0);
    // retained legacy writes — see the note above; NOT known to be dead
    b.add(2).writeU8(gSteer > 0 ? 0xff : 0);
    b.add(3).writeU8(gSteer < 0 ? 0xff : 0);
    b.add(0x0e).writeU8(gSteer > 0 ? 0xff : 0);
    b.add(0x0f).writeU8(gSteer < 0 ? 0xff : 0);
  }}); return 'cook injector armed (0x00496530)'; }
  catch(e){ return 'ERR '+e; } }
// ISOLATION CONTROL (2026-08-20, D2): same attach point, same arming moment
// (before the phase poke), EMPTY callback and no forced input. Separates the
// cost of instrumenting a hot function from the effect of the race actually
// starting -- dropping --statediff-drive removes both at once, so it cannot
// tell them apart. Pair with a plain --no-drive run: if the phase-2 hang comes
// back with this armed, the Interceptor attachment alone is the cause.
function armCookNoop(){ if (cookArmed) return 'already on'; cookArmed = true;
  try { Interceptor.attach(ga(COOK_RVA), { onLeave(){} });
    return 'cook injector armed NO-OP (0x00496530)'; }
  catch(e){ return 'ERR '+e; } }
let stepCalls = 0, bypassOn = false, stepCounterOn = false;
function armStepCounter(){
  if (stepCounterOn) return 'already on';
  if (bypassOn) return 'ERR bypass already armed';
  try { Interceptor.attach(ga(PSTEP_RVA), { onEnter(){ stepCalls++; } });
        stepCounterOn = true; return 'step counter armed (0x0047eb30)'; }
  catch(e){ return 'ERR '+e; }
}
function armBypass(){
  if (bypassOn) return 'already on';
  if (stepCounterOn) return 'ERR counter already attached — bypass runs must not arm it';
  try {
    const cb = new NativeCallback(function(){ stepCalls++; return 0; }, 'int', [], 'mscdecl');
    Interceptor.replace(ga(PSTEP_RVA), cb);
    globalThis._keepBypassCb = cb;   // keepAlive — Frida reclaims otherwise (feedback_frida_keepalive_scratch_buffers)
    bypassOn = true; return 'BYPASS ON: 0x0047eb30 -> ret 0';
  } catch(e){ return 'ERR '+e; }
}
// 10 Hz telemetry of the player car record. Offsets (worker extraction
// 2026-07-06, cited to vehicle_coupling.md / capture_player_dynamics.py /
// diff_mostsep_pair.py): render pos = +0x928 matrix block words [0xc..0xe]
// (bytes +0x958/+0x95c/+0x960 — THE surface the proxy readback writes);
// vel +0x9b0..b8; yaw rate +0x9c0; fwd.x/z +0x9d4/+0x9dc (heading);
// grounded +0x9e0 (4.0 = all wheels); scalar speed +0x9e4; airflag +0xb20.
const TEL = { on:false, t0:0, rows:[] };
function telSample(){
  try {
    const r = ga(CARREC);
    TEL.rows.push([ Date.now()-TEL.t0, ga(PHASE).readU8(),
      r.add(0x958).readFloat(), r.add(0x95c).readFloat(), r.add(0x960).readFloat(),
      r.add(0x9b0).readFloat(), r.add(0x9b4).readFloat(), r.add(0x9b8).readFloat(),
      r.add(0x9e4).readFloat(), r.add(0x9c0).readFloat(),
      r.add(0x9d4).readFloat(), r.add(0x9dc).readFloat(),
      r.add(0x9e0).readFloat(), r.add(0xb20).readU32(),
      stepCalls, bypassOn ? 1 : 0, ga(PWORLD).readU32() !== 0 ? 1 : 0,
      // [A8-SUSPGLOBALS 2026-08-26] the two suspension-scale GLOBALS. They are not
      // in the vehicle record, so the statediff record capture cannot see them —
      // which is why the p[0x1b] question stayed open. Our port computes them as
      //   suspDtTerm = frameMs * _DAT_005cea80(0.0027809);  suspScale = 3000/that
      // giving 0.139045 / 21575.7 at a 50-unit budget. Reading the ORIGINAL's
      // values is the only non-circular way to check that.
      ga(0x0088e610).readFloat(),      // suspDtTerm  (_DAT_0088e610)
      ga(0x0088e5f0).readFloat(),      // suspScale   (_DAT_0088e5f0)
      // per-wheel load p[0x1a] (+0x20c) for wheel 0, as a cross-check against the
      // record capture's 1091.56 — confirms the live run is the same regime.
      r.add(0x20c).readFloat(),
      r.add(0x50).readFloat() ]);      // vehicle mass, expected 1000.0
  } catch(e){ /* sample dropped */ }
}
function telStart(){ if (TEL.on) return 'already on';
  TEL.on = true; TEL.t0 = Date.now(); setInterval(telSample, 100);
  return 'telemetry started (10 Hz)'; }
// ---------------------------------------------------------------------------

// --- WS-G rules-debt ORACLE (2026-07-02, D-11052 verification) -------------
// Validates the standalone RuleEngine port (mashedmod/src/mashed_re/Race/
// RuleEngine.cpp) against the LIVE ORIGINAL: hooks FUN_00410d10 (segment
// check), FUN_00410510 (result eval), FUN_004177b0 (finish-order append);
// reads the port's documented input globals at entry, computes the port's
// transcribed law in JS, compares with the original's return value /
// side-effect writes at exit. READ-ONLY — no state writes, no re-execution.
// Input mapping (RuleEngine.h citations):
//   rule           DAT_007f0fd0            metric[4]   DAT_0089a880
//   participants   DAT_008a94d0            score[4]    DAT_008a94e0
//   finishOrder[4] 0x0089a870              timer       DAT_007f0fe4
//   collect        DAT_0063a5d0/0063a5d4   teams       DAT_0067ea64
//   motion[i]      DAT_008815a0+i*0xd04+0x9f0 (FUN_0046cbb0 out1)
//   snapshot1      0x008995ec+0x138 = 0x00899724 (FUN_00423b20(1), U-9004)
//   active[i]      *([0x005f2770]+0x34+i*4) != 0 (slot probe)
//   alive[i]       FUN_0046c7b0(i)==1  (getter CALLED, not re-derived)
//   resultDeclared FUN_00443080()      (verbatim getter of DAT_00897ffc)
//   timeAttack     FUN_0042f6a0()==2   (game-mode getter CALLED)
const OR = { armed:false, err:null,
  seg:{calls:0, agree:0, mis:0, ret1:0, byRule:{}},
  ev :{calls:0, agree:0, mis:0, byRule:{}},
  ord:{calls:0, agree:0, mis:0, appends:0, resets:0},
  misrec:[], samp:[], lastOrder:null };
const ORF = {};
const K = { FIN: 3.0, R10: 2.0, GAP: Math.fround(0.9) }; // 0x005cc31c/0x005cc574/0x005cc9c8
function orPush(rec){ if (OR.misrec.length < 60) OR.misrec.push(rec); }
function orActive(){
  const out = [];
  let ab = null;
  try { ab = ga(0x005f2770).readPointer(); } catch(e){}
  for (let i = 0; i < 4; i++){
    let a = 0;
    try { if (ab && !ab.isNull()) a = ab.add(0x34 + i*4).readS32(); } catch(e){}
    out.push(a !== 0);
  }
  return out;
}
function orReadCars(){
  const c = { rule: ga(0x007f0fd0).readS32(),
              participants: ga(0x008a94d0).readS32(),
              metric: [], score: [], alive: [], motion: [], order: [],
              snapshot1: ga(0x00899724).readS32(),
              timer: ga(0x007f0fe4).readFloat(),
              collectTotal: ga(0x0063a5d0).readS32(),
              collectDone:  ga(0x0063a5d4).readS32(),
              teams: ga(0x0067ea64).readS32() !== 0,
              declRaw: ORF.decl(), mode: ORF.mode() };
  for (let i = 0; i < 4; i++){
    c.metric.push(ga(0x0089a880 + i*4).readFloat());
    c.score.push(ga(0x008a94e0 + i*4).readS32());
    c.alive.push(ORF.alive(i) === 1);
    c.motion.push(ga(0x008815a0 + i*0xd04 + 0x9f0).readS32());
    c.order.push(ga(0x0089a870 + i*4).readFloat());
  }
  c.active = orActive();
  return c;
}
// RuleEngine::SegmentCheck transcribed (pre-blocks on ENTRY state; the
// alive-count tail on EXIT state — the elimination block runs inside the call).
function predSegment(en, exActive, exAlive){
  if (en.declRaw === 1) return 0;              // 0x00410d10 head law: FUN_00443080() == 1 -> return 0
  switch (en.rule){
  case 4: if (!(en.metric[0] < K.FIN)) return 1; break;
  case 5: if (!en.alive[0]) return 1;
          return (en.collectTotal !== 0 && en.collectDone === en.collectTotal) ? 1 : 0;
  case 7: {
    if (!en.alive[0]) return 1;
    let dead = 0;
    for (let i = 0; i < en.participants; i++){
      if (!en.alive[i]) dead++;
      if (K.FIN < en.metric[i]) return 1;
    }
    return (en.participants - 1 <= dead) ? 1 : 0; }
  case 8: if (K.FIN <= en.metric[0]) return 1;
          if (en.motion[0] !== 0) return 1; break;
  case 9: if (K.FIN <= en.metric[0]) return 1;
          if (K.FIN <= en.metric[1]) return 1;
          if (en.motion[0] !== 0) return 1;
          if (en.snapshot1 !== 0) return 1; break;
  case 10: if (en.motion[0] !== 0) return 1;
           if ((en.timer < 0) !== (en.timer === 0)) return 1;   // NaN-aware expiry
           if (!(en.metric[0] < K.R10)) return 1; break;
  }
  let slots = 0, alive = 0;
  for (let i = 0; i < 4; i++) if (exActive[i]) { slots++; if (exAlive[i]) alive++; }
  if (slots === 1) { if (alive === 0) return 1; }
  else { if (alive === 1) return 1; if (alive === 0) return 1; }
  return 0;
}
// RuleEngine::EvaluateResult transcribed (all inputs ENTRY state).
function predEval(en){
  if (en.mode === 2) return { ret: 0 };
  let winner = 0;
  for (let i = 1; i <= 4; i++){
    const sc = en.score[i-1];
    if ((en.participants === 2 || en.participants === 3 || en.teams) && sc === 8) winner = i;
    if (en.rule === 2 && sc === 8) winner = i;
    if (en.participants === 4 && sc > 11) winner = i;
  }
  if (winner !== 0){
    for (let i = 0; i < 4; i++){
      if (!en.active[i] || !en.alive[i]) continue;
      const sc = en.score[i];
      if ((en.participants === 2 || en.participants === 3 || en.teams) && sc === 8) winner = i + 1;
      if (en.participants === 4 && sc > 11) winner = i + 1;
    }
  }
  let concluded = false, won0 = false;
  const slot0 = Math.trunc(en.order[0]);      // __ftol truncation (FUN_00417740)
  switch (en.rule){
  case 4: winner = slot0 + 1; break;
  case 5: if (!en.alive[0]) { winner = -1; concluded = true; break; }
          winner = (en.collectTotal !== 0 && en.collectDone === en.collectTotal) ? 1 : 0;
          break;
  case 7: winner = 0;
          for (let i = 0; i < en.participants; i++)
            if (K.FIN < en.metric[i]) { winner = i + 1; break; }
          break;
  case 8: winner = slot0 + 1;
          if (winner !== 1){ if (winner === 0) winner = -1; concluded = true; }
          break;
  case 9: if (slot0 !== -1)      { winner = -1; concluded = true; break; }
          if (en.motion[0] !== 0){ winner = -1; concluded = true; break; }
          if (K.GAP <= en.metric[1] - en.metric[0]) { winner = -1; concluded = true; break; }
          winner = 1; won0 = true; concluded = true; break;
  case 10: if (en.motion[0] !== 0){ winner = -1; concluded = true; break; }
           if (en.metric[0] < K.R10){
             if ((en.timer < 0) === (en.timer === 0)) return { ret: 0 };
             winner = -1; concluded = true; break;
           }
           winner = 1; won0 = true; concluded = true; break;
  }
  if (!concluded && winner === 0) return { ret: 0 };
  return { ret: winner, fcc: (winner === -1) ? 0 : ((won0 || winner - 1 === 0) ? 1 : 0) };
}
// RuleEngine::UpdateFinishOrder transcribed (entry order + EXIT metrics).
function predOrder(rule, entryOrder, exMetric){
  const out = entryOrder.slice();
  if (rule !== 4 && rule !== 9 && rule !== 7 && rule !== 8) return out;
  for (let car = 0; car < 4; car++){
    if (exMetric[car] < K.FIN) continue;
    if (out[0] === car || out[1] === car || out[2] === car || out[3] === car) continue;
    for (let s = 0; s < 4; s++) if (out[s] === -1) { out[s] = car; break; }
  }
  return out;
}
function armOracle(){
  if (OR.armed) return 'already armed';
  try {
    ORF.decl  = new NativeFunction(ga(0x00443080), 'int', [], 'mscdecl');
    ORF.mode  = new NativeFunction(ga(0x0042f6a0), 'int', [], 'mscdecl');
    ORF.alive = new NativeFunction(ga(0x0046c7b0), 'int', ['int'], 'mscdecl');
    Interceptor.attach(ga(0x00410d10), {
      onEnter(){ try { this.en = orReadCars(); } catch(e){ OR.err = 'seg.enter '+e; } },
      onLeave(ret){ try {
        if (!this.en) return;
        const exAlive = [];
        for (let i = 0; i < 4; i++) exAlive.push(ORF.alive(i) === 1);
        const exActive = orActive();
        const got = ret.toInt32();
        const want = predSegment(this.en, exActive, exAlive);
        const r = this.en.rule;
        OR.seg.calls++;
        OR.seg.byRule[r] = OR.seg.byRule[r] || {calls:0, agree:0, mis:0, ret1:0};
        OR.seg.byRule[r].calls++;
        if (got !== 0) { OR.seg.ret1++; OR.seg.byRule[r].ret1++; }
        if (got === want){
          OR.seg.agree++; OR.seg.byRule[r].agree++;
          if (got !== 0 && OR.samp.length < 40)
            OR.samp.push({fn:'seg', rule:r, got:got, want:want, en:this.en, exActive:exActive, exAlive:exAlive});
        } else {
          OR.seg.mis++; OR.seg.byRule[r].mis++;
          orPush({fn:'seg', rule:r, got:got, want:want, en:this.en, exActive:exActive, exAlive:exAlive});
        }
      } catch(e){ OR.err = 'seg.leave '+e; } }
    });
    Interceptor.attach(ga(0x00410510), {
      onEnter(){ try { this.en = orReadCars(); } catch(e){ OR.err = 'ev.enter '+e; } },
      onLeave(ret){ try {
        if (!this.en) return;
        const got = ret.toInt32();
        const p = predEval(this.en);
        const fccGot = ga(0x007f0fcc).readS32();
        let ok = (got === p.ret);
        if (ok && got !== 0 && p.fcc !== undefined) ok = (fccGot === p.fcc);
        const r = this.en.rule;
        OR.ev.calls++;
        OR.ev.byRule[r] = OR.ev.byRule[r] || {calls:0, agree:0, mis:0};
        OR.ev.byRule[r].calls++;
        if (ok){
          OR.ev.agree++; OR.ev.byRule[r].agree++;
          if (OR.samp.length < 40) OR.samp.push({fn:'ev', rule:r, got:got, fccGot:fccGot, pred:p, en:this.en});
        } else {
          OR.ev.mis++; OR.ev.byRule[r].mis++;
          orPush({fn:'ev', rule:r, got:got, fccGot:fccGot, pred:p, en:this.en});
        }
      } catch(e){ OR.err = 'ev.leave '+e; } }
    });
    Interceptor.attach(ga(0x004177b0), {
      onEnter(){ try {
        this.rule = ga(0x007f0fd0).readS32();
        this.order = [];
        for (let s = 0; s < 4; s++) this.order.push(ga(0x0089a870 + s*4).readFloat());
        // U-9005 witness: a reset to all -1 BETWEEN calls (round restart re-init)
        if (OR.lastOrder && OR.lastOrder.some(v => v !== -1) && this.order.every(v => v === -1))
          OR.ord.resets++;
      } catch(e){ OR.err = 'ord.enter '+e; } },
      onLeave(){ try {
        if (this.order === undefined) return;
        const exOrder = [], exMetric = [];
        for (let s = 0; s < 4; s++){
          exOrder.push(ga(0x0089a870 + s*4).readFloat());
          exMetric.push(ga(0x0089a880 + s*4).readFloat());
        }
        const want = predOrder(this.rule, this.order, exMetric);
        OR.ord.calls++;
        let same = true, appended = false;
        for (let s = 0; s < 4; s++){
          if (exOrder[s] !== want[s]) same = false;
          if (exOrder[s] !== this.order[s]) appended = true;
        }
        if (appended) OR.ord.appends++;
        if (same) OR.ord.agree++;
        else { OR.ord.mis++; orPush({fn:'ord', rule:this.rule, entry:this.order, got:exOrder, want:want, exMetric:exMetric}); }
        OR.lastOrder = exOrder;
      } catch(e){ OR.err = 'ord.leave '+e; } }
    });
    OR.armed = true;
    return 'oracle armed (0x00410d10 + 0x00410510 + 0x004177b0)';
  } catch(e){ return 'ERR ' + e; }
}
// ---------------------------------------------------------------------------

// --- generic invocation counter (opt-in) ----------------------------------
// Attach Interceptor at arbitrary RVAs and count entries. Used to PROVE a code
// path was actually executed during a scenario — a clean run is meaningless as
// verification if the function under test never ran. Cold paths only: see the
// hot-path rule in CLAUDE.md (>1000 calls/s destabilises the process).
const CNT = {};
// Tokens that could not be armed yet because mashed_re_dev.asi was not loaded at
// attach time. armCounters() runs at spawn+attach (entry point), which is BEFORE
// dinput8 has loaded the .asi, so every "asi:" token returns NOEXPORT there. The
// driver calls rearmAsi() once the menu is up (phase 1) — by then the .asi is
// loaded and the export resolves. Without this the asi: counter is always
// NOEXPORT and the C4 lift has no evidence. (orch-iter21.)
const PENDING_ASI = [];
function armAsiToken(tok, out){
  const nm = tok.slice(4);
  let ep = null;
  // Frida 17 removed the STATIC Module.findExportByName(moduleName, symbol); it
  // now lives on the module instance. The static form throws TypeError, which the
  // old catch swallowed into a null — indistinguishable from "not loaded yet", and
  // it cost two boots in orch-iter21 chasing a load-order theory. Try the instance
  // API first and only then the legacy static.
  try {
    const m = Process.findModuleByName('mashed_re_dev.asi');
    if (m) ep = m.findExportByName(nm);
  } catch(e){}
  if (!ep) { try { ep = Module.findExportByName('mashed_re_dev.asi', nm); } catch(e){} }
  if (!ep) { out.push(tok + '=NOEXPORT'); return false; }
  CNT[tok] = 0;
  Interceptor.attach(ep, { onEnter: function(){ CNT[tok]++; } });
  out.push(tok + '=armed@' + ep);
  return true;
}
function rearmAsi(){
  try {
    const out = [];
    for (let i = PENDING_ASI.length - 1; i >= 0; i--) {
      if (armAsiToken(PENDING_ASI[i], out)) PENDING_ASI.splice(i, 1);
    }
    if (PENDING_ASI.length) {
      // Still unresolved: say WHY. Either the .asi is not loaded (no module) or it
      // is loaded but does not export the name. Guessing between those cost a boot
      // in orch-iter21.
      const mods = Process.enumerateModules()
        .filter(function(m){ return /asi$|dinput8|d3d9/i.test(m.name); })
        .map(function(m){ return m.name; });
      out.push('[loaded: ' + (mods.join(',') || 'none') + ']');
    }
    return out.length ? out.join(' ') : 'nothing pending';
  } catch(e){ return 'ERR ' + e; }
}
function armCounters(csv){
  try {
    const out = [];
    csv.split(',').forEach(function(tok){
      tok = tok.trim(); if(!tok) return;
      // "asi:ExportName" counts entries into OUR PORT rather than into the
      // original RVA. This is the measurement the C4 rubric actually wants.
      // Counting at the original RVA cannot answer "did our code run": the
      // inline JMP may not be installed yet when counters are armed (attach
      // happens very early in boot, before dinput8 has loaded the .asi), and
      // once Interceptor.attach patches the site, re-reading the bytes shows
      // Frida's trampoline instead of our JMP — so the install state is
      // unreadable at both ends. A counter on the .asi export sidesteps all of
      // it: the export is only reachable THROUGH the installed JMP, so a
      // non-zero count is positive proof the port executed. (orch-iter20, after
      // an armed[orig] reading nearly became a false C4.)
      if (tok.indexOf('asi:') === 0) {
        // Deferred on failure — rearmAsi() retries once the menu is up.
        if (!armAsiToken(tok, out)) PENDING_ASI.push(tok);
        return;
      }
      const rva = parseInt(tok, 16);
      const p = ga(rva); if(!p) { out.push(tok + '=NOBASE'); return; }
      // READ THE INSTALL STATE BEFORE ATTACHING. Interceptor.attach patches the
      // target itself, so reading after would report Frida's trampoline rather
      // than whether OUR inline JMP is live. Order is load-bearing here.
      //
      // This exists for the C4 lift: the rubric wants a canonical-scenario run
      // with the hook ACTUALLY INSTALLED, and counting entries alone does not
      // show that. Doing it in the SAME run closes the gap that otherwise makes
      // the claim an inference across two separate boots (orch-iter20).
      let inst = 'orig';
      try {
        if (p.readU8() === 0xe9) {
          const tgt = p.add(5).add(p.add(1).readS32());
          const m = Process.findModuleByAddress(tgt);
          inst = 'JMP->' + (m ? m.name : '?') + '@' + tgt;
        }
      } catch(e){ inst = 'READERR'; }
      CNT[tok] = 0;
      Interceptor.attach(p, { onEnter: function(){ CNT[tok]++; } });
      out.push(tok + '=armed[' + inst + ']');
    });
    return out.join(' ');
  } catch(e){ return 'ERR ' + e; }
}
// ---------------------------------------------------------------------------

// --- STATE-DIFF capture (2026-07-31, re/tools/statediff/) -------------------
// Per-render-frame snapshot of ONE vehicle record (base 0x008815a0 +
// car*0xd04, size 0xd04), gated on phase==3. Frame 0 = the FIRST phase-3
// render tick (FUN_004c1be0, the replay clock of replay_verify.py armClock):
// the menu-tick anchor cannot align two separate boots because the warp poke
// is python-timed, but the phase-2->3 transition is engine-driven. Fires
// ~60/s — far under the 1000/s hot-path limit. Payload rides the Frida
// binary-data channel; the python side writes MSD1 (re/tools/statediff/FORMAT.md).
const RENDER_TICK = 0x004c1be0;   // render-frame clock
const SD = { armed:false, frames:0, car:0, err:null };
function sdArm(car){
  if (SD.armed) return 'already armed';
  SD.car = car;
  try {
    const rec = ga(CARREC).add(car * 0xd04);
    const ph  = ga(PHASE);
    Interceptor.attach(ga(RENDER_TICK), { onEnter(){
      try {
        if (ph.readU8() !== 3) return;
        send({kind:'sd', f: SD.frames++}, rec.readByteArray(0xd04));
      } catch(e){ if (!SD.err) SD.err = '' + e; }
    }});
    SD.armed = true;
    return 'statediff armed (tick 0x004c1be0, car ' + car + ', rec@' + rec + ')';
  } catch(e){ return 'ERR ' + e; }
}
// ---------------------------------------------------------------------------

// --- CANONICAL-OBSERVATION BLOCK ------------------------------------------
// The texObserve/texResults implementation lives in re/frida/observe_block.js
// and is CONCATENATED onto this agent below (see OBSERVE_JS). It used to be
// inline here; it was extracted once replay_session.py needed the same
// capture, because two copies of an observation harness is exactly the
// duplicate-implementation drift this project has already been bitten by.
// It registers itself onto rpc.exports, so it must be appended AFTER the
// rpc.exports assignment below.
// ---------------------------------------------------------------------------

rpc.exports = {
  ready: function(){ return modBase() ? 1 : 0; },
  sdArm: function(car){ return sdArm(car); },
  sdStats: function(){ return JSON.stringify(SD); },
  armCounters: function(csv){ return armCounters(csv); },
  rearmAsi: function(){ return rearmAsi(); },
  counters: function(){ return JSON.stringify(CNT); },
  armOracle: function(){ return armOracle(); },
  armBypass: function(){ return armBypass(); },
  armStepCounter: function(){ return armStepCounter(); },
  armCook: function(){ return armCook(); },
  armCookNoop: function(){ return armCookNoop(); },
  drive: function(accel, steer){ gAccel = accel; gSteer = steer; return 1; },
  telStart: function(){ return telStart(); },
  telemetry: function(){ return JSON.stringify({
    cols: ['t_ms','phase','px','py','pz','vx','vy','vz','speed','yawRate',
           'fwdx','fwdz','grounded','airflag','stepCalls','bypass','worldPtr',
           'suspDtTerm','suspScale','wheel0Load','mass'],
    rows: TEL.rows }); },
  pokeTimer: function(v){ try { ga(0x007f0fe4).writeFloat(v); return 1; } catch(e){ return 'ERR '+e; } },
  // lap counter row 0x008a9620 stride 0x30c field +0x28 (U-8988 resolution);
  // FUN_004177b0 recomputes metric[car] from it next tick -> finisher edges
  // flow through the ORIGINAL's own metric writer. CONTRIVED (C3-grade).
  pokeLap: function(car, laps){ try { ga(0x008a9620 + car*0x30c + 0x28).writeS32(laps); return 1; } catch(e){ return 'ERR '+e; } },
  // rule-5 collect counters DAT_0063a5d0/DAT_0063a5d4 (registrar chain untraced, D-11056)
  pokeCollect: function(total, done){ try { ga(0x0063a5d0).writeS32(total); ga(0x0063a5d4).writeS32(done); return 1; } catch(e){ return 'ERR '+e; } },
  oracleStats: function(){
    return JSON.stringify(OR, function(k, v){
      return (typeof v === 'number' && !isFinite(v)) ? 'non-finite:' + String(v) : v;
    });
  },
  phase: function(){ try { return ga(PHASE).readU8(); } catch(e){ return -1; } },
  // COURSE-LOAD VERIFIER (area-track r1; assert set CORRECTED 2026-09-01, U-9066).
  //
  // TWO deterministic load-integrity observables, each cited to the load chain:
  //   DAT_0066d704 == 1   set at the tail of FUN_00426e10 (0x00426e10) after the track
  //                       .piz + COURSE.LUA/LAPDATA.LUA load.
  //   DAT_0063ba78 == DAT_0063ba7c   loaded-course == selected-course after
  //                       FUN_0040d440 (Course::LoadCurrent, 0x0040d440).
  //
  // DAT_0063ba8c IS NOT AN ASSERT — it is reported as a raw OBSERVATION only.
  // As originally written this verifier asserted DAT_0063ba8c == 1 and therefore FAILED
  // ITS OWN ZERO-HOOK BASELINE (expected 1, got 3, stable across 3 runs / 2 tracks), which
  // made every verdict uninformative. Root cause, from an XrefRange scan of
  // [0x0063ba8c..0x0063ba8f] on the anchored binary (28 refs): the address is a race STATE
  // MACHINE, not a load-complete flag. It is written with 12 distinct constants by 8
  // functions -- 0x0040d3e7 FUN_0040d270 (Course::Finish) writes 1, but later writers
  // advance it: 0x0040dbf5/dc17/dc30/dc40 FUN_0040dbd0 write 5; 0x0040dda3 write 2 and
  // 0x0040ddf5 write 0xa and 0x004100dd write 2 and 0x00410279 write 3 and 0x00410287 /
  // 0x004102ac write 4 and 0x00410b02 write 9, all in FUN_004111c0 (the spawn loop);
  // 0x00410387 FUN_004102f0 writes 4; 0x004104e3 FUN_004103a0 writes 6; 0x00410645
  // FUN_00410510 writes 0xb; 0x00410a5e / 0x00410a6e FUN_00410860 write 9 and 8;
  // 0x004111a6 FUN_00411170 writes 7; 0x0040e364 FUN_0040e360 writes EAX. Readers include
  // 0x0040fe46 FUN_0040fc00 (CMP against 0x7). So "1" is one transient state of at least
  // eleven, Course::Finish is only its FIRST writer, and by race-running the spawn loop has
  // legitimately moved it on. The value 3 observed at phase 3 is written at 0x00410279.
  // NO SEMANTIC IS ASSIGNED to 3 or to any other value here -- it is reported raw so a
  // reader can diff baseline against hooked runs, and the pass/fail verdict does not
  // depend on it. Reinstating it as an assert requires establishing what state each
  // constant denotes; until then it cannot carry a load-integrity claim.
  //
  // Read-only. Baseline (no hooks) must be pass=true; each dispatcher hook live must
  // KEEP it pass=true (no-regression). Not perturbed by the render-quad thunk 0x0047b9e0.
  courseLoadAsserts: function(){
    try {
      const flag_66d704 = ga(0x0066d704).readU32();
      const state_63ba8c = ga(0x0063ba8c).readU32();
      const loaded      = ga(0x0063ba78).readS32();
      const selected    = ga(0x0063ba7c).readS32();
      const a1 = (flag_66d704 === 1);
      const a3 = (loaded === selected);
      return JSON.stringify({
        pass: (a1 && a3),
        asserts: {
          'DAT_0066d704==1': {ok: a1, got: flag_66d704},
          'DAT_0063ba78==DAT_0063ba7c': {ok: a3, loaded: loaded, selected: selected}
        },
        observations: {
          // raw state-machine value, NOT asserted -- see the comment above (U-9066).
          'DAT_0063ba8c': state_63ba8c
        }
      });
    } catch(e){ return JSON.stringify({pass:false, err:''+e}); }
  },
  setup: function(cfg){
    try {
      ga(TRACK_ENG ).writeS32(cfg.track);
      ga(TRACK_MENU).writeS32(cfg.track);
      ga(MODE      ).writeS32(cfg.mode);
      ga(RULE      ).writeS32(cfg.rule);
      ga(CAR_P0    ).writeS32(cfg.car);
      ga(TEAM      ).writeS32(cfg.team);
      // difficulty / powerups: encoding [UNCERTAIN] — only write when explicitly given (>=0),
      // else leave the game default so an unknown value can't break the race.
      if (cfg.difficulty >= 0) ga(DIFFICULTY).writeS32(cfg.difficulty);
      if (cfg.powerups   >= 0) ga(POWERUPS  ).writeS32(cfg.powerups);
      // Activate the per-slot vehicles via FUN_0040e480(slot,val) — THIS is the array the
      // spawn loop reads. slot 0 = human player (1); slots 1..cars-1 = AI (2); rest = empty (0).
      // (The earlier raw DAT_007f1a14 write was the wrong array.) DAT_008a94d0 (player count)
      // is recomputed by the spawn loop, so we do NOT preset it.
      const e480 = new NativeFunction(ga(ACTIVATE), 'void', ['int','int'], 'mscdecl');
      for (let s = 0; s < 4; s++) e480(s, s === 0 ? 1 : (s < cfg.cars ? 2 : 0));
      armSpawn();
      armInput();
      return 'set track='+cfg.track+' mode='+cfg.mode+' cars='+cfg.cars+' car='+cfg.car
             +' rule='+cfg.rule+' team='+cfg.team
             +(cfg.difficulty>=0?' diff='+cfg.difficulty:'')+(cfg.powerups>=0?' powerups='+cfg.powerups:'');
    } catch(e){ return 'ERR '+e; }
  },
  launch: function(){ try { ga(PHASE).writeU8(2); return 1; } catch(e){ return 'ERR '+e; } },
  press: function(c, ms){ pressCtrl = c; pressUntil = Date.now() + ms; return 1; },
  boost: function(v){ try { ga(CARREC).add(0x9b4).writeFloat(v); return 1; } catch(e){ return 'ERR '+e; } },
  carinfo: function(){
    try { const r = ga(CARREC);
      return { spawnFired: spawnFired,
               grounded: r.add(0x9e0).readFloat(),
               pos_via_fwd: [r.add(0x9d4).readFloat(), r.add(0x9d8).readFloat(), r.add(0x9dc).readFloat()],
               vel: [r.add(0x9b0).readFloat(), r.add(0x9b4).readFloat(), r.add(0x9b8).readFloat()],
               airflag: r.add(0xb20).readU32() };
    } catch(e){ return { err: ''+e }; }
  }
};
send({kind:'ready'});
'''


# --- texture/raster cluster observation spec (2026-09-02, parent booted lane) --
#
# The 12 rows r8 mapped as the texture/raster neighbourhood
# (re/analysis/bucket_00549580/r8_texture_raster_neighbourhood.md). Measured
# 2026-09-02: ALL 12 fire on an ordinary track load (--track 3 --mode 10, counts
# 4..15262), so no special provocation is needed - a plain scenario run IS the
# "one real texture-load capture" r8 recommended.
#
# CORRECTION recorded here because it contradicts r8's stated mechanism: r8 says
# the 12 "all fire in FUN_0054fd60's own execution". In that same measured run
# FUN_0054fd60 was called ZERO times while all 12 callees ran. They are reached
# through some other path in a race load. Co-location under one capture still
# holds (which is what the recommendation was for); the explanation does not.
#
# `obs` entries dereference an ARG (by 0-based index) at +off after the call.
# Signatures are r8's; where r8 gives only a role and no signature, nargs is a
# conservative 4 and there are no derefs - args and return are still recorded,
# which is the point: for those 7 rows r8 identified NO observable at all, and
# this run is what decides whether one exists.
TEXTURE_CLUSTER_SPEC = [
    # -- Group A: the 5 Ghidra-leaves, RW DEVICE/RASTER vtable dispatch --------
    # f(raster, mode, *w,*h,*d,*fmt) - "locks raster, reads back w/h/d +
    # byte-swapped stride into out-params" (vtable +0x6c). The 4 out-params ARE
    # the observable; r8's degenerate mode is "fake buf -> lock returns 0 ->
    # out-params untouched".
    {"rva": "0x004d5340", "cap": 24, "nargs": 6,
     "obs": [{"from": 2, "off": 0, "size": 4}, {"from": 3, "off": 0, "size": 4},
             {"from": 4, "off": 0, "size": 4}, {"from": 5, "off": 0, "size": 4}]},
    # int f(raster) - "returns 1 if flag +0x23 high-bit clear, else calls
    # device" (vtable +0xb8). Read the flag byte so the return can be attributed
    # to a branch: r8 warns the no-call path returns constant 1 (degenerate).
    {"rva": "0x004c76f0", "cap": 24, "nargs": 4,
     "obs": [{"from": 0, "off": 0x23, "size": 1}]},
    # uint f(raster, level, flags) - "lock mip level, returns level or 0"
    # (vtable +0x84). Return is the observable.
    {"rva": "0x004c7860", "cap": 24, "nargs": 4, "obs": []},
    # int f(raster, image) - "device copy, sets raster flag +0x22 bit0"
    # (vtable +0x64). Both the return AND the flag bit are observable.
    {"rva": "0x004d5310", "cap": 24, "nargs": 4,
     "obs": [{"from": 0, "off": 0x22, "size": 1}]},
    # f(raster) - unlock (vtable +0x88). r8: "pure side-effect on the device; no
    # scalar observable". Read both flag bytes anyway - the note's claim is a
    # claim, and this is the cheapest way to test it rather than inherit it.
    {"rva": "0x004c7600", "cap": 24, "nargs": 4,
     "obs": [{"from": 0, "off": 0x22, "size": 1}, {"from": 0, "off": 0x23, "size": 1}]},
    # -- Group B: allocators / stream readers / dispatchers --------------------
    # r8 names NO observable for any of these seven. Return value is the only
    # candidate it implies (allocators return the thing they allocated).
    {"rva": "0x004c77c0", "cap": 24, "nargs": 4, "obs": []},   # RasterCreate
    # HOT: 15262 calls per load. cap raised 12->200 after the first capture came
    # back "one constant" - at cap 12 that was a sample of the first 0.08% of
    # calls, which is a statement about the cap, not about the function.
    {"rva": "0x004cc5e0", "cap": 200, "nargs": 4, "obs": []},  # sub-chunk header read
    {"rva": "0x004cee90", "cap": 24, "nargs": 4, "obs": []},   # level-image stream read (allocates)
    {"rva": "0x004cefd0", "cap": 24, "nargs": 4, "obs": []},   # gamma/flag fixup on read image
    {"rva": "0x004cdd00", "cap": 64, "nargs": 4, "obs": []},   # image destroy (frees) - likely void, watch d_args
    {"rva": "0x004c7650", "cap": 24, "nargs": 4, "obs": []},   # raster pre-resize helper (only 4 calls/load)
    {"rva": "0x004db2e0", "cap": 24, "nargs": 4, "obs": []},   # per-level image->raster mip convert
]


# The canonical-observation block is shared with replay_session.py. Appending it
# here (rather than keeping a second copy) is what keeps the two capture drivers
# byte-identical; it registers its own rpc.exports entries, so it has to land
# after the agent's own rpc.exports assignment - i.e. at the very end.
AGENT = AGENT + '\n' + (Path(__file__).resolve().parent / 'observe_block.js').read_text(encoding='utf-8')


def _keep_display_awake():
    """Stop the screensaver / display-sleep from tearing down the D3D device mid-race, and
    nudge the input queue to dismiss an already-active screensaver. Scoped to THIS process:
    ES_CONTINUOUS holds the request until the harness exits; no global power settings touched.
    Distinct from the reboot-only DirectShow-intro wedge — this only cures the display-asleep
    'no active display' CreateDevice failure (hr=0x8876086A / ChangeDisplaySettings=-1)."""
    try:
        import ctypes
        ES_CONTINUOUS, ES_SYSTEM_REQUIRED, ES_DISPLAY_REQUIRED = 0x80000000, 0x00000001, 0x00000002
        ctypes.windll.kernel32.SetThreadExecutionState(
            ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED)
        MOUSEEVENTF_MOVE = 0x0001                      # relative wake nudge (dismiss active saver)
        ctypes.windll.user32.mouse_event(MOUSEEVENTF_MOVE, 1, 0, 0, 0)
        ctypes.windll.user32.mouse_event(MOUSEEVENTF_MOVE, -1, 0, 0, 0)
        print("  [keep-awake] display-sleep suppressed + wake nudge sent")
    except Exception as e:
        print(f"  [keep-awake] skipped: {e}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--track", type=int, default=0,
                    help="engine track index 0..12 (NOT Course_Id/filename; RE'd via "
                         "ptr table 0x005f2728 -> 0x005f33f8): 0=Training 1=Egypt "
                         "2=Neustein 3=Arctic 4=Highway 5=Sands 6=SuperG 7=Roundabout "
                         "8=Storm 9=Forest 10=Dump 11=Warzone 12=City")
    ap.add_argument("--mode", type=int, default=10, help="game-mode (10=QuickRace, 2=TimeTrial)")
    ap.add_argument("--cars", type=int, default=1, help="active car slots (slot 0 = player; rest AI)")
    ap.add_argument("--car", type=int, default=0, help="player car/character index (DAT_0067ea98)")
    ap.add_argument("--rule", type=int, default=0, help="race-rule sub-mode 0..10 (DAT_007f0fd0)")
    ap.add_argument("--team", type=int, default=0, help="team-game flag (DAT_0067ea64; 1=team)")
    ap.add_argument("--powerups", type=int, default=-1, help="power-up setting (DAT_0067ea80; -1=game default)")
    ap.add_argument("--difficulty", type=int, default=-1, help="difficulty (DAT_0067ea7c; -1=game default)")
    ap.add_argument("--boost", type=float, default=0,
                    help="upward vel-Y impulse per tick on the player car to FORCE it airborne "
                         "(grounded->0 => A6b airborne body runs). 0=off. Contrived state (C3-grade).")
    ap.add_argument("--fps", default="60")
    ap.add_argument("--hold", type=int, default=20, help="seconds to hold in the race after spawn")
    ap.add_argument("--hooks", default="",
                    help="comma .asi hook RVAs/names to install LIVE + turn on the physics A/B "
                         "self-test (MASHED_PHYS_C4_SELFTEST -> original/phys_c4_*_selftest.log). "
                         "Empty = stock original. e.g. 0x00468980 for A6b airborne capture.")
    ap.add_argument("--oracle", action="store_true",
                    help="WS-G rules-debt: arm the RuleEngine oracle (hooks FUN_00410d10/"
                         "FUN_00410510/FUN_004177b0 read-only, predicts with the ported law, "
                         "compares per call). Writes log/rules_oracle_rule<r>.json.")
    ap.add_argument("--rule10-timer", type=float, default=None,
                    help="seed DAT_007f0fe4 (rule-10 countdown seconds) once at race start. "
                         "CONTRIVED state (the real seed FUN_004046a0 only runs in the real "
                         "challenge flow) — exercises the pre-expiry branch of the law.")
    ap.add_argument("--poke-lap", default="",
                    help="car:laps — set the lap counter (row 0x008a9620+car*0x30c+0x28) after "
                         "--poke-delay s; the original metric writer then produces a finished "
                         "metric. CONTRIVED (C3-grade) — forces finisher edges.")
    ap.add_argument("--poke-collect", default="",
                    help="total:done — set rule-5 collect counters DAT_0063a5d0/DAT_0063a5d4 "
                         "after --poke-delay s. CONTRIVED (C3-grade).")
    ap.add_argument("--poke-delay", type=int, default=10,
                    help="seconds into the hold before applying --poke-lap/--poke-collect")
    ap.add_argument("--bypass-proxy", action="store_true",
                    help="D1 spike: Interceptor.replace VehiclePhysicsWorldStep 0x0047eb30 "
                         "with 'return 0' (the function's own null-world guard path) after "
                         "--bypass-at seconds of racing. Settles whether the RW-Physics "
                         "proxy-body world is load-bearing for rendered motion "
                         "(COLLISION_GATE_BRIEF_D1_2026-07.md Open-Unknown #1).")
    ap.add_argument("--bypass-at", type=float, default=3.0,
                    help="seconds into the hold before arming --bypass-proxy (lets world "
                         "init + the one-shot qhull hull build finish normally)")
    ap.add_argument("--statediff-out", default="",
                    help="write a per-frame MSD1 snapshot of one vehicle record (0xd04 bytes "
                         "at 0x008815a0+car*0xd04, one record per phase-3 render tick "
                         "0x004c1be0) to this path. Diff two captures with "
                         "re/tools/statediff/statediff.py. Suppresses the control-4 press "
                         "pulses (wall-clock-timed input would break cross-boot determinism).")
    ap.add_argument("--statediff-car", type=int, default=0,
                    help="car slot to snapshot for --statediff-out (default 0 = player)")
    ap.add_argument("--statediff-drive-late", action="store_true",
                    help="D2 variant B: like --statediff-drive but arm the cook injector only "
                         "AFTER phase 3 is reached, so track load runs uninstrumented. Requires "
                         "--statediff-drive. Trades frame-0 alignment (recover it via the "
                         "+0xBF4 countdown anchor, which is the documented drive anchor anyway).")
    ap.add_argument("--statediff-noop-cook", action="store_true",
                    help="D2 isolation control: attach the cook Interceptor (0x00496530) at the "
                         "same moment as --statediff-drive but with an EMPTY callback and no "
                         "forced input, to separate hot-path instrumentation cost from the "
                         "effect of the race starting. Ignored if --statediff-drive is set.")
    ap.add_argument("--statediff-drive", action="store_true",
                    help="statediff driving scenario: arm the cook injector (0x00496530) with "
                         "full accel / zero steer BEFORE the phase poke, so the forced input is "
                         "frame-locked to the race (cross-boot deterministic), unlike the "
                         "wall-clock-timed --spike drive arming")
    ap.add_argument("--statediff-steer", type=int, default=0, choices=[-1, 0, 1],
                    help="D2/A8 steer-sign: held steer for the drive injector. +1 -> descriptor "
                         "steer byte [2] (gSteer>0), -1 -> byte [3] (gSteer<0), 0 -> straight "
                         "(default). Applies to both --statediff-drive and --statediff-drive-late; "
                         "accel is always full. Lets the original be driven with a held steer so "
                         "its steer-sign convention (steerAng +0x1a8 vs velocity-heading change) "
                         "can be measured against the ported chain.")
    ap.add_argument("--observe-texture-cluster", action="store_true",
                    help="TEXTURE/RASTER CLUSTER CAPTURE (2026-09-02, parent booted lane). Record "
                         "what the ORIGINAL does - args, return value, and per-row dereferenced "
                         "memory - for the 12 rows of r8's texture/raster neighbourhood during a "
                         "real track load, and write log/texture_cluster_observe.json plus a "
                         "per-row degenerate/non-degenerate verdict. These 12 dispatch the RW "
                         "DEVICE vtable (D3D9-backed), so a synthetic path1 on a fabricated "
                         "raster returns 0 or faults; observing the real load is the route "
                         "around that. Counting invocations alone is NOT the point and is not "
                         "enough - that is what got 0x0047b9e0 refused. Use with a plain race "
                         "(all 12 fire on an ordinary track load; no special provocation).")
    ap.add_argument("--assert-course-load", action="store_true",
                    help="COURSE-LOAD VERIFIER (area-track r1; assert set corrected 2026-09-01, "
                         "U-9066): after reaching a loaded-course state (phase 3), check two "
                         "deterministic load-integrity observables (DAT_0066d704==1 and "
                         "DAT_0063ba78==DAT_0063ba7c) and print PASS/FAIL + write "
                         "log/course_load_assert.json. DAT_0063ba8c is REPORTED RAW, not "
                         "asserted: an XrefRange scan shows it is a race state machine written "
                         "with 12 distinct constants by 8 functions, so the original "
                         "'DAT_0063ba8c==1' assert failed its own zero-hook baseline. Baseline (no "
                         "--hooks) must PASS; run again with --hooks <dispatcher cluster> and it "
                         "must STILL pass (no-regression) — that is how the load-dispatcher hooks "
                         "get their booted-race verification. Exits shortly after the assert "
                         "(no long hold needed); combine with --hold 0.")
    ap.add_argument("--spike-telemetry", default="",
                    help="tag: sample the player car at 10 Hz (render pos/vel/speed/yaw-rate/"
                         "heading/grounded) and write log/d1_spike_<tag>.json. In control "
                         "runs (no --bypass-proxy) also arms a 0x0047eb30 call counter.")
    args = ap.parse_args()

    if args.oracle and args.hooks:
        print("error: --oracle cannot be combined with --hooks. The oracle must observe the "
              "live ORIGINAL functions (FUN_00410d10/FUN_00410510/FUN_004177b0); --hooks can "
              "install the ported law (e.g. 0x004177b0, 0x00410510) over those very functions, "
              "so the oracle would validate the port against its own transcription — a vacuous "
              "GREEN. Run the oracle against the stock original (no --hooks).")
        return 2

    if not EXE.exists():
        print(f"error: {EXE} not found"); return 2

    env = dict(os.environ)
    env["MASHED_FPS_CAP"] = str(args.fps)
    if args.hooks == "all":
        # Full canonical hook set: default auto-hook (no MASHED_HOOK_ONLY filter).
        env["MASHED_RE_DEV"] = "1"
        env["MASHED_PHYS_C4_SELFTEST"] = "1"
        env.pop("MASHED_HOOK_ONLY", None)
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    elif args.hooks:
        env["MASHED_RE_DEV"] = "1"
        env["MASHED_HOOK_ONLY"] = args.hooks
        env["MASHED_PHYS_C4_SELFTEST"] = "1"
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    else:
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"     # stock original, no installed hooks
    if args.statediff_out:
        # The C4 selftest re-executes hook bodies in-process (A3 spawn runs 3x per
        # call with only partial rollback — survey 2026-07-31) and temp-patches
        # control flow. Statediff runs must observe ONE clean execution per call.
        env.pop("MASHED_PHYS_C4_SELFTEST", None)
    _keep_display_awake()
    dev = frida.get_local_device()
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env)
    pid = proc.pid
    print(f"=== scenario_launch  pid={pid}  track={args.track} mode={args.mode} cars={args.cars} ===")
    print("  attaching ASAP...")
    sess = None
    for _ in range(200):
        try: sess = dev.attach(pid); break
        except Exception: time.sleep(0.1)
    if sess is None:
        print("  error: could not attach")
        try: proc.kill()
        except Exception: pass
        return 3

    sd_records = []          # (frame_idx, 0xd04 bytes) — statediff capture buffer

    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "sd" and d is not None:
            sd_records.append((p["f"], d)); return
        if p.get("kind") in ("ready", "err"): print("  [agent]", p.get("msg") or "ready")

    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    E = scr.exports_sync

    # MASHED_COUNT_RVAS=0x00409900,0x00408a70 — arm invocation counters BEFORE the
    # phase poke so the track-load path is covered. Proves a path actually executed;
    # a clean scenario run verifies nothing about a function that never ran.
    _count_csv = os.environ.get("MASHED_COUNT_RVAS", "").strip()
    if _count_csv:
        print("  [counters]", E.arm_counters(_count_csv))

    # Arm the texture/raster observation BEFORE the phase poke, for the same
    # reason the counters are armed here: the track load is what exercises this
    # cluster, and it happens during the poke, not after it.
    if args.observe_texture_cluster:
        import json as _tj
        # MASHED_OBSERVE_SPEC=<path.json> points the same capture at any RVA set.
        # The machinery is not texture-specific - it records args/return/derefs
        # for whatever it is given - and the next two lanes (the replay/ghost
        # family, and finding an observable for 0x0047b9e0) need exactly this.
        _spec_path = os.environ.get("MASHED_OBSERVE_SPEC", "").strip()
        if _spec_path:
            _spec = _tj.loads(Path(_spec_path).read_text(encoding="utf-8"))
            print(f"  [texobs] spec from {_spec_path} ({len(_spec)} rows)")
        else:
            _spec = TEXTURE_CLUSTER_SPEC
        print("  [texobs]", E.tex_observe(_tj.dumps(_spec)))

    def wait_phase(target, timeout, label):
        end = time.time() + timeout
        last = None
        while time.time() < end:
            if psutil and not psutil.pid_exists(pid):
                print(f"  game exited while waiting for {label}"); return None
            try: ph = E.phase()
            except Exception: ph = None
            if ph != last:
                print(f"    phase={ph}  (waiting for {label})"); last = ph
            if ph == target: return ph
            time.sleep(0.25)
        print(f"  TIMEOUT waiting for {label} (last phase={last})"); return None

    rc = 1
    try:
        # 1) wait for the menu (main loop live, phase 1)
        if wait_phase(1, 40, "menu (phase 1)") is None: raise SystemExit
        time.sleep(0.5)
        # asi:ExportName counters could not resolve at attach time (dinput8 had not
        # loaded mashed_re_dev.asi yet). The menu being up means it is loaded now.
        if "asi:" in _count_csv:
            print("  [counters/asi]", E.rearm_asi())
        # 2) write the selection globals
        cfg = {"track": args.track, "mode": args.mode, "cars": args.cars, "car": args.car,
               "rule": args.rule, "team": args.team,
               "difficulty": args.difficulty, "powerups": args.powerups}
        print("  [setup]", E.setup(cfg))
        if args.statediff_out:
            # Arm BEFORE the phase poke so frame 0 = the very first phase-3 tick.
            print("  [statediff]", E.sd_arm(args.statediff_car))
            if args.statediff_drive and not args.statediff_drive_late:
                print("  [statediff]", E.arm_cook())
                print(f"  [statediff] drive: full accel, steer={args.statediff_steer:+d} ->",
                      E.drive(1, args.statediff_steer))
            elif args.statediff_drive_late:
                print("  [statediff] drive-late: cook NOT armed yet "
                      "(deferred until phase 3 to keep track load uninstrumented)")
            elif args.statediff_noop_cook:
                # D2 isolation control: instrumentation without the drive.
                print("  [statediff]", E.arm_cook_noop())
        time.sleep(0.2)
        # 3) poke the state machine into load+spawn
        print("  [launch] poke DAT_00771968 = 2 ->", E.launch())
        if args.oracle:
            print("  [oracle]", E.arm_oracle())
        # 4) wait for the race to be running (phase 3)
        ph3 = wait_phase(3, 40, "race running (phase 3)")
        if ph3 is None: raise SystemExit
        print("\n  *** RACE RUNNING (phase 3) ***")
        if args.assert_course_load:
            import json
            # Give the phase-2 load chain a moment to finish writing the post-load flags.
            time.sleep(1.0)
            try:
                res = json.loads(E.course_load_asserts())
            except Exception as ex:
                res = {"pass": False, "err": f"rpc failed: {ex}"}
            res["run"] = {"track": args.track, "mode": args.mode, "cars": args.cars,
                          "hooks": args.hooks or None, "pid": pid,
                          "ts": time.strftime("%Y-%m-%d %H:%M:%S")}
            out = ROOT / "log" / "course_load_assert.json"
            out.parent.mkdir(exist_ok=True)
            out.write_text(json.dumps(res, indent=1))
            print("\n  === COURSE-LOAD VERIFIER ===")
            for k, v in (res.get("asserts") or {}).items():
                print(f"    {'OK ' if v.get('ok') else 'FAIL'}  {k}   {v}")
            for k, v in (res.get("observations") or {}).items():
                # NOT asserted: raw state-machine values, printed so baseline and hooked
                # runs can be diffed by eye without the verdict depending on them (U-9066).
                print(f"    obs   {k} = {v}   (not asserted)")
            if res.get("err"):
                print(f"    agent err: {res['err']}")
            verdict = "PASS" if res.get("pass") else "FAIL"
            print(f"  COURSE-LOAD VERDICT: {verdict}   hooks={args.hooks or 'none(baseline)'}   -> {out}")
            rc = 0 if res.get("pass") else 4
            # No long hold needed for the verifier; tear down after the assert.
            raise SystemExit
        if args.statediff_out and args.statediff_drive_late:
            # D2 variant B: arm the cook injector only NOW, so phase 2 (track
            # load + car spawn) runs with 0x00496530 uninstrumented. Costs the
            # frame-0 == first-phase-3-tick alignment, but the documented drive
            # anchor is the +0xBF4 countdown witness anyway (deterministic to
            # one frame), so alignment is unaffected in practice.
            print("  [statediff]", E.arm_cook())
            print(f"  [statediff] drive-late: full accel, steer={args.statediff_steer:+d} ->",
                  E.drive(1, args.statediff_steer))
        if args.rule10_timer is not None:
            print("  [rule10-timer] DAT_007f0fe4 =", args.rule10_timer,
                  "->", E.poke_timer(args.rule10_timer))
        # 5) confirm a car spawned + read its record
        for _ in range(8):       # give the spawn a moment to populate the record
            time.sleep(0.5)
            ci = E.carinfo()
            if ci.get("spawnFired", 0) > 0: break
        print(f"  car spawn fired: {ci.get('spawnFired')}   grounded={ci.get('grounded')}"
              f"  airflag={ci.get('airflag')}")
        print(f"  vel={ci.get('vel')}  fwd={ci.get('pos_via_fwd')}")
        if ci.get("spawnFired", 0) > 0:
            print("\n  VERDICT: launcher reached a running race and spawned a car. [OK]")
            rc = 0
        else:
            print("\n  VERDICT: phase 3 reached but VehicleSpawnInit never fired — spawn incomplete.")
        if args.spike_telemetry:
            print("  [spike]", E.tel_start())
            print("  [spike]", E.arm_cook())
            print("  [spike] drive: full accel, straight ->", E.drive(1, 0))
            if not args.bypass_proxy:
                print("  [spike]", E.arm_step_counter())
        print(f"\n  racing {args.hold}s — pulsing control 4 (confirm/accel) to skip the start intro + continue rounds...")
        t0 = time.time(); t = t0 + args.hold; n = 0; poked = False; oracle_cache = None
        tel_cache = None; bypass_armed = False; exited_early = False; steer_on = False
        while time.time() < t:
            if psutil and not psutil.pid_exists(pid):
                print("\n  game exited."); exited_early = True; break
            if args.bypass_proxy and not bypass_armed and time.time() - t0 >= args.bypass_at:
                bypass_armed = True
                print(f"\n  [spike] +{time.time()-t0:.1f}s", E.arm_bypass())
            if args.spike_telemetry and not steer_on and time.time() - t0 >= 8.0:
                steer_on = True
                try: print(f"\n  [spike] +{time.time()-t0:.1f}s drive: accel + steer ->", E.drive(1, 1))
                except Exception: pass
            if not args.statediff_out:      # press pulses are wall-clock-timed nondeterministic input
                try: E.press(4, 250)        # pulse: 250ms held, ~0.35s gap -> edges for round-end prompts
                except Exception: pass
            if (args.poke_lap or args.poke_collect) and not poked \
                    and time.time() - t0 >= args.poke_delay:
                poked = True
                try:
                    if args.poke_lap:
                        car, laps = (int(x) for x in args.poke_lap.split(":"))
                        print(f"\n  [poke-lap] car {car} laps={laps} ->", E.poke_lap(car, laps))
                    if args.poke_collect:
                        tot, done = (int(x) for x in args.poke_collect.split(":"))
                        print(f"\n  [poke-collect] total={tot} done={done} ->", E.poke_collect(tot, done))
                except Exception as ex:
                    print(f"\n  poke failed: {ex}")
            if args.boost:
                for _ in range(4):          # re-launch a few times per tick so it stays airborne
                    try: E.boost(args.boost)
                    except Exception: pass
                    time.sleep(0.08)
            n += 1
            if n % 8 == 0:
                try:
                    ci = E.carinfo()
                    try: ci["ph"] = E.phase()
                    except Exception: ci["ph"] = "?"
                    print(f"\r    +{int(time.time()-t0):>3}s  ph={ci['ph']}  spawnFired={ci.get('spawnFired')}"
                          f"  p0.grounded={ci.get('grounded')} airflag={ci.get('airflag')}"
                          f"  vel={[round(v,1) for v in ci.get('vel',[0,0,0])]}   ", end="", flush=True)
                except Exception: pass
            if args.oracle and n % 3 == 0:
                try: oracle_cache = E.oracle_stats()   # crash-proof incremental snapshot
                except Exception: pass
            if args.spike_telemetry and n % 3 == 0:
                try: tel_cache = E.telemetry()         # crash-proof incremental snapshot
                except Exception: pass
            time.sleep(0.6)
        print()
        if args.spike_telemetry:
            try:
                import json
                try: raw = E.telemetry()
                except Exception:
                    raw = tel_cache
                    print("  (spike: live telemetry fetch failed, using last snapshot)")
                if raw is None:
                    print("  spike: NO telemetry captured")
                else:
                    st = json.loads(raw)
                    st["run"] = {"tag": args.spike_telemetry, "bypass": args.bypass_proxy,
                                 "bypass_at": args.bypass_at if args.bypass_proxy else None,
                                 "track": args.track, "mode": args.mode, "cars": args.cars,
                                 "hold": args.hold, "pid": pid, "exited_early": exited_early,
                                 "ts": time.strftime("%Y-%m-%d %H:%M:%S")}
                    out = ROOT / "log" / f"d1_spike_{args.spike_telemetry}.json"
                    out.parent.mkdir(exist_ok=True)
                    out.write_text(json.dumps(st))
                    print(f"  spike telemetry: {len(st['rows'])} samples"
                          f"  exited_early={exited_early}  -> {out}")
            except Exception as ex:
                print(f"  spike telemetry dump failed: {ex}")
        if args.oracle:
            try:
                import json
                try: raw = E.oracle_stats()
                except Exception:
                    raw = oracle_cache          # game/script died — use last snapshot
                    print("  (oracle: live fetch failed, using last incremental snapshot)")
                if raw is None:
                    print("  oracle: no oracle snapshot captured — game exited before the first "
                          "incremental snapshot; no evidence file written")
                    raise SystemExit
                st = json.loads(raw)
                out = ROOT / "log" / f"rules_oracle_rule{args.rule}.json"
                out.parent.mkdir(exist_ok=True)
                st["run"] = {"track": args.track, "mode": args.mode, "cars": args.cars,
                             "rule": args.rule, "hold": args.hold, "pid": pid,
                             "hooks": args.hooks or None,
                             "ts": time.strftime("%Y-%m-%d %H:%M:%S")}
                out.write_text(json.dumps(st, indent=1))
                seg, ev, ordr = st["seg"], st["ev"], st["ord"]
                print(f"\n  === RULES ORACLE (rule={args.rule}) ===")
                print(f"  SegmentCheck  0x00410d10: calls={seg['calls']} agree={seg['agree']} "
                      f"MISMATCH={seg['mis']} segment-end(ret!=0)={seg['ret1']} byRule={seg['byRule']}")
                print(f"  EvaluateResult 0x00410510: calls={ev['calls']} agree={ev['agree']} "
                      f"MISMATCH={ev['mis']} byRule={ev['byRule']}")
                print(f"  FinishOrder   0x004177b0: calls={ordr['calls']} agree={ordr['agree']} "
                      f"MISMATCH={ordr['mis']} appends={ordr['appends']} round-resets={ordr['resets']}")
                if st.get("err"): print(f"  agent err: {st['err']}")
                verdict = "GREEN" if (seg["mis"] == 0 and ev["mis"] == 0 and ordr["mis"] == 0
                                      and (seg["calls"] or ev["calls"] or ordr["calls"])
                                      and not st.get("err")) else "RED"
                print(f"  ORACLE VERDICT: {verdict}   -> {out}")
            except Exception as ex:
                print(f"  oracle stats fetch failed: {ex}")
    except SystemExit:
        pass
    finally:
        if args.statediff_out:
            # MSD1 writer (re/tools/statediff/FORMAT.md). In finally so an early
            # game exit still yields whatever was captured.
            try:
                import struct
                try: print("  [statediff] agent:", E.sd_stats())
                except Exception: pass
                outp = Path(args.statediff_out)
                outp.parent.mkdir(parents=True, exist_ok=True)
                base_va = 0x008815a0 + args.statediff_car * 0xd04
                # Snapshot the list ONCE: the agent can still be appending during
                # teardown, and reading len(sd_records) again later reported 2340
                # against the 2335 actually on disk (2026-08-24). The provenance
                # sidecar must describe the bytes written, not a later count.
                sd_snapshot = list(sd_records)
                with open(outp, "wb") as f:
                    f.write(b"MSD1" + struct.pack("<III", 0xd04, base_va, 0))
                    for idx, payload in sd_snapshot:
                        f.write(struct.pack("<I", idx) + bytes(payload))
                # Non-degeneracy: a capture of N identical (or all-zero) records
                # verifies nothing (feedback_evidence_discipline).
                distinct = len({bytes(p) for _, p in sd_snapshot})
                nonzero = sum(1 for _, p in sd_snapshot if any(bytes(p)))
                print(f"  [statediff] {len(sd_snapshot)} frames -> {outp}"
                      f"  (distinct payloads={distinct}, nonzero={nonzero})")
                if not sd_snapshot:
                    print("  [statediff] WARNING: EMPTY capture — no phase-3 render tick observed")
                elif distinct <= 1:
                    print("  [statediff] WARNING: DEGENERATE capture — record never changed")
                # PROVENANCE SIDECAR (added 2026-08-24, D2/A8). A capture with no
                # record of how it was made is not a datum: the 2026-08-23 A8
                # capture verify/a8_steer_20260823/orig_steerR.msd could not be
                # confirmed to have had --statediff-steer in effect, because the
                # directory held the .msd and nothing else. Always write this.
                try:
                    import json as _json, platform as _plat, subprocess as _sp
                    _sha = ""
                    try:
                        _sha = _sp.check_output(["git", "rev-parse", "HEAD"],
                                                stderr=_sp.DEVNULL, text=True).strip()
                    except Exception:
                        pass
                    _prov = {
                        "msd": outp.name,
                        "captured_utc": __import__("datetime").datetime.now(__import__("datetime").timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
                        "argv": sys.argv,
                        "cwd": str(Path.cwd()),
                        "git_head": _sha,
                        "host": _plat.node(),
                        "statediff": {
                            "car": args.statediff_car,
                            "base_va": hex(base_va),
                            "rec_size": "0xd04",
                            "drive": bool(args.statediff_drive),
                            "drive_late": bool(args.statediff_drive_late),
                            "steer": args.statediff_steer,
                            "noop_cook": bool(args.statediff_noop_cook),
                            "hooks": getattr(args, "hooks", ""),
                        },
                        "capture": {
                            "frames": len(sd_snapshot),
                            "distinct_payloads": distinct,
                            "nonzero_frames": nonzero,
                        },
                        # what the injector actually wrote, so a future reader does
                        # not have to re-derive the byte map from the source
                        "descriptor_writes": {
                            "block_base": "0x007f1038",
                            "accel_byte": 4,
                            "steer_bytes": [0, 1],
                            "legacy_steer_bytes": [2, 3, 14, 15],
                        },
                    }
                    _pp = outp.with_suffix(outp.suffix + ".provenance.json")
                    _pp.write_text(_json.dumps(_prov, indent=2), encoding="utf-8")
                    print(f"  [statediff] provenance -> {_pp}")
                except Exception as _pex:
                    print(f"  [statediff] provenance write FAILED: {_pex}")
            except Exception as ex:
                print(f"  [statediff] write failed: {ex}")
        if _count_csv:
            try:
                print("  [counters] " + E.counters())
            except Exception as ex:
                print(f"  [counters] fetch failed: {ex}")
        if args.observe_texture_cluster:
            try:
                import json as _tj
                rows = _tj.loads(E.tex_results())
                outp = ROOT / "log" / (os.environ.get("MASHED_OBSERVE_OUT", "").strip()
                                       or "texture_cluster_observe.json")
                outp.parent.mkdir(parents=True, exist_ok=True)
                outp.write_text(_tj.dumps(rows, indent=1), encoding="utf-8")
                print(f"  [texobs] -> {outp}")
                # Per-row verdict on the SPOT, because the whole point of this
                # capture is to decide which rows even HAVE a witnessable
                # observable. distinct(ret) and distinct(obs) are the numbers
                # that separate "ran and was correct" from "never ran": a row
                # whose return and derefs are one constant across every call is
                # degenerate and must NOT be promoted off this run.
                # distinct_args matters independently of distinct_ret: a void
                # function's return register is whatever the body happened to
                # leave in EAX, so a constant there is not evidence of anything.
                # Varying INPUT with a constant output is the real degenerate
                # shape; constant input is just an under-exercised capture.
                print("  rva          calls  recs  d_args  d_ret  d_obs  moved  verdict")
                for rva, r in rows.items():
                    recs = r.get("recs") or []
                    dargs = {tuple(x.get("args") or []) for x in recs}
                    drets = {x.get("ret") for x in recs}
                    dobs = {tuple(o.get("v") for o in (x.get("obs") or [])) for x in recs}
                    # moved = calls where an absolute-block observable actually
                    # CHANGED across the call. This is the strongest signal in
                    # the table: it is a within-call delta, so unlike a distinct
                    # count it cannot be produced by the surrounding scenario
                    # drifting on its own.
                    moved = sum(1 for x in recs
                                if any(o.get("moved") for o in (x.get("obs") or [])))
                    if not recs:
                        v = "NEVER RAN"
                    elif moved:
                        v = f"non-degenerate (writes on {moved}/{len(recs)})"
                    elif len(drets) > 1 or len(dobs) > 1:
                        v = "non-degenerate"
                    elif len(dargs) > 1:
                        v = "DEGENERATE (varying args, constant output)"
                    else:
                        v = "UNDER-EXERCISED (input constant too)"
                    print(f"  {rva}  {r.get('calls',0):6d}  {len(recs):4d}  "
                          f"{len(dargs):6d}  {len(drets):5d}  {len(dobs):5d}  {moved:5d}  {v}")
            except Exception as ex:
                print(f"  [texobs] fetch failed: {ex}")
        try: sess.detach()
        except Exception: pass
        try:
            if (not psutil) or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
