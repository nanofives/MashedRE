# Nav-driven diff-original for FUN_00432b30 (prompt-strip glyph row builder,
# called once per FUN_0043d2a0 nav op from 0x0043d79c) — synthetic direct-call
# sweep variant of the on-game-thread A/B (menu_anim/logo/drawloop precedents).
#
# Why synthetic: organic traffic (log/probe_prompt_calls.json, 2026-06-11)
# shows pushes deliver mode=0 + key=screen-kind and reloads deliver mode=2
# (early-return, no row) — only 3 of the 10 jump-table keys fire on the
# verified screen set. A direct-call sweep on the game thread (live menu
# state for the FUN_0042add0/FUN_0042b920/FUN_0042a9c0 callees) covers all
# 10 keys x both sides of the key1 b920-gate x modes 0/1/2 + bounds keys.
#
# True ABI (FUN_00432b30_full.asm): EAX=mode, ESI=rec-index ptr,
# stack=(key, cmp), plain ret. A Frida-written stub trampoline marshals
# from fixed memory slots; target slot switches original <-> twin export.
# Per vector: idx=6; run A (original) -> postA; restore; run B (twin) ->
# postB; GREEN iff table bytes + idx-after match; restore pre (as-if-never).
# FUN_0042ad10 (row init, both paths) is the fired-at-all witness.
#
# Usage: py -3.12 re/frida/menu_prompt_diff.py
import json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
ASI = ORIG / "mashed_re_dev.asi"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_HOST=0x0043c5b0;      // per-frame menu draw loop (host only)
const RVA_STRIP=0x00432b30;     // target under test
const RVA_AD10=0x0042ad10;      // per-row init (witness, both paths call it)
const RVA_B920=0x0042b920;
const RVA_NAV=0x0043d2a0, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
// state to equalize + compare between A and B runs:
const SNAPS=[[0x00898aa0,0x20+31*0x34], // record count vicinity + table
             [0x0067e840,0x700],        // nav block incl. deep screen stack
             [0x008990e0,0x10],         // slide offset + dim-all
             [0x0067f0b8,4],            // sprite B counter
             [0x0086ecc8,8]];           // logo fade pair
let twinPtr=null, stripPtr=null, nav=null, b920=null, stubFn=null;
let slots=null, idxSlot=null;   // keepAlive
const _keepAlive=[];
let results=[], want=0, collecting=0, adA=[], adB=[];
let rawCalls=0, lastErr='';
function abs(r){return ptr(r+DELTA);}
function snap(){ return SNAPS.map(function(r){ return abs(r[0]).readByteArray(r[1]); }); }
function restore(s){ SNAPS.forEach(function(r,i){ abs(r[0]).writeByteArray(s[i]); }); }
function hex(buf,off,len){
  const u=new Uint8Array(buf); let h='';
  for(let i=off;i<Math.min(off+len,u.length);i++){ h+=('0'+u[i].toString(16)).slice(-2); }
  return h;
}
function cmpSnaps(a,b){
  for(let r=0;r<a.length;r++){
    const ua=new Uint8Array(a[r]), ub=new Uint8Array(b[r]);
    for(let i=0;i<ua.length;i++){
      if(ua[i]!==ub[i]) return {ok:false, range:r, off:i};
    }
  }
  return {ok:true, range:-1, off:-1};
}
function callStrip(target, mode, key, cmp){
  slots.writePointer(target);
  slots.add(4).writeS32(mode);
  slots.add(8).writeS32(key);
  slots.add(0xc).writeS32(cmp);
  slots.add(0x10).writePointer(idxSlot);
  stubFn();
}
rpc.exports={
  init:function(asiPath){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    if(DELTA!==0) throw new Error('DELTA!=0: twin hardcodes 0x0042xxxx callees');
    const asi=Module.load(asiPath);
    twinPtr=asi.findExportByName('PromptStripTwin');
    if(twinPtr===null) throw new Error('PromptStripTwin export missing');
    stripPtr=abs(RVA_STRIP);
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    b920=new NativeFunction(abs(RVA_B920),'int',[]);
    slots=Memory.alloc(0x20); idxSlot=Memory.alloc(8);
    _keepAlive.push(slots, idxSlot);
    // stub: push esi; eax=[slots+4]; esi=[slots+0x10]; push [slots+0xc];
    //       push [slots+8]; call [slots]; add esp,8; pop esi; ret
    const stub=Memory.alloc(Process.pageSize); _keepAlive.push(stub);
    Memory.patchCode(stub, 64, function(code){
      const w=new X86Writer(code, {pc: stub});
      w.putPushReg('esi');
      w.putMovRegNearPtr('eax', slots.add(4));
      w.putMovRegNearPtr('esi', slots.add(0x10));
      w.putMovRegNearPtr('ecx', slots.add(0xc)); w.putPushReg('ecx');
      w.putMovRegNearPtr('ecx', slots.add(8));   w.putPushReg('ecx');
      w.putMovRegNearPtr('ecx', slots);
      w.putCallReg('ecx');
      w.putAddRegImm('esp', 8);
      w.putPopReg('esi');
      w.putRet();
      w.flush();
    });
    stubFn=new NativeFunction(stub,'void',[]);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push:function(scr){ nav(scr,0); return abs(RVA_DEPTH).readS32(); },
  pop:function(){ nav(0,1); return abs(RVA_DEPTH).readS32(); },
  arm:function(n){
    want=n; results=[];
    Interceptor.attach(abs(RVA_AD10), { onEnter(args){
      if(collecting===0) return;
      const rec={i:this.context.eax.readS32(),
                 tag:this.context.edx.toUInt32().toString(16)};
      if(collecting===1) adA.push(rec); else adB.push(rec);
    }});
    const origPtr=Interceptor.replaceFast(abs(RVA_HOST), new NativeCallback(function(){
      rawCalls++;
      const orig=new NativeFunction(origPtr,'void',[]);
      try {
        if(results.length<want){
          const bv=b920();
          const vectors=[];
          for(let k=1;k<=10;k++){ vectors.push({m:0,k:k,c:bv});
                                  vectors.push({m:0,k:k,c:bv+1}); }
          // mode 1 = pop-reveal; valid ONLY when stack[depth+1] holds a
          // previously-popped entry (probe 2026-06-11: organic shape is
          // {m:1, k:kind, c:0}); the controller samples on the way down.
          for(let k=1;k<=10;k++){ vectors.push({m:1,k:k,c:0});
                                  vectors.push({m:1,k:k,c:bv}); }
          vectors.push({m:2,k:5,c:bv});     // organic reload shape
          vectors.push({m:2,k:-1,c:bv});    // organic reload shape (key=-1)
          vectors.push({m:0,k:-1,c:bv});    // bounds
          vectors.push({m:0,k:11,c:bv});    // bounds
          const vres=[];
          for(let vi=0;vi<vectors.length;vi++){
            const v=vectors[vi];
            adA=[]; adB=[];
            const pre=snap();
            idxSlot.writeS32(6);
            send({run:'A', vi:vi, v:v});
            collecting=1; callStrip(stripPtr,v.m,v.k,v.c); collecting=0;
            const postA=snap(); const idxA=idxSlot.readS32();
            restore(pre);
            idxSlot.writeS32(6);
            send({run:'B', vi:vi, v:v});
            collecting=2; callStrip(twinPtr,v.m,v.k,v.c); collecting=0;
            const postB=snap(); const idxB=idxSlot.readS32();
            restore(pre);                  // as-if-never-called
            const c=cmpSnaps(postA,postB);
            const ok=c.ok&&(idxA===idxB);
            const o=Math.max(0,c.off-8);
            vres.push({v:v, ok:ok, idxA:idxA, idxB:idxB,
                       range:c.range, off:c.off,
                       a:c.ok?null:hex(postA[c.range],o,32),
                       b:c.ok?null:hex(postB[c.range],o,32),
                       adA:adA.slice(), adB:adB.slice()});
          }
          results.push({b920:bv, vres:vres});
        }
      } catch (e) { lastErr=''+e; }
      orig();
    },'void',[]));
    return 1;
  },
  more:function(n){ want+=n; return want; },
  done:function(){ return results.length; },
  dbg:function(){ return JSON.stringify({raw:rawCalls, err:lastErr, phase:abs(RVA_PHASE).readS32()}); },
  report:function(){
    Interceptor.revert(abs(RVA_HOST));
    return JSON.stringify(results);
  }
};
'''


def main():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    last = {"msg": None}
    def on_msg(m, d):
        if m.get("type") == "send":
            last["msg"] = m["payload"]
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    scr.exports_sync.init(str(ASI))
    dev.resume(pid)
    E = scr.exports_sync

    print("waiting for menu...")
    end = time.time() + 25
    while time.time() < end and E.phase() != 3:
        time.sleep(0.2)
    time.sleep(1.5)

    # Push deep FIRST (1 -> 8 -> 19), then sample after each pop on the way
    # down: mode-1 (pop-reveal) vectors need stack[depth+1] to hold a valid
    # just-popped entry (probe_prompt_calls.json, 2026-06-11).
    for s in [1, 8, 19]:
        print(f"push screen {s} ->", E.push(s))
        time.sleep(0.4)
    PER = 2
    total = 0
    try:
        for i in range(3):
            print("pop ->", E.pop())
            time.sleep(0.4)
            total += PER
            if i == 0:
                E.arm(total)
            else:
                E.more(PER)
            end = time.time() + 15
            while time.time() < end and E.done() < total:
                time.sleep(0.2)
            print(f"  done {E.done()}/{total}")
    except frida.InvalidOperationError:
        print("  PROCESS DIED — last vector before death:", last["msg"])
        return 2
    print("dbg:", E.dbg())
    samples = json.loads(E.report())
    rows = [v for s in samples for v in s["vres"]]
    n_green = sum(1 for r in rows if r["ok"])
    fired = sum(1 for r in rows if r["adA"])
    print(f"samples={len(samples)} vectors={len(rows)} GREEN={n_green} fired={fired}")
    bad = next((r for r in rows if not r["ok"]), None)
    if bad:
        print("  first mismatch vec", bad["v"], "range", bad["range"], "off", bad["off"],
              "idxA/B", bad["idxA"], bad["idxB"])
        print("   A:", bad["a"])
        print("   B:", bad["b"])
        print("   adA:", bad["adA"], " adB:", bad["adB"])
    out = ROOT / "log" / "diff_menu_prompt_navdriven.json"
    out.write_text(json.dumps({"samples": samples, "green": n_green,
                               "vectors": len(rows), "fired": fired}, indent=1))
    print("->", out)
    try: dev.kill(pid)
    except Exception: pass
    return 0 if rows and n_green == len(rows) and fired > 0 else 1


if __name__ == "__main__":
    sys.exit(main())
