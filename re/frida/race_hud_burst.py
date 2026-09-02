# race_hud_burst.py — capture the ORIGINAL's in-race HUD draw stream (quads AND
# text) with retaddr attribution, for the parity harness.
#
# WHY THIS EXISTS
# ---------------
# menu_draw_burst.py is the only original-side Im2D draw-list burst and it is
# frontend-gated: its generic +0x30 device hook WOULD fire in a race, but
# `collecting` is only armed by the ShellA frame delimiter FUN_0042e3a0, a
# frontend function, so in a race it never arms. race_draw_burst.py is NOT the
# sibling tool -- it captures 3D draw-call TOTALS via the d3d9 shim slot
# counters plus a backbuffer BMP, not a draw list. Nothing in re/frida/ or
# re/tools/ captured an in-race 2D draw list before this.
#
# MEASURED FACTS THIS TOOL IS BUILT ON (MASHED_COUNT_RVAS, mode 10, 4 cars,
# ~20 s / ~1250 frames, 2026-09-02):
#     0x0040dfc0 HudIngameDispatch      1254
#     0x00450b10 HudIm2DQuad            2003
#     0x00556ca0 font-ctx thunk         1352
#     0x00554940 charset glyph renderer 1352
#     0x004cd070 RwRenderPrimitiveSubmit 7158
#     0x0041db80 post-switch tail       1106
#     0x00427f00 / 0x00427ff0              0    <- dead in-race
#     0x0041e850 / 0x0041ded0              0    <- dead in-race
#
# TWO CHANNELS, because text is invisible on the device slot:
#   Quad channel: device vtable *(0x007d3ff8) + 0x30 = RwIm2DRenderPrimitive.
#     HudIm2DQuad 0x00450b10 ends in rw_draw_4verts -> that slot
#     (DrawQuadPrimitives.cpp:506-507). Emits the same "v"/"r" schema as
#     menu_draw_burst.py so re/tools/drawlist_diff.py decodes both with one
#     decoder.
#   Text channel: FUN_00556ca0, the font-ctx thunk. The original's glyphs go
#     0x00554940 -> FUN_004cd070 (Im3D), which does NOT pass through the +0x30
#     draw (MenuDrawLoopTwin.cpp:114-117). A device-slot capture therefore
#     cannot see a single digit or label, and since the in-race HUD is mostly
#     digits and labels a quad-only diff would score GREEN while every number
#     on screen was wrong.
#     We hook the thunk rather than 0x00554940 itself because 0x00554940 has NO
#     Ghidra function (label only, U-1067) and its args are listing-inferred,
#     whereas the thunk's 5-arg form (ctx, str_buf, scale, xy_coords*, style)
#     comes from a DECOMPILED call site in the 0x00427f00 plate (:35).
#
# WHAT DAT_0063ba8c AND 0x0040dfc0 ACTUALLY DO (measured; earlier notes here
# claimed the opposite and were wrong)
# ----------------------------------------------------------------------------
# DAT_0063ba8c == 3 while DRIVING, and 5/6/7 on the between-round STANDINGS
# screen. HudIngameDispatch's guard needs {5,6,7}, so it early-returns for the
# whole driving phase (0 of 321 calls passed) and runs only on standings. It is
# the standings dispatcher, NOT a driving HUD dispatcher.
# Backbuffer dumps settle it: while driving there is NO 2D HUD at all (one
# fully-transparent 512x512 quad per frame, font pipe silent); the rich UI
# (letterbox bands, "MASHED"/"Current Standings"/" Continue", per-car icons,
# score bars, +2/+1/-1/-2 circles) is the standings screen at guard 7, which
# needs a ~28 s settle to reach.
#
# FRAME BOUNDARIES: use the shim's Present counter, not an anchor.
# ---------------------------------------------------------------
# MASHED has no verified once-per-frame function. Both anchors tried failed:
# 0x004c1be0 fires ~5-10x per frame (quad_out:quad_in ran ~9:1, so every
# captured "frame" was a sub-frame slice), and 0x00492e90 is no better. Nor can
# DAT_0063ba8c gate frames -- it is mutated MANY times per frame, so gating on
# == 7 skipped 344 of 347 anchor hits.
# Present is the only true frame boundary, so the d3d9 shim now exports the
# ADDRESS of its Present counter (MashedShim_PresentCounter, d3d9_shim.cpp) and
# every captured draw is tagged with its frame index in the "f" field. Prefer
# --free-run (no bracketing) and group by "f" offline. --anchor is retained only
# for comparison against the old, unreliable behaviour.
# [UNCERTAIN] arg4 (xy_coords) float layout is undocumented -> first 16 bytes
#   dumped RAW, no meaning assigned to any slot. String may be 8-bit or UTF-16
#   (FontText_UTF16WidenCopy exists in HudBatch.cpp) -> raw bytes plus BOTH
#   decodings, without picking one.
#
# COVERAGE COUNTERS are armed before the first run, per the standing rule: a
# bare "0 draws" cannot separate "the HUD drew nothing" from "the hook never
# fired". The guard diagnostic is the control for the HUD-entry question.
#
# Race warp is scenario_launch.py's, NOT the nav recipe: subprocess.Popen (never
# frida.spawn -- perturbs boot layout), poke PHASE 0x00771968 = 2, wait 3.
#
# Usage:
#   py -3.12 re/frida/race_hud_burst.py [--frames K] [--label NAME]
#       [--track N] [--mode N] [--cars N] [--settle SECS] [--anchor 0xRVA]
#
# Output:
#   log/race_hud_burst.json  { "<label>_f<i>": [ {"v","r"}, ... ] }   quads
#   log/race_hud_text.json   { "<label>_f<i>": [ {"str_*","xy_raw",...} ] } text
#   log/race_hud_attrib.json  retaddr -> count, per channel (who draws what)
import argparse
import json
import os
import struct
import subprocess
import sys
import time
from collections import Counter
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
GAME_ROOT = Path(os.environ.get("MASHED_ROOT", ROOT))
EXE = GAME_ROOT / "original" / "MASHED.exe"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;

// --- scenario_launch.py warp globals (cited in that file at the given lines) --
const PHASE=0x00771968;      // :41  1=menu 2=load+spawn 3=race running
const TRACK_ENG=0x0063ba7c;  // :43
const TRACK_MENU=0x0067f17c; // :44
const MODE=0x0067e9fc;       // :45
const RULE=0x007f0fd0;       // :46
const CAR_P0=0x0067ea98;     // :47
const DIFFICULTY=0x0067ea7c; // :48
const POWERUPS=0x0067ea80;   // :49
const TEAM=0x0067ea64;       // :51
const SLOT_FN=0x0040e480;    // FUN_0040e480(slot, val): 1=human 2=AI 0=empty
const RES_RVA=0x00497310;    // input poll; return-override = synthetic press
const SPAWN_RVA=0x0046b540;  // VehicleSpawnInit, counts car spawns

// --- nav driver globals (nav_agent.js:6-8) ---------------------------------
// The NORMAL menu->race transition, as opposed to the PHASE poke. Needed
// because a warped race appears to draw no HUD at all (see the driver note in
// the header): different phase global from PHASE above.
const NAV_DEPTH=0x0067e9f8, NAV_PHASE=0x0067eca4, NAV_CURSOR=0x0067ed80;

// --- capture ---------------------------------------------------------------
const VTBL=0x007d3ff8, VBUF=0x00898a20, STRIDE=0x1c;
// Text channel. MEASURED 2026-09-02: FUN_00556ca0 gets ZERO calls during a
// running race (0 inside AND 0 outside the bracket) even though a whole-run
// MASHED_COUNT_RVAS put it at 1352 -- those are menu/load frames. 0x00554940 is
// stored at font_ctx+0x138 and invoked through that function POINTER, so the
// in-race HUD reaches it from a different caller than the thunk. Hooking the
// glyph renderer itself catches every caller regardless, and its retaddr chain
// is what names the real in-race text emitter.
// Args are listing-inferred (U-1067, no Ghidra function at 0x00554940) but the
// two cited stack offsets cross-validate: arg1 read at [ESP+0xD4] after
// SUB ESP,0xC8 + 2 pushes, arg2 at [ESP+0xDC] after 3 pushes, both => on entry
// [esp+4]=font_ctx, [esp+8]=str_buf.
const RVA_TEXT=0x00556ca0;      // thunk (kept: proves the menu-only finding)
const RVA_GLYPH=0x00554940;     // per-string glyph renderer, all callers
const RVA_IM3D=0x004cd070;      // RwRenderPrimitiveSubmit (Im3D staging)
// HUD-entry guard diagnostic (HudDispatch.cpp:133-141).
const RVA_HUD=0x0040dfc0, DAT_63ba8c=0x0063ba8c, DAT_66d704=0x0066d704;

let slotFn=null, pressCtrl=-1, pressUntil=0, spawnFired=0;
let collecting=0, cur=null, quads={}, texts={}, im3ds={}, order=[], hooked=0;
let cov={frames:0, quad_in:0, quad_out:0, text_in:0, text_out:0,
         glyph_in:0, glyph_out:0, im3d_in:0, im3d_out:0, im3d_err:0,
         quad_err:0, text_err:0, glyph_err:0, xorfold:0, frame_skip:0,
         hud_calls:0, hud_guard_pass:0, hud_flag_on:0};
let guardSeen={};   // DAT_0063ba8c value -> times seen at HUD entry
let vtHits={}, vtRets={};

// TRUE frame boundary. The d3d9 shim exports the address of its Present counter
// (MashedShim_PresentCounter, d3d9_shim.cpp) because Present is the only
// verified once-per-frame event in MASHED -- no in-game function qualifies.
// Reading it inline in each draw hook tags every draw with the frame it belongs
// to, which removes the need for a frame anchor entirely.
// Resolved LAZILY, not in init(): init() runs immediately after spawn, before
// the game has loaded d3d9.dll, so an eager lookup always failed and every draw
// was tagged -1.
let presentPtr=null, presentTried=0, presentErr='';
function resolvePresent(){
  if(presentTried) return presentPtr!==null;
  presentTried=1;
  // The Frida module API moved between versions: the top-level
  // Module.findExportByName is gone in 17.x ("TypeError: not a function"), so
  // try each spelling instead of pinning one.
  const NAME='MashedShim_PresentCounter';
  let ex=null;
  const attempts=[
    function(){ return Module.findExportByName('d3d9.dll',NAME); },
    function(){ return Module.getGlobalExportByName(NAME); },
    function(){ return Process.getModuleByName('d3d9.dll').findExportByName(NAME); },
    function(){
      const ms=Process.enumerateModules();
      for(let i=0;i<ms.length;i++){
        if(ms[i].name.toLowerCase()==='d3d9.dll'){
          const a=ms[i].findExportByName(NAME);
          if(a) return a;
        }
      }
      return null;
    }
  ];
  for(let i=0;i<attempts.length;i++){
    try{ ex=attempts[i](); if(ex) break; }catch(e){ presentErr=''+e; }
  }
  try{
    if(!ex){ presentErr='export not found ('+presentErr+')';
             return false; }
    const p=new NativeFunction(ex,'pointer',[])();
    if(p.isNull()){ presentErr='accessor returned NULL'; return false; }
    presentPtr=p;
    return true;
  }catch(e){ presentErr=''+e; return false; }
}
function frameNo(){
  if(presentPtr===null) return -1;
  try{ return presentPtr.readS32(); }catch(e){ return -1; }
}

function abs(r){return ptr(r+DELTA);}
function hex(buf){
  const u=new Uint8Array(buf); let h='';
  for(let i=0;i<u.length;i++){ h+=('0'+u[i].toString(16)).slice(-2); }
  return h;
}
function rets(ctx){
  try{
    return Thread.backtrace(ctx, Backtracer.FUZZY).slice(0,5)
           .map(function(a){ return a.sub(DELTA).toString(); });
  }catch(e){ return []; }
}

rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    slotFn=new NativeFunction(abs(SLOT_FN),'void',['int','int']);
    // input override (same function nav_agent.js uses, scenario_launch.py:66)
    Interceptor.attach(abs(RES_RVA), {
      onEnter(a){ const sp=this.context.esp;
        this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil)
        ret.replace(ptr(0xff)); }
    });
    Interceptor.attach(abs(SPAWN_RVA), { onEnter(a){ spawnFired++; } });
    // Guard diagnostic: armed from init so it covers the whole race, and it is
    // the CONTROL for "is 0x0040dfc0 the in-race HUD entry point".
    Interceptor.attach(abs(RVA_HUD), { onEnter(a){
      cov.hud_calls++;
      let v=-1, f=-1;
      try{ v=abs(DAT_63ba8c).readU32(); }catch(e){}
      try{ f=abs(DAT_66d704).readU32(); }catch(e){}
      const k='0x'+(v>>>0).toString(16);
      guardSeen[k]=(guardSeen[k]||0)+1;
      if(f!==0){ cov.hud_flag_on++; }
      if(v===5||v===6||v===7){ cov.hud_guard_pass++; }
    }});
    return DELTA;
  },
  phase:function(){ try{ return abs(PHASE).readU8(); }catch(e){ return -999; } },
  navphase:function(){ try{ return abs(NAV_PHASE).readS32(); }catch(e){ return -999; } },
  navdepth:function(){ try{ return abs(NAV_DEPTH).readS32(); }catch(e){ return -999; } },
  navsetsel:function(v){ try{ const d=abs(NAV_DEPTH).readS32();
    abs(NAV_CURSOR+(d-1)*0x40).writeS32(v); return 1; }catch(e){ return 0; } },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  spawned:function(){ return spawnFired; },
  guardval:function(){ try{ return abs(DAT_63ba8c).readU32(); }catch(e){ return -1; } },
  cov:function(){ return {cov:cov, guard:guardSeen}; },

  setup:function(cfg){
    abs(TRACK_ENG).writeS32(cfg.track);
    abs(TRACK_MENU).writeS32(cfg.track);
    abs(MODE).writeS32(cfg.mode);
    abs(RULE).writeS32(cfg.rule);
    abs(CAR_P0).writeS32(cfg.car);
    abs(TEAM).writeS32(cfg.team);
    if(cfg.difficulty>=0){ abs(DIFFICULTY).writeS32(cfg.difficulty); }
    if(cfg.powerups>=0){ abs(POWERUPS).writeS32(cfg.powerups); }
    for(let s=0;s<4;s++){
      const v = (s===0) ? 1 : (s<cfg.cars ? 2 : 0);
      slotFn(s,v);
    }
    return 1;
  },
  launch:function(){ abs(PHASE).writeU8(2); return 1; },

  // Enumerate the RW device vtable so slots can be named by RVA before hooking.
  vtdump:function(nslots){
    const vt=abs(VTBL).readU32();
    if(vt===0) return null;
    const out=[];
    for(let s=0;s<nslots;s++){
      let fn=0;
      try{ fn=ptr(vt).add(s*4).readU32(); }catch(e){}
      out.push({slot:'0x'+(s*4).toString(16),
                rva: fn ? '0x'+(fn-DELTA).toString(16) : null});
    }
    return out;
  },

  // Count calls per vtable slot while collecting, with a few sample retaddrs.
  // The icons/bars/circles are on NEITHER the +0x30 Im2D slot nor Im3D, so the
  // remaining candidate is another slot on this same vtable. Count-only (no
  // backtrace after the first few) to stay off the hot-path budget.
  armvtscan:function(nslots){
    const vt=abs(VTBL).readU32();
    if(vt===0) return -1;
    for(let s=0;s<nslots;s++){
      let fn=0;
      try{ fn=ptr(vt).add(s*4).readU32(); }catch(e){ continue; }
      if(!fn) continue;
      // The vtable is a STRUCT: past roughly slot 0x120 it holds data, not code
      // (observed 0x3, 0x200000, 0x80000000, heap pointers). Attaching an
      // Interceptor to those crashed the game outright. Only hook values that
      // land inside MASHED's code range.
      const rva=fn-DELTA;
      if(rva<0x00401000 || rva>0x005e0000){ continue; }
      const key='0x'+(s*4).toString(16)+'@0x'+rva.toString(16);
      vtHits[key]=0; vtRets[key]=[];
      try{
        Interceptor.attach(ptr(fn), { onEnter(args){
          if(collecting===0) return;
          vtHits[key]++;
          if(vtRets[key].length<3){
            vtRets[key].push(rets(this.context).slice(0,3).join(' '));
          }
        }});
      }catch(e){}
    }
    return 1;
  },
  vtreport:function(){ return JSON.stringify({hits:vtHits, rets:vtRets}); },

  // FREE-RUN capture: no frame bracketing at all.
  // Bracketing was abandoned because no once-per-frame anchor is established in
  // this codebase and both candidates fail: 0x004c1be0 fires ~5-10x per frame
  // (quad_out/quad_in was ~9:1, i.e. each "frame" was a sub-frame slice), and
  // 0x00492e90 combined with the guard gate skipped 344 of 347 hits because
  // DAT_0063ba8c is mutated MANY times per frame -- it is a mode variable, not a
  // stable per-frame screen state. Free-run collects every draw with a sequence
  // number so frames can be inferred offline from the periodic draw cycle, and
  // answers the actual question (what does this screen draw) without depending
  // on a frame boundary at all.
  // Pass anchorRva == 0 to armburst for free-run.
  presentinfo:function(){
    const ok=resolvePresent();
    return {ok:ok, err:presentErr,
            frame: ok ? frameNo() : -1,
            ptr: presentPtr ? '0x'+presentPtr.toString(16) : null};
  },

  armburst:function(label,k,anchorRva,guardEq){
    if(hooked) return 0;
    resolvePresent();   // must be live BEFORE the first tagged draw
    const vt=abs(VTBL).readU32();
    if(vt===0){ return -1; }          // device not up: refuse rather than guess
    const drawFn=ptr(vt).add(0x30).readU32();

    Interceptor.attach(ptr(drawFn), { onEnter(args){
      if(collecting===0 || cur===null){ cov.quad_out++; return; }
      // RwIm2DRenderPrimitive(prim, verts, num), menu_draw_burst.py:82-85
      let verts=args[1]; let n=args[2].toInt32();
      if(n<1||n>64){ n=4; }
      if(verts.isNull()){ verts=abs(VBUF); }
      let h='';
      try{ h=hex(verts.readByteArray(n*STRIDE)); }
      catch(e){ cov.quad_err++; return; }
      cov.quad_in++;
      // XOR fold so a stream of identical constants is visible as such.
      for(let i=0;i<h.length;i+=8){ cov.xorfold^=parseInt(h.substr(i,8),16)|0; }
      quads[cur].push({f:frameNo(), v:h, r:rets(this.context)});
    }});

    // __cdecl, 5 args at [esp+4..esp+0x14] on entry.
    Interceptor.attach(abs(RVA_TEXT), { onEnter(args){
      if(collecting===0 || cur===null){ cov.text_out++; return; }
      const sp=this.context.esp;
      const rec={};
      try{
        const a1=sp.add(0x04).readU32();      // font_ctx
        const a2=sp.add(0x08).readPointer();  // str_buf
        const a3=sp.add(0x0c).readU32();      // scale (float bits)
        const a4=sp.add(0x10).readPointer();  // xy_coords*
        const a5=sp.add(0x14).readU32();      // style
        rec.ctx='0x'+a1.toString(16);
        rec.scale_bits='0x'+a3.toString(16);
        rec.style='0x'+a5.toString(16);
        rec.str_raw=null; rec.str_ascii=null; rec.str_utf16=null;
        if(!a2.isNull()){
          try{ rec.str_raw=hex(a2.readByteArray(64)); }catch(e){}
          try{ rec.str_ascii=a2.readCString(); }catch(e){}
          try{ rec.str_utf16=a2.readUtf16String(32); }catch(e){}
        }
        rec.xy_raw=null;
        if(!a4.isNull()){ try{ rec.xy_raw=hex(a4.readByteArray(16)); }catch(e){} }
      }catch(e){ cov.text_err++; return; }
      cov.text_in++;
      rec.chan='thunk_00556ca0';
      rec.f=frameNo();
      rec.r=rets(this.context);
      texts[cur].push(rec);
    }});

    // Glyph renderer, ALL callers. On entry: [esp+4]=font_ctx, [esp+8]=str_buf.
    // No scale/xy/style args are established for this level -> not invented.
    Interceptor.attach(abs(RVA_GLYPH), { onEnter(args){
      if(collecting===0 || cur===null){ cov.glyph_out++; return; }
      const sp=this.context.esp;
      const rec={chan:'glyph_00554940'};
      try{
        const a1=sp.add(0x04).readU32();      // font_ctx
        const a2=sp.add(0x08).readPointer();  // str_buf
        rec.ctx='0x'+a1.toString(16);
        rec.str_raw=null; rec.str_ascii=null; rec.str_utf16=null;
        if(!a2.isNull()){
          try{ rec.str_raw=hex(a2.readByteArray(64)); }catch(e){}
          try{ rec.str_ascii=a2.readCString(); }catch(e){}
          try{ rec.str_utf16=a2.readUtf16String(32); }catch(e){}
        }
      }catch(e){ cov.glyph_err++; return; }
      cov.glyph_in++;
      rec.f=frameNo();
      rec.r=rets(this.context);
      texts[cur].push(rec);
    }});

    // Im3D submit channel. MEASURED: during the DRIVING state (DAT_0063ba8c==3)
    // the Im2D device slot carries exactly one invisible 512x512 quad per frame
    // and the font pipe carries nothing, on both TRAINING and a real track. So
    // the driving HUD is on neither. This channel is the remaining candidate:
    // 0x004cd070 RwRenderPrimitiveSubmit counted 7158 in ~1250 frames (~5.7/f).
    // Args per STUBS S-2120's cited call site 0x00554afa:
    //   (base_ptr, ptr, count, prim) with prim 5 = TRISTRIP, __cdecl.
    // Vertices are NOT dumped: the arg2 pointer semantics are not established
    // (tag 0x24 = sizeof(Im3DVertex) is documented, the indexing is not), and
    // attribution by retaddr is what this channel is for.
    Interceptor.attach(abs(RVA_IM3D), { onEnter(args){
      if(collecting===0 || cur===null){ cov.im3d_out++; return; }
      const sp=this.context.esp;
      const rec={chan:'im3d_004cd070'};
      try{
        // Arg shape established by capstone disasm of BOTH call sites, not
        // guessed (the earlier S-2120-derived labels were wrong and produced
        // count=0 prim=25 nonsense):
        //   0x00422a4b: push 0x19 / push 0 / push edx(=3*esi) / push eax(stack buf)
        //   0x00555182: push edx(= span/0x24) / push esi(verts)
        // => arg1 = verts_ptr, arg2 = vertex count.
        // Stride is 0x24: 0x00555182 divides the byte span by 36 via the
        // reciprocal magic 0x38e38e39 + SAR 3, matching the documented
        // tag=0x24=sizeof(Im3DVertex). RW3.x RwIm3DVertex =
        //   objVertex(12) + objNormal(12) + color(4) + u(4) + v(4) = 36.
        const vp=sp.add(0x04).readPointer();
        const n=sp.add(0x08).readS32();
        rec.verts_ptr='0x'+vp.toString(16);
        rec.count=n;
        rec.a3='0x'+sp.add(0x0c).readU32().toString(16);
        rec.a4='0x'+sp.add(0x10).readU32().toString(16);
        rec.v=null;
        if(!vp.isNull() && n>0 && n<=512){
          try{ rec.v=hex(vp.readByteArray(n*0x24)); }catch(e){}
        }
      }catch(e){ cov.im3d_err++; return; }
      cov.im3d_in++;
      rec.f=frameNo();
      rec.r=rets(this.context);
      im3ds[cur].push(rec);
    }});

    if(anchorRva===0){
      // Free-run: one open bucket, no anchor, no gate.
      quads[label]=[]; texts[label]=[]; im3ds[label]=[]; order.push(label);
      cur=label; collecting=1; cov.frames=1; hooked=1;
      return 1;
    }

    // Frame delimiter. [UNCERTAIN] once-per-frame is not established; the
    // caller checks draws-per-frame against ~1.6 quads/frame before trusting.
    let seen=0;
    const h=Interceptor.attach(abs(anchorRva), { onEnter(args){
      // Optional state gate. The nav driver's confirm pulses can carry the game
      // out of the race into the between-rounds STANDINGS screen, which draws
      // its own HUD ("MASHED" + "Current Standings" over letterbox bands) and
      // is indistinguishable from a race capture unless gated. guardEq pins the
      // capture to one DAT_0063ba8c value. -1 = ungated.
      if(guardEq>=0){
        let v=-1;
        try{ v=abs(DAT_63ba8c).readU32(); }catch(e){}
        if(v!==guardEq){ cov.frame_skip++; collecting=0; cur=null; return; }
      }
      // Close the previous frame on the NEXT anchor hit rather than detaching
      // while still collecting -- detaching mid-frame left the final frame open
      // and it swallowed every later draw (12 in f5 vs ~1 in f0..f4).
      if(seen>=k){ collecting=0; cur=null; h.detach(); return; }
      cur=label+'_f'+seen;
      quads[cur]=[]; texts[cur]=[]; im3ds[cur]=[]; order.push(cur);
      collecting=1; seen++; cov.frames++;
    }});
    hooked=1;
    return 1;
  },

  done:function(){ return order.length; },
  stop:function(){ collecting=0; cur=null; return 1; },
  report:function(){
    const q={}, t={}, i3={};
    order.forEach(function(l){ q[l]=quads[l]; t[l]=texts[l]; i3[l]=im3ds[l]; });
    return JSON.stringify({quads:q, texts:t, im3ds:i3, cov:cov, guard:guardSeen});
  }
};
'''


def preview_quad(rec):
    raw = bytes.fromhex(rec["v"])
    n = len(raw) // 0x1c
    xs, ys, col = [], [], 0
    for i in range(n):
        x, y, _z, _w, c = struct.unpack_from("<ffffI", raw, i * 0x1c)
        xs.append(x); ys.append(y)
        if i == 0:
            col = c
    return (f"x={min(xs):7.2f} y={min(ys):7.2f} w={max(xs)-min(xs):7.2f} "
            f"h={max(ys)-min(ys):6.2f} col={col:08x} verts={n} "
            f"ret={rec['r'][0] if rec.get('r') else '?'}")


def preview_text(rec):
    # UTF-16 FIRST: the in-race buffers are wide (FontText_UTF16WidenCopy), so
    # an ASCII read stops at the first NUL and shows only the initial letter --
    # that is what made real strings print as "M" and "C" on the first run.
    s = rec.get("str_utf16") or rec.get("str_ascii") or ""
    s = s.replace("\n", "\\n")[:24]
    f = ""
    xy = rec.get("xy_raw") or ""
    if len(xy) >= 16:
        try:
            fv = struct.unpack_from("<ff", bytes.fromhex(xy))
            f = f"xy_f=({fv[0]:.2f},{fv[1]:.2f})"
        except Exception:
            pass
    try:
        scf = struct.unpack("<f", struct.pack("<I", int(rec.get("scale_bits", "0x0"), 16)))[0]
    except Exception:
        scf = float("nan")
    return (f'str="{s}" {f} scale={scf:.4f} style={rec.get("style")} '
            f'ret={rec["r"][0] if rec.get("r") else "?"}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", type=int, default=6)
    ap.add_argument("--label", default="race")
    ap.add_argument("--track", type=int, default=0)
    ap.add_argument("--mode", type=int, default=10, help="10=QuickRace 2=TimeTrial")
    ap.add_argument("--cars", type=int, default=4)
    ap.add_argument("--car", type=int, default=0)
    ap.add_argument("--rule", type=int, default=0)
    ap.add_argument("--team", type=int, default=0)
    ap.add_argument("--powerups", type=int, default=-1)
    ap.add_argument("--difficulty", type=int, default=-1)
    ap.add_argument("--driver", choices=["warp", "nav"], default="warp",
                    help="warp = scenario_launch PHASE poke (fast, but appears "
                         "to skip HUD setup); nav = normal menu->race flow")
    ap.add_argument("--settle", type=float, default=6.0,
                    help="seconds in-race before arming (start intro + HUD settle)")
    ap.add_argument("--free-run", type=float, default=None,
                    help="collect every draw for N seconds with NO frame "
                         "bracketing and no guard gate; frames are inferred "
                         "offline from the periodic draw cycle")
    ap.add_argument("--vtable-scan", action="store_true",
                    help="enumerate the RW device vtable and count calls per "
                         "slot while collecting, to locate emitters that are "
                         "on neither the +0x30 Im2D slot nor Im3D")
    ap.add_argument("--vtable-slots", type=int, default=32,
                    help="how many vtable slots to enumerate/hook")
    ap.add_argument("--bbdump", default=None,
                    help="also dump the backbuffer via the d3d9 shim's "
                         "MASHED_ORIG_BBDUMP_REQ protocol, at a moment when "
                         "DAT_0063ba8c is verified == 3 (driving)")
    ap.add_argument("--guard-eq", type=int, default=-1,
                    help="only open a frame when DAT_0063ba8c == this value "
                         "(-1 = ungated). Pins the capture to one race state; "
                         "measured in-race values are 5 and 6.")
    ap.add_argument("--press-settle", dest="press_settle",
                    action="store_true", default=True,
                    help="pulse confirm during settle to clear the start intro")
    ap.add_argument("--no-press-settle", dest="press_settle",
                    action="store_false",
                    help="never synthesise input after the race starts; use "
                         "when confirm pulses skip past the race into standings")
    ap.add_argument("--anchor", default="0x004c1be0",
                    help="frame-delimiter RVA (~60/s); see [UNCERTAIN] note")
    ap.add_argument("--out", default=str(ROOT / "log" / "race_hud_burst.json"))
    ap.add_argument("--out-text", default=str(ROOT / "log" / "race_hud_text.json"))
    ap.add_argument("--out-attrib", default=str(ROOT / "log" / "race_hud_attrib.json"))
    args = ap.parse_args()

    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"       # stock original, no .asi
    env["MASHED_FPS_CAP"] = "60"
    env.setdefault("MASHED_WIN_POS", "left-bl")  # memory: game-window-on-left-monitor

    bb_out = bb_req = None
    if args.bbdump:
        # MUST be absolute: the shim resolves the request path against the GAME's
        # cwd (original\), not ours, so a relative --bbdump silently looks for
        # original\<path> and the request is never seen.
        bb_out = Path(args.bbdump).resolve()
        bb_out.parent.mkdir(parents=True, exist_ok=True)
        bb_req = bb_out.parent / "orig_bbdump.req"
        # Delete stale outputs FIRST. Polling for the BMP to appear is only
        # sound if a previous run's file cannot satisfy the poll -- otherwise
        # the wait returns instantly on a stale menu frame and every number
        # derived from it is confidently wrong (memory
        # race-capture-wait-for-exit).
        for p in (bb_out, bb_req):
            try:
                p.unlink()
            except FileNotFoundError:
                pass
        env["MASHED_ORIG_BBDUMP_REQ"] = str(bb_req)

    # NEVER frida.spawn -- perturbs boot layout (scenario_launch.py:16).
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env)
    pid = proc.pid
    print(f"spawned MASHED pid={pid}  (this session owns ONLY this pid)")

    rc = 1
    try:
        dev = frida.get_local_device()
        sess = None
        for _ in range(200):
            try:
                sess = dev.attach(pid); break
            except Exception:
                time.sleep(0.1)
        if sess is None:
            print("FAIL: could not attach")
            return 2

        scr = sess.create_script(AGENT)
        scr.on("message", lambda m, d: None)
        scr.load()
        E = scr.exports_sync
        E.init()

        def wait_phase(target, timeout, what):
            end = time.time() + timeout
            while time.time() < end:
                if E.phase() == target:
                    return True
                if proc.poll() is not None:
                    print(f"FAIL: process exited while waiting for {what}")
                    return False
                time.sleep(0.25)
            print(f"FAIL: timeout waiting for {what}")
            return False

        if not wait_phase(1, 40, "menu (phase 1)"):
            return 3
        time.sleep(0.5)

        if args.driver == "warp":
            # scenario_launch.py's warp: poke the session phase and let the
            # engine load the track and spawn cars itself.
            E.setup({"track": args.track, "mode": args.mode, "cars": args.cars,
                     "car": args.car, "rule": args.rule, "team": args.team,
                     "powerups": args.powerups, "difficulty": args.difficulty})
            print(f"[setup] track={args.track} mode={args.mode} cars={args.cars}")
            time.sleep(0.2)
            E.launch()
            print("[launch] poke PHASE 0x00771968 = 2")
            if not wait_phase(3, 40, "race running (phase 3)"):
                return 4
        else:
            # NORMAL menu->race transition (nav_to_race.py:72-79). Slower and
            # less parameterisable, but it runs whatever per-race setup the warp
            # skips -- which is the hypothesis under test for the missing HUD.
            end = time.time() + 30
            while time.time() < end and not (E.navphase() == 3 and E.navdepth() >= 1):
                time.sleep(0.2)
            if E.navphase() != 3:
                print("FAIL: never reached the menu (nav phase != 3)")
                return 3
            time.sleep(1.0)

            def confirm_to(target, tries=12):
                for _ in range(tries):
                    E.press(4, 120)
                    time.sleep(0.35)
                    if E.navdepth() >= target:
                        return True
                return False

            confirm_to(2)
            E.press(4, 120); time.sleep(0.35)
            confirm_to(3)
            E.navsetsel(1)                 # Quick Battle
            time.sleep(0.3)
            confirm_to(4)
            confirm_to(5)
            for _ in range(6):
                E.press(4, 120)
                time.sleep(0.4)
                if E.navphase() != 3:
                    break
            end = time.time() + 25
            while time.time() < end and E.navphase() == 3:
                time.sleep(0.3)
            if E.navphase() == 3:
                print("FAIL: NOT in race (nav phase still 3)")
                return 4
            print(f"[nav] in race, nav phase={E.navphase()}")

        # Confirm cars actually spawned before believing we are racing.
        for _ in range(8):
            time.sleep(0.5)
            if E.spawned() > 0:
                break
        print(f"*** RACE RUNNING *** driver={args.driver} "
              f"spawnFired={E.spawned()} nav_phase={E.navphase()}")

        # Pulse control 4 through the settle to clear the start intro.
        t0 = time.time()
        while time.time() < t0 + args.settle:
            if args.press_settle:
                E.press(4, 250)
            time.sleep(0.6)
        print(f"[settle] done; DAT_0063ba8c now = "
              f"0x{E.guardval() & 0xffffffff:x}, guard-eq={args.guard_eq}")

        if args.vtable_scan:
            slots = E.vtdump(args.vtable_slots)
            print(f"RW device vtable *(0x007d3ff8) — {args.vtable_slots} slots:")
            for s in slots:
                if s["rva"]:
                    print(f"  {s['slot']:>6} -> {s['rva']}")
            if E.armvtscan(args.vtable_slots) != 1:
                print("FAIL: device NULL, cannot scan")
                return 5

        pi = E.presentinfo()
        if pi["ok"]:
            print(f"[frames] shim Present counter live at {pi['ptr']}, "
                  f"now at frame {pi['frame']}")
        else:
            print(f"[frames] WARNING: Present counter unavailable ({pi['err']}); "
                  "every draw will be tagged f=-1 and frame grouping is "
                  "impossible. Rebuild: mashedmod\\build_d3d9_shim.bat")

        anchor = 0 if args.free_run else int(args.anchor, 16)
        armed = E.armburst(args.label, args.frames, anchor, args.guard_eq)
        if armed == -1:
            print("FAIL: RW device *(0x007d3ff8) is NULL -- refusing to hook")
            return 5
        print(f"armed at anchor {args.anchor}; waiting for {args.frames} frames")
        # Do NOT pulse confirm here. scenario_launch.py's hold loop presses 4 to
        # advance between rounds; leaving that on during the capture window
        # skipped the race entirely and captured the STANDINGS screen instead
        # ("MASHED" + "Current Standing" over letterbox bands). Confirm belongs
        # in the settle phase only, to clear the start intro.
        if args.free_run:
            print(f"  FREE-RUN: collecting everything for {args.free_run}s")
            time.sleep(args.free_run)
        else:
            end = time.time() + 20
            while time.time() < end and E.done() < args.frames:
                time.sleep(0.3)
        E.stop()
        time.sleep(0.2)

        if bb_out is not None:
            gv = E.guardval()
            print(f"[bbdump] DAT_0063ba8c = 0x{gv & 0xffffffff:x} at request time"
                  + ("" if gv == 3 else "  <-- NOT the driving state (3)"))
            bb_req.write_text(str(bb_out) + "\n")
            end = time.time() + 15
            while time.time() < end:
                if bb_out.exists() and not bb_req.exists():
                    break
                time.sleep(0.2)
            if bb_out.exists():
                sz = bb_out.stat().st_size
                time.sleep(0.5)
                print(f"[bbdump] -> {bb_out}  ({sz} bytes, "
                      f"{'stable' if bb_out.stat().st_size == sz else 'STILL WRITING'})"
                      f"  guard_at_dump=0x{gv & 0xffffffff:x}")
            else:
                print("[bbdump] FAIL: shim never produced the BMP. Either the "
                      "deployed d3d9.dll predates the request protocol "
                      "(rebuild: mashedmod\\build_d3d9_shim.bat) or Present "
                      "was not reached.")

        payload = json.loads(E.report())
        cov, guard = payload["cov"], payload["guard"]

        outq, outt = Path(args.out), Path(args.out_text)
        outq.parent.mkdir(parents=True, exist_ok=True)
        outq.write_text(json.dumps(payload["quads"], indent=1))
        outt.write_text(json.dumps(payload["texts"], indent=1))

        # Retaddr attribution: who actually emits the in-race HUD.
        aq, at = Counter(), Counter()
        for lbl, rows in payload["quads"].items():
            for r in rows:
                for a in (r.get("r") or [])[:3]:
                    aq[a] += 1
        for lbl, rows in payload["texts"].items():
            for r in rows:
                for a in (r.get("r") or [])[:3]:
                    at[a] += 1
        ai = Counter()
        for lbl, rows in payload.get("im3ds", {}).items():
            for r in rows:
                for a in (r.get("r") or [])[:3]:
                    ai[a] += 1
        Path(str(args.out).replace(".json", "_im3d.json")).write_text(
            json.dumps(payload.get("im3ds", {}), indent=1))
        attrib = {"quad_retaddrs": aq.most_common(25),
                  "text_retaddrs": at.most_common(25),
                  "im3d_retaddrs": ai.most_common(25),
                  "coverage": cov, "hud_guard_values": guard}
        Path(args.out_attrib).write_text(json.dumps(attrib, indent=1))

        if args.vtable_scan:
            vt = json.loads(E.vtreport())
            live = {k: v for k, v in vt["hits"].items() if v}
            print("\nvtable slots that fired INSIDE the bracket "
                  "(slot@rva -> calls):")
            for k, v in sorted(live.items(), key=lambda kv: -kv[1]):
                print(f"  {k}  {v}")
                for r in vt["rets"].get(k, [])[:2]:
                    print(f"      via {r}")
            if not live:
                print("  none — the missing layer is not on this vtable at all")

        print("\ncoverage:", json.dumps(cov))
        print("HUD-entry guard values (DAT_0063ba8c @ 0x0040dfc0 entry):",
              json.dumps(guard))
        print("->", outq); print("->", outt); print("->", args.out_attrib)

        nf = max(1, len(payload["quads"]))
        print(f"\nframes={len(payload['quads'])} "
              f"quads/frame={cov['quad_in']/nf:.2f} "
              f"text/frame={cov['text_in']/nf:.2f}   "
              f"(counters imply ~1.6 quads/frame; a wildly different ratio "
              f"means the anchor is not once-per-frame)")

        for lbl in payload["quads"]:
            q, t = payload["quads"][lbl], payload["texts"][lbl]
            print(f"--- {lbl}: {len(q)} quads, {len(t)} text ---")
            for r in q[:8]:
                print("  Q " + preview_quad(r))
            for r in t[:8]:
                print("  T " + preview_text(r))

        print("\ntop quad emitters (retaddr -> count):")
        for a, c in aq.most_common(10):
            print(f"  {a}  {c}")
        print("top text emitters (retaddr -> count):")
        for a, c in at.most_common(10):
            print(f"  {a}  {c}")
        print("top Im3D emitters (retaddr -> count):")
        for a, c in ai.most_common(12):
            print(f"  {a}  {c}")
        for lbl in list(payload.get("im3ds", {}))[:1]:
            print(f"--- {lbl} Im3D submits ---")
            for r in payload["im3ds"][lbl][:12]:
                print(f"  I count={r.get('count')} a3={r.get('a3')} "
                      f"a4={r.get('a4')} verts={r.get('verts_ptr')} "
                      f"bytes={len(r.get('v') or '')//2} "
                      f"ret={r['r'][0] if r.get('r') else '?'}")

        # Verdict. Controls: *_out is the "hook alive but bracket wrong" control;
        # hud_guard_pass is the control for the HUD-entry-point question.
        print()
        gp, hc = cov["hud_guard_pass"], cov["hud_calls"]
        if hc:
            print(f"HUD-entry diagnostic: 0x0040dfc0 called {hc}x, guard "
                  f"DAT_0063ba8c in {{5,6,7}} on {gp} of them "
                  f"({'EARLY-RETURN on all -- not the in-race HUD entry point' if gp == 0 else 'guard DOES pass -- it IS on the in-race path'}).")
        if cov["frames"] == 0:
            print(f"VERDICT: RED -- anchor {args.anchor} never fired; frame "
                  "delimiter is wrong. Says nothing about the HUD.")
            rc = 6
        elif cov["quad_in"] == 0 and cov["text_in"] == 0 and cov["glyph_in"] == 0:
            ctl = cov["quad_out"] + cov["text_out"] + cov["glyph_out"]
            print(f"VERDICT: RED -- {cov['frames']} frames bracketed, 0 draws "
                  f"inside, {ctl} outside. "
                  + ("Channel hooks are alive but the bracket misses them."
                     if ctl else "Control is DEAD too -- both channel hooks are suspect."))
            rc = 7
        else:
            print(f"VERDICT: GREEN -- {cov['frames']} frames, {cov['quad_in']} "
                  f"quads + {cov['text_in']} thunk + {cov['glyph_in']} glyph "
                  f"inside (control: {cov['quad_out']}/{cov['text_out']}/"
                  f"{cov['glyph_out']} outside). "
                  f"xorfold=0x{cov['xorfold'] & 0xffffffff:08x}")
            rc = 0
    finally:
        try:
            proc.kill()      # kill ONLY the pid we spawned (CLAUDE.md hygiene)
        except Exception:
            pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
