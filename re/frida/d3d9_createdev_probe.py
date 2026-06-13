#!/usr/bin/env py -3.12
"""d3d9_createdev_probe.py — does IDirect3D9::CreateDevice succeed on this display?

Spawn MASHED (NO_AUTO_HOOK), hook the real d3d9 Direct3DCreate9 -> grab the
IDirect3D9*, read vtable[16] (CreateDevice), Interceptor.attach it, and log the
HRESULT + key present params + whether *ppDevice came back non-null. Also logs
GetAdapterCount / GetAdapterDisplayMode for the single-monitor picture.
"""
import os, sys, time, frida
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(ROOT, "original", "MASHED.exe")

AGENT = r'''
'use strict';
function findReal() {
  // prefer d3d9_real.dll (our proxy forwards to it); fall back to d3d9.dll
  var names = ['d3d9_real.dll','d3d9.dll'];
  var mods = Process.enumerateModules();
  for (var i=0;i<names.length;i++){
    for (var j=0;j<mods.length;j++){
      if (mods[j].name.toLowerCase()===names[i]){
        var e=null; try{ e=mods[j].findExportByName('Direct3DCreate9'); }catch(_){}
        if(e) return {mod:names[i],exp:e};
      }
    }
  }
  return null;
}
function arm() {
  var r = findReal();
  if(!r){ send({log:'Direct3DCreate9 not found yet'}); return false; }
  send({log:'hooking Direct3DCreate9 in '+r.mod});
  Interceptor.attach(r.exp, {
    onLeave: function(ret){
      try {
        var pD3D = ptr(ret.toString());
        if (pD3D.isNull()){ send({log:'Direct3DCreate9 returned NULL'}); return; }
        var vt = pD3D.readPointer();
        var pCreateDevice = vt.add(16*4).readPointer();   // IDirect3D9::CreateDevice = slot 16
        var pGetAdapterCount = vt.add(4*4).readPointer();  // slot 4
        var GAC = new NativeFunction(pGetAdapterCount,'uint32',['pointer']);
        send({log:'IDirect3D9='+pD3D+' adapterCount='+GAC(pD3D)});
        Interceptor.attach(pCreateDevice, {
          onEnter: function(a){
            this.adapter=a[1].toInt32(); this.devtype=a[2].toInt32();
            this.flags=a[4].toUInt32(); this.pp=ptr(a[5].toString()); this.ppDev=ptr(a[6].toString());
            var w=0,h=0,windowed=0,fmt=0;
            try{ w=this.pp.add(0).readU32(); h=this.pp.add(4).readU32(); fmt=this.pp.add(8).readU32();
                 windowed=this.pp.add(0x38).readU32(); }catch(e){}
            send({log:'CreateDevice enter adapter='+this.adapter+' devtype='+this.devtype+
                      ' flags=0x'+this.flags.toString(16)+' bbW='+w+' bbH='+h+' fmt='+fmt+' windowed='+windowed});
          },
          onLeave: function(ret){
            var hr = ret.toUInt32();
            var dev = '0';
            try{ dev = this.ppDev.readPointer().toString(); }catch(e){}
            send({log:'CreateDevice LEAVE hr=0x'+hr.toString(16)+' *ppDevice='+dev});
          }
        });
      } catch(e){ send({log:'hook err '+e}); }
    }
  });
  return true;
}
// d3d9 may not be loaded at spawn; poll briefly
var tries=0;
var iv=setInterval(function(){ tries++; if(arm()||tries>200){ clearInterval(iv);} }, 25);
send({armed:true});
'''

def main():
    env={**os.environ,'MASHED_RE_NO_AUTO_HOOK':'1'}
    pid=frida.spawn([EXE],cwd=os.path.join(ROOT,'original'),env=env)
    s=frida.attach(pid)
    def on(m,d):
        if m.get('type')=='send':
            p=m['payload']
            if p.get('log'): print('  ',p['log'])
            elif p.get('armed'): print('   (armed)')
            else: print('   msg:',p)
        elif m.get('type')=='error':
            print('   JS-ERROR:', m.get('description'), m.get('stack','')[:200])
    sc=s.create_script(AGENT); sc.on('message',on); sc.load()
    frida.resume(pid)
    time.sleep(8)
    try: frida.kill(pid)
    except Exception: pass

if __name__=='__main__': main()
