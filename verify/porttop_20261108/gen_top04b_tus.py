"""PORT-TOP-04B: generate the exe-only TUs that compile DisplayInit 0x004921d0's and ViewportInit 0x00428590's matched
callees with no body in either target, for the MASHED_ORIG_TOP path. One TU per matched source (the sources declare
the same RW callee with different prototypes, e.g. FUN_004c1d30 int / undefined1*, FUN_004c7760 int / void*).

usage: py -3.12 verify/porttop_20261108/gen_top04b_tus.py   (rewrites Match/PortTop_<rva>.cpp)
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
MATCH = os.path.normpath(os.path.join(HERE, '..', '..', 'mashedmod', 'src', 'mashed_re', 'Match'))

RW = {
    'FUN_004c1d30': ('MashedRw_CameraCreate_004c1d30', 'RW: RwCameraCreate -> rw::Camera::create behind an RW-layout record'),
    'FUN_004c0b30': ('MashedRw_FrameCreate_004c0b30', 'RW: RwFrameCreate -> rw::Frame::create'),
    'FUN_004c0740': ('MashedRw_ObjectHasFrameSetFrame_004c0740', 'RW: _rwObjectHasFrameSetFrame -> rw::Camera::setFrame'),
    'FUN_004c77c0': ('MashedRw_RasterCreate_004c77c0', 'RW: RwRasterCreate -> rw::Raster::create'),
    'FUN_004c1c80': ('MashedRw_CameraSetViewWindow_004c1c80', 'RW: RwCameraSetViewWindow -> rw::Camera::setViewWindow'),
    'FUN_004c1a40': ('MashedRw_CameraSetNearClipPlane_004c1a40', 'RW: RwCameraSetNearClipPlane -> rw::Camera::setNearPlane'),
    'FUN_004c1b10': ('MashedRw_CameraSetFarClipPlane_004c1b10', 'RW: RwCameraSetFarClipPlane -> rw::Camera::setFarPlane'),
    'FUN_004c2bf0': ('MashedRw_CameraClone_004c2bf0', 'RW: RwCameraClone -> rw::Camera::clone'),
    'FUN_004c7760': ('MashedRw_RasterSubRaster_004c7760', 'RW: RwRasterSubRaster -> rw::Raster::subRaster'),
    'FUN_004c0c20': ('MashedRw_FrameDestroy_004c0c20', 'RW: RwFrameDestroy -> rw::Frame::destroy'),
    'FUN_004c1cf0': ('MashedRw_CameraDestroy_004c1cf0', 'RW: RwCameraDestroy -> rw::Camera::destroy'),
    'FUN_004c7650': ('MashedRw_RasterDestroy_004c7650', 'RW: RwRasterDestroy -> rw::Raster::destroy'),
    'FUN_004c7730': ('MashedRw_RasterShowRaster_004c7730', 'RW: RwRasterShowRaster -> Present on the adopted device'),
}
GAME = {
    'FUN_00467020': ('Orig_00467020', 'matched, Match/PortTop_00467020.cpp'),
    'FUN_0042d420': ('Orig_0042d420', 'matched, Match/PortTop_0042d420.cpp'),
    'FUN_0042d4a0': ('Orig_0042d4a0', 'matched, Match/PortTop_0042d4a0.cpp'),
    'FUN_0042f530': ('Orig_0042f530', 'matched (C2 source, 96.9%), Match/PortTop_0042f530.cpp'),
    'FUN_0042b8b0': ('ScreenWidthGet', 'matched, Match/MatchS07_0042b8b0.cpp'),
    'FUN_0042b8c0': ('ScreenHeightGet', 'matched, Match/MatchS07_0042b8c0.cpp'),
    'FUN_004671a0': ('BackBuffRasterOpen', 'matched, Match/MatchS11_004671a0.cpp (exe since PORT-TOP-04B)'),
}

# rva -> (source, exe symbol, what it does (mechanical), [callee FUN names])
TUS = {
    '00467110': ('s11/00467110.c', 'Orig_00467110',
                 'static 0x004670a0: camera + frame + rasters type 2 / 1 into DAT_006905b0; view window 0.8 / 0.8, near 0.1, far 180; clone into DAT_006905b4 with a new frame DAT_006905b8',
                 ['FUN_00467020', 'FUN_004c0740', 'FUN_004c0b30', 'FUN_004c1d30', 'FUN_004c77c0', 'FUN_004c1a40',
                  'FUN_004c1b10', 'FUN_004c1c80', 'FUN_004c2bf0']),
    '00467020': ('s10/00467020.c', 'Orig_00467020',
                 'destroys a camera: frame (+0x04), rasters (+0x60, +0x64), the camera',
                 ['FUN_004c0c20', 'FUN_004c1cf0', 'FUN_004c7650']),
    '0042d420': ('s07/0042d420.c', 'Orig_0042d420',
                 'camera + frame + raster type 2 (and type 1 when param_3) of size param_1 x param_2',
                 ['FUN_00467020', 'FUN_004c0740', 'FUN_004c0b30', 'FUN_004c1d30', 'FUN_004c77c0']),
    '0042d560': ('s07/0042d560.c', 'Orig_0042d560',
                 'DAT_0067f18c = 0x0042d420(0, 0, 1), near 0x3dcccccd, far 0x43340000, then 0x0042d4a0; returns 1',
                 ['FUN_0042d420', 'FUN_0042d4a0', 'FUN_004c1a40', 'FUN_004c1b10']),
    '0042d4a0': ('s07/0042d4a0.c', 'Orig_0042d4a0',
                 'view window 0.8 / 0.8 on DAT_0067f18c, its two rasters made sub rasters of 0x004671a0(-1)\'s',
                 ['FUN_0042b8b0', 'FUN_0042b8c0', 'FUN_004671a0', 'FUN_004c1c80', 'FUN_004c7760']),
    '0042f660': ('s07/0042f660.c', 'DefaultViewportCameraInit',
                 'DAT_0067f190 = 0x0042d420(0, 0, 1), near 0x3dcccccd, far 0x43340000, then 0x0042f530',
                 ['FUN_0042d420', 'FUN_0042f530', 'FUN_004c1a40', 'FUN_004c1b10']),
    '0042f530': ('s07/0042f530.c', 'Orig_0042f530',
                 'rect from the screen size and DAT_0067ed68 into DAT_00899150..5c, DAT_0067f190\'s rasters made sub rasters',
                 ['FUN_0042b8b0', 'FUN_0042b8c0', 'FUN_004671a0', 'FUN_004c7760']),
    '004c1be0': ('s19/004c1be0.c', 'Orig_004c1be0',
                 'RwCameraShowRaster: 0x004c7730 on the camera\'s raster (+0x60); returns the camera or 0',
                 ['FUN_004c7730']),
}

TEMPLATE = '''// PORT-TOP-04B (L-96 top-down lane): exe-only port body of rva 0x{rva} = the matched source re/match/{src}
// ({what}). Generated by verify/porttop_20261108/gen_top04b_tus.py. Reached only on the MASHED_ORIG_TOP path
// (SubsystemInit 0x00492270 -> DisplayInit 0x004921d0 / ViewportInit 0x00428590). Callees bind by name; no raw-address
// call. RW core callees bind to LibRw/RwApiBind.cpp.
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_{rva} {sym}
{binds}
extern "C" {{
#include "../../../../re/match/{src}"
}}
#endif // MASHED_STANDALONE
'''


def main():
    for rva, (src, sym, what, callees) in TUS.items():
        lines = []
        for n in callees:
            s, c = RW.get(n) or GAME[n]
            lines.append('#define %s %s   // %s' % (n, s, c))
        txt = TEMPLATE.format(rva=rva, src=src, what=what, sym=sym, binds='\n'.join(lines))
        open(os.path.join(MATCH, 'PortTop_%s.cpp' % rva), 'w', encoding='utf-8', newline='\n').write(txt)
        print('PortTop_%s.cpp' % rva)


if __name__ == '__main__':
    main()
