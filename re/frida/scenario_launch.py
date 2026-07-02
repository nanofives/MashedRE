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
  if (en.declRaw !== 0) return 0;              // port: resultDeclared != 0
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

rpc.exports = {
  ready: function(){ return modBase() ? 1 : 0; },
  armOracle: function(){ return armOracle(); },
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
    args = ap.parse_args()

    if not EXE.exists():
        print(f"error: {EXE} not found"); return 2

    env = dict(os.environ)
    env["MASHED_FPS_CAP"] = str(args.fps)
    if args.hooks:
        env["MASHED_RE_DEV"] = "1"
        env["MASHED_HOOK_ONLY"] = args.hooks
        env["MASHED_PHYS_C4_SELFTEST"] = "1"
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    else:
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"     # stock original, no installed hooks
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

    def on_msg(m, d):
        if m.get("type") == "error": print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("ready", "err"): print("  [agent]", p.get("msg") or "ready")

    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    E = scr.exports_sync

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
        # 2) write the selection globals
        cfg = {"track": args.track, "mode": args.mode, "cars": args.cars, "car": args.car,
               "rule": args.rule, "team": args.team,
               "difficulty": args.difficulty, "powerups": args.powerups}
        print("  [setup]", E.setup(cfg))
        time.sleep(0.2)
        # 3) poke the state machine into load+spawn
        print("  [launch] poke DAT_00771968 = 2 ->", E.launch())
        if args.oracle:
            print("  [oracle]", E.arm_oracle())
        # 4) wait for the race to be running (phase 3)
        ph3 = wait_phase(3, 40, "race running (phase 3)")
        if ph3 is None: raise SystemExit
        print("\n  *** RACE RUNNING (phase 3) ***")
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
        print(f"\n  racing {args.hold}s — pulsing control 4 (confirm/accel) to skip the start intro + continue rounds...")
        t0 = time.time(); t = t0 + args.hold; n = 0; poked = False; oracle_cache = None
        while time.time() < t:
            if psutil and not psutil.pid_exists(pid): print("\n  game exited."); break
            try: E.press(4, 250)            # pulse: 250ms held, ~0.35s gap -> edges for round-end prompts
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
                    print(f"\r    +{int(time.time()-t0):>3}s  spawnFired={ci.get('spawnFired')}"
                          f"  p0.grounded={ci.get('grounded')} airflag={ci.get('airflag')}"
                          f"  vel={[round(v,1) for v in ci.get('vel',[0,0,0])]}   ", end="", flush=True)
                except Exception: pass
            if args.oracle and n % 3 == 0:
                try: oracle_cache = E.oracle_stats()   # crash-proof incremental snapshot
                except Exception: pass
            time.sleep(0.6)
        print()
        if args.oracle:
            try:
                import json
                try: raw = E.oracle_stats()
                except Exception:
                    raw = oracle_cache          # game/script died — use last snapshot
                    print("  (oracle: live fetch failed, using last incremental snapshot)")
                st = json.loads(raw)
                out = ROOT / "log" / f"rules_oracle_rule{args.rule}.json"
                out.parent.mkdir(exist_ok=True)
                st["run"] = {"track": args.track, "mode": args.mode, "cars": args.cars,
                             "rule": args.rule, "hold": args.hold, "pid": pid,
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
        try: sess.detach()
        except Exception: pass
        try:
            if (not psutil) or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
