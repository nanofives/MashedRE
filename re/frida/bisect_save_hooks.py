# Binary-search crash isolation for save hooks.
# Installs all save hooks via InjectHooks, then restores a configurable subset
# to find which save hook(s) cause the t=10s crash.
# Usage: py -3.12 re/frida/bisect_save_hooks.py --keep-range 0 14
import argparse, json, os, pefile, subprocess, sys, time
from pathlib import Path
import frida, psutil, ctypes
from ctypes import wintypes

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'

user32 = ctypes.windll.user32
SW_SHOWMINNOACTIVE = 7
si_base = subprocess.STARTUPINFO()
si_base.dwFlags    |= subprocess.STARTF_USESHOWWINDOW
si_base.wShowWindow = SW_SHOWMINNOACTIVE

# All 29 save hook RVAs (in order)
ALL_RVAS = [
    0x00404e50, 0x00404e80, 0x00404ee0, 0x00404f50, 0x00404f80,
    0x004099a0, 0x004099e0, 0x0040dd60, 0x0040de00, 0x0042a920,
    0x00430290, 0x004963e0, 0x00496400, 0x00498910, 0x00498950,
    0x004989b0, 0x00498d20, 0x00498d60, 0x00498f60, 0x00499170,
    0x004991f0, 0x00499740, 0x004a4541, 0x004b3b70, 0x004b3bb0,
    0x004cbe80, 0x00550980, 0x00550b00, 0x00550bc0
]

NAMES = [
    'SaveLoad', 'DeserializeFromBuffer', 'SerializeToBuffer', 'SaveWrite', 'SaveFileExists',
    'AutosaveTrigger', 'SaveStatusClear', 'GuardConcludedAndP1Won', 'ThunkReplaySave',
    'PostTrophyEvent', 'ChampionshipComplete', 'ConfigLogError', 'ConfigLogDebug',
    'ConfigFilenameGet', 'ConfigLoad', 'ConfigSave', 'ReadModeFromCombo_j5',
    'PopulateModeCombo_s2', 'VideoDialogInit_i3', 'SubsystemSelectionChanged_i3',
    'VideoSettingsDlgProc_s2', 'SetControlTextFromResource_i3', 'FsopenSafe',
    'FileReadWrapper_i3', 'FileWriteWrapper_i3', 'RwStreamWrite_s2',
    'VfsStreamRead', 'VfsFileExists', 'VfsStreamGetType',
]

AGENT_TMPL = r"""
'use strict';
const ASI_PATH   = $ASI_PATH$;
const RESTORE_MAP = $RESTORE_MAP$;

Module.load(ASI_PATH);
const m = Process.findModuleByName('mashed_re_dev.asi');
if (!m) { send({t:'error', msg:'ASI not found after load'}); throw new Error(); }
const inj = m.findExportByName('InjectHooks');
new NativeFunction(inj, 'void', [], 'mscdecl')();
send({t:'injected'});

// Restore bytes for hooks NOT in the keep-set
let restored = 0;
for (const [rva_str, bytes] of Object.entries(RESTORE_MAP)) {
    const p = ptr(rva_str);
    Memory.protect(p, 8, 'rwx');
    for (let i = 0; i < bytes.length; i++) p.add(i).writeU8(bytes[i]);
    restored++;
}
send({t:'restored', count: restored});
rpc.exports = {};
"""


def get_orig_bytes(rva):
    pe = pefile.PE(str(MASHED_EXE))
    ib  = pe.OPTIONAL_HEADER.ImageBase
    off = pe.get_offset_from_rva(rva - ib)
    return list(pe.__data__[off:off+8])


def run_test(keep_indices, seconds=12, boot_wait=8.0):
    """Run game with only keep_indices hooks active. Return True if stable."""
    keep_set = set(keep_indices)
    restore_map = {}
    for i, rva in enumerate(ALL_RVAS):
        if i not in keep_set:
            restore_map[hex(rva)] = get_orig_bytes(rva)

    agent = (AGENT_TMPL
             .replace('$ASI_PATH$',    json.dumps(str(ASI_PATH).replace('\\', '\\\\')))
             .replace('$RESTORE_MAP$', json.dumps(restore_map)))

    env = {k: v for k, v in os.environ.items()}
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        startupinfo=si_base, creationflags=0x00000200|0x00000008)
    pid = proc.pid

    time.sleep(boot_wait)
    if not psutil.pid_exists(pid):
        try: proc.kill()
        except: pass
        return False

    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f"  attach failed: {e}")
        try: proc.kill()
        except: pass
        return False

    done = [False]
    errors = [False]
    def on_msg(msg, data):
        p = msg.get('payload', {})
        if msg['type'] == 'error':
            errors[0] = True; done[0] = True; return
        t = p.get('t')
        if t == 'error': errors[0] = True; done[0] = True
        elif t == 'restored': done[0] = True

    script = session.create_script(agent)
    script.on('message', on_msg)
    script.load()
    t0 = time.time()
    while not done[0] and time.time()-t0 < 15: time.sleep(0.1)
    if errors[0]:
        try: session.detach()
        except: pass
        try: proc.kill()
        except: pass
        return False

    stable = True
    for i in range(seconds):
        time.sleep(1)
        if not psutil.pid_exists(pid):
            stable = False
            break

    try: session.detach()
    except: pass
    try: proc.kill()
    except: pass
    try: proc.wait(timeout=3)
    except: pass
    return stable


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--keep-range', type=int, nargs=2, metavar=('LO','HI'),
                        help='Keep hooks LO..HI (0-based, inclusive)')
    parser.add_argument('--keep-indices', type=int, nargs='*',
                        help='Keep specific hook indices (empty = restore all)')
    parser.add_argument('--seconds', type=int, default=12)
    parser.add_argument('--boot-wait', type=float, default=8.0)
    parser.add_argument('--auto', action='store_true',
                        help='Automatic binary search')
    args = parser.parse_args()

    print(f"Save hooks ({len(ALL_RVAS)} total):")
    for i, (rva, name) in enumerate(zip(ALL_RVAS, NAMES)):
        print(f"  [{i:2d}] {hex(rva)}  {name}")
    print()

    if args.auto:
        # Automatic binary search
        # Start: keep all — is it stable?
        keep = list(range(len(ALL_RVAS)))
        print("Testing with ALL hooks active...")
        if run_test(keep, args.seconds, args.boot_wait):
            print("STABLE with all hooks — no crash!")
            return
        print("CRASH with all hooks — bisecting...")

        # Find the crashing hook(s) via linear elimination
        bad = []
        candidate_set = list(range(len(ALL_RVAS)))
        for i in candidate_set:
            test_set = [x for x in candidate_set if x != i]
            name = NAMES[i]
            print(f"  Testing without hook [{i}] {name}...")
            if run_test(test_set, args.seconds, args.boot_wait):
                print(f"  -> STABLE without [{i}] {name} — THIS IS THE CRASH HOOK")
                bad.append(i)
                candidate_set = test_set  # remove it from candidate set
                break
            else:
                print(f"  -> still crashes")
        if bad:
            print(f"\nCrash hooks identified: {[NAMES[i] for i in bad]}")
        else:
            print("Could not isolate — may need multi-hook interaction")
        return

    if args.keep_range:
        lo, hi = args.keep_range
        keep = list(range(lo, hi+1))
    elif args.keep_indices is not None:
        keep = args.keep_indices  # may be empty list = restore all
    else:
        parser.error("specify --keep-range, --keep-indices, or --auto")

    active = [NAMES[i] for i in keep]
    print(f"Keeping {len(keep)} hooks: {active}")
    print(f"Testing for {args.seconds}s at boot_wait={args.boot_wait}s...")
    stable = run_test(keep, args.seconds, args.boot_wait)
    print(f"\nResult: {'STABLE' if stable else 'CRASH'}")


if __name__ == '__main__':
    main()
