# Read MASHED's live per-player keyboard binding map (DAT_007e96c8, 13 controls each;
# device type at DAT_007e96fc). Usage: py -3.12 re/frida/find_bindings.py <pid>
import sys, time
import frida

pid = int(sys.argv[1])
AGENT = r'''
'use strict';
const m = Process.findModuleByName('MASHED.exe');
const D = m.base.sub(0x400000);                 // ASLR delta (0 if based at 0x400000)
function rd(rva){ try { return D.add(rva).readU32(); } catch(e){ return -1; } }
const out = {};
for (let p = 0; p < 4; p++){
  let t = -1; try { t = D.add(0x7e96fc + p*0x80*4).readS32(); } catch(e){}
  const cs = []; for (let c = 0; c < 13; c++) cs.push(rd(0x7e96c8 + (p*0x80 + c)*4));
  out['p'+p] = {type: t, cs: cs};
}
send(out);
'''
DIK={0x00:'(none)',0x01:'ESC',0x02:'1',0x03:'2',0x04:'3',0x05:'4',0x06:'5',0x07:'6',0x08:'7',0x09:'8',0x0a:'9',0x0b:'0',
0x0c:'-',0x0d:'=',0x0e:'BACKSPACE',0x0f:'TAB',0x10:'Q',0x11:'W',0x12:'E',0x13:'R',0x14:'T',0x15:'Y',0x16:'U',0x17:'I',0x18:'O',0x19:'P',
0x1a:'[',0x1b:']',0x1c:'ENTER',0x1d:'LCTRL',0x1e:'A',0x1f:'S',0x20:'D',0x21:'F',0x22:'G',0x23:'H',0x24:'J',0x25:'K',0x26:'L',
0x27:';',0x28:"'",0x29:'`',0x2a:'LSHIFT',0x2b:'\\',0x2c:'Z',0x2d:'X',0x2e:'C',0x2f:'V',0x30:'B',0x31:'N',0x32:'M',0x33:',',0x34:'.',0x35:'/',0x36:'RSHIFT',
0x37:'NUM*',0x38:'LALT',0x39:'SPACE',0x3a:'CAPS',0x47:'NUM7',0x48:'NUM8',0x49:'NUM9',0x4a:'NUM-',0x4b:'NUM4',0x4c:'NUM5',0x4d:'NUM6',0x4e:'NUM+',0x4f:'NUM1',0x50:'NUM2',0x51:'NUM3',0x52:'NUM0',0x53:'NUM.',
0x9c:'NUMENTER',0x9d:'RCTRL',0xb5:'NUM/',0xb8:'RALT',0xc7:'HOME',0xc8:'UP',0xc9:'PGUP',0xcb:'LEFT',0xcd:'RIGHT',0xcf:'END',0xd0:'DOWN',0xd1:'PGDN',0xd2:'INS',0xd3:'DEL'}
def nm(v):
    if v < 0: return '(unread)'
    sc = v & 0xff
    return f"0x{sc:02x} {DIK.get(sc,'?')}"

dev=frida.get_local_device(); sess=dev.attach(pid)
res={}
def on(m,d):
    if m.get('type')=='error': print('  err', m.get('description')); return
    res.update(m.get('payload',{}))
scr=sess.create_script(AGENT); scr.on('message',on); scr.load()
time.sleep(1.0); sess.detach()
TYPE={0:'none',1:'joystick',2:'keyboard'}
for p in range(4):
    pd=res.get('p'+str(p))
    if not pd: continue
    print(f"Player {p} (device type {pd['type']}={TYPE.get(pd['type'],'?')}):")
    for c,v in enumerate(pd['cs']):
        print(f"  ctrl[{c:>2}] = {nm(v)}")
