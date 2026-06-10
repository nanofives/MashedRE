# Confirm the crash #2 mechanism: hook IDirect3D9::CreateDevice (via the d3d9
# proxy's Direct3DCreate9), capture the creation params, then hook the resulting
# device's SetRenderState (vtable index 57 = +0xe4). A flag tracks whether we are
# INSIDE SetRenderState when the AV fires → proves driver-side vs game-side fault.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_createdevice.txt'

AGENT = r'''
'use strict';
function hx(v){ return '0x'+(v>>>0).toString(16); }
var inSRS = 0;       // depth inside SetRenderState
var srsCount = 0;
var hookedSRS = false;

function hookDevice(devPtr){
  if (hookedSRS || devPtr.isNull()) return;
  var vt = devPtr.readPointer();
  var srs = vt.add(57*4).readPointer();   // IDirect3DDevice9::SetRenderState
  send({kind:'devinfo', device: devPtr.toString(), vtable: vt.toString(), srs: srs.toString()});
  Interceptor.attach(srs, {
    onEnter: function(a){
      inSRS++; srsCount++;
      if (srsCount <= 12)
        send({kind:'srs', state: hx(a[1].toUInt32()), value: hx(a[2].toUInt32())});
    },
    onLeave: function(){ inSRS--; }
  });
  hookedSRS = true;
}

// Hook the proxy's Direct3DCreate9 to get IDirect3D9, then patch-detect CreateDevice.
var mods = ['d3d9.dll','d3d9_real.dll'];
var d3dCreate = null;
mods.forEach(function(m){ if(!d3dCreate){ try{ d3dCreate = Module.getExportByName(m,'Direct3DCreate9'); }catch(e){} } });
if (d3dCreate){
  Interceptor.attach(d3dCreate, { onLeave: function(ret){
    if (ret.isNull()) return;
    var id3d9 = ptr(ret.toString());
    var vt = id3d9.readPointer();
    var createDev = vt.add(16*4).readPointer();   // IDirect3D9::CreateDevice (index 16)
    send({kind:'note', t:'Direct3DCreate9 -> '+ret+' CreateDevice='+createDev});
    Interceptor.attach(createDev, {
      onEnter: function(a){
        // (this, Adapter, DevType, hWnd, BehaviorFlags, pPP, ppDevice)
        this.ppDevice = a[6];
        var pp = a[5];
        var info = {adapter: a[1].toUInt32(), devType: a[2].toUInt32(), flags: hx(a[4].toUInt32())};
        try {
          info.bbWidth  = pp.add(0).readU32();
          info.bbHeight = pp.add(4).readU32();
          info.bbFormat = pp.add(8).readU32();
          info.windowed = pp.add(0x14).readU32();   // approx offset; informational
        } catch(e){}
        send({kind:'createdev', info: info});
      },
      onLeave: function(ret){
        try { var dev = this.ppDevice.readPointer(); if(!dev.isNull()) hookDevice(dev); } catch(e){}
        send({kind:'note', t:'CreateDevice returned hr='+hx(ret.toUInt32())});
      }
    });
  }});
  send({kind:'note', t:'hooked Direct3DCreate9 @ '+d3dCreate});
} else {
  send({kind:'note', t:'Direct3DCreate9 NOT FOUND yet (will the module load later?)'});
}

var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', type:d.type, eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        inSetRenderState: inSRS, srsCount: srsCount,
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
        time.sleep(0.03)
    if not pid: sys.exit("timeout")
    sess=frida.get_local_device().attach(pid); print("  attached")
    ev=[]
    def on(m,_):
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='note': print("  NOTE:",p['t']); ev.append(p)
        elif k=='ready': print("  watching")
        elif k=='createdev': print("  CreateDevice params:",p['info']); ev.append(p)
        elif k=='devinfo': print("  device=%s vtable=%s SetRenderState=%s"%(p['device'],p['vtable'],p['srs'])); ev.append(p)
        elif k=='srs': print("  SetRenderState(state=%s, value=%s)"%(p['state'],p['value'])); ev.append(p)
        elif k=='crash':
            print(f"  === CRASH {p['type']} eip={p['eip']} mem={p['mem']} ===")
            print(f"   inSetRenderState={p['inSetRenderState']}  srsCount={p['srsCount']}  ecx={p['ecx']} ebx={p['ebx']}")
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
