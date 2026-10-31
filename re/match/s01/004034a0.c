// FLAGS: /O2 /GS /Ob1
/* a2 0x004034a0 (412 B). Best: 100%.
   g2s 29.9%: Ghidra lost the D3D vtable calls (stack garbage). Rebuilt as IDirect3DDevice9 GetDeviceCaps
   (+0x1c), GetRenderTarget (+0x98), CreateTexture (+0x5c), IDirect3DSurface9 GetDesc (+0x30) / Release.
   D3DCAPS9 padded with dword arrays (char padding makes /GS add a cookie the original lacks). Log calls as
   if/else (a ?: select gives mov eax,imm). bpp test as if/else-if, desc.Format test as a switch (range
   compare). fmt is uninitialised on the fall-through paths (the original reads the dead surf slot, we do
   too). Last step 99.2 -> 100: DAT_00636b74 is file-static (store sinks below the push). Its 5 .text refs
   are all in 0x004034a0 and 0x00403910, so those two share one TU. */
typedef struct {
  unsigned long pad0[0x3c / 4];
  unsigned long TextureCaps;   /* +0x3c */
  unsigned long pad1[(0xd4 - 0x40) / 4];
  unsigned long DevCaps2;      /* +0xd4 */
  unsigned long pad2[(0x130 - 0xd8) / 4];
} D3DCAPS9;
typedef struct {
  int Format, Type;
  unsigned long Usage;
  int Pool, MultiSampleType;
  unsigned long MultiSampleQuality;
  unsigned int Width, Height;
} D3DSURFACE_DESC;
typedef struct IDirect3DSurface9 IDirect3DSurface9;
typedef struct {
  void *s[2];
  unsigned long (__stdcall *Release)(IDirect3DSurface9 *);              /* +0x08 */
  void *s2[9];
  long (__stdcall *GetDesc)(IDirect3DSurface9 *, D3DSURFACE_DESC *);    /* +0x30 */
} SurfVtbl;
struct IDirect3DSurface9 { SurfVtbl *vt; };
typedef struct IDirect3DTexture9 IDirect3DTexture9;
typedef struct IDirect3DDevice9 IDirect3DDevice9;
typedef struct {
  void *s[7];
  long (__stdcall *GetDeviceCaps)(IDirect3DDevice9 *, D3DCAPS9 *);      /* +0x1c */
  void *s2[15];
  long (__stdcall *CreateTexture)(IDirect3DDevice9 *, unsigned int, unsigned int, unsigned int,
                                  unsigned long, int, int, IDirect3DTexture9 **, void *); /* +0x5c */
  void *s3[14];
  long (__stdcall *GetRenderTarget)(IDirect3DDevice9 *, unsigned long, IDirect3DSurface9 **); /* +0x98 */
} DevVtbl;
struct IDirect3DDevice9 { DevVtbl *vt; };
typedef struct RwRaster RwRaster;

extern IDirect3DDevice9 *DAT_00771a0c;
extern int DAT_00636ad8;
static IDirect3DDevice9 *DAT_00636b74; /* file-static: see header */
extern IDirect3DTexture9 *DAT_00636b70;
void FUN_004963e0(const char *fmt, ...);
void FUN_00496400(const char *fmt, ...);
int FUN_00498bc0(void);
int FUN_00498bd0(void);
int FUN_00498be0(void);
int FUN_00498bf0(void);
RwRaster *FUN_004c77c0(int w, int h, int d, int flags);

// MATCH: 0x004034a0 FUN_004034a0
RwRaster *FUN_004034a0(void)
{
  IDirect3DSurface9 *surf;
  D3DSURFACE_DESC desc;
  D3DCAPS9 caps;
  unsigned long nonPow2, stretch;
  int w, h, fmt;

  surf = 0;
  DAT_00636ad8 = 1;
  DAT_00636b74 = DAT_00771a0c;
  if (DAT_00771a0c->vt->GetDeviceCaps(DAT_00771a0c, &caps) >= 0) {
    nonPow2 = caps.TextureCaps & 0x100;
    stretch = caps.DevCaps2 & 0x10;
    if (nonPow2)
      FUN_004963e0("Video card supports D3DPTEXTURECAPS_NONPOW2CONDITIONAL\n");
    else
      FUN_004963e0("Video card does NOT support D3DPTEXTURECAPS_NONPOW2CONDITIONAL\n");
    if (stretch)
      FUN_004963e0("Video card supports D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES\n");
    else
      FUN_004963e0("Video card does NOT support D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES\n");
    if (nonPow2 && stretch) {
      w = FUN_00498bc0();
      h = FUN_00498bd0();
      DAT_00636b74->vt->GetRenderTarget(DAT_00636b74, 0, &surf);
      surf->vt->GetDesc(surf, &desc);
      surf->vt->Release(surf);
      if (FUN_00498bf0() != 0) {
        int bpp = FUN_00498be0();
        if (bpp == 32) {
          fmt = 0x16;
          FUN_004963e0("format = D3DFMT_X8R8G8B8\n");
        } else if (bpp == 16) {
          fmt = 0x17;
          FUN_004963e0("format = D3DFMT_R5G6B5\n");
        }
      } else {
        switch (desc.Format) {
        case 0x15:
        case 0x16:
          fmt = 0x16;
          break;
        case 0x17:
          fmt = 0x17;
          break;
        }
      }
      FUN_00496400("Attempting to create texture for back buffer (%dx%d)\n", w, h);
      if (DAT_00636b74->vt->CreateTexture(DAT_00636b74, w, h, 1, 1, fmt, 0, &DAT_00636b70, 0) >= 0)
        FUN_004963e0("\tCreated texture\n");
      else
        FUN_004963e0("\tUnable to create texture\n");
      DAT_00636ad8 = 0;
    } else {
      DAT_00636ad8 = 1;
    }
  }
  return FUN_004c77c0(0, 0, 0, 0x84);
}
