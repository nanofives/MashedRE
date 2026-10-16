// MASHED LOCAL PATCH (P9, rtex 2026-10-16) -- MatFX DUAL pixel shader, the fixed-function
// stage cascade RenderWare's D3D9 MatFX sets up in FUN_005405c0 @ 0x005405c0 for a dual
// texture with srcBlend SRCALPHA / dstBlend INVSRCALPHA (1518 of the 1566 shipped dual
// materials; the 48 ZERO/ONE ones on the Atmos vehicle are not ported, evidence/matfx_census.txt):
//
//   base texture present (0x005408f9..0x005409af):
//     stage 0  COLOROP/ALPHAOP SELECTARG1, ARG1 TEXTURE              -> T0
//     stage 1  COLOROP/ALPHAOP BLENDTEXTUREALPHA, ARG1 TEXTURE (T1),
//              ARG2 CURRENT                                         -> T1*T1.a + T0*(1-T1.a)
//     stage 2  COLOROP/ALPHAOP MODULATE, ARG1 DIFFUSE, ARG2 CURRENT  -> * diffuse
//   base texture absent (0x005409be..0x00540a47, dual bound to STAGE 0, texcoord set 0):
//     TFACTOR 0xffffffff; stage 0 COLOROP BLENDTEXTUREALPHA(TEXTURE, TFACTOR),
//     ALPHAOP SELECTARG2 (TFACTOR); stage 1 MODULATE(DIFFUSE, CURRENT) colour and alpha.
//
// /DTEX  : base present, stage-1 coordinates from TEXCOORD1 (rpGEOMETRYTEXTURED2 set)
// /DTEX /DUV0 : base present, one texcoord set -- RW forces stage-1 TEXCOORDINDEX 0
//          (0x00540c56 when flag 0x80 is clear)
// (none) : base absent
// Fog as default_PS.hlsl (P7, per pixel). See MASHED_PATCHES.md P9.
struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;
#if defined(TEX) && !defined(UV0)
	float2 TexCoord1	: TEXCOORD1;
#endif
	float4 Color		: COLOR0;
};

sampler2D tex0 : register(s0);
sampler2D tex1 : register(s1);

float4 fogColor : register(c0);
float4 fogData : register(c1);

float4 main(VS_out input) : COLOR
{
	float4 color;
#ifdef TEX
	float4 t0 = tex2D(tex0, input.TexCoord0.xy);
#ifdef UV0
	float4 t1 = tex2D(tex1, input.TexCoord0.xy);
#else
	float4 t1 = tex2D(tex1, input.TexCoord1.xy);
#endif
	color = lerp(t0, t1, t1.a);
#else
	float4 t = tex2D(tex0, input.TexCoord0.xy);
	color.rgb = lerp(float3(1.0, 1.0, 1.0), t.rgb, t.a);
	color.a = 1.0;
#endif
	color *= input.Color;
	float fog = clamp((input.TexCoord0.z - fogData.y)*fogData.z, fogData.w, 1.0);
	color.rgb = lerp(fogColor.rgb, color.rgb, fog);
	return color;
}
