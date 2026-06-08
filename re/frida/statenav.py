# State-aware menu navigation: instead of fixed-dwell taps (which desync past ~3 screens),
# poll the menu-stack depth (DAT_0067e9f8) / phase (DAT_0067eca4) and issue each input only
# when the expected screen is reached, waiting for the transition before the next input. This
# is the reliable deep-nav prerequisite for Part 2 (reach Player Colour Select, then race setup).
#
# Input is the in-process FUN_00497310 return override (4=confirm,11=up,12=down,...) — OS-input
# free. After reaching Player Colour Select it PROBES each control to find the player-JOIN
# (the control that advances past colour-select), since blind confirm does not.
#
# Usage: py -3.12 re/frida/statenav.py [--seconds N] [--shot-dir verify/p2]
import ctypes, json, os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

def _find_hwnd(pid):
    u=ctypes.windll.user32; found=[]
    proto=ctypes.WINFUNCTYPE(wintypes.BOOL,wintypes.HWND,wintypes.LPARAM)
    def cb(h,_):
        p=wintypes.DWORD(); u.GetWindowThreadProcessId(h,ctypes.byref(p))
        if p.value==pid and u.IsWindowVisible(h):
            n=u.GetWindowTextLengthW(h)
            if n:
                b=ctypes.create_unicode_buffer(n+1); u.GetWindowTextW(h,b,n+1)
                if "MASHED" in b.value.upper(): found.append(h)
        return True
    u.EnumWindows(proto(cb),0); return found[0] if found else None
def shoot(pid, path):
    try:
        from PIL import Image
        h=_find_hwnd(pid)
        if not h: return False
        u=ctypes.windll.user32; g=ctypes.windll.gdi32
        r=wintypes.RECT(); u.GetClientRect(h,ctypes.byref(r)); w,ht=r.right,r.bottom
        if w<=0 or ht<=0: return False
        hdc=u.GetDC(h); md=g.CreateCompatibleDC(hdc); bm=g.CreateCompatibleBitmap(hdc,w,ht); g.SelectObject(md,bm)
        u.PrintWindow(h,md,2)
        class BH(ctypes.Structure):
            _fields_=[("biSize",wintypes.DWORD),("biWidth",wintypes.LONG),("biHeight",wintypes.LONG),
                      ("biPlanes",wintypes.WORD),("biBitCount",wintypes.WORD),("biCompression",wintypes.DWORD),
                      ("biSizeImage",wintypes.DWORD),("biXPelsPerMeter",wintypes.LONG),("biYPelsPerMeter",wintypes.LONG),
                      ("biClrUsed",wintypes.DWORD),("biClrImportant",wintypes.DWORD)]
        bi=BH(); bi.biSize=ctypes.sizeof(BH); bi.biWidth=w; bi.biHeight=-ht; bi.biPlanes=1; bi.biBitCount=32
        buf=(ctypes.c_char*(w*ht*4))(); g.GetDIBits(md,bm,0,ht,buf,ctypes.byref(bi),0)
        Image.frombuffer("RGBA",(w,ht),bytes(buf),"raw","BGRA",0,1).convert("RGB").save(str(path))
        g.DeleteObject(bm); g.DeleteDC(md); u.ReleaseDC(h,hdc); print(f"  [shot] {path}"); return True
    except Exception as e: print("  [shot] err",e); return False

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4, RVA_SEL=0x0067ed40;
let pressCtrl=-1, pressUntil=0; const CNT={};
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }});
    return DELTA; },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  clear:function(){ pressCtrl=-1; return 1; },
  countthese:function(rvas){ rvas.forEach(function(r){ const a=parseInt(r,16); CNT[r]=0;
    try{ Interceptor.attach(abs(a),{onEnter:function(){CNT[r]++;}}); }catch(e){ CNT[r]=-1; } }); return 1; },
  counts:function(){ return CNT; },
  depth:function(){ try{return abs(RVA_DEPTH).readS32();}catch(e){return -999;} },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  sel:function(){ try{const d=abs(RVA_DEPTH).readS32(); return abs(RVA_SEL+d*0x40).readS32();}catch(e){return -999;} }
};
send({kind:'ready'});
'''

class Nav:
    def __init__(s, scr, pid): s.scr=scr; s.pid=pid
    def depth(s): return s.scr.exports_sync.depth()
    def phase(s): return s.scr.exports_sync.phase()
    def alive(s): return (not psutil) or psutil.pid_exists(s.pid)
    def press(s, c, ms=180): s.scr.exports_sync.press(c, ms); time.sleep(ms/1000.0+0.25)
    def wait(s, pred, timeout=8.0, what=""):
        end=time.time()+timeout
        while time.time()<end:
            if not s.alive(): print("   process exited"); return False
            if pred(): return True
            time.sleep(0.1)
        print(f"   wait timeout: {what} (depth={s.depth()} phase={s.phase()})"); return False
    def confirm_to_depth(s, target, tries=6):
        for _ in range(tries):
            if s.depth()>=target: return True
            s.press(4)
            if s.wait(lambda: s.depth()>=target, 2.0): return True
        return s.depth()>=target

def main():
    seconds=int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 60
    shotdir=sys.argv[sys.argv.index("--shot-dir")+1] if "--shot-dir" in sys.argv else "verify/p2"
    Path(ROOT/shotdir).mkdir(parents=True, exist_ok=True)
    env=dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"]="1"   # stock; we only drive input
    dev=frida.get_local_device(); pid=dev.spawn(str(EXE),cwd=str(ORIG),env=env); sess=dev.attach(pid)
    scr=sess.create_script(AGENT); scr.on("message",lambda m,d:None); scr.load()
    scr.exports_sync.init()
    # representative gameplay/results-gated HOLD hooks (non-hot: init/event/results, not per-frame)
    GAMEPLAY = ["0x0046c5c0","0x00492340","0x00423b40","0x00408ad0","0x00429a80",
                "0x0045ba00","0x00424920","0x00436810","0x0040e470","0x00431d80"]
    scr.exports_sync.countthese(GAMEPLAY)
    dev.resume(pid)
    nav=Nav(scr,pid)
    print("  booting...")
    nav.wait(lambda: nav.phase()==3 and nav.depth()>=1, 18.0, "title up")
    print(f"  title: depth={nav.depth()} phase={nav.phase()}")
    # confirm title -> GTS (depth 2), then dismiss the Load-Successful modal (extra confirm)
    nav.confirm_to_depth(2); time.sleep(0.3); nav.press(4); time.sleep(0.5)
    print(f"  after GTS+modal: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    shoot(pid, ROOT/shotdir/"sn_gts.png")
    # GTS cursor on Single Player(0). confirm -> Single Player mode-select (depth 3)
    nav.confirm_to_depth(3)
    print(f"  single player: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    # mode-select: down,down to Time Trial(2), confirm -> colour-select (depth 4)
    nav.press(12); nav.press(12)
    print(f"  mode sel after down,down: sel={scr.exports_sync.sel()}")
    nav.confirm_to_depth(4, tries=4)
    print(f"  colour-select: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    shoot(pid, ROOT/shotdir/"sn_colour.png")
    nav.confirm_to_depth(5, tries=4)   # colour -> track select
    print(f"  track-select: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    shoot(pid, ROOT/shotdir/"sn_track.png")
    # DESCEND toward the race: confirm, wait for depth-increase OR phase-change (race leaves the
    # frontend), screenshot+log each new state. Stop when stuck or phase leaves 3 (in race).
    last_d=nav.depth(); last_ph=nav.phase()
    for step in range(8):
        if not nav.alive(): print("   exited during descent"); break
        nav.press(4)
        time.sleep(1.2)
        d=nav.depth(); ph=nav.phase()
        print(f"  descend step {step}: depth={d} phase={ph} sel={scr.exports_sync.sel()}")
        shoot(pid, ROOT/shotdir/f"sn_descend{step}.png")
        if ph!=3 and ph!=last_d:   # phase left menu-active -> likely in race/transition
            print(f"  >>> phase changed to {ph} (likely race/transition) at step {step}")
        if d==last_d and ph==last_ph and step>=2:
            print(f"  >>> stuck at depth={d} phase={ph} (needs different input here)")
        last_d=d; last_ph=ph
    print(f"  FINAL: depth={nav.depth()} phase={nav.phase()}")
    shoot(pid, ROOT/shotdir/"sn_final.png")
    try:
        counts = scr.exports_sync.counts()
        print("  === gameplay-gated hook counts (in-race exercise) ===")
        for r,c in counts.items(): print(f"     {r} : {c}")
        exercised=[r for r,c in counts.items() if isinstance(c,int) and c>0]
        print(f"  EXERCISED in-race: {len(exercised)}/{len(counts)} -> {exercised}")
    except Exception as e: print("  counts err", e)
    try:
        if nav.alive(): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__=="__main__":
    sys.exit(main())
