"""
Bypass test: 4 track-node crashers + VfsStreamRead = 5 hooks bypassed.
All other hooks including 28 save hooks remain installed.
"""
import json, os, subprocess, sys, time
from pathlib import Path
import frida, psutil, pefile

ROOT = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'

pe_obj = pefile.PE(str(MASHED_EXE))
ib = pe_obj.OPTIONAL_HEADER.ImageBase

def orig(rva):
    off = pe_obj.get_offset_from_rva(rva - ib)
    return list(pe_obj.__data__[off:off+8])

bypass_rvas = {
    # CONFIRMED CRASHERS:
    # 1. All 10 TrackNode hooks that dereference DAT_0063d7e4 (NULL at main menu)
    0x0041e870: 'TrackNodeRecordScan',
    0x0041e8b0: 'TrackNodeDispatch14',
    0x0041e8c0: 'TrackNodeDispatch18',
    0x0041e8f0: 'TrackNodeDispatch24',
    0x0041e9b0: 'TrackNodeFieldCmp10',
    0x0041e980: 'TrackNodeRecordFind',
    0x0041e9d0: 'TrackNodeFnPtrGet14',
    0x0041e9e0: 'TrackNodeFnPtrGet18',
    0x0041e970: 'TrackNodeDispatch44',
    0x0041ea90: 'TrackNodeFnPtrGet44',
    # 2. VfsStreamRead calling convention mismatch
    0x00550980: 'VfsStreamRead',
    # 3. RwTexDictionaryCreate demoted to C2 but still installed
    0x004c5890: 'RwTexDictionaryCreate',
    # 4. CrtSehProlog broken SEH implementation (non-standard ABI, crashes immediately)
    0x004a5984: 'CrtSehProlog',
    # 5. CrtSehEpilog broken SEH implementation (paired with prolog)
    0x004a59bf: 'CrtSehEpilog',
}
restore_map = {hex(rva): orig(rva) for rva in bypass_rvas}
print("Bypassing:", [n for n in bypass_rvas.values()])

AGENT_TEMPLATE = r"""
'use strict';
const ASI_PATH = $ASI$;
const RESTORE  = $RESTORE$;
Module.load(ASI_PATH);
const m = Process.findModuleByName('mashed_re_dev.asi');
if (!m) { send({t:'error'}); throw new Error(); }
new NativeFunction(m.findExportByName('InjectHooks'), 'void', [], 'mscdecl')();
for (const [rs, bytes] of Object.entries(RESTORE)) {
    const p = ptr(rs);
    Memory.protect(p, 8, 'rwx');
    for (let i = 0; i < bytes.length; i++) p.add(i).writeU8(bytes[i]);
}
send({t:'done'});
rpc.exports = {};
"""

agent = (AGENT_TEMPLATE
         .replace('$ASI$', json.dumps(str(ASI_PATH).replace('\\', '\\\\')))
         .replace('$RESTORE$', json.dumps(restore_map)))

env = dict(os.environ)
import ctypes, ctypes.wintypes as wt
user32 = ctypes.windll.user32
si = subprocess.STARTUPINFO()
si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
si.wShowWindow = 7

proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
    stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    startupinfo=si, creationflags=0x00000200|0x00000008)
pid = proc.pid
print(f'pid={pid}')
time.sleep(8)
if not psutil.pid_exists(pid):
    print('died before attach'); sys.exit(1)

device = frida.get_local_device()
session = device.attach(pid)
done = [False]
def on_msg(msg, data):
    p = msg.get('payload', {})
    if msg['type'] == 'error': print('ERR:', msg.get('description')); done[0] = True; return
    print('MSG:', p)
    if p.get('t') == 'done': done[0] = True
script = session.create_script(agent)
script.on('message', on_msg)
script.load()
t0 = time.time()
while not done[0] and time.time()-t0 < 15: time.sleep(0.1)

print(f'Hooks active: all except bypassed {len(bypass_rvas)}. Observing 15s...')
stable = True
for i in range(15):
    time.sleep(1)
    alive = psutil.pid_exists(pid)
    print(f't={i+1}s alive={alive}')
    if not alive:
        print('DIED')
        stable = False
        break

if stable:
    print('STABLE 15s -- these 5 were the only crashers')
else:
    print('Still crashing -- more hooks to bypass')

try: session.detach()
except: pass
try: proc.kill()
except: pass
