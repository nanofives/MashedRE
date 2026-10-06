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
    // steer, per the RVA-cited map (A4 reads these two). MAGNITUDE-aware since
    // 2026-09-13 (A8 ramp capture): A4 consumes the byte as a scalar
    // (BodyOrientationIntegrate.cpp: sL = (float)in[0]), so |gSteer| in (0,1] maps
    // to round(|gSteer|*255); +-1 still writes 0xff exactly as before.
    const sm = Math.min(255, Math.round(Math.abs(gSteer) * 255));
    b.add(0).writeU8(gSteer > 0 ? sm : 0);
    b.add(1).writeU8(gSteer < 0 ? sm : 0);
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
  misrec:[], samp:[], lastOrder:null,
  // D3 2026-09-26: per-rule ENTRY-input ranges over EVERY SegmentCheck call, so a
  // run proves which pre-block inputs it actually exercised (samp only keeps
  // segment ends). Non-degeneracy evidence, not a verdict.
  seen:{},
  // D3 2026-09-26: every FUN_004046a0 (rule-10 seed) exit: when (as a SegmentCheck
  // call index) and what DAT_007f0fe4 it left. Cold (once per race/round setup).
  seed10:[] };
function orSeen(en){
  const k = en.rule, s = OR.seen[k] || (OR.seen[k] = {n:0, collTotMax:0, collDoneMax:0,
    timerMin:null, timerMax:null, snap1:{}, m0Max:null, m1Max:null, motion0Nz:0, deadMax:0});
  s.n++;
  if (en.collectTotal > s.collTotMax) s.collTotMax = en.collectTotal;
  if (en.collectDone > s.collDoneMax) s.collDoneMax = en.collectDone;
  if (s.timerMin === null || en.timer < s.timerMin) s.timerMin = en.timer;
  if (s.timerMax === null || en.timer > s.timerMax) s.timerMax = en.timer;
  if (Object.keys(s.snap1).length < 8) s.snap1[en.snapshot1] = (s.snap1[en.snapshot1] || 0) + 1;
  else if (s.snap1[en.snapshot1] !== undefined) s.snap1[en.snapshot1]++;
  if (s.m0Max === null || en.metric[0] > s.m0Max) s.m0Max = en.metric[0];
  if (s.m1Max === null || en.metric[1] > s.m1Max) s.m1Max = en.metric[1];
  if (en.motion[0] !== 0) s.motion0Nz++;
  const dead = en.alive.filter(a => !a).length; if (dead > s.deadMax) s.deadMax = dead;
}
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
      onEnter(){ try { this.en = orReadCars(); orSeen(this.en); } catch(e){ OR.err = 'seg.enter '+e; } },
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
    Interceptor.attach(ga(0x004046a0), {
      onLeave(){ try { if (OR.seed10.length < 40) OR.seed10.push({atSeg: OR.seg.calls,
        segRet1: OR.seg.ret1, rule: ga(0x007f0fd0).readS32(), timer: ga(0x007f0fe4).readFloat(),
        state: ga(0x0063ba8c).readS32()}); } catch(e){ OR.err = 'seed10 '+e; } }
    });
    OR.armed = true;
    return 'oracle armed (0x00410d10 + 0x00410510 + 0x004177b0 + seed 0x004046a0)';
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
//
// [D3 2026-09-14] The same tick ALSO samples the AI control block for the same car,
// as an additive JSON field on the existing 'sd' message (the MSD1 binary payload is
// untouched, so every existing .msd reader is unaffected). This is the observable the
// D3 AI gate needs: the bytes the original's FUN_00416250 writes each frame.
//   ctrl block   base 0x007f1038 stride 0x4c, slot = *(int*)(0x007f1a14 + car*0x10)
//                (Ai/AiState.h, cited to FUN_00418560 @0x00418575 / 0x0041856c)
//   byte map     [0],[1] steer pair  [3] fire  [4] accel  [5] brake
//                (re/analysis/ai_ctrl_byte_map_RESOLVED_2026-06-16.md)
//   ai record    base 0x0089a4cc stride 0x74: +0x00 line type, +0x04 spline index,
//                +0x30 input-override countdown, +0x60 behaviour mode
// Join to the .msd on the frame index for position/velocity; no field offset of the
// 0xd04 record is assumed here.
const RENDER_TICK = 0x004c1be0;   // render-frame clock
const CTRL_BASE   = 0x007f1038;   // FUN_00418560 0x00418575
const CTRL_STRIDE = 0x4c;         // FUN_00418560 0x00418572
const SLOT_TABLE  = 0x007f1a14;   // FUN_00418560 0x0041856c
const SLOT_STRIDE = 0x10;
const AISTATE     = 0x0089a4cc;   // stride 0x74
const SD = { armed:false, frames:0, car:0, err:null, ai:false, aiErr:null };
function sdArm(car, withAi){
  if (SD.armed) return 'already armed';
  SD.car = car;
  SD.ai  = !!withAi;
  try {
    const rec = ga(CARREC).add(car * 0xd04);
    const ph  = ga(PHASE);
    const slotp = ga(SLOT_TABLE).add(car * SLOT_STRIDE);
    const aip   = ga(AISTATE).add(car * 0x74);
    Interceptor.attach(ga(RENDER_TICK), { onEnter(){
      try {
        if (ph.readU8() !== 3) return;
        let msg = {kind:'sd', f: SD.frames++};
        if (SD.ai && SD.aiErr === null) {
          try {
            const slot = slotp.readS32();
            const c = ga(CTRL_BASE).add(slot * CTRL_STRIDE);
            msg.ai = [slot, c.readU8(), c.add(1).readU8(), c.add(3).readU8(),
                      c.add(4).readU8(), c.add(5).readU8(),
                      aip.readS32(), aip.add(0x04).readS32(),
                      aip.add(0x30).readS32(), aip.add(0x60).readS32()];
            // [D3] slot-table cross-check: the first car-1 capture (2026-09-14) read
            // slot==0 for car 1, i.e. the SAME block the player uses. Rather than assume
            // the AiState.h indexing is right OR wrong, dump all four blocks' steer/
            // accel/brake every frame; if blocks 1..3 carry distinct per-car commands
            // then the lookup is at fault, and if they do not, the block really is
            // shared. Read, do not infer.
            for (let k = 0; k < 4; ++k) {
              const b = ga(CTRL_BASE).add(k * CTRL_STRIDE);
              msg.ai.push(b.readU8(), b.add(1).readU8(),
                          b.add(4).readU8(), b.add(5).readU8());
            }
          } catch(e){ SD.aiErr = '' + e; }
        }
        send(msg, rec.readByteArray(0xd04));
      } catch(e){ if (!SD.err) SD.err = '' + e; }
    }});
    SD.armed = true;
    return 'statediff armed (tick 0x004c1be0, car ' + car + ', rec@' + rec + ')'
           + (SD.ai ? ' + aictrl (slot table @' + slotp + ')' : '');
  } catch(e){ return 'ERR ' + e; }
}

// --- AI CONTROL-STEP capture (D3, 2026-09-14) ------------------------------
// Ground truth for what the ORIGINAL's opponent AI commands, taken at the
// function boundary instead of through the slot table.
//
// WHY NOT THE SLOT TABLE: the first D3 capture read slot = *(int*)(0x007f1a14 +
// car*0x10) == 0 for car 1, and blocks 1..3 stayed all-zero for 2710 frames while
// only block 0 moved. FUN_00418560's decompilation (pool0, 2026-09-14) confirms the
// AiState.h indexing is right -- `iVar7 = (&DAT_007f1a14)[param_1 * 4]` on an int*
// IS byte offset param_1*0x10, and the writes land at block+0/+1/+4/+5 with
// stride 0x4c -- so slot 0 is genuinely what the table HOLDS in a warp-launched
// race. The table's writers are all in the frontend/race-launch band (WRITEs at
// 0x0043df41, 0x0042b991, 0x0042bab0, 0x0042baf5, 0x0043f14f, 0x0043f820,
// 0x0043f895), which the scenario warp poke does not run.
//
// So hook the control step and read the block pointer the caller actually passes.
// FUN_00418560 @0x004187b7 calls FUN_00416250 cdecl with four stack args --
// EAX=spline, EBP=v, EDI=block, 0x42c80000 -- pushed BEFORE the shared branch at
// 0x004187b7 (which is why Ghidra renders the call with no arguments; cf.
// feedback_ghidra_prebranch_args). EDI is THAT car's ctrl block, whatever the
// slot table says. Fires once per AI car per frame (~180/s at 4 cars) -- well
// under the 1000/s Interceptor limit in CLAUDE.md.
//
// Byte map (re/analysis/ai_ctrl_byte_map_RESOLVED_2026-06-16.md, re-confirmed
// against FUN_00418560's writes this session): [0],[1] steer pair, [3] fire,
// [4] accel, [5] brake.
const CTRL_STEP = 0x00416250;
// [D3 2026-09-27] SECOND probe, the one that answers "why does car 1 split
// differently": the lookahead TARGET and the FUN_00443440 CURVATURE are stack
// locals of FUN_00416250 and are gone by the time the outer onLeave runs.
//
// MEASURED 2026-09-27, two dead ends before this shape (both killed the game inside
// 2 render ticks, with MASHED_AISTEP_LOCALS=0 as the control, which completed a
// 5463-call capture): Interceptor.attach at 0x004165a5 (`fld [0x5d757c]`, esp == F)
// and at 0x0041657c (`mov edx,[esp+0x38]`, esp == F). Frida's Interceptor is an
// ENTRY hook — it swaps the dword at [esp] to route the return through its leave
// trampoline — so pointing it mid-body makes it overwrite a LOCAL. The address being
// a plain MOV rather than a call or an x87 op made no difference, which is what rules
// out both the x87 and the call-relocation explanations.
//
// So take the two values at genuine FUNCTION ENTRIES instead, one callee each:
//   0x00415e20  FUN_00415e20(v, tgt_x, tgt_z) — the FINAL target, after the whole
//               targeting chain, because 0x00416580..0x0041658f pushes it straight
//               out of [F+0x34]/[F+0x38] into this call.
//   0x00443440  FUN_00443440(spline, &xz, 10.0f, &curv_out, 0) — args pushed at
//               0x0041629a..0x004162ab; the out float is folded to [0,180] by the
//               caller at 0x004162be..0x004162d5 (`if (180 < c) c = 360 - c`), so the
//               fold is applied here to match what the bands actually see.
// Both are strictly nested inside the FUN_00416250 call being recorded, so one
// pending slot is enough on the single-threaded game loop, and the CTRL_STEP onEnter
// clears it so a stale value can never be attributed to the next call.
//
// The steering ERROR is not read off the stack at all: FUN_00416250 stores it to a
// GLOBAL on both band paths — 0x008032dc + v*0x14 at 0x004165f7 (band err < 180, with
// 0x008032d8 forced to 360.0 at 0x004165cc) and 0x008032d8 + v*0x14 at 0x0041670c
// (band err > 180, with 0x008032dc forced to 0.0 at 0x004166db). Reading both globals
// at the outer onLeave gives the error AND which of the two steering bands took it,
// with no stack dependency. Own x/z likewise come from the record base the existing
// rec_9e4 column already uses (0x008815a0 + v*0xd04, +0x30/+0x38 per 0x0041628d).
// [D3 2026-09-27 pass 2] FUN_00416230(v, idx == 0) is called once per pass of
// FUN_00443dc0's phase-8 wall-march (0x00444a2c, inside the `je 0x4446a4` loop at
// 0x00444a3a), and its whole body is `[0x89a500 + v*0x74] = arg2` (0x0041623b). So
// counting its calls between two FUN_00416250 entries = how many times the lookahead
// had to step its target back, and the last arg2 = whether it ended up at walk point 0.
// That is the shared observable for "is the standalone's target short because the
// wall-march keeps rejecting it".
const MARCH_FN    = 0x00416230;
// [D3 2026-09-27 pass 3] The car's own XZ. MEASURED: reading 0x008815a0 + v*0xd04 +
// 0x30/+0x38 (the base the rec_9e4 column uses) returns 0.0 on every row, so that base
// is NOT where FUN_00416250 gets the position: it calls FUN_0046d4a0(&p, v) and then
// reads *(p + 0x30) / *(p + 0x38) (0x00416284 / 0x0041628d / 0x00416297). So learn the
// per-vehicle record POINTER from FUN_0046d4a0 once, cache it, and detach — an entry
// hook on a helper this hot would otherwise sit on the AI frame budget for the whole run.
const RECPTR_FN   = 0x0046d4a0;
const AS_RECP     = {};
let AS_RECL       = null;
const STEER_ANGLE = 0x00415e20;
const CURV_FN     = 0x00443440;
// [D3 2026-10-03, PREREG_STEP2B.md 2B.5/2B.6] the two TARGETING producers whose
// return values decide `mode` at 0x004162f7..0x0041645e and, for FUN_00414a70 == 2,
// take the IMMEDIATE-RETURN arm at 0x00416405 that writes ctrl[4]=0, ctrl[5]=0xff
// and leaves FUN_00416250 before the steer-history stores at 0x004165cc /
// 0x0041670c. Both are stubbed in the port (AiControlStep.cpp:91 / no body at all),
// which is the hypothesis under test. Both are called at most once per
// FUN_00416250 call and are strictly nested inside it, so one pending slot each is
// enough on the single-threaded game loop; CTRL_STEP's onEnter clears them so a
// stale value can never be attributed to the next call.
const TGT_14A70   = 0x00414a70;   // closest-vehicle   (1 = chase, 2 = brake)
const TGT_14C30   = 0x00414c30;   // obstacle avoidance (1 -> mode 3, 2 -> mode 7)
const TGT_150E0   = 0x004150e0;   // track-wall lateral-zone query (the mode-9 arm,
                                  // reached only when 14c30 returns 0)
const TGT_16060   = 0x00416060;   // line-of-sight ray-march; every mode commit is
                                  // conjoined with it (0x004163ab / 0x004163da /
                                  // 0x00416423), so a 0 here is a committed-mode veto
const TGT_148B0   = 0x004148b0;   // leader-ranking timer. In the `local_48 == 0` arm of
                                  // the `gameMode == 6 && FUN_00443080() == 0` block
                                  // (0x0041649b..), `FUN_004148b0 != 0 && LOS != 0` takes
                                  // a SECOND immediate return: ctrl[5] = 0xff, ctrl[0] =
                                  // ctrl[1] = 0, return -- before the mode commit at
                                  // 0x00416590 and before the steer-history stores, and
                                  // WITHOUT touching ctrl[4], which therefore keeps its
                                  // entry value (hence the c4_in column).
let AS_TGT = [-1, -1, -1, -1, -1, 0, 0, 0, 0, 0];
//           [ret 14a70, ret 14c30, ret 150e0, ret 16060, ret 148b0, calls x5]
let AS_PEND = null;      // [tgt_x, tgt_z, curv, v_of_steer_call]
let AS_MARCH = [0, -1];  // [passes since the last CTRL_STEP entry, last idx0 flag]
const AS = { armed:false, rows:[], calls:0, err:null, cap:200000,
             locals:0, curv:0, localsErr:null, joinMiss:0, noLocals:0 };
function aiStepArm(withLocals){
  if (AS.armed) return 'already armed';
  try {
    const ph = ga(PHASE);
    if (withLocals) {
      Interceptor.attach(ga(CURV_FN), { onEnter(a){
        try {
          // only the FUN_00416250 call site: dist == 10.0f and the 5th arg == 0
          const sp = this.context.esp;
          if (sp.add(12).readU32() !== 0x41200000 || sp.add(20).readS32() !== 0) return;
          this.out = sp.add(16).readPointer();
        } catch(e){ if (!AS.localsErr) AS.localsErr = 'curvEnter ' + e; }
      }, onLeave(){
        try {
          if (!this.out) return;
          let c = this.out.readFloat();
          if (c > 180.0) c = 360.0 - c;       // 0x004162be..0x004162d5
          if (AS_PEND) AS_PEND[2] = c; else AS_PEND = [null, null, c, -1];
          AS.curv++;
        } catch(e){ if (!AS.localsErr) AS.localsErr = 'curvLeave ' + e; }
      }});
      AS_RECL = Interceptor.attach(ga(RECPTR_FN), {
        onEnter(){ try { const sp = this.context.esp;
                         this.o = sp.add(4).readPointer(); this.v = sp.add(8).readS32(); }
                   catch(e){ this.o = null; } },
        onLeave(){
          try {
            if (this.o === null || this.v < 0 || this.v > 3 || AS_RECP[this.v]) return;
            AS_RECP[this.v] = this.o.readPointer();
            if (AS_RECP[0] && AS_RECP[1] && AS_RECP[2] && AS_RECP[3] && AS_RECL) {
              AS_RECL.detach(); AS_RECL = null;     // learned; stop paying for it
            }
          } catch(e){ if (!AS.localsErr) AS.localsErr = 'recLeave ' + e; }
        }});
      Interceptor.attach(ga(MARCH_FN), { onEnter(){
        try {
          const sp = this.context.esp;
          AS_MARCH[0] += 1;
          AS_MARCH[1] = sp.add(8).readS32();
        } catch(e){ if (!AS.localsErr) AS.localsErr = 'marchEnter ' + e; }
      }});
      [[TGT_14A70, 0], [TGT_14C30, 1], [TGT_150E0, 2], [TGT_16060, 3], [TGT_148B0, 4]]
        .forEach(function(pair){
          Interceptor.attach(ga(pair[0]), { onLeave(ret){
            try { AS_TGT[pair[1]] = ret.toInt32(); AS_TGT[pair[1] + 5]++; }
            catch(e){ if (!AS.localsErr) AS.localsErr = 'tgt' + pair[0] + ' ' + e; }
          }});
        });
      Interceptor.attach(ga(STEER_ANGLE), { onEnter(){
        try {
          const sp = this.context.esp;
          const v  = sp.add(4).readS32();
          const tx = sp.add(8).readFloat();
          const tz = sp.add(12).readFloat();
          if (AS_PEND) { AS_PEND[0] = tx; AS_PEND[1] = tz; AS_PEND[3] = v; }
          else AS_PEND = [tx, tz, null, v];
          AS.locals++;
        } catch(e){ if (!AS.localsErr) AS.localsErr = 'steerEnter ' + e; }
      }});
    }
    Interceptor.attach(ga(CTRL_STEP), {
      onEnter(a){
        AS_PEND = null;          // so a stale inner probe can never be attributed
        AS_MARCH = [0, -1];
        AS_TGT = [-1, -1, -1, -1, -1,
                  AS_TGT[5], AS_TGT[6], AS_TGT[7], AS_TGT[8], AS_TGT[9]];
        this.skip = (ph.readU8() !== 3);
        if (this.skip) return;
        const sp = this.context.esp;
        this.spline = sp.add(4).readU32();
        this.v      = sp.add(8).readS32();
        this.blk    = sp.add(12).readPointer();
        // [D3 2026-10-03, PREREG_STEP2B.md 2B.6] the ctrl block ON ENTRY. The tail at
        // 0x004167d5 writes ctrl[4] = 0xff unconditionally but never zeroes ctrl[5],
        // so a logged (c4, c5) at onLeave cannot by itself tell "this call set the
        // brake" from "this call inherited it". Two extra reads in the hook that is
        // already here; appended columns, so every existing reader is unaffected.
        try { this.c4in = this.blk.add(4).readU8(); this.c5in = this.blk.add(5).readU8(); }
        catch(e){ this.c4in = -1; this.c5in = -1; }
      },
      onLeave(){
        if (this.skip || AS.rows.length >= AS.cap) return;
        try {
          const b = this.blk;
          const ai = ga(0x0089a4cc).add(this.v * 0x74);
          AS.rows.push([SD.frames, AS.calls++, this.v, this.blk.toUInt32(),
                        this.spline,
                        b.readU8(), b.add(1).readU8(), b.add(3).readU8(),
                        b.add(4).readU8(), b.add(5).readU8(),
                        ai.readS32(), ai.add(0x04).readS32(),
                        ai.add(0x30).readS32(), ai.add(0x60).readS32(),
                        // [D3 2026-09-26] inputs the AI port depends on, read at onLeave:
                        // DAT_007f0ff4 clock, DAT_007f1008 step (0x0040fc63), DAT_0089a360
                        // difficulty row float, DAT_0089a368 flag, DAT_00897ffc
                        // (FUN_00443080), DAT_0063ba8c sub-state, rec+0x9e4 / +0xb0c
                        // (FUN_0046d6d0 / FUN_0046d6a0), ctrl[7] fire byte.
                        ga(0x007f0ff4).readS32(), ga(0x007f1008).readS32(),
                        ga(0x0089a360).readFloat(), ga(0x0089a368).readS32(),
                        ga(0x00897ffc).readS32(), ga(0x0063ba8c).readS32(),
                        ga(0x008815a0).add(this.v * 0xd04 + 0x9e4).readFloat(),
                        ga(0x008815a0).add(this.v * 0xd04 + 0xb0c).readFloat(),
                        b.add(7).readU8()]);
          // [D3 2026-09-27] the stack locals from the 0x00416596 probe plus the two
          // steer-history globals, appended so the CSV stays a superset of the old
          // columns. look_x,look_z,curv,mode,own_x,own_z,hist_d8,hist_dc.
          const r = AS.rows[AS.rows.length - 1];
          if (AS_PEND && (AS_PEND[3] === -1 || AS_PEND[3] === this.v)) {
            r.push(AS_PEND[0] === null ? '' : AS_PEND[0],
                   AS_PEND[1] === null ? '' : AS_PEND[1],
                   AS_PEND[2] === null ? '' : AS_PEND[2]);
          } else {
            if (AS_PEND) AS.joinMiss++; else AS.noLocals++;
            r.push('', '', '');
          }
          const rp = AS_RECP[this.v];
          if (rp) r.push(rp.add(0x30).readFloat(), rp.add(0x38).readFloat()); // 0x0041628d/0x00416297
          else    r.push('', '');
          const h = ga(0x008032d8).add(this.v * 0x14);
          r.push(h.readFloat(), h.add(4).readFloat());   // 0x004165cc/0x004165f7, 0x004166db/0x0041670c
          r.push(AS_MARCH[0], AS_MARCH[1]);              // 0x00444a2c call count, last 0x0041623b arg
          // [D3 2026-10-03] the two targeting returns of THIS call, and the ctrl
          // block as it was ON ENTRY (ctrl[5] is never zeroed by the tail).
          r.push(AS_TGT[0], AS_TGT[1], AS_TGT[2], AS_TGT[3], AS_TGT[4],
                 this.c4in === undefined ? -1 : this.c4in,
                 this.c5in === undefined ? -1 : this.c5in);
          AS_PEND = null;
        } catch(e){ if (!AS.err) AS.err = '' + e; }
      }
    });
    AS.armed = true;
    return 'aistep armed (FUN_00416250 @0x00416250)';
  } catch(e){ return 'ERR ' + e; }
}
function aiStepDrain(){ const r = AS.rows; AS.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- LATERAL BRACKET (U-9156, 2026-09-30) ----------------------------------
// [D2 section 21.3] Measures WHERE the original's lateral velocity goes inside one
// frame, so the port's 40-100-speed lateral collapse (section 21.2: retention 0.8897
// against the original's 0.9989 at matched speed/slide/yaw) can be attributed to a
// stage instead of inferred from a clamp's arithmetic.
//
// The frame order, from VehiclePhysicsRun.cpp:1003-1008 (decode of FUN_00470c70):
//   step 3  FUN_00470670 (A4) -> A5 FUN_0046ddb0, A6a FUN_00467650, A6b FUN_00468980
//   step 5  FUN_004709a0 substep loop, called TWICE with 25.0
// So sampling at A6a ENTRY and at FUN_004709a0 ENTRY splits each frame into
//   [A6a + A6b]  and  [the 2 x 25 contact substeps].
// Grip-clamp #6 lives at A6a's tail (Integrate2.cpp:651-737), so the first interval
// is the one that carries it.
//
// ENTRY HOOKS ONLY, per memory `frida-interceptor-is-entry-only` and CLAUDE.md's
// hot-path rule: no onLeave, no mid-function probe, no write. Rate is 1/frame for
// A6a and 2/frame for the substep loop = ~180/s at 60 fps, well under the ~1000/s
// where Interceptor destabilises Mashed.
//
// The record is read from its STATIC base (the statediff base_va, car 0 =
// 0x008815a0, stride 0xd04), not from a register, so neither site needs a
// context-pointer contract -- U-9149 records that A6b's is contested. The A6a site
// still filters on ESI == the record ("0x00467650 - A6a body. self=ESI record",
// PhysicsChainHooks.cpp:1877) so a multi-car run cannot mix vehicles in.
// [D2 section 21.5, 2026-09-30] A6b's entry was ADDED after section 21.4 over-claimed on
// the two-site version. With only A6a and the substep loop, the first interval is
// [A6a + A6b] -- and A6b (FUN_00468980) also writes the angular velocity, so the `|av|`
// ratio across it is NOT a clean fingerprint of which arm A6a's clamp #6 took. The third
// site makes [A6a entry -> A6b entry] contain A6a and nothing else.
const LB_A6A     = 0x00467650;
const LB_A6B     = 0x00468980;
const LB_SUBSTEP = 0x004709a0;
const LB = { armed:false, rows:[], nA6a:0, nA6b:0, nSub:0, err:null, skipped:0 };
let LB_REC = null, LB_SEQ = 0;
function lbSample(site){
  try {
    const r = LB_REC;
    LB.rows.push([LB_SEQ++, site,
                  r.add(0x9b0).readFloat(), r.add(0x9b4).readFloat(), r.add(0x9b8).readFloat(),
                  r.add(0x9d4).readFloat(), r.add(0x9d8).readFloat(), r.add(0x9dc).readFloat(),
                  r.add(0x9e4).readFloat(), r.add(0x9e0).readFloat(),
                  r.add(0x9bc).readFloat(), r.add(0x9c0).readFloat(), r.add(0x9c4).readFloat()]);
  } catch(e){ if (!LB.err) LB.err = 'sample' + site + ' ' + e; }
}
function latBracketArm(recBaseHex, car){
  if (LB.armed) return 'already armed';
  try {
    LB_REC = ptr(parseInt(recBaseHex, 16) + car * 0xd04);
    Interceptor.attach(ga(LB_A6A), { onEnter(){
      try {
        // self=ESI; ignore any other vehicle's call so the series stays car-pure.
        if (!this.context.esi.equals(LB_REC)) { LB.skipped++; return; }
        LB.nA6a++; lbSample(0);
      } catch(e){ if (!LB.err) LB.err = 'a6aEnter ' + e; }
    }});
    // A6b: no ESI filter. U-9149 records that A6b's context-pointer contract is
    // contested, and this probe does not depend on it -- it reads the static record. With
    // one active car there is one A6b call per frame, which the `0,2,1,1` pattern check
    // in a8_latbracket.py verifies per frame rather than assuming.
    Interceptor.attach(ga(LB_A6B), { onEnter(){
      try { LB.nA6b++; lbSample(2); }
      catch(e){ if (!LB.err) LB.err = 'a6bEnter ' + e; }
    }});
    Interceptor.attach(ga(LB_SUBSTEP), { onEnter(){
      try { LB.nSub++; lbSample(1); }
      catch(e){ if (!LB.err) LB.err = 'subEnter ' + e; }
    }});
    LB.armed = true;
    return 'lat-bracket armed: A6a @0x' + LB_A6A.toString(16)
         + ' + A6b @0x' + LB_A6B.toString(16)
         + ' + substep @0x' + LB_SUBSTEP.toString(16)
         + ' rec=' + LB_REC;
  } catch(e){ return 'ERR ' + e; }
}
function latBracketDrain(){ const r = LB.rows; LB.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- AXIS probe (U-9175, D2 attempt 16) ------------------------------------
// The drive-force accumulator's Y component +0xb18 is exactly 0.0 on 9336/9336
// original frames (RESULT_STEP4.md §3b) and non-zero on the port. +0xb18 is
// written ONLY by A6a FUN_00467650's drive arms (0x00467cc5/ccb active-drive,
// 0x00467d97/d9d boost) as `wheelAxisY * drive + b18`, where the wheel forward
// axis Y lives at wheel-block +0xb8 (record +0x224/+0x2e8/+0x3ac/+0x470 for the
// four wheels). That axis is set by A5 FUN_0046ddb0 at 0x0046de74
// (`mov [esi+0xb8], ecx` from `[edi+0x9d8]`, the body world-forward Y) in the
// steer==0 branch, or via the rotation transform 0x4c3df0 at 0x0046de5b in the
// steer!=0 branch. The body forward +0x9d4/d8/dc is itself `xform*(0,0,1)`,
// written at 0x0046ddc9. A4 FUN_00470670 zeroes b14/18/1c every frame at
// 0x0047072c (PhysicsChainHooks.cpp:292) BEFORE A5/A6a run.
//
// The question U-9175 poses: is the original's +0xb18 zero because its producer
// writes 0 (a STRUCTURAL zero -- forward-Y is 0 on this flat arm) or because a
// LATER step zeroes a non-zero A6a output? Two ENTRY hooks bracket A6a exactly,
// the sanctioned technique (memory `frida-interceptor-is-entry-only`):
//   site 0 = A6a entry 0x00467650 (ESI-filtered)  -> PRE  (A4 just zeroed b14/18/1c)
//   site 1 = A6b entry 0x00468980                  -> POST (what A6a itself left)
// A6a -> A6b is the immediate dispatch (A4_Body: Call_A5; Call_A6a; Call_A6b), and
// A6b does not write b14/18/1c, so POST == A6a's own output. If forward-Y and the
// four axis-Ys are exactly 0.0 at PRE and b18 is exactly 0.0 at POST, the zero is
// STRUCTURAL. If b18 is non-zero at POST but 0 at the .msd snapshot, a later step
// zeroes it.
//
// Rate: 1/frame each site = ~120/s at 60 fps, well under the ~1000/s destabilise
// floor. Reads only; no writes; no onLeave; no mid-function probe.
const AX_A6A = 0x00467650;
const AX_A6B = 0x00468980;
// wheel-block forward-axis Y, record byte offsets (wheelN base = 0x16c + N*0xc4,
// axis Y = +0xb8): w0 0x224, w1 0x2e8, w2 0x3ac, w3 0x470.
const AX_WHEEL_AXY = [0x224, 0x2e8, 0x3ac, 0x470];
const AX_FWD_CONST = 0x00614708;   // DAT_00614708 = (0,0,1), A5's forward input
const AX = { armed:false, rows:[], nA6a:0, nA6b:0, skipped:0, err:null,
             chk:null, nFwdYnz:0, nAxYnz:0, nB18nzPre:0, nB18nzPost:0, nDrive:0,
             // [U-9193 2026-10-05] angular velocity at a KNOWN phase, plus KA-B, the
             // A6b car-attribution check. PREREG verify/d3_omega_20261005/PREREG_OMEGA.md.
             nWxNz:0, nWyNz:0, nWzNz:0, nDriveWyNz:0, nA6bEsiRec:0, recPtrs:null };
let AX_REC = null, AX_SEQ = 0;
function axSample(site, esiPtr){
  try {
    const r = AX_REC;
    const row = [AX_SEQ++, site,
                 r.add(0x9d4).readFloat(), r.add(0x9d8).readFloat(), r.add(0x9dc).readFloat(),
                 r.add(0xb14).readFloat(), r.add(0xb18).readFloat(), r.add(0xb1c).readFloat(),
                 r.add(0x9e4).readFloat(), r.add(0x9e0).readFloat(),
                 r.add(0xbf8).readS32(), r.add(0x1f0).readS32()];
    for (let w = 0; w < 4; w++) row.push(r.add(AX_WHEEL_AXY[w]).readFloat());
    // [U-9193] APPENDED so every existing consumer of .axisprobe.csv keeps its
    // columns. +0x9bc/+0x9c0/+0x9c4 is the angular velocity U-9193 is about, read
    // here at a KNOWN program point instead of at a per-render-frame .msd snapshot
    // phase, which leg 3's KA-1 showed is not a usable rate. +0x958/+0x960 is the
    // record's position. `esi` is recorded RAW at both sites because whether ESI
    // holds the record pointer at A6b is NOT established and is not assumed -- the
    // analysis checks it (KA-B) instead of this hook pretending to know.
    const esis = esiPtr ? esiPtr.toString() : '';
    row.push(r.add(0x9bc).readFloat(), r.add(0x9c0).readFloat(), r.add(0x9c4).readFloat(),
             r.add(0x958).readFloat(), r.add(0x960).readFloat(), esis);
    // coverage counters, computed from the row (not from an assumption that a
    // site fired): a probe that reports 0 of N must also prove it covered N.
    if (site === 0) {
      const wx = row[16], wy = row[17], wz = row[18];
      if (wx !== 0.0) AX.nWxNz++;
      if (wy !== 0.0) AX.nWyNz++;
      if (wz !== 0.0) AX.nWzNz++;
      // G-OMEGA's denominator is the ACTIVE-DRIVE regime, not every call.
      if (row[10] === 0 && row[8] > 1.0 && wy !== 0.0) AX.nDriveWyNz++;
    } else if (AX.recPtrs && AX.recPtrs.indexOf(esis) >= 0) {
      AX.nA6bEsiRec++;   // KA-B: does ESI hold a record pointer at A6b at all?
    }
    if (site === 0) {
      AX.nA6a++;
      const fwdY = row[3], b18pre = row[6], bf8 = row[10], sp = row[8];
      if (fwdY !== 0.0) AX.nFwdYnz++;
      if (b18pre !== 0.0) AX.nB18nzPre++;
      // "active drive" = post-launch driving (bf8==0) with real speed, the regime
      // where the accumulation actually adds a non-zero drive term.
      if (bf8 === 0 && sp > 1.0) AX.nDrive++;
      for (let w = 0; w < 4; w++) if (row[12 + w] !== 0.0) { AX.nAxYnz++; break; }
    } else {
      AX.nA6b++;
      if (row[6] !== 0.0) AX.nB18nzPost++;
    }
    AX.rows.push(row);
  } catch(e){ if (!AX.err) AX.err = 'sample' + site + ' ' + e; }
}
function axisProbeArm(recBaseHex, car){
  if (AX.armed) return 'already armed';
  try {
    AX_REC = ptr(parseInt(recBaseHex, 16) + car * 0xd04);
    // KA-B's reference set: the four record pointers. Built here so the A6b check
    // compares against addresses derived the same way AX_REC is, not a literal.
    AX.recPtrs = [0, 1, 2, 3].map(function(n){
      return ptr(parseInt(recBaseHex, 16) + n * 0xd04).toString(); });
    // KNOWN-ANSWER self-check: the static forward-axis constant A5 feeds to
    // Rw_TransformPoints must read (0,0,1), proving the probe reads the image at
    // the right VA before any verdict rests on its float reads.
    AX.chk = { fwdConst: [ga(AX_FWD_CONST).readFloat(),
                          ga(AX_FWD_CONST).add(4).readFloat(),
                          ga(AX_FWD_CONST).add(8).readFloat()],
               rec: AX_REC.toString() };
    Interceptor.attach(ga(AX_A6A), { onEnter(){
      try {
        if (!this.context.esi.equals(AX_REC)) { AX.skipped++; return; }
        axSample(0, this.context.esi);
      } catch(e){ if (!AX.err) AX.err = 'a6aEnter ' + e; }
    }});
    Interceptor.attach(ga(AX_A6B), { onEnter(){
      // DELIBERATELY still unfiltered, and that is now VISIBLE rather than silent.
      // A6b's ESI is not established to hold the record pointer, so filtering on it
      // would be a guess; instead the raw ESI is recorded and KA-B decides whether
      // POST rows are attributable at all. With >1 car live, an unfiltered A6b
      // samples the TARGET car's record on every car's call -- see PREREG section 1.
      try { axSample(1, this.context.esi); }
      catch(e){ if (!AX.err) AX.err = 'a6bEnter ' + e; }
    }});
    AX.armed = true;
    return 'axis-probe armed: A6a @0x' + AX_A6A.toString(16)
         + ' + A6b @0x' + AX_A6B.toString(16)
         + ' rec=' + AX_REC + ' fwdConst=' + JSON.stringify(AX.chk.fwdConst);
  } catch(e){ return 'ERR ' + e; }
}
function axisProbeDrain(){ const r = AX.rows; AX.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- SLIDE probe (+0xb0c, D2 attempt 17) -----------------------------------
// +0xb0c is written ONLY by A4 FUN_00470670 per frame, from SEVEN record fields
// and nothing else (transcribed byte-exact, verify/d2_b0c_20261002/
// RESULT_STEP1.md section 2.1):
//
//   speed == 0.0 ->  +0xb0c = 0                                     0x0047072c
//   otherwise    ->  +0xb0c = (1.0 - |dot| / speed) * speed         0x00470724
//     dot = (fwd.y*vel.y + fwd.x*vel.x) + fwd.z*vel.z   0x004706db..0x00470701
//     1.0 = _DAT_005cc320 = 0x3f800000, read from the anchored exe
//     vel = +0x9b0/+0x9b4/+0x9b8     fwd = +0x9d4/+0x9d8/+0x9dc     speed = +0x9e4
//
// WHY A LIVE HOOK AND NOT THE .msd. The capture is a RENDER-TICK snapshot and vel
// is integrated later in the same frame, so the snapshot's inputs are not the
// inputs A4 used. That is not an assumption: gate KA of PREREG_STEP2.md recomputed
// +0xb0c from the snapshot on three original captures and got 0.9488 / 0.9401 /
// 0.9453 at the best lag (-1) against a 0.99 threshold -- a FAIL, which the
// pre-registration answers by requiring this hook rather than by moving the bar.
//
// The record base arrives in EAX (0x00470679 `mov edi, eax`; `in_EAX` in the
// decompilation), so the ESI filter the other probes use does not apply here.
//
// Rate: A4 is one call per car per frame = ~60/s on the solo arm, far under the
// ~1000/s destabilise floor. Entry only -- no onLeave, no mid-function probe, no
// write (memory `frida-interceptor-is-entry-only`).
//
// Each row carries its own frame marker and its own release marker, so pairing
// never depends on an external clock (memory `next-sample-pairing-needs-a-frame-
// marker`, `check-the-capture-carries-the-event-itself`):
//   [seq, SD.frames, tick 0x007f101c, velx,vely,velz, fwdx,fwdy,fwdz, speed,
//    b0c_stale, bf8, bf4]
// `b0c_stale` is the value standing in the record AT ENTRY, i.e. the one A4 wrote
// on the PREVIOUS call. The known-answer check is therefore a CROSS-CALL one:
// predict from row i's seven inputs and compare to row i+1's b0c_stale. Nothing is
// trusted until that reproduces the original's own stored value.
const SL_A4   = 0x00470670;
const SL_TICK = 0x007f101c;
const SL = { armed:false, rows:[], calls:0, mine:0, skipped:0, err:null, chk:null,
             cap:20000, capped:false, nZeroSpeed:0, nB0cNz:0 };
let SL_REC = null, SL_SEQ = 0;
function slideProbeArm(recBaseHex, car, cap){
  if (SL.armed) return 'already armed';
  try {
    SL_REC = ptr(parseInt(recBaseHex, 16) + car * 0xd04);
    SL.cap = cap || 20000;
    // KNOWN-ANSWER self-check #1, static: the literal the writer folds in at
    // 0x00470718 (`fsubr dword ptr [0x5cc320]`) must read 1.0 in the live image.
    // If it does not, the probe is reading the wrong image and no float it
    // reports means anything.
    SL.chk = { one: ga(0x005cc320).readFloat(),
               zero: ga(0x005d757c).readFloat(),
               k1500: ga(0x005cd0ac).readFloat(),
               k500: ga(0x005ccd04).readFloat(),
               rec: SL_REC.toString() };
    Interceptor.attach(ga(SL_A4), { onEnter(){
      try {
        SL.calls++;
        if (!this.context.eax.equals(SL_REC)) { SL.skipped++; return; }
        SL.mine++;
        if (SL.rows.length >= SL.cap) { SL.capped = true; return; }
        const r = SL_REC;
        let tick = 0; try { tick = ga(SL_TICK).readS32(); } catch(_){}
        const sp = r.add(0x9e4).readFloat();
        const b0c = r.add(0xb0c).readFloat();
        if (sp === 0.0) SL.nZeroSpeed++;
        if (b0c !== 0.0) SL.nB0cNz++;
        SL.rows.push([SL_SEQ++, SD.frames, tick,
                      r.add(0x9b0).readFloat(), r.add(0x9b4).readFloat(),
                      r.add(0x9b8).readFloat(),
                      r.add(0x9d4).readFloat(), r.add(0x9d8).readFloat(),
                      r.add(0x9dc).readFloat(),
                      sp, b0c,
                      r.add(0xbf8).readS32(), r.add(0xbf4).readS32()]);
      } catch(e){ if (!SL.err) SL.err = 'a4Enter ' + e; }
    }});
    SL.armed = true;
    return 'slide-probe armed: A4 @0x' + SL_A4.toString(16)
         + ' rec=' + SL_REC + ' cap=' + SL.cap
         + ' consts=' + JSON.stringify(SL.chk);
  } catch(e){ return 'ERR ' + e; }
}
function slideProbeDrain(){ const r = SL.rows; SL.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- CONTACT-FIXUP probe (U-9156, D2 section 22.2) -------------------------
// [D2 section 22.1] The registered rule named C4 (the post-bounce horizontal speed) as the
// first diverging term at d = 0, and reading 4 points at the last-contact damp
// `min(0.9, 3*(1 - min(1, |m|/speed)))` at 0x0046f5ba/0x0046f5c0 inside
// VehicleContactFixup 0x0046ef70.
//
// The .msd cannot answer it: the 18 contact slots at +0x4a8 + i*0x40 have key (+0x04) == -1
// on 2332 of orig_solo3.msd's 2333 frames and +0x9ec is 0 on every frame, because the
// render-tick snapshot lands after the substep loop has cleared them. This probe reads the
// slots at the ONE point in the frame where they are live: the ENTRY of 0x0046ef70.
//
// ENTRY HOOKS ONLY (memory `frida-interceptor-is-entry-only`). Two sites with one shared
// sequence, exactly the lat-bracket shape:
//   site 0 = 0x0046ef70 entry -> the PRE-fixup velocity + the live slot set
//   site 1 = 0x004709a0 entry -> the substep entry, so the sample that FOLLOWS a site-0 row
//            is the POST-fixup velocity (between the fixup and the next substep entry
//            nothing writes +0x9b0: the original's substep order is 0x0046e9e0 ->
//            0x0046f6c0 -> 0x00469aa0 -> 0x0046ef70, FUN_004709a0's own body).
//   site 2 = A6a entry 0x00467650 -> the FRAME marker. A6a runs once per frame before the
//            substep loop and DOES write velocity, so a site-2 row between a site-0 and the
//            next site-1 means a frame boundary intervened and that pair's post-velocity is
//            NOT the fixup's output. The reducer must check this rather than assume it.
//            Counting site-2 rows also gives every row a frame index, so a probe row can be
//            matched to a .msd frame.
// Not hot: the port's equivalent log fires 211 times in a 27 s race.
//
// SELF-CHECK, not an assumption: the first FP_SELFCHECK site-0 rows also carry ESI/ECX/EDI
// so the reduction can confirm which register holds `self` and that it equals the static
// player record 0x008815a0 + car*0xd04, rather than assuming a calling convention. The
// sample itself always reads the STATIC record, so the probe does not depend on the answer.
const FP_FIXUP   = 0x0046ef70;
const FP_SUBSTEP = 0x004709a0;
const FP_A6A     = 0x00467650;
// [D2 section 22.2] the other two substep members, so the +0x9e4 == |velocity| invariant can
// be located at a point in the substep rather than guessed. FUN_004709a0's body order is
// 0x0046e9e0 -> 0x0046f6c0 -> 0x00469aa0 -> 0x0046ef70.
const FP_WHEEL   = 0x0046f6c0;
const FP_WORLDC  = 0x00469aa0;
const FP_SELFCHECK = 8;
const FP = { armed:false, rows:[], nFix:0, nSub:0, nFrm:0, nWh:0, nWc:0, err:null, chk:[] };
let FP_REC = null, FP_SEQ = 0;
function fpSlots(r){
  // Slot base S = 0x4a8 + i*0x40: +0x00 depth, +0x04 key (-1 = empty), +0x08..+0x10 normal,
  // +0x14 scale, +0x2c..+0x34 arm, +0x38 magnitude (ContactFixup.cpp:85-92).
  const idx = []; const s = [];
  for (let i = 0; i < 0x12; i++){
    const S = 0x4a8 + i * 0x40;
    if (r.add(S + 4).readS32() === -1) continue;
    idx.push(i);
    if (s.length < 20) s.push([i, r.add(S).readFloat(),
                               r.add(S + 8).readFloat(), r.add(S + 0xc).readFloat(),
                               r.add(S + 0x10).readFloat(), r.add(S + 0x14).readFloat(),
                               r.add(S + 0x2c).readFloat(), r.add(S + 0x30).readFloat(),
                               r.add(S + 0x34).readFloat(), r.add(S + 0x38).readFloat()]);
  }
  return [idx, s];
}
function fpSample(site){
  try {
    const r = FP_REC;
    const base = [FP_SEQ++, site, FP.nFrm,
                  r.add(0x9b0).readFloat(), r.add(0x9b4).readFloat(), r.add(0x9b8).readFloat(),
                  r.add(0x9e4).readFloat(), r.add(0x9e0).readFloat(), r.add(0x9ec).readS32(),
                  r.add(0x144).readFloat(), r.add(0x148).readFloat(), r.add(0x14c).readFloat(),
                  r.add(0x9d4).readFloat(), r.add(0x9dc).readFloat()];
    const sl = fpSlots(r);
    base.push(sl[0].length);
    base.push(sl[0].join('|'));
    // two fixed slot columns (the port's Training bounce reports exactly two), then the
    // full set as a packed string so nothing is silently dropped.
    for (let k = 0; k < 2; k++){
      const q = sl[1][k];
      for (let j = 0; j < 10; j++) base.push(q ? q[j] : '');
    }
    base.push(sl[1].map(function(q){ return q.join(':'); }).join('|'));
    FP.rows.push(base);
  } catch(e){ if (!FP.err) FP.err = 'sample' + site + ' ' + e; }
}
function contactFixupProbeArm(recBaseHex, car){
  if (FP.armed) return 'already armed';
  try {
    FP_REC = ptr(parseInt(recBaseHex, 16) + car * 0xd04);
    Interceptor.attach(ga(FP_FIXUP), { onEnter(){
      try {
        if (FP.chk.length < FP_SELFCHECK){
          let a4 = '?';
          try { a4 = this.context.esp.add(4).readPointer().toString(); } catch(e){ }
          FP.chk.push({seq:FP_SEQ, esi:this.context.esi.toString(),
                       ecx:this.context.ecx.toString(), edi:this.context.edi.toString(),
                       esp4:a4, rec:FP_REC.toString()});
        }
        FP.nFix++; fpSample(0);
      } catch(e){ if (!FP.err) FP.err = 'fixEnter ' + e; }
    }});
    Interceptor.attach(ga(FP_SUBSTEP), { onEnter(){
      try { FP.nSub++; fpSample(1); }
      catch(e){ if (!FP.err) FP.err = 'subEnter ' + e; }
    }});
    Interceptor.attach(ga(FP_A6A), { onEnter(){
      try {
        // ESI filter, as latBracketArm does: keep the frame count car-pure.
        if (!this.context.esi.equals(FP_REC)) return;
        FP.nFrm++; fpSample(2);
      } catch(e){ if (!FP.err) FP.err = 'a6aEnter ' + e; }
    }});
    Interceptor.attach(ga(FP_WHEEL),  { onEnter(){
      try { FP.nWh++;  fpSample(3); } catch(e){ if (!FP.err) FP.err = 'whEnter ' + e; }
    }});
    Interceptor.attach(ga(FP_WORLDC), { onEnter(){
      try { FP.nWc++;  fpSample(4); } catch(e){ if (!FP.err) FP.err = 'wcEnter ' + e; }
    }});
    FP.armed = true;
    return 'fixup-probe armed: fixup @0x' + FP_FIXUP.toString(16)
         + ' + substep @0x' + FP_SUBSTEP.toString(16)
         + ' + A6a @0x' + FP_A6A.toString(16) + ' rec=' + FP_REC;
  } catch(e){ return 'ERR ' + e; }
}
function contactFixupProbeDrain(){ const r = FP.rows; FP.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- WHEEL-STATE probe (U-9179, D2 attempt 20) -----------------------------
// [D2 attempt 20] U-9179 asks which branch of 0x0046f6c0's per-wheel 3-state machine
// leaves two wheels at state 0 on the PORT, and what the ORIGINAL's per-wheel inputs are at
// the same `d`. The state machine's complete input set is three per-wheel fields
// (verify/d2_wheelstate_20261002/RESULT_STEP1.md section 1):
//     state  +0x198 + w*0xc4     (written only by 0x0046f6c0 itself)
//     fv     +0x194 + w*0xc4     (init loop -> 10.0f, then classifier 0x0046cc40 -> depth)
//     key    +0x1ec + w*0xc4     (init loop -> -1,    then classifier 0x0046cc40)
// `--fixup-probe` already hooks this RVA (site 3, FP_WHEEL) but its sample carries
// +0x9b0/+0x9e0/+0x9e4 and the 18 contact slots and NOT ONE per-wheel field, so it cannot
// answer this (memory `grep-the-harness-for-the-rva-before-writing-a-probe`: checked first).
//
// ENTRY HOOKS ONLY (memory `frida-interceptor-is-entry-only`). At the entry of call N the
// init loop has not run yet, so the row carries the state AFTER call N-1's whole body and
// the fv/key call N-1's classifier left -- i.e. a CONSECUTIVE PAIR of rows is the complete
// input/output record of call N-1. PREREG_STEP2.md section 1.3.
//
// Two sites:
//   site 0 = 0x0046f6c0 entry -> the per-wheel sample
//   site 1 = 0x00467650 A6a entry, ESI-filtered to the player record -> the FRAME marker,
//            so every row carries a frame index and can be matched to a .msd frame.
// EDI is NOT used as a filter (that would turn a wrong register assumption into zero rows);
// instead every row carries `edi_is_rec` and the first rows carry the raw registers, so the
// reduction confirms the convention rather than depending on it.
//
// COUNT-FIRST safety gate: with countOnly the site-0 body is `nWh++` and nothing else, so
// the rate is measured before any run reads memory. Expected ~2/frame (the original runs 2
// substeps per frame), ~70/s, two orders under the ~1000/s destabilise floor.
const WS_WHEEL = 0x0046f6c0;
const WS_A6A   = 0x00467650;
const WS = { armed:false, countOnly:false, rows:[], nWh:0, nFrm:0, nOther:0, err:null, chk:[] };
let WS_REC = null, WS_SEQ = 0;
function wsSample(isRec){
  try {
    const r = WS_REC;
    const row = [WS_SEQ++, 0, WS.nFrm, isRec ? 1 : 0,
                 r.add(0x9e4).readFloat(), r.add(0x9e0).readFloat(),
                 r.add(0xbf8).readS32(),
                 r.add(0x9b0).readFloat(), r.add(0x9b4).readFloat(), r.add(0x9b8).readFloat()];
    for (let w = 0; w < 4; w++){
      const B = w * 0xc4;
      row.push(r.add(0x198 + B).readS32());       // state
      row.push(r.add(0x194 + B).readFloat());     // fv
      row.push(r.add(0x1ec + B).readS32());       // key
    }
    WS.rows.push(row);
  } catch(e){ if (!WS.err) WS.err = 'wsSample ' + e; }
}
function wheelStateProbeArm(recBaseHex, car, countOnly){
  if (WS.armed) return 'already armed';
  try {
    WS_REC = ptr(parseInt(recBaseHex, 16) + car * 0xd04);
    WS.countOnly = !!countOnly;
    Interceptor.attach(ga(WS_WHEEL), { onEnter(){
      try {
        WS.nWh++;
        const isRec = this.context.edi.equals(WS_REC);
        if (!isRec) WS.nOther++;
        if (WS.chk.length < 8)
          WS.chk.push({seq:WS_SEQ, edi:this.context.edi.toString(),
                       esi:this.context.esi.toString(), ecx:this.context.ecx.toString(),
                       rec:WS_REC.toString()});
        if (!WS.countOnly) wsSample(isRec);
      } catch(e){ if (!WS.err) WS.err = 'wsEnter ' + e; }
    }});
    Interceptor.attach(ga(WS_A6A), { onEnter(){
      try { if (!this.context.esi.equals(WS_REC)) return; WS.nFrm++; }
      catch(e){ if (!WS.err) WS.err = 'wsA6a ' + e; }
    }});
    WS.armed = true;
    return 'wheelstate-probe armed: solver @0x' + WS_WHEEL.toString(16)
         + ' + A6a @0x' + WS_A6A.toString(16) + ' rec=' + WS_REC
         + (WS.countOnly ? ' COUNT-ONLY' : '');
  } catch(e){ return 'ERR ' + e; }
}
function wheelStateProbeDrain(){ const r = WS.rows; WS.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- RwV3dLength ARGUMENT probe (U-9156, D2 section 21.7) ------------------
// [D2 section 21.7] Gets the ORIGINAL's `l_60 = sum ld4 * le4` (Integrate2.cpp:464) as a
// MEASUREMENT instead of an inference. Section 21.5 closed every other route: clamp #6 is
// byte-faithful, +0x18c is 1.0 on both sides so grip == l_60, and the `ld4` path is circular
// from the port side.
//
// Why this function. A6a `0x00467650`'s body (4908 bytes, 1236 instructions, FULL capstone
// coverage) contains **zero `fsqrt`** and 22 CALLs, nine of them to `0x004c3ac0`. That
// function is RwV3dLength: `mov eax,[esp+4]` then it squares `[eax]`/`[eax+4]`/`[eax+8]` and
// dispatches the root through the RW globals table `[0x007d3ff8]`/`[0x007d3ffc]` (memory
// `rwglobals-is-dat-007d3ff8`). It takes a POINTER, so an ENTRY hook yields the exact vector
// whose magnitude is being taken -- `le4` (Integrate2.cpp:425) and `ld4` (:440) among them.
// That is the sanctioned technique: hook a callee's entry, never probe mid-function
// (memory `frida-interceptor-is-entry-only`).
//
// Call sites are told apart by `this.returnAddress`, so one hook serves all nine. The nine
// A6a return addresses are 0x00467673, 0x00467685, 0x004680fb, 0x0046820f, 0x00468343,
// 0x004684c0, 0x004684dc, 0x004685bc, 0x004686a9 (the last is Integrate2.cpp:633's `speed`,
// which sits AFTER the force integration at :630-632 and BEFORE the clamp at :656 -- so it
// also splits A6a's own two halves, which section 21.5's `I_a6a` could not).
//
// HOT-PATH DISCIPLINE. `0x004c3ac0` has **120 confirmed call sites image-wide**
// (`re/tools/callsites.py`), so it is exactly the class CLAUDE.md warns about (>1000 calls/s
// destabilises Mashed in ~6 s). Mitigations, all three:
//   1. COUNT-FIRST. With an empty site list the callback body is `MP.n++` and nothing else,
//      so the rate can be measured before any run reads memory.
//   2. ONE object lookup on the hot path (`MP_SITES[rv]`), then an immediate return.
//   3. HARD ROW LIMIT with AUTO-DETACH, so exposure is bounded even if the rate is high.
const MP_FN = 0x004c3ac0;
const MP = { armed:false, rows:[], n:0, other:0, err:null, limit:0, detached:false };
let MP_L = null, MP_SITES = null, MP_REC = null, MP_SEQ = 0;
function magProbeArm(sitesCsv, limit, recBaseHex, car){
  if (MP.armed) return 'already armed';
  try {
    MP.limit = limit | 0;
    MP_REC = ptr(parseInt(recBaseHex, 16) + car * 0xd04);
    if (sitesCsv) {
      MP_SITES = {};
      for (const s of sitesCsv.split(',')) {
        if (!s) continue;
        MP_SITES[ga(parseInt(s, 16)).toUInt32()] = s;
      }
    }
    MP_L = Interceptor.attach(ga(MP_FN), { onEnter(){
      MP.n++;
      if (MP_SITES === null) return;              // count-only: cheapest possible body
      const tag = MP_SITES[this.returnAddress.toUInt32()];
      if (tag === undefined) { MP.other++; return; }
      try {
        const v = this.context.esp.add(4).readPointer();
        const r = MP_REC;
        MP.rows.push([MP_SEQ++, tag,
                      v.readFloat(), v.add(4).readFloat(), v.add(8).readFloat(),
                      r.add(0x9b0).readFloat(), r.add(0x9b8).readFloat(),
                      r.add(0x9e4).readFloat(), r.add(0x9e0).readFloat(),
                      r.add(0x18c).readFloat()]);
        if (MP.limit && MP.rows.length >= MP.limit && MP_L && !MP.detached) {
          MP_L.detach(); MP_L = null; MP.detached = true;   // bound the exposure
        }
      } catch(e){ if (!MP.err) MP.err = 'magEnter ' + e; }
    }});
    MP.armed = true;
    return 'mag-probe armed @0x' + MP_FN.toString(16)
         + (MP_SITES === null ? ' COUNT-ONLY' : ' sites=' + Object.keys(MP_SITES).length)
         + ' limit=' + MP.limit + ' rec=' + MP_REC;
  } catch(e){ return 'ERR ' + e; }
}
function magProbeDrain(){ const r = MP.rows; MP.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- LAUNCH REV-CHARGE probe (D2 attempt 15, U-9174, 2026-10-01) -----------
// Confirms on the RUNNING original which instruction sets the vehicle record's
// `+0xbf8` to 2 at the green light. U-9174 recorded that every LITERAL-displacement
// writer of `+0xbf8` writes ZERO; the real writer folds the base, so it encodes
// `0x00882198` (= 0x008815a0 + 0xbf8) and no `+0xbf8` scan can see it:
//
//   0x0046d780  FUN_0046d780(int car)            -- the RELEASE
//       0x0046d794  mov ecx,[eax+0x882194]       charge  = veh[car].+0xbf4
//       0x0046d79a  cmp ecx,0x3e8                > 1000 ?
//       0x0046d7a2  mov [eax+0x882198],2         <== the write under test
//       0x0046d7cf  mov [eax+0x882198],1         the other arm
//       0x0046d7d9  mov [eax+0x882194],ecx       charge += 1000 on that arm
//   0x0046d7f0  FUN_0046d7f0(int car, int delta) -- the per-tick CHARGE
//       gate `160.0 < accel_byte` (_DAT_005cea3c), clamp [0,3000] (0xbb8)
//   both are called only from 0x004103a0, the pre-race countdown tick, which
//   releases when DAT_0063d588 >= 1.86 and then sets DAT_0063ba8c = 6.
//
// ENTRY HOOKS ONLY -- two `Interceptor.attach` at the two function entries, with
// onEnter/onLeave on the same attach. No mid-function probe, no watchpoint, no
// page guard (memory `frida-interceptor-is-entry-only`).
// RATE: both fire only inside the pre-race state, once per active car per tick,
// so far below the ~1000 calls/s that destabilises Mashed. A hard row limit with
// auto-detach bounds the exposure anyway, matching magProbeArm's discipline.
const BP_REL = 0x0046d780;        // FUN_0046d780 release
const BP_CHG = 0x0046d7f0;        // FUN_0046d7f0 charge
const BP_TICK = 0x007f101c;       // frame counter, ++ at the top of FUN_004111c0
const BP_STATE = 0x0063ba8c;      // race state (FUN_0040e350 returns this)
const BP_CTRL = 0x007f1a14;       // per-car control index, stride 4 dwords
// accel byte = 0x007f1038 + 4, control-block stride 0x4c BYTES. The decompiler
// renders FUN_0046d7f0's read as `(&DAT_007f103c)[ctrl * 0x13]`, which is a DWORD
// index (0x13 * 4 = 0x4c); `Ai/AiState.h:38` and `Ai/AiController.cpp:170` use the
// same 0x4c byte stride. CORRECTED 2026-10-01 after the STEP 1 runs, which both used
// ctrl index 0 (`--poke-ctrl-slots` -> [0,1,2,3], car 0 -> ctrl 0) where 0x13 and
// 0x4c give the same address 0 -- so neither run's accel column is affected, and the
// STEP 1 verdict never read this column.
const BP_ACCEL = 0x007f103c;
const BP_CTRL_STRIDE = 0x4c;
const BP = { armed:false, rows:[], nRel:0, nChg:0, err:null, limit:0, detached:false };
let BP_L1 = null, BP_L2 = null, BP_BASE = 0, BP_SEQ = 0;
function bpRead(car){
  // [charge, state, accel] for one car, all reads, no writes.
  const r = ptr(BP_BASE + car * 0xd04);
  let acc = -1;
  try {
    const c = ga(BP_CTRL).add(car * 0x10).readS32();
    acc = ga(BP_ACCEL).add(c * BP_CTRL_STRIDE).readU8();
  } catch(e){ acc = -1; }
  return [r.add(0xbf4).readS32(), r.add(0xbf8).readS32(),
          ga(BP_STATE).readS32(), acc];
}
function boostProbeArm(recBaseHex, limit){
  if (BP.armed) return 'already armed';
  try {
    BP_BASE = parseInt(recBaseHex, 16);
    BP.limit = limit | 0;
    const mk = (fn, tag) => Interceptor.attach(ga(fn), {
      onEnter(args){
        try {
          this.bpCar = this.context.esp.add(4).readS32();
          if (tag === 'rel') BP.nRel++; else BP.nChg++;
          const t = ga(BP_TICK).readS32();
          const v = bpRead(this.bpCar);
          this.bpEnter = [BP_SEQ++, tag, 'enter', t, this.bpCar,
                          v[0], v[1], v[2], v[3]];
        } catch(e){ if (!BP.err) BP.err = 'bpEnter ' + e; this.bpEnter = null; }
      },
      onLeave(){
        if (!this.bpEnter) return;
        try {
          const t = ga(BP_TICK).readS32();
          const v = bpRead(this.bpCar);
          // one row per call: enter-side then leave-side, so a write is visible
          // as a change across the pair without ever probing mid-function.
          BP.rows.push(this.bpEnter.concat([v[0], v[1], v[2], v[3]]));
          if (BP.limit && BP.rows.length >= BP.limit && !BP.detached) {
            if (BP_L1) { BP_L1.detach(); BP_L1 = null; }
            if (BP_L2) { BP_L2.detach(); BP_L2 = null; }
            BP.detached = true;
          }
        } catch(e){ if (!BP.err) BP.err = 'bpLeave ' + e; }
      }
    });
    BP_L1 = mk(BP_REL, 'rel');
    BP_L2 = mk(BP_CHG, 'chg');
    BP.armed = true;
    return 'boost-probe armed @0x' + BP_REL.toString(16) + ' +0x' + BP_CHG.toString(16)
         + ' rec=0x' + BP_BASE.toString(16) + ' limit=' + BP.limit;
  } catch(e){ return 'ERR ' + e; }
}
function boostProbeDrain(){ const r = BP.rows; BP.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- ALIVE / ELIMINATION probe (D3 STEP 1, 2026-10-03) ---------------------
// The port eliminates the PLAYER at rt = 1.8667 s on the (b)/(e) recipe
// (verify/d3_noboost_20261003/RESULT.md, D2 WATCH row). Nothing in the repo
// records what the ORIGINAL does in the same scenario, and the --statediff-aistep
// stream structurally cannot carry it (it hooks FUN_00416250, which the original
// never calls for slot 0). So take it live, in the SAME run as the aistep capture
// so the two streams join on `frame`.
//
// Three ENTRY hooks only (memory frida-interceptor-is-entry-only); ~360 calls/s
// at 4 cars, well under the 1000/s ceiling in CLAUDE.md:
//   0x00410d10  FUN_00410d10  SegmentCheck   -- the race-rule update, 1/frame
//   0x00418560  FUN_00418560  AiVehicleStep  -- 1 per STEPPED slot (<= 4/frame)
//   0x00418860  FUN_00418860  AiTickLoop     -- 1 per AI tick
// alive[i] is read where FUN_0046c7b0 reads it: *(i32*)(0x008815a4 + i*0xd04).
// hooks_registry 'vehicle_slot_getter' records `DAT_008815a4[idx*0x341]` and
// 0x341*4 == 0xd04; re/frida/camera_probe.py:128-137 reads the same address.
// KA-1 validates that read against the function itself every sample.
const AL_REC   = 0x008815a0;   // vehicle record base; +0x04 is the slot-alive word
const AL_SEG   = 0x00410d10;
const AL_VSTEP = 0x00418560;
const AL_TICK  = 0x00418860;
const AL_ZOOM  = 0x00898980;   // FUN_00442df0's backing float; 0x00410d10 cmp vs 10.0
const AL_PCT   = 0x008a96ec;   // + i*0x30c, the progress % EliminationCheck reads
const AL = { armed:false, rows:[], ev:[], err:null, vstep:[0,0,0,0], tick:0,
             seq:0, last:null, ka1n:0, ka1ok:0, ka1bad:null };
let AL_FN = null;
function alAlive(i){ return ptr(AL_REC + i*0xd04 + 4).readS32(); }
function aliveProbeArm(){
  if (AL.armed) return 'already armed';
  try {
    AL_FN = new NativeFunction(ga(0x0046c7b0), 'int', ['int'], 'mscdecl');
    Interceptor.attach(ga(AL_VSTEP), { onEnter(){
      try { const v = this.context.esp.add(4).readS32();
            if (v >= 0 && v < 4) AL.vstep[v]++; }
      catch(e){ if (!AL.err) AL.err = 'vstep ' + e; } } });
    Interceptor.attach(ga(AL_TICK), { onEnter(){ AL.tick++; } });
    Interceptor.attach(ga(AL_SEG), {
      onEnter(){
        try {
          this.alIn = [alAlive(0), alAlive(1), alAlive(2), alAlive(3)];
          // KA-1, every sample for the first 200 calls: the direct read must equal
          // what FUN_0046c7b0 itself returns. This is the probe's liveness control
          // (memory absent-log-proves-nothing-run-a-control): an all-ones vector
          // means "nobody died" ONLY if this passes.
          if (AL.ka1n < 200) {
            AL.ka1n++;
            let ok = true;
            for (let i = 0; i < 4; i++) if (AL_FN(i) !== this.alIn[i]) ok = false;
            if (ok) AL.ka1ok++;
            else if (AL.ka1bad === null)
              AL.ka1bad = [AL.seq, this.alIn[0], this.alIn[1], this.alIn[2], this.alIn[3],
                           AL_FN(0), AL_FN(1), AL_FN(2), AL_FN(3)];
          }
        } catch(e){ if (!AL.err) AL.err = 'segEnter ' + e; this.alIn = null; }
      },
      onLeave(ret){
        if (!this.alIn) return;
        try {
          const o = [alAlive(0), alAlive(1), alAlive(2), alAlive(3)];
          const clk = ga(0x007f0ff4).readS32();
          const row = [AL.seq++, SD.frames, AL.tick, clk,
                       ga(0x0063ba8c).readS32(), ga(0x007f0fd0).readS32(),
                       ga(0x008a94d0).readS32(), ga(AL_ZOOM).readFloat(),
                       ret.toInt32(),
                       this.alIn[0], this.alIn[1], this.alIn[2], this.alIn[3],
                       o[0], o[1], o[2], o[3],
                       AL.vstep[0], AL.vstep[1], AL.vstep[2], AL.vstep[3]];
          for (let i = 0; i < 4; i++) row.push(ga(AL_PCT).add(i*0x30c).readFloat());
          AL.rows.push(row);
          // event list: every change of the alive vector, entry-side or across the call
          const key = o.join('/');
          if (AL.last !== key) {
            if (AL.ev.length < 200)
              AL.ev.push([AL.seq - 1, SD.frames, AL.tick, clk, AL.last, key,
                          this.alIn.join('/'), ret.toInt32(),
                          AL.vstep[0], AL.vstep[1], AL.vstep[2], AL.vstep[3]]);
            AL.last = key;
          }
        } catch(e){ if (!AL.err) AL.err = 'segLeave ' + e; }
      }
    });
    AL.armed = true;
    return 'alive-probe armed (0x00410d10 + 0x00418560 + 0x00418860, rec 0x'
         + AL_REC.toString(16) + ')';
  } catch(e){ return 'ERR ' + e; }
}
function aliveProbeDrain(){ const r = AL.rows; AL.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- LEADER-TIMER input probe (D3, 2026-10-03) -----------------------------
// verify/d3_leader_20261003/PREREG_WITNESS.md, leg W-DATA, ORIGINAL side.
// Question: when FUN_004148b0 actually runs on the ORIGINAL, what are the values
// of the globals `Ai/AiLeaderTimer.cpp`'s LeaderTimer reads? The port side of the
// same question is measured without Frida by re/tools/sa_leaderwatch.py.
//
// ONE ENTRY HOOK (memory frida-interceptor-is-entry-only). FUN_004148b0 is called
// once per mode-0 AI call -- ~180/s at 3 AI cars, far under the 1000/s ceiling,
// and the elimination session already ran this exact hook safely as o_t3's fifth.
// Nothing is written and no return value is touched, so the probe cannot perturb
// the arm it is measuring.
//
// Addresses, each cited to the line of Ai/AiLeaderTimer.cpp that reads it:
const LP_MODE368 = 0x0089a368;  // :91   mode gate (== 2 -> return 0)
const LP_FLT360  = 0x0089a360;  // :92   float, __ftol'd -> iVar1
const LP_IDX364  = 0x0089a364;  // :93   idx364
const LP_BIAS374 = 0x0089a374;  // :98   table bias
const LP_LIMITTB = 0x005f2dd8;  // :98   int limit table          [INITIALISED .data]
const LP_RANK    = 0x0089a4c4;  // :99   RankAt  = base + v*0x74
const LP_TIMER   = 0x0089a4c8;  // :108  TimerAt = base + v*0x74
const LP_PROG    = 0x008989b0;  // :101/:106 via FUN_00442cc0, stride 4
const LP_FRAMEDT = 0x007f1008;  // :108  frame delta
const LP_THR_A8  = 0x005cd0a8;  // :107                           [INITIALISED .rdata]
const LP_THR_A4  = 0x005cd0a4;  // :116                           [INITIALISED .rdata]
const LP_THR_A0  = 0x005cd0a0;  // :118                           [INITIALISED .rdata]
const LP_THR_35C = 0x005cc35c;  // :119  (4.0)                    [INITIALISED .rdata]
const LP_FN      = 0x004148b0;  // FUN_004148b0 itself
const LP = { armed:false, rows:[], err:null, n:0, limit:20000, ka:null };
function leaderProbeArm(limit){
  if (LP.armed) return 'already armed';
  try {
    LP.limit = limit || 20000;
    // KA: the four thresholds are initialised .rdata, so their values are fixed in
    // the image and knowable before the run. AiLeaderTimer.cpp:61 states
    // _DAT_005cc35c == 4.0. Record all four once at arm time; the RESULT checks
    // them against the file image read independently with the PE section table.
    LP.ka = [ptr(LP_THR_A8).readFloat(), ptr(LP_THR_A4).readFloat(),
             ptr(LP_THR_A0).readFloat(), ptr(LP_THR_35C).readFloat()];
    Interceptor.attach(ga(LP_FN), { onEnter(args){
      try {
        if (LP.rows.length >= LP.limit) return;
        LP.n++;
        const v = args[3].toInt32();                 // param_4 = vehicle index
        const row = [LP.n, ga(0x007f0ff4).readS32(), v,
                     ga(LP_MODE368).readS32(), ga(LP_FLT360).readFloat(),
                     ga(LP_IDX364).readS32(), ga(LP_BIAS374).readS32(),
                     ga(LP_FRAMEDT).readS32(),
                     ga(LP_RANK).add(v*0x74).readS32(),
                     ga(LP_TIMER).add(v*0x74).readS32()];
        for (let i = 0; i < 4; i++) row.push(ga(LP_PROG).add(i*4).readFloat());
        // the limit table entry this call will actually index, plus a nonzero count
        let nz = 0; for (let i = 0; i < 64; i++) if (ga(LP_LIMITTB).add(i*4).readS32() !== 0) nz++;
        row.push(nz);
        LP.rows.push(row);
      } catch(e){ if (!LP.err) LP.err = 'lpEnter ' + e; }
    }});
    LP.armed = true;
    return 'leader-probe armed (entry-only 0x' + LP_FN.toString(16)
         + ', thresholds a8/a4/a0/35c = ' + LP.ka.join('/') + ')';
  } catch(e){ return 'ERR ' + e; }
}
function leaderProbeDrain(){ const r = LP.rows; LP.rows = []; return r; }
// ---------------------------------------------------------------------------

// --- POWERUP DISPATCHER capture (D3 WS-D, 2026-09-26) ----------------------
// Ground truth for the power-up DECISION logic, taken around the per-frame
// dispatcher FUN_0045bba0 (sole caller 0x0040fcd0). Byte-level facts this block
// relies on (capstone, MASHED.exe.unpatched, 2026-09-26):
//   0x0045bba7  CALL FUN_0040e350 (= MOV EAX,[0x0063ba8c]; RET)  -> race sub-state
//   0x0045bc2b  CMP EAX,6 / JNE 0x0045bdc1  -> per-slot state/fire logic only when ==6
//   0x0045bc6b  state = DAT_0068d1f0[slot]  (4 -> skip, 2 -> set 3 + deactivate, 3 -> skip)
//   0x0045bcab  armed = [slot+0xac]; ctrl = *(int*)(0x007f1a14 + slot*0x10)
//   0x0045bd58  CANFIRE (*(entry+0x0c))(slot) cdecl; nonzero -> FUN_0045bac0 @0x0045bd62
//   0x0045bd72  cur  = byte [0x007f103f + ctrl*0x4c]   (ctrl block 0x007f1038 +7)
//   0x0045bd84  prev = byte [0x007f14ff + ctrl*0x4c]   (shadow 0x007f14f8 +7; the cook
//               FUN_00496530 copies block->shadow with REP MOVSD at 0x00496552 and then
//               zeroes the block at 0x00496568, so prev = LAST FRAME's cur)
//   mode: cur&&!prev -> 2, cur&&prev -> 3, !cur&&prev -> 1; FIRE (*(entry+8))(slot,mode)
//               cdecl at 0x0045bdbb
//   0x0045be31  neither -> byte [0x007f1040+ctrl*0x4c] set and [0x007f1500+ctrl*0x4c]
//               clear -> FUN_0045bac0 (deactivate) at 0x0045be4d
//   0x0045bd24  not armed -> [0x007f1055+ctrl*0x4c] && ![0x007f1515+..] ->
//               FUN_0045c010(slot, 0x10) at 0x0045bd47
//   FUN_0045c010(idx, code) cdecl: slot = 0x0088fbe0 + idx*0xb4, JMP FUN_0045bfa0, which
//               loads ESI = [esp+0xc] = the SECOND stack arg (the type code) at 0x0045bfa1.
// CONTRIVED STATE (C3-grade, same class as pokeCtrlSlots): the scripted mode below
// activates each type through the ORIGINAL's own activate path FUN_0045c010 and
// writes the ctrl byte the dispatcher reads; everything downstream is the original.
const PU_DISP = 0x0045bba0, PU_ACT = 0x0045c010, PU_DEACT = 0x0045bac0;
const PU_BOX = 0x0045ba00;            // FUN_0045ba00, DAT_0068d1f0[idx] = value
const PU_SLOT0 = 0x0088fbe0, PU_SSTRIDE = 0xb4, PU_STATE = 0x0063ba8c;
const PU_FIRE = {0x004561c0:9, 0x00454740:10, 0x00455150:11, 0x00457ef0:12, 0x0045a850:16,
                 0x0045b6e0:17, 0x00454db0:18, 0x00457800:19, 0x004533b0:7};
const PU_CANF = [0x004566d0, 0x00457ab0, 0x00455360, 0x0045a890, 0x0045b260, 0x00454a90,
                 0x00456dd0, 0x00453610];
// per-type pool record strides (powerup_effects_decomp.md §2) -- bytes dumped from the
// ARM handle ([slot+0xac], a pool-record pointer: MISSILE ARM returns EBX=0x006885d0+
// idx*0x2c at 0x004550f9, OIL ARM returns ESI=0x0068a250+idx*0x10 at 0x00456dc1)
const PU_STRIDE = {9:0x48, 10:0x2c, 11:0x2c, 12:0x18, 16:0x68, 17:0x24, 18:0x14, 19:0x10, 7:0x1c};
const PU = { armed:false, n:0, calls:0, rows:[], err:null, cap:200000, cur:null,
             plan:[], subj:0, warm:120, pi:0, st:'warm', t:0, acts:[], lastArmed:0,
             pat:[], held:0, boxAt:-1, boxDone:false };
function puCode(e){ try { return e.isNull() ? -1 : e.readS32(); } catch(_){ return -2; } }
function puRec(h, code){
  const n = PU_STRIDE[code]; if (!n || h.isNull()) return '';
  try { const u = new Uint8Array(h.readByteArray(n)); let s = '';
        for (let i = 0; i < u.length; ++i) s += (u[i] < 16 ? '0' : '') + u[i].toString(16);
        return s; } catch(_){ return 'ERR'; }
}
function puSnap(){
  const o = [];
  for (let s = 0; s < 4; ++s) {
    const sl = ga(PU_SLOT0 + s * PU_SSTRIDE);
    const e = sl.add(0xa8).readPointer(), h = sl.add(0xac).readPointer();
    const code = puCode(e);
    o.push({e:e, h:h, code:code, rec:puRec(h, code)});
  }
  return o;
}
// fire pattern for the scripted subject: [on,off] frame pairs, exercised in order.
// Covers the press edge (mode 2), holds of 1/5/29 frames (mode 3) and release (mode 1).
const PU_PATTERN = [[1,6],[2,6],[6,6],[30,8],[1,6],[1,6],[2,10],[60,10]];
function puArm(planCsv, subj, warm, boxAt){
  if (PU.armed) return 'already armed';
  try {
    PU.plan = planCsv ? planCsv.split(',').map(x => parseInt(x, 10)) : [];
    PU.subj = subj|0; PU.warm = warm|0; PU.boxAt = (boxAt === undefined) ? -1 : (boxAt|0);
    const act = new NativeFunction(ga(PU_ACT), 'void', ['int', 'int'], 'mscdecl');
    globalThis._puAct = act;
    // FUN_0045ba00 @0x0045ba00: the bare `DAT_0068d1f0[idx] = value` setter
    // (MOV [ECX*4+0x68d1f0],EAX, cdecl idx/value). --pu-box calls it with 2, the
    // value FUN_00422fd0 @0x00422fd0 and FUN_0040be50 @0x0040be50 write. CONTRIVED
    // state, same class as --pu-plan's FUN_0045c010 call: the VALUE is forced, and
    // everything the dispatcher then does with it is the original's own code.
    globalThis._puBox = new NativeFunction(ga(PU_BOX), 'void', ['int', 'int'], 'mscdecl');
    const ph = ga(PHASE), stg = ga(PU_STATE), dtp = ga(0x007f100c);
    for (const k in PU_FIRE) {
      const code = PU_FIRE[k];
      Interceptor.attach(ga(parseInt(k)), { onEnter(){
        if (!PU.cur) return;
        const sp = this.context.esp;
        PU.cur.ev.push(['F', sp.add(4).readPointer().toUInt32(), sp.add(8).readS32(), code]);
      }});
    }
    for (const a of PU_CANF) {
      Interceptor.attach(ga(a), {
        onEnter(){ this.sl = PU.cur ? this.context.esp.add(4).readPointer().toUInt32() : 0; },
        onLeave(r){ if (PU.cur && this.sl) PU.cur.ev.push(['C', this.sl, r.toInt32(), a]); }
      });
    }
    Interceptor.attach(ga(PU_DEACT), { onEnter(){
      if (!PU.cur) return;
      PU.cur.ev.push(['D', this.context.esi.toUInt32(), this.returnAddress.toUInt32(), 0]);
    }});
    Interceptor.attach(ga(PU_DISP), {
      onEnter(){
        this.skip = true;
        try {
          if (ph.readU8() !== 3) return;
          const state = stg.readS32();
          PU.calls++;
          this.skip = false; this.state = state;
          const subj = PU.subj;
          const ctrl = ga(0x007f1a14 + subj * 0x10).readS32();
          let act = '';
          if (state === 6 && PU.plan.length) {
            PU.n++;
            const sl = ga(PU_SLOT0 + subj * PU_SSTRIDE);
            const held = !sl.add(0xa8).readPointer().isNull();
            let cur = 0, disc = 0;
            if (PU.st === 'warm') {
              if (PU.n >= PU.warm) PU.st = 'arm';
            }
            if (PU.st === 'arm') {
              if (PU.pi >= PU.plan.length) { PU.st = 'done'; }
              else if (held) { disc = (PU.t++ % 2 === 0) ? 1 : 0; }   // discard a natural pickup
              else if (PU.pi === PU.boxAt && !PU.boxDone) {
                // one dispatcher call BEFORE the activation, force the box state
                // to 2. The dispatcher's own 0x0045bc7b arm then latches it to 3
                // (0x0045bc85) with nothing held, and the NEXT call activates
                // under state 3 -- the shape s2 reached by accident.
                PU.boxDone = true;
                try { globalThis._puBox(subj, 2); } catch(e){ PU.err = 'box ' + e; }
              }
              else {
                const code = PU.plan[PU.pi];
                act = 'A' + code;
                try { globalThis._puAct(subj, code); } catch(e){ PU.err = 'act ' + e; }
                PU.acts.push([PU.n, code]);
                PU.st = 'fire'; PU.t = 0; PU.pat = []; PU.idle = 0;
                for (const p of PU_PATTERN) { for (let i=0;i<p[0];++i) PU.pat.push(1);
                                              for (let i=0;i<p[1];++i) PU.pat.push(0); }
              }
            } else if (PU.st === 'fire') {
              if (!held) {                       // deactivated by the original itself
                if (++PU.idle >= 30) { PU.pi++; PU.st = 'arm'; PU.t = 0; }
              } else if (PU.t < PU.pat.length) {
                cur = PU.pat[PU.t++];
              } else {                           // pattern exhausted, still armed
                disc = (PU.t++ % 2 === 0) ? 1 : 0;   // native discard edge (0x0045be31)
              }
            }
            if (ctrl >= 0 && ctrl < 16) {
              ga(0x007f103f + ctrl * 0x4c).writeU8(cur ? 0xff : 0);
              if (disc) ga(0x007f1040 + ctrl * 0x4c).writeU8(0xff);
            }
          }
          const c = [];
          for (let s = 0; s < 4; ++s) {
            const k = ga(0x007f1a14 + s * 0x10).readS32();
            const ok = k >= 0 && k < 16;
            c.push([k, ok ? ga(0x007f103f + k*0x4c).readU8() : -1,
                       ok ? ga(0x007f14ff + k*0x4c).readU8() : -1,
                       ok ? ga(0x007f1040 + k*0x4c).readU8() : -1,
                       ok ? ga(0x007f1500 + k*0x4c).readU8() : -1,
                       ga(0x0068d1f0 + s*4).readS32()]);
          }
          this.c = c; this.act = act;
          this.dt = '0x' + dtp.readU32().toString(16);   // exact float bits of DAT_007f100c
          this.pre = puSnap();
          PU.cur = { ev: [] };
        } catch(e){ if (!PU.err) PU.err = 'enter ' + e; this.skip = true; }
      },
      onLeave(){
        if (this.skip) { PU.cur = null; return; }
        try {
          const post = puSnap(), ev = PU.cur ? PU.cur.ev : [];
          PU.cur = null;
          for (let s = 0; s < 4; ++s) {
            const base = (PU_SLOT0 + s * PU_SSTRIDE) >>> 0;
            const mine = ev.filter(x => x[1] === base);
            const a = this.pre[s], b = post[s];
            if (a.code === -1 && b.code === -1 && !mine.length) continue;
            if (PU.rows.length >= PU.cap) return;
            const f = mine.filter(x => x[0] === 'F'), cf = mine.filter(x => x[0] === 'C'),
                  d = mine.filter(x => x[0] === 'D');
            const cc = this.c[s];
            PU.rows.push([SD.frames, PU.calls, this.state, s, cc[0], cc[1], cc[2], cc[3], cc[4],
                          cc[5], this.dt, (s === PU.subj ? this.act : ''),
                          a.code, a.h.toUInt32(), b.code, b.h.toUInt32(),
                          f.map(x => x[2]).join('|'), cf.map(x => x[2]).join('|'),
                          d.map(x => '0x' + (x[2] >>> 0).toString(16)).join('|'),
                          a.rec, b.rec]);
          }
        } catch(e){ if (!PU.err) PU.err = 'leave ' + e; }
      }
    });
    PU.armed = true;
    return 'puhook armed (FUN_0045bba0 + 9 FIRE + 8 CANFIRE + FUN_0045bac0; plan=['
           + PU.plan.join(',') + '] subj slot ' + PU.subj + ' warm ' + PU.warm
           + (PU.boxAt >= 0 ? ' box2-before-act ' + PU.boxAt : '') + ')';
  } catch(e){ return 'ERR ' + e; }
}
function puDrain(){ const r = PU.rows; PU.rows = []; return r; }

// --- D3-CONTACT 2026-09-27: the power-up CONTACT / IMPACT chain -------------
// Opt-in (--puhook-contacts), additive: it does not touch puArm's hooks or the
// .puhook.csv schema, so an existing capture recipe is unaffected.
//
// The dispatcher's armed sweep, disassembled at 0x0045bcc8..0x0045bd11 from
// original/MASHED.exe.unpatched (the pinned anchor):
//   0x0045bcc8  PUSH ECX / PUSH EBP
//   0x0045bccd  CALL 0x45bfe0        ; 0x0045bfe0 is TWO instructions:
//                                    ;   MOV EAX,[0x0080332c] ; RET
//                                    ; a global getter, NOT a sweep. Its result is
//                                    ; pushed at 0x0045bcd2 as the 3rd arg below.
//   0x0045bcd3  CALL 0x4b4b60        ; 3 args, __cdecl (ADD ESP,0xc at 0x0045bcd8).
//                                    ; 0x004b4b60 copies 4 dwords from arg1, pushes
//                                    ; 1 / arg2 / &copy / arg3, writes 3 at [esp+0x28]
//                                    ; and tails CALL 0x4b4a80 (0x004b4b7e..0x004b4b9b)
//   0x0045bcdb  TEST EAX,EAX / JE 0x45bd14     ; 0 -> no contact, skip
//   0x0045bce5  CALL 0x45c350        ; 2 args (&[esp+0x38], EBP)
//   0x0045bced  TEST EAX,EAX / JNE 0x45bd14    ; nonzero -> keep the power-up
//   0x0045bcf7  CALL 0x45bac0        ; deactivate (ESI = EDI-0x90)
//   0x0045bd0c  CALL 0x476880        ; (0x6146fc, 0x43b40000=360.0f, 0x3fc00000=1.5f, EBP)
// So "deactivate on failure" = sweep hit (0x4b4b60 != 0) AND 0x45c350 == 0.
//
// NOTE on naming: hooks.csv labels 0x0045bfe0 `Bezier::GetLocate`, 0x0045c350
// `Bezier::Interpolate` and 0x004b4cd0 `Bezier::QueryWrapper`. 0x0045bfe0's body is
// the 2-instruction getter above, which supports neither that name nor the
// "armed contact sweep" gloss in re/analysis/D3_POWERUPS_2026-09-26.md §6. The
// names are recorded here, not endorsed; this capture reports mechanics only.
const PU_CONTACT = {
  0x004b4b60: 'sweep_query',     // dispatcher armed sweep, 0x0045bcd3
  0x0045c350: 'sweep_confirm',   // 0x0045bce5; == 0 -> deactivate
  0x004b4d10: 'query_4b4d10',    // D3_POWERUPS §6 missile contact chain
  0x004b4cd0: 'query_4b4cd0',    // §6 OIL ground placement
  0x004b4650: 'query_4b4650',    // §6 OIL ground placement
  0x004b5080: 'query_4b5080',    // §6 OIL ground placement
  0x00455910: 'missile_impact_a',
  0x00455100: 'missile_impact_b',
  // D3_CONTACT_PORT 2026-09-28: the SECOND drop gate. Disassembled from the
  // pinned anchor: OIL FUN_00457800 `CALL 0x0045c110` @0x00457909 / `TEST EAX,EAX`
  // @0x00457911 / `JNE 0x00457a18` (bare epilogue) @0x00457913; P_MINE
  // FUN_00457c10 `CALL 0x0045c110` @0x00457cf4 / `TEST EAX,EAX` @0x00457cfc /
  // `JNE 0x00457e08` (bare epilogue) @0x00457cfe. Its argument is the hit
  // triangle's RpMaterial; FUN_0045c110 reads *(uint*)(mat+4) (the RwRGBA) and
  // returns 1 for 0xff010101 / 0xffff0080 (0x0045c116..0x0045c129). Added to
  // close D3_CONTACT_2026-09-27 §8 item 1 -- which of the two gates refused 6 of
  // P_MINE's 7 press edges.
  0x0045c110: 'surface_gate',
  // D3_CONTACT_PORT 2026-09-28: SHOTGUN's world query. FUN_0045b390 (the pellet
  // detonation, reached from FIRE 0x0045b6e0) runs a 2-iteration loop; each pass
  // does `CALL 0x004b4b20` @0x0045b4bd / `TEST EAX,EAX` @0x0045b4c5 /
  // `JE 0x0045b5cb` @0x0045b4c7, and on a hit `CALL 0x004b5080` @0x0045b57d --
  // the 0x45b582 site the prior note counted 8/8 for SHOTGUN. Without this row
  // the gate that produces those 8 is invisible.
  0x004b4b20: 'query_4b4b20',
};
// Per-RVA early cap. surface_gate sits behind a 0x004b4cd0 hit, whose own
// measured rate is ~60/s, but it also has callers outside the power-up path
// (FUN_0045c350), so it gets a low cap rather than the shared 20000: if it turns
// out to be a hot path the listener detaches long before the ~6 s destabilisation
// window CLAUDE.md records for >1000 calls/s Interceptor attachments.
const PU_CX_CAP = { surface_gate: 4000 };
// cap: once an RVA exceeds this many calls the listener DETACHES itself. Frida
// Interceptor on a >1000 calls/s path destabilises MASHED in ~6 s (CLAUDE.md,
// log/auto_count_at_menu.txt), and none of these RVAs has a measured rate yet, so
// the capture is designed to survive discovering that one of them is hot: the
// count and the `hot` flag still come back, only the per-call rows stop.
// MEASURED 2026-09-27 over a 110 s Training race (verify/d3_contact_20260927/c2.msd,
// 6629 frames, all 8 attached, no crash): query_4b4cd0 6001 (capped; ~1 per frame,
// ~60/s), sweep_query 1035, query_4b4650 98, query_4b4d10 44, missile_impact_b 31,
// query_4b5080 21, sweep_confirm 18, missile_impact_a 1. None is a >1000/s path, so
// the cap is set well above one long race rather than at the discovery value.
const PU_CX_HOT = 20000;
const PU_CX = { armed:false, rows:[], counts:{}, hot:{}, lis:{}, cap:40000, err:null };
function puCxArm(listCsv){
  if (PU_CX.armed) return 'already armed';
  try {
    let want = null;
    if (listCsv && listCsv !== 'all') {
      want = {}; for (const t of listCsv.split(',')) want[parseInt(t, 16) >>> 0] = 1;
    }
    const names = [];
    for (const k in PU_CONTACT) {
      const rva = parseInt(k) >>> 0;
      if (want && !want[rva]) continue;
      const nm = PU_CONTACT[k];
      PU_CX.counts[nm] = 0; PU_CX.hot[nm] = 0;
      names.push(nm);
      PU_CX.lis[nm] = Interceptor.attach(ga(rva), {
        onEnter(){
          PU_CX.counts[nm]++;
          if (PU_CX.counts[nm] > (PU_CX_CAP[nm] || PU_CX_HOT)) {
            if (!PU_CX.hot[nm]) { PU_CX.hot[nm] = 1;
              try { PU_CX.lis[nm].detach(); } catch(_){} }
            this.skip = true; return;
          }
          this.skip = false;
          const sp = this.context.esp;
          this.a = [];
          for (let i = 1; i <= 3; ++i) {
            try { this.a.push(sp.add(i * 4).readU32() >>> 0); } catch(_){ this.a.push(0); }
          }
          this.fr = SD.frames; this.cl = PU.calls;
          this.ra = this.returnAddress.toUInt32() >>> 0;
        },
        onLeave(r){
          if (this.skip) return;
          if (PU_CX.rows.length >= PU_CX.cap) return;
          // D3-CONTACT 2026-09-28c: also record the hit's SEGMENT PARAMETER.
          // The 0x40-byte result buffer the query fills is arg3, and its `t` is
          // at +0x38 (the collector FUN_004b4bb0 writes puVar2[0xe]). Without it
          // a replay can only inject the COUNT, and anything the original derives
          // from `t` is unreproducible -- which bit the MISSILE ground bias:
          // pu_replay used to hardcode t = 0.5 with a comment saying nothing
          // downstream was measured, and that stopped being true.
          let ht = '';
          if (r.toInt32() !== 0 && this.a[2]) {
            try { ht = '0x' + (ptr(this.a[2]).add(0x38).readU32() >>> 0).toString(16); }
            catch(_){ ht = ''; }
          }
          PU_CX.rows.push([this.fr, this.cl, '0x' + rva.toString(16), nm,
                           '0x' + this.ra.toString(16),
                           '0x' + this.a[0].toString(16), '0x' + this.a[1].toString(16),
                           '0x' + this.a[2].toString(16), r.toInt32(), ht]);
        }
      });
    }
    PU_CX.armed = true;
    return 'puhook-contacts armed (' + names.join(',') + ')';
  } catch(e){ PU_CX.err = '' + e; return 'ERR ' + e; }
}
function puCxDrain(){ const r = PU_CX.rows; PU_CX.rows = []; return r; }

// --- D3-CONTACT 2026-09-28b: the TARGET-ACQUISITION inputs -----------------
// Opt-in (--puhook-aim). Additive: its own channel, its own CSV, no change to
// the .puhook.csv or .pucontact.csv schemas.
//
// WHY this channel exists. MORTAR, GUN and MISSILE all route their contact
// calls through FUN_00459620, and its two query sites sit on OPPOSITE sides of
// one branch: `if (local_104 == 0)`, the count of acquisition candidates.
// Measured in verify/d3_contact_20260928 (pu_contact_report.py):
//   g3 GUN     0x459c19 163/163 calls  -> candidate count was 0 on every call
//   g4 GUN     0x459c19 163/163 calls  -> likewise
//   g2 MISSILE 0x459c19   0/32  calls  -> a candidate on EVERY call
//   g2 MORTAR  0x459c19  96/153 calls  -> split
// So the branch is exercised both ways and no stub can be right. Reproducing it
// needs the four car positions, their active flags and the firing car's aim
// matrix -- none of which the capture recorded, and none of which the replay's
// Backend::car[4] is ever given (it is default-constructed all-zero).
//
// Addresses, all disassembled from original/MASHED.exe.unpatched (the pinned
// anchor) or decompiled from a read-only pool clone:
//   FUN_00459620 prologue 0x00459620..0x004596b9:
//     `SUB ESP,0x110` @0x00459620 then `MOV EDX,[ESP+0x114]` @0x00459626 = arg1.
//     After `PUSH EBX/EBP/ESI/EDI` (0x10) the args sit at [ESP+0x124..0x134].
//     `IMUL EBP,EBP,0x58` @0x00459632 + `ADD EBP,0x68b9f8` @0x00459638, so the
//     aim record base is 0x0068b9f8 stride 0x58 -- NOT the 0x0068b9fc the
//     decompiler glosses (it names the +4 field).
//     `MOV [EBP+4],EAX` @0x0045966a  = arg4 (the cone limit)
//     `MOV [EBP+0x18],ECX` @0x0045966d = arg3 (the range)
//     [EBP+0x48..0x50] = arg2[0..2]  @0x00459685..0x00459693 (the origin)
//     [EBP+0x38] = the 16-dword aim matrix buffer, filled by
//     `CALL 0x41f220` @0x004596b0.
//   FUN_0041f030 (0x0041f030): car position = 4 dwords at 0x0063dc38 + car*0x2ac.
//   FUN_0040e370 (0x0040e370): `car > 3 -> false`, else
//     `*(int*)([0x005f2770] + car*4 + 0x34) != 0`.
//   FUN_004075a0 (0x004075a0): pure getter, `return [0x0063a5d0]` -- the second
//     candidate list's count, which the acquisition loop also walks.
//
// Floats are emitted as raw hex dwords. The dt lesson from the 2026-09-27
// captures applies: a 6-digit decimal is not the value the game computed with.
const PU_AIM = { armed:false, rows:[], calls:0, cap:20000, pending:null,
                 orphans:0, err:null };
function puAimF(p){            // one float as its exact bits, never a decimal
  try { return '0x' + (p.readU32() >>> 0).toString(16); } catch(_){ return ''; }
}
function puAimArm(){
  if (PU_AIM.armed) return 'already armed';
  try {
    // (1) the aim matrix. FUN_0041f220 fills arg2 with 16 dwords copied from the
    // car's own matrix; read it on LEAVE, because FUN_00459620's tail overwrites
    // the same buffer (`FUN_004b42c0` + the 0x10-dword copy at its epilogue), so
    // reading it at the aim call's EXIT would report the NEXT frame's basis.
    // Attached at a function ENTRY, not mid-function: CLAUDE.md records that a
    // mid-function Interceptor clobbers a local and kills the game in ~2 ticks.
    //
    // Call order is FUN_00459620.onEnter -> this onEnter/onLeave -> .onLeave, so
    // this is where the pending row gets its at-row and where it is pushed. A row
    // is emitted ONLY here: an acquisition call that somehow never reached
    // 0x004596b0 would have no basis to report, and dropping it is honest where
    // padding it with the previous call's basis would not be.
    PU_AIM.lisMat = Interceptor.attach(ga(0x0041f220), {
      onEnter(){
        // only the call FUN_00459620 makes; RA of `CALL 0x41f220` @0x004596b0.
        this.mine = (this.returnAddress.toUInt32() >>> 0) === 0x004596b5;
        if (this.mine) this.dst = this.context.esp.add(8).readU32();
      },
      onLeave(){
        if (!this.mine) return;
        const row = PU_AIM.pending; PU_AIM.pending = null;
        if (!row) { PU_AIM.orphans++; return; }
        try {
          const m = ptr(this.dst);
          for (let i = 8; i < 11; ++i) row.push(puAimF(m.add(i * 4)));  // +0x20..0x28
        } catch(_){ row.push('', '', ''); }
        if (PU_AIM.rows.length < PU_AIM.cap) PU_AIM.rows.push(row);
      }
    });
    // (2) the acquisition call itself.
    PU_AIM.lis = Interceptor.attach(ga(0x00459620), {
      onEnter(){
        PU_AIM.calls++;
        if (PU_AIM.rows.length >= PU_AIM.cap) return;
        const sp = this.context.esp;            // pre-prologue: arg1 at esp+4
        const row = [SD.frames, PU.calls];
        let slot = -1;
        try { slot = sp.add(4).readS32(); } catch(_){}
        row.push(slot);
        try {                                   // arg2 -> the origin vec3
          const o = ptr(sp.add(8).readU32());
          for (let i = 0; i < 3; ++i) row.push(puAimF(o.add(i * 4)));
        } catch(_){ row.push('', '', ''); }
        row.push(puAimF(sp.add(0x0c)));         // arg3 range
        row.push(puAimF(sp.add(0x10)));         // arg4 cone limit
        // the second candidate list's count, FUN_004075a0's global.
        try { row.push(ga(0x0063a5d0).readS32()); } catch(_){ row.push(-1); }
        // the four cars: active flag then position.
        let base = null;
        try { base = ptr(ga(0x005f2770).readU32()); } catch(_){}
        for (let c = 0; c < 4; ++c) {
          let act = -1;
          try { act = base ? base.add(c * 4 + 0x34).readS32() : -1; } catch(_){}
          row.push(act === 0 ? 0 : (act === -1 ? -1 : 1));
          try {
            const p = ga(0x0063dc38 + c * 0x2ac);
            for (let i = 0; i < 3; ++i) row.push(puAimF(p.add(i * 4)));
          } catch(_){ row.push('', '', ''); }
        }
        PU_AIM.pending = row;   // pushed by (1) above, once the at-row is known
      }
    });
    PU_AIM.armed = true;
    return 'puhook-aim armed (0x00459620 + 0x0041f220)';
  } catch(e){ PU_AIM.err = '' + e; return 'ERR ' + e; }
}
function puAimDrain(){ const r = PU_AIM.rows; PU_AIM.rows = []; return r; }

// --- D3-CONTACT 2026-09-28b: the MORTAR projectile pool ---------------------
// Opt-in (--puhook-mortar). Its own channel, its own CSV.
//
// MORTAR's own contact site (0x453789, inside FUN_00453730) fires once per
// AIRBORNE PROJECTILE per frame, so reproducing its count needs the projectile's
// per-frame integration, which lives in FUN_004538b0. Both were decompiled from
// a read-only pool clone; the pool geometry is disassembled from the tick's own
// second loop at 0x00453c30..0x00453c51:
//   `MOV ESI,0x684ea8` @0x00453c28, `ADD ESI,0x110` @0x00453c45,
//   `CMP ESI,0x6870a8` @0x00453c4b  ->  base 0x00684ea8, stride 0x110,
//   (0x6870a8-0x684ea8)/0x110 = 32 records.
// The tick's FIRST loop (0x00453b90..0x00453c22) is a separate per-CAR block at
// 0x00684e3c stride 0x1c, 4 entries, and it is what calls the acquisition
// routine: `PUSH 0x41a00000` @0x00453bca (20.0), `PUSH 0x41700000` @0x00453bcf
// (15.0), `CALL 0x459620` @0x00453bd9 -- i.e. MORTAR's cone and range are
// LITERALS at the call site, which is where --puhook-aim's recorded 20.0/15.0
// come from.
//
// Record fields, by dword index, all from FUN_004538b0's body:
//   [0]        owner car, passed to FUN_0046d4a0
//   [2..4]     an offset added to the target's position to make the aim point
//   [5..7]     position   -- FUN_00453730 reads rec+0x14 as segment point A
//   [8..10]    velocity
//   [0xb..0xd] previous position
//   [0xe..0x10] this frame's delta -- FUN_00453730 reads rec+0x38 and probes
//              A -> A + delta, so the detonation segment is the frame's travel
//   [0x11]     homing flag        [0x13] age seconds   [0x14] age/1.3
//   [0x15]     the arc's base Y
//
// PRE and POST state are both recorded, so the port's integration can be diffed
// directly rather than inferred from a call count.
const PU_MTR = { armed:false, rows:[], calls:0, cap:20000, pending:null,
                 orphans:0, err:null };
function puMtrArm(){
  if (PU_MTR.armed) return 'already armed';
  try {
    const BASE = 0x00684ea8, STRIDE = 0x110;
    const rdF = (p, i) => { try { return '0x' + (p.add(i*4).readU32() >>> 0).toString(16); }
                            catch(_){ return ''; } };
    const rdI = (p, i) => { try { return p.add(i*4).readS32(); } catch(_){ return 0; } };
    const snap = (p, out) => {
      for (const i of [5,6,7, 8,9,10, 0xe,0xf,0x10, 0x13,0x14]) out.push(rdF(p, i));
      out.push(rdI(p, 0x11));
    };
    // the homing target. FUN_0046d4a0(&slot, rec[0]) then the position is read at
    // [slot]+0x30..0x38 -- so arg1 holds a POINTER once the call returns.
    // RA 0x0045394a is the homing-branch call (the other, 0x00453ada, is the
    // render branch and reads the same thing one field later).
    PU_MTR.lisTgt = Interceptor.attach(ga(0x0046d4a0), {
      onEnter(){
        this.mine = (this.returnAddress.toUInt32() >>> 0) === 0x0045394a;
        if (this.mine) this.slot = this.context.esp.add(4).readU32();
      },
      onLeave(){
        if (!this.mine || !PU_MTR.pending) return;
        try {
          const q = ptr(ptr(this.slot).readU32());
          PU_MTR.pending.tgt = [rdF(q, 0xc), rdF(q, 0xd), rdF(q, 0xe)];  // +0x30..0x38
        } catch(_){ PU_MTR.pending.tgt = null; }
      }
    });
    PU_MTR.lis = Interceptor.attach(ga(0x004538b0), {
      onEnter(){
        PU_MTR.calls++;
        const p = this.context.eax;
        const idx = (p.toUInt32() - BASE) / STRIDE;
        const pre = [];
        snap(p, pre);
        // the spawn-time fields the integrator reads but never writes
        for (const i of [0, 2,3,4, 0x15]) pre.push(i === 0 ? rdI(p, 0) : rdF(p, i));
        PU_MTR.pending = { fr: SD.frames, cl: PU.calls, idx: idx, p: p,
                           pre: pre, tgt: null };
      },
      onLeave(){
        const q = PU_MTR.pending; PU_MTR.pending = null;
        if (!q) { PU_MTR.orphans++; return; }
        if (PU_MTR.rows.length >= PU_MTR.cap) return;
        const post = [];
        snap(q.p, post);
        const t = q.tgt || ['', '', ''];
        PU_MTR.rows.push([q.fr, q.cl, q.idx].concat(q.pre, t, post));
      }
    });
    PU_MTR.armed = true;
    return 'puhook-mortar armed (0x004538b0 + 0x0046d4a0)';
  } catch(e){ PU_MTR.err = '' + e; return 'ERR ' + e; }
}
function puMtrDrain(){ const r = PU_MTR.rows; PU_MTR.rows = []; return r; }

// --- D3-CONTACT 2026-09-28c: the MISSILE projectile pool -------------------
// Opt-in (--puhook-missile). Its own channel, its own CSV.
//
// The MISSILE tick is 0x00455c90 and Ghidra's auto-analysis never defined it, so
// it was read with the `--create` mode added to re/tools/decomp_pc.py this
// session (a TRANSIENT function definition against a -readOnly pool clone; it is
// discarded on exit and is NOT a master write).
//
// It walks TWO pools in lockstep in one loop:
//   aim records   0x006885d0 stride 0x2c, FIVE entries
//     (`MOV EDI,0x6886ac` @0x00455c9a, `SUB EDI,0x2c` @0x00455ca9)
//   projectiles   0x006883b0 stride 0x6c, FIVE entries
//     (`MOV EBP,0x688620` @0x00455c9f, `SUB EBP,0x6c` @0x00455caf, loop exits at
//      0x00688404 -- so the record BASES are 0x6883b0/41c/488/4f4/560)
// The older note's "pool DAT_006883bc stride 0x6c" named the record's POSITION
// field (base+0xc), not its base -- the same off-by-a-field the acquisition
// record's &DAT_0068b9fc gloss had.
//
// Record fields, byte offsets from the base, from FUN_00455610/FUN_004556f0
// (both __thiscall-style on ESI) and the tick's own indices:
//   +0x0c..0x14  position          +0x1c..0x24  this frame's delta
//   +0x28        the GROUND-FOLLOW bias added to delta.y -- written by the tick
//                from the 0x4b4cd0 probe's result, so the port can reproduce it
//                exactly from the injected verdict
//   +0x18 age    +0x30 speed       +0x54/+0x58 target ids (-1,-1 = unguided)
//   +0x50        live flag
//
// What the tick does per live record (0x00455cf9..0x00455e9f):
//   flight step: (+0x54==-1 && +0x58==-1) ? FUN_00455610 : FUN_004556f0
//   age += DAT_007f100c;  if (age > 3.0) -> FUN_00455910 terminal, done
//   EVEN FRAMES ONLY (`DAT_007f101c & 0x80000001`): sphere query FUN_004b4d10
//     with radius |delta|^2 * 1.5 + 0.05; on a hit, gate FUN_0045c350, and a
//     ZERO gate -> FUN_00455910. That parity gate is why 0x455de0 shows ~half
//     the calls of 0x455e59 in every capture (m1 15 vs 31, m2 6 vs 12).
//   EVERY frame: ground probe FUN_004b4cd0 from pos to pos - (0,3,0).
//
// PRE and POST are both recorded, per record, once per tick.
const PU_MIS = { armed:false, rows:[], calls:0, cap:20000, pending:null,
                 orphans:0, err:null };
function puMisArm(){
  if (PU_MIS.armed) return 'already armed';
  try {
    const BASE = 0x006883b0, STRIDE = 0x6c, N = 5;
    const rdF = (p, o) => { try { return '0x' + (p.add(o).readU32() >>> 0).toString(16); }
                            catch(_){ return ''; } };
    const rdI = (p, o) => { try { return p.add(o).readS32(); } catch(_){ return 0; } };
    const snap = () => {
      const out = [];
      for (let i = 0; i < N; ++i) {
        const p = ga(BASE + i * STRIDE);
        out.push([rdI(p, 0x50),
                  rdF(p, 0x0c), rdF(p, 0x10), rdF(p, 0x14),
                  rdF(p, 0x1c), rdF(p, 0x20), rdF(p, 0x24),
                  rdF(p, 0x28), rdF(p, 0x18), rdF(p, 0x30),
                  rdI(p, 0x54), rdI(p, 0x58)]);
      }
      return out;
    };
    PU_MIS.lis = Interceptor.attach(ga(0x00455c90), {
      onEnter(){
        PU_MIS.calls++;
        let fc = 0;
        try { fc = ga(0x007f101c).readS32(); } catch(_){}
        PU_MIS.pending = { fr: SD.frames, cl: PU.calls, fc: fc, pre: snap() };
      },
      onLeave(){
        const q = PU_MIS.pending; PU_MIS.pending = null;
        if (!q) { PU_MIS.orphans++; return; }
        const post = snap();
        for (let i = 0; i < N; ++i) {
          // emit only records that were LIVE on entry: a dead slot's fields are
          // stale, and a row for one would read as a projectile that never existed.
          if (q.pre[i][0] === 0) continue;
          if (PU_MIS.rows.length >= PU_MIS.cap) return;
          PU_MIS.rows.push([q.fr, q.cl, i, q.fc].concat(q.pre[i], post[i]));
        }
      }
    });
    PU_MIS.armed = true;
    return 'puhook-missile armed (0x00455c90, 5 records @0x006883b0 stride 0x6c)';
  } catch(e){ PU_MIS.err = '' + e; return 'ERR ' + e; }
}
function puMisDrain(){ const r = PU_MIS.rows; PU_MIS.rows = []; return r; }
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
  sdArm: function(car, withAi){ return sdArm(car, withAi); },
  // [D3 2026-09-14] OUTPUT-SLOT TABLE repair. CONTRIVED state (C3-grade), same
  // class as pokeLap/pokeCollect: it writes what the ORIGINAL's own allocator
  // writes, because the warp launch skips the allocator.
  //
  // Evidence (pool0, 2026-09-14): the table is 4 entries at 0x007f1a14 stride 0x10
  // (loop bound 0x007f1a54 at 0x0043f832). FUN_0042b9e0 @0x0042bab0 resets every
  // entry to -1; the race-launch allocator at 0x0043f870..0x0043f8a4 then scans for
  // the lowest index no entry holds and commits it with MOV [EBX],ESI @0x0043f895,
  // followed by CarSlotStateSet(car,2) @0x0043f89a. Run in car order over 4 cars
  // that yields 0,1,2,3. FUN_00418560 reads the entry at 0x0041856c and derives the
  // ctrl block as 0x007f1038 + entry*0x4c, so leaving the table at its .bss zeros
  // makes ALL FOUR cars write controller 0's block -- measured directly: 3 cars,
  // 1 distinct block, blocks 1..3 all-zero for 2710 frames.
  pokeCtrlSlots: function(){
    try {
      const out = [];
      for (let i = 0; i < 4; ++i) {
        const p = ga(0x007f1a14 + i * 0x10);
        out.push(p.readS32());
        p.writeS32(i);
      }
      return 'ctrl slot table was [' + out.join(',') + '] -> [0,1,2,3]';
    } catch(e){ return 'ERR ' + e; }
  },
  // [U-9147 2026-09-29] --peek: read image globals by RVA. NO Interceptor, no hook,
  // no write -- a plain Memory read, so it is exempt from the hot-path rule in
  // CLAUDE.md ("Frida overhead on hot paths"). Spec is "rva:type[,...]" with type in
  // {f=float32, d=float64, i=int32, u=uint32}. Added because g_suspScale-class
  // globals (_DAT_0088e5f0, _DAT_00613108) are NOT in the 0xd04 statediff record and
  // a cross-side law comparison needs their ORIGINAL values measured, not assumed.
  peek: function(spec){
    const out = {};
    for (const part of spec.split(',')) {
      if (!part) continue;
      const bits = part.split(':');
      // a leading '@' means the hex is an ABSOLUTE address (a heap pointer read out
      // of an earlier peek), not an image RVA. Needed for the RenderWare device
      // table: [0x007d4028] and [0x007d3ff8] are an offset and a runtime base, and
      // the function pointers live at base+offset, off-image.
      // 'i<rvaA>+<rvaB>+<off>' is the RenderWare device-table form: read the u32 at
      // each of rvaA and rvaB, add them and `off`, then read THAT absolute address.
      // RwMatrixMultiply 0x004c4600 does exactly this -- ecx=[0x007d4028],
      // ebp=[0x007d3ff8], then [ecx+ebp+4] (caps) and [ecx+ebp+8] (the mul fn ptr).
      const abs = bits[0][0] === '@';
      const ind = bits[0][0] === 'i';
      const rva = parseInt(abs || ind ? bits[0].slice(1) : bits[0], 16);
      const ty = (bits[1] || 'f');
      try {
        let p;
        if (ind) {
          const t = bits[0].slice(1).split('+');
          const b = ga(parseInt(t[0], 16)).readU32() + ga(parseInt(t[1], 16)).readU32()
                  + parseInt(t[2] || '0', 16);
          p = ptr(b);
        } else {
          p = abs ? ptr(rva) : ga(rva);
        }
        out[bits[0]] = ty === 'f' ? p.readFloat()
                     : ty === 'd' ? p.readDouble()
                     : ty === 'u' ? p.readU32() : p.readS32();
      } catch(e){ out[bits[0]] = 'ERR ' + e; }
    }
    return JSON.stringify(out);
  },
  aiStepArm: function(withLocals){ return aiStepArm(withLocals); },
  aiStepDrain: function(){ return aiStepDrain(); },
  latBracketArm: function(recBaseHex, car){ return latBracketArm(recBaseHex, car); },
  latBracketDrain: function(){ return latBracketDrain(); },
  latBracketStats: function(){ return JSON.stringify({armed:LB.armed, a6a:LB.nA6a, a6b:LB.nA6b,
                                                      sub:LB.nSub, pending:LB.rows.length,
                                                      skipped:LB.skipped, err:LB.err}); },
  axisProbeArm: function(recBaseHex, car){ return axisProbeArm(recBaseHex, car); },
  axisProbeDrain: function(){ return axisProbeDrain(); },
  axisProbeStats: function(){ return JSON.stringify({armed:AX.armed, a6a:AX.nA6a, a6b:AX.nA6b,
                                                     skipped:AX.skipped, drive:AX.nDrive,
                                                     fwdYnz:AX.nFwdYnz, axYnz:AX.nAxYnz,
                                                     b18nzPre:AX.nB18nzPre, b18nzPost:AX.nB18nzPost,
                                                     // [U-9193] G-OMEGA / G-ZERO / KA-B counters,
                                                     // printed BEFORE the rows are drained.
                                                     wxNz:AX.nWxNz, wyNz:AX.nWyNz, wzNz:AX.nWzNz,
                                                     driveWyNz:AX.nDriveWyNz,
                                                     a6bEsiRec:AX.nA6bEsiRec,
                                                     pending:AX.rows.length, chk:AX.chk, err:AX.err}); },
  slideProbeArm: function(recBaseHex, car, cap){ return slideProbeArm(recBaseHex, car, cap); },
  slideProbeDrain: function(){ return slideProbeDrain(); },
  slideProbeStats: function(){ return JSON.stringify({armed:SL.armed, calls:SL.calls,
                                                      mine:SL.mine, skipped:SL.skipped,
                                                      zeroSpeed:SL.nZeroSpeed, b0cNz:SL.nB0cNz,
                                                      pending:SL.rows.length, cap:SL.cap,
                                                      capped:SL.capped, chk:SL.chk, err:SL.err}); },
  wheelStateProbeArm: function(recBaseHex, car, countOnly){ return wheelStateProbeArm(recBaseHex, car, countOnly); },
  wheelStateProbeDrain: function(){ return wheelStateProbeDrain(); },
  wheelStateProbeStats: function(){ return JSON.stringify({armed:WS.armed, countOnly:WS.countOnly,
                                                           solver:WS.nWh, frames:WS.nFrm,
                                                           otherEdi:WS.nOther,
                                                           pending:WS.rows.length,
                                                           chk:WS.chk, err:WS.err}); },
  contactFixupProbeArm: function(recBaseHex, car){ return contactFixupProbeArm(recBaseHex, car); },
  contactFixupProbeDrain: function(){ return contactFixupProbeDrain(); },
  contactFixupProbeStats: function(){ return JSON.stringify({armed:FP.armed, fixup:FP.nFix,
                                                             sub:FP.nSub, frames:FP.nFrm, wheel:FP.nWh, worldc:FP.nWc,
                                                             pending:FP.rows.length,
                                                             selfcheck:FP.chk, err:FP.err}); },
  magProbeArm: function(sites, limit, recBaseHex, car){ return magProbeArm(sites, limit, recBaseHex, car); },
  magProbeDrain: function(){ return magProbeDrain(); },
  boostProbeArm: function(recBaseHex, limit){ return boostProbeArm(recBaseHex, limit); },
  boostProbeDrain: function(){ return boostProbeDrain(); },
  boostProbeStats: function(){ return JSON.stringify({armed:BP.armed, rel:BP.nRel, chg:BP.nChg,
                                                      pending:BP.rows.length, limit:BP.limit,
                                                      detached:BP.detached, err:BP.err}); },
  aliveProbeArm: function(){ return aliveProbeArm(); },
  aliveProbeDrain: function(){ return aliveProbeDrain(); },
  leaderProbeArm: function(l){ return leaderProbeArm(l); },
  leaderProbeDrain: function(){ return leaderProbeDrain(); },
  leaderProbeStats: function(){ return JSON.stringify({armed:LP.armed, calls:LP.n,
                                                       pending:LP.rows.length,
                                                       ka:LP.ka, err:LP.err}); },
  aliveProbeStats: function(){ return JSON.stringify({armed:AL.armed, seg:AL.seq, tick:AL.tick,
                                                      vstep:AL.vstep, pending:AL.rows.length,
                                                      events:AL.ev, ka1n:AL.ka1n, ka1ok:AL.ka1ok,
                                                      ka1bad:AL.ka1bad, err:AL.err}); },
  magProbeStats: function(){ return JSON.stringify({armed:MP.armed, calls:MP.n, other:MP.other,
                                                    pending:MP.rows.length, limit:MP.limit,
                                                    detached:MP.detached, err:MP.err}); },
  aiStepStats: function(){ return JSON.stringify({armed:AS.armed, calls:AS.calls, pending:AS.rows.length, err:AS.err,
                                                  locals:AS.locals, curv:AS.curv, localsErr:AS.localsErr, noLocals:AS.noLocals, joinMiss:AS.joinMiss, recp:Object.keys(AS_RECP).length,
                                                  tgt14a70:AS_TGT[5], tgt14c30:AS_TGT[6], tgt150e0:AS_TGT[7], tgt16060:AS_TGT[8], tgt148b0:AS_TGT[9]}); },
  puArm: function(plan, subj, warm, boxAt){ return puArm(plan, subj, warm, boxAt); },
  puDrain: function(){ return puDrain(); },
  puStats: function(){ return JSON.stringify({armed:PU.armed, calls:PU.calls, n6:PU.n, st:PU.st,
                                              pi:PU.pi, acts:PU.acts, pending:PU.rows.length, err:PU.err}); },
  puCxArm: function(list){ return puCxArm(list); },
  puCxDrain: function(){ return puCxDrain(); },
  puAimArm: function(){ return puAimArm(); },
  puAimDrain: function(){ return puAimDrain(); },
  puMisArm: function(){ return puMisArm(); },
  puMisDrain: function(){ return puMisDrain(); },
  puMisStats: function(){ return JSON.stringify({armed:PU_MIS.armed,
                          calls:PU_MIS.calls, rows:PU_MIS.rows.length,
                          orphans:PU_MIS.orphans, err:PU_MIS.err}); },
  puMtrArm: function(){ return puMtrArm(); },
  puMtrDrain: function(){ return puMtrDrain(); },
  puMtrStats: function(){ return JSON.stringify({armed:PU_MTR.armed,
                          calls:PU_MTR.calls, rows:PU_MTR.rows.length,
                          orphans:PU_MTR.orphans, err:PU_MTR.err}); },
  puAimStats: function(){ return JSON.stringify({armed:PU_AIM.armed,
                          calls:PU_AIM.calls, rows:PU_AIM.rows.length,
                          orphans:PU_AIM.orphans, err:PU_AIM.err}); },
  puCxStats: function(){ return JSON.stringify({armed:PU_CX.armed, counts:PU_CX.counts,
                                                hot:PU_CX.hot, pending:PU_CX.rows.length,
                                                err:PU_CX.err}); },
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
  // [D4 2026-10-05] generic u32 poke, "rva=val[,rva=val]" with both in hex.
  // CONTRIVED STATE (C3-grade), same class as pokeLap / pokeCtrlSlots below: it
  // writes a value the game's own code writes, to drive a state the harness cannot
  // otherwise reach. Added to enter the save state machine at 0x00409b0e, whose
  // selector DAT_008a9588 is read at 0x00409b00 and whose case 0xd calls SaveLoad.
  pokeU32: function(spec){
    const out={};
    for (const part of spec.split(',')){
      if(!part) continue;
      const kv=part.split('='); const rva=parseInt(kv[0],16); const val=parseInt(kv[1],16);
      try { ga(rva).writeU32(val); out[kv[0]]=ga(rva).readU32(); }
      catch(e){ out[kv[0]]='ERR '+e; }
    }
    return out;
  },
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
    ap.add_argument("--poke-u32", default="",
                    help="[D4 2026-10-05] CONTRIVED STATE (C3-grade). 'rva=val[,...]' "
                         "both hex, written once --poke-delay seconds into the hold. "
                         "Added to drive the save state machine at 0x00409b0e via its "
                         "selector DAT_008a9588 (read at 0x00409b00; case 0xd calls "
                         "SaveLoad 0x00404e50). Proves a path CAN run; it does not show "
                         "the game reaches that state on its own.")
    ap.add_argument("--no-warp", action="store_true",
                    help="[D4 2026-10-05] boot to the MENU and hold there for --hold "
                         "seconds instead of poking DAT_00771968=2 to warp into a race. "
                         "The frontend save-load path only runs on the way through the "
                         "menus, so this is the only mode in which gamesave.bin is "
                         "actually READ -- a warp-launched run leaves the save buffer "
                         "empty and the championship table at code defaults. Everything "
                         "downstream that needs phase 3 is skipped.")
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
    ap.add_argument("--poke-ctrl-slots", action="store_true",
                    help="[D3] write the AI output-slot table 0x007f1a14[0..3] = 0,1,2,3 just "
                         "before the race starts. CONTRIVED state (C3-grade) but it restores "
                         "what the ORIGINAL's own allocator commits at 0x0043f895 and the warp "
                         "launch skips: without it every car reads slot 0 and all four AI "
                         "cars write controller 0's ctrl block (measured 2026-09-14 -- 3 cars, "
                         "1 block, blocks 1..3 all-zero for 2710 frames). REQUIRED for any AI "
                         "behavioural capture; physics captures that drive block 0 directly "
                         "(the cook injector) are unaffected.")
    ap.add_argument("--statediff-aistep", action="store_true",
                    help="[D3] hook the AI control step FUN_00416250 and write every call to "
                         "<out>.aistep.csv: frame,seq,v,block,spline,c0,c1,c3,c4,c5,ai_type,"
                         "ai_spline_idx,ai_override,ai_mode. This is the D3 AI ground truth -- "
                         "it reads the ctrl block the CALLER passes (FUN_00418560 @0x004187b7, "
                         "cdecl, EDI=block), so it does not depend on the slot table at "
                         "0x007f1a14, which a warp-launched race leaves unpopulated. Covers "
                         "every AI car in one run. ~180 calls/s at 4 cars.")
    ap.add_argument("--statediff-puhook", action="store_true",
                    help="[D3 WS-D] hook the power-up dispatcher FUN_0045bba0 (+ the 9 FIRE fns, "
                         "8 CANFIRE fns and FUN_0045bac0) and write <out>.puhook.csv: one row per "
                         "dispatcher call per slot that holds a power-up or had an event: frame,"
                         "call,state,slot,ctrl,cur3,prev3,cur4,prev4,boxstate,dt,act,code_pre,"
                         "h_pre,code_post,h_post,fire_modes,canfire_rets,deact_ra,rec_pre,"
                         "rec_post. rec = the ARM handle's pool record, hex.")
    ap.add_argument("--pu-plan", default="",
                    help="[D3 WS-D] comma list of type codes to force-activate on --pu-subj "
                         "through the original's FUN_0045c010(slot,code), each followed by a "
                         "scripted fire pattern on ctrl byte 0x007f103f (CONTRIVED, C3-grade). "
                         "Empty = observe natural pickups only.")
    ap.add_argument("--pu-subj", type=int, default=0, help="slot the --pu-plan drives (default 0)")
    ap.add_argument("--pu-box", type=int, default=-1,
                    help="[D3 powerups] plan index before which the subject slot's box state "
                         "DAT_0068d1f0[subj] is forced to 2 through the original's own setter "
                         "FUN_0045ba00 (0x0045ba00). The dispatcher then latches it to 3 at "
                         "0x0045bc85 and skips the whole per-slot pass (0x0045bca5) for that "
                         "activation. CONTRIVED state (C3-grade, same class as --pu-plan): it "
                         "forces the VALUE and nothing else -- every branch taken on it is the "
                         "original's. Use it to exercise the box gate on demand instead of "
                         "waiting for a wreck.")
    ap.add_argument("--puhook-missile", action="store_true",
                    help="[D3-CONTACT] with --statediff-puhook, ALSO hook the MISSILE "
                         "tick FUN_00455c90 and write <out>.pumissile.csv: one row per "
                         "LIVE projectile per frame with the record's PRE and POST state "
                         "and the frame counter DAT_007f101c (the tick gates its sphere "
                         "query on that counter's parity, which is why 0x455de0 shows "
                         "about half the calls of 0x455e59). Pool 0x006883b0, stride "
                         "0x6c, 5 records. Floats are raw hex dwords.")
    ap.add_argument("--puhook-mortar", action="store_true",
                    help="[D3-CONTACT] with --statediff-puhook, ALSO hook the MORTAR "
                         "projectile update FUN_004538b0 and write <out>.pumortar.csv: "
                         "one row per projectile per frame with the record's PRE and "
                         "POST state (position, velocity, this frame's delta, age, "
                         "homing flag) plus the homing target. MORTAR's own contact "
                         "site 0x453789 probes pos -> pos + delta, so this is what "
                         "lets a replay reproduce both the segment and the call count. "
                         "Pool 0x00684ea8, stride 0x110, 32 records. Floats are raw "
                         "hex dwords.")
    ap.add_argument("--puhook-aim", action="store_true",
                    help="[D3-CONTACT] with --statediff-puhook, ALSO hook the target "
                         "ACQUISITION routine FUN_00459620 (the one MORTAR, GUN and "
                         "MISSILE share) and write <out>.puaim.csv. One row per "
                         "acquisition call: the call's four arguments, the firing car's "
                         "aim-matrix `at` row, the second candidate list's count, and "
                         "all four cars' active flag + position. These are exactly the "
                         "inputs to the `candidate count == 0` branch that decides "
                         "whether the 0x459c19 query site fires; without them a replay "
                         "cannot reproduce either query site's call count. Floats are "
                         "emitted as raw hex dwords.")
    ap.add_argument("--puhook-contacts", nargs="?", const="all", default="",
                    help="[D3-CONTACT] with --statediff-puhook, ALSO hook the power-up "
                         "contact/impact chain and write <out>.pucontact.csv: frame,call,rva,"
                         "name,ret_addr,a1,a2,a3,ret (the 3 stack args and the return value of "
                         "each call). Set = the dispatcher armed sweep 0x004b4b60 / 0x0045c350 "
                         "(disassembled at 0x0045bcc8..0x0045bd11: sweep != 0 AND confirm == 0 "
                         "-> FUN_0045bac0 deactivate) plus the RVAs D3_POWERUPS_2026-09-26.md "
                         "§6 names for the MISSILE/OIL chains: 0x004b4d10, 0x004b4cd0, "
                         "0x004b4650, 0x004b5080, 0x00455910, 0x00455100. Pass a comma list of "
                         "hex RVAs to narrow it. Each listener DETACHES itself after 6000 calls "
                         "(no RVA here has a measured rate; Interceptor on a >1000/s path "
                         "destabilises MASHED in ~6 s), and the count plus a `hot` flag still "
                         "come back in the stats line.")
    ap.add_argument("--pu-warm", type=int, default=120,
                    help="state-6 dispatcher calls to wait before the first activation")
    ap.add_argument("--statediff-aictrl", action="store_true",
                    help="[D3] alongside --statediff-out, also sample the AI CONTROL BLOCK "
                         "for the same car each phase-3 render tick and write it to "
                         "<out>.aictrl.csv. Columns: frame,slot,c0,c1,c3,c4,c5,ai_type,"
                         "ai_spline_idx,ai_override,ai_mode. c0/c1 = the steer pair, c4 = "
                         "accel, c5 = brake (re/analysis/ai_ctrl_byte_map_RESOLVED_2026-06-16"
                         ".md); block base 0x007f1038 stride 0x4c, slot = *(int*)(0x007f1a14 "
                         "+ car*0x10). The MSD1 payload is unchanged, so join on the frame "
                         "index for position/velocity. Use an OPPONENT slot (1..3) to capture "
                         "what the original's FUN_00416250 commands.")
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
    ap.add_argument("--statediff-steer-schedule", default="",
                    help="A8 ramp regime (2026-09-13): 't:steer,t:steer,...' with t in seconds "
                         "after the FIRST DRIVING FRAME (record speed +0x9e4 > 50, polled every "
                         "0.25 s) and steer a float in [-1,1] (fractions write A4's byte as "
                         "round(|s|*255)). Overrides --statediff-steer once driving starts. The "
                         "port's MASHED_PLAY_DEMO ramp is '0:0,1:0.5,6:-0.5,11:1,16:-1' in "
                         "physics-log time (its td clock starts ~3 s earlier, in the countdown).")
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
    ap.add_argument("--lat-bracket", action="store_true",
                    help="[U-9156 / D2 section 21.3] sample the player record's velocity, "
                         "forward axis and angular velocity at A6a ENTRY (0x00467650) and "
                         "at the substep-loop ENTRY (0x004709a0), so one frame's lateral "
                         "change splits into [A6a+A6b] and [the 2x25 contact substeps]. "
                         "Entry hooks only, ~180 calls/s. Writes <statediff-out>."
                         "latbracket.csv. Requires --statediff-out.")
    ap.add_argument("--slide-probe", action="store_true",
                    help="[D2 attempt 17] entry-hook A4 0x00470670 (record base in EAX) "
                         "and log the SEVEN inputs the +0xb0c writer reads -- vel "
                         "+0x9b0/b4/b8, fwd +0x9d4/d8/dc, speed +0x9e4 -- plus the "
                         "+0xb0c standing in the record at entry (i.e. the value the "
                         "PREVIOUS call stored), the frame marker SD.frames, the game "
                         "tick 0x007f101c and the launch markers +0xbf8/+0xbf4. The "
                         "known-answer check is cross-call: predict from row i's inputs, "
                         "compare to row i+1's b0c. Entry only, ~60 calls/s. Writes "
                         "<statediff-out>.slideprobe.csv. Requires --statediff-out.")
    ap.add_argument("--slide-probe-limit", type=int, default=20000,
                    help="hard row limit for --slide-probe; stops appending at the limit "
                         "and reports capped=true rather than growing without bound.")
    ap.add_argument("--axis-probe", action="store_true",
                    help="[U-9175 / D2 attempt 16] entry-hook A6a 0x00467650 (PRE, "
                         "ESI-filtered) and A6b 0x00468980 (POST) and log the player "
                         "record's body forward +0x9d4/d8/dc, drive accumulator "
                         "+0xb14/18/1c, speed, grounded, boost-state +0xbf8, trackId "
                         "+0x1f0 and the four wheels' forward-axis Y (+0x224/2e8/3ac/470). "
                         "Brackets A6a's write of +0xb18 to distinguish a STRUCTURAL zero "
                         "(forward-Y == 0) from a later-step zero. Entry hooks only, "
                         "~120 calls/s. Writes <statediff-out>.axisprobe.csv. Requires "
                         "--statediff-out. NOT player-only: the record is picked by "
                         "--statediff-car, so --statediff-car 1 samples AI car 1 (and with "
                         "4 cars live the call rate is ~4x the single-car figure above). "
                         "[U-9193 2026-10-05] also logs angular velocity "
                         "+0x9bc/+0x9c0/+0x9c4 (wx,wy,wz), record position "
                         "+0x958/+0x960 (px,pz) and the RAW esi at both sites. A6b is "
                         "deliberately left UNFILTERED because its ESI is not established "
                         "to hold the record pointer; `esi` makes POST-row attribution "
                         "checkable instead of assumed (KA-B).")
    ap.add_argument("--fixup-probe", action="store_true",
                    help="[U-9156 / D2 section 22.1] entry-hook VehicleContactFixup "
                         "0x0046ef70 and the substep loop 0x004709a0 with one shared "
                         "sequence, and log the player record's velocity, speed, grounded, "
                         "+0x9ec, the +0x144 accumulator and the LIVE 18-slot contact set "
                         "(depth/normal/scale/arm/magnitude). The .msd cannot see the slots "
                         "-- they are cleared by the time the render tick snapshots the "
                         "record. Entry hooks only; the port's equivalent fires 211 times in "
                         "a 27 s race, so this is not a hot path. Writes <statediff-out>."
                         "fixupprobe.csv. Requires --statediff-out.")
    ap.add_argument("--wheelstate-probe", action="store_true",
                    help="[U-9179, D2 attempt 20] entry-hook-only per-wheel sample at "
                         "0x0046f6c0 (state +0x198, fv +0x194, key +0x1ec, stride 0xc4) "
                         "plus the A6a frame marker; writes <out>.wheelstate.csv")
    ap.add_argument("--wheelstate-probe-count", action="store_true",
                    help="count-first safety gate for --wheelstate-probe: arm the hooks "
                         "with an n++ body and take no sample, so the rate is measured "
                         "before any run reads memory")
    ap.add_argument("--mag-probe", default="",
                    help="[U-9156 / D2 section 21.7] entry-hook RwV3dLength 0x004c3ac0 and log "
                         "the VECTOR it was passed, tagged by call site via the return "
                         "address. Value is a comma list of hex RETURN addresses, or the "
                         "literal 'count' to measure the call RATE only (callback body is a "
                         "single increment -- run this FIRST, the function has 120 call sites "
                         "image-wide). Writes <statediff-out>.magprobe.csv. Requires "
                         "--statediff-out.")
    ap.add_argument("--boost-probe", action="store_true",
                    help="[U-9174] entry hooks on 0x0046d780 (launch rev-charge RELEASE, "
                         "writes veh+0xbf8 = 1 or 2 at 0x0046d7cf / 0x0046d7a2) and "
                         "0x0046d7f0 (the per-tick charge on veh+0xbf4). Records "
                         "charge/state/race-state/accel-byte on entry AND on return, so a "
                         "write is visible across the pair with no mid-function probe. "
                         "Writes <out>.boostprobe.csv.")
    ap.add_argument("--boost-probe-limit", type=int, default=20000,
                    help="hard row limit for --boost-probe; auto-detaches at the limit "
                         "(a run that hits it is VOID, not a result).")
    ap.add_argument("--alive-probe", action="store_true",
                    help="[D3 2026-10-03] per-race-update ALIVE / ELIMINATION record on the "
                         "ORIGINAL. Entry hooks on FUN_00410d10 (SegmentCheck), FUN_00418560 "
                         "(AiVehicleStep, per-slot tally) and FUN_00418860 (AiTickLoop). "
                         "Writes <out>.alive.csv and prints the alive-vector change events "
                         "plus KA-1 (the direct read of 0x008815a4 + i*0xd04 against "
                         "FUN_0046c7b0 itself). Requires --statediff-out.")
    ap.add_argument("--leader-probe", action="store_true",
                    help="[D3 2026-10-03] ONE ENTRY HOOK on FUN_004148b0 that logs every "
                         "global Ai/AiLeaderTimer.cpp's LeaderTimer reads, so the "
                         "'would wiring it into mashed_re.exe be inert?' question is "
                         "answered from the ORIGINAL's own values. Reads nothing back and "
                         "writes nothing, so it cannot perturb the arm it measures. "
                         "Pre-registration verify/d3_leader_20261003/PREREG_WITNESS.md. "
                         "Writes <out>.leaderprobe.csv. Requires --statediff-out.")
    ap.add_argument("--leader-probe-limit", type=int, default=20000,
                    help="hard row limit for --leader-probe; a run that hits it is VOID.")
    ap.add_argument("--mag-probe-limit", type=int, default=20000,
                    help="[U-9156] hard row cap; the hook DETACHES itself on reaching it, so "
                         "hot-path exposure is bounded. 0 = no cap (not recommended).")
    ap.add_argument("--peek", default="",
                    help="[U-9147] comma-separated image globals to READ periodically, "
                         "'rva:type' with type f/d/i/u (default f). No Interceptor, no "
                         "hook, no write. Use for globals that are not in the 0xd04 "
                         "statediff record, e.g. 0088e5f0:f (g_suspScale) or "
                         "00613108:f (the steer-torque constant).")
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
    env.setdefault("MASHED_MUTE", "1")     # project rule: every launch muted
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
    if os.environ.get("MASHED_NO_SELFTEST", "").strip() not in ("", "0"):
        # Control run: hooks installed LIVE but the in-process shadow A/B NOT armed. Used to
        # tell "the port crashes" from "the A/B harness crashes" (Lane 3 first run, 2026-09-10).
        env.pop("MASHED_PHYS_C4_SELFTEST", None)
        env.pop("MASHED_SHADOW_AB", None)
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
    sd_ai      = []          # [D3] (frame_idx, [slot,c0,c1,c3,c4,c5,type,idx,ovr,mode])

    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") == "sd" and d is not None:
            sd_records.append((p["f"], d))
            if p.get("ai") is not None: sd_ai.append((p["f"], p["ai"]))
            return
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
            print("  [statediff]", E.sd_arm(args.statediff_car, args.statediff_aictrl))
            if args.statediff_aistep:
                print("  [statediff]", E.ai_step_arm(os.environ.get("MASHED_AISTEP_LOCALS", "1") != "0"))
            if args.lat_bracket:
                print("  [statediff]", E.lat_bracket_arm("0x008815a0", args.statediff_car))
            if args.slide_probe:
                print("  [statediff]", E.slide_probe_arm("0x008815a0", args.statediff_car,
                                                          args.slide_probe_limit))
            if args.axis_probe:
                print("  [statediff]", E.axis_probe_arm("0x008815a0", args.statediff_car))
            if args.fixup_probe:
                print("  [statediff]", E.contact_fixup_probe_arm("0x008815a0",
                                                                 args.statediff_car))
            if args.wheelstate_probe or args.wheelstate_probe_count:
                print("  [statediff]", E.wheel_state_probe_arm(
                    "0x008815a0", args.statediff_car,
                    bool(args.wheelstate_probe_count)))
            if args.mag_probe:
                _sites = "" if args.mag_probe == "count" else args.mag_probe
                print("  [statediff]", E.mag_probe_arm(_sites, args.mag_probe_limit,
                                                        "0x008815a0", args.statediff_car))
            if args.boost_probe:
                print("  [statediff]", E.boost_probe_arm("0x008815a0",
                                                          args.boost_probe_limit))
            if args.alive_probe:
                print("  [statediff]", E.alive_probe_arm())
            if args.leader_probe:
                print("  [statediff]", E.leader_probe_arm(args.leader_probe_limit))
            if args.statediff_puhook:
                print("  [statediff]", E.pu_arm(args.pu_plan, args.pu_subj, args.pu_warm,
                                                 args.pu_box))
                if args.puhook_contacts:
                    print("  [statediff]", E.pu_cx_arm(args.puhook_contacts))
                if args.puhook_aim:
                    print("  [statediff]", E.pu_aim_arm())
                if args.puhook_mortar:
                    print("  [statediff]", E.pu_mtr_arm())
                if args.puhook_missile:
                    print("  [statediff]", E.pu_mis_arm())
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
        # [D4 2026-10-05] --no-warp: boot to the MENU and hold there, skipping the
        # one poke that warps into a race.
        #
        # WHY THIS EXISTS. The warp is exactly one call, and it is what made the save
        # subsystem unobservable. verify/d4_save_20261005 spent three legs on "does
        # the original accept the standalone's gamesave.bin" and all three were VOID
        # or inconclusive for the SAME reason, found from three directions:
        #   * 0x00803358 (the serialization buffer) read 0 on every sample
        #     (RESULT_SAVE.md) -- the buffer is only live during a save/load call;
        #   * *0x008a94a8 was never reached;
        #   * the live championship table 0x007F0A40 showed CODE DEFAULTS, proven by
        #     a positive-control save whose edited row never appeared
        #     (RESULT_ACCEPT2.md).
        # Root cause: a warp-launched run never executes the frontend path that reads
        # gamesave.bin. The file is only loaded on the way through the menus.
        #
        # So this flag gates ONE line and changes nothing else. With it set the script
        # holds at the menu for --hold seconds, which is all --peek needs; everything
        # downstream that assumes phase 3 is skipped and said so.
        if args.no_warp:
            print("  [launch] --no-warp: NOT poking DAT_00771968; holding at the menu")
            if args.oracle:
                print("  [oracle] SKIPPED -- needs a running race")
            print("\n  *** HOLDING AT MENU (no warp) ***")
            # Same --peek cadence and same call as the race hold loop below
            # (every 6th 0.6 s tick), so menu-side and race-side peek output are
            # directly comparable.
            t0nw = time.time()
            t_end = t0nw + args.hold
            n = 0
            poked = False
            while time.time() < t_end:
                if (args.poke_u32 and not poked
                        and time.time() - t0nw >= args.poke_delay):
                    poked = True
                    print(f"\n  [poke-u32] +{time.time()-t0nw:.1f}s CONTRIVED ->",
                          E.pokeU32(args.poke_u32))
                if args.peek and n % 6 == 0:
                    try:
                        print(f"\n  [peek] +{time.time()-t0nw:.1f}s", E.peek(args.peek))
                    except Exception as ex:
                        print(f"\n  [peek] failed: {ex}")
                time.sleep(0.6)
                n += 1
            print("  [no-warp] hold elapsed; detaching")
            raise SystemExit(0)
        # 3) poke the state machine into load+spawn
        print("  [launch] poke DAT_00771968 = 2 ->", E.launch())
        if args.oracle:
            print("  [oracle]", E.arm_oracle())
        # 4) wait for the race to be running (phase 3)
        ph3 = wait_phase(3, 40, "race running (phase 3)")
        if ph3 is None: raise SystemExit
        print("\n  *** RACE RUNNING (phase 3) ***")
        # [D3] Repair the output-slot table now that the race is up. It must happen
        # AFTER phase 3 (the spawn path is what would normally have populated it) and
        # BEFORE the AI has done meaningful work, so this is the only correct moment.
        if args.poke_ctrl_slots:
            print("  [ctrl-slots]", E.poke_ctrl_slots())
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
        # A8 ramp schedule (2026-09-13): drive-relative steer steps, clock = first frame
        # with record speed > 50. Applied through the same E.drive RPC as the held steer.
        sched = []
        if args.statediff_steer_schedule:
            for kv in args.statediff_steer_schedule.split(","):
                ts, sv = kv.split(":"); sched.append((float(ts), float(sv)))
            sched.sort()
        sched_t0 = None; sched_i = 0; sched_log = []
        while time.time() < t:
            if psutil and not psutil.pid_exists(pid):
                print("\n  game exited."); exited_early = True; break
            if sched:
                try:
                    if sched_t0 is None:
                        cinfo = E.carinfo(); v = cinfo.get("vel", [0, 0, 0])
                        if (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) ** 0.5 > 50.0:
                            sched_t0 = time.time()
                            print(f"\n  [schedule] first driving frame at +{sched_t0 - t0:.1f}s (|vel|>50); schedule clock started")
                    if sched_t0 is not None:
                        td = time.time() - sched_t0
                        while sched_i < len(sched) and td >= sched[sched_i][0]:
                            sv = sched[sched_i][1]
                            E.drive(1, sv); sched_log.append((round(td, 2), sv))
                            print(f"\n  [schedule] td={td:.2f}s steer -> {sv:+.2f}")
                            sched_i += 1
                except Exception as ex:
                    print(f"\n  [schedule] step failed: {ex}")
            if args.peek and n % 6 == 0:   # [U-9147] plain global reads, no hook
                try: print(f"\n  [peek] +{time.time()-t0:.1f}s", E.peek(args.peek))
                except Exception as ex: print(f"\n  [peek] failed: {ex}")
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
            time.sleep(0.25 if sched else 0.6)
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
                print(f"  inputs seen (per rule, every SegmentCheck entry): {st.get('seen')}")
                print(f"  rule-10 seed FUN_004046a0 exits: {st.get('seed10')}")
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
                # [D3 2026-09-14] AI control-block sidecar. Same non-degeneracy
                # discipline as the .msd above: a ctrl trace whose bytes never move
                # tells you the AI never commanded anything, which is a capture
                # failure, not a measurement — so it is reported, not hidden.
                if args.lat_bracket:
                    # [U-9156 / D2 section 21.3] Report the counters BEFORE the rows, and
                    # report them even when the drain is empty: memory
                    # `absent-log-proves-nothing-run-a-control` -- a zero-row file cannot
                    # tell "the sites never fired" from "the probe is broken", and the
                    # counters can.
                    try: print("  [statediff] lat-bracket agent:", E.lat_bracket_stats())
                    except Exception as _e: print("  [statediff] lat-bracket stats failed:", _e)
                    lb_rows = []
                    try: lb_rows = E.lat_bracket_drain()
                    except Exception as _e: print("  [statediff] lat-bracket drain failed:", _e)
                    lbp = outp.with_suffix(outp.suffix + ".latbracket.csv")
                    with open(lbp, "w", newline="") as f:
                        f.write("seq,site,velx,vely,velz,fwdx,fwdy,fwdz,speed,gnd,avx,avy,avz"
                                + chr(10))
                        for r in lb_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    n0 = sum(1 for r in lb_rows if r[1] == 0)
                    n1 = sum(1 for r in lb_rows if r[1] == 1)
                    n2 = sum(1 for r in lb_rows if r[1] == 2)
                    print(f"  [statediff] lat-bracket {len(lb_rows)} samples "
                          f"(A6a {n0}, A6b {n2}, substep {n1}) -> {lbp}")
                if args.axis_probe:
                    # Counters + known-answer self-check BEFORE the rows, CSV written even
                    # when empty: memory `absent-log-proves-nothing-run-a-control`.
                    try: print("  [statediff] axis-probe agent:", E.axis_probe_stats())
                    except Exception as _e: print("  [statediff] axis-probe stats failed:", _e)
                    ax_rows = []
                    try: ax_rows = E.axis_probe_drain()
                    except Exception as _e: print("  [statediff] axis-probe drain failed:", _e)
                    axp = outp.with_suffix(outp.suffix + ".axisprobe.csv")
                    with open(axp, "w", newline="") as f:
                        f.write("seq,site,fwdx,fwdy,fwdz,b14,b18,b1c,speed,gnd,bf8,tid,"
                                "axy0,axy1,axy2,axy3,"
                                # [U-9193] appended; existing columns unmoved.
                                "wx,wy,wz,px,pz,esi" + chr(10))
                        for r in ax_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    _n0 = sum(1 for r in ax_rows if r[1] == 0)
                    _n1 = sum(1 for r in ax_rows if r[1] == 1)
                    print(f"  [statediff] axis-probe {len(ax_rows)} samples "
                          f"(A6a-PRE {_n0}, A6b-POST {_n1}) -> {axp}")
                if args.slide_probe:
                    # Counters and the static known-answer constants BEFORE the rows, and
                    # the CSV is written even when empty: memory
                    # `absent-log-proves-nothing-run-a-control` -- a zero-row file cannot
                    # tell "A4 never fired for this record" from "the probe is broken".
                    try: print("  [statediff] slide-probe agent:", E.slide_probe_stats())
                    except Exception as _e: print("  [statediff] slide-probe stats failed:", _e)
                    sl_rows = []
                    try: sl_rows = E.slide_probe_drain()
                    except Exception as _e: print("  [statediff] slide-probe drain failed:", _e)
                    slp = outp.with_suffix(outp.suffix + ".slideprobe.csv")
                    with open(slp, "w", newline="") as f:
                        f.write("seq,sdframe,tick,velx,vely,velz,fwdx,fwdy,fwdz,"
                                "speed,b0c_entry,bf8,bf4" + chr(10))
                        for r in sl_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    print(f"  [statediff] slide-probe {len(sl_rows)} rows -> {slp}")
                if args.fixup_probe:
                    # Counters and the register self-check BEFORE the rows, and the CSV is
                    # written even when empty: memory
                    # `absent-log-proves-nothing-run-a-control` -- a zero-row file cannot
                    # tell "0x0046ef70 never fired" from "the probe is broken".
                    try: print("  [statediff] fixup-probe agent:", E.contact_fixup_probe_stats())
                    except Exception as _e: print("  [statediff] fixup-probe stats failed:", _e)
                    fp_rows = []
                    try: fp_rows = E.contact_fixup_probe_drain()
                    except Exception as _e: print("  [statediff] fixup-probe drain failed:", _e)
                    fpp = outp.with_suffix(outp.suffix + ".fixupprobe.csv")
                    _hdr = ("seq,site,frame,velx,vely,velz,speed,gnd,c9ec,a144,a148,a14c,"
                            "fwdx,fwdz,nslot,slotidx")
                    for _k in range(2):
                        _hdr += (",s%d_i,s%d_depth,s%d_nx,s%d_ny,s%d_nz,s%d_scale,"
                                 "s%d_armx,s%d_army,s%d_armz,s%d_mag") % tuple([_k] * 10)
                    _hdr += ",slots_all"
                    with open(fpp, "w", newline="") as f:
                        f.write(_hdr + chr(10))
                        for r in fp_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    _n0 = sum(1 for r in fp_rows if r[1] == 0)
                    _n1 = sum(1 for r in fp_rows if r[1] == 1)
                    _n2 = sum(1 for r in fp_rows if r[1] == 2)
                    print(f"  [statediff] fixup-probe {len(fp_rows)} samples "
                          f"(fixup {_n0}, substep {_n1}, A6a/frame {_n2}) -> {fpp}")
                if args.wheelstate_probe or args.wheelstate_probe_count:
                    # Counters and the register self-check BEFORE the rows, and the CSV is
                    # written even when empty: memory
                    # `absent-log-proves-nothing-run-a-control`.
                    try: print("  [statediff] wheelstate-probe agent:",
                               E.wheel_state_probe_stats())
                    except Exception as _e:
                        print("  [statediff] wheelstate-probe stats failed:", _e)
                    ws_rows = []
                    try: ws_rows = E.wheel_state_probe_drain()
                    except Exception as _e:
                        print("  [statediff] wheelstate-probe drain failed:", _e)
                    wsp = outp.with_suffix(outp.suffix + ".wheelstate.csv")
                    _h = "seq,site,frame,edi_is_rec,speed,gnd,bf8,velx,vely,velz"
                    for _w in range(4):
                        _h += ",w%d_state,w%d_fv,w%d_key" % (_w, _w, _w)
                    with open(wsp, "w", newline="") as f:
                        f.write(_h + chr(10))
                        for r in ws_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    print(f"  [statediff] wheelstate-probe {len(ws_rows)} samples -> {wsp}")
                if args.mag_probe:
                    # Counters BEFORE the rows, and the CSV is written even when empty:
                    # memory `absent-log-proves-nothing-run-a-control`.
                    try: print("  [statediff] mag-probe agent:", E.mag_probe_stats())
                    except Exception as _e: print("  [statediff] mag-probe stats failed:", _e)
                    mp_rows = []
                    try: mp_rows = E.mag_probe_drain()
                    except Exception as _e: print("  [statediff] mag-probe drain failed:", _e)
                    mpp = outp.with_suffix(outp.suffix + ".magprobe.csv")
                    with open(mpp, "w", newline="") as f:
                        f.write("seq,site,vx,vy,vz,rec_velx,rec_velz,rec_speed,gnd,rec_18c"
                                + chr(10))
                        for r in mp_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    _bysite = {}
                    for r in mp_rows:
                        _bysite[r[1]] = _bysite.get(r[1], 0) + 1
                    print(f"  [statediff] mag-probe {len(mp_rows)} rows -> {mpp}")
                    print(f"  [statediff] mag-probe per-site: {_bysite}")
                if args.boost_probe:
                    # Counters BEFORE the rows, CSV written even when empty:
                    # memory `absent-log-proves-nothing-run-a-control`.
                    try: print("  [statediff] boost-probe agent:", E.boost_probe_stats())
                    except Exception as _e: print("  [statediff] boost-probe stats failed:", _e)
                    bp_rows = []
                    try: bp_rows = E.boost_probe_drain()
                    except Exception as _e: print("  [statediff] boost-probe drain failed:", _e)
                    bpp = outp.with_suffix(outp.suffix + ".boostprobe.csv")
                    with open(bpp, "w", newline="") as f:
                        f.write("seq,site,phase,tick,car,bf4_in,bf8_in,state_in,accel_in,"
                                "bf4_out,bf8_out,state_out,accel_out" + chr(10))
                        for r in bp_rows:
                            f.write(",".join(str(x) for x in r) + chr(10))
                    _rel = [r for r in bp_rows if r[1] == "rel"]
                    print(f"  [statediff] boost-probe {len(bp_rows)} rows -> {bpp}")
                    for r in _rel:
                        print(f"  [statediff] boost-probe RELEASE tick={r[3]} car={r[4]} "
                              f"bf4 {r[5]}->{r[9]}  bf8 {r[6]}->{r[10]}  "
                              f"state {r[7]}->{r[11]}  accel {r[8]}->{r[12]}")
                if args.alive_probe:
                    # Counters and KA-1 BEFORE the rows; CSV written even when empty
                    # (memory `absent-log-proves-nothing-run-a-control`).
                    import json
                    _al = None
                    try:
                        _al = json.loads(E.alive_probe_stats())
                        print("  [statediff] alive-probe agent:", json.dumps(
                            {k: v for k, v in _al.items() if k != "events"}))
                    except Exception as _e:
                        print("  [statediff] alive-probe stats failed:", _e)
                    al_rows = []
                    try: al_rows = E.alive_probe_drain()
                    except Exception as _e: print("  [statediff] alive-probe drain failed:", _e)
                    alp = outp.with_suffix(outp.suffix + ".alive.csv")
                    with open(alp, "w", newline="") as f:
                        f.write("seq,frame,tick,clk_0ff4,substate,rule,participants,zoom,ret,"
                                "a0_in,a1_in,a2_in,a3_in,a0_out,a1_out,a2_out,a3_out,"
                                "vstep0,vstep1,vstep2,vstep3,"
                                "pct0,pct1,pct2,pct3" + chr(10))
                        for r in al_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    print(f"  [statediff] alive-probe {len(al_rows)} rows -> {alp}")
                    if _al:
                        print(f"  [statediff] alive-probe KA-1 (direct read vs FUN_0046c7b0): "
                              f"{_al.get('ka1ok')}/{_al.get('ka1n')} bad={_al.get('ka1bad')}")
                        print(f"  [statediff] alive-probe VehicleStep per slot "
                              f"(FUN_00418560): {_al.get('vstep')}  ticks={_al.get('tick')}")
                        for _e2 in (_al.get("events") or []):
                            print(f"  [statediff] alive-probe EVENT seq={_e2[0]} frame={_e2[1]} "
                                  f"tick={_e2[2]} clk={_e2[3]} {_e2[4]} -> {_e2[5]} "
                                  f"(enter {_e2[6]}, ret {_e2[7]}, vstep {_e2[8:12]})")
                if args.leader_probe:
                    import json
                    _lp = None
                    try:
                        _lp = json.loads(E.leader_probe_stats())
                        print("  [statediff] leader-probe agent:", json.dumps(_lp))
                    except Exception as _e:
                        print("  [statediff] leader-probe stats failed:", _e)
                    lp_rows = []
                    try: lp_rows = E.leader_probe_drain()
                    except Exception as _e: print("  [statediff] leader-probe drain failed:", _e)
                    lpp = outp.with_suffix(outp.suffix + ".leaderprobe.csv")
                    with open(lpp, "w", newline="") as f:
                        f.write("n,clk_0ff4,v,mode368,flt360,idx364,bias374,framedt,"
                                "rank,timer,prog0,prog1,prog2,prog3,"
                                "limittbl_nonzero_of64" + chr(10))
                        for r in lp_rows:
                            f.write(",".join(repr(x) if isinstance(x, float) else str(x)
                                             for x in r) + chr(10))
                    print(f"  [statediff] leader-probe {len(lp_rows)} rows -> {lpp}")
                    if _lp:
                        print(f"  [statediff] leader-probe KA thresholds "
                              f"a8/a4/a0/35c = {_lp.get('ka')}  (AiLeaderTimer.cpp:61 "
                              f"states _DAT_005cc35c == 4.0)")
                if args.statediff_aistep:
                    try: print("  [statediff] aistep agent:", E.ai_step_stats())
                    except Exception: pass
                    step_rows = []
                    try: step_rows = E.ai_step_drain()
                    except Exception as _e: print("  [statediff] aistep drain failed:", _e)
                    stp = outp.with_suffix(outp.suffix + ".aistep.csv")
                    with open(stp, "w", newline="") as f:
                        f.write("frame,seq,v,block,spline,c0,c1,c3,c4,c5,"
                                "ai_type,ai_spline_idx,ai_override,ai_mode,"
                                "clk_0ff4,step_1008,diff_a360,flag_a368,tgt_7ffc,"
                                "substate,rec_9e4,rec_b0c,c7,"
                                # [D3 2026-09-27] FUN_00416250 stack locals (probe
                                # 0x00416596) + the two steer-history globals
                                # 0x008032d8/0x008032dc + v*0x14, which carry the error
                                # and which of the two steering bands took it.
                                "look_x,look_z,curv,own_x,own_z,hist_d8,hist_dc,march_n,march_idx0,"
                                # [D3 2026-10-03] the targeting returns that decide
                                # `mode` (0x004162f7..0x0041645e) and the 0x00416405
                                # immediate-brake return, plus ctrl[4]/[5] on entry.
                                "ret14a70,ret14c30,ret150e0,ret16060,ret148b0,c4_in,c5_in"
                                + chr(10))
                        for r in step_rows:
                            f.write(",".join(str(x) for x in r) + chr(10))
                    cars = sorted({r[2] for r in step_rows})
                    blocks = sorted({r[3] for r in step_rows})
                    print(f"  [statediff] aistep {len(step_rows)} calls -> {stp}"
                          f"  (cars={cars}, distinct blocks={len(blocks)})")
                    # Non-degeneracy, same discipline as the .msd: one block for
                    # several cars means the cars are overwriting each other, which
                    # is a scenario defect, not a measurement.
                    if not step_rows:
                        print("  [statediff] WARNING: aistep EMPTY -- FUN_00416250 never ran "
                              "at phase 3 (mode 4/8/9 take the FUN_00416a30/FUN_00417da0 tails)")
                    elif len(blocks) < len(cars):
                        print(f"  [statediff] WARNING: {len(cars)} cars share "
                              f"{len(blocks)} ctrl block(s) -- slot table unpopulated")
                if args.statediff_puhook:
                    try: print("  [statediff] puhook agent:", E.pu_stats())
                    except Exception: pass
                    pu_rows = []
                    try: pu_rows = E.pu_drain()
                    except Exception as _e: print("  [statediff] puhook drain failed:", _e)
                    pup = outp.with_suffix(outp.suffix + ".puhook.csv")
                    with open(pup, "w", newline="") as f:
                        f.write("frame,call,state,slot,ctrl,cur3,prev3,cur4,prev4,boxstate,dt,act,"
                                "code_pre,h_pre,code_post,h_post,fire_modes,canfire_rets,"
                                "deact_ra,rec_pre,rec_post" + chr(10))
                        for r in pu_rows:
                            f.write(",".join(str(x) for x in r) + chr(10))
                    fired = [r for r in pu_rows if r[16] != ""]
                    codes = sorted({r[12] for r in pu_rows} | {r[14] for r in pu_rows})
                    print(f"  [statediff] puhook {len(pu_rows)} rows -> {pup}"
                          f"  (codes seen={codes}, rows with FIRE={len(fired)})")
                    if not pu_rows:
                        print("  [statediff] WARNING: puhook EMPTY -- no slot held a power-up "
                              "at phase 3 (check state==6 at 0x0063ba8c)")
                if args.statediff_puhook and args.puhook_contacts:
                    try: print("  [statediff] puhook-contacts agent:", E.pu_cx_stats())
                    except Exception as _e: print("  [statediff] pucontact stats failed:", _e)
                    cx_rows = []
                    try: cx_rows = E.pu_cx_drain()
                    except Exception as _e: print("  [statediff] pucontact drain failed:", _e)
                    cxp = outp.with_suffix(outp.suffix + ".pucontact.csv")
                    with open(cxp, "w", newline="") as f:
                        f.write("frame,call,rva,name,ret_addr,a1,a2,a3,ret,hit_t" + chr(10))
                        for r in cx_rows:
                            f.write(",".join(str(x) for x in r) + chr(10))
                    print(f"  [statediff] pucontact {len(cx_rows)} rows -> {cxp}")
                if args.statediff_puhook and args.puhook_aim:
                    try: print("  [statediff] puhook-aim agent:", E.pu_aim_stats())
                    except Exception as _e: print("  [statediff] puaim stats failed:", _e)
                    aim_rows = []
                    try: aim_rows = E.pu_aim_drain()
                    except Exception as _e: print("  [statediff] puaim drain failed:", _e)
                    aimp = outp.with_suffix(outp.suffix + ".puaim.csv")
                    cols = ["frame", "call", "slot", "ox", "oy", "oz", "range", "cone",
                            "list_n"]
                    for c in range(4):
                        cols += [f"c{c}_act", f"c{c}_x", f"c{c}_y", f"c{c}_z"]
                    cols += ["at_x", "at_y", "at_z"]
                    with open(aimp, "w", newline="") as f:
                        f.write(",".join(cols) + chr(10))
                        for r in aim_rows:
                            f.write(",".join(str(x) for x in r) + chr(10))
                    print(f"  [statediff] puaim {len(aim_rows)} rows -> {aimp}")
                if args.statediff_puhook and args.puhook_mortar:
                    try: print("  [statediff] puhook-mortar agent:", E.pu_mtr_stats())
                    except Exception as _e: print("  [statediff] pumortar stats failed:", _e)
                    mt_rows = []
                    try: mt_rows = E.pu_mtr_drain()
                    except Exception as _e: print("  [statediff] pumortar drain failed:", _e)
                    mtp = outp.with_suffix(outp.suffix + ".pumortar.csv")
                    snapc = ["px", "py", "pz", "vx", "vy", "vz",
                             "dx", "dy", "dz", "age", "agen", "homing"]
                    cols = (["frame", "call", "rec"]
                            + ["pre_" + c for c in snapc]
                            + ["owner", "aim_x", "aim_y", "aim_z", "baseY"]
                            + ["tgt_x", "tgt_y", "tgt_z"]
                            + ["post_" + c for c in snapc])
                    with open(mtp, "w", newline="") as f:
                        f.write(",".join(cols) + chr(10))
                        for r in mt_rows:
                            f.write(",".join(str(x) for x in r) + chr(10))
                    print(f"  [statediff] pumortar {len(mt_rows)} rows -> {mtp}")
                if args.statediff_puhook and args.puhook_missile:
                    try: print("  [statediff] puhook-missile agent:", E.pu_mis_stats())
                    except Exception as _e: print("  [statediff] pumissile stats failed:", _e)
                    mi_rows = []
                    try: mi_rows = E.pu_mis_drain()
                    except Exception as _e: print("  [statediff] pumissile drain failed:", _e)
                    mip = outp.with_suffix(outp.suffix + ".pumissile.csv")
                    snapc = ["live", "px", "py", "pz", "dx", "dy", "dz",
                             "bias", "age", "speed", "tgt0", "tgt1"]
                    cols = (["frame", "call", "rec", "framectr"]
                            + ["pre_" + c for c in snapc]
                            + ["post_" + c for c in snapc])
                    with open(mip, "w", newline="") as f:
                        f.write(",".join(cols) + chr(10))
                        for r in mi_rows:
                            f.write(",".join(str(x) for x in r) + chr(10))
                    print(f"  [statediff] pumissile {len(mi_rows)} rows -> {mip}")
                if args.statediff_aictrl:
                    ai_snapshot = list(sd_ai)
                    aip = outp.with_suffix(outp.suffix + ".aictrl.csv")
                    with open(aip, "w", newline="") as f:
                        f.write("frame,slot,c0,c1,c3,c4,c5,ai_type,ai_spline_idx,"
                                "ai_override,ai_mode,"
                                "b0_c0,b0_c1,b0_c4,b0_c5,b1_c0,b1_c1,b1_c4,b1_c5,"
                                "b2_c0,b2_c1,b2_c4,b2_c5,b3_c0,b3_c1,b3_c4,b3_c5\n")
                        for idx, row in ai_snapshot:
                            f.write(str(idx) + "," + ",".join(str(v) for v in row) + "\n")
                    ai_distinct = len({tuple(r[1:6]) for _, r in ai_snapshot})
                    print(f"  [statediff] aictrl {len(ai_snapshot)} frames -> {aip}"
                          f"  (distinct ctrl tuples={ai_distinct})")
                    if not ai_snapshot:
                        print("  [statediff] WARNING: aictrl EMPTY — the block was never "
                              "sampled (see sd_stats aiErr)")
                    elif ai_distinct <= 1:
                        print("  [statediff] WARNING: aictrl DEGENERATE — the AI never "
                              "changed its command; car "
                              f"{args.statediff_car} is probably not AI-driven")
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
                            "aictrl": bool(args.statediff_aictrl),
                            "aistep": bool(args.statediff_aistep),
                            "steer_schedule": args.statediff_steer_schedule,
                            "steer_schedule_applied": sched_log if args.statediff_steer_schedule else None,
                            "steer_schedule_t0_offset_s": (round(sched_t0 - t0, 2) if args.statediff_steer_schedule and sched_t0 else None),
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
