# Original-side menu slide timing: observe-only attach on FUN_004325c0 (60 Hz, not a hot path),
# push screen 1 via FUN_0043d2a0(1,0) once the menu is up, log wall-clock ms + the 30 records'
# tag/type/slide at every tick until all settle. Usage: orig_anim_timing.py <out.json>
import json, os, sys, time
from pathlib import Path
import frida
GAME_ROOT = Path(os.environ["MASHED_ROOT"]); ORIG = GAME_ROOT / "original"; EXE = ORIG / "MASHED.exe"
AGENT = r'''
'use strict';
const IMG=0x00400000; let D=0; const TBL=0x00898ac0;
const A=(r)=>ptr(r+D); let rows=[], on=false, t0=0;
rpc.exports={
 init:function(asiPath){const m=Process.enumerateModules()[0]; D=m.base.toUInt32()-IMG; Module.load(asiPath); return D;},
 arm:function(){
  Interceptor.attach(A(0x004325c0),{onEnter:function(){ if(!on) return;
   const recs=[]; for(let i=0;i<30;i++){const b=A(TBL+i*0x34); const tag=b.readU32();
     if(tag!==0) recs.push([i,tag,b.add(0x10).readS32(), b.add(0x04).readS32()]);}
   rows.push({ms:Date.now()-t0, dt:A(0x007f1004).readFloat(), phase:A(0x0067eca4).readS32(), recs:recs});}});
  return 1;},
 phase:function(){return A(0x0067eca4).readS32();},
 go:function(){const nav=new NativeFunction(A(0x0043d2a0),'void',['int','int']); on=true; t0=Date.now(); rows=[]; nav(1,0); return 1;},
 n:function(){return rows.length;},
 dump:function(){return JSON.stringify(rows);}
};
'''
def main():
    env=dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"]="1"
    dev=frida.get_local_device(); pid=dev.spawn(str(EXE),cwd=str(ORIG),env=env)
    s=dev.attach(pid); sc=s.create_script(AGENT); sc.on("message",lambda m,d:None); sc.load()
    sc.exports_sync.init(str(ORIG/'mashed_re_dev.asi')); dev.resume(pid); E=sc.exports_sync
    end=time.time()+30
    while time.time()<end and E.phase()!=3: time.sleep(0.2)
    time.sleep(1.5); E.arm(); E.go(); time.sleep(4.0)
    rows=json.loads(E.dump()); Path(sys.argv[1]).write_text(json.dumps({"pid":pid,"rows":rows}))
    print("pid",pid,"ticks",len(rows))
    try: dev.kill(pid)
    except Exception: pass
main()
