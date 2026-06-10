# Watch FUN_005ab380's return-address slot for the 0x44 corruption.
#   - at entry (0x5ab380): record entry esp, [entry_esp] (the retaddr slot),
#     args (stream, out-ptr, ...), and the raw stack window.
#   - at 0x5ab395 (right AFTER the inner FUN_004cbd30 header read returns):
#     re-read [entry_esp] and the 0xc-byte header buffer + bytes-read (eax).
#   This tells us whether the read clobbers the return slot, and the header.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_ab380_corrupt.txt'

AGENT = r'''
'use strict';
var entryEsp = null, entryRet = null, bufPtr = null;
function hx(p){ return '0x'+(p>>>0).toString(16); }

Interceptor.attach(ptr(0x5ab380), {
  onEnter: function(){
    // at entry, esp points at the (Frida-managed) original return slot.
    entryEsp = this.context.esp;
    // Frida replaces the on-stack retaddr; use this.returnAddress for the real one.
    var sp = this.context.esp;
    var win = [];
    for (var i=0;i<10;i++){ try{ win.push(hx(sp.add(i*4).readU32())); }catch(e){ win.push('?'); } }
    send({kind:'enter', esp: sp.toString(), retAddr: this.returnAddress.toString(),
          arg0: this.context.esp.add(4).readU32()? hx(this.context.esp.add(4).readU32()):'0',
          stack: win});
  }
});

// 0x5ab395: just after `call 0x4cbd30` returns (add esp,0xc follows). esp here
// equals entry_esp-0x10 (sub 0xc + push esi). Read the header buffer & retslot.
Interceptor.attach(ptr(0x5ab395), {
  onEnter: function(){
    var sp = this.context.esp;          // = entryEsp - 0x10
    // buffer was at (entryEsp - 0xc); header is 0xc bytes there.
    var buf = sp.add(0x4);              // entryEsp-0x10+0x4 = entryEsp-0xc = &buffer
    var hdr = [];
    for (var i=0;i<3;i++){ try{ hdr.push(hx(buf.add(i*4).readU32())); }catch(e){ hdr.push('?'); } }
    // the original return slot is at entryEsp (= sp + 0x10)
    var retslot = sp.add(0x10);
    send({kind:'afterread', eax: hx(this.context.eax), bytesRead: this.context.eax.toInt32(),
          header: hdr, retslot_val: hx(retslot.readU32())});
  }
});
send({kind:'hooked'});

var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', eip:c.pc.toString(), mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        eax:c.eax.toString(), ecx:c.ecx.toString(), esp:c.esp.toString()});
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
        time.sleep(0.05)
    if not pid: sys.exit("timeout")
    sess=frida.get_local_device().attach(pid); print("  attached")
    ev=[]; ne=0; na=0
    def on(m,_):
        nonlocal ne,na
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='hooked': print("  hooked FUN_005ab380 entry + post-read")
        elif k=='ready': print("  watching")
        elif k=='enter':
            ne+=1
            if ne<=10: print(f"  ENTER #{ne} esp={p['esp']} ret={p['retAddr']} stack={p['stack']}")
            ev.append(p)
        elif k=='afterread':
            na+=1
            corrupt = p['retslot_val'] in ('0x44',)
            if na<=10 or corrupt:
                print(f"  AFTERREAD #{na} bytesRead={p['bytesRead']} header={p['header']}"
                      f" retslot={p['retslot_val']}{'  <== RETSLOT CORRUPTED!' if corrupt else ''}")
            ev.append(p)
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} ecx={p['ecx']} | enters={ne} afterreads={na} ===")
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.3); break
        try:
            if not psutil.pid_exists(pid): print("  gone (enters=%d afterreads=%d)"%(ne,na)); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
