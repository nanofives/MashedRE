# Unblock test: the font TXD load fails because VfsFileExists("fgdc20.txd") returns 0
# (file reported absent from the VFS default mount), even though the open path resolves
# font36.piz. Force VfsFileExists -> 1 for the BARE "fgdc20.txd" query (FGDC20.TXD is at
# the piz root, not under pc/) and see if FUN_004b3d80 then loads the TXD via the working
# open path and MASHED survives past the ~94s title-render crash.
#
# Run with hooks DISABLED (MASHED_RE_NO_AUTO_HOOK=1) so 0x00550b00 is the ORIGINAL
# (no inline JMP) and Frida can Interceptor it cleanly. The crash is stock, so this is a
# valid unblock test. If it survives -> a dev workaround is viable; then implement in .asi
# with hooks enabled to test the ports in-game.
#
# Launch: $env:MASHED_RE_NO_AUTO_HOOK='1'; Start-Process ...\MASHED.exe
import json, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    sys.exit("psutil required")

ROOT     = Path(__file__).resolve().parent.parent.parent
OUT_FILE = ROOT / 'log' / 'force_fgdc_exists.txt'

AGENT_JS = r'''
'use strict';
function rd(p){ try { return p.readCString(); } catch(e){ return ''; } }
var forced = 0, seen = 0;
Interceptor.attach(ptr('0x00550b00'), {   // VfsFileExists(name)
    onEnter: function(){ try { this.nm = rd(this.context.esp.add(4).readPointer()); } catch(e){ this.nm=''; } },
    onLeave: function(ret){
        var n = (this.nm||'').toLowerCase();
        if (n.indexOf('fgdc20') >= 0) {
            seen++;
            if (n === 'fgdc20.txd' && ret.toInt32() === 0) {
                ret.replace(ptr(1));
                forced++;
                send({ kind:'forced', name:this.nm });
            } else {
                send({ kind:'seen', name:this.nm, ret:ret.toString() });
            }
        }
    }
});
const FATAL = { 'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1 };
Process.setExceptionHandler(function(d){
    if (!FATAL[d.type]) return false;
    send({ kind:'av', pc:d.context.pc.toString(), esi:d.context.esi.toString(),
           mem:(d.memory&&d.memory.address)?d.memory.address.toString():'?' });
    return false;
});
send({ kind:'ready' });
'''

def find_pid():
    for p in psutil.process_iter(['name','pid']):
        try:
            if p.info['name'] and p.info['name'].lower()=='mashed.exe': return p.info['pid']
        except Exception: continue
    return None

def main():
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    print("waiting for MASHED.exe — launch it now (with MASHED_RE_NO_AUTO_HOOK=1)")
    dl=time.time()+120; pid=None
    while time.time()<dl:
        pid=find_pid()
        if pid: print(f"  found pid={pid}"); break
        time.sleep(0.1)
    if not pid: sys.exit("timeout")
    t0=time.time()
    time.sleep(0.05)
    session=frida.get_local_device().attach(pid); print("  attached")
    events=[]; crashed=[False]
    def on_message(m,d):
        if m['type']=='error': print("  agent error:", m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='ready': print("  armed; will force VfsFileExists('fgdc20.txd')->1")
        elif k=='forced': print(f"  [{time.time()-t0:5.1f}s] FORCED exists=1 for {p['name']!r}"); events.append(p)
        elif k=='seen': print(f"  [{time.time()-t0:5.1f}s] VfsFileExists({p['name']!r}) -> {p['ret']}"); events.append(p)
        elif k=='av': print(f"  [{time.time()-t0:5.1f}s] AV pc={p['pc']} esi={p['esi']} mem={p['mem']}"); events.append(p); crashed[0]=True
    script=session.create_script(AGENT_JS); script.on('message', on_message); script.load()
    dl=time.time()+135
    last_alive=0.0
    while time.time()<dl:
        try:
            if psutil.Process(pid).name().lower()!='mashed.exe':
                print(f"  [{time.time()-t0:5.1f}s] process gone"); break
            last_alive=time.time()-t0
        except Exception:
            print(f"  [{time.time()-t0:5.1f}s] process gone"); break
        time.sleep(0.5)
    else:
        last_alive=time.time()-t0
        print(f"  [{last_alive:5.1f}s] survived to timeout (still alive)")
    try: session.detach()
    except Exception: pass
    summary={'last_alive_s':round(last_alive,1), 'crashed':crashed[0], 'forced_count':sum(1 for e in events if e.get('kind')=='forced')}
    events.append({'kind':'summary', **summary})
    OUT_FILE.write_text(json.dumps(events, indent=2), encoding='utf-8')
    print(f"\nSUMMARY: last_alive={summary['last_alive_s']}s crashed={summary['crashed']} forced={summary['forced_count']}")
    print(f"wrote {len(events)} event(s) -> {OUT_FILE}")
    return 0

if __name__=='__main__':
    sys.exit(main())
