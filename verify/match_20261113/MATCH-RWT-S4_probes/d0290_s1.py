C0_OLD = """      ext->lockedMipLevel = 0xff;
      param_2->privateFlags &= ~6;
      top = param_2;"""
C0_NEW = """      param_2->privateFlags &= ~6;
      ext->lockedMipLevel = 0xff;
      top = param_2;"""
CUBE_OLD = """        tex = topExt->texture;
        if (topExt->cube) {
          hr = (*(getcube_fn *)(*tex + 0x48))(tex, topExt->face, ext->lockedMipLevel, &surface);
        } else {
          hr = (*(getsurf_fn *)(*tex + 0x48))(tex, ext->lockedMipLevel, &surface);
        }"""
CUBE_A = """        if (topExt->cube) {
          tex = topExt->texture;
          hr = (*(getcube_fn *)(*tex + 0x48))(tex, topExt->face, ext->lockedMipLevel, &surface);
        } else {
          tex = topExt->texture;
          hr = (*(getsurf_fn *)(*tex + 0x48))(tex, ext->lockedMipLevel, &surface);
        }"""
CUBE_B = """        uchar cube = topExt->cube;
        tex = topExt->texture;
        if (cube) {
          hr = (*(getcube_fn *)(*tex + 0x48))(tex, topExt->face, ext->lockedMipLevel, &surface);
        } else {
          hr = (*(getsurf_fn *)(*tex + 0x48))(tex, ext->lockedMipLevel, &surface);
        }"""
VARIANTS = {
    'base': [],
    'c0': [(C0_OLD, C0_NEW)],
    'c0cA': [(C0_OLD, C0_NEW), (CUBE_OLD, CUBE_A)],
    'c0cB': [(C0_OLD, C0_NEW), (CUBE_OLD, CUBE_B)],
}
