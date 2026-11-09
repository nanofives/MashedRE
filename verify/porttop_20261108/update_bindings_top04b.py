"""PORT-TOP-04B: DisplayInit 0x004921d0 and ViewportInit 0x00428590 with their callee trees in BINDINGS.tsv (one row per
RVA, idempotent). usage: py -3.12 verify/porttop_20261108/update_bindings_top04b.py
"""
import os

TSV = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'BINDINGS.tsv')
G = 'Match/PortTop_<rva>.cpp (gen_top04b_tus.py)'
B = 'LibRw/RwApiBind.cpp: '
T = 'PORT-TOP-04B: '
D, V = 0x004921d0, 0x00428590

UPD = {
    D: ('DisplayInit', 'matched', 'DisplayInit', T + 'Match/MatchS15_004921d0.cpp (re/match/s15/004921d0.c 100%); its MASHED_STANDALONE branch binds every callee by name', 2, 0x00492270),
    V: ('ViewportInit', 'matched', 'ViewportInit', T + 'Match/MatchS06_00428590.cpp (100%); MASHED_STANDALONE binds every callee by name', 2, 0x00492270),
    0x00498bc0: ('VideoGetRenderWidth', 'stand-in', 'PortTop_Width', 'exe resolution kWidth (DAT_00616028 = 0 in the exe); callers 0x00499ba0 (TOP-02) and 0x004921d0 (TOP-04B)', 2, 0x00499ba0),
    0x00498bd0: ('VideoGetRenderHeight', 'stand-in', 'PortTop_Height', 'exe resolution kHeight (see 0x00498bc0); callers 0x00499ba0 and 0x004921d0', 2, 0x00499ba0),
    0x00467110: (None, 'matched', 'Orig_00467110', T + G.replace('<rva>', '00467110') + ', re/match/s11/00467110.c 100%, holds static 0x004670a0', 3, D),
    0x004670a0: (None, 'matched', 'FUN_004670a0 (static)', T + 'file-static in re/match/s11/00467110.c, compiled in Match/PortTop_00467110.cpp', 4, 0x00467110),
    0x00467020: (None, 'matched', 'Orig_00467020', T + G.replace('<rva>', '00467020') + ' (failure path of 0x004670a0 / 0x0042d420)', 5, 0x004670a0),
    0x0042b890: ('RenderWidthSet', 'matched', 'RenderWidthSet', T + 'Match/MatchS07_0042b890.cpp, now also in the exe list', 3, D),
    0x0042b8a0: ('RenderHeightSet', 'matched', 'RenderHeightSet', T + 'Match/MatchS07_0042b8a0.cpp, now also in the exe list', 3, D),
    0x0042d560: (None, 'matched', 'Orig_0042d560', T + G.replace('<rva>', '0042d560'), 3, D),
    0x0042d420: (None, 'matched', 'Orig_0042d420', T + G.replace('<rva>', '0042d420'), 4, 0x0042d560),
    0x0042d4a0: (None, 'matched', 'Orig_0042d4a0', T + G.replace('<rva>', '0042d4a0'), 4, 0x0042d560),
    0x0042f660: ('DefaultViewportCameraInit', 'matched', 'DefaultViewportCameraInit', T + G.replace('<rva>', '0042f660'), 3, D),
    0x0042f530: (None, 'matched', 'Orig_0042f530', T + G.replace('<rva>', '0042f530') + ' (C2 source 96.9%, the only source)', 4, 0x0042f660),
    0x0042b8b0: ('ScreenWidthGet', 'matched', 'ScreenWidthGet', T + 'Match/MatchS07_0042b8b0.cpp (exe body already)', 5, 0x0042d4a0),
    0x0042b8c0: ('ScreenHeightGet', 'matched', 'ScreenHeightGet', T + 'Match/MatchS07_0042b8c0.cpp (exe body already)', 5, 0x0042d4a0),
    0x004671a0: ('BackBuffRasterOpen', 'matched', 'BackBuffRasterOpen', T + 'Match/MatchS11_004671a0.cpp, now also in the exe list; MASHED_STANDALONE binds 0x0042b930 MenuAlphaGet / 0x0042f510 Vehicle0HandleGet', 3, V),
    0x004997b0: ('Win32ResourceLoader', 'matched', 'Win32ResourceLoader', T + 'Match/MatchS16_004997b0.cpp, now also in the exe list: FindResourceA(DAT_007e9580 = the exe module, 404, RWTEXDICTIONARY) finds the copy assets/mashed_re.rc embeds', 3, D),
    0x00499720: ('GetInputHinst', 'matched', 'GetInputHinst', T + 'Match/MatchS16_00499720.cpp, now also in the exe list', 4, 0x004997b0),
    0x004b3eb0: ('FUN_004b3eb0', 'port', 'PortTop_Port_004b3eb0', T + B + 'port body from the decomp (no matched source): RW memory stream + chunk 0x23 + 0x0054f8d0 replaced by Txd/TxdDecoder (written from 0x0054f8d0) and rw::TexDictionary / rw::Texture', 3, D),
    0x004c5c00: ('RwTexDictionaryFindNamedTexture', 'bound', 'MashedRw_TexDictionaryFindNamedTexture_004c5c00', B + 'rw::TexDictionary::find (TOP-03); reached since PORT-TOP-04B', 3, D),
    0x004c5930: ('sub_004c5930', 'bound', 'MashedRw_TexDictionaryDestroy_004c5930', B + 'RwTexDictionaryDestroy -> rw::TexDictionary::destroy (TOP-03); reached since PORT-TOP-04B (DisplayInit sets DAT_0077195c)', 1, 0x00492370),
    0x004c1d30: ('RwCameraCreate', 'bound', 'MashedRw_CameraCreate_004c1d30', T + B + 'rw::Camera::create behind an RW 3.x-layout camera record (the matched callers use +0x04 / +0x18 / +0x1c / +0x60 / +0x64 directly)', 5, 0x004670a0),
    0x004c0b30: ('RwFrameCreate', 'bound', 'MashedRw_FrameCreate_004c0b30', T + B + 'rw::Frame::create', 5, 0x004670a0),
    0x004c0740: ('_rwObjectHasFrameSetFrame', 'bound', 'MashedRw_ObjectHasFrameSetFrame_004c0740', T + B + 'rw::Camera::setFrame (every knob-path caller passes a camera record)', 5, 0x004670a0),
    0x004c77c0: ('RwRasterCreate', 'bound', 'MashedRw_RasterCreate_004c77c0', T + B + 'rw::Raster::create (same flags split; 0 x 0 = do-not-allocate)', 5, 0x004670a0),
    0x004c1c80: ('RwCameraSetViewWindow', 'bound', 'MashedRw_CameraSetViewWindow_004c1c80', T + B + 'rw::Camera::setViewWindow', 4, 0x00467110),
    0x004c1a40: ('RwCameraSetNearClipPlane', 'bound', 'MashedRw_CameraSetNearClipPlane_004c1a40', T + B + 'rw::Camera::setNearPlane', 4, 0x00467110),
    0x004c1b10: ('RwCameraSetFarClipPlane', 'bound', 'MashedRw_CameraSetFarClipPlane_004c1b10', T + B + 'rw::Camera::setFarPlane', 4, 0x00467110),
    0x004c2bf0: ('RwCameraClone', 'bound', 'MashedRw_CameraClone_004c2bf0', T + B + 'rw::Camera::clone (DAT_006905b4)', 4, 0x00467110),
    0x004c7760: ('RwRasterSubRaster', 'bound', 'MashedRw_RasterSubRaster_004c7760', T + B + 'rw::Raster::subRaster', 5, 0x0042d4a0),
    0x004c0c20: ('RwFrameDestroy', 'bound', 'MashedRw_FrameDestroy_004c0c20', T + B + 'rw::Frame::destroy (only on 0x00467020, a failure path)', 6, 0x00467020),
    0x004c1cf0: ('RwCameraDestroy', 'bound', 'MashedRw_CameraDestroy_004c1cf0', T + B + 'rw::Camera::destroy + frees the record (failure path)', 6, 0x00467020),
    0x004c7650: ('RwRasterDestroy', 'bound', 'MashedRw_RasterDestroy_004c7650', T + B + 'rw::Raster::destroy (failure path)', 6, 0x00467020),
    0x004c1bb0: ('RwCameraClear', 'bound', 'MashedRw_CameraClear_004c1bb0', T + B + 'rw::Camera::clear. The matched body (Match/MatchS19_004c1bb0.cpp, .asi) calls RwGlobals +0x9c, absent in the exe, so the binding is the exe path', 3, V),
    0x004c1a00: ('IntroSplashVtableSlot6', 'matched', 'IntroSplashVtableSlot6', T + 'Match/MatchS19_004c1a00.cpp, now also in the exe list; calls the record +0x18 = MashedRw_CameraBeginUpdateCb_004c2bb0', 3, V),
    0x004c19f0: ('RwVtableSlot07Call', 'matched', 'RwVtableSlot07Call', T + 'exe body already; calls the record +0x1c = MashedRw_CameraEndUpdateCb_004c2b80', 3, V),
    0x004c2bb0: ('FUN_004c2bb0', 'bound', 'MashedRw_CameraBeginUpdateCb_004c2bb0', T + B + 'camera begin-update function stored at +0x18 by 0x004c1d30 (no source, no hooks row): rw::Camera::beginUpdate + BeginScene (librw skips it under adoption, P3)', 4, 0x004c1a00),
    0x004c2b80: ('FUN_004c2b80', 'bound', 'MashedRw_CameraEndUpdateCb_004c2b80', T + B + 'camera end-update function stored at +0x1c by 0x004c1d30 (no source, no hooks row): EndScene + rw::Camera::endUpdate', 4, 0x004c19f0),
    0x004c1be0: (None, 'matched', 'Orig_004c1be0', T + G.replace('<rva>', '004c1be0') + ' (re/match/s19/004c1be0.c)', 3, V),
    0x004c7730: ('RwRasterShowRaster', 'bound', 'MashedRw_RasterShowRaster_004c7730', T + B + 'IDirect3DDevice9::Present on the adopted device. Not librw showRaster: it Resets the device when the vsync flag differs from the PresentationInterval', 4, 0x004c1be0),
    0x00428450: ('FUN_00428450', 'missing', 'PortTop_Missing_00428450', T + 'Match/PortTop_00493710.cpp: the loading-disc draw is not run. Only port body = .asi-only naked asm (HUD/TextCluster.cpp, raw calls); source 84.6%, so an exe copy is a second body (rva-lint CROSS-TARGET). Its callee 0x00450b10 (HudIm2DQuad, exe body) is not reached', 3, V),
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
