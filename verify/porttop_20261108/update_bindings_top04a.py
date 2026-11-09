"""PORT-TOP-04A: hardware gate 0x00493540 -> 0x00495150 and exit 0x00493560 -> 0x004954f0 in BINDINGS.tsv (one row per
RVA, idempotent). usage: py -3.12 verify/porttop_20261108/update_bindings_top04a.py
"""
import os

TSV = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'BINDINGS.tsv')
G = 'Match/PortTop_<rva>.cpp (gen_top04a_tus.py)'
I = 'Match/PortTop_InputPort.cpp'
T = 'PORT-TOP-04A: '

UPD = {
    0x00493540: ('thunk_LaunchLangGate', 'matched', 'thunk_LaunchLangGate (via PortTop_Bound_00493540)', T + 'Match/MatchS15_00493540.cpp, MASHED_STANDALONE binds 0x00495150 by name; the bool result is widened to int for WinMain', 1, 0x00492370),
    0x00493560: ('thunk_FUN_004954f0', 'matched', 'thunk_HwExitDispatch (via PortTop_Bound_00493560)', T + 'Match/MatchS15_00493560.cpp, MASHED_STANDALONE binds 0x004954f0 by name', 1, 0x00492370),
    0x00492770: ('MainLoopInit', 'matched', 'MainLoopInit (via PortTop_MainLoopInit)', T + 'Match/MatchS15_00492770.cpp called directly; the SEH guard is gone (callees bound by PORT-TOP-01, QPF stored by the gate first)', 1, 0x00492290),
    0x00495150: (None, 'matched', 'HwInitGate_00495150', T + G.replace('<rva>', '00495150') + ', re/match/s16/00495150.c 100%', 2, 0x00493540),
    0x00495120: ('TimerQpfStore', 'matched', 'TimerQpfStore', T + 'Match/MatchS16_00495120.cpp; on the knob path it runs before MainLoopInit, as in the original, so TOP-01\'s 0x004950b0 stand-in finds QPF stored and passes through', 3, 0x00495150),
    0x004950b0: ('QpcTimeScaledTo3Mhz', 'stand-in', 'MatchS16_00495110_Call004950b0', T + 'retired on the knob path (QPF is stored by 0x00495120 before 0x00495110 runs; the stand-in\'s store branch does not fire); still needed on the default path, whose boot chain calls MainLoopInit without the gate. Retire with the default flip', 3, 0x00495110),
    0x004955b0: ('DInputInitPredicate', 'matched', 'DInputInitPredicate', T + 'Match/MatchS16_004955b0.cpp, now also in the exe list', 3, 0x00495150),
    0x00495530: ('CreateDInputObject', 'matched', 'CreateDInputObject', T + 'Match/MatchS16_00495530.cpp, now also in the exe list; MASHED_STANDALONE binds 0x0049b300 / 0x004987b0 by name', 4, 0x004955b0),
    0x0049b300: ('DirectInput8Create', 'port', 'PortTop_DirectInput8Create_0049b300', T + I + ': the dinput8 export (import thunk in the original)', 5, 0x00495530),
    0x004987b0: (None, 'matched', 'DbgPrintf_004987b0', T + G.replace('<rva>', '004987b0') + '; vsprintf 0x004a42c5 -> CRT', 5, 0x00495530),
    0x004a42c5: ('vsprintf', 'crt', 'PortTop_Crt_004a42c5', T + I, 6, 0x004987b0),
    0x004960a0: ('FUN_004960a0', 'port', 'PortTop_Port_004960a0', T + I + ': port body from the decomp (no matched source)', 3, 0x00495150),
    0x00496040: (None, 'matched', 'JoypadEnumerate_00496040', T + G.replace('<rva>', '00496040'), 4, 0x004960a0),
    0x00495520: ('Get771e78', 'matched', 'Get771e78', T + 'Match/MatchS16_00495520.cpp, now also in the exe list', 4, 0x004960a0),
    0x00495ee0: ('FUN_00495ee0', 'missing', 'PortTop_Missing_00495ee0', T + I + ': EnumDevices callback, no matched source; creates no joypad device, returns DIENUM_CONTINUE (0x00772fac stays 0)', 5, 0x00496040),
    0x004963b0: ('FUN_004963b0', 'port', 'PortTop_Port_004963b0', T + I + ': port body from the decomp (no matched source)', 3, 0x00495150),
    0x00496320: (None, 'matched', 'KeyboardDeviceCreate_00496320', T + G.replace('<rva>', '00496320') + '; GUID 0x005d09ec seeded from the original, DIDATAFORMAT 0x005d0974 = the exe\'s c_dfDIKeyboard (header checked equal)', 4, 0x004963b0),
    0x00498510: (None, 'missing', 'PortTop_Missing_00498510', T + I + ': C2 source 95.3%; its file-static callees 0x00497190 / 0x004971b0 have an asi-only wrapper, so it needs one shared TU first. No exe reader of 0x007e95c0 / 0x00773118', 3, 0x00495150),
    0x004954f0: ('HardwareExitApplication', 'matched', 'HardwareExitApplication', T + G.replace('<rva>', '004954f0'), 2, 0x00493560),
    0x00498bf0: ('DisplayActiveFlagGet', 'matched', 'DisplayActiveFlagGet', T + 'Match/MatchS16_00498bf0.cpp', 3, 0x004954f0),
    0x00498b60: (None, 'matched', 'FreeTables_00498b60', T + G.replace('<rva>', '00498b60') + '; _free -> CRT free', 3, 0x004954f0),
    0x004963d0: ('thunk_FUN_00496370', 'port', 'PortTop_Port_004963d0', T + I + ': thunk (no matched source)', 3, 0x004954f0),
    0x00496370: (None, 'matched', 'KeyboardRelease_00496370', T + G.replace('<rva>', '00496370'), 4, 0x004963d0),
    0x00496010: (None, 'matched', 'JoypadsDestroy_00496010', T + G.replace('<rva>', '00496010') + '; 0x004b6520 -> ZeroFillWrapper through a pointer overload', 3, 0x004954f0),
    0x00495e80: (None, 'matched', 'JoypadsRelease_00495e80', T + G.replace('<rva>', '00495e80'), 4, 0x00496010),
    0x004955c0: ('thunk_FUN_00495580', 'matched', 'Thunk_004955c0', T + G.replace('<rva>', '004955c0'), 3, 0x004954f0),
    0x00495580: (None, 'matched', 'DInputRelease_00495580', T + G.replace('<rva>', '00495580'), 4, 0x004955c0),
}


def main():
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in UPD.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    from collections import Counter
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
