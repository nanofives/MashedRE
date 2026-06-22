# Hardware write-watchpoint hunt for the shutdown-corruption wild write.
#
# The eip=0 crash is a ret-to-null at the WinMain->exit boundary: the slot at
# ~0x1afe4c (WinMain-thunk FUN_00492370's saved-ESI, written ONCE at startup and
# untouched until exit because the game runs BELOW it on the stack) gets overwritten
# with 0. So ANY write to 0x1afe4c during gameplay is the wild write.
#
# This sets a hardware data breakpoint (Dr0 + Dr7, break-on-write, len 4) on the main
# thread via SetThreadContext, catches the faulting instruction through Frida's
# exception handler, reports the WRITER's eip + registers + bytes, then disables the
# watchpoint. Drives the 001 input plan to trigger the corruption.
#
# Usage: py -3.12 re/frida/watch_corruption.py --pid <PID> [--addr 0x1afe4c]
import sys, time, json, argparse
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent

AGENT = r'''
'use strict';
let WP_ADDR = 0; let mainTid = 0; let armed = false; let CAUGHT = null;
let frameCount=0, PLAN=[], pi=0, cur=[], writes=0;
let textLo=null, textHi=null;
function inText(p){ return textLo && p.compare(textLo)>=0 && p.compare(textHi)<0; }
function addText(mod){ mod.enumerateRanges('r-x').forEach(function(r){ const e=r.base.add(r.size);
  if(textLo===null||r.base.compare(textLo)<0)textLo=r.base; if(textHi===null||e.compare(textHi)>0)textHi=e; }); }

// ---- input driver (drive 001 plan per GDS frame) ----
function onFrame(buf){
  frameCount++;
  while (pi<PLAN.length && PLAN[pi].f<=frameCount){ cur=PLAN[pi].d; pi++; }
  for (const k of cur){ try{ buf.add(k).writeU8(0x80); writes++; }catch(e){} }
}
const seen={};
function hookGDS(addr){ const k=addr.toString(); if(seen[k]) return; seen[k]=1;
  Interceptor.attach(addr,{ onEnter(a){ this.cb=a[1].toInt32(); this.buf=a[2]; },
                            onLeave(){ if(this.cb===256) onFrame(this.buf); } }); }
function resolveGDS(){ let n=0;
  ['dinput8_real.DLL','DINPUT8.dll','dinput8.dll'].forEach(function(mn){
    const mod=Process.findModuleByName(mn); if(!mod) return; addText(mod);
    mod.enumerateRanges('r--').forEach(function(r){
      let a=r.base; const end=r.base.add(r.size).sub(4); let rs=null,rl=0;
      function fl(){ if(rs!==null&&rl>=16){ try{ const g=rs.add(9*4).readPointer(); if(inText(g)){ hookGDS(g); n++; } }catch(e){} } rs=null; rl=0; }
      while(a.compare(end)<0){ let p=null; try{ p=a.readPointer(); }catch(e){} if(p&&inText(p)){ if(rs===null){rs=a;rl=0;} rl++; } else fl(); a=a.add(4); }
      fl();
    });
  });
  return n;
}

// ---- Win32 thunks for debug-register watchpoint ----
const k32mod = Process.findModuleByName('kernel32.dll') || Process.findModuleByName('KERNEL32.DLL');
function k32f(name, ret, args){ return new NativeFunction(k32mod.findExportByName(name), ret, args); }
const OpenThread     = k32f('OpenThread',      'pointer', ['uint32','int','uint32']);
const SuspendThread  = k32f('SuspendThread',   'uint32',  ['pointer']);
const ResumeThread   = k32f('ResumeThread',    'uint32',  ['pointer']);
const GetThreadCtx   = k32f('GetThreadContext','int',     ['pointer','pointer']);
const SetThreadCtx   = k32f('SetThreadContext','int',     ['pointer','pointer']);
const CloseHandle    = k32f('CloseHandle',     'int',     ['pointer']);
const CTX_DEBUG = 0x00010010;     // CONTEXT_i386 | CONTEXT_DEBUG_REGISTERS
const CTX_SIZE  = 0x2cc;
// x86 CONTEXT offsets: Dr0=4, Dr7=0x18

function findMainTid(){
  let best=0, bestSp=0;
  Process.enumerateThreads().forEach(function(t){
    let sp=0; try{ sp=parseInt(''+t.context.sp,16);}catch(e){}
    if(sp>=0x00190000 && sp<0x00200000){ best=t.id; bestSp=sp; }   // main-thread stack band
  });
  return {tid:best, sp:bestSp};
}

function setWatch(tid, addr, dr7){
  const h = OpenThread(0x1fffff, 0, tid);
  if(h.isNull()) return 'OpenThread failed';
  SuspendThread(h);
  const ctx = Memory.alloc(CTX_SIZE);
  ctx.writeU32(CTX_DEBUG);                       // ContextFlags
  if(GetThreadCtx(h, ctx)===0){ ResumeThread(h); CloseHandle(h); return 'GetThreadContext failed'; }
  ctx.add(0x04).writeU32(addr);                  // Dr0 = watch address
  ctx.add(0x18).writeU32(dr7);                   // Dr7
  const ok = SetThreadCtx(h, ctx);
  ResumeThread(h); CloseHandle(h);
  return ok!==0 ? 'ok' : 'SetThreadContext failed';
}

// ---- exception handler: catch EACH write to the slot; stop when it becomes 0 ----
let nCatch = 0; let disabled = false;
Process.setExceptionHandler(function(d){
  if(disabled) return false;
  if(d.type!=='single-step' && d.type!=='breakpoint') return false;
  const ctx=d.context;
  let pc=0; try{ pc=ctx.pc; }catch(e){}
  let bb=''; try{ for(let i=-16;i<8;i++){ bb += ('0'+ptr(''+pc).add(i).readU8().toString(16)).slice(-2)+' '; } }catch(e){}
  let chain=[]; const MX_LO=0x400000,MX_HI=0x995000;
  try{ const esp=ptr(''+ctx.sp); for(let i=0;i<200;i++){ let v; try{ v=esp.add(i*4).readU32(); }catch(e){ break; }
    if(v>=MX_LO && v<MX_HI) chain.push('+0x'+(i*4).toString(16)+':0x'+v.toString(16)); } }catch(e){}
  const slot = ptr(WP_ADDR).readU32();
  nCatch++;
  const rec={ n:nCatch, pc:''+pc, addr:'0x'+WP_ADDR.toString(16), slot_val:('0x'+slot.toString(16)),
    eax:''+ctx.eax, ebx:''+ctx.ebx, ecx:''+ctx.ecx, edx:''+ctx.edx, esi:''+ctx.esi, edi:''+ctx.edi,
    ebp:''+ctx.ebp, esp:''+ctx.sp, bytes_around_pc:bb.trim(), code_chain:chain.slice(0,24),
    frame:frameCount, idx:pi };
  send({kind:'caught', data:rec});
  // the corruption is the write that leaves the slot == 0 (doexit's return smashed)
  if(slot===0){ CAUGHT=rec; disabled=true; setWatch(mainTid,0,0); }
  else if(nCatch>=40){ disabled=true; setWatch(mainTid,0,0); }   // safety cap
  return true;               // resume past the trap
});

rpc.exports = {
  setplan:function(p){ PLAN=p; pi=0; cur=[]; return PLAN.length; },
  arm:function(addr){
    WP_ADDR = addr;
    const ngds = resolveGDS();
    const m = findMainTid(); mainTid = m.tid;
    if(!mainTid) return {ok:false, msg:'main thread not found', gds:ngds};
    const dr7 = 0xd0001;     // L0=1, RW0=01(write), LEN0=11(4 bytes)
    const r = setWatch(mainTid, addr, dr7);
    armed = (r==='ok');
    return {ok:armed, msg:r, tid:mainTid, sp:m.sp, gds:ngds, slot_val:('0x'+ptr(addr).readU32().toString(16))};
  },
  status:function(){ return {frames:frameCount, idx:pi, writes:writes, caught:CAUGHT?1:0}; },
  dump:function(){ return {caught:CAUGHT, frames:frameCount}; }
};
send({kind:'ready'});
'''


def load_trace(scn_dir):
    plan=[]
    with open(scn_dir/"input_trace.tsv", encoding="utf-8") as f:
        next(f,None)
        for line in f:
            p=line.rstrip("\n").split("\t")
            if len(p)<3: continue
            diks=[] if (not p[2].strip() or p[2].strip()=="(none)") else [int(x.replace("0x",""),16) for x in p[2].split(",") if x.strip()]
            plan.append({"f":int(p[0]),"d":diks})
    return plan


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, required=True)
    ap.add_argument("--addr", default="0x1afe4c")
    ap.add_argument("--lead", type=int, default=200)
    ap.add_argument("--seconds", type=int, default=90)
    ap.add_argument("--scenario", default="re/scenarios/001-nav-demo")
    args=ap.parse_args()
    addr=int(args.addr,16)

    scn=Path(args.scenario)
    if not scn.is_absolute(): scn=ROOT/scn
    plan=load_trace(scn)
    if args.lead>0 and plan:
        fi=next((e["f"] for e in plan if e["d"]), plan[0]["f"])
        delta=args.lead-fi
        for e in plan: e["f"]=max(1,e["f"]+delta)

    dev=frida.get_local_device()
    sess=dev.attach(args.pid)
    caught={}
    def on(m,d):
        if m.get("type")=="error": print("  agent error:", m.get("description")); return
        p=m.get("payload",{})
        if p.get("kind")=="ready": print("  agent ready")
        elif p.get("kind")=="caught":
            c=p["data"]; zero = (c["slot_val"] in ("0x0","0x00000000"))
            tag = "  <<< SLOT==0 (THE CORRUPTION)" if zero else ""
            print(f"\n  [write #{c['n']}] eip={c['pc']} slot_now={c['slot_val']} esp={c['esp']} @frame={c['frame']}{tag}")
            print(f"      eax={c['eax']} ebx={c['ebx']} ecx={c['ecx']} edx={c['edx']} esi={c['esi']} edi={c['edi']} ebp={c['ebp']}")
            print(f"      bytes around pc: {c['bytes_around_pc']}")
            if zero:
                caught["c"]=c
                print( "      stack code-chain:")
                for e in c.get("code_chain",[])[:16]: print("        "+e)

    scr=sess.create_script(AGENT); scr.on("message",on); scr.load()
    scr.exports_sync.setplan(plan)
    r=scr.exports_sync.arm(addr)
    print(f"  arm: {json.dumps(r)}")
    if not r.get("ok"):
        print("  ERROR arming watchpoint; aborting"); sess.detach(); return 2

    deadline=time.time()+args.seconds
    while time.time()<deadline:
        if caught: break
        if psutil and not psutil.pid_exists(args.pid): print("\n  process exited (no catch)"); break
        try:
            st=scr.exports_sync.status()
            print(f"\r  frame={st['frames']} idx={st['idx']}/{len(plan)} writes={st['writes']} caught={st['caught']}   ",end="",flush=True)
        except Exception: pass
        time.sleep(0.5)
    try: sess.detach()
    except Exception: pass
    print("\n  done." if caught else "\n  no write caught in window.")
    return 0


if __name__=="__main__":
    sys.exit(main())
