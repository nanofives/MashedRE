# Disambiguate crash #2: poll the D3D9 device global [0x7d4110], hook its
# SetRenderState (vtable +0xe4), and track in/out depth. If the AV fires with
# inSRS>0 the fault is INSIDE the device method (runtime/driver); if SetRenderState
# entered+returned and the AV is after, it's a game-side stack imbalance.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_srs.txt'

AGENT = r'''
'use strict';
function hx(v){ return '0x'+(v>>>0).toString(16); }
var inSRS=0, srsCount=0, hooked=false, polls=0;
var iv = setInterval(function(){
  if (hooked){ clearInterval(iv); return; }
  if (++polls > 2000){ clearInterval(iv); return; }
  var dev;
  try { dev = ptr(0x7d4110).readPointer(); } catch(e){ return; }
  if (dev.isNull()) return;
  try {
    var vt = dev.readPointer();
    var srs = vt.add(0xe4).readPointer();
    if (srs.isNull()) return;
    Interceptor.attach(srs, {
      onEnter: function(a){
        inSRS++; srsCount++;
        if (srsCount<=16) send({kind:'srs', n:srsCount, state:hx(a[1].toUInt32()), value:hx(a[2].toUInt32())});
      },
      onLeave: function(){ if(inSRS>0) inSRS--; }
    });
    hooked=true; clearInterval(iv);
    send({kind:'note', t:'hooked SetRenderState @ '+srs+' (device '+dev+', after '+polls+' polls)'});
  } catch(e){ }
}, 5);

var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', type:d.type, eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        inSetRenderState:inSRS, srsCount:srsCount, hooked:hooked,
        ecx:c.ecx.toString(), ebx:c.ebx.toString()});
  return false;
});
send({kind:'ready'});
'''

def find_pid():
    for p in psutil.process_iter(['name','pid']):
        try:
            if p.info['name'] and p.info['name'].lower()=='mashed.exe': return p.info['pid']
        except Exception: pass
    return None

def main():
    print("waiting for MASHED.exe — launch now")
    dl=time.time()+120; pid=None
    while time.time()<dl:
        pid=find_pid()
        if pid: print("  pid=%d"%pid); break
        time.sleep(0.02)
    if not pid: sys.exit("timeout")
    sess=frida.get_local_device().attach(pid); print("  attached")
    ev=[]
    def on(m,_):
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='note': print("  NOTE:",p['t']); ev.append(p)
        elif k=='ready': print("  watching (polling for device)")
        elif k=='srs': print("  SetRenderState #%d state=%s value=%s"%(p['n'],p['state'],p['value'])); ev.append(p)
        elif k=='crash':
            print(f"  === CRASH {p['type']} eip={p['eip']} mem={p['mem']} ===")
            print(f"   hooked={p['hooked']} inSetRenderState={p['inSetRenderState']} srsCount={p['srsCount']} ecx={p['ecx']} ebx={p['ebx']}")
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.3); break
        try:
            if not psutil.pid_exists(pid): print("  gone"); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
