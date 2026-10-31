// FLAGS: /O2 /GS /Ob1
/* a2 0x004036a0 (612 B) 77.4% + 0x00403910 (170 B) 100%.
   0x004036a0 is a static Im2D screen-quad blit with a register convention: EBX = camera, ECX = top-left
   RwV2d*, EAX = RwRGBA*, stack = bottom-right RwV2d*, raster. The parameter order
   (cam, tl, br, col, raster) of a static function defined before its caller gives exactly those
   registers (regs/ops 78/98.7% now). 0x00403910 (its only caller here) matched 100%: D3D9 texture
   GetSurfaceLevel / device GetBackBuffer / StretchRect, Release order texSurf then backBuf (that order is
   what fixes the two stack slots, declaration order and names do not).
   0x004036a0 g2s did not compile (thiscall/fastcall clash). Residual class (b): frame 0x9c vs 0xa0. The
   original converts the two RwRasterGetOffset shorts through one dedicated int temp right after the call
   (mov tmp,eax / fild / mov tmp,ecx / fild) into separate ox/oy slots, then loads nearPlane and does
   fdivr 1.0. Ours reuses the short slots and delays the filds and the divide (fld 1.0 / fdiv). Tried:
   decl order (no effect), (float)(int)sx, int temp, RwV2d size/off structs (frame becomes 0xa0 but the
   vertex block shifts, 71-76%), inline (float)sx at use (62%), rz inline / before ox (59%), zNear local
   (52%), (float)(1.0/np) (63%), permuter (+0.7: oy before ox). volatile on nearPlane gives 78.6 (not used).
   Vertex block, colour packing (RWRGBALONG at the end, CSE'd), render-state calls already line up. */
typedef struct { float x, y; } RwV2d;
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { float x, y, z, rhw; unsigned int emissiveColor; float u, v; } RwIm2DVertex;
typedef struct RwRaster { int pad0[3]; int width; int height; } RwRaster;
typedef struct RwCamera {
  char pad0[0x60];
  RwRaster *frameBuffer;   /* +0x60 */
  char pad1[0x80 - 0x64];
  float nearPlane;         /* +0x80 */
} RwCamera;
typedef struct {
  RwCamera *curCamera;     /* +0x00 */
  void *pad0[5];
  float zNear;             /* +0x18 */
  void *pad1;
  int (*RenderStateSet)(int state, int value);                       /* +0x20 */
  void *pad2[2];
  int (*Im2DRenderTriangle)(RwIm2DVertex *v, int n, int a, int b, int c); /* +0x2c */
} RwGlobalsT;
extern RwGlobalsT *DAT_007d3ff8;
void FUN_004c75e0(RwRaster *r, short *x, short *y);

#define RWRGBALONG(r,g,b,a) ((unsigned int)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

// MATCH: 0x004036a0 FUN_004036a0
static void FUN_004036a0(RwCamera *cam, RwV2d *tl, RwV2d *br, RwRGBA *col, int raster)
{
  RwRGBA white;
  short sx, sy;
  float w, h, ox, oy, rz;
  RwV2d def[2];
  RwIm2DVertex v[4];

  def[0].x = 0.0f; def[0].y = 0.0f;
  def[1].x = 1.0f; def[1].y = 1.0f;
  white.red = 255; white.green = 255; white.blue = 255; white.alpha = 255;
  if (tl == 0) tl = &def[0];
  if (br == 0) br = &def[1];
  if (col == 0) col = &white;
  w = (float)cam->frameBuffer->width;
  h = (float)cam->frameBuffer->height;
  FUN_004c75e0(cam->frameBuffer, &sx, &sy);
  oy = (float)sy;
  ox = (float)sx;
  rz = 1.0f / cam->nearPlane;
  v[0].x = w * tl->x + ox; v[0].y = h * tl->y + oy; v[0].z = DAT_007d3ff8->zNear; v[0].rhw = rz;
  v[0].u = 1.0f; v[0].v = 0.0f;
  v[1].x = v[0].x; v[1].y = h * br->y + oy; v[1].z = DAT_007d3ff8->zNear; v[1].rhw = rz;
  v[1].u = 1.0f; v[1].v = 1.0f;
  v[2].x = w * br->x + ox; v[2].y = v[1].y; v[2].z = DAT_007d3ff8->zNear; v[2].rhw = rz;
  v[2].u = 0.0f; v[2].v = 1.0f;
  v[3].x = v[2].x; v[3].y = v[0].y; v[3].z = DAT_007d3ff8->zNear; v[3].rhw = rz;
  v[3].u = 0.0f; v[3].v = 0.0f;
  v[0].emissiveColor = RWRGBALONG(col->red, col->green, col->blue, col->alpha);
  v[1].emissiveColor = RWRGBALONG(col->red, col->green, col->blue, col->alpha);
  v[2].emissiveColor = RWRGBALONG(col->red, col->green, col->blue, col->alpha);
  v[3].emissiveColor = RWRGBALONG(col->red, col->green, col->blue, col->alpha);
  DAT_007d3ff8->RenderStateSet(2, 3);
  DAT_007d3ff8->RenderStateSet(7, 1);
  DAT_007d3ff8->RenderStateSet(1, raster);
  if (col->alpha != 255) DAT_007d3ff8->RenderStateSet(0xc, 1);
  DAT_007d3ff8->Im2DRenderTriangle(v, 4, 0, 1, 2);
  DAT_007d3ff8->Im2DRenderTriangle(v, 4, 2, 3, 0);
  if (col->alpha != 255) DAT_007d3ff8->RenderStateSet(0xc, 0);
  DAT_007d3ff8->RenderStateSet(7, 2);
}

/* IDirect3DTexture9 (0x00636b70) and IDirect3DDevice9 (0x00636b74), only the slots used here */
typedef struct IDirect3DSurface9 IDirect3DSurface9;
typedef struct { void *s[2]; unsigned long (__stdcall *Release)(IDirect3DSurface9 *); } SurfVtbl;
struct IDirect3DSurface9 { SurfVtbl *vt; };
typedef struct IDirect3DTexture9 IDirect3DTexture9;
typedef struct { void *s[18]; long (__stdcall *GetSurfaceLevel)(IDirect3DTexture9 *, unsigned int, IDirect3DSurface9 **); } TexVtbl;
struct IDirect3DTexture9 { TexVtbl *vt; };
typedef struct IDirect3DDevice9 IDirect3DDevice9;
typedef struct {
  void *s[18];
  long (__stdcall *GetBackBuffer)(IDirect3DDevice9 *, unsigned int, unsigned int, int, IDirect3DSurface9 **); /* +0x48 */
  void *s2[15];
  long (__stdcall *StretchRect)(IDirect3DDevice9 *, IDirect3DSurface9 *, const void *, IDirect3DSurface9 *, const void *, int); /* +0x88 */
} DevVtbl;
struct IDirect3DDevice9 { DevVtbl *vt; };
extern IDirect3DTexture9 *DAT_00636b70;
static IDirect3DDevice9 *DAT_00636b74; /* file-static, see 0x004034a0 */
extern int DAT_00636b78;
void FUN_004938e0(int p0, IDirect3DTexture9 *p1);

// MATCH: 0x00403910 FUN_00403910
void FUN_00403910(void)
{
  IDirect3DSurface9 *texSurf;
  IDirect3DSurface9 *backBuf;
  if (DAT_00636b70 != 0) {
    texSurf = 0;
    backBuf = 0;
    DAT_00636b70->vt->GetSurfaceLevel(DAT_00636b70, 0, &texSurf);
    DAT_00636b74->vt->GetBackBuffer(DAT_00636b74, 0, 0, 0, &backBuf);
    DAT_00636b74->vt->StretchRect(DAT_00636b74, backBuf, 0, texSurf, 0, 0);
    FUN_004938e0(DAT_00636b78, DAT_00636b70);
    FUN_004036a0(DAT_007d3ff8->curCamera, 0, 0, 0, DAT_00636b78);
    texSurf->vt->Release(texSurf);
    backBuf->vt->Release(backBuf);
  }
}
