# Canonical-scenario C4 harness for the Race Results stat-page getters (triage A1 group R):
#   PlayerBlock2Field00Get 0x00423ee0, Field04Get 0x00423f60, Field50Get 0x00423ff0, Field08Get 0x00424070.
#
# Why a new harness (c4_detect_e_scenario.py's Quick Battle route cannot call these bodies):
#   Call sites (E8 scan of .text): 0x0042457c/c2/0x0042460c/4f in FUN_00424270 (stat sort, called from
#   FUN_0042c960 at 0x0042caa2 / 0x0042cda8), 0x0042c736/46/66/76 in FUN_0042c510 (stat-page advance),
#   0x0042cbce/e1 and 0x0042cf31/3f/61/6f in FUN_0042c960 (Race Results draw-list build, gated on
#   [0x0067ebc8] == 0 at 0x0042c9ca). The getters serve stat types 0xb/0xd/0xa/0xc, which live on pages
#   9/10/12/13 of the page table 0x005f7854. FUN_0042c510 skips pages 7..13 (0x0042c5f3..0x0042c63a)
#   unless game mode [0x0067e9fc] is 6 or 2 (0x0042c5e9 / 0x0042c5ee). Quick Battle is mode 10, so its
#   Race Results never reaches those pages. Mode 6 comes from the Multi Player screen (screen 3,
#   0xff2c0000 Top Dog / 0xff2e0000 Team Play). The old defect (team-id stride 4 instead of 0x10) is
#   only visible when the team gate [0x0067ea64] != 0 (FUN_0042f500), which Team Play sets.
#
# Route (no push, no poke): title -> GTS (screen 1) -> down -> Multi Player -> screen 3 -> down -> Team
#   Play -> colour select -> ... -> race (control-4 pulses while 0x00771968 == 3, the E recipe) ->
#   natural Race Results (TYPE 5) -> action-7 presses (FUN_00497310 override) to walk the stat pages.
#
# Arms (one fresh MASHED per run, own PID only), as c4_detect_e_scenario.py:
#   off  : deployed .asi loaded, MASHED_HOOK_ONLY=__none__ -> stock getters run; probes on orig entries.
#   on   : deployed .asi loaded, MASHED_HOOK_ONLY=<the 4 RVAs> -> E9 live; probes on the E9 targets.
#   stock: MASHED_RE_NO_AUTO_HOOK=1, nothing loaded.
#   ctl  : NO_AUTO_HOOK=1, then Module.load(--ctl-asi) + InjectHooks with HOOK_ONLY=<4 RVAs> at the title.
#          --ctl-asi = deployed .asi gives the 'late' twin.
#
# Per getter call: param, return address, return, and every input the bytes read (0x0067e9fc,
# 0x0067ea64, team dwords 0x007f1a18 + k*0x10 AND + k*4, field dwords 0x00899f20 + k*0x138 + off).
# m_orig = the bytes (0x00423ee0..0x00423f5e and siblings); m_old = the pre-2e3cfead port (stride 4).
# Consumer: FUN_0042c300 (value,row) entries from 0x0042cbfa and FUN_0042c3c0 (ref,own,row,flag) entries
# from 0x0042cf95 are checked against the getter returns that fed them; FUN_00424270's output
# 0x0067eae0[0..3] is recorded; FUN_0042c510 entries by return address; page index 0x0067ea08.
#
# Usage: py -3.12 re/frida/c4_pb2_scenario.py --arm off|on|stock|ctl [--ctl-asi PATH] [--round 300]
#        [--out DIR] [--tag NAME] [--presses 18] [--catch-crash]
# --catch-crash also loads poll_attach_catch_crash.py's exception-handler agent (registers, bytes at
# EIP, 320-dword stack, module table) into the same session; the payload is saved as rec['crash_full'].
import json, os, sys, time, hashlib
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, shoot, _open_handle, _exit_code

EXE = statenav.EXE
ORIG = statenav.ORIG
# C4PB2_GAMEROOT=<dir holding MASHED.exe>: run a private install copy instead of original/ (crash1509b4).
if os.environ.get('C4PB2_GAMEROOT'):
    ORIG = Path(os.environ['C4PB2_GAMEROOT']); EXE = ORIG / "MASHED.exe"
# C4PB2_HOLD_PIZ=1: just before the screen-18 Play Game confirm, open every <root>/TOASTART/TRACKS/*.piz
# read-only from this harness and keep the handles until exit, i.e. what a second MASHED process with
# that track loaded looks like to FUN_004b6710's CreateFileA(.., dwShareMode=0, ..). Use with
# C4PB2_GAMEROOT only, never on the shared original/ (it would break other sessions' track loads).
HOLD_PIZ = os.environ.get('C4PB2_HOLD_PIZ') == '1'
assert not HOLD_PIZ or os.environ.get('C4PB2_GAMEROOT'), "C4PB2_HOLD_PIZ needs C4PB2_GAMEROOT"
# C4PB2_PROBE_PIZ=1: from the Play Game confirm on, try open(..,'rb') + close on every track piz of the copy
# each race-loop tick; a PermissionError means the game holds that file without read sharing. Copy only.
PROBE_PIZ = os.environ.get('C4PB2_PROBE_PIZ') == '1'
assert not PROBE_PIZ or os.environ.get('C4PB2_GAMEROOT'), "C4PB2_PROBE_PIZ needs C4PB2_GAMEROOT"
# C4PB2_BARRIER=<dir>: two harness runs (own PIDs each) meet before the screen-18 Play Game confirm and press
# it at the same wall-clock instant, i.e. two MASHED processes starting the same track load together.
BARRIER = os.environ.get('C4PB2_BARRIER')
def barrier_wait(tag, log):
    d = Path(BARRIER); d.mkdir(parents=True, exist_ok=True); (d / f"ready_{tag}").write_text(str(time.time()))
    go = d / "go"; t0 = time.time()
    while not go.exists():
        if len(list(d.glob("ready_*"))) >= 2 and not go.exists():
            try:
                with open(go, "x") as fh: fh.write(str(time.time() + 2.0))
            except FileExistsError: pass
        if time.time() - t0 > 240: log("  BARRIER timeout"); return None
        time.sleep(0.05)
    for _ in range(50):
        try: tgt = float(go.read_text()); break
        except Exception: time.sleep(0.02)
    tgt += float(os.environ.get('C4PB2_BARRIER_DELAY', '0'))   # per-side offset (desynchronized control)
    while time.time() < tgt: time.sleep(0.002)
    log(f"  BARRIER released at {time.time():.3f} (target {tgt:.3f})"); return tgt
def probe_piz():
    locked = []
    for f in sorted((ORIG / "TOASTART" / "TRACKS").glob("*.piz")):
        try:
            with open(f, "rb"): pass
        except PermissionError:
            locked.append(f.name)
    return locked
RVAS = "0x00423ee0,0x00423f60,0x00423ff0,0x00424070"
PICKS = [(0, 1), (1, 2), (2, 3)]     # profile -> RIGHT presses on screen 4 (colour choice 1/2/3)
TEAMS = [(0, 1), (1, 2), (2, 1)]     # profile -> RIGHT presses on screen 16 (A / B / A)
EXTRA_OPP = int(os.environ.get('C4PB2_EXTRA_OPP', '0'))   # RIGHT presses on screen 18 row S18_ROW
# Screen-18 row to edit (default 3, Extra Opponents). C4PB2_S18_ROW=4 (Vehicle, 0x0067ea94) with
# C4PB2_EXTRA_OPP=1 is explore5's route that AV'd the stock original at race start (crash1509b4).
# Setting C4PB2_S18_ROW runs the cursor leg even with 0 presses (the no-edit control of that route).
S18_ROW = int(os.environ.get('C4PB2_S18_ROW', '3'))
S18_FORCE = 'C4PB2_S18_ROW' in os.environ
# Profiles whose input-device dword 0x007e96fc + p*0x200 is written 0 at the title (crash1509b4
# investigation input): FUN_00497450 (0x0049745f) then reports the profile absent and FUN_004332a0 sets its
# colour choice to -1, which is what explore3..6 recorded for profile 3 with 3 devices attached.
DEV_ZERO = [int(x) for x in os.environ.get('C4PB2_DEV_ZERO', '').split(',') if x]
POKE_F00 = [3, 5, 7, 11]             # forced phase-'poke' values, f00 field (0x00899f20 + k*0x138)
POKE_F04 = [2, 6, 1, 9]              # forced phase-'poke' values, f04 field (0x00899f24 + k*0x138)
RACE_PULSE = [0, 1, 2]               # profiles pulsed with control 4 while 0x00771968 == 3
RACE_HOLD = [int(x) for x in os.environ.get('C4PB2_HOLD', '0').split(',') if x]    # profiles holding accelerate (control 0) while 0x00771968 == 3
RACE_STEER = [int(x) for x in os.environ.get('C4PB2_STEER', '').split(',') if x]  # profiles zig-zagging (controls 9/10)

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4, RVA_SEL=0x0067ed40;
// name, rva, field base, has the [0x0067e9fc]==4 -> 0 gate (0x00423f60/0x00424070 only)
const G = [ ['f00',0x00423ee0,0x00899f20,false], ['f04',0x00423f60,0x00899f24,true],
            ['f50',0x00423ff0,0x00899f70,false], ['f08',0x00424070,0x00899f28,true] ];
let navCtrl=-1, navUntil=0;
const PP = {};                       // 'p:c' -> until (per-player control override)
const ST = {calls:{}, byRa:{}, mmOrig:{}, mmOld:{}, disc:{}, entry:{}, jmp:{}, c510:0, c510by:{},
            consOk:0, consBad:0, consBy:{}, consBadEv:[], sort:[], dupKeys:0};
const EV = [];
let lastG = [];                      // getter returns since the last consumer draw call
function abs(r){ return ptr((r+DELTA)>>>0); }
function u8(a){ return abs(a).readU8(); }
function s32(a){ return abs(a).readS32(); }
function snap(base){
  const s = {mode:s32(0x0067e9fc), tg:s32(0x0067ea64), t16:[], t4:[], f:[]};
  for (let k=0;k<4;k++){ s.t16.push(s32(0x007f1a18+k*0x10)); s.t4.push(s32(0x007f1a18+k*4));
    s.f.push(s32(base+k*0x138)); }
  return s;
}
function model(s, p, gate4, teams){
  if (gate4 && s.mode===4) return 0;
  if (s.tg===0) return s.f[p];
  const my = teams[p]; let v = s.f[p];
  for (let k=0;k<4;k++) if (teams[k]===my && k!==p) v = (v + s.f[k])|0;
  return v|0;
}
function attachG(g, entry){
  const nm=g[0];
  ST.calls[nm]=0; ST.mmOrig[nm]=0; ST.mmOld[nm]=0; ST.disc[nm]=0; ST.byRa[nm]={};
  Interceptor.attach(entry, {
    onEnter(a){ this.p = this.context.esp.add(4).readS32();
      this.ra = ((this.returnAddress.toUInt32()-DELTA)>>>0);
      try { this.s = (this.p>=0 && this.p<4) ? snap(g[2]) : null; } catch(e){ this.s=null; } },
    onLeave(r){ const ret=r.toInt32(); ST.calls[nm]++;
      const rk=this.ra.toString(16); ST.byRa[nm][rk]=(ST.byRa[nm][rk]||0)+1;
      lastG.push({g:nm, ra:this.ra, ret:ret});
      if (lastG.length>16) lastG.shift();
      const s=this.s; if (!s) { if (EV.length<6000) EV.push({t:Date.now(), g:nm, p:this.p, ra:rk, ret:ret, s:null}); return; }
      const mo=model(s,this.p,g[3],s.t16), mold=model(s,this.p,g[3],s.t4);
      if (mo!==ret) ST.mmOrig[nm]++; if (mold!==ret) ST.mmOld[nm]++; if (mo!==mold) ST.disc[nm]++;
      if (EV.length<6000) EV.push({t:Date.now(), g:nm, p:this.p, ra:rk, ret:ret, mo:mo, mold:mold,
        page:s32(0x0067ea08), s:s});
    }});
}
function consumerCheck(kind, args){
  // FUN_0042c300(value,row) from 0x0042cbfa: value = the last getter return (0x0042cbd3 / 0x0042cbe6).
  // FUN_0042c3c0(ref,own,row,flag) from 0x0042cf95: ref/own = the two getter calls 0x0042cf31/3f (or 61/6f).
  let ok=null, exp=null, gn=null;
  if (kind==='c300'){
    const l=lastG.length? lastG[lastG.length-1] : null;
    if (l && (l.ra===0x0042cbd3 || l.ra===0x0042cbe6)){ exp=[l.ret]; ok = (args[0]===l.ret); gn=l.g; }
  } else {
    if (lastG.length>=2){ const a=lastG[lastG.length-2], b=lastG[lastG.length-1];
      if ((a.ra===0x0042cf36 && b.ra===0x0042cf44) || (a.ra===0x0042cf66 && b.ra===0x0042cf74)){
        exp=[a.ret,b.ret]; ok = (args[0]===a.ret && args[1]===b.ret); gn=b.g; } }
  }
  lastG = [];
  if (ok===null) return;
  ST.consBy[gn] = ST.consBy[gn] || {ok:0, bad:0};
  if (ok) { ST.consOk++; ST.consBy[gn].ok++; } else { ST.consBad++; ST.consBy[gn].bad++; if (ST.consBadEv.length<50) ST.consBadEv.push({kind:kind, g:gn, args:args, exp:exp}); }
  if (EV.length<6000) EV.push({t:Date.now(), cons:kind, g:gn, args:args, exp:exp, ok:ok, page:s32(0x0067ea08)});
}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ const now=Date.now();
        if (this.p===0 && this.c===navCtrl && now<navUntil){ ret.replace(ptr(0xff)); return; }
        const u=PP[this.p+':'+this.c]; if (u!==undefined && now<u) ret.replace(ptr(0xff)); }});
    Interceptor.attach(abs(0x0042c510),{ onEnter(){ ST.c510++;
      const ra=((this.returnAddress.toUInt32()-DELTA)>>>0).toString(16); ST.c510by[ra]=(ST.c510by[ra]||0)+1;
      if (EV.length<6000) EV.push({t:Date.now(), c510:ra, page:s32(0x0067ea08)}); }});
    Interceptor.attach(abs(0x0042c300),{ onEnter(){ const sp=this.context.esp;
      const ra=((this.returnAddress.toUInt32()-DELTA)>>>0);
      if (ra===0x0042cbff) consumerCheck('c300',[sp.add(4).readS32(), sp.add(8).readS32()]); }});
    Interceptor.attach(abs(0x0042c3c0),{ onEnter(){ const sp=this.context.esp;
      const ra=((this.returnAddress.toUInt32()-DELTA)>>>0);
      if (ra===0x0042cf9a) consumerCheck('c3c0',[sp.add(4).readS32(), sp.add(8).readS32(), sp.add(12).readS32(), sp.add(16).readS32()]); }});
    Interceptor.attach(abs(0x00424270),{ onEnter(){ const sp=this.context.esp; this.o=sp.add(4).readU32(); this.ty=sp.add(8).readS32(); this.fl=sp.add(12).readS32(); },
      onLeave(){ const o=[]; for (let i=0;i<4;i++) o.push(s32(0x0067eae0+i*4));
        if (ST.sort.length<400) ST.sort.push({t:Date.now(), ty:this.ty, fl:this.fl, out:o, page:s32(0x0067ea08)}); }});
    return DELTA; },
  // mode 'off': probe the original entries. 'on': every getter must carry E9; probe the targets.
  armg:function(mode){
    const res={};
    for (const g of G){ const a=abs(g[1]); const b=a.readU8(); ST.jmp[g[0]]=b;
      if (mode==='off'){ if (b===0xe9) return {ok:false, why:g[0]+' E9 present on off arm'}; res[g[0]]={b0:b}; continue; }
      if (b!==0xe9) return {ok:false, why:g[0]+' no E9', b0:b};
      const tgt=a.add(5).add(a.add(1).readS32()); const mm=Process.findModuleByAddress(tgt);
      res[g[0]]={b0:b, tgt:tgt.toString(), mod: mm?(mm.name+'+0x'+tgt.sub(mm.base).toUInt32().toString(16)):null, path: mm?mm.path:null}; }
    for (const g of G){ const a=abs(g[1]);
      const e = (mode==='off') ? a : a.add(5).add(a.add(1).readS32());
      ST.entry[g[0]]=e.toString(); attachG(g, e); }
    return {ok:true, g:res}; },
  loadctl:function(path){ const m=Module.load(path); const f=m.findExportByName('InjectHooks');
    if (!f) return {ok:false, why:'no InjectHooks'}; new NativeFunction(f,'void',[])();
    return {ok:true, base:m.base.toString(), name:m.name, path:m.path}; },
  modinfo:function(){ const r={}; for (const g of G){ const a=abs(g[1]); const b=a.readU8();
      const tgt = b===0xe9 ? a.add(5).add(a.add(1).readS32()) : null; const mm = tgt ? Process.findModuleByAddress(tgt) : null;
      r[g[0]]={b0:b, dword:a.readU32().toString(16), tgt:tgt?tgt.toString():null, mod: mm?(mm.name+'+0x'+tgt.sub(mm.base).toUInt32().toString(16)):null}; }
    return r; },
  press:function(c,ms){ navCtrl=c; navUntil=Date.now()+ms; return 1; },
  pressp:function(players,c,ms){ const u=Date.now()+ms; players.forEach(function(p){ PP[p+':'+c]=u; }); return 1; },
  state:function(){ let d=-999,ph=-999,gate=-999,page=-999,mode=-999,tg=-999,sph=-999,ebc8=-999,sel=-999;
    try{ d=s32(RVA_DEPTH); ph=s32(RVA_PHASE); gate=s32(0x0067ed3c+d*0x40); page=s32(0x0067ea08); sel=s32(RVA_SEL+d*0x40);
         mode=s32(0x0067e9fc); tg=s32(0x0067ea64); ebc8=s32(0x0067ebc8); sph=u8(0x00771968);}catch(e){}
    let ui=-999, fc=-999; try{ ui=s32(0x0063ba8c); fc=s32(0x007f0ff4); }catch(e){}
    const c={}; let tot=0; for (const g of G){ c[g[0]]=ST.calls[g[0]]||0; tot+=c[g[0]]; }
    return {depth:d, phase:ph, gate:gate, sel:sel, page:page, mode:mode, tg:tg, ebc8:ebc8, sph:sph, ui:ui, fc:fc, calls:c, tot:tot,
            c510:ST.c510, consOk:ST.consOk, consBad:ST.consBad}; },
  inputs:function(){ const r={mode:s32(0x0067e9fc), tg:s32(0x0067ea64), rec:[], f:{}};
    for (let i=0;i<16;i++) r.rec.push(s32(0x007f1a14+i*4));
    for (const g of G){ const v=[]; for (let k=0;k<4;k++) v.push(s32(g[2]+k*0x138)); r.f[g[0]]=v; }
    return r; },
  dump:function(){ return {st:ST, ev:EV}; },
  rd:function(a){ try{ return s32(a); }catch(e){ return -999; } },
  // Close a press phase: archive the per-getter counters under ST.phases[name] and zero them.
  closephase:function(name){ ST.phases = ST.phases || {};
    ST.phases[name] = JSON.parse(JSON.stringify({calls:ST.calls, byRa:ST.byRa, mmOrig:ST.mmOrig, mmOld:ST.mmOld, disc:ST.disc,
      consBy:ST.consBy, consOk:ST.consOk, consBad:ST.consBad, c510:ST.c510, c510by:ST.c510by}));
    for (const g of G){ const n=g[0]; ST.calls[n]=0; ST.byRa[n]={}; ST.mmOrig[n]=0; ST.mmOld[n]=0; ST.disc[n]=0; }
    ST.consBy={}; ST.consOk=0; ST.consBad=0; ST.c510=0; ST.c510by={}; return ST.phases[name]; },
  // Forced state for phase 'poke': write the given dwords (block-2 fields only), return the previous values.
  poke:function(pairs){ const old=[]; for (const pr of pairs){ old.push([pr[0], s32(pr[0])]); abs(pr[0]).writeS32(pr[1]); } return old; },
  depth:function(){ try{return s32(RVA_DEPTH);}catch(e){return -999;} },
  phase:function(){ try{return s32(RVA_PHASE);}catch(e){return -999;} },
  sel:function(){ try{const d=s32(RVA_DEPTH); return s32(RVA_SEL+d*0x40);}catch(e){return -999;} }
};
Process.setExceptionHandler(function (d) {
  if (d.type !== 'access-violation') return false;
  try { const pc=d.context.pc; const m=Process.findModuleByAddress(pc);
    // Deep stack scan (crash1509b4): FUN_0047b8d0 keeps a 0x8000-byte buffer on the stack, so the 320-dword
    // walk of poll_attach_catch_crash.py stops inside it. Scan 0x10000 bytes for .text return candidates.
    const deep=[]; try { const sp=d.context.esp; for (let o=0;o<0x10000;o+=4){ let v; try{ v=sp.add(o).readU32(); }catch(e){ break; }
      if (v>=0x401000 && v<0x5cc000) deep.push([o, v.toString(16)]); } } catch(e){}
    send({kind:'crash', deep:deep, tid:Process.getCurrentThreadId(), threads:Process.enumerateThreads().map(function(t){return t.id;}), pc:pc.toString(), module: m ? (m.name+'+0x'+pc.sub(m.base).toUInt32().toString(16)) : 'UNMAPPED',
          addr: d.memory ? (d.memory.operation+' @ '+d.memory.address) : null}); } catch(e){}
  return false;
});
'''


def sha256(p):
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""): h.update(b)
    return h.hexdigest().upper()


def main():
    av = sys.argv
    def opt(f, d): return av[av.index(f) + 1] if f in av else d
    arm = opt("--arm", "off"); assert arm in ("off", "on", "ctl", "stock")
    ctl_asi = opt("--ctl-asi", None)
    round_secs = int(opt("--round", "300"))
    n_press = int(opt("--presses", "18"))
    out = Path(opt("--out", str(ROOT / "log" / "c4_pb2")))
    tag = opt("--tag", arm)
    out.mkdir(parents=True, exist_ok=True)
    shotdir = out / f"c4_pb2_shots_{tag}"; shotdir.mkdir(exist_ok=True)

    env = dict(os.environ)
    if arm == "off":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = "__none__"
    elif arm == "on":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = RVAS
    elif arm == "stock":
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    else:
        assert ctl_asi and Path(ctl_asi).exists(), "--ctl-asi required"
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = RVAS
    rec = {"arm": arm, "tag": tag, "exe_sha256": sha256(EXE),
           "deployed_asi": str(ORIG / "mashed_re_dev.asi"),
           "deployed_asi_sha256": sha256(ORIG / "mashed_re_dev.asi"),
           "ctl_asi": ctl_asi if arm == "ctl" else None, "ctl_asi_sha256": sha256(ctl_asi) if arm == "ctl" else None,
           "env": {k: env.get(k) for k in ("MASHED_HOOK_ONLY", "MASHED_RE_NO_AUTO_HOOK")},
           "recipe": {"picks": PICKS, "teams": TEAMS, "pulse": RACE_PULSE, "hold": RACE_HOLD, "steer": RACE_STEER, "extra_opp": EXTRA_OPP,
                      "s18_row": S18_ROW, "s18_force": S18_FORCE, "dev_zero": DEV_ZERO,
                      "gameroot": str(ORIG), "hold_piz": HOLD_PIZ},
           "log": [], "nav": [], "presses": [], "crash": [], "crash_full": []}
    def log(msg):
        print(msg, flush=True); rec["log"].append(msg)

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    rec["pid"] = pid; log(f"  spawned pid={pid} arm={arm}")
    sess = dev.attach(pid); hproc = _open_handle(pid)
    def on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash":
            rec["crash"].append(p); print("  [CRASH]", p, flush=True)
        elif m.get("type") == "error":
            print("  agent err:", m.get("description"), flush=True)
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    if "--catch-crash" in av:
        import poll_attach_catch_crash as pacc
        def on_full(m, d):
            p = m.get("payload") if isinstance(m, dict) else None
            if isinstance(p, dict) and p.get("kind") == "crash":
                rec["crash_full"].append(p)
                print("  [CRASH-FULL]", {k: p.get(k) for k in ("type", "eip", "mem_op", "mem_address", "eax", "ebx",
                      "ecx", "edx", "esi", "edi", "ebp", "esp", "bytes_at_eip")}, flush=True)
        scr2 = sess.create_script(pacc.AGENT_JS); scr2.on("message", on_full); scr2.load()
    X = scr.exports_sync
    X.init()
    dev.resume(pid)
    nav = Nav(scr, pid)
    verdict = "INCOMPLETE"
    def navlog(what):
        s = X.state(); rec["nav"].append({"what": what, **s})
        log(f"  {what}: depth={s['depth']} type={s['gate']} sel={s['sel']} phase={s['phase']} mode={s['mode']} tg={s['tg']} sph={s['sph']}")
        return s
    try:
        log("  booting...")
        nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 25.0, "title up")
        navlog("title")
        rec["devices_at_title"] = [X.rd(0x007e96fc + p * 0x200) for p in range(4)]
        log(f"  devices 0x007e96fc+p*0x200 at title: {rec['devices_at_title']}")
        if DEV_ZERO:
            X.poke([[0x007e96fc + p * 0x200, 0] for p in DEV_ZERO])
            rec["devices_after_zero"] = [X.rd(0x007e96fc + p * 0x200) for p in range(4)]
            log(f"  DEV_ZERO {DEV_ZERO}: devices now {rec['devices_after_zero']}")
        if arm == "ctl":
            r = X.loadctl(str(Path(ctl_asi).resolve())); log(f"  ctl load: {r}")
        if arm in ("off", "stock"):
            r = X.armg("off")
        else:
            r = None
            for _ in range(30):
                r = X.armg("on")
                if r.get("ok"): break
                time.sleep(0.5)
        rec["arm_result"] = r; rec["modinfo_at_arm"] = X.modinfo(); log(f"  armg: {r}")
        if not r or not r.get("ok"):
            verdict = "ARM-FAILED"; raise SystemExit
        # --- Multi Player -> Team Play
        nav.confirm_to_depth(2); navlog("GTS")
        # The async Load-Successful modal (statenav.advance_past_load_modal) swallows input while up.
        # A down press that leaves the cursor at 0 means the modal is up: confirm closes it, then retry.
        time.sleep(1.5)
        for k in range(8):
            if X.sel() == 1: break
            nav.press(12); time.sleep(0.5)
            s = navlog(f"GTS down {k}")
            if s["sel"] == 1: break
            nav.press(4); time.sleep(1.5)
            s = navlog(f"GTS modal-confirm {k}")
            if s["depth"] >= 3:
                log("  !! confirm entered depth 3 from cursor 0 (no modal)"); verdict = "NAV-MP-FAILED"; raise SystemExit
        if X.sel() != 1:
            verdict = "NAV-MP-FAILED"; raise SystemExit
        nav.advance_past_load_modal(3); s = navlog("MP screen")
        if s["gate"] != 3:
            log("  !! depth-3 screen is not type 3 (Multi Player)"); verdict = "NAV-MP-FAILED"; raise SystemExit
        nav.press(12); navlog("MP down (Team Play)")
        nav.confirm_to_depth(4, tries=4); s = navlog("after Team Play confirm")
        if s["gate"] != 4 or s["tg"] == 0:
            verdict = "NAV-COLOUR-FAILED"; raise SystemExit
        # --- screen 4 (Player Colour Select) in MP: the confirm arm 0xff1d0000 (0x0043e557) runs
        # FUN_0042b9e0, which needs >= 2 profiles with a colour choice >= 1 and no duplicates in
        # 0x0067eaf0 + 12*p. FUN_004332a0 moves each profile's choice on its own column 1 (RIGHT:
        # 0x00433306 / 0x00433315), cooked from control 10 (0x00496655..0x00496664). Profiles 0/1/2
        # pick 1/2/3 with 1/2/3 RIGHT presses.
        def tap(p, c, n):
            for _ in range(n):
                X.pressp([p], c, 200); time.sleep(0.6)
        time.sleep(1.0)
        for p, n in PICKS:
            tap(p, 10, n)
        rec["colour_choice"] = [X.rd(0x0067eaf0 + 12 * p) for p in range(4)]
        log(f"  colour choices 0x0067eaf0+12p: {rec['colour_choice']}")
        nav.press(4); time.sleep(1.5); s = navlog("colour confirm")
        if s["gate"] != 16:
            nav.press(4); time.sleep(1.5); s = navlog("colour confirm 2")
        if s["gate"] != 16:
            verdict = "NAV-TEAMSEL-FAILED"; raise SystemExit
        # --- screen 16 (Team Select): MenuTeamSelectTick 0x0042fa00 steps 0x0067e938 + 12*p
        # (0 none, 1 team A, 2 team B) on column 1 (control 10). MenuTeamBalance 0x0042bb60 derives
        # 0x007f1a18 + n*0x10 = pick - 1 and accepts 1v1 / 2v1 / 1v2 splits.
        time.sleep(1.0)
        for p, n in TEAMS:
            tap(p, 10, n)
        rec["team_pick"] = [X.rd(0x0067e938 + 12 * p) for p in range(4)]
        log(f"  team picks 0x0067e938+12p: {rec['team_pick']}")
        shoot(pid, shotdir / "after_teamplay.png")
        # --- descend: confirm with player 0 until the frontend is left. If a screen does not advance on
        # player 0's confirm, also confirm for players 1..2 (join) and record that it was needed.
        last = (s["depth"], s["gate"]); stuck = 0; did18 = False
        def blk(): return [X.rd(0x0067ea60 + 4 * i) for i in range(22)]
        for k in range(24):
            if nav.phase() != 3: break
            if s["gate"] == 18 and not did18 and (EXTRA_OPP > 0 or S18_FORCE):
                # Screen 18 (game setup, rows 0x150 Play Game, 0x56, 0x24c Game Length, 0x103 Extra
                # Opponents, ...; frontend_config_screens_REmap_20260614.md). A human-only Team Play
                # match never ended in 420/480 s (explore3/4): no AI car, so no elimination. Raise
                # Extra Opponents with player 0's RIGHT (control 10) and record which dword moved.
                did18 = True; b0 = blk()
                # Closed loop on the cursor: three blind downs landed on row 4 (Vehicle, 0x0067ea94),
                # whose edit then AV'd the stock original at race start (explore5).
                for _ in range(8):
                    if X.sel() == S18_ROW: break
                    nav.press(12 if X.sel() < S18_ROW else 11); time.sleep(0.3)
                s = navlog(f"s18 cursor -> row {S18_ROW}")
                if s["sel"] != S18_ROW:
                    verdict = "NAV-S18-FAILED"; raise SystemExit
                for _ in range(EXTRA_OPP): X.pressp([0], 10, 200); time.sleep(0.6)
                b1 = blk()
                rec["s18_extra_opp"] = {hex(0x0067ea60 + 4 * i): [b0[i], b1[i]] for i in range(22) if b0[i] != b1[i]}
                rec["s18_block_after"] = {hex(0x0067ea60 + 4 * i): b1[i] for i in range(22)}
                log(f"  s18 row {S18_ROW} edit x{EXTRA_OPP}, changed dwords: {rec['s18_extra_opp']}")
                for _ in range(8):
                    if X.sel() == 0: break
                    nav.press(11); time.sleep(0.3)
                s = navlog("s18 cursor -> Play Game")
                if BARRIER:
                    rec["barrier_t"] = barrier_wait(tag, log)
                if HOLD_PIZ:
                    held = [open(f, "rb") for f in sorted((ORIG / "TOASTART" / "TRACKS").glob("*.piz"))]
                    rec["held_piz"] = [h.name for h in held]
                    log(f"  HOLD_PIZ: holding {len(held)} track piz open read-only")
            nav.press(4)
            if PROBE_PIZ:
                t_c = time.time(); prev = None
                while time.time() - t_c < 1.2:
                    lk = probe_piz()
                    if lk != prev:
                        rec.setdefault("piz_locks", []).append([round(time.time() - t_c, 3), lk]); log(f"  piz locked t+{time.time()-t_c:.2f}s after confirm {k}: {lk}"); prev = lk
                    time.sleep(0.02)
            else:
                time.sleep(1.2)
            s = navlog(f"confirm {k}")
            cur = (s["depth"], s["gate"])
            if cur == last:
                stuck += 1
                if stuck >= 2:
                    X.pressp([1, 2], 4, 200); time.sleep(1.2); s = navlog(f"  join players 1,2 at {cur}")
                    rec.setdefault("joins", []).append(list(cur))
            else:
                stuck = 0
                shoot(pid, shotdir / f"nav_{k}_d{s['depth']}_t{s['gate']}.png")
            last = (s["depth"], s["gate"])
            if s["phase"] == 0: break
        shoot(pid, shotdir / "race_enter.png")
        # --- race: E recipe (control 4 pulses for player 0 while the session phase byte is 3)
        end = time.time() + round_secs; t_log = 0; reached = None; t_pulse = 0; n_pulse = 0
        while time.time() < end:
            if not nav.alive(): log("   process exited during race"); break
            s = X.state()
            if PROBE_PIZ:
                lk = probe_piz(); pv = rec.get("piz_locks", [[0, None]])[-1][1]
                if lk != pv:
                    rec.setdefault("piz_locks", []).append([round(time.time() - (end - round_secs), 3), lk]); log(f"  piz locked race t+{time.time()-(end-round_secs):.2f}s: {lk}")
            # All three human profiles (0..2, the ones that picked a colour) get the pulse: with only
            # player 0 pulsed the match sat 420 s in session phase 3 with no round end (explore3).
            if s["sph"] == 3 and time.time() - t_pulse > 1.0:
                X.pressp(RACE_PULSE, 4, 200); t_pulse = time.time(); n_pulse += 1
                # Hold accelerate (control 0 -> 0x007f103c, Ai/AiController.cpp:170) for profile 0 only.
                # The default round-over test (0x00410ee3..) ends a round on car separation, and a
                # human-only match has no AI to separate the pack (explore3/4 sat 420/480 s in state 6).
                X.pressp(RACE_HOLD, 0, 1300)
                # Steer (controls 9/10 -> 0x007f1038/39, 0x00496648/0x00496664) on a 6 s cycle, offset
                # per profile, so cars meet: f00 ("aggressive") and f04 ("kills") stayed 0 for every
                # player when car 0 only drove straight (explore7), which makes those two getters
                # non-discriminating.
                ph = time.time() % 6.0
                for j, pp in enumerate(RACE_STEER):
                    q = (ph + 3.0 * j) % 6.0
                    if q < 1.5: X.pressp([pp], 9, 900)
                    elif 3.0 <= q < 4.5: X.pressp([pp], 10, 900)
            if time.time() - t_log > 10:
                t_log = time.time(); log(f"   wait t+{int(round_secs-(end-time.time()))}s pulses={n_pulse} {s}")
                if psutil:
                    try:
                        pz = sorted(f.path for f in psutil.Process(pid).open_files() if f.path.lower().endswith(".piz"))
                        rec.setdefault("open_piz", []).append(pz); log(f"   open .piz in pid {pid}: {pz}")
                    except Exception as e: log(f"   open_files failed: {e}")
            if s["phase"] == 3 and s["gate"] == 5 and s["sph"] != 3:
                reached = s; break
            time.sleep(0.25)
        rec["reached"] = reached; rec["race_pulses"] = n_pulse
        if not reached:
            verdict = "SCREEN5-NOT-REACHED"; shoot(pid, shotdir / "timeout.png"); raise SystemExit
        log(f"  >>> Race Results reached: {reached}")
        time.sleep(2.0)
        rec["inputs_results"] = X.inputs(); log(f"  inputs at results: {rec['inputs_results']}")
        base = X.state(); rec["baseline"] = base
        time.sleep(3.0); b2 = X.state(); rec["baseline_after_3s"] = b2
        log(f"  idle 3s: calls {base['calls']}->{b2['calls']} page {base['page']}->{b2['page']} c510 {base['c510']}->{b2['c510']}")
        # --- action-7 presses for player 0 (the E harness showed pad 0 is the eligible one): walk the pages.
        # Phase 'nat': natural data. Phase 'poke': FORCED STATE, distinct non-zero values written into the
        # f00 / f04 block-2 fields (0x00899f20 / 0x00899f24 + k*0x138) only. Those two stats ("aggressive",
        # "kills") came out 0 for every player in every natural run, which makes their getters unable to
        # tell the fixed stride from the old one. The poke changes the data the getter reads, not the code
        # path: the game still calls the getter from its own call sites on its own screen.
        half = n_press // 2
        for i in range(n_press):
            if i == half:
                rec["phase_nat"] = X.closephase("nat"); rec["poke_t_ms"] = int(time.time() * 1000)
                pk = [[0x00899f20 + k * 0x138, POKE_F00[k]] for k in range(4)] +                      [[0x00899f24 + k * 0x138, POKE_F04[k]] for k in range(4)]
                rec["poke_prev"] = X.poke(pk); rec["poke"] = pk
                log(f"  POKE f00/f04 (previous values {rec['poke_prev']})")
            if not nav.alive(): log("   process exited during presses"); break
            s0 = X.state()
            X.pressp([0], 7, 200); time.sleep(2.5)
            s1 = X.state()
            pr = {"i": i, "page0": s0["page"], "page1": s1["page"], "gate": s1["gate"], "c510": s1["c510"] - s0["c510"],
                  "calls": {k: s1["calls"][k] - s0["calls"][k] for k in s1["calls"]},
                  "consOk": s1["consOk"] - s0["consOk"], "consBad": s1["consBad"] - s0["consBad"]}
            rec["presses"].append(pr); log(f"  press7 {i}: {pr}")
        rec["final"] = X.state(); rec["modinfo_final"] = X.modinfo()
        rec["dump"] = X.dump()
        verdict = "DONE"
    except SystemExit:
        pass
    finally:
        try: rec.setdefault("final", X.state())
        except Exception: pass
        try: rec.setdefault("dump", X.dump())
        except Exception: pass
        rec["alive_at_end"] = nav.alive()
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        rec["exit_code"] = _exit_code(hproc)
        rec["verdict"] = verdict
        p = out / f"c4_pb2_{tag}.json"
        p.write_text(json.dumps(rec, indent=1), encoding="utf-8")
        print(f"  verdict={verdict} -> {p}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
