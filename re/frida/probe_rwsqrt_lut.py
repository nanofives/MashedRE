# Probe the RW sqrt-LUT root globals in the running standalone to see why the
# RwSqrtLutReady() guard does not fire. Reads 0x007d3ff8 / 0x007d3ffc and the
# derived LUT root pointer.
import os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
EXE  = ROOT / 'mashedmod' / 'build' / 'mashed_re.exe'

AGENT = r'''
'use strict';
function dump() {
    try {
        const g = ptr('0x7d3ff8').readU32();
        const o = ptr('0x7d3ffc').readU32();
        send({ kind:'lut', g:g.toString(16), o:o.toString(16),
               g_plus_o: (g+o).toString(16) });
    } catch(e) { send({ kind:'err', e:String(e) }); }
}
// dump a few times over the first ~2s
let n=0;
const id=setInterval(function(){ dump(); if(++n>=5){clearInterval(id);} }, 300);
send({kind:'ready'});
'''

def main():
    env = dict(os.environ)
    env.update({'MASHED_REAL_PHYSICS':'1','MASHED_RACE_DEMO':'1','MASHED_PLAY_DEMO':'1',
                'MASHED_GOTO':'6','MASHED_TRACK_SEL':'0','MASHED_CAR_SEL':'0'})
    dev = frida.get_local_device()
    pid = dev.spawn([str(EXE)], cwd=str(EXE.parent), env=env)
    s = dev.attach(pid)
    def on(m,d):
        if m['type']=='error': print('err',m.get('description')); return
        print(m.get('payload'))
    sc = s.create_script(AGENT); sc.on('message',on); sc.load()
    dev.resume(pid)
    time.sleep(3)
    try: s.detach()
    except Exception: pass
    try: dev.kill(pid)
    except Exception: pass

if __name__=='__main__': main()
