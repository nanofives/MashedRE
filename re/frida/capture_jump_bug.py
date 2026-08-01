# capture_jump_bug.py — player-in-the-loop capture for the DEAD-JUMP bug.
#
# You drive normally (launch the game with Play Mashed.bat, or any MASHED.exe),
# run this tool, and when a jump "dies" (forward motion stops at takeoff and the
# car drops straight down) it logs the smoking gun to log/jump_capture.txt:
#   - the per-tick window around the takeoff (grounded flag, horizontal speed,
#     vertical speed) from the render transform — the OBSERVED symptom, and
#   - the FUN_0046EF70 (wheel contact spring/damper resolver) call on that frame:
#     the car's linear velocity BEFORE vs AFTER the function, which is the
#     hypothesised culprit (it overwrites +0x9B0 with the contact-spring force
#     along the contact face normal, then clamps it). If AFTER << BEFORE on the
#     takeoff frame, FUN_0046EF70 is confirmed as the velocity-killer.
#   - the per-wheel contact normals + states at that instant (to see the bad
#     ramp-lip normal).
#
# Usage (while a race is running):
#   py -3.12 re/frida/capture_jump_bug.py            # auto-attach if one MASHED
#   py -3.12 re/frida/capture_jump_bug.py --pid 1234 # explicit PID (if several)
# Runs until Ctrl-C. Keep playing and jumping until you hit a dead one; every
# dead jump is appended to the log. Share log/jump_capture.txt.
#
# Hygiene: attaches only; never spawns/kills. Refuses to guess when several
# MASHED are running (multi-session safe).
#
# Anchored to MASHED.exe SHA-256 BDCAE093...EFD3C0E. RVAs/offsets from
# re/analysis/vehicle_coupling.md, vehicle_dynamics/0046ef70.md, scenario_launch.py.
import sys, os, time, argparse
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
LOG  = ROOT / "log" / "jump_capture.txt"

def find_pid(explicit):
    if explicit: return explicit
    pids = []
    if psutil:
        for p in psutil.process_iter(["pid", "name"]):
            if (p.info["name"] or "").lower() == "mashed.exe":
                pids.append(p.info["pid"])
    else:
        import subprocess
        out = subprocess.run(["tasklist", "/fi", "imagename eq MASHED.exe", "/fo", "csv", "/nh"],
                             capture_output=True, text=True).stdout
        for line in out.splitlines():
            if "MASHED.exe" in line:
                try: pids.append(int(line.split(",")[1].strip('"')))
                except Exception: pass
    if not pids:
        print("No MASHED.exe running. Launch the game (Play Mashed.bat) and start a race first.")
        sys.exit(1)
    if len(pids) > 1:
        print(f"Multiple MASHED.exe running: {pids}. Re-run with --pid <the one you're playing>.")
        sys.exit(1)
    return pids[0]

AGENT = r"""
'use strict';
const IMG=0x400000; const B=Process.findModuleByName('MASHED.exe').base;
function ga(r){return B.add(r-IMG);}
function rF(p){ try { return p.readFloat(); } catch(e){ return NaN; } }
const REC=0x008815a0, MAT=0x00881ec8, STRIDE=0xd04, COUNT=0x008a94d0;

function carIdx(recPtr){
  const off = recPtr.sub(ga(REC));
  const v = off.toInt32 ? off.toInt32() : parseInt(off.toString());
  if (v < 0 || (v % STRIDE) !== 0) return -1;
  const i = v / STRIDE;
  return (i>=0 && i<8) ? i : -1;
}
function renderPos(i){
  const mr=ga(MAT).add(i*STRIDE); const act=mr.add(0x80).readS32()&1; const m=mr.add(act*0x40);
  return [rF(m.add(0x30)), rF(m.add(0x34)), rF(m.add(0x38))];
}
function hyp(a,b){ return Math.sqrt(a*a+b*b); }

// per-car ring buffer of recent ticks (grounded + render pos)
const HIST=24;
let ring={};   // car -> [{tick,g,x,y,z}]
let tick=0;
// last FUN_0046ef70 velocity event per car
let ef={};     // car -> {tick,g,vb:[..],va:[..]}

// hook the once-per-tick bridge: sample all cars, detect dead takeoffs
Interceptor.attach(ga(0x0047eb30), { onEnter(){
  tick++;
  const cc=ga(COUNT).readS32(); const n=(cc<0?0:(cc>8?8:cc));
  for(let i=0;i<n;i++){ try{
    const rec=ga(REC).add(i*STRIDE);
    const g=rF(rec.add(0x9e0));
    const p=renderPos(i);
    if(!ring[i]) ring[i]=[];
    const r=ring[i]; r.push({tick,g,x:p[0],y:p[1],z:p[2]}); if(r.length>HIST) r.shift();
    // detect dead takeoff: was grounded & moving fast, now airborne with collapsed
    // horizontal speed and falling.
    if(r.length>=6){
      const k=r.length-4;                 // evaluate a few ticks back (need post samples)
      const a=r[k-1], b=r[k];
      if(a.g>0.5 && b.g<0.5){             // grounded -> airborne at b
        const pre = (hyp(r[k-1].x-r[k-2].x, r[k-1].z-r[k-2].z) +
                     hyp(r[k].x-r[k-1].x,  r[k].z-r[k-1].z))/2;
        let postH=0, postV=0, m=0;
        for(let j=k;j+1<r.length;j++){ postH+=hyp(r[j+1].x-r[j].x, r[j+1].z-r[j].z); postV+=(r[j+1].y-r[j].y); m++; }
        postH=m?postH/m:0; postV=m?postV/m:0;
        if(pre>1.5 && postH < pre*0.3 && postV < 0){   // DEAD: fast in, forward speed dies, falling
          const e = ef[i];
          send({kind:'DEAD', car:i, tick:b.tick, pre:pre, postH:postH, postV:postV,
                window:r.map(s=>({t:s.tick,g:s.g,x:s.x,y:s.y,z:s.z})),
                ef: e || null,
                wheels: wheelInfo(rec)});
        }
      }
    }
  }catch(e){} }
}});

function wheelInfo(rec){
  // per-wheel contact normal candidates + state (stride 0xC4 = 0x31 dwords, 4 wheels)
  const w=[];
  for(let k=0;k<4;k++){
    const base=rec.add(0x194 + k*0xc4);
    w.push({ n:[rF(rec.add(0x65*4 + k*0x31*4 + 0x1b*4)), rF(rec.add(0x65*4 + k*0x31*4 + 0x1c*4)), rF(rec.add(0x65*4 + k*0x31*4 + 0x1d*4))],
             load: rF(base) });
  }
  return w;
}

// hook the contact resolver: capture linear velocity before/after (implicit EDI=car rec)
Interceptor.attach(ga(0x0046ef70), {
  onEnter(args){
    const rec=this.context.edi;
    const i=carIdx(rec);
    if(i<0) { this.i=-1; return; }
    this.i=i; this.rec=rec;
    this.g=rF(rec.add(0x9e0));
    this.vb=[rF(rec.add(0x9b0)), rF(rec.add(0x9b4)), rF(rec.add(0x9b8))];
  },
  onLeave(ret){
    if(this.i<0) return;
    const va=[rF(this.rec.add(0x9b0)), rF(this.rec.add(0x9b4)), rF(this.rec.add(0x9b8))];
    ef[this.i]={tick:tick, g:this.g, vb:this.vb, va:va};
  }
});
"""

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pid", type=int, default=0)
    args = ap.parse_args()
    pid = find_pid(args.pid)
    print(f"Attaching to MASHED.exe pid={pid} ... (drive and jump; Ctrl-C to stop)")
    LOG.parent.mkdir(exist_ok=True)
    sess = frida.attach(pid)
    script = sess.create_script(AGENT)
    caught = [0]
    def on_msg(m, data):
        if m.get("type") != "send":
            if m.get("type")=="error": print("  agent error:", m.get("description"))
            return
        p = m["payload"]
        if p.get("kind") != "DEAD": return
        caught[0]+=1
        e = p.get("ef")
        lines = []
        lines.append(f"\n===== DEAD JUMP #{caught[0]}  car {p['car']}  tick {p['tick']} =====")
        lines.append(f"observed: pre_hspeed={p['pre']:.2f}  post_hspeed={p['postH']:.2f}  post_vspeed(y)={p['postV']:.2f} (falling)")
        if e:
            vb, va = e["vb"], e["va"]
            hb = (vb[0]**2+vb[2]**2)**0.5; hac = (va[0]**2+va[2]**2)**0.5
            lines.append(f"FUN_0046EF70 (contact resolver) on this car, grounded={e['g']:.1f}:")
            lines.append(f"   vel BEFORE = ({vb[0]:.2f},{vb[1]:.2f},{vb[2]:.2f})  |horiz|={hb:.2f}")
            lines.append(f"   vel AFTER  = ({va[0]:.2f},{va[1]:.2f},{va[2]:.2f})  |horiz|={hac:.2f}")
            verdict = "CONFIRMED velocity-kill by FUN_0046EF70" if (hb>1.0 and hac<hb*0.4) else "0046EF70 did not obviously kill it (look elsewhere)"
            lines.append(f"   >>> {verdict}")
        else:
            lines.append("   (no FUN_0046EF70 event recorded for this car near takeoff)")
        lines.append(f"wheel contact normals/loads: {p.get('wheels')}")
        lines.append(f"tick window (t, grounded, x,y,z):")
        for s in p["window"]:
            lines.append(f"   t{s['t']} g={s['g']:.1f} pos=({s['x']:.1f},{s['y']:.1f},{s['z']:.1f})")
        text = "\n".join(lines)
        print(text)
        with open(LOG, "a") as f: f.write(text + "\n")
        print(f"\n  [logged to {LOG}]  keep playing for more, or Ctrl-C to stop.")
    script.on("message", on_msg)
    script.load()
    print("Ready. DRIVE and JUMP. Each dead jump is captured + logged.\n")
    try:
        while True: time.sleep(0.5)
    except KeyboardInterrupt:
        print(f"\nStopped. {caught[0]} dead jump(s) captured -> {LOG}")
    finally:
        try: sess.detach()
        except Exception: pass

if __name__ == "__main__":
    main()
