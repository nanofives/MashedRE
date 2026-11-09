"""PORT-TOP-03: apply the RW API binding layer to verify/porttop_20261108/BINDINGS.tsv (one row per RVA).

Kinds added by this step:
  bound   an RW 3.x core entry point bound to a librw-backed exe function (LibRw/RwApiBind.cpp)
  shim    an RW core call with no librw equivalent: an explicit shim in LibRw/RwApiBind.cpp (keeps the return value)
  crt     a CRT call bound to an exe stand-in
Rows of kind rw that stay rw are not on the knob path (their caller is still a stand-in) or have an unconfirmed RW 3.x
identity; each gets a note saying which.

usage: py -3.12 verify/porttop_20261108/update_bindings_top03.py   (idempotent)
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
B = 'LibRw/RwApiBind.cpp'
P = 'Match/PortTop_00493710.cpp'

# rva -> (name or None to keep, kind, exe_symbol, note, level, caller)
UPD = {
    # level 1 (WinMain 0x00492370)
    0x00492270: (None, 'matched', 'SubsystemInit (via PortTop_Bound_00492270)', 'Match/MatchS15_00492270.cpp; its MASHED_STANDALONE branch binds 0x00493710 to the matched bring-up (RW on librw) and 0x004921d0 / 0x00428590 to missing stubs. PORT-TOP-03', 1, 0x00492370),
    0x004c5930: (None, 'bound', 'MashedRw_TexDictionaryDestroy_004c5930', B + ': RwTexDictionaryDestroy -> rw::TexDictionary::destroy. Not reached: DAT_0077195c stays 0 (0x004921d0 not run). PORT-TOP-03', 1, 0x00492370),
    0x00493550: (None, 'matched', 'thunk_EngineStopDispatch (via PortTop_Bound_00493550)', 'Match/MatchS15_00493550.cpp; MASHED_STANDALONE binds 0x004938c0 by name to SoftwareTidyUpBeforeExiting. PORT-TOP-03', 1, 0x00492370),
    # level 2
    0x00493710: (None, 'matched', 'Orig_00493710 (via PortTop_Bound_00493710)', P + ' compiles re/match/s15/00493710.c (h8 best 94.2%, the only source). RW engine bring-up. PORT-TOP-03', 2, 0x00492270),
    0x004921d0: (None, 'missing', 'PortTop_Missing_004921d0', P + ': not run. exe body DisplayInit (MatchS15_004921d0.cpp) forwards its 12 callees to raw addresses. PORT-TOP-04', 2, 0x00492270),
    0x00428590: (None, 'missing', 'PortTop_Missing_00428590', P + ': not run, returns 1. exe body ViewportInit (MatchS06_00428590.cpp) forwards 5 of 6 callees to raw addresses. PORT-TOP-04', 2, 0x00492270),
    0x004938c0: (None, 'matched', 'SoftwareTidyUpBeforeExiting', 'Match/MatchS15_004938c0.cpp; MASHED_STANDALONE binds its 5 RW calls to ' + B + '. PORT-TOP-03', 2, 0x00493550),
    # level 3: callees of 0x00493710 (RW core)
    0x004c32b0: (None, 'bound', 'MashedRw_EngineInit_004c32b0', B + ': RwEngineInit -> rw::Engine::init', 3, 0x00493710),
    0x004c30b0: (None, 'bound', 'MashedRw_EngineOpen_004c30b0', B + ': RwEngineOpen(params.displayID = hwnd) -> rw::Engine::open + last subsystem', 3, 0x00493710),
    0x004c2fb0: (None, 'bound', 'MashedRw_EngineStart_004c2fb0', B + ': RwEngineStart -> exe InitD3D9 device, adopted, rw::Engine::start, librw race renderer camera / state block', 3, 0x00493710),
    0x004c3040: (None, 'bound', 'MashedRw_EngineClose_004c3040', B + ': RwEngineClose -> rw::Engine::close', 3, 0x00493710),
    0x004c3270: (None, 'bound', 'MashedRw_EngineTerm_004c3270', B + ': RwEngineTerm -> rw::Engine::term', 3, 0x00493710),
    0x004c2f00: (None, 'bound', 'MashedRw_EngineGetCurrentMode_004c2f00', B + ': RwEngineGetCurrentMode -> rw::Engine::getCurrentVideoMode. Called by 0x00493710 (knob path) and 0x00402750 (stand-in). The exe body RwEngineGetCurrentMode (MatchS19_004c2f00.cpp) forwards raw to 0x004c2c90 and is not used', 2, 0x00402750),
    0x004c2ed0: (None, 'bound', 'MashedRw_EngineGetModeInfo_004c2ed0', B + ': RwEngineGetModeInfo -> rw::Engine::getVideoModeInfo (refRate / format written 0). Called by 0x00493710 (knob path) and 0x00402750 (stand-in)', 2, 0x00402750),
    0x004c5c80: (None, 'bound', 'MashedRw_TexDictionarySetCurrent_004c5c80', B + ': RwTexDictionarySetCurrent -> rw::TexDictionary::setCurrent (0x00493710 passes 0)', 3, 0x00493710),
    0x00550350: (None, 'shim', 'MashedRw_RtFSManagerOpen_00550350', B + ': RtFSManagerOpen, no RtFSManager in librw; returns 1', 3, 0x00493710),
    0x00550390: ('RtFSManagerClose', 'shim', 'MashedRw_RtFSManagerClose_00550390', B + ': no-op (no RtFSManager in librw). Also called by 0x004938c0', 3, 0x00493710),
    0x004c9f50: (None, 'shim', 'MashedRw_D3D9Set7d4134_004c9f50', B + ': keeps the value; RW D3D9-driver setting with no librw equivalent', 3, 0x00493710),
    0x004c9eb0: ('DeviceModeBestBelowSet', 'shim', 'MashedRw_D3D9SetRefreshRate_004c9eb0', B + ': keeps the value (0x3c); the exe device is windowed', 3, 0x00493710),
    0x004c9f60: ('FUN_004c9f60', 'shim', 'MashedRw_D3D9Set7d413c_004c9f60', B + ': keeps the value; RW D3D9-driver setting with no librw equivalent', 3, 0x00493710),
    0x004cbc60: (None, 'shim', 'MashedRw_Set7d4598_004cbc60', B + ': keeps the pointer (0x00493700, no exe body), librw not told', 3, 0x00493710),
    0x004cbc70: (None, 'shim', 'MashedRw_Get7d4598_004cbc70', B + ': returns the kept value', 3, 0x00493710),
    0x004cbc80: (None, 'shim', 'MashedRw_Set7d459c_004cbc80', B + ': keeps the pointer (0x004936f0, no exe body), librw not told', 3, 0x00493710),
    0x004cbc90: (None, 'shim', 'MashedRw_Get7d459c_004cbc90', B + ': returns the kept value', 3, 0x00493710),
    0x005584c0: (None, 'shim', 'MashedRw_Alloc7dc8b8_005584c0', B + ': returns 1, no allocation (two RW-heap buffers, no exe reader)', 3, 0x00493710),
    0x0045b350: (None, 'matched', 'RwInitNullStub', 'Match/MatchS09_0045b350.cpp, now also in the exe list (bare RET). Called twice by 0x00493710 and by 0x004924f0', 2, 0x004924f0),
    # level 3: game callees of 0x00493710
    0x005c9d00: (None, 'port', 'GetRaceEndTrigger', 'Util/GameStateGetters.cpp (xor eax,eax; ret). Also a WinMain callee', 1, 0x00492370),
    0x00495780: ('HardwareInstallFileSystem', 'missing', 'PortTop_Missing_00495780', P + ': no-op (0x004955d0 unported; no RtFSManager in librw)', 3, 0x00493710),
    0x00495270: ('HWNDGet', 'matched', 'HWNDGet', 'Match/MatchS16_00495270.cpp, now also in the exe list', 3, 0x00493710),
    0x00499710: ('Global7e9584Get', 'matched', 'Global7e9584Get', 'Match/MatchS16_00499710.cpp, now also in the exe list', 4, 0x00495270),
    0x00493640: ('RenderwareAttachPlugins', 'stand-in', 'MashedRw_AttachPlugins_00493640', B + ': librw plugin registration (RwBridge.cpp RegisterPlugins), between Engine::init and open as RW 3.x requires', 3, 0x00493710),
    0x004951f0: ('HardwareSetVideoMode', 'stand-in', 'PortTop_StandIn_004951f0', P + ': returns 1; the exe InitD3D9 picks the windowed mode inside the RwEngineStart binding', 3, 0x00493710),
    0x00493600: ('FUN_00493600', 'missing', 'PortTop_Missing_00493600', P + ': returns 1 (0x004ce790 and its 4 function arguments unported)', 3, 0x00493710),
    0x004a2cbd: ('printf', 'crt', 'PortTop_Crt_004a2cbd', P + ': CRT printf; the formatted line goes to mashed_re.log', 3, 0x00493710),
    0x004936f0: ('FUN_004936f0', 'missing', 'PortTop_Missing_004936f0', P + ': pointer argument only (not in hooks.csv); the RW shims never call it', 3, 0x00493710),
    0x00493700: ('FUN_00493700', 'missing', 'PortTop_Missing_00493700', P + ': pointer argument only (not in hooks.csv); the RW shims never call it', 3, 0x00493710),
    # level 3: RW callees of 0x004938c0
    0x004c2f60: ('RwEngineStop', 'bound', 'MashedRw_EngineStop_004c2f60', B + ': RwEngineStop -> librw race-renderer scene teardown, rw::Engine::stop, then the exe releases its device (ShutdownD3D9)', 3, 0x004938c0),
    0x00558470: ('sub_00558470', 'shim', 'MashedRw_Free7dc8b8_00558470', B + ': no-op (counterpart of the 0x005584c0 shim)', 3, 0x004938c0),
    # RW callees of 0x004c5930 (librw does the same work inside TexDictionary::destroy)
    0x004c5a60: (None, 'bound', '(inside MashedRw_TexDictionaryDestroy_004c5930)', B + ': per-texture destroy of RwTexDictionaryDestroy; librw TexDictionary::destroy destroys each texture', 2, 0x004c5930),
    0x004d8060: (None, 'bound', '(inside MashedRw_TexDictionaryDestroy_004c5930)', B + ': plugin destructor dispatch; librw runs its plugin destructors inside destroy', 2, 0x004c5930),
}

KEEP_RW = {
    0x004c57a0: 'bound by PORT-TOP-01 (orch/PORT-TOP-01: MashedRw_MatrixCreate_004c57a0 in LibRw/RwBridge.cpp, rw::Matrix::create); on this branch still the raw forwarder until that branch merges',
    0x004c19f0: 'not on the knob path (caller 0x00492e90 is a stand-in). exe port body Boot/FrameDispatch.cpp + MatchS19_004c19f0.cpp (camera vtable slot +0x1c call)',
    0x004cbd30: 'not on the knob path (caller 0x004274e0 under a stand-in). exe port bodies D3d9Render/RwWorldStream.cpp, Render/RwStreamCore.cpp',
    0x00534b60: 'not on the knob path (caller 0x004881d0 under a stand-in). exe port=called Render/RfxEmitter.cpp',
}
DEFAULT_NOTE = 'not bound in PORT-TOP-03: not on the knob path (its caller is still a stand-in or under one) and its RW 3.x identity is not confirmed from its matched source'


def main():
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    seen = {}
    for r in rows:
        seen[int(r[0], 16)] = r
    for rva, (name, kind, sym, note, lvl, caller) in UPD.items():
        if rva in seen:
            r = seen[rva]
            if name:
                r[1] = name
            r[2], r[3], r[4] = kind, sym, note
            r[5], r[6] = str(lvl), '0x%08x' % caller
        else:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, kind, sym, note, str(lvl), '0x%08x' % caller]
            rows.append(r)
            seen[rva] = r
    for r in rows:
        rva = int(r[0], 16)
        if r[2] == 'rw' and rva not in UPD:
            extra = KEEP_RW.get(rva, DEFAULT_NOTE)
            if 'PORT-TOP-03' not in r[4]:
                r[4] = (r[4].rstrip() + ' | PORT-TOP-03: ' + extra).strip()
    # original order kept; new rows appended
    out = [head] + ['\t'.join(r) for r in rows]
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
    from collections import Counter
    print('rows', len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
