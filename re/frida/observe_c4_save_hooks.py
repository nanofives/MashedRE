# Canonical-scenario C4 observation for Save subsystem — 29 C3 hooks.
#
# Pattern (mirrors observe_c4_render_math_hooks.py — no Interceptor):
#   1. Spawn MASHED.exe with d3d9 shim (required for windowed boot; NOT parked).
#   2. Wait for main-menu boot (~8s).
#   3. Frida-attach; agent: Module.load(.asi) -> InjectHooks() -> byte-verify.
#   4. Restore original bytes at VfsStreamRead (0x00550980) after install:
#      this hook has a runtime bug causing crashes at main menu when called
#      with fwrite-style args (e.g. from RwStreamWrite_s2 case 1/2). Excluded.
#   5. Idle 15s at main menu with NO Interceptor — alive-check only.
#      Per CLAUDE.md: "For hot-path verification, use behavioral observation:
#      Module.load the .asi, watch the game for N seconds with no Interceptor,
#      confirm no visual/crash regression."
#   6. Final RPC byte-poll to confirm patches stayed installed.
#   7. Write log/observe_c4_save_hooks.csv + human-readable report.
#
# C4 evidence: hook E9+rel32 verified installed + process alive 15s = sufficient
# per re/CONFIDENCE.md. VfsStreamRead excluded (crash isolation confirmed;
# binary search: 28-hook run stable, VfsStreamRead-bypass stable, VfsStreamRead
# alone crashed within ~10s).
#
# Usage:
#   py -3.12 re/frida/observe_c4_save_hooks.py [--seconds 15] [--boot-wait 8]
import argparse
import csv
import ctypes
from ctypes import wintypes
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import frida
import psutil

try:
    from pycaw.pycaw import AudioUtilities
    HAS_PYCAW = True
except ImportError:
    HAS_PYCAW = False

try:
    import pefile as _pefile
    HAS_PEFILE = True
except ImportError:
    HAS_PEFILE = False

user32 = ctypes.windll.user32
user32.PostMessageW.argtypes = [wintypes.HWND, wintypes.UINT, wintypes.WPARAM, wintypes.LPARAM]
user32.PostMessageW.restype  = wintypes.BOOL
user32.ShowWindow.argtypes   = [wintypes.HWND, ctypes.c_int]
user32.ShowWindow.restype    = wintypes.BOOL
user32.IsWindowVisible.argtypes = [wintypes.HWND]
user32.IsWindowVisible.restype  = wintypes.BOOL
user32.GetWindowThreadProcessId.argtypes = [wintypes.HWND, ctypes.POINTER(wintypes.DWORD)]
user32.GetWindowThreadProcessId.restype  = wintypes.DWORD
user32.GetWindow.argtypes    = [wintypes.HWND, wintypes.UINT]
user32.GetWindow.restype     = wintypes.HWND

_EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
user32.EnumWindows.argtypes = [_EnumWindowsProc, wintypes.LPARAM]
user32.EnumWindows.restype  = wintypes.BOOL
user32.GetWindowTextW.argtypes = [wintypes.HWND, wintypes.LPWSTR, ctypes.c_int]
user32.GetWindowTextW.restype  = ctypes.c_int
user32.GetClassNameW.argtypes  = [wintypes.HWND, wintypes.LPWSTR, ctypes.c_int]
user32.GetClassNameW.restype   = ctypes.c_int

SW_SHOWMINNOACTIVE = 7
WM_CLOSE           = 0x0010
GW_OWNER           = 4

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
LOG_DIR    = ROOT / 'log'
LOG_DIR.mkdir(parents=True, exist_ok=True)
CSV_OUT    = LOG_DIR / 'observe_c4_save_hooks.csv'
TXT_OUT    = LOG_DIR / 'observe_c4_save_hooks.txt'

# Hooks bypassed after install (confirmed crashers at main menu).
# Each entry: {rva_str, name, reason}
# The bytes at each RVA are read from the original PE and restored immediately
# after InjectHooks() installs the inline-JMP patches.
BYPASS_HOOKS = [
    {
        'rva':    '0x00550980',
        'name':   'VfsStreamRead',
        'reason': 'crash_isolation: fwrite callers pass (buf,1,count,fh) but '
                  'reimpl expects (ctx,buf,element_size,count); causes AV',
    },
    {
        'rva':    '0x004a5984',
        'name':   'CrtSehProlog',
        'reason': 'crash_isolation: __SEH_prolog has non-standard compiler ABI; '
                  'reimpl does push ebp/mov ebp,esp which corrupts caller\'s SEH '
                  'frame setup mid-prolog; crashes any SEH-using function at main menu',
    },
    {
        'rva':    '0x004a59bf',
        'name':   'CrtSehEpilog',
        'reason': 'crash_isolation: paired with CrtSehProlog; epilog unlinks '
                  'SEH node installed by prolog; if prolog is wrong, epilog crashes',
    },
]
# Also bypass 10 TrackNode hooks (NULL deref of DAT_0063d7e4 at main menu):
TRACKNODE_BYPASS_RVAS = [
    '0x0041e870', '0x0041e8b0', '0x0041e8c0', '0x0041e8f0', '0x0041e9b0',
    '0x0041e980', '0x0041e9d0', '0x0041e9e0', '0x0041e970', '0x0041ea90',
]
# RwTexDictionaryCreate (demoted C3->C2, RH_ScopedInstall not removed from source):
BYPASS_HOOKS.append({
    'rva':    '0x004c5890',
    'name':   'RwTexDictionaryCreate',
    'reason': 'demoted C3->C2 (integration RED, pointer-identity mismatch in '
              'heap allocator); RH_ScopedInstall left in source; still installed',
})
# Add TrackNode hooks to bypass list
for _rva in TRACKNODE_BYPASS_RVAS:
    BYPASS_HOOKS.append({
        'rva': _rva,
        'name': f'TrackNode_{_rva}',
        'reason': 'NULL deref: DAT_0063d7e4 is NULL at quiescent main menu',
    })

BYPASS_RVA = '0x00550980'   # kept for backward compat (first bypass in old logic)

# 29 Save-subsystem C3 hooks — {name, rva, export}
# 'name'   = hooks.csv name (for report readability)
# 'rva'    = image-relative virtual address (matches hooks.csv)
# 'export' = exact symbol name exported from mashed_re_dev.asi
HOOKS = [
    {'name': 'SaveLoad',                    'rva': '0x00404e50', 'export': 'SaveLoad'},
    {'name': 'DeserializeFromBuffer',       'rva': '0x00404e80', 'export': 'DeserializeFromBuffer'},
    {'name': 'SerializeToBuffer',           'rva': '0x00404ee0', 'export': 'SerializeToBuffer'},
    {'name': 'SaveWrite',                   'rva': '0x00404f50', 'export': 'SaveWrite'},
    {'name': 'SaveFileExists',              'rva': '0x00404f80', 'export': 'SaveFileExists'},
    {'name': 'AutosaveTrigger',             'rva': '0x004099a0', 'export': 'AutosaveTrigger'},
    {'name': 'SaveStatusClear',             'rva': '0x004099e0', 'export': 'SaveStatusClear'},
    {'name': 'GuardConcludedAndP1Won',      'rva': '0x0040dd60', 'export': 'GuardConcludedAndP1Won'},
    {'name': 'ThunkReplaySave',             'rva': '0x0040de00', 'export': 'ThunkReplaySave'},
    {'name': 'PostTrophyEvent',             'rva': '0x0042a920', 'export': 'PostTrophyEvent'},
    {'name': 'ChampionshipComplete',        'rva': '0x00430290', 'export': 'ChampionshipComplete'},
    {'name': 'ConfigLogError',              'rva': '0x004963e0', 'export': 'ConfigLogError'},
    {'name': 'ConfigLogDebug',             'rva': '0x00496400', 'export': 'ConfigLogDebug'},
    {'name': 'ConfigFilenameGet',           'rva': '0x00498910', 'export': 'ConfigFilenameGet'},
    {'name': 'ConfigLoad',                  'rva': '0x00498950', 'export': 'ConfigLoad'},
    {'name': 'ConfigSave',                  'rva': '0x004989b0', 'export': 'ConfigSave'},
    {'name': 'ReadModeFromCombo_j5',        'rva': '0x00498d20', 'export': 'ReadModeFromCombo_j5'},
    {'name': 'PopulateModeCombo_s2',        'rva': '0x00498d60', 'export': 'PopulateModeCombo_s2'},
    {'name': 'VideoDialogInit_i3',          'rva': '0x00498f60', 'export': 'VideoDialogInit_i3'},
    {'name': 'SubsystemSelectionChanged_i3','rva': '0x00499170', 'export': 'SubsystemSelectionChanged_i3'},
    {'name': 'VideoSettingsDlgProc_s2',     'rva': '0x004991f0', 'export': 'VideoSettingsDlgProc_s2'},
    {'name': 'SetControlTextFromResource_i3','rva': '0x00499740', 'export': 'SetControlTextFromResource_i3'},
    {'name': 'FsopenSafe',                  'rva': '0x004a4541', 'export': 'FsopenSafe'},
    {'name': 'FileReadWrapper_i3',          'rva': '0x004b3b70', 'export': 'FileReadWrapper_i3'},
    {'name': 'FileWriteWrapper_i3',         'rva': '0x004b3bb0', 'export': 'FileWriteWrapper_i3'},
    {'name': 'RwStreamWrite_s2',            'rva': '0x004cbe80', 'export': 'RwStreamWrite_s2'},
    # NOTE: VfsStreamRead (0x00550980) intentionally OMITTED from observation list.
    # Its E9 is installed by InjectHooks but restored immediately (see BYPASS_RVA).
    # See crash isolation notes above.
    {'name': 'VfsFileExists',               'rva': '0x00550b00', 'export': 'VfsFileExists'},
    {'name': 'VfsStreamGetType',            'rva': '0x00550bc0', 'export': 'VfsStreamGetType'},
]

# AGENT: load ASI, InjectHooks, restore bypass bytes, byte-verify 28 save hooks.
AGENT_JS = r"""
'use strict';

const HOOKS        = $HOOKS$;
const ASI_PATH     = $ASI_PATH$;
const BYPASS_MAP   = $BYPASS_MAP$;   // {rva_str: [byte, ...], ...}

function bytesHex(p, n) {
    const a = [];
    for (let i = 0; i < n; i++) a.push(p.add(i).readU8().toString(16).padStart(2,'0'));
    return a.join(' ');
}

function snapshotBytes() {
    const out = {};
    for (const h of HOOKS) {
        const addr = ptr(h.rva);
        out[h.name] = { rva: h.rva, bytes: bytesHex(addr,8), opcode: '0x'+addr.readU8().toString(16) };
    }
    return out;
}

rpc.exports = {
    poll: function() { return snapshotBytes(); },
};

// --- Phase 1: byte-verify install ---
const pre = snapshotBytes();
send({ type: 'pre_snapshot', data: pre });

let module;
try {
    Module.load(ASI_PATH);
    module = Process.findModuleByName('mashed_re_dev.asi');
    if (!module) throw new Error('findModuleByName returned null');
} catch(e) {
    send({ type: 'error', msg: 'Module.load failed: ' + e.message });
    throw e;
}

const reimpl = {};
for (const h of HOOKS) {
    const exp = module.findExportByName(h.export);
    if (!exp) {
        send({ type: 'error', msg: 'export not found: ' + h.export });
        throw new Error('missing export ' + h.export);
    }
    reimpl[h.name] = exp.toString();
}

const injectAddr = module.findExportByName('InjectHooks');
if (!injectAddr) { send({ type: 'error', msg: 'InjectHooks not found' }); throw new Error(); }
try {
    new NativeFunction(injectAddr, 'void', [], 'mscdecl')();
    send({ type: 'inject_called', addr: injectAddr.toString() });
} catch(e) {
    send({ type: 'error', msg: 'InjectHooks() threw: ' + e.message }); throw e;
}

// --- Restore original bytes at all bypassed RVAs immediately after InjectHooks ---
// Order matters: SEH hooks must be restored first to avoid corrupting exception handling.
let bypassed_count = 0;
for (const [rva_str, orig_bytes] of Object.entries(BYPASS_MAP)) {
    try {
        const bp = ptr(rva_str);
        Memory.protect(bp, 8, 'rwx');
        for (let i = 0; i < orig_bytes.length; i++) bp.add(i).writeU8(orig_bytes[i]);
        bypassed_count++;
    } catch(e) {
        send({ type: 'bypass_error', rva: rva_str, err: e.message });
    }
}
send({ type: 'bypass_restored', count: bypassed_count });

// --- Byte-verify the 28 save hooks (all are in HOOKS list, not in bypass set) ---
const post = snapshotBytes();
const verified = [];
for (const h of HOOKS) {
    const addr = ptr(h.rva);
    const opcode  = addr.readU8();
    const rel32   = addr.add(1).readS32();
    const exp_rel32 = ptr(reimpl[h.name]).toInt32() - addr.toInt32() - 5;
    verified.push({
        name:           h.name,
        rva:            h.rva,
        export:         h.export,
        reimpl_addr:    reimpl[h.name],
        pre_bytes:      pre[h.name].bytes,
        post_bytes:     post[h.name].bytes,
        opcode_hex:     '0x' + opcode.toString(16),
        opcode_ok:      opcode === 0xE9,
        rel32_hex:      '0x' + (rel32>>>0).toString(16),
        exp_rel32_hex:  '0x' + (exp_rel32>>>0).toString(16),
        rel32_ok:       rel32 === exp_rel32,
    });
}
send({ type: 'post_snapshot', module_base: module.base.toString(),
       module_size: module.size, hooks: verified });

// No Interceptor phase — per CLAUDE.md safe-observation rule.
send({ type: 'counters_ready' });
"""


state = {
    'pre_snapshot':   None,
    'post_snapshot':  None,
    'bypass_ok':      False,
    'counters_ready': False,
    'errors':         [],
    'done':           False,
}


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'), message.get('stack','')[:200])
        state['errors'].append(message)
        state['done'] = True
        return
    p = message.get('payload', {})
    kind = p.get('type')
    if kind == 'error':
        print(f"  [agent] ERROR: {p['msg']}")
        state['errors'].append(p)
        state['done'] = True
    elif kind == 'pre_snapshot':
        state['pre_snapshot'] = p
        print('  [agent] pre_snapshot captured')
    elif kind == 'inject_called':
        print(f"  [agent] InjectHooks() at {p['addr']}")
    elif kind == 'bypass_restored':
        state['bypass_ok'] = True
        print(f"  [agent] {p['count']} bypass hooks restored to original bytes")
    elif kind == 'bypass_error':
        print(f"  [agent] bypass restore FAILED at {p.get('rva','?')}: {p['err']}")
    elif kind == 'post_snapshot':
        state['post_snapshot'] = p
        print('  [agent] post_snapshot captured')
    elif kind == 'counters_ready':
        state['counters_ready'] = True
        state['done'] = True
        print('  [agent] ready for observation (no Interceptor)')
    else:
        print('  [agent]', p)


def mute_pid_audio(pid):
    if not HAS_PYCAW:
        return False
    try:
        for sess in AudioUtilities.GetAllSessions():
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception as e:
        print(f"  mute failed: {e}")
    return False


def list_pid_windows(pid):
    found = []
    @_EnumWindowsProc
    def cb(hwnd, _):
        owner_pid = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(owner_pid))
        if owner_pid.value != pid:
            return True
        tb = ctypes.create_unicode_buffer(256)
        kb = ctypes.create_unicode_buffer(256)
        user32.GetWindowTextW(hwnd, tb, 256)
        user32.GetClassNameW(hwnd, kb, 256)
        found.append({'hwnd': hwnd, 'visible': bool(user32.IsWindowVisible(hwnd)),
                      'owned': user32.GetWindow(hwnd, GW_OWNER) != 0,
                      'title': tb.value, 'klass': kb.value})
        return True
    user32.EnumWindows(cb, 0)
    return found


def find_main_window(pid):
    wins = list_pid_windows(pid)
    wins.sort(key=lambda w: (w['visible'], not w['owned'], bool(w['title'])), reverse=True)
    return wins[0]['hwnd'] if wins else None


def get_original_bytes(rva_str):
    """Read original bytes from MASHED.exe at the given RVA (0x-prefixed hex string)."""
    if not HAS_PEFILE:
        return None
    try:
        import pefile
        rva = int(rva_str, 16)
        pe = pefile.PE(str(MASHED_EXE))
        image_base = pe.OPTIONAL_HEADER.ImageBase
        offset = pe.get_offset_from_rva(rva - image_base)
        return list(pe.__data__[offset:offset + 8])
    except Exception as e:
        print(f"  WARN: could not read original bytes for {rva_str}: {e}")
        return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seconds',   type=int,   default=15)
    parser.add_argument('--boot-wait', type=float, default=8.0)
    parser.add_argument('--no-mute',   action='store_true')
    args = parser.parse_args()

    if not MASHED_EXE.exists():
        sys.exit(f"missing {MASHED_EXE}")
    if not ASI_PATH.exists():
        sys.exit(f"missing {ASI_PATH}")

    print(f"=== observe_c4_save_hooks — {len(HOOKS)} save hooks + {len(BYPASS_HOOKS)} bypassed ===")
    print(f"ASI:  {ASI_PATH}")
    print(f"EXE:  {MASHED_EXE}")
    print()
    print("d3d9 shim: present (required for windowed boot — NOT parking)")
    print()

    # Build bypass_map: {rva_str: [bytes]} for all bypassed hooks
    bypass_map = {}
    for bh in BYPASS_HOOKS:
        orig_bytes = get_original_bytes(bh['rva'])
        if orig_bytes is None:
            sys.exit(f"ERROR: could not read original bytes for {bh['name']} ({bh['rva']}) — need pefile")
        print(f"  bypass: {bh['name']} ({bh['rva']})  orig={[hex(b) for b in orig_bytes]}")
        bypass_map[bh['rva']] = orig_bytes
    print()

    return _run(args, bypass_map)


def _run(args, bypass_map):
    # Clear mashed.log
    log_path = ROOT / 'original' / 'mashed.log'
    try:
        if log_path.exists():
            log_path.unlink()
    except Exception:
        pass

    env = {k: v for k, v in os.environ.items() if k != 'MASHED_RE_NO_AUTO_HOOK'}

    si = subprocess.STARTUPINFO()
    si.dwFlags    |= subprocess.STARTF_USESHOWWINDOW
    si.wShowWindow = SW_SHOWMINNOACTIVE

    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        startupinfo=si, creationflags=0x00000200 | 0x00000008)
    pid = proc.pid
    print(f"  spawned MASHED.exe  pid={pid}")

    hwnd = None
    deadline = time.time() + 8
    while time.time() < deadline:
        hwnd = find_main_window(pid)
        if hwnd:
            user32.ShowWindow(hwnd, SW_SHOWMINNOACTIVE)
            print(f"  hwnd 0x{hwnd:08x} minimized")
            break
        time.sleep(0.3)
    if not hwnd:
        print("  WARN: no window found within 8s")

    if not args.no_mute:
        muted = False
        deadline = time.time() + 6
        while time.time() < deadline:
            if mute_pid_audio(pid):
                muted = True
                break
            time.sleep(0.2)
        print("  muted audio" if muted else "  could not mute audio")

    print(f"  waiting {args.boot_wait}s for main-menu boot...")
    t0 = time.time()
    while time.time() - t0 < args.boot_wait:
        if not psutil.pid_exists(pid):
            print("  process died during boot — ABORT")
            return 6
        time.sleep(0.5)

    if hwnd:
        user32.ShowWindow(hwnd, SW_SHOWMINNOACTIVE)

    print("  attaching Frida...")
    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f"  Frida attach failed: {e}")
        try: proc.kill()
        except Exception: pass
        return 4

    js_config_hooks  = json.dumps(HOOKS)
    js_config_asi    = json.dumps(str(ASI_PATH).replace('\\', '\\\\'))
    js_bypass_map    = json.dumps(bypass_map)
    agent_src = (AGENT_JS
                 .replace('$HOOKS$',      js_config_hooks)
                 .replace('$ASI_PATH$',   js_config_asi)
                 .replace('$BYPASS_MAP$', js_bypass_map))

    script = session.create_script(agent_src)
    script.on('message', on_message)
    script.load()

    # Wait for counters_ready (or error)
    deadline = time.time() + 25
    while not state['done'] and time.time() < deadline:
        time.sleep(0.1)

    if state['errors'] or not state['counters_ready']:
        print("  agent failed — aborting")
        try: proc.kill()
        except Exception: pass
        return 5

    post = state['post_snapshot']
    if not post:
        print("  no post_snapshot — aborting")
        try: proc.kill()
        except Exception: pass
        return 5

    n_bypassed = len(BYPASS_HOOKS)
    n_observed = len(HOOKS)

    # Report byte-verify
    print()
    print(f"=== hook install verification ({n_observed} save hooks; {n_bypassed} crashers bypassed) ===")
    all_installed = True
    for h in post['hooks']:
        ok = h['opcode_ok'] and h['rel32_ok']
        tag = "OK  " if ok else "FAIL"
        if not ok:
            all_installed = False
        print(f"  [{tag}] {h['name']:<36s} rva={h['rva']}")
        if not ok:
            print(f"         opcode={h['opcode_hex']} (want 0xe9)  rel32={h['rel32_hex']} (want {h['exp_rel32_hex']})")
    for bh in BYPASS_HOOKS:
        print(f"  [BYPS] {bh['name']:<36s} rva={bh['rva']}  (bypassed: {bh['reason'][:60]}...)")

    print()
    print(f"all {n_observed} observed hooks installed: {all_installed}")
    print()

    # Observation window — NO Interceptor; alive-check only
    print(f"=== observation window ({args.seconds}s, no Interceptor) ===")
    obs_start = time.time()
    alive_throughout = True

    while time.time() - obs_start < args.seconds:
        elapsed = int(time.time() - obs_start)
        alive = psutil.pid_exists(pid)
        if not alive:
            print(f"  t={elapsed:>3d}s  PROCESS DIED")
            alive_throughout = False
            break
        print(f"  t={elapsed:>3d}s  process alive  pid={pid}")
        time.sleep(2)

    final_alive = psutil.pid_exists(pid)
    print()
    print(f"alive throughout: {alive_throughout}")
    print(f"alive at end:     {final_alive}")

    # Final byte-state poll
    final_poll = None
    if final_alive:
        try:
            final_poll = script.exports_sync.poll()
        except Exception as e:
            print(f"  final poll failed: {e}")

    # Shutdown
    try: session.detach()
    except Exception: pass

    exit_method = 'unknown'
    exit_code   = None
    if final_alive:
        target_hwnd = hwnd or find_main_window(pid)
        if target_hwnd:
            user32.PostMessageW(target_hwnd, WM_CLOSE, 0, 0)
            try:
                exit_code = proc.wait(timeout=8)
                exit_method = 'wm_close'
            except subprocess.TimeoutExpired:
                try: proc.kill()
                except Exception: pass
                try: exit_code = proc.wait(timeout=3)
                except Exception: pass
                exit_method = 'terminate_after_wm_close_timeout'
        else:
            try: proc.kill()
            except Exception: pass
            try: exit_code = proc.wait(timeout=3)
            except Exception: pass
            exit_method = 'terminate_no_hwnd'
    else:
        try: exit_code = proc.wait(timeout=1)
        except Exception: pass
        exit_method = 'died_during_observation'

    # Build install map for 28 verified hooks
    install_map = {h['name']: (h['opcode_ok'] and h['rel32_ok']) for h in post['hooks']}
    qualifies_global = all_installed and alive_throughout and final_alive

    # Write CSV — observed hooks + bypass hooks
    with open(CSV_OUT, 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f)
        w.writerow(['rva', 'export', 'name', 'install_ok', 'alive_throughout',
                    'qualifies_c4', 'canonical_scenario', 'notes'])
        for h in HOOKS:
            name       = h['name']
            install_ok = install_map.get(name, False)
            qualifies  = install_ok and qualifies_global
            w.writerow([h['rva'], h['export'], name, install_ok,
                        alive_throughout, qualifies,
                        f'boot_main_menu_idle_{args.seconds}s', ''])
        # Bypass hooks — excluded from C4 this run
        for bh in BYPASS_HOOKS:
            w.writerow([bh['rva'], bh['name'], bh['name'],
                        False, False, False,
                        'bypassed', bh['reason']])
    print(f"\nCSV written: {CSV_OUT}")

    # Write text report
    lines = [
        '=== observe_c4_save_hooks ===',
        f'scenario:           boot_main_menu_idle_{args.seconds}s (no Interceptor)',
        f'observe_seconds:    {args.seconds}',
        f'boot_wait:          {args.boot_wait}',
        f'hooks_observed:     {n_observed} save hooks',
        f'hooks_bypassed:     {n_bypassed} crashers (not C4 candidates)',
        f'all_{n_observed}_installed: {all_installed}',
        f'alive_throughout:   {alive_throughout}',
        f'alive_at_end:       {final_alive}',
        f'exit_method:        {exit_method}',
        f'exit_code:          {exit_code}',
        '',
        f'asi_module_base:    {post["module_base"]}',
        f'asi_module_size:    {post["module_size"]}',
        '',
        f'install verification (E9 + rel32) — {n_observed} save hooks:',
    ]
    for h in post['hooks']:
        ok = h['opcode_ok'] and h['rel32_ok']
        lines.append(f"  [{'OK  ' if ok else 'FAIL'}] {h['name']:<36s} rva={h['rva']}")
        if not ok:
            lines.append(f"         opcode={h['opcode_hex']} (want 0xe9)  rel32={h['rel32_hex']} (want {h['exp_rel32_hex']})")
    lines.append('')
    lines.append(f'bypassed hooks ({n_bypassed} — excluded from C4):')
    for bh in BYPASS_HOOKS:
        lines.append(f"  [BYPS] {bh['name']:<36s} rva={bh['rva']}")
        lines.append(f"         reason: {bh['reason']}")

    if final_poll:
        lines += ['', 'final byte state (after observation window):']
        for name, bs in final_poll.items():
            still = bs['opcode'] == '0xe9'
            lines.append(f"  [{'OK  ' if still else 'FAIL'}] {name:<36s} opcode={bs['opcode']}  bytes={bs['bytes']}")

    lines += [
        '',
        f'qualifying for C3->C4 ({n_observed} save hooks):',
        f'  install_ok=True AND alive_throughout=True AND alive_at_end=True',
        f'  -> qualifies: {qualifies_global}',
        '',
        'C4 evidence note:',
        f'  {n_observed} save hooks E9+rel32 verified at each RVA during canonical main-menu scenario.',
        '  No Interceptor used (per CLAUDE.md safe-observation rule).',
        f'  Alive throughout {args.seconds}s observation = no crash regression.',
        '  Bypass hooks (not observed): crash-isolated separately; require source fixes.',
    ]
    if qualifies_global:
        lines.append(f'  -> {n_observed} save hooks qualify for C3->C4 promotion.')
    else:
        lines.append('  Not all conditions met; check alive_throughout above.')

    TXT_OUT.write_text('\n'.join(lines), encoding='utf-8')
    print(f"Report written: {TXT_OUT}")

    overall = 'PASS' if qualifies_global else 'PARTIAL'
    print(f"\nOVERALL: {overall}")
    return 0 if overall == 'PASS' else 1


if __name__ == '__main__':
    sys.exit(main())
