#!/usr/bin/env py -3.12
"""raster_create_probe.py — why does the camera frameBuffer raster create fail?

Hooks FUN_004c77c0 (RwRasterCreate) + FUN_004670a0 (active-camera builder) during
a NO_AUTO_HOOK boot (both run in DisplayInit BEFORE the 0x4c7785 null-deref crash).
Logs RwRasterCreate's args (w,h,depth,flags from the stack) and return (raster ptr
or 0), and the camera builder's return. Reveals the exact dimensions/flags the
camera raster is requested with and whether the D3D9 constructor rejects them.
"""
import os, sys, time, frida
ROOT=os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE=os.path.join(ROOT,"original","MASHED.exe")
AGENT=r'''
'use strict';
var base=Process.enumerateModules().find(function(m){return m.name.toLowerCase()==='mashed.exe';}).base;
function R(off){return base.add(off);}
// FUN_004c77c0 = RwRasterCreate  (image base 0x400000 -> file off 0xc77c0)
Interceptor.attach(R(0xc77c0),{
  onEnter:function(a){
    this.w=this.context.esp.add(4).readU32();
    this.h=this.context.esp.add(8).readU32();
    this.d=this.context.esp.add(12).readU32();
    this.f=this.context.esp.add(16).readU32();
  },
  onLeave:function(ret){
    send({log:'RwRasterCreate(w='+this.w+' h='+this.h+' depth='+this.d+' flags=0x'+this.f.toString(16)+
              ') -> 0x'+ret.toUInt32().toString(16)});
  }
});
// FUN_004670a0 = active camera builder
Interceptor.attach(R(0x670a0),{
  onLeave:function(ret){ send({log:'CameraBuilder FUN_004670a0 -> 0x'+ret.toUInt32().toString(16)+
                                   '  DAT_006905b0=0x'+R(0x2905b0).readU32().toString(16)}); }
});
// dump the camera frameBuffer/zBuffer rasters after build
send({log:'armed at base '+base});
'''
def main():
    env={**os.environ,'MASHED_RE_NO_AUTO_HOOK':'1'}
    pid=frida.spawn([EXE],cwd=os.path.join(ROOT,'original'),env=env)
    s=frida.attach(pid)
    def on(m,d):
        if m.get('type')=='send': print('  ',m['payload'].get('log',m['payload']))
        elif m.get('type')=='error': print('  JS-ERR',m.get('description'))
    sc=s.create_script(AGENT); sc.on('message',on); sc.load()
    frida.resume(pid); time.sleep(8)
    try: frida.kill(pid)
    except Exception: pass
if __name__=='__main__': main()
