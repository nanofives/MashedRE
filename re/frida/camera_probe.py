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
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
LOG = ROOT / "log"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4;
const CAM=0x00897fe0, OVR=0x0063a5f0, NODES=0x00663658, NCOUNT=0x0066d6d8;
const VEH_BASE=0x00881ec8, VEH_STRIDE=0xd04;
let pressCtrl=-1, pressUntil=0;
let camFires=0; let trace=[]; let tracing=false;
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }});
    return DELTA; },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  hookcam:function(){
    Interceptor.attach(abs(0x00446520), {
      onEnter(args){ this.cam = this.context.esp.add(4).readU32(); },
      onLeave(r){
        camFires++;
        if (!tracing) return;
        const c = ptr(this.cam);
        const row = {t: Date.now(), fires: camFires,
          pos:[c.add(0x40).readFloat(), c.add(0x44).readFloat(), c.add(0x48).readFloat()],
          tgt:[c.add(0x4c).readFloat(), c.add(0x50).readFloat(), c.add(0x54).readFloat()],
          elev:c.add(0x34).readFloat(), azim:c.add(0x38).readFloat(),
          zoom:c.add(0x9a0).readFloat(), pair:[c.add(0x994).readS32(), c.add(0x998).readS32()],
          cars:[] };
        for (let i=0;i<4;i++){
          const v=abs(VEH_BASE+i*VEH_STRIDE);
          row.cars.push([v.add(0x30).readFloat(), v.add(0x34).readFloat(), v.add(0x38).readFloat()]);
        }
        trace.push(row);
        if (trace.length > 1200) tracing = false;
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
    LOG.mkdir(exist_ok=True)
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT); scr.on("message", lambda m, d: None); scr.load()
    scr.exports_sync.init()
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
            if wait(lambda: E.depth() >= target, 2.0, f"d{target}"): return True
        return E.depth() >= target

    print("booting...")
    wait(lambda: E.phase() == 3 and E.depth() >= 1, 20, "title")
    time.sleep(1.0)
    confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
    confirm_to(3)
    E.setsel(1); time.sleep(0.3)        # Quick Battle
    confirm_to(4, 4)
    confirm_to(5, 4)
    press(4); time.sleep(1.5)
    for _ in range(5):
        if E.phase() != 3: break
        press(4); time.sleep(1.5)
    print(f"in race? phase={E.phase()}")
    time.sleep(2.0)

    E.hookcam()
    time.sleep(3.0)
    fires3s = E.fires()
    print(f"FUN_00446520 fires in 3s: {fires3s}")

    g = json.loads(E.globals())
    print("globals:", g)
    nodecount = max(0, min(g.get("nodecount", 0), 200))
    ovr = json.loads(E.ovrtable(min(nodecount or 32, 64)))
    nodes = json.loads(E.nodes(min(nodecount or 8, 8)))
    (LOG / "camera_probe_static.json").write_text(json.dumps(
        {"fires_3s": fires3s, "globals": g, "override_table": ovr,
         "nodes_first8": nodes}, indent=1))
    print(f"override entries (first {len(ovr)}): " +
          ", ".join(f"({a:.1f},{b:.1f},{c:.1f})" for a, b, c in ovr[:8]))

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
    with open(LOG / "camera_trace.csv", "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["t", "fires", "px", "py", "pz", "tx", "ty", "tz",
                    "elev", "azim", "zoom", "pairA", "pairB",
                    "c0x", "c0y", "c0z", "c1x", "c1y", "c1z",
                    "c2x", "c2y", "c2z", "c3x", "c3y", "c3z"])
        for r in rows:
            w.writerow([r["t"], r["fires"], *r["pos"], *r["tgt"],
                        r["elev"], r["azim"], r["zoom"], *r["pair"],
                        *r["cars"][0], *r["cars"][1], *r["cars"][2], *r["cars"][3]])
    try: dev.kill(pid)
    except Exception: pass
    print("done -> log/camera_trace.csv, log/camera_probe_static.json")
    return 0


if __name__ == "__main__":
    sys.exit(main())
