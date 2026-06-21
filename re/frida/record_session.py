# Guided-playthrough capture — passive, IN-PROCESS, by explicit PID.
#
# While a HUMAN plays the real spawned game window, records:
#   (1) per-frame DirectInput keyboard state, delta-encoded + frame-indexed
#       -> re/scenarios/<NNN-slug>/input_trace.tsv   (replayable deterministically)
#   (2) first-hit coverage over a TARGETED candidate RVA set (one-shot detach)
#       -> re/scenarios/<NNN-slug>/coverage.tsv       (what this play exercised)
#   + meta.json (binary, exe SHA-256 anchor, date, env, frame count).
#
# SAFETY (CANONICAL_INPUT_DESIGN.md): NEVER OS-level input injection. The recorder
# only READS the DirectInput buffer dinput8 fills from the user's real keyboard; the
# user's physical key presses are what get captured. No keybd_event/SendInput/focus.
#
# Spawns MASHED itself (so DirectInput8Create is caught at boot) and kills ONLY that
# PID. Multi-session safe: never blanket-kills MASHED by name.
#
# Usage:
#   py -3.12 re/frida/record_session.py --name ramp-airborne --mode physics --seconds 180
#   py -3.12 re/frida/record_session.py --name topdog --cov @cov_topdog.txt --seconds 150
#   py -3.12 re/frida/record_session.py --name menus --cov 0x43c5b0,0x473ee0 --no-asi
#   py -3.12 re/frida/record_session.py --selftest-roundtrip   # autonomous, no human input
#
# Stops on: --seconds elapsed, OR the game process exiting (quit/crash). The trace +
# coverage are flushed to disk every ~1s, so a mid-jump crash still keeps what played.
import os, sys, time, json, csv, hashlib, datetime, argparse
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
MASHED_EXE = ORIG / "MASHED.exe"
STANDALONE_EXE = ROOT / "mashedmod" / "mashed_re.exe"
SCN_DIR = ROOT / "re" / "scenarios"
HOOKS_CSV = ROOT / "hooks.csv"

# DIK scancode -> name (display only; the trace stores raw indices 0..255).
DIK_NAME = {
    0x01: "ESC", 0x1C: "ENTER", 0x39: "SPACE", 0x0F: "TAB",
    0xC8: "UP", 0xD0: "DOWN", 0xCB: "LEFT", 0xCD: "RIGHT",
    0x1D: "LCTRL", 0x9D: "RCTRL", 0x2A: "LSHIFT", 0x36: "RSHIFT",
    0x38: "LALT", 0xB8: "RALT",
    0x11: "W", 0x1E: "A", 0x1F: "S", 0x20: "D",
}

# Default candidate RVA sets per --mode (image-base 0x00400000 RVAs).
MODE_COV = {
    # airborne physics group + per-frame dispatcher (COVERAGE_SCENARIO_FINDINGS_2026-06-17.md)
    "physics": [0x0046b540, 0x00470670, 0x0046ddb0, 0x00467650, 0x00468980, 0x00470c70],
    "generic": [],
}

# ---------------------------------------------------------------------------
# Frida agent — read-only input recorder + one-shot coverage.
# ---------------------------------------------------------------------------
AGENT = r'''
'use strict';
let di8Hooked=false, cdHooked=false; const seenDev={};
let frameCount=0, t0=0;
let prevKey=null;
const EVENTS=[];          // {f, t, d:[dik...]}  delta-encoded
const COV=[];             // {rva, f, t}
let covSet=null;          // Set of armed rvas (to dedup)
const _toDetach=[];

function onFrame(buf){
  frameCount++;
  if (t0===0) t0=Date.now();
  let down=[];
  try { for (let i=0;i<256;i++){ if (buf.add(i).readU8() & 0x80) down.push(i); } } catch(e){}
  const keyStr = down.join(',');
  if (keyStr !== prevKey){ EVENTS.push({f:frameCount, t:Date.now()-t0, d:down}); prevKey=keyStr; }
}

function armCoverage(modName, imgBase, rvas){
  const mod = Process.findModuleByName(modName);
  if (!mod){ send({kind:'err', msg:'cov: module '+modName+' not found'}); return 0; }
  covSet = new Set();
  let n=0;
  rvas.forEach(function(rva){
    try{
      const addr = mod.base.add(rva - imgBase);
      const holder = {hit:false, rva:rva};
      holder.L = Interceptor.attach(addr, { onEnter(){
        if (holder.hit) return; holder.hit=true;
        COV.push({rva:rva, f:frameCount, t:Date.now()-t0});
        _toDetach.push(holder.L);                 // deferred (never detach from inside cb)
      }});
      n++;
    }catch(e){ send({kind:'err', msg:'cov arm 0x'+rva.toString(16)+': '+e}); }
  });
  // flush detaches off the callback path
  setInterval(function(){ if(!_toDetach.length) return; const d=_toDetach.splice(0); d.forEach(function(L){ try{L.detach();}catch(e){} }); }, 200);
  send({kind:'info', msg:'coverage armed on '+n+'/'+rvas.length+' candidates in '+modName+' @'+mod.base});
  return n;
}

rpc.exports = {
  armcoverage: function(modName, imgBase, rvas){ return armCoverage(modName, imgBase, rvas); },
  dump: function(){ return {events:EVENTS, frames:frameCount, cov:COV}; }
};

function hookGDS(vt){
  try{
    const gds = vt.add(9*4).readPointer(); const key=gds.toString();
    if (seenDev[key]) return; seenDev[key]=1;
    Interceptor.attach(gds,{
      onEnter(a){ this.cb=a[1].toInt32(); this.buf=a[2]; },
      onLeave(r){ if (this.cb===256) onFrame(this.buf); }    // 256 => keyboard immediate state
    });
    send({kind:'info', msg:'recorder armed on GetDeviceState @'+gds});
  }catch(e){ send({kind:'err', msg:'hookGDS '+e}); }
}
function hookCD(vt){
  if (cdHooked) return; cdHooked=true;
  try{ const cd=vt.add(3*4).readPointer();
    Interceptor.attach(cd,{ onEnter(a){this.out=a[2];},
      onLeave(r){ try{ const d=this.out.readPointer(); if(!d.isNull()) hookGDS(d.readPointer()); }catch(e){} } });
  }catch(e){ send({kind:'err', msg:'hookCD '+e}); }
}
function hookDI8(){
  if (di8Hooked) return true;
  let ex=null;
  for (const m of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']){
    try{ const mod=Process.findModuleByName(m); if(mod){ ex=mod.findExportByName('DirectInput8Create'); if(ex) break; } }catch(e){}
  }
  if(!ex) return false; di8Hooked=true;
  Interceptor.attach(ex,{ onEnter(a){this.ppv=a[3];},
    onLeave(r){ try{ const di=this.ppv.readPointer(); if(!di.isNull()) hookCD(di.readPointer()); }catch(e){} } });
  send({kind:'info', msg:'hooked DirectInput8Create'});
  return true;
}
if (!hookDI8()){ const iv=setInterval(function(){ if(hookDI8()) clearInterval(iv); }, 50); }
send({kind:'ready'});
'''

# ---------------------------------------------------------------------------
# Round-trip self-test agent: inject a synthetic frame-indexed plan AND record
# what the buffer holds afterwards, in one process. Proves the record<->replay
# plumbing deterministically with NO human and NO OS injection.
# ---------------------------------------------------------------------------
SELFTEST_AGENT = r'''
'use strict';
let di8Hooked=false, cdHooked=false; const seenDev={};
let frameCount=0, t0=0, prevKey=null;
const EVENTS=[];
let PLAN=[]; let pi=0; let cur=[];
rpc.exports = {
  setplan: function(plan){ PLAN=plan; pi=0; cur=[]; return PLAN.length; },
  dump: function(){ return {events:EVENTS, frames:frameCount}; }
};
function onFrame(buf){
  frameCount++; if(t0===0) t0=Date.now();
  while (pi<PLAN.length && PLAN[pi].f<=frameCount){ cur=PLAN[pi].d; pi++; }
  for (const k of cur){ try{ buf.add(k).writeU8(0x80); }catch(e){} }   // IN-PROCESS inject
  let down=[]; try{ for(let i=0;i<256;i++){ if(buf.add(i).readU8()&0x80) down.push(i); } }catch(e){}
  const s=down.join(','); if(s!==prevKey){ EVENTS.push({f:frameCount, d:down}); prevKey=s; }  // record back
}
function hookGDS(vt){ try{ const gds=vt.add(9*4).readPointer(); const key=gds.toString(); if(seenDev[key]) return; seenDev[key]=1;
  Interceptor.attach(gds,{ onEnter(a){this.cb=a[1].toInt32(); this.buf=a[2];}, onLeave(r){ if(this.cb===256) onFrame(this.buf); } });
  send({kind:'info', msg:'selftest armed @'+gds}); }catch(e){ send({kind:'err', msg:'hookGDS '+e}); } }
function hookCD(vt){ if(cdHooked) return; cdHooked=true; try{ const cd=vt.add(3*4).readPointer();
  Interceptor.attach(cd,{ onEnter(a){this.out=a[2];}, onLeave(r){ try{ const d=this.out.readPointer(); if(!d.isNull()) hookGDS(d.readPointer()); }catch(e){} } }); }catch(e){} }
function hookDI8(){ if(di8Hooked) return true; let ex=null;
  for(const m of ['DINPUT8.dll','dinput8_real.DLL','dinput8.dll']){ try{ const mod=Process.findModuleByName(m); if(mod){ ex=mod.findExportByName('DirectInput8Create'); if(ex) break; } }catch(e){} }
  if(!ex) return false; di8Hooked=true;
  Interceptor.attach(ex,{ onEnter(a){this.ppv=a[3];}, onLeave(r){ try{ const di=this.ppv.readPointer(); if(!di.isNull()) hookCD(di.readPointer()); }catch(e){} } });
  return true; }
if(!hookDI8()){ const iv=setInterval(function(){ if(hookDI8()) clearInterval(iv); },50); }
send({kind:'ready'});
'''


def sha256(p: Path) -> str:
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest().upper()


def load_hooks_index():
    """rva(int) -> (name, subsystem, confidence) from hooks.csv (best-effort)."""
    idx = {}
    if not HOOKS_CSV.exists():
        return idx
    try:
        with open(HOOKS_CSV, newline="", encoding="utf-8", errors="replace") as f:
            r = csv.reader(f)
            header = next(r, None)
            for row in r:
                if not row:
                    continue
                try:
                    rva = int(row[0].strip().replace("0x", ""), 16)
                except ValueError:
                    continue
                name = row[1].strip() if len(row) > 1 else ""
                sub = row[2].strip() if len(row) > 2 else ""
                conf = row[3].strip() if len(row) > 3 else ""
                idx[rva] = (name, sub, conf)
    except Exception:
        pass
    return idx


def parse_cov_arg(val: str):
    if not val:
        return []
    if val.startswith("@"):
        p = Path(val[1:])
        if not p.is_absolute():
            p = ROOT / p
        rvas = []
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines():
            line = line.split("#", 1)[0].strip()
            if line:
                rvas.append(int(line.replace("0x", ""), 16))
        return rvas
    return [int(x.strip().replace("0x", ""), 16) for x in val.split(",") if x.strip()]


def next_ordinal():
    SCN_DIR.mkdir(parents=True, exist_ok=True)
    n = 0
    for d in SCN_DIR.iterdir():
        if d.is_dir() and d.name[:3].isdigit():
            n = max(n, int(d.name[:3]))
    return n + 1


def write_artifacts(out_dir: Path, dump, meta, hooks_idx):
    out_dir.mkdir(parents=True, exist_ok=True)
    # input_trace.tsv (delta-encoded)
    with open(out_dir / "input_trace.tsv", "w", encoding="utf-8") as f:
        f.write("frame\tt_ms\tdik_csv\tnames_csv\n")
        for e in dump["events"]:
            diks = e["d"]
            dik_csv = ",".join("0x%02x" % k for k in diks)
            names = ",".join(DIK_NAME.get(k, "0x%02x" % k) for k in diks) if diks else "(none)"
            f.write(f"{e['f']}\t{e['t']}\t{dik_csv}\t{names}\n")
    # coverage.tsv (annotated from hooks.csv)
    with open(out_dir / "coverage.tsv", "w", encoding="utf-8") as f:
        f.write("rva\tfirst_frame\tfirst_t_ms\tname\tsubsystem\tconfidence\n")
        for c in sorted(dump["cov"], key=lambda c: c["f"]):
            name, sub, conf = hooks_idx.get(c["rva"], ("", "", ""))
            f.write(f"0x{c['rva']:08x}\t{c['f']}\t{c['t']}\t{name}\t{sub}\t{conf}\n")
    meta = dict(meta)
    meta["frames"] = dump["frames"]
    meta["input_events"] = len(dump["events"])
    meta["coverage_hits"] = len(dump["cov"])
    with open(out_dir / "meta.json", "w", encoding="utf-8") as f:
        json.dump(meta, f, indent=2)


def run_selftest_roundtrip():
    """Spawn ONE MASHED, inject a synthetic frame-indexed plan in-process and record
    it back; assert equality. Validates the full pipeline with no human / no OS input."""
    print("=== record_session round-trip self-test (no human, in-process only) ===")
    # synthetic plan: DOWN held f100..f120, then ENTER held f200..f210, then released.
    plan = [
        {"f": 100, "d": [0xD0]},
        {"f": 121, "d": []},
        {"f": 200, "d": [0x1C]},
        {"f": 211, "d": []},
    ]
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(MASHED_EXE), cwd=str(ORIG), env=env)
    print(f"  spawned MASHED pid={pid} (will kill ONLY this pid)")
    sess = dev.attach(pid)

    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("info", "err"):
            print("  [agent]", p.get("msg"))

    scr = sess.create_script(SELFTEST_AGENT); scr.on("message", on_msg); scr.load()
    scr.exports_sync.setplan(plan)
    dev.resume(pid)
    print("  resumed; menu polls GetDeviceState ~60/s — collecting ~240 frames (~6-10s)")
    ok = True
    alive = True
    try:
        deadline = time.time() + 25
        last_frames = 0
        while time.time() < deadline:
            if psutil and not psutil.pid_exists(pid):
                print("  process exited early"); ok = False; alive = False; break
            d = scr.exports_sync.dump()
            last_frames = d["frames"]
            if last_frames >= 230:
                break
            time.sleep(0.5)
        if not alive:
            print("  BOOT FAILED: MASHED exited before any input was polled.")
            print("  -> environment/boot issue (NOT the recorder); see input_recon.py reproduces it.")
            return 1
        d = scr.exports_sync.dump()
        rec = {e["f"]: e["d"] for e in d["events"]}
        print(f"  frames polled: {d['frames']}; recorded {len(d['events'])} change-events")
        # Expect a DOWN press starting at/after f100 and an ENTER press at/after f200.
        down_seen = any(0xD0 in v for v in rec.values())
        enter_seen = any(0x1C in v for v in rec.values())
        # Frame-index fidelity: first DOWN event frame should be ~100 (>=100, < 130).
        down_frames = sorted(f for f, v in rec.items() if 0xD0 in v)
        enter_frames = sorted(f for f, v in rec.items() if 0x1C in v)
        print(f"  DOWN recorded at frames: {down_frames[:3]}  ENTER at: {enter_frames[:3]}")
        if not (down_seen and enter_seen):
            print("  FAIL: injected keys were not recorded back"); ok = False
        elif not (down_frames and 100 <= down_frames[0] < 140):
            print(f"  FAIL: DOWN frame index off (got {down_frames[:1]}, expected ~100)"); ok = False
        elif not (enter_frames and 200 <= enter_frames[0] < 240):
            print(f"  FAIL: ENTER frame index off (got {enter_frames[:1]}, expected ~200)"); ok = False
        else:
            print("  PASS: in-process inject -> record round-trip is frame-accurate")
    finally:
        try:
            if not psutil or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--name", help="scenario slug (kebab-case)")
    ap.add_argument("--mode", choices=["physics", "generic"], default="generic")
    ap.add_argument("--cov", default="", help="candidate RVAs: comma list or @file (overrides --mode set)")
    ap.add_argument("--seconds", type=int, default=180, help="max recording window")
    ap.add_argument("--binary", choices=["original", "standalone"], default="original")
    ap.add_argument("--asi", dest="asi", action="store_true", help="load dev .asi hooks")
    ap.add_argument("--no-asi", dest="asi", action="store_false")
    ap.add_argument("--env", action="append", default=[], help="extra env K=V (repeatable)")
    ap.add_argument("--selftest-roundtrip", action="store_true")
    ap.set_defaults(asi=None)
    args = ap.parse_args()

    if args.selftest_roundtrip:
        return run_selftest_roundtrip()

    if not args.name:
        print("error: --name <slug> is required (or use --selftest-roundtrip)"); return 2

    exe = MASHED_EXE if args.binary == "original" else STANDALONE_EXE
    img_base = 0x00400000 if args.binary == "original" else 0x00010000
    mod_name = exe.name
    if not exe.exists():
        print(f"error: {exe} not found"); return 2

    # default: physics mode loads the .asi (so the per-branch self-test also logs);
    # generic mode is stock unless --asi given.
    asi = args.asi if args.asi is not None else (args.mode == "physics")
    cov = parse_cov_arg(args.cov) if args.cov else MODE_COV[args.mode]

    env = dict(os.environ)
    if not asi:
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if args.mode == "physics":
        env.setdefault("MASHED_PHYS_C4_SELFTEST", "1")
    for kv in args.env:
        if "=" in kv:
            k, v = kv.split("=", 1); env[k] = v

    ordinal = next_ordinal()
    out_dir = SCN_DIR / f"{ordinal:03d}-{args.name}"
    hooks_idx = load_hooks_index()

    print(f"=== record_session: {out_dir.name} ===")
    print(f"  binary={mod_name} (base 0x{img_base:08x})  asi={asi}  mode={args.mode}")
    print(f"  coverage candidates: {len(cov)}  window={args.seconds}s")
    print("  computing exe SHA-256 anchor...")
    exe_sha = sha256(exe)
    print(f"  {mod_name} SHA-256 = {exe_sha}")

    meta = {
        "name": args.name, "ordinal": ordinal, "slug": out_dir.name,
        "binary": args.binary, "exe": str(exe), "exe_sha256": exe_sha,
        "image_base": "0x%08x" % img_base, "asi": asi, "mode": args.mode,
        "date": datetime.datetime.now().isoformat(timespec="seconds"),
        "env_overrides": {k: env[k] for k in
                          (["MASHED_RE_NO_AUTO_HOOK", "MASHED_PHYS_C4_SELFTEST"]
                           + [kv.split("=", 1)[0] for kv in args.env if "=" in kv])
                          if k in env},
        "coverage_candidates": ["0x%08x" % r for r in cov],
    }

    dev = frida.get_local_device()
    pid = dev.spawn(str(exe), cwd=str(exe.parent), env=env)
    print(f"  spawned {mod_name} pid={pid}  (will kill ONLY this pid)")
    sess = dev.attach(pid)

    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("info", "err"):
            print("  [agent]", p.get("msg"))

    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    if cov:
        scr.exports_sync.armcoverage(mod_name, img_base, cov)   # arm BEFORE resume
    dev.resume(pid)
    print(f"\n  >>> PLAY NOW in the '{mod_name}' window. Recording for up to {args.seconds}s.")
    print(f"  >>> Quit the game (or wait out the window) to stop. Flushing to {out_dir.name}/ every ~1s.\n")

    last_dump = {"events": [], "frames": 0, "cov": []}
    try:
        deadline = time.time() + args.seconds
        while time.time() < deadline:
            if psutil and not psutil.pid_exists(pid):
                print("  game process exited — stopping."); break
            try:
                last_dump = scr.exports_sync.dump()
                write_artifacts(out_dir, last_dump, meta, hooks_idx)   # live flush (crash-robust)
            except Exception:
                pass
            print(f"\r  frames={last_dump['frames']}  input-events={len(last_dump['events'])}"
                  f"  coverage={len(last_dump['cov'])}/{len(cov)}   ", end="", flush=True)
            time.sleep(1.0)
    except KeyboardInterrupt:
        print("\n  interrupted — stopping.")
    finally:
        try:
            last_dump = scr.exports_sync.dump()
        except Exception:
            pass
        write_artifacts(out_dir, last_dump, meta, hooks_idx)
        try:
            if not psutil or psutil.pid_exists(pid): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass

    print(f"\n\n=== captured {out_dir.name} ===")
    print(f"  frames={last_dump['frames']}  input-events={len(last_dump['events'])}"
          f"  coverage={len(last_dump['cov'])}/{len(cov)}")
    hit = sorted((c["rva"] for c in last_dump["cov"]))
    miss = [r for r in cov if r not in hit]
    if cov:
        print("  coverage HIT : " + (", ".join("0x%08x" % r for r in hit) or "(none)"))
        print("  coverage MISS: " + (", ".join("0x%08x" % r for r in miss) or "(none)"))
    print(f"  artifacts -> {out_dir}")
    print("  next: write notes.md (what you navigated), then replay_session.py to verify.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
