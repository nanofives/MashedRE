# Deterministic replay of a captured guided scenario — IN-PROCESS, by explicit PID.
#
# Reads re/scenarios/<NNN-slug>/input_trace.tsv (frame-indexed, delta-encoded) and
# re-drives MASHED's DirectInput keyboard buffer per frame, so a deep game state the
# human reached is re-reached WITHOUT the human. Replaying by GetDeviceState call
# index (not wall-clock) makes it reproducible across runs.
#
# SAFETY (CANONICAL_INPUT_DESIGN.md): writes ONLY MASHED's own DirectInput buffer,
# in-process. No keybd_event/SendInput/SetForegroundWindow. Cannot affect the host PC.
#
# Spawns MASHED itself and kills ONLY that PID (multi-session safe).
#
# Usage:
#   py -3.12 re/frida/replay_session.py re/scenarios/001-ramp-airborne
#   py -3.12 re/frida/replay_session.py re/scenarios/001-ramp-airborne --asi --seconds 200
#
# With --asi the dev .asi loads, so installed-hook self-tests run against the exact
# replayed scenario — that is the canonical-scenario / installed-hook C4 evidence the
# C4_REVALIDATION backlog needs (no human in the loop).
import os, sys, time, json, argparse
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
MASHED_EXE = ORIG / "MASHED.exe"
STANDALONE_EXE = ROOT / "mashedmod" / "mashed_re.exe"

AGENT = r'''
'use strict';
let di8Hooked=false, cdHooked=false; const seenDev={};
let frameCount=0; let PLAN=[]; let pi=0; let cur=[]; let writes=0;
rpc.exports = {
  setplan: function(plan){ PLAN=plan; pi=0; cur=[]; return PLAN.length; },
  status: function(){ return {frames:frameCount, idx:pi, writes:writes, cur:cur}; }
};
function onFrame(buf){
  frameCount++;
  while (pi<PLAN.length && PLAN[pi].f<=frameCount){ cur=PLAN[pi].d; pi++; }
  for (const k of cur){ try{ buf.add(k).writeU8(0x80); writes++; }catch(e){} }  // IN-PROCESS only
}
function hookGDS(vt){ try{ const gds=vt.add(9*4).readPointer(); const key=gds.toString(); if(seenDev[key]) return; seenDev[key]=1;
  Interceptor.attach(gds,{ onEnter(a){this.cb=a[1].toInt32(); this.buf=a[2];}, onLeave(r){ if(this.cb===256) onFrame(this.buf); } });
  send({kind:'info', msg:'replayer armed on GetDeviceState @'+gds}); }catch(e){ send({kind:'err', msg:'hookGDS '+e}); } }
function hookCD(vt){ if(cdHooked) return; cdHooked=true; try{ const cd=vt.add(3*4).readPointer();
  Interceptor.attach(cd,{ onEnter(a){this.out=a[2];}, onLeave(r){ try{ const d=this.out.readPointer(); if(!d.isNull()) hookGDS(d.readPointer()); }catch(e){} } }); }catch(e){} }
function hookDI8(){ if(di8Hooked) return true; let ex=null;
  for(const m of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']){ try{ const mod=Process.findModuleByName(m); if(mod){ ex=mod.findExportByName('DirectInput8Create'); if(ex) break; } }catch(e){} }
  if(!ex) return false; di8Hooked=true;
  Interceptor.attach(ex,{ onEnter(a){this.ppv=a[3];}, onLeave(r){ try{ const di=this.ppv.readPointer(); if(!di.isNull()) hookCD(di.readPointer()); }catch(e){} } });
  send({kind:'info', msg:'hooked DirectInput8Create'}); return true; }
if(!hookDI8()){ const iv=setInterval(function(){ if(hookDI8()) clearInterval(iv); },50); }
send({kind:'ready'});
'''


def load_trace(scn_dir: Path):
    """input_trace.tsv -> frame-indexed plan [{f, d:[dik...]}]."""
    plan = []
    tf = scn_dir / "input_trace.tsv"
    with open(tf, encoding="utf-8") as f:
        next(f, None)  # header
        for line in f:
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 3:
                continue
            frame = int(parts[0])
            dik_csv = parts[2].strip()
            diks = [] if (not dik_csv or dik_csv == "(none)") else \
                   [int(x.replace("0x", ""), 16) for x in dik_csv.split(",") if x.strip()]
            plan.append({"f": frame, "d": diks})
    return plan


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("scenario", help="re/scenarios/<NNN-slug> directory")
    ap.add_argument("--asi", action="store_true", help="load dev .asi (installed-hook diff lane)")
    ap.add_argument("--seconds", type=int, default=0, help="override window (default: trace span + 30s)")
    ap.add_argument("--env", action="append", default=[])
    ap.add_argument("--observe", metavar="SPEC.json",
                    help="CANONICAL-OBSERVATION CAPTURE (2026-09-02). Record args, return value "
                         "and per-row observables for the RVAs in SPEC.json while this recorded "
                         "human scenario replays, and write log/<--observe-out>. Shares "
                         "re/frida/observe_block.js with scenario_launch.py, so the two capture "
                         "drivers cannot drift. This is the route to functions the auto-driver "
                         "cannot reach: a replayed human lap gets to game states that pulsing "
                         "accelerate never does.")
    ap.add_argument("--observe-out", default="replay_observe.json",
                    help="filename under log/ for the --observe capture")
    args = ap.parse_args()

    scn_dir = Path(args.scenario)
    if not scn_dir.is_absolute():
        scn_dir = ROOT / scn_dir
    if not (scn_dir / "input_trace.tsv").exists():
        print(f"error: no input_trace.tsv in {scn_dir}"); return 2

    meta = {}
    if (scn_dir / "meta.json").exists():
        meta = json.loads((scn_dir / "meta.json").read_text(encoding="utf-8"))
    binary = meta.get("binary", "original")
    exe = MASHED_EXE if binary == "original" else STANDALONE_EXE

    plan = load_trace(scn_dir)
    last_frame = plan[-1]["f"] if plan else 0
    seconds = args.seconds or max(60, int(last_frame / 60) + 30)
    print(f"=== replay {scn_dir.name} ===")
    print(f"  binary={exe.name}  plan-events={len(plan)}  last-frame={last_frame}  window={seconds}s  asi={args.asi}")
    if meta.get("exe_sha256"):
        print(f"  scenario anchored to {exe.name} SHA-256 {meta['exe_sha256']}")

    env = dict(os.environ)
    if not args.asi:
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    for kv in args.env:
        if "=" in kv:
            k, v = kv.split("=", 1); env[k] = v

    dev = frida.get_local_device()
    pid = dev.spawn(str(exe), cwd=str(exe.parent), env=env)
    print(f"  spawned {exe.name} pid={pid}  (will kill ONLY this pid)")
    sess = dev.attach(pid)

    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("info", "err"):
            print("  [agent]", p.get("msg"))

    agent_src = AGENT
    if args.observe:
        agent_src = agent_src + "\n" + (Path(__file__).resolve().parent
                                        / "observe_block.js").read_text(encoding="utf-8")
    scr = sess.create_script(agent_src); scr.on("message", on_msg); scr.load()
    scr.exports_sync.setplan(plan)
    if args.observe:
        spec_path = Path(args.observe)
        if not spec_path.is_absolute():
            spec_path = ROOT / spec_path
        spec = json.loads(spec_path.read_text(encoding="utf-8"))
        # Armed BEFORE resume, so the capture covers the whole replayed run
        # including boot and track load - the same ordering reason the counters
        # in scenario_launch.py are armed before the phase poke.
        print(f"  [observe] {spec_path.name} ({len(spec)} rows):",
              scr.exports_sync.tex_observe(json.dumps(spec)))
    dev.resume(pid)
    print(f"  resumed; replaying {len(plan)} frame-indexed input events in-process\n")

    try:
        deadline = time.time() + seconds
        while time.time() < deadline:
            if psutil and not psutil.pid_exists(pid):
                print("\n  game process exited."); break
            try:
                st = scr.exports_sync.status()
                cur_names = ",".join("0x%02x" % k for k in st["cur"]) or "(none)"
                print(f"\r  frame={st['frames']}  plan-idx={st['idx']}/{len(plan)}  writes={st['writes']}  held={cur_names}   ",
                      end="", flush=True)
            except Exception:
                pass
            time.sleep(1.0)
    except KeyboardInterrupt:
        print("\n  interrupted.")
    finally:
        if args.observe:
            try:
                rows = json.loads(scr.exports_sync.tex_results())
                outp = ROOT / "log" / args.observe_out
                outp.parent.mkdir(parents=True, exist_ok=True)
                outp.write_text(json.dumps(rows, indent=1), encoding="utf-8")
                print(f"\n  [observe] -> {outp}")
                print("  rva          calls  recs  d_args  d_ret  d_obs  moved  verdict")
                for rva, r in rows.items():
                    recs = r.get("recs") or []
                    dargs = {tuple(x.get("args") or []) for x in recs}
                    drets = {x.get("ret") for x in recs}
                    dobs = {tuple(o.get("v") for o in (x.get("obs") or [])) for x in recs}
                    moved = sum(1 for x in recs
                                if any(o.get("moved") for o in (x.get("obs") or [])))
                    if not recs:
                        v = "NEVER RAN"
                    elif moved:
                        v = f"non-degenerate (writes on {moved}/{len(recs)})"
                    elif len(drets) > 1 or len(dobs) > 1:
                        v = "non-degenerate"
                    elif len(dargs) > 1:
                        v = "DEGENERATE (varying args, constant output)"
                    else:
                        v = "UNDER-EXERCISED (input constant too)"
                    print(f"  {rva}  {r.get('calls',0):6d}  {len(recs):4d}  "
                          f"{len(dargs):6d}  {len(drets):5d}  {len(dobs):5d}  {moved:5d}  {v}")
            except Exception as ex:
                print(f"\n  [observe] fetch failed: {ex}")
        try:
            if not psutil or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
    print("\n  done.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
