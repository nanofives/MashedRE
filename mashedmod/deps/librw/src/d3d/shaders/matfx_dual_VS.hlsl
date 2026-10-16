// MASHED LOCAL PATCH (P9, rtex 2026-10-16) -- MatFX DUAL vertex shader. default_VS.hlsl
// plus texcoord set 1 passed through for the stage-1 (dual) texture. RenderWare's D3D9
// MatFX dual path FUN_005405c0 @ 0x005405c0 samples the dual texture with TEXCOORDINDEX 1
// when the geometry carries rpGEOMETRYTEXTURED2 (0x80); the one-set case uses the default
// VS and matfx_dual_PS.hlsl /DUV0 instead. See MASHED_PATCHES.md P9.
#include "standardConstants.h"

struct VS_in
{
	float4 Position		: POSITION;
	float3 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD0;
	float2 TexCoord1	: TEXCOORD1;
	float4 Prelight		: COLOR0;
};

struct VS_out {
	float4 Position		: POSITION;
	float3 TexCoord0	: TEXCOORD0;	// also fog (raw eye depth, P7)
	float2 TexCoord1	: TEXCOORD1;
	float4 Color		: COLOR0;
};


VS_out main(in VS_in input)
{
	VS_out output;

	output.Position = mul(combinedMat, input.Position);
	float3 Vertex = mul(worldMat, input.Position).xyz;
	float3 Normal = mul(normalMat, input.Normal);

	output.TexCoord0.xy = input.TexCoord;
	output.TexCoord1 = input.TexCoord1;

	output.Color = input.Prelight;
	output.Color.rgb += ambientLight.rgb * surfAmbient;

	int i;
#ifdef DIRECTIONALS
	for(i = 0; i < numDirLights; i++)
		output.Color.xyz += DoDirLight(lights[i+firstDirLight], Normal)*surfDiffuse;
#endif
#ifdef POINTLIGHTS
	for(i = 0; i < numPointLights; i++)
		output.Color.xyz += DoPointLight(lights[i+firstPointLight], Vertex.xyz, Normal)*surfDiffuse;
#endif
#ifdef SPOTLIGHTS
	for(i = 0; i < numSpotLights; i++)
		output.Color.xyz += DoSpotLight(lights[i+firstSpotLight], Vertex.xyz, Normal)*surfDiffuse;
#endif
	output.Color = clamp(output.Color, 0.0, 1.0);
	output.Color *= matCol;

	// P7 per-pixel fog: raw eye depth, ramp evaluated in the PS.
	output.TexCoord0.z = output.Position.w;

	return output;
}
