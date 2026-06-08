# Canonical C3->C4 verifier for NAVIGATE scenarios (rubric-clean: installed + exercised +
# survived), driving the menu IN-PROCESS (no OS input) so the candidate hook is exercised on
# the real screen it lives on (Game Type Select / Options / submenus), not just boot-to-menu.
#
# Same evidence model as canonical_c4_verify.py, but the canonical scenario is
# "boot -> navigate to screen X" instead of "boot to menu". Navigation is driven by
# overriding the per-control resolver FUN_00497310(player@esp+4, control@esp+8) return to
# 0xff for a scripted control sequence -- fully in-process (Frida return override), ZERO OS
# input. Control map (see re/analysis/frontend_input_nav_trace/TRACE.md):
#     4 = confirm/select   11 = up   12 = down   9 = left   10 = right
# Typical nav: title needs 1 confirm; the "Load Successful" modal needs 1 more; then up/down.
#   e.g. --nav 4,4,12,12   = confirm,confirm,down,down  (-> Options on Game Type Select)
#        --nav 4,4,12,12,4 = ...then confirm (enter Options)
#
#   OFF run  - .asi loaded but MASHED_HOOK_ONLY="__none__" (installs nothing). Interceptor
#              counts each ORIGINAL candidate RVA WHILE the nav script runs. Proves the fn is
#              on the navigated canonical path (exercised>0). Cold/boot-or-menu fns only --
#              do NOT feed hot >1000/s fns (Interceptor destabilizes; see CLAUDE.md).
#   ON run   - MASHED_HOOK_ONLY=<candidates>; same nav; confirm each inline-JMP is live
#              (first byte 0xE9) and the game survives the navigated scenario with OUR code.
#
# C4 verdict per candidate = exercised(OFF calls>0 on the navigated scenario) AND
# installed(0xE9) AND survived. Optional --shot PNG of the ON run for visual confirmation the
# nav reached the intended screen. Pair with scripts/patch_mashed_no_focus_pause.py so the run
# is robust when the window is not focused.
#
# Usage:
#   py -3.12 re/frida/canonical_c4_navigate.py 0xRVA[,0xRVA...] --nav 4,4,12,12 \
#       [--settle 7000] [--dwell 1500] [--seconds 26] [--shot verify/x.png] [--shotat 18]
import ctypes, json, os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"
EXE  = ORIG / "MASHED.exe"
LOG  = ROOT / "log"


def _find_hwnd(target_pid):
    user32 = ctypes.windll.user32; found = []
    proto = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    def cb(hwnd, _):
        pid = wintypes.DWORD(); user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        if pid.value == target_pid and user32.IsWindowVisible(hwnd):
            n = user32.GetWindowTextLengthW(hwnd)
            if n:
                buf = ctypes.create_unicode_buffer(n+1); user32.GetWindowTextW(hwnd, buf, n+1)
                if "MASHED" in buf.value.upper(): found.append(hwnd)
        return True
    user32.EnumWindows(proto(cb), 0)
    return found[0] if found else None

def shoot(pid, path):
    """PrintWindow capture (works occluded; no focus change)."""
    try:
        from PIL import Image
        hwnd = _find_hwnd(pid)
        if not hwnd: print("  [shot] no hwnd"); return False
        user32 = ctypes.windll.user32; gdi32 = ctypes.windll.gdi32
        r = wintypes.RECT(); user32.GetClientRect(hwnd, ctypes.byref(r))
        w, h = r.right - r.left, r.bottom - r.top
        if w <= 0 or h <= 0: return False
        hdc = user32.GetDC(hwnd); memdc = gdi32.CreateCompatibleDC(hdc)
        bmp = gdi32.CreateCompatibleBitmap(hdc, w, h); gdi32.SelectObject(memdc, bmp)
        user32.PrintWindow(hwnd, memdc, 0x00000002)
        class BH(ctypes.Structure):
            _fields_ = [("biSize", wintypes.DWORD), ("biWidth", wintypes.LONG),
                        ("biHeight", wintypes.LONG), ("biPlanes", wintypes.WORD),
                        ("biBitCount", wintypes.WORD), ("biCompression", wintypes.DWORD),
                        ("biSizeImage", wintypes.DWORD), ("biXPelsPerMeter", wintypes.LONG),
                        ("biYPelsPerMeter", wintypes.LONG), ("biClrUsed", wintypes.DWORD),
                        ("biClrImportant", wintypes.DWORD)]
        bi = BH(); bi.biSize = ctypes.sizeof(BH); bi.biWidth = w; bi.biHeight = -h
        bi.biPlanes = 1; bi.biBitCount = 32; bi.biCompression = 0
        buf = (ctypes.c_char * (w * h * 4))()
        gdi32.GetDIBits(memdc, bmp, 0, h, buf, ctypes.byref(bi), 0)
        Image.frombuffer("RGBA", (w, h), bytes(buf), "raw", "BGRA", 0, 1).convert("RGB").save(str(path))
        gdi32.DeleteObject(bmp); gdi32.DeleteDC(memdc); user32.ReleaseDC(hwnd, hdc)
        print(f"  [shot] saved {path} ({w}x{h})"); return True
    except Exception as e:
        print(f"  [shot] err {e}"); return False


# ---- agent: crash handler + candidate counters + in-process nav driver (FUN_00497310) ----
AGENT = r'''
'use strict';
const IMGBASE = 0x00400000;
let DELTA = 0;
const RVA_RESOLVER = 0x00497310;          // FUN_00497310(player@esp+4, control@esp+8)
const COUNT = {};
let NAV = [], SETTLE = 7000, DWELL = 1500, t0 = Date.now(), navStarted = 0, lastSlot = -99;

function abs(rva){ return ptr(rva + DELTA); }
function modtab(){ const o=[]; Process.enumerateModules().forEach(m=>o.push(
  {name:m.name,base:m.base.toString(),end:m.base.add(m.size).toString()})); return o; }
const FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  const c=d.context; let bAt='';
  try{const e=ptr(d.address.toString()); for(let i=0;i<16;i++){try{bAt+=e.add(i).readU8().toString(16).padStart(2,'0')+' ';}catch(x){bAt+='?? ';}}}catch(x){}
  let st=[]; try{const sp=ptr(c.esp.toString()); for(let i=0;i<320;i++){try{st.push({off:i*4,val:sp.add(i*4).readU32().toString(16)});}catch(x){break;}}}catch(x){}
  send({kind:'crash',type:d.type,address:d.address.toString(),
    eax:c.eax.toString(),ebx:c.ebx.toString(),ecx:c.ecx.toString(),edx:c.edx.toString(),
    esi:c.esi.toString(),edi:c.edi.toString(),ebp:c.ebp.toString(),esp:c.esp.toString(),eip:c.pc.toString(),
    bytes_at_eip:bAt.trim(),
    mem_address:(d.memory&&d.memory.address)?d.memory.address.toString():'?',
    mem_op:(d.memory&&d.memory.operation)?d.memory.operation:'?', stack:st, modules:modtab()});
  return false;
});

// current scripted control to force right now (one clean press per dwell-slot after settle)
function navControl(){
  const tt = Date.now() - t0 - SETTLE;
  if (tt < 0 || !NAV.length) return -1;
  const k = Math.floor(tt / DWELL);
  if (k >= NAV.length) return -1;
  return NAV[k];                              // dwell-slot index k -> control id
}

rpc.exports = {
  setbase:function(b){ DELTA = b - IMGBASE; return DELTA; },
  setnav:function(list, settle, dwell){ NAV = list||[]; SETTLE = settle; DWELL = dwell; t0 = Date.now();
    // arm the nav driver: override FUN_00497310 return for the scripted control
    try { Interceptor.attach(abs(RVA_RESOLVER), {
      onEnter(a){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
      onLeave(ret){
        const k = navControl();
        if (k>=0 && this.p===0 && this.c===k){
          const slot = Math.floor((Date.now()-t0-SETTLE)/DWELL);   // one press per slot
          if (slot===lastSlot) return;
          lastSlot = slot; ret.replace(ptr(0xff)); navStarted++;
        }
      }
    }); send({kind:'info', msg:'nav driver armed on FUN_00497310'}); }
    catch(e){ send({kind:'err', msg:'navarm '+e}); }
    return 1; },
  navcount:function(){ return navStarted; },
  firstbyte:function(rva){ try{ return ptr(rva).readU8(); }catch(e){ return -1; } },
  counts:function(){ return COUNT; },
  countthese:function(rvas){
    rvas.forEach(function(r){ COUNT[r]=0;
      try{ Interceptor.attach(ptr(r),{onEnter:function(){COUNT[r]++;}}); }catch(e){ COUNT[r]=-1; } });
    return Object.keys(COUNT).length; }
};
send({kind:'ready'});
'''


def run(candidates_csv, rvas, count_rvas, nav, settle, dwell, seconds, shot=None, shotat=None):
    env = dict(os.environ)
    env["MASHED_RE_DEV"]    = "1"
    env["MASHED_HOOK_ONLY"] = candidates_csv
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    st = {"crash": None}
    def on_msg(m, d):
        if m.get("type") == "error": return
        p = m.get("payload", {})
        if p.get("kind") == "crash": st["crash"] = p
        elif p.get("kind") in ("info", "err"): print("   ", p.get("msg"))
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    # resolve base from the agent side via a known export: read MASHED.exe base
    # (image base 0x400000 in practice; compute via firstbyte on a known addr is unreliable,
    #  so pass base = 0x400000 unless relocated -- agent will use it for nav RVA math)
    scr.exports_sync.setbase(0x00400000)
    if count_rvas:
        scr.exports_sync.countthese(count_rvas)          # attach before resume
    scr.exports_sync.setnav(nav, settle, dwell)          # arm nav driver before resume
    dev.resume(pid)
    start = time.time(); deadline = start + seconds; shot_done = False
    while time.time() < deadline:
        if st["crash"]: break
        if psutil and not psutil.pid_exists(pid): break
        if shot and not shot_done and shotat is not None and (time.time()-start) >= shotat:
            shoot(pid, ROOT / shot); shot_done = True
        time.sleep(0.25)
    if shot and not shot_done and not st["crash"]:
        shoot(pid, ROOT / shot)
    alive = psutil.pid_exists(pid) if psutil else None
    jmp, counts, navc = {}, {}, 0
    if not st["crash"]:
        for r in rvas:
            try: jmp[r] = scr.exports_sync.firstbyte(int(r, 16))
            except Exception as e: jmp[r] = str(e)
        if count_rvas:
            try: counts = scr.exports_sync.counts()
            except Exception: pass
        try: navc = scr.exports_sync.navcount()
        except Exception: pass
    try:
        if alive: dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return {"crash": st["crash"], "alive": alive, "jmp": jmp, "counts": counts, "navc": navc}


def main():
    if len(sys.argv) < 2 or sys.argv[1].startswith("--"):
        sys.exit("usage: canonical_c4_navigate.py 0xRVA[,0xRVA...] --nav 4,4,12,12 "
                 "[--settle 7000] [--dwell 1500] [--seconds 26] [--shot verify/x.png] [--shotat 18]")
    rvas = [c.strip() for c in sys.argv[1].split(",") if c.strip()]
    nav  = [int(x) for x in sys.argv[sys.argv.index("--nav")+1].split(",")] if "--nav" in sys.argv else []
    settle  = int(sys.argv[sys.argv.index("--settle")+1])  if "--settle"  in sys.argv else 7000
    dwell   = int(sys.argv[sys.argv.index("--dwell")+1])   if "--dwell"   in sys.argv else 1500
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 26
    shot    = sys.argv[sys.argv.index("--shot")+1]   if "--shot"   in sys.argv else None
    shotat  = float(sys.argv[sys.argv.index("--shotat")+1]) if "--shotat" in sys.argv else None
    LOG.mkdir(parents=True, exist_ok=True)
    print(f"  candidates={rvas} nav={nav} settle={settle} dwell={dwell} seconds={seconds}")

    print(f"\n=== OFF run (navigate; count candidate calls; installs nothing) ===")
    off = run("__none__", [], rvas, nav, settle, dwell, seconds)
    if off["crash"]:
        print(f"  OFF run CRASHED (baseline): eip={off['crash']['eip']}")
    print(f"  OFF alive={off['alive']} navTaps={off['navc']} counts={off['counts']}")

    print(f"\n=== ON run (install ONLY candidates; navigate; confirm JMP-live + survive) ===")
    on = run(",".join(rvas), rvas, None, nav, settle, dwell, seconds, shot, shotat)
    if on["crash"]:
        c = on["crash"]
        print(f"  ON run CRASH: {c['type']} eip={c['eip']} mem={c.get('mem_address')}")
        (LOG/"crash_eip.txt").write_text(json.dumps(c, indent=2), encoding="utf-8")
    print(f"  ON alive={on['alive']} navTaps={on['navc']}")

    print(f"\n=== C4 VERDICT (exercised-on-navigated-scenario AND installed AND survived) ===")
    verdicts = {}
    for r in rvas:
        rl = r.lower()
        calls = off["counts"].get(rl, off["counts"].get(r, 0))
        try: calls = int(calls)
        except Exception: calls = 0
        installed = (on["jmp"].get(r) == 0xE9)
        survived  = (on["crash"] is None) and bool(on["alive"])
        ok = (calls > 0) and installed and survived
        verdicts[r] = {"calls": calls, "installed": installed, "survived": survived, "C4": ok}
        print(f"  {r}: calls={calls} installed={installed} survived={survived} -> {'C4-READY' if ok else 'HOLD'}")
    print(f"  NOTE: 'C4-READY' = exercised+installed+survived on the navigated scenario. This is")
    print(f"        sufficient C4 ONLY for a pure leaf whose C3 force-call diff already covered the")
    print(f"        input domain. Non-leaf dispatchers additionally need behavioral-diff coverage;")
    print(f"        promote via the re-classify skill (it gates leaf vs non-leaf), not this label.")
    verdicts["_nav"] = {"nav": nav, "off_navtaps": off["navc"], "on_navtaps": on["navc"]}
    (LOG/"c4_navigate_result.json").write_text(json.dumps(verdicts, indent=2), encoding="utf-8")
    print(f"\n  -> log/c4_navigate_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
