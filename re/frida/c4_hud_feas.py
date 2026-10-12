# Feasibility probe (stock original) for the group-H C4 scenarios. Counts game entries into
# 0x00403160 / 0x0041db80 / 0x004295a0 / 0x00429e10 / 0x0043df00 and samples the gating globals.
# Usage: py -3.12 re/frida/c4_hud_feas.py --scen attract|quick [--secs N] [--tag T]
import json, os, sys, time
from pathlib import Path
import frida
ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, _open_handle, _exit_code
AGENT = r'''
const IMG=0x00400000; let DELTA=0; let navCtrl=-1, navUntil=0;
function abs(r){ return ptr((r+DELTA)>>>0); }
const C={}; const FIRST={};
function s32(a){ return abs(a).readS32(); } function f32(a){ return abs(a).readFloat(); }
function snap(){ return {mode:s32(0x0067e9fc), st:s32(0x0063ba8c), rule:s32(0x007f0fd0), ai:s32(0x00897ffc),
  cnt:f32(0x0063d588), sph:abs(0x00771968).readU8(), ph:s32(0x0067eca4), d:s32(0x0067e9f8),
  t8:f32(0x00636ae8), tc:f32(0x00636aec), fcc:s32(0x007f0fcc), g426:0}; }
rpc.exports={
 init:function(){ const m=Process.findModuleByName('MASHED.exe'); DELTA=m.base.toUInt32()-IMG;
  Interceptor.attach(abs(0x00497310),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if (this.p===0 && this.c===navCtrl && Date.now()<navUntil) ret.replace(ptr(0xff)); }});
  [0x00403160,0x0041db80,0x004295a0,0x00429e10,0x0043df00,0x0041b630,0x0041ccc0].forEach(function(r){ C[r.toString(16)]=0;
   Interceptor.attach(abs(r),{onEnter(){ const k=r.toString(16); C[k]++; if(!FIRST[k]) FIRST[k]=snap(); }}); });
  return DELTA; },
 press:function(c,ms){ navCtrl=c; navUntil=Date.now()+ms; return 1; },
 depth:function(){ return s32(0x0067e9f8); }, phase:function(){ return s32(0x0067eca4); },
 sel:function(){ const d=s32(0x0067e9f8); return s32(0x0067ed40+d*0x40); },
 state:function(){ const s=snap(); s.C=C; return s; }, first:function(){ return FIRST; }
};
'''
def main():
    av=sys.argv
    def opt(f,d): return av[av.index(f)+1] if f in av else d
    scen=opt("--scen","attract"); secs=int(opt("--secs","180")); tag=opt("--tag",scen)
    out=ROOT/"log"/"c4_hud"; out.mkdir(parents=True,exist_ok=True)
    env=dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"]="1"; env["MASHED_HOOK_ONLY"]="__none__"
    dev=frida.get_local_device(); pid=dev.spawn(str(statenav.EXE),cwd=str(statenav.ORIG),env=env)
    print("pid",pid,flush=True); sess=dev.attach(pid); h=_open_handle(pid)
    scr=sess.create_script(AGENT); scr.on("message",lambda m,d: print("msg",m,flush=True)); scr.load()
    X=scr.exports_sync; X.init(); dev.resume(pid); nav=Nav(scr,pid); rec={"scen":scen,"pid":pid,"samples":[]}
    try:
        nav.wait(lambda: nav.phase()==3 and nav.depth()>=1, 25.0, "title")
        if scen=="quick":
            nav.confirm_to_depth(2); nav.advance_past_load_modal(3); nav.press(12)
            nav.confirm_to_depth(4,tries=4); nav.confirm_to_depth(5,tries=4); nav.press(4); time.sleep(1.5)
            for k in range(5):
                if nav.phase()!=3: break
                nav.press(4); time.sleep(1.5)
        end=time.time()+secs; tp=0; tl=0
        while time.time()<end and nav.alive():
            s=X.state()
            if scen=="quick" and s["sph"]==3 and time.time()-tp>1.0: X.press(4,200); tp=time.time()
            if time.time()-tl>5: tl=time.time(); rec["samples"].append(s); print(int(secs-(end-time.time())), s, flush=True)
            time.sleep(0.25)
        rec["first"]=X.first(); rec["final"]=X.state()
    finally:
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        rec["exit"]=_exit_code(h); (out/f"feas_{tag}.json").write_text(json.dumps(rec,indent=1))
        print("first",rec.get("first"),flush=True)
main()
