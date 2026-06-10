# Hook ALL direct callees of FUN_00402750 (AppInitialiseOnBootup) to localize the
# baseline eip=0 crash. Deepest entered-but-not-returned function = the culprit.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_appinit_callees.txt'

# Every direct callee of FUN_00402750 (logger 0x4963e0 omitted — noisy, leaf).
CALLEES = [0x402750,0x4d8560,0x4b6540,0x4b6560,0x496e40,0x4caea0,0x4c2f00,0x4c2ed0,
           0x498bc0,0x498bd0,0x495350,0x494c80,0x495280,0x4283a0,0x427ca0,0x4671a0,
           0x4275d0,0x4274d0,0x4274e0,0x4952f0,0x4113b0,0x558240,0x4841d0,0x484170,
           0x4669b0,0x40bbb0,0x4723d0,0x45b350,0x40bb50,0x40bb30,0x4881d0,0x403640,
           0x47ba00,0x499ce0]

AGENT = r'''
'use strict';
var F = %s;
var stack = [];
F.forEach(function(va){
  var name = '0x'+va.toString(16);
  try {
    Interceptor.attach(ptr(va), {
      onEnter: function(){ stack.push({n:name, ra:this.returnAddress.toString()}); },
      onLeave: function(){ for (var i=stack.length-1;i>=0;i--){ if(stack[i].n===name){ stack.splice(i,1); break; } } }
    });
  } catch(e){ send({kind:'hookerr', name:name, err:e.message}); }
});
send({kind:'hooked', n:F.length});
var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        eax:c.eax.toString(), ecx:c.ecx.toString(), ebx:c.ebx.toString(),
        edx:c.edx.toString(), esi:c.esi.toString(), edi:c.edi.toString(),
        stack: stack.slice()});
  return false;
});
send({kind:'ready'});
''' % json.dumps(CALLEES)

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
    ev=[]
    def on(m,_):
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='hooked': print("  hooked %d funcs"%p['n'])
        elif k=='hookerr': print("  HOOK ERR %s: %s"%(p['name'],p['err']))
        elif k=='ready': print("  watching")
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} ecx={p['ecx']} ebx={p['ebx']} edx={p['edx']} ===")
            print("  shadow call stack (outer->inner):")
            if not p['stack']: print("    <empty — crash above all hooked callees>")
            for f in p['stack']: print(f"    {f['n']}  (from {f['ra']})")
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
