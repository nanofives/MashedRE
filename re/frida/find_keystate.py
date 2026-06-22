# Find MASHED's DirectInput keyboard keystate buffer global (the lpvData passed to
# GetDeviceState cb=256). Attach to a running MASHED, resolve GetDeviceState via the
# dinput8 vtable scan, and log the buffer address on the next keyboard poll.
# Usage: py -3.12 re/frida/find_keystate.py <pid>
import sys, time
import frida

pid = int(sys.argv[1])
AGENT = r'''
'use strict';
let textLo=null,textHi=null;
function inText(p){ return textLo && p.compare(textLo)>=0 && p.compare(textHi)<0; }
let done=false;
['dinput8_real.DLL','DINPUT8.dll','dinput8.dll'].forEach(function(mn){
  const mod=Process.findModuleByName(mn); if(!mod) return;
  mod.enumerateRanges('r-x').forEach(function(r){ const e=r.base.add(r.size);
    if(textLo===null||r.base.compare(textLo)<0)textLo=r.base; if(textHi===null||e.compare(textHi)>0)textHi=e; });
  mod.enumerateRanges('r--').forEach(function(r){
    let a=r.base; const end=r.base.add(r.size).sub(4); let rs=null,rl=0;
    function fl(){ if(rs!==null&&rl>=16){ try{ const g=rs.add(36).readPointer();
      if(inText(g)){ Interceptor.attach(g,{ onEnter(args){ if(args[1].toInt32()===256 && !done){ done=true;
        send({buf:''+args[2]}); } } }); } }catch(e){} } rs=null; rl=0; }
    while(a.compare(end)<0){ let p=null; try{ p=a.readPointer(); }catch(e){}
      if(p&&inText(p)){ if(rs===null){rs=a;rl=0;} rl++; } else fl(); a=a.add(4); }
    fl();
  });
});
send({ready:1});
'''
dev=frida.get_local_device()
sess=dev.attach(pid)
got={}
def on(m,d):
    p=m.get('payload',{})
    if p.get('ready'): print("  hooked; press any key / wait for a poll...")
    if 'buf' in p:
        b=int(p['buf'],16); got['buf']=b
        rva = b-0x400000
        print(f"  keystate buffer @ 0x{b:08x}  (MASHED RVA 0x{rva:08x} if in-image)")
scr=sess.create_script(AGENT); scr.on('message',on); scr.load()
t=time.time()+6
while time.time()<t and 'buf' not in got: time.sleep(0.25)
sess.detach()
