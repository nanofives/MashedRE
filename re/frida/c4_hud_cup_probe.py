# Reachability probe for HudDualLabelRender 0x004295a0 (group-H C4, triage A1).
#
# 0x004295a0 has three call sites, all in FUN_00429e10 (0x0042a077, 0x0042a08e, 0x0042a16a), behind
#   FUN_00443080() == 1  ([0x00897ffc], 0x00429f88) and FUN_0040e350() == 0xb ([0x0063ba8c], 0x00429f9b),
# then switch [0x007f0fd0]-5 (table 0x0042a44c): rule 5 / 8 / 9 / 10 reach a call site, 6 / 7 do not.
# FUN_00410510 writes [0x0063ba8c] = 0xb (0x00410645) and [0x00897ffc] = 1 (VehicleSeedWritePair,
# 0x0041067e / 0x00410811) when its rule test fires. Rules 5/8/9/10 come only from the cup event
# table 0x005f65c8 for modes 4 and 5 (game_mode_rules_REmap_20260616.md step 3).
#
# This probe boots the STOCK original from an install COPY (--root; never original\) whose save
# was edited in the copy, walks the challenge flow (depth-3 cursor 0), presses down --cup times at
# depth 4 and --event times at depth 5, launches, and logs mode / rule / state / counters.
# Usage: py -3.12 re/frida/c4_hud_cup_probe.py --root <copy>\original [--cup K] [--event N]
#        [--secs S] [--pulse 0|1] [--tag T]
import json, os, sys, time
from pathlib import Path
import frida
ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
from statenav import Nav, _open_handle, _exit_code

AGENT = r'''
const IMG=0x00400000; let DELTA=0; let navCtrl=-1, navUntil=0;
function abs(r){ return ptr((r+DELTA)>>>0); }
function s32(a){ return abs(a).readS32(); } function f32(a){ return abs(a).readFloat(); }
const C={}; const FIRST={}; const PIZ=[];
function snap(){ let o={}; try { const d=s32(0x0067e9f8); o={d:d, ph:s32(0x0067eca4), type:s32(0x0067ed3c+d*0x40),
  sel:s32(0x0067ed40+d*0x40), f184:s32(0x0067f184), mode:s32(0x0067e9fc), f17c:s32(0x0067f17c), rule:s32(0x007f0fd0),
  st:s32(0x0063ba8c), ai:s32(0x00897ffc), fcc:s32(0x007f0fcc), sph:abs(0x00771968).readU8(), y:f32(0x0063b910),
  fe4:f32(0x007f0fe4)}; } catch(e){} return o; }
rpc.exports={
 init:function(){ const m=Process.findModuleByName('MASHED.exe'); DELTA=m.base.toUInt32()-IMG;
  Interceptor.attach(abs(0x00497310),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if (this.p===0 && this.c===navCtrl && Date.now()<navUntil) ret.replace(ptr(0xff)); }});
  [0x004295a0,0x00429e10,0x00410510].forEach(function(r){ const k=r.toString(16); C[k]=0;
    Interceptor.attach(abs(r),{onEnter(){ C[k]++; if(!FIRST[k]) FIRST[k]=snap(); }}); });
  let cf=null; try { cf=Process.getModuleByName('kernel32.dll').findExportByName('CreateFileA'); } catch(e){}
  if (cf) Interceptor.attach(cf,{onEnter(a){ try{ const s=a[0].readAnsiString(); if (s && /\.piz$/i.test(s) && PIZ.length<60) PIZ.push(s); }catch(e){} }});
  return DELTA; },
 press:function(c,ms){ navCtrl=c; navUntil=Date.now()+ms; return 1; },
 depth:function(){ return s32(0x0067e9f8); }, phase:function(){ return s32(0x0067eca4); },
 sel:function(){ const d=s32(0x0067e9f8); return s32(0x0067ed40+d*0x40); },
 state:function(){ const s=snap(); s.C=C; return s; }, first:function(){ return FIRST; }, piz:function(){ return PIZ; }
};
'''

def main():
    av = sys.argv
    def opt(f, d): return av[av.index(f) + 1] if f in av else d
    root = Path(opt("--root", "")); assert root.name.lower() == "original" and (root / "MASHED.exe").exists()
    assert root.resolve() != (ROOT / "original").resolve() and "Proyectos\\Mashed\\original" not in str(root.resolve()), "refuse: real install"
    cup = int(opt("--cup", "0")); ev = int(opt("--event", "0")); secs = int(opt("--secs", "120"))
    pulse = int(opt("--pulse", "1")); tag = opt("--tag", f"cup{cup}_ev{ev}")
    out = ROOT / "log" / "c4_hud" / "cup"; out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    dev = frida.get_local_device(); pid = dev.spawn(str(root / "MASHED.exe"), cwd=str(root), env=env)
    h = _open_handle(pid)
    rec = {"root": str(root), "cup": cup, "event": ev, "pid": pid, "log": [], "samples": []}
    def log(m): print(m, flush=True); rec["log"].append(m)
    import psutil
    class _N:  # alive() before Nav exists
        def alive(self): return psutil.pid_exists(pid)
    nav = _N()
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT); scr.on("message", lambda m, d: print("msg", m, flush=True)); scr.load()
        X = scr.exports_sync; X.init(); dev.resume(pid); nav = Nav(scr, pid)
        nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 25.0, "title")
        nav.confirm_to_depth(2); nav.advance_past_load_modal(3); log(f"  d3: {X.state()}")
        nav.confirm_to_depth(4, tries=4); time.sleep(0.5); log(f"  d4: {X.state()}")
        for _ in range(cup): nav.press(12); time.sleep(0.4); log(f"  d4 down: {X.state()}")
        nav.confirm_to_depth(5, tries=4); time.sleep(0.5); log(f"  d5: {X.state()}")
        for k in [int(x) for x in opt("--d5keys", "").split(",") if x]:
            nav.press(k); time.sleep(0.5); log(f"  d5 key {k}: {X.state()}")
        for _ in range(ev): nav.press(12); time.sleep(0.4); log(f"  d5 down: {X.state()}")
        if "--nolaunch" in av: raise SystemExit
        nav.press(4); time.sleep(1.5); log(f"  after confirm: {X.state()}")
        for k in range(6):
            if nav.phase() != 3: break
            nav.press(4); time.sleep(1.5); log(f"  start-attempt {k}: {X.state()}")
        end = time.time() + secs; tp = 0; tl = 0
        while time.time() < end and nav.alive():
            s = X.state()
            if pulse and s.get("sph") == 3 and time.time() - tp > 1.0: X.press(4, 200); tp = time.time()
            if time.time() - tl > 5: tl = time.time(); rec["samples"].append(s); log(f"   {int(secs-(end-time.time()))} {s}")
            time.sleep(0.25)
        rec["first"] = X.first(); rec["piz"] = X.piz()
    except SystemExit:
        pass
    finally:
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        rec["exit"] = _exit_code(h); (out / f"probe_{tag}.json").write_text(json.dumps(rec, indent=1))
        print("first", rec.get("first"), "\npiz", rec.get("piz"), flush=True)

main()
