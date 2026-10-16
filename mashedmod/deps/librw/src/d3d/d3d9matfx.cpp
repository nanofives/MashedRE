#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>	// MASHED LOCAL PATCH (P9): lrintf

#define WITH_D3D
#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwanim.h"
#include "../rwengine.h"
#include "../rwrender.h"
#include "../rwplugins.h"
#include "rwd3d.h"
#include "rwd3d9.h"

// [mashed] d1r1 / rtex per-draw recorder hooks (d3d9render.cpp); null by default.
extern void (*rw_drawrec_pre)(const char *texname, const float *ltm16);
extern void (*rw_drawrec_post)(void);
extern void (*rw_drawrec_tex1)(const char *tex1name);

namespace rw {
namespace d3d9 {
using namespace d3d;

#ifndef RW_D3D9
void matfxRenderCB_Shader(Atomic *atomic, InstanceDataHeader *header) {}
#else

static void *matfx_env_amb_VS;
static void *matfx_env_amb_dir_VS;
static void *matfx_env_all_VS;
static void *matfx_env_PS;
static void *matfx_env_tex_PS;
// MASHED LOCAL PATCH (P9, rtex) -- MatFX DUAL shaders (shaders/matfx_dual_*.hlsl)
static void *matfx_dual_amb_VS;
static void *matfx_dual_amb_dir_VS;
static void *matfx_dual_all_VS;
static void *matfx_dual_tex_PS;
static void *matfx_dual_tex_uv0_PS;
static void *matfx_dual_PS;
// MASHED LOCAL PATCH (P9, rtex) -- MatFX ENVMAP as FUN_005412d0 draws it (shaders/matfx_envrw_*)
static void *matfx_envrw_amb_VS;
static void *matfx_envrw_amb_dir_VS;
static void *matfx_envrw_all_VS;
static void *matfx_envrw_tex_PS;
static void *matfx_envrw_PS;
static void *matfx_envrw_pass2_PS;

// MASHED LOCAL PATCH (P9) -- one recorded draw (rw_drawrec_* hooks are null unless the
// mashed_re per-draw recorder is installed).
static void
drawInstRec(InstanceDataHeader *header, InstanceData *inst, Atomic *atomic,
            Texture *tex0, Texture *tex1)
{
	if(rw_drawrec_tex1)
		rw_drawrec_tex1(tex1 ? tex1->name : nil);
	if(rw_drawrec_pre)
		rw_drawrec_pre(tex0 ? tex0->name : nil, (const float*)atomic->getFrame()->getLTM());
	drawInst(header, inst);
	if(rw_drawrec_post)
		rw_drawrec_post();
}

static Atomic *curAtomic;	// the atomic matfxRenderCB_Shader is drawing

enum
{
	VSLOC_texMat = VSLOC_afterLights,
	VSLOC_colorClamp = VSLOC_texMat + 4,
	VSLOC_envColor,

	PSLOC_shininess = 1,
};

void
matfxRender_Default(InstanceDataHeader *header, InstanceData *inst, int32 lightBits)
{
	Material *m = inst->material;

	// Pick a shader
	if((lightBits & VSLIGHT_MASK) == 0)
		setVertexShader(default_amb_VS);
	else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
		setVertexShader(default_amb_dir_VS);
	else
		setVertexShader(default_all_VS);

	SetRenderState(VERTEXALPHA, inst->vertexAlpha || m->color.alpha != 255);

	if(inst->material->texture){
		d3d::setTexture(0, m->texture);
		setPixelShader(default_tex_PS);
	}else
		setPixelShader(default_PS);

	drawInstRec(header, inst, curAtomic, m->texture, nil);	// MASHED LOCAL PATCH (P9)
}

// MASHED LOCAL PATCH (P9, rtex 2026-10-16) -- MatFX DUAL, ported from RenderWare's D3D9
// MatFX dual renderer FUN_005405c0 @ 0x005405c0 (MASHED.exe), branch srcBlend SRCALPHA /
// dstBlend INVSRCALPHA with DAT_007dc614 != 0 and DAT_007dc624 >= 3 (the single-pass,
// three-stage path the original takes on this hardware: stage-1 COLOROP 13 recorded in
// verify/d1r1_20261016). The stage cascade is in shaders/matfx_dual_PS.hlsl.
//   - null dual texture: plain draw (FUN_00540340 via the test at 0x005405d0)
//   - base texture present: base on stage 0, dual on stage 1 (0x005408f9); stage-1
//     coordinates are texcoord set 1 when the geometry has rpGEOMETRYTEXTURED2, else set 0
//     (TEXCOORDINDEX 0 at 0x00540c56); stage 1 is unbound after the draw (0x005411f3)
//   - base texture absent: dual on stage 0 (0x005409ca)
//   - any other blend pair: [UNCERTAIN] not ported (no shipped asset uses one,
//     verify/rtex_20261016/evidence/matfx_census.txt); drawn plain
// Blending follows stage 0 only, as FUN_004d6ce0 does (stage != 0 skips the alpha rule at
// 0x004d6d00), which is what setRasterStage already does.
static void
matfxRender_Dual(InstanceDataHeader *header, InstanceData *inst, int32 lightBits,
                 MatFX::Dual *dual, uint32 geoFlags)
{
	Material *m = inst->material;
	if(dual->tex == nil ||
	   dual->srcBlend != BLENDSRCALPHA || dual->dstBlend != BLENDINVSRCALPHA){
		matfxRender_Default(header, inst, lightBits);
		return;
	}

	SetRenderState(VERTEXALPHA, inst->vertexAlpha || m->color.alpha != 255);

	if(m->texture){
		bool two = (geoFlags & Geometry::TEXTURED2) != 0;
		if(two){
			if((lightBits & VSLIGHT_MASK) == 0)
				setVertexShader(matfx_dual_amb_VS);
			else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
				setVertexShader(matfx_dual_amb_dir_VS);
			else
				setVertexShader(matfx_dual_all_VS);
			setPixelShader(matfx_dual_tex_PS);
		}else{
			if((lightBits & VSLIGHT_MASK) == 0)
				setVertexShader(default_amb_VS);
			else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
				setVertexShader(default_amb_dir_VS);
			else
				setVertexShader(default_all_VS);
			setPixelShader(matfx_dual_tex_uv0_PS);
		}
		d3d::setTexture(0, m->texture);
		d3d::setTexture(1, dual->tex);
		drawInstRec(header, inst, curAtomic, m->texture, dual->tex);
		d3d::setTexture(1, nil);
	}else{
		if((lightBits & VSLIGHT_MASK) == 0)
			setVertexShader(default_amb_VS);
		else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
			setVertexShader(default_amb_dir_VS);
		else
			setVertexShader(default_all_VS);
		setPixelShader(matfx_dual_PS);
		d3d::setTexture(0, dual->tex);
		drawInstRec(header, inst, curAtomic, dual->tex, nil);
	}
}

static Frame *lastEnvFrame;

static RawMatrix normal2texcoord = {
	{ 0.5f,  0.0f, 0.0f }, 0.0f,
	{ 0.0f, -0.5f, 0.0f }, 0.0f,
	{ 0.0f,  0.0f, 1.0f }, 0.0f,
	{ 0.5f,  0.5f, 0.0f }, 1.0f
};

void
uploadEnvMatrix(Frame *frame)
{
	Matrix invMat;
	if(frame == nil)
		frame = engine->currentCamera->getFrame();

	// cache the matrix across multiple meshes
// can't do it, frame matrix may change
//	if(frame == lastEnvFrame)
//		return;
//	lastEnvFrame = frame;

	RawMatrix envMtx, invMtx;
	Matrix::invert(&invMat, frame->getLTM());
	convMatrix(&invMtx, &invMat);
	invMtx.pos.set(0.0f, 0.0f, 0.0f);
	float uscale = fabs(normal2texcoord.right.x);
	normal2texcoord.right.x = MatFX::envMapFlipU ? -uscale : uscale;
	RawMatrix::mult(&envMtx, &invMtx, &normal2texcoord);
	d3ddevice->SetVertexShaderConstantF(VSLOC_texMat, (float*)&envMtx, 4);
}

// MASHED LOCAL PATCH (P9, rtex 2026-10-16) -- the raster alpha flag RenderWare's driver keeps
// at raster-ext +8 (FUN_004cfee0 @ 0x004cfee0 returns it) decides FUN_005412d0's single vs two
// pass. librw flags every C8888 raster alpha, so mashed_re's raster bridge answers with the
// original's rule (a base-mip texel with alpha < 255, verify/d1r1_20261016/texalpha_rule.txt).
// Null -> librw's own flag.
int32 (*rw_mashed_rasteralpha)(Raster *raster) = nil;
static bool
rasterAlphaRW(Texture *t)
{
	if(t == nil || t->raster == nil)
		return false;
	if(rw_mashed_rasteralpha)
		return rw_mashed_rasteralpha(t->raster) != 0;
	return GETD3DRASTEREXT(t->raster)->hasAlpha != 0;
}

static void
pickDefaultVS(int32 lightBits)
{
	if((lightBits & VSLIGHT_MASK) == 0)
		setVertexShader(default_amb_VS);
	else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
		setVertexShader(default_amb_dir_VS);
	else
		setVertexShader(default_all_VS);
}

static void
pickEnvVS(int32 lightBits)
{
	if((lightBits & VSLIGHT_MASK) == 0)
		setVertexShader(matfx_envrw_amb_VS);
	else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
		setVertexShader(matfx_envrw_amb_dir_VS);
	else
		setVertexShader(matfx_envrw_all_VS);
}

void uploadEnvMatrix(Frame *frame);

// MASHED LOCAL PATCH (P9, rtex 2026-10-16) -- MatFX ENVMAP, ported from RenderWare's D3D9 env
// renderer FUN_005412d0 @ 0x005412d0 (MASHED.exe), dispatched by FUN_00543500 on effect 2:
//   - coefficient byte = ROUND(coef * [0x005cd04c] = 255.0) & 0xff; byte 0 or no env texture
//     draws plain (FUN_00540340 at 0x00541b31)
//   - single pass when (fbAlpha || opaque base) and the env raster has no alpha
//     (0x005413ba..0x005413da): base * diffuse on stage 0 (0x00541466..), env on stage 1 with
//     MULTIPLYADD(CURRENT, TEXTURE, TFACTOR = coef byte) (0x005417b7..0x005417dd) or ADD at
//     byte 255 (0x005417eb..0x005417fe), drawn at 0x00541874. DAT_007dc618 (the MULTIPLYADD
//     cap) is nonzero on this hardware: the original's body draws carry stage-1 COLOROP 25.
//   - otherwise two draws: the plain draw (0x0054150b), then the env texture on STAGE 0
//     (FUN_00541b50 at 0x005418df) with SRCBLEND 5 / DESTBLEND 2 (0x0054191c / 0x00541923),
//     MODULATE(TEXTURE, TFACTOR) or SELECTARG1 at 255 (0x00541944..0x005419ba), LIGHTING 0,
//     ZWRITE 0, FOGCOLOR 0 (0x005419f0..0x00541a19), drawn at 0x00541a4b, states restored.
//   - [UNCERTAIN U-9697] an env raster WITH alpha takes the DAT_007dc600 branch
//     (0x005413da..) -- not ported; such a material draws two-pass here.
// Env texture coordinates: FUN_00541b50 @ 0x00541b50 sets TCI CAMERASPACENORMAL, COUNT2 and
// (camera LTM x inverse(env frame LTM)) with the right row negated, translation zeroed,
// times [0x005e4848] = {0.5,0,0,0, 0,-0.5,0,0, 0,0,1,0, 0.5,0.5,0,0}. librw's uploadEnvMatrix
// gives inverse(env frame LTM) x {0.5,0,0; 0,-0.5,0; 0,0,1; 0.5,0.5,0} applied to the WORLD
// normal; the right-row negation cancels the D3D view's x flip, so the u,v agree.
static void
matfxRender_EnvRW(InstanceDataHeader *header, InstanceData *inst, int32 lightBits,
                  MatFX::Env *env, uint32 geoFlags)
{
	Material *m = inst->material;
	int32 cb = (int32)lrintf(env->coefficient * 255.0f) & 0xff;
	if(cb == 0 || env->tex == nil){
		matfxRender_Default(header, inst, lightBits);
		return;
	}
	bool base = (geoFlags & (Geometry::TEXTURED | Geometry::TEXTURED2)) && m->texture;
	bool opaque = !inst->vertexAlpha && m->color.alpha == 255 &&
	              (!base || !rasterAlphaRW(m->texture));
	bool single = (env->fbAlpha || opaque) && !rasterAlphaRW(env->tex);

	float params[4] = { cb / 255.0f, 0.0f, 0.0f, 0.0f };
	d3ddevice->SetPixelShaderConstantF(2, params, 1);
	uploadEnvMatrix(env->frame);
	SetRenderState(VERTEXALPHA, inst->vertexAlpha || m->color.alpha != 255);
	if(base)
		d3d::setTexture(0, m->texture);

	if(single){
		pickEnvVS(lightBits);
		d3d::setTexture(1, env->tex);
		setPixelShader(base ? matfx_envrw_tex_PS : matfx_envrw_PS);
		drawInstRec(header, inst, curAtomic, base ? m->texture : nil, env->tex);
		d3d::setTexture(1, nil);
		return;
	}

	// pass 1: the plain draw
	pickDefaultVS(lightBits);
	setPixelShader(base ? default_tex_PS : default_PS);
	drawInstRec(header, inst, curAtomic, base ? m->texture : nil, nil);

	// pass 2: env on stage 0, additive by its alpha, no depth write
	uint32 srcb = GetRenderState(SRCBLEND), dstb = GetRenderState(DESTBLEND);
	uint32 zw = GetRenderState(ZWRITEENABLE);
	pickEnvVS(lightBits);
	d3d::setTexture(0, env->tex);
	setPixelShader(matfx_envrw_pass2_PS);
	SetRenderState(VERTEXALPHA, 1);
	SetRenderState(SRCBLEND, BLENDSRCALPHA);
	SetRenderState(DESTBLEND, BLENDONE);
	SetRenderState(ZWRITEENABLE, 0);
	drawInstRec(header, inst, curAtomic, env->tex, nil);
	SetRenderState(ZWRITEENABLE, zw);
	SetRenderState(SRCBLEND, srcb);
	SetRenderState(DESTBLEND, dstb);
	SetRenderState(VERTEXALPHA, inst->vertexAlpha || m->color.alpha != 255);
}

void
matfxRender_EnvMap(InstanceDataHeader *header, InstanceData *inst, int32 lightBits, MatFX::Env *env)
{
	Material *m = inst->material;

	if(env->tex == nil || env->coefficient == 0.0f){
		matfxRender_Default(header, inst, lightBits);
		return;
	}

	d3d::setTexture(1, env->tex);
	uploadEnvMatrix(env->frame);

	SetRenderState(SRCBLEND, BLENDONE);

	static float zero[4];
	static float one[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	struct  {
		float shininess;
		float disableFBA;
		float unused[2];
	} fxparams;
	fxparams.shininess = env->coefficient;
	fxparams.disableFBA = env->fbAlpha ? 0.0f : 1.0f;
	d3ddevice->SetPixelShaderConstantF(PSLOC_shininess, (float*)&fxparams, 1);
	// This clamps the vertex color below. With it we can achieve both PC and PS2 style matfx
	if(MatFX::envMapApplyLight)
		d3ddevice->SetVertexShaderConstantF(VSLOC_colorClamp, zero, 1);
	else
		d3ddevice->SetVertexShaderConstantF(VSLOC_colorClamp, one, 1);
	RGBAf envcol[4];
	if(MatFX::envMapUseMatColor)
		convColor(envcol, &m->color);
	else
		convColor(envcol, &MatFX::envMapColor);
	d3ddevice->SetVertexShaderConstantF(VSLOC_envColor, (float*)&envcol, 1);

	// Pick a shader
	if((lightBits & VSLIGHT_MASK) == 0)
		setVertexShader(matfx_env_amb_VS);
	else if((lightBits & VSLIGHT_MASK) == VSLIGHT_DIRECT)
		setVertexShader(matfx_env_amb_dir_VS);
	else
		setVertexShader(matfx_env_all_VS);

	bool32 texAlpha = GETD3DRASTEREXT(env->tex->raster)->hasAlpha;

	if(inst->material->texture){
		d3d::setTexture(0, m->texture);
		setPixelShader(matfx_env_tex_PS);
	}else
		setPixelShader(matfx_env_PS);

	SetRenderState(VERTEXALPHA, texAlpha || inst->vertexAlpha || m->color.alpha != 255);

	drawInst(header, inst);

	SetRenderState(SRCBLEND, BLENDSRCALPHA);
}

void
matfxRenderCB_Shader(Atomic *atomic, InstanceDataHeader *header)
{
	int vsBits;
	uint32 flags = atomic->geometry->flags;
	setStreamSource(0, (IDirect3DVertexBuffer9*)header->vertexStream[0].vertexBuffer,
	                           0, header->vertexStream[0].stride);
	setIndices((IDirect3DIndexBuffer9*)header->indexBuffer);
	setVertexDeclaration((IDirect3DVertexDeclaration9*)header->vertexDeclaration);

	lastEnvFrame = nil;
	curAtomic = atomic;	// MASHED LOCAL PATCH (P9)

	vsBits = lightingCB_Shader(atomic);
	uploadMatrices(atomic->getFrame()->getLTM());

	bool normals = !!(atomic->geometry->flags & Geometry::NORMALS);

	InstanceData *inst = header->inst;
	for(uint32 i = 0; i < header->numMeshes; i++){
		Material *m = inst->material;

		setMaterial(flags, m->color, m->surfaceProps);

		MatFX *matfx = MatFX::get(m);
		if(matfx == nil)
			matfxRender_Default(header, inst, vsBits);
		else switch(matfx->type){
		case MatFX::ENVMAP:
			// MASHED LOCAL PATCH (P9) -- FUN_00543500 effect 2 -> FUN_005412d0
			if(normals)
				matfxRender_EnvRW(header, inst, vsBits, &matfx->fx[0].env, flags);
			else
				matfxRender_Default(header, inst, vsBits);
			break;
		case MatFX::DUAL:
			// MASHED LOCAL PATCH (P9) -- FUN_00543500 effect 4 -> FUN_005405c0
			{
				int32 di = matfx->getEffectIndex(MatFX::DUAL);
				if(di >= 0)
					matfxRender_Dual(header, inst, vsBits, &matfx->fx[di].dual, flags);
				else
					matfxRender_Default(header, inst, vsBits);
			}
			break;
		case MatFX::NOTHING:
		case MatFX::BUMPMAP:
		case MatFX::BUMPENVMAP:
		case MatFX::UVTRANSFORM:
		case MatFX::DUALUVTRANSFORM:
			// not supported yet
			matfxRender_Default(header, inst, vsBits);
			break;
		}

		inst++;
	}
	d3d::setTexture(1, nil);
}

#define VS_NAME g_vs20_main
#define PS_NAME g_ps20_main

void
createMatFXShaders(void)
{
	{
		static
#include "shaders/matfx_env_amb_VS.h"
		matfx_env_amb_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_env_amb_VS);
	}
	{
		static
#include "shaders/matfx_env_amb_dir_VS.h"
		matfx_env_amb_dir_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_env_amb_dir_VS);
	}
	{
		static
#include "shaders/matfx_env_all_VS.h"
		matfx_env_all_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_env_all_VS);
	}


	{
		static
#include "shaders/matfx_env_PS.h"
		matfx_env_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_env_PS);
	}
	{
		static
#include "shaders/matfx_env_tex_PS.h"
		matfx_env_tex_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_env_tex_PS);
	}

	// MASHED LOCAL PATCH (P9, rtex) -- MatFX DUAL shaders
	{
		static
#include "shaders/matfx_dual_amb_VS.h"
		matfx_dual_amb_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_dual_amb_VS);
	}
	{
		static
#include "shaders/matfx_dual_amb_dir_VS.h"
		matfx_dual_amb_dir_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_dual_amb_dir_VS);
	}
	{
		static
#include "shaders/matfx_dual_all_VS.h"
		matfx_dual_all_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_dual_all_VS);
	}
	{
		static
#include "shaders/matfx_dual_tex_PS.h"
		matfx_dual_tex_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_dual_tex_PS);
	}
	{
		static
#include "shaders/matfx_dual_tex_uv0_PS.h"
		matfx_dual_tex_uv0_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_dual_tex_uv0_PS);
	}
	{
		static
#include "shaders/matfx_dual_PS.h"
		matfx_dual_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_dual_PS);
	}
	// MASHED LOCAL PATCH (P9, rtex) -- MatFX ENVMAP (FUN_005412d0) shaders
	{
		static
#include "shaders/matfx_envrw_amb_VS.h"
		matfx_envrw_amb_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_envrw_amb_VS);
	}
	{
		static
#include "shaders/matfx_envrw_amb_dir_VS.h"
		matfx_envrw_amb_dir_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_envrw_amb_dir_VS);
	}
	{
		static
#include "shaders/matfx_envrw_all_VS.h"
		matfx_envrw_all_VS = createVertexShader((void*)VS_NAME);
		assert(matfx_envrw_all_VS);
	}
	{
		static
#include "shaders/matfx_envrw_tex_PS.h"
		matfx_envrw_tex_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_envrw_tex_PS);
	}
	{
		static
#include "shaders/matfx_envrw_PS.h"
		matfx_envrw_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_envrw_PS);
	}
	{
		static
#include "shaders/matfx_envrw_pass2_PS.h"
		matfx_envrw_pass2_PS = createPixelShader((void*)PS_NAME);
		assert(matfx_envrw_pass2_PS);
	}
}

void
destroyMatFXShaders(void)
{
	destroyVertexShader(matfx_env_amb_VS);
	matfx_env_amb_VS = nil;

	destroyVertexShader(matfx_env_amb_dir_VS);
	matfx_env_amb_dir_VS = nil;

	destroyVertexShader(matfx_env_all_VS);
	matfx_env_all_VS = nil;


	destroyPixelShader(matfx_env_PS);
	matfx_env_PS = nil;

	destroyPixelShader(matfx_env_tex_PS);
	matfx_env_tex_PS = nil;

	// MASHED LOCAL PATCH (P9, rtex)
	destroyVertexShader(matfx_dual_amb_VS);
	matfx_dual_amb_VS = nil;
	destroyVertexShader(matfx_dual_amb_dir_VS);
	matfx_dual_amb_dir_VS = nil;
	destroyVertexShader(matfx_dual_all_VS);
	matfx_dual_all_VS = nil;
	destroyPixelShader(matfx_dual_tex_PS);
	matfx_dual_tex_PS = nil;
	destroyPixelShader(matfx_dual_tex_uv0_PS);
	matfx_dual_tex_uv0_PS = nil;
	destroyPixelShader(matfx_dual_PS);
	matfx_dual_PS = nil;
	destroyVertexShader(matfx_envrw_amb_VS);
	matfx_envrw_amb_VS = nil;
	destroyVertexShader(matfx_envrw_amb_dir_VS);
	matfx_envrw_amb_dir_VS = nil;
	destroyVertexShader(matfx_envrw_all_VS);
	matfx_envrw_all_VS = nil;
	destroyPixelShader(matfx_envrw_tex_PS);
	matfx_envrw_tex_PS = nil;
	destroyPixelShader(matfx_envrw_PS);
	matfx_envrw_PS = nil;
	destroyPixelShader(matfx_envrw_pass2_PS);
	matfx_envrw_pass2_PS = nil;
}

#endif

static void*
matfxOpen(void *o, int32, int32)
{
#ifdef RW_D3D9
	createMatFXShaders();
#endif

	matFXGlobals.pipelines[PLATFORM_D3D9] = makeMatFXPipeline();
	return o;
}

static void*
matfxClose(void *o, int32, int32)
{
#ifdef RW_D3D9
	destroyMatFXShaders();
#endif

	((ObjPipeline*)matFXGlobals.pipelines[PLATFORM_D3D9])->destroy();
	matFXGlobals.pipelines[PLATFORM_D3D9] = nil;
	return o;
}

void
initMatFX(void)
{
	Driver::registerPlugin(PLATFORM_D3D9, 0, ID_MATFX,
	                       matfxOpen, matfxClose);
}

ObjPipeline*
makeMatFXPipeline(void)
{
	ObjPipeline *pipe = ObjPipeline::create();
	pipe->instanceCB = defaultInstanceCB;
	pipe->uninstanceCB = defaultUninstanceCB;
	pipe->renderCB = matfxRenderCB_Shader;
	pipe->pluginID = ID_MATFX;
	pipe->pluginData = 0;
	return pipe;
}

}
}
