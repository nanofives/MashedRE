// MASHED LOCAL PATCH (P9, rtex 2026-10-16) -- MatFX ENVMAP as RenderWare's D3D9 driver draws
// it, FUN_005412d0 @ 0x005412d0 (MASHED.exe). Upstream's matfx_env_PS.hlsl folds both
// passes into one blended draw; RW issues a single-pass stage cascade when the base is opaque
// and a real second draw otherwise, so the passes are separate shaders here:
//
//   single pass, base present (/DTEX):  stage 0 MODULATE(TEXTURE, DIFFUSE) colour and alpha;
//     stage 1 MULTIPLYADD(CURRENT, TEXTURE, TFACTOR) colour (0x00540d80 .. 0x00540dba region,
//     here 0x00541580..: COLOROP 0x19, ARG0 CURRENT, ARG1 TEXTURE, ARG2 TFACTOR = coef byte
//     replicated), or ADD(TEXTURE, CURRENT) when the coef byte is 255 -- the same sum with
//     coef 1. Stage-1 alpha is not set: alpha stays CURRENT.
//   single pass, base absent (no TEX):   stage 0 SELECTARG2 DIFFUSE, stage 1 as above.
//   pass 2 (/DPASS2): env texture on STAGE 0; MODULATE(TEXTURE, TFACTOR) colour (coef < 255)
//     or SELECTARG1 TEXTURE (255), SELECTARG1 TEXTURE alpha; FOGCOLOR 0 -> fog toward black.
//
// envParams.x = coef byte / 255 (TFACTOR as the FF pipe reads it). Fog as default_PS (P7).
struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;
	float2 TexCoord1	: TEXCOORD1;
	float4 Color		: COLOR0;
	float4 EnvColor		: COLOR1;
};

sampler2D tex0 : register(s0);
sampler2D tex1 : register(s1);

float4 fogColor : register(c0);
float4 fogData : register(c1);
float4 envParams : register(c2);

float4 main(VS_out input) : COLOR
{
	float fog = clamp((input.TexCoord0.z - fogData.y)*fogData.z, fogData.w, 1.0);
#ifdef PASS2
	float4 e = tex2D(tex0, input.TexCoord1.xy);
	float4 color;
	color.rgb = e.rgb * envParams.x * fog;
	color.a = e.a;
	return color;
#else
	float4 color = input.Color;
#ifdef TEX
	color *= tex2D(tex0, input.TexCoord0.xy);
#endif
	color.rgb += tex2D(tex1, input.TexCoord1.xy).rgb * envParams.x;
	color.rgb = lerp(fogColor.rgb, color.rgb, fog);
	return color;
#endif
}
