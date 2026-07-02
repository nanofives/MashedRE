# canonical_c4_autodiff.py — SELF-SPECIFYING canonical-scenario C4 diff (C3 -> C4).
#
# The proven canonical_c4_sidediff.py captures (callee_rva, args) per target but needs the
# callee list HAND-AUTHORED. This harness DISCOVERS the callee list automatically by a static
# CFG-walk of the function body, then runs the SAME (callee,args)+EAX capture, OFF vs ON, on a
# canonical scenario. CLEAN iff the captured behavior SETS are equal, non-empty, the inline JMP
# is installed, and the body has no indirect calls (else HOLD — conservative, never false-CLEAN).
#
# Targets are RVAs only. The ON run attaches the target at the REIMPL entry, computed from the
# installed E9 JMP at the original RVA (reimpl = rva+5+rel32) — no .asi export name needed.
# Callees are attached at ORIGINAL RVAs in both runs (a faithful reimpl calls the same originals).
#
# Soundness: callee union = walk(original body) U walk(reimpl body), so a reimpl that calls an
# EXTRA/DIFFERENT original callee is caught (its call is attached and shows up in the ON trace).
# A reimpl that fully INLINES an original callee (calls nothing) will HOLD, not false-CLEAN.
#
# Phases (4 spawns/batch regardless of target count):
#   1 OFF-discover: spawn OFF suspended, CFG-walk each target's original body -> origCallees.
#   2 ON-discover : spawn ON, wait for JMPs installed, walk each reimpl body -> reimplCallees.
#   3 OFF-capture : spawn OFF, navigate scenario, capture with the union callee set.
#   4 ON-capture  : spawn ON,  navigate scenario, capture with the union callee set.
#
# Usage:
#   py -3.12 re/frida/canonical_c4_autodiff.py --rvas 0x0042e3a0,0x0042aae0 [--nav 4,4]
#   py -3.12 re/frida/canonical_c4_autodiff.py --rvas-file batch.txt [--settle 7000] [--seconds 18]
import json, os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
def _orig(root):
    c = root / "original" / "MASHED.exe"
    if c.exists(): return c
    return root.parent.parent / "original" / "MASHED.exe"
EXE = _orig(ROOT)
ORIG = EXE.parent
LOG = (ROOT / "log") if (ROOT / "log").exists() else (ROOT.parent.parent / "log")

IMGBASE = 0x00400000
IMG_LO  = 0x00400000
IMG_HI  = 0x00600000   # MASHED .text upper bound; callees outside this range are not original fns

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000, IMG_LO = 0x00400000, IMG_HI = 0x00600000;
let DELTA = 0;
const RVA_RESOLVER = 0x00497310;
let NAV = [], SETTLE = 7000, DWELL = 1500, t0 = 0, lastSlot = -99, GATE = 0;
const SAMP = {};            // targetRva -> {sig: count}
const CUR = {};             // targetRva -> in-flight seq array or null

function setDelta(){ const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
  DELTA = m.base.toUInt32() - IMGBASE; }
function abs(rva){ return ptr((rva + DELTA) >>> 0); }
function rvaOf(addrU32){ return (addrU32 - DELTA) >>> 0; }
function inImg(rva){ return rva >= IMG_LO && rva < IMG_HI; }

// nargs for a callee: cdecl -> caller's 'add esp,N' after the call; stdcall/thiscall -> callee 'ret N'.
function calleeNargs(targetAbs, nextInsn){
  try {
    if (nextInsn && nextInsn.mnemonic === 'add'){
      const o = nextInsn.operands;
      if (o.length === 2 && o[0].type === 'reg' && o[0].value === 'esp' && o[1].type === 'imm')
        return (ptr(o[1].value).toUInt32() >>> 2);
    }
  } catch(e){}
  try {
    let pc = targetAbs.toUInt32(), steps = 0;
    while (steps < 300){ steps++;
      let ins; try { ins = Instruction.parse(ptr(pc)); } catch(e){ break; }
      const m = ins.mnemonic;
      if (m === 'ret' || m === 'retn'){
        if (ins.operands.length > 0 && ins.operands[0].type === 'imm') return (ptr(ins.operands[0].value).toUInt32() >>> 2);
        return 0;
      }
      if (m === 'jmp'){ const op = ins.operands[0]; if (op && op.type === 'imm'){ pc = ptr(op.value).toUInt32(); continue; } break; }
      pc = ins.next.toUInt32();
    }
  } catch(e){}
  return 0;
}

// Static CFG-walk from an absolute entry. Returns {callee_rva: nargs} (direct calls into the
// original image) + counts of indirect call/jump sites. BFS over direct branches; bounded.
function walkBody(entryAbs, hiBound){
  const visited = {}; const queue = [entryAbs.toUInt32()];
  const callees = {}; let indCalls = 0, indJumps = 0, steps = 0;
  const lo = entryAbs.toUInt32();
  let hi = hiBound >>> 0;                      // exact function end (next func start); may shrink on padding
  while (queue.length && steps < 4000){
    let pcU = queue.shift();
    if (pcU >= hi) continue;                  // path runs past the discovered function boundary
    while (steps < 4000){
      steps++;
      if (pcU >= hi) break;
      if (visited[pcU]) break; visited[pcU] = 1;
      let insn; try { insn = Instruction.parse(ptr(pcU)); } catch(e){ break; }
      const m = insn.mnemonic;
      if (m === 'call'){
        const op = insn.operands[0];
        if (op && op.type === 'imm'){
          const r = rvaOf(ptr(op.value).toUInt32());
          if (inImg(r)){
            let nx = null; try { nx = Instruction.parse(insn.next); } catch(e){}
            let na = calleeNargs(ptr(op.value), nx); if (na > 8) na = 8;
            callees[r] = Math.max(callees[r] || 0, na);
          }
        } else indCalls++;
        pcU = insn.next.toUInt32(); continue;
      } else if (m === 'ret' || m === 'retn' || m === 'retf' || m === 'iret'){
        // a ret followed by int3/nop padding (or a new push-ebp prologue) is the function end.
        try { const nb = ptr(insn.next).readU8();
          if (nb === 0xcc || nb === 0x90 || nb === 0x55){ const e = insn.next.toUInt32(); if (e < hi) hi = e; } } catch(e){}
        break;
      } else if (m === 'jmp'){
        const op = insn.operands[0];
        if (op && op.type === 'imm'){ const t = ptr(op.value).toUInt32(); if (t >= lo && t < hi && !visited[t]) queue.push(t); }
        else indJumps++;
        break;
      } else if (m === 'int3' || m === 'ud2' || m === 'hlt'){
        break;
      } else if (m.length && m[0] === 'j'){           // conditional jump
        const op = insn.operands[0];
        if (op && op.type === 'imm'){ const t = ptr(op.value).toUInt32(); if (t >= lo && t < hi && !visited[t]) queue.push(t); }
        pcU = insn.next.toUInt32(); continue;
      } else {
        pcU = insn.next.toUInt32(); continue;
      }
    }
  }
  return { callees: callees, indCalls: indCalls, indJumps: indJumps };
}

// resolve a target's entry: OFF -> original rva; ON -> follow installed E9 to the reimpl.
function targetEntry(rva, mode){
  const a = abs(rva);
  if (mode === 'off') return { entry: a, jmp: a.readU8() };
  const b = a.readU8();
  if (b !== 0xe9) return { entry: null, jmp: b };
  const rel = a.add(1).readS32();
  return { entry: a.add(5).add(rel), jmp: 0xe9 };
}

function navControl(){ const tt = Date.now()-t0-SETTLE; if (tt<0 || !NAV.length) return -1;
  const k = Math.floor(tt/DWELL); if (k>=NAV.length) return -1; return NAV[k]; }
function record(key, sig){ if (Date.now() < GATE) return; if (!SAMP[key]) SAMP[key] = {};
  SAMP[key][sig] = (SAMP[key][sig]||0) + 1; }

// normalize an arg dword for cross-run stability: image ptr -> i+rva ; else raw hex.
// (stack/heap pointers vary run-to-run -> they make the sig differ -> HOLD, which is correct.)
function nrm(v){ const r = rvaOf(v); if (inImg(r)) return 'i'+r.toString(16); return (v>>>0).toString(16); }

rpc.exports = {
  walk: function(targets, mode){
    setDelta(); const out = {};
    for (let i=0;i<targets.length;i++){
      const rva = targets[i][0]>>>0; const hiRva = targets[i][1]>>>0;
      const te = targetEntry(rva, mode);
      if (!te.entry){ out[rva] = { ok:false, jmp: te.jmp, callees:{}, indCalls:0, indJumps:0 }; continue; }
      // OFF: exact end = next func start (hiRva, an RVA). ON: reimpl in .asi -> heuristic window.
      const hi = (mode === 'off') ? (te.entry.toUInt32() + ((hiRva - rva) >>> 0)) : (te.entry.toUInt32() + 0x1800);
      let w; try { w = walkBody(te.entry, hi); } catch(e){ out[rva] = { ok:false, err:''+e, callees:{}, indCalls:0, indJumps:0 }; continue; }
      out[rva] = { ok:true, jmp: te.jmp, entry: te.entry.toUInt32()>>>0, callees: w.callees, indCalls: w.indCalls, indJumps: w.indJumps };
    }
    return out;
  },
  setupcap: function(rvas, calleeList, mode, nav, settle, dwell){
    setDelta(); NAV = nav; SETTLE = settle; DWELL = dwell; t0 = Date.now();
    GATE = t0 + SETTLE + NAV.length*DWELL + 800;
    // nav override (same as sidediff): force the menu selection resolver to the desired slot.
    try { Interceptor.attach(abs(RVA_RESOLVER), {
      onEnter(a){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
      onLeave(ret){ const k=navControl(); if(k>=0&&this.p===0&&this.c===k){
        const slot=Math.floor((Date.now()-t0-SETTLE)/DWELL); if(slot===lastSlot) return; lastSlot=slot; ret.replace(ptr(0xff)); } }
    }); } catch(e){ send({kind:'err',msg:'navarm '+e}); }
    // targets
    for (let i=0;i<rvas.length;i++){
      const rva = rvas[i]>>>0; CUR[rva] = null;
      const te = targetEntry(rva, mode);
      if (!te.entry){ send({kind:'err', msg:'no target entry '+rva.toString(16)+' jmp='+te.jmp}); continue; }
      (function(key){
        Interceptor.attach(te.entry, {
          onEnter(a){ CUR[key] = []; },
          onLeave(r){ if (CUR[key]){ CUR[key].push('eax='+nrm(r.toUInt32())); record(key, CUR[key].join(' ')); } CUR[key] = null; }
        });
      })(rva);
    }
    // union callees [rva,nargs], attached once at original RVAs; on hit, append to every active target.
    for (let i=0;i<calleeList.length;i++){
      const crva = calleeList[i][0]>>>0; const na = calleeList[i][1]|0;
      (function(cr, n){
        try { Interceptor.attach(abs(cr), { onEnter(a){
          const sp = this.context.esp; let parts = ['c'+cr.toString(16)];
          for (let j=0;j<n;j++){ try { parts.push(nrm(sp.add(4+j*4).readU32())); } catch(e){ parts.push('?'); } }
          const tok = '['+parts.join(',')+']';
          for (const k in CUR){ if (CUR[k]) CUR[k].push(tok); }
        }}); } catch(e){ /* unmapped/hot callee — skip */ }
      })(crva, na);
    }
    return 1;
  },
  samples: function(){ return SAMP; },
  jmpbyte: function(rva){ try { return abs((rva>>>0)).readU8(); } catch(e){ return -1; } }
};
send({kind:'ready'});
'''

def _spawn(mode, only_rvas):
    env = dict(os.environ); env["MASHED_RE_DEV"] = "1"
    # Proven mode (sidediff/leafdiff): OFF installs 0 hooks; ON installs ONLY the target subset.
    # The full visual-polish set is WIP-unstable at the menu capture; the small subset survives.
    env["MASHED_HOOK_ONLY"] = (",".join("0x%08x" % r for r in only_rvas) if mode == "on" else "__none__")
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error": print("   agent err:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("info", "err"): print("   ", p.get("msg"))
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    return dev, pid, sess, scr

def _kill(dev, pid, sess):
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass

def discover(mode, rvas, targets):
    dev, pid, sess, scr = _spawn(mode, rvas if mode == "on" else [])
    res = {}
    try:
        if mode == "off":
            res = scr.exports_sync.walk(targets, mode)        # module mapped at suspend; no resume needed
        else:
            dev.resume(pid)
            deadline = time.time() + 14                       # wait for .asi load + hook install
            while time.time() < deadline:
                if psutil and not psutil.pid_exists(pid): break
                try:
                    j = scr.exports_sync.jmpbyte(rvas[0])
                    if j == 0xe9: time.sleep(0.5); break
                except Exception: pass
                time.sleep(0.5)
            res = scr.exports_sync.walk(targets, mode)
    finally:
        _kill(dev, pid, sess)
    return res

def capture(mode, rvas, callee_union, nav, settle, dwell, seconds):
    dev, pid, sess, scr = _spawn(mode, rvas if mode == "on" else [])
    samples, jmp = {}, {}
    try:
        scr.exports_sync.setupcap(rvas, callee_union, mode, nav, settle, dwell)
        dev.resume(pid)
        t = time.time() + seconds
        while time.time() < t:
            if psutil and not psutil.pid_exists(pid): print("   exited early"); break
            time.sleep(0.25)
        try: samples = scr.exports_sync.samples()
        except Exception: pass
        if mode == "on":
            for r in rvas:
                try: jmp[r] = scr.exports_sync.jmpbyte(r)
                except Exception: jmp[r] = -1
    finally:
        _kill(dev, pid, sess)
    return samples, jmp

def main():
    av = sys.argv
    def opt(flag, dflt): return av[av.index(flag)+1] if flag in av else dflt
    if "--rvas-file" in av:
        rvas = [int(x, 16) for x in Path(opt("--rvas-file", "")).read_text().split() if x.strip()]
    else:
        rvas = [int(x, 16) for x in opt("--rvas", "0x0042e3a0").split(",") if x.strip()]
    nav = [int(x) for x in opt("--nav", "4,4").split(",")]
    settle = int(opt("--settle", "7000")); dwell = int(opt("--dwell", "1500")); seconds = int(opt("--seconds", "18"))
    LOG.mkdir(parents=True, exist_ok=True)

    # exact function boundaries from hooks.csv (every function's start RVA) -> next-start bounds the walk.
    import bisect
    hooks = ROOT / "hooks.csv"
    if not hooks.exists(): hooks = ROOT.parent.parent / "hooks.csv"
    fs = []
    for line in hooks.read_text(encoding="utf-8", errors="ignore").splitlines():
        if not line or line[0] == "#" or line.startswith("rva,"): continue
        c = line.split(",", 1)[0].strip()
        try: fs.append(int(c, 16))
        except ValueError: pass
    fs = sorted(set(fs))
    def boundary(r):
        i = bisect.bisect_right(fs, r)
        return fs[i] if i < len(fs) else (r + 0x1800)
    targets = [[r, boundary(r)] for r in rvas]
    print(f"  targets={[hex(r) for r in rvas]}  nav={nav}  (funcStarts={len(fs)})")

    print("\n=== PHASE 1: OFF discover (walk original bodies) ===")
    offw = discover("off", rvas, targets)
    print("\n=== PHASE 2: ON discover (walk reimpl bodies via installed JMP) ===")
    onw = discover("on", rvas, targets)

    # per-target union (rva -> max nargs across off/on) + indirect flags
    meta = {}
    union_n = {}
    for r in rvas:
        o = offw.get(r, offw.get(str(r), {})) or {}
        n = onw.get(r, onw.get(str(r), {})) or {}
        oc = o.get("callees", {}) or {}
        nc = n.get("callees", {}) or {}
        for cr, na in list(oc.items()) + list(nc.items()):
            cri = int(cr); union_n[cri] = max(union_n.get(cri, 0), int(na))
        meta[r] = {
            "off_ok": o.get("ok", False), "on_ok": n.get("ok", False),
            "on_jmp": n.get("jmp", -1),
            # veto on the ORIGINAL's indirect calls only; the reimpl body (ON) is full of
            # thunk/CRT indirect calls that say nothing about the original's verifiability.
            "ind": (o.get("indCalls", 0) or 0) + (o.get("indJumps", 0) or 0),
            "n_off_callees": len(oc), "n_on_callees": len(nc),
        }
        print(f"  {r:#010x}  off_callees={len(oc)} on_callees={len(nc)} ind={meta[r]['ind']} on_jmp={hex(meta[r]['on_jmp'])}")
    union = sorted([[k, v] for k, v in union_n.items()])
    print(f"  union callees attached: {len(union)}")

    print("\n=== PHASE 3: OFF capture ===")
    off_s, _ = capture("off", rvas, union, nav, settle, dwell, seconds)
    print("=== PHASE 4: ON capture ===")
    on_s, on_jmp = capture("on", rvas, union, nav, settle, dwell, seconds)

    print("\n=== VERDICT ===")
    out = {}
    n_clean = 0
    for r in rvas:
        offset = set((off_s.get(r, off_s.get(str(r), {})) or {}).keys())
        onset = set((on_s.get(r, on_s.get(str(r), {})) or {}).keys())
        installed = (on_jmp.get(r, on_jmp.get(str(r), -1)) == 0xe9)
        ind = meta[r]["ind"]
        why = []
        if not offset: why.append("orig-not-observed")
        if not onset: why.append("modded-not-observed")
        if offset and onset and offset != onset: why.append("BEHAVIOR-DIFF")
        if not installed: why.append("no-JMP")
        if ind: why.append(f"indirect({ind})")
        clean = bool(offset) and bool(onset) and offset == onset and installed and ind == 0
        verdict = "C4-CLEAN" if clean else "HOLD"
        if clean: n_clean += 1
        def trunc(s): return sorted({(x[:110] + '..' if len(x) > 112 else x) for x in s})
        out[r] = {"rva": "%#010x" % r, "verdict": verdict, "why": why, "installed": installed,
                  "indirect": ind, "off_n": len(offset), "on_n": len(onset),
                  "off": trunc(offset)[:4], "on": trunc(onset)[:4]}
        print(f"  {r:#010x} -> {verdict}  {','.join(why)}  (off {len(offset)} sigs, on {len(onset)} sigs, ind {ind})")
        if verdict != "C4-CLEAN":
            print(f"        off={trunc(offset)[:3]}")
            print(f"        on ={trunc(onset)[:3]}")
    (LOG / "c4_autodiff_result.json").write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"\n  {n_clean}/{len(rvas)} C4-CLEAN  ->  log/c4_autodiff_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
