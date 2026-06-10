# Checkpoint FUN_00402750 to pin the eip=0 site. Hooks the two vtable-call return
# points + a few later checkpoints; at 0x4027f9 / 0x40280b reads [reg+0xe4] (the
# method pointer about to be called). The last checkpoint reached before the AV
# localizes the crash.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_appinit_checkpoints.txt'

# label -> VA
CP = {
  'enter_402750'      : 0x402750,
  'before_call_ecx_e4': 0x4027f9,  # read ecx,[ecx+0xe4]
  'after_call_ecx_e4' : 0x4027ff,
  'before_call_edx_e4': 0x40280b,  # read edx,[edx+0xe4]
  'after_call_edx_e4' : 0x402811,
  'after_4c2f00'      : 0x402816,
  'before_audio_4669b0':0x4028db,
  'near_end_40294c'   : 0x40294c,
}

AGENT = r'''
'use strict';
var CP = %s;
function hx(v){ return '0x'+(v>>>0).toString(16); }
Object.keys(CP).forEach(function(name){
  var va = ptr(CP[name]);
  try {
    Interceptor.attach(va, { onEnter: function(){
      var extra = '';
      if (name === 'before_call_ecx_e4') {
        var ecx=this.context.ecx; var m=-1; try{m=ecx.add(0xe4).readU32();}catch(e){}
        extra = ' ecx='+ecx+' [ecx+0xe4]='+hx(m);
      } else if (name === 'before_call_edx_e4') {
        var edx=this.context.edx; var m=-1; try{m=edx.add(0xe4).readU32();}catch(e){}
        extra = ' edx='+edx+' [edx+0xe4]='+hx(m);
      }
      send({kind:'cp', name:name, extra:extra});
    }});
  } catch(e){ send({kind:'hookerr', name:name, err:e.message}); }
});
send({kind:'hooked'});
var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', eip:c.pc.toString(), mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        eax:c.eax.toString(), ecx:c.ecx.toString(), ebx:c.ebx.toString(), edx:c.edx.toString()});
  return false;
});
send({kind:'ready'});
''' % json.dumps(CP)

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
        time.sleep(0.05)
    if not pid: sys.exit("timeout")
    sess=frida.get_local_device().attach(pid); print("  attached")
    ev=[]; seq=[]
    def on(m,_):
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='hooked': print("  hooked checkpoints")
        elif k=='hookerr': print("  HOOK ERR %s: %s"%(p['name'],p['err']))
        elif k=='ready': print("  watching")
        elif k=='cp':
            seq.append(p['name']); print(f"  CP {p['name']}{p['extra']}"); ev.append(p)
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} ecx={p['ecx']} ebx={p['ebx']} edx={p['edx']} ===")
            print("  last checkpoint reached:", seq[-1] if seq else '<none>')
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.3); break
        try:
            if not psutil.pid_exists(pid): print("  gone; last cp:", seq[-1] if seq else '<none>'); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
