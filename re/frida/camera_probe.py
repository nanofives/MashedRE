# Race-camera live probe: drive the ORIGINAL into a Quick Battle and capture
# ground truth for the FUN_00446520 verbatim port (re/analysis/race_camera/).
#
# Captures:
#  1. FUN_00446520 fire-rate (expects ~once/frame; Interceptor is safe at 60/s)
#  2. cmd-stream override table DAT_0063a5f0 (does a real track populate it?)
#  3. gate-node array DAT_00663658 (count DAT_0066d6d8) — first nodes
#  4. runtime globals DAT_007f1030 (sway timer), DAT_007f0fc8 (jitter amp),
#     DAT_007f100c (blend step)
#  5. per-frame trace CSV: car positions, cam pos/target/angles, zoom
#     cam[0x268] -> log/camera_trace.csv
#
# Usage: py -3.12 re/frida/camera_probe.py
import csv, json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
# A worktree has no original/ (never junctioned, WORKTREE-SYMLINK-WIPE): walk up to the
# main repo's install, as race_draw_burst.py:find_original does.
ORIG = next((p / "original" for p in (ROOT, *ROOT.parents)
             if (p / "original" / "MASHED.exe").exists()), ROOT / "original")
EXE = ORIG / "MASHED.exe"
LOG = ROOT / "log"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
const CAM=0x00897fe0, OVR=0x0063a5f0, NODES=0x00663658, NCOUNT=0x0066d6d8;
const VEH_BASE=0x00881ec8, VEH_STRIDE=0xd04, REC_BASE=0x008815a0;
let pressCtrl=-1, pressUntil=0;
let camFires=0; let trace=[]; let tracing=false;
// [d1r2 2026-10-16] --frame mode: also record what the RwCamera FRAME holds after the
// director returns, so "which matrix renders" is measured, not inferred. maxRows caps
// the trace (1200 = the v2 default). shotRows = trace rows at which the d3d9 shim's
// MASHED_ORIG_BBDUMP_REQ request file is written FROM INSIDE onLeave, so the very next
// Present dumps the frame this row's camera produced (d3d9_shim.cpp:526-527 polls the
// request once per Present).
let frameMode=false, maxRows=1200, shotRows={}, reqPath='', shotDir='';
// relShots: offsets from the FIRST DAT_0063ba8c == 6 row (racing), because phase 3's
// length follows the QPC clock and differs per run (memory: phase-3 length is the QPC clock).
let relShots={}, raceStart=-1;
// [camribbon 2026-10-16] forceProg >= 0: for the duration of ONE director call (onEnter ->
// onLeave) every car's per-car path progress float (0x008a96e8 + i*0x30c, what FUN_00408a50
// returns) is pinned to forceProg and then restored, so the director samples a chosen node
// pair (e.g. one with no LED elevation) while nothing else sees the pinned value. The trace's
// c{i}prog column is read BEFORE the restore, so it records the pinned value the director used.
let forceProg=-1;
function restoreProg(saved){ if(!saved) return;
  for (let i=0;i<4;i++) abs(0x008a96e8+i*0x30c).writeFloat(saved[i]); }
// [d1r1 2026-10-16] per-draw recorder (PREREG_d1r1.md I-1). Armed at the onLeave of the row
// BEFORE a shot row, detached at the first Present after the shot row's onLeave, so it spans two
// frames and the frame of row K is cut out offline between Present markers. Device COM pointer
// DAT_007d4110: `mov eax,[0x7d4110]` at 0x004dbc31, `call [edx+0x144]` (DrawPrimitive) at
// 0x004dbc47. Texture names from RwTextureSetName 0x004c5ae0 (strncpy to tex+0x10, 0x20 bytes,
// at 0x004c5af6/0x004c5afa); raster -> IDirect3DTexture9* through the D3D9 raster extension
// offset DAT_00911ae4 (`mov eax,[0x911ae4]` at 0x004d6ceb, `lea esi,[eax+edx]` at 0x004d6cf0).
let drawRec=false, drArmed=false, drHooks=[], drEvents=[], drFrames={}, drShotRow=-1;
let texByPtr={}, d3dName={}, d3dDesc={};
let carFrames=false, forceTrack=-1, trackFiles=[];
const RS_LIST=[7,14,23,15,24,25,27,19,20,22,28,137];   // ZENABLE ZWRITE ZFUNC ATEST AREF AFUNC ABLEND SRC DST CULL FOG LIGHTING
const TSS_LIST=[[0,1],[0,4],[1,1]];                     // stage0 COLOROP, stage0 ALPHAOP, stage1 COLOROP
let NF=null;
function devfns(dev){
  const vt=dev.readPointer();
  const f=(slot,ret,args)=>new NativeFunction(vt.add(slot*4).readPointer(),ret,args,'stdcall');
  return { vt:vt,
    getTexture:f(64,'int',['pointer','uint32','pointer']),
    getRS:f(58,'int',['pointer','uint32','pointer']),
    getTSS:f(66,'int',['pointer','uint32','uint32','pointer']),
    getTransform:f(45,'int',['pointer','uint32','pointer']),
    getFVF:f(90,'int',['pointer','pointer']),
    getVS:f(93,'int',['pointer','pointer']),
    getStream:f(101,'int',['pointer','uint32','pointer','pointer','pointer']) };
}
function comRelease(p){ if(p.isNull()) return; new NativeFunction(p.readPointer().add(8).readPointer(),'uint32',['pointer'],'stdcall')(p); }
function texDesc(t){
  const k=t.toString(); if (d3dDesc[k]!==undefined) return d3dDesc[k];
  let s='?';
  try { const vt=t.readPointer();
    const typ=new NativeFunction(vt.add(10*4).readPointer(),'int',['pointer'],'stdcall')(t);
    if (typ===3) { const d=Memory.alloc(32);
      new NativeFunction(vt.add(17*4).readPointer(),'int',['pointer','uint32','pointer'],'stdcall')(t,0,d);
      s='#'+d.add(0x18).readU32()+'x'+d.add(0x1c).readU32()+':'+d.readU32(); }
    else s='#type'+typ;
  } catch(e) { s='#err'; }
  d3dDesc[k]=s; return s;
}
function buildNameMap(){
  d3dName={}; const extoff=abs(0x00911ae4).readU32(); let n=0, bad=0;
  for (const k in texByPtr) {
    try { const tex=ptr(k); const r=tex.readPointer(); if (r.isNull()) continue;
      const d=r.add(extoff).readPointer(); if (d.isNull()) continue;
      d3dName[d.toString()]={name:texByPtr[k], rw:r.add(0xc).readS32()+'x'+r.add(0x10).readS32()}; n++;
    } catch(e) { bad++; } }
  return [n,bad];
}
// [d1r1 amendment 1] geometry actually drawn: non-degenerate triangle count and the world-space
// AABB of the vertices those triangles use, read from the bound index/vertex buffers (READONLY
// lock; static RW buffers are managed) or the UP pointers. This is independent of how a mesh
// is split into calls or of strip vs list topology, which differ by construction between RW's
// D3D9 instancing (strips) and librw's (lists).
function vfn(o,slot,ret,args){ return new NativeFunction(o.readPointer().add(slot*4).readPointer(),ret,args,'stdcall'); }
function triIdx(pt, idx, pc){
  const t=[];
  if (pt===4) for (let k=0;k<pc;k++) t.push([idx[3*k],idx[3*k+1],idx[3*k+2]]);
  else if (pt===5) for (let k=0;k<pc;k++) t.push([idx[k],idx[k+1],idx[k+2]]);
  else if (pt===6) for (let k=0;k<pc;k++) t.push([idx[0],idx[k+1],idx[k+2]]);
  return t;
}
function nIdx(pt, pc){ return pt===4 ? 3*pc : (pt===5||pt===6) ? pc+2 : 0; }
function geomOf(rec, slot, a){
  const pt=a[1]; if (pt<4 || pt>6) return;
  const fvfpos = rec.fvf & 0xe;           // 2 = XYZ, 4 = XYZRHW (pre-transformed)
  let idx=null, base=0, vbp=null, stride=0, vbo=0, lockedVB=null, lockedIB=null;
  const pp=Memory.alloc(8), o=Memory.alloc(4), s=Memory.alloc(4);
  try {
    if (slot===82) {
      const pc=a[6], n=nIdx(pt,pc); base=a[2]|0;
      pp.writePointer(ptr(0)); vfn(ptr(a[0]),105,'int',['pointer','pointer'])(ptr(a[0]),pp);
      const ib=pp.readPointer(); if (ib.isNull()) return;
      const desc=Memory.alloc(24); vfn(ib,13,'int',['pointer','pointer'])(ib,desc);
      const isz = desc.readU32()===102 ? 4 : 2;
      const q=Memory.alloc(8);
      const hr=vfn(ib,11,'int',['pointer','uint32','uint32','pointer','uint32'])(ib,a[5]*isz,n*isz,q,0x10);
      if (hr===0) { const p=q.readPointer(); idx=[];
        for (let k=0;k<n;k++) idx.push(isz===4 ? p.add(4*k).readU32() : p.add(2*k).readU16());
        vfn(ib,12,'int',['pointer'])(ib); }
      comRelease(ib);
      if (idx===null) return;
      rec.tri = 0;
      const tris=triIdx(pt,idx,pc).filter(t=>t[0]!==t[1]&&t[1]!==t[2]&&t[0]!==t[2]);
      rec.tri=tris.length;
      const used={}; for (const t of tris) for (const v of t) used[v+base]=1;
      pp.writePointer(ptr(0)); NF.getStream(ptr(a[0]),0,pp,o,s);
      const vb=pp.readPointer(); if (vb.isNull()) return; stride=s.readU32(); vbo=o.readU32();
      const q2=Memory.alloc(8);
      const hr2=vfn(vb,11,'int',['pointer','uint32','uint32','pointer','uint32'])(vb,0,0,q2,0x10);
      if (hr2===0) { const p=q2.readPointer(); bbox(rec, Object.keys(used).map(v=>p.add(vbo+(v|0)*stride)), fvfpos);
        vfn(vb,12,'int',['pointer'])(vb); }
      comRelease(vb);
    } else if (slot===81) {
      const pc=a[3], n=nIdx(pt,pc), sv=a[2];
      pp.writePointer(ptr(0)); NF.getStream(ptr(a[0]),0,pp,o,s);
      const vb=pp.readPointer(); if (vb.isNull()) return; stride=s.readU32(); vbo=o.readU32();
      const q2=Memory.alloc(8);
      const hr2=vfn(vb,11,'int',['pointer','uint32','uint32','pointer','uint32'])(vb,0,0,q2,0x10);
      if (hr2===0) { const p=q2.readPointer(); const ptsA=[]; for (let k=0;k<n;k++) ptsA.push(p.add(vbo+(sv+k)*stride));
        seqTris(rec, pt, pc, ptsA, fvfpos); vfn(vb,12,'int',['pointer'])(vb); }
      comRelease(vb);
    } else if (slot===83) {
      const pc=a[2], n=nIdx(pt,pc), p=ptr(a[3]); stride=a[4]; const ptsA=[];
      for (let k=0;k<n;k++) ptsA.push(p.add(k*stride));
      seqTris(rec, pt, pc, ptsA, fvfpos);
    } else if (slot===84) {
      const pc=a[4], n=nIdx(pt,pc), ip=ptr(a[5]), isz = a[6]===102 ? 4 : 2, vp=ptr(a[7]); stride=a[8];
      idx=[]; for (let k=0;k<n;k++) idx.push(isz===4 ? ip.add(4*k).readU32() : ip.add(2*k).readU16());
      const tris=triIdx(pt,idx,pc).filter(t=>t[0]!==t[1]&&t[1]!==t[2]&&t[0]!==t[2]);
      rec.tri=tris.length; const used={}; for (const t of tris) for (const v of t) used[v]=1;
      bbox(rec, Object.keys(used).map(v=>vp.add((v|0)*stride)), fvfpos);
    }
  } catch(e) { rec.gerr=''+e; }
}
function seqTris(rec, pt, pc, pts, fvfpos){
  const P=pts.map(p=>[p.readFloat(),p.add(4).readFloat(),p.add(8).readFloat()]);
  const eq=(u,v)=>u[0]===v[0]&&u[1]===v[1]&&u[2]===v[2];
  const idx=[]; for (let k=0;k<P.length;k++) idx.push(k);
  const tris=triIdx(pt,idx,pc).filter(t=>!eq(P[t[0]],P[t[1]])&&!eq(P[t[1]],P[t[2]])&&!eq(P[t[0]],P[t[2]]));
  rec.tri=tris.length; const used={}; for (const t of tris) for (const v of t) used[v]=1;
  bbox(rec, Object.keys(used).map(v=>pts[v|0]), fvfpos);
}
function bbox(rec, pts, fvfpos){
  const w=rec.w; let mn=[1e30,1e30,1e30], mx=[-1e30,-1e30,-1e30];
  for (const p of pts) {
    const x=p.readFloat(), y=p.add(4).readFloat(), z=p.add(8).readFloat();
    let q=[x,y,z];
    if (fvfpos!==4 && w) q=[x*w[0]+y*w[4]+z*w[8]+w[12], x*w[1]+y*w[5]+z*w[9]+w[13], x*w[2]+y*w[6]+z*w[10]+w[14]];
    for (let i=0;i<3;i++){ if(q[i]<mn[i]) mn[i]=q[i]; if(q[i]>mx[i]) mx[i]=q[i]; }
  }
  if (pts.length) rec.bb=[...mn,...mx];
  if (fvfpos===4) rec.screen=1;
}
function recDraw(slot, a){
  const dev=ptr(a[0]); if (NF===null) NF=devfns(dev);
  const b=Memory.alloc(64), b2=Memory.alloc(8), b3=Memory.alloc(8);
  // n = comparable counts: [primCount] for the non-indexed calls (vertex count follows from the
  // primitive type), [numVertices, primCount] for the indexed ones. o = buffer offsets, which
  // depend on the allocator and are kept for debugging only.
  const rec={e:'d', k:slot, pt:a[1]};
  if (slot===81) { rec.n=[a[3]]; rec.o=[a[2]]; }                    // StartVertex, PrimitiveCount
  else if (slot===82) { rec.n=[a[4],a[6]]; rec.o=[a[2],a[3],a[5]]; } // Base, Min, NumV, Start, Prims
  else if (slot===83) { rec.n=[a[2]]; rec.o=[]; }                    // PrimitiveCount, data, stride
  else { rec.n=[a[3],a[4]]; rec.o=[a[2]]; }                          // Min, NumV, Prims, idx, fmt, data, stride
  NF.getTexture(dev,0,b); const t=b.readPointer();
  if (t.isNull()) rec.tex='-';
  else { const nm=d3dName[t.toString()]; rec.tex = nm ? nm.name : texDesc(t); rec.td=texDesc(t);
    if (nm) rec.rw=nm.rw; comRelease(t); }
  rec.rs=RS_LIST.map(s=>{ b.writeU32(0xdeadbeef); const hr=NF.getRS(dev,s,b); return hr===0 ? b.readU32() : -1; });
  rec.tss=TSS_LIST.map(p=>{ b.writeU32(0xdeadbeef); const hr=NF.getTSS(dev,p[0],p[1],b); return hr===0 ? b.readU32() : -1; });
  const hr=NF.getTransform(dev,256,b); rec.w = hr===0 ? rd(b,16) : null;
  NF.getFVF(dev,b2); rec.fvf=b2.readU32();
  b2.writePointer(ptr(0)); NF.getVS(dev,b2); const vs=b2.readPointer(); rec.vs = vs.isNull()?0:1; comRelease(vs);
  if (slot===81 || slot===82) { b2.writePointer(ptr(0)); b3.writeU32(0); const o=Memory.alloc(4);
    NF.getStream(dev,0,b2,o,b3); const vb=b2.readPointer(); rec.st=b3.readU32(); comRelease(vb); }
  else rec.st = slot===83 ? a[4] : a[8];
  // [d1r1 amendment 1] stage-1 texture: the original's world meshes are two-stage
  // (FVF 0x242, stage-1 COLOROP 13 on the smoke capture)
  NF.getTexture(dev,1,b); const t1=b.readPointer();
  if (t1.isNull()) rec.tex1='-';
  else { const nm1=d3dName[t1.toString()]; rec.tex1 = nm1 ? nm1.name : texDesc(t1); comRelease(t1); }
  geomOf(rec, slot, a);
  drEvents.push(rec);
}
function arm(){
  if (drArmed) return; drArmed=true; drEvents=[{e:'arm', t:Date.now(), names:buildNameMap()}];
  const dev=abs(0x007d4110).readPointer(); const vt=dev.readPointer();
  const nargs={81:4, 82:7, 83:5, 84:9};
  for (const s of [81,82,83,84]) {
    const fn=vt.add(s*4).readPointer();
    drHooks.push(Interceptor.attach(fn,{ onEnter(args){ if(!drArmed) return;
      const a=[]; for(let i=0;i<nargs[s];i++) a.push(i===0? args[0] : args[i].toUInt32()); a[0]=args[0];
      try { recDraw(s,a); } catch(e) { drEvents.push({e:'err', k:s, m:''+e}); } }}));
  }
  drHooks.push(Interceptor.attach(vt.add(17*4).readPointer(),{ onEnter(){ if(!drArmed) return;
    drEvents.push({e:'P', t:Date.now()});
    // disarm at the first Present after the shot row's director call
    let seen=false; for (const ev of drEvents) if (ev.e==='D' && ev.row===drShotRow) seen=true;
    if (seen) { drArmed=false; drFrames[drShotRow]=drEvents; drEvents=[];
      setTimeout(()=>{ for (const h of drHooks) h.detach(); drHooks=[]; }, 0); } }}));
}
function carMats(){
  const out=[];
  for (let i=0;i<4;i++){
    try { const rend=abs(0x0063da18+i*0x2ac).readPointer(); const fr=rend.add(4).readPointer();
      let root=fr.add(0xa0).readPointer(); const same = root.isNull() || root.equals(fr) ? 1 : 0;
      if (root.isNull()) root=fr;
      out.push({same:same, M:mat12(root.add(0x10)), L:mat12(root.add(0x50)), FL:mat12(fr.add(0x50))});
    } catch(e) { out.push({same:-1, M:new Array(12).fill(0), L:new Array(12).fill(0), FL:new Array(12).fill(0)}); }
  }
  return out;
}
function abs(r){return ptr(r+DELTA);}
function rd(p,n){ const o=[]; for(let i=0;i<n;i++) o.push(p.add(i*4).readFloat()); return o; }
// RwMatrix rows right/up/at/pos at +0x00/+0x10/+0x20/+0x30 -> 12 floats (flags/pads dropped)
function mat12(p){ const m=rd(p,16); return [m[0],m[1],m[2],m[4],m[5],m[6],m[8],m[9],m[10],m[12],m[13],m[14]]; }
rpc.exports={
  config:function(fm, mr, shots, rq, sd, rel, fp, dr, cf, tr){ frameMode=!!fm; maxRows=mr; shotRows={};
    for (const k of shots) shotRows[k]=1; reqPath=rq; shotDir=sd;
    relShots={}; for (const k of (rel||[])) relShots[k]=1; raceStart=-1;
    forceProg=(fp===undefined||fp===null)?-1:fp;
    drawRec=!!dr; carFrames=!!cf; forceTrack=(tr===undefined||tr===null)?-1:tr; return 1; },
  // [camribbon] engine track index (scenario_launch.py --track: 0=Training .. 12=City). Writing
  // the menu globals mid-menu stalled the Quick Battle confirm (the race never loaded), so the
  // override is applied at the load itself: onEnter of FUN_0040d440 (Course::LoadCurrent, which
  // loads the course named by 0x0063ba7c, scenario_launch.py:42) writes 0x0063ba7c. The track
  // that actually loaded is read back from the trace's track_type column (DAT_00644158).
  settrack:function(v){
    Interceptor.attach(abs(0x0040d440), { onEnter(){ abs(0x0063ba7c).writeS32(v); } });
    return 1; },
  tracelen:function(){ return trace.length; },
  getdraws:function(){ return JSON.stringify(drFrames); },
  trackfiles:function(){ return JSON.stringify(trackFiles); },
  texcount:function(){ return Object.keys(texByPtr).length; },
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }});
    // [d1r1] RwTextureSetName 0x004c5ae0 (texture at [esp+4]; the name lands at tex+0x10).
    // Load-time only: one call per texture read.
    Interceptor.attach(abs(0x004c5ae0),{ onEnter(){ this.t=this.context.esp.add(4).readPointer(); },
      onLeave(){ try { texByPtr[this.t.toString()]=(this.t.add(0x10).readCString()||'').slice(0,31); } catch(e) {} }});
    // [d1r1] --track N: FUN_0040d440 reads the engine track index at its first instruction
    // (`mov eax,[0x63ba7c]` at 0x0040d440) and hands it to FUN_0040d270 at 0x0040d44d. Writing
    // the index (and the menu-side copy 0x0067f17c, as scenario_launch.py:3290-3291 does) at
    // onEnter makes the Quick Battle flow load track N. The piz actually opened is recorded below.
    Interceptor.attach(abs(0x0040d440),{ onEnter(){ if (forceTrack < 0) return;
      abs(0x0063ba7c).writeS32(forceTrack); abs(0x0067f17c).writeS32(forceTrack); }});
    // Frida 17: no static Module.findExportByName (race_draw_burst.py:81-83 idiom)
    const k32=Module.load('kernel32.dll');
    for (const nm of ['CreateFileA','CreateFileW']) {
      const f=k32.findExportByName(nm); if (!f || f.isNull()) continue;
      Interceptor.attach(f,{ onEnter(a){ try { const s = nm.endsWith('W') ? a[0].readUtf16String() : a[0].readAnsiString();
        if (s && /TRACKS[\\/]/i.test(s) && /\.piz$/i.test(s) && trackFiles.indexOf(s)<0) trackFiles.push(s); } catch(e) {} }});
    }
    return DELTA; },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  hookcam:function(){
    Interceptor.attach(abs(0x00446520), {
      onEnter(args){ const sp=this.context.esp;
        this.cam = sp.add(4).readU32();
        // param_2 = force_reset (RaceCamera.h:60 "force_reset = param_2"). Must be
        // captured PER ROW: it selects the snap path over the spring path
        // (RaceCamera.cpp:335-367), so a reset frame the driver does not know about
        // would show up as a large spurious pose divergence.
        this.reset = sp.add(8).readS32();
        if (forceProg >= 0) {
          this.savedProg = [];
          for (let i=0;i<4;i++){ const q=abs(0x008a96e8+i*0x30c); this.savedProg.push(q.readFloat()); q.writeFloat(forceProg); }
        } },
      onLeave(r){
       try {
        camFires++;
        if (!tracing) return;
        const c = ptr(this.cam);
        // NOTE on `tgt` (+0x4c..+0x54): this is the aim DIRECTION VECTOR, not a
        // look-at point. Established 2026-08-26 from this trace's own columns:
        // deriving elev/azim from (tx,ty,tz) reproduces the recorded +0x34/+0x38 to
        // 0.0000 deg (azim) and 0.002 deg (elev) over 286 frames, while deriving
        // them from (tgt - pos) is off by up to 60 deg / 22 deg. The look-at point
        // is pos + tgt, and its Y is CONSTANT over a run -- measured 0.486 on the
        // corrected capture, i.e. roughly car ground height (0.43..0.54), not 0.
        // (An earlier draft of this comment said "y=0 ground plane, |py+ty| median
        // 0.018". That 0.018 came from the 2026-06-10 capture whose car columns are
        // invalid; the plane is a measured constant, not zero.)
        // race_camera.md:28 labels it "look-target", which is what misled the port
        // (RaceCamera.cpp:404 writes a world point).
        const row = {t: Date.now(), fires: camFires,
          pos:[c.add(0x40).readFloat(), c.add(0x44).readFloat(), c.add(0x48).readFloat()],
          tgt:[c.add(0x4c).readFloat(), c.add(0x50).readFloat(), c.add(0x54).readFloat()],
          elev:c.add(0x34).readFloat(), azim:c.add(0x38).readFloat(),
          zoom:c.add(0x9a0).readFloat(), pair:[c.add(0x994).readS32(), c.add(0x998).readS32()],
          // The two remaining Update() arguments, which nothing captured before:
          //   track_type = FUN_00426c00() = *(i32*)0x00644158 (track/level index;
          //     hooks.csv 00426c00 "getter DAT_00644158"). The camera calls it at
          //     0x004474c8 and 0x00447669 and compares `eax, 0x1a` -> the City
          //     pitch/zoom law (RaceCamera.cpp:299-302). Without this the offline
          //     drive cannot pick the right branch.
          //   overhead = DAT_007f0f38 != 0, tested `cmp dword ptr [0x7f0f38], 0`
          //     at 0x00447331 and 0x0044752d (RaceCamera.cpp:274,306,315,328).
          // Also capturing mode (DAT_007f0fd0) PER ROW, not just once: the camera
          // branches on it against 4,5,7,8,9,0xa at eleven sites, and our port
          // implements the STANDARD PATH ONLY (RaceCamera.cpp:179-180,269,430
          // say the mode-1/4/7/8/9 overrides are not ported). So any row with
          // mode != 0 is outside what the port models and must be excluded from
          // the diff rather than silently compared.
          track_type: abs(0x00644158).readS32(),
          overhead: abs(0x007f0f38).readS32(),
          mode: abs(0x007f0fd0).readS32(),
          // Sway clock, PER ROW. RaceCamera.cpp:369-383 runs 9 oscillators off it
          // (read at 0x00447c33), so a single start/end sample cannot reproduce the
          // pose. Advances at ~3.0 MHz live (measured 45,019,027 ticks over 15 s).
          ticks: abs(0x007f1030).readU32(),
          dtb: abs(0x007f100c).readFloat(),
          jit: abs(0x007f0fc8).readFloat(),
          reset: this.reset,
          cars:[], vels:[], act:[], alive:[], dead:[], deadt:[], prog:[], pct:[] };
        for (let i=0;i<4;i++){
          const recBase = REC_BASE + i*VEH_STRIDE;      // 0x008815a0 + i*0xd04
          const rec = VEH_BASE + i*VEH_STRIDE;          // = recBase + 0x928
          // CAR POSITION — replicate FUN_0046d4a0 (hooks.csv C3, 10/10 GREEN,
          // log/diff_ptr_compute_881ec8.csv) EXACTLY:
          //   *out = 0x00881ec8 + idx*0xd04 + (*(i32*)(0x00881f48 + idx*0xd04))*0x40
          // then the camera loads +0x30/+0x34/+0x38 off that pointer
          // (0x00446b5b/6b64/6b6d and three more sites).
          // The act*0x40 term is the WHOLE FIX: 0x00881f48 is record +0x9a8, the
          // wheel-matrix set selector, which INITS TO 1 and flips per frame
          // (structs/vehicle.md:98,101,178). The previous version of this probe
          // hardcoded act=0, so it read the wrong double-buffer block on roughly
          // half the frames and produced denormal garbage in the Y column
          // (|y| max 1.0e-07 / 3.5e-05 across cars, against a real ground height
          // of ~0.425). Every car column in the 2026-06-10 capture is invalid.
          const act = abs(recBase + 0x9a8).readS32();
          row.act.push(act);
          // Guard rather than mask: the selector is documented to be 0 or 1
          // (init 1, flipped via &1). Anything else means the base or stride is
          // wrong, and silently masking it would fabricate a plausible-looking
          // number -- which is exactly how the previous version produced junk that
          // sat in re/analysis/ for eleven weeks looking like ground truth.
          const p = abs(rec + ((act === 0 || act === 1) ? act*0x40 : 0));
          row.cars.push([p.add(0x30).readFloat(), p.add(0x34).readFloat(),
                         p.add(0x38).readFloat()]);
          // CAR VELOCITY — FUN_0046cb30 / Player::GetOffset3D (hooks.csv C2).
          // Reads record +0x9b0 (= 0x00881f50 + i*0xd04, "linear velocity world")
          // ONLY while the record +0x14 gate == 0; otherwise it takes a
          // FUN_0047f1e0 branch that is NOT decoded. Capture the gate so the
          // offline diff can drop rows the alt branch would have served.
          row.vels.push([abs(recBase + 0x9b0).readFloat(),
                         abs(recBase + 0x9b4).readFloat(),
                         abs(recBase + 0x9b8).readFloat(),
                         abs(recBase + 0x14).readS32()]);    // [3] = gate
          // FLAGS, all raw reads at the offsets the camera's getters use:
          //   alive  FUN_0046c7b0 -> *(i32*)(0x008815a4 + i*0xd04); -1 = OOB
          //   dead   FUN_0046cbb0 out1 -> *(i32*)(0x00881f90 + i*0xd04) (0 alive, 2 slide)
          //   deadt  FUN_0046cbb0 out2 -> *(i32*)(0x00881f94 + i*0xd04) — an INT,
          //          the camera does `fild` then `fdiv 1000.0` (0x00447273). Our
          //          port's `float dead_ms` is an adapter divergence.
          //   prog   FUN_00408a50 -> *(f32*)(0x008a96e8 + i*0x30c)
          //   pct    FUN_00408ad0 -> *(f32*)(0x008a96ec + i*0x30c) — NOT read by the
          //          camera itself (FUN_00442a60 and the elim check read it); captured
          //          because our port's adapter passes it in.
          row.alive.push(abs(recBase + 0x04).readS32());
          row.dead.push(abs(recBase + 0x9f0).readS32());
          row.deadt.push(abs(recBase + 0x9f4).readS32());
          row.prog.push(abs(0x008a96e8 + i*0x30c).readFloat());
          row.pct.push(abs(0x008a96ec + i*0x30c).readFloat());
        }
        if (frameMode) {
          // Director tail (re/analysis/race_camera/FUN_00446520.asm): 0x004481a3 roll +0x3c = 0,
          // 0x004481c1 Camera::Apply writes the frame, 0x004481ff..0x00448208 only when cam[0]
          // != 0 does 0x00442a20 overwrite it with entry 0's matrix 0x0089650c. Read all of
          // them here, at onLeave, so the trace says which one the frame actually holds.
          row.camp = this.cam;
          row.ba8c = abs(0x0063ba8c).readS32();      // render sub-state (FUN_0040e350)
          row.flag = c.readS32();                    // cam[0], the phase-3 hold flag
          row.c7 = c.add(0x1c).readS32();            // cam[7], FUN_004427c0 gate
          row.roll = c.add(0x3c).readFloat();
          row.vw58 = c.add(0x58).readFloat();
          row.f101c = abs(0x007f101c).readU32();     // frame counter (entry roll phase)
          row.ff4 = abs(0x007f0ff4).readU32();       // frame clock, 50/frame
          const rwc = c.add(0x84).readPointer();     // RwCamera*
          const fr = rwc.add(4).readPointer();       // its RwFrame
          row.M = mat12(fr.add(0x10));               // modelling matrix (what Apply wrote)
          row.L = mat12(fr.add(0x50));               // LTM (synced at render)
          // RwCamera viewWindow x/y at +0x68/+0x6c (RwCameraSetViewWindow 0x004c1c80;
          // race_draw_burst.py:129-146 reads the same pair)
          row.rvw = [rwc.add(0x68).readFloat(), rwc.add(0x6c).readFloat()];
          row.E = mat12(abs(0x0089650c));            // entry 0 + 0x4c
          row.e0c0 = abs(0x00896580).readFloat();    // entry 0 + 0xc0
          row.shot = 0;
          if (row.ba8c === 6 && raceStart < 0) raceStart = trace.length;
          const relHit = raceStart >= 0 && relShots[trace.length - raceStart];
          if ((shotRows[trace.length] || relHit) && reqPath) {
            const nm = shotDir + '\\orig_row_' + ('00000' + trace.length).slice(-5) + '.bmp';
            const f = new File(reqPath, 'w'); f.write(nm + '\n'); f.close();
            row.shot = 1;
          }
          // [d1r1] car render roots (I-3) and the draw recorder's frame markers (I-1, I-FRAME)
          if (carFrames) row.cm = carMats();
          if (drawRec) {
            const n = trace.length;
            if (drArmed) drEvents.push({e:'D', row:n, t:Date.now()});
            const nextShot = shotRows[n+1] || (raceStart >= 0 && relShots[n+1-raceStart]);
            if (nextShot && !drArmed && drFrames[n+1] === undefined) {
              drShotRow = n+1; arm(); drEvents.push({e:'D', row:n, t:Date.now()});
            }
          }
        }
        trace.push(row);
        if (trace.length >= maxRows) tracing = false;
       } finally { restoreProg(this.savedProg); }
      }});
    return 1; },
  starttrace:function(){ trace=[]; tracing=true; return 1; },
  stoptrace:function(){ tracing=false; return trace.length; },
  gettrace:function(){ return JSON.stringify(trace); },
  fires:function(){ return camFires; },
  globals:function(){ return JSON.stringify({
    t_7f1030: abs(0x007f1030).readU32(),
    jitter_7f0fc8: abs(0x007f0fc8).readFloat(),
    blend_7f100c: abs(0x007f100c).readFloat(),
    mode_7f0fd0: abs(0x007f0fd0).readS32(),
    phase_63ba8c: abs(0x0063ba8c).readS32(),
    nodecount: abs(NCOUNT).readS32(),
    elim_898980: abs(0x00898980).readFloat() }); },
  ovrtable:function(n){ const out=[];
    for(let i=0;i<n;i++){ const e=abs(OVR+i*0xc);
      out.push([e.readFloat(), e.add(4).readFloat(), e.add(8).readFloat()]); }
    return JSON.stringify(out); },
  nodes:function(n){ const out=[];
    for(let i=0;i<n;i++){ const e=abs(NODES+i*0x4c);
      const dir=[e.readFloat(), e.add(4).readFloat(), e.add(8).readFloat()];
      const c0=[e.add(0x18).readFloat(), e.add(0x1c).readFloat(), e.add(0x20).readFloat()];
      const c3=[e.add(0x18+0x24).readFloat(), e.add(0x18+0x28).readFloat(), e.add(0x18+0x2c).readFloat()];
      out.push({dir:dir, c0:c0, c3:c3}); }
    return JSON.stringify(out); },
  depth:function(){ try{return abs(RVA_DEPTH).readS32();}catch(e){return -999;} },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  setsel:function(v){ try{const d=abs(RVA_DEPTH).readS32(); abs(0x0067ed80+(d-1)*0x40).writeS32(v); return 1;}catch(e){return 0;} }
};
send({kind:'ready'});
'''


def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=str(LOG / "camera_trace_v2.csv"))
    ap.add_argument("--static-out", default=str(LOG / "camera_probe_static.json"))
    ap.add_argument("--rows", type=int, default=1200, help="trace length cap")
    ap.add_argument("--frame", action="store_true",
                    help="[d1r2] v3 columns: sub-state, hold flag, RwCamera frame "
                         "modelling + LTM, viewWindow, entry 0 matrix")
    ap.add_argument("--early", action="store_true",
                    help="[d1r2] hook + trace from the first camera call (the phase-3 "
                         "reset), instead of 2 s + 3 s into the race")
    ap.add_argument("--shots", default="",
                    help="[d1r2] comma list of trace rows to dump the backbuffer at "
                         "(needs the d3d9 shim; implies --frame)")
    ap.add_argument("--shots-race", default="",
                    help="[d1r2] comma list of offsets from the first DAT_0063ba8c == 6 "
                         "(racing) row to dump the backbuffer at")
    ap.add_argument("--track", type=int, default=-1,
                    help="[camribbon] engine track index to race instead of the Quick Battle "
                         "default (0=Training 1=Egypt 2=Neustein 3=Arctic 4=Highway 5=Sands "
                         "6=SuperG 7=Roundabout 8=Storm 9=Forest 10=Dump 11=Warzone 12=City). "
                         "[d1r1] the same index also goes to config(): init's FUN_0040d440 hook "
                         "writes it to 0x0063ba7c and the menu copy 0x0067f17c "
                         "(scenario_launch.py:3290-3291); the opened piz is recorded")
    ap.add_argument("--nav-wait", type=float, default=2.0,
                    help="[camribbon] seconds to wait for each menu depth change (default 2.0, "
                         "the previous constant); raise it on a loaded machine")
    ap.add_argument("--force-prog", type=float, default=-1.0,
                    help="[camribbon] pin every car's path progress to this value for each "
                         "director call (see AGENT forceProg); -1 = off")
    ap.add_argument("--drawrec", action="store_true",
                    help="[d1r1] record every D3D9 draw of each shot frame (PREREG_d1r1 I-1) "
                         "into <out_dir>/orig_row_<K>.draws.json")
    ap.add_argument("--carframes", action="store_true",
                    help="[d1r1] per-row car render-root matrices (R{i}_* modelling, "
                         "Q{i}_* LTM, F{i}_* the renderable's own frame LTM, S{i} root==frame)")
    args = ap.parse_args()
    out_csv, out_static = Path(args.out), Path(args.static_out)
    shots = [int(s) for s in args.shots.split(",") if s.strip()]
    rel = [int(s) for s in args.shots_race.split(",") if s.strip()]
    if shots or rel or args.carframes: args.frame = True
    LOG.mkdir(exist_ok=True)
    out_csv.parent.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    env.setdefault("MASHED_WIN_POS", "primary-bl")
    req = out_csv.parent / "orig_bbdump.req"
    if shots or rel:
        if not (ORIG / "d3d9.dll").exists():
            sys.exit(f"--shots needs the d3d9 shim at {ORIG / 'd3d9.dll'}")
        try: req.unlink()
        except OSError: pass
        env["MASHED_ORIG_BBDUMP_REQ"] = str(req)
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"spawned pid={pid}")
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
    scr.exports_sync.config(args.frame, args.rows, shots, str(req) if (shots or rel) else "",
                            str(out_csv.parent.resolve()), rel,
                            args.force_prog if args.force_prog >= 0 else -1,
                            args.drawrec, args.carframes, args.track)
    dev.resume(pid)
    E = scr.exports_sync

    def wait(pred, timeout, what):
        end = time.time() + timeout
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        print(f"timeout: {what} depth={E.depth()} phase={E.phase()}")
        return False

    def press(c, ms=180):
        E.press(c, ms); time.sleep(ms / 1000.0 + 0.3)

    def confirm_to(target, tries=6):
        for _ in range(tries):
            if E.depth() >= target: return True
            press(4)
            if wait(lambda: E.depth() >= target, args.nav_wait, f"d{target}"): return True
            print(f"  [nav] target depth {target}: depth={E.depth()} phase={E.phase()}")
        return E.depth() >= target

    print("booting...")
    wait(lambda: E.phase() == 3 and E.depth() >= 1, 20, "title")
    time.sleep(1.0)
    confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
    confirm_to(3)
    E.setsel(1); time.sleep(0.3)        # Quick Battle
    confirm_to(4, 4)
    confirm_to(5, 4)
    if args.track >= 0:
        E.settrack(args.track)
        print(f"track index written: {args.track}")
    if args.early:
        # Trace from the very first director call, i.e. the FUN_0040d470(1) reset that
        # opens phase 3, so a replay of these rows starts from the same state.
        E.hookcam(); E.starttrace()
    press(4); time.sleep(1.5)
    for _ in range(5):
        if E.phase() != 3: break
        press(4); time.sleep(1.5)
    print(f"in race? phase={E.phase()}")
    if args.early:
        fires3s = -1
        t0 = time.time()
        last_n, last_t = 0, time.time()
        while E.tracelen() < args.rows and time.time() - t0 < args.rows / 30.0 + 60:
            time.sleep(1.0)
            n_now = E.tracelen()
            if n_now != last_n:
                last_n, last_t = n_now, time.time()
            elif n_now > 100 and time.time() - last_t > 8.0:
                print(f"trace stalled at {n_now} rows (director no longer called)")
                break
    else:
        time.sleep(2.0)
        E.hookcam()
        time.sleep(3.0)
        fires3s = E.fires()
        print(f"FUN_00446520 fires in 3s: {fires3s}")

    g = json.loads(E.globals())
    print("globals:", g)
    nodecount = max(0, min(g.get("nodecount", 0), 400))   # [camribbon] was 200; LED capacity is 384
    ovr = json.loads(E.ovrtable(min(nodecount or 32, 64)))
    # Capture the FULL ribbon. The previous version took only the first 8 of 30,
    # which alone made an offline drive of the port impossible (RaceCamera::Update
    # early-returns without a ribbon, and wraps on node+1 against the live count).
    nodes = json.loads(E.nodes(nodecount or 8))
    out_static.write_text(json.dumps(
        {"fires_3s": fires3s, "globals": g, "override_table": ovr,
         "node_count": nodecount, "nodes": nodes}, indent=1))
    print(f"nodes captured: {len(nodes)} of {nodecount}")
    print(f"override entries (first {len(ovr)}): " +
          ", ".join(f"({a:.1f},{b:.1f},{c:.1f})" for a, b, c in ovr[:8]))

    if args.early:
        g2 = g
    else:
        print("tracing 15s...")
        E.starttrace()
        t0 = time.time()
        g2 = None
        while time.time() - t0 < 15.0:
            time.sleep(2.5)
            g2 = json.loads(E.globals())
    n = E.stoptrace()
    rows = json.loads(E.gettrace())
    print(f"trace rows: {n}; globals at end: {g2}")
    # v2 schema. Written to a NEW filename on purpose: the 2026-06-10
    # camera_trace.csv has invalid c*x/c*y/c*z columns (missing the act*0x40
    # selector), and anything reading it by name must not silently pick up a file
    # whose car columns changed meaning.
    hdr = ["t", "fires", "px", "py", "pz", "dx", "dy", "dz",
           "elev", "azim", "zoom", "pairA", "pairB",
           "track_type", "overhead", "mode", "ticks", "dtb", "jit", "reset"]
    for i in range(4):
        hdr += [f"c{i}x", f"c{i}y", f"c{i}z", f"c{i}vx", f"c{i}vy", f"c{i}vz",
                f"c{i}vgate", f"c{i}act", f"c{i}alive", f"c{i}dead",
                f"c{i}deadt", f"c{i}prog", f"c{i}pct"]
    # [d1r2] v3 = v2 + these columns (a superset: cam_driver reads by header name).
    # M_* modelling matrix, L_* LTM, E_* entry 0 matrix; each right/up/at/pos.
    m12 = [f"{a}{b}" for a in ("r", "u", "a", "p") for b in ("x", "y", "z")]
    if args.frame:
        hdr += ["camp", "ba8c", "flag", "c7", "roll", "vw58", "f101c", "ff4"]
        hdr += [f"M_{k}" for k in m12] + [f"L_{k}" for k in m12]
        hdr += ["rvwx", "rvwy"] + [f"E_{k}" for k in m12] + ["e0c0", "shot"]
    if args.carframes:
        for i in range(4):
            hdr += [f"S{i}"] + [f"R{i}_{k}" for k in m12] + [f"Q{i}_{k}" for k in m12] \
                   + [f"F{i}_{k}" for k in m12]
    with open(out_csv, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(hdr)
        for r in rows:
            out = [r["t"], r["fires"], *r["pos"], *r["tgt"],
                   r["elev"], r["azim"], r["zoom"], *r["pair"],
                   r["track_type"], r["overhead"], r["mode"],
                   r["ticks"], r["dtb"], r["jit"], r["reset"]]
            for i in range(4):
                out += [*r["cars"][i], *r["vels"][i][:3], r["vels"][i][3],
                        r["act"][i], r["alive"][i], r["dead"][i],
                        r["deadt"][i], r["prog"][i], r["pct"][i]]
            if args.frame:
                out += [r["camp"], r["ba8c"], r["flag"], r["c7"], r["roll"], r["vw58"],
                        r["f101c"], r["ff4"], *r["M"], *r["L"], *r["rvw"], *r["E"],
                        r["e0c0"], r["shot"]]
            if args.carframes:
                for i in range(4):
                    c = r["cm"][i]
                    out += [c["same"], *c["M"], *c["L"], *c["FL"]]
            w.writerow(out)

    # SELF-CHECK. The whole point of this rewrite is that the previous capture
    # emitted denormal garbage and nobody noticed for eleven weeks. Fail loudly.
    bad = []
    if rows:
        for i in range(4):
            ys = [abs(r["cars"][i][1]) for r in rows]
            acts = {r["act"][i] for r in rows}
            if max(ys) < 1e-3:
                bad.append(f"car{i}: |y| max {max(ys):.2e} — position offsets still wrong")
            if not acts <= {0, 1}:
                bad.append(f"car{i}: selector took values {sorted(acts)}, expected 0/1")
        gates = {r["vels"][i][3] for r in rows for i in range(4)}
        if gates - {0}:
            bad.append(f"velocity gate (+0x14) nonzero on some rows: {sorted(gates)} "
                       f"— those rows took the undecoded FUN_0047f1e0 branch")
        modes = sorted({r["mode"] for r in rows})
        tts = sorted({r["track_type"] for r in rows})
        ohs = sorted({r["overhead"] for r in rows})
        print(f"track_type {tts}   overhead {ohs}   mode {modes}")
        if modes != [0]:
            bad.append(f"mode (DAT_007f0fd0) took values {modes}; the port implements "
                       f"the STANDARD PATH ONLY, so non-zero rows are not comparable")
    if bad:
        print("\n*** CAPTURE SELF-CHECK FAILED ***")
        for b in bad:
            print("   " + b)
        print("Do NOT treat log/camera_trace_v2.csv as ground truth.")
    else:
        print("capture self-check: OK (car Y plausible, selector in {0,1}, "
              "velocity gate all zero)")
    if shots or rel:
        end = time.time() + 5.0
        while req.exists() and time.time() < end: time.sleep(0.1)
        fired = [i for i, r in enumerate(rows) if r.get("shot")]
        print(f"shot rows requested: abs {shots} race-relative {rel}; fired at {fired}")
        for k in fired:
            bmp = out_csv.parent / f"orig_row_{k:05d}.bmp"
            print(f"shot row {k} (ba8c={rows[k]['ba8c']}): {'OK' if bmp.exists() else 'MISSING'} {bmp}")
    tf = json.loads(E.trackfiles())
    print(f"[d1r1] track piz opened: {tf}  (forced index {args.track})  textures named: {E.texcount()}")
    (out_csv.parent / (out_csv.stem + ".tracks.json")).write_text(json.dumps(tf))
    if args.drawrec:
        frames = json.loads(E.getdraws())
        for k, ev in sorted(frames.items(), key=lambda kv: int(kv[0])):
            p = out_csv.parent / f"orig_row_{int(k):05d}.draws.json"
            p.write_text(json.dumps({"side": "orig", "row": int(k), "events": ev}))
            nd = sum(1 for e in ev if e.get("e") == "d")
            print(f"[d1r1] drawrec row {k}: {len(ev)} events, {nd} draws -> {p}")
    try: dev.kill(pid)
    except Exception: pass
    print(f"done -> {out_csv}, {out_static}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
