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
const VEH_BASE=0x00881ec8, VEH_STRIDE=0xd04, REC_BASE=0x008815a0;
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
      onEnter(args){ const sp=this.context.esp;
        this.cam = sp.add(4).readU32();
        // param_2 = force_reset (RaceCamera.h:60 "force_reset = param_2"). Must be
        // captured PER ROW: it selects the snap path over the spring path
        // (RaceCamera.cpp:335-367), so a reset frame the driver does not know about
        // would show up as a large spurious pose divergence.
        this.reset = sp.add(8).readS32(); },
      onLeave(r){
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
    # Capture the FULL ribbon. The previous version took only the first 8 of 30,
    # which alone made an offline drive of the port impossible (RaceCamera::Update
    # early-returns without a ribbon, and wraps on node+1 against the live count).
    nodes = json.loads(E.nodes(nodecount or 8))
    (LOG / "camera_probe_static.json").write_text(json.dumps(
        {"fires_3s": fires3s, "globals": g, "override_table": ovr,
         "node_count": nodecount, "nodes": nodes}, indent=1))
    print(f"nodes captured: {len(nodes)} of {nodecount}")
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
    with open(LOG / "camera_trace_v2.csv", "w", newline="") as f:
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
    try: dev.kill(pid)
    except Exception: pass
    print("done -> log/camera_trace_v2.csv, log/camera_probe_static.json")
    return 0


if __name__ == "__main__":
    sys.exit(main())
