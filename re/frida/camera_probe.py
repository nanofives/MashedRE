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
let relShots={}, raceStart=-1, shotErrs=[];
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
let carFrames=false, forceTrack=-1, trackFiles=[], btRe=null, drCtx=null;
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
  // [rsky] the car shadow render-to-texture. FUN_0041f8f0 @ 0x0041f8f0 picks (&DAT_0063e490)[b] as the camera and
  // (&DAT_0063d854)[b] as the texture (b = rec+0x298 == 2, two entries), hands it to the shadow pass through
  // FUN_00422ac0 / FUN_00422af0, and FUN_004228f0 @ 0x004228f0 draws it (`**(int**)slot` = RwTexture -> raster, first
  // field) with RenderState 1 = TEXTURERASTER. It is an RwTexture created from the camera's raster, so
  // RwTextureSetName never names it and it surfaced as `#64x64:21` (Neustein) / `#256x256:21` (TRAINING),
  // U-9702 I-NAME. Name it by its slot. Same raster -> D3D texture step as the loop above.
  for (let i=0;i<2;i++) {
    try { const t=abs(0x0063d854+4*i).readPointer(); if (t.isNull()) continue;
      const r=t.readPointer(); if (r.isNull()) continue;
      const d=r.add(extoff).readPointer(); if (d.isNull()) continue;
      if (d3dName[d.toString()]===undefined) {
        d3dName[d.toString()]={name:'@shadowrt'+i, rw:r.add(0xc).readS32()+'x'+r.add(0x10).readS32()}; n++; }
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
  // [d1r1] vertex alpha: D3DFVF_DIFFUSE (0x40) follows the position (12 XYZ / 16 XYZRHW) and
  // the normal (0x10, 12 bytes); va = used vertices whose diffuse alpha byte is < 255
  if (rec.fvf & 0x40) {
    let off = (fvfpos===4 ? 16 : 12) + ((rec.fvf & 0x10) ? 12 : 0);
    let va=0; for (const p of pts) { if (p.add(off+3).readU8() < 255) va++; }
    rec.va=va; rec.vn=pts.length;
  }
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
  // [d1r1] --drawrec-bt REGEX: return-address chain (module-relative to MASHED.exe image base
  // 0x400000) for draws whose stage-0 texture matches, to cite each category's render site
  if (btRe && btRe.test(rec.tex) && drCtx) {
    try { rec.bt = Thread.backtrace(drCtx, Backtracer.ACCURATE).slice(0, 12).map(p => {
      const m = Process.findModuleByAddress(p);
      return m && m.name.toLowerCase() === 'mashed.exe' ? '0x' + (p.sub(m.base).toUInt32() + IMG).toString(16)
                                                      : (m ? m.name + '+0x' + p.sub(m.base).toString(16) : p.toString()); }); }
    catch(e) { rec.bt = ['err ' + e]; }
  }
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
      drCtx=this.context;
      try { recDraw(s,a); } catch(e) { drEvents.push({e:'err', k:s, m:''+e}); }
      drCtx=null; }}));
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
// [rtex] car MatFX state (PREREG_rtex section 5): globals FUN_0041ecc0 / FUN_0041f360 /
// FUN_00420290 read, and per car record (0x0063d9e0 + i*0x2ac) the env coefficient / texture
// / frame (+0x274 / +0x278 / +0x27c) and every part atomic's materials with their MatFX data
// (*(mat + DAT_007dc5dc): slots of 0x18 with the effect at +0x14, material effect at +0x30;
// env {frame, texture, coef, fbAlpha}, dual {texture, src, dst}). RpGeometry: flags +0x08,
// materials +0x20, count +0x24. RwTexture name +0x10.
let matfxOn=false, matfxSnaps=[];
function texName(t){ if (t.isNull()) return null; try { return t.add(0x10).readCString(32); } catch(e){ return '?'; } }
// RwTexture filterAddressing +0x50 (FUN_004d6e70 reads param_1[0x14]); raster +0 -> width
// +0xc, height +0x10, depth +0x14, cFormat +0x23 (d1r1 texalpha reader's offsets)
function texInfo(t){ if (t.isNull()) return null;
  try { const r=t.readPointer(); return {name: texName(t), fa: t.add(0x50).readU32(),
    w: r.add(0xc).readS32(), h: r.add(0x10).readS32(), d: r.add(0x14).readS32(), cf: r.add(0x23).readU8()}; }
  catch(e){ return {name: texName(t), err: ''+e}; } }
function carMatfx(){
  const off = abs(0x007dc5dc).readS32();
  const g = { ad0: abs(0x00636ad0).readS32(), ad4: abs(0x00636ad4).readS32(),
    gloss_35c: abs(0x0064435c).readFloat(), gloss_360: abs(0x00644360).readFloat(),
    dirt_90: texInfo(abs(0x0063ba90).readPointer()), dirt_94: texInfo(abs(0x0063ba94).readPointer()),
    track_7c: abs(0x0063ba7c).readS32(), cars: [] };
  for (let i=0;i<4;i++){
    const rec = abs(0x0063d9e0 + i*0x2ac);
    const fr = rec.add(0x27c).readPointer();
    const c = { coef: rec.add(0x274).readFloat(), env_tex: texInfo(rec.add(0x278).readPointer()),
      pos258: rd(rec.add(0x258), 3), win200: rd(rec.add(0x200), 8), f288: rec.add(0x288).readFloat(),
      env_frame: fr.toString(), f280: rec.add(0x280).readFloat(), f284: rec.add(0x284).readFloat(),
      i290: rec.add(0x290).readS32(), flags: rec.add(0x294).readU32(),
      p29c: rec.add(0x29c).readPointer().toString(), parts: [] };
    // FUN_0041f120 @ 0x0041f120: *(*(rec+0x29c)+4) +0x40/+0x44/+0x48 is the point the
    // coefficient's distance fade is measured from
    try { const o=rec.add(0x29c).readPointer(); if (!o.isNull()) c.p29c_pos = rd(o.add(4).readPointer().add(0x40), 3); } catch(e) {}
    if (!fr.isNull()) { try { c.env_ltm = mat12(fr.add(0x50)); c.env_m = mat12(fr.add(0x10)); } catch(e) {} }
    for (let p=0;p<0x80;p++){
      const at = rec.add(p*4).readPointer(); if (at.isNull()) continue;
      const geo = at.add(0x18).readPointer(); if (geo.isNull()) continue;
      const nm = geo.add(0x24).readS32(), mats = geo.add(0x20).readPointer();
      const ms = [];
      for (let m=0;m<nm && m<16;m++){
        const mat = mats.add(m*4).readPointer();
        const e = { tex: texName(mat.readPointer()) };
        const fx = mat.add(off).readPointer();
        if (!fx.isNull()) {
          e.eff = fx.add(0x30).readU32();
          for (let s=0;s<2;s++){ const b=fx.add(s*0x18), ty=b.add(0x14).readU32();
            if (ty===2) e['env'+s]={frame:b.readPointer().toString(), tex:texName(b.add(4).readPointer()),
                                     coef:b.add(8).readFloat(), fba:b.add(0xc).readS32()};
            else if (ty===4) e['dual'+s]={tex:texInfo(b.readPointer()), src:b.add(4).readS32(), dst:b.add(8).readS32()};
            else e['t'+s]=ty; }
        }
        ms.push(e);
      }
      c.parts.push({ p: p, gflags: geo.add(8).readU32(), rflags: at.add(2).readU8(), mats: ms });
    }
    g.cars.push(c);
  }
  return g;
}
// [librwrest 2026-10-16] car part frames (PREREG_librwrest.md). Per car record
// rec = 0x0063d9e0 + i*0x2ac (FUN_00420420 @ 0x00420420 scatters atomics by part code into
// rec[part]): the spin angles FUN_0041f3d0 @ 0x0041f3d0 keeps at rec+0x220+w*4, LOD +0x298,
// shade +0x284 (FUN_0041f590), descriptor +0x268 (FUN_00420420: DAT_0063dc48 + i*0x2ac); per
// part atomic: its frame (atomic+4) modelling +0x10 and LTM +0x50, and the parent frame's
// modelling (frame+4 = RwFrame parent; FUN_0041f3d0 rotates it about Y for steer). Vehicle
// physics record veh = 0x008815a0 + i*0xd04: the per-wheel spin int FUN_0046bd60 @ 0x0046bd60
// reads (+0x1a4 + w*0xc4), the 16-float steer window FUN_0046bda0 @ 0x0046bda0 averages
// (+0x1ac + w*0xc4, x 0.0625), and the two vec3s FUN_0041f4f0 @ 0x0041f4f0 reads through
// FUN_0046bce0 (+0xaf4) / FUN_0046bd20 (+0xb00). Light DAT_0063d850 (FUN_00420870 0x0042087c)
// colour at +0x18 (FUN_00420050 scales it by rec+0x284 around each car's render).
let carParts=false, cpRows={}, cpSnaps=[], cpLivery=null;
const CP_PARTS=[0,1,2,3,0x21,0x22,0x23,0x24,0x12,0x13,0x14];
function cpLiveryRead(){
  const out={ mode_word_0067e9fc: abs(0x0067e9fc).readS32(), cars: [] };
  for (let i=0;i<4;i++){
    const rec=abs(0x0063d9e0+i*0x2ac); const c={ colour: abs(0x007f1a1c+i*16).readS32() };
    try { const d=rec.add(0x268).readPointer(); if (!d.isNull()) { c.desc=d.toString();
      c.name=d.readCString(16); c.dff=d.add(0x10).readCString(16); c.txd=d.add(0x20).readCString(16);
      c.env=d.add(0x30).readCString(16); c.id=d.add(0x80).readS32(); } } catch(e) { c.err=''+e; }
    out.cars.push(c); }
  return out; }
function carPartsRead(){
  const cars=[];
  let light=null; try { const l=abs(0x0063d850).readPointer(); if (!l.isNull()) light=rd(l.add(0x18),4); } catch(e) {}
  for (let i=0;i<4;i++){
    const rec=abs(0x0063d9e0+i*0x2ac), veh=abs(0x008815a0+i*0xd04);
    // al: veh+4 = FUN_0046c7b0(i)'s word (the `== 1` gate FUN_0040da50 @ 0x0040da50 tests first)
    const c={ ang: rd(rec.add(0x220),4), lod: rec.add(0x298).readS32(), shade: rec.add(0x284).readFloat(),
      al: veh.add(4).readS32(),
      flags: rec.add(0x294).readU32(), spin: [], steer: [], va: rd(veh.add(0xaf4),3), vb: rd(veh.add(0xb00),3), p: {} };
    for (let w=0;w<4;w++){ c.spin.push(veh.add(0x1a4+w*0xc4).readS32());
      let s=0; for (let k=0;k<16;k++) s+=veh.add(0x1ac+w*0xc4+k*4).readFloat(); c.steer.push(s*0.0625); }
    for (const pc of CP_PARTS){
      try { const at=rec.add(pc*4).readPointer(); if (at.isNull()) continue;
        const fr=at.add(4).readPointer(); const pa=fr.add(4).readPointer();
        c.p[pc]={ m: mat12(fr.add(0x10)), l: mat12(fr.add(0x50)), pm: pa.isNull()?null:mat12(pa.add(0x10)),
                  vis: at.add(2).readU8() }; } catch(e) {} }
    cars.push(c); }
  // [librwrest_checks] units: DAT_007f1008, the 1/3000 s tick count FUN_0040fc00 @ 0x0040fc00 stores
  // from ESI before it calls FUN_0040da50 (decomp: `DAT_007f1008 = unaff_ESI`; the ff4 clock
  // DAT_007f0ff4 += ESI only at the end of the same function, so a row's ff4 is the value BEFORE
  // this frame's increment)
  return { light: light, units: abs(0x007f1008).readS32(), cars: cars }; }
function abs(r){return ptr(r+DELTA);}
function rd(p,n){ const o=[]; for(let i=0;i<n;i++) o.push(p.add(i*4).readFloat()); return o; }
// RwMatrix rows right/up/at/pos at +0x00/+0x10/+0x20/+0x30 -> 12 floats (flags/pads dropped)
function mat12(p){ const m=rd(p,16); return [m[0],m[1],m[2],m[4],m[5],m[6],m[8],m[9],m[10],m[12],m[13],m[14]]; }
rpc.exports={
  config:function(fm, mr, shots, rq, sd, rel, fp, dr, cf, tr, bt){ frameMode=!!fm; maxRows=mr; shotRows={};
    for (const k of shots) shotRows[k]=1; reqPath=rq; shotDir=sd;
    relShots={}; for (const k of (rel||[])) relShots[k]=1; raceStart=-1;
    forceProg=(fp===undefined||fp===null)?-1:fp;
    drawRec=!!dr; carFrames=!!cf; forceTrack=(tr===undefined||tr===null)?-1:tr;
    btRe = bt ? new RegExp(bt, 'i') : null; return 1; },
  // [camribbon] engine track index (scenario_launch.py --track: 0=Training .. 12=City). Writing
  // the menu globals mid-menu stalled the Quick Battle confirm (the race never loaded), so the
  // override is applied at the load itself: onEnter of FUN_0040d440 (Course::LoadCurrent, which
  // loads the course named by 0x0063ba7c, scenario_launch.py:42) writes 0x0063ba7c. The track
  // that actually loaded is read back from the trace's track_type column (DAT_00644158).
  settrack:function(v){
    Interceptor.attach(abs(0x0040d440), { onEnter(){ abs(0x0063ba7c).writeS32(v); } });
    return 1; },
  // [rfx2] --teamplay N: the Team Play word DAT_0067ea64 (FUN_0042f500) written at the same course-load
  // entry, so the race's HUD set-up (FUN_0041b4d0, race state 1) reads N. An INPUT poke like --colours.
  setteamplay:function(v){
    Interceptor.attach(abs(0x0040d440), { onEnter(){ abs(0x0067ea64).writeS32(v); } });
    return 1; },
  tracelen:function(){ return trace.length; },
  setmatfx:function(v){ matfxOn=!!v; return 1; },
  getmatfx:function(){ return JSON.stringify(matfxSnaps); },
  // [librwrest_checks] --colours / --livery-mode: FUN_0040d110 @ 0x0040d110 reads the per-car
  // colour DAT_007f1a1c[i*4] (0x0040d110 loop, stride 0x10) and FUN_0042f6a0 (== 0xb) at the
  // moment it picks each car's vehicle id. The poke writes those inputs at its entry (and puts
  // the mode word back at its exit) so the ORIGINAL's own selection runs on non-zero colours.
  // A colour < 0 leaves that car's value alone; mode < 0 leaves the mode word alone.
  setliverypoke:function(cols, mode){
    Interceptor.attach(abs(0x0040d110), {
      onEnter(){ this.mode0 = abs(0x0067e9fc).readS32();
        for (let i=0;i<4;i++) if (cols[i] >= 0) abs(0x007f1a1c+i*16).writeS32(cols[i]);
        if (mode >= 0) abs(0x0067e9fc).writeS32(mode); },
      onLeave(){ if (mode >= 0) abs(0x0067e9fc).writeS32(this.mode0); } });
    return 1; },
  setcarparts:function(v){ carParts=!!v; return 1; },
  getcarparts:function(){ return JSON.stringify({livery: cpLivery, rows: cpSnaps}); },
  getdraws:function(){ return JSON.stringify(drFrames); },
  // [d1r1] per named texture: the D3D9 raster extension's alpha byte (ext+8, the flag
  // FUN_004d6ce0 tests at 0x004d6d06 to switch ALPHABLEND/ALPHATEST for stage 0), and the
  // raster's width/height/depth (+0xc/+0x10/+0x14) and cFormat byte (+0x23)
  texalpha:function(){ const ext=abs(0x00911ae4).readU32(); const out={};
    for (const k in texByPtr) { try { const r=ptr(k).readPointer(); if (r.isNull()) continue;
      const nm=texByPtr[k];
      out[nm]=[r.add(ext+8).readU8(), r.add(0xc).readS32(), r.add(0x10).readS32(), r.add(0x14).readS32(), r.add(0x23).readU8()];
    } catch(e) {} } return JSON.stringify(out); },
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
            // [capfix] reqPath must be ABSOLUTE: the game's CWD is original\, so a relative path
            // made this throw ENOENT at the first shot row. The throw skipped trace.push, the trace
            // froze at that row on every later director call (looking like "the director stopped"),
            // and no shot fired. A failed request now marks the row (shot = -1) instead of
            // dropping it.
            try { const f = new File(reqPath, 'w'); f.write(nm + '\n'); f.close(); row.shot = 1; }
            catch(e) { row.shot = -1; shotErrs.push(trace.length + ': ' + e); }
          }
          // [d1r1] car render roots (I-3) and the draw recorder's frame markers (I-1, I-FRAME)
          if (carFrames) row.cm = carMats();
          if (carParts) {   // [librwrest] window of 60 rows ending at each shot row
            const n = trace.length;
            if (cpLivery === null && row.ba8c >= 3) { try { cpLivery = cpLiveryRead(); } catch(e) { cpLivery = {err: ''+e}; } }
            let win = false;
            for (const k in shotRows) if (n <= +k && n >= +k - 60) win = true;
            if (raceStart >= 0) for (const k in relShots) { const a = raceStart + (+k); if (n <= a && n >= a - 60) win = true; }
            if (win) { try { cpSnaps.push({row: n, ba8c: row.ba8c, ff4: row.ff4, s: carPartsRead()}); }
                       catch(e) { cpSnaps.push({row: n, err: '' + e}); } } }
          if (matfxOn && trace.length % 100 === 0) {
            try { matfxSnaps.push({row: trace.length, ba8c: row.ba8c, s: carMatfx()}); }
            catch(e) { matfxSnaps.push({row: trace.length, err: '' + e}); } }
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
  shoterrs:function(){ return JSON.stringify(shotErrs); },
  globals:function(){ return JSON.stringify({
    t_7f1030: abs(0x007f1030).readU32(),
    jitter_7f0fc8: abs(0x007f0fc8).readFloat(),
    blend_7f100c: abs(0x007f100c).readFloat(),
    mode_7f0fd0: abs(0x007f0fd0).readS32(),
    phase_63ba8c: abs(0x0063ba8c).readS32(),
    nodecount: abs(NCOUNT).readS32(),
    elim_898980: abs(0x00898980).readFloat() }); },
  // [rfx 2026-10-16] the in-race HUD's inputs and state (verify/rfx_20261016/PREREG_rfx.md
  // amendment 1), read-only: team play DAT_0067ea64 (FUN_0042f500), the per-player slot table
  // 0x007f1a14 (stride 0x10: +0 slot, +4 team, +8 colour), scores DAT_008a94e0[4] (FUN_0040b6d0),
  // DAT_008a94d0 (FUN_0040e340), DAT_0067e9fc (FUN_0042f6a0), the leader flags DAT_0063cd90[4],
  // the start-light timer DAT_0063d588, and both team-panel records 0x0063cab8 (stride 0x16c:
  // members +0..+0xc, count +0xc, visibility +0x10..+0xac, position +0x150, flags +0x168).
  hudglobals:function(){ const rd=(a)=>abs(a).readS32();
    const slots=[]; for(let i=0;i<4;i++){ const b=0x007f1a14+i*16;
      slots.push([rd(b), rd(b+4), rd(b+8), rd(b+12)]); }
    const panels=[]; for(let p=0;p<2;p++){ const b=0x0063cab8+p*0x16c; const vis=[];
      for(let k=0;k<40;k++) vis.push(rd(b+0x10+k*4));
      panels.push({members:[rd(b),rd(b+4),rd(b+8)], count:rd(b+0xc), vis:vis,
        pos:[abs(b+0x150).readFloat(),abs(b+0x154).readFloat(),abs(b+0x158).readFloat()],
        idx:rd(b+0x164), flags:rd(b+0x168)>>>0}); }
    return JSON.stringify({row: trace.length, ba8c: rd(0x0063ba8c), ea64: rd(0x0067ea64),
      e9fc: rd(0x0067e9fc), fd0: rd(0x007f0fd0), n4d0: rd(0x008a94d0),
      scores:[rd(0x008a94e0),rd(0x008a94e4),rd(0x008a94e8),rd(0x008a94ec)],
      leaders:[rd(0x0063cd90),rd(0x0063cd94),rd(0x0063cd98),rd(0x0063cd9c)],
      timer: abs(0x0063d588).readFloat(), slots: slots, panels: panels}); },
  // [rfx2] read-only witness of the in-race FX inputs (PREREG_rfx2.md): the dynamic-object shadow
  // arrays of FUN_00481a30 @ 0x00481a30 (count DAT_006c6eb0, atomic 0x006c71d8, id 0x006c6b90,
  // frustum state 0x006c3350 / previous 0x006c3030, shadow flag 0x006cad50, bbox 0x006c9a88 (0x18),
  // shadow matrix 0x006c3670 (0x40), triangle cache 0x006c74f8 (0x28), reset matrix 0x006cb070),
  // the course light DAT_0065743c (FUN_004260b0) and its frame LTM, and the RWObjShad batch
  // DAT_00703068 (added this tick DAT_00703058; matrices [+0x1c], colours [+0x18]).
  fxstate:function(){ const r32=(a)=>abs(a).readS32(); const u32=(a)=>abs(a).readU32();
    const out={row: trace.length, f101c: r32(0x007f101c), n: r32(0x006c6eb0)};
    try{ const L=abs(0x0065743c).readPointer(); out.light_ptr=L.toString();
      if(!L.isNull()){ const fr=L.add(4).readPointer(); out.light_sub=L.add(1).readU8();
        out.light_ltm=rd(fr.add(0x50),16); out.light_mod=rd(fr.add(0x10),16); } }catch(e){ out.light_err=''+e; }
    const plug=u32(0x007dc8d8); const objs=[];
    for(let i=0;i<Math.min(out.n,200);i++){ const o={i:i};
      try{ const a=abs(0x006c71d8+i*4).readPointer(); o.atomic=a.toString();
        o.id=r32(0x006c6b90+i*4); o.st=r32(0x006c3350+i*4); o.prev=r32(0x006c3030+i*4);
        o.flag=r32(0x006cad50+i*4); o.f9438=r32(0x006c9438+i*4); o.slot=r32(0x006c9758+i*4);
        o.bbox=rd(abs(0x006c9a88+i*0x18),6); o.shm=rd(abs(0x006c3670+i*0x40),16);
        o.init=rd(abs(0x006cb070+i*0x40),16);
        const cb=abs(0x006c74f8+i*0x28); o.cache=rd(cb,10); o.cache_i=[cb.add(0x18).readS32()];
        const vp=[]; for(let k=7;k<10;k++){ const p=cb.add(k*4).readPointer();
          vp.push(p.isNull()?null:rd(p,3)); } o.cache_v=vp;
        if(!a.isNull()){ o.aflags=a.add(2).readU8(); const fr=a.add(4).readPointer();
          if(!fr.isNull()) o.ltm=rd(fr.add(0x50),16);
          const ph=a.add(plug).readPointer(); o.phys=ph.toString();
          if(!ph.isNull()){ const p10=ph.add(0x10).readPointer();
            o.p10_4=p10.add(4).readPointer().toString(); const pr=p10.readPointer();
            o.prim_type=pr.add(0x5c).readPointer().toString(); o.prim_dims=rd(pr.add(0x40),4);
            o.prim_m=rd(pr,16); } } }catch(e){ o.err=''+e; }
      objs.push(o); }
    out.objs=objs;
    // the race camera DAT_006905b0: viewMatrix +0x20, raster +0x60, viewWindow +0x68, recip +0x70,
    // offset +0x78, near/far +0x80/+0x84, zScale/zShift +0x8c/+0x90; frame modelling / LTM; and
    // RwGlobals DAT_007d3ff8 +0x18/+0x1c (FUN_004128f0's z range)
    try{ const C=abs(0x006905b0).readPointer(); const fr=C.add(4).readPointer(); const ras=C.add(0x60).readPointer();
      out.cam={view:rd(C.add(0x20),16), vw:rd(C.add(0x68),2), rvw:rd(C.add(0x70),2), off:rd(C.add(0x78),2),
        nf:rd(C.add(0x80),2), zss:rd(C.add(0x8c),2), mod:rd(fr.add(0x10),16), ltm:rd(fr.add(0x50),16),
        ras:[ras.add(0xc).readS32(), ras.add(0x10).readS32()]};
      const G=abs(0x007d3ff8).readPointer(); out.zbuf=rd(G.add(0x18),2); }catch(e){ out.cam_err=''+e; }
    // Headlight (FUN_00412f30 / FUN_00413a00): direct records 0x0089bea0 (count DAT_0063bc58, 0x2c
    // each), reflections 0x0089a8a0 (DAT_0063bc5c), screen quads 0x00828320 (DAT_008a94a0, 0x70)
    try{ const nd=r32(0x0063bc58), nr=r32(0x0063bc5c), nq=r32(0x008a94a0); out.hl={nd:nd, nr:nr, nq:nq, d:[], q:[]};
      for(let j=0;j<Math.min(nd,127);j++){ const b=abs(0x0089bea0+j*0x2c); out.hl.d.push({f:rd(b,9), c:b.add(0x24).readU32(), fl:b.add(0x28).readFloat()}); }
      for(let j=0;j<Math.min(nq,127);j++) out.hl.q.push(rd(abs(0x00828320+j*0x70),28));
      const cars=[]; for(let i=0;i<4;i++){ const sl=r32(0x007f1a14+i*16); const parts={};
        for(const p of [0x19,0x1a,0x1b,0x1c,0x2a,0x37,0x38,0x39,0x3a]){ const a=abs(0x0063d9e0+(i*0xab+p)*4).readPointer();
          if(!a.isNull()){ const pf=a.add(4).readPointer(); parts[p]=pf.isNull()?null:rd(pf.add(0x50),16); } }
        cars.push({hlflag:r32(0x0063bc44+i*4), flags:u32(0x0063dc74+i*0x2ac), slot:sl,
          brake:(sl>=0&&sl<16)?abs(0x007f103c+sl*0x4c+1).readU8():-1, parts:parts}); }
      out.hl.cars=cars; }catch(e){ out.hl_err=''+e; }
    // lensflare-fs8 light list (FUN_00488e70): 128 entries of 0x38 at 0x0086ae24, enabled at +0x10
    try{ const L=[]; for(let j=0;j<128;j++){ const b=abs(0x0086ae24+j*0x38); if(b.add(0x10).readS32()===0) continue;
        const e={j:j, pos:rd(b,3), n:b.add(0xc).readS32(), k:b.add(0x14).readFloat(), w:[b.add(0x18).readS32(), b.add(0x1c).readS32(), b.add(0x20).readS32(), b.add(0x24).readS32(), b.add(0x28).readU32()]};
        const fm=b.add(0x28).readPointer(); if(!fm.isNull()) e.m=rd(fm,16);
        const nn=Math.min(e.n,8); const sz=b.add(0x1c).readPointer(), uv=b.add(0x20).readPointer(), co=b.add(0x24).readPointer();
        if(!sz.isNull()) e.sz=rd(sz,Math.max(nn,5)); if(!uv.isNull()){ e.uv=[]; for(let k=0;k<nn;k++) e.uv.push(uv.add(k*4).readS32()); }
        if(!co.isNull()){ e.co=[]; for(let k=0;k<nn;k++) e.co.push(co.add(k*4).readU32()); }
        L.push(e); } out.lf=L; }catch(e){ out.lf_err=''+e; }
    try{ const k=r32(0x00703058); out.shad_n=k; out.shad_cap=r32(0x00703070);
      const mp=abs(0x00703084).readPointer(), cp=abs(0x00703080).readPointer(); const sh=[];
      for(let j=0;j<Math.min(k,64);j++) sh.push({m:mp.isNull()?null:rd(mp.add(j*0x40),16),
        c:cp.isNull()?null:cp.add(j*4).readU32()}); out.shad=sh; }catch(e){ out.shad_err=''+e; }
    return JSON.stringify(out); },
  // [rres 2026-10-16] read-only witness of the car shadow render-to-texture (PREREG_rres.md): per player p
  // the record rec = 0x0063d9e0 + p*0x2ac (fade rec+0x284/0x288, hull index +0x28c, slot +0x290, mode +0x298,
  // car position +0x258, camera +0x2a0, texture +0x2a8), the chosen RwCamera (viewMatrix +0x20, projection
  // +0x14, viewWindow +0x68, near/far +0x80/+0x84, frame modelling +0x10 / LTM +0x50 and raster size), the
  // hull atomic rec[idx] (frame LTM, geometry), the batch of FUN_00422570 / FUN_004228f0 for slot p
  // (count 0x00641318, colour 0x00641310, previous query position 0x006403b8 + p*0xc, entries 0x006403e8,
  // stride 0x3c: v0 v1 v2 (9 floats) then u0 v0 u1 v1 u2 v2), the gates DAT_00636acc / DAT_00657490, the
  // worlds DAT_0065742c / DAT_00656ee8, the light DAT_0063d850 (frame modelling / LTM) and the floats
  // 0x00644368 / 0x0064435c / 0x00644360 / 0x006146fc.
  shadowstate:function(){ const r32=(a)=>abs(a).readS32(); const u32=(a)=>abs(a).readU32();
    const out={row: trace.length, ba8c: r32(0x0063ba8c), g636acc: r32(0x00636acc), g657490: r32(0x00657490),
      w65742c: u32(0x0065742c), w656ee8: u32(0x00656ee8), w80332c: u32(0x0080332c),
      f644368: abs(0x00644368).readFloat(), f64435c: abs(0x0064435c).readFloat(), f644360: abs(0x00644360).readFloat(),
      up: rd(abs(0x006146fc),3)};
    try{ const L=abs(0x0063d850).readPointer(); out.light_ptr=L.toString();
      if(!L.isNull()){ const fr=L.add(4).readPointer(); out.light_mod=rd(fr.add(0x10),16); out.light_ltm=rd(fr.add(0x50),16); } }catch(e){ out.light_err=''+e; }
    const cams=[]; for(let b=0;b<2;b++){ try{ const c=abs(0x0063e490+b*4).readPointer(); const t=abs(0x0063d854+b*4).readPointer();
      const fr=c.add(4).readPointer(); const ras=c.add(0x60).readPointer();
      cams.push({ptr:c.toString(), tex:t.toString(), proj:c.add(0x14).readS32(), view:rd(c.add(0x20),16), vw:rd(c.add(0x68),2),
        nf:rd(c.add(0x80),2), mod:rd(fr.add(0x10),16), ltm:rd(fr.add(0x50),16), ras:[ras.add(0xc).readS32(), ras.add(0x10).readS32(), ras.add(0x14).readS32(), ras.add(0x23).readU8()]}); }catch(e){ cams.push({err:''+e}); } }
    out.cams=cams;
    const pl=[]; for(let p=0;p<4;p++){ const rec=abs(0x0063d9e0+p*0x2ac); const o={p:p};
      try{ o.f284=rec.add(0x284).readFloat(); o.f288=rec.add(0x288).readFloat(); o.idx=rec.add(0x28c).readS32();
        o.slot=rec.add(0x290).readS32(); o.flags=rec.add(0x294).readU32(); o.mode=rec.add(0x298).readS32();
        o.pos=rd(rec.add(0x258),4); o.cam=rec.add(0x2a0).readU32(); o.tex=rec.add(0x2a8).readU32(); o.fr26c=rec.add(0x26c).readU32();
        const hulls=[]; for(let k=0x3b;k<=0x3e;k++){ const a=rec.add(k*4).readPointer(); const h={k:k,ptr:a.toString()};
          if(!a.isNull()){ const fr=a.add(4).readPointer(); h.aflags=a.add(2).readU8(); if(!fr.isNull()) h.ltm=rd(fr.add(0x50),16);
            const g=a.add(0x18).readPointer(); if(!g.isNull()){ h.gflags=g.add(8).readU32(); h.ntri=g.add(0x10).readS32(); h.nvert=g.add(0x14).readS32(); } }
          hulls.push(h); }
        o.hulls=hulls;
        const carf=rec.add(0x26c).readPointer(); if(!carf.isNull()){ const f2=carf.add(4).readPointer(); if(!f2.isNull()) o.carmod=rd(f2.add(0x10),16); }
        const base=0x006403e8+p*0xf40; o.cnt=r32(0x00641318+p*0xf40); o.col=u32(0x00641310+p*0xf40); o.bias=abs(0x00641324+p*0xf40).readFloat();
        o.lastq=rd(abs(0x006403b8+p*12),3); o.texidx=r32(0x0064131c+p*0xf40); o.texptrs=[0,1,2,3].map(k=>u32(0x006412e8+p*0xf40+k*4));
        o.camsaved=u32(0x00641320+p*0xf40);
        const ents=[]; for(let k=0;k<Math.min(o.cnt,64);k++) ents.push(rd(abs(base+k*0x3c),15)); o.ents=ents;
      }catch(e){ o.err=''+e; }
      pl.push(o); }
    out.pl=pl;
    return JSON.stringify(out); },
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
    ap.add_argument("--boot-wait", type=float, default=60.0,
                    help="[capfix] seconds to wait for the title screen (was a fixed 20 s; a loaded "
                         "machine boots slower than that and every later nav step then times out)")
    ap.add_argument("--force-prog", type=float, default=-1.0,
                    help="[camribbon] pin every car's path progress to this value for each "
                         "director call (see AGENT forceProg); -1 = off")
    ap.add_argument("--drawrec", action="store_true",
                    help="[d1r1] record every D3D9 draw of each shot frame (PREREG_d1r1 I-1) "
                         "into <out_dir>/orig_row_<K>.draws.json")
    ap.add_argument("--carframes", action="store_true",
                    help="[d1r1] per-row car render-root matrices (R{i}_* modelling, "
                         "Q{i}_* LTM, F{i}_* the renderable's own frame LTM, S{i} root==frame)")
    ap.add_argument("--carparts", action="store_true",
                    help="[librwrest] per-car livery descriptor (once) and, for the 60 rows "
                         "ending at each shot row, spin angles / steer / body vecs and the "
                         "part frames' modelling matrices into <out>.carparts.json "
                         "(PREREG_librwrest.md)")
    ap.add_argument("--colours", default="",
                    help="[librwrest_checks] comma list of 4 per-car colours (a number, or '-' to "
                         "leave a car alone) written to DAT_007f1a1c[i*4] at FUN_0040d110's entry, "
                         "so the original's livery selection runs on non-zero colours "
                         "(PREREG_librwrest_checks.md)")
    ap.add_argument("--livery-mode", type=int, default=-1,
                    help="[librwrest_checks] write this value to the mode word 0x0067e9fc for the "
                         "duration of FUN_0040d110 only (11 = the 0xb arm: livery = slot index)")
    ap.add_argument("--matfx", action="store_true",
                    help="[rtex] every 100 trace rows, snapshot the cars' MatFX state (env "
                         "coefficient/texture/frame, per-part material effects) into "
                         "<out>.matfx.json (PREREG_rtex section 5)")
    ap.add_argument("--hud-out", default="",
                    help="[rfx] with --early: sample the in-race HUD inputs (hudglobals) 4x/s "
                         "into this JSON, each sample tagged with the trace row count")
    ap.add_argument("--teamplay", type=int, default=-1,
                    help="[rfx2] write this value to the Team Play word 0x0067ea64 at the course load "
                         "(FUN_0040d440 entry), e.g. 0 for the non-team HUD panels; -1 = leave it")
    ap.add_argument("--fx-out", default="",
                    help="[rfx2] with --early: sample the FX witness (fxstate: object shadows, "
                         "course light, RWObjShad batch) every 0.5 s into this JSON")
    ap.add_argument("--shadow-out", default="",
                    help="[rres] with --early: sample the car shadow render-to-texture witness (shadowstate) "
                         "every 0.5 s into this JSON")
    ap.add_argument("--drawrec-bt", default="",
                    help="[d1r1] regex on stage-0 texture names: record a return-address "
                         "backtrace (MASHED.exe VAs) for each matching draw")
    args = ap.parse_args()
    out_csv, out_static = Path(args.out), Path(args.static_out)
    shots = [int(s) for s in args.shots.split(",") if s.strip()]
    rel = [int(s) for s in args.shots_race.split(",") if s.strip()]
    if shots or rel or args.carframes or args.carparts: args.frame = True
    LOG.mkdir(exist_ok=True)
    out_csv.parent.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    env.setdefault("MASHED_WIN_POS", "primary-bl")
    # [capfix] absolute: the game runs with CWD = original\, and both the agent's File() and the
    # d3d9 shim's fopen/GetFileAttributes resolve this string against THAT directory
    req = (out_csv.parent / "orig_bbdump.req").resolve()
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
    # [capfix] script errors are collected and printed: onLeave exceptions were swallowed here, so
    # a throwing callback looked like "the director is no longer called" (the trace just stops growing)
    agent_errors = []
    def on_msg(m, d):
        if m.get("type") == "error":
            agent_errors.append(m)
            if len(agent_errors) <= 5:
                print(f"[agent error] {m.get('description')} @ {m.get('fileName')}:{m.get('lineNumber')}")
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    scr.exports_sync.init()
    scr.exports_sync.config(args.frame, args.rows, shots, str(req) if (shots or rel) else "",
                            str(out_csv.parent.resolve()), rel,
                            args.force_prog if args.force_prog >= 0 else -1,
                            args.drawrec, args.carframes, args.track, args.drawrec_bt)
    if args.matfx:
        scr.exports_sync.setmatfx(1)
    if args.carparts:
        scr.exports_sync.setcarparts(1)
    poke_cols = [-1, -1, -1, -1]
    if args.colours:
        toks = [t.strip() for t in args.colours.split(",")]
        poke_cols = [(-1 if t in ("-", "") else int(t)) for t in toks][:4]
        poke_cols += [-1] * (4 - len(poke_cols))
    if args.colours or args.livery_mode >= 0:
        scr.exports_sync.setliverypoke(poke_cols, args.livery_mode)
        print(f"livery poke: colours {poke_cols} mode {args.livery_mode}")
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
    wait(lambda: E.phase() == 3 and E.depth() >= 1, args.boot_wait, "title")
    time.sleep(1.0)
    confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
    confirm_to(3)
    E.setsel(1); time.sleep(0.3)        # Quick Battle
    confirm_to(4, 4)
    confirm_to(5, 4)
    if args.teamplay >= 0:
        E.setteamplay(args.teamplay)
        print(f"team play written at the course load: {args.teamplay}")
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
        last_n, last_t, last_f = 0, time.time(), E.fires()
        hud = []
        fx = []
        shad = []
        while E.tracelen() < args.rows and time.time() - t0 < args.rows / 30.0 + 60:
            if args.hud_out:
                # [rfx] sample the HUD inputs 4x per second, tagged with the trace row count
                for _ in range(4):
                    hud.append(json.loads(E.hudglobals()))
                    time.sleep(0.25)
            elif args.shadow_out:
                # [rres] the shadow render-to-texture witness twice per second, tagged with the trace row count
                for _ in range(2):
                    shad.append(json.loads(E.shadowstate()))
                    time.sleep(0.5)
            elif args.fx_out:
                # [rfx2] the FX witness twice per second, tagged with the trace row count
                for _ in range(2):
                    fx.append(json.loads(E.fxstate()))
                    time.sleep(0.5)
            else:
                time.sleep(1.0)
            n_now = E.tracelen()
            if n_now != last_n:
                last_n, last_t, last_f = n_now, time.time(), E.fires()
            elif n_now > 100 and time.time() - last_t > float(os.environ.get("PROBE_STALL_S", "8.0")):
                # [capfix] say which kind of stall: the director's fire counter still advancing
                # while no rows are added means the agent's onLeave is throwing, not that the game
                # stopped calling the director
                f_now = E.fires()
                print(f"trace stalled at {n_now} rows (director fires {last_f} -> {f_now}: "
                      f"{'STILL CALLED, the agent callback is failing' if f_now != last_f else 'no longer called'})")
                break
    else:
        time.sleep(2.0)
        E.hookcam()
        time.sleep(3.0)
        fires3s = E.fires()
        print(f"FUN_00446520 fires in 3s: {fires3s}")

    if args.early and args.hud_out:
        Path(args.hud_out).write_text(json.dumps(hud, indent=0))
        print(f"[rfx] HUD samples: {len(hud)} -> {args.hud_out}")
    if args.early and args.shadow_out:
        Path(args.shadow_out).write_text(json.dumps(shad))
        print(f"[rres] shadow samples: {len(shad)} -> {args.shadow_out}")
    if args.early and args.fx_out:
        Path(args.fx_out).write_text(json.dumps(fx))
        print(f"[rfx2] FX samples: {len(fx)} -> {args.fx_out}")
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
    if args.carparts:
        cp = json.loads(E.getcarparts())
        cp["poke"] = {"colours": poke_cols, "mode": args.livery_mode}
        Path(str(out_csv) + ".carparts.json").write_text(json.dumps(cp))
        print(f"carparts: {len(cp.get('rows') or [])} rows, livery={cp.get('livery')} "
              f"-> {out_csv}.carparts.json")
    if args.matfx:
        mf = json.loads(E.getmatfx())
        Path(str(out_csv) + ".matfx.json").write_text(json.dumps(mf, indent=1))
        print(f"matfx snapshots: {len(mf)} -> {out_csv}.matfx.json")
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
    shot_fail = False
    if shots or rel:
        end = time.time() + 5.0
        while req.exists() and time.time() < end: time.sleep(0.1)
        fired = [i for i, r in enumerate(rows) if r.get("shot") == 1]
        print(f"shot rows requested: abs {shots} race-relative {rel}; fired at {fired}")
        for k in fired:
            bmp = out_csv.parent / f"orig_row_{k:05d}.bmp"
            ok = bmp.exists()
            print(f"shot row {k} (ba8c={rows[k]['ba8c']}): {'OK' if ok else 'MISSING'} {bmp}")
            if not ok: shot_fail = True
        # [capfix] a requested shot that did not fire must fail the run, not pass silently
        if len(fired) != len(shots) + len(rel):
            shot_fail = True
            print(f"\n*** SHOT CAPTURE FAILED: {len(shots) + len(rel)} requested, {len(fired)} fired "
                  f"(trace has {len(rows)} rows) ***")
        se = json.loads(E.shoterrs())
        if se: print(f"[agent] shot request errors: {se[:3]}")
    if agent_errors:
        print(f"[agent] {len(agent_errors)} script error(s); first: {agent_errors[0].get('description')}")
    tf = json.loads(E.trackfiles())
    print(f"[d1r1] track piz opened: {tf}  (forced index {args.track})  textures named: {E.texcount()}")
    (out_csv.parent / (out_csv.stem + ".tracks.json")).write_text(json.dumps(tf))
    if args.drawrec:
        (out_csv.parent / (out_csv.stem + ".texalpha.json")).write_text(E.texalpha())
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
    return 2 if shot_fail else 0


if __name__ == "__main__":
    sys.exit(main())
