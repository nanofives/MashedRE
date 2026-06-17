# Probe the standalone vehicle record + contact globals during a real-physics race
# to diagnose why speed stays 0 (post crash-fix). Reads the player car record
# (g_records[0]) grounded count (+0x278 float, +0x9e0 int), velocity (+0x9b0),
# speed (+0x9e4), drive accum (+0xb14), and Collision::g_worldTriCount /
# g_terrainEntryCount by resolving symbols from the .map.
import os, sys, time, bisect
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
EXE  = ROOT / 'mashedmod' / 'build' / 'mashed_re.exe'
MAP  = ROOT / 'mashedmod' / 'build' / 'mashed_re.map'

def map_sym(name_sub):
    out = []
    for line in open(MAP, encoding='utf-8', errors='ignore'):
        p = line.split()
        if len(p) >= 4 and p[0].startswith('0001:') or (len(p)>=4 and p[0].startswith('0002:')) or (len(p)>=4 and p[0].startswith('0003:')):
            pass
    return out

def find_addr(sub):
    for line in open(MAP, encoding='utf-8', errors='ignore'):
        p = line.split()
        if len(p) >= 4 and sub in p[1]:
            try: return int(p[2], 16), p[1]
            except: pass
    return None, None

# resolve g_records (static in VehiclePhysicsRun), g_worldTriCount, g_terrainEntryCount
g_records, n1 = find_addr('g_records')
g_wtc, n2 = find_addr('g_worldTriCount')
g_tec, n3 = find_addr('g_terrainEntryCount')
print('g_records', hex(g_records) if g_records else None, n1)
print('g_worldTriCount', hex(g_wtc) if g_wtc else None, n2)
print('g_terrainEntryCount', hex(g_tec) if g_tec else None, n3)

AGENT = '''
'use strict';
const REC = ptr('%s');
const WTC = ptr('%s');
const TEC = ptr('%s');
function f(off){ return REC.add(off).readFloat(); }
function i(off){ return REC.add(off).readU32(); }
let n=0;
const id=setInterval(function(){
    try {
        send({kind:'st',
            wtc: WTC.readU32(),
            tec: TEC.readU32(),
            // grounded count is a FLOAT at byte +0x9e0 (int idx 0x278); 4.0 = all-grounded
            grounded_9e0_f: f(0x9e0),
            grounded_9e0_hex: '0x'+i(0x9e0).toString(16),
            vel:[f(0x9b0),f(0x9b4),f(0x9b8)],
            speed9e4: f(0x9e4),
            driveB14:[f(0xb14),f(0xb18),f(0xb1c)],   // drive accum (FUN_00467650 +0xb14)
            torque1a8: f(0x1a8),
            state9f0: i(0x9f0),
            wstate:[i(0x198),i(0x25c),i(0x320),i(0x3e4)],  // wheel states 0..3
        });
    } catch(e) { send({kind:'err', e:String(e)}); }
    if (++n>=24) clearInterval(id);
}, 500);
''' % (hex(g_records), hex(g_wtc), hex(g_tec))

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
    time.sleep(14)
    try: s.detach()
    except Exception: pass
    try: dev.kill(pid)
    except Exception: pass

if __name__=='__main__': main()
