# Canonical-scenario C4 harness for triage A1 group M (frontend menu rows), modelled on
# re/frida/c4_detect_e_scenario.py (MenuButtonDetectE 0x0042b770).
#
# Rows and their natural callers (re/analysis/negctl/reearn_20261011/c4_a1_m_RESULT.md):
#   A  MenuButtonDetectA        0x0042aff0  call 0x0043fca4 (FUN_0043dfd0), enable ff0b ([ESP+0x24])
#   B  MenuButtonDetectB        0x0042b180  call 0x0043fedd, enable ff0c ([ESP+0x28])
#   C  MenuButtonDetectC        0x0042b310  call 0x0043ff29, enable ff0d ([ESP+0x2c])
#   T  TimerSlotTickDispatcher  0x0043c000  call 0x0043e05d (every frontend tick)
#   F  FrontendPlayerSlotCheck  0x0042ebe0  calls 0x00439736/0x00439a71/0x00439e08/0x00439e31 (FUN_00439210)
# Held bytes come from the cook at 0x00496633..0x0049669c: control 9 -> 0x007f1044 (C),
# control 11 -> 0x007f1046 (A), control 12 -> 0x007f1047 (B). Presses go through the
# FUN_00497310 return override statenav.py uses (no OS input).
#
# Per call of each row (original entry on off/stock, the E9 target where the row is hooked):
# every input the original bytes read is snapshotted at entry, the outputs at leave, and two
# byte-level models are evaluated on the snapshot: m_orig (the original bytes) and m_ctl (the
# row's old defect). mmOrig / mmCtl count calls whose real output differs from each model.
#
# Arms (one fresh MASHED per run, own PID only):
#   off   : shim loads the deployed .asi, MASHED_HOOK_ONLY=__none__ (stock bodies run)
#   on    : shim loads the deployed .asi, MASHED_HOOK_ONLY=<--rows RVAs> (inline E9 live)
#   stock : MASHED_RE_NO_AUTO_HOOK=1, nothing loaded
#   late  : NO_AUTO_HOOK=1, Module.load(--ctl-asi) + InjectHooks at the title, HOOK_ONLY=<--rows>
#           (pass the deployed .asi for the late-load twin of 'on', or a control .asi)
#
# Usage: py -3.12 re/frida/c4_m_scenario.py --arm off|on|stock|late [--rows A,B,C,T,F]
#        [--ctl-asi PATH] [--ctl-kind stride|t_st0|t_dword|f_row0] [--force-f] [--out DIR] [--tag NAME]
import json, os, sys, time, hashlib
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, _open_handle, _exit_code

EXE = statenav.EXE
ORIG = statenav.ORIG
RVAS = {"A": 0x0042aff0, "B": 0x0042b180, "C": 0x0042b310, "T": 0x0043c000, "F": 0x0042ebe0}

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
function abs(r){ return ptr((r+DELTA)>>>0); }
function u8(a){ return abs(a).readU8(); }
function s32(a){ return abs(a).readS32(); }
function u32(a){ return abs(a).readU32(); }
function f32(a){ return abs(a).readFloat(); }
const F32=new Float32Array(1), U32=new Uint32Array(F32.buffer);
function fbits(x){ F32[0]=x; return U32[0]; }
function fr(x){ return Math.fround(x); }
let PRESS=[];                      // {p,c,until}
let WIN='boot';
const ST={};                       // fn -> {calls, ret1, mmOrig, mmCtl, disc, ex:[]}
const WS={};                       // win -> fn -> {calls, ret1, mmOrig, mmCtl, disc}
const ARM={};
let CTLKIND='none';
function bumpw(fn, k, v){ let w=WS[WIN]; if(!w) w=WS[WIN]={}; let e=w[fn]; if(!e) e=w[fn]={calls:0,ret1:0,mmOrig:0,mmCtl:0,disc:0};
  e[k]+= (v===undefined?1:v); }
function stat(fn){ let e=ST[fn]; if(!e) e=ST[fn]={calls:0,ret1:0,mmOrig:0,mmCtl:0,disc:0,ex:[],cwSplit:0,unscored:0}; return e; }
function scr(){ const d=s32(0x0067e9f8); return {d:d, t:s32(0x0067ed3c+d*0x40), sub:s32(0x0067ed40+d*0x40)}; }
function stArr(n){ const o=[]; let base=null; try{ base=abs(0x005f2770).readPointer(); }catch(e){}
  for (let i=0;i<n;i++){ let v=-999999; try{ v=base.add(0x34+i*4).readS32(); }catch(e){} o.push(v); } return o; }
function padb(base, id){ if (id<0 || id>63) return -1; try{ return abs(base+id*0x4c).readU8(); }catch(e){ return -1; } }
// ---------- A / B: 0x0042aff0 / 0x0042b180 ----------
function snapAB(h,p,tm){ const s={sel:s32(0x007f1a0c), ids:[], st:stArr(16), held:[], prev:[], tin:u32(tm), dt:f32(0x007f1004)};
  for (let i=0;i<16;i++) s.ids.push(s32(0x007f1a14+i*4));
  for (let k=0;k<8;k++){ s.held.push(padb(h,k)); s.prev.push(padb(p,k)); }
  const c=scr(); s.t=c.t; s.sub=c.sub; return s; }
function mAB(s,h,p,stride,n){   // returns [{ret, tout}] (one per cw variant)
  let held=false;
  if (s.sel===0x1000){ for (let k=0;k<8;k++) if (s.held[k]>0) held=true; }
  else for (let i=0;i<n;i++){ const id=s.ids[i*stride/4]; if (id===-1) continue; if (s.st[i]===2) continue; if (padb(h,id)>0) held=true; }
  const outs=[];
  for (const pc24 of [false,true]){
    let tout=s.tin;
    if (!held) tout=0;
    else if (s.t>=6 && s.t<=7){
      F32[0]=0; U32[0]=s.tin; const tin=F32[0]; const sum=s.dt+tin; const sumR=fr(sum);
      tout=fbits(sumR); const thr=fr(0.4000000059604645);
      const ex = pc24 ? (sumR>thr) : (sum>thr);
      if (ex){ outs.push({ret:1,tout:0x3e4ccccd}); continue; }
    }
    let ret=0;
    if (s.sel===0x1000){ for (let k=0;k<8;k++) if (s.held[k]>0 && s.prev[k]===0){ ret=1; break; } }
    else for (let i=0;i<n;i++){ const id=s.ids[i*stride/4]; if (id===-1) continue; if (s.st[i]===2) continue;
      if (padb(h,id)>0 && padb(p,id)===0){ ret=1; break; } }
    outs.push({ret:ret,tout:tout});
  }
  return outs; }
// ---------- C: 0x0042b310 ----------
function snapC(){ const s={sel:s32(0x007f1a0c), ids:[], st:stArr(16), held:[], prev:[], cin:u32(0x0067f1b8)};
  for (let i=0;i<16;i++) s.ids.push(s32(0x007f1a14+i*4));
  for (let k=0;k<8;k++){ s.held.push(padb(0x7f1044,k)); s.prev.push(padb(0x7f1504,k)); }
  const c=scr(); s.t=c.t; s.sub=c.sub; return s; }
function mC(s,stride,n){
  const h=0x7f1044, p=0x7f1504; let held=false;
  if (s.sel===0x1000){ for (let k=0;k<8;k++) if (s.held[k]>0) held=true; }
  else for (let i=0;i<n;i++){ const id=s.ids[i*stride/4]; if (id===-1) continue; if (s.st[i]===2) continue; if (padb(h,id)>0) held=true; }
  let cout=s.cin;
  if (!held) cout=0;
  else { if (s.t===0x13 && (s.sub===0||s.sub===1||s.sub===2)) return {ret:1,cout:cout};
         if (s.t===0x1e) return {ret:1,cout:cout}; }
  let ret=0;
  if (s.sel===0x1000){ for (let k=0;k<8;k++) if (s.held[k]>0 && s.prev[k]===0) ret=1; }
  else for (let i=0;i<n;i++){ const id=s.ids[i*stride/4]; if (id===-1) continue; if (s.st[i]===2) continue;
    if (padb(h,id)>0 && padb(p,id)===0) ret=1; }
  return {ret:ret,cout:cout}; }
// ---------- T: 0x0043c000 ----------
const RESET=[0x67e7d8,0x67e7b0,0x67e7f0,0x67e7d0,0x67e7a8,0x67e7c0,0x67e7c8,0x67e7f8,0x67e838,0x67e828,0x67e830,0x67e820,
             0x67e7e0,0x67e7e8,0x67e808,0x67e810,0x67e818,0x67e7b8,0x67e800];
const TICK=[[0x67e7a8,1],[0x67e7d8,1],[0x67e7b0,0],[0x67e7f0,0],[0x67e7d0,0],[0x67e7c0,0],[0x67e7c8,1],[0x67e7f8,0],[0x67e838,0],
            [0x67e828,0],[0x67e830,0],[0x67e820,0],[0x67e7e0,1],[0x67e7e8,0],[0x67e808,0],[0x67e810,0],[0x67e818,0],[0x67e7b8,0]];
function snapT(){ const s={x:f32(0x0067e840), xb:u32(0x0067e840), st:{}, cnt:{}, pad:{}};
  for (const a of RESET){ s.st[a]=u32(a); s.cnt[a]=u8(a+4); s.pad[a]=u32(a+4)>>>8; } return s; }
function ftolv(v){ return (v<0?Math.ceil(v):Math.floor(v))|0; }
function mT(s){   // -> {st, cnt, pad, ret[] (both cw variants)} ; null entries = not predicted
  const st={}, cnt={}, pad={};
  for (const a of RESET){ st[a]=s.st[a]; cnt[a]=s.cnt[a]; pad[a]=s.pad[a]; }
  if (s.x===0){ for (const a of RESET) if (st[a]!==1) st[a]=0; }
  const one=fr(1.0), k255=fr(255.0);
  let ret=[null,null]; let actionSeen=false;
  for (const [a,act] of TICK){
    const v=st[a];
    if (v===1){ const r53=ftolv((one - s.x)*k255), r24=ftolv(fr(fr(one - s.x)*k255));
      cnt[a]= (r53 & 0xff); if ((r24&0xff)!==(r53&0xff)) cnt[a]=[r53&0xff, r24&0xff];
      ret=[r53,r24]; if (act) actionSeen=true; }
    else if (v===2){ const r53=ftolv(s.x*k255), r24=ftolv(fr(s.x*k255));
      cnt[a]=(r53&0xff); if ((r24&0xff)!==(r53&0xff)) cnt[a]=[r53&0xff, r24&0xff]; ret=[r53,r24]; }
    else { ret=[v,v]; if (a===0x67e7d8){ st[a]=(st[a] & 0xffffff00)>>>0; } else cnt[a]=0; }
  }
  return {st:st, cnt:cnt, pad:pad, ret:ret, actionSeen:actionSeen}; }
function cmpT(m, s1, ret){ // count mismatching fields
  let bad=[];
  for (const a of RESET){
    if ((s1.st[a]>>>0)!==(m.st[a]>>>0)) bad.push('st'+a.toString(16));
    const c=m.cnt[a]; const ok = Array.isArray(c) ? (c.indexOf(s1.cnt[a])>=0) : (s1.cnt[a]===c);
    if (!ok) bad.push('cnt'+a.toString(16)+':'+s1.cnt[a]+'!='+JSON.stringify(c));
    if (s1.pad[a]!==m.pad[a]) bad.push('pad'+a.toString(16)+':'+s1.pad[a]);
  }
  if (m.ret[0]!==null && ret!==(m.ret[0]>>>0) && ret!==(m.ret[1]>>>0)) bad.push('ret '+ret+' vs '+m.ret);
  return bad; }
function mTctl(s){  // old defects on state 1 only (kind chosen by CTLKIND); predicts what changes vs mT
  return null; }
// ---------- F: 0x0042ebe0 ----------
const FCOL={10:0x7f0a44, 11:0x7f0a48, 12:0x7f0a54};
function snapF(p){ const s={p:p, en:null, col:[]};
  try{ s.en=u32(0x7f0a44+p*0x30); }catch(e){}
  if (FCOL[p]!==undefined) for (let r=0;r<10;r++) s.col.push(u32(FCOL[p]+r*0x30)); return s; }
function mF(s, rows){ if (!s.en) return 0; let found=0;
  if (FCOL[s.p]!==undefined) for (let r=0;r<rows;r++) if (s.col[r]===0){ found=1; break; }
  return found===0 ? 1 : 0; }

function rec(fn, ok, okCtl, disc, ex){
  const e=stat(fn); e.calls++; bumpw(fn,'calls');
  if (!ok){ e.mmOrig++; bumpw(fn,'mmOrig'); if (e.ex.length<12) e.ex.push(ex); }
  if (okCtl===false){ e.mmCtl++; bumpw(fn,'mmCtl'); }
  if (disc){ e.disc++; bumpw(fn,'disc'); }
}
function attachRow(fn, entry){
  if (fn==='A' || fn==='B'){
    const h = fn==='A'?0x7f1046:0x7f1047, p = fn==='A'?0x7f1506:0x7f1507, tm = fn==='A'?0x67f1b0:0x67f1b4;
    Interceptor.attach(entry,{ onEnter(){ try{ this.s=snapAB(h,p,tm); }catch(e){ this.s=null; } },
      onLeave(r){ const s=this.s; if(!s){ stat(fn).unscored++; return; } const ret=r.toUInt32(); const tout=u32(tm);
        if (ret===1){ stat(fn).ret1++; bumpw(fn,'ret1'); }
        const mo=mAB(s,h,p,16,4), mc=mAB(s,h,p,4,16);
        const ok = mo.some(o=>o.ret===ret && o.tout===tout);
        const okC = mc.some(o=>o.ret===ret && o.tout===tout);
        if (mo[0].ret!==mo[1].ret || mo[0].tout!==mo[1].tout) stat(fn).cwSplit++;
        const disc = !(mo[0].ret===mc[0].ret && mo[0].tout===mc[0].tout);
        rec(fn, ok, okC, disc, {t:s.t, sel:s.sel, ids:s.ids.slice(0,8), st:s.st.slice(0,8), held:s.held, ret:ret, tout:tout.toString(16), mo:mo, mc:mc, win:WIN}); }});
  } else if (fn==='C'){
    Interceptor.attach(entry,{ onEnter(){ try{ this.s=snapC(); }catch(e){ this.s=null; } },
      onLeave(r){ const s=this.s; if(!s){ stat(fn).unscored++; return; } const ret=r.toUInt32(); const cout=u32(0x0067f1b8);
        if (ret===1){ stat(fn).ret1++; bumpw(fn,'ret1'); }
        const mo=mC(s,16,4), mc=mC(s,4,8);
        const ok = mo.ret===ret && mo.cout===cout, okC = mc.ret===ret && mc.cout===cout;
        rec(fn, ok, okC, !(mo.ret===mc.ret && mo.cout===mc.cout), {t:s.t, sub:s.sub, sel:s.sel, ids:s.ids.slice(0,8), st:s.st.slice(0,8), held:s.held, ret:ret, cout:cout, mo:mo, mc:mc, win:WIN}); }});
  } else if (fn==='T'){
    Interceptor.attach(entry,{ onEnter(){ try{ this.s=snapT(); }catch(e){ this.s=null; } },
      onLeave(r){ const s=this.s; if(!s){ stat(fn).unscored++; return; } const ret=r.toUInt32(); const s1=snapT();
        const m=mT(s); const bad=cmpT(m,s1,ret);
        let n1=0, n2=0; for (const [a,act] of TICK){ if (m.st[a]===1) n1++; if (m.st[a]===2) n2++; }
        const e=stat(fn); e.st1=(e.st1||0)+n1; e.st2=(e.st2||0)+n2; e.padNZ=(e.padNZ||0)+(RESET.some(a=>s.pad[a]!==0)?1:0);
        bumpw(fn,'st1',n1); bumpw(fn,'st2',n2);
        // control prediction: under either T control the counter of every state-1 slot can differ
        rec(fn, bad.length===0, null, n1>0, {x:s.x, bad:bad.slice(0,6), ret:ret, win:WIN}); }});
  } else if (fn==='F'){
    Interceptor.attach(entry,{ onEnter(){ try{ this.s=snapF(this.context.esp.add(4).readS32()); }catch(e){ this.s=null; } },
      onLeave(r){ const s=this.s; if(!s){ stat(fn).unscored++; return; } const ret=r.toUInt32();
        if (ret===1){ stat(fn).ret1++; bumpw(fn,'ret1'); }
        const mo=mF(s,10), mc=mF(s,1);
        rec(fn, mo===ret, mc===ret, mo!==mc, {p:s.p, en:s.en, col:s.col, ret:ret, mo:mo, mc:mc, win:WIN});
        const e=stat(fn); e.byP=e.byP||{}; const k='p'+s.p+'_r'+ret; e.byP[k]=(e.byP[k]||0)+1; }});
  }
}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(0x00497310),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ const now=Date.now();
        for (const q of PRESS) if (q.p===this.p && q.c===this.c && now<q.until){ ret.replace(ptr(0xff)); return; } }});
    return DELTA; },
  // snapshot the first bytes of each row BEFORE anything is attached (hook-live witness)
  witness:function(rows){ const o={}; for (const fn of Object.keys(rows)){ const a=abs(rows[fn]); const b0=a.readU8();
      let tgt=null, mod=null; if (b0===0xe9){ tgt=a.add(5).add(a.add(1).readS32()); const mm=Process.findModuleByAddress(tgt);
        mod = mm ? {name:mm.name, path:mm.path, off:'0x'+tgt.sub(mm.base).toUInt32().toString(16)} : null; }
      o[fn]={b0:b0, dword:a.readU32().toString(16), tgt: tgt?tgt.toString():null, mod:mod}; } return o; },
  arm:function(rows, live){ // live: fn -> true if this row must be hooked (E9) on this arm
    for (const fn of Object.keys(rows)){ const a=abs(rows[fn]); const b0=a.readU8();
      if (live[fn] && b0!==0xe9) return {ok:false, why:fn+' not hooked'};
      if (!live[fn] && b0===0xe9) return {ok:false, why:fn+' hooked on a stock row'};
      const entry = b0===0xe9 ? a.add(5).add(a.add(1).readS32()) : a;
      const mm=Process.findModuleByAddress(entry);
      ARM[fn]={entry:entry.toString(), mod: mm?(mm.name+'+0x'+entry.sub(mm.base).toUInt32().toString(16)):null, hooked:b0===0xe9};
      attachRow(fn, entry); }
    return {ok:true, arm:ARM}; },
  loadctl:function(path){ const m=Module.load(path); const f=m.findExportByName('InjectHooks');
    if (!f) return {ok:false, why:'no InjectHooks'}; new NativeFunction(f,'void',[])();
    return {ok:true, base:m.base.toString(), name:m.name, path:m.path}; },
  press:function(c,ms){ PRESS=[{p:0,c:c,until:Date.now()+ms}]; return 1; },
  pressp:function(p,c,ms){ PRESS=[{p:p,c:c,until:Date.now()+ms}]; return 1; },
  win:function(w){ WIN=w; return 1; },
  forcef:function(){ // progress-table cells read by 0x0042ebe0 (see RESULT.md: forced input, F only)
    const w=[[0x7f0c24,1],[0x7f0c54,1],[0x7f0c84,1]]; for (let r=0;r<10;r++) w.push([0x7f0a48+r*0x30,1]);
    const before=w.map(x=>[x[0].toString(16), u32(x[0])]); for (const [a,v] of w) abs(a).writeU32(v);
    return {before:before, after:w.map(x=>[x[0].toString(16), u32(x[0])])}; },
  depth:function(){ try{return s32(0x0067e9f8);}catch(e){return -999;} },
  phase:function(){ try{return s32(0x0067eca4);}catch(e){return -999;} },
  state:function(){ const c=scr(); return {d:c.d, t:c.t, sub:c.sub, ph:s32(0x0067eca4), sel:s32(0x007f1a0c), sph:u8(0x00771968),
      row:s32(0x0067f17c), col:s32(0x0067f184), gm:s32(0x0067e9fc), cur6:s32(0x0067ee80),
      calls:Object.fromEntries(Object.keys(ST).map(k=>[k,ST[k].calls])), mm:Object.fromEntries(Object.keys(ST).map(k=>[k,ST[k].mmOrig]))}; },
  slots:function(){ return {sel:s32(0x007f1a0c), ids:[0,1,2,3].map(i=>s32(0x007f1a14+i*0x10)), raw:Array.from({length:16},(x,i)=>s32(0x007f1a14+i*4)), st:stArr(16)}; },
  dump:function(){ return {ST:ST, WS:WS, ARM:ARM}; }
};
Process.setExceptionHandler(function (d) {
  if (d.type !== 'access-violation') return false;
  try { const pc=d.context.pc; const m=Process.findModuleByAddress(pc);
    send({kind:'crash', pc:pc.toString(), module: m ? (m.name+'+0x'+pc.sub(m.base).toUInt32().toString(16)) : 'UNMAPPED',
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
    arm = opt("--arm", "off"); assert arm in ("off", "on", "stock", "late")
    rows = [r for r in opt("--rows", "A,B,C,T,F").split(",") if r]
    ctl_asi = opt("--ctl-asi", None)
    force_f = "--force-f" in av
    out = Path(opt("--out", str(ROOT / "log" / "c4_m"))); out.mkdir(parents=True, exist_ok=True)
    tag = opt("--tag", arm)
    rowmap = {k: RVAS[k] for k in RVAS}
    hook_only = ",".join("0x%08x" % RVAS[r] for r in rows)
    env = dict(os.environ)
    if arm == "off":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = "__none__"
    elif arm == "on":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = hook_only
    elif arm == "stock":
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    else:
        assert ctl_asi and Path(ctl_asi).exists(), "--ctl-asi required for late"
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = hook_only
    live = {k: (arm in ("on", "late") and k in rows) for k in RVAS}
    rec = {"arm": arm, "tag": tag, "rows_live": [k for k in RVAS if live[k]], "force_f": force_f,
           "exe_sha256": sha256(EXE), "deployed_asi_sha256": sha256(ORIG / "mashed_re_dev.asi"),
           "ctl_asi": ctl_asi, "ctl_asi_sha256": sha256(ctl_asi) if ctl_asi else None,
           "ctl_kind": opt("--ctl-kind", None),
           "env": {k: env.get(k) for k in ("MASHED_HOOK_ONLY", "MASHED_RE_NO_AUTO_HOOK")},
           "log": [], "windows": [], "crash": []}
    def log(msg): print(msg, flush=True); rec["log"].append(msg)

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    rec["pid"] = pid; log(f"  spawned pid={pid} arm={arm} live={rec['rows_live']}")
    sess = dev.attach(pid); hproc = _open_handle(pid)
    def on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash": rec["crash"].append(p); print("  [CRASH]", p, flush=True)
        elif m.get("type") == "error": print("  agent err:", m.get("description"), flush=True)
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    X = scr.exports_sync
    X.init(); dev.resume(pid)
    nav = Nav(scr, pid)
    def st(msg): s = X.state(); log(f"  {msg}: {s}"); return s
    def window(name, actions, settle=1.0):
        """actions: list of (player, control, ms, wait_after)"""
        X.win(name); s0 = X.state()
        for (p, c, ms, wa) in actions:
            X.pressp(p, c, ms); time.sleep(ms / 1000.0 + wa)
        time.sleep(settle); s1 = X.state()
        rec["windows"].append({"win": name, "actions": actions, "s0": s0, "s1": s1})
        log(f"  win {name}: d={s1['d']} t={s1['t']} sub={s1['sub']} row={s1['row']} col={s1['col']} cur6={s1['cur6']} calls={s1['calls']} mm={s1['mm']}")
    verdict = "INCOMPLETE"
    try:
        nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 25.0, "title up")
        rec["witness_pre_attach"] = X.witness(rowmap)
        log(f"  witness (pre-attach): {rec['witness_pre_attach']}")
        if arm == "late":
            r = X.loadctl(str(Path(ctl_asi).resolve())); rec["ctl_load"] = r; log(f"  ctl load: {r}")
            rec["witness_after_load"] = X.witness(rowmap); log(f"  witness (after load): {rec['witness_after_load']}")
        r = X.arm(rowmap, live); rec["arm_result"] = r; log(f"  arm: {r}")
        if not r.get("ok"):
            verdict = "ARM-FAILED"; raise SystemExit
        X.win("nav_title")
        nav.confirm_to_depth(2); st("after GTS")
        X.win("nav_gts")
        nav.advance_past_load_modal(3); st("single player")
        rec["slots_sp"] = X.slots()
        if force_f:
            rec["forcef"] = X.forcef(); log(f"  force-f: {rec['forcef']}")
        window("sp_down", [(0, 12, 200, 0.6)])                                   # -> Quick Battle (B, t2)
        X.win("nav_sp"); nav.confirm_to_depth(4, tries=4); st("colour-select")
        rec["slots_t4"] = X.slots()
        window("t4_c9", [(0, 9, 200, 0.8), (1, 9, 200, 0.8), (2, 9, 200, 0.8), (0, 10, 200, 0.8)])
        X.win("nav_t4"); nav.confirm_to_depth(5, tries=4); st("track-select")
        rec["slots_t6"] = X.slots()
        window("t6_hold12", [(0, 12, 1500, 0.6)])
        window("t6_hold11", [(0, 11, 1500, 0.6)])
        window("t6_taps", [(1, 11, 200, 0.6), (1, 12, 200, 0.6), (1, 9, 200, 0.6),
                           (2, 11, 200, 0.6), (2, 12, 200, 0.6), (2, 9, 200, 0.6),
                           (0, 12, 200, 0.6), (0, 11, 200, 0.6)])
        X.win("nav_t6"); nav.press(4); time.sleep(1.5); st("after track confirm")
        window("t24_taps", [(1, 11, 200, 0.6), (1, 12, 200, 0.6), (1, 9, 200, 0.6), (2, 11, 200, 0.6),
                            (0, 12, 200, 0.6), (0, 11, 200, 0.6)])
        X.win("nav_t24")
        for k in range(5):
            if nav.phase() != 3: break
            nav.press(4); time.sleep(1.5); st(f"start-attempt {k}")
            if nav.phase() == 0: break
        X.win("race")
        end = time.time() + int(opt("--round", "240")); t_p = 0; t_l = 0; reached = None
        while time.time() < end:
            if not nav.alive(): log("   process exited during race"); break
            s = X.state()
            if s["sph"] == 3 and time.time() - t_p > 1.0: X.press(4, 200); t_p = time.time()
            if time.time() - t_l > 20: t_l = time.time(); log(f"   race t {s}")
            if s["ph"] == 3 and s["t"] == 5: reached = s; break
            time.sleep(0.25)
        rec["results_reached"] = reached
        if not reached:
            verdict = "RESULTS-NOT-REACHED"; raise SystemExit
        X.win("t5"); time.sleep(3.0); st("results")
        X.win("nav_t5"); nav.press(4); time.sleep(2.0); s = st("after results confirm")
        rec["slots_t7"] = X.slots()
        if s["t"] == 7:
            window("t7_hold12", [(0, 12, 1500, 0.6)])
            window("t7_hold11", [(0, 11, 1500, 0.6)])
            window("t7_taps", [(1, 11, 200, 0.6), (1, 12, 200, 0.6), (1, 9, 200, 0.6),
                               (2, 11, 200, 0.6), (2, 12, 200, 0.6), (2, 9, 200, 0.6)])
        X.win("end"); time.sleep(1.0)
        rec["witness_final"] = X.witness(rowmap)
        verdict = "DONE"
    except SystemExit:
        pass
    finally:
        try: rec["final"] = X.state()
        except Exception: pass
        try: rec["dump"] = X.dump()
        except Exception: pass
        rec["alive_at_end"] = nav.alive()
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        rec["exit_code"] = _exit_code(hproc)
        rec["verdict"] = verdict
        p = out / f"c4_m_{tag}.json"
        p.write_text(json.dumps(rec, indent=1), encoding="utf-8")
        d = rec.get("dump", {}).get("ST", {})
        print(f"  verdict={verdict} crash={len(rec['crash'])} " +
              " ".join(f"{k}:calls={v['calls']},ret1={v['ret1']},mmO={v['mmOrig']},mmC={v['mmCtl']},disc={v['disc']}" for k, v in sorted(d.items())) +
              f" -> {p}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
