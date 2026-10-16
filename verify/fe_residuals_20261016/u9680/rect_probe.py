"""U-9680 probe: read-only. Spawns original\MASHED.exe (MASHED_RE_NO_AUTO_HOOK=1), hooks the cold
font-init path (FUN_004275d0, FUN_00552920) BEFORE resume and, on leave, snapshots the inputs of the
rectangle path whose bbox becomes [0x0067d828..0x0067d834]:
  DAT_00912b08/0c (view window x2), DAT_00912be0, DAT_00912c0c (camera) +0x14 +0x60 +0x68 +0x6c,
  matrices 0x00912a44, 0x00912b18, 0x00912b58, 0x00912b98 (16 floats each), the matrix-stack top,
  camera frame LTM, the transform fn pointer *(DAT_007d3ffc+0xc+DAT_007d3ff8), and the result bbox.
Writes nothing into the game. Kills only its own PID."""
import json, os, sys, time
from pathlib import Path
import frida
ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed"); ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0; const snaps=[];
function abs(r){return ptr(r+DELTA);}
function f16(p){const a=[];for(let i=0;i<16;i++)a.push(p.add(i*4).readU32().toString(16));return a;}
function snap(tag){
  const o={tag:tag};
  o.b08=abs(0x00912b08).readU32().toString(16); o.b0c=abs(0x00912b0c).readU32().toString(16);
  o.be0=abs(0x00912be0).readU32().toString(16); o.bd8=abs(0x00912bd8).readU32();
  o.b04=abs(0x00912b04).readU32();
  const cam=abs(0x00912c0c).readPointer(); o.cam=cam.toString();
  if(!cam.isNull()){ o.cam14=cam.add(0x14).readU32(); o.cam60=cam.add(0x60).readU32().toString(16);
    o.cam68=cam.add(0x68).readU32().toString(16); o.cam6c=cam.add(0x6c).readU32().toString(16);
    const fr=cam.add(4).readPointer(); o.frame=fr.toString();
    if(!fr.isNull()){ o.frame_ltm=f16(fr.add(0x50)); o.frame_mdl=f16(fr.add(0x10)); }
    o.cam_view=f16(cam.add(0x10)); }
  o.m_a44=f16(abs(0x00912a44)); o.m_b18=f16(abs(0x00912b18)); o.m_b58=f16(abs(0x00912b58)); o.m_b98=f16(abs(0x00912b98));
  const sp=abs(0x00912b04).readU32(); const top=abs(0x00912a84).add(sp*4).readPointer();
  o.stack_top=top.isNull()?null:f16(top);
  o.d3ff8=abs(0x007d3ff8).readU32().toString(16); o.d3ffc=abs(0x007d3ffc).readU32().toString(16);
  try{ const g8=abs(0x007d3ff8).readPointer(); const gc=abs(0x007d3ffc).readPointer(); o.g8=g8.toString(); o.gc=gc.toString();
       o.xform_fn=g8.add(0xc).add(gc.toUInt32()).readPointer().toString(); }catch(e){o.xform_err=String(e);}
  o.bbox=[0,4,8,12].map(function(k){return abs(0x0067d828+k).readU32().toString(16);});
  snaps.push(o);
}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    [0x004275d0,0x00552920,0x00553cf0].forEach(function(a){
      Interceptor.attach(abs(a),{onLeave(){ snap('leave_'+a.toString(16)); }});
    });
    return DELTA;
  },
  now:function(){ snap('now'); return 1; },
  phase:function(){ return abs(0x0067eca4).readS32(); },
  result:function(){ return JSON.stringify(snaps); }
};
'''
def main():
    out = Path(sys.argv[1])
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env); print("spawned PID", pid, flush=True)
    try:
        sess = dev.attach(pid); scr = sess.create_script(AGENT); scr.on("message", lambda m, d: print(m)); scr.load()
        E = scr.exports_sync; E.init(); dev.resume(pid)
        end = time.time() + 40
        while time.time() < end and E.phase() != 3: time.sleep(0.2)
        time.sleep(2); E.now()
        out.write_text(json.dumps({"pid": pid, "snaps": json.loads(E.result())}, indent=1)); print("ok", flush=True)
    finally:
        try: dev.kill(pid); print("killed PID", pid)
        except Exception as e: print("kill", pid, e)
main()
