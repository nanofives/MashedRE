"""RWG-RASTER-BIND: RwImageSetFromRaster 0x004d52d0 and RwGlobals +0x60 (stdFunc[rwSTANDARDIMAGEGETRASTER], original
0x004de1b0) bound on rw::Raster for MASHED_ORIG_TOP RWMOD.

No-chain convention (HB-GENFILES-NOCONFLICT): only this row's delta, through bindings_rows.apply.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_rwgrasterbind.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'RWG-RASTER-BIND: '
EV = ' (verify/portorigtop_20261109/RESULT_RWG-RASTER-BIND.md)'

FIXED = {
    0x004d52d0: ('RwImageSetFromRaster', 'bound', 'MashedRw_ImageSetFromRaster_004d52d0 (LibRw/RwApiBind.cpp)',
                 T + 'RW 3.x RwImageSetFromRaster by its code (stdFunc[6] call, privateFlags +0x22 bit 0 -> RwImage.flags 2, '
                 "RW's WriteRaster caller sequence at 0x00498810); on rw::Raster: RwGlobals +0x60 call, then "
                 'rw::Raster::privateFlags bit 0. No knob-path caller yet (0x00555ff0 waits on HUDINIT); checked by '
                 'MASHED_DBG_RASTERBIND_SELFTEST' + EV,
                 3, 0x00555ff0),
    0x004de1b0: ('FUN_004de1b0', 'bound', 'MashedRw_ImageGetRaster_004de1b0 at RwGlobals +0x60 (LibRw/RwApiBind.cpp)',
                 T + 'stdFunc[rwSTANDARDIMAGEGETRASTER] on rw::Raster: toImage, convertTo32, row copy into the RW-layout '
                 '32-bit image; other image depths return 0 (U-9803)' + EV,
                 4, 0x004d52d0),
}

if __name__ == '__main__':
    apply(T, FIXED, {})
