"""PORT-OT-BIND-D3D (L-111 step 2): the DirectShow movie glue 0x004944c0 bound on the exe's own graph and device
(Match/PortTop_MovieBind.cpp), and the matched 0x00494a80 compiled in the exe with that binding as its callee.

No-chain convention (HB-GENFILES-NOCONFLICT): only this row's delta, through bindings_rows.apply.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otbindd3d.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'PORT-OT-BIND-D3D: '
EV = ' (verify/portorigtop_20261109/RESULT_PORT-OT-BIND-D3D.md)'

FIXED = {
    0x004944c0: ('FUN_004944c0', 'bound', 'PortTop_Bound_004944c0 (Match/PortTop_MovieBind.cpp)',
                 T + 'DirectShow graph for movie [0x006147c4 + idx*4] by its code (CLSID_FilterGraph, IID_IGraphBuilder / '
                 'IMediaControl / IMediaPosition / IMediaEvent / IBasicAudio GUIDs, AddFilter TEXTURERENDERER, '
                 'AddSourceFilter, Render, Run). On D3d9Render/MpegVideoTexture with the exe device: AddRef\'d '
                 'interfaces, SampleGrabber filter and the one texture published into 0x00771a10..0x00771a34 and the '
                 'size floats 0x007719ec..f8, so the matched 0x004942b0 / 0x00494320 run on live objects. Frames need '
                 'PortTop_MovieBind_Pump (no knob-path caller yet). Checked by MASHED_DBG_MOVIEBIND_SELFTEST' + EV,
                 3, 0x00494a80),
    0x00494a80: ('sub_00494a80', 'under-stand-in', 'FUN_00494a80 (Match/MatchS16_00494a80.cpp, now also in the exe list)',
                 T + 'matched body compiled in the exe; callees bind by name (0x004944c0 -> PortTop_Bound_004944c0, '
                 '0x004c77c0 -> MashedRw_RasterCreate_004c77c0). Its knob-path callers 0x00495350 (piece 2) and '
                 '0x0043dfd0 (under the 0x00492d30 stand-in) do not run it yet; the self-test does' + EV,
                 2, 0x00495350),
}

if __name__ == '__main__':
    apply(T, FIXED, {})
