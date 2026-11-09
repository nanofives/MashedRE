"""PORT-TOP-02: write verify/porttop_20261108/BINDINGS.tsv (the MASHED_ORIG_TOP knob path's callee bindings).

Columns: rva, name, kind, exe_symbol, note, level, caller.
kind:
  matched         a byte-matched source compiled into the exe and bound by name
  port            an existing exe port body bound by name (CRT rows are library code)
  stand-in        the hand top level's piece doing that job (exe_main.cpp HandTop_*)
  missing         no exe body and no hand equivalent: a logged stub returning what the reference returns
  under-stand-in  callee of a node bound to a stand-in/missing stub: not reached on the knob path. Note = its exe state
  rw              RenderWare core entry point the original bodies call (PORT-TOP-03 input; L-98 keeps librw)
Levels count from the two roots, WinMain 0x00492370 and the run function 0x00492290 (both level 0). The bound rows (levels 0-1 and the compiled level 2-3 rows) are listed
by hand below; the under-stand-in rows (level 2) and the rw rows (levels 1-3) come from log/xref_mashed.json (direct
calls) and hooks.csv (name, subsystem, port, exe_file).

usage: py -3.12 verify/porttop_20261108/gen_bindings.py
"""
import collections
import csv
import json
import os

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))
XREF = os.path.join(os.environ.get('MASHED_ROOT', r'C:\Users\maria\Desktop\Proyectos\Mashed'), 'log', 'xref_mashed.json')
OUT = os.path.join(os.path.dirname(__file__), 'BINDINGS.tsv')

P = 'mashedmod/src/mashed_re/'
BOUND = [
    # rva, kind, exe_symbol, note, level, caller
    (0x00492370, 'matched', 'Orig_00492370', 're/match/s15/00492370.c (100%), Match/PortTop_00492370.cpp. Entered from the exe WinMain when MASHED_ORIG_TOP=1', 0, 0),
    (0x00492290, 'matched', 'Orig_00492290', 're/match/s15/00492290.c (100%), Match/PortTop_00492370.cpp', 0, 0x00492370),
    (0x00499ba0, 'matched', 'Orig_00499ba0', 're/match/s16/00499ba0.c (h6), Match/PortTop_00499ba0.cpp. Creates the window (class MASHED, WndProc 0x00499820)', 1, 0x00492370),
    (0x00493900, 'matched', 'Orig_00493900', 're/match/s15/00493900.c, best 66.9% (hooks C2), the only source. Match/PortTop_00493900.cpp. Parses -vs/-cs/-l switches', 1, 0x00492370),
    (0x00493540, 'missing', 'PortTop_Missing_00493540', 'exe body thunk_LaunchLangGate (MatchS15_00493540.cpp) forwards to raw 0x00495150, which faults in the exe. 0x00495150 and its 6 callees have no exe body, no hand equivalent. Stub returns DAT_007719e4 == 0, as 0x00495150 does', 1, 0x00492370),
    (0x004924f0, 'stand-in', 'PortTop_StandIn_004924f0', 'job done by the hand boot chain steps 1-3 (ExecuteFrontendBootChain) inside the 0x00402750 stand-in. exe body Boot/SubsystemInit.cpp is a tunnel to the original address', 1, 0x00492370),
    (0x00492270, 'stand-in', 'PortTop_StandIn_00492270 -> HandTop_InitDevice', 'InitD3D9 + librw RaceSubmit_Init on the window 0x00499ba0 made. exe body SubsystemInit (MatchS15_00492270.cpp) forwards to raw 0x00493710 / 0x004921d0 / 0x00428590', 1, 0x00492370),
    (0x005c9d00, 'port', 'GetRaceEndTrigger', P + 'Util/GameStateGetters.cpp', 1, 0x00492370),
    (0x004996f0, 'matched', 'WindowShow', P + 'Match/MatchS16_004996f0.cpp', 1, 0x00492370),
    (0x004963e0, 'stand-in', 'PortTop_CfgLogError', 'exe body ConfigLogErrorCore is file-internal (Save/SettingsConfig.cpp, PORT-SAVE-01). Reference is a no-op under disable_log. Stub mirrors the text into mashed_re.log', 1, 0x00492370),
    (0x00402750, 'stand-in', 'PortTop_StandIn_00402750 -> HandTop_InitRest', 'data wedge, boot chain B7/B10, DirectInput, dsound guard, boot movies, audio, frontend assets. PORT-TOP-04 replaces it', 1, 0x00492370),
    (0x00402a40, 'stand-in', 'PortTop_StandIn_00402a40 -> HandTop_AppDestroy', 'Audio::Shutdown, ShutdownDirectInput, small.mpg close. PORT-TOP-04 replaces it', 1, 0x00492370),
    (0x004c5930, 'rw', 'PortTop_Rw_004c5930', 'RW texture-dictionary destroy (its source walks RwTexDictionary). Logged stub. Not reached: DAT_0077195c stays 0 because 0x004921d0 never runs in the exe. PORT-TOP-03', 1, 0x00492370),
    (0x00493550, 'stand-in', 'PortTop_StandIn_00493550 -> HandTop_DeviceShutdown', 'ShutdownD3D9 for 0x004938c0 (RW engine stop/close/term). exe body thunk_EngineStopDispatch forwards to raw 0x004938c0', 1, 0x00492370),
    (0x00493560, 'missing', 'PortTop_Missing_00493560', '0x004954f0 HardwareExitApplication and its 5 callees have no exe body. exe body thunk_HwExitDispatch forwards raw. Stub returns 0 like 0x004954f0', 1, 0x00492370),
    (0x00499cc0, 'matched', 'Orig_00499cc0', 're/match/s16/00499cc0.c, Match/PortTop_Leaves.cpp. DestroyWindow + returns the pump MSG wParam', 1, 0x00492370),
    (0x00492770, 'matched', 'PortTop_MainLoopInitGuarded -> MainLoopInit', P + 'Match/MatchS15_00492770.cpp, SEH-guarded: its 5 callees forward raw and fault (PORT-TOP-01). The hand boot chain also calls it once (step 7)', 1, 0x00492290),
    (0x00499690, 'matched', 'WindowMsgPump', P + 'Match/MatchS16_00499690.cpp. Focus gate kept open (DAT_0077391c=1) as the reference no_focus_pause patch does; MASHED_ORIG_TOP_FOCUSPAUSE=1 keeps WaitMessage', 1, 0x00492290),
    (0x004929d0, 'port', 'PortTop_Port_004929d0 -> GameModeMachine_Tick', P + 'Race/GameModeMachine.cpp wrapper the hand loop calls (no-op unless MASHED_PORTSTATE_GAMEFLOW_FU=1, which runs the matched 0x004929d0)', 1, 0x00492290),
    (0x00492d20, 'matched', 'IntroSplashFrameTickShim', P + 'Match/MatchS15_00492d20.cpp, now also in the exe list (its 0x004967e0 forwarder binds the stand-in in the exe)', 1, 0x00492290),
    (0x00492d30, 'stand-in', 'PortTop_StandIn_00492d30 -> HandTop_FrameDetTick + HandTop_FrameUpdate', 'hand frame update (det frame cap, harness steps, menu nav, input globals, GameFlow_Update). 0x00492d30 dispatches on DAT_00771968, unported tree. PORT-TOP-05', 1, 0x00492290),
    (0x00492e90, 'stand-in', 'PortTop_StandIn_00492e90 -> RenderFrame', 'hand RenderFrame; device loss posts WM_QUIT. PORT-TOP-05', 1, 0x00492290),
    (0x004926c0, 'missing', 'PortTop_Missing_004926c0', 'exe body AudioTickAndAvg (MatchS15_004926c0.cpp) forwards its 5 callees raw (none has an exe body). No-op', 1, 0x00492290),
    (0x00493480, 'missing', 'PortTop_Missing_00493480', 'exe body FpsDiscretise forwards raw to 0x00493390 (no exe body). No-op; hand pacing is the present/limiter in RenderFrame', 1, 0x00492290),
    # level 2/3 under compiled matched bodies
    (0x00499820, 'matched', 'Orig_00499820', 're/match/s16/00499820.c (h6), Match/PortTop_00499820.cpp. Window procedure of the knob window', 2, 0x00499ba0),
    (0x00498bc0, 'stand-in', 'PortTop_Width', 'exe resolution kWidth. exe body VideoGetRenderWidth reads DAT_00616028 = 0 in the exe; the reference returns 640 by fix_camera_res', 2, 0x00499ba0),
    (0x00498bd0, 'stand-in', 'PortTop_Height', 'exe resolution kHeight (see 0x00498bc0)', 2, 0x00499ba0),
    (0x00499890, 'matched', 'Orig_00499890', 're/match/s16/00499890.c (h6), Match/PortTop_00499890.cpp. OS-version log', 2, 0x00499ba0),
    (0x00499730, 'matched', 'BootPtr773818Get', P + 'Match/MatchS16_00499730.cpp, now also in the exe list', 2, 0x00493900),
    (0x004a3ac9, 'port', 'strtok', 'CRT (library row, out of the P-DoD port count)', 2, 0x00493900),
    (0x004b302f, 'port', '_stricmp', 'CRT (library row)', 2, 0x00493900),
    (0x004a2be9, 'port', '__security_check_cookie', 'CRT /GS, emitted by the compiler', 2, 0x00493900),
    (0x004967e0, 'stand-in', 'PortTop_StandIn_004967e0 -> PollKeyboard', 'hand DirectInput keyboard poll. 0x004967e0 is C2, no exe body', 2, 0x00492d20),
    (0x00496490, 'missing', 'PortTop_Missing_00496490', 'log open on WM_CREATE: returns 0, the reference value under disable_log', 3, 0x00499820),
    (0x00496470, 'missing', 'PortTop_Missing_00496470', 'log close on WM_DESTROY: no-op (handle NULL under disable_log)', 3, 0x00499820),
    (0x004960e0, 'stand-in', 'PortTop_StandIn_004960e0 -> Audio::SetOutputActive', 'the hand WndProc job on WM_ACTIVATE', 3, 0x00499820),
    (0x00496400, 'stand-in', 'PortTop_CfgLogDebug', 'exe body ConfigLogDebugCore is file-internal; reference RET under disable_log. Mirrors into mashed_re.log', 3, 0x00499890),
]
MAINLOOP_CALLEES = [0x00495110, 0x0042b920, 0x0042b950, 0x00433240, 0x004c57a0]


def main():
    H = {}
    for r in csv.DictReader(open(os.path.join(W, 'hooks.csv'), encoding='utf-8')):
        try:
            H[int(r['rva'], 16)] = r
        except ValueError:
            pass
    X = json.load(open(XREF))
    calls = lambda n: X.get(str(n), {}).get('calls', [])
    nm = lambda n: H.get(n, {}).get('name', '?')
    is_rw = lambda n: ('renderware' in H.get(n, {}).get('subsystem', '').lower() or nm(n)[:2] in ('Rw', 'Rp', 'Rt'))
    rows, have = [], set()
    for rva, kind, sym, note, lvl, caller in BOUND:
        rows.append((rva, nm(rva), kind, sym, note, lvl, caller))
        have.add(rva)
    for c in MAINLOOP_CALLEES:
        r = H.get(c, {})
        rows.append((c, nm(c), 'rw' if is_rw(c) else 'missing', 'MatchS15_00492770_Call%08x (raw forwarder)' % c,
                     'MainLoopInit callee, forwards to the raw address and faults in the exe; PORT-TOP-01. exe port=%s %s'
                     % (r.get('port', ''), r.get('exe_file', '')), 2, 0x00492770))
        have.add(c)
    stand = {r[0] for r in BOUND if r[1] in ('stand-in', 'missing', 'rw')}
    lvl1 = [r for r in BOUND if r[4] == 1]
    for par in [r[0] for r in lvl1]:
        for c in calls(par):
            if c in have:
                continue
            have.add(c)
            r = H.get(c, {})
            port, exe = r.get('port', '') or 'none', r.get('exe_file', '')
            if is_rw(c):
                kind, sym, why = 'rw', '', 'RW core called by 0x%08x' % par
            elif par in stand:
                kind, sym, why = 'under-stand-in', '', 'parent bound to a stand-in, not reached'
            elif port in ('body', 'called') and exe:
                kind, sym, why = 'port', '(inside the parent exe body)', 'bound by the parent\'s exe body'
            else:
                kind, sym, why = 'missing', '', 'parent runs on the knob path, callee has no exe body'
            rows.append((c, nm(c), kind, sym, '%s. exe port=%s %s; hooks %s'
                         % (why, port, exe, r.get('confidence', '')), 2, par))
    # rw entry points one level further down (level 3), the PORT-TOP-03 inventory
    lvl2 = [r[0] for r in rows if r[5] == 2]
    for par in lvl2:
        for c in calls(par):
            if c in have or not is_rw(c):
                continue
            have.add(c)
            r = H.get(c, {})
            rows.append((c, nm(c), 'rw', '', 'RW core called by level-2 node 0x%08x. exe port=%s %s'
                         % (par, r.get('port', '') or 'none', r.get('exe_file', '')), 3, par))
    with open(OUT, 'w', newline='\n', encoding='utf-8') as f:
        f.write('rva\tname\tkind\texe_symbol\tnote\tlevel\tcaller\n')
        for rva, n, kind, sym, note, lvl, caller in rows:
            f.write('0x%08x\t%s\t%s\t%s\t%s\t%d\t%s\n' % (rva, n, kind, sym, note, lvl, '0x%08x' % caller if caller else ''))
    cnt = collections.Counter((r[5] if r[5] < 3 else 3, r[2]) for r in rows)
    print('wrote %d rows -> %s' % (len(rows), os.path.normpath(OUT)))
    for k in sorted(cnt):
        print('  level %s %-15s %d' % (k[0], k[1], cnt[k]))


if __name__ == '__main__':
    main()
