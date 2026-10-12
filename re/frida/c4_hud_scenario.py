# Canonical-scenario C4 harness for the group-H in-race HUD rows (triage A1).
#
#   --row 403160  Sub00403160_SubMode0BViewport. Only call site 0x0040e0c4 in HudIngameDispatch
#                 0x0040dfc0, reached when DAT_0063ba8c in {5,6} (0x0040dfce..0x0040dfd6) and
#                 DAT_0067e9fc == 0xb (jump table 0x0040e12c, index byte 0x0040e140[9] = 3).
#                 The only writer of 0xb is FUN_0043df00 @0x0043df19, called from 0x0040312c when
#                 the title idle timer [0x00636ae8] passes [0x00636aec] (0x004030ff). So the
#                 scenario is: boot, touch nothing, the game starts its own attract demo race.
#   --row 41db80  Sub0041db80_HudThresholdDispatch. Only call site 0x0040e0ee (jmp) in the same
#                 dispatcher, reached in DAT_0063ba8c in {5,6} for every mode except 0xb
#                 (0x0040e0e5). Scenario: statenav Quick Battle navigation, then the race start.
#   --row 40d440  Course::LoadCurrent (c4race 2026-10-12). Once per course load, from 0x00492a67.
#   --row 4464c0  CameraEntry::DispatchAll. ~60/s in a race plus the 100-call FUN_00448700 loop.
#                 No Interceptor on the game thread (native counter stub + JS-thread poll).
#   --row 41c2d0  FUN_0041c2d0. Rule 5/10 only (c4race_RESULT.md); per call the member dispatch
#                 order and the return (c4save 2026-10-12), plus the feasibility counters.
#   --route attract|qb|cup selects the scenario for these rows. The attract route installs no input
#   hook (row 403160 also runs without it now; it never sent input). The cup route needs
#   --root <install copy>\original (never original\) and takes --reset-save <edited gamesave.bin>.
#
# Arms (one fresh MASHED per run, own PID only), as c4_detect_e_scenario.py:
#   off   deployed .asi, MASHED_HOOK_ONLY=__none__            -> stock body, instrumented at its entry
#   on    deployed .asi, MASHED_HOOK_ONLY=<rva>               -> E9 live, instrumented at the E9 target
#   stock MASHED_RE_NO_AUTO_HOOK=1, nothing loaded             -> setup-matched reference for late/ctl
#   late  NO_AUTO_HOOK=1, Module.load(--ctl-asi) + InjectHooks at the title (deployed .asi = late)
#   ctl   as late, with the old-defect control .asi
#
# 403160 observable, per call: the return (EAX), and every call it makes to FUN_004c1c80 (view
# window, (cam, *vp) read at entry), FUN_004c1a00 (return), FUN_00402fb0 (entry), FUN_00428760
# (args), recorded only while the body is executing (flag set by the body's own entry hook).
# Plus cam+0x68/+0x6c read after the body returns.
# 41db80 observable, per call: DAT_0063d588 (the counter at record +0x2c) at entry and exit, and
# the ordered list of record slots (+0x00..+0x14 of 0x0063d55c) whose obj+0x48 function the body
# called. The +0x48 pointers of the six objects are swapped to a recorder for the duration of each
# call only and restored on leave (the recorder calls the saved function with the same argument).
#
# Usage: py -3.12 re/frida/c4_hud_scenario.py --row 403160|41db80 --arm off|on|stock|late|ctl
#        [--ctl-asi PATH] [--out DIR] [--tag NAME] [--secs N]
import json, os, sys, time, hashlib
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, _open_handle, _exit_code

EXE = statenav.EXE
ORIG = statenav.ORIG

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0; let navCtrl=-1, navUntil=0;
function abs(r){ return ptr((r+DELTA)>>>0); }
function s32(a){ return abs(a).readS32(); } function u32(a){ return abs(a).readU32(); }
function f32(a){ return abs(a).readFloat(); }
function fh(p){ return p.readU32().toString(16); }
const ST = {enter:0, tx:0, calls:0, entry:null, jmp:-1, keys:{}, ev:[], mmOrig:0, mmOld:0, nodiff:0, cchg:0, buckets:{}};
let IN = null;          // per-call record while the body executes
let LASTCAM = null;
let ROW = '';
function snap(){ let o={}; try{ o={mode:s32(0x0067e9fc), st:s32(0x0063ba8c), rule:s32(0x007f0fd0), sph:abs(0x00771968).readU8(),
  ph:s32(0x0067eca4), d:s32(0x0067e9f8), cnt:f32(0x0063d588), t8:f32(0x00636ae8), f184:s32(0x0067f184), f17c:s32(0x0067f17c),
  ai:s32(0x00897ffc), fcc:s32(0x007f0fcc), y:f32(0x0063b910)}; }catch(e){} return o; }
function addKey(k, extra){ let e=ST.keys[k]; if(!e){ e=ST.keys[k]={n:0}; if(extra) Object.assign(e,extra); } e.n++; }

// ---- row 403160 ----------------------------------------------------------------------
function inner403160(){
  Interceptor.attach(abs(0x004c1c80),{ onEnter(a){ if(!IN) return; const sp=this.context.esp;
      const cam=sp.add(4).readPointer(), vp=sp.add(8).readPointer();
      if(!IN.cam) IN.cam=cam; const v=fh(vp)+','+fh(vp.add(4));
      IN.e.push('V'+(cam.equals(IN.cam)?'':'!')+(IN.pre!==null && v===IN.pre ? 'PRE' : v)); }});
  Interceptor.attach(abs(0x004c1a00),{ onEnter(a){ this.i = !!IN; this.cam = IN ? this.context.esp.add(4).readPointer() : null; },
      onLeave(r){ if(!this.i || !IN) return; IN.e.push('B'+(r.equals(this.cam)?'cam':r.toString())); }});
  Interceptor.attach(abs(0x00402fb0),{ onEnter(){ if(IN) IN.e.push('D'); }});
  Interceptor.attach(abs(0x00428760),{ onEnter(){ if(!IN) return; const sp=this.context.esp; let s='P';
      const f=u32(0x00771964);
      for(let i=1;i<=6;i++){ const x=sp.add(4*i).readU32(); s+=(i>1?',':'')+(i===1 && x===f ? 'F771964' : x.toString(16)); } IN.e.push(s); }});
}
function attach403160(entry){
  // pre-call cam+0x68/+0x6c: the camera FUN_004671a0(0) returned on the previous call (constant
  // in practice; the key carries '!' on any V whose cam differs from the call's first V).
  Interceptor.attach(entry,{ onEnter(){ ST.enter++; let pre=null; if (LASTCAM) { try { pre=fh(LASTCAM.add(0x68))+','+fh(LASTCAM.add(0x6c)); } catch(e){} }
      IN={e:[], cam:null, pre:pre, t:Date.now(), s:snap()}; },
    onLeave(r){ const c=IN; IN=null; ST.calls++; if(!c) return;
      let post=''; try{ post = c.cam ? (fh(c.cam.add(0x68))+','+fh(c.cam.add(0x6c))) : 'nocam'; }catch(e){ post='err'; }
      if (c.pre!==null && post===c.pre) post='PRE'; if (c.pre===null) post='first:'+post;
      if (c.cam) { if (LASTCAM && !c.cam.equals(LASTCAM)) ST.camchg=(ST.camchg||0)+1; LASTCAM=c.cam; }
      const ret = (c.cam && r.equals(c.cam)) ? 'cam' : r.toString();
      const k = 'ret='+ret+'|'+c.e.join(' ')+'|post='+post;
      addKey(k);
      if (ST.ev.length<200 && (ST.calls<=5 || ST.calls%500===0)) ST.ev.push({n:ST.calls, k:k, s:c.s}); }});
}

// ---- row 41db80 ----------------------------------------------------------------------
const THR = {t0:0x005cc35c, t1:0x005cd238, t2:0x005cd234, t3:0x005cd230, t4:0x005ccdf4};
function th(n){ return f32(THR[n]); }
function mOrig(c){   // body 0x0041dab0..0x0041db79, from the bytes (c constant over the call)
  const L=[];
  if (c < th('t0')) { L.push(0); L.push(5); }               // 0x0041dab3..0x0041dad0
  if (c < th('t1')) return L;                                // 0x0041dae1 jnp 0x0041db78
  if (c < th('t2')) { L.push(1); }                           // 0x0041daf5
  else if (c < th('t3')) { L.push(1); L.push(2); }           // 0x0041db11
  else if (c < th('t4')) { L.push(1); L.push(2); L.push(3);} // 0x0041db34
  if (!(c >= th('t4'))) return L;                            // 0x0041db59 test ah,1; jne
  if (c < th('t0')) L.push(4);                               // 0x0041db6c
  return L; }
function mOld(c){    // control model: the 0.34 test inverted, nothing else changed
  const L=[];
  if (c < th('t0')) { L.push(0); L.push(5); }
  if (!(c < th('t1'))) return L;
  if (c < th('t2')) { L.push(1); }
  else if (c < th('t3')) { L.push(1); L.push(2); }
  else if (c < th('t4')) { L.push(1); L.push(2); L.push(3);}
  if (!(c >= th('t4'))) return L;
  if (c < th('t0')) L.push(4);
  return L; }
function bucket(c){ if (c!==c) return 'nan'; if (c<th('t1')) return 'a<0.34'; if (c<th('t2')) return 'b<0.82';
  if (c<th('t3')) return 'c<1.3'; if (c<th('t4')) return 'd<1.86'; if (c<th('t0')) return 'e<4'; return 'f>=4'; }
let REC=null; const SAVED=[]; let objs=[];
function attach41db80(entry){
  REC = new NativeCallback(function(obj){
    let fn=null; for (let i=0;i<SAVED.length;i++) if (SAVED[i].o.equals(obj)) { fn=SAVED[i].f; break; }
    if (IN) { let sl=[]; for (let i=0;i<6;i++) if (objs[i] && objs[i].equals(obj)) sl.push(i); IN.d.push(sl.length===1?sl[0]:('?'+sl.join('/'))); }
    if (!fn) return ptr(0);
    return new NativeFunction(fn,'pointer',['pointer'],'mscdecl')(obj);
  }, 'pointer', ['pointer'], 'mscdecl');
  Interceptor.attach(entry,{ onEnter(){ ST.enter++;
      const base=abs(0x0063d55c); objs=[]; SAVED.length=0;
      for (let i=0;i<6;i++){ const o=base.add(i*4).readPointer(); objs.push(o);
        if (o.isNull()) continue; let dup=false; for (const s of SAVED) if (s.o.equals(o)) dup=true; if (dup) continue;
        const slot=o.add(0x48); SAVED.push({o:o, f:slot.readPointer(), slot:slot}); slot.writePointer(REC); }
      IN={d:[], c0:f32(0x0063d588)}; },
    onLeave(){ for (const s of SAVED) s.slot.writePointer(s.f); SAVED.length=0;
      const c=IN; IN=null; ST.calls++; if(!c) return;
      const c1=f32(0x0063d588); if (c1!==c.c0 && !(c1!==c1 && c.c0!==c.c0)) ST.cchg++;
      const mo=mOrig(c.c0).join(','), mold=mOld(c.c0).join(','), act=c.d.join(',');
      if (mo!==act) ST.mmOrig++; if (mold!==act) ST.mmOld++; if (mo===mold) ST.nodiff++;
      const b=bucket(c.c0); const k=b+'|'+act;
      addKey(k, {mo:mo, mold:mold});
      ST.buckets[b]=(ST.buckets[b]||0)+1;
      if (ST.ev.length<400 && (mo!==act || ST.calls<=3 || ST.calls%100===0)) ST.ev.push({n:ST.calls, c0:c.c0, c1:c1, act:act, mo:mo, mold:mold}); }});
}

// ---- row 4295a0 ----------------------------------------------------------------------
function attach4295a0(entry){
  Interceptor.attach(abs(0x00427e00),{ onEnter(){ ST.tx++; if(!IN) return; const sp=this.context.esp; const a=[];
      for(let i=1;i<=6;i++) a.push(sp.add(4*i).readU32()); this.rec=a; this.i=true; },
    onLeave(r){ if(!this.i || !IN) return; IN.c.push({a:this.rec, r:r.toUInt32()}); }});
  Interceptor.attach(entry,{ onEnter(){ ST.enter++; IN={c:[], y:f32(0x0063b910), s:snap()}; },
    onLeave(r){ const c=IN; IN=null; ST.calls++; if(!c) return;
      const buf=Memory.alloc(4); buf.writeFloat(c.y+3.0); const y3=buf.readU32(); buf.writeFloat(c.y); const y0=buf.readU32();
      const parts=c.c.map(function(x,i){ const a=x.a.slice(); const yy=(i===0?y3:y0);
        return '('+a.map(function(v,j){ return j===1 ? (v===yy ? (i===0?'Y+3':'Y') : ('!'+v.toString(16))) : v.toString(16); }).join(',')+')'; });
      const last=c.c.length ? c.c[c.c.length-1].r : null; const ret=r.toUInt32();
      const k='n='+c.c.length+' '+parts.join(' ')+' ret='+(last!==null && ret===last ? 'LAST' : ret.toString(16));
      addKey(k);
      if (ST.ev.length<200 && (ST.calls<=5 || ST.calls%500===0)) ST.ev.push({n:ST.calls, y:c.y, k:k, raw:c.c, s:c.s}); }});
}

// ---- row 40d440 (Course::LoadCurrent) ------------------------------------------------
// Bytes 0x0040d440..0x0040d461: FUN_0040d270([0x005f2770], [0x0063ba7c]); [0x0063ba78] = [0x0063ba7c].
// Per call: selected/loaded/obj at entry, every FUN_0040d270 call made while the body runs (args),
// [0x0063ba78] after the body, and the body's return address (the game caller).
function attach40d440(entry){
  Interceptor.attach(abs(0x0040d270),{ onEnter(){ ST.tx++; const sp=this.context.esp;
      const a1=sp.add(4).readU32(), a2=sp.add(8).readS32(); if (IN) IN.f.push([a1,a2]); else ST.outside=(ST.outside||0)+1; }});
  Interceptor.attach(entry,{ onEnter(){ ST.enter++;
      IN={f:[], sel:s32(0x0063ba7c), pre:s32(0x0063ba78), obj:u32(0x005f2770), ret:(this.context.esp.readU32()-DELTA)>>>0, s:snap()}; },
    onLeave(){ const c=IN; IN=null; ST.calls++; if(!c) return; const post=s32(0x0063ba78);
      const fs=c.f.map(function(x){ return 'F('+(x[0]===c.obj?'OBJ':x[0].toString(16))+','+(x[1]===c.sel?'SEL':('!'+x[1]))+')'; }).join(' ');
      const k='sel='+c.sel+' pre78='+c.pre+' ret='+c.ret.toString(16)+' '+fs+' post78='+(post===c.sel?'SEL':('!'+post));
      addKey(k); if (ST.ev.length<50) ST.ev.push({n:ST.calls, k:k, obj:c.obj.toString(16), s:c.s}); }});
}

// ---- row 4464c0 (CameraEntry::DispatchAll) -------------------------------------------
// No Interceptor on the game thread: the old defect (a2-camera_RESULT.md) is a 4-byte frame shift
// that changes which uninitialised stack dword the callees copy into the entry's RwMatrix pad slots
// (+0x68/+0x78/+0x88), and an Interceptor frame writes below ESP, i.e. exactly into that garbage.
// Call count: a native stub at the entry (the E9 target on the port arm, 0x004464c0 itself on the
// original arms) that only reads [esp] and increments globals, then runs the relocated first
// instruction (mov eax,[0x00898994]) and jumps back. Observable: a JS-thread poll of the entry
// array (count 0x00898994, type +4, pads classified).
let CNT=null, STK=null, MODS=[], STUB=null;   // keep Memory.alloc results referenced (GC frees them)
function installCounter(tgt){
  const ins=Instruction.parse(tgt); const b=tgt.readByteArray(ins.size); const by=new Uint8Array(b);
  // original: A1 94 89 89 00 (5 bytes). The naked port assembles with a ds: prefix: 3E A1 ... (6 bytes).
  const pre=(ins.size===6 && by[0]===0x3e) ? 1 : 0;
  if (!(ins.size===5+pre && by[pre]===0xa1 && tgt.add(1+pre).readU32()===((0x00898994+DELTA)>>>0)))
    return {ok:false, why:'unexpected first instruction '+ins.toString()+' size '+ins.size};
  CNT=Memory.alloc(64); for (let i=0;i<16;i++) CNT.add(i*4).writeU32(0);
  const stub=Memory.alloc(Process.pageSize); STUB=stub;
  Memory.patchCode(stub, 64, function(code){ const w=new X86Writer(code, {pc:stub});
    w.putBytes([0x8b,0x04,0x24]);                       // mov eax,[esp]
    w.putBytes([0xa3]); w.putBytes(u32b(CNT.add(4)));   // mov [CNT+4],eax   (last return address)
    w.putBytes([0x3d]); w.putBytes(u32b(abs(0x00448710)));  // cmp eax,0x00448710 (FUN_00448700 loop)
    w.putBytes([0x75,0x06]);                            // jne +6
    w.putBytes([0xff,0x05]); w.putBytes(u32b(CNT.add(8)));  // inc [CNT+8]  (seed-loop calls)
    w.putBytes([0xff,0x05]); w.putBytes(u32b(CNT));     // inc [CNT]    (all calls)
    w.putBytes(Array.from(by));                         // relocated mov eax,[0x00898994]
    w.putJmpAddress(tgt.add(ins.size)); w.flush(); });
  Memory.patchCode(tgt, 5, function(code){ const w=new X86Writer(code, {pc:tgt}); w.putJmpAddress(stub); w.flush(); });
  return {ok:true, stub:stub.toString()};
}
function u32b(p){ const v=p.toUInt32(); return [v&0xff,(v>>>8)&0xff,(v>>>16)&0xff,(v>>>24)&0xff]; }
function padClass(v){
  if ((v>>>0) < 0x10000) return 'I'+v;
  const p=ptr(v>>>0);
  if (STK && p.compare(STK.base)>=0 && p.compare(STK.base.add(STK.size))<0) return 'S-'+STK.base.add(STK.size).sub(p).toUInt32().toString(16);
  const W0=abs(0x008964c0), W1=abs(0x00898998);
  if (p.compare(W0)>=0 && p.compare(W1)<0){ const o=p.sub(W0).toUInt32(); return 'E'+Math.floor(o/0xd8)+'+'+(o%0xd8).toString(16); }
  for (const m of MODS) if (p.compare(m.base)>=0 && p.compare(m.base.add(m.size))<0) return m.name+'+'+p.sub(m.base).toUInt32().toString(16);
  const e=(v>>>23)&0xff; if (e>=0x60 && e<=0x9f) return 'F';
  return 'X';
}
let POLL=null;
function startPoll(){
  MODS=Process.enumerateModules().map(function(m){ return {name:m.name, base:m.base, size:m.size}; });
  try { const th=Process.enumerateThreads()[0]; const r=Process.findRangeByAddress(th.context.esp); if (r) STK={base:r.base, size:r.size, tid:th.id}; } catch(e){}
  ST.raw={};
  POLL=setInterval(function(){
    let n; try { n=s32(0x00898994); } catch(e){ return; }
    const c=CNT ? CNT.readU32() : 0; ST.calls=c; ST.seedCalls=CNT?CNT.add(8).readU32():0; ST.lastRet=CNT?((CNT.add(4).readU32()-DELTA)>>>0).toString(16):null;
    if (c===0 || n<=0) { ST.idle=(ST.idle||0)+1; return; }
    const m=Math.min(n,44); const parts=[];
    for (let i=0;i<m;i++){ const e=abs(0x008964c0+i*0xd8); const t=e.add(4).readU32();
      const pv=[e.add(0x68).readU32(), e.add(0x78).readU32(), e.add(0x88).readU32()];
      parts.push(t+':'+pv.map(padClass).join(','));
      for (let j=0;j<3;j++){ const rk='e'+i+'+'+['68','78','88'][j]; const a=ST.raw[rk]||(ST.raw[rk]=[]); const h=pv[j].toString(16); if (a.length<6 && a.indexOf(h)<0) a.push(h); } }
    addKey('n='+n+' '+parts.join(' '));
    ST.polls=(ST.polls||0)+1;
  }, 50);
}

// ---- row 41c2d0 ------------------------------------------------------------------------
// Bytes 0x0041c2d0..0x0041c2f2: block in EAX; call [m+0x48](m) for m = block[3],[1],[0],[2]
// (0x0041c2d3..0x0041c2eb), EAX = the last call's EAX. Per call (c4save 2026-10-12): the block
// (EAX at entry), the return address, the ordered member slots whose +0x48 function was called
// (the four members' +0x48 swapped to a recorder for the call only, as row 41db80), and the
// return vs the last member call's return. Plus the feasibility counters and the
// (mode/rule/state/flags) histogram of c4race.
let REC41=null; const SAV41=[]; let blk41=[];
function attach41c2d0(entry){
  Interceptor.attach(abs(0x0041a3e0),{ onEnter(){ ST.m10=(ST.m10||0)+1; if (u32(0x0063c628)!==0) ST.m10f=(ST.m10f||0)+1; }});
  Interceptor.attach(abs(0x0041c300),{ onEnter(){ ST.m5=(ST.m5||0)+1; if (u32(0x0063cdbc)!==0) ST.m5f=(ST.m5f||0)+1; }});
  REC41 = new NativeCallback(function(obj){
    let fn=null; for (let i=0;i<SAV41.length;i++) if (SAV41[i].o.equals(obj)) { fn=SAV41[i].f; break; }
    let sl=[]; for (let i=0;i<4;i++) if (blk41[i] && blk41[i].equals(obj)) sl.push(i);
    if (IN) IN.d.push(sl.length===1?sl[0]:('?'+sl.join('/')));
    if (!fn) return ptr(0);
    const r=new NativeFunction(fn,'pointer',['pointer'],'mscdecl')(obj); if (IN) IN.last=r.toUInt32(); return r;
  }, 'pointer', ['pointer'], 'mscdecl');
  Interceptor.attach(entry,{ onEnter(){ ST.enter++;
      const b=this.context.eax; blk41=[]; SAV41.length=0;
      for (let i=0;i<4;i++){ const o=b.add(i*4).readPointer(); blk41.push(o);
        if (o.isNull()) continue; let dup=false; for (const s of SAV41) if (s.o.equals(o)) dup=true; if (dup) continue;
        const slot=o.add(0x48); SAV41.push({o:o, f:slot.readPointer(), slot:slot}); slot.writePointer(REC41); }
      IN={d:[], last:null, blk:((b.toUInt32()-DELTA)>>>0).toString(16), ret:(this.returnAddress.toUInt32()-DELTA)>>>0, s:snap()}; },
    onLeave(r){ for (const s of SAV41) s.slot.writePointer(s.f); SAV41.length=0;
      const c=IN; IN=null; ST.calls++; if(!c) return;
      const rv=r.toUInt32();
      const k='blk='+c.blk+' order='+c.d.join(',')+' ret='+(c.last!==null && rv===c.last ? 'LAST' : rv.toString(16))+' rule='+c.s.rule+' st='+c.s.st;
      addKey(k);
      ST.rets=ST.rets||{}; const ra=c.ret.toString(16); ST.rets[ra]=(ST.rets[ra]||0)+1;
      if (ST.ev.length<40 && (ST.calls<=5 || ST.calls%200===0)) ST.ev.push({n:ST.calls, k:k, ret:ra, s:c.s}); }});
  // key = game mode 0x0067e9fc / rule 0x007f0fd0 / state 0x0063ba8c / flags 0x0063c628,0x0063cdbc
  ST.rules={}; setInterval(function(){ try{ const k=s32(0x0067e9fc)+'/'+s32(0x007f0fd0)+'/'+s32(0x0063ba8c)+'/f'+u32(0x0063c628)+','+u32(0x0063cdbc);
    ST.rules[k]=(ST.rules[k]||0)+1; }catch(e){} }, 250);
}

rpc.exports={
  init:function(row, nav){ ROW=row; const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    if (nav) Interceptor.attach(abs(0x00497310),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if (this.p===0 && this.c===navCtrl && Date.now()<navUntil) ret.replace(ptr(0xff)); }});
    return DELTA; },
  arm:function(mode, rva){
    const a=abs(rva); const b=a.readU8(); ST.jmp=b; let tgt=a;
    if (mode==='off'){ if (b===0xe9) return {ok:false, why:'E9 present on off arm'}; }
    else { if (b!==0xe9) return {ok:false, why:'no E9', jmp:b}; tgt=a.add(5).add(a.add(1).readS32()); }
    const mm=Process.findModuleByAddress(tgt);
    ST.entry=tgt.toString()+(mm?(' '+mm.name+'+0x'+tgt.sub(mm.base).toUInt32().toString(16)):'');
    const dword=a.readU32().toString(16); let extra=null;
    if (ROW==='403160') { inner403160(); attach403160(tgt); } else if (ROW==='4295a0') attach4295a0(tgt);
    else if (ROW==='40d440') attach40d440(tgt);
    else if (ROW==='4464c0') { extra=installCounter(tgt); if (!extra.ok) return {ok:false, why:extra.why}; startPoll(); }
    else if (ROW==='41c2d0') attach41c2d0(tgt);
    else attach41db80(tgt);
    return {ok:true, entry:ST.entry, jmp:b, dword:dword, counter:extra}; },
  loadctl:function(path){ const m=Module.load(path); const f=m.findExportByName('InjectHooks');
    if (!f) return {ok:false, why:'no InjectHooks'}; new NativeFunction(f,'void',[])();
    return {ok:true, base:m.base.toString(), name:m.name, path:m.path}; },
  modinfo:function(rva){ const a=abs(rva); const tgt=a.readU8()===0xe9 ? a.add(5).add(a.add(1).readS32()) : null;
    const mm = tgt ? Process.findModuleByAddress(tgt) : null;
    return {b0:a.readU8(), dword:a.readU32().toString(16), tgt: tgt?tgt.toString():null,
            mod: mm?{name:mm.name, path:mm.path, base:mm.base.toString(), off:'0x'+tgt.sub(mm.base).toUInt32().toString(16)}:null}; },
  press:function(c,ms){ navCtrl=c; navUntil=Date.now()+ms; return 1; },
  depth:function(){ try{return s32(0x0067e9f8);}catch(e){return -999;} },
  phase:function(){ try{return s32(0x0067eca4);}catch(e){return -999;} },
  sel:function(){ try{const d=s32(0x0067e9f8); return s32(0x0067ed40+d*0x40);}catch(e){return -999;} },
  state:function(){ const s=snap(); s.enter=ST.enter; s.tx=ST.tx; s.calls=ST.calls; s.mmOrig=ST.mmOrig; s.mmOld=ST.mmOld; s.nkeys=Object.keys(ST.keys).length; return s; },
  dump:function(){ return ST; }
};
Process.setExceptionHandler(function (d) {
  if (d.type !== 'access-violation') return false;
  try { const pc=d.context.pc; const m=Process.findModuleByAddress(pc);
    send({kind:'crash', pc:pc.toString(), module: m ? (m.name+'+0x'+pc.sub(m.base).toUInt32().toString(16)) : 'UNMAPPED',
          addr: d.memory ? (d.memory.operation+' @ '+d.memory.address) : null,
          enter:ST.enter, calls:ST.calls, inBody:!!IN});
    recv('ack', function(){}).wait(); } catch(e){}
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
    row = opt("--row", "403160"); assert row in ("403160", "41db80", "4295a0", "40d440", "4464c0", "41c2d0")
    rva = {"403160": 0x00403160, "41db80": 0x0041db80, "4295a0": 0x004295a0,
           "40d440": 0x0040d440, "4464c0": 0x004464c0, "41c2d0": 0x0041c2d0}[row]
    # --route for the c4race rows: 'attract' = boot and touch nothing (the title idle timer starts the
    # attract demo race, as row 403160), 'qb' = the statenav Quick Battle route of row 41db80, with
    # optional --track-presses c,c,... sent on the track-select screen before the start.
    route = opt("--route", {"403160": "attract", "41db80": "qb", "4295a0": "cup"}.get(row, "attract"))
    assert route in ("attract", "qb", "cup")
    track_presses = [int(x) for x in opt("--track-presses", "").split(",") if x.strip()]
    # The cup route (row 4295a0's, and since c4save 2026-10-12 any row) runs from an install COPY
    # (--root <copy>\original) whose gamesave.bin opens the Silver-cup launch gates; never original\.
    # --reset-save FILE copies FILE over the copy's gamesave.bin before the spawn, so every run
    # starts from the same edited save (the game may write the save during a run).
    exe, cwd = EXE, ORIG
    reset_save = opt("--reset-save", None)
    if route == "cup":
        root = Path(opt("--root", "")); assert (root / "MASHED.exe").exists(), "--root <copy>/original required"
        assert root.resolve() != ORIG.resolve() and "proyectos\\mashed\\original" not in str(root.resolve()).lower(), "refuse: real install"
        exe, cwd = root / "MASHED.exe", root
        if reset_save:
            import shutil; shutil.copyfile(reset_save, root / "gamesave.bin")
    else:
        assert not reset_save, "--reset-save only with the cup route (install copy)"
    arm = opt("--arm", "off"); assert arm in ("off", "on", "stock", "late", "ctl")
    ctl_asi = opt("--ctl-asi", str(ORIG / "mashed_re_dev.asi") if arm == "late" else None)
    out = Path(opt("--out", str(ROOT / "log" / "c4_hud"))); out.mkdir(parents=True, exist_ok=True)
    tag = opt("--tag", f"{row}_{arm}")
    secs = int(opt("--secs", "60"))
    pulse = "--pulse" in av   # cup route: the qb in-race recipe (control 4, 200 ms, 1 Hz while 0x00771968 == 3)

    env = dict(os.environ)
    hk = f"0x{rva:08x}"
    if arm == "off":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = "__none__"
    elif arm == "on":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = hk
    elif arm == "stock":
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    else:
        assert ctl_asi and Path(ctl_asi).exists(), "--ctl-asi required"
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = hk
    rec = {"row": row, "rva": hk, "arm": arm, "tag": tag, "route": route, "track_presses": track_presses, "pulse": pulse, "reset_save": reset_save,
           "exe": str(exe), "exe_sha256": sha256(exe),
           "deployed_asi_sha256": sha256(ORIG / "mashed_re_dev.asi"),
           "loaded_dir_asi_sha256": sha256(Path(cwd) / "mashed_re_dev.asi") if (Path(cwd) / "mashed_re_dev.asi").exists() else None,
           "save_sha256": sha256(Path(cwd) / "gamesave.bin"),
           "ctl_asi": ctl_asi if arm in ("late", "ctl") else None,
           "ctl_asi_sha256": sha256(ctl_asi) if arm in ("late", "ctl") else None,
           "env": {k: env.get(k) for k in ("MASHED_HOOK_ONLY", "MASHED_RE_NO_AUTO_HOOK")},
           "log": [], "crash": [], "samples": []}
    def log(msg):
        print(msg, flush=True); rec["log"].append(msg)

    dev = frida.get_local_device()
    pid = dev.spawn(str(exe), cwd=str(cwd), env=env)
    rec["pid"] = pid; log(f"  spawned pid={pid} row={row} arm={arm}")
    sess = dev.attach(pid); hproc = _open_handle(pid)
    def on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash":
            rec["crash"].append(p); print("  [CRASH]", p, flush=True)
            try: scr.post({"type": "ack"})
            except Exception: pass
        elif m.get("type") == "error":
            print("  agent err:", m.get("description"), flush=True); rec["log"].append("agent err: " + str(m.get("description")))
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    X = scr.exports_sync
    X.init(row, route != "attract")   # the attract route installs no input hook at all
    dev.resume(pid)
    nav = Nav(scr, pid)
    verdict = "INCOMPLETE"
    try:
        nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 25.0, "title up")
        log(f"  title: depth={nav.depth()} phase={nav.phase()}")
        rec["pre_witness"] = X.modinfo(rva); log(f"  witness before attach: {rec['pre_witness']}")
        if arm in ("late", "ctl"):
            r = X.loadctl(str(Path(ctl_asi).resolve())); log(f"  ctl load: {r}"); rec["loadctl"] = r
            rec["pre_witness"] = X.modinfo(rva); log(f"  witness after load, before attach: {rec['pre_witness']}")
        r = None
        for _ in range(30):
            r = X.arm("off" if arm in ("off", "stock") else "on", rva)
            if r.get("ok") or arm in ("off", "stock"): break
            time.sleep(0.5)
        rec["arm_result"] = r; log(f"  arm: {r}")
        if not r or not r.get("ok"):
            verdict = "ARM-FAILED"; raise SystemExit
        if route == "qb":
            # statenav.py Quick Battle sequence (as c4_detect_e_scenario.py)
            nav.confirm_to_depth(2); log(f"  after GTS: depth={nav.depth()} sel={X.sel()}")
            nav.advance_past_load_modal(3); log(f"  single player: depth={nav.depth()} sel={X.sel()}")
            nav.press(12); log(f"  mode sel: sel={X.sel()}")
            nav.confirm_to_depth(4, tries=4); log(f"  colour-select: depth={nav.depth()} sel={X.sel()}")
            nav.confirm_to_depth(5, tries=4); log(f"  track-select: depth={nav.depth()} sel={X.sel()}")
            for c in track_presses:
                nav.press(c); time.sleep(0.4); log(f"  track press {c}: sel={X.sel()} state={X.state()}")
            nav.press(4); time.sleep(1.5)
            for k in range(5):
                if nav.phase() != 3: break
                nav.press(4); time.sleep(1.5)
                log(f"  start-attempt {k}: depth={nav.depth()} phase={nav.phase()}")
        if route == "cup":
            # Challenge flow: depth-3 cursor 0 (mode 3), colour select, challenge select (screen
            # type 6). Control 10 there moves the cup selector 0x0067f184 1 -> 2 (mode 4,
            # 0x00440415/FUN_0042f6b0), two downs move 0x0067f17c 0 -> 2 (FUN_004322c0), confirm.
            # Event table 0x005f65c8 (mode 4, track 2) = ev4 -> rule 10. No input in the race:
            # the event timer 0x007f0fe4 runs out and FUN_00410510 takes its rule-10 fail path.
            nav.confirm_to_depth(2); nav.advance_past_load_modal(3); log(f"  d3: depth={nav.depth()} sel={X.sel()}")
            nav.confirm_to_depth(4, tries=4); nav.confirm_to_depth(5, tries=4); time.sleep(0.5)
            nav.press(10); time.sleep(0.5); log(f"  cup: {X.state()}")
            nav.press(12); time.sleep(0.4); nav.press(12); time.sleep(0.4); log(f"  event: {X.state()}")
            nav.press(4); time.sleep(1.5)
            for k in range(5):
                if nav.phase() != 3: break
                nav.press(4); time.sleep(1.5)
            log(f"  launched: {X.state()}")
        # Wait for the body's first game call, then observe `secs` more seconds (race rows pulse
        # control 4 at 1 Hz only while 0x00771968 == 3, the scenario_launch in-race recipe).
        t0 = time.time(); first = None; tp = 0; tl = 0; lastn = 0; t_last_change = time.time()
        limit = 150 if route in ("attract", "cup") else 120
        quiet = 15 if row != "40d440" else 1e9   # 40d440 runs once per course load: observe `secs` regardless
        while nav.alive():
            try: s = X.state()
            except frida.InvalidOperationError: log("   script destroyed (process gone)"); break
            now = time.time()
            if (route == "qb" or pulse) and s.get("sph") == 3 and now - tp > 1.0: X.press(4, 200); tp = now
            if s["calls"] != lastn: lastn = s["calls"]; t_last_change = now
            if first is None and s["calls"] > 0:
                first = now; rec["first_call_state"] = s; log(f"  >>> first game call: {s}")
            if now - tl > 5: tl = now; rec["samples"].append(s); log(f"   t+{int(now-t0)}s {s}")
            if first is None and now - t0 > limit: break
            if first is not None and now - first > secs: break
            if first is not None and now - t_last_change > quiet: log(f"   no new calls for {quiet} s"); break
            time.sleep(0.25)
        if first is None:
            verdict = "NOT-REACHED"; raise SystemExit
        rec["final"] = X.state(); rec["modinfo_final"] = X.modinfo(rva)
        verdict = "DONE"
    except SystemExit:
        pass
    finally:
        try: rec["dump"] = X.dump()
        except Exception as e: rec["dump_err"] = str(e)
        rec["alive_at_end"] = nav.alive()
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        rec["exit_code"] = _exit_code(hproc)
        rec["verdict"] = verdict
        p = out / f"c4_hud_{tag}.json"
        p.write_text(json.dumps(rec, indent=1), encoding="utf-8")
        d = rec.get("dump", {})
        print(f"  verdict={verdict} calls={d.get('calls')} keys={len(d.get('keys', {}))} mmOrig={d.get('mmOrig')} "
              f"mmOld={d.get('mmOld')} crash={len(rec['crash'])} alive_at_end={rec['alive_at_end']} -> {p}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
